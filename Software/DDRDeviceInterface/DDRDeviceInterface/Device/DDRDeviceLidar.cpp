#include "DDRDeviceLidar.h"
#include <iostream>
namespace DDRDevice
{
	bool LidarBase::Init()
	{
		std::cout << "LidarBase::Init() +++\n";
		return true;
	}

	bool LidarBase::DeInit()
	{
		std::cout << "LidarBase::DeInit() +++\n";
		return true;
	}

	std::shared_ptr<LidarData> LidarBase::GetData()
	{
		return nullptr;
	}
}


