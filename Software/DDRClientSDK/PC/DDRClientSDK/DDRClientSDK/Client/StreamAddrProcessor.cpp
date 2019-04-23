#include "StreamAddrProcessor.h"
#include "../../../Shared/src/Utility/DDRMacro.h"
#include "GlobalManager.h"
using namespace DDRFramework;
using namespace DDRCommProto;


StreamAddrProcessor::StreamAddrProcessor(BaseMessageDispatcher& dispatcher) : BaseProcessor::BaseProcessor(dispatcher)
{
}


StreamAddrProcessor::~StreamAddrProcessor()
{
}


void StreamAddrProcessor::Process(std::shared_ptr<BaseSocketContainer> spSockContainer, std::shared_ptr<CommonHeader> spHeader, std::shared_ptr<google::protobuf::Message> spMsg)
{
	auto bodytype = spHeader->bodytype();
	rspStreamAddr* pRaw = reinterpret_cast<rspStreamAddr*>(spMsg.get());


	std::string error = pRaw->error();
	if (error.empty())
	{
		size_t size = pRaw->channels().size();

		if (size > 0)
		{
			for (auto channel : pRaw->channels())
			{
				if (channel.networktype() == ChannelNetworkType::Local)
				{
					if (channel.streamtype() == ChannelStreamType::Audio)
					{
						if (channel.srcport().size() > 0)
						{

							std::string ip = channel.srcaddr();
							int port = channel.srcport(0);
							std::string srcname = channel.srcname();
							DebugLog("StreamAddrProcessor %s", srcname.c_str());
							//GlobalManager::Instance()->StartAudioClient(ip, port);

						}
						else
						{
							//to do get 
						}

					}


				}
				else if (channel.networktype() == ChannelNetworkType::Remote)
				{

				}

			}
		}
		else
		{
		}

	}
	else
	{
		auto serror = GlobalManager::Instance()->GetLocalizationConfig().GetString(error);
		DebugLog("%s", serror.c_str());
	}




	

}
