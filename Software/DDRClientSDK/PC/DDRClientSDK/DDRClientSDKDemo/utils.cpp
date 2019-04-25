#include "utils.h"


#include <stdio.h>
#include <stdarg.h>

using namespace std;


ConsoleUtility::ConsoleUtility()
{
	m_Quit = false;
	AddCommand("quit", std::bind(&ConsoleUtility::Quit, this));
}


void ConsoleUtility::Quit()
{
	m_Quit = true;
}


std::vector<std::string> split(const std::string &text, char sep)
{
	std::vector<std::string> tokens;
	std::size_t start = 0, end = 0;
	while ((end = text.find(sep, start)) != std::string::npos) {
		if (end != start) {
			tokens.push_back(text.substr(start, end - start));
		}
		start = end + 1;
	}
	if (end != start) {
		tokens.push_back(text.substr(start));
	}
	return tokens;
}
void ConsoleUtility::ConsoleLoop()
{
	std::string input;
	do
	{
		getline(cin, input);

		m_CurrentCmd = input;

		auto vec = split(m_CurrentCmd, ':');
		auto cmd = vec[0];

		if (m_Functionmap.find(cmd) != m_Functionmap.end())
		{
			m_Functionmap[cmd]();
		}

	} while (!m_Quit);
}

void ConsoleUtility::AddCommand(std::string cmd, std::function<void()> func)
{
	m_Functionmap.insert(std::make_pair(cmd, func));
}