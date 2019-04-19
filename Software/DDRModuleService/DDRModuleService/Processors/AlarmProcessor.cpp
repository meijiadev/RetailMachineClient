#include "AlarmProcessor.h"
#include "../../../Shared/proto/BaseCmd.pb.h"
#include "../../../Shared/src/Utility/DDRMacro.h"
#include "../Behaviors/BaseClientBehavior.h"
using namespace DDRFramework;
using namespace DDRCommProto;


AlarmProcessor::AlarmProcessor(BaseMessageDispatcher& dispatcher) : BaseProcessor::BaseProcessor(dispatcher)
{
}


AlarmProcessor::~AlarmProcessor()
{
}


void AlarmProcessor::Process(std::shared_ptr<BaseSocketContainer> spSockContainer, std::shared_ptr<CommonHeader> spHeader, std::shared_ptr<google::protobuf::Message> spMsg)
{
	auto bodytype = spHeader->bodytype();
	reqTalk* pRaw = reinterpret_cast<reqTalk*>(spMsg.get());

	
}
