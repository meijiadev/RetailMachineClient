

#include <memory>
#include "../../../Shared/proto/DDRModuleCmd.pb.h"
#include "RobotVersionInfoProcessor.h"
#include "../../../Shared/src/Utility/DDRMacro.h"
#include "../../../Shared/src/Utility/Logger.h"

using namespace DDRFramework;
using namespace DDRModuleProto;

RobotVersionInfoProcessor::RobotVersionInfoProcessor(BaseMessageDispatcher& dispatcher) :BaseProcessor(dispatcher)
{
}


RobotVersionInfoProcessor::~RobotVersionInfoProcessor()
{
}

void RobotVersionInfoProcessor::Process(std::shared_ptr<BaseSocketContainer> spSockContainer, std::shared_ptr<CommonHeader> spHeader, std::shared_ptr<google::protobuf::Message> spMsg)
{

	reqRobotVersionInfo* pRaw = reinterpret_cast<reqRobotVersionInfo*>(spMsg.get());
	if (pRaw)
	{


		auto sprsp = std::make_shared<rspRobotVersionInfo>();

		sprsp->set_robotid("Module Service");
		sprsp->set_hardwareversion("Module Hardware 1.0");

		spSockContainer->Send(sprsp);
	}

}

void RobotVersionInfoProcessor::AsyncProcess(std::shared_ptr<BaseSocketContainer> spSockContainer, std::shared_ptr<DDRCommProto::CommonHeader> spHeader, std::shared_ptr<google::protobuf::Message> spMsg)
{


}
