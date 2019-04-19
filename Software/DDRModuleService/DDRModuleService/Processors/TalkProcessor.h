#ifndef TalkProcessor_h__
#define TalkProcessor_h__



#include "../../../Shared/src/Network/BaseProcessor.h"
#include "../../../Shared/src/Network/TcpSocketContainer.h"

class  TalkProcessor : public DDRFramework::BaseProcessor
{
public:
	TalkProcessor(DDRFramework::BaseMessageDispatcher& dispatcher);
	~TalkProcessor();


	virtual void Process(std::shared_ptr<DDRFramework::BaseSocketContainer> spSockContainer, std::shared_ptr<DDRCommProto::CommonHeader> spHeader, std::shared_ptr<google::protobuf::Message> spMsg) override;
};


#endif // TalkProcessor_h__
