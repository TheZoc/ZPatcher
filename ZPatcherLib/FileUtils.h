//////////////////////////////////////////////////////////////////////////
//
// ZPatcher - Patcher system - Part of the ZUpdater suite
// Felipe "Zoc" Silveira - (c) 2016-2018
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

	/**
	 * Receives two files as input and stores in "result" if they are identical, byte to byte.
	 * The function returns true if it executed successfully or false if there was an IO error.
	 */
	bool DoAreFilesIdentical(FILE* file1, FILE* file2, bool &result);

	/**
	* Receives two files as input and stores in "result" if they are identical, byte to byte, given their filenames.
	* The function returns true if it executed successfully or false if there was an IO error.
	*/
	bool AreFilesIdentical(const std::string& file1, const std::string& file2, bool &result);

	/**
	 * Converts the supplied path to use forward slash
	 */
	void NormalizeFileName(std::string& fileName);

	/**
	 * Delete all files and directories in supplied base directory. (The "directory" variable is used for recursion).
	 * If it's impossible to delete one of the files or directories, it keep deleting whatever else is possible inside the base directory.
	 */
	bool DeleteDirectoryTree(const std::string& base, const std::string directory = "");

	/**
	 * Create the supplied directory tree. It may or may not contain a file at the end - It will create directories up to until the last / found.
	 * It will ignore silently if any directory in the tree that already exists.
	 */
	void CreateDirectoryTree(const std::string& directory, const bool& logCommand = true);

	/**
	 * Backup the supplied file to BackupSuffix folder
	 */
	bool BackupFile(const std::string& baseDirectory, const std::string& fileName, const std::string& suffix);

	/**
	* Copy one file from source to target. Directory structure must already exist.
	*/
	bool CopyOneFile(const std::string& source, const std::string& target);

	/**
	* Delete/Remove the supplied file
	*/
	bool RemoveFile(const std::string& fileName);

	/**
	* Delete/Remove the supplied directory. It must be empty.
	*/
	bool RemoveOneDirectory(const std::string& directory);

}

#endif // _FILEUTILS_H_
