#ifndef ClientSDKUdpDispatcher_h__
#define ClientSDKUdpDispatcher_h__

#ifdef QT_PRECOMPILED_HEADER
    #include "stable.h"
#else
#endif

#include "../../../Shared/src/Network/BaseMessageDispatcher.h"


class ClientSDKUdpDispatcher : public DDRFramework::BaseMessageDispatcher
{
public:
	ClientSDKUdpDispatcher();
	~ClientSDKUdpDispatcher();
};


#endif // ClientSDKUdpDispatcher_h__
