/*
==============================================================================
	Module			LoggerWrapper
	Description		A builder class to configure and create an SPDLog logger.
==============================================================================
*/

#pragma once

#include <memory>
#include <chrono>

enum class LogLevel
{
	Trace,
	Debug,
	Info,
	Warn,
	Error,
	Critical
};


namespace filesize
{
inline constexpr unsigned long long operator""_KB(unsigned long long value)
{
	return value * 1'024;
}

inline constexpr unsigned long long operator""_MB(unsigned long long value)
{
	return value * 1'024 * 1_KB;
}
} // namespace filesize


using namespace filesize;

namespace logging
{

class LoggerImpl;

void addConsoleOutput(LogLevel level, std::chrono::microseconds maxSkipDuration, const std::string &pattern = "[%Y-%m-%d %H:%M:%S.%e] [%l] %v");

void addFileOutput(LogLevel level, std::chrono::microseconds maxSkipDuration, const std::string &fileName, size_t maxFileSize, size_t maxFiles, bool rotateOnSession);

void addMSVCOutput(LogLevel level, bool checkForDebuggerPresent, std::chrono::microseconds maxSkipDuration);

void initializeLogger(const std::string &configFilePath);

void log(LogLevel level, const std::string &file, int line, const std::string &function, const std::string &msg);


/*
 *	@brief		Options for specifying custom sink's features
 */
template <typename LogOutput>
struct Options
{
public:
	LogOutput &setLevel(LogLevel level) noexcept
	{
		this->level = level;
		return static_cast<LogOutput &>(*this);
	}

	LogOutput &setMaxSkipDuration(std::chrono::microseconds maxSkipDuration) noexcept
	{
		this->maxSkipDuration = maxSkipDuration;
		return static_cast<LogOutput &>(*this);
	}

protected:
	LogLevel				  level = LogLevel::Info;
	std::chrono::microseconds maxSkipDuration;
};


/*
 *	@brief		Options to create a console output sink
 */
struct ConsoleOptions : Options<ConsoleOptions>
{
	ConsoleOptions()							= default;
	ConsoleOptions(const ConsoleOptions &other) = delete;
	~ConsoleOptions() { logging::addConsoleOutput(level, maxSkipDuration); }
};


/*
 *	@brief		Options to create a file output sink
 */
struct FileOptions : Options<FileOptions>
{
	FileOptions()						  = default;
	FileOptions(const FileOptions &other) = delete;
	~FileOptions() { logging::addFileOutput(level, maxSkipDuration, filename, maxFileSize, maxFiles, rotateOnSession); }

	FileOptions &setFilename(const std::string &filename);
	FileOptions &setMaxFileSize(size_t maxFileSize);
	FileOptions &setMaxFiles(size_t maxFiles);
	FileOptions &setRotateOnSession(bool rotateOnSession);

private:
	std::string filename		= "";
	size_t		maxFileSize		= 10_MB;
	size_t		maxFiles		= 3;
	bool		rotateOnSession = false;
};


/*
 *	@brief		Options to create a MSVC output sink
 */
struct MSVCOptions : Options<MSVCOptions>
{
public:
	MSVCOptions()						  = default;
	MSVCOptions(const MSVCOptions &other) = delete;
	~MSVCOptions() { logging::addMSVCOutput(level, checkForDebugger, maxSkipDuration); }

	MSVCOptions &checkForPresentDebugger(bool check);

private:
	bool checkForDebugger = false;
};


ConsoleOptions addConsoleOutput();

FileOptions	   addFileOutput();

MSVCOptions	   addMSVCOutput();

}; // namespace logging
