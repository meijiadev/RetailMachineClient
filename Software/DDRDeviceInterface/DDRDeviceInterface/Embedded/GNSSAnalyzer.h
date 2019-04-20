#ifndef __DDR_GNSS_TEXT_TO_INFO_ANALYZER_H_INCLUDED__
#define __DDR_GNSS_TEXT_TO_INFO_ANALYZER_H_INCLUDED__

#include <vector>

#ifndef BYTE
typedef unsigned char BYTE;
#endif
#ifndef uint16_t
typedef unsigned short uint16_t;
#endif

namespace DDRDrivers
{
#pragma pack (1)
	struct GNSS_info
	{
		struct _utc_date_
		{
			BYTE bAvailable;
			BYTE day;
			BYTE month;
			uint16_t year;
			_utc_date_() : bAvailable((BYTE)0) {}
		} UTC_Date;
		struct _utc_time_
		{
			BYTE bAvailable;
			BYTE hh;
			BYTE mm;
			BYTE ss;
			uint16_t msms;
			_utc_time_() : bAvailable((BYTE)0) {}
		} UTC_Time;
		struct _latlong_
		{
			BYTE bAvailable;
			BYTE lat_deg;
			double lat_min;
			BYTE NS;
			BYTE long_deg;
			double long_min;
			_latlong_() : bAvailable((BYTE)0) {}
			BYTE EW;
		} LatLong;
		struct _cog_
		{
			BYTE bAvailable;
			double cog;
			_cog_() : bAvailable((BYTE)0) {}
		} CourseOverGround;
		struct _altitude_
		{
			BYTE bAvailable;
			double altitude;
			_altitude_() : bAvailable((BYTE)0) {}
		} Altitude;
		struct _precision_
		{
			BYTE bAvailable;
			BYTE quality; // quality indicator
			BYTE numSV;
			double PDOP;
			double HDOP;
			double VDOP;
			_precision_() : bAvailable((BYTE)0) {}
		} Quality;
	};
#pragma pack ()

	typedef void(*_formProcFunc)(const char*, int, int, GNSS_info*);
	class GNSSAnalyzer
	{
	public:
		void FeedBytes(const void *pData, int nDataLen);

		bool GetGNSSData(double &dstLng, double &dstLat,
			unsigned short &cog, float &altitude,
			unsigned int &UNIXTimeStamp);

	protected:
		std::vector<char> m_txtBuf;
		GNSS_info m_GNSSInfo;

		void analyzeBuffer();
		void analyzeLine(const char *pStart, int len);

		static bool _convertHexStr2Int(const char *pStr, int len, int &dst);
		static bool _convertStr2Dec(const char *pStr, int len, int &dst);
		static bool _convertStr2LatLong(const char *pStr, int len, int degLen, BYTE &deg, double &min);
		static bool _convertStr2Double(const char *pStr, int len, double &dst);

		static std::vector<_formProcFunc> s_FormProcFuncs;
		static void _GGA_process(const char *pstr, int len, int talkerID,
			                     GNSS_info *pGNSSInfo);
		static void _GSA_process(const char *pstr, int len, int talkerID,
			                     GNSS_info *pGNSSInfo);
		static void _GSV_process(const char *pstr, int len, int talkerID,
			                     GNSS_info *pGNSSInfo);
		static void _RMC_process(const char *pstr, int len, int talkerID,
			                     GNSS_info *pGNSSInfo);
		static void _VTG_process(const char *pstr, int len, int talkerID,
			                     GNSS_info *pGNSSInfo);
		static std::vector<int> s_Hash2FuncInd;
		static int mapHeaderStr2FuncInd(const char *pHeader);

		static void wgs2gcj(double wgsLat, double wgsLng, double *gcjLat, double *gcjLng);
		static void gcj2wgs(double gcjLat, double gcjLng, double *wgsLat, double *wgsLng);
		static void gcj2wgs_exact(double gcjLat, double gcjLng, double *wgsLat, double *wgsLng);
		static double distance(double latA, double lngA, double latB, double lngB);
	};
}

#endif
