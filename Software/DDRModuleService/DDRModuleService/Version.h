#pragma once
#include <string>
std::string g_BuildTime = "4/28/2019 3:02:58 PM";
std::string g_Version = "1.0.0";

#ifdef _DEBUG
std::string g_DMode = "Debug";
#else

std::string g_DMode = "Release";
#endif
