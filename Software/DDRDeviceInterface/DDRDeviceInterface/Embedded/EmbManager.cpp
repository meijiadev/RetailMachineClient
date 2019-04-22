#include "EmbManager.h"

namespace DDRDevice
{
	DDREmbeddedManager * DDREmbeddedManager::GetInstance()
	{
		static DDREmbeddedManager *ptr = nullptr;
		if (!ptr)
		{
			ptr = new DDREmbeddedManager();
		}
		return ptr;
	}

	DDREmbeddedManager::DDREmbeddedManager()
		: m_EmbServer(),
		m_EmbUser()
	{
	
	}

	DDREmbeddedManager::~DDREmbeddedManager()
	{
		m_EmbUser.Quit();
		m_EmbServer.Close();
	}

	bool DDREmbeddedManager::GetIMUData(IMUData &data)
	{
		return false;
	}
	
	bool DDREmbeddedManager::GetMotorData(MotorData &data)
	{
		return false;
	}
}
