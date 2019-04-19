#ifndef LoginProcessor_h__
#define LoginProcessor_h__

#include "../../../Shared/src/Network/BaseProcessor.h"
#include "../../../Shared/src/Network/TcpSocketContainer.h"



class LoginProcessor : public DDRFramework::BaseProcessor
{
public:
	LoginProcessor(DDRFramework::BaseMessageDispatcher& dispatcher);
	~LoginProcessor();


	virtual void Process(std::shared_ptr<DDRFramework::BaseSocketContainer> spSockContainer, std::shared_ptr<DDRCommProto::CommonHeader> spHeader, std::shared_ptr<google::protobuf::Message> spMsg) override;

	virtual void AsyncProcess(std::shared_ptr<DDRFramework::BaseSocketContainer> spSockContainer, std::shared_ptr<DDRCommProto::CommonHeader> spHeader, std::shared_ptr<google::protobuf::Message> spMsg) override;
};


#endif // LoginProcessor_h__