
#include <memory>
#include "CmdMoveProcessor.h"
#include "../../../Shared/src/Utility/DDRMacro.h"
#include "../../../Shared/src/Utility/Logger.h"
#include "GlobalManager.h"

using namespace DDRFramework;
using namespace DDRCommProto;

CmdMoveProcessor::CmdMoveProcessor(BaseMessageDispatcher& dispatcher) :BaseProcessor(dispatcher)
{
}


CmdMoveProcessor::~CmdMoveProcessor()
{
}

void CmdMoveProcessor::Process(std::shared_ptr<BaseSocketContainer> spSockContainer, std::shared_ptr<CommonHeader> spHeader, std::shared_ptr<google::protobuf::Message> spMsg)
{

	rspCmdMove* pRaw = reinterpret_cast<rspCmdMove*>(spMsg.get());
	if (pRaw)
	{
		DebugLog("CmdMoveProcessor : %i", pRaw->type());
	}


}

void CmdMoveProcessor::AsyncProcess(std::shared_ptr<BaseSocketContainer> spSockContainer, std::shared_ptr<DDRCommProto::CommonHeader> spHeader, std::shared_ptr<google::protobuf::Message> spMsg)
{


}
