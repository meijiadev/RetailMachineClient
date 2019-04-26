#include "../Behaviors/BaseClientBehavior.h"
#include "src/Network/TcpSocketContainer.h"
#include "src/Utility/DDRMacro.h"
#include <chrono>
#include <thread>

#include "src/Utility/Logger.h"
BaseClientBehavior::BaseClientBehavior()
{
}

BaseClientBehavior::~BaseClientBehavior()
{
	m_Timer.remove(m_HeartBeatTimerID);
	DebugLog("BaseClientBehavior Destroy")
}
void BaseClientBehavior::OnStart(std::shared_ptr<DDRFramework::TcpSocketContainer> spContainer)
{
	DebugLog("BaseClientBehavior OnStart----------------------------------------------------------------")
	auto interval = std::chrono::seconds(15);
	m_HeartBeatTimerID = m_Timer.add(interval, std::bind(&BaseClientBehavior::HeartBeatTimeout, this, spContainer));
}
void BaseClientBehavior::Update(std::shared_ptr<DDRFramework::TcpSocketContainer> spContainer)
{

}
void BaseClientBehavior::OnStop(std::shared_ptr<DDRFramework::TcpSocketContainer> spContainer)
{
	DebugLog("BaseClientBehavior OnStop----------------------------------------------------------------")
	m_Timer.remove(m_HeartBeatTimerID);
}

void BaseClientBehavior::HeartBeatTimeout(std::shared_ptr<DDRFramework::TcpSocketContainer> spContainer)
{
	spContainer->Stop();
	m_Timer.remove(m_HeartBeatTimerID);
	DebugLog("HeartBeat Timeout----------------------------------------------------------------")


}
void BaseClientBehavior::ResetHeartBeat(std::shared_ptr<DDRFramework::TcpSocketContainer> spContainer)
{
	m_Timer.reset(m_HeartBeatTimerID);
	//DebugLog("ResetHeartBeat----------------------------------------------")
	
	
}