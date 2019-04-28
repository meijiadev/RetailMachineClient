

#include <memory>
#include "ChargingState.h"
#include "src/Utility/DDRMacro.h"
#include "src/Utility/Logger.h"

using namespace DDRFramework;
using namespace DDRCommProto;

ChargingState::ChargingState(std::shared_ptr<RobotEntity> sp) :RobotStatusBaseState(sp)
{
}


ChargingState::~ChargingState()
{
}
void ChargingState::updateWithDeltaTime(float delta)
{

}
