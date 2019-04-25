#include "ClientSDKTcpClient.h"
#include "src/Network/TcpSocketContainer.h"
#include "src/Network/MessageSerializer.h"
#include "src/Network/BaseMessageDispatcher.h"
#include "src/ClientSDK/ClientSDKDispatcher.h"
#include "src/ClientSDK/ClientSDKUdpDispatcher.h"
#include "src/Interface/DDRClientInterface.h"


#include "src/Utility/DDRMacro.h"

using namespace DDRCommProto;

namespace DDRSDK
{
	ClientSDKTcpClient::ClientSDKTcpClient()
	{
	}


	ClientSDKTcpClient::~ClientSDKTcpClient()
	{
		DebugLog("ClientSDKTcpClient Destroy")
	}

	std::shared_ptr<TcpClientSessionBase> ClientSDKTcpClient::BindSerializerDispatcher()
	{
		BIND_IOCONTEXT_SERIALIZER_DISPATCHER(m_IOContext, TcpClientSessionBase, MessageSerializer, ClientSDKDispatcher, BaseHeadRuleRouter)
			return spTcpClientSessionBase;
	}
	void ClientSDKTcpClient::OnConnected(std::shared_ptr<TcpSocketContainer> spContainer)
	{


		StartHeartBeat();
		DebugLog("OnConnected! ClientSDKTcpClient");

		if (m_wpParentInterface.lock()->m_spStatusListener)
		{
			m_wpParentInterface.lock()->m_spStatusListener->OnConnected();
		}


	}
	void ClientSDKTcpClient::OnDisconnect(std::shared_ptr<TcpSocketContainer> spContainer)
	{
		StopHeartBeat();
		TcpClientBase::OnDisconnect(spContainer);

		DebugLog("OnDisconnect! ClientSDKTcpClient");

		if (m_wpParentInterface.lock()->m_spStatusListener)
		{
			m_wpParentInterface.lock()->m_spStatusListener->OnDisconnected();
		}
	}

	void ClientSDKTcpClient::OnConnectTimeout(std::shared_ptr<TcpSocketContainer> spContainer)
	{

		DebugLog("OnConnectTimeout! ClientSDKTcpClient");
		if (m_wpParentInterface.lock()->m_spStatusListener)
		{
			m_wpParentInterface.lock()->m_spStatusListener->OnConnectTimeout();
		}
	}

	void ClientSDKTcpClient::OnConnectFailed(std::shared_ptr<TcpSocketContainer> spContainer)
	{

		DebugLog("OnConnectFailed! ClientSDKTcpClient");
		if (m_wpParentInterface.lock()->m_spStatusListener)
		{
			m_wpParentInterface.lock()->m_spStatusListener->OnConnectFailed();
		}
	}

	void ClientSDKTcpClient::StartHeartBeat()
	{
		m_HeartBeatTimerID = m_Timer.add(std::chrono::seconds(1), std::bind(&ClientSDKTcpClient::SendHeartBeatOnce, shared_from_base(), std::placeholders::_1), std::chrono::seconds(1));

	}
	void ClientSDKTcpClient::StopHeartBeat()
	{
		m_Timer.remove(m_HeartBeatTimerID);
	}
	void ClientSDKTcpClient::SendHeartBeatOnce(timer_id id)
	{
		HeartBeat hb;
		auto sphb = std::make_shared<HeartBeat>();
		sphb->set_whatever("hb");

		if (IsConnected())
		{
			Send(sphb);
		}
		sphb.reset();
	}




}