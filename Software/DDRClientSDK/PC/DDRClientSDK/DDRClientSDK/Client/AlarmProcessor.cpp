

#include "AlarmProcessor.h"
#include "GlobalManager.h"
#include <memory>
#include "../../../Shared/src/Utility/DDRMacro.h"
#include "../../../Shared/src/Utility/Logger.h"

using namespace DDRFramework;
using namespace DDRCommProto;

AlarmProcessor::AlarmProcessor(BaseMessageDispatcher& dispatcher) :BaseProcessor(dispatcher)
{
}


AlarmProcessor::~AlarmProcessor()
{
}

void AlarmProcessor::Process(std::shared_ptr<BaseSocketContainer> spSockContainer, std::shared_ptr<CommonHeader> spHeader, std::shared_ptr<google::protobuf::Message> spMsg)
{

	notifyAlarm* pRaw = reinterpret_cast<notifyAlarm*>(spMsg.get());
	DebugLog("%s", GlobalManager::Instance()->GetLocalizationConfig().GetString(pRaw->error()).c_str());

}

void AlarmProcessor::AsyncProcess(std::shared_ptr<BaseSocketContainer> spSockContainer, std::shared_ptr<DDRCommProto::CommonHeader> spHeader, std::shared_ptr<google::protobuf::Message> spMsg)
{


}
