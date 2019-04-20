#ifndef __LIM_H__
#define __LIM_H__

#if defined (LINUX)

#ifndef STRUCTPACKED
#define STRUCTPACKED __attribute__((__packed__))
#endif

#elif defined(WIN32)||defined(WINNT)

#ifndef STRUCTPACKED
#define STRUCTPACKED
#endif

#endif

#define LIM_DT_IP      "237.1.1.200" 
#define LIM_DT_PORT    2111

/*
	Lidar Interaction Message (LIM) Structure:
	LIM ::= LIM_HEAD + Extended_Data
	Remark:
	1) Extended_Data : optional.
	
	Extended_Data:
	1) Lidar Measurement Data (LMD) Structure:
	   LMD ::= LMD_INFO + Data_Array
	   Remark:
	   1) Lidar mesaurement: distance value, unit: cm;
	   2) Data_Array: array of measurement; type: LMD_D_Type; length: LMD_INFO.nMDataNum.
*/

#define LIM_USER_PORT		2112				//Socket port for LIM.

/*
	Head info of LIM.
*/
#define LIM_TAG				0xF5EC96A5
#define	LIM_VER				0x01000000
#define LIM_DATA_LEN		4
typedef struct
{
	unsigned int			TAG;				//LIM_TAG.
	unsigned int			VER;				//Version.
	unsigned int			nCID;				//Application-specified network communication ID for ack LIM in multi-thread applications.
	unsigned int			nCode;				//LIM code.
	unsigned int			Data[LIM_DATA_LEN];	//LIM Data.
	unsigned int			nLIMLen;			//Length of LIM, including Extended_Data.
	unsigned int			CheckSum;			//Checksum including LIM_Head & Extended LIM Data.
}STRUCTPACKED LIM_HEAD;

#define LIM_CODE_LDBCONFIG          111
#define LIM_CODE_START_LDBCONFIG    110
#define LIM_CODE_STOP_LDBCONFIG     112
#define LIM_CODE_GET_LDBCONFIG      114

//LIM Code.
#define LIM_CODE_HB				10				//Heartbeat. Initiating: User-side.
#define LIM_CODE_HBACK			11				//Heartbeat acknowledgement. Initiating: Lidar-side.
#define LIM_CODE_LMD			901				//LMD. Initiating: Lidar-side. See LMD description.
#define LIM_CODE_LMD_RSSI       911
#define LIM_CODE_START_LMD		1900			//Start LMD transfering. Initiating: user-side.
#define LIM_CODE_STOP_LMD		1902			//Stop LMD transfering. Initiating: user-side.
#define LIM_CODE_ALARM			9001			//Lidar alarming. Initiating: Lidar-side. See ALARM/DISALARM description below.
#define LIM_CODE_DISALARM		9003			//Lidar disalarming. Initiating: Lidar-side. See ALARM/DISALARM description below.

#define LIM_CODE_FMSIG_QUERY    1912            //查询区域监测信号
#define LIM_CODE_FMSIG          1911            //区域监测信号

#define LIM_CODE_IOREAD         1920            //读取I/O端子状态
#define LIM_CODE_IOSET          1922            //设置I/O端子状态（仅限输出端子）
#define LIM_CODE_IOSET_RELEASE  1924            //解除设置I/O端子状态
#define LIM_CODE_IOSTATUS       1921            //IO状态回复应答报文

/*
	LMD description.
	Info of LMD.
*/
typedef struct
{
	unsigned int			nRange;				//Measuring range. Unit: cm.
	int						nBAngle;			//Begining angle. Unit: 1/1000 degree.
	int						nEAngle;			//Ending angle. Unit: 1/1000 degree.
	unsigned int			nAnglePrecision;	//Angle precision. Unit: 1/1000 degree.
	unsigned int			nRPM;				//Scanning frequency. Unti: RPM (round per minute).
	unsigned int			nMDataNum;			//Number of measurement data.
}STRUCTPACKED LMD_INFO;
/*
	LMD data type.
*/
typedef unsigned short LMD_D_Type;
typedef unsigned short LMD_D_RSSI_Type;

