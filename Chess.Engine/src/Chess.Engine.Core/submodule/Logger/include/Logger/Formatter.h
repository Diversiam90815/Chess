/*
==============================================================================
	Module			Formatter
	Description		Custom formatter class for the log messages
==============================================================================
*/

#pragma once

#include <spdlog/formatter.h>
#include <spdlog/details/log_msg.h>


class Formatter : public spdlog::formatter
{
public:
	Formatter();

	void							   format(const spdlog::details::log_msg &msg, spdlog::memory_buf_t &dest) override;

	std::unique_ptr<spdlog::formatter> clone() const override;

private:
	static std::string format_time(const spdlog::log_clock::time_point &tp);

	static std::string get_basename_no_ext(const char *fullpath);
};
