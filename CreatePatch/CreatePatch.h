//////////////////////////////////////////////////////////////////////////
//
// ZPatcher - Patcher system - Part of the ZUpdater suite
// Felipe "Zoc" Silveira - (c) 2016
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

namespace ZPatcher
{
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
	* Get the difference between two directories (oldVersion and newVersion) and build a PatchFileList_t containing all the changes
	*/
	PatchFileList_t* GetDifferences(std::string& oldVersion, std::string& newVersion);

	/**
	 * Print the progress bar used when comparing directories
	 */
	void PrintCreatePatchProgressBar(const float& Percentage, const size_t& leftAmount, const size_t& rightAmount);

	/**
	 * Creates the patch file with all the changes listed in patchFileList.
	 * patchfile is the file handle for the target patch file (output file)
	 * newVersionPath is the directory that contains the updated files
	 * patchFileList is a PatchFileList_t filled by GetDifferences() with the changes between directories
	 */
	bool CreatePatchFile(FILE* patchFile, std::string& newVersionPath, PatchFileList_t* patchFileList);

	/**
	 * This is a shortcut to CreatePatchFile() that receives the output patch file as a string
	 */
	bool CreatePatchFile(std::string& patchFileName, std::string& newVersionPath, PatchFileList_t* patchFileList);
}

#endif // _CREATEPATCH_H_

