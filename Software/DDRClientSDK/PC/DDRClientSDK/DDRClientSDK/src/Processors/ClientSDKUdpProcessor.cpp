#include "ClientSDKUdpProcessor.h"
#include <memory>
#include "src/Utility/DDRMacro.h"
#include "src/Utility/CommonFunc.h"
#include "thirdparty/asio/include/asio.hpp"
#include "src/ClientSDK/ClientSDKTcpClient.h"
#include "src/Interface/DDRClientInterface.h"

using namespace DDRFramework;
using namespace DDRCommProto;

ClientSDKUdpProcessor::ClientSDKUdpProcessor(BaseMessageDispatcher& dispatcher) :BaseProcessor(dispatcher)
{
}


ClientSDKUdpProcessor::~ClientSDKUdpProcessor()
{
	DebugLog("ClientSDKUdpProcessor Destroy");
}

void ClientSDKUdpProcessor::Process(std::shared_ptr<BaseSocketContainer> spSockContainer, std::shared_ptr<CommonHeader> spHeader, std::shared_ptr<google::protobuf::Message> spMsg)
{
	auto bodytype = spHeader->bodytype();

	bcLSAddr* pRaw = reinterpret_cast<bcLSAddr*>(spMsg.get());


	for (auto serverinfo : pRaw->lsinfos())
	{
		if (serverinfo.stype() == bcLSAddr_eServiceType_LocalServer)
		{
			DealLocalServer(serverinfo);
		}
	}



}
void ClientSDKUdpProcessor::DealLocalServer(bcLSAddr_ServerInfo& serverinfo)
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

		DebugLog("Receive Broadcast %s: %s:%s", name.c_str(), conntectip.c_str(),sPort.c_str());

		DDRSDK::DDRClientInterface::CallUdpReceiver(name, conntectip, sPort);

	}
	else
	{


		DebugLog("Receive Server Broadcast But No IP in same segment");
	}

}
