//////////////////////////////////////////////////////////////////////////
//
// ZPatcher - Patcher system - Part of the ZUpdater suite
// Felipe "Zoc" Silveira - (c) 2016
//
//////////////////////////////////////////////////////////////////////////
//
// FileUtil.h
// File Utilities for the patcher system
//
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FileUtil.h"
#include <vector>
#include <string.h>
#include <algorithm>

#ifdef _WIN32
	#include "dirent.h"
#else
	#include <dirent.h>
#endif


bool ZPatcher::GetFilesInDirectory(const std::string& base, std::string directory, std::vector<std::string>& fileList)
{
	DIR* dir;
	struct dirent* ent;

	// Open the directory
	std::string targetDir = base;
	if (!directory.empty())
	{
		targetDir += "/" + directory;
	}

	dir = opendir(targetDir.c_str());
	if (!dir)
		return false;

	// Get all files and directories inside the directory
	while ((ent = readdir(dir)) != nullptr)
	{
		// Decide what to do with each entry
		if (ent->d_type == DT_REG) // Regular files
		{
			std::string output;
			if (!directory.empty())
			{
				output = directory + "/";
			}
			output += ent->d_name;

			fileList.push_back(output);

		}
		else if (ent->d_type = DT_DIR) // Directories
		{
			if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0)
			{

				std::string output;
				if (!directory.empty())
				{
					output = directory + "/";
				}
				output += ent->d_name;

				fileList.push_back(output + "/"); // Add a slash at the end, so we know it's a directory

				GetFilesInDirectory(base, output, fileList);
			}
		}
	}
	closedir(dir);
	return true;
}

bool ZPatcher::AreFilesIdentical(FILE* file1, FILE* file2)
{
	// Compare their size, first
	_fseeki64(file1, 0, SEEK_END);
	long long int file1size = _ftelli64(file1);
	rewind(file1);

	_fseeki64(file2, 0, SEEK_END);
	long long int file2size = _ftelli64(file2);
	rewind(file2);

	if (file1size != file2size)
		return false;

	// If their size are equal, compare them byte by byte
	bool identical = true;

	while (identical && file1size > 0)
	{
		const unsigned long long buffer_size = 1;// << 16;
		unsigned char file1Buffer[buffer_size];
		unsigned char file2Buffer[buffer_size];

		unsigned long long file1Len = fread(file1Buffer, 1, buffer_size, file1);
		unsigned long long file2Len = fread(file2Buffer, 1, buffer_size, file2);

		assert(file1Len == file2Len);

		if (memcmp(file1Buffer, file2Buffer, file1Len) != 0)
			return false;

		file1size -= file1Len;
	}

	return true;
}

bool ZPatcher::AreFilesIdentical(const std::string& file1, const std::string& file2)
{
	errno_t err;
	FILE* f1;
	FILE* f2;

	err = fopen_s(&f1, file1.c_str(), "rb");
	assert(err == 0);

	err = fopen_s(&f2, file2.c_str(), "rb");
	assert(err == 0);

	bool result = AreFilesIdentical(f1, f2);

	fclose(f1);
	fclose(f2);

	return result;
}

void ZPatcher::NormalizeFileName(std::string& fileName)
{
	for (size_t i = 0; i < fileName.length(); ++i)
	{
		if (fileName[i] == '\\')
		{
			fileName[i] = '/';
		}
	}
}

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
			// Unneeded to compare directories
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

