#ifndef StatusProcessor_h__
#define StatusProcessor_h__



#include "src/Network/BaseProcessor.h"
#include "src/Network/TcpSocketContainer.h"

class  StatusProcessor : public DDRFramework::BaseProcessor
{
public:
	StatusProcessor(DDRFramework::BaseMessageDispatcher& dispatcher);
	~StatusProcessor();


	virtual void Process(std::shared_ptr<DDRFramework::BaseSocketContainer> spSockContainer, std::shared_ptr<DDRCommProto::CommonHeader> spHeader, std::shared_ptr<google::protobuf::Message> spMsg) override;
};


#endif // StatusProcessor_h__
