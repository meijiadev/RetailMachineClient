#ifndef LocalTcpClient_h__
#define LocalTcpClient_h__

#ifdef QT_PRECOMPILED_HEADER
    #include "stable.h"
#else
#endif

#include "../../../Shared/src/Network/TcpClientBase.h"
#include "../../../Shared/src/Utility/Singleton.h"
#include "../../../Shared/src/Utility/Timer.hpp"

using namespace DDRFramework;
class LocalTcpClient : public TcpClientBase 
{
public:
	LocalTcpClient();
	~LocalTcpClient();

	void OnConnected(std::shared_ptr<TcpSocketContainer> spContainer) override;
	void OnDisconnect(std::shared_ptr<TcpSocketContainer> spContainer) override;

	virtual std::shared_ptr<TcpClientSessionBase> BindSerializerDispatcher();

	SHARED_FROM_BASE(LocalTcpClient)

	void StartHeartBeat();
	void StopHeartBeat();



private:
	void SendHeartBeatOnce(timer_id id);




	DDRFramework::Timer m_Timer;
	DDRFramework::timer_id m_HeartBeatTimerID;
};

class LSBroadcastReceiveTcpClient : public TcpClientBase
{
public:
	LSBroadcastReceiveTcpClient();
	~LSBroadcastReceiveTcpClient();

	void OnConnected(std::shared_ptr<TcpSocketContainer> spContainer) override;

	virtual std::shared_ptr<TcpClientSessionBase> BindSerializerDispatcher();

	SHARED_FROM_BASE(LSBroadcastReceiveTcpClient)
};
#endif // LocalTcpClient_h__
