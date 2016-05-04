//////////////////////////////////////////////////////////////////////////
//
// ZPatcher - Patcher system - Part of the ZUpdater suite
// Felipe "Zoc" Silveira - (c) 2016
//
//////////////////////////////////////////////////////////////////////////
//
// CreatePatch.cpp
// Functions to create the Patch data
//
//////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "CreatePatch.h"
#include "FileUtils.h"
#include "Lzma2Encoder.h"


ZPatcher::PatchFileList_t* ZPatcher::GetDifferences(std::string& oldVersion, std::string& newVersion)
{
	PatchFileList_t* patchFileList = new PatchFileList_t();

	std::vector<std::string> oldVersionFileList;
	GetFilesInDirectory(oldVersion, "", oldVersionFileList);

	std::vector<std::string> newVersionFileList;
	GetFilesInDirectory(newVersion, "", newVersionFileList);

	// Sort them now to avoid worries later. (Easier to find added/deleted files)
	std::sort(oldVersionFileList.begin(), oldVersionFileList.end());
	std::sort(newVersionFileList.begin(), newVersionFileList.end());

	unsigned int oldFileIndex = 0;
	unsigned int newFileIndex = 0;

	fprintf(stdout, "Detecting file differences...");

	while (oldFileIndex < oldVersionFileList.size() && newFileIndex < newVersionFileList.size())
	{
		float progress = (oldFileIndex + newFileIndex) * 100.0f / (oldVersionFileList.size() + newVersionFileList.size());
		PrintFileCompareProgressBar(progress);

		const std::string& oldFileName = oldVersionFileList[oldFileIndex];
		const std::string& newFileName = newVersionFileList[newFileIndex];

		if (oldFileName == newFileName)
		{
			// Check if we are dealing with directories.
			size_t fileNameLength = oldFileName.length();
			if (fileNameLength > 0 && oldFileName[fileNameLength - 1] != '/')
			{
				// Check if the files have the same contents
				if (!AreFilesIdentical(oldVersion + "/" + oldFileName, newVersion + "/" + newFileName))
				{
					patchFileList->ModifiedFileList.push_back(oldFileName);
				}
			}

			oldFileIndex++;
			newFileIndex++;

		}
		else if (oldFileName < newFileName)
		{
			// The new version doesn't contain this file.
			patchFileList->RemovedFileList.push_back(oldFileName);
			oldFileIndex++;
		}
		else
		{
			// The old version doesn't contain this file.
			patchFileList->AddedFileList.push_back(newFileName);
			newFileIndex++;
		}
	}

	// Okay, one of the lists is smaller than the other. Add or Remove all missing files, depending on the file list.
	while (oldFileIndex < oldVersionFileList.size())
	{
		float progress = (oldFileIndex + newFileIndex) * 100.0f / (oldVersionFileList.size() + newVersionFileList.size());
		PrintFileCompareProgressBar(progress);

		patchFileList->RemovedFileList.push_back(oldVersionFileList[oldFileIndex]);
		oldFileIndex++;
	}

	while (newFileIndex < newVersionFileList.size())
	{
		float progress = (newFileIndex + newFileIndex) * 100.0f / (newVersionFileList.size() + newVersionFileList.size());
		PrintFileCompareProgressBar(progress);

		patchFileList->RemovedFileList.push_back(newVersionFileList[newFileIndex]);
		newFileIndex++;
	}

	PrintFileCompareProgressBar(100.0f);
	fprintf(stdout, "\n");

	return patchFileList;
}

void ZPatcher::PrintFileCompareProgressBar(float Percentage)
{
	int barWidth = 80;

	fprintf(stdout, "\xd[");

	int pos = (int)(barWidth * Percentage / 100.0f);
	for (int i = 0; i < barWidth; ++i)
	{
		if (i < pos) fprintf(stdout, "=");
		else if (i == pos) fprintf(stdout, ">");
		else fprintf(stdout, " ");
	}
	fprintf(stdout, "] %0.2f %%     ", Percentage);

}

void ZPatcher::CreatePatchFile(FILE* patchFile, std::string& newVersionPath, PatchFileList_t* patchFileList)
{
	// Initialize our custom LZMA2 Encoder
	CLzma2EncHandle hLzma2Enc = InitLzma2Encoder();

	// Write the file header, including our custom LZMA2 props
	Byte props = Lzma2Enc_WriteProperties(hLzma2Enc);
	WritePatchFileHeader(patchFile, props);

	// Process the file list. We start with the removed files - Why not? :)
	for (std::vector<std::string>::iterator itr = patchFileList->RemovedFileList.begin(); itr < patchFileList->RemovedFileList.end(); ++itr)
	{
		WriteFileInfo(patchFile, Patch_File_Delete, itr->c_str());
	}

	for (std::vector<std::string>::iterator itr = patchFileList->AddedFileList.begin(); itr < patchFileList->AddedFileList.end(); ++itr)
	{
		size_t fileNameLength = itr->length();
		if (fileNameLength > 0 && (*itr)[fileNameLength - 1] != '/')
		{
			WriteFileInfo(patchFile, Patch_File_AddReplace, itr->c_str());
			std::string localPath = newVersionPath + "/" + *itr;
			WriteCompressedFile(hLzma2Enc, localPath, patchFile);
		}
		else
		{
			WriteFileInfo(patchFile, Patch_Dir_Add, itr->c_str());
		}
	}

	// Right now, we replace both the modified files and the added files
	for (std::vector<std::string>::iterator itr = patchFileList->ModifiedFileList.begin(); itr < patchFileList->ModifiedFileList.end(); ++itr)
	{
		WriteFileInfo(patchFile, Patch_File_AddReplace, itr->c_str());
		std::string localPath = newVersionPath + "/" + *itr;
		WriteCompressedFile(hLzma2Enc, localPath, patchFile);
	}

	DestroyLzma2EncHandle(hLzma2Enc);
}

void ZPatcher::CreatePatchFile(std::string& pathFileName, std::string& newVersionPath, PatchFileList_t* patchFileList)
{
	FILE* patchFile;
	errno_t err = 0;

	err = fopen_s(&patchFile, pathFileName.c_str(), "wb");
	assert(err == 0);

	CreatePatchFile(patchFile, newVersionPath, patchFileList);

	fclose(patchFile);
}

