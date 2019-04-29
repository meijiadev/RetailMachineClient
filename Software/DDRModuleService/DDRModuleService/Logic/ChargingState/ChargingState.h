
#ifndef ChargingState_h__
#define ChargingState_h__

#include <Logic/RobotEntity.h>


using namespace DDRFramework;
class ChargingState : public RobotStatusBaseState
{
public:
	ChargingState(std::shared_ptr<RobotEntity> sp);
	~ChargingState();
	virtual void updateWithDeltaTime(float delta) override;
};

#endif // ChargingState_h__
