/*
  ==============================================================================
	Module:         Logger
	Description:    Logging instance for the project
  ==============================================================================
*/



#pragma once

#include "Logger/Logger.h"

#include <string>

#include "FileManager.h"
#include "LoggingHelper.h"


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
