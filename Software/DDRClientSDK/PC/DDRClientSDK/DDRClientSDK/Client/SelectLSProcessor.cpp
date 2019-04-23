

#include <memory>
#include "../../../Shared/src/Utility/DDRMacro.h"
#include "SelectLSProcessor.h"
#include "../../../Shared/src/Utility/Logger.h"
#include "GlobalManager.h"

using namespace DDRFramework;
using namespace DDRCommProto;

SelectLSProcessor::SelectLSProcessor(BaseMessageDispatcher& dispatcher) :BaseProcessor(dispatcher)
{
}


SelectLSProcessor::~SelectLSProcessor()
{
}

void SelectLSProcessor::Process(std::shared_ptr<BaseSocketContainer> spSockContainer, std::shared_ptr<CommonHeader> spHeader, std::shared_ptr<google::protobuf::Message> spMsg)
{

	rspSelectLS* pRaw = reinterpret_cast<rspSelectLS*>(spMsg.get());
	if (pRaw)
	{
		DebugLog("Select LS:%s error: %s", pRaw->udid().c_str(),pRaw->error().c_str());
	}


}

void SelectLSProcessor::AsyncProcess(std::shared_ptr<BaseSocketContainer> spSockContainer, std::shared_ptr<DDRCommProto::CommonHeader> spHeader, std::shared_ptr<google::protobuf::Message> spMsg)
{


}
