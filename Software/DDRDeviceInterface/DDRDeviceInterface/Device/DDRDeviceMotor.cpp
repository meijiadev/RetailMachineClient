#include "DDRDeviceMotor.h"
#include "DeviceManager.h"

namespace DDRDevice
{
	bool MotorBase::Init()
	{
		return true;
	}

	bool MotorBase::DeInit()
	{
		return true;
	}

	std::shared_ptr<MotorData> MotorBase::GetData()
	{
		MotorData data;
		if (DDRDevicedManager::GetInstance()->GetMotorData(data))
		{
			std::shared_ptr<MotorData> ptr = std::make_shared<MotorData>();
			ptr->m_sLeftMotorSpeed = data.m_sLeftMotorSpeed;
			ptr->m_sRightMotorSpeed = data.m_sRightMotorSpeed;
			return ptr;
		}

		return nullptr;
	}
}


