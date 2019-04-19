#include "LSClientManager.h"



LSClientManager::LSClientManager()
{
}
LSClientManager::~LSClientManager()
{

}

//#define DebugRemoteServer
void LSClientManager::Init()
{
	if (!m_spTcpClient)
	{
		m_spTcpClient = std::make_shared<LSTcpClient>();
		m_spTcpClient->Start();
	}


#ifdef DebugRemoteServer

	std::string ip = "192.168.1.183";
	std::string port = "8900";

#else

	std::string ip = m_GlobalConfig.GetValue("BroadcastServerIP");
	std::string port = m_GlobalConfig.GetValue("BroadcastServerPort");
#endif

	if (!m_spLSBroadcastReceiveTcpClient)
	{
		m_spLSBroadcastReceiveTcpClient = std::make_shared<LSBroadcastReceiveTcpClient>(ip,port);
		m_spLSBroadcastReceiveTcpClient->Start();

	}



}


void LSClientManager::StartCheckBroadcast()
{
	if (m_spLSBroadcastReceiveTcpClient)
	{
		m_spLSBroadcastReceiveTcpClient->StartCheck();

	}
}

void LSClientManager::CloseBroadcastServer(std::vector<DDRCommProto::rspRemoteServerList_RemoteServer> servers)
{
	m_Servers = servers;
	if (m_spLSBroadcastReceiveTcpClient)
	{
		m_spLSBroadcastReceiveTcpClient->Close();

	}
}



void LSClientManager::ConnectRemoteServer()
{
	if (m_Servers.size() > 0)
	{
		auto server = m_Servers[0];
		TcpConnect(server.ip(), std::to_string(server.port()));

	}
}

