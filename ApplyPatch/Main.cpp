// ApplyPatch.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <cstdlib>
#include "FileUtils.h"
#include "LogSystem.h"
#include "ApplyPatch.h"

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

int main(int argc, char* argv[])
{
	using namespace ZPatcher;

	fprintf(stderr, "\nApplyPatch : ZPatcher [%d] v1.0 beta : %s\n", ENVBITS, __DATE__);
	fprintf(stderr, "Copyright (c) 2016 Felipe \"Zoc\" Silveira : http://www.github.com/TheZoc/ZPatcher\n\n");

	if (argc < 3)
	{
		fprintf(stderr, "Usage: %s <Patch file> <Directory to be patched>\n", argv[0]);
		exit(EXIT_SUCCESS);
	}

	// Store our targets
	std::string patchFileName = argv[1];
	std::string targetDirectory = argv[2];

	InitLogSystem("./");
	Log(LOG, "Patch file: %s", patchFileName.c_str());
	Log(LOG, "Directory to be upgraded: %s", targetDirectory.c_str());

	uint64_t version = 1;
	bool success = ApplyPatchFile(patchFileName, targetDirectory, version); // TODO: Add a version file

	DestroyLogSystem();

//	system("pause");

	if (success)
		exit(EXIT_SUCCESS);
	else
		exit(EXIT_FAILURE);
}
