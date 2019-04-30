#include "ClientSDKDispatcher.h"
#include "../../../Shared/src/Utility/DDRMacro.h"

#include "../Processors/ClientSDKProcessor.h"
using namespace DDRCommProto;
using namespace DDRModuleProto;
using namespace DDRFramework;



#define RegisterClientSDKProcessor(msg) msg msg;\
m_ProcessorMap[msg.GetTypeName()] = sp;

namespace DDRSDK
{

	ClientSDKDispatcher::ClientSDKDispatcher()
	{
		auto sp = std::make_shared<ClientSDKProcessor>(*this);

		RegisterClientSDKProcessor(rspLogin)

			RegisterClientSDKProcessor(rspRobotVersionInfo)
			RegisterClientSDKProcessor(rspSetParameter)

			RegisterClientSDKProcessor(rspCmdMove)
			RegisterClientSDKProcessor(rspCmdChangeSpeed)
			RegisterClientSDKProcessor(rspCmdPauseResume)
			RegisterClientSDKProcessor(rspCmdAudio)


			RegisterClientSDKProcessor(rspCmdUpdateConfig)
			RegisterClientSDKProcessor(rspCmdGetConfigdata)
			RegisterClientSDKProcessor(rspCmdSetWorkPath)
			RegisterClientSDKProcessor(rspCmdAutoChat)
			RegisterClientSDKProcessor(rspCmdGeneralOctree)
			RegisterClientSDKProcessor(rspCmdStartActionMode)
			RegisterClientSDKProcessor(rspCmdEndActionMode)
			RegisterClientSDKProcessor(rspCmdDeleteCurrRouteOrPath)
			RegisterClientSDKProcessor(rspCmdGetCurrRouteAndPath)
			RegisterClientSDKProcessor(rspCmdSignPoint)


			RegisterClientSDKProcessor(notifyBaseStatus)
			RegisterClientSDKProcessor(notifyAlarmInfo)
			RegisterClientSDKProcessor(notifyAlarmNewFace)
			RegisterClientSDKProcessor(notifyAlarmFire)


			RegisterClientSDKProcessor(rspStreamAddr)
			RegisterClientSDKProcessor(rspAudioTalk)
			RegisterClientSDKProcessor(notifyStreamAddr)
			//RegisterClientSDKProcessor(DDRCommProto::notifyAlarm)
			RegisterClientSDKProcessor(rspFileAddress)



			RegisterClientSDKProcessor(rspStreamServiceInfo)
			RegisterClientSDKProcessor(notifyStreamServiceInfoChanged)
			RegisterClientSDKProcessor(rspCmdGetThermalParams)
			RegisterClientSDKProcessor(rspCmdSetThermalParams)
			RegisterClientSDKProcessor(rspCmdAddFace)
			RegisterClientSDKProcessor(rspCmdFaceDataBaseOrder)


			RegisterClientSDKProcessor(rspCmdGetFaceParams)
			RegisterClientSDKProcessor(rspCmdSetFaceParams)
	}


	ClientSDKDispatcher::~ClientSDKDispatcher()
	{
	}
}