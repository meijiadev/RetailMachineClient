

#include <memory>
#include "../../../Shared/proto/DDRModuleCmd.pb.h"
#include "SetParameterProcessor.h"
#include "../../../Shared/src/Utility/DDRMacro.h"
#include "../../../Shared/src/Utility/Logger.h"

using namespace DDRFramework;
using namespace DDRModuleCmd;

SetParameterProcessor::SetParameterProcessor(BaseMessageDispatcher& dispatcher) :BaseProcessor(dispatcher)
{
}


SetParameterProcessor::~SetParameterProcessor()
{
}

void SetParameterProcessor::Process(std::shared_ptr<BaseSocketContainer> spSockContainer, std::shared_ptr<CommonHeader> spHeader, std::shared_ptr<google::protobuf::Message> spMsg)
{

	reqSetParameter* pRaw = reinterpret_cast<reqSetParameter*>(spMsg.get());
	if (pRaw)
	{


		auto sprsp = std::make_shared<rspSetParameter>();

		sprsp->set_ret(eRspStatus::eSuccess);

		spSockContainer->Send(sprsp);
	}

}

void SetParameterProcessor::AsyncProcess(std::shared_ptr<BaseSocketContainer> spSockContainer, std::shared_ptr<DDRCommProto::CommonHeader> spHeader, std::shared_ptr<google::protobuf::Message> spMsg)
{


}
