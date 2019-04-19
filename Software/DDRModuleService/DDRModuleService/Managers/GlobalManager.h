#ifndef GlobalManager_h__
#define GlobalManager_h__

#include "../../../Shared/src/Network/UdpSocketBase.h"
#include "../../../Shared/src/Utility/Singleton.h"
#include "../../../Shared/src/Utility/XmlLoader.h"
#include "../../../Shared/src/Utility/LocalizationLoader.h"
#include "../../../Shared/src/Utility/GlobalManagerBase.h"
#include "../../../Shared/thirdparty/cpp-sqlite3/cppsqlite3.h"
#include "../Servers/LocalTcpServer.h"

using namespace DDRFramework;
class DBManager;
class GlobalManager : public CSingleton<GlobalManager>,public GlobalManagerBase
{

public:
	GlobalManager();
	~GlobalManager();

	void StartTcpServer();
	void StopTcpServer();


	void StartUdpServer();
	void StopUdpServer();
	void OnUdpDisconnect(UdpSocketBase& container);


	std::shared_ptr<LocalTcpServer> GetTcpServer()
	{
		return m_spTcpServer;
	}
	std::shared_ptr<UdpSocketBase> GetUdpServer()
	{
		return m_spUdpServer;
	}

	bool CheckRemoteNetwork();


	XmlLoader& GetLocalServerConfig()
	{
		return m_LocalServerConfig;
	}
private:
	std::shared_ptr<LocalTcpServer> m_spTcpServer;
	std::shared_ptr<UdpSocketBase> m_spUdpServer;

	XmlLoader m_LocalServerConfig;
};


class DBManager : public CSingleton<DBManager>
{
public:	 
	DBManager();
	~DBManager();

	bool Open(std::string& filename);
	bool Remove(std::string& filename);

	bool VerifyUser(std::string username, std::string pwd); 
	int GetUserPriority(std::string username);

private:
	bool CreateUserTable();
	bool CodeSample();

	std::string m_FileName;
	CppSQLite3DB m_DB;
};

#endif // GlobalManager_h__
