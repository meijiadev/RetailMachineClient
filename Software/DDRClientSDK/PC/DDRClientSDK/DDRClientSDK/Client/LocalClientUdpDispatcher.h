#ifndef LocalClientUdpDispatcher_h__
#define LocalClientUdpDispatcher_h__

#ifdef QT_PRECOMPILED_HEADER
    #include "stable.h"
#else
#endif

#include "../../../Shared/src/Network/BaseMessageDispatcher.h"


class LocalClientUdpDispatcher : public DDRFramework::BaseMessageDispatcher
{
public:
	LocalClientUdpDispatcher();
	~LocalClientUdpDispatcher();
};


#endif // LocalClientUdpDispatcher_h__
