// CreatePatch.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Lzma2Encoder.h"
#include "Lzma2Decoder.h"
#include "FileUtils.h"
#include "CreatePatch.h"
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

int main(int argc, char* argv[])
{
	using namespace ZPatcher;

	fprintf(stderr, "\nCreatePatch : ZPatcher [%d] v2.0 beta : %s\n", ENVBITS, __DATE__);
	fprintf(stderr, "Copyright (c) 2016-2017 Felipe \"Zoc\" Silveira : http://www.github.com/TheZoc/ZPatcher\n\n");

	if (argc < 4)
	{
		fprintf(stderr, "Usage: %s <old version directory> <new version directory> <output patch file>\n", argv[0]);
		exit(EXIT_SUCCESS);
	}

	// Log our entries. TODO: Parse the input file name, get the new patch file name (without the path).
	SetActiveLog("CreatePatch");

	// Store our targets
	std::string oldDirectory = argv[1];
	std::string newDirectory = argv[2];
	std::string outputFilename = argv[3];

	// Make sure all directories are represented in the same format
	NormalizeFileName(oldDirectory);
	NormalizeFileName(newDirectory);
	NormalizeFileName(outputFilename);

	Log(LOG, "Output patch file: %s", outputFilename.c_str());
	Log(LOG, "Old version directory: %s", oldDirectory.c_str());
	Log(LOG, "New version directory: %s", newDirectory.c_str());

	PatchFileList_t* patchFileList = GetDifferences(oldDirectory, newDirectory);

	CreatePatchFile(outputFilename, newDirectory, patchFileList);

	if (patchFileList)
		delete(patchFileList);

	DestroyLogSystem();

	exit(EXIT_SUCCESS);
}

