#ifndef __DDR_DEVICE_LIDAR_TYPE_BASE_H__
#define __DDR_DEVICE_LIDAR_TYPE_BASE_H__

#include "DDRDeviceTypeBase.h"
#include "DDRDeviceCommData.h"

namespace DDRDevice
{
	class LidarBase :public DeviceTypeBase<LidarData>, public DevicePtrContainer
	{
	public:
		bool Init();
		bool DeInit();
		std::shared_ptr<LidarData> GetData();

	private:
	};
}

#endif //  __DDR_DEVICE_LIDAR_TYPE_BASE_H__

