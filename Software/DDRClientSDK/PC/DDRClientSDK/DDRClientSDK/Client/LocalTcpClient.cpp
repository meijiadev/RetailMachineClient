#include "LocalTcpClient.h"
#include "../../../Shared/src/Network/TcpSocketContainer.h"
#include "../../../Shared/src/Network/MessageSerializer.h"
#include "../../../Shared/src/Network/BaseMessageDispatcher.h"
#include "LocalClientDispatcher.h"
#include "LocalClientUdpDispatcher.h"


#include "../../../Shared/src/Utility/DDRMacro.h"
#include "GlobalManager.h"
using namespace DDRCommProto;

LocalTcpClient::LocalTcpClient()
{
}


LocalTcpClient::~LocalTcpClient()
{
	DebugLog("LocalTcpClient Destroy")
}

std::shared_ptr<TcpClientSessionBase> LocalTcpClient::BindSerializerDispatcher()
{
	BIND_IOCONTEXT_SERIALIZER_DISPATCHER(m_IOContext, TcpClientSessionBase, MessageSerializer, LocalClientDispatcher,BaseHeadRuleRouter)
		return spTcpClientSessionBase;
}
void LocalTcpClient::OnConnected(std::shared_ptr<TcpSocketContainer> spContainer)
{


	StartHeartBeat();
	DebugLog("OnConnectSuccess! LocalTcpClient");


}
void LocalTcpClient::OnDisconnect(std::shared_ptr<TcpSocketContainer> spContainer)
{
	StopHeartBeat();
	TcpClientBase::OnDisconnect(spContainer);
	
	GlobalManager::Instance()->StartUdp();
}

void LocalTcpClient::StartHeartBeat()
{
	/*m_HeartBeatTimerID = m_Timer.add(std::chrono::seconds(1) , [](timer_id id)
	{

	});*/

	m_HeartBeatTimerID = m_Timer.add(std::chrono::seconds(1), std::bind(&LocalTcpClient::SendHeartBeatOnce, shared_from_base(),std::placeholders::_1), std::chrono::seconds(1));


}
void LocalTcpClient::StopHeartBeat()
{
	m_Timer.remove(m_HeartBeatTimerID);
}
void LocalTcpClient::SendHeartBeatOnce(timer_id id)
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








LSBroadcastReceiveTcpClient::LSBroadcastReceiveTcpClient()
{
}


LSBroadcastReceiveTcpClient::~LSBroadcastReceiveTcpClient()
{
	DebugLog("LSBroadcastReceiveTcpClient Destroy")
}

std::shared_ptr<TcpClientSessionBase> LSBroadcastReceiveTcpClient::BindSerializerDispatcher()
{
	BIND_IOCONTEXT_SERIALIZER_DISPATCHER(m_IOContext, TcpClientSessionBase, MessageSerializer, LSBroadcastReceiveClientDispatcher, BaseHeadRuleRouter)
		return spTcpClientSessionBase;
}
void LSBroadcastReceiveTcpClient::OnConnected(std::shared_ptr<TcpSocketContainer> spContainer)
{

	DebugLog("OnConnectSuccess! LSBroadcastReceiveTcpClient");
	auto spreq = std::make_shared<reqRemoteServerList>();
	spreq->set_fromip(spContainer->GetSocket().remote_endpoint().address().to_string());
	if (IsConnected())
	{
		Send(spreq);
	}
	spreq.reset();


}