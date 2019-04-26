#ifndef RemoteServerListProcessor_h__
#define RemoteServerListProcessor_h__


#include "src/Network/MessageSerializer.h"
#include "src/Network/BaseProcessor.h"


using namespace DDRFramework;
class RemoteServerListProcessor : public BaseProcessor
{
public:
	RemoteServerListProcessor(BaseMessageDispatcher& dispatcher);
	~RemoteServerListProcessor();

	virtual void Process(std::shared_ptr<BaseSocketContainer> spSockContainer, std::shared_ptr<DDRCommProto::CommonHeader> spHeader, std::shared_ptr<google::protobuf::Message> spMsg) override;
	virtual void AsyncProcess(std::shared_ptr<BaseSocketContainer> spSockContainer, std::shared_ptr<DDRCommProto::CommonHeader> spHeader, std::shared_ptr<google::protobuf::Message> spMsg) override;
private:
};

#endif // RemoteServerListProcessor_h__
