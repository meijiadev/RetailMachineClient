

#include <memory>
#include "proto/BaseCmd.pb.h"
#include "ClientSDKProcessor.h"
#include "src/Utility/DDRMacro.h"
#include "src/Utility/Logger.h"
#include "src/Interface/DDRClientInterface.h"
using namespace DDRFramework;
using namespace DDRCommProto;




namespace DDRSDK
{

	ClientSDKProcessor::ClientSDKProcessor(BaseMessageDispatcher& dispatcher) :BaseProcessor(dispatcher)
	{
	}


	ClientSDKProcessor::~ClientSDKProcessor()
	{
	}

	void ClientSDKProcessor::Process(std::shared_ptr<BaseSocketContainer> spSockContainer, std::shared_ptr<CommonHeader> spHeader, std::shared_ptr<google::protobuf::Message> spMsg)
	{

		DDRSDK::DDRClientInterface::Dispatch(spHeader,spMsg);

	}

	void ClientSDKProcessor::AsyncProcess(std::shared_ptr<BaseSocketContainer> spSockContainer, std::shared_ptr<DDRCommProto::CommonHeader> spHeader, std::shared_ptr<google::protobuf::Message> spMsg)
	{


	}
}