#include "DDRDeviceChargingRelated.h"
#include "DeviceManager.h"

namespace DDRDevice
{
	bool ChargingRelatedBase::Init(ChargingRelatedInfo info)
	{
		return true;
	}

	bool ChargingRelatedBase::DeInit()
	{
		return true;
	}

	std::shared_ptr<ChargingRelatedData> ChargingRelatedBase::GetData()
	{
		std::shared_ptr<ChargingRelatedData> ptr = std::make_shared<ChargingRelatedData>();
		if (DDRDevicedManager::GetInstance()->GetChargingRelatedData(*ptr))
		{
			return ptr;
		}
		ptr.reset();
		return nullptr;
	}
	ChargingRelatedInfo ChargingRelatedBase::GetDeviceInfo()
	{
		ChargingRelatedInfo info;
		return info;
	}

	bool ChargingRelatedBase::SendData(ChargingRelatedData data)
	{
		//DDRDevicedManager::GetInstance()->SwitchRechargingStatus(data.m_bEnterCharging);
		return true;
	}


	std::string ChargingRelatedBase::GetName()
	{
		return m_info.m_strName;
	}
}


