

#include <memory>
#include "UnselectLSProcessor.h"
#include "../../../Shared/src/Utility/DDRMacro.h"
#include "../../../Shared/src/Utility/Logger.h"
#include "GlobalManager.h"

using namespace DDRFramework;
using namespace DDRCommProto;

UnselectLSProcessor::UnselectLSProcessor(BaseMessageDispatcher& dispatcher) :BaseProcessor(dispatcher)
{
}


UnselectLSProcessor::~UnselectLSProcessor()
{
}

void UnselectLSProcessor::Process(std::shared_ptr<BaseSocketContainer> spSockContainer, std::shared_ptr<CommonHeader> spHeader, std::shared_ptr<google::protobuf::Message> spMsg)
{

	rspUnselectLS* pRaw = reinterpret_cast<rspUnselectLS*>(spMsg.get());
	if (pRaw)
	{
		DebugLog("Unelect LS: %s  Error: %s",pRaw->udid().c_str(), pRaw->error().c_str());
	}


}

void UnselectLSProcessor::AsyncProcess(std::shared_ptr<BaseSocketContainer> spSockContainer, std::shared_ptr<DDRCommProto::CommonHeader> spHeader, std::shared_ptr<google::protobuf::Message> spMsg)
{


}
