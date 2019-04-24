#ifndef DDRClientInterface_h__
#define DDRClientInterface_h__


#include <src/Utility/DDRMacro.h>
#include <src/Utility/GlobalManagerBase.h>
#include "include/DDRClientSDK.h"

namespace DDRSDK
{

	class  DDRClientInterface : public DDRFramework::GlobalManagerClientBase
	{
	public:	 
		DDRClientInterface();
		~DDRClientInterface();

		virtual void Init() override;
		virtual bool StartUdp() override;


		virtual void TcpConnect(std::string ip, std::string port);
		void TcpConnect();



		void SetUdpReceiver(std::shared_ptr<DDRBroadcastReceiver> sp)
		{
			m_spUdpReceiver = sp;
		}
		static void CallUdpReceiver(std::string name,std::string ip,std::string port)
		{
			if (m_spUdpReceiver)
			{
				m_spUdpReceiver->OnBroadcastArrival(name, ip, port);
			}
		}

		static void Dispatch(std::shared_ptr<DDRCommProto::CommonHeader> spHeader, std::shared_ptr<google::protobuf::Message> spMsg)
		{
			if (m_ListenerMap.find(spMsg->GetTypeName()) != m_ListenerMap.end())
			{
				auto spListener = m_ListenerMap[spMsg->GetTypeName()];
				spListener->OnMsgArrival(spHeader, spMsg);
			}

		}


		bool AddListener(std::shared_ptr<google::protobuf::Message> spMsg, std::shared_ptr<DDRSDK::DDRBaseListener> spListener);
		bool RemoveListener(std::shared_ptr<google::protobuf::Message> spMsg);



	private:


		std::string m_ServerIP;
		std::string m_ServerPort;


		static std::shared_ptr<DDRSDK::DDRBroadcastReceiver> m_spUdpReceiver;
		static std::map<std::string, std::shared_ptr<DDRSDK::DDRBaseListener>> m_ListenerMap;
	};
}



#endif // DDRClientInterface_h__
