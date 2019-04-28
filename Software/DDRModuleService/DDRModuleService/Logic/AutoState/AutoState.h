
#ifndef AutoState_h__
#define AutoState_h__

#include <Logic/RobotEntity.h>


using namespace DDRFramework;
class AutoState : public RobotStatusBaseState
{
public:
	AutoState(std::shared_ptr<RobotEntity> sp);
	~AutoState();

	virtual bool Init() override;


	virtual void didEnterWithPreviousState(std::shared_ptr<State<RobotEntity>> previousState) override;;
	virtual void updateWithDeltaTime(float delta) override;
	virtual bool isValidNextState(std::shared_ptr<State<RobotEntity>> state) override { return true; };
	virtual void willExitWithNextState(std::shared_ptr<State<RobotEntity>> nextState) override;
};

#endif // AutoState_h__
