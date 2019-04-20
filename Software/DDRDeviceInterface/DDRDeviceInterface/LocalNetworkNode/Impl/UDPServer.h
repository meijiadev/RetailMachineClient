/**
Local Subscription UDP Server Node of the DADAO Network Communication Libraries.
Mar 2019, Jianrui Long @ DADAOII

This class derives from ServerNode_Base0 and implements server establishment,
raw data sending to all clients, and connection closing.

NOTE THAT NO AUTHORIZATION IS FEASIBLE IN THIS MODEL BECAUSE OF ITS NON
CONNECTION-ORIENTED NATURE.

General stages to run this server node:
1. SetUDPNode()
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

#ifndef __DDR_LOCAL_UDP_SERVER_H_INCLUDED__
#define __DDR_LOCAL_UDP_SERVER_H_INCLUDED__

#include <memory>
#include "CommLib/CommLibInterface.h"
#include "ServerBase.h"
#include "MIMC.h"

namespace DDRLNN
{
	class ServerNode_UDP : public ServerNode_Base0
	{
	public:
		ServerNode_UDP();
		virtual ~ServerNode_UDP() { close(); }
		bool SetUDPServerNode(const char *pNodeName,
			                  const char *pSDProtoTxt = nullptr,
							  int nSDProtoLen = 0);
		void close() override;
		int GetNumOfConnections() const override { return -1; }

	protected:
		static const int UDP_MULTICAST_PORT_TEST_PERIOD_IN_MS = 800;
		
		sockaddr_in m_tarMCAddr;
		MIMCServer m_MCServer;
		std::shared_ptr<DDRCommLib::FrameSliceGenerator> m_FSG;
		BYTE m_slice[UDP_MULTICAST_MAX_SLICE_SIZE];
		std::vector<sockaddr_in> m_MCSrcSocks;
		MIMCClient m_MCMonitor;

		bool _establishServer() override;
		void _send2all(const BYTE *pData, int nDataLen) override;
	};
}

#endif
