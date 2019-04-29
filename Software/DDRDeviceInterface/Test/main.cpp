#include <iostream>
#include <windows.h>
#include <mutex>
#include <thread>

#include "../DDRDeviceInterface/DDRDeviceInterface.h"
#include "../DDRDeviceInterface/DDRDeviceFactory.h"

#include "opencv2/opencv.hpp"

#ifdef _DEBUG
#pragma comment (lib, "../x64/Debug/DDRDevice_x64d.lib")
#else
#pragma comment (lib, "../x64/Release/DDRDevice_x64r.lib")
#endif

bool g_bGotoMain = false;
std::mutex g_mutex;

void showLidarData(DDRDevice::LidarData &data)
{
	auto & lidar = data.m_Data;
	//std::cout << "lidar point count:" << lidar.pts.size() << std::endl;

	cv::Mat black_img = cv::Mat::zeros(cv::Size(1600, 1024), CV_8UC3);//黑色图像

	for (int i = 0; i < lidar.pts.size(); i++)
	{
		auto lidarpoint = lidar.pts[i];
		lidarpoint.angle;

		double theta, distance;
		theta = lidarpoint.angle;
		distance = lidarpoint.distance;
		theta *= (3.1415926f / 180);
		double x = distance * (cos(theta));
		double y = distance * (sin(theta));
		cv::Point2f pt;

		pt.x = -y + 800;
		pt.y = -x + 600;

		if (pt.x < black_img.cols && pt.y < black_img.rows)
		{
			cv::circle(black_img, pt, 3, cv::Scalar(255, 255, 255), 2);
		}
	}

	cv::imshow("LidarData", black_img);
	cv::waitKey(10);
}

bool GetGotoMain()
{
	bool bret = false;
	g_mutex.lock();
	bret = g_bGotoMain;
	if (bret)
	{
		g_bGotoMain = false;
	}
	g_mutex.unlock();
	return bret;
}

void lidarTest(void *params)
{
	std::cout << "Lidar test begin ...\n";
	std::shared_ptr<DDRDevice::DDRDeviceInterface> ptr = DDRDevice::CreateDDRDeviceModule();

	if (!ptr.get())
	{
		return;
	}

	std::cout << "Device Version:" << ptr->GetDeviceVersion().c_str() << " Date:" << ptr->GetDeviceDate().c_str() << std::endl;

	std::string strLidarName("LidarYoung");
	ptr->AddDevice(DDRDevice::en_DeviceLidar, strLidarName);
	
	auto lidar = ptr->GetPtr(DDRDevice::en_DeviceLidar, strLidarName);

	DDRDevice::LidarBase* pData = (DDRDevice::LidarBase*)(lidar);
	DDRDevice::LidarInfo info;
	info.m_strIp = "192.168.0.82";
	info.m_strName = strLidarName;

	if (pData->Init(info))
	{
		std::cout << "Init lidar success ...\n";
		while (1)
		{
			auto lidarData = pData->GetData();
			if (lidarData.get())
			{
				showLidarData(*lidarData);
			}

			::Sleep(20);

			if (GetGotoMain())
			{
				pData->DeInit();
				break;
			}
		}
		
	}
	else
	{
		std::cout << "Init lidar failed ...\n";
	}

	ptr->RemoveDevice(DDRDevice::en_DeviceLidar);

	std::cout << "Lidar test end ...\n";
}

int KeyTest() {
	HANDLE keyboard = GetStdHandle(STD_INPUT_HANDLE);
	DWORD dw, num;
	if (!::GetNumberOfConsoleInputEvents(keyboard, &num) || num == 0) {
		return 0;
	}
	for (int i = 0; i < (int)num; ++i) {
		INPUT_RECORD input;
		::ReadConsoleInputA(keyboard, &input, 1, (LPDWORD)(&dw));
		if (input.EventType == KEY_EVENT && !input.Event.KeyEvent.bKeyDown) {
			return (int)(input.Event.KeyEvent.wVirtualKeyCode);
		}
	}
	return 0;
}

