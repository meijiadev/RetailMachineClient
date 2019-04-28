#ifndef ClientSDKTcpClient_h__
#define ClientSDKTcpClient_h__

#ifdef QT_PRECOMPILED_HEADER
    #include "stable.h"
#else
#endif

#include "../../../Shared/src/Network/TcpClientBase.h"
#include "../../../Shared/src/Utility/Singleton.h"
#include "../../../Shared/src/Utility/Timer.hpp"

using namespace DDRFramework;
namespace DDRSDK
{
	class DDRClientInterface;
	class ClientSDKTcpClient;

	class ClientSDKSession : public TcpClientSessionBase
	{
	public:
		ClientSDKSession(asio::io_context& context);
		~ClientSDKSession();

		void SetParentClient(std::shared_ptr<ClientSDKTcpClient> sp)
		{
			m_wpParentClient = sp;
		}
		std::shared_ptr<ClientSDKTcpClient> GetParentClient()
		{
			return m_wpParentClient.lock();
		}

	private:
		std::weak_ptr<ClientSDKTcpClient> m_wpParentClient;
	};

	class ClientSDKTcpClient : public TcpClientBase
	{
	public:
		ClientSDKTcpClient();
		~ClientSDKTcpClient();

		virtual void OnConnected(std::shared_ptr<TcpSocketContainer> spContainer) override;
		virtual void OnDisconnect(std::shared_ptr<TcpSocketContainer> spContainer) override;
		virtual void OnConnectTimeout(std::shared_ptr<TcpSocketContainer> spContainer) override;
		virtual void OnConnectFailed(std::shared_ptr<TcpSocketContainer> spContainer) override;

		virtual std::shared_ptr<TcpClientSessionBase> BindSerializerDispatcher();

		SHARED_FROM_BASE(ClientSDKTcpClient)

			void StartHeartBeat();
		void StopHeartBeat();


		void SetParentInterface(std::shared_ptr<DDRClientInterface> sp)
		{
			m_wpParentInterface = sp;
		}

		std::shared_ptr<DDRClientInterface> GetParentInterface()
		{
			return m_wpParentInterface.lock();
		}
	private:
		void SendHeartBeatOnce(timer_id id);

		DDRFramework::Timer m_Timer;
		DDRFramework::timer_id m_HeartBeatTimerID;

		std::weak_ptr<DDRClientInterface> m_wpParentInterface;
	};
}
#endif // ClientSDKTcpClient_h__
