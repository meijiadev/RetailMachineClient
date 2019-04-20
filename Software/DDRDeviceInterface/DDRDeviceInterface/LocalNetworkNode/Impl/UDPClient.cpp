#include "stdafx.h"

#include "UDPClient.h"

#include "CommLib/CommLibInterface.h"
#include "MIMC.h"

namespace DDRLNN {

ClientNode_UDP::ClientNode_UDP() : m_FSC(DDRCommLib::_createFSC(1450, 65536, 1)) {}

bool ClientNode_UDP::SetUDPClientNode(const char *pNodeName) {
	if (m_nState != (BYTE)0 ||
		!((pNodeName[0] >= 'A' && pNodeName[0] <= 'Z') ||
		(pNodeName[0] >= 'a' && pNodeName[0] <= 'z'))) {
		return false;
	}
	SetName(pNodeName);
	m_transProtoID = DDRLNB_PROTO_ID_UDP;
	m_nDataType = DDRLNB_DATATYPE_ID_STREAM;
	m_authType = DDRLNB_AUTHTYPE_NOAUTH;
	m_nState = (BYTE)3;
	return true;
}

bool ClientNode_UDP::addFD(fd_set *pSet2Read, fd_set *pSet2Write) {
#if 0
	if ((BYTE)4 == m_nState) {
#else
	if ((BYTE)4 == m_nState && m_soc.IsValid()) {
#endif
		FD_SET(m_soc.Socket(), pSet2Read);
		return true;
	} else { return false; }
}

void ClientNode_UDP::close() {
	if ((BYTE)4 == m_nState) {
		m_soc.Close();
		m_nState = (BYTE)3;
	}
}

bool ClientNode_UDP::_establishClient(unsigned long bcIP_n,
	                                  unsigned short tarPort_n) {
	m_soc = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (!m_soc.IsValid()) {
		m_nState = (BYTE)3;
		return false;
	}
	int enabling = 1;
	if (setsockopt(m_soc.Socket(), SOL_SOCKET, SO_REUSEADDR,
		           (const char*)&enabling, sizeof(enabling)) < 0) {
		return false;
	}
	if (setsockopt(m_soc.Socket(), SOL_SOCKET, SO_SNDBUF, (char*)&m_szSndBuf,
		sizeof(m_szSndBuf)) < 0) {
		return false;
	}
	if (setsockopt(m_soc.Socket(), SOL_SOCKET, SO_RCVBUF, (char*)&m_szRcvBuf,
		sizeof(m_szRcvBuf)) < 0) {
		return false;
	}
	std::vector<unsigned long> IP_n;
	if (!ListAllInterfaces(IP_n) || IP_n.empty()) {
		m_soc.Close();
		return false;
	}
	sockaddr_in myAddr;
	myAddr.sin_family = AF_INET;
	myAddr.sin_port = tarPort_n;
	myAddr.sin_addr.s_addr = 0;
	for (int i = 0; i < (int)IP_n.size(); ++i) {
		if ((IP_n[i] & 0xFFFFFF) == (bcIP_n & 0xFFFFFF)) {
			myAddr.sin_addr.s_addr = IP_n[i];
			break;
		}
	}
	if (0 == myAddr.sin_addr.s_addr) { return false; }
	if (bind(m_soc.Socket(), (struct sockaddr*)&myAddr, sizeof(myAddr)) < 0) {
		return false;
	}
	ip_mreq mreq;
	inet_pton(AF_INET, DDRLNB_UDP_CONTENT_BROADCASTING_ADDRESS_IP,
		      &(mreq.imr_multiaddr.s_addr));
	mreq.imr_interface.s_addr = myAddr.sin_addr.s_addr;
	if (setsockopt(m_soc.Socket(), IPPROTO_IP, IP_ADD_MEMBERSHIP,
		(const char*)&mreq, sizeof(mreq)) < 0) {
		return false;
	}
	return true;
}

bool ClientNode_UDP::_respond2read(std::vector<BYTE> &rcvVec) {
	while (1) {
		rcvVec.resize(UDP_MULTICAST_MAX_SLICE_SIZE);
		sockaddr_in bcAddr;
		int len = sizeof(sockaddr_in);
		int n = recvfrom(m_soc.Socket(), (char*)(&rcvVec[0]), UDP_MULTICAST_MAX_SLICE_SIZE,
			0, (sockaddr*)&bcAddr, &len);
		if (n <= 0) { return false; }
		if (n < UDP_MULTICAST_MAX_SLICE_SIZE) {
			rcvVec.resize(n);
		}
		m_FSC->FeedOneSlice(&rcvVec[0], (int)rcvVec.size());
		timeval zerotv = { 0, 0 };
		fd_set _sset;
		FD_ZERO(&_sset);
		FD_SET(m_soc.Socket(), &_sset);
		if (select(0, &_sset, nullptr, nullptr, &zerotv) == 0) { break; }
	}
	int frameInd;
	unsigned int nTotalBytes;
	int ret = m_FSC->GetCompleteFrame(&rcvVec[0], rcvVec.size(), &frameInd, &nTotalBytes);
	if (-1 == ret) {
		rcvVec.resize(nTotalBytes);
		ret = m_FSC->GetCompleteFrame(&rcvVec[0], nTotalBytes, &frameInd, &nTotalBytes);
	}
	if (ret > 0) {
		rcvVec.resize(nTotalBytes);
	} else {
		rcvVec.resize(0);
	}
	return true;
}

}