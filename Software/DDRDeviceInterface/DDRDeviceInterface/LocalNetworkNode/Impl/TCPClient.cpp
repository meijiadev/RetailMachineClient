#include "stdafx.h"

#include "TCPClient.h"
#include <string>

namespace DDRLNN {

ClientNode_TCP::ClientNode_TCP() {
	m_connStat = (BYTE)0;
	m_pWriteAuthStr = nullptr;
}

ClientNode_TCP::~ClientNode_TCP() {
	close();
	if (m_pWriteAuthStr) {
		delete m_pWriteAuthStr;
		m_pWriteAuthStr = nullptr;
	}
}

bool ClientNode_TCP::SetTCPClientNode(const char *pNodeName,
	                                  BYTE authType
									  ) {
	if (m_nState != (BYTE)0 ||
		!((pNodeName[0] >= 'A' && pNodeName[0] <= 'Z') ||
		(pNodeName[0] >= 'a' && pNodeName[0] <= 'z')) ||
		(authType != 0 && authType != 1 && authType != 2)) {
		return false;
	}
	SetName(pNodeName);
	m_transProtoID = DDRLNB_PROTO_ID_TCP;
	m_nDataType = DDRLNB_DATATYPE_ID_STREAM;
	m_authType = authType;
	if (DDRLNB_AUTHTYPE_WRITEAUTH == authType) {
		m_pWriteAuthStr = new std::vector<BYTE>;
		m_pWriteAuthStr->reserve(MAX_SIZE_FOR_WRITE_AUTH);
	}
	m_nState = (BYTE)3;
	return true;
}

bool ClientNode_TCP::addFD(fd_set *pSet2Read, fd_set *pSet2Write) {
#if 0
	if ((BYTE)4 == m_nState) {
#else
	if ((BYTE)4 == m_nState && m_soc.IsValid()) {
#endif
		FD_SET(m_soc.Socket(), pSet2Read);
		if ((BYTE)0 == m_connStat) {
			FD_SET(m_soc.Socket(), pSet2Write);
		}
		return true;
	} else { return false; }
}

void ClientNode_TCP::close() {
	if ((BYTE)4 == m_nState) {
		m_soc.Close();
		m_nState = (BYTE)3;
	}
}

bool ClientNode_TCP::_establishClient(unsigned long bcIP_n,
	                                  unsigned short tarPort_n) {
	m_soc = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (!m_soc.IsValid()) {	return false; }
	if (setsockopt(m_soc.Socket(), SOL_SOCKET, SO_SNDBUF, (char*)&m_szSndBuf,
		sizeof(m_szSndBuf)) < 0) {
		return false;
	}
	if (setsockopt(m_soc.Socket(), SOL_SOCKET, SO_RCVBUF, (char*)&m_szRcvBuf,
		sizeof(m_szRcvBuf)) < 0) {
		return false;
	}
	unsigned long DisableBlocking = 1;
	if (ioctlsocket(m_soc.Socket(), FIONBIO, &DisableBlocking) == SOCKET_ERROR) {
		return false;
	}
	sockaddr_in remoteAddr;
	remoteAddr.sin_family = AF_INET;
	remoteAddr.sin_port = tarPort_n;
	remoteAddr.sin_addr.s_addr = bcIP_n;
	connect(m_soc.Socket(), (sockaddr*)(&remoteAddr), sizeof(sockaddr_in));
	m_lastHBTime = std::chrono::system_clock::now();
	m_lastContentReceptionTime = m_lastHBTime;
	m_connStat = (BYTE)0;
	return true;
}

bool ClientNode_TCP::_respond2write() {
	if (m_connStat != (BYTE)0) { return false; }
	int err, len = sizeof(int);
	getsockopt(m_soc.Socket(), SOL_SOCKET, SO_ERROR, (char*)(&err), &len);
	if (err != 0) { return false; }
	if (DDRLNB_AUTHTYPE_WRITEAUTH == m_authType) {
		m_pWriteAuthStr->resize(0);
		m_connStat = (BYTE)1;
	} else {
		if (DDRLNB_AUTHTYPE_READAUTH == m_authType)	{
			std::vector<BYTE> authStr;
			if ((*m_pFuncRAG)(&authStr, m_pArgRAG) && authStr.size() > 0) {
				send(m_soc.Socket(), (const char*)&authStr[0], authStr.size(), 0);
			}
		}
		send(m_soc.Socket(), "HB\0", 3, 0);
		m_lastHBTime = std::chrono::system_clock::now();
		m_lastAnyReceptionTime = m_lastHBTime;
		m_lastContentReceptionTime = m_lastHBTime;
		m_connStat = (BYTE)2;
	}
	return true;
}

bool ClientNode_TCP::_respond2read(std::vector<BYTE> &rcvVec) {
	rcvVec.resize(0);
	if ((BYTE)2 == m_connStat) {
		while (1) {
			int pos = (int)rcvVec.size();
			rcvVec.resize(pos + DATALEN_BYTES_EACH_READOUT);
			int n = recv(m_soc.Socket(), (char*)&rcvVec[pos],
				         DATALEN_BYTES_EACH_READOUT, 0);
			if (n <= 0)	{ return false;	}
			if (n < DATALEN_BYTES_EACH_READOUT) {
				rcvVec.resize(pos + n);
				break;
			} else {
				timeval ztv = { 0, 0 };
				fd_set _sset;
				FD_ZERO(&_sset);
				FD_SET(m_soc.Socket(), &_sset);
				if (select(0, &_sset, nullptr, nullptr, &ztv) == 0) { break; }
			}
		}
		return true;
	} else if ((BYTE)1 == m_connStat) {	// possible only with write auth.
		int pos = (int)m_pWriteAuthStr->size();
		m_pWriteAuthStr->resize(MAX_SIZE_FOR_WRITE_AUTH);
		int n = recv(m_soc.Socket(), (char*)&((*m_pWriteAuthStr)[0]),
			         MAX_SIZE_FOR_WRITE_AUTH - pos, 0);
		if (n <= 0) { return false; }
		if (n < MAX_SIZE_FOR_WRITE_AUTH - pos) {
			m_pWriteAuthStr->resize(pos + n);
		}
		int offset;
		bool bValidated = (*m_pFuncWAV)((const BYTE*)&((*m_pWriteAuthStr)[0]),
			                            (int)m_pWriteAuthStr->size(), &offset, m_pArgWAV);
		if (bValidated) {
			// write auth validated
			if (offset < (int)m_pWriteAuthStr->size()) {
				rcvVec.resize(m_pWriteAuthStr->size() - offset);
				memcpy(&rcvVec[0], &((*m_pWriteAuthStr)[offset]),
					   m_pWriteAuthStr->size() - offset);
			}
			send(m_soc.Socket(), "HB\0", 3, 0);
			m_lastHBTime = std::chrono::system_clock::now();
			m_lastAnyReceptionTime = m_lastHBTime;
			m_lastContentReceptionTime = m_lastHBTime;
			m_connStat = (BYTE)2;
			return true;
		} else if (MAX_SIZE_FOR_WRITE_AUTH == (int)m_pWriteAuthStr->size() ||
			       -1 == offset) {
			// longest possible auth received but still not validated
			// OR impossible to validated
			return false;
		}
		return true;
	}
	else { return true; }
}

bool ClientNode_TCP::_updateConnection() {
	if ((BYTE)0 == m_connStat || (BYTE)1 == m_connStat) {
		auto _tic = std::chrono::system_clock::now();
		__int64 timediff = std::chrono::duration_cast
			<std::chrono::milliseconds>(_tic - m_lastAnyReceptionTime).count();
		if (timediff > (((BYTE)0 == m_connStat) ? MAXTIME_FOR_CONNECTION :
			                                      MAXTIME_FOR_WRITE_AUTH_MS)) {
			// connection / write auth. timed-out
			return false;
		}
	}
	return ClientNode_Base0::_updateConnection();
}

void ClientNode_TCP::_sendHB() {
	auto _tic = std::chrono::system_clock::now();
	__int64 timediff = std::chrono::duration_cast<std::chrono::milliseconds>
		(_tic - m_lastHBTime).count();
	if (timediff >= HEART_BEAT_PERIOD_IN_MS) {
		int nRet = send(m_soc.Socket(), "HB\0", 3, 0);
#if 0
		std::cout << "Client ('" << m_Name << "') sending HB (state="
			<< (int)m_nState << ", " << (int)m_connStat << "), nRet="
			<< nRet << "!" << std::endl;
#endif
		m_lastHBTime = _tic;
	}
}

}