/*
	ALARM/DISALARM description.
	Alarming-code: LIM_HEAD.Data[0].
*/
#define LIM_DATA_ALARMCODE_INTERNAL			1			//Internal error.
#define LIM_DATA_ALARMCODE_Occluded			101			//Lidar is occluded or too dirty.
#define LIM_DATA_ALARMCODE_High_Temperature	1001		//Temperature too high.
#define LIM_DATA_ALARMCODE_Low_Temperature	1002		//Temperature too low.


#define	ULDINI_MAX_ATTR_STR_LEN	0x20

typedef struct 
{
	// Product Info
	char szType[ULDINI_MAX_ATTR_STR_LEN];
	char szManufacturer[ULDINI_MAX_ATTR_STR_LEN];
	char szReleaseDate[ULDINI_MAX_ATTR_STR_LEN];
	char szSerialNo[ULDINI_MAX_ATTR_STR_LEN];

	// Network Configuration
	char szMAC[ULDINI_MAX_ATTR_STR_LEN];
	char szIP[ULDINI_MAX_ATTR_STR_LEN];
	char szMask[ULDINI_MAX_ATTR_STR_LEN];
	char szGate[ULDINI_MAX_ATTR_STR_LEN];
	char szDNS[ULDINI_MAX_ATTR_STR_LEN];

	// Measurement Parameters
	int nMR;
	int nESAR;
	int nESA[2];
	int nSAR;
	int nSA[2];
	int nSAV;
	int nSAP;
	int nPF;

}ULDINI_Type;

#ifdef __cplusplus
extern "C"{
#endif

/*
	LIM utilities.
*/

/*
	LIM_CheckSum:
	Function: calculating the checksum of LIM.
	Return value: checksum of _lim.
	Remark:
	1) Checksum calculation for LIM creating & sending;
	2) Checksum checking for received LIM.
*/
unsigned int LIM_CheckSum(LIM_HEAD * _lim);

/*
LIM_ExData:
Function: memory address of Extended_Data of LIM.
Return value: pointer to Extended_Data.
Remark:
1) When a LIM has extended data, e.g., LMD LIM, use LIM_ExData to obtain the memory address;
2) LIM_HEAD and Extended_Data locate in continuous memory, so the address of Extended_Data equals to (void*)(_lim + 1).
*/
void* LIM_ExData(LIM_HEAD* _lim);

/*
LMD_Info & LMD_D:
Function: memory address of LMD_INFO & measurement data array in LIM.
Return value: pointer to LMD_INFO & measurement data array.
Remark:
1) For an LMD LIM, address of LMD_INFO equals to (LMD_INFO*)LIM_ExData(_lim);
2) The whole LMD LIM locates in continuous memory, so the address of measurement data equals to (LMD_D_Type*)(LMD_Info(_lim) + 1).
*/
LMD_INFO* LMD_Info(LIM_HEAD* _lim);
LMD_D_Type* LMD_D(LIM_HEAD* _lim);
LMD_D_RSSI_Type* LMD_D_RSSI(LIM_HEAD* _lim);

/*
LIM_Pack:
Function: compose a LIM.
return value: true / false
Remark:
1) composed LIM is returned in _lim.
*/
bool LIM_Pack(LIM_HEAD*& _lim, unsigned int _cid, unsigned int _code, unsigned int* _data = nullptr, unsigned int _ext_data_len = 0, void* _ext_data = nullptr);
/*
LIM_Copy:
Function: copy a LIM.
return value: true / false
Remark:
1) LIM copied from _slim is returned in _dlim.
*/
bool LIM_Copy(LIM_HEAD*& _dlim, LIM_HEAD* _slim);
/*
LIM_Release:
Function: release a LIM.
Remark:
1) memory of _lim is released, and _lim is cleared to NULL.
*/
void LIM_Release(LIM_HEAD*& _lim);

#ifdef __cplusplus
}
#endif

#endif
