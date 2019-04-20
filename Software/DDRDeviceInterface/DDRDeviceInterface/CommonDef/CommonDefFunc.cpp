#include "CommonDefFunc.h"

#include <vector>
#include <chrono>

namespace DDRCommonDef {

void ClearKeyboardInput() {
	::FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
}

bool KeyStatusChange(int &vCode, bool &bDown) {
	HANDLE keyboard = GetStdHandle(STD_INPUT_HANDLE);
	DWORD dw, num;
	if (!::GetNumberOfConsoleInputEvents(keyboard, &num) || num == 0) {
		return false;
	}
	for (int i = 0; i < (int)num; ++i) {
		INPUT_RECORD input;
		::ReadConsoleInputA(keyboard, &input, 1, (LPDWORD)(&dw));
		if (KEY_EVENT == input.EventType) {
			vCode = input.Event.KeyEvent.wVirtualKeyCode;
			bDown = (input.Event.KeyEvent.bKeyDown == TRUE);
			return true;
		}
	}
	return false;
}

int KeyTest() {
	HANDLE keyboard = GetStdHandle(STD_INPUT_HANDLE);
	DWORD dw, num;
	if (!::GetNumberOfConsoleInputEvents(keyboard, &num) || num == 0) {
		return 0;
	}
	for (int i = 0; i < (int)num; ++i) {
		INPUT_RECORD input;
		::ReadConsoleInputA(keyboard, &input, 1, (LPDWORD)(&dw));
		if (input.EventType == KEY_EVENT && !input.Event.KeyEvent.bKeyDown)	{
			//return (int)(input.Event.KeyEvent.uChar.AsciiChar);
			return (int)(input.Event.KeyEvent.wVirtualKeyCode);
		}
	}
	return 0;
}

void WaitOrKey(unsigned int maxWaitTimeInMS) {
	ClearKeyboardInput();
	HANDLE keyboard = ::GetStdHandle(STD_INPUT_HANDLE);
	auto tic_start = std::chrono::system_clock::now();
	while (1) {
		auto tic_now = std::chrono::system_clock::now();
		__int64 timediff = std::chrono::duration_cast
			<std::chrono::milliseconds>(tic_now - tic_start).count();
		if (timediff >= maxWaitTimeInMS) { break; }
		if (WAIT_OBJECT_0 == ::WaitForSingleObject(
			GetStdHandle(STD_INPUT_HANDLE),
			(DWORD)round(maxWaitTimeInMS - timediff))) {
			DWORD dw, num;
			if (!::GetNumberOfConsoleInputEvents(keyboard, &num) || num == 0) {
				break;
			}
			bool bKeyUp = false;
			for (int i = 0; i < (int)num; ++i) {
				INPUT_RECORD input;
				ReadConsoleInput(keyboard, &input, 1, (LPDWORD)(&dw));
				if (input.EventType == KEY_EVENT &&
					!input.Event.KeyEvent.bKeyDown) {
					bKeyUp = true;
				}
			}
			if (bKeyUp)	{
				break;
			}
		}
	}
}

void GetFormattedTime(char *str, int *pLen) {
	SYSTEMTIME st;
	GetSystemTime(&st);
	sprintf_s(str, 24, "%04d/%02d/%02d-%02d:%02d:%02d.%03d\0",
		st.wYear, st.wMonth, st.wDay,
		st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
	if (pLen) {
		*pLen = 23;
	}
}

void GetUNIXTimeStamp(unsigned int &UTC_Sec, unsigned short &milSec) {
	std::chrono::milliseconds ms = std::chrono::duration_cast
		<std::chrono::milliseconds>
		(std::chrono::system_clock::now().
		time_since_epoch());
	UTC_Sec = (unsigned int)(ms.count() / 1000);
	milSec = (unsigned short)(ms.count() % 1000);
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
	}
	else {
		pStr[0] = '0';
		pStr[1] = '\0';
		pos = 1;
	}
	if (pEndingPos) {
		*pEndingPos = pos;
	}
}

float pi2pif(float angInRad) {
	int k = (int)floor(angInRad / PI2);
	angInRad -= k * PI2;
	if (angInRad > PI) { return (angInRad - PI2); }
	else { return angInRad; }
}

float WeightedAngleAddition_f(float ang1, float ang2, float alpha) {
	// designed to properly add two angles
	// avoid +pi,-pi cross
	ang1 = pi2pif(ang1);
	ang2 = pi2pif(ang2);
	if (ang1 < ang2) {
		// make sure ang1 >= ang2
		float tt = ang2;
		ang2 = ang1;
		ang1 = tt;
		alpha = 1 - alpha;
	}
	if (ang1 - ang2 > PI) {
		// danger of +pi,-pi crossing
		ang2 += PI2;
	}
	float r = alpha * ang1 + (1 - alpha) * ang2;
	return pi2pif(r);
}

double pi2pi(double angInRad) {
	int k = (int)floor(angInRad / PI2);
	angInRad -= k * PI2;
	if (angInRad > PI) { return (angInRad - PI2); }
	else { return angInRad; }
}

double WeightedAngleAddition(double ang1, double ang2, double alpha) {
	// designed to properly add two angles
	// avoid +pi,-pi cross
	ang1 = pi2pi(ang1);
	ang2 = pi2pi(ang2);
	if (ang1 < ang2) {
		// make sure ang1 >= ang2
		double tt = ang2;
		ang2 = ang1;
		ang1 = tt;
		alpha = 1 - alpha;
	}
	if (ang1 - ang2 > PI) {
		// danger of +pi,-pi crossing
		ang2 += PI2;
	}
	double r = alpha * ang1 + (1 - alpha) * ang2;
	return pi2pi(r);
}

void GetRelative2DPose_f(float x0, float y0, float th0,
	float x1, float y1, float th1,
	float &x1_0, float &y1_0, float &th1_0) {
	float cc = cosf(th0);
	float ss = sinf(th0);
	x1_0 = (x1 - x0) * cc + (y1 - y0) * ss;
	y1_0 = -(x1 - x0) * ss + (y1 - y0) * cc;
	th1_0 = pi2pif(th1 - th0);
}

void GetRelative2DPose(double x0, double y0, double th0,
	double x1, double y1, double th1,
	double &x1_0, double &y1_0, double &th1_0) {
	double cc = cos(th0);
	double ss = sin(th0);
	x1_0 = (x1 - x0) * cc + (y1 - y0) * ss;
	y1_0 = -(x1 - x0) * ss + (y1 - y0) * cc;
	th1_0 = pi2pi(th1 - th0);
}

void Get2DPoseFromBaseAndRelative(double x0, double y0, double th0,
	double x1_0, double y1_0, double th1_0,
	double &x1, double &y1, double &th1) {
	double cc = cos(th0);
	double ss = sin(th0);
	x1 = x0 + cc * x1_0 - ss * y1_0;
	y1 = y0 + ss * x1_0 + cc * y1_0;
	th1 = pi2pi(th0 + th1_0);
}

void Get2DPoseFromBaseAndRelative_f(float x0, float y0, float th0,
	float x1_0, float y1_0, float th1_0,
	float &x1, float &y1, float &th1) {
	float cc = cosf(th0);
	float ss = sinf(th0);
	x1 = x0 + cc * x1_0 - ss * y1_0;
	y1 = y0 + ss * x1_0 + cc * y1_0;
	th1 = pi2pif(th0 + th1_0);
}

}
