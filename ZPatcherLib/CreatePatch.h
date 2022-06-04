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
	struct PatchFileList_t
	{
		std::vector<std::string> RemovedFileList;
		std::vector<std::string> ModifiedFileList;
		std::vector<std::string> AddedFileList;
	};

	/**
	* Get the difference between two directories (oldVersion and newVersion) and build a PatchFileList_t containing all the changes
	*/
	PatchFileList_t* GetDifferences(const std::string& oldVersion, const std::string& newVersion);

	/**
	* Get the difference between two directories (oldVersion and newVersion) and build a PatchFileList_t containing all the changes
	* This version allows the customization of the Progress Callback
	*/
	PatchFileList_t* GetDifferencesEx(const std::string& oldVersion, const std::string& newVersion, ProgressCallback progressFunction);

	/**
	 * Creates the patch file with all the changes listed in patchFileList.
	 * patchFileName is the name of the output file
	 * newVersionPath is the directory that contains the updated files
	 * patchFileList is a PatchFileList_t filled by GetDifferences() with the changes between directories
	 * This function will default to use PrintCreatePatchProgressBar() and OnProgress() (from LZMAInterfaces.h). To change those, use CreatePatchFileEx().
	 */
	bool CreatePatchFile(const std::string& patchFileName, const std::string& newVersionPath, PatchFileList_t* patchFileList);

	/**
	 * Creates the patch file with all the changes listed in patchFileList.
	 * patchFileName is the name of the output file
	 * newVersionPath is the directory that contains the updated files
	 * patchFileList is a PatchFileList_t filled by GetDifferences() with the changes between directories
	 * progressFunction is a pointer to a function to display the current progress of file processing.
	 * LZMAProgressCallback is a pointer to a function to display the current progress of the file being compressed by the LZMA algorithm.
	 */
	bool CreatePatchFileEx(const std::string& patchFileName, const std::string& newVersionPath, PatchFileList_t* patchFileList, ProgressCallback progressFunction, ICompressProgress LZMAProgressCallback);
}

#endif // _CREATEPATCH_H_

