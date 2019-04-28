#include <iostream>
#include "DataFormat.h"
#include "CoreNavigator.h"

int main()
{
	DDRData::AngularVelocity av;
	DDRData::LinearVelocity lv;
	DDRData::LidarScan_2D lidarScan;
	DDRData::StereoImages stereoImg;
	DDRData::Pose_d robotPose;
	DDRNavigator::CoreNavigator cn;

	DDRData::Pose_d p2;
	p2.orie.yaw = 0.3;
	p2.orie.pitch = 0.22;
	p2.orie.roll = -0.1;
	p2.pos.x = 0.1;
	p2.pos.y = -0.3;
	p2.pos.z = 0.5;
	DDRData::Pose p1 = p2;
	p1 = p2;

	while (1) {
		if (0/*用户要求退出*/) {
			break;
		}

		if (0/*!DeviceManager::GetGyro(&av)*/ &&
			0/*!DeviceManager::GetWheel(&lv)*/) {
			//Sleep(1);
			continue;
		}

		// 获得当前工作模式
		auto mode = cn.GetMode();
		auto subMode = cn.GetSubMode();

		lidarScan.Validate(false);
		/*DeviceManager::GetLidarScan(&lidarScan);*/

		// 如果上次双目捕获完成, 在Update_Start()和Update_Finish()对之前调用
		// EndStereoCap()正确通知自动模式/采集模式
		stereoImg.Validate(false);
		int lastCapStat = 0;/*DeviceManager::GetLastStereoCapStatus();*/
		if (0/*SUCCESSFUL == lastCapStat*/) {
			if (0/*DeviceManager::GetStereoImages(&stereoImg);*/) {
				if (DDRNavigator::MODE::AUTO == mode) {
					cn.AUTO_EndStereoCap(true);
				} else if (DDRNavigator::MODE::COLLECTION == mode) {
					cn.COLLECTION_EndStereoCap(true);
				}
			}
		} else if (0/*FAILURE == lastCapStat*/) {
			if (DDRNavigator::MODE::AUTO == mode) {
				cn.AUTO_EndStereoCap(false);
			} else if (DDRNavigator::MODE::COLLECTION == mode) {
				cn.COLLECTION_EndStereoCap(false);
			}
		} else if (0/*STILLBUSY == lastCapStat*/) {
			; // do nothing
		}

		if (0/*用户要求进入采集模式*/) {
			/* 调用DeviceManager做自检、自标定等基础工作 */
			std::string routeName = std::string(); //用户要求的新地图名
			cn.COLLECTION_Start(routeName);
		}

		if (0/*用户要求进入自动模式*/) {
			/* 调用DeviceManager做自检、自标定等基础工作 */
			std::string routeName = std::string(); //用户要求的行走地图名
			std::string pathFileName = std::string(); //用户要求的行走路径文件名
			if (0/*!isRouteNotExisting(str)*/) {
				cn.AUTO_Start(true, routeName, pathFileName);
			}
		}

		if (0/*用户要求结束采集模式*/) {
			cn.COLLECTION_Finish();
		}

		// 更新当前工作模式
		mode = cn.GetMode();
		subMode = cn.GetSubMode();

		if (DDRNavigator::MODE::AUTO == mode &&
			DDRNavigator::SUBMODE::AUTO_EXCEPTION == subMode &&
			DDRNavigator::MODEEXCEPTIONS::AUTO_RELOCFAILED == cn.GetLastError()) {
			std::cout << "警告! 自动模式重定位失败!" << std::endl;
			cn.ClearExceptions();
		}
		if (DDRNavigator::MODE::COLLECTION == mode &&
			DDRNavigator::SUBMODE::COLLECTION_EXCEPTION == subMode &&
			DDRNavigator::MODEEXCEPTIONS::COLLECTION_EXISTINGROUTE == cn.GetLastError()) {
			std::cout << "警告! 试图进入采集模式失败(同名地图已存在)!" << std::endl;
			cn.ClearExceptions();
		}
		if (DDRNavigator::MODE::AUTO == mode &&
			DDRNavigator::SUBMODE::AUTO_EXCEPTION == subMode &&
			DDRNavigator::MODEEXCEPTIONS::AUTO_NONEXISTINGROUTE == cn.GetLastError()) {
			std::cout << "警告! 试图进入自动模式失败(地图或路径不存在)!" << std::endl;
			cn.ClearExceptions();
		}
		if (DDRNavigator::MODE::COLLECTION == mode) {
			if (DDRNavigator::SUBMODE::COLLECTION_GENERATING == subMode) {
				std::cout << "提示: 采集停止, 正在建立地图..." << std::endl;
			} else if (DDRNavigator::SUBMODE::COLLECTION_FINISHED == subMode) {
				std::cout << "提示: 采集建图完成, 返回待命模式..." << std::endl;
				cn.COLLECTION_Back2Idling();
			}
		}
		if (DDRNavigator::MODE::AUTO == mode) {
			if (0/*用户要求结束自动行走*/) {
				/*DeviceManager::SendControl()零速度指令*/
				std::cout << "提示: 用户要求结束自动行走, 即将停止..." << std::endl;
				cn.AUTO_Back2Idling();
			} else if (DDRNavigator::SUBMODE::AUTO_FINISHED == subMode) {
				std::cout << "提示: 完成自动行走全部路径, 即将停止..." << std::endl;
				cn.AUTO_Back2Idling();
			} else if (DDRNavigator::SUBMODE::AUTO_ENDPOINT_UNREACHABLE == subMode) {
				std::cout << "警告: 自动行走终点无法抵达, 请检查!" << std::endl;
				cn.COLLECTION_Back2Idling();
			}
		}
		
		// 通过传感器数据更新机器人位姿和状态
		cn.Update_Start();
		do {
			cn.FeedSensorData(av);
			cn.FeedSensorData(lv);
		} while (0/*DeviceManager::GetGyro(&av)*/ &&
			     0/*DeviceManager::GetWheel(&lv)*/);
		cn.FeedSensorData(lidarScan);
		cn.FeedSensorData(stereoImg);
		cn.Update_Finish();

		DDRData::Pose_d pose;
		cn.GetCurrentPose(pose);
		if (DDRNavigator::MODE::AUTO == mode) {
			DDRData::AngularVelocity avCmd;
			DDRData::LinearVelocity lvCmd;
			cn.AUTO_GetControl(lvCmd, avCmd);
			/*DeviceManager::SendControl(lvCmd, avCmd);*/
			DDRData::U64_TIMESTAMP t;
			cn.AUTO_GetTimeElapsedSinceLastPC(t);
			std::cout << "当前处于自动模式, 位姿(X="
				<< pose.pos.x << ", Y="
				<< pose.pos.y << ", th="
				<< pose.orie.yaw * 180 / 3.14159265359
				<< "), 上次修正至今" << t
				<< "毫秒..." << std::endl;
			std::vector<DDRData::Position> lidarPts, visualPts;
			if (cn.AUTO_GetMatchedLidarPts(lidarPts)) {
				/*绘制匹配成功的激光点云*/
			}
			if (cn.AUTO_GetMatchedVisualFeatures(visualPts)) {
				/*绘制匹配成功的视觉特征点云*/
			}
		}

		// 在自动模式/采集模式下, 双目相机尽可能连续处于拍摄状态(可用于调
		// 整曝光). 每成功开始一次捕获时, 通知自动模式/采集模式做出运行对
		// 应逻辑.
		if ((DDRNavigator::MODE::AUTO == mode ||
			DDRNavigator::MODE::COLLECTION == mode) &&
			0/*!DeviceManger::IsStereoCamBusy()*/) {
			if (0/*DeviceManager::StartStereoCamCapture()*/) {
				if (DDRNavigator::MODE::AUTO == mode) {
					cn.AUTO_BeginStereoCap();
				} else {
					cn.COLLECTION_BeginStereoCap();
				}
			}
		}
	}

	return 0;
}