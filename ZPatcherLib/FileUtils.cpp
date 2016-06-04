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
#include <cerrno>
#include <cstdint>
#include "FileUtils.h"
#include "LogSystem.h"

#ifdef _WIN32
	#include "dirent.h"
	#include <direct.h>

	#define ftell64 _ftelli64
	#define fseek64 _fseeki64
#else
	#include <dirent.h>
	#include <unistd.h>
	#include <sys/stat.h>

	#define ftell64 ftell
	#define fseek64 fseek
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
		else if (ent->d_type == DT_DIR) // Directories
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
	fseek64(file1, 0, SEEK_END);
	int64_t file1size = ftell64(file1);
	rewind(file1);

	fseek64(file2, 0, SEEK_END);
	int64_t	file2size = ftell64(file2);
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
		const uint64_t buffer_size = 1 << 16;
		unsigned char file1Buffer[buffer_size];
		unsigned char file2Buffer[buffer_size];

		size_t file1Len = fread(file1Buffer, 1, buffer_size, file1);
		fread(file2Buffer, 1, buffer_size, file2);

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
	FILE* f1;
	FILE* f2;

	errno = 0;
	f1 = fopen(file1.c_str(), "rb");
	if (errno != 0)
	{
		Log(LOG_FATAL, "Error opening file \"%s\" to reading for data comparison: %s", file1.c_str(), strerror(errno));
		result = false;
		return false;
	}

	errno = 0;
	f2 = fopen(file2.c_str(), "rb");
	if (errno != 0)
	{
		Log(LOG_FATAL, "Error opening file \"%s\" to reading for data comparison: %s", file2.c_str(), strerror(errno));
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
		else if (ent->d_type == DT_DIR) // Directories
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
#ifdef _WIN32
	_rmdir(deleteCurrentDirectory.c_str());
#else
	rmdir(deleteCurrentDirectory.c_str());
#endif

	return true;
}

void ZPatcher::CreateDirectoryTree(const std::string& directory)
{
	Log(LOG, "Creating directory tree: %s", directory.c_str());
	const char* slash = strpbrk(directory.c_str(), "\\/");
	while (slash != NULL)
	{

		size_t length = slash - directory.c_str();
		std::string subDirectory = directory.substr(0, length);

#ifdef _WIN32
		_mkdir(subDirectory.c_str());
#else
		mode_t mask = umask(0);
		umask(mask);
		mkdir(subDirectory.c_str(), 0777 & ~mask);
#endif

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

	Log(LOG, "Copying file %s to %s", source.c_str(), target.c_str());

	// Open source and target file
	errno = 0;
	sourceFile = fopen(source.c_str(), "rb");
	if (errno != 0)
	{
		Log(LOG_FATAL, "Error opening file \"%s\" to read for copy: %s", source.c_str(), strerror(errno));
		return false;
	}

	errno = 0;
	targetFile = fopen(target.c_str(), "wb");
	if (errno != 0)
	{
		Log(LOG_FATAL, "Error opening file \"%s\" to write for copy: %s", target.c_str(), strerror(errno));
		fclose(sourceFile);
		return false;
	}

	// Do the actual copy
	const uint64_t buffer_size = 1 << 16;
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

	errno = 0;
	int result = remove(fileName.c_str());
	if (result != 0)
	{
		Log(LOG_FATAL, "Error deleting file: %s", strerror(errno));
		return false;
	}

	return true;
}

bool ZPatcher::RemoveOneDirectory(const std::string& directory)
{
	Log(LOG, "Removing directory %s", directory.c_str());

	errno = 0;

#ifdef _WIN32
	int result = _rmdir(directory.c_str());
#else
	int result = rmdir(directory.c_str());
#endif

	if (result != 0)
	{
		Log(LOG_FATAL, "Error removing directory %s: %s", directory.c_str(), strerror(errno));
		return false;
	}

	return true;
}
