
#include <memory>
#include "BuildingMapState.h"
#include "src/Utility/DDRMacro.h"
#include "src/Utility/Logger.h"

using namespace DDRFramework;
using namespace DDRCommProto;

BuildingMapState::BuildingMapState(std::shared_ptr<RobotEntity> sp) :RobotStatusBaseState(sp)
{
}


BuildingMapState::~BuildingMapState()
{
}
void BuildingMapState::updateWithDeltaTime(float delta)
{

}
