/*
==============================================================================
	Module			LoggerWrapper
	Description		A builder class to configure and create an SPDLog logger.
==============================================================================
*/

#include "LoggerWrapper.h"
#include "LoggerImpl.h"


using namespace filesize;

namespace logging
{

void addConsoleOutput(LogLevel level, std::chrono::microseconds maxSkipDuration, const std::string &pattern)
{
	LoggerImpl::GetInstance().addConsoleOutput(level, maxSkipDuration, pattern);
}


void addFileOutput(LogLevel level, std::chrono::microseconds maxSkipDuration, const std::string &fileName, size_t maxFileSize, size_t maxFiles, bool rotateOnSession)
{
	LoggerImpl::GetInstance().addFileOutput(level, maxSkipDuration, fileName, maxFileSize, maxFiles, rotateOnSession);
}


void addMSVCOutput(LogLevel level, bool checkForDebuggerPresent, std::chrono::microseconds maxSkipDuration)
{
	LoggerImpl::GetInstance().addMSVCOutput(level, checkForDebuggerPresent, maxSkipDuration);
}


void initializeLogger(const std::string &configFilePath)
{
	LoggerImpl::GetInstance().initializeLogger(configFilePath);
}


void log(LogLevel level, const std::string &file, int line, const std::string &function, const std::string &msg)
{
	LoggerImpl::GetInstance().log(level, file, line, function, msg);
}



// Options:

// File Options:

FileOptions &FileOptions::setFilename(const std::string &filename)
{
	this->filename = filename;
	return *this;
}

FileOptions &FileOptions::setMaxFileSize(size_t maxFileSize)
{
	this->maxFileSize = maxFileSize;
	return *this;
}

FileOptions &FileOptions::setMaxFiles(size_t maxFiles)
{
	this->maxFiles = maxFiles;
	return *this;
}

FileOptions &FileOptions::setRotateOnSession(bool rotateOnSession)
{
	this->rotateOnSession = rotateOnSession;
	return *this;
}


// MSVC Options:

MSVCOptions &MSVCOptions::checkForPresentDebugger(bool check)
{
	this->checkForDebugger = check;
	return *this;
}


// Console Options:

ConsoleOptions addConsoleOutput()
{
	return {};
}

FileOptions addFileOutput()
{
	return {};
}

MSVCOptions addMSVCOutput()
{
	return {};
}

} // namespace logging
