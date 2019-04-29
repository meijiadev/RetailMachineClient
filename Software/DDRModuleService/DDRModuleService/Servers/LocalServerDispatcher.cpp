#include "LocalServerDispatcher.h"

#include "../Processors/LoginProcessor.h"
#include "../Processors/StreamRelay/StreamServiceInfoProcessor.h"
#include "../Processors/HeartBeatProcessor.h"
#include "../Processors/Client/StreamAddrProcessor.h"
#include "../Processors/Client/FileAddressProcessor.h"
#include "../Processors/StreamRelay/StreamRelayAlarmProcessor.h"
#include "../Processors/FileStatusProcessor.h"
#include "LocalTcpServer.h"

#include "../Processors/SDK/RobotVersionInfoProcessor.h"
#include "../Processors/SDK/SetParameterProcessor.h"

#include "src/Utility/DDRMacro.h"

using namespace DDRCommProto;
using namespace DDRModuleCmd;
using namespace DDRFramework;

LocalServerDispatcher::LocalServerDispatcher()
{
	HeartBeat hBeat;
	m_ProcessorMap[hBeat.GetTypeName()] = std::make_shared<HeartBeatProcessor>(*this);

		//Global
	RegisterProcessor(req, Login)
		RegisterProcessor(req, RobotVersionInfo)
		RegisterProcessor(req, SetParameter)


		//Client
		RegisterProcessor(req, StreamAddr)
		RegisterProcessor(req, FileAddress)

		//Stream Relay
		RegisterProcessor(req, StreamServiceInfo)
		RegisterProcessor(req, StreamRelayAlarm)

			RegisterProcessor(ans, FileStatus)


}\


LocalServerDispatcher::~LocalServerDispatcher()
{
}

void LocalServerDispatcher::Dispatch(std::shared_ptr< DDRFramework::BaseSocketContainer> spParentSocketContainer, std::shared_ptr<DDRCommProto::CommonHeader> spHeader, std::shared_ptr<google::protobuf::Message> spMsg)
{
	auto spServerTcpSession = dynamic_pointer_cast<LocalServerTcpSession>(spParentSocketContainer->GetTcp());

	if (spServerTcpSession->HasLogin() || spHeader->bodytype() == "DDRCommProto.reqLogin")
	{

		BaseMessageDispatcher::Dispatch(spParentSocketContainer, spHeader, spMsg);
	}
	else
	{
		DebugLog("Dispatch Error , has not login")
	}

}
