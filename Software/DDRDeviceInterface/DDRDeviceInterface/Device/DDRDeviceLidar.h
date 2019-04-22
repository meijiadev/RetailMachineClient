#ifndef __DDR_DEVICE_LIDAR_TYPE_BASE_H__
#define __DDR_DEVICE_LIDAR_TYPE_BASE_H__

#include "DDRDeviceTypeBase.h"
#include "DDRDeviceCommData.h"
#include "LidarSDK/AkuLidar.h"

namespace DDRDevice
{
	class LidarBase :public DeviceTypeBase<LidarData>
	{
	public:
		bool Init();
		bool DeInit();
		std::shared_ptr<LidarData> GetData();

	private:
		DDRDrivers::Lidar_AkuSense m_Lidar;
	};
}

#endif //  __DDR_DEVICE_LIDAR_TYPE_BASE_H__

