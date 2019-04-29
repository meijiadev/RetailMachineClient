#ifndef DDRClientInterface_h__
#define DDRClientInterface_h__


#include <src/Utility/DDRMacro.h>
#include "include/DDRClientSDK.h"
#include <src/Network/TcpClientBase.h>
#include <src/Network/UdpSocketBase.h>

namespace DDRSDK
{

	class  DDRClientInterface : public std::enable_shared_from_this<DDRClientInterface>
	{
	public:	 
		DDRClientInterface(std::shared_ptr<DDRStatusListener> spListener);
		~DDRClientInterface();

		void Init();
		
		bool StartUdp(int port);
		void StopUdp();


		void TcpConnect(std::string ip, std::string port);
		void TcpDisconnect();

		void SetUserInfo(eCltType astype, std::string username, std::string password);
		void Login();

		void Send(std::shared_ptr<google::protobuf::Message> spMsg);



		void SetUdpReceiver(std::shared_ptr<DDRBroadcastReceiver> sp)
		{
			m_spUdpReceiver = sp;
		}
		void CallUdpReceiver(std::string name,std::string ip,std::string port)
		{
			if (m_spUdpReceiver)
			{
				m_spUdpReceiver->OnBroadcastArrival(name, ip, port);
			}
		}

		void Dispatch(std::shared_ptr<DDRCommProto::CommonHeader> spHeader, std::shared_ptr<google::protobuf::Message> spMsg)
		{
			if (m_ListenerMap.find(spMsg->GetTypeName()) != m_ListenerMap.end())
			{
				auto spListener = m_ListenerMap[spMsg->GetTypeName()];
				spListener->OnMsgArrival(spHeader, spMsg);
			}

		}


		bool AddListener(std::shared_ptr<google::protobuf::Message> spMsg, std::shared_ptr<DDRSDK::DDRBaseListener> spListener);
		bool RemoveListener(std::shared_ptr<google::protobuf::Message> spMsg);


		void OnUdpDisconnect(DDRFramework::UdpSocketBase& container);

		std::shared_ptr<DDRStatusListener> m_spStatusListener;

		static std::shared_ptr<DDRClientInterface> FindInterfacePtr(DDRHANDLE h)
		{
			if (m_InterfacePtrMap.find(h) != m_InterfacePtrMap.end())
			{
				return m_InterfacePtrMap[h];
			}
			return nullptr;
		}
		static void RemoveInterfacePtr(DDRHANDLE h)
		{
			if (m_InterfacePtrMap.find(h) != m_InterfacePtrMap.end())
			{
				m_InterfacePtrMap.erase(h);
			}
		}

		static std::map<std::string, std::shared_ptr<DDRClientInterface>> m_InterfacePtrMap;
	private:
		
		std::shared_ptr<DDRFramework::TcpClientBase> m_spTcpClient;
		std::shared_ptr<DDRFramework::UdpSocketBase> m_spUdpClient;

		std::string m_ServerIP;
		std::string m_ServerPort;
		std::string m_Username;
		std::string m_Password;
		eCltType m_AsType;


		std::shared_ptr<DDRSDK::DDRBroadcastReceiver> m_spUdpReceiver;
		std::map<std::string, std::shared_ptr<DDRSDK::DDRBaseListener>> m_ListenerMap;


	};
}



#endif // DDRClientInterface_h__
