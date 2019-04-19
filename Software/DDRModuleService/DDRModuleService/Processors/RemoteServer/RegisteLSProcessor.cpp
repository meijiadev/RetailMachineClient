

#include <memory>
#include "../../../Shared/proto/BaseCmd.pb.h"
#include "RegisteLSProcessor.h"
#include "../../../Shared/src/Utility/DDRMacro.h"
#include "../../../Shared/src/Utility/Logger.h"
#include "../../Managers/StreamRelayServiceManager.h"

using namespace DDRFramework;
using namespace DDRCommProto;

RegisteLSProcessor::RegisteLSProcessor(BaseMessageDispatcher& dispatcher) :BaseProcessor(dispatcher)
{
}


RegisteLSProcessor::~RegisteLSProcessor()
{
}

void RegisteLSProcessor::Process(std::shared_ptr<BaseSocketContainer> spSockContainer, std::shared_ptr<CommonHeader> spHeader, std::shared_ptr<google::protobuf::Message> spMsg)
{

	rspRegisteLS* pRaw = reinterpret_cast<rspRegisteLS*>(spMsg.get());

	if (pRaw)
	{

		if (pRaw->error() == "")
		{

			auto spreq = std::make_shared<reqRtspStreamUploadAddr>();

			auto channels = StreamRelayServiceManager::Instance()->m_ChannelsToUploadConfig;

			for (auto channel : channels)
			{
				auto c = spreq->add_channels();
				c->CopyFrom(channel);
			}

			auto spTcp = spSockContainer->GetTcp();
		
			if (spTcp)
			{
				spTcp->Send(spreq);
			}
		}

	}






}

void RegisteLSProcessor::AsyncProcess(std::shared_ptr<BaseSocketContainer> spSockContainer, std::shared_ptr<DDRCommProto::CommonHeader> spHeader, std::shared_ptr<google::protobuf::Message> spMsg)
{


}
