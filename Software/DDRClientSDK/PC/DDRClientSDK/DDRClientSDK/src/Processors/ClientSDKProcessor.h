#ifndef ClientSDKProcessor_h__
#define ClientSDKProcessor_h__


#include "../../../Shared/src/Network/MessageSerializer.h"
#include "../../../Shared/src/Network/BaseProcessor.h"
#include <include/DDRClientSDK.h>

using namespace DDRFramework;

namespace DDRSDK
{

	class ClientSDKProcessor : public BaseProcessor
	{
	public:
		ClientSDKProcessor(BaseMessageDispatcher& dispatcher);
		~ClientSDKProcessor();

		virtual void Process(std::shared_ptr<BaseSocketContainer> spSockContainer, std::shared_ptr<DDRCommProto::CommonHeader> spHeader, std::shared_ptr<google::protobuf::Message> spMsg) override;
		virtual void AsyncProcess(std::shared_ptr<BaseSocketContainer> spSockContainer, std::shared_ptr<DDRCommProto::CommonHeader> spHeader, std::shared_ptr<google::protobuf::Message> spMsg) override;


	private:

	};
}
#endif // ClientSDKProcessor_h__
