#ifndef __DDR_DEVICE_LIDAR_TYPE_BASE_H__
#define __DDR_DEVICE_LIDAR_TYPE_BASE_H__

#include "DDRDeviceTypeBase.h"
#include "DDRDeviceCommData.h"

namespace DDRDevice
{
	class LidarBase :public DeviceTypeBase<LidarData, LidarInfo>, public DevicePtrContainer
	{
	public:
		LidarBase();
		~LidarBase();
		bool Init(LidarInfo info);
		bool DeInit();
		std::shared_ptr<LidarData> GetData();
		std::string GetName();
		LidarInfo GetDeviceInfo();
		bool SendData(LidarData data);

	private:
		LidarInfo m_info;
	};
}

#endif //  __DDR_DEVICE_LIDAR_TYPE_BASE_H__

