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


#ifndef _FILEUTILS_H_
#define _FILEUTILS_H_

#include <vector>
#include <string>

namespace ZPatcher
{
	// Create a list with all the files and directories inside given "base" directory and writes it to fileList
	bool GetFilesInDirectory(std::vector<std::string>& fileList, const std::string& base, std::string directory = "");

	// Check if two files are identical, byte to byte
	bool AreFilesIdentical(FILE* file1, FILE* file2);

	// Check if two files are identical, byte to byte, given their filenames
	bool AreFilesIdentical(const std::string& file1, const std::string& file2);

	// Converts the supplied path to use forward slash
	void NormalizeFileName(std::string& fileName);

	// Delete all files and directories in supplied base directory. (The "directory" variable is used for recursion). If it's impossible to delete one of them, it keep deleting whatever is possible.
	bool DeleteDirectoryTree(const std::string& base, const std::string directory = "");

	// Create the supplied directory tree. It may or may not contain a file at the end - It will create directories up to until the last / found. It will ignore silently if any directory in the tree already exists.
	void CreateDirectoryTree(const std::string& directory);

	// Backup the supplied filename to BackupSuffix folder
	bool BackupFile(const std::string& fileName, const std::string& suffix);
}

#endif // _FILEUTILS_H_
