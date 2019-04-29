#ifndef __DDR_DEVICE_MOTOR_TYPE_BASE_H__
#define __DDR_DEVICE_MOTOR_TYPE_BASE_H__

#include "DDRDeviceTypeBase.h"
#include "DDRDeviceCommData.h"

namespace DDRDevice
{
	class MotorBase :public DeviceTypeBase<MotorData, MotorInfo> , public DevicePtrContainer
	{
	public:
		bool Init(MotorInfo info);
		bool DeInit();
		std::shared_ptr<MotorData> GetData();
		MotorInfo GetDeviceInfo();
		bool SendData(MotorData data);
		std::string GetName();
	private:
		MotorInfo m_info;
	};
}

#endif //  __DDR_DEVICE_MOTOR_TYPE_BASE_H__

