#include "CmdProcessor.h"
#include "../../../Shared/proto/BaseCmd.pb.h"
#include "../../../Shared/src/Utility/DDRMacro.h"
#include "../Behaviors/BaseClientBehavior.h"
using namespace DDRFramework;
using namespace DDRCommProto;


CmdProcessor::CmdProcessor(BaseMessageDispatcher& dispatcher) : BaseProcessor::BaseProcessor(dispatcher)
{
}


CmdProcessor::~CmdProcessor()
{
}


void CmdProcessor::Process(std::shared_ptr<BaseSocketContainer> spSockContainer, std::shared_ptr<CommonHeader> spHeader, std::shared_ptr<google::protobuf::Message> spMsg)
{
	auto bodytype = spHeader->bodytype();

	
}
