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
#include <assert.h>
#include "FileUtils.h"
#include "LogSystem.h"

#ifdef _WIN32
	#include "dirent.h"
	#include <direct.h>
#else
	#include <dirent.h>
	#include <unistd.h>
#endif


bool ZPatcher::GetFilesInDirectory(std::vector<std::string>& fileList, const std::string& base, std::string directory)
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

				GetFilesInDirectory(fileList, base, output);
			}
		}
	}
	closedir(dir);
	return true;
}

bool ZPatcher::AreFilesIdentical(FILE* file1, FILE* file2, bool &result)
{
	// Compare their size, first
	_fseeki64(file1, 0, SEEK_END);
	long long int file1size = _ftelli64(file1);
	rewind(file1);

	_fseeki64(file2, 0, SEEK_END);
	long long int file2size = _ftelli64(file2);
	rewind(file2);

	if (file1size != file2size)
	{
		result = false;
		return true;
	}

	// If their size are equal, compare them byte by byte
	bool identical = true;

	while (identical && file1size > 0)
	{
		const unsigned long long buffer_size = 1 << 16;
		unsigned char file1Buffer[buffer_size];
		unsigned char file2Buffer[buffer_size];

		size_t file1Len = fread(file1Buffer, 1, buffer_size, file1);
		size_t file2Len = fread(file2Buffer, 1, buffer_size, file2);

		assert(file1Len == file2Len);

		if (memcmp(file1Buffer, file2Buffer, file1Len) != 0)
		{
			result = false;
			return true;
		}

		file1size -= file1Len;
	}

	result = true;
	return true;
}

bool ZPatcher::AreFilesIdentical(const std::string& file1, const std::string& file2, bool &result)
{
	errno_t err;
	FILE* f1;
	FILE* f2;

	err = fopen_s(&f1, file1.c_str(), "rb");
	if (err != 0)
	{
		const size_t buffer_size = 1024;
		char buffer[buffer_size];
		strerror_s(buffer, buffer_size, err);
		Log(LOG_FATAL, "Error opening file \"%s\" to reading for data comparison: %s", file1.c_str(), buffer);
		result = false;
		return false;
	}

	err = fopen_s(&f2, file2.c_str(), "rb");
	if (err != 0)
	{
		const size_t buffer_size = 1024;
		char buffer[buffer_size];
		strerror_s(buffer, buffer_size, err);
		Log(LOG_FATAL, "Error opening file \"%s\" to reading for data comparison: %s", file2.c_str(), buffer);
		result = false;
		return false;
	}

	bool success = AreFilesIdentical(f1, f2, result);

	fclose(f1);
	fclose(f2);

	return success;
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

bool ZPatcher::DeleteDirectoryTree(const std::string& base, const std::string directory)
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
			// Delete this file
			std::string deleteFile = base + "/";
			if (!directory.empty())
				deleteFile += directory + "/";

			deleteFile += ent->d_name;
			remove(deleteFile.c_str());
		}
		else if (ent->d_type = DT_DIR) // Directories
		{
			if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0)
			{
				// Delete files and directories recursively
				std::string deleteDirectory;
				if (!directory.empty())
					deleteDirectory = directory + "/";

				deleteDirectory += ent->d_name;
				DeleteDirectoryTree(base, deleteDirectory);
			}
		}
	}
	closedir(dir);

	// Delete the curent base + directory before exiting
	std::string deleteCurrentDirectory = base + "/";
	if (!directory.empty())
		deleteCurrentDirectory += directory + "/";

	_rmdir(deleteCurrentDirectory.c_str());


	return true;
}

void ZPatcher::CreateDirectoryTree(const std::string& directory)
{

	const char* slash = strpbrk(directory.c_str(), "\\/");

	Log(LOG, "Creating directory tree: %s", slash);

	while (slash != NULL)
	{

		size_t length = slash - directory.c_str();
		std::string subDirectory = directory.substr(0, length);

		_mkdir(subDirectory.c_str());
		slash = strpbrk(slash + 1, "\\/");

	}

}

bool ZPatcher::BackupFile(const std::string& baseDirectory, const std::string& fileName, const std::string& suffix)
{
	// Create the backup directory structure
	std::string fullFilename = baseDirectory + "/" + fileName;
	std::string fullBackupFileName = baseDirectory + "/backup-" + suffix + "/" + fileName;
// 	size_t lastSlash = fullBackupFileName.find_last_of("\\/");
// 	fullBackupFileName.insert(lastSlash, "/backup-" + suffix);

	Log(LOG, "Backing up file %s to %s (There should be a Create Directory Tree, Copy and Delete below)", fullFilename.c_str(), fullBackupFileName.c_str());

	CreateDirectoryTree(fullBackupFileName);
	return CopyOneFile(fullFilename, fullBackupFileName);
}

bool ZPatcher::CopyOneFile(const std::string& source, const std::string& target)
{
	FILE* sourceFile;
	FILE* targetFile;
	errno_t err = 0;

	Log(LOG, "Copying file %s to %s", source.c_str(), target.c_str());

	// Open source and target file
	err = fopen_s(&sourceFile, source.c_str(), "rb");
	if (err != 0)
	{
		const size_t buffer_size = 1024;
		char buffer[buffer_size];
		strerror_s(buffer, buffer_size, err);
		Log(LOG_FATAL, "Error opening file \"%s\" to read for copy: %s", source.c_str(), buffer);
		return false;
	}

	err = fopen_s(&targetFile, target.c_str(), "wb");
	if (err != 0)
	{
		const size_t buffer_size = 1024;
		char buffer[buffer_size];
		strerror_s(buffer, buffer_size, err);
		Log(LOG_FATAL, "Error opening file \"%s\" to write for copy: %s", target.c_str(), buffer);
		fclose(sourceFile);
		return false;
	}

	// Do the actual copy
	const unsigned long long buffer_size = 1 << 16;
	unsigned char readBuffer[buffer_size];
	size_t bytesRead;

	while (0 < (bytesRead = fread(readBuffer, 1, buffer_size, sourceFile)))
	{
		size_t bytesWritten = fwrite(readBuffer, 1, bytesRead, targetFile);

		if (bytesWritten != bytesRead)
		{
			fclose(sourceFile);
			fclose(targetFile);
			return false;
		}
	}

	// Close the files
	fclose(sourceFile);
	fclose(targetFile);

	return true;
}

bool ZPatcher::RemoveFile(const std::string& fileName)
{
	Log(LOG, "Deleting file: %s", fileName.c_str());

	_set_errno(0);
	int result = remove(fileName.c_str());

	if (result != 0)
	{
		_get_errno(&result);

		const size_t buffer_size = 1024;
		char buffer[buffer_size];
		strerror_s(buffer, buffer_size, result);
		Log(LOG_FATAL, "Error deleting file: %s", buffer);
		return false;
	}

	return true;
}

bool ZPatcher::RemoveOneDirectory(const std::string& directory)
{
	Log(LOG, "Removing directory %s", directory.c_str());

	_set_errno(0);
	int result = _rmdir(directory.c_str());

	if (result != 0)
	{
		_get_errno(&result);

		const size_t buffer_size = 1024;
		char buffer[buffer_size];
		strerror_s(buffer, buffer_size, result);
		Log(LOG_FATAL, "Error removing directory: %s", result, buffer);
		return false;
	}

	return true;
}

