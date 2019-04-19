

#include <memory>
#include "../../../Shared/proto/BaseCmd.pb.h"
#include "UploadFileProcessor.h"
#include "../../../Shared/src/Utility/DDRMacro.h"
#include "../../../Shared/src/Utility/Logger.h"
#include "../../Managers/GlobalManager.h"
#include "../../../Shared/src/Network/HttpClient.h"
#include "../../Managers/FileManager.h"

using namespace DDRFramework;
using namespace DDRCommProto;

UploadFileProcessor::UploadFileProcessor(BaseMessageDispatcher& dispatcher) :BaseProcessor(dispatcher)
{
}


UploadFileProcessor::~UploadFileProcessor()
{
}

void UploadFileProcessor::Process(std::shared_ptr<BaseSocketContainer> spSockContainer, std::shared_ptr<CommonHeader> spHeader, std::shared_ptr<google::protobuf::Message> spMsg)
{

	notifyUploadFile* pRaw = reinterpret_cast<notifyUploadFile*>(spMsg.get());
	if (pRaw)
	{

		//Record Mannually ,Router Info
		auto pNode = MsgRouterManager::Instance()->RecordPassNode(spHeader, spSockContainer->GetTcp());
		notifyUploadFile* pData = new notifyUploadFile();
		pData->CopyFrom(*pRaw);
		pNode->add_intptrdata((size_t)pData);



		auto spchk = std::make_shared<chkFileStatus>();
		spchk->set_tarservicetype(pRaw->tarservicetype());
		spchk->set_filetype(eFileTypes::FileHttpAddress);

		std::vector<std::string> matchedfiles;
		for (auto fmt : pRaw->filefmt())
		{
			spchk->add_filefmt(fmt);

			DebugLog("UploadFileProcessor %s", fmt.c_str());
			auto files = FileManager::Instance()->Match(fmt);



			for (auto fullpath : files)
			{
				matchedfiles.push_back(FileManager::Instance()->GetRelativeDirFromFull(fullpath));
			}
		}


		std::set<std::string> remoteExistFiles;
		for (auto file : pRaw->existfiles())
		{
			remoteExistFiles.insert(file);
		}




		for (auto file : matchedfiles)
		{
			if (remoteExistFiles.find(file) != remoteExistFiles.end())//remote server exist
			{

			}
			else
			{

				std::string  fullpath = FileManager::Instance()->GetFullDirFromRelative(file);

				if (FileManager::Instance()->FileExist(fullpath))//local server exist
				{
					/*std::string httpurl = pRaw->httpaddr();
					auto spHttpSession = std::make_shared<HttpSession>();
					spHttpSession->DoPost(httpurl, FileManager::Instance()->GetRootPath(), file);*/


					spchk->add_existfiles(file);
				}
				else
				{
				}

			}

			
		}


		eCltType toType = pRaw->tarservicetype();
		if ((toType & eAllLSM) != 0)
		{

			auto spSession = GlobalManager::Instance()->GetTcpServer()->GetSessionByType(toType);
			if (spSession)
			{
				spSession->Send(spHeader, spchk);
			}
			else
			{
				DebugLog("UploadFileProcessor No Session Conncected type:%i", toType);
			}
		}


	}


}

void UploadFileProcessor::AsyncProcess(std::shared_ptr<BaseSocketContainer> spSockContainer, std::shared_ptr<DDRCommProto::CommonHeader> spHeader, std::shared_ptr<google::protobuf::Message> spMsg)
{


}

void UploadFileProcessor::OnPostDone(float f)
{
	DebugLog("UploadFileProcessor Post Done");
}