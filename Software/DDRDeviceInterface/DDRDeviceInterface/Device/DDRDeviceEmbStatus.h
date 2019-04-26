#ifndef __DDR_DEVICE_EMB_STATUS_TYPE_BASE_H__
#define __DDR_DEVICE_EMB_STATUS_TYPE_BASE_H__

#include "DDRDeviceTypeBase.h"
#include "DDRDeviceCommData.h"

namespace DDRDevice
{
	class EmbStatusBase :public DeviceTypeBase<EmbStatusData, EmbStatusInfo> , public DevicePtrContainer
	{
	public:
		bool Init(EmbStatusInfo info);
		bool DeInit();
		std::shared_ptr<EmbStatusData> GetData();
		EmbStatusInfo GetDeviceInfo();
		bool SendData(EmbStatusInfo data);
	private:
	};
}

#endif //  __DDR_DEVICE_EMB_STATUS_TYPE_BASE_H__

