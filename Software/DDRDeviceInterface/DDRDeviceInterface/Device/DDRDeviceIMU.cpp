#include "DDRDeviceIMU.h"

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
		return nullptr;
	}
}


