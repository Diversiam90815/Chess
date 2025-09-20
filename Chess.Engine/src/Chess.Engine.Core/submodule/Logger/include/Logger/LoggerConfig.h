/*
==============================================================================
	Module			LoggerConfig
	Description		Initializing the logger with a JSON config file
==============================================================================
*/

#pragma once

#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>


using json = nlohmann::json;

class LoggerConfig
{
public:
	LoggerConfig(const std::string &configFilePath)
	{
		std::ifstream file(configFilePath);
		if (!file.is_open())
		{
			throw std::runtime_error("Could not open config file!");
		}

		file >> config;
	}

	json getConfig() const { return config; }

private:
	json config;
};