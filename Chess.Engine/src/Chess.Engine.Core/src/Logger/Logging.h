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

/// <summary>
/// Provides logging functionality, including initialization of the logging system.
/// </summary>
class Logging
{
public:
	Logging()  = default;
	~Logging() = default;

	void initLogging();

private:
	std::string mLoggerName;
	const int	mSlowLogTimeMS = 500;
};
