#ifndef CmdProcessor_h__
#define CmdProcessor_h__



#include "src/Network/BaseProcessor.h"
#include "src/Network/TcpSocketContainer.h"

class  CmdProcessor : public DDRFramework::BaseProcessor
{
public:
	CmdProcessor(DDRFramework::BaseMessageDispatcher& dispatcher);
	~CmdProcessor();


	virtual void Process(std::shared_ptr<DDRFramework::BaseSocketContainer> spSockContainer, std::shared_ptr<DDRCommProto::CommonHeader> spHeader, std::shared_ptr<google::protobuf::Message> spMsg) override;
};


#endif // CmdProcessor_h__
