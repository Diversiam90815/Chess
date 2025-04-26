/*
  ==============================================================================
	Module:         Conversion
	Description:	Conversion functions to be used throughout the project
  ==============================================================================
*/

#pragma once

#include <string>


static std::string WStringToStdString(const std::wstring &wstr)
{
	if (wstr.empty())
		return {};

	std::string str{};
	size_t		size{};
	str.resize(wstr.length());
	wcstombs_s(&size, &str[0], str.size() + 1, wstr.c_str(), wstr.size());
	return str;
}
