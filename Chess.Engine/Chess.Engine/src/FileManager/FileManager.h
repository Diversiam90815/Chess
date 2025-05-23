/*
  ==============================================================================
	Module:         FileManager
	Description:    Singleton of the file system manager (AppData path's need to be set from UI project due to WinUi's virtualization.
  ==============================================================================
*/

#pragma once

#include <filesystem>
#include <json.hpp>
#include <fstream>

#include "Parameters.h"
#include "Logging.h"
#include "NetworkAdapter.h"
#include "JsonConversion.h"


namespace fs = std::filesystem;


class FileManager
{
public:
	~FileManager() = default;

	// Singleton instance
	static FileManager			 *GetInstance();
	static void					  ReleaseInstance();

	// Getter - Setter
	fs::path					  getAppDataPath();
	void						  setAppDataPath(std::string path);

	// Path retrieval functions
	fs::path					  getLoggingPath();
	fs::path					  getSettingsPath();
	fs::path					  getUserSettingsPath();

	std::string					  readSettingFromFile(const std::string &setting);
	bool						  writeSettingToFile(const std::string &setting, const std::string &value);

	std::optional<NetworkAdapter> readSelectedNetworkAdapter();
	bool						  setSelectedNetworkAdapter(const NetworkAdapter &adpater);

private:
	FileManager() = default;

	bool		createDirectoryIfNeeded(const fs::path &directory);

	std::string mAppDataPath = "";
};
