#ifndef HeartBeatProcessor_h__
#define HeartBeatProcessor_h__

#include "../../../Shared/src/Network/BaseProcessor.h"
#include "../../../Shared/src/Network/TcpSocketContainer.h"

class HeartBeatProcessor : public DDRFramework::BaseProcessor
{
public:
	HeartBeatProcessor(DDRFramework::BaseMessageDispatcher& dispatcher);
	~HeartBeatProcessor();


	virtual void Process(std::shared_ptr<DDRFramework::BaseSocketContainer> spSockContainer, std::shared_ptr<DDRCommProto::CommonHeader> spHeader, std::shared_ptr<google::protobuf::Message> spMsg) override;
};


#endif // HeartBeatProcessor_h__
