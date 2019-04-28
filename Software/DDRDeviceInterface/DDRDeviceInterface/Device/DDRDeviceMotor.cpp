#include "DDRDeviceMotor.h"
#include "DeviceManager.h"

namespace DDRDevice
{
	bool MotorBase::Init(MotorInfo info)
	{
		return true;
	}

	bool MotorBase::DeInit()
	{
		return true;
	}

	std::shared_ptr<MotorData> MotorBase::GetData()
	{
		std::shared_ptr<MotorData> ptr = std::make_shared<MotorData>();
		if (DDRDevicedManager::GetInstance()->GetMotorData(*ptr))
		{
			return ptr;
		}
		ptr.reset();
		return nullptr;
	}
	MotorInfo MotorBase::GetDeviceInfo()
	{
		MotorInfo info;
		return info;
	}

	bool MotorBase::SendData(MotorData data)
	{
		return true;
	}

}


