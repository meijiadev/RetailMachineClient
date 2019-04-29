

#include <memory>
#include "IdleState.h"
#include "src/Utility/DDRMacro.h"
#include "src/Utility/Logger.h"

using namespace DDRFramework;
using namespace DDRCommProto;

IdleState::IdleState(std::shared_ptr<RobotEntity> sp) :RobotStatusBaseState(sp)
{
}


IdleState::~IdleState()
{
}

void IdleState::didEnterWithPreviousState(std::shared_ptr<State<RobotEntity>> previousState)
{
	LevelLog(DDRFramework::Log::INFO, "Idle State Enter");
}

void IdleState::updateWithDeltaTime(float delta)
{

	//LevelLog(DDRFramework::Log::INFO, "Idle State Update");
}

void IdleState::willExitWithNextState(std::shared_ptr<State<RobotEntity>> nextState)
{
	LevelLog(DDRFramework::Log::INFO, "Idle State Exit");
}
