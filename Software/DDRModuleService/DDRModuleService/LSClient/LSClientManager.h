#ifndef LSClientManager_h__
#define LSClientManager_h__


#include "src/Network/UdpSocketBase.h"
#include "src/Network/TcpClientBase.h"
#include "src/Utility/Singleton.h"
#include "LSTcpClient.h"
#include "src/Utility/LocalizationLoader.h"
#include "src/Utility/GlobalManagerBase.h"

using namespace DDRFramework;
class LSClientManager : public DDRFramework::CSingleton<LSClientManager> , public GlobalManagerClientBase
{
public:
	LSClientManager();
	~LSClientManager();

public:

	virtual void Init() override;
	void StartCheckBroadcast();
	void CloseBroadcastServer(std::vector<DDRCommProto::rspRemoteServerList_RemoteServer> servers);
	void ConnectRemoteServer();

	std::vector<DDRCommProto::rspRemoteServerList_RemoteServer> m_Servers;

	std::shared_ptr <LSBroadcastReceiveTcpClient> m_spLSBroadcastReceiveTcpClient;


	std::string m_ServerIP;
	std::string m_ServerPort;

};

#endif // LSClientManager_h__
