#include "LocalClientDispatcher.h"
#include "../../../Shared/src/Utility/DDRMacro.h"

#ifdef QT_PRECOMPILED_HEADER
#include "LoginProcessor.h"
#include "../../Dadaoii_Proto/Proto/AudioStreamClientProcessor.h"
#else
#include "LoginProcessor.h"
#include "StreamAddrProcessor.h"
#include "AlarmProcessor.h"
#include "FileAddressProcessor.h"
#include "RemoteFileAddressProcessor.h"
#include "RemoteServerListProcessor.h"
#include "RemoteLoginProcessor.h"
#include "SelectLSProcessor.h"
#include "UnselectLSProcessor.h"
#include "CmdMoveProcessor.h"
#include "AudioTalkProcessor.h"
#endif
using namespace DDRCommProto;
using namespace DDRFramework;

LocalClientDispatcher::LocalClientDispatcher()
{
        RegisterProcessor(rsp, Login)
        RegisterProcessor(rsp, StreamAddr)
        RegisterProcessor(notify, Alarm)
			RegisterProcessor(rsp, FileAddress)
			RegisterProcessor(rsp, RemoteFileAddress)
			RegisterProcessor(rsp, RemoteLogin)
			RegisterProcessor(rsp, CmdMove)
			RegisterProcessor(rsp, AudioTalk)


			RegisterProcessor(rsp, SelectLS)

			RegisterProcessor(rsp, UnselectLS)

}


LocalClientDispatcher::~LocalClientDispatcher()
{
}

LSBroadcastReceiveClientDispatcher::LSBroadcastReceiveClientDispatcher()
{
	RegisterProcessor(rsp, RemoteServerList)

}


LSBroadcastReceiveClientDispatcher::~LSBroadcastReceiveClientDispatcher()
{
}