void EmbTest(void *)
{
	std::cout << "Emb test Begin ...\n";
	std::shared_ptr<DDRDevice::DDRDeviceInterface> ptr = DDRDevice::CreateDDRDeviceModule();
	if (!ptr.get())
	{
		return;
	}

	std::cout << "Device Version:" << ptr->GetDeviceVersion().c_str() << " Date:" << ptr->GetDeviceDate().c_str() << std::endl;


	// IMU test
	std::string strImuName("IMU");
	if (ptr->AddDevice(DDRDevice::en_DeviceIMU, strImuName))
	{
		auto imuDevice = ptr->GetPtr(DDRDevice::en_DeviceIMU, strImuName);
		DDRDevice::IMUBase* pData = (DDRDevice::IMUBase*)(imuDevice);
		DDRDevice::IMUInfo info;
		info.m_strName = strImuName;
		info.m_fIMUTemp = 55.0f;
		pData->Init(info);

		// 接着还需要设定IMU到指定温度。 比较费时
	}
	


	// motor test
	std::string strMotorName("MotorName");
	if (ptr->AddDevice(DDRDevice::en_DeviceMotor, strMotorName))
	{
		auto imuMotor = ptr->GetPtr(DDRDevice::en_DeviceMotor, strMotorName);
		DDRDevice::MotorBase* pDeviceMotor = (DDRDevice::MotorBase*)(imuMotor);
		if (pDeviceMotor)
		{
			DDRDevice::MotorInfo infoMotor;
			infoMotor.m_dLeftRadius = 2; // 初始化轮机
			infoMotor.m_dRightRadius = 11;
			infoMotor.m_dReading2AR_gyro = 11;
			infoMotor.m_dReading2AR_wheel = 11;
			infoMotor.m_dWheelBase = 11;
			infoMotor.m_strName = strMotorName;
			pDeviceMotor->Init(infoMotor);
		}
	}

	// en_DeviceControlMoveNormal test
	std::string strControlName("NormalControlName");
	if (ptr->AddDevice(DDRDevice::en_DeviceControlMoveNormal, strControlName))
	{
		auto pControl = ptr->GetPtr(DDRDevice::en_DeviceControlMoveNormal, strControlName);
		DDRDevice::ControlMoveNormalBase* pDeviceConrtol = (DDRDevice::ControlMoveNormalBase*)(pControl);
		if (pDeviceConrtol)
		{
			DDRDevice::ControlMoveNormalInfo infoControl;
			pDeviceConrtol->Init(infoControl);


			while (1)
			{
				auto controlData = pDeviceConrtol->GetData();
				if (controlData.get())
				{
					// std::cout << "Ang:" << controlData->m_Ang.x_av << " Line:" << controlData->m_Lin.x_lv << std::endl;
				}

				DDRDevice::ControlMoveNormalData sendData;
				sendData.m_Ang.x_av = 0;
				sendData.m_Lin.x_lv = 0.5;
				pDeviceConrtol->SendData(sendData);

				if (GetGotoMain())
				{
					pDeviceConrtol->DeInit();
					break;
				}
			}
		}
	}

	// en_DeviceControlMoveNormal test
	std::string strEmbStatus("StatusName");
	if (ptr->AddDevice(DDRDevice::en_DeviceEmbStatus, strEmbStatus))
	{
		auto pEmbStatusPtr = ptr->GetPtr(DDRDevice::en_DeviceEmbStatus, strEmbStatus);
		DDRDevice::EmbStatusBase* pEmbStatus = (DDRDevice::EmbStatusBase*)(pEmbStatusPtr);
		if (pEmbStatus)
		{
			DDRDevice::EmbStatusInfo infoEmbStatus;
			pEmbStatus->Init(infoEmbStatus);
		}


		while (1)
		{
			auto pData = pEmbStatus->GetData();
			if (pData.get())
			{
				std::cout << "Batt:" << pData->m_sBattStat << " BattPercentageBy100:" << pData->m_sBattPercentageBy100 << std::endl;
			}

			if (GetGotoMain())
			{
				pEmbStatus->DeInit();
				break;
			}

			//DDRDevice::ControlMoveNormalData sendData;
			//sendData.m_Ang.x_av = 0;
			//sendData.m_Lin.x_lv = 0.5;
			//pDeviceConrtol->SendData(sendData);

			//if (GetGotoMain())
			//{
			//	pDeviceConrtol->DeInit();
			//	break;
			//}
		}
	}
	std::cout << "Emb test end ...\n";
}


bool Test() 
{
	while (1)
	{
		int c = KeyTest();
		switch (c)
		{
		case 'G':
		{
			std::cout << "Recv G ...\n";
			break;
		}
		case 'L':
		{
			std::cout << "Recv L ...\n";
			std::thread t2(lidarTest, nullptr);
			t2.detach();
			break;
		}
		case 'E':
		{
			std::cout << "Recv E ...\n";
			std::thread t2(EmbTest, nullptr);
			t2.detach();
			break;
		}
		case 'Z':
			std::cout << "Recv Z ...\n";
			break;
		case 'Q':
			g_mutex.lock();
			g_bGotoMain = true;
			g_mutex.unlock();
		default:
			break;
		}
	}
	return true;
}

int main()
{
	std::cout << "Main() +++\n";

	//// 大于INFO的会被输出
	//DDRFramework::Log::getInstance()->setLevel(DDRFramework::Log::Level::INFO);

	//// 设定输出的地方
	//DDRFramework::Log::getInstance()->setTarget(DDRFramework::Log::Target::STDOUT);

	//// 输出Log
	//LevelLog(DDRFramework::Log::Level::INFO, "Debug level %s", "ssssssss");


	Test();
	system("pause");
	return 0;
}