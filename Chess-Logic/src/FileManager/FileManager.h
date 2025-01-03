/*
  ==============================================================================

	Class:          FileManager

	Description:    Singleton of the file system manager (AppData path's need to be set from UI project due to WinUi's virtualization.

  ==============================================================================
*/

#pragma once

#include <filesystem>
#include "Parameters.h"

namespace fs = std::filesystem;


class FileManager
{
public:
	~FileManager() = default;

	// Singleton instance
	static FileManager *GetInstance();
	static void			ReleaseInstance();

	// Getter - Setter
	fs::path			getAppDataPath();
	void				setAppDataPath(std::string path);

	// Path retrieval functions
	fs::path			getLoggingPath();

private:
	FileManager() = default;

	bool		createDirectoryIfNeeded(const fs::path &directory);

	std::string mAppDataPath = "";
};
