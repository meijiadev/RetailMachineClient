// DDRDeviceInterface.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "DDRDeviceInterface.h"
#include "DeleteUnwrapper.h"
#include "Device/DDRDeviceCommData.h"
#include <map>

namespace DDRDevice {

	int g_cntPPOACModule = 0;
	static int g_VerPPOAC = 10003;

	class DDRDeviceImpl : public DefaultDelete<DDRDeviceInterface> 
	{
		bool AddOneLidar(DDR_IN char* strLidarIP, DDR_OUT int *nLidarID)
		{
			return false;
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
		//std::map<EnDeviceType, xxxxx> m_mapDevice;

		//m_mapDevice.insert(pair<int, string>(1, "student_one"));
	};

	DDRDeviceInterface* _stdcall _createDDRDeviceModule() {
		if (0 == g_cntPPOACModule) {
			++g_cntPPOACModule;
			return (new DDRDeviceImpl);
		}
		else { return nullptr; }
	}
}



