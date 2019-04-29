#include "include/DDRDeviceMotor.h"
#include "DeviceManager.h"

namespace DDRDevice
{
	bool MotorBase::Init(MotorInfo info)
	{
		m_info.m_dLeftRadius = info.m_dLeftRadius;
		m_info.m_dRightRadius = info.m_dRightRadius;
		m_info.m_dReading2AR_gyro = info.m_dReading2AR_gyro;
		m_info.m_dReading2AR_wheel = info.m_dReading2AR_wheel;
		m_info.m_dWheelBase = info.m_dWheelBase;
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
		return m_info;
	}

	bool MotorBase::SendData(MotorData data)
	{
		return true;
	}

	std::string MotorBase::GetName()
	{
		return m_info.m_strName;
	}

}


