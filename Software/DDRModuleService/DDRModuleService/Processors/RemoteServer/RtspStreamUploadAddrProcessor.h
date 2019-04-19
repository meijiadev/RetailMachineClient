#ifndef RtspStreamUploadAddrProcessor_h__
#define RtspStreamUploadAddrProcessor_h__


#include "../../../Shared/src/Network/MessageSerializer.h"
#include "../../../Shared/src/Network/BaseProcessor.h"


using namespace DDRFramework;
class RtspStreamUploadAddrProcessor : public BaseProcessor
{
public:
	RtspStreamUploadAddrProcessor(BaseMessageDispatcher& dispatcher);
	~RtspStreamUploadAddrProcessor();

	virtual void Process(std::shared_ptr<BaseSocketContainer> spSockContainer, std::shared_ptr<DDRCommProto::CommonHeader> spHeader, std::shared_ptr<google::protobuf::Message> spMsg) override;
	virtual void AsyncProcess(std::shared_ptr<BaseSocketContainer> spSockContainer, std::shared_ptr<DDRCommProto::CommonHeader> spHeader, std::shared_ptr<google::protobuf::Message> spMsg) override;
private:
};

#endif // RtspStreamUploadAddrProcessor_h__
