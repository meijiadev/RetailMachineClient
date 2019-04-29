#ifndef __DDR_DEVICE_IMU_TYPE_BASE_H__
#define __DDR_DEVICE_IMU_TYPE_BASE_H__
#include "DDRDeviceTypeBase.h"
#include "DDRDeviceCommData.h"
namespace DDRDevice
{
	class IMUBase :public DeviceTypeBase<IMUData, IMUInfo>, public DevicePtrContainer
	{
	public:
		bool Init(IMUInfo info);
		bool DeInit();
		std::shared_ptr<IMUData> GetData();
		IMUInfo GetDeviceInfo();
		bool SendData(IMUData);
		std::string GetName();
	private:
		IMUInfo m_info;
	};
}

#endif //  __DDR_DEVICE_IMU_TYPE_BASE_H__

