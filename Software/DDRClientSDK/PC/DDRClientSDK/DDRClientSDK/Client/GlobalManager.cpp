#include "GlobalManager.h"
#include "LocalClientUdpDispatcher.h"
#include "LocalClientUdpProcessor.h"
#include "LocalTcpClient.h"

GlobalManager::GlobalManager():m_Config("Config/ClientSDK/ClientConfig.xml")
{
}
GlobalManager::~GlobalManager()
{

}

void GlobalManager::Init()
{
	if (!m_spUdpClient)
	{
		m_spUdpClient = std::make_shared<UdpSocketBase>();
		m_spUdpClient->BindOnDisconnect(std::bind(&GlobalManagerClientBase::OnUdpDisconnect, this, std::placeholders::_1));
		StartUdp();
	}
	if (!m_spTcpClient)
	{
		m_spTcpClient = std::make_shared<LocalTcpClient>();
		m_spTcpClient->Start();
	}
}
bool GlobalManager::StartUdp()
{
	if (m_spUdpClient)
	{
		m_spUdpClient->Start();

		auto spDispatcher = std::make_shared<BaseUdpMessageDispatcher>();

		//both AddProcessor or RegisterExternalProcessor is ok
		//spDispatcher->AddProcessor<bcLSAddr, LocalClientUdpProcessor>();
		bcLSAddr msg;
		spDispatcher->RegisterExternalProcessor(msg, std::make_shared<LocalClientUdpProcessor>(*(spDispatcher.get())));


		m_spUdpClient->GetSerializer()->BindDispatcher(spDispatcher);
		m_spUdpClient->StartReceive(m_GlobalConfig.GetValue<int>("UdpPort"));
	}
	return true;

}






void GlobalManager::SetServerAddr(std::string ip, std::string port)
{
	m_ServerIP = ip;
	m_ServerPort = port;
}


void GlobalManager::TcpConnect()
{
	if (!m_ServerIP.empty() && !m_ServerPort.empty())
	{
		GlobalManagerClientBase::TcpConnect(m_ServerIP, m_ServerPort);
	}
}

void GlobalManager::TcpConnect(std::string ip, std::string port)
{
	GlobalManagerClientBase::TcpConnect(ip, port);
}

void GlobalManager::ConnectBroadcastServer()
{
	m_spLSBroadcastReceiveTcpClient = std::make_shared<LSBroadcastReceiveTcpClient>();
	m_spLSBroadcastReceiveTcpClient->Start();

	std::string ip = m_GlobalConfig.GetValue("BroadcastServerIP");
	std::string port = m_GlobalConfig.GetValue("BroadcastServerPort");
	m_spLSBroadcastReceiveTcpClient->Connect(ip, port);
}

void GlobalManager::CloseBroadcastServer()
{
	if (m_spLSBroadcastReceiveTcpClient)
	{
		m_spLSBroadcastReceiveTcpClient->Stop();
	}
}

