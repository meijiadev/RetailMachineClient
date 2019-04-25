#include "DDRClientInterface.h"
#include <src/Network/UdpSocketBase.h>
#include <src/Network/BaseMessageDispatcher.h>

#include "src/ClientSDK/ClientSDKTcpClient.h"
#include "src/ClientSDK/ClientSDKUdpClient.h"
#include "src/Processors/ClientSDKProcessor.h"
#include "src/Processors/ClientSDKUdpProcessor.h"

#include "include/DDRClientSDK.h"

#include <thirdparty/crossguid/include/crossguid/guid.hpp>
using namespace DDRFramework;

namespace DDRSDK
{

	DDRHANDLE CreateDDR(std::shared_ptr<DDRStatusListener> spListener)
	{
		auto spInterface = std::make_shared<DDRClientInterface>(spListener);


		auto g = xg::newGuid();
		std::string guid = g.str();
		DDRClientInterface::m_InterfacePtrMap[guid] = spInterface;


		spInterface->Init();

		return guid;
	}
	void ReleaseDDR(DDRHANDLE h)
	{
		DDRClientInterface::RemoveInterfacePtr(h);
	}


    bool RegisterListener(DDRHANDLE h, std::shared_ptr<google::protobuf::Message> spMsg, std::shared_ptr<DDRBaseListener> spListener)
	{
		auto spInterface = DDRClientInterface::FindInterfacePtr(h);
		if (spInterface)
		{
			return	spInterface->AddListener(spMsg,spListener);
		}
		return false;
	}
	bool UnregisterListener(DDRHANDLE h, std::shared_ptr<google::protobuf::Message> spMsg)
	{
		auto spInterface = DDRClientInterface::FindInterfacePtr(h);
		if (spInterface)
		{
			spInterface->RemoveListener(spMsg);

		}
		return false;
	}



	void StartReceivingBroadcast(DDRHANDLE h, std::shared_ptr<DDRBroadcastReceiver> spReceiver, int port)
	{
		auto spInterface = DDRClientInterface::FindInterfacePtr(h);
		if (spInterface)
		{
			spInterface->SetUdpReceiver(dynamic_pointer_cast<DDRBroadcastReceiver>(spReceiver));
			spInterface->StartUdp(port);
		}
	}
	void StopReceivingBroadcast(DDRHANDLE h)
	{
		auto spInterface = DDRClientInterface::FindInterfacePtr(h);
		if (spInterface)
		{
			spInterface->StopUdp();
		}
	}


	bool StartCommunication(DDRHANDLE h,std::string ip, std::string port)
	{
		auto spInterface = DDRClientInterface::FindInterfacePtr(h);
		if (spInterface)
		{
			spInterface->TcpConnect(ip,port);
		}

		return false;

	}
	bool StopCommunication(DDRHANDLE h)
	{
		auto spInterface = DDRClientInterface::FindInterfacePtr(h);
		if (spInterface)
		{
			spInterface->TcpDisconnect();
		}
		return false;
	}
	void Send(DDRHANDLE h, std::shared_ptr<google::protobuf::Message> spmsg)
	{
		auto spInterface = DDRClientInterface::FindInterfacePtr(h);
		if (spInterface)
		{
			spInterface->Send(spmsg);
		}
	}



	std::map<std::string, std::shared_ptr<DDRClientInterface>>  DDRClientInterface::m_InterfacePtrMap;

	DDRClientInterface::DDRClientInterface(std::shared_ptr<DDRStatusListener> spListener)
	{
		m_spStatusListener = spListener;
	}
	DDRClientInterface::~DDRClientInterface()
	{
		DebugLog("Destroy DDRClientInterface")
	}

	void DDRClientInterface::Init()
	{
		if (!m_spUdpClient)
		{
			auto spCLientSDKUdpClient = std::make_shared<ClientSDKUdpClient>();
			spCLientSDKUdpClient->SetParentInterface(shared_from_this());
			m_spUdpClient = spCLientSDKUdpClient;
			m_spUdpClient->BindOnDisconnect(std::bind(&DDRClientInterface::OnUdpDisconnect, this, std::placeholders::_1));
		
		}
		if (!m_spTcpClient)
		{
			auto spClientSDKTcpClient = std::make_shared<ClientSDKTcpClient>();
			spClientSDKTcpClient->SetParentInterface(shared_from_this());
			
			
			m_spTcpClient = spClientSDKTcpClient;

			m_spTcpClient->Start();
		}
	}
	bool DDRClientInterface::StartUdp(int port)
	{
		if (m_spUdpClient)
		{
			m_spUdpClient->Start();

			auto spDispatcher = std::make_shared<BaseUdpMessageDispatcher>();

			bcLSAddr msg;
			spDispatcher->RegisterExternalProcessor(msg, std::make_shared<ClientSDKUdpProcessor>(*(spDispatcher.get())));


			m_spUdpClient->GetSerializer()->BindDispatcher(spDispatcher);
			m_spUdpClient->StartReceive(port);
		}
		return true;

	}



	void DDRClientInterface::StopUdp()
	{
		if (m_spUdpClient && m_spUdpClient->IsWorking())
		{
			m_spUdpClient->StopReceive();
			m_spUdpClient->Stop();
			//m_spUdpClient.reset();donot reset here cause Stop is async ,it will release when OnDisconnect is called

		}
	}

	bool DDRClientInterface::AddListener(std::shared_ptr<google::protobuf::Message> spMsg, std::shared_ptr<DDRBaseListener> spListener)
	{
		if (m_ListenerMap.find(spMsg->GetTypeName()) != m_ListenerMap.end())
		{
			return false;
		}
		else
		{
			std::string stype = spMsg->GetTypeName();
			m_ListenerMap.insert(std::make_pair(stype, spListener));
		}
	}

	bool DDRClientInterface::RemoveListener(std::shared_ptr<google::protobuf::Message> spMsg)
	{
		if (m_ListenerMap.find(spMsg->GetTypeName()) == m_ListenerMap.end())
		{
			return false;
		}
		else
		{
			m_ListenerMap.erase(spMsg->GetTypeName());
		}
	}



	void DDRClientInterface::TcpConnect(std::string ip, std::string port)
	{
		if (m_spTcpClient)
		{
			m_spTcpClient->Connect(ip, port);
		}
	}



	void DDRClientInterface::TcpDisconnect()
	{
		if (m_spTcpClient)
		{
			m_spTcpClient->Disconnect();
		}
	}

	void DDRClientInterface::Send(std::shared_ptr<google::protobuf::Message> spMsg)
	{
		if (m_spTcpClient)
		{
			m_spTcpClient->Send(spMsg);
		}
	}

	void DDRClientInterface::OnUdpDisconnect(UdpSocketBase& container)
	{
		if (m_spUdpClient)
		{
			m_spUdpClient->Stop();
		}
	}

}