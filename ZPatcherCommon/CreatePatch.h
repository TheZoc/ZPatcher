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

namespace ZPatcher
{
	class PatchFileList_t
	{
	public:
		std::vector<std::string> RemovedFileList;
		std::vector<std::string> ModifiedFileList;
		std::vector<std::string> AddedFileList;
	};

	enum PatchOperation
	{
		Patch_File_Delete		= 1,				// Delete an existing file (Removed on the new version)
		Patch_File_AddReplace	= 2,				// Add or Replace a file with the one contained in the patch (File Added or Replaced in the new version)
		Patch_Dir_Add			= 3,				// Add a Directory that is new in the new version
		Patch_MAX				= Patch_Dir_Add,
	};

	PatchFileList_t* GetDifferences(std::string& oldVersion, std::string& newVersion);
	void PrintFileCompareProgressBar(float Percentage);

	void CreatePatchFile(FILE* patchFile, std::string& newVersionPath, PatchFileList_t* patchFileList);
	void CreatePatchFile(std::string& pathFileName, std::string& newVersionPath, PatchFileList_t* patchFileList);
}

#endif // _CREATEPATCH_H_

