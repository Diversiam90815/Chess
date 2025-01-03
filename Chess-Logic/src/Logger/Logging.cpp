/*
  ==============================================================================

	Class:          Logger

	Description:    Logging instance for the project

  ==============================================================================
*/


#include "Logging.h"


Logging::Logging()
{
}


Logging::~Logging()
{
}


void Logging::initLogging()
{
	FileManager *fmg	 = FileManager::GetInstance();

	auto		 logPath = fmg->getLoggingPath();
	auto		 log	 = logPath / LogFile;

	logging::addMSVCOutput().checkForPresentDebugger(true).setLevel(LogLevel::Debug).setMaxSkipDuration(std::chrono::microseconds(mSlowLogTimeMS));

	logging::addFileOutput()
		.setFilename(log.string())
		.setLevel(LogLevel::Info)
		.setMaxFiles(10_MB)
		.setMaxFiles(10)
		.setMaxSkipDuration(std::chrono::microseconds(mSlowLogTimeMS))
		.setRotateOnSession(true);
}
