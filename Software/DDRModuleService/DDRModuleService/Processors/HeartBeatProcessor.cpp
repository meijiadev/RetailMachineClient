#include "HeartBeatProcessor.h"
#include "../../../Shared/proto/BaseCmd.pb.h"
#include "../../../Shared/src/Utility/DDRMacro.h"
#include "../Behaviors/BaseClientBehavior.h"
using namespace DDRFramework;
using namespace DDRCommProto;


HeartBeatProcessor::HeartBeatProcessor(BaseMessageDispatcher& dispatcher) : BaseProcessor::BaseProcessor(dispatcher)
{
}


HeartBeatProcessor::~HeartBeatProcessor()
{
}


void HeartBeatProcessor::Process(std::shared_ptr<BaseSocketContainer> spSockContainer, std::shared_ptr<CommonHeader> spHeader, std::shared_ptr<google::protobuf::Message> spMsg)
{
	auto bodytype = spHeader->bodytype();

	HeartBeat* pRaw = reinterpret_cast<HeartBeat*>(spMsg.get());

	std::string whatever = pRaw->whatever();


	auto sprsp = std::make_shared<HeartBeat>();
	sprsp->set_whatever("heartBeat whatever");

	spSockContainer->Send(sprsp);

	std::shared_ptr<BaseClientBehavior> spBehavior = std::dynamic_pointer_cast<BaseClientBehavior>(spSockContainer->GetTcp()->GetBehavior());
	if (spBehavior)
	{
		auto spContainer = spSockContainer->GetTcp();
		spBehavior->ResetHeartBeat(spContainer);
	}
}
