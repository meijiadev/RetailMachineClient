#ifndef RobotLogic_h__
#define RobotLogic_h__
#include "src/Utility/Singleton.h"
#include "src/FSM/FSM.h"
#include <memory>

namespace DDRFramework
{
	class RobotEntity;

	class RobotLogic : public CSingleton<RobotLogic>
	{
	public:
		RobotLogic()
		{
			m_spEntity = std::make_shared<RobotEntity>();
		}
		~RobotLogic()
		{

		}

		std::shared_ptr<RobotEntity> GetEntity()
		{
			return m_spEntity;
		}

	private:
		std::shared_ptr<RobotEntity> m_spEntity;
	};

	class RobotEntity : public std::enable_shared_from_this<RobotEntity>
	{
	public:
		RobotEntity();
		~RobotEntity();

		virtual bool Init();
		virtual void Update();

	protected:
		virtual void ThreadEntry();


		std::shared_ptr<StateMachine<RobotEntity>> m_spStateMachine;

		std::mutex m_MutexUpdate;

		int m_FrameRate;
		int m_FrameTimeLenMs;
	};


	class RobotStatusBaseState : public State<RobotEntity>
	{
	public:
		RobotStatusBaseState(std::shared_ptr<RobotEntity> sp) : State<RobotEntity>::State(sp)
		{

		};
		~RobotStatusBaseState()
		{

		}

		virtual bool isValidNextState(std::shared_ptr<State<RobotEntity>> state) { return true; };

	private:

	};
}
#endif // RobotLogic_h__
