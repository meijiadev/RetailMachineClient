#ifndef __DDR_DEVICE_IMU_TYPE_BASE_H__
#define __DDR_DEVICE_IMU_TYPE_BASE_H__
#include "DDRDeviceTypeBase.h"
#include "DDRDeviceCommData.h"
namespace DDRDevice
{
	class IMUBase :public DeviceTypeBase<IMUData>, public DevicePtrContainer
	{
	public:
		bool Init();
		bool DeInit();
		std::shared_ptr<IMUData> GetData();
	};
}

#endif //  __DDR_DEVICE_IMU_TYPE_BASE_H__

