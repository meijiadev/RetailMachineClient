#ifndef StreamAddrProcessor_h__
#define StreamAddrProcessor_h__




#include "../../../Shared/src/Network/BaseProcessor.h"
#include "../../../Shared/src/Network/TcpSocketContainer.h"

class  StreamAddrProcessor : public DDRFramework::BaseProcessor
{
public:
	StreamAddrProcessor(DDRFramework::BaseMessageDispatcher& dispatcher);
	~StreamAddrProcessor();


	virtual void Process(std::shared_ptr<DDRFramework::BaseSocketContainer> spSockContainer, std::shared_ptr<DDRCommProto::CommonHeader> spHeader, std::shared_ptr<google::protobuf::Message> spMsg) override;
};

#endif // StreamAddrProcessor_h__

