

#include <memory>
#include "AutoState.h"
#include "src/Utility/DDRMacro.h"
#include "src/Utility/Logger.h"

using namespace DDRFramework;
using namespace DDRCommProto;

AutoState::AutoState(std::shared_ptr<RobotEntity> sp) :RobotStatusBaseState(sp)
{
}


AutoState::~AutoState()
{
}



bool AutoState::Init()
{
	StateEntity::Init();


	ADD_STATE(m_spStateMachine, IdleState)

	return true;
}

void AutoState::didEnterWithPreviousState(std::shared_ptr<State<RobotEntity>> previousState)
{

	LevelLog(DDRFramework::Log::INFO, "Auto State Enter");
}

void AutoState::updateWithDeltaTime(float delta)
{

	//LevelLog(DDRFramework::Log::INFO, "Auto State Update");
}

void AutoState::willExitWithNextState(std::shared_ptr<State<RobotEntity>> nextState)
{

	LevelLog(DDRFramework::Log::INFO, "Auto State Exit");
}
