#ifndef __LIA_COMMON_H__
#define __LIA_COMMON_H__

#if defined(WIN32)||(WINNT)

#pragma warning(disable : 4786)
#pragma warning(disable : 4996)

#ifndef STRUCTPACKED
#define STRUCTPACKED
#endif

#endif

#if defined LINUX

#ifndef STRUCTPACKED
#define STRUCTPACKED __attribute__((__packed__))
#endif

#endif



#define LIA_MAX_FIELD_SET_NUM     16 // Maximum field number which can be defined.
#define LIA_FIELD_TYPE_NUM        3  // FF/AF/ALF
#define MAX_BND_NODE_NUM          32


#define LIA_ALG_PE       0
#define LIA_ALG_BLK      1
#define LIA_ALG_CONTOUR  2
#define LIA_ALG_Reserved 3

enum emFSampleType
{
	FIELD_SAM_RECT   = 0,       // ����
	FIELD_SAM_ARC    = 1,       // Բ��
	FIELD_SAM_CIRCLE = 2,      // Բ
	FIELD_SAM_PLN    = 3,       // �����
	FIELD_SAM_SL     = 4,        // ��ѧϰ
	FIELD_SAM_Reserved=5
};

enum emFLineType
{
	FIELD_LINE_LS    = 0,       // �߶�
	FIELD_LINE_ARC   = 1,       // Բ��
	FIELD_LINE_CURVE = 2,        // ����
	FIELD_LINE_Reserved = 3
};

enum emFCCType
{
	FIELD_CC_OUT = 0,       // ������
	FIELD_CC_IN_CP = 1,       // ���Ƶ�
	FIELD_CC_IN_E = 2,        // �߽�
	FIELD_CC_IN   = 3,         // ������
	FIELD_CC_Reserved = 4
};

#pragma pack(push) //�������״̬
#pragma pack(4)//�趨Ϊ4�ֽڶ���
typedef struct
{
	int x;
	int y;
}STRUCTPACKED GPOINT;	//Unit: cm

typedef struct
{
	double cita;
	double rou;
	double x;
	double y;
}STRUCTPACKED PPoint;	//Unit: cm

// polar region
typedef struct
{
	double rou1;
	double rou2;
}STRUCTPACKED PRegion;	//Unit: cm

typedef struct
{
	int rou1;
	int rou2;
}STRUCTPACKED PRTRegion;	//Unit: cm


#define MAX_FIELD_CONN_NUM  2
typedef struct
{
	//int scp;
	int c1;
	int c2;
	int pregsize;
	PRegion preg[360];
}STRUCTPACKED PField;	//Unit: cm

typedef struct
{
	int ais[MAX_FIELD_CONN_NUM];
	int aie[MAX_FIELD_CONN_NUM];
	int pregsize;
	PRTRegion* preg;
}STRUCTPACKED PRTField;	//Unit: cm



typedef struct
{
	// alert & attention field param
	// RECT:  dx12, dy12����������Ԥ�����ļ����dx23, dy23Ԥ������ע�����ļ��
	// ARC: dx12,��������Ԥ�����ļ�� dx23Ԥ������ע�����ļ��;
	// Circle: dx12,��������Ԥ�����ļ�� dx23Ԥ������ע�����ļ��;
	// PLN:  dx12,��������Ԥ�����ļ�� dx23Ԥ������ע�����ļ��;
	// BG: dx12,��������Ԥ�����ļ�� dx23Ԥ������ע�����ļ��;
	double dx[LIA_FIELD_TYPE_NUM];
	double dy[LIA_FIELD_TYPE_NUM];
	double dx1[LIA_FIELD_TYPE_NUM];
	double dy1[LIA_FIELD_TYPE_NUM];
}STRUCTPACKED EF_AAA_Param;


typedef struct
{
	int pctrlptnum;
	PPoint pctrlpt[MAX_BND_NODE_NUM];
	emFLineType linetype[MAX_BND_NODE_NUM];

}STRUCTPACKED EF_Ctrl_Pt;


typedef struct
{
	int scp;

	emFSampleType fsType;

	double x;
	double y;
	double r1;
	double r2;
	int c1;
	int c2;
	EF_AAA_Param aaaparam;

	EF_Ctrl_Pt ctrlpt;

	//EQF_AA23_Param aa23;
}STRUCTPACKED EF_Basic_Param;




typedef struct
{

	int enable;
	int b;
	float k;
	float c;

}STRUCTPACKED EF_Line_Equation;
typedef struct
{
	int isacw;   // 1 anticlockwise   -1 clockwise
	int lnum;
	int da[MAX_BND_NODE_NUM];
	EF_Line_Equation le[MAX_BND_NODE_NUM];
}STRUCTPACKED EF_Field_Edge;

typedef struct
{
	int asp;
	int cita_ofs;
	int nanum;
	int *na;
	int *bg;
	
}STRUCTPACKED PFSLBGParam;	//Unit: cm







