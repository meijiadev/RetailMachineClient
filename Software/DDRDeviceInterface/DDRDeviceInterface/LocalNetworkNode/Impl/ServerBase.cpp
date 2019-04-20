#include "stdafx.h"

#include "ServerBase.h"

namespace DDRLNN {

ServerNode_Base0::ServerNode_Base0() {
	m_szSndBuf = 1024;
	m_szRcvBuf = 256;
	m_nMaxClients = 5;
	m_IdlingPeriod = 60000;
	m_nState = (BYTE)0;

	m_dataType = DDRLNB_DATATYPE_ID_STREAM;

	m_pArgNDP = nullptr;
	m_pFuncNDP = _hasNewDataToPublish;
	m_pArgIG = nullptr;
	m_pFuncIG = _generateIdlingString;
	m_pArgWAG = nullptr;
	m_pFuncWAG = _generateWriteAuth;
	m_pArgRAV = nullptr;
	m_pFuncRAV = _validateReadAuth;
}

bool ServerNode_Base0::SetName(const char *pName) {
	if ((BYTE)0 == m_nState && strlen(pName) > 0 &&
		((pName[0] >= 'A' && pName[0] <= 'Z') ||
		(pName[0] >= 'a' && pName[0] <= 'z'))) {
		m_Name = pName;
		m_nState = (BYTE)1;
		return true;
	} else { return false; }
}

bool ServerNode_Base0::SetServerOption(int szSndBuf, int szRcvBuf,
	                                   int nMaxClients) {
	if ((int)m_nState < 4) {
		m_szSndBuf = (szSndBuf < 1024) ? 1024 : szSndBuf;
		m_szRcvBuf = (szRcvBuf < 128) ? 128 : szRcvBuf;
		m_nMaxClients = (nMaxClients < 1) ? 1 :
			            (nMaxClients > 20 ? 20 : nMaxClients);
		return true;
	} else { return false; }
}

bool ServerNode_Base0::Try2Launch() {
	if (m_nState != (BYTE)3) { return false; }
	if (_establishServer())	{
		m_lastContentTime = std::chrono::system_clock::now();
		m_lastHeartBeatTime = m_lastContentTime;
		m_nState = (BYTE)4;
		return true;
	} else { return false; }
}

void ServerNode_Base0::processIO(fd_set *pSetRead) {
	if ((BYTE)4 == m_nState) {
		_try2send();
		_processRecv(pSetRead);
		_sendHB();
		_updateConnections();
		if (m_pFuncNDP && (*m_pFuncNDP)(&m_tmpBuf, m_pArgNDP) &&
			m_tmpBuf.size() > 0) {
			PublishContent(&m_tmpBuf[0], (int)m_tmpBuf.size());
			m_tmpBuf.resize(0);
		}
	}
}

bool ServerNode_Base0::PublishContent(const BYTE *pContent, int nContentLen) {
	if (m_nState != (BYTE)4 ||
		nContentLen <= 0 ||
		nullptr == pContent) {
		return false;
	}/*
	if (!m_SDPH.WriteStructuredData((const char*)pContent, nContentLen)) {
		return false;
	}*/
	if (m_bufLock.try_lock_for(std::chrono::milliseconds(1))) {
		size_t pos = m_sndBuffer.size();
		m_sndBuffer.resize(pos + 7 + nContentLen);
		memcpy(&m_sndBuffer[pos], "DF\0", 3);
		// encoded content size
		*((int*)(&m_sndBuffer[pos + 3])) = DDRCommLib::EncryptDataLen_32(nContentLen);
		memcpy(&m_sndBuffer[pos + 7], pContent, nContentLen);
		m_bufLock.unlock();
		return true;
	} else { return false; }
}

bool ServerNode_Base0::attachNodeInfo(std::vector<BYTE> &vec2append) {
	if (m_nState != (BYTE)4) { return false; }
	int pos = (int)vec2append.size();
	vec2append.resize(pos + 4 + m_Name.length() + 4 + sizeof(unsigned short));
	*((int*)&vec2append[pos]) = DDRCommLib::EncryptDataLen_32(m_Name.length() + 4 +
														      sizeof(unsigned short));
	memcpy(&vec2append[pos + 4], m_Name.c_str(), m_Name.length());
	vec2append[pos + 4 + m_Name.length()] = (BYTE)'\0';
	*((BYTE*)&vec2append[pos + 4 + m_Name.length() + 1]) = m_dataType;
	*((BYTE*)&vec2append[pos + 4 + m_Name.length() + 2]) = m_transProtoID;
	*((BYTE*)&vec2append[pos + 4 + m_Name.length() + 3]) = m_authType;
	*((unsigned short*)&vec2append[pos + 4 + m_Name.length() + 4]) = m_port;
	return true;
}

void ServerNode_Base0::_try2send() {
	int bSentSomething = 0;
	// processing sending buffer
	m_bufLock.lock();
	// if sending buffer is non-empty, send it
	if (m_sndBuffer.size() > 0)	{
		_send2all(&m_sndBuffer[0], m_sndBuffer.size());
		bSentSomething |= 0x01;
	}
	m_sndBuffer.resize(0);
	m_bufLock.unlock();
	if (bSentSomething) {
		m_lastHeartBeatTime = std::chrono::system_clock::now();
		if (bSentSomething & 0x01) {
			m_lastContentTime = m_lastHeartBeatTime;
		}
	}
}

void ServerNode_Base0::_sendHB() {
	auto _tic = std::chrono::system_clock::now();
	__int64 timediff = std::chrono::duration_cast<std::chrono::milliseconds>
		(_tic - m_lastContentTime).count();
	if (timediff >= m_IdlingPeriod &&
		(*m_pFuncIG)(&m_tmpBuf, m_pArgIG) && m_tmpBuf.size() > 0) {
		int dLen = (int)m_tmpBuf.size();
		m_tmpBuf.resize(7 + dLen);
		memmove(&m_tmpBuf[7], &m_tmpBuf[0], dLen);
		memcpy(&m_tmpBuf[0], "DF\0", 3); // data frame header
		*((int*)&m_tmpBuf[3]) = DDRCommLib::EncryptDataLen_32(dLen);
		_send2all(&m_tmpBuf[0], (int)m_tmpBuf.size());
		m_tmpBuf.resize(0);
		m_lastContentTime = _tic;
		m_lastHeartBeatTime = _tic;
		return;
	}
	timediff = std::chrono::duration_cast<std::chrono::milliseconds>
		(_tic - m_lastHeartBeatTime).count();
	if (timediff >= HEART_BEAT_PERIOD_IN_MS) {
		_send2all((const BYTE*)"HB\0", 3);
		m_lastHeartBeatTime = _tic;
	}
}

__int64 ServerNode_Base0::GetTimeSinceLastPublication() const {
	auto temp = m_lastContentTime;
	auto tic = std::chrono::system_clock::now();
	return (std::chrono::duration_cast
		<std::chrono::milliseconds>(tic - temp).count());
}

}