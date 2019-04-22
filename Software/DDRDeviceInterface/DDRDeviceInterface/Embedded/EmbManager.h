#ifndef __DDR_EMBEDDED_MANAGER_H_INCLUDED__
#define __DDR_EMBEDDED_MANAGER_H_INCLUDED__

#include "EmbServer.h"
#include "EmbUser.h"
#include "Device/DDRDeviceCommData.h"

namespace DDRDevice
{
	class DDREmbeddedManager
	{
		DDREmbeddedManager();
	public:
		static DDREmbeddedManager * GetInstance();
		~DDREmbeddedManager();
		bool GetIMUData(IMUData &data);
		bool GetMotorData(MotorData &data);
	protected:
		DDRDrivers::DDREmbeddedServer m_EmbServer;
		DDRDrivers::EmbUser m_EmbUser;
	};
}

#endif // __DDR_EMBEDDED_MANAGER_H_INCLUDED__