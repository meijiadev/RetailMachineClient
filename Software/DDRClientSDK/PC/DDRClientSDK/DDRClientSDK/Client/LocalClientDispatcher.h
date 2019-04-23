#ifndef LocalClientDispatcher_h__
#define LocalClientDispatcher_h__

#ifdef QT_PRECOMPILED_HEADER
    #include "stable.h"
#else
#endif

#include "../../../Shared/src/Network/BaseMessageDispatcher.h"


class LocalClientDispatcher : public DDRFramework::BaseMessageDispatcher
{
public:
	LocalClientDispatcher();
	~LocalClientDispatcher();

};

class LSBroadcastReceiveClientDispatcher : public DDRFramework::BaseMessageDispatcher
{
public:
	LSBroadcastReceiveClientDispatcher();
	~LSBroadcastReceiveClientDispatcher();
};

#endif // LocalClientDispatcher_h__
