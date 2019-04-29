#include "RobotEntity.h"
#include "Logic/IdleState/IdleState.h"
#include "Logic/AutoState/AutoState.h"
#include "Logic/ChargingState/ChargingState.h"
#include "Logic/BuildingMapStae/BuildingMapState.h"
#include <ctime>
namespace DDRFramework
{
	RobotEntity::RobotEntity()
	{
		m_FrameRate = 30;
		m_FrameTimeLenMs = 1000 / m_FrameRate;
	}

	RobotEntity::~RobotEntity()
	{

	}



	bool StateEntity::Init()
	{
		m_spStateMachine = std::make_shared<StateMachine<RobotEntity>>();
		return true;
	}

	void StateEntity::Update()
	{
		std::lock_guard<std::mutex> lock(m_MutexUpdate);

		if (m_spStateMachine && this)
		{
			auto pCurState = m_spStateMachine->getState();

			if (m_spStateMachine)
			{
				m_spStateMachine->updateWithDeltaTime();
				pCurState = m_spStateMachine->getState();//must put after updateWithDeltaTime,cause state may change in that function
			}
		}
	}




	bool RobotEntity::Init()
	{
		StateEntity::Init();

		ADD_STATE(m_spStateMachine, IdleState)
			ADD_STATE(m_spStateMachine, BuildingMapState)
			ADD_STATE(m_spStateMachine, ChargingState)
			ADD_STATE(m_spStateMachine, AutoState)


			m_spStateMachine->enterState<IdleState>(); // Enter the init state by default

		std::thread t(std::bind(&RobotEntity::ThreadEntry, this));
		t.detach();

		m_ptrDeviceManager = DDRDevice::CreateDDRDeviceModule();

		return true;
	}

	bool RobotEntity::InitDeviceManager()
	{
		if (!m_ptrDeviceManager.get())
		{
			return false;
		}

		std::cout << "Device Version:" << m_ptrDeviceManager->GetDeviceVersion().c_str() << " Date:" << m_ptrDeviceManager->GetDeviceDate().c_str() << std::endl;

		std::string strLidarName("LidarYoung");
		m_ptrDeviceManager->AddDevice(DDRDevice::en_DeviceLidar, strLidarName);

		auto lidar = m_ptrDeviceManager->GetPtr(DDRDevice::en_DeviceLidar, strLidarName);

		DDRDevice::LidarBase* pData = (DDRDevice::LidarBase*)(lidar);
		DDRDevice::LidarInfo info;
		info.m_strIp = "192.168.0.82";
		info.m_strName = strLidarName;

		if (pData->Init(info))
		{
			std::cout << "Lidar init success \n";
		}
		return false;
	}



#define  STARTCLOCK(name) clock_t startTime##name, endTime##name;\
startTime##name = clock();
#define STOPCLOCK(name) 	endTime##name = clock();\
int usedMs##name = (int)(endTime##name - startTime##name);// / CLOCKS_PER_SEC;
#define SHOWCLOCK(name)	DebugLog("%s Use Time ms : %i", #name , usedMs#name);


	void RobotEntity::ThreadEntry()
	{
		int ii = 0;
		clock_t startTime, endTime;

		//STARTCLOCK(total)

		while (true)
		{
			STARTCLOCK(update)

			Update();

			//endTime = clock();//¼ÆÊ±½áÊø
			//int usedMs = (int)(endTime - startTime);// / CLOCKS_PER_SEC;

			STOPCLOCK(update)
		
			int left = m_FrameTimeLenMs - usedMsupdate;
			if (left > 0)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(left));
			}

			//STOPCLOCK(total)
			//LevelLog(DDRFramework::Log::Level::DEBUG, "RobotLogic TotalTime:%i Frame:%i", usedMstotal / CLOCKS_PER_SEC ,ii++);
		}


	}

	RobotStatusBaseState::RobotStatusBaseState(std::shared_ptr<RobotEntity> sp) : State<RobotEntity>::State(sp)
	{
		Init();
	}

	RobotStatusBaseState::~RobotStatusBaseState()
	{

	}

	void RobotStatusBaseState::updateWithDeltaTime(float delta)
	{
		Update();
	}

}