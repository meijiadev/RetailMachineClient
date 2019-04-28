#include "src/Network/MessageSerializer.h"
#include "src/Network/TcpServerBase.h"
#include "src/Network/TcpClientBase.h"
#include "src/Utility/XmlLoader.h"
#include "src/Utility/LocalizationLoader.h"
#include "src/Utility/MiniDump.h"
#include "src/Utility/CommonFunc.h"
#include "src/Network/UdpSocketBase.h"
#include "Servers/LocalServerUdpDispatcher.h"
#include "Managers/GlobalManager.h"
#include "Managers/FileManager.h"
#include "Servers/LocalTcpServer.h"
#include "LSClient/LSClientManager.h"

#include "src/Utility/PythonDebugTools.h"
#include "Logic/RobotEntity.h"


#include <src/Utility/DDRMacro.h>

#include <thread>
#include <chrono>
using namespace DDRFramework;
using namespace DDRCommProto;
using namespace std;

#ifdef _WINDOWS
#include <Windows.h>
#include <cppfs/windows/LocalFileSystem.h>
#endif
#include <cppfs/FilePath.h>
#include "Managers/FileManager.h"
#include "Version.h"


char gQuit = 0;

#include "thirdparty/curl-asio/curl-asio.hpp"
#include "thirdparty/cpp-sqlite3/cppsqlite3.h"
#include <ctime>
#include <iostream>

#include <iostream>
#include <fstream>

#include "src/Utility/Logger.h"
#include <regex>


#include "opencv2/opencv.hpp"
#include "opencv2/imgproc.hpp"

using namespace std;


static curl_asio::data_action::type on_transfer_data_read(std::ofstream &out, const asio::const_buffer& buffer)
{
	out.write(asio::buffer_cast<const char*>(buffer), asio::buffer_size(buffer));
	return curl_asio::data_action::success;
}

static void on_transfer_done(curl_asio::transfer::ptr transfer, std::ofstream &out, const std::string &file, CURLcode result)
{
	if (result == CURLE_OK)
	{
		out.close();

		std::cout << "Transfer of " << transfer->info().effective_url() << " completed successfully (" << transfer->info().total_time() << " seconds)! Content saved to file " << file << std::endl;
		exit(0);
	}
	else
	{
		std::cerr << "Transfer of " << transfer->info().effective_url() << " failed with error " << result << std::endl;
		exit(1);
	}
}

int asiocurl(std::string filename,std::string usage,std::string url)
{
	asio::io_service io;
	curl_asio curl(io);
	curl_asio::transfer::ptr transfer = curl.create_transfer();
	if (transfer)
	{
		std::ofstream out(filename);
		transfer->opt.protocols = CURLPROTO_HTTP | CURLPROTO_HTTPS;
		transfer->opt.max_redirs = 5;
		transfer->opt.redir_protocols = CURLPROTO_HTTP | CURLPROTO_HTTPS;
		transfer->opt.follow_location = true;
		transfer->on_data_read = std::bind(on_transfer_data_read, std::ref(out), std::placeholders::_1);
		transfer->on_done = std::bind(on_transfer_done, transfer, std::ref(out), filename, std::placeholders::_1);
		if (transfer->start(url))
		{
			while (1)
				io.run();
		}
	}

	return 1;
}


class _ConsoleDebug : public DDRFramework::ConsoleDebug ,public CSingleton<_ConsoleDebug>
{
public:
	_ConsoleDebug()
	{
		AddCommand("ls sc", std::bind(&_ConsoleDebug::ListServerConnections, this));
		AddCommand("ls cc", std::bind(&_ConsoleDebug::ListClientConnections, this));
		AddCommand("py", std::bind(&_ConsoleDebug::RunPython, this));
		AddCommand("upf", std::bind(&_ConsoleDebug::UploadFile, this)); 	
		AddCommand("con remote", std::bind(&_ConsoleDebug::ConnectRemoteServer, this));
		AddCommand("dis remote", std::bind(&_ConsoleDebug::DisconnectRemoteServer, this));
		AddCommand("ver", std::bind(&_ConsoleDebug::PrintVersion, this));


		AddCommand("send2all", std::bind(&_ConsoleDebug::Send2All, this));
		AddCommand("startbroadcast", std::bind(&_ConsoleDebug::BroadcastCheck, this));

		AddCommand("log", std::bind(&_ConsoleDebug::TestLog, this));
	}
	void PrintVersion()
	{
		printf_s("\nBuild Time:%s", g_BuildTime.c_str());
		printf_s("\nBuild Version:%s", g_Version.c_str());
		printf_s("\nBuild Mode:%s", g_DMode.c_str());
		
	}

	void Send2All()
	{

		char* data = new char[256];
		for (int i = 0 ;i<256;i++)
		{
			data[i] = i;
		}

		std::shared_ptr<asio::streambuf> buf = std::make_shared<asio::streambuf>();

		std::ostream oshold(buf.get());
		oshold.write((const char*)data, 256);
		oshold.flush();

		for (auto pair : GlobalManager::Instance()->GetTcpServer()->GetTcpSocketContainerMap())
		{

			auto spSession = pair.second;
			auto spServerSessionTcp = dynamic_pointer_cast<LocalServerTcpSession>(spSession);


			spServerSessionTcp->Send(buf);
		}

	}