typedef struct
{
	int almDT;        // ����ʱ�䣬PE BLK COUNTER����Ч
	int objLen;       // PE ��Ӧ��ĸ���   BLK��ӦĿ���Сcm    COUNTER��Ӧ�ı����������С
	int objMLen;      // COUNTER ��Ӧ�������ƶ�����

}STRUCTPACKED LIA_Param;
typedef struct
{
	int slflag;

	char szfdsp[64];

	int vc1;
	int vc2;

	// basic param
	EF_Basic_Param basicparam;

	// AAA Ctrl pt
	EF_Ctrl_Pt aaactrlpt[LIA_FIELD_TYPE_NUM];

	// boundary pt
	PField pf[LIA_FIELD_TYPE_NUM];

	int bFieldEnable;// ����ʹ��
	int eFieldEvaAlg;// �����Ӧ���㷨
	int nSheildRegionR;
	LIA_Param liap;



}STRUCTPACKED FSetParam;

typedef struct
{
	int sfNum;

	FSetParam fsp[LIA_MAX_FIELD_SET_NUM];

}STRUCTPACKED LAFieldParam;

typedef struct
{
	int isv;
	int bgdn;
	int asp;   // 
	int bg[2880];

}STRUCTPACKED LAFieldBG;

#define LAIO_MAX_IN_NUM     8
#define LAIO_MAX_OUT_NUM    8

#define IO_IN_FUNC_S1         0
#define IO_IN_FUNC_S2         1
#define IO_IN_FUNC_S3         2
#define IO_IN_FUNC_S4         3
#define IO_IN_FUNC_DISARM     4
#define IO_IN_FUNC_FA         5//Forced alarm
#define IO_IN_FUNC_USERDEF1   6
#define IO_IN_FUNC_USERDEF2   7
#define IO_IN_FUNC_SLR        8



#define IO_OUT_FUNC_READY         0
#define IO_OUT_FUNC_ATTENTION     1
#define IO_OUT_FuNC_ALERT         2
#define IO_OUT_FUNC_ALARM         3
#define IO_OUT_FUNC_ERROR         4
#define IO_OUT_FUNC_HTR           5

#define FIELD_SEL_MODE_NULL      -1
#define FIELD_SEL_MODE_FILPS     0
#define FIELD_SEL_MODE_IO        1

typedef struct
{

	int IO_InNum;
	int IO_InNumExt;
	int IO_InHLEnalbe[LAIO_MAX_IN_NUM];
	int IO_InAdr[LAIO_MAX_IN_NUM];
	int IO_InFunc[LAIO_MAX_IN_NUM];


	int IO_OutNum;
	int IO_OutNumExt;
	int IO_OUTHLEnable[LAIO_MAX_OUT_NUM];
	int IO_OUTAdr[LAIO_MAX_OUT_NUM];
	int IO_OUTFunc[LAIO_MAX_OUT_NUM];

}STRUCTPACKED LAIOCfg;

#define	LDINI_MAX_ATTR_STR_LEN	0x20
typedef struct
{
	// Product Info
	char szType[LDINI_MAX_ATTR_STR_LEN];
	char szManufacturer[LDINI_MAX_ATTR_STR_LEN];
	char szReleaseDate[LDINI_MAX_ATTR_STR_LEN];
	char szSerialNo[LDINI_MAX_ATTR_STR_LEN];

	// Network Configuration
	char szMAC[LDINI_MAX_ATTR_STR_LEN];
	char szIP[LDINI_MAX_ATTR_STR_LEN];
	char szMask[LDINI_MAX_ATTR_STR_LEN];
	char szGate[LDINI_MAX_ATTR_STR_LEN];
	char szDNS[LDINI_MAX_ATTR_STR_LEN];

	// Measurement Parameters
	int RMode;
	int nMR;  //����(cm)
	int nESAR; //ɨ��Ƕ�
	int nOA_OFS;
	int nSAV;
	int nSAV_DMP[2];
	int nSA[2]; // ��Ч���ݽǶ�(��λ:��)
	int bFogSwitch; //������
	int bPTF_ON;
	int nPTF_WW;
	int bPSF_ON;
	int bAI_ON;
	int bLMD_ON;
	int nFS_Mode;

	// Equipment Attribute
	int nTF;
	int nLEA;
	int nLBD;
	int nPF; //�������Ƶ��
	int nMT;
	int nEOBN;
	int nOBN;

	//Intermediate Parameters
	int nSAR;
	int nESA[2]; //��Ч���ݽǶ�(��λ��1/1000�ȣ�
	int nSAP; // �Ƕȷֱ���(��λ:1/1000��)
	int nSNAN; //ÿȦ��������
	int nRNAN;  //ÿȦ��Ч�������ݸ���
	int nRPM;  // ���ÿ����ת��
} LDINI_Type;


#pragma pack(pop)

#endif
