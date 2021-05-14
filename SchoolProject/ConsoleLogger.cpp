#include "pch.h"

#include "ConsoleLogger.h"

//--------------------------------------------------------------------------------------
void ConsoleLogger::RedirectIOToConsole()
{
	AllocConsole();
	HANDLE stdHandle;
	int hConsole; 
	FILE* fp;
	stdHandle = GetStdHandle(STD_OUTPUT_HANDLE); 
	hConsole = _open_osfhandle((intptr_t)stdHandle, _O_TEXT);
	fp = _fdopen(hConsole, "w");
	freopen_s(&fp, "CONOUT$", "w", stdout); printf("[ConsoleLogger]: ON\n"); 
}





//--------------------------------------------------------------------------------------
void ConsoleLogger::LogInfo(const std::string& message, std::ostream& out) {
	auto now = std::chrono::system_clock::now();
	time_t time = std::chrono::system_clock::to_time_t(now);
	char* ctime = std::ctime(&time);
	ctime[strlen(ctime) - 1] = '\0';

	out << "[INFO]" << ctime << ": " << message << std::endl;
}





//--------------------------------------------------------------------------------------
void ConsoleLogger::LogWarning(const std::string& message, std::ostream& out) {
	auto now = std::chrono::system_clock::now();
	time_t time = std::chrono::system_clock::to_time_t(now);
	char* ctime = std::ctime(&time);
	ctime[strlen(ctime) - 1] = '\0';

	out << "[WARNING]" << ctime << ": " << message << std::endl;

}





//--------------------------------------------------------------------------------------
void ConsoleLogger::LogError(const std::string& message, std::ostream& out) {
	auto now = std::chrono::system_clock::now();
	time_t time = std::chrono::system_clock::to_time_t(now);
	char* ctime = std::ctime(&time);
	ctime[strlen(ctime) - 1] = '\0';

	out << "[ERROR]" << ctime << ": " << message << std::endl;

}
