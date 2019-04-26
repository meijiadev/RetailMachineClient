#include "LocalServerUdpProcessor.h"
#include <memory>
#include "proto/BaseCmd.pb.h"
#include "src/Utility/DDRMacro.h"
#include "src/Utility/CommonFunc.h"
#include "src/Network/BaseSocketContainer.h"
using namespace DDRFramework;
using namespace DDRCommProto;

LocalServerUdpProcessor::LocalServerUdpProcessor(BaseMessageDispatcher& dispatcher) :BaseProcessor(dispatcher)
{
}


LocalServerUdpProcessor::~LocalServerUdpProcessor()
{
}

void LocalServerUdpProcessor::Process(std::shared_ptr<BaseSocketContainer> spSockContainer, std::shared_ptr<CommonHeader> spHeader, std::shared_ptr<google::protobuf::Message> spMsg)
{
	auto bodytype = spHeader->bodytype();

	reqLogin* pRaw = reinterpret_cast<reqLogin*>(spMsg.get());

	std::string name = pRaw->username();


	auto sprsp = std::make_shared<rspLogin>();
	sprsp->set_retcode(rspLogin_eLoginRetCode_success);

	spSockContainer->Send(sprsp);

	//DebugLog("Login %s:" ,name.c_str());


}