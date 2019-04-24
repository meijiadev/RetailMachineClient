#include "DDRClientInterface.h"
#include <src/Network/UdpSocketBase.h>
#include <src/Network/BaseMessageDispatcher.h>

#include "src/ClientSDK/ClientSDKTcpClient.h"
#include "src/Processors/ClientSDKProcessor.h"
#include "src/Processors/ClientSDKUdpProcessor.h"

#include "include/DDRClientSDK.h"

namespace DDRSDK
{

	DDRHANDLE CreateDDR()
	{
		DDRClientInterface* pInterface = new DDRClientInterface();
		pInterface->Init();

		return pInterface;
	}
	void ReleaseDDR(DDRHANDLE h)
	{
		DDRClientInterface* pInterface = (DDRClientInterface*)h;
		delete pInterface;
	}


    bool RegisterListener(DDRHANDLE h, std::shared_ptr<google::protobuf::Message> spMsg, std::shared_ptr<DDRBaseListener> spListener)
	{
		DDRClientInterface* pInterface = (DDRClientInterface*)h;
		if (pInterface)
		{
			return	pInterface->AddListener(spMsg,spListener);
		}
		return false;
	}
	bool UnregisterListener(DDRHANDLE h, std::shared_ptr<google::protobuf::Message> spMsg)
	{
		DDRClientInterface* pInterface = (DDRClientInterface*)h;
		if (pInterface)
		{
			pInterface->RemoveListener(spMsg);

		}
		return false;
	}



	void StartReceivingBroadcast(DDRHANDLE h, std::shared_ptr<DDRBroadcastReceiver> spReceiver)
	{
		DDRClientInterface* pInterface = (DDRClientInterface*)h;
		if (pInterface)
		{
			pInterface->SetUdpReceiver(dynamic_pointer_cast<DDRBroadcastReceiver>(spReceiver));
			pInterface->StartUdp();
		}
	}
	void StopReceivingBroadcast(DDRHANDLE h)
	{
		DDRClientInterface* pInterface = (DDRClientInterface*)h;
		if (pInterface)
		{
			pInterface->StopUdp();
		}
	}


	bool StartCommunication(DDRHANDLE h,std::string ip, std::string port)
	{
		return false;

	}
	bool StopCommunication(DDRHANDLE h)
	{
		return false;
	}
	void Send(DDRHANDLE h, std::shared_ptr<google::protobuf::Message> spmsg)
	{

	}



	std::shared_ptr<DDRSDK::DDRBroadcastReceiver> DDRClientInterface::m_spUdpReceiver;
	std::map<std::string, std::shared_ptr<DDRSDK::DDRBaseListener>> DDRClientInterface::m_ListenerMap;

	DDRClientInterface::DDRClientInterface()
	{
	}
	DDRClientInterface::~DDRClientInterface()
	{

	}

	void DDRClientInterface::Init()
	{
		if (!m_spUdpClient)
		{
			m_spUdpClient = std::make_shared<UdpSocketBase>();
			m_spUdpClient->BindOnDisconnect(std::bind(&GlobalManagerClientBase::OnUdpDisconnect, this, std::placeholders::_1));
		
		}
		if (!m_spTcpClient)
		{
			m_spTcpClient = std::make_shared<ClientSDKTcpClient>();
			m_spTcpClient->Start();
		}
	}
	bool DDRClientInterface::StartUdp()
	{
		if (m_spUdpClient)
		{
			m_spUdpClient->Start();

			auto spDispatcher = std::make_shared<BaseUdpMessageDispatcher>();

			//both AddProcessor or RegisterExternalProcessor is ok
			//spDispatcher->AddProcessor<bcLSAddr, ClientSDKUdpProcessor>();
			bcLSAddr msg;
			spDispatcher->RegisterExternalProcessor(msg, std::make_shared<ClientSDKUdpProcessor>(*(spDispatcher.get())));


			m_spUdpClient->GetSerializer()->BindDispatcher(spDispatcher);
			m_spUdpClient->StartReceive(m_GlobalConfig.GetValue<int>("UdpPort"));
		}
		return true;

	}



	bool DDRClientInterface::AddListener(std::shared_ptr<google::protobuf::Message> spMsg, std::shared_ptr<DDRBaseListener> spListener)
	{
		if (m_ListenerMap.find(spMsg->GetTypeName()) != m_ListenerMap.end())
		{
			return false;
		}
		else
		{
			m_ListenerMap[spMsg->GetTypeName()] = spListener;
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



	void DDRClientInterface::TcpConnect()
	{
		if (!m_ServerIP.empty() && !m_ServerPort.empty())
		{
			GlobalManagerClientBase::TcpConnect(m_ServerIP, m_ServerPort);
		}
	}

	void DDRClientInterface::TcpConnect(std::string ip, std::string port)
	{
		GlobalManagerClientBase::TcpConnect(ip, port);
	}



}