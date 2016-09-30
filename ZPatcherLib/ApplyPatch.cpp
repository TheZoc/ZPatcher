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
#include <cstdio>
#include <string>
#include <cerrno>
#include <cstdint>
#include "ApplyPatch.h"
#include "Lzma2Decoder.h"
#include "LogSystem.h"
#include "FileUtils.h"

#ifdef _WIN32
	#define ftell64 _ftelli64
	#define fseek64 _fseeki64
#else
 	#define ftell64 ftell
	#define fseek64 fseek
#endif

void ZPatcher::PrintPatchApplyingProgressBar(const float& Percentage)
{
	int barWidth = 60;

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

bool ZPatcher::ApplyPatchFile(FILE* patchFile, const std::string& targetPath, uint64_t &previousVersionNumber, ProgressCallback progressCallback)
{
	std::string prevVersionNumber = std::to_string(previousVersionNumber);

	fseek64(patchFile, 0LL, SEEK_END);
	int64_t patchFileSize = ftell64(patchFile);
	rewind(patchFile);

	std::string normalizedTargetPath = targetPath;
	NormalizeFileName(normalizedTargetPath);

	size_t fileNameLength = normalizedTargetPath.length();
	if (fileNameLength == 0 || normalizedTargetPath[fileNameLength - 1] != '/')
	{
		Log(LOG_FATAL, "Invalid target directory. It must end either with \\ or /.");
		return false;
	}

	Byte props;
	if (!ReadPatchFileHeader(patchFile, props))
	{
		Log(LOG_FATAL, "Failed to read patch data.");
		return false;
	}

	CLzma2Dec* decoder = InitLzma2Decoder(props);

	std::vector<std::string> backupFileList;
	std::vector<std::string> addedFileList;

	bool success = true;
	for(int64_t currentPos = ftell64(patchFile); success && (currentPos < patchFileSize); currentPos = ftell64(patchFile))
	{
		float percentage = (float)ftell64(patchFile) / (float)patchFileSize * 100.0f;
		progressCallback(percentage);

		std::string outputFile;
		unsigned char operation;
		GetFileinfo(patchFile, outputFile, operation);

		switch (static_cast<PatchOperation>(operation))
		{
		case Patch_File_Delete:
			// Check if we are dealing with a directory
			if (outputFile.length() > 0 && outputFile.back() == '/')
			{
				success = success && RemoveOneDirectory(normalizedTargetPath + outputFile);
			}
			else
			{
				success = success && BackupFile(normalizedTargetPath, outputFile, prevVersionNumber);
				if (!success)
					break;
				backupFileList.push_back(outputFile);
				success = success && RemoveFile(normalizedTargetPath + outputFile);
			}
			break;
		case Patch_File_Replace:
			success = success && BackupFile(normalizedTargetPath, outputFile, prevVersionNumber);
			if (!success)
				break;
			backupFileList.push_back(outputFile);
			success = success && RemoveFile(normalizedTargetPath + outputFile);
			success = success && FileDecompress(decoder, patchFile, normalizedTargetPath + outputFile);
			break;
		case Patch_File_Add:
			CreateDirectoryTree(normalizedTargetPath + outputFile);
			success = success && FileDecompress(decoder, patchFile, normalizedTargetPath + outputFile);
			if (success)
				addedFileList.push_back(outputFile);
			break;
		case Patch_Dir_Add:
			CreateDirectoryTree(normalizedTargetPath + outputFile);
			break;
		default:
			Log(LOG_FATAL, "Undefined operation (%d) requested for file %s", static_cast<int>(operation), outputFile.c_str());
			success = false; // Rollback the operation
			break;
		}
	}


	if (success)
	{
		// Everything went fine, so, let's do a backup cleanup
		Log(LOG, "Patching successful! Removing backup directory.");
		std::string backupDirectoryName = normalizedTargetPath + "/" + "backup-" + prevVersionNumber + "/";
		DeleteDirectoryTree(backupDirectoryName);
		progressCallback(100.0f);
	}
	else
	{
		// Something bad happened. Roll back.
		bool restoreSucess = true;
		Log(LOG_FATAL, "Something went wrong with the patch process! Rolling back!");
		restoreSucess = restoreSucess && RestoreBackup(backupFileList, addedFileList, normalizedTargetPath, prevVersionNumber);

		if (!restoreSucess)
			Log(LOG_FATAL, "At least one file failed to be restored! The application is probably in an inconsistent state!");
	}

	DestroyLzma2Decoder(decoder);

	return success;
}

bool ZPatcher::ApplyPatchFile(const std::string& patchFileName, const std::string& targetPath, uint64_t& previousVersionNumber, ProgressCallback progressCallback)
{
	FILE* patchFile;

	std::string normalizedPatchFileName = patchFileName;
	NormalizeFileName(normalizedPatchFileName);

	errno = 0;
	patchFile = fopen(normalizedPatchFileName.c_str(), "rb");
	if(errno != 0)
	{
		Log(LOG_FATAL, "Unable to open for reading the patch file %s: %s", normalizedPatchFileName.c_str(), strerror(errno));
		return false;
	}

	Log(LOG, "Reading patch file %s", normalizedPatchFileName.c_str());

	bool success = ApplyPatchFile(patchFile, targetPath, previousVersionNumber, progressCallback);

	fclose(patchFile);

	return success;
}

bool ZPatcher::RestoreBackup(std::vector<std::string>& backupFileList, std::vector<std::string>& addedFileList, const std::string& baseDirectory, std::string previousVersionNumber)
{
	bool result = true;

	for (std::vector<std::string>::iterator itr = backupFileList.begin(); itr < backupFileList.end(); ++itr)
	{
		std::string fullFilename = baseDirectory + "/" + *itr;
		std::string fullBackupFileName = baseDirectory + "/backup-" + previousVersionNumber + "/" + *itr;

		CreateDirectoryTree(fullFilename); // Lazy++;
		result = result && CopyOneFile(fullBackupFileName, fullFilename);
	}

	for (std::vector<std::string>::iterator itr = addedFileList.begin(); itr < addedFileList.end(); ++itr)
	{
		result = result && RemoveFile(baseDirectory + "/" + *itr);
	}

	return result;
}