	void ListServerConnections()
	{
		printf_s("\nServer Connections");
		for (auto pair : GlobalManager::Instance()->GetTcpServer()->GetTcpSocketContainerMap())
		{

			auto spSession = pair.second;
			auto spServerSessionTcp = dynamic_pointer_cast<LocalServerTcpSession>(spSession);

			std::string ip = spServerSessionTcp->GetSocket().remote_endpoint().address().to_string();
			printf_s("\n%s  type:%i", ip.c_str(), spServerSessionTcp->GetLoginInfo().type());
		}
	}
	void ListClientConnections()
	{
		printf_s("\nClient Connections");
	
		auto spClient = LSClientManager::Instance()->GetTcpClient();
		if (spClient)
		{

			auto spSession = spClient->GetConnectedSession();
			auto spServerSessionTcp = dynamic_pointer_cast<LocalServerTcpSession>(spSession);
			if (spServerSessionTcp)
			{

				std::string ip = spServerSessionTcp->GetSocket().remote_endpoint().address().to_string();
				printf_s("\n%s  type:%i", ip.c_str(), spServerSessionTcp->GetLoginInfo().type());
			}
		}
		else
		{
			printf_s("\n no lsclient");
		}


	}
	void RunPython()
	{
		auto vec = split(m_CurrentCmd, ':');

		std::string funcname = "msg";
		std::string ip = "";
		if (vec.size() == 2)
		{
			funcname = vec[1];
		}
		else if (vec.size() == 3)
		{
			funcname = vec[1];
			ip = vec[2];
		}

		PythonDebugTools pdt(GlobalManager::Instance()->GetGlobalConfig().GetValue("PythonPath"));
		auto spmsg = pdt.Run(funcname);
		if (spmsg)
		{
			if (!ip.empty())
			{
				for (auto pair : GlobalManager::Instance()->GetTcpServer()->GetTcpSocketContainerMap())
				{
					auto pSocket = pair.first;

					if (pSocket)
					{
						std::string socketip = pSocket->remote_endpoint().address().to_string();
						if (socketip == ip)
						{

							auto spSession = pair.second;
							spSession->Send(spmsg);
						}
					}
				}

			}
			else
			{
				for (auto pair :GlobalManager::Instance()->GetTcpServer()->GetTcpSocketContainerMap())
				{
					auto spSession = pair.second;
					spSession->Send(spmsg);

				}

			}

		}
		else
		{

			DebugLog("RunPython Error");
		}
	}

	void UploadFile()
	{
		//bool b = false;



		//b = FileManager::Instance()->FileExist("D:/DevelopProj/Dadao/DDRFramework/www/1.txt");
		//b = FileManager::Instance()->FileExist("D:/DevelopProj/Dadao/DDRFramework/www/2.txt");
		//b = FileManager::Instance()->FileExist("http://192.168.1.1:8080/1.txt");
		//b = FileManager::Instance()->FileExist("http://192.168.1.1:8080/2.txt");

		TcpClientBase* p = new TcpClientBase();
		delete p;
		p->Connect("", "");

		rspRemoteFileAddress* pp = new rspRemoteFileAddress();
		delete pp;
		pp->set_error("");
	}

	void ConnectRemoteServer()
	{
		LSClientManager::Instance()->ConnectRemoteServer();
	}
	void DisconnectRemoteServer()
	{
		LSClientManager::Instance()->GetTcpClient()->Stop();
	}

	void BroadcastCheck()
	{
		LSClientManager::Instance()->StartCheckBroadcast();
	}

	void TestLog()
	{
		//设置LOG为INFO级，大于INFO级的会被打印
		DDRFramework::Log::getInstance()->setLevel(DDRFramework::Log::Level::INFO);

		//设置输出目标（屏幕,文件或..)
		DDRFramework::Log::getInstance()->setTarget(Log::Target::STDOUT);// (Log::Target::STDOUT | Log::Target::LOG_FILE);

		LevelLog(DDRFramework::Log::Level::DEBUG, "DEBUG level %f", 10.12f);
		LevelLog(DDRFramework::Log::Level::INFO, "Info level %i", 100);
		LevelLog(DDRFramework::Log::Level::ERR, "Error level %s", "abc");
		LevelLog(DDRFramework::Log::Level::WARNING, "WARNING level %f", 10.12f);
	}
};


int main()
{
	MsgRouterManager::Instance()->SetCltType(eLocalServer);
	DisableMouseSelectConsole();

	InitMinDump();
	GlobalManager::Instance()->StartTcpServer();
	GlobalManager::Instance()->StartUdpServer();


	//if (GlobalManager::Instance()->CheckRemoteNetwork())
	//{
	//	LSClientManager::Instance()->Init();
	//	LSClientManager::Instance()->StartCheckBroadcast();

	//}



	
	DDRFramework::RobotLogic::Instance()->GetEntity()->Init();


	_ConsoleDebug::Instance()->ConsoleDebugLoop();

	return 0;
}