#ifndef TextChatProcessor_h__
#define TextChatProcessor_h__



#include "src/Network/BaseProcessor.h"
#include "src/Network/TcpSocketContainer.h"

class  TextChatProcessor : public DDRFramework::BaseProcessor
{
public:
	TextChatProcessor(DDRFramework::BaseMessageDispatcher& dispatcher);
	~TextChatProcessor();


	virtual void Process(std::shared_ptr<DDRFramework::BaseSocketContainer> spSockContainer, std::shared_ptr<DDRCommProto::CommonHeader> spHeader, std::shared_ptr<google::protobuf::Message> spMsg) override;
};


#endif // TextChatProcessor_h__
