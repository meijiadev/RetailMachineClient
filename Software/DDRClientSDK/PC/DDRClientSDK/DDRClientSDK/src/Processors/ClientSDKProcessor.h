#ifndef ClientSDKProcessor_h__
#define ClientSDKProcessor_h__


#include "../../../Shared/src/Network/MessageSerializer.h"
#include "../../../Shared/src/Network/BaseProcessor.h"
#include <include/DDRClientSDK.h>

using namespace DDRFramework;

namespace DDRSDK
{
	class DDRClientInterface;

	//注册消息监听器类，收到回包后会回调到 OnMsgArrival
	class LoginListener : public DDRSDK::DDRBaseListener
	{

	public:
		LoginListener(std::shared_ptr<DDRClientInterface> spInterface)
		{
			m_spInterface = spInterface;
		}
		virtual void OnMsgArrival(std::shared_ptr<DDRCommProto::CommonHeader> spHeader, std::shared_ptr<google::protobuf::Message> spMsg) override;

	protected:
		std::shared_ptr<DDRClientInterface> m_spInterface;
	};

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
