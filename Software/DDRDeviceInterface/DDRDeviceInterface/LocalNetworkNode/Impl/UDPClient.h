/**
Local Subscription UDP Client Node of the DADAO Network Communication Libraries.
Mar 2019, Jianrui Long @ DADAOII

This class derives from ClientNode_Base0 and implements server establishment
and closing, socket file descriptior addition, and received data processing.

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

#ifndef __DDR_LOCAL_UDP_CLIENT_H_INCLUDED__
#define __DDR_LOCAL_UDP_CLIENT_H_INCLUDED__

#include <memory>
#include "CommLib/CommLibInterface.h"
#include "ClientBase.h"

namespace DDRLNN
{
	class ClientNode_UDP : public ClientNode_Base0
	{
	public:
		ClientNode_UDP();
		virtual ~ClientNode_UDP() { close(); }
		bool SetUDPClientNode(const char *pNodeName);
		bool addFD(fd_set *pSet2Read, fd_set *pSet2Write) override;
		void close() override;

	protected:
		static const int DATALEN_BYTES_EACH_READOUT = 1024;
		std::shared_ptr<DDRCommLib::FrameSliceConsumer> m_FSC; // module to integrate a frame from separate slices
		//DDRCommLib::FrameSliceConsumer m_FSC; // module to integrate a frame from separate slices
		bool _establishClient(unsigned long bcIP_n, unsigned short tarPort_n) override;
		bool _respond2read(std::vector<BYTE> &rcvVec) override;
	};
}

#endif
