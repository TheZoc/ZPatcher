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
	extern std::vector<Patch>				g_Patches;
	extern std::vector<unsigned int>		g_BestPatchPath;
	extern unsigned long long				g_BestPathFileSize;
	extern unsigned long long				g_LatestVersion;

	std::string MD5File(std::string fileName);

	bool GetUpdatePath(const unsigned long long& sourceBuildNumber, const unsigned long long& targetBuildNumber, std::vector<unsigned int>& path, unsigned long long& pathFileSize);
	bool CheckForUpdates(const std::string& updateURL, const unsigned long long& currentBuildNumber);
	bool SimpleDownloadFile(const std::string& URL, const std::string& targetPath = "./");
	bool GetTargetCurrentVersion(const std::string& configFile, unsigned long long& version);
	bool SaveTargetNewVersion(const std::string& configFile, const unsigned long long& version);

}
#endif
