// DDRDeviceInterface.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "DDRDeviceInterface.h"
#include "DeleteUnwrapper.h"
#include "LidarSDK/AkuLidar.h"

namespace DDRDevice {

	int g_cntPPOACModule = 0;
	static int g_VerPPOAC = 10003;

	class DDRDeviceImpl : public DefaultDelete<DDRDeviceInterface> 
	{
		bool AddOneLidar(DDR_IN char* strLidarIP, DDR_OUT int *nLidarID)
		{
			return m_Lidar.AddOneLidar(strLidarIP, *nLidarID);
		}

		bool SendSpeed(DDR_IN float fSpeedA, DDR_IN float fSpeedL)
		{
			return true;
		}

		void _stdcall destroy()
		{
			delete this;
			--g_cntPPOACModule;
		}

	private:
		DDRDrivers::Lidar_AkuSense m_Lidar;
	};

	DDRDeviceInterface* _stdcall _createDDRDeviceModule() {
		if (0 == g_cntPPOACModule) {
			++g_cntPPOACModule;
			return (new DDRDeviceImpl);
		}
		else { return nullptr; }
	}
}



