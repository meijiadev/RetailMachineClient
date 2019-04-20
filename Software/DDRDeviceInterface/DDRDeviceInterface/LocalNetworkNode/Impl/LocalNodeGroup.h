/**
Local Device of Nodes of the DADAO Network Communication Libraries.
Mar 2018, Jianrui Long @ DADAOII

This class manages server/client nodes in a group with a separate running
thread that handles IO in an efficient multiplexing fashion. It also
broadcasts its own nodes info and retieves servers it wants to connect by
analyzing node info broadcasted by other devices.

A typical scheme to use this class is to derive from TCPServer/TCPClient/
UDPServer/UDPClient and implement user's own nodes. Then construct a
LocalNodeGroup object, AddServerNode()/AddClientNode() with conversion to
base class pointers, and FinishAddingNodes(). After these step, Launch() the
service. To obtain data as a subscriber or publish data as a publisher, call
GetData() and Publish().

It is suggested that users implement their own classes that include derived
classes of nodes (TCP/UDP * Server/Client) and a LocalNodeGroup object.
Implementation details and exposed API with potential risks can be hidden with
this strategy.

NOTE THAT IF YOUR COMBINE A LOCAL NODE GROUP AND A FEW NODES INTO ONE CLASS,
BETTER EXPLICITLY DEFINE ITS DESTRUCTOR TO "STOP" THE LOCAL NODE GORUP FIRST.
THIS IS BECAUSE THE NODES ARE PASSED TO THE NODE GROUP BY POINTERS, AND IF THEY
GET DESTRUCTED BEFORE THE NODE GROUP, THEN THE NODE GROUP CAN NOT PROPERLY
RELEASE RESOURCES RELATED TO THE NODES (DOUBLE RELEASE).

EXAMPLE:

class MyDevice {
protected:
	class Apple : public ServerNode_TCP {
	protected:
		const std::string protoTxt = "VERSION=1000\nDATATYPE AppleInfo {\n\
									 									 \'size\' : \'float\';\n\'price\' : \'float\'\n;\
																		 									 \'quantity\' : \'unsigned int\'\n}";
	public:
		Apple() {
			// TCP server node "Apple", structured data, no auth.
			SetTCPServerNode("Apple", protoTxt.c_str(), protoTxt.length() + 1, 0);
		}
	};
	class Bee : public ClientNode_UDP {
	public:
		Bee() {
			// UDP client node "Bee", streamed data, no auth.
			SetUDPClientNode("Bee", false);
		}
	};
	class Coffee : public ServerNode_UDP {
	public:
		Coffee() {
			// UDP server node "Coffee", streamed data, no auth.
			SetUDPServerNode("Coffee");
		}
	};
	class Donut : public ClientNode_TCP {
	public:
		Donut() {
			// TCP client node "Donut", streamed data, write auth.
			SetTCPClientNode("Donut", false, DDRLNB_AUTHTYPE_WRITEAUTH);
		}
	protected:
		bool _validateWriteAuth(const BYTE *pAuthStr, int nAuthLen,
			int &nextPos) override {
			nextPos = 12;
			return (strncmp((const char*)pAuthStr, "GreatDonut!",
				nAuthLen > 11 ? 11 : nAuthLen) == 0);
		}
	};
	Apple m_Apple;
	Bee m_Bee;
	Coffee m_Coffee;
	Donut m_Donut;
	LocalNodeGroup m_Dev;
public:
	MyDevice() {
		m_Dev.AddServerNode(&m_Apple);
		m_Dev.AddClientNode(&m_Bee);
		m_Dev.AddServerNode(&m_Coffee);
		m_Dev.AddClientNode(&m_Donut);
		m_Dev.FinishAddingNodes();
	}
	~MyDevice() {
		m_Dev.Stop();
	}
	void Start() { m_Dev.Launch(); }
	bool PublishApple(const BYTE *pContent, int nContentLen) {
		return m_Dev.Publish(0, pContent, nContentLen);
	}
	bool GetBee(std::vector<BYTE> &dataVec) {
		return m_Dev.GetData(0, dataVec);
	}
	bool PublishCoffee(const BYTE *pContent, int nContentLen) {
		return m_Dev.Publish(1, pContent, nContentLen);
	}
	bool GetDonut(std::vector<BYTE> &dataVec) {
		return m_Dev.GetData(1, dataVec);
	}
	void Stop() { m_Dev.Stop(); }
}

*/

#ifndef __DDR_LOCAL_DEVICE_OF_NODES_H_INCLUDED__
#define __DDR_LOCAL_DEVICE_OF_NODES_H_INCLUDED__

#include <vector>
#include <thread>
#include <atomic>
#include "ServerBase.h"
#include "ClientBase.h"
#include "SocketWrapper.h"

namespace DDRLNN
{
	class LocalNodeGroup
	{
	public:
		LocalNodeGroup();
		~LocalNodeGroup() { Stop(); }
		void SetLoopTempo(int ioTime_ms, int sleepTime_ms);
		bool AddServerNode(ServerNode_Base0 *pRawServer);
		bool AddClientNode(ClientNode_Base0 *pRawClient);
		bool FinishAddingNodes();
		bool Launch();
		bool Publish(int serverNodeInd,
			         const BYTE *pContent, int nContentLen);
		bool GetData(int clientNodeInd, std::vector<BYTE> &dataVec);
		void Stop();
		bool IsConnected(bool bServer, int nodeIndex) const {
			if (bServer) {
				if (nodeIndex >= 0 && nodeIndex < (int)m_rawServerNodes.size()) {
					return m_rawServerNodes[nodeIndex]->IsEstablished();
				} else { return false; }
			} else {
				if (nodeIndex >= 0 && nodeIndex < (int)m_rawClientNodes.size()) {
					return m_rawClientNodes[nodeIndex]->IsConnected();
				} else { return false; }
			}
		}

	protected:
		static const int NODEINFO_BROADCASTING_PERIOD_IN_MS = 200;
		// 0 - uninitialized; 1 - adding nodes; 2 - finished adding nodes; 3 - launched
		bool m_bSubThreadStarted;
		std::atomic<BYTE> m_nState;
		std::vector<ServerNode_Base0*> m_rawServerNodes;
		std::vector<ClientNode_Base0*> m_rawClientNodes;
		std::atomic<BYTE> m_bRequested2Quit;
		std::thread m_subThread;
		timeval m_ioTV;
		int m_sleepTimeMs;

		static int _threadFunc(void *pArg) {
			return ((LocalNodeGroup*)pArg)->_runThis();
		}
		int _runThis();
		static bool _getNodeInfo(const BYTE *pInfo, int nMaxInfoLen,
							     int &nextPossibleOffset,
								 std::string &name, BYTE &dataTypeID,
								 BYTE &transProtoID, BYTE &authTypeID,
								 unsigned short &port);
	};
}

#endif
