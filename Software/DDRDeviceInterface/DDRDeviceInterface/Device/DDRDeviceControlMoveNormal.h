#ifndef __DDR_DEVICE_DDRDeviceControlMoveNormal_TYPE_BASE_H__
#define __DDR_DEVICE_DDRDeviceControlMoveNormal_TYPE_BASE_H__

#include "DDRDeviceTypeBase.h"
#include "DDRDeviceCommData.h"

namespace DDRDevice
{
	class ControlMoveNormalBase :public DeviceTypeBase<ControlMoveNormalData, ControlMoveNormalInfo> , public DevicePtrContainer
	{
	public:
		bool Init(ControlMoveNormalInfo info);
		bool DeInit();
		std::shared_ptr<ControlMoveNormalData> GetData();
		ControlMoveNormalInfo GetDeviceInfo();
		bool SendData(ControlMoveNormalData data);
		std::string GetName();
	private:
		ControlMoveNormalInfo m_info;
	};
}

#endif //  __DDR_DEVICE_MOTOR_TYPE_BASE_H__

