//////////////////////////////////////////////////////////////////////////
//
// ZPatcher - Patcher system - Part of the ZUpdater suite
// Felipe "Zoc" Silveira - (c) 2016
//
//////////////////////////////////////////////////////////////////////////
//
// FileUtils.h
// File Utilities for the patcher system
//
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FileUtils.h"


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
