/*
==============================================================================
	Module			LoggerImpl
	Description		Implement all logging functionalities using SPDLOG
==============================================================================
*/

#pragma once

#include <memory>
#include <string>
#include <chrono>

#include "LoggerWrapper.h" // For function delaration


namespace logging
{

class LoggerImpl
{

public:
	static LoggerImpl &GetInstance();

	void			   addConsoleOutput(LogLevel level, std::chrono::microseconds maxSkipDuration, const std::string &pattern);

	void addFileOutput(LogLevel level, std::chrono::microseconds maxSkipDuration, const std::string &fileName, size_t maxFileSize, size_t maxFiles, bool rotateOnSession);

	void addMSVCOutput(LogLevel level, bool checkForDebuggerPresent, std::chrono::microseconds maxSkipDuration);

	void initializeLogger(const std::string &configFilePath);

	void log(LogLevel level, const std::string &file, int line, const std::string &function, const std::string &msg);


private:
	LoggerImpl();
	LoggerImpl(const LoggerImpl &)			  = delete;
	LoggerImpl &operator=(const LoggerImpl &) = delete;

	class ImplData; // Defined in the cpp
	std::unique_ptr<ImplData> data;
};

} // namespace logging
