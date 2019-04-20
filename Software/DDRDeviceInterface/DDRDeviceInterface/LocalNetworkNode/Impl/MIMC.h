#ifndef __MULTICAST_ON_MULTIPLE_NETWORK_INTERFACES_H_INCLUDED__
#define __MULTICAST_ON_MULTIPLE_NETWORK_INTERFACES_H_INCLUDED__

#include <vector>
#include <winsock2.h>

namespace DDRLNN
{
	bool ListAllInterfaces(std::vector<unsigned long> &IPVec_n);
	class MIMCServer
	{
	public:
		MIMCServer();
		bool IsValid() const;
		void Close();
		~MIMCServer() { Close(); }
		int Setsockopt(int level, int optname,
			           const char *optval, int optlen);
		void ReInit();
		bool Broadcast(const sockaddr_in *pTargetMCAddr,
			           const char *pBuf,
					   int nBufLen);
		bool GetMCSrcAddr(std::vector<sockaddr_in> &addrs) const;

	private:
		struct _IMPL;
		_IMPL *m_pImpl;
	};

	class MIMCClient
	{
	public:
		MIMCClient();
		void Close();
		bool IsValid() const;
		~MIMCClient() { Close(); }
		int Setsockopt(int level, int optname,
			           const char *optval, int optlen);
		bool JoinGroup(const sockaddr_in *pTargetMCAddr);
		void AddToSet(fd_set *set);
		bool IsSet(fd_set *set);
		int recv(char *pBuf, int nBufLen, sockaddr *from, int *fromlen);
		void ClrSet(fd_set *set);
	private:
		struct _IMPL;
		_IMPL *m_pImpl;
	};
}

#endif // __MULTICAST_ON_MULTIPLE_NETWORK_INTERFACES_H_INCLUDED__