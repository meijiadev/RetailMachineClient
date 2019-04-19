#include "LoginProcessor.h"
#include <memory>
#include "../../../Shared/proto/BaseCmd.pb.h"
#include "../../../Shared/src/Utility/DDRMacro.h"
#include "../Managers/GlobalManager.h"
using namespace DDRFramework;
using namespace DDRCommProto;

LoginProcessor::LoginProcessor(BaseMessageDispatcher& dispatcher):BaseProcessor::BaseProcessor(dispatcher)
{
}


LoginProcessor::~LoginProcessor()
{
}

void LoginProcessor::Process(std::shared_ptr<BaseSocketContainer> spSockContainer, std::shared_ptr<CommonHeader> spHeader, std::shared_ptr<google::protobuf::Message> spMsg)
{
	std::string fromip = spSockContainer->GetTcp()->GetSocket().remote_endpoint().address().to_string();
	DebugLog("Receive Login from:%s" , fromip.c_str());

	auto bodytype = spHeader->bodytype();

	reqLogin* pRaw = reinterpret_cast<reqLogin*>(spMsg.get());

	std::string name = pRaw->username();
	std::string pwd = pRaw->userpwd();
	eCltType type = pRaw->type();


	auto sprsp = std::make_shared<rspLogin>();

	bool closeSession = false;
	if (type == eCltType::eLocalPCClient || type == eCltType::eLocalAndroidClient)
	{
		bool b = DBManager::Instance()->VerifyUser(name, pwd);
		if (b)
		{

			if (GlobalManager::Instance()->GetTcpServer()->GetSessionByTypeName(type, name))//this user already login
			{

				sprsp->set_retcode(rspLogin_eLoginRetCode_server_limit_reached);

				closeSession = true;
			}
			else
			{

				sprsp->set_yourrole(type);
				sprsp->set_retcode(rspLogin_eLoginRetCode_success);

				int priority = DBManager::Instance()->GetUserPriority(name);
				sprsp->set_priority(priority);

			}
		}
		else
		{
			sprsp->set_retcode(rspLogin_eLoginRetCode_incorrect_password);		
	
			closeSession = true;
		}

	}
	else if (type == eCltType::eForwarderClient		||
			 type == eCltType::eLSMStreamRelay		|| 
			 type == eCltType::eLSMFaceRecognition  ||
			 type == eCltType::eLSMSlamNavigation   ||
			 type == eCltType::eLSMThermalImaging  )
	{
		if (GlobalManager::Instance()->GetTcpServer()->GetSessionByType(type) != nullptr)
		{
			sprsp->set_retcode(rspLogin_eLoginRetCode_server_limit_reached);
			closeSession = true;
		}
		else
		{
			sprsp->set_retcode(rspLogin_eLoginRetCode_success);

		}

	}


	spSockContainer->Send(sprsp);
	if (closeSession)
	{
		spSockContainer->GetTcp()->Stop();
	}
	else
	{ 

		auto spTcp = spSockContainer->GetTcp();
		if (spTcp)
		{
			auto spServerSessionTcp = dynamic_pointer_cast<LocalServerTcpSession>(spTcp);
			if (spServerSessionTcp)
			{
				spServerSessionTcp->AssignLoginInfo(*pRaw);
			}
		}
	}



	DebugLog("Login Name:%s   type:%i" ,name.c_str(), type);


}
void LoginProcessor::AsyncProcess(std::shared_ptr<BaseSocketContainer> spSockContainer,std::shared_ptr<DDRCommProto::CommonHeader> spHeader, std::shared_ptr<google::protobuf::Message> spMsg)
{
	//std::this_thread::sleep_for(std::chrono::seconds(10));
}