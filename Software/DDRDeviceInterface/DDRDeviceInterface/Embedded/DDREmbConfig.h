#ifndef __DDR_EMBEDDED_CONFIGURATIONS_H_INCLUDED__
#define __DDR_EMBEDDED_CONFIGURATIONS_H_INCLUDED__

#include "CommonDef/Types.h"

namespace DDRDrivers
{
	const int EMBEDDED_MB_FRAME_LENGTH = 28;
	const BYTE EMBEDDED_MB_FRAME_HEADER0 = 0x55;
	const BYTE EMBEDDED_MB_FRAME_HEADER1 = 0xFF;
	const BYTE EMBEDDED_MB_FRAME_TAIL0 = 0xFF;
	const BYTE EMBEDDED_MB_FRAME_TAIL1 = 0xFE;

	const int MB_POS_FUNCCODE_U8 = 2;
	const int MB_POS_TIMESTAMP_U32 = 21;

	const BYTE MB_FRAMETYPE_IMUMOTOR = 0x01;
	const BYTE MB_FRAMETYPE_EBR = 0x02;
	const BYTE MB_FRAMETYPE_RANGING = 0x03;
	const BYTE MB_FRAMETYPE_GETVERSION = 0x0A;

	// within the functional code
	const int MB_POS_FRAMETYPE_4B = 4;
	const int MB_BPOS_STICKCONTROLLER_1B = 3;
	const int MB_BPOS_EMGSTOP_1B = 2;
	const int MB_BPOS_IRSTOP_1B = 1;
	const int MB_BPOS_USSTOP_1B = 0;

	// IMU/motor frames
	const int MB_POS_IMFRAME_ACCX_16 = 3;
	const int MB_POS_IMFRAME_ACCY_16 = 5;
	const int MB_POS_IMFRAME_ACCZ_16 = 7;
	const int MB_POS_IMFRAME_GX_16 = 9;
	const int MB_POS_IMFRAME_GY_16 = 11;
	const int MB_POS_IMFRAME_GZ_16 = 13;
	const int MB_POS_IMFRAME_IMUTEMP_16 = 15;
	const int MB_POS_IMFRAME_LEFT_MOTORSPEED_16 = 17;
	const int MB_POS_IMFRAME_RIGHT_MOTORSPEED_16 = 19;
	// equivalent frame struct
#pragma pack (1)
	typedef struct {
		uint16_t header;			//帧头
		uint8_t func_code;			//功能字, 高4位为0001，低四位(3~0)分别为WIFI串口接入标识、急停打开标识、红外避障触发标识和超声避障触发标识
		int16_t	acc_x;				//加速度计读数x
		int16_t	acc_y;				//加速度计读数y
		int16_t	acc_z;				//加速度计读数z
		int16_t	gyro_x;				//陀螺仪读数x
		int16_t	gyro_y;				//陀螺仪读数y
		int16_t	gyro_z;				//陀螺仪读数z
		int16_t imu_temp100;		//IMU温度摄氏度乘以100
		int16_t rf_left_motor;		//左电机转速（大白及V2.6自制底盘为换向频率乘以10；步科底盘及之后底盘为RPM乘以100）
		int16_t rf_right_motor; 	//右电机转速
		uint32_t timestamp;			//时间戳
		uint8_t check_sum;			//累加校验和
		uint16_t tail;				//帧尾
	}	IMU_MOTOR_DATA_FRAME;
#pragma pack ()

	// Env./Batt./Recharging frames
	const int MB_POS_EBRFRAME_ENVTEMP_16 = 3;
	const int MB_POS_EBRFRAME_ENVHUM_U16 = 5;
	const int MB_POS_EBRFRAME_BATTSTAT_U16 = 7;
	const int MB_POS_EBRFRAME_BATTVOLT_U16 = 9;
	const int MB_POS_EBRFRAME_BATTPERCENT_U16 = 11;
	const int MB_POS_EBRFRAME_BATTTEMP_16 = 13;
	const int MB_POS_EBRFRAME_ENVPMINDEX_U16 = 15;
	const int MB_POS_EBRFRAME_RCGMODE_U8 = 17;
	const int MB_POS_EBRFRAME_RCGSTAT_U8 = 18;
	const int MB_BPOS_EBRFRAME_RCGSTAT_ISCHARGING_1B = 4;
	const int MB_BPOS_EBRFRAME_RCGSTAT_ISDOCKED_1B = 3;
	const int MB_BPOS_EBRFRAME_RCGSTAT_RIGHTIR_1B = 2;
	const int MB_BPOS_EBRFRAME_RCGSTAT_MIDIR_1B = 1;
	const int MB_BPOS_EBRFRAME_RCGSTAT_LEFTIR_1B = 0;
	const int MB_BPOS_EBRFRAME_RESERVED_U16 = 19;
	// equivalent frame struct
#pragma pack (1)
	typedef struct {
		uint16_t header;			//帧头
		uint8_t func_code;			//功能字, 高4位为0010，低四位(3~0)分别为WIFI串口接入标识、急停打开标识、红外避障触发标识和超声避障触发标识
		int16_t	env_temp;			//环境温度摄氏度乘以100
		uint16_t env_humidity;		//环境湿度百分比乘以100（例5123表示51.23%）
		uint16_t batt_status;		//电池状态
		uint16_t batt_voltage;		//电池电压（单位毫伏）
		uint16_t batt_percentage;	//电池电量百分比(例5123表示51.23%)
		int16_t	batt_temp;			//电池温度，两个字节各表示一个测量点温度，单位摄氏度
		uint16_t env_PMindex;		//微小颗粒浓度(例101表示PM2.5指数为101)
		uint8_t rcg_RcgMode;        //是否处于自动充电模式。如果为是，环境和电池侦测数据被忽略。
		uint8_t rcg_RcgState;       //自动充电状态，从第4位至第0位依次代表：是否已经在充电、是否已经完成对准、左红外是否收到、中红外是否收到、右红外是否收到
		uint16_t reserved;
		uint32_t timestamp;			//时间戳
		uint8_t check_sum;			//累加校验和
		uint16_t tail;				//帧尾
	}	ENVIRONMENT_BATTERY_RECHARGING_DATA_FRAME;
#pragma pack ()

	// Ranging frames
	const int MB_POS_RNGRAME_GROUPNO_U8 = 3;
	const int MB_POS_RNGRAME_RANGINGDATA_HEADER_U16 = 4;
	const int MB_POS_RNGRAME_TRIGGER_STAT_HEADER_U8 = 20;
}

#endif // __DDR_EMBEDDED_CONFIGURATIONS_H_INCLUDED__
