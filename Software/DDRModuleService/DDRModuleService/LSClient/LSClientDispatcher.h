#ifndef LSClientDispatcher_h__
#define LSClientDispatcher_h__



#include "src/Network/BaseMessageDispatcher.h"


class LSClientDispatcher : public DDRFramework::BaseMessageDispatcher
{
public:
	LSClientDispatcher();
	~LSClientDispatcher();
};

class LSBroadcastReceiveClientDispatcher : public DDRFramework::BaseMessageDispatcher
{
public:
	LSBroadcastReceiveClientDispatcher();
	~LSBroadcastReceiveClientDispatcher();
};
#endif // LSClientDispatcher_h__