#include "DDRDeviceEnvironment.h"
#include "DeviceManager.h"

namespace DDRDevice
{
	bool EnvironmentBase::Init(EnvironmentInfo info)
	{
		return true;
	}

	bool EnvironmentBase::DeInit()
	{
		return true;
	}

	std::shared_ptr<EnvironmentData> EnvironmentBase::GetData()
	{
		std::shared_ptr<EnvironmentData> ptr = std::make_shared<EnvironmentData>();
		if (DDRDevicedManager::GetInstance()->GetEnvironmentData(*ptr))
		{
			std::shared_ptr<EnvironmentData> ptr = std::make_shared<EnvironmentData>();
			return ptr;
		}
		ptr.reset();
		return nullptr;
	}
	EnvironmentInfo EnvironmentBase::GetDeviceInfo()
	{
		EnvironmentInfo info;
		return info;
	}

	bool EnvironmentBase::SendData(EnvironmentInfo data)
	{
		return true;
	}

}


