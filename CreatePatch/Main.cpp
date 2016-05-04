// CreatePatch.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Lzma2Encoder.h"
#include "Lzma2Decoder.h"
#include "FileUtils.h"
#include "CreatePatch.h"

int main(int argc, char* argv[])
{
	using namespace ZPatcher;

	if (argc < 4)
	{
		fprintf(stderr, "CreatePatch.exe <old version directory> <new version directory> <output patch file>\n");
		exit(EXIT_SUCCESS);
	}

	// Store our targets
	std::string oldDirectory = argv[1];
	std::string newDirectory = argv[2];
	std::string outputFilename = argv[3];

	// Make sure all directories are represented in the same format
	NormalizeFileName(oldDirectory);
	NormalizeFileName(newDirectory);
	NormalizeFileName(outputFilename);

	PatchFileList_t* patchFileList = GetDifferences(oldDirectory, newDirectory);

	CreatePatchFile(outputFilename, newDirectory, patchFileList);

	system("pause");

	exit(EXIT_SUCCESS);

}
