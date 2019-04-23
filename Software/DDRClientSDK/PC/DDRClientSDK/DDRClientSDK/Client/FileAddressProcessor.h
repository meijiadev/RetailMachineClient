#ifndef FileAddressProcessor_h__
#define FileAddressProcessor_h__


#include "../../../Shared/src/Network/MessageSerializer.h"
#include "../../../Shared/src/Network/BaseProcessor.h"
#include "../../../Shared/src/Network/HttpClient.h"


using namespace DDRFramework;
class FileAddressProcessor : public BaseProcessor
{
public:
	FileAddressProcessor(BaseMessageDispatcher& dispatcher);
	~FileAddressProcessor();

	virtual void Process(std::shared_ptr<BaseSocketContainer> spSockContainer, std::shared_ptr<DDRCommProto::CommonHeader> spHeader, std::shared_ptr<google::protobuf::Message> spMsg) override;
	virtual void AsyncProcess(std::shared_ptr<BaseSocketContainer> spSockContainer, std::shared_ptr<DDRCommProto::CommonHeader> spHeader, std::shared_ptr<google::protobuf::Message> spMsg) override;

	
	void OnGetDone(float f);

private:



	std::shared_ptr<HttpSession> spHttpSession;
};

#endif // FileAddressProcessor_h__

