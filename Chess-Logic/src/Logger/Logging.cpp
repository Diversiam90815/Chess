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
	FileManager *fmg = FileManager::GetInstance();

	auto		 logPath = fmg->getLoggingPath();
	auto log = logPath / LogFile;

}
