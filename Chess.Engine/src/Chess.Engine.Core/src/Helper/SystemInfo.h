/*
  ==============================================================================
	Module:         SystemInfo
	Description:    Logging system info
  ==============================================================================
*/

#pragma once

#include "buildinfo.h"
#include "Logging.h"


class SystemInfo
{
public:
	SystemInfo()  = default;
	~SystemInfo() = default;

	void logSystemInfo()
	{
		LOG_INFO("=======================================");
		LOG_INFO("============ SYSTEM INFO ==============");

		LOG_INFO("");

		LOG_INFO("================= END =================");
		LOG_INFO("=======================================");
	}
};
