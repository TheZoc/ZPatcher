// ApplyPatch.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "FileUtils.h"
#include "LogSystem.h"
#include "ApplyPatch.h"

int main(int argc, char* argv[])
{
	using namespace ZPatcher;

	fprintf(stderr, "\nApplyPatch : ZPatcher [%d] v1.0 beta : %s\n", _INTEGRAL_MAX_BITS, __DATE__);
	fprintf(stderr, "Copyright (c) 2016 Felipe \"Zoc\" Silveira : http://www.github.com/TheZoc/ZPatcher\n\n");

	if (argc < 3)
	{
		fprintf(stderr, "Usage: ApplyPatch.exe <Patch file> <Directory to be patched>\n");
		exit(EXIT_SUCCESS);
	}

	// Store our targets
	std::string patchFileName = argv[1];
	std::string targetDirectory = argv[2];

	InitLogSystem("./");
	Log(LOG, "Patch file: %s", patchFileName.c_str());
	Log(LOG, "Directory to be upgraded: %s", targetDirectory.c_str());

	unsigned long long version = 1;
	bool success = ApplyPatchFile(patchFileName, targetDirectory, version); // TODO: Add a version file

	DestroyLogSystem();

//	system("pause");

	if (success)
		exit(EXIT_SUCCESS);
	else
		exit(EXIT_FAILURE);
}

