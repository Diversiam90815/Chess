/*
==============================================================================
	Module			LoggerImpl
	Description		Implement all logging functionalities using SPDLOG
==============================================================================
*/

#include "LoggerImpl.h"
#include "Helper.h"

#include <nlohmann/json.hpp>
#include <fstream>
#include <mutex>
#include <vector>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/msvc_sink.h>
#include <spdlog/formatter.h>
#include <spdlog/details/log_msg.h>

#include "Formatter.h"
#include "LoggerConfig.h"
#include "LoggerJSONConfigNames.h" // Defined names for JSON settings via CMake


namespace logging
{

spdlog::level::level_enum toSpdLogLevel(LogLevel level)
{
	switch (level)
	{
	case LogLevel::Trace: return spdlog::level::trace;
	case LogLevel::Debug: return spdlog::level::debug;
	case LogLevel::Info: return spdlog::level::info;
	case LogLevel::Warn: return spdlog::level::warn;
	case LogLevel::Error: return spdlog::level::err;
	case LogLevel::Critical: return spdlog::level::critical;
	default: return spdlog::level::off;
	}
}


LogLevel toLogLevel(const std::string &level)
{
	if (level == "trace")
		return LogLevel::Trace;
	if (level == "debug")
		return LogLevel::Debug;
	if (level == "info")
		return LogLevel::Info;
	if (level == "warn")
		return LogLevel::Warn;
	if (level == "error")
		return LogLevel::Error;
	if (level == "critical")
		return LogLevel::Critical;
	throw std::invalid_argument("Invalid log level: " + level);
}


// private data structure that holds our spdlog objects
class LoggerImpl::ImplData
{
public:
	std::shared_ptr<spdlog::logger> logger;
	std::vector<spdlog::sink_ptr>	sinks;
	std::mutex						mtx; // For thread-safe sink registration
};


LoggerImpl &LoggerImpl::GetInstance()
{
	static LoggerImpl sInstance;
	return sInstance;
}


LoggerImpl::LoggerImpl() : data(std::make_unique<ImplData>())
{
	data->logger = std::make_shared<spdlog::logger>("Logger", data->sinks.begin(), data->sinks.end());
	spdlog::register_logger(data->logger);
}


void LoggerImpl::addConsoleOutput(LogLevel level, std::chrono::microseconds maxSkipDuration, const std::string &pattern)
{
	auto sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
	sink->set_pattern(pattern);
	sink->set_level(toSpdLogLevel(level));
	sink->set_formatter(std::make_unique<Formatter>());

	{
		std::lock_guard<std::mutex> lock(data->mtx);
		data->sinks.push_back(sink);
		data->logger->sinks() = data->sinks;

		auto spdLevel		  = toSpdLogLevel(level);
		if (data->logger->level() > spdLevel)
		{
			data->logger->set_level(spdLevel);
		}
	}
}


void LoggerImpl::addFileOutput(LogLevel level, std::chrono::microseconds maxSkipDuration, const std::string &fileName, size_t maxFileSize, size_t maxFiles, bool rotateOnSession)
{
	if (fileName.empty())
		throw std::invalid_argument("File name cannot be empty");

	auto sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(fileName, maxFileSize, maxFiles, rotateOnSession);
	sink->set_level(toSpdLogLevel(level));
	sink->set_formatter(std::make_unique<Formatter>());

	{
		std::lock_guard<std::mutex> lock(data->mtx);
		data->sinks.push_back(sink);
		data->logger->sinks() = data->sinks;

		auto spdLevel		  = toSpdLogLevel(level);
		if (data->logger->level() > spdLevel)
		{
			data->logger->set_level(spdLevel);
		}
	}
}


void LoggerImpl::addMSVCOutput(LogLevel level, bool checkForDebuggerPresent, std::chrono::microseconds maxSkipDuration)
{
#ifdef _WIN32
	auto sink = std::make_shared<spdlog::sinks::msvc_sink_mt>(checkForDebuggerPresent);
	sink->set_level(toSpdLogLevel(level));
	sink->set_formatter(std::make_unique<Formatter>());

	{
		std::lock_guard<std::mutex> lock(data->mtx);
		data->sinks.push_back(sink);
		data->logger->sinks() = data->sinks;

		auto spdLevel		  = toSpdLogLevel(level);
		if (data->logger->level() > spdLevel)
		{
			data->logger->set_level(spdLevel);
		}
	}

#endif
}


unsigned long long getFileSize(const json &j, const std::string &key, unsigned long long defaultValue)
{
	if (j.contains(key))
	{
		if (j[key].is_number_unsigned())
		{
			return j[key].get<unsigned long long>();
		}
		else if (j[key].is_string())
		{
			return parseFileSize(j[key].get<std::string>());
		}
		else
		{
			throw std::runtime_error("Invalid type for file size in config for key: " + key);
		}
	}
	return defaultValue;
}


void LoggerImpl::initializeLogger(const std::string &configFilePath)
{
	LoggerConfig config(configFilePath);
	auto		 jsonConfig = config.getConfig();

	if (!jsonConfig.contains(LOGGER_CONFIG_SINK))
	{
		addConsoleOutput(LogLevel::Info, std::chrono::microseconds(0), "[%Y-%m-%d %H:%M:%S.%e] [%l] %v"); // Adding basic Console output for logger by default
		return;
	}


	for (auto &sinkConfig : jsonConfig[LOGGER_CONFIG_SINK])
	{
		// Use default type "console" and level "info" if missing.
		std::string type	 = sinkConfig.value(LOGGER_CONFIG_SINK_TYPE, "console");
		std::string levelStr = sinkConfig.value(LOGGER_CONFIG_LEVEL, "info");
		LogLevel	level	 = toLogLevel(levelStr);

		if (type == "console")
		{
			auto		maxSkipDuration = std::chrono::milliseconds(sinkConfig.value(LOGGER_CONFIG_MAX_SKIP_DURATION, 0));
			std::string pattern			= sinkConfig.value(LOGGER_CONFIG_PATTERN, "[%Y-%m-%d %H:%M:%S.%e] [%l] %v");

			addConsoleOutput(level, maxSkipDuration, pattern);
		}
		else if (type == "file")
		{
			auto		maxSkipDuration = std::chrono::microseconds(sinkConfig.value(LOGGER_CONFIG_MAX_SKIP_DURATION, 0));
			std::string fileName		= sinkConfig.value(LOGGER_CONFIG_FILE_NAME, "default.log");
			size_t		maxFileSize		= getFileSize(sinkConfig, LOGGER_CONFIG_MAX_FILE_SIZE, 10_MB);
			size_t		maxFiles		= sinkConfig.value(LOGGER_CONFIG_MAX_FILES, 3);
			bool		rotateOnSession = sinkConfig.value(LOGGER_CONFIG_ROTATE_ON_SESSION, false);
			addFileOutput(level, maxSkipDuration, fileName, maxFileSize, maxFiles, rotateOnSession);
		}
		else if (type == "msvc")
		{
			bool checkForDebugger = sinkConfig.value(LOGGER_CONFIG_CHECK_FOR_DEBUGGER, false);
			auto maxSkipDuration  = std::chrono::microseconds(sinkConfig.value(LOGGER_CONFIG_MAX_SKIP_DURATION, 0));
			addMSVCOutput(level, checkForDebugger, maxSkipDuration);
		}
	}
}


void LoggerImpl::log(LogLevel level, const std::string &file, int line, const std::string &function, const std::string &msg)
{
	if (!data->logger)
	{
		// If no logger is available, initialize a default console logger
		addConsoleOutput(LogLevel::Info, std::chrono::microseconds(0), "[%Y-%m-%d %H:%M:%S.%e] [%l] %v");
	}
	
	spdlog::source_loc loc{file.c_str(), line, function.c_str()}; // Create spdlog::source_loc object from provided data
	auto			   spdLevel = toSpdLogLevel(level);			  // Convert LogLevel to spdlog Level
	data->logger->log(loc, spdLevel, msg);						  // pass the source_loc along with msg and level
}



} // namespace logging
