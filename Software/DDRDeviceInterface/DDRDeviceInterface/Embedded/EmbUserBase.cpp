#include "EmbUserBase.h"
#include<iostream>
namespace DDRDrivers {

int _FetchEmbMB(const void *pData, int nMaxLen) {
	const BYTE *pC = (const BYTE*)pData;
	if (*pC == EMBEDDED_MB_FRAME_HEADER0) {
		if (nMaxLen < EMBEDDED_MB_FRAME_LENGTH) { return 0; }
		else {
			if (EMBEDDED_MB_FRAME_HEADER1 == pC[1] &&
				EMBEDDED_MB_FRAME_TAIL0 == pC[EMBEDDED_MB_FRAME_LENGTH - 2] &&
				EMBEDDED_MB_FRAME_TAIL1 == pC[EMBEDDED_MB_FRAME_LENGTH - 1]) {
				int cs = 0;
				for (int i = 2; i < EMBEDDED_MB_FRAME_LENGTH - 3; ++i) {
					cs += (int)pC[i];
				}
				if ((BYTE)(cs & 0xFF) == pC[EMBEDDED_MB_FRAME_LENGTH - 3]) {
					return EMBEDDED_MB_FRAME_LENGTH;
				}
			}
		}
	}
	return -1;
}

int _FetchUBlox(const void *pData, int nMaxLen) {
	const char *pC = (const char*)pData;
	const char *const pEnd = pC + nMaxLen;
	for (pC; pC < pEnd; ++pC) {
		if ((*pC < ' ' || *pC > '~') &&
			*pC != (char)0x0A && *pC != (char)0x0D) {
			break;
		}
	}
	if ((const char*)pData == pC) { return -1; }
	else { return (pC - (const char*)pData); }
}

EmbUserBase::EmbUserBase() {
	m_nState = 0;
	m_pEmbServer = nullptr;
	m_pNodeGroup = DDRLNN::CreateNodeGroup();
	m_pNodeGroup->AddTCPClient("EMBEDDED_MB_DATA", 0);
	m_pNodeGroup->SetClientOption("EMBEDDED_MB_DATA", 128, 4096);
	m_pNodeGroup->AddTCPClient("GNSSUBLOX_DATA", 0);
	m_pNodeGroup->SetClientOption("GNSSUBLOX_DATA", 128, 1024);
	m_pNodeGroup->SetLoopTempo(20, 1);
	if (m_pNodeGroup->StartRunning()) {
		m_MBRcvBuf.reserve(1024);
		m_UBloxRcvBuf.reserve(1024);
		m_bufPtrs.push_back(&m_MBRcvBuf);
		m_bufPtrs.push_back(&m_UBloxRcvBuf);
		m_nState = 1;
	}
}

bool EmbUserBase::ReadEmbData() {
	bool bRet = false;
	_receive(0);
	if (m_MBRcvBuf.size() == 0) { return false; }
	const BYTE *pC = &m_MBRcvBuf[0];
	int remLen = (int)m_MBRcvBuf.size();
	for (; remLen > 0; ++pC, --remLen) {
		int nFetch = _FetchEmbMB(pC, remLen);
		if (EMBEDDED_MB_FRAME_LENGTH == nFetch) {
			bRet = true;
			break; // take at most ONE FRAME
		} else if (0 == nFetch) { break; }
	}
	if (bRet) {
		memcpy(m_MBFrame, pC, EMBEDDED_MB_FRAME_LENGTH);
		pC += EMBEDDED_MB_FRAME_LENGTH;
		remLen -= EMBEDDED_MB_FRAME_LENGTH;
	}
	if (remLen > 0) {
		memmove(&m_MBRcvBuf[0], pC, remLen);
	}
	if (remLen >= 0) {
		m_MBRcvBuf.resize(remLen);
	} else {
		m_MBRcvBuf.resize(0);
	}
	return bRet;
}

bool EmbUserBase::ReadGNSSData() {
	bool bRet = false;
	_receive(1);
	if (m_UBloxRcvBuf.size() == 0) { return false; }
	const BYTE *pC = &m_UBloxRcvBuf[0];
	int remLen = (int)m_UBloxRcvBuf.size();
	while (remLen > 0) {
		int nFetch = _FetchUBlox(pC, remLen);
		if (nFetch > 0) {
			m_UBloxData.FeedBytes(pC, nFetch);
			pC += nFetch;
			remLen -= nFetch;
			bRet = true;
		} else if (0 == nFetch) {
			break;
		} else {
			++pC;
			--remLen;
		}
	}
	if (remLen > 0) {
		memmove(&m_UBloxRcvBuf[0], pC, remLen);
	}
	if (remLen >= 0) {
		m_UBloxRcvBuf.resize(remLen);
	}
	else {
		m_UBloxRcvBuf.resize(0);
	}
	return bRet;
}

bool EmbUserBase::_receive(int clientID) {
	if (clientID != 0 && clientID != 1) { return false; }
	if (0 == clientID) {
		DDRLNN::GetDataFromClientNode(m_pNodeGroup.get(),
									  "EMBEDDED_MB_DATA",
									  m_tmpVec);
	} else {
		DDRLNN::GetDataFromClientNode(m_pNodeGroup.get(),
									  "GNSSUBLOX_DATA",
									  m_tmpVec);
	}
	if (m_tmpVec.size() == 0) { return false; }
	//std::cout << "EmbUserBase::_receive() size:" << m_tmpVec.size() << std::endl;
	size_t pos = m_bufPtrs[clientID]->size();
	m_bufPtrs[clientID]->resize(pos + m_tmpVec.size());
	memcpy(&(*m_bufPtrs[clientID])[pos], &m_tmpVec[0], m_tmpVec.size());
	m_tmpVec.resize(0);
	return true;
}

}