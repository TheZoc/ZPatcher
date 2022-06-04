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

#include <vector>

namespace ZPatcher
{
	/// Our function pointer callback for progress display
	typedef void(*ProgressCallback)(const float& percentage);

	/**
	* Print the progress bar used applying a patch
	*/
	void PrintPatchApplyingProgressBar(const float& percentage);

	/**
	* Apply the patch file to the target path, given the patch name
	*/
	bool ApplyPatchFile(const std::string& patchFileName, const std::string& targetPath, const uint64_t& previousVersionNumber, ProgressCallback progressCallback = &PrintPatchApplyingProgressBar);
}


#endif // _APPLYPATCH_H_
