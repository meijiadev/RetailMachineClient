#include "src/ClientSDK/ClientSDKUdpDispatcher.h"
#include "src/Processors/ClientSDKUdpProcessor.h"
#include "src/Utility/DDRMacro.h"



using namespace DDRCommProto;
ClientSDKUdpDispatcher::ClientSDKUdpDispatcher()
{

	bcLSAddr bcClientInfo;
	m_ProcessorMap[bcClientInfo.GetTypeName()] = std::make_shared<ClientSDKUdpProcessor>(*this);
}


ClientSDKUdpDispatcher::~ClientSDKUdpDispatcher()
{
	DebugLog("ClientSDKUdpDispatcher Destroy");
}
