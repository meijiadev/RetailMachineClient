#ifndef StreamRelayAlarmProcessor_h__
#define StreamRelayAlarmProcessor_h__




#include "../../../Shared/src/Network/MessageSerializer.h"
#include "../../../Shared/src/Network/BaseProcessor.h"


using namespace DDRFramework;
class StreamRelayAlarmProcessor : public BaseProcessor
{
public:
	StreamRelayAlarmProcessor(BaseMessageDispatcher& dispatcher);
	~StreamRelayAlarmProcessor();

	virtual void Process(std::shared_ptr<BaseSocketContainer> spSockContainer, std::shared_ptr<DDRCommProto::CommonHeader> spHeader, std::shared_ptr<google::protobuf::Message> spMsg) override;

private:
};

#endif // StreamRelayAlarmProcessor_h__