#ifndef IdleState_h__
#define IdleState_h__
#include <Logic/RobotEntity.h>

namespace DDRFramework
{
	class IdleState : public RobotStatusBaseState
	{
	public:
		IdleState(std::shared_ptr<RobotEntity> sp) : IdleState::RobotStatusBaseState(sp)
		{

		};
		virtual void updateWithDeltaTime(float delta) override;
	};
}

#endif // IdleState_h__
