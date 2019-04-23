//#include "../../Shared/src/Network/MessageSerializer.h"
//#include "../../Shared/src/Network/TcpServerBase.h"
//#include "../../Shared/src/Network/TcpClientBase.h"
//#include "../../Shared/src/Network/HttpClient.h"
//#include <google/protobuf/descriptor.h>
//#include <google/protobuf/message.h>
//#include "../../Shared/src/Utility/DDRMacro.h"
//
//#include "../../Shared/src/Utility/MiniDump.h"
//#include "Client/LocalClientUdpDispatcher.h"
//#include <thread>
//#include <chrono>
//#include "Client/LocalTcpClient.h"
//#include "Client/GlobalManager.h"
//#include "Client/AudioTcpClient.h"
//#include <memory>
//#include <string>
//#include <sstream>
//#include <vector>
//#include <iterator>
//
//#include "../../../Shared/src/Utility/CommonFunc.h"
//#ifdef _WINDOWS
//#include <Windows.h>
//#include "cppfs/windows/LocalFileSystem.h"
//#endif
//
//
//#include "opencv2/opencv.hpp"
//#include "opencv2/imgproc.hpp"
//
//#include "../../Shared/src/Utility/PythonDebugTools.h"
//
//using namespace DDRFramework;
//using namespace DDRCommProto;
//
//using namespace std;
//char gQuit = 0;
//
//
//void DoOnce(std::shared_ptr<TcpClientBase> spClient)
//{
//
//	std::this_thread::sleep_for(chrono::seconds(2));
//	auto spSession = spClient->Connect("127.0.0.1", "88");
//
//	std::thread t = std::thread(std::bind([](std::shared_ptr<TcpClientBase> spClient) {
//
//
//		for (int j = 0; j < 2;)
//		{
//			try
//			{
//
//				for (int i = 0; i < 10000;)
//				{
//
//					//std::this_thread::sleep_for(chrono::seconds(1));
//
//					auto spreq = std::make_shared<reqLogin>();
//					spreq->set_username("michaelmichaelmichaelmichaelmichaelmichaelmichaelmichaelmichaelmichael");
//
//					const char* psrc = "1234567890";
//
//					if (spClient && spClient->IsConnected())
//					{
//						//spClient->Send(spreq);
//						spClient->Send(psrc, 10);
//						i++;
//					}
//					spreq.reset();
//				}
//			}
//			catch (asio::system_error& e)
//			{
//				break;
//			}
//
//
//			std::this_thread::sleep_for(chrono::seconds(10));
//		}
//
//	}, spClient));
//	t.join();
//
//	getchar();
//
//	std::this_thread::sleep_for(chrono::seconds(2));
//	spClient->Disconnect(spSession);
//	spClient.reset();
//}
//
//std::shared_ptr<AudioTcpClient> TcpAudioClient()
//{
//	//std::shared_ptr<AudioTcpClient> spAudioClient = std::make_shared<AudioTcpClient>();
//	auto spAudioClient = std::shared_ptr<AudioTcpClient>(new AudioTcpClient);
//	//spAudioClient->Connect("192.168.1.183", "88");
//	spAudioClient->Start();
//	spAudioClient->Connect("192.168.1.183", "89");
//
//	return spAudioClient;
//}
//
//
//class _ConsoleDebug : public DDRFramework::ConsoleDebug, public CSingleton<_ConsoleDebug>
//{
//public:
//	_ConsoleDebug()
//	{
//		AddCommand("ls cc", std::bind(&_ConsoleDebug::ListClientConnection, this));
//		AddCommand("castart", std::bind(&_ConsoleDebug::StartAudio, this));
//		AddCommand("talkstart", std::bind(&_ConsoleDebug::StartTalk, this));
//		AddCommand("talkstop", std::bind(&_ConsoleDebug::StopTalk, this));
//		AddCommand("castop", std::bind(&_ConsoleDebug::StopAudio, this));
//
//		AddCommand("reqf", std::bind(&_ConsoleDebug::RequestFile, this));
//		AddCommand("reqrf", std::bind(&_ConsoleDebug::RequestRemoteFile, this));
//		AddCommand("upf", std::bind(&_ConsoleDebug::UploadFile, this));
//
//
//		AddCommand("connect", std::bind(&_ConsoleDebug::Connect, this));
//		AddCommand("disconnect", std::bind(&_ConsoleDebug::Disconnect, this));
//		AddCommand("login", std::bind(&_ConsoleDebug::Login, this));
//		AddCommand("httpget", std::bind(&_ConsoleDebug::HttpGet, this));
//		AddCommand("cmd", std::bind(&_ConsoleDebug::SendCmd, this));
//
//		AddCommand("cmdmove", std::bind(&_ConsoleDebug::SendCmdMove, this));
//		AddCommand("slist", std::bind(&_ConsoleDebug::GetServerList, this));
//
//		AddCommand("rlogin", std::bind(&_ConsoleDebug::RemoteLogin, this));
//		AddCommand("sls", std::bind(&_ConsoleDebug::SelectLS, this));
//
//		AddCommand("py", std::bind(&_ConsoleDebug::RunPython, this));
//
//		AddCommand("playwav", std::bind(&_ConsoleDebug::PlayAudio, this));
//		AddCommand("stophb", std::bind(&_ConsoleDebug::StopHeartBeat, this));
//
//
//
//		AddCommand("startsend", std::bind(&_ConsoleDebug::StartSend, this));
//		AddCommand("stopsend", std::bind(&_ConsoleDebug::StopSend, this));
//
//
//		AddCommand("connects", std::bind(&_ConsoleDebug::Connects, this));
//
//		AddCommand("ac", std::bind(&_ConsoleDebug::AudioClient, this));
//		AddCommand("test", std::bind(&_ConsoleDebug::Test, this));
//	
//	}
//	void ListClientConnection()
//	{
//		printf_s("\nClient Connection");
//		auto spSession = GlobalManager::Instance()->GetTcpClient()->GetConnectedSession();
//		if (spSession)
//		{
//			std::string ip = spSession->GetSocket().remote_endpoint().address().to_string();
//			printf_s("\n%s", ip.c_str());
//		}
//		else
//		{
//			printf_s("\nClient Not Connection");
//		}
//	}
//	void AudioClient()
//	{
//		auto spAudioClient = std::make_shared<AudioTcpClient>();
//		//spAudioClient->Connect("192.168.1.183", "88");
//		spAudioClient->Start();
//		spAudioClient->Connect("192.168.1.183", "88");
//
//		
//	}
//	void Test()
//	{
//		std::thread t(std::bind(&_ConsoleDebug::TestThread, this));
//		t.detach();
//	}
//	void TestThread()
//	{
//		while (true)
//		{
//			std::this_thread::sleep_for(std::chrono::seconds(1));
//			DebugLog("test");
//		}
//	}
//	void StartAudio()
//	{
//		DebugLog("Call Audio");
//		auto spreq = std::make_shared<reqStreamAddr>();
//		//spreq->set_networktype(ChannelNetworkType::Remote);
//		spreq->set_networktype(ChannelNetworkType::Local);
//		
//		GlobalManager::Instance()->GetTcpClient()->Send(spreq);
//		spreq.reset();
//
//		
//	}
//	void StartTalk()
//	{
//		auto sp = std::make_shared<reqAudioTalk>();
//		sp->set_optype(reqAudioTalk_eOpMode_eStart);
//
//		GlobalManager::Instance()->GetTcpClient()->Send(sp);
//		sp.reset();
//
//	}
//	void StopTalk()
//	{
//
//		auto sp = std::make_shared<reqAudioTalk>();
//		sp->set_optype(reqAudioTalk_eOpMode_eStop);
//
//		GlobalManager::Instance()->GetTcpClient()->Send(sp);
//		sp.reset();
//	}
//	void StopAudio()
//	{
//
//
//		GlobalManager::Instance()->StopAudioClient();
//	}
//	void PlayAudio()
//	{
//		DebugLog("Call Audio");
//		auto spreq = std::make_shared<reqCmdAudio>();
//		spreq->set_audiop(reqCmdAudio_eAudioOperational_eStart);
//		spreq->set_level(0);
//		//spreq->set_type(reqCmdAudio_eAudioMode_eTTS);
//		//spreq->set_audiostr("123454好人汉字big bell");
//		spreq->set_type(reqCmdAudio_eAudioMode_eFile);
//		spreq->set_audiostr("alarm.wav");
//
//		GlobalManager::Instance()->GetTcpClient()->Send(spreq);
//		spreq.reset();
//	}
//	void RequestFile()
//	{
//		string fmt;
//
//		fmt = "Path*\\*.txt";
//		DoRequestOnce(fmt);
//
//		fmt = "OneRoute_111\\bkPic.png";
//		DoRequestOnce(fmt);
//
//		fmt = "OneRoute_111\\affine_mat.txt";
//		DoRequestOnce(fmt);
//	}
//	void DoRequestOnce(string fmt)
//	{
//
//		DebugLog("Request File Once");
//		auto spreq = std::make_shared<reqFileAddress>();
//		spreq->set_tarservicetype(eCltType::eLSMStreamRelay);
//		spreq->set_filetype(eFileTypes::FileHttpAddress);
//		//spreq->add_filenames("x64/*DDR*Server.e*");
//		//spreq->add_filenames("x64/*api*l*.*");
//		//spreq->add_filenames("*中*/*件*.*");
//		//spreq->add_filenames("x64/Debug/*.db");
//		//spreq->add_filenames("fs/*/*.*");
//		spreq->add_filenames(fmt.c_str());
//		GlobalManager::Instance()->GetTcpClient()->Send(spreq);
//		spreq.reset();
//	}
//
//	void RequestRemoteFile()
//	{
//		DebugLog("Request File");
//		auto spreq = std::make_shared<reqRemoteFileAddress>();
//		spreq->set_tarservicetype(eCltType::eLSMStreamRelay);
//		spreq->set_filetype(eFileTypes::FileHttpAddress);
//		//spreq->add_filenames("x64/*DDR*Server.e*");
//		//spreq->add_filenames("x64/*api*l*.*");
//		//spreq->add_filenames("*中*/*件*.*");
//		//spreq->add_filenames("x64/Debug/*.db");
//		spreq->add_filenames("fs/*/*.*");
//
//
//		GlobalManager::Instance()->GetTcpClient()->Send(spreq);
//		spreq.reset();
//	}
//
//	void UploadFile()
//	{
//		auto spHttpSession = std::make_shared<HttpSession>();
//
//		std::string url = "http://localhost:8000/";
//		std::string basedir = "D:/DevelopProj/Dadao/DDRFramework/www/";
//		std::string filename = "fs/root/t.dll";
//		spHttpSession->DoPost(url, basedir , filename);
//
//
//		//std::this_thread::sleep_for(chrono::seconds(10));
//	}
//
//	void GetServerList()
//	{
//		GlobalManager::Instance()->ConnectBroadcastServer();
//	}
//	void SelectLS()
//	{
//		rspRemoteLogin& info = GlobalManager::Instance()->GetRemoteLoginInfo();
//
//		if (info.lslist().size() > 0)
//		{
//			auto ls = info.lslist(0);
//			
//			auto spreq = std::make_shared<reqSelectLS>();
//			
//			spreq->set_udid(ls.udid());
//
//			GlobalManager::Instance()->GetTcpClient()->Send(spreq);
//		}
//	}
//	void RemoteLogin()
//	{
//		auto spreq = std::make_shared<reqRemoteLogin>();
//
//		spreq->set_type(eCltType::eRemotePCClient);
//		spreq->set_username("admin");
//		spreq->set_userpwd("admin");
//
//		GlobalManager::Instance()->GetTcpClient()->Send(spreq);
//		DebugLog("Send Romote Login");
//
//	}
//
//	std::shared_ptr<TcpClientSessionBase> spConnectedSession;
//	void Connect()
//	{
//		auto vec = split(m_CurrentCmd, ':');
//		if (vec.size() == 3)
//		{
//			GlobalManager::Instance()->StopUdp();
//			//GlobalManager::Instance()->TcpConnect(vec[1],vec[2]);
//
//			spConnectedSession = GlobalManager::Instance()->GetTcpClient()->Connect(vec[1], vec[2]);
//		}
//		else if(vec.size() == 1)
//		{
//
//			GlobalManager::Instance()->StopUdp();
//			GlobalManager::Instance()->TcpConnect();
//
//		}
//
//
//	}
//	void Disconnect()
//	{
//		if (spConnectedSession)
//		{
//			 GlobalManager::Instance()->GetTcpClient()->Disconnect(spConnectedSession);
//			 spConnectedSession = nullptr;
//		}
//	}
//
//	void Login()
//	{
//
//		auto spreq = std::make_shared<reqLogin>();
//		spreq->set_type(eLocalPCClient);
//		spreq->set_username("admin_pc");
//		spreq->set_userpwd("admin_pc");
//
//		GlobalManager::Instance()->GetTcpClient()->Send(spreq);
//
//		spreq.reset();
//	}
//
//	void Connects()
//	{
//		std::thread t(std::bind(&_ConsoleDebug::SendingThread, this, m_CurrentCmd));
//		t.detach();
//
//
//	}
//	void SendingThread(string cmd)
//	{
//		while (true)
//		{
//
//			auto vec = split(cmd, ':');
//			if (vec.size() == 3)
//			{
//				GlobalManager::Instance()->TcpConnect(vec[1], vec[2]);
//			}
//			else if (vec.size() == 1)
//			{
//				GlobalManager::Instance()->TcpConnect();
//
//			}
//			DebugLog("Connect")
//
//			std::this_thread::sleep_for(chrono::milliseconds(5000));
//
//
//
//			auto spClient = GlobalManager::Instance()->GetTcpClient();
//			spClient->Disconnect(spClient->GetConnectedSession());
//
//			DebugLog("Disconnect")
//			std::this_thread::sleep_for(chrono::milliseconds(500));
//		}
//
//
//	}
//
//	
//	void HttpGet()
//	{
//
//		auto spHttpSession = std::make_shared<HttpSession>();
//		//auto url = std::string("http://192.168.1.183:8080////bin/x64/26-12-2018-17-00-4.log");
//		//auto outfile = std::string("D:/DevelopProj/Dadao/DDRFramework/DDRLocalServer/x64/Debug/26-12-2018-17-00-4.log");
//		auto url = std::string("http://192.168.1.183:8080/wav/9.wav");
//		//auto url = std::string("http://192.168.1.183:8080/wav/wav.wav");
//		//auto url = std::string("http://192.168.1.183:8080/wav/a.txt");
//		auto outfile = std::string("D:/DevelopProj/Dadao/DDRFramework/DDRLocalServer/x64/Debug/x.wav");
//		spHttpSession->DoGet(url ,outfile);
//	}
//
//	void OpenCVTest()
//	{
//		cv::Mat mat;
//		int cols = mat.cols;
//	}
//
//	void SendCmd()
//	{
//		//auto spreq = std::make_shared<reqCmdMove>();
//		//spreq->set_line_speed(100);
//		//spreq->set_angulau_speed(200);
//
//
//		//auto spheader = std::make_shared<CommonHeader>();
//		//GlobalManager::Instance()->GetTcpClient()->Send(spheader,spreq);
//		//spreq.reset();
//
//		while (true)
//		{
//			StartAudio();
//
//			std::this_thread::sleep_for(chrono::milliseconds(500));
//
//			StartTalk();
//
//			std::this_thread::sleep_for(chrono::milliseconds(5000));
//			StopTalk();
//
//			std::this_thread::sleep_for(chrono::milliseconds(500));
//			StopAudio();
//
//			std::this_thread::sleep_for(chrono::milliseconds(500));
//
//		}
//	}
//
//	void SendCmdMove()
//	{
//		while (true)
//		{
//
//			std::this_thread::sleep_for(chrono::milliseconds(10));
//			auto spreq = std::make_shared<reqCmdMove>();
//			spreq->set_angulau_speed(10);
//
//			GlobalManager::Instance()->GetTcpClient()->Send(spreq);
//			spreq.reset();
//
//			char data[1024] = { 1 };
//			GlobalManager::Instance()->GetTcpClient()->Send(&data, 1024);
//
//		}
//
//
//		
//
//	}
//	void RunPython()
//	{
//		auto vec = split(m_CurrentCmd, ':');
//
//		std::string funcname = "msg";
//		if (vec.size() == 2)
//		{
//			funcname = vec[1];
//		}
//		
//		PythonDebugTools pdt(GlobalManager::Instance()->GetGlobalConfig().GetValue("PythonPath"));
//		auto spmsg = pdt.Run(funcname);
//		if (spmsg)
//		{
//			GlobalManager::Instance()->GetTcpClient()->Send(spmsg);
//
//		}
//		else
//		{
//
//			DebugLog("RunPython Error");
//		}
//	}
//
//	void StopHeartBeat()
//	{
//		auto sp = GlobalManager::Instance()->GetTcpClient();
//		auto spLocalTcpClient = dynamic_pointer_cast<LocalTcpClient>(sp);
//		if (spLocalTcpClient)
//		{
//			spLocalTcpClient->StopHeartBeat();
//		}
//	}
//
//
//
//
//	DDRFramework::Timer m_Timer;
//	DDRFramework::timer_id m_HeartBeatTimerID;
//	int m_SendTimes;
//	int m_TotalTimes;
//	void StartSend()
//	{
//		auto vec = split(m_CurrentCmd, ':');
//		if (vec.size() == 2)
//		{
//			m_TotalTimes = atoi(vec[1].c_str());
//			m_SendTimes = 0;
//			m_HeartBeatTimerID = m_Timer.add(std::chrono::milliseconds(50), std::bind(&_ConsoleDebug::SendOnce, this, std::placeholders::_1), std::chrono::milliseconds(1));
//		}
//		else
//		{
//			m_HeartBeatTimerID = m_Timer.add(std::chrono::milliseconds(50), std::bind(&_ConsoleDebug::SendOnce, this, std::placeholders::_1), std::chrono::milliseconds(1));
//		}
//	}
//	void SendOnce(timer_id id)
//	{
//		m_SendTimes++;
//		if (m_SendTimes > m_TotalTimes)
//		{
//			StopSend();
//			return;
//		}
//		printf_s("\nSend Once");
//		auto spSession = GlobalManager::Instance()->GetTcpClient()->GetConnectedSession();
//		if (spSession)
//		{
//
//			auto spreq = std::make_shared<reqCmdMove>();
//			//spreq->set_type(eCltType::eLSMStreamRelay);
//			spreq->set_line_speed(100);
//			spreq->set_angulau_speed(200);
//			//spreq->set_whatever("0");
//
//
//			auto spheader = std::make_shared<CommonHeader>();
//			spheader->set_fromclttype(eLocalPCClient);
//			spheader->set_toclttype(eLSMStreamRelay);
//			spheader->add_flowdirection(CommonHeader_eFlowDir_Forward);
//			spSession->Send(spheader,spreq);
//			spreq.reset();
//		}
//		else
//		{
//			printf_s("\nClient Not Connection");
//		}
//	}
//	void StopSend()
//	{
//
//		m_Timer.remove(m_HeartBeatTimerID);
//	}
//};
//
//int main()
//{
//
//	DisableMouseSelectConsole();
//
//	InitMinDump();
//
//	GlobalManager::Instance()->Init();
//
//	
//	_ConsoleDebug::Instance()->ConsoleDebugLoop();
//	return 0;
//}

#include "src/Utility/DDRMacro.h"
int main()
{
	DDRCommProto::reqCmdMove req;
	printf("%f", req.line_speed());
	return 0;
}