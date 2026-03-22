/*
  ==============================================================================
	Module:         Logger
	Description:    Logging instance for the project
  ==============================================================================
*/


#include "Logging.h"


void Logging::initLogging()
{
	FileManager *fmg	  = FileManager::GetInstance();

	auto		 logPath  = fmg->getLoggingPath();
	auto		 log	  = logPath / FileName::LogFile;

	std::string	 fileName = log.string();

	logging::addFileOutput()
		.setFilename(fileName)
		.setLevel(LogLevel::Info)
		.setMaxFileSize(10_MB)
		.setMaxFiles(10)
		.setMaxSkipDuration(std::chrono::microseconds(mSlowLogTimeMS))
		.setRotateOnSession(true);

	logging::addMSVCOutput().checkForPresentDebugger(true).setLevel(LogLevel::Debug).setMaxSkipDuration(std::chrono::microseconds(mSlowLogTimeMS));
}
