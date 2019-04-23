

#include <memory>
#include "../../../Shared/src/Utility/DDRMacro.h"
#include "RemoteServerListProcessor.h"
#include "../../../Shared/src/Utility/Logger.h"
#include "GlobalManager.h"

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
	for (auto server : servers)
	{
		DebugLog("Get Server:%s", server.ip().c_str());
	}
	if (servers.size() > 0)
	{
		auto server = servers[0];
		GlobalManager::Instance()->CloseBroadcastServer();
		GlobalManager::Instance()->TcpConnect(server.ip(), std::to_string(server.port()));
	}
}

void RemoteServerListProcessor::AsyncProcess(std::shared_ptr<BaseSocketContainer> spSockContainer, std::shared_ptr<DDRCommProto::CommonHeader> spHeader, std::shared_ptr<google::protobuf::Message> spMsg)
{


}
