#include "RobotEntity.h"
#include "Logic/IdleState.h"
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

	bool RobotEntity::Init()
	{
		m_spStateMachine = std::make_shared<StateMachine<RobotEntity>>();



		ADD_STATE(m_spStateMachine, IdleState)

		std::thread t(std::bind(&RobotEntity::ThreadEntry, this));
		t.detach();
		
		return true;
	}

	void RobotEntity::Update()
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


#define  STARTCLOCK(name) clock_t startTime##name, endTime##name;\
startTime##name = clock();


#define STOPCLOCK(name) 	endTime##name = clock();\
int usedMs##name = (int)(endTime##name - startTime##name);// / CLOCKS_PER_SEC;

#define SHOWCLOCK(name)	DebugLog("%s Use Time ms : %i", #name , usedMs#name);


	void RobotEntity::ThreadEntry()
	{
		int ii = 0;
		clock_t startTime, endTime;

		STARTCLOCK(total)

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

			STOPCLOCK(total)

			DebugLog("RobotLogic TotalTime:%i Frame:%i", usedMstotal / CLOCKS_PER_SEC ,ii++);
		}


	}

}