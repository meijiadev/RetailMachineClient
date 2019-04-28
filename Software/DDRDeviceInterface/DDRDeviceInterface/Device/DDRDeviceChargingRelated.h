#ifndef __DDR_DEVICE_DDRDevice_ChargingRelated_STATUS_TYPE_BASE_H__
#define __DDR_DEVICE_DDRDevice_ChargingRelated_STATUS_TYPE_BASE_H__

#include "DDRDeviceTypeBase.h"
#include "DDRDeviceCommData.h"

namespace DDRDevice
{
	class ChargingRelatedBase :public DeviceTypeBase<ChargingRelatedData, ChargingRelatedInfo> , public DevicePtrContainer
	{
	public:
		bool Init(ChargingRelatedInfo info);
		bool DeInit();
		std::shared_ptr<ChargingRelatedData> GetData();
		ChargingRelatedInfo GetDeviceInfo();
		bool SendData(ChargingRelatedInfo data);
	private:
	};
}

#endif //  __DDR_DEVICE_DDRDeviceChargingRelated_STATUS_TYPE_BASE_H__

