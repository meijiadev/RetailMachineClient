#include "LoginProcessor.h"
#include <memory>
#include "../../../Shared/src/Utility/DDRMacro.h"

#include "../../../Shared/src/Utility/Logger.h"
#include "GlobalManager.h"
using namespace DDRFramework;
using namespace DDRCommProto;

LoginProcessor::LoginProcessor(BaseMessageDispatcher& dispatcher) :BaseProcessor(dispatcher)
{
}


LoginProcessor::~LoginProcessor()
{
}

void LoginProcessor::Process(std::shared_ptr<BaseSocketContainer> spSockContainer, std::shared_ptr<CommonHeader> spHeader, std::shared_ptr<google::protobuf::Message> spMsg)
{

	rspLogin* pRaw = reinterpret_cast<rspLogin*>(spMsg.get());


	rspLogin_eLoginRetCode retcode = pRaw->retcode();

	if (retcode == rspLogin_eLoginRetCode_success)
	{

	

		DebugLog("--------------------------------------------Login Success:%i Priority:%i", retcode,pRaw->priority());
	}
	else
	{
		DebugLog("--------------------------------------------Login Failed:%i", retcode);

	}


}