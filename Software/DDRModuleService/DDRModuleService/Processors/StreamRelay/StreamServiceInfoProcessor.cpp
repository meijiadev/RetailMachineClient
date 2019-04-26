#include "StreamServiceInfoProcessor.h"
#include <memory>
#include "proto/BaseCmd.pb.h"
#include "src/Utility/DDRMacro.h"

#include "src/Utility/Logger.h"
#include "../../Managers/GlobalManager.h"
#include "../../Managers/StreamRelayServiceManager.h"
using namespace DDRFramework;
using namespace DDRCommProto;

StreamServiceInfoProcessor::StreamServiceInfoProcessor(BaseMessageDispatcher& dispatcher) :BaseProcessor(dispatcher)
{ 
}


StreamServiceInfoProcessor::~StreamServiceInfoProcessor()
{
}

void StreamServiceInfoProcessor::Process(std::shared_ptr<BaseSocketContainer> spSockContainer, std::shared_ptr<CommonHeader> spHeader, std::shared_ptr<google::protobuf::Message> spMsg)
{

	reqStreamServiceInfo* pRaw = reinterpret_cast<reqStreamServiceInfo*>(spMsg.get());


	auto sprsp = std::make_shared<rspStreamServiceInfo>();

	//sprsp->set_srcip("rtsp://192.168.1.88");
	for (auto channel : StreamRelayServiceManager::Instance()->m_LocalStreamConfig)
	{
			auto pchannel = sprsp->add_channels();


			pchannel->set_src(channel.mSrc);
			pchannel->set_srcname(channel.mSrcName);
			pchannel->set_rate(channel.mBandWidth);
			pchannel->set_networktype(ChannelNetworkType::Local);

			if (channel.mType == StreamRelayServiceManager::LocalStreamSrc::Audio)
			{
				pchannel->set_streamtype(ChannelStreamType::Audio);
			}
			else if (channel.mType == StreamRelayServiceManager::LocalStreamSrc::Video)
			{
				pchannel->set_streamtype(ChannelStreamType::Video);

			}
			else if (channel.mType == StreamRelayServiceManager::LocalStreamSrc::VideoAudio)
			{
				pchannel->set_streamtype(ChannelStreamType::VideoAudio);

			}

	}

	for (int i = 0;i< StreamRelayServiceManager::Instance()->m_ChannelsToUploadOnRemoteServer.size();i++)
	{
		auto remote_dst = StreamRelayServiceManager::Instance()->m_ChannelsToUploadOnRemoteServer[i];
		auto local_src = StreamRelayServiceManager::Instance()->m_LocalStreamConfig[i];


		auto pchannel = sprsp->add_channels();


		pchannel->set_src(local_src.mSrc);
		pchannel->set_srcname(local_src.mSrcName);


		pchannel->set_dst(remote_dst.url());
		pchannel->set_rate(remote_dst.uploadbandwidth());
		pchannel->set_networktype(ChannelNetworkType::Remote);

		if (remote_dst.type() == RemoteStreamChannel_StreamType_Audio)
		{
			pchannel->set_streamtype(ChannelStreamType::Audio);
		}
		else if (remote_dst.type() == RemoteStreamChannel_StreamType_Video)
		{
			pchannel->set_streamtype(ChannelStreamType::Video);

		}
		else if (remote_dst.type() == RemoteStreamChannel_StreamType_VideoAudio)
		{
			pchannel->set_streamtype(ChannelStreamType::VideoAudio);

		}
	}

	sprsp->set_tcpport(StreamRelayServiceManager::Instance()->GetServerTcpPort());

	spSockContainer->Send(sprsp);

}