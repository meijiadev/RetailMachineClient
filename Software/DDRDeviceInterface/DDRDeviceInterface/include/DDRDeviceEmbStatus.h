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
		bool SendData(EmbStatusData data);
		std::string GetName();

		//bool Init(ControlMoveNormalInfo info);
		//bool DeInit();
		//std::shared_ptr<ControlMoveNormalData> GetData();
		//ControlMoveNormalInfo GetDeviceInfo();
		//bool SendData(ControlMoveNormalData data);
		//std::string GetName();

	private:
		EmbStatusInfo m_info;
	};
}

#endif //  __DDR_DEVICE_EMB_STATUS_TYPE_BASE_H__

