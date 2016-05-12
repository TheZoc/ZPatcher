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
// Lets avoid adding an unnecessary header here.
typedef unsigned long       DWORD;

namespace ZUpdater
{
	struct Patch
	{
		unsigned long long	sourceBuildNumber;
		unsigned long long	targetBuildNumber;

		std::string         fileURL;
		DWORD               fileLength;
		std::string			fileMD5;

	};

	// Lazy++
// 	extern std::vector<Patch>				g_Patches;
// 	extern std::vector<unsigned int>		g_BestPatchPath;
// 	extern unsigned long long				g_BestPathFileSize;
//	extern unsigned long long				g_LatestVersion;

	/**
	 * Download the Update XML from the server and process it. The output is stored in g_Patches.
	 * Returns if file was downloaded and processed successfully
	 */
	bool CheckForUpdates(const std::string& updateURL, const unsigned long long& currentBuildNumber);

	/**
	 * Returns g_LatestVersion.
	 */
	unsigned long long GetLatestVersion();

	/**
	 * With all the patches stored in g_Patches, find the best path to download updates, using the file download size as the value that should be minimized.
	 * Returns true if there is a path from the source version to the target version (i.e. if the update is possible)
	 */
	bool GetSmallestUpdatePath(const unsigned long long& sourceBuildNumber, const unsigned long long& targetBuildNumber, std::vector<unsigned int>& path, unsigned long long& pathFileSize);

	/**
	 *
	 */
	bool DownloadAndApplyPatch(std::string targetDirectory, std::string targetVersionFile, unsigned long long targetCurrentVersion);

	/**
	 * Returns true if it was able to determine the current version stored in configFile.
	 * The found version is returned in the "version" variable.
	 * NOTE: If no file is found, it returns true and version gets the value 0 (i.e. No previous version found, perform full instalation)
 	 */
	bool GetTargetCurrentVersion(const std::string& configFile, unsigned long long& version);

	/**
	 * Updates given configFile to store the supplied version
	 */
	bool SaveTargetNewVersion(const std::string& configFile, const unsigned long long& version);

	/**
	 * Calculate the MD5 hash of a file
	 */
	std::string MD5File(std::string fileName);

	/**
	 * Download file, from given URL, without renaming, to given targetPath.
	 * Note: The filename to be saved is the last part (i.e. after last slash) of given URL. Complex URLs might be a problem for this function.
	 */
	bool SimpleDownloadFile(const std::string& URL, const std::string& targetPath = "./");

}
#endif
