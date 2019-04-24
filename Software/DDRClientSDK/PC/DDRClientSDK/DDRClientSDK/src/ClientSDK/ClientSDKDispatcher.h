#ifndef ClientSDKDispatcher_h__
#define ClientSDKDispatcher_h__

#ifdef QT_PRECOMPILED_HEADER
    #include "stable.h"
#else
#endif

#include "../../../Shared/src/Network/BaseMessageDispatcher.h"
namespace DDRSDK
{

	class ClientSDKDispatcher : public DDRFramework::BaseMessageDispatcher
	{
	public:
		ClientSDKDispatcher();
		~ClientSDKDispatcher();

	};

}
#endif // ClientSDKDispatcher_h__
