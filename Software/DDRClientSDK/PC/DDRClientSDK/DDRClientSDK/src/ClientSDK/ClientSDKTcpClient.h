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



	private:
		void SendHeartBeatOnce(timer_id id);

		DDRFramework::Timer m_Timer;
		DDRFramework::timer_id m_HeartBeatTimerID;
	};
}
#endif // ClientSDKTcpClient_h__
