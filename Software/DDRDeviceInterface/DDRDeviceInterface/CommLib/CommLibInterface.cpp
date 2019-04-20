#include "stdafx.h"

#include "CommLib/CommLibInterface.h"

#include <WinSock2.h>
#pragma comment (lib,"ws2_32.lib")

namespace DDRCommLib {
	
class WSAInitializer {
public:
	WSAInitializer() {
		WSAData wsaData;
		int iErrorMsg = ::WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (iErrorMsg != NO_ERROR) {
			//std::cout << "WSAStartup()Ê§°Ü,´íÎóÂë=" << iErrorMsg << ".°´»Ø³µ¼üÍË³ö...\n";
		}
	}
	~WSAInitializer() {
		::WSACleanup();
	}
};
WSAInitializer __wsaInitializer;

#if 1
static const int nPrimes = 168;
static const int Primes[168] = { 2, 3, 5, 7, 11, 13, 17, 19, 23, 29,
31, 37, 41, 43, 47, 53, 59, 61, 67, 71,
73, 79, 83, 89, 97, 101, 103, 107, 109, 113,
127, 131, 137, 139, 149, 151, 157, 163, 167, 173,
179, 181, 191, 193, 197, 199, 211, 223, 227, 229,
233, 239, 241, 251, 257, 263, 269, 271, 277, 281,
283, 293, 307, 311, 313, 317, 331, 337, 347, 349,
353, 359, 367, 373, 379, 383, 389, 397, 401, 409,
419, 421, 431, 433, 439, 443, 449, 457, 461, 463,
467, 479, 487, 491, 499, 503, 509, 521, 523, 541,
547, 557, 563, 569, 571, 577, 587, 593, 599, 601,
607, 613, 617, 619, 631, 641, 643, 647, 653, 659,
661, 673, 677, 683, 691, 701, 709, 719, 727, 733,
739, 743, 751, 757, 761, 769, 773, 787, 797, 809,
811, 821, 823, 827, 829, 839, 853, 857, 859, 863,
877, 881, 883, 887, 907, 911, 919, 929, 937, 941,
947, 953, 967, 971, 977, 983, 991, 997 };
#endif

bool Txt_Encrypt(const void *pSrc, int lenSrc,
	             const void *pTar, int lenTar) {
	if (!pSrc || lenSrc <= 0 || !pTar) { return 0; }
	if (lenTar < lenSrc + 4 || lenSrc <= 0) {
		// one char index, two chars check
		return false;
	}
	const unsigned char *pTxt = (const unsigned char*)pSrc;
	unsigned char *pCode = (unsigned char*)pTar;
	SYSTEMTIME st;
	int ms, aa, bb, cc;
	::GetSystemTime(&st);
	ms = st.wMilliseconds;
	aa = 0;
	bb = nPrimes - 1;
	while (bb > aa + 1) {
		if (ms == Primes[aa]) {
			cc = aa;
			bb = aa;
			break;
		} else if (ms == Primes[bb]) {
			cc = bb;
			aa = bb;
			break;
		}
		cc = (aa + bb) >> 1;
		if (ms == Primes[cc]) {
			aa = bb = cc;
			break;
		} else if (ms > Primes[cc]) {
			aa = cc;
		} else {
			bb = cc;
		}
	}
	if (aa != bb) {
		if (ms - Primes[aa] > Primes[bb] - ms) {
			cc = bb;
		} else {
			cc = aa;
		}
	}
	bb = 0;
	for (aa = 0; aa < lenSrc; ++aa) {
		*pCode = (unsigned char)(((int)(*pTxt) + Primes[cc]) & 0xFF);
		bb += (*pCode) * (*pCode);
		*pCode = ((*pCode) + ((Primes[(cc + aa) % nPrimes] *
			Primes[(cc + aa) % nPrimes]) & 0xFF)) & 0xFF;
		bb &= 0xFFFF;
		++pCode, ++pTxt;
		cc = (cc + 1) % nPrimes;
	}
	*(unsigned short*)pCode = ms;
	pCode += 2;
	*(unsigned short*)pCode = (unsigned short)bb;
	pCode += 2;
	for (aa += 4; aa < lenTar; ++aa) {
		bb += (pCode[-4] * pCode[-4]);
		bb &= 0xFFFF;
		*pCode = (unsigned char)((bb >> 8) ^ (bb & 0xFF) ^ (Primes[(cc + aa) % nPrimes] & 0xFF));
		++pCode;
	}
	return true;
}

bool Txt_Encrypt2(const void *pSrc1, int len1,
	              const void *pSrc2, int len2,
				  void *pTar, int lenTar) {
	if (!pSrc1 || len1 <= 0 || !pSrc2 || len2 <= 0) { return 0; }
	if (!pTar || lenTar < len1 + len2 + 4) {
		// one char index, two chars check
		return false;
	}
	int lenTxt = len1 + len2;
	const unsigned char *pTxt1 = (const unsigned char*)pSrc1;
	const unsigned char *pTxt2 = (const unsigned char*)pSrc2;
	unsigned char *pCode = (unsigned char*)pTar;
	SYSTEMTIME st;
	int ms, aa, bb, cc;
	::GetSystemTime(&st);
	ms = st.wMilliseconds;
	aa = 0;
	bb = nPrimes - 1;
	while (bb > aa + 1) {
		if (ms == Primes[aa]) {
			cc = aa;
			bb = aa;
			break;
		} else if (ms == Primes[bb]) {
			cc = bb;
			aa = bb;
			break;
		}
		cc = (aa + bb) >> 1;
		if (ms == Primes[cc]) {
			aa = bb = cc;
			break;
		} else if (ms > Primes[cc]) {
			aa = cc;
		} else {
			bb = cc;
		}
	}
	if (aa != bb) {
		if (ms - Primes[aa] > Primes[bb] - ms) {
			cc = bb;
		} else {
			cc = aa;
		}
	}
	const unsigned char *pTxt = pTxt1;
	bb = 0;
	for (aa = 0; aa < lenTxt; ++aa) {
		*pCode = (unsigned char)(((int)(*pTxt) + Primes[cc]) & 0xFF);
		bb += (*pCode) * (*pCode);
		*pCode = ((*pCode) + ((Primes[(cc + aa) % nPrimes] *
			Primes[(cc + aa) % nPrimes]) & 0xFF)) & 0xFF;
		bb &= 0xFFFF;
		++pCode, ++pTxt;
		cc = (cc + 1) % nPrimes;
		if (len1 - 1 == aa) {
			pTxt = pTxt2;
		}
	}
	*(unsigned short*)pCode = ms;
	pCode += 2;
	*(unsigned short*)pCode = (unsigned short)bb;
	pCode += 2;
	for (aa += 4; aa < lenTar; ++aa) {
		bb += (pCode[-4] * pCode[-4]);
		bb &= 0xFFFF;
		*pCode = (unsigned char)((bb >> 8) ^ (bb & 0xFF) ^ (Primes[(cc + aa) % nPrimes] & 0xFF));
		++pCode;
	}
	return true;
}

bool Txt_Decrypt(const void *pSrc, int lenSrc,
	             void *pTar, int lenTar) {
	int x, aa, bb, cc, c_;
	const unsigned char *p1;
	unsigned char val;
	if (!pSrc || lenSrc <= 0 || !pTar || lenSrc < lenTar + 4) {
		// one unsigned char index, two unsigned chars check
		return false;
	}
	const unsigned char *pCode = (const unsigned char*)pSrc;
	unsigned char *pTxt = (unsigned char*)pTar;
	x = *(unsigned short*)(pCode + lenTar);
	aa = 0;
	bb = nPrimes - 1;
	while (bb > aa + 1)	{
		if (x == Primes[aa]) {
			cc = aa;
			bb = aa;
			break;
		} else if (x == Primes[bb]) {
			cc = bb;
			aa = bb;
			break;
		}
		cc = (aa + bb) >> 1;
		if (x == Primes[cc]) {
			aa = bb = cc;
			break;
		} else if (x > Primes[cc]) {
			aa = cc;
		} else {
			bb = cc;
		}
	}
	if (aa != bb) {
		if (x - Primes[aa] > Primes[bb] - x) {
			cc = bb;
		} else {
			cc = aa;
		}
	}

	p1 = pCode;
	c_ = cc;
	bb = 0;
	for (aa = 0; aa < lenTar; ++aa) {
		val = (unsigned char)(*pCode - ((Primes[(cc + aa) % nPrimes] * Primes[(cc + aa) % nPrimes]) & 0xFF));
		bb += val * val;
		bb &= 0xFFFF;
		cc = (cc + 1) % nPrimes;
		++pCode;
	}

	if (*(unsigned short*)(pCode + 2) != bb) {
		return false;
	}
	pCode += 4;
	for (aa += 4; aa < lenSrc; ++aa) {
		bb += (pCode[-4] * pCode[-4]);
		bb &= 0xFFFF;
		if (*pCode != (unsigned char)((bb >> 8) ^ (bb & 0xFF) ^
			(Primes[(cc + aa) % nPrimes] & 0xFF))) {
			return false;
		}
		++pCode;
	}
	cc = c_;
	pCode = p1;
	for (aa = 0; aa < lenTar; ++aa) {
		val = (unsigned char)(*pCode - ((Primes[(cc + aa) % nPrimes] *
			Primes[(cc + aa) % nPrimes]) & 0xFF));
		*pTxt = (unsigned char)(val - (Primes[cc] & 0xFF));
		cc = (cc + 1) % nPrimes;
		++pCode, ++pTxt;
	}
	return true;
}

__int64 EncryptDataLen_64(unsigned int len) {
	__int64 x = (__int64)(~len);
	x <<= 32;
	x |= len;
	return x;
}

bool VerifyDataLen_64(const void *pDataHead, unsigned int *plen) {
	*plen = *((unsigned int*)pDataHead);
	return (*((unsigned int*)pDataHead + 1) == ~(*plen));
}

int EncryptDataLen_32(int len) {
	if (len > 0xFFFFFF || len < 0)	{ return -1; }
	int ret = (len >> 16) ^ (len >> 8) ^ (len & 0xFF);
	if (len & 0xFF0000)	{
		ret = len | ((~ret) << 24);
	} else {
		ret = len | (ret << 24);
	}
	return ret;
}

bool VerifyDataLen_32(const void *pDataHead, int *plen) {
	const unsigned char *pC = (const unsigned char*)pDataHead;
	*plen = pC[0] | (pC[1] << 8) | (pC[2] << 16);
	unsigned char vv = pC[0] ^ pC[1] ^ pC[2];
	if (pC[2] != 0) {
		vv = 255 - vv;
	}
	if (pC[3] == vv) {
		return true;
	} else {
		*plen = -1;
		return false;
	}
}

void ConvertUIP2Str_IPv4(unsigned long ip,
	                     char *pStr, int *pEndingPos) {
	int pos = 0;
	char *pStrHead = pStr;
	for (int i = 0; i < 4; ++i) {
		BYTE seg = ((BYTE*)(&ip))[i];
		bool bSegStarted = false;
		if (seg / 100 > 0) {
			(*pStr++) = (int)(seg / 100) + '0';
			bSegStarted = true;
		}
		if ((seg % 100) / 10 > 0 || bSegStarted) {
			(*pStr++) = (int)((seg % 100) / 10) + '0';
		}
		(*pStr++) = (int)(seg % 10) + '0';
		if (i < 3) {
			(*pStr++) = '.';
		}
	}
	*pStr = '\0';
	if (pEndingPos) {
		*pEndingPos = pStr - pStrHead;
	}
}

bool ConvertStr2UIP_IPv4(const char *pStr, unsigned long *ip,
	                     char cDelimiter, int *pEndingPos) {
	*ip = 0;
	const char *pStrHead = pStr;
	BYTE *seg = (BYTE*)ip;
	int segPos = 0, segVal = 0;
	for (; *pStr != cDelimiter && *pStr != '\0'; ++pStr) {
		if (*pStr >= '0' && *pStr <= '9') {
			segVal = segVal * 10 + (*pStr - '0');
		}
		else if (*pStr == '.') {
			if (segVal > 255) { return false; }
			else {
				seg[segPos] = (BYTE)segVal;
				segVal = 0;
			}
			if (++segPos > 3) {
				return false;
			}
		}
		else { return false; }
	}
	seg[segPos] = (BYTE)segVal;
	if (pEndingPos) {
		*pEndingPos = pStr - pStrHead;
	}
	return true;
}

bool ConvertStr2Int(const char *pStr, int *x,
	                char cDelimiter, int *pEndingPos) {
	const char *pStrHead = pStr;
	for ((*x) = 0; *pStr != '\0' && *pStr != cDelimiter; ++pStr) {
		if (*pStr < '0' || *pStr > '9') {
			return false;
		}
		(*x) = (*x) * 10 + (*pStr - '0');
	}
	if (pEndingPos) {
		*pEndingPos = pStr - pStrHead;
	}
	return true;
}

void ConvertInt2Str(int x, char *pStr, int *pEndingPos) {
	bool bNeg = false;
	if (x < 0) {
		bNeg = true;
		x = -x;
	}
	int pos;
	if (x != 0) {
		for (pos = 0; x > 0; x = x / 10, ++pos) {
			pStr[pos] = (x % 10) + '0';
		}
		if (bNeg) {
			pStr[pos++] = '-';
		}
		for (int a = 0, b = pos - 1; a < b; ++a, --b) {
			char t = pStr[a];
			pStr[a] = pStr[b];
			pStr[b] = t;
		}
		pStr[pos] = '\0';
	} else {
		pStr[0] = '0';
		pStr[1] = '\0';
		pos = 1;
	}
	if (pEndingPos) {
		*pEndingPos = pos;
	}
}

unsigned short ConvertHTONS(unsigned short u16val) {
	return (((u16val & 0xFF) << 8) | ((u16val >> 8) & 0xFF));
}



}