#include "LocalClientUdpDispatcher.h"
#include "LocalClientUdpProcessor.h"
#include "../../../Shared/src/Utility/DDRMacro.h"
#include "LoginProcessor.h"


using namespace DDRCommProto;
LocalClientUdpDispatcher::LocalClientUdpDispatcher()
{

	bcLSAddr bcClientInfo;
	m_ProcessorMap[bcClientInfo.GetTypeName()] = std::make_shared<LocalClientUdpProcessor>(*this);
}


LocalClientUdpDispatcher::~LocalClientUdpDispatcher()
{
	DebugLog("LocalClientUdpDispatcher Destroy");
}
