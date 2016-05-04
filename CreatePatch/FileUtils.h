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
	bool GetFilesInDirectory(const std::string& base, std::string directory, std::vector<std::string>& fileList);
	bool AreFilesIdentical(FILE* file1, FILE* file2);
	bool AreFilesIdentical(const std::string& file1, const std::string& file2);
	void NormalizeFileName(std::string& fileName);
}

#endif // _FILEUTILS_H_
