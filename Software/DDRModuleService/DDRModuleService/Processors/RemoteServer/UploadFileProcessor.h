#ifndef UploadFileProcessor_h__
#define UploadFileProcessor_h__


#include "src/Network/MessageSerializer.h"
#include "src/Network/BaseProcessor.h"


using namespace DDRFramework;
class UploadFileProcessor : public BaseProcessor
{
public:
	UploadFileProcessor(BaseMessageDispatcher& dispatcher);
	~UploadFileProcessor();

	virtual void Process(std::shared_ptr<BaseSocketContainer> spSockContainer, std::shared_ptr<DDRCommProto::CommonHeader> spHeader, std::shared_ptr<google::protobuf::Message> spMsg) override;
	virtual void AsyncProcess(std::shared_ptr<BaseSocketContainer> spSockContainer, std::shared_ptr<DDRCommProto::CommonHeader> spHeader, std::shared_ptr<google::protobuf::Message> spMsg) override;


	void OnPostDone(float f);
private:
};

#endif // UploadFileProcessor_h__
