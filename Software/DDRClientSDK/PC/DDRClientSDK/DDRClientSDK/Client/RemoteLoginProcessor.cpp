

#include <memory>
#include "../../../Shared/src/Utility/DDRMacro.h"
#include "RemoteLoginProcessor.h"
#include "../../../Shared/src/Utility/Logger.h"
#include "GlobalManager.h"

using namespace DDRFramework;
using namespace DDRCommProto;

RemoteLoginProcessor::RemoteLoginProcessor(BaseMessageDispatcher& dispatcher) :BaseProcessor(dispatcher)
{
}


RemoteLoginProcessor::~RemoteLoginProcessor()
{
}

void RemoteLoginProcessor::Process(std::shared_ptr<BaseSocketContainer> spSockContainer, std::shared_ptr<CommonHeader> spHeader, std::shared_ptr<google::protobuf::Message> spMsg)
{

	rspRemoteLogin* pRaw = reinterpret_cast<rspRemoteLogin*>(spMsg.get());
	if (pRaw)
	{
		rspRemoteLogin_eRemoteLoginRetCode retcode = pRaw->retcode();


		if (retcode == rspRemoteLogin_eRemoteLoginRetCode_success)
		{


			GlobalManager::Instance()->AssignRemoteLoginInfo(*pRaw);

			DebugLog("--------------------------------------------Login Success:%i", retcode);
		}
		else
		{

			DebugLog("--------------------------------------------Login Failed :%i", retcode);
		}

		

	}

}

void RemoteLoginProcessor::AsyncProcess(std::shared_ptr<BaseSocketContainer> spSockContainer, std::shared_ptr<DDRCommProto::CommonHeader> spHeader, std::shared_ptr<google::protobuf::Message> spMsg)
{


}

