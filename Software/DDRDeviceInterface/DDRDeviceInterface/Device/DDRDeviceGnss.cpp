#include "DDRDeviceGnss.h"
#include "DeviceManager.h"

namespace DDRDevice
{
	bool GnssBase::Init(GnssInfo info)
	{
		return true;
	}

	bool GnssBase::DeInit()
	{
		return true;
	}

	std::shared_ptr<GnssData> GnssBase::GetData()
	{
		std::shared_ptr<GnssData> ptr = std::make_shared<GnssData>();
		if (DDRDevicedManager::GetInstance()->GetGnssData(*ptr))
		{
			return ptr;
		}
		ptr.reset();
		return nullptr;
	}
	GnssInfo GnssBase::GetDeviceInfo()
	{
		GnssInfo info;
		return info;
	}

	bool GnssBase::SendData(GnssData data)
	{
		return true;
	}

	std::string GnssBase::GetName()
	{
		return m_info.m_strName;
	}

}


