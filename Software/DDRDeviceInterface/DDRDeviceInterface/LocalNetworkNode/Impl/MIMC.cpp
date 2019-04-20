#include "stdafx.h"

#include "MIMC.h"

#include <ws2tcpip.h>
#include <ws2ipdef.h>
#include <iphlpapi.h>
#include "SocketWrapper.h"
#pragma comment(lib, "iphlpapi.lib")
#pragma comment (lib,"ws2_32.lib")

namespace DDRLNN {

bool ListAllInterfaces(std::vector<unsigned long> &IPVec_n) {
	PMIB_IPADDRTABLE pIPAddrTable;
	DWORD dwSize = 0;
	DWORD dwRetVal = 0;
	pIPAddrTable = (MIB_IPADDRTABLE *)malloc(sizeof(MIB_IPADDRTABLE));
	if (pIPAddrTable) {
		if (GetIpAddrTable(pIPAddrTable, &dwSize, 0) ==	ERROR_INSUFFICIENT_BUFFER) {
			free(pIPAddrTable);
			pIPAddrTable = (MIB_IPADDRTABLE *)malloc(dwSize);
		}
		if (!pIPAddrTable) { return false; }
	}
	if ((dwRetVal = GetIpAddrTable(pIPAddrTable, &dwSize, 0)) != NO_ERROR) {
		return false;
	}
	IPVec_n.resize(0);
	for (int i = 0; i < (int)pIPAddrTable->dwNumEntries; ++i) {
		if (pIPAddrTable->table[i].dwAddr != 0x0100007F) {
			IPVec_n.emplace_back(pIPAddrTable->table[i].dwAddr);
		}
	}
	if (pIPAddrTable) {
		free(pIPAddrTable);
	}
	return true;
}

struct MIMCServer::_IMPL{
	bool bInitialized;
	std::vector<SocketWrapper> socs;
	_IMPL() {
		bInitialized = false;
		std::vector<unsigned long> IP_n;
		if (!ListAllInterfaces(IP_n) || IP_n.empty()) { return; }
		socs.resize(0);
		for (int i = 0; i < (int)IP_n.size(); ++i) {
			SOCKET s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
			if (INVALID_SOCKET == s) { continue; }
			sockaddr_in localAddr;
			localAddr.sin_family = AF_INET;
			localAddr.sin_addr.s_addr = IP_n[i];
			localAddr.sin_port = htons(0);
			if (bind(s, (struct sockaddr*)&localAddr, sizeof(localAddr)) >= 0) {
				socs.emplace_back(s);
			}
		}
		bInitialized = true;
	}
};

MIMCServer::MIMCServer() {
	m_pImpl = new _IMPL;
}

bool MIMCServer::IsValid() const {
	return (m_pImpl != nullptr && m_pImpl->bInitialized);
}

void MIMCServer::ReInit() {
	if (m_pImpl) {
		delete m_pImpl;
	}
	m_pImpl = new _IMPL;
}

void MIMCServer::Close() {
	if (m_pImpl) {
		delete m_pImpl;
		m_pImpl = nullptr;
	}
}

int MIMCServer::Setsockopt(int level, int optname,
	                       const char *optval, int optlen) {
	if (IsValid()) {
		for (int i = 0; i < (int)m_pImpl->socs.size(); ++i) {
			int nRet = ::setsockopt(m_pImpl->socs[i].Socket(), level, optname, optval, optlen);
			if (nRet < 0) {
				return nRet;
			}
		}
	}
	return 0;
}

bool MIMCServer::Broadcast(const sockaddr_in *pTargetMCAddr,
	                       const char *pBuf, int nBufLen) {
	if (!m_pImpl || !m_pImpl->bInitialized) { return false; }
	for (int i = 0; i < (int)m_pImpl->socs.size(); ++i) {
		sendto(m_pImpl->socs[i].Socket(), pBuf, nBufLen, 0,
			   (const sockaddr*)pTargetMCAddr, sizeof(sockaddr_in));
	}
	return true;
}

bool MIMCServer::GetMCSrcAddr(std::vector<sockaddr_in> &addrs) const {
	if (IsValid()) {
		addrs.resize(0);
		sockaddr_in myAddr;
		int lll = sizeof(sockaddr_in);
		for (int i = 0; i < (int)m_pImpl->socs.size(); ++i) {
			if (getsockname(m_pImpl->socs[i].Socket(), (sockaddr*)&myAddr, &lll) < 0) {
				return false;
			} else {
				addrs.emplace_back(myAddr);
			}
		}
		return true;
	}
	return false;
}

struct MIMCClient::_IMPL {
	bool bInitialized;
	std::vector<SocketWrapper> socs;
	_IMPL() : bInitialized(false) {}
	bool Join(const sockaddr_in *pTargetMCAddr) {
		if (bInitialized) {
			socs.resize(0);
			bInitialized = false;
		}
		std::vector<unsigned long> IP_n;
		if (!ListAllInterfaces(IP_n) || IP_n.empty()) { return false; }
		for (int i = 0; i < (int)IP_n.size(); ++i) {
			SOCKET s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
			int enabling = 1;
			if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR,
				(const char*)&enabling, sizeof(enabling)) < 0) {
				closesocket(s);
				continue;
			}
			sockaddr_in lAddr;
			lAddr.sin_family = AF_INET;
			lAddr.sin_addr.s_addr = IP_n[i];
			lAddr.sin_port = pTargetMCAddr->sin_port;
			if (bind(s, (const sockaddr*)&lAddr, sizeof(lAddr)) < 0) {
				closesocket(s);
				continue;
			}
			ip_mreq mreq;
			mreq.imr_multiaddr.s_addr = pTargetMCAddr->sin_addr.s_addr;
			mreq.imr_interface.s_addr = IP_n[i];
			if (setsockopt(s, IPPROTO_IP, IP_ADD_MEMBERSHIP,
				(const char*)&mreq, sizeof(mreq)) < 0) {
				closesocket(s);
			} else {
				socs.emplace_back(s);
			}
		}
		bInitialized = (socs.size() > 0);
		return bInitialized;
	}
};

