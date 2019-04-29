

#include <memory>
#include "proto/BaseCmd.pb.h"
#include "ClientSDKProcessor.h"
#include "src/Utility/DDRMacro.h"
#include "src/Utility/Logger.h"
#include "src/Interface/DDRClientInterface.h"
#include "src/ClientSDK/ClientSDKTcpClient.h"
using namespace DDRFramework;
using namespace DDRCommProto;




namespace DDRSDK
{


	void LoginListener::OnMsgArrival(std::shared_ptr<DDRCommProto::CommonHeader> spHeader, std::shared_ptr<google::protobuf::Message> spMsg)
	{
		auto spRsp = std::dynamic_pointer_cast<DDRCommProto::rspLogin>(spMsg);
		if (spRsp)
		{
			printf("\Login Listener:%i", spRsp->yourrole());

			if (spRsp->retcode() == rspLogin_eLoginRetCode_success)
			{
				if (m_spInterface)
				{
					m_spInterface->m_spStatusListener->OnLoginSuccess();
				}
			}
			else
			{
				if (m_spInterface)
				{
					m_spInterface->m_spStatusListener->OnLoginFailed("");
				}

			}

		}
	}



	ClientSDKProcessor::ClientSDKProcessor(BaseMessageDispatcher& dispatcher) :BaseProcessor(dispatcher)
	{
	}


	ClientSDKProcessor::~ClientSDKProcessor()
	{
	}

	void ClientSDKProcessor::Process(std::shared_ptr<BaseSocketContainer> spSockContainer, std::shared_ptr<CommonHeader> spHeader, std::shared_ptr<google::protobuf::Message> spMsg)
	{
		auto spTcp = spSockContainer->GetTcp();
		auto spClientSDKSession = dynamic_pointer_cast<DDRSDK::ClientSDKSession>(spTcp);
		if (spClientSDKSession)
		{

			spClientSDKSession->GetParentClient()->GetParentInterface()->Dispatch(spHeader, spMsg);
		}

	}

	void ClientSDKProcessor::AsyncProcess(std::shared_ptr<BaseSocketContainer> spSockContainer, std::shared_ptr<DDRCommProto::CommonHeader> spHeader, std::shared_ptr<google::protobuf::Message> spMsg)
	{


	}

}