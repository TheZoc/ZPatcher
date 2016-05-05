//////////////////////////////////////////////////////////////////////////
//
// ZPatcher - Patcher system - Part of the ZUpdater suite
// Felipe "Zoc" Silveira - (c) 2016
//
//////////////////////////////////////////////////////////////////////////
//
// ApplyPatch.cpp
// Functions to apply the Patch data
//
//////////////////////////////////////////////////////////////////////////

#ifndef _APPLYPATCH_H_
#define _APPLYPATCH_H_

#include <string.h>
#include <vector>

namespace ZPatcher
{
	/**
	* Print the progress bar used applying a patch
	*/
	void PrintPatchApplyingProgressBar(float Percentage);

	/**
	* Apply the patch file to the target path
	*/
	bool ApplyPatchFile(FILE* patchFile, const std::string& targetPath, std::string previousVersionNumber);

	/**
	* Apply the patch file to the target path, given the patch name
	*/
	bool ApplyPatchFile(const std::string& patchFileName, const std::string& targetPath, std::string previousVersionNumber);

	/**
	* Restore the target backup list
	*/
	bool RestoreBackup(std::vector<std::string>& backupFileList, const std::string& baseDirectory, std::string previousVersionNumber);

}


#endif // _APPLYPATCH_H_
