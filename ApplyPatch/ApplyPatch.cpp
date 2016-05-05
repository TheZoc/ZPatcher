//////////////////////////////////////////////////////////////////////////
//
// ZPatcher - Patcher system - Part of the ZUpdater suite
// Felipe "Zoc" Silveira - (c) 2016
//
//////////////////////////////////////////////////////////////////////////
//
// ApplyPatch.cpp
// Functions to apply the Patch data
//
//////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "ApplyPatch.h"
#include "Lzma2Decoder.h"
#include "LogSystem.h"
#include "FileUtils.h"

void ZPatcher::PrintPatchApplyingProgressBar(float Percentage)
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
	fflush(stdout);
}

bool ZPatcher::ApplyPatchFile(FILE* patchFile, const std::string& targetPath, std::string previousVersionNumber)
{
	_fseeki64(patchFile, 0LL, SEEK_END);
	long long patchFileSize = _ftelli64(patchFile);
	rewind(patchFile);
	
	Byte props;
	if (!ReadPatchFileHeader(patchFile, props))
	{
		Log(LOG_FATAL, "Failed to read patch data.");
		return false;
	}

	CLzma2Dec* decoder = InitLzma2Decoder(props);

	std::vector<std::string> backupFileList;
	bool success = true;

	while (_ftelli64(patchFile) < patchFileSize)
	{
		float percentage = (float)_ftelli64(patchFile) / (float)patchFileSize * 100.0f;
		PrintPatchApplyingProgressBar(percentage);

		std::string outputFile;
		unsigned char operation;
		GetFileinfo(patchFile, outputFile, operation);

		switch (static_cast<PatchOperation>(operation))
		{
		case Patch_File_Delete:
			// Check if we are dealing with a directory
			if (outputFile.length() > 0 && outputFile[outputFile.length() - 1] == '/')
			{
				success = success && RemoveOneDirectory(outputFile);
			}
			else
			{
				success = success && BackupFile(outputFile, previousVersionNumber);
				if (!success)
					break;
				backupFileList.push_back(outputFile);
				success = success && RemoveFile(outputFile);
			}
			break;
		case Patch_File_AddReplace:
			success = success && BackupFile(outputFile, previousVersionNumber);
			if (!success)
				break;
			backupFileList.push_back(outputFile);
			success = success && RemoveFile(outputFile);
			success = success && FileDecompress(decoder, patchFile, outputFile);
			break;
		case Patch_Dir_Add:
			CreateDirectoryTree(outputFile);
			break;
		default:
			Log(LOG_FATAL, "Undefined operation (%d) requested for file %s", static_cast<int>(operation), outputFile.c_str());
			success = false; // Rollback the operation
			return false;
			break;
		}

		// If something went wrong, abort the operation
		if (!success)
		{
			bool restoreSucess = true;
			Log(LOG_FATAL, "Something went wrong with the patch process! Rolling back!");
			restoreSucess = restoreSucess && RestoreBackup(backupFileList, previousVersionNumber);

			if (!restoreSucess)
				Log(LOG_FATAL, "At least one file failed to be restored! The application is probably in an inconsistent state!");

			return false;
		}

	}

	// Everything went fine, so, let's do a backup cleanup
	if (success)
	{
		Log(LOG, "Patching successful! Removing backup directory.");
		std::string backupDirectoryName = "backup" + previousVersionNumber + "/";
		DeleteDirectoryTree(backupDirectoryName);
	}
	else
	{
		// This is not supposed to happen
		Log(LOG_ERROR, "=================================== This is not supposed to happen! Report if you see this! ===================================");

		assert(success == true); // Why not? Might help us catch a bug.
	}

	DestroyLzma2Decoder(decoder);

	return true;
}

bool ZPatcher::RestoreBackup(std::vector<std::string>& backupFileList, std::string previousVersionNumber)
{
	bool result = true;

	for (std::vector<std::string>::iterator itr = backupFileList.begin(); itr < backupFileList.end(); ++itr)
	{
		std::string backupFileName = "backup" + previousVersionNumber + "/" + *itr;
		CreateDirectoryTree(backupFileName); // Lazy++;
		result = result && CopyOneFile(backupFileName, *itr);
	}

	return result;
}

bool ZPatcher::ApplyPatchFile(const std::string& patchFileName, const std::string& targetPath, std::string previousVersionNumber)
{
	FILE* patchFile;
	errno_t err = 0;

	err = fopen_s(&patchFile, patchFileName.c_str(), "rb");

	if (err == 0)
	{
		Log(LOG, "Reading patch file %s", patchFileName);
	}
	else
	{
		Log(LOG_FATAL, "Unable to open for reading the patch file %s", patchFileName);
		return false;
	}

	bool success = ApplyPatchFile(patchFile, targetPath);

	fclose(patchFile);

	return success;
}

