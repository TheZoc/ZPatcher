// ApplyPatch.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "FileUtils.h"
#include "LogSystem.h"
#include "ApplyPatch.h"

int main(int argc, char* argv[])
{
	using namespace ZPatcher;

	if (argc < 3)
	{
		fprintf(stderr, "ApplyPatch.exe <Patch file> <Directory to be patched>\n");
		exit(EXIT_SUCCESS);
	}

	// Store our targets
	std::string patchFileName = argv[1];
	std::string targetDirectory = argv[2];

	InitLogSystem("./");
	Log(LOG, "Patch file: %s", patchFileName.c_str());
	Log(LOG, "Directory to be upgraded: %s", targetDirectory.c_str());

	bool success = ApplyPatchFile(patchFileName, targetDirectory, "1");

	DestroyLogSystem();

	system("pause");

	if (success)
		exit(EXIT_SUCCESS);
	else
		exit(EXIT_FAILURE);
}

