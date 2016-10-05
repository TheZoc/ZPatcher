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
#include <algorithm>
#include <cassert>
#include <cerrno>
#include "LogSystem.h"

void ZPatcher::PrintCreatePatchProgressBar(const float& Percentage, const uint64_t& leftAmount, const uint64_t& rightAmount)
{
	static const int bufferSize = 30;
	char buffer[bufferSize];

	// Write file size remaining to a buffer first, so the number of characters
	//   can be subtracted from the maximum progress bar size
	int charsForPercentage = snprintf(buffer, bufferSize, "%0.2f%% %llu/%llu ", Percentage, leftAmount, rightAmount);

	static const int fixedChars = 4; // Number of static progress bar chars ("[" and "] ")
	int barWidth = 80 - fixedChars - charsForPercentage;

#ifdef  _WIN32
	fprintf(stdout, "\xd[");
#else
	// http://stackoverflow.com/a/6774395
	fprintf(stdout, "\r\033[K[");
#endif

	int pos = (int)(barWidth * Percentage / 100.0f);
	for (int i = 0; i < barWidth; ++i)
	{
		if (i < pos) fprintf(stdout, "=");
		else if (i == pos) fprintf(stdout, ">");
		else fprintf(stdout, " ");
	}
	fprintf(stdout, "] ");
	fputs(buffer, stdout);
	fflush(stdout);
}

