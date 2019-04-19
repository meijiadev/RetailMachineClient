

#include <memory>
#include "../../../Shared/proto/BaseCmd.pb.h"
#include "FileAddressProcessor.h"
#include "../../../Shared/src/Utility/DDRMacro.h"
#include "../../../Shared/src/Utility/Logger.h"
#include "../../Managers//GlobalManager.h"
#include "../../Managers//StreamRelayServiceManager.h"


#include "eventpp/callbacklist.h"
#include "eventpp/eventdispatcher.h"
#include "eventpp/eventqueue.h"
#include "../../Servers/LocalTcpServer.h"
using namespace DDRFramework;
using namespace DDRCommProto;

FileAddressProcessor::FileAddressProcessor(BaseMessageDispatcher& dispatcher) :BaseProcessor(dispatcher)
{
}


FileAddressProcessor::~FileAddressProcessor()
{
}

void FileAddressProcessor::Process(std::shared_ptr<BaseSocketContainer> spSockContainer, std::shared_ptr<CommonHeader> spHeader, std::shared_ptr<google::protobuf::Message> spMsg)
{


}

void FileAddressProcessor::AsyncProcess(std::shared_ptr<BaseSocketContainer> spSockContainer, std::shared_ptr<DDRCommProto::CommonHeader> spHeader, std::shared_ptr<google::protobuf::Message> spMsg)
{
	reqFileAddress* pRaw = reinterpret_cast<reqFileAddress*>(spMsg.get());

	if (pRaw)
	{
		//Record Mannually ,Router Info
		MsgRouterManager::Instance()->RecordPassNode(spHeader, spSockContainer->GetTcp());


		auto sprsp = std::make_shared<chkFileStatus>();
		sprsp->set_filetype(pRaw->filetype());
		for (auto fmt : pRaw->filenames())
		{
			sprsp->add_filefmt(fmt);
		}

		eCltType toType = pRaw->tarservicetype();
		if ((toType & eAllLSM) != 0)
		{

			auto spSession = GlobalManager::Instance()->GetTcpServer()->GetSessionByType(pRaw->tarservicetype());
			if (spSession)
			{
				spSession->Send(spHeader,sprsp);
			}
			else
			{
				DebugLog("FileAddressProcessor No Session Conncected type:%i", pRaw->tarservicetype());
			}
		}
	}

}
