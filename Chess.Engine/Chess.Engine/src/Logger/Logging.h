/*
  ==============================================================================
	Module:         Logger
	Description:    Logging instance for the project
  ==============================================================================
*/



#pragma once

#include <Logger.h>

#include <string>

#include "FileManager.h"
#include "LoggingHelper.h"


class Logging
{
public:
	Logging();
	~Logging();

	void initLogging();

private:
	std::string mLoggerName;
	const int	mSlowLogTimeMS = 500;
};
