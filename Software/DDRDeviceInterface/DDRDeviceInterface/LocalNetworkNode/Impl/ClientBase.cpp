#include "stdafx.h"

#include "ClientBase.h"

namespace DDRLNN {

ClientNode_Base0::ClientNode_Base0() {
	m_szSndBuf = 1024;
	m_szRcvBuf = 256;
	m_nState = (BYTE)0;

	m_nDataType = DDRLNB_DATATYPE_ID_STREAM;
	m_pData = new std::vector<BYTE>;

	m_pArgORD = nullptr;
	m_pFuncORD = onReceivedData;
	m_pArgRAG = nullptr;
	m_pFuncRAG = _generateReadAuth;
	m_pArgWAV = nullptr;
	m_pFuncWAV = _validateWriteAuth;
}

ClientNode_Base0::~ClientNode_Base0() {
	if (m_pData != nullptr) {
		if (DDRLNB_DATATYPE_ID_STREAM == m_nDataType) {
			delete ((std::vector<BYTE>*)m_pData);
		}
	}
}

bool ClientNode_Base0::SetName(const char *pName) {
	if ((BYTE)0 == m_nState	&&
		((pName[0] >= 'A' && pName[0] <= 'Z') ||
		(pName[0] >= 'a' && pName[0] <= 'z'))) {
		m_Name = pName;
		m_nState = (BYTE)1;
		return true;
	} else { return false; }
}

bool ClientNode_Base0::SetClientOption(int szSndBuf, int szRcvBuf) {
	if ((int)m_nState < 4) {
		m_szSndBuf = (szSndBuf < 128) ? 128 : szSndBuf;
		m_szRcvBuf = (szRcvBuf < 1024) ? 1024 : szRcvBuf;
		return true;
	} else { return false; }
}

bool ClientNode_Base0::Try2Launch(unsigned long srcIP_n, unsigned short tarPort_n,
	                              const char *pNodeName,
								  BYTE dataTypeID, BYTE transProtoID,
								  BYTE authTypeID) {
	if (m_nState != (BYTE)3) { return false; }
	if (strcmp(pNodeName, m_Name.c_str()) == 0 &&
		dataTypeID == this->m_nDataType &&
		transProtoID == this->m_transProtoID &&
		authTypeID == this->m_authType) {
		if (_establishClient(srcIP_n, tarPort_n)) {
			m_curTarAddr.sin_addr.s_addr = srcIP_n;
			m_curTarAddr.sin_port = tarPort_n;
			m_nState = (BYTE)4;
			m_lastAnyReceptionTime = std::chrono::system_clock::now();
			return true;
		} else {
			if (!m_soc.IsValid()) {
				m_soc.Close();
			}
			return false;
		}
	} else { return false; }
}

void ClientNode_Base0::processIO(fd_set *pSetRead, fd_set *pSetWritten) {
	if (m_nState != (BYTE)4) { return; }
	if (FD_ISSET(m_soc.Socket(), pSetWritten)) {
		FD_CLR(m_soc.Socket(), pSetWritten);
		if (!_respond2write()) {
			m_soc.Close();
			m_nState = (BYTE)3;
		}
	}
	if (FD_ISSET(m_soc.Socket(), pSetRead)) {
		FD_CLR(m_soc.Socket(), pSetRead);
		if (_respond2read(m_tmpBuf)) {
			if (m_tmpBuf.size() > 0) {
				int pos = (int)m_rcvBuf.size();
				m_rcvBuf.resize(pos + (int)m_tmpBuf.size());
				memcpy(&m_rcvBuf[pos], &m_tmpBuf[0], (int)m_tmpBuf.size());
				m_tmpBuf.resize(0);
				_processRcvBuf();

				if (DDRLNB_DATATYPE_ID_STREAM == m_nDataType) {
					while (!m_dataLock.try_lock_for(std::chrono::milliseconds(1)));
					std::vector<BYTE> &dataVec = *((std::vector<BYTE>*)m_pData);
					if (dataVec.size() > 0) {
						unsigned int nRet = (*m_pFuncORD)(&dataVec[0], dataVec.size(), m_pArgORD);
						if (nRet > 0 && nRet < dataVec.size()) {
							memmove(&dataVec[0], &dataVec[dataVec.size() - nRet], nRet);
						}
						if (nRet >= 0 && nRet < dataVec.size()) {
							dataVec.resize(nRet);
						}
					}
					m_dataLock.unlock();
				}
			}
		} else {
			m_soc.Close();
			m_nState = (BYTE)3;
		}
	}
	if (m_nState != (BYTE)4) { return; }
	if (!_updateConnection()) {
		m_soc.Close();
		m_nState = (BYTE)3;
	}
	_sendHB();
}

bool ClientNode_Base0::GetData(std::vector<BYTE> &dataVec) {
	if ((BYTE)4 != m_nState && (BYTE)3 != m_nState) { return false; }
	bool bSuc = false;
	if (m_dataLock.try_lock_for(std::chrono::milliseconds(1))) {
		if (DDRLNB_DATATYPE_ID_STREAM == m_nDataType) {
			std::vector<BYTE> *pStoredVec = (std::vector<BYTE>*)m_pData;
			if (pStoredVec->size() > 0) {
				dataVec.resize(pStoredVec->size());
				memcpy(&dataVec[0], &((*pStoredVec)[0]), pStoredVec->size());
				pStoredVec->resize(0);
				bSuc = true;
			}
		}
		m_dataLock.unlock();
	}
	return bSuc;
}

bool ClientNode_Base0::_updateConnection() {
	auto _tic = std::chrono::system_clock::now();
	__int64 timediff = std::chrono::duration_cast<std::chrono::milliseconds>
		(_tic - m_lastAnyReceptionTime).count();
	return (timediff <= MAXTIME_FOR_INACTIVE_CONNECTION);
}

void ClientNode_Base0::_processRcvBuf() {
	if (m_rcvBuf.size() == 0) { return; }
	const BYTE *pC = &m_rcvBuf[0];
	const BYTE* const pEnd = pC + m_rcvBuf.size();
	int stuffReceived = 0;
	
	while (pC < pEnd) {
		if ((BYTE)'D' == *pC) {
			if (pEnd - pC < 8) { break; }
			int nlen;
			if ((BYTE)'F' != pC[1] || (BYTE)'\0' != pC[2] ||
				!DDRCommLib::VerifyDataLen_32(pC + 3, &nlen)) {
				++pC;
				continue;
			}
			if (pEnd - pC < nlen + 7) { break; }
			if (DDRLNB_DATATYPE_ID_STREAM == m_nDataType) {
				std::vector<BYTE> &dataVec = *((std::vector<BYTE>*)m_pData);
				while (!m_dataLock.try_lock_for(std::chrono::milliseconds(1))) {};
				int pos = (int)dataVec.size();
				dataVec.resize(pos + nlen);
				memcpy(&dataVec[pos], pC + 7, nlen);
				m_dataLock.unlock();
				stuffReceived |= 0x01;
			}
			pC += 7 + nlen;
		} else if ((BYTE)'P' == *pC) {
			if (pEnd - pC < 8) { break; }
			int nlen;
			if ((BYTE)'F' != pC[1] || (BYTE)'\0' != pC[2] ||
				!DDRCommLib::VerifyDataLen_32(pC + 3, &nlen)) {
				++pC;
				continue;
			}
			if (pEnd - pC < nlen + 7) { break; }
			pC += 7 + nlen;
		} else if ((BYTE)'H' == *pC) {
			if (pEnd - pC < 3) { break; }
			if ((BYTE)'B' == pC[1] && (BYTE)'\0' == pC[2]) {
				pC += 3;
				stuffReceived |= 0x02;
			} else {
				++pC;
			}
		} else {
			++pC;
		}
	}
	if (stuffReceived & 0x01) {
		m_lastContentReceptionTime = std::chrono::system_clock::now();
	}
	if (stuffReceived) {
		m_lastAnyReceptionTime = std::chrono::system_clock::now();
	}

	if (pC < pEnd) {
		memmove(&m_rcvBuf[0], pC, pEnd - pC);
	}
	m_rcvBuf.resize(pEnd - pC);
}

__int64 ClientNode_Base0::GetTimeSinceLastContentReception() const {
	auto temp = m_lastContentReceptionTime;
	auto tic = std::chrono::system_clock::now();
	return (std::chrono::duration_cast
		<std::chrono::milliseconds>(tic - temp).count());
}

}