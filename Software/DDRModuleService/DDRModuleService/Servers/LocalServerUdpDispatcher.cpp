#include "LocalServerUdpDispatcher.h"
#include "../Processors/LocalServerUdpProcessor.h"
#include "../../../Shared/proto/BaseCmd.pb.h"
#include "../Processors/LoginProcessor.h"
#include "../Processors/HeartBeatProcessor.h"


using namespace DDRCommProto;
LocalServerUdpDispatcher::LocalServerUdpDispatcher()
{

	bcLSAddr bcServerInfo;
	m_ProcessorMap[bcServerInfo.GetTypeName()] = std::make_shared<LocalServerUdpProcessor>(*this);
}


LocalServerUdpDispatcher::~LocalServerUdpDispatcher()
{
}
