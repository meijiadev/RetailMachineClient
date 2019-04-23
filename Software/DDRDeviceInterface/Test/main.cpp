#include <iostream>

#include "../DDRDeviceInterface/DDRDeviceInterface.h"
#include "../DDRDeviceInterface/DDRDeviceFactory.h"

#ifdef _DEBUG
#pragma comment (lib, "../x64/Debug/DDRDevice_x64d.lib")
#else
#pragma comment (lib, "../x64/Release/DDRDevice_x64r.lib")
#endif

int main()
{
	std::cout << "Main() +++\n";

	std::shared_ptr<DDRDevice::DDRDeviceInterface> ptr = DDRDevice::CreateDDRDeviceModule();
	ptr->AddDevice(DDRDevice::en_DeviceLidar);
	ptr->LidarTest(); // LidarTest();
	system("pause");
	return 0;
}