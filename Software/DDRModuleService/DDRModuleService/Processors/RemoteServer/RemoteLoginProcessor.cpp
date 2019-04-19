

#include <memory>
#include "../../../Shared/proto/BaseCmd.pb.h"
#include "RemoteLoginProcessor.h"
#include "../../../Shared/src/Utility/DDRMacro.h"
#include "../../../Shared/src/Utility/Logger.h"

#include "../../Managers/StreamRelayServiceManager.h"


using namespace DDRFramework;
using namespace DDRCommProto;

RemoteLoginProcessor::RemoteLoginProcessor(BaseMessageDispatcher& dispatcher) :BaseProcessor(dispatcher)
{
}


RemoteLoginProcessor::~RemoteLoginProcessor()
{
}

void RemoteLoginProcessor::Process(std::shared_ptr<BaseSocketContainer> spSockContainer, std::shared_ptr<CommonHeader> spHeader, std::shared_ptr<google::protobuf::Message> spMsg)
{

	rspRemoteLogin* pRaw = reinterpret_cast<rspRemoteLogin*>(spMsg.get());
	if (pRaw)
	{
		
	}


}

void RemoteLoginProcessor::AsyncProcess(std::shared_ptr<BaseSocketContainer> spSockContainer, std::shared_ptr<DDRCommProto::CommonHeader> spHeader, std::shared_ptr<google::protobuf::Message> spMsg)
{


}
