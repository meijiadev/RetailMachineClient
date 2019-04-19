#ifndef AlarmProcessor_h__
#define AlarmProcessor_h__



#include "../../../Shared/src/Network/BaseProcessor.h"
#include "../../../Shared/src/Network/TcpSocketContainer.h"

class  AlarmProcessor : public DDRFramework::BaseProcessor
{
public:
	AlarmProcessor(DDRFramework::BaseMessageDispatcher& dispatcher);
	~AlarmProcessor();


	virtual void Process(std::shared_ptr<DDRFramework::BaseSocketContainer> spSockContainer, std::shared_ptr<DDRCommProto::CommonHeader> spHeader, std::shared_ptr<google::protobuf::Message> spMsg) override;
};


#endif // AlarmProcessor_h__
