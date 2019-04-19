#ifndef TalkHBProcessor_h__
#define TalkHBProcessor_h__



#include "../../../Shared/src/Network/BaseProcessor.h"
#include "../../../Shared/src/Network/TcpSocketContainer.h"

class  TalkHBProcessor : public DDRFramework::BaseProcessor
{
public:
	TalkHBProcessor(DDRFramework::BaseMessageDispatcher& dispatcher);
	~TalkHBProcessor();


	virtual void Process(std::shared_ptr<DDRFramework::BaseSocketContainer> spSockContainer, std::shared_ptr<DDRCommProto::CommonHeader> spHeader, std::shared_ptr<google::protobuf::Message> spMsg) override;
};


#endif // TalkHBProcessor_h__
