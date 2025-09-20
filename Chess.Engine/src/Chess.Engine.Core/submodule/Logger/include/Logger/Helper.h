/*
==============================================================================
	Module			Helper
	Description		Helper methods needed throughout the project
==============================================================================
*/

#pragma once

#include <string>
#include <stdexcept>


namespace
{

// Function to trim whitespaces from a string
std::string trim(const std::string &s)
{
	size_t start = s.find_first_not_of(" \t\n\r");
	size_t end	 = s.find_last_not_of(" \t\n\r");
	return (start == std::string::npos) ? "" : s.substr(start, end - start + 1);
}


unsigned long long parseFileSize(const std::string &sizeStr)
{
	std::string s	= trim(sizeStr); // Trim in case of extra whitespaces

	// Extract numeric part
	size_t		pos = 0;
	while (pos < s.size() && (std::isdigit(s[pos]) || s[pos] == '.'))
	{
		++pos;
	}

	if (pos == 0)
		throw std::invalid_argument("No numeric part found in file size string : " + sizeStr);

	unsigned long long numberValue = std::stoull(s.substr(0, pos)); // Extract numeric value


	// Extract the unit
	std::string		   unit		   = trim(s.substr(pos));
	unit.erase(std::remove(unit.begin(), unit.end(), '_'), unit.end());

	// Convert unit to upper case
	for (auto &ch : unit)
	{
		ch = std::toupper(ch);
	}

	// Get correct unit
	if (unit == "KB")
		return numberValue * 1024ULL;
	else if (unit == "MB")
		return numberValue * 1024ULL * 1024ULL;
	else if (unit == "GB")
		return numberValue * 1024ULL * 1024ULL * 1024ULL;
	else if (unit.empty())
		return numberValue; // No unit means bytes
	else
		throw std::invalid_argument("Unkown file size unit: " + unit);
}

} // anonymous namespace