#ifndef utils_h__
#define utils_h__


#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <ctime>
#include <chrono>
#include <iomanip>
#include <memory>
#include <stdarg.h>
#include <functional>
#include <vector>

class ConsoleUtility
{

public:
	ConsoleUtility();

	void ConsoleLoop();
	void AddCommand(std::string cmd, std::function<void()> func);


	void Quit();



protected:

	std::map<std::string, std::function<void()>> m_Functionmap;

	bool m_Quit;


	std::string m_CurrentCmd;
};

#endif // utils_h__
