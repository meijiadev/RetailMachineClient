#include "StreamRelayAlarmProcessor.h"
#include <memory>
#include "../../../Shared/proto/BaseCmd.pb.h"
#include "../../../Shared/src/Utility/DDRMacro.h"

#include "../../../Shared/src/Utility/Logger.h"
#include "../../Managers/GlobalManager.h"
#include "../../Managers/StreamRelayServiceManager.h"
using namespace DDRFramework;
using namespace DDRCommProto;

StreamRelayAlarmProcessor::StreamRelayAlarmProcessor(BaseMessageDispatcher& dispatcher) :BaseProcessor(dispatcher)
{ 
}


StreamRelayAlarmProcessor::~StreamRelayAlarmProcessor()
{
}

void StreamRelayAlarmProcessor::Process(std::shared_ptr<BaseSocketContainer> spSockContainer, std::shared_ptr<CommonHeader> spHeader, std::shared_ptr<google::protobuf::Message> spMsg)
{

	reqStreamRelayAlarm* pRaw = reinterpret_cast<reqStreamRelayAlarm*>(spMsg.get());


	auto sprsp = std::make_shared<notifyAlarm>();
	sprsp->set_from(eCltType::eLSMStreamRelay);

	std::string ip = spSockContainer->GetTcp()->GetSocket().remote_endpoint().address().to_string();

	sprsp->set_fromip(ip);
	sprsp->set_error(pRaw->error());


	for (auto pair : GlobalManager::Instance()->GetTcpServer()->GetTcpSocketContainerMap())
	{
		for (auto clttype : pRaw->to())
		{

			auto spSession = pair.second;
			auto spServerSessionTcp = dynamic_pointer_cast<LocalServerTcpSession>(spSession);
			if (spServerSessionTcp->GetLoginInfo().type() == clttype)
			{
				spServerSessionTcp->Send(sprsp);
			}
		}
	}


}