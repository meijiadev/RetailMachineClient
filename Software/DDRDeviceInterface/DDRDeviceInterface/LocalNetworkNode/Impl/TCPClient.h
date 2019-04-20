/**
Local Subscription TCP Client Node of the DADAO Network Communication Libraries.
Mar 2019, Jianrui Long @ DADAOII

This class derives from ClientNode_Base0 and implements server establishment
and closing, socket file descriptior addition, received data processing, and
connection updates.

To include meaningful read/write authorization functions, derive this class and
implement your own version of read authorization generation or write
authorization validation.

General stages to run this server node:
1. SetTCPNode()
2. SetServerOption()
3*. User defines his own way to read node info being broadcasted at
DDRLNB_NODEINFO_BROADCASTING_ADDRESS_IP:DDRLNB_NODEINFO_BROADCASTING_ADDRESS_PORT
till a string is received.
4. Try2Launch() using the received node info
5. addFD()
6. select()
7. processIO()
Stage 3-7 are generally repeated in a loop in a one thread. In another
data-feeding thread, do:
8. GetData()
Statge 8 is generally repeated in a loop.
*/

#ifndef __DDR_LOCAL_TCP_CLIENT_H_INCLUDED__
#define __DDR_LOCAL_TCP_CLIENT_H_INCLUDED__

#include "ClientBase.h"

namespace DDRLNN
{
	class ClientNode_TCP : public ClientNode_Base0
	{
	public:
		ClientNode_TCP();
		virtual ~ClientNode_TCP();
		bool SetTCPClientNode(const char *pNodeName,
							  BYTE authType = DDRLNB_AUTHTYPE_NOAUTH // 0 - no auth.; 1 - read auth. (C->S); 2 - write auth. (S->C)
							  );
		bool addFD(fd_set *pSet2Read, fd_set *pSet2Write) override;
		void close() override;
		bool IsConnected() const override { return (4 == m_nState && 2 == m_connStat); }

	protected:
		static const int MAXTIME_FOR_CONNECTION = 2000;
		static const int MAX_SIZE_FOR_WRITE_AUTH  = 256;
		static const int MAXTIME_FOR_WRITE_AUTH_MS = 3000;
		static const int DATALEN_BYTES_EACH_READOUT = 1024;
		static const int HEART_BEAT_PERIOD_IN_MS = 1000;
		// 0 - connection initiated; 1 - connection accepted and yet to be authenticated (write auth.);
		// 2 - authenticated and available
		BYTE m_connStat;
		std::chrono::system_clock::time_point m_lastHBTime;
		std::vector<BYTE> *m_pWriteAuthStr;

		bool _establishClient(unsigned long bcIP, unsigned short serPort) override;
		bool _respond2write() override;
		bool _respond2read(std::vector<BYTE> &rcvVec) override;
		bool _updateConnection() override;
		void _sendHB() override;
	};
}

#endif
