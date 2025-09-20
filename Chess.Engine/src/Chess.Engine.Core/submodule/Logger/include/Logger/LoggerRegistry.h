/*
  ==============================================================================

	Module			LoggerRegistry
	Description		Singleton of the Logger Registry

  ==============================================================================
*/

#pragma once

#include <memory>
#include <string>
#include <vector>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/base_sink.h>
#include <spdlog/sinks/dup_filter_sink.h>
#include <spdlog/logger.h>

#include "Helper.h"


class LoggerRegistry
{
public:
	static LoggerRegistry &sInstance()
	{
		static LoggerRegistry instance;
		return instance;
	}

	std::vector<spdlog::sink_ptr> &sinks()	{return mSinks;}

	LogLevel &defaultLogLevel()	{return mDefaultLogLevel;}

private:
	LoggerRegistry()												= default;

	~LoggerRegistry()												= default;
	
	LoggerRegistry(const LoggerRegistry &)							= delete;
	
	LoggerRegistry				 &operator=(const LoggerRegistry &) = delete;

	std::vector<spdlog::sink_ptr> mSinks;

	LogLevel					  mDefaultLogLevel = {LogLevel::Info};
};
