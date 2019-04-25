#ifndef LocadClientUdpProcessor_h__
#define LocadClientUdpProcessor_h__

#ifdef QT_PRECOMPILED_HEADER
    #include "stable.h"
#else
#endif


#include "../../../Shared/src/Utility/DDRMacro.h"
#include "../../../Shared/src/Network/BaseProcessor.h"
#include "../../../Shared/src/Network/TcpSocketContainer.h"

using namespace DDRCommProto;


class ClientSDKUdpProcessor : public DDRFramework::BaseProcessor
{
public:
	ClientSDKUdpProcessor(DDRFramework::BaseMessageDispatcher& dispatcher);
	~ClientSDKUdpProcessor();


	virtual void Process(std::shared_ptr<DDRFramework::BaseSocketContainer> spSockContainer, std::shared_ptr<DDRCommProto::CommonHeader> spHeader, std::shared_ptr<google::protobuf::Message> spMsg) override;
private:

};


#endif // LocadClientUdpProcessor_h__
