
#ifndef IdleState_h__
#define IdleState_h__

#include <Logic/RobotEntity.h>


using namespace DDRFramework;
class IdleState : public RobotStatusBaseState
{
public:
	IdleState(std::shared_ptr<RobotEntity> sp);
	~IdleState();

	virtual void didEnterWithPreviousState(std::shared_ptr<State<RobotEntity>> previousState) override; ;
	virtual void updateWithDeltaTime(float delta) override;
	virtual bool isValidNextState(std::shared_ptr<State<RobotEntity>> state) override { return true; };
	virtual void willExitWithNextState(std::shared_ptr<State<RobotEntity>> nextState) override;;
};

#endif // IdleState_h__
