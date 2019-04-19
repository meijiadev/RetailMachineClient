#include "LSClientDispatcher.h"
#include "../../../Shared/proto/BaseCmd.pb.h"

#include "../Processors/RemoteServer/RemoteServerListProcessor.h"
#include "../Processors/RemoteServer/UploadFileProcessor.h"
#include "../Processors/RemoteServer/RegisteLSProcessor.h"
#include "../Processors/RemoteServer/RtspStreamUploadAddrProcessor.h"
using namespace DDRCommProto;
using namespace DDRFramework;

LSClientDispatcher::LSClientDispatcher()
{		

	RegisterProcessor(notify, UploadFile)

		RegisterProcessor(rsp, RegisteLS)
		RegisterProcessor(rsp, RtspStreamUploadAddr)
}


LSClientDispatcher::~LSClientDispatcher()
{
}

//----------------------------------------------------------------------------------------------------------------------------------------

LSBroadcastReceiveClientDispatcher::LSBroadcastReceiveClientDispatcher()
{
	RegisterProcessor(rsp, RemoteServerList)

}


LSBroadcastReceiveClientDispatcher::~LSBroadcastReceiveClientDispatcher()
{
}
