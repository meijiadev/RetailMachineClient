#include "DeviceManager.h"

namespace DDRDevice
{
	DDRDevicedManager * DDRDevicedManager::GetInstance()
	{
		static DDRDevicedManager *ptr = nullptr;
		if (!ptr)
		{
			ptr = new DDRDevicedManager();
		}
		return ptr;
	}

	DDRDevicedManager::DDRDevicedManager()
		: m_EmbServer(),
		m_EmbUser()
	{
	
	}

	DDRDevicedManager::~DDRDevicedManager()
	{
		m_EmbUser.Quit();
		m_EmbServer.Close();
	}

	bool DDRDevicedManager::GetIMUData(IMUData &data)
	{
		return false;
	}
	
	bool DDRDevicedManager::GetMotorData(MotorData &data)
	{
		return false;
	}

	bool DDRDevicedManager::AddOneLidar(char *ip, int &OutID)
	{
		return m_Lidar.AddOneLidar(ip, OutID);
	}
}
