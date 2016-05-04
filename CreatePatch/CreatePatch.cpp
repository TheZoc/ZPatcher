// CreatePatch.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Lzma2Encoder.h"
#include "Lzma2Decoder.h"
#include "FileUtil.h"

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

	fprintf(stderr, "\nRemoved files:\n");
	for (std::vector<std::string>::iterator itr = patchFileList->RemovedFileList.begin(); itr < patchFileList->RemovedFileList.end(); ++itr)
	{
		fprintf(stderr, "%s\n", itr->c_str());
	}

	fprintf(stderr, "\nModified files:\n");
	for (std::vector<std::string>::iterator itr = patchFileList->ModifiedFileList.begin(); itr < patchFileList->ModifiedFileList.end(); ++itr)
	{
		fprintf(stderr, "%s\n", itr->c_str());
	}

	fprintf(stderr, "\nAdded files:\n");
	for (std::vector<std::string>::iterator itr = patchFileList->AddedFileList.begin(); itr < patchFileList->AddedFileList.end(); ++itr)
	{
		fprintf(stderr, "%s\n", itr->c_str());
	}

	exit(EXIT_SUCCESS);

}
