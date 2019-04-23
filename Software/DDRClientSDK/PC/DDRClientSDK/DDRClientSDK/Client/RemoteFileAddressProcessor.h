#ifndef RemoteFileAddressProcessor_h__
#define RemoteFileAddressProcessor_h__


#include "../../../Shared/src/Network/MessageSerializer.h"
#include "../../../Shared/src/Network/BaseProcessor.h"
#include "../../../Shared/src/Network/HttpClient.h"


using namespace DDRFramework;
class RemoteFileAddressProcessor : public BaseProcessor
{
public:
	RemoteFileAddressProcessor(BaseMessageDispatcher& dispatcher);
	~RemoteFileAddressProcessor();

	virtual void Process(std::shared_ptr<BaseSocketContainer> spSockContainer, std::shared_ptr<DDRCommProto::CommonHeader> spHeader, std::shared_ptr<google::protobuf::Message> spMsg) override;
	virtual void AsyncProcess(std::shared_ptr<BaseSocketContainer> spSockContainer, std::shared_ptr<DDRCommProto::CommonHeader> spHeader, std::shared_ptr<google::protobuf::Message> spMsg) override;


	void OnGetDone(float f);
private:

	std::shared_ptr<HttpSession> spHttpSession;
};

#endif // RemoteFileAddressProcessor_h__
