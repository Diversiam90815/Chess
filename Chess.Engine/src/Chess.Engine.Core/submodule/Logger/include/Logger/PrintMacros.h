/*
  ==============================================================================

	Module			PrintMacros
	Description		Definitions of print macros to be used in projects

  ==============================================================================
*/

#pragma once
#include <fmt/format.h>
#include "LoggerWrapper.h"


#define LOG(level, fmtStr, ...)	  logging::log(LogLevel::level, __FILE__, __LINE__, __FUNCTION__, fmt::format(fmtStr, ##__VA_ARGS__))

#define LOG_TRACE(fmtStr, ...)	  LOG(Trace, fmtStr, ##__VA_ARGS__)
#define LOG_DEBUG(fmtStr, ...)	  LOG(Debug, fmtStr, ##__VA_ARGS__)
#define LOG_INFO(fmtStr, ...)	  LOG(Info, fmtStr, ##__VA_ARGS__)
#define LOG_WARNING(fmtStr, ...)  LOG(Warn, fmtStr, ##__VA_ARGS__)
#define LOG_ERROR(fmtStr, ...)	  LOG(Error, fmtStr, ##__VA_ARGS__)
#define LOG_CRITICAL(fmtStr, ...) LOG(Critical, fmtStr, ##__VA_ARGS__)
