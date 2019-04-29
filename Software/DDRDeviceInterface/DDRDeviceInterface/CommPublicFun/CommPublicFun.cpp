#include "CommPublicFun.h"
#include <chrono>
#include <thread>
#include <sys\timeb.h>
#include <time.h>

namespace DDRDevice
{
	/*
		获取当前系统时间(ms)
	*/
	long long getSystemTimeMillitm()
	{
		timeb t;
		ftime(&t);
		return t.time * 1000 + t.millitm;
	}

	void Sleep(int nMillisec)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(nMillisec));
	}

	std::string GetTimeNowstring()
	{
		time_t rawtime;
		struct tm * timeinfo;
		char buffer[80];

		time(&rawtime);
		timeinfo = localtime(&rawtime);

		strftime(buffer, sizeof(buffer), "%d-%m-%Y-%H-%M-%S", timeinfo);
		std::string str(buffer);
		return str;
	}
}
