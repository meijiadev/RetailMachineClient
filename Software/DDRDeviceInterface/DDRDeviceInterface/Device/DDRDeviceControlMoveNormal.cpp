#include "include/DDRDeviceControlMoveNormal.h"
#include "DeviceManager.h"

namespace DDRDevice
{
	bool ControlMoveNormalBase::Init(ControlMoveNormalInfo info)
	{
		return true;
	}

	bool ControlMoveNormalBase::DeInit()
	{
		return true;
	}

	std::shared_ptr<ControlMoveNormalData> ControlMoveNormalBase::GetData()
	{
		std::shared_ptr<ControlMoveNormalData> ptr = std::make_shared<ControlMoveNormalData>();
		if (DDRDevicedManager::GetInstance()->GetControlMoveNormalData(*ptr))
		{
			return ptr;
		}
		ptr.reset();
		return nullptr;
	}

	ControlMoveNormalInfo ControlMoveNormalBase::GetDeviceInfo()
	{
		return m_info;
	}

	bool ControlMoveNormalBase::SendData(ControlMoveNormalData data)
	{
		if (DDRDevicedManager::GetInstance()->SendControlMoveNormalData(data))
		{
			return false;
		}
		return true;
	}

	std::string ControlMoveNormalBase::GetName()
	{
		return m_info.m_strName;
	}

}


