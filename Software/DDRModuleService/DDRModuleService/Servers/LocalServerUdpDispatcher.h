#ifndef LocalServerUdpDispatcher_h__
#define LocalServerUdpDispatcher_h__


#include "../../../Shared/src/Network/BaseMessageDispatcher.h"


class LocalServerUdpDispatcher : public DDRFramework::BaseMessageDispatcher
{
public:
	LocalServerUdpDispatcher();
	~LocalServerUdpDispatcher();
};


#endif // LocalServerUdpDispatcher_h__