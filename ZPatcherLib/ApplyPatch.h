//////////////////////////////////////////////////////////////////////////
//
// ZPatcher - Patcher system - Part of the ZUpdater suite
// Felipe "Zoc" Silveira - (c) 2016-2018
//
//////////////////////////////////////////////////////////////////////////
//
// ApplyPatch.cpp
// Functions to apply the Patch data
//
//////////////////////////////////////////////////////////////////////////

#ifndef _APPLYPATCH_H_
#define _APPLYPATCH_H_

#include <cstring>
#include <vector>

namespace ZPatcher
{
	/// Our function pointer callback for progress display
	typedef void(*ProgressCallback)(const float& percentage);

	/**
	* Print the progress bar used applying a patch
	*/
	void PrintPatchApplyingProgressBar(const float& Percentage);

	/**
	* Apply the patch file to the target path
	*/
	bool DoApplyPatchFile(FILE* patchFile, const std::string& targetPath, uint64_t& previousVersionNumber, ProgressCallback progressCallback = &PrintPatchApplyingProgressBar);

	/**
	* Apply the patch file to the target path, given the patch name
	*/
	bool ApplyPatchFile(const std::string& patchFileName, const std::string& targetPath, uint64_t& previousVersionNumber, ProgressCallback progressCallback = &PrintPatchApplyingProgressBar);

	/**
	* Restore the target backup list
	*/
	bool RestoreBackup(std::vector<std::string>& backupFileList, std::vector<std::string>& addedFileList, const std::string& baseDirectory, std::string previousVersionNumber);

}


#endif // _APPLYPATCH_H_
