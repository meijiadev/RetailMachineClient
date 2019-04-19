#pragma once
#include <string>
std::string g_BuildTime = "4/19/2019 10:35:24 AM";
std::string g_Version = "1.0.0";

#ifdef _DEBUG
std::string g_DMode = "Debug";
#else

std::string g_DMode = "Release";
#endif
