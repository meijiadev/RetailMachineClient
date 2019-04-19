#ifndef LSClientManager_h__
#define LSClientManager_h__


#include "../../../Shared/src/Network/UdpSocketBase.h"
#include "../../../Shared/src/Network/TcpClientBase.h"
#include "../../../Shared/src/Utility/Singleton.h"
#include "LSTcpClient.h"
#include "../../../Shared/src/Utility/LocalizationLoader.h"
#include "../../../Shared/src/Utility/GlobalManagerBase.h"

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
