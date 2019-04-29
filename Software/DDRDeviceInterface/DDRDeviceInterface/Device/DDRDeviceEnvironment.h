#ifndef __DDR_DEVICE_DDRDevice_Environment_STATUS_TYPE_BASE_H__
#define __DDR_DEVICE_DDRDevice_Environment_STATUS_TYPE_BASE_H__

#include "DDRDeviceTypeBase.h"
#include "DDRDeviceCommData.h"

namespace DDRDevice
{
	class EnvironmentBase :public DeviceTypeBase<EnvironmentData, EnvironmentInfo> , public DevicePtrContainer
	{
	public:
		bool Init(EnvironmentInfo info);
		bool DeInit();
		std::shared_ptr<EnvironmentData> GetData();
		EnvironmentInfo GetDeviceInfo();
		bool SendData(EnvironmentData data);
		std::string GetName();
	private:
		EnvironmentInfo m_info;
	};
}

#endif //  __DDR_DEVICE_DDRDeviceChargingRelated_STATUS_TYPE_BASE_H__

