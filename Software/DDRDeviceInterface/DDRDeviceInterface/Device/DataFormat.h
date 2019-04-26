#ifndef __DDR_FUSION_NAVIGATION_SENSOR_AND_INTERMEDIATE_DATA_STRUCTURES_H_INCLUDED__
#define __DDR_FUSION_NAVIGATION_SENSOR_AND_INTERMEDIATE_DATA_STRUCTURES_H_INCLUDED__

#include <vector>
#include <chrono>

namespace DDRData {

	/*
	Without specification, all time stamps are in units of MILLSECONDS since the EPOCH (1970/1/1-0:0:0 UTC).
	If unavailable, the value is -1(0xFFFFFFFFFFFFFFFF).
	如无特别说明, 所有时间戳都表示从纪元时间(格林尼治时间1970年1月1日零点零时零分)起计算的毫秒数.
	-1(0xFFFFFFFFFFFFFFFF)表示此时间无效.
	*/
	typedef unsigned long long U64_TIMESTAMP;

	/*
	Classes/structs derived from TimeStampBase has a time stamp
	that indicates the START of the measurement
	所有派生类中timeStamp一项均指明测量 开始 时的时间戳.
	*/
	struct DataBase {
		DataBase() : bValid(false), timeStamp(-1) {}
		bool bValid;
		U64_TIMESTAMP timeStamp;
		void MAKE_IT_NOW() {
			timeStamp = std::chrono::duration_cast<std::chrono::milliseconds>
				(std::chrono::system_clock::now().time_since_epoch()).count();
			bValid = true;
		}
		void Invalidate() {
			bValid = false;
		}
	};

	/*
	3-axis definition
	All sensor data in 3-axis coordinate relative to the agent (robot).
	X		-		facing forward.
	Y		-		left.
	Z		-		upward.
	坐标轴定义
	所有3轴传感器数据都位于机器人坐标系.
	X		-		正前方
	Y		-		左侧
	Z		-		垂直向上
	*/

	/*
	A frame of 3-axis angular velocity measuments
	x_av		-		angular velocity on X axis (in radian/second)
	y_av		-		angular velocity on Y axis (in radian/second)
	z_av		-		angular velocity on Z axis (in radian/second)
	一帧三轴角速度测量结果 (通过陀螺仪获取并处理)
	x_av		-		X轴方向角速度, 单位 弧度/秒
	y_av		-		Y轴方向角速度, 单位 弧度/秒
	z_av		-		Z轴方向角速度, 单位 弧度/秒
	*/
	typedef struct : public DataBase {
		double x_av;
		double y_av;
		double z_av;
	} AngularVelocity;

	/*
	A frame of 3-axis linear velocity measurement
	x_lv		-		linear velocity on X axis (in meter/second)
	y_lv		-		linear velocity on Y axis (in meter/second)
	z_lv		-		linear velocity on Z axis (in meter/second)
	一帧三轴线速度测量结果 (通常通过电机码盘获取并处理)
	x_lv		-		X轴方向线速度, 单位 米/秒
	y_lv		-		Y轴方向线速度, 单位 米/秒
	z_lv		-		Z轴方向线速度, 单位 米/秒
	*/
	typedef struct : public DataBase {
		double x_lv;
		double y_lv;
		double z_lv;
	} LinearVelocity;

	/*
	A 2D lidar scan point
	angle		-		angle relative to lidar's heading direction, in DEGREEs and COUNTER-CLOCKWISE
	dist		-		distance to lidar's center, in CENTIMETERS. MINUS VALUE INDICATES VOID DETECTION.
	单个2D激光雷达点
	angle		-		扫描点相对于激光雷达正前方的角度, 单位 度, 朝向 逆时针
	dist		-		扫描点与激光雷达中心的距离, 单位 厘米. 负值表示未检测到任何物体
	*/
	typedef struct {
#pragma pack(1)
		float angle;
		float distance;
#pragma pack()
	} LidarPoint_2D;

