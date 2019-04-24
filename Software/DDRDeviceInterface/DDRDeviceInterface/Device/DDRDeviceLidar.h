#ifndef __DDR_DEVICE_LIDAR_TYPE_BASE_H__
#define __DDR_DEVICE_LIDAR_TYPE_BASE_H__

#include "DDRDeviceTypeBase.h"
#include "DDRDeviceCommData.h"

namespace DDRDevice
{
	class LidarBase :public DeviceTypeBase<LidarData, LidarInfo>, public DevicePtrContainer
	{
	public:
		bool Init(LidarInfo info);
		bool DeInit();
		std::shared_ptr<LidarData> GetData();
		std::string GetName();

	private:
		int m_nLidarID;
		
		std::string m_strIp;
		std::string m_strName;
	};
}

#endif //  __DDR_DEVICE_LIDAR_TYPE_BASE_H__

