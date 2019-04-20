#include "GNSSAnalyzer.h"

#include <time.h>

namespace DDRDrivers {

void GNSSAnalyzer::FeedBytes(const void *pData, int nDataLen) {
	if (nullptr == pData || nDataLen <= 0) { return; }
	int pos = (int)m_txtBuf.size();
	m_txtBuf.resize(pos + nDataLen);
	memcpy(&m_txtBuf[pos], pData, nDataLen);
	analyzeBuffer();
}

bool GNSSAnalyzer::GetGNSSData(double &dstLng, double &dstLat,
	                           unsigned short &cog, float &altitude,
							   unsigned int &UNIXTimeStamp) {
	if ((BYTE)0 == m_GNSSInfo.LatLong.bAvailable ||
		(BYTE)0 == m_GNSSInfo.UTC_Date.bAvailable ||
		(BYTE)0 == m_GNSSInfo.UTC_Time.bAvailable) {
		return false;
	}
	dstLng = m_GNSSInfo.LatLong.long_deg + m_GNSSInfo.LatLong.long_min / 60.0;
	dstLat = m_GNSSInfo.LatLong.lat_deg + m_GNSSInfo.LatLong.lat_min / 60.0;

	if ((BYTE)0 == m_GNSSInfo.CourseOverGround.bAvailable) {
		cog = -1;
	} else {
		cog = (unsigned short)round(m_GNSSInfo.CourseOverGround.cog);
	}
	if ((BYTE)0 == m_GNSSInfo.Altitude.bAvailable) {
		altitude = -1.0f;
	} else {
		altitude = (float)m_GNSSInfo.Altitude.altitude;
	}
	struct tm tttt;
	tttt.tm_year = m_GNSSInfo.UTC_Date.year;
	tttt.tm_mon = m_GNSSInfo.UTC_Date.month;
	tttt.tm_mday = m_GNSSInfo.UTC_Date.day;
	tttt.tm_hour = m_GNSSInfo.UTC_Time.hh;
	tttt.tm_min = m_GNSSInfo.UTC_Time.mm;
	tttt.tm_sec = m_GNSSInfo.UTC_Time.ss;
	tttt.tm_isdst = 0;
	tttt.tm_year -= 1900;
	tttt.tm_mon--;
	UNIXTimeStamp = (unsigned int)mktime(&tttt);
	m_GNSSInfo.LatLong.bAvailable = (BYTE)0;
	m_GNSSInfo.CourseOverGround.bAvailable = (BYTE)0;
	m_GNSSInfo.Altitude.bAvailable = (BYTE)0;
	m_GNSSInfo.UTC_Date.bAvailable = (BYTE)0;
	m_GNSSInfo.UTC_Time.bAvailable = (BYTE)0;
	return true;
}

void GNSSAnalyzer::analyzeBuffer() {
	if (m_txtBuf.size() < 2) { return; }
	const char *pCurTxt = &m_txtBuf[0];
	const char *const pEnd = pCurTxt + m_txtBuf.size();
	for (pCurTxt = &m_txtBuf[0]; pCurTxt < pEnd - 1; ++pCurTxt) {
		if (0x0D == pCurTxt[0] && 0x0A == pCurTxt[1]) { break; }
	}
	if (pCurTxt < pEnd - 1) {
		while (1) {
			const char *pNextLine = pCurTxt + 2;
			for (; pNextLine < pEnd - 1; ++pNextLine) {
				if (0x0D == pNextLine[0] && 0x0A == pNextLine[1]) {
					break;
				}
			}
			if (pNextLine >= pEnd - 1) {
				break;
			} else {
				analyzeLine(pCurTxt + 2, pNextLine - pCurTxt - 2);
				pCurTxt = pNextLine;
			}
		}
	}

	if (pCurTxt < &m_txtBuf[0] + m_txtBuf.size()) {
		memmove(&m_txtBuf[0], pCurTxt,
			    &m_txtBuf[0] + m_txtBuf.size() - pCurTxt);
	}
	m_txtBuf.resize(&m_txtBuf[0] + m_txtBuf.size() - pCurTxt);
}

void GNSSAnalyzer::analyzeLine(const char *pStart, int len) {
	if (len < 9) { return; }// length constraint
	if (*pStart != '$' || pStart[len - 3] != '*') { return; } // format constraint
	int strSum;
	if (!_convertHexStr2Int(pStart + len - 2, 2, strSum)) {	return; } // checksum format constraint
	BYTE csum = 0;
	for (int i = 1; i < len - 3; ++i)
		csum ^= pStart[i];
	if ((int)csum != strSum) { return; }// checksum validation

	int talkerId = -1;
	if (pStart[1] == 'G' && pStart[2] == 'P') {
		talkerId = 0;
	} else if (pStart[1] == 'G' && pStart[2] == 'N') {
		talkerId = 1;
	}
	if (talkerId == -1) { return; }
	int strFuncID = mapHeaderStr2FuncInd(pStart + 3);
	if (-1 == strFuncID) { return; }
	(s_FormProcFuncs[strFuncID])(pStart, len, talkerId, &m_GNSSInfo);
}

bool GNSSAnalyzer::_convertStr2Dec(const char *pStr, int len, int &dst)	{
	dst = 0;
	for (int i = 0; i < len; ++i, ++pStr) {
		dst *= 10;
		if (*pStr >= '0' && *pStr <= '9') {
			dst += (*pStr) - '0';
		} else {
			return false;
		}
	}
	return true;
}

bool GNSSAnalyzer::_convertHexStr2Int(const char *pStr, int len, int &dst) {
	dst = 0;
	for (int i = 0; i < len; ++i, ++pStr) {
		dst <<= 4;
		if (*pStr >= '0' && *pStr <= '9') {
			dst += (*pStr) - '0';
		} else if (*pStr >= 'A' && *pStr <= 'F') {
			dst += (*pStr) - 'A' + 10;
		} else if (*pStr >= 'a' && *pStr <= 'f') {
			dst += (*pStr) - 'a' + 10;
		} else { return false; }
	}
	return true;
}

bool GNSSAnalyzer::_convertStr2LatLong(const char *pStr, int len,
	                                   int degLen, BYTE &deg, double &min) {
	if (degLen < 2 || degLen > 3) { return false; }
	const char *pM;
	if (degLen == 2) {
		deg = (pStr[0] - '0') * 10 + (pStr[1] - '0');
		pM = pStr + 2;
	}
	else {
		deg = (pStr[0] - '0') * 100 + (pStr[1] - '0') * 10 + (pStr[2] - '0');
		pM = pStr + 3;
	}
	min = (pM[0] - '0') * 10 + (pM[1] - '0');
	double x = 0.1;
	for (pM += 3; pM < pStr + len; ++pM) {
		min += (*pM - '0') * x;
		x *= 0.1;
	}
	return true;
}

bool GNSSAnalyzer::_convertStr2Double(const char *pStr,
	                                  int len, double &dst) {
	dst = 0.0;
	int i = 0;
	for (; i < len; ++i, ++pStr) {
		if (*pStr == '.') {
			++i, ++pStr;
			break;
		}
		else {
			if (*pStr < '0' || *pStr > '9')
				return false;
			dst = dst * 10.0 + (*pStr - '0');
		}
	}
	__int64 decInt = 0;
	int decLen = 0;
	for (; i < len; ++i) {
		if (*pStr < '0' || *pStr > '9') { return false; }
		decInt = decInt * 10 + (*pStr - '0');
		++pStr;
		++decLen;
	}
	dst += decInt * pow(10.0, -decLen);
	return true;
}

std::vector<_formProcFunc> GNSSAnalyzer::s_FormProcFuncs{ _GGA_process,
	_GSA_process,
	_GSV_process,
	_RMC_process,
	_VTG_process };

std::vector<int> GNSSAnalyzer::s_Hash2FuncInd{
	('G' - 0x0A) * 10000 + ('G' - 0x0A) * 100 + ('A' - 0x0A),
	('G' - 0x0A) * 10000 + ('S' - 0x0A) * 100 + ('A' - 0x0A),
	('G' - 0x0A) * 10000 + ('S' - 0x0A) * 100 + ('V' - 0x0A),
	('R' - 0x0A) * 10000 + ('M' - 0x0A) * 100 + ('C' - 0x0A),
	('V' - 0x0A) * 10000 + ('T' - 0x0A) * 100 + ('G' - 0x0A) };

int GNSSAnalyzer::mapHeaderStr2FuncInd(const char *pHeader) {
	int s = 0;
	for (int i = 0; i < 3; ++i) {
		if ((pHeader[i] < ' ' || pHeader[i] > '~') &&
			pHeader[i] != (char)0x0A && pHeader[i] != (char)0x0D &&
			pHeader[i] != '$') {
			return -1;
		} else {
			s = s * 100 + (pHeader[i] - 0x0A);
		}
	}
	for (int i = 0; i < (int)s_Hash2FuncInd.size(); ++i) {
		if (s_Hash2FuncInd[i] == s) {
			return i;
		}
	}
	return -1;
}

void GNSSAnalyzer::_GGA_process(const char *pstr, int len, int talkerID,
	                            GNSS_info *pGNSSInfo) {
	const char *p0, *p1;
	for (p0 = pstr; p0 < pstr + len; ++p0) {
		if (*p0 == ',')
			break;
	}
	for (int ind = 0; p0 < pstr + len; p0 = p1, ++ind) {
		for (p1 = p0 + 1; p1 < pstr + len; ++p1) {
			if (*p1 == ',' || *p1 == '*') { break; }
		}
		if (p1 >= pstr + len || p1 - p0 == 1) { continue; }
		int tt;
		switch (ind) {
		case 0:
			pGNSSInfo->UTC_Time.bAvailable = 1;
			pGNSSInfo->UTC_Time.hh = (p0[1] - '0') * 10 + (p0[2] - '0');
			pGNSSInfo->UTC_Time.mm = (p0[3] - '0') * 10 + (p0[4] - '0');
			pGNSSInfo->UTC_Time.ss = (p0[5] - '0') * 10 + (p0[6] - '0');
			if (_convertStr2Dec(p0 + 8, p1 - p0 - 8, tt)) {
				pGNSSInfo->UTC_Time.msms = (uint16_t)tt;
			}
			break;
		case 1:
			pGNSSInfo->LatLong.bAvailable = (BYTE)_convertStr2LatLong(p0 + 1, p1 - p0 - 1, 2,
				                                                      pGNSSInfo->LatLong.lat_deg,
																	  pGNSSInfo->LatLong.lat_min);
			break;
		case 2:
			pGNSSInfo->LatLong.NS = p0[1];
			break;
		case 3:
			_convertStr2LatLong(p0 + 1, p1 - p0 - 1, 3,
				                pGNSSInfo->LatLong.long_deg,
								pGNSSInfo->LatLong.long_min);
			break;
		case 4:
			pGNSSInfo->LatLong.EW = p0[1];
			break;
		case 5:
			pGNSSInfo->Quality.quality = p0[1] - '0';
			break;
		case 6:
			pGNSSInfo->Quality.numSV = (p0[1] - '0') * 10 + (p0[2] - '0');
			break;
		case 7:
			_convertStr2Double(p0 + 1, p1 - p0 - 1,
				               pGNSSInfo->Quality.HDOP);
			break;
		case 8:
			pGNSSInfo->Altitude.bAvailable = (BYTE)_convertStr2Double(p0 + 1, p1 - p0 - 1,
				                                                      pGNSSInfo->Altitude.altitude);
			break;
		}
	}
}
void GNSSAnalyzer::_GSA_process(const char *pstr, int len, int talkerID,
	                            GNSS_info *pGNSSInfo) {
	const char *p0, *p1;
	for (p0 = pstr; p0 < pstr + len; ++p0) {
		if (*p0 == ',') { break; }
	}
	for (int ind = 0; p0 < pstr + len; p0 = p1, ++ind) {
		for (p1 = p0 + 1; p1 < pstr + len; ++p1) {
			if (*p1 == ',' || *p1 == '*') { break; }
		}
		if (p1 >= pstr + len || p1 - p0 == 1) { continue; }
		switch (ind) {
		case 14:
			pGNSSInfo->Quality.bAvailable = (BYTE)_convertStr2Double(p0 + 1, p1 - p0 - 1,
				                                                     pGNSSInfo->Quality.PDOP);
			break;
		case 15:
			_convertStr2Double(p0 + 1, p1 - p0 - 1, pGNSSInfo->Quality.HDOP);
			break;
		case 16:
			_convertStr2Double(p0 + 1, p1 - p0 - 1, pGNSSInfo->Quality.VDOP);
			break;
		}
	}
}
void GNSSAnalyzer::_GSV_process(const char *pstr, int len, int talkerID,
	                            GNSS_info *pGNSSInfo) {
}
void GNSSAnalyzer::_RMC_process(const char *pstr, int len, int talkerID,
	                            GNSS_info *pGNSSInfo) {
	const char *p0, *p1;
	for (p0 = pstr; p0 < pstr + len; ++p0) {
		if (*p0 == ',') { break; }
	}
	for (int ind = 0; p0 < pstr + len; p0 = p1, ++ind) {
		for (p1 = p0 + 1; p1 < pstr + len; ++p1) {
			if (*p1 == ',' || *p1 == '*') { break; }
		}
		if (p1 >= pstr + len || p1 - p0 == 1) { continue; }
		int tt;
		switch (ind) {
		case 0:
			pGNSSInfo->UTC_Time.bAvailable = 1;
			pGNSSInfo->UTC_Time.hh = (p0[1] - '0') * 10 + (p0[2] - '0');
			pGNSSInfo->UTC_Time.mm = (p0[3] - '0') * 10 + (p0[4] - '0');
			pGNSSInfo->UTC_Time.ss = (p0[5] - '0') * 10 + (p0[6] - '0');
			if (_convertStr2Dec(p0 + 8, p1 - p0 - 8, tt)) {
				pGNSSInfo->UTC_Time.msms = (uint16_t)tt;
			}
			break;
		case 1:
			if (p0[1] == 'V') { return; }
			break;
		case 2:
			_convertStr2LatLong(p0 + 1, p1 - p0 - 1, 2,
				                pGNSSInfo->LatLong.lat_deg,
								pGNSSInfo->LatLong.lat_min);
			break;
		case 3:
			pGNSSInfo->LatLong.NS = p0[1];
			break;
		case 4:
			_convertStr2LatLong(p0 + 1, p1 - p0 - 1, 3,
				                pGNSSInfo->LatLong.long_deg,
								pGNSSInfo->LatLong.long_min);
			break;
		case 5:
			pGNSSInfo->LatLong.EW = p0[1];
			break;
		case 7:
			pGNSSInfo->CourseOverGround.bAvailable = 1;
			_convertStr2Double(p0 + 1, p1 - p0 - 1,
				               pGNSSInfo->CourseOverGround.cog);
			break;
		case 8:
			pGNSSInfo->UTC_Date.bAvailable = 1;
			pGNSSInfo->UTC_Date.day = (p0[1] - '0') * 10 + (p0[2] - '0');
			pGNSSInfo->UTC_Date.month = (p0[3] - '0') * 10 + (p0[4] - '0');
			pGNSSInfo->UTC_Date.year = (p0[5] - '0') * 10 + (p0[6] - '0') + 2000;
			break;
		}
	}
}
void GNSSAnalyzer::_VTG_process(const char *pstr, int len, int talkerID,
	GNSS_info *pGNSSInfo){
}

}