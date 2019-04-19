

#include <memory>
#include "../../../Shared/proto/BaseCmd.pb.h"
#include "RemoteServerListProcessor.h"
#include "../../../Shared/src/Utility/DDRMacro.h"
#include "../../../Shared/src/Utility/Logger.h"
#include "../../Managers/GlobalManager.h"

#include "../../LSClient/LSClientManager.h"
#include <string>
#include <iostream>

using namespace DDRFramework;
using namespace DDRCommProto;

RemoteServerListProcessor::RemoteServerListProcessor(BaseMessageDispatcher& dispatcher) :BaseProcessor(dispatcher)
{
}


RemoteServerListProcessor::~RemoteServerListProcessor()
{
}

void RemoteServerListProcessor::Process(std::shared_ptr<BaseSocketContainer> spSockContainer, std::shared_ptr<CommonHeader> spHeader, std::shared_ptr<google::protobuf::Message> spMsg)
{
	rspRemoteServerList* pRaw = reinterpret_cast<rspRemoteServerList*>(spMsg.get());

	auto servers = pRaw->servers();
	std::vector<DDRCommProto::rspRemoteServerList_RemoteServer> serverVec;
	for (auto server : servers)
	{
		serverVec.push_back(server);
		DebugLog("Get Server:%s", server.ip().c_str());
	}
	if (servers.size() > 0)
	{
		LSClientManager::Instance()->CloseBroadcastServer(serverVec);

		//auto connect remote server if exist
		LSClientManager::Instance()->ConnectRemoteServer();
	}
}

void RemoteServerListProcessor::AsyncProcess(std::shared_ptr<BaseSocketContainer> spSockContainer, std::shared_ptr<DDRCommProto::CommonHeader> spHeader, std::shared_ptr<google::protobuf::Message> spMsg)
{


}
