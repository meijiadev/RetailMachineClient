#ifndef __DDR_DEVICE_GNSS_STATUS_TYPE_BASE_H__
#define __DDR_DEVICE_GNSS_STATUS_TYPE_BASE_H__

#include "DDRDeviceTypeBase.h"
#include "DDRDeviceCommData.h"

namespace DDRDevice
{
	class GnssBase :public DeviceTypeBase<GnssData, GnssInfo> , public DevicePtrContainer
	{
	public:
		bool Init(GnssInfo info);
		bool DeInit();
		std::shared_ptr<GnssData> GetData();
		GnssInfo GetDeviceInfo();
		bool SendData(GnssInfo data);
	private:
	};
}

#endif //  __DDR_DEVICE_GNSS_STATUS_TYPE_BASE_H__

