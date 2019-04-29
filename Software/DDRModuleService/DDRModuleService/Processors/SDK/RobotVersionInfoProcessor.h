#ifndef RobotVersionInfoProcessor_h__
#define RobotVersionInfoProcessor_h__


#include "../../../Shared/src/Network/MessageSerializer.h"
#include "../../../Shared/src/Network/BaseProcessor.h"


using namespace DDRFramework;
class RobotVersionInfoProcessor : public BaseProcessor
{
public:
	RobotVersionInfoProcessor(BaseMessageDispatcher& dispatcher);
	~RobotVersionInfoProcessor();

	virtual void Process(std::shared_ptr<BaseSocketContainer> spSockContainer, std::shared_ptr<DDRCommProto::CommonHeader> spHeader, std::shared_ptr<google::protobuf::Message> spMsg) override;
	virtual void AsyncProcess(std::shared_ptr<BaseSocketContainer> spSockContainer, std::shared_ptr<DDRCommProto::CommonHeader> spHeader, std::shared_ptr<google::protobuf::Message> spMsg) override;
private:
};

#endif // RobotVersionInfoProcessor_h__