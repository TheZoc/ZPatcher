//////////////////////////////////////////////////////////////////////////
//
// ZPatcher - Patcher system - Part of the ZUpdater suite
// Felipe "Zoc" Silveira - (c) 2016
//
//////////////////////////////////////////////////////////////////////////
//
// DownloadFileWriter.h
// Class to download a file from an URL and save to disk
//
//////////////////////////////////////////////////////////////////////////


#ifndef _ZUPDATER_H_
#define _ZUPDATER_H_

#include <vector>
#include <string>

#ifdef _WIN32
	#include <windows.h>
#endif // !_WIN32

// Lets avoid adding an unnecessary header here.
typedef unsigned long       DWORD;

namespace ZUpdater
{
	struct Patch
	{
		uint64_t	sourceBuildNumber;
		uint64_t	targetBuildNumber;

		std::string         fileURL;
		DWORD               fileLength;
		std::string			fileMD5;

	};

	/**
	 * Download the Update XML from the server and process it. The output is stored in g_Patches.
	 * Returns if file was downloaded and processed successfully
	 */
	bool CheckForUpdates(const std::string& updateURL, const uint64_t& currentBuildNumber);

	/**
	 * Returns g_LatestVersion.
	 */
	uint64_t GetLatestVersion();

	/**
	 * With all the patches stored in g_Patches, find the best path to download updates, using the file download size as the value that should be minimized.
	 * Returns true if there is a path from the source version to the target version (i.e. if the update is possible)
	 */
	bool GetSmallestUpdatePath(const uint64_t& sourceBuildNumber, const uint64_t& targetBuildNumber, std::vector<unsigned int>& path, uint64_t& pathFileSize);

	/**
	 *
	 */
	bool DownloadAndApplyPatch(std::string targetDirectory, std::string targetVersionFile, uint64_t targetCurrentVersion);

	/**
	 * Returns true if it was able to determine the current version stored in configFile.
	 * The found version is returned in the "version" variable.
	 * NOTE: If no file is found, it returns true and version gets the value 0 (i.e. No previous version found, perform full instalation)
 	 */
	bool GetTargetCurrentVersion(const std::string& configFile, uint64_t& version);

	/**
	 * Updates given configFile to store the supplied version
	 */
	bool SaveTargetNewVersion(const std::string& configFile, const uint64_t& version);

	/**
	 * Calculate the MD5 hash of a file
	 */
	std::string MD5File(std::string fileName);

	/**
	 * Download file, from given URL, without renaming, to given targetPath.
	 * Note: The filename to be saved is the last part (i.e. after last slash) of given URL. Complex URLs might be a problem for this function.
	 */
	bool SimpleDownloadFile(const std::string& URL, const std::string& targetPath = "./");


	//////////////////////////////////////////////////////////////////////////
	// Windows specific stuff

	/**
	 * If the updater finds a file with it's own name with an appended 'a' character (e.g. For Zupdater.exe, searches for ZUpdatera.exe),
	 * it will replace itself with the updated found version and restart the application.
	 * it will return true if no write error happened.
	 * Please note, YOU (yes, you!) need to explicitly finish the application after calling this function and it returns updateFound == true
	 */
	bool SelfUpdate(bool &updateFound);
}
#endif