MIMCClient::MIMCClient() {
	m_pImpl = new _IMPL;
}

void MIMCClient::Close() {
	if (m_pImpl) {
		delete m_pImpl;
		m_pImpl = nullptr;
	}
}

bool MIMCClient::IsValid() const {
	return (m_pImpl != nullptr && m_pImpl->bInitialized);
}

int MIMCClient::Setsockopt(int level, int optname,
	                       const char *optval, int optlen) {
	if (m_pImpl) {
		for (int i = 0; i < (int)m_pImpl->socs.size(); ++i) {
			int nRet = ::setsockopt(m_pImpl->socs[i].Socket(), level, optname, optval, optlen);
			if (nRet < 0) {
				return nRet;
			}
		}
	}
	return 0;
}

bool MIMCClient::JoinGroup(const sockaddr_in *pTargetMCAddr) {
	if (m_pImpl) {
		return m_pImpl->Join(pTargetMCAddr);
	} else { return false; }
}

void MIMCClient::AddToSet(fd_set *set) {
	if (IsValid()) {
		for (int i = 0; i < (int)m_pImpl->socs.size(); ++i) {
			FD_SET(m_pImpl->socs[i].Socket(), set);
		}
	}
}

bool MIMCClient::IsSet(fd_set *set) {
	if (IsValid()) {
		for (int i = 0; i < (int)m_pImpl->socs.size(); ++i) {
			if (FD_ISSET(m_pImpl->socs[i].Socket(), set)) {
				return true;
			}
		}
	}
	return false;
}

int MIMCClient::recv(char *pBuf, int nBufLen, sockaddr *from, int *fromlen) {
	if (IsValid()) {
		fd_set ss;
		FD_ZERO(&ss);
		for (int i = 0; i < (int)m_pImpl->socs.size(); ++i) {
			FD_SET(m_pImpl->socs[i].Socket(), &ss);
		}
		timeval ztv = { 0, 0 };
		if (select(0, &ss, nullptr, nullptr, &ztv) > 0) {
			for (int i = 0; i < (int)m_pImpl->socs.size(); ++i) {
				if (FD_ISSET(m_pImpl->socs[i].Socket(), &ss)) {
					return recvfrom(m_pImpl->socs[i].Socket(), pBuf, nBufLen,
						            0, from, fromlen);
				}
			}
		}
	}
	return 0;
}

void MIMCClient::ClrSet(fd_set *set) {
	if (IsValid()) {
		for (int i = 0; i < (int)m_pImpl->socs.size(); ++i) {
			FD_CLR(m_pImpl->socs[i].Socket(), set);
		}
	}
}

}