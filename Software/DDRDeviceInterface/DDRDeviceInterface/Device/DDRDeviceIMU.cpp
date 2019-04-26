#include "DDRDeviceIMU.h"
#include "DeviceManager.h"

namespace DDRDevice
{
	bool IMUBase::Init(IMUInfo info)
	{
		return true;
	}

	bool IMUBase::DeInit()
	{
		return true;
	}

	std::shared_ptr<IMUData> IMUBase::GetData()
	{
		std::shared_ptr<IMUData> ptr = std::make_shared<IMUData>();
		if (DDRDevicedManager::GetInstance()->GetIMUData(*ptr))
		{
			return ptr;
		}
		ptr.reset();
		return nullptr;
	}

	IMUInfo IMUBase::GetDeviceInfo()
	{
		IMUInfo info;
		return info;
	}

	bool IMUBase::SendData(IMUData data)
	{
		DDRDevicedManager::GetInstance()->SetIMUTargetTemp((float)data.m_sIMUTempBy100 / 100.0f);
		return true;
	}
}