	/*
	A lidar scan
	pts			-		points of the scan.
	scanTime	-		time consumed to collect this scan (in milliseconds).
	For this structure, it is recommended NOT to return/transfer raw variable.
	Prefer reference or preallocation.
	一帧激光雷达扫描
	pts			-		构成本帧扫描的激光雷达点
	scanTime	-		本帧扫描耗时, 单位 毫秒
	对于本结构, 建议不使用直接返回量, 而推荐使用引用方式获取.
	*/
	typedef struct _lidarScan_2d : public DataBase {
		std::vector<LidarPoint_2D> pts;
		int scanTime;
		_lidarScan_2d() : scanTime(-1) {}
	} LidarScan_2D;

	/*
	One monocular image.
	hgt			-		image height (in pixels). -1 if invalid.
	wdt			-		image width (in pixels). -1 if invalid.
	nCh			-		number of color channels (1 for graylevel, 3 for RGB)
	imgData		-		for each pixel, nCh byte values (0-255) are stored
						consecutively. Pixels are stored row by row.
	expTime		-		exposure time (in MICROSECONDS). -1 if invalid.
	For this structure, it is recommended NOT to return/transfer raw variable.
	Prefer reference or preallocation.
	单目图片
	hgt			-		图像高度 (像素数). -1表示无效.
	wdt			-		图像宽度 (像素数)
	nCh			-		颜色通道数 (灰度图为1, 24位RGB彩色图为3)
	imgData		-		图像数据本体. 每个像素包含nCh个字节值(0-255)用于记录其亮度
						(和色彩). 全部像素的存储逐行进行.
	expTime		-		曝光时间, 单位 微秒. -1表示无效.
	对于本结构, 建议不使用直接返回量, 而推荐使用引用方式获取.
	*/
	typedef struct _monoimage {
		int hgt;
		int wdt;
		std::vector<unsigned char> img;
		int expTime;
		_monoimage() : hgt(-1), wdt(-1), expTime(-1) {}
	} MonoImage;

	/*
	Stereo images
	双目图像
	*/
	typedef struct _stereoimages : public DataBase {
		_monoimage img[2];
	} StereoImages;

	/*
	World coordinate, relative to the pose of the robot starting mapping
	X		-		facing forward (meter)
	Y		-		left (meter)
	Z		-		upward (meter)
	yaw		-		yaw (radian)
	pitch	-		pitch (radian)
	roll	-		roll (radian)
	世界坐标系, 以机器人开始采集时朝向为准
	X		-		前方, 单位 米
	Y		-		左侧, 单位 米
	Z		-		垂直向上, 单位 米
	yaw		-		航向角, 单位 弧度
	pitch	-		俯仰角, 单位 弧度
	roll	-		翻滚角, 单位 弧度
	*/

	// 3D位置
	typedef struct _position_d {
#pragma pack(1)
		double x;
		double y;
		double z;
#pragma pack()
		_position_d() : x(0.0), y(0.0), z(0.0) {}
	} Position_d;
	typedef struct _position {
#pragma pack(1)
		float x;
		float y;
		float z;
#pragma pack()
		_position() : x(0.0f), y(0.0f), z(0.0f) {}
	} Position;

	// 3D姿态
	typedef struct _orie_d {
#pragma pack(1)
		double yaw;
		double pitch;
		double roll;
#pragma pack()
		_orie_d() : yaw(0.0), pitch(0.0), roll(0.0) {}
	} Orientation_d;

	typedef struct _orie {
#pragma pack(1)
		float yaw;
		float pitch;
		float roll;
#pragma pack()
		_orie() : yaw(0.0f), pitch(0.0f), roll(0.0f) {}
	} Orientation;

	// 6D位姿
	typedef struct {
		Position pos;
		Orientation orie;
	} Pose;
	typedef struct {
		Position_d pos;
		Orientation_d orie;
	} Pose_d;

}

#endif // __DDR_FUSION_NAVIGATION_SENSOR_AND_INTERMEDIATE_DATA_STRUCTURES_H_INCLUDED__, 