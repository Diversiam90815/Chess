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

	static void logSystemInfo()
	{
		LOG_INFO("=======================================");
		LOG_INFO("============ SYSTEM INFO ==============");

		LOG_INFO("Project Name:\t\t {}", BuildInfo::Name);
		LOG_INFO("Project Version:\t\t {}", BuildInfo::Version);

		LOG_INFO("Build Time:\t\t {}", BuildInfo::Timestamp);
		LOG_INFO("Branch:\t\t\t {}", BuildInfo::Branch);
		LOG_INFO("Commit SHA:\t\t {}", BuildInfo::CommitSHA);
		LOG_INFO("Commit SHA Short:\t {}", BuildInfo::CommitSHAShort);

		LOG_INFO("Generator:\t\t {}", BuildInfo::Generator);
		LOG_INFO("CPP Version:\t\t {}", BuildInfo::CxxStandard);

		LOG_INFO("System Name:\t\t {}", BuildInfo::SystemName);
		LOG_INFO("System Version:\t\t {}", BuildInfo::SystemVersion);
		LOG_INFO("System CPU:\t\t {}", BuildInfo::SystemCPU);

		LOG_INFO("Compiler:\t\t\t {}", BuildInfo::CompilerID);
		LOG_INFO("Compiler Version:\t {}", BuildInfo::CompilerVersion);


		LOG_INFO("================= END =================");
		LOG_INFO("=======================================");
	}
};
