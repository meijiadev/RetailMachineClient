#ifndef RobotLogic_h__
#define RobotLogic_h__
#include "src/Utility/Singleton.h"
#include "src/FSM/FSM.h"

#include "DDRDeviceInterface.h"
#include "DDRDeviceFactory.h"

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


	class StateEntity
	{
	public:
		virtual bool Init();;
		virtual void Deinit() {};
		virtual void Update();

		template <class T>
		void SwitchState()
		{
			auto spState = m_spStateMachine->findState<T>();
			if (spState && m_spStateMachine->canEnterState<T>())
			{
				m_spStateMachine->enterState(typeid(T).name());
			}
		};

	protected:

		std::shared_ptr<StateMachine<RobotEntity>> m_spStateMachine;
		std::mutex m_MutexUpdate;
	};


	class RobotEntity : public std::enable_shared_from_this<RobotEntity> , public StateEntity
	{
	public:
		RobotEntity();
		~RobotEntity();

		virtual bool Init() override;
	protected:
		bool InitDeviceManager();
		void ThreadEntry();


		int m_FrameRate;
		int m_FrameTimeLenMs;

		std::shared_ptr<DDRDevice::DDRDeviceInterface> m_ptrDeviceManager;

	};


	class RobotStatusBaseState : public State<RobotEntity> , public StateEntity
	{
	public:
		RobotStatusBaseState(std::shared_ptr<RobotEntity> sp);;
		~RobotStatusBaseState();


		virtual void didEnterWithPreviousState(std::shared_ptr<State<RobotEntity>> previousState) override {} ;
		virtual void updateWithDeltaTime(float delta) override;
		virtual bool isValidNextState(std::shared_ptr<State<RobotEntity>> state) override { return true; };
		virtual void willExitWithNextState(std::shared_ptr<State<RobotEntity>> nextState) override {};


	};
}
#endif // RobotLogic_h__
