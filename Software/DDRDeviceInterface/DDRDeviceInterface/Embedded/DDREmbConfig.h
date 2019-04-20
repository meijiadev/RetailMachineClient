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
		uint16_t header;			//֡ͷ
		uint8_t func_code;			//������, ��4λΪ0001������λ(3~0)�ֱ�ΪWIFI���ڽ����ʶ����ͣ�򿪱�ʶ��������ϴ�����ʶ�ͳ������ϴ�����ʶ
		int16_t	acc_x;				//���ٶȼƶ���x
		int16_t	acc_y;				//���ٶȼƶ���y
		int16_t	acc_z;				//���ٶȼƶ���z
		int16_t	gyro_x;				//�����Ƕ���x
		int16_t	gyro_y;				//�����Ƕ���y
		int16_t	gyro_z;				//�����Ƕ���z
		int16_t imu_temp100;		//IMU�¶����϶ȳ���100
		int16_t rf_left_motor;		//����ת�٣���׼�V2.6���Ƶ���Ϊ����Ƶ�ʳ���10�����Ƶ��̼�֮�����ΪRPM����100��
		int16_t rf_right_motor; 	//�ҵ��ת��
		uint32_t timestamp;			//ʱ���
		uint8_t check_sum;			//�ۼ�У���
		uint16_t tail;				//֡β
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
		uint16_t header;			//֡ͷ
		uint8_t func_code;			//������, ��4λΪ0010������λ(3~0)�ֱ�ΪWIFI���ڽ����ʶ����ͣ�򿪱�ʶ��������ϴ�����ʶ�ͳ������ϴ�����ʶ
		int16_t	env_temp;			//�����¶����϶ȳ���100
		uint16_t env_humidity;		//����ʪ�Ȱٷֱȳ���100����5123��ʾ51.23%��
		uint16_t batt_status;		//���״̬
		uint16_t batt_voltage;		//��ص�ѹ����λ������
		uint16_t batt_percentage;	//��ص����ٷֱ�(��5123��ʾ51.23%)
		int16_t	batt_temp;			//����¶ȣ������ֽڸ���ʾһ���������¶ȣ���λ���϶�
		uint16_t env_PMindex;		//΢С����Ũ��(��101��ʾPM2.5ָ��Ϊ101)
		uint8_t rcg_RcgMode;        //�Ƿ����Զ����ģʽ�����Ϊ�ǣ������͵��������ݱ����ԡ�
		uint8_t rcg_RcgState;       //�Զ����״̬���ӵ�4λ����0λ���δ����Ƿ��Ѿ��ڳ�硢�Ƿ��Ѿ���ɶ�׼��������Ƿ��յ����к����Ƿ��յ����Һ����Ƿ��յ�
		uint16_t reserved;
		uint32_t timestamp;			//ʱ���
		uint8_t check_sum;			//�ۼ�У���
		uint16_t tail;				//֡β
	}	ENVIRONMENT_BATTERY_RECHARGING_DATA_FRAME;
#pragma pack ()

	// Ranging frames
	const int MB_POS_RNGRAME_GROUPNO_U8 = 3;
	const int MB_POS_RNGRAME_RANGINGDATA_HEADER_U16 = 4;
	const int MB_POS_RNGRAME_TRIGGER_STAT_HEADER_U8 = 20;
}

#endif // __DDR_EMBEDDED_CONFIGURATIONS_H_INCLUDED__
