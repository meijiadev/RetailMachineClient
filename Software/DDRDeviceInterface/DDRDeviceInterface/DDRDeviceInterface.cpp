// DDRDeviceInterface.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "DDRDeviceInterface.h"
#include "DeleteUnwrapper.h"
#include "Device/DDRDeviceCommData.h"
#include <map>
#include "Device/DDRDeviceTypeBase.h"

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

		template <class T>
		T GetPtr(EnDeviceType type)
		{
			auto p = m_mapDevice[type];
			return  (T*)p;
		}


	private:
		std::map<EnDeviceType, DevicePtrContainer*> m_mapDevice;

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



