
#ifndef __LIDAR_H__
#define __LIDAR_H__

#include "lim.h"

#define LIDAR_SDK_VERSION		"Ver1.21"	

class CLidarImpl;


/* 在class 和类名中间加上一个导出宏_declspec(dllexport)，表示将此类及其变量、方法成员都导出 */
#ifdef USE_LIDAR_SDK_LIBRARY
typedef void (*LIDAR_PROBE_CALLBACK)(int flag, ULDINI_Type* ldini,void *_data);  //雷达探测回调函数类型
typedef void(*LIDAR_DATA_CALLBACK)(int _id,void* _lim, void *_data);	//雷达数据回调函数类型
typedef void (*LIDAR_STATE_CALLBACK)(int _id, void* _lim, void *_data);	//雷达状态回调函数类型

class  _declspec(dllimport) CLidar
#else
class  _declspec(dllexport) CLidar
#endif
{
public:
	CLidar();
	~CLidar();

	void get_sdk_version(char *strVer);

	/************* 雷达探测接口函数 ********************/
	int open_lidar_probe();
	int close_lidar_probe();
	//注册探测雷达回调函数
	// 回调函数定义： typedef void (*PROBE_LIDAR_CALLBACK)(int flag, ULDINI_Type* ldini,void *_data);
	// _data是用户注册回调函数时提供的指针，SDK调用回调函数时，会把这个参数传递回给回调函数
	int reg_probe_callback(void *_func,void *_data);  

	/************* 雷达数据&状态接口函数 ********************/
	int get_id();  // 获取雷达ID
	//	向雷达发送LIM报文
	//	lim：LIM 报文指针
	//	len：LIM 报文长度，即LIM_HEAD.nLIMLen
	int send_lim(int id, void *lim, int len);

	int start_lidar_config(int id);
	int stop_lidar_config(int id);

	//获取雷达配置
	//	id : SDK分配的ID号
	//	bWaitFlag: 是否等待结果，=1，等待结果再返回；=0，直接返回，然后再回调函数中处理查询结果；
	// 成功返回LIDARSDK_ERR_SUCCESS,_ldcfg保存ULDINI_Type信息;失败返回LIDARSDK_ERR_FAIL
	int get_lidar_config(int id, ULDINI_Type* _ldcfg, bool bWaitFlag);  

	//查询报警区域组报警状态
	//	id : SDK分配的ID号
	//	_port : 要查询的区域组标号(0-15)
	//	bWaitFlag: 是否等待结果，=1，等待结果再返回；=0，直接返回，然后再回调函数中处理查询结果；
	// 成功，返回所查询的区域报警状态
	//		Bit0:告警信号状态
	//		Bit1:告警信号总状态
	//		Bit2 : 预警信号状态
	//		Bit3 : 预警信号总状态
	//		Bit4 : 注意信号状态
	//		Bit5 : 注意信号总状态
	//      Bit7 - Bit15 : 报警区号
	//		其他位: 0
	//	失败，返回LIDARSDK_ERR_FAIL
	int query_region(int id, int _port,bool bWaitFlag);

	// 读IO状态
	//		id : SDK分配的ID号
	//		bWaitFlag: 是否等待结果，=1，等待结果再返回；=0，直接返回LIDARSDK_ERR_SUCESS，然后再回调函数中处理IO的查询结果；
	// 成功返回IO状态
	//		bit0：OUT1
	//		bit1：OUT2
	//		bit2：OUT3
	//		bit3：OUT4
	//		bit4：IN1
	//		bit5：IN2
	//		bit6：IN3
	//		bit7：IN4
	// 失败返回LIDARSDK_ERR_FAIL
	int read_iostatus(int id, bool bWaitFlag);

	// 设置IO输出状态:此时，区域监测功能暂停通过I/O 输出端口输出监测信号
	//		id : SDK分配的ID号
	//		_port : 对应bit为1，表示设置对应的IO输出口
	//			bit0：OUT1
	//			bit1：OUT2
	//			bit2：OUT3
	//			bit3：OUT4
	//		_status : 0/1
	//		bWaitFlag: 是否等待结果，=1，等待结果再返回；=0，直接返回LIDARSDK_ERR_SUCESS，然后再回调函数获取设置的结果；
	// 成功返回设置的状态,需要用户验证是否与设置的一致
	// 失败返回LIDARSDK_ERR_FAIL
	int set_io_out(int id, int _port, int _status, bool bWaitFlag);

	// 释放IO输出设置：调用set_io_out()后，要调用release_io_out()释放IO输出端口(区域监测功能恢复通过I/O 输出端口输出监测信号)
	//		id : SDK分配的ID号
	//		bWaitFlag: 是否等待结果，=1，等待结果再返回；=0，直接返回LIDARSDK_ERR_SUCESS，然后再回调函数获取设置的结果；
	// 成功返回IO口状态,失败返回LIDARSDK_ERR_FAIL
	int release_io_out(int id, bool bWaitFlag);

	int open_lidar(char *ip, int port, int flag);  //打开雷达，成功，返回雷达ID（大于0），失败返回错误代码（小于0）
	int close_lidar(int id); //关闭雷达

	int reg_data_callback(int id, void *_func, void *_data); //注册雷达数据回调函数
	int reg_state_callback(int id, void *_func, void *_data); //注册雷达状态回调函数

private:
	CLidarImpl* m_pLidarImpl;

};

#endif


