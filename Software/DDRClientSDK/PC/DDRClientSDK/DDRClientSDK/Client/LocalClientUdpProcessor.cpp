#include "LocalClientUdpProcessor.h"
#include <memory>
#include "../../../Shared/src/Utility/DDRMacro.h"
#include "../../../Shared/src/Utility/CommonFunc.h"
#include "../../../Shared/thirdparty/asio/include/asio.hpp"
#include "LocalTcpClient.h"
#include "GlobalManager.h"

using namespace DDRFramework;
using namespace DDRCommProto;

LocalClientUdpProcessor::LocalClientUdpProcessor(BaseMessageDispatcher& dispatcher) :BaseProcessor(dispatcher)
{
}


LocalClientUdpProcessor::~LocalClientUdpProcessor()
{
	DebugLog("LocalClientUdpProcessor Destroy");
}

void LocalClientUdpProcessor::Process(std::shared_ptr<BaseSocketContainer> spSockContainer, std::shared_ptr<CommonHeader> spHeader, std::shared_ptr<google::protobuf::Message> spMsg)
{
	auto bodytype = spHeader->bodytype();

	bcLSAddr* pRaw = reinterpret_cast<bcLSAddr*>(spMsg.get());


	for (auto serverinfo : pRaw->lsinfos())
	{
		if (serverinfo.robotid() == GlobalManager::Instance()->GetRobotID())
		{

			if (serverinfo.stype() == bcLSAddr_eServiceType_LocalServer)
			{
				DealLocalServer(serverinfo);
			}
			else if (serverinfo.stype() == bcLSAddr_eServiceType_RemoteServer)
			{
			}
			else if (serverinfo.stype() == bcLSAddr_eServiceType_RTSPStreamServer)
			{
			}
			else if (serverinfo.stype() == bcLSAddr_eServiceType_TalkBackServer)
			{
			}
		}
		else
		{
			//Not Same Robot , Abandon
		}

	}



}
void LocalClientUdpProcessor::DealLocalServer(bcLSAddr_ServerInfo& serverinfo)
{
	std::string name = serverinfo.name();
	std::string ips;
	int port = serverinfo.port();


	auto localAddr = DDRFramework::GetLocalIPV4();


	std::vector<std::string> remoteAddrs;
	for (auto ip : serverinfo.ips())
	{
		remoteAddrs.push_back(ip);
	}

	auto rmap = DDRFramework::GetSameSegmentIPV4(localAddr, remoteAddrs);
	if (rmap.size() > 0)
	{
		auto conntectip = (rmap.begin())->second;

		std::string sPort = std::to_string(port);
		GlobalManager::Instance()->SetServerAddr(conntectip, sPort);
		//DebugLog("Receive Server Broadcast %s: %s", name.c_str(), conntectip.c_str());
	}
	else
	{


		DebugLog("Receive Server Broadcast But No IP in same segment");
	}

}
