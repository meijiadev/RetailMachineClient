#ifndef StreamRelayServiceManager_h__
#define StreamRelayServiceManager_h__
#include <string>
#include "../../../Shared/src/Utility/XmlLoader.h"
#include "../../../Shared/src/Utility/Singleton.h"

#include "../../../Shared/src/Network/TcpServerBase.h"
#include "../../../Shared/proto/BaseCmd.pb.h"
#include "../../../Shared/proto/RemoteCmd.pb.h"
#include "eventpp/eventdispatcher.h"
#include "../Servers/LocalTcpServer.h"

using namespace DDRCommProto;
using namespace DDRFramework;
class StreamRelayServiceManager : public CSingleton<StreamRelayServiceManager>
{
public:
	struct LocalStreamSrc
	{
		enum EStreamType
		{
			Video,
			Audio,
			VideoAudio,
		};

		EStreamType mType;
		std::string mSrc;
		int mBandWidth;
		std::string mSrcName;
	};

public:
	StreamRelayServiceManager();
	~StreamRelayServiceManager();

	void InitAVChannels();

	void InitChannelsToUpload();
	
	int GetServerTcpPort();

	void UpdateUploadServer(rspRtspStreamUploadAddr* prsp);

	void NotifyUploadServerChanged();

	std::vector<LocalStreamSrc> m_LocalStreamConfig;
	std::vector<RemoteStreamChannel> m_ChannelsToUploadConfig;

	std::vector<RemoteStreamChannel> m_ChannelsToUploadOnRemoteServer;


private:


	XmlLoader m_StreamSrcProxyConfigLoader;




};



#endif // StreamRelayServiceManager_h__
