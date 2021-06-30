//////////////////////////////////////////////////////////////////////////
//
// ZPatcher - Patcher system - Part of the ZUpdater suite
// Felipe "Zoc" Silveira - (c) 2016-2018
//
//////////////////////////////////////////////////////////////////////////
//
// CreatePatch.cpp
// Functions to create the Patch data
//
//////////////////////////////////////////////////////////////////////////

#ifndef _CREATEPATCH_H_
#define _CREATEPATCH_H_

#include <vector>
#include "LzmaInterfaces.h"

namespace ZPatcher
{
	/// Our function pointer callback for progress display
	typedef void(*ProgressCallback)(const float& percentage, const uint64_t& processedAmount, const uint64_t& totalToBeProcessed);

	/**
	* This structure holds the files that were Removed, Modified or Added in the patch to be created.
	* Note: This is purposely declared as a class to simplify calling the constructor of each vector ;)
	*/
	class PatchFileList_t
	{
	public:
		std::vector<std::string> RemovedFileList;
		std::vector<std::string> ModifiedFileList;
		std::vector<std::string> AddedFileList;
	};

	/**
	* Print the progress bar used when comparing directories
	*/
	void PrintCreatePatchProgressBar(const float& Percentage, const uint64_t& leftAmount, const uint64_t& rightAmount);

	/**
	* Get the difference between two directories (oldVersion and newVersion) and build a PatchFileList_t containing all the changes
	*/
	PatchFileList_t* GetDifferences(std::string& oldVersion, std::string& newVersion, ProgressCallback progressFunction = &PrintCreatePatchProgressBar);

	/**
	 * Creates the patch file with all the changes listed in patchFileList.
	 * patchfile is the file handle for the target patch file (output file)
	 * newVersionPath is the directory that contains the updated files
	 * patchFileList is a PatchFileList_t filled by GetDifferences() with the changes between directories
	 * progressFunction is a pointer to a function to display the current progress. It defaults to our own PrintCreatePatchProgressBar(), but it can be changed.
	 * LZMAProgressCallback is a pointer to a function to display the current progress of the file being compressed by the LZMA algorithm. It defaults to ZPatcher::OnProgress()
	 */
	bool DoCreatePatchFile(FILE* patchFile, const std::string& newVersionPath, PatchFileList_t* patchFileList, ProgressCallback progressFunction = &PrintCreatePatchProgressBar, ICompressProgress LZMAProgressCallback = { reinterpret_cast<CompressProgressCallback>(&OnProgress) });

	/**
	 * This is a shortcut to CreatePatchFile() that receives the output patch file as a string
	 */
	bool CreatePatchFile(const std::string& patchFileName, const std::string& newVersionPath, PatchFileList_t* patchFileList, ProgressCallback progressFunction = &PrintCreatePatchProgressBar, ICompressProgress LZMAProgressCallback = { reinterpret_cast<CompressProgressCallback>(&OnProgress) });
}

#endif // _CREATEPATCH_H_

