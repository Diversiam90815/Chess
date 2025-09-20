/*
==============================================================================
	Module			Formatter
	Description		Custom formatter class for the log messages
==============================================================================
*/

#include "Formatter.h"


Formatter::Formatter() {}


void Formatter::format(const spdlog::details::log_msg &msg, spdlog::memory_buf_t &dest)
{
	auto  timeString  = format_time(msg.time);
	auto &levelString = spdlog::level::to_string_view(msg.level);
	auto  threadID	  = msg.thread_id;
	auto  fileName	  = get_basename_no_ext(msg.source.filename);

	auto		funcName = (msg.source.funcname ? msg.source.funcname : "");

	std::string userMessage(msg.payload.begin(), msg.payload.end());

	auto		formatted_line = fmt::format("{:<25.25} {:>8} {:<8.8} {:<20.20} {:<45.45} {}", timeString, threadID, levelString, fileName, funcName, userMessage);

	fmt::format_to(std::back_inserter(dest), "{}\n", formatted_line);
}


std::unique_ptr<spdlog::formatter> Formatter::clone() const
{
	return std::make_unique<Formatter>();
}


std::string Formatter::format_time(const spdlog::log_clock::time_point &tp)
{
	using namespace std::chrono;

	auto		duration = tp.time_since_epoch();
	auto		secs	 = duration_cast<seconds>(duration);
	auto		ms		 = duration_cast<milliseconds>(duration) - duration_cast<milliseconds>(secs);

	std::time_t t		 = static_cast<std::time_t>(secs.count());
	std::tm		tm_local;

#if defined(_WIN32)
	localtime_s(&tm_local, &t);
#else
	localtime_r(&t, &tm_local);
#endif

	char dateTimeBuf[64];
	std::strftime(dateTimeBuf, sizeof(dateTimeBuf), "%Y-%m-%d %H:%M:%S", &tm_local);

	// Append milliseconds
	// e.g. "2024-12-23 13:13:55.123"
	std::ostringstream oss;
	oss << dateTimeBuf << '.' << std::setw(3) << std::setfill('0') << ms.count();
	return oss.str();
}


std::string Formatter::get_basename_no_ext(const char *fullpath)
{
	if (!fullpath || !*fullpath)
		return "Unknown File";

	std::string_view sv(fullpath);
	size_t			 lastSlash = sv.find_last_of("/\\");
	if (lastSlash != std::string_view::npos)
		sv.remove_prefix(lastSlash + 1);

	size_t dotPos = sv.rfind('.');
	if (dotPos != std::string_view::npos)
		sv.remove_suffix(sv.size() - dotPos);

	return std::string(sv);
}
