//////////////////////////////////////////////////////////////////////////
//
// ZPatcher - Patcher system - Part of the ZUpdater suite
// Felipe "Zoc" Silveira - (c) 2016-2018
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
#include <cinttypes>
#include "LogSystem.h"


/**
 * Creates the patch file with all the changes listed in patchFileList.
 */
static bool DoCreatePatchFile(FILE* patchFile, const std::string& newVersionPath, ZPatcher::PatchFileList_t* patchFileList, ZPatcher::ProgressCallback progressFunction, ICompressProgress LZMAProgressCallback = { reinterpret_cast<ZPatcher::CompressProgressCallback>(&ZPatcher::OnProgress) });

/**
* Print the progress bar used when comparing directories
*/
static void PrintCreatePatchProgressBar(const float& percentage, const uint64_t& leftAmount, const uint64_t& rightAmount);


static void PrintCreatePatchProgressBar(const float& percentage, const uint64_t& leftAmount, const uint64_t& rightAmount)
{
	int barWidth = 60;

	fprintf(stdout, "\xd[");

	int pos = (int)(barWidth * percentage / 100.0f);
	for (int i = 0; i < barWidth; ++i)
	{
		if (i < pos) fprintf(stdout, "=");
		else if (i == pos) fprintf(stdout, ">");
		else fprintf(stdout, " ");
	}

	fprintf(stdout, "] %0.2f%% %" PRIu64 "/%" PRIu64, percentage, leftAmount, rightAmount);

	fflush(stdout);
}

ZPatcher::PatchFileList_t* ZPatcher::GetDifferences(const std::string& oldVersion, const std::string& newVersion)
{
	return GetDifferencesEx(oldVersion, newVersion, &PrintCreatePatchProgressBar);
}

ZPatcher::PatchFileList_t* ZPatcher::GetDifferencesEx(const std::string& oldVersion, const std::string& newVersion, ProgressCallback progressFunction)
{
	PatchFileList_t* patchFileList = new PatchFileList_t();

	std::string normalizedOldVersion = oldVersion;
	NormalizeFileName(normalizedOldVersion);

	std::string normalizedNewVersion = newVersion;
	NormalizeFileName(normalizedNewVersion);

	std::vector<std::string> oldVersionFileList;
	GetFilesInDirectory(oldVersionFileList, normalizedOldVersion);

	std::vector<std::string> newVersionFileList;
	GetFilesInDirectory(newVersionFileList, normalizedNewVersion);

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
			const size_t fileNameLength = oldFileName.length();
			if (fileNameLength > 0 && oldFileName[fileNameLength - 1] != '/')
			{
				// Check if the files have the same contents
				bool identical;
				const bool success = AreFilesIdentical(normalizedOldVersion + "/" + oldFileName, normalizedNewVersion + "/" + newFileName, identical);

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

static bool DoCreatePatchFile(FILE* patchFile, const std::string& newVersionPath, ZPatcher::PatchFileList_t* patchFileList, ZPatcher::ProgressCallback progressFunction, ICompressProgress LZMAProgressCallback)
{
	using namespace ZPatcher;

	std::string normalizedNewVersionPath = newVersionPath;
	NormalizeFileName(normalizedNewVersionPath);

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

		Log(LOG, "[del] %s", *ritr);

		WriteFileInfo(patchFile, Patch_File_Delete, *ritr);
	}

	for (std::vector<std::string>::iterator itr = patchFileList->AddedFileList.begin(); itr < patchFileList->AddedFileList.end(); ++itr)
	{
		// Update our progress bar
		float progress = ((float)++i / (float)totalFiles) * 100.0f;
		progressFunction(progress, i, totalFiles);

		Log(LOG, "[add] %s", itr->c_str());

		const size_t fileNameLength = itr->length();
		if (fileNameLength > 0 && (*itr)[fileNameLength - 1] != '/')
		{
			WriteFileInfo(patchFile, Patch_File_Add, *itr);
			std::string localPath = normalizedNewVersionPath + "/" + *itr;

			if (!WriteCompressedFile(hLzma2Enc, localPath, patchFile, LZMAProgressCallback))
			{
				return false;
			}
		}
		else
		{
			WriteFileInfo(patchFile, Patch_Dir_Add, *itr);
		}
	}

	// Right now, we replace both the modified files and the added files
	for (std::vector<std::string>::iterator itr = patchFileList->ModifiedFileList.begin(); itr < patchFileList->ModifiedFileList.end(); ++itr)
	{
		// Update our progress bar
		float progress = ((float)++i / (float)totalFiles) * 100.0f;
		progressFunction(progress, i, totalFiles);

		Log(LOG, "[mod] %s", *itr);

		WriteFileInfo(patchFile, Patch_File_Replace, *itr);
		std::string localPath = normalizedNewVersionPath + "/" + *itr;

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

bool ZPatcher::CreatePatchFile(const std::string& patchFileName, const std::string& newVersionPath, PatchFileList_t* patchFileList)
{
	return CreatePatchFileEx(patchFileName, newVersionPath, patchFileList, &PrintCreatePatchProgressBar, { reinterpret_cast<CompressProgressCallback>(&OnProgress) });
}


bool ZPatcher::CreatePatchFileEx(const std::string& patchFileName, const std::string& newVersionPath, PatchFileList_t* patchFileList, ProgressCallback progressFunction, ICompressProgress LZMAProgressCallback)
{
	FILE* patchFile;

	std::string normalizedPatchFileName = patchFileName;
	NormalizeFileName(normalizedPatchFileName);

	errno = 0;
	patchFile = fopen(normalizedPatchFileName.c_str(), "wb");
	if (errno != 0)
	{
		Log(LOG_FATAL, "Error opening file \"%s\" to write patch data: %s", normalizedPatchFileName.c_str(), strerror(errno));
		return false;
	}

	const bool result = DoCreatePatchFile(patchFile, newVersionPath, patchFileList, progressFunction, LZMAProgressCallback);

	fclose(patchFile);

	return result;
}
