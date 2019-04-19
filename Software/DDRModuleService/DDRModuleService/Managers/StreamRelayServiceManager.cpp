#include "StreamRelayServiceManager.h"
#include "GlobalManager.h"


StreamRelayServiceManager::StreamRelayServiceManager() : m_StreamSrcProxyConfigLoader("Config/LocalServer/StreamSrcProxyConfig.xml")
{

	InitAVChannels();
	InitChannelsToUpload();
}

StreamRelayServiceManager::~StreamRelayServiceManager()
{
}

void StreamRelayServiceManager::InitAVChannels()
{
	auto config = m_StreamSrcProxyConfigLoader;
	int channelNum = config.RowGetKeyCount();


	for (int i = 0; i < channelNum; i++)
	{
		LocalStreamSrc channel;

		auto sStreamType = config.GetValue(i, "StreamType");


		LocalStreamSrc::EStreamType  streamType;
		if (sStreamType == "A")
		{
			streamType = LocalStreamSrc::EStreamType::Audio;
		}
		else if (sStreamType == "V")
		{
			streamType = LocalStreamSrc::EStreamType::Video;
		}
		if (sStreamType == "AV")
		{
			streamType = LocalStreamSrc::EStreamType::VideoAudio;
		}
		channel.mType = (streamType);



		channel.mBandWidth = (config.GetValue<int>(i, "BandWidth"));
		channel.mSrc = (config.GetValue(i, "SrcAddress"));
		channel.mSrcName = (config.GetValue(i, "SrcName"));


		m_LocalStreamConfig.push_back(channel);
	}
}


void StreamRelayServiceManager::InitChannelsToUpload()
{
	for (auto channel : m_LocalStreamConfig)
	{

		RemoteStreamChannel c;
		c.set_url("");//do not need fill url

		if (channel.mType == LocalStreamSrc::EStreamType::Video)
		{

			c.set_type(RemoteStreamChannel_StreamType_Video);
		}
		else if (channel.mType == LocalStreamSrc::EStreamType::Audio)
		{

			c.set_type(RemoteStreamChannel_StreamType_Audio);
		}
		else if (channel.mType == LocalStreamSrc::EStreamType::VideoAudio)
		{

			c.set_type(RemoteStreamChannel_StreamType_VideoAudio);
		}


		c.set_uploadbandwidth(channel.mBandWidth);
		c.set_downloadbandwidth(0);//do not need fill downloadbandwidth
		c.set_srcname(channel.mSrcName);

		m_ChannelsToUploadConfig.push_back(c);

	}
}

int StreamRelayServiceManager::GetServerTcpPort()
{
	return GlobalManager::Instance()->GetLocalServerConfig().GetValue<int>("StreamRelayTcpPort");

}

void StreamRelayServiceManager::UpdateUploadServer(rspRtspStreamUploadAddr* prsp)
{
	for (int i = 0; i < prsp->channels_size(); i++)
	{
		auto& channel = prsp->channels(i);
		m_ChannelsToUploadOnRemoteServer.push_back(channel);
	}

	NotifyUploadServerChanged();
}

void StreamRelayServiceManager::NotifyUploadServerChanged()
{

	auto spNotify = std::make_shared<notifyStreamServiceInfoChanged>();
	auto spTcpServer = GlobalManager::Instance()->GetTcpServer();
	if (spNotify && spTcpServer)
	{
		auto spSession = spTcpServer->GetSessionByType(DDRCommProto::eCltType::eLSMStreamRelay);
		if (spSession)
		{
			for (int i = 0; i < m_ChannelsToUploadOnRemoteServer.size(); i++)
			{
				auto pchannel = spNotify->add_channels();

				pchannel->set_src(m_LocalStreamConfig[i].mSrc);

				auto fromChannel = m_ChannelsToUploadOnRemoteServer[i];

				pchannel->set_dst(fromChannel.url());
				pchannel->set_srcname(fromChannel.srcname());


				if (fromChannel.type() == RemoteStreamChannel_StreamType_Audio)
				{

					pchannel->set_streamtype(ChannelStreamType::Audio);
				}
				else if (fromChannel.type() == RemoteStreamChannel_StreamType_Video)
				{

					pchannel->set_streamtype(ChannelStreamType::Video);
				}
				else if (fromChannel.type() == RemoteStreamChannel_StreamType_VideoAudio)
				{

					pchannel->set_streamtype(ChannelStreamType::VideoAudio);
				}

				pchannel->set_networktype(ChannelNetworkType::Remote);
			}

			spSession->Send(spNotify);
		}
	}
}
