/**
Local Subscription TCP Server Node of the DADAO Network Communication Libraries.
Mar 2019, Jianrui Long @ DADAOII

This class derives from ServerNode_Base0 and implements server establishment,
socket file descriptior addition, raw data sending to all clients,connection
closing, received instruction processing, and connection updates.

To include meaningful read/write authorization functions, derive this class and
implement your own version of write authorization generation or read
authorization validation.

General stages to run this server node:
1. SetTCPNode()
2. SetServerOption()
3. Try2Launch()
In a loop of the server thread:
{
4. addReadFD()
5. select()
6. processIO()
}
In a loop of the data-feeding thread:
{
7. PublishContent()
}
*/

#ifndef __DDR_LOCAL_TCP_SERVER_H_INCLUDED__
#define __DDR_LOCAL_TCP_SERVER_H_INCLUDED__

#include <atomic>
#include "ServerBase.h"

namespace DDRLNN
{
	class ServerNode_TCP : public ServerNode_Base0
	{
	public:
		ServerNode_TCP() : m_connCnt(0) {}
		virtual ~ServerNode_TCP() { close(); }
		bool SetTCPServerNode(const char *pNodeName,
			                  BYTE authType // 0 - no auth.; 1 - read auth. (C->S); 2 - write auth. (S->C)
							  );
		bool addReadFD(fd_set *pSet2Append) override;
		void close() override;
		int GetNumOfConnections() const override { return m_connCnt; }

	protected:
		static const int RCVBUFF_SIZE_FOR_EACH_CLIENT = 256;
		static const int MAXTIME_FOR_READ_AUTH_MS = 3000;
		static const int CLIENTS_UPDATE_PERIOD_MS = 200;
		static const int MAXTIME_FOR_INACTIVE_CONN_MS = 10000;

		SocketWrapper m_soc; // server socket
		typedef struct __oneClient {
			SocketWrapper clt_soc;
			sockaddr_in conn_addr;
			char rcvBuf[RCVBUFF_SIZE_FOR_EACH_CLIENT];
			int rcvPos;
			BYTE stat; // 0 - accepted and yet to be authenticated (read auth.); 1 - authenticated and available
			std::chrono::system_clock::time_point lastActiveTime;
			__oneClient() : stat(0) {}
		} _oneClient;
		std::vector<__oneClient> m_clients;
		std::atomic<int> m_connCnt;
		std::chrono::system_clock::time_point m_lastClientUpdate;

		bool _establishServer() override;
		void _send2all(const BYTE *pData, int nDataLen) override;
		void _processRecv(fd_set *pSetRead) override; // TCP definition to process bytes received from clients, overriding default UDP behavior
		BYTE _parseRecv(const BYTE *pRcvBuf, int nBufLen, int &nextOffset); // return status bits. bit #0 - HB; bit #1 - ProtocolRequired
		void _updateConnections() override;
	};
}

#endif
