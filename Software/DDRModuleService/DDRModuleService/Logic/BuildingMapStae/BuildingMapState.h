
#ifndef BuildingMapState_h__
#define BuildingMapState_h__

#include <Logic/RobotEntity.h>


using namespace DDRFramework;
class BuildingMapState : public RobotStatusBaseState
{
public:
	BuildingMapState(std::shared_ptr<RobotEntity> sp);
	~BuildingMapState();
	virtual void updateWithDeltaTime(float delta) override;
};

#endif // BuildingMapState_h__

