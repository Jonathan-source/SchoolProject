#pragma once
#include "pch.h"

#include <chrono>
#include <ctime>

#pragma warning(disable : 4996)

class ConsoleLogger
{
public:
	ConsoleLogger() = default;
	ConsoleLogger(const ConsoleLogger& other) = delete;
	ConsoleLogger(ConsoleLogger&& other) = delete;
	ConsoleLogger& operator=(const ConsoleLogger& other) = delete;
	ConsoleLogger& operator=(ConsoleLogger&& other) = delete;
	virtual ~ConsoleLogger() = default;

	static void RedirectIOToConsole();

	static void LogInfo(const std::string& message, std::ostream& out = std::cout);
	static void LogWarning(const std::string& message, std::ostream& out = std::cout);
	static void LogError(const std::string& message, std::ostream& out = std::cout);

};

