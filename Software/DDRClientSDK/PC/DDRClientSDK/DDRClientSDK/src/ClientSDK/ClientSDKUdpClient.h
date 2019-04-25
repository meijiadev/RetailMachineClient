#ifndef ClientSDKUdpClient_h__
#define ClientSDKUdpClient_h__

#include "src/Network/UdpSocketBase.h"
#include "src/Interface/DDRClientInterface.h"

namespace DDRSDK
{
	class ClientSDKUdpClient : public DDRFramework::UdpSocketBase
	{
	public:
		ClientSDKUdpClient();;
		~ClientSDKUdpClient();;



		void SetParentInterface(std::shared_ptr<DDRClientInterface> sp)
		{
			m_wpParentInterface = sp;
		}

		std::shared_ptr<DDRClientInterface> GetParentInterface()
		{
			return m_wpParentInterface.lock();
		}
	private:


		std::weak_ptr<DDRClientInterface> m_wpParentInterface;
	};
}
#endif // ClientSDKUdpClient_h__
