#include "stdafx.h"

#include "UDPServer.h"

#include <thread>
#include "CommonDef/CommonDefFunc.h"

namespace DDRLNN {
	
ServerNode_UDP::ServerNode_UDP()
	: m_FSG(DDRCommLib::_createFSG()) {}

bool ServerNode_UDP::SetUDPServerNode(const char *pNodeName,
                             		  const char *pSDProtoTxt,
									  int nSDProtoLen) {
	if (m_nState != (BYTE)0 ||
		!((pNodeName[0] >= 'A' && pNodeName[0] <= 'Z') ||
		(pNodeName[0] >= 'a' && pNodeName[0] <= 'z'))) {
		return false;
	}
	SetName(pNodeName);
	m_transProtoID = DDRLNB_PROTO_ID_UDP;
	m_authType = DDRLNB_AUTHTYPE_NOAUTH;
	m_nState = (BYTE)3;
	return true;
}

void ServerNode_UDP::close() {
	if ((BYTE)4 == m_nState) {
		m_MCServer.Close();
		m_nState = (BYTE)3;
	}
}

bool ServerNode_UDP::_establishServer() {
	if (m_nState != (BYTE)3) { return false; }
	// try to find an available port for multicast
	m_port = 0;
	unsigned short rPort;
	while (1) {
		unsigned int SEC;
		unsigned short milSec;
		DDRCommonDef::GetUNIXTimeStamp(SEC, milSec);
		milSec &= 0xFF;
		milSec = (milSec * 23 + 5) & 0xFF;
		rPort = milSec << 8;
		milSec = (milSec * 23 + 5) & 0xFF;
		rPort |= milSec;
		if (rPort < 10000) { continue; }
		m_port = rPort;
		break;
	}
	
	if (!m_MCServer.IsValid()) {
		m_MCServer.ReInit();
	}
	if (!m_MCServer.IsValid()) { return false; }

	if (m_MCServer.Setsockopt(SOL_SOCKET, SO_SNDBUF,
		                      (char*)&m_szSndBuf,
							  sizeof(m_szSndBuf)) < 0) {
		m_MCServer.Close();
		return false;
	}
	if (m_MCServer.Setsockopt(SOL_SOCKET, SO_RCVBUF,
		                      (char*)&m_szRcvBuf,
							  sizeof(m_szRcvBuf)) < 0) {
		m_MCServer.Close();
		return false;
	}
	m_tarMCAddr.sin_family = AF_INET;
	inet_pton(AF_INET, DDRLNB_UDP_CONTENT_BROADCASTING_ADDRESS_IP,
		      &(m_tarMCAddr.sin_addr.s_addr));
	m_tarMCAddr.sin_port = htons(m_port);

	m_MCMonitor.JoinGroup(&m_tarMCAddr);

	m_nState = (BYTE)4;
	return true;
}

void ServerNode_UDP::_send2all(const BYTE *pData, int nDataLen) {
	m_FSG->FeedOneFrame(pData, nDataLen);
	while (1) {
		int n = m_FSG->GetOneSlice(m_slice, UDP_MULTICAST_MAX_SLICE_SIZE);
		if (n <= 0) { break; } // finished or error occured
		else {
			m_MCServer.Broadcast(&m_tarMCAddr, (const char*)m_slice, n);
		}
	}
	m_MCServer.GetMCSrcAddr(m_MCSrcSocks);
	while (1) {
		int len = sizeof(sockaddr_in);
		sockaddr_in addr;
		int n = m_MCMonitor.recv((char*)m_slice, UDP_MULTICAST_MAX_SLICE_SIZE,
			                     (sockaddr*)&addr, &len);
		if (n <= 0) { break; }
		bool bIn = false;
		for (int i = 0; i < (int)m_MCSrcSocks.size(); ++i) {
			if (m_MCSrcSocks[i].sin_addr.s_addr == addr.sin_addr.s_addr &&
				m_MCSrcSocks[i].sin_port == addr.sin_port) {
				bIn = true;
				break;
			}
		}
		if (!bIn) {
			m_MCServer.Close();
			m_nState = (BYTE)3;
			break;
		}
	}
}

}
