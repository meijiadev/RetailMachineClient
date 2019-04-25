#ifndef __DDR_COMM_PUBLIC_FUN_H_INCLUDED__
#define __DDR_COMM_PUBLIC_FUN_H_INCLUDED__
#include <string>
namespace DDRDevice
{
	long long getSystemTimeMillitm();
	void Sleep(int nMillisec);
	std::string GetTimeNowstring();
}

#endif // __DDR_DEVICE_MANAGER_H_INCLUDED__