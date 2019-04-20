#include "stdafx.h"

#include "TCPServer.h"

namespace DDRLNN {

bool ServerNode_TCP::SetTCPServerNode(const char *pNodeName,
	                                  BYTE authType) {
	if (m_nState != (BYTE)0 ||
		!((pNodeName[0] >= 'A' && pNodeName[0] <= 'Z') ||
		(pNodeName[0] >= 'a' && pNodeName[0] <= 'z')) ||
		(authType != 0 && authType != 1 && authType != 2)) {
		return false;
	}
	SetName(pNodeName);
	m_transProtoID = DDRLNB_PROTO_ID_TCP;
	m_authType = authType;
	m_nState = (BYTE)3;
	return true;
}

bool ServerNode_TCP::addReadFD(fd_set *pSet2Append) {
	if (m_nState != (BYTE)4) { return false; }
	FD_SET(m_soc.Socket(), pSet2Append);
	for (int i = 0; i < (int)m_clients.size(); ++i)	{
		FD_SET(m_clients[i].clt_soc.Socket(), pSet2Append);
	}
	return true;
}

void ServerNode_TCP::close() {
	if ((BYTE)4 == m_nState) {
		for (int i = 0; i < (int)m_clients.size(); ++i)	{
			m_clients[i].clt_soc.Close();
		}
		m_clients.clear();
		m_connCnt = 0;
		m_soc.Close();
		m_nState = (BYTE)3;
	}
}

bool ServerNode_TCP::_establishServer() {
	if (m_nState != (BYTE)3) { return false; }
	// starting TCP server
	sockaddr_in tarAddr;
	tarAddr.sin_family = AF_INET;
	tarAddr.sin_addr.s_addr = 0;
	tarAddr.sin_port = htons(0);
	m_soc = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (!m_soc.IsValid()) { return false; }
	if (setsockopt(m_soc.Socket(), SOL_SOCKET, SO_SNDBUF,
		           (char*)&m_szSndBuf, sizeof(m_szSndBuf)) < 0) {
		m_soc.Close();
		return false;
	}
	if (setsockopt(m_soc.Socket(), SOL_SOCKET, SO_RCVBUF,
		(char*)&m_szRcvBuf, sizeof(m_szRcvBuf)) < 0)	{
		m_soc.Close();
		return false;
	}
	if (bind(m_soc.Socket(), (sockaddr*)(&tarAddr), sizeof(sockaddr_in)) < 0) {
		m_soc.Close();
		return false;
	}
	if (listen(m_soc.Socket(), 1) != 0) {
		m_soc.Close();
		return false;
	}
	// getting port number
	sockaddr_in myAddr;
	int lll = sizeof(sockaddr_in);
	if (getsockname(m_soc.Socket(), (sockaddr*)&myAddr, &lll) < 0) { return false; }
	m_port = ntohs(myAddr.sin_port);
	m_lastClientUpdate = std::chrono::system_clock::now();

	//std::cout << "Establishing TCP server @0.0.0.0 : " << m_port << " ('" << m_Name << "')\n";

	m_nState = (BYTE)4;
	return true;
}

void ServerNode_TCP::_send2all(const BYTE *pData, int nDataLen) {
	if (pData != nullptr && nDataLen > 0) {
		for (int i = 0; i < (int)m_clients.size(); ++i) {
			send(m_clients[i].clt_soc.Socket(), (const char*)pData, nDataLen, 0);
		}
	}
}

BYTE ServerNode_TCP::_parseRecv(const BYTE *pRcvBuf,
	                            int nBufLen, int &nextOffset) {
	BYTE sig = 0;
	nextOffset = 0;
	while (nextOffset < nBufLen) {
		if ((BYTE)'H' == *pRcvBuf) {
			if (nBufLen - nextOffset >= 3) {
				if ('B' == pRcvBuf[1] && '\0' == pRcvBuf[2]) {
					pRcvBuf += 3;
					nextOffset += 3;
					sig |= 0x01;
					continue;
				}
			} else { return sig; }
		}/* else if ((BYTE)'P' == *pRcvBuf) {
			if (nBufLen - nextOffset >= 3) {
				if ('I' == pRcvBuf[1] && '\0' == pRcvBuf[2]) {
					pRcvBuf += 3;
					nextOffset += 3;
					sig |= 0x02;
					continue;
				}
			} else { return sig; }
		}*/
		++pRcvBuf;
		++nextOffset;
	}
	return sig;
}

void ServerNode_TCP::_processRecv(fd_set *pSetRead) {
	// checking status change
	if (FD_ISSET(m_soc.Socket(), pSetRead)) {
		// server socket ON, new connection
		sockaddr_in conn_addr;
		int len = sizeof(sockaddr);
		SocketWrapper soc = accept(m_soc.Socket(), (sockaddr*)&conn_addr, &len);
		if (soc.IsValid() && (int)m_clients.size() < m_nMaxClients) {
			// adding new connection
			m_clients.resize(m_clients.size() + 1);
			m_clients.back().clt_soc = soc;
			m_clients.back().conn_addr = conn_addr;
			m_clients.back().lastActiveTime = std::chrono::system_clock::now();
			++m_connCnt;
#if 0
			std::cout << "\nServer ('" << m_Name << "') accepting client ("
				<< m_connCnt << " conns now)!\n" << std::endl;
#endif
			if (DDRLNB_AUTHTYPE_WRITEAUTH == m_authType) {
				std::vector<BYTE> writeAuthTexts;
				if ((*m_pFuncWAG)(&writeAuthTexts, m_pArgWAG) &&
					writeAuthTexts.size() > 0) {
					send(soc.Socket(), (const char*)&writeAuthTexts[0], writeAuthTexts.size(), 0);
				}
			}
			m_clients.back().stat = (DDRLNB_AUTHTYPE_READAUTH == m_authType) ? (BYTE)0 : (BYTE)1;
			m_clients.back().rcvPos = 0;
		} else {
			// max number of connections reached
			soc.Close();
		}
		FD_CLR(m_soc.Socket(), pSetRead);
	}

	// checking new inbound data
	timeval zeroTV = { 0, 0 };
	for (int i = (int)m_clients.size() - 1; i >= 0; --i) {
		if (!FD_ISSET(m_clients[i].clt_soc.Socket(), pSetRead)) { continue; }
		bool bDisconnected = false;
		BYTE sig = 0;
		while (1) {
			int n = recv(m_clients[i].clt_soc.Socket(),
				         m_clients[i].rcvBuf + m_clients[i].rcvPos,
						 RCVBUFF_SIZE_FOR_EACH_CLIENT - m_clients[i].rcvPos,
						 0);
			bool bPossibleRem = (n + m_clients[i].rcvPos ==
				                 RCVBUFF_SIZE_FOR_EACH_CLIENT);
#if 0
			std::cout << "Server ('" << m_Name << "') client #"
				<< i << " active (" << RCVBUFF_SIZE_FOR_EACH_CLIENT - m_clients[i].rcvPos
				<< " bytes tried, " << n << " bytes received!\n" << std::endl;
#endif
			if (n <= 0)	{
				// disconnected
				FD_CLR(m_clients[i].clt_soc.Socket(), pSetRead);
				m_clients[i].clt_soc.Close();
				m_clients.erase(m_clients.begin() + i);
				--m_connCnt;
				bDisconnected = true;
				break;
			}
			m_clients[i].rcvPos += n;
			if ((BYTE)0 == m_clients[i].stat) {
				int offset;
				bool bValidated = (*m_pFuncRAV)((const BYTE*)m_clients[i].rcvBuf,
					                            m_clients[i].rcvPos, &offset, m_pArgRAV);
				if (bValidated) {
					// read auth validated
					memmove(m_clients[i].rcvBuf,
						    m_clients[i].rcvBuf + offset,
						    m_clients[i].rcvPos - offset);
					m_clients[i].rcvPos -= offset;
					m_clients[i].lastActiveTime = std::chrono::system_clock::now();
					m_clients[i].stat = (BYTE)1;
				} else if (RCVBUFF_SIZE_FOR_EACH_CLIENT == m_clients[i].rcvPos ||
					       -1 == offset) {
					// longest possible auth received but still not validated
					// OR impossible to be validated
					m_clients[i].clt_soc.Close();
					m_clients.erase(m_clients.begin() + i);
					--m_connCnt;
					continue;
				} else { continue; } // yet to be validated
			}
			if ((BYTE)1 == m_clients[i].stat) {
				int offset;
				sig |= _parseRecv((const BYTE*)m_clients[i].rcvBuf, m_clients[i].rcvPos, offset);
				memmove(m_clients[i].rcvBuf, m_clients[i].rcvBuf + offset, m_clients[i].rcvPos - offset);
				m_clients[i].rcvPos -= offset;
			}
			if (!bPossibleRem) {
				FD_CLR(m_clients[i].clt_soc.Socket(), pSetRead);
				break;
			} else {
				timeval zeroTV = { 0, 0 };
				fd_set ss_set;
				FD_ZERO(&ss_set);
				FD_SET(m_clients[i].clt_soc.Socket(), &ss_set);
				if (select(0, &ss_set, nullptr, nullptr, &zeroTV) == 0) {
					// no more incoming data
					FD_CLR(m_clients[i].clt_soc.Socket(), pSetRead);
					break;
				}
			}
		} // end while()
		if (bDisconnected) { continue; }
		if (sig != (BYTE)0)	{
			m_clients[i].lastActiveTime = std::chrono::system_clock::now();
		}
		/*
		if (sig & 0x02) {
			// inquiry for structured data protocol
			m_tmpBuf.resize(0);
			if (m_SDPH.AcquireProtocolTxt(m_tmpBuf, true)) {
				send(m_clients[i].clt_soc.Socket(), (const char*)&m_tmpBuf[0], m_tmpBuf.size(), 0);
				m_tmpBuf.resize(0);
			}
		}*/
	} // end for (each client)
}

void ServerNode_TCP::_updateConnections() {
	auto _tic = std::chrono::system_clock::now();
	__int64 timediff = std::chrono::duration_cast<std::chrono::milliseconds>
		(_tic - m_lastClientUpdate).count();
	if (timediff < CLIENTS_UPDATE_PERIOD_MS) { return; }

	for (int i = (int)m_clients.size() - 1; i >= 0; --i) {
		if ((BYTE)1 == m_clients[i].stat) {
			// connection available
			__int64 timediff = std::chrono::duration_cast
				<std::chrono::milliseconds>
				(_tic - m_clients[i].lastActiveTime).count();
			if (timediff > MAXTIME_FOR_INACTIVE_CONN_MS) {
				// max time elapsed and still not active
				m_clients[i].clt_soc.Close();
				m_clients.erase(m_clients.begin() + i);
				--m_connCnt;
#if 0
				std::cout << "Server ('" << m_Name << "') KICKING off client ("
					<< m_connCnt << " conns now), INACTIVE, timediff="
					<< timediff << "!" << std::endl;
#endif
				continue;
			}
		} else if ((BYTE)0 == m_clients[i].stat) {
			// yet to be authenticated
			__int64 timediff = std::chrono::duration_cast
				<std::chrono::milliseconds>
				(_tic - m_clients[i].lastActiveTime).count();
			if (timediff > MAXTIME_FOR_READ_AUTH_MS) {
				// max time elapsed and still not authenticated
				m_clients[i].clt_soc.Close();
				m_clients.erase(m_clients.begin() + i);
				--m_connCnt;
			}
		}
	}
	m_lastClientUpdate = _tic;
}

}