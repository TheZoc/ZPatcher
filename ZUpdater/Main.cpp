//////////////////////////////////////////////////////////////////////////
//
// ZPatcher - Patcher system - Part of the ZUpdater suite
// Felipe "Zoc" Silveira - (c) 2016
//
//////////////////////////////////////////////////////////////////////////
//
// Main.cpp
// Defines the console-version of the ZUpdater entry point
//
//////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include <string>
#include <cstdio>
#include <cstdlib>
#include "ZUpdater.h"
#include "ApplyPatch.h"
#include "LogSystem.h"

// Check windows
#if _WIN32 || _WIN64
	#if _WIN64
		#define ENVBITS 64
	#else
		#define ENVBITS 32
	#endif
#endif

// Check GCC
#if __GNUC__
	#if __x86_64__ || __ppc64__
		#define ENVBITS 64
	#else
		#define ENVBITS 32
	#endif
#endif

#if _WIN32
#define WINDOWS_PAUSE() system("pause");
#else
#define WINDOWS_PAUSE()
#endif

// This downloads and applies the patch on the current folder.
int main()
{
	using namespace ZUpdater;

	fprintf(stdout, "\nZUpdater Command Line : ZPatcher [%d] v1.0 beta : %s\n", ENVBITS, __DATE__);
	fprintf(stdout, "Copyright (c) 2016 Felipe \"Zoc\" Silveira : http://www.github.com/TheZoc/ZPatcher\n\n");

#ifdef _WIN32
	// Check if there was an update to the updater itself
	bool shouldRestart = false;
	if (!SelfUpdate(shouldRestart))
	{
		WINDOWS_PAUSE();
		ZPatcher::DestroyLogSystem();
		exit(EXIT_FAILURE);
	}
	else
	{
		if (shouldRestart)
		{
			ZPatcher::DestroyLogSystem();
			exit(EXIT_SUCCESS);
		}
	}
#endif

	std::string					updateURL = "https://raw.githubusercontent.com/TheZoc/ZPatcher/master/tests/zpatcher_test.xml";
	std::string					targetDirectory = "./";
	std::string					versionFile = "zpatcher_test.zversion";
	uint64_t					currentVersion;

	ZPatcher::SetActiveLog("ZUpdater");

	// Check if we have version information on file. If not, we consider this is a fresh install.
	if (!GetTargetCurrentVersion(versionFile, currentVersion))
	{
		fprintf(stderr, "An error occurred while getting current application version.\n");
		WINDOWS_PAUSE();
		ZPatcher::DestroyLogSystem();
		exit(EXIT_FAILURE);
	}

	fprintf(stdout, "\nCurrent version: %llu\n\n", currentVersion);

	// Check the Updates URL for the XML file with updates and process it.
	if (!CheckForUpdates(updateURL, currentVersion))
	{
		fprintf(stderr, "An error occurred while checking for updates.\n");
		WINDOWS_PAUSE();
		ZPatcher::DestroyLogSystem();
		exit(EXIT_FAILURE);
	}

	fprintf(stdout, "\nLatest version available for download: %llu\n", GetLatestVersion());

	// If there is an updated version to download, download and apply it. (This might download multiple files).
	if (!DownloadAndApplyPatch(targetDirectory, versionFile, currentVersion))
	{
		WINDOWS_PAUSE();
		ZPatcher::DestroyLogSystem();
		exit(EXIT_FAILURE);
	}

	fprintf(stdout, "\n");
	WINDOWS_PAUSE();
	ZPatcher::DestroyLogSystem();
	exit(EXIT_SUCCESS);
}
