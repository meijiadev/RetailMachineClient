

#include <memory>
#include "../../../Shared/proto/RemoteCmd.pb.h"
#include "RtspStreamUploadAddrProcessor.h"
#include "../../../Shared/src/Utility/DDRMacro.h"
#include "../../../Shared/src/Utility/Logger.h"
#include "../../Managers/StreamRelayServiceManager.h"
using namespace DDRFramework;
using namespace DDRCommProto;

RtspStreamUploadAddrProcessor::RtspStreamUploadAddrProcessor(BaseMessageDispatcher& dispatcher) :BaseProcessor(dispatcher)
{
}


RtspStreamUploadAddrProcessor::~RtspStreamUploadAddrProcessor()
{
}

void RtspStreamUploadAddrProcessor::Process(std::shared_ptr<BaseSocketContainer> spSockContainer, std::shared_ptr<CommonHeader> spHeader, std::shared_ptr<google::protobuf::Message> spMsg)
{

	rspRtspStreamUploadAddr* pRaw = reinterpret_cast<rspRtspStreamUploadAddr*>(spMsg.get());
	if (pRaw)
	{
		StreamRelayServiceManager::Instance()->UpdateUploadServer(pRaw);

		for (auto channel : pRaw->channels())
		{
			DebugLog("RtspStreamUploadAddrProcessor %s", channel.url().c_str())
		}
	}
}

void RtspStreamUploadAddrProcessor::AsyncProcess(std::shared_ptr<BaseSocketContainer> spSockContainer, std::shared_ptr<DDRCommProto::CommonHeader> spHeader, std::shared_ptr<google::protobuf::Message> spMsg)
{


}
