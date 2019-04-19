#include "StreamAddrProcessor.h"
#include "../../Managers/StreamRelayServiceManager.h"
#include "../../Managers/GlobalManager.h"
#include "../../../Shared/proto/BaseCmd.pb.h"
#include "../../../Shared/src/Utility/DDRMacro.h"
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
	reqStreamAddr* pRaw = reinterpret_cast<reqStreamAddr*>(spMsg.get());


	auto sprsp = std::make_shared<rspStreamAddr>();





	if (pRaw->networktype() == ChannelNetworkType::Local)
	{
		auto spSession = GlobalManager::Instance()->GetTcpServer()->GetSessionByType(eLSMStreamRelay);
		if (spSession)
		{

			auto& channels = StreamRelayServiceManager::Instance()->m_LocalStreamConfig;
			for (auto channel : channels)
			{
				auto *pChannel = sprsp->add_channels();

				if (channel.mType == StreamRelayServiceManager::LocalStreamSrc::EStreamType::Video)
				{

					pChannel->set_streamtype(ChannelStreamType::Video);
					pChannel->set_srcaddr(channel.mSrc);

				}
				else if (channel.mType == StreamRelayServiceManager::LocalStreamSrc::EStreamType::VideoAudio)
				{


					pChannel->set_streamtype(ChannelStreamType::VideoAudio);
					pChannel->set_srcaddr(channel.mSrc);

				}
				else if (channel.mType == StreamRelayServiceManager::LocalStreamSrc::EStreamType::Audio)
				{

					std::string dstip = spSession->GetSocket().remote_endpoint().address().to_string();
					pChannel->set_srcaddr(dstip);
					pChannel->add_srcport(StreamRelayServiceManager::Instance()->GetServerTcpPort());
					pChannel->set_streamtype(ChannelStreamType::Audio);


				}


				pChannel->set_networktype(ChannelNetworkType::Local);
				pChannel->set_rate(channel.mBandWidth);
				pChannel->set_srcname(channel.mSrcName);
			}

		}
		else
		{
			sprsp->set_error("LS_Err_NoStreamRelayService_Connected");
		}

	}
	else if (pRaw->networktype() == ChannelNetworkType::Remote)
	{

		sprsp->set_error("Local Server Don't Process StreamAddr Local Request");
		//auto spSession = GlobalManager::Instance()->GetTcpServer()->GetSessionByType(eLSMStreamRelay);
		//if (spSession)
		//{
		//	auto& channels = StreamRelayServiceManager::Instance()->m_ChannelsToUploadOnRemoteServer;
		//	for (auto channel : channels)
		//	{
		//		auto *pChannel = sprsp->add_channels();

		//		pChannel->set_networktype(ChannelNetworkType::Remote);


		//		pChannel->set_srcaddr(channel.url());

		//		if (channel.type() == RemoteStreamChannel_StreamType_Audio)
		//		{

		//			pChannel->set_streamtype(ChannelStreamType::Audio);
		//		}
		//		if (channel.type() == RemoteStreamChannel_StreamType_Video)
		//		{

		//			pChannel->set_streamtype(ChannelStreamType::Video);
		//		}
		//		if (channel.type() == RemoteStreamChannel_StreamType_VideoAudio)
		//		{

		//			pChannel->set_streamtype(ChannelStreamType::VideoAudio);
		//		}



		//		pChannel->set_rate(channel.downloadbandwidth());
		//		pChannel->set_srcname(channel.srcname());


		//	}


		}
	else
		{
			sprsp->set_error("LS_Err_NoStreamRelayService_Connected");

		}


	


	spSockContainer->Send(sprsp);
}
