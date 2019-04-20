// DDRDeviceInterface.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "DDRDeviceInterface.h"
#include "DeleteUnwrapper.h"

namespace DDRDevice {

	int g_cntPPOACModule = 0;
	static int g_VerPPOAC = 10003;

	class DDRDeviceImpl : public DefaultDelete<DDRDeviceInterface> 
	{

		void _stdcall destroy()
		{
			delete this;
			--g_cntPPOACModule;
		}
	};

	DDRDeviceInterface* _stdcall _createDDRDeviceModule() {
		if (0 == g_cntPPOACModule) {
			++g_cntPPOACModule;
			return (new DDRDeviceImpl);
		}
		else { return nullptr; }
	}
}



