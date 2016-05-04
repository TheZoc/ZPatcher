// ZPatcher - Patcher system - Part of the ZUpdater suite
// Felipe "Zoc" Silveira - (c) 2016
//
//////////////////////////////////////////////////////////////////////////
//
// FileUtil.h
// File Utilities for the patcher system
//
//////////////////////////////////////////////////////////////////////////


#ifndef _FILEUTIL_H_
#define _FILEUTIL_H_

#include <vector>
#include <string>

namespace ZPatcher
{
	bool GetFilesInDirectory(const std::string& base, std::string directory, std::vector<std::string>& fileList);
	bool AreFilesIdentical(FILE* file1, FILE* file2);
	bool AreFilesIdentical(const std::string& file1, const std::string& file2);
	void NormalizeFileName(std::string& fileName);

	class PatchFileList_t
	{
	public:
		std::vector<std::string> RemovedFileList;
		std::vector<std::string> ModifiedFileList;
		std::vector<std::string> AddedFileList;
	};

	PatchFileList_t* GetDifferences(std::string& oldVersion, std::string& newVersion);
	void PrintFileCompareProgressBar(float Percentage);
}

#endif // _FILEUTIL_H_
