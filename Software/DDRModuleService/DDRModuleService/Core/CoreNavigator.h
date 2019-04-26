#ifndef __DDR_FUSION_NAVIGATION_CORE_SLAM_FUNC_H_INCLUDED__
#define __DDR_FUSION_NAVIGATION_CORE_SLAM_FUNC_H_INCLUDED__

#include <string>
#include "DataFormat.h"

namespace DDRNavigator {

// 获取版本号
int GetCurrentVersionNo();

// 获取版本描述
std::string GetCurrentVersionDescription();

//主模式
enum class MODE {
	IDLING, // 待命模式
	COLLECTION, // 采集建图模式
	AUTO // 自动行走模式
};

//次级模式
enum class SUBMODE {
	IDLING_EXCEPTION, // 待命模式异常
	COLLECTION_EXCEPTION, // 采集模式异常
	AUTO_EXCEPTION, // 自动模式异常

	IDLING_STILL, // 空闲模式-静止
	IDLING_MOVING, // 空闲模式-运动

	COLLECTION_COLLECTING, // 采集模式-采集中
	COLLECTION_GENERATING, // 采集模式-地图生成中
	COLLECTION_FINISHED, // 采集模式-建图完成

	AUTO_RELOC, // 自动模式-重定位中
	AUTO_MOVING, // 自动模式-运动中
	AUTO_STOPPED, // 自动模式-停止(由调用者或避障模块要求停止)
	AUTO_ENDPOINT_UNREACHABLE, // 自动模式-路径终点无法到达
	AUTO_FINISHED // 自动模式-成功到达终点
};

// 自动运动状态
enum class AUTOMOVINGSTATUS {
	STATIC_NORMAL, // 静态避障-正常行走
	STATIC_SLOWDOWN, // 静态避障-减速避障
	STATIC_STOPPED, // 静态避障-停止
	DYNAMIC_NORMAL, // 动态避障-正常行走
	DYNAMIC_DETOUR, // 动态避障-绕行
	DYNAMIC_NOWAYOUT, // 动态避障-围困
	DYNAMIC_VIRTUAL_COLLISION, // 动态避障-遭遇虚拟障碍
	PP_TURNING, // 原地旋转
	PP_TEMP_SEGMENT // 执行临时路径(此过程静态避障)
};

// 各模式下的异常状态
enum class MODEEXCEPTIONS {
	COLLECTION_EXISTINGROUTE, // 试图采集的地图名已存在
	COLLECTION_FILEIOERROR, // 采集模式下文件IO错误
	AUTO_NONEXISTINGROUTE, // 自动模式选择的地图不存在
	AUTO_RELOCFAILED, // 自动模式重定位失败
	AUTO_FILEIOERROR, // 自动模式下文件IO错误
	AUTO_PP_EXCEPTION, // 自动模式下路径规划模块异常
	AUTO_OA_EXCEPTION // 自动模式下避障模块异常
};

class CoreNavigator
{
public:
	CoreNavigator();
	virtual ~CoreNavigator();
	
	// 获取当前模式
	MODE GetMode() const;
	// 获取当前次级模式
	SUBMODE GetSubMode() const;
	// 获取上一次异常
	MODEEXCEPTIONS GetLastError() const;
	// 清理异常状态并回到待命模式
	void ClearExceptions();
	// 获取当前位姿
	bool GetCurrentPose(DDRData::Pose&) const;
	bool GetCurrentPose(DDRData::Pose_d&) const;

	/*
	更新机器人运动状态和位姿
			1. Update_start()
			2. FeedSensorData() (使用各种重载喂入不同类型传感器数据)
			3. Update_finish()
	警告: 在且仅在一对Update_Start()和Update_finish()之间(下称"此空间")
	可以安全调用FeedSensorData(). 而获取模式和位姿、获取控制指令、开始双
	目捕获等操作仅在此空间之外有效, 在此空间内调用可能造成异常或错误结果.
	*/
	bool Update_Start();
	bool Update_Finish();
	// 喂入角速度数据(通常由陀螺仪获取)
	bool FeedSensorData(const DDRData::AngularVelocity&);
	// 喂入角速度数据(通常由轮机获取)
	bool FeedSensorData(const DDRData::LinearVelocity&);
	// 喂入2D点云(通常由平面激光雷达获取)
	bool FeedSensorData(const DDRData::LidarScan_2D&);
	// 喂入双目图像(通常由双目相机获取)
	bool FeedSensorData(const DDRData::StereoImages&);

	// 所有特定模式函数统一返回值为bool型, 表示此次调用是否奏效

	// ========================= collection mode ========================= 
	// 开始采集模式, routeName为将要保存的地图名称
	bool COLLECTION_Start(const std::string &routeName);
	// 通知采集模式刚刚开始了一次双目图像捕捉
	bool COLLECTION_BeginStereoCap();
	// 通知采集模式当前轮次的双目捕捉结果, bSuccess表示是否成功
	bool COLLECTION_EndStereoCap(bool bSuccess);
	// 获取最近的雷达点云(世界坐标)填入引用vector中
	bool COLLECTION_GetLidarPts(std::vector<DDRData::Position>&);
	// 获取最近的视觉点云(世界坐标)放入引用vector中
	bool COLLECTION_GetVisualFeatures(std::vector<DDRData::Position>&);
	// 结束采集模式. 采集模式将进入地图生成阶段(SUBMODE::COLLECTION_GENERATING),
	// 完成后进入SUBMODE::COLLECTION_FINISHED
	bool COLLECTION_Finish();
	// 采集模式下SUBMODE::COLLECTION_FINISHED表示采集建图已全部完成, 返回待命模式
	bool COLLECTION_Back2Idling();
	// ========================= collection mode ========================= 

	// ============================== auto mode ============================== 
	// 开始自动模式, bDynamicOA是否为动态避障模式(否则为静态避障),
	// routeName为要载入的地图名称, pathFileName为要运行的路径文件名
	bool AUTO_Start(bool bDynamicOA, const std::string &routeName,
		            const std::string &pathFileName);
	// 切换动态/静态避障模式
	bool AUTO_SwitchOAMode(bool bDynamicOA);
	// 通知自动模式刚刚开始了一次双目图像捕捉
	bool AUTO_BeginStereoCap();
	// 通知自动模式当前轮次的双目捕捉结果, bSuccess表示是否成功
	bool AUTO_EndStereoCap(bool bSuccess);
	// 获取自动模式下的自动运动状态填入引用中
	bool AUTO_GetMovingStatus(AUTOMOVINGSTATUS&);
	// 获取自动模式下的控制指令(包含线速度指令和角速度指令)
	bool AUTO_GetControl(DDRData::LinearVelocity&,
		                 DDRData::AngularVelocity&);
	// 获取最近匹配成功的雷达点云(世界坐标)填入引用vector中
	bool AUTO_GetMatchedLidarPts(std::vector<DDRData::Position>&);
	// 获取最近匹配成功的视觉点云(世界坐标)填入引用vector中
	bool AUTO_GetMatchedVisualFeatures(std::vector<DDRData::Position>&);
	// 计算从上次位姿修正(激光或视觉)至今的时间, 单位毫秒
	bool AUTO_GetTimeElapsedSinceLastPC(DDRData::U64_TIMESTAMP&);
	// 结束自动模式. 除异常外所有次级模式下都可以直接结束自动模式返回待命模式
	bool AUTO_Back2Idling();
	// ============================== auto mode ============================== 

private:
	struct IMPL;
	IMPL *m_pImp;
};

}

#endif // __DDR_FUSION_NAVIGATION_CORE_SLAM_FUNC_H_INCLUDED__