ZPatcher::PatchFileList_t* ZPatcher::GetDifferences(std::string& oldVersion, std::string& newVersion, ProgressCallback progressFunction)
{
	PatchFileList_t* patchFileList = new PatchFileList_t();

	std::vector<std::string> oldVersionFileList;
	GetFilesInDirectory(oldVersionFileList, oldVersion);

	std::vector<std::string> newVersionFileList;
	GetFilesInDirectory(newVersionFileList, newVersion);

	// Sort them now to avoid worries later. (Easier to find added/deleted files)
	std::sort(oldVersionFileList.begin(), oldVersionFileList.end());
	std::sort(newVersionFileList.begin(), newVersionFileList.end());

	unsigned int oldFileIndex = 0;
	unsigned int newFileIndex = 0;

	fprintf(stdout, "Detecting file differences between folders...\n");

	while (oldFileIndex < oldVersionFileList.size() && newFileIndex < newVersionFileList.size())
	{
		float progress = (oldFileIndex + newFileIndex) * 100.0f / (oldVersionFileList.size() + newVersionFileList.size());
		progressFunction(progress, oldFileIndex + newFileIndex, oldVersionFileList.size() + newVersionFileList.size());

		const std::string& oldFileName = oldVersionFileList[oldFileIndex];
		const std::string& newFileName = newVersionFileList[newFileIndex];

		if (oldFileName == newFileName)
		{
			// Check if we are dealing with directories.
			size_t fileNameLength = oldFileName.length();
			if (fileNameLength > 0 && oldFileName[fileNameLength - 1] != '/')
			{
				// Check if the files have the same contents
				bool identical;
				bool success = AreFilesIdentical(oldVersion + "/" + oldFileName, newVersion + "/" + newFileName, identical);

				assert(success == true); // TODO: Handle this.

				if (success != true)
				{
					fprintf(stderr, "\n\nError comparing files! Patch file might be inconsistent! Check the logs for details.\n\n");
				}

				if (success && !identical)
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
		progressFunction(progress, oldFileIndex + newFileIndex, oldVersionFileList.size() + newVersionFileList.size());

		patchFileList->RemovedFileList.push_back(oldVersionFileList[oldFileIndex]);
		oldFileIndex++;
	}

	while (newFileIndex < newVersionFileList.size())
	{
		float progress = (newFileIndex + newFileIndex) * 100.0f / (newVersionFileList.size() + newVersionFileList.size());
		progressFunction(progress, oldFileIndex + newFileIndex, oldVersionFileList.size() + newVersionFileList.size());

		patchFileList->AddedFileList.push_back(newVersionFileList[newFileIndex]);
		newFileIndex++;
	}

	progressFunction(100.0f, oldFileIndex + newFileIndex, oldVersionFileList.size() + newVersionFileList.size());
	fprintf(stdout, "\n\n");

	return patchFileList;
}

bool ZPatcher::DoCreatePatchFile(FILE* patchFile, const std::string& newVersionPath, PatchFileList_t* patchFileList, ProgressCallback progressFunction, ICompressProgress LZMAProgressCallback)
{
	// Initialize our custom LZMA2 Encoder
	CLzma2EncHandle hLzma2Enc = InitLzma2Encoder();

	fprintf(stdout, "Writing patch data...\n");

	Log(LOG, "Writing patch data");

	// Write the file header, including our custom LZMA2 props
	Byte props = Lzma2Enc_WriteProperties(hLzma2Enc);
	WritePatchFileHeader(patchFile, props);

	size_t totalFiles = patchFileList->RemovedFileList.size() + patchFileList->AddedFileList.size() + patchFileList->ModifiedFileList.size();

	unsigned int i = 0;
	// Process the removed file list in reverse order - Directories should be the last thing being deleted.
	for (std::vector<std::string>::reverse_iterator ritr = patchFileList->RemovedFileList.rbegin(); ritr != patchFileList->RemovedFileList.rend(); ++ritr)
	{
		// Update our progress bar
		float progress = ((float)++i / (float)totalFiles) * 100.0f;
		progressFunction(progress, i, totalFiles);

		Log(LOG, "[del] %s", ritr->c_str());

		WriteFileInfo(patchFile, Patch_File_Delete, ritr->c_str());
	}

	for (std::vector<std::string>::iterator itr = patchFileList->AddedFileList.begin(); itr < patchFileList->AddedFileList.end(); ++itr)
	{
		// Update our progress bar
		float progress = ((float)++i / (float)totalFiles) * 100.0f;
		progressFunction(progress, i, totalFiles);

		Log(LOG, "[add] %s", itr->c_str());

		size_t fileNameLength = itr->length();
		if (fileNameLength > 0 && (*itr)[fileNameLength - 1] != '/')
		{
			WriteFileInfo(patchFile, Patch_File_Add, itr->c_str());
			std::string localPath = newVersionPath + "/" + *itr;

			if (!WriteCompressedFile(hLzma2Enc, localPath, patchFile, LZMAProgressCallback))
			{
				return false;
			}
		}
		else
		{
			WriteFileInfo(patchFile, Patch_Dir_Add, itr->c_str());
		}
	}

	// Right now, we replace both the modified files and the added files
	for (std::vector<std::string>::iterator itr = patchFileList->ModifiedFileList.begin(); itr < patchFileList->ModifiedFileList.end(); ++itr)
	{
		// Update our progress bar
		float progress = ((float)++i / (float)totalFiles) * 100.0f;
		progressFunction(progress, i, totalFiles);

		Log(LOG, "[mod] %s", itr->c_str());

		WriteFileInfo(patchFile, Patch_File_Replace, itr->c_str());
		std::string localPath = newVersionPath + "/" + *itr;

		if (!WriteCompressedFile(hLzma2Enc, localPath, patchFile, LZMAProgressCallback))
		{
			return false;
		}
	}

	// Update our progress bar
	float progress = ((float)i / (float)totalFiles) * 100.0f;
	progressFunction(progress, i, totalFiles);

	// Hacky hack if we are using our own provided function ;)
	if (progressFunction == &PrintCreatePatchProgressBar)
	{
		fprintf(stdout, "\n");
	}

	Log(LOG, "Patch data writing process completed");

	DestroyLzma2EncHandle(hLzma2Enc);

	return true;
}


bool ZPatcher::CreatePatchFile(const std::string& patchFileName, const std::string& newVersionPath, PatchFileList_t* patchFileList, ProgressCallback progressFunction, ICompressProgress LZMAProgressCallback)
{
	FILE* patchFile;

	errno = 0;
	patchFile = fopen(patchFileName.c_str(), "wb");
	if (errno != 0)
	{
		Log(LOG_FATAL, "Error opening file \"%s\" to write patch data: %s", patchFileName.c_str(), strerror(errno));
		return false;
	}

	bool result = DoCreatePatchFile(patchFile, newVersionPath, patchFileList, progressFunction, LZMAProgressCallback);

	fclose(patchFile);

	return result;
}
