//////////////////////////////////////////////////////////////////////////
//
// ZPatcher - Patcher system - Part of the ZUpdater suite
// Felipe "Zoc" Silveira - (c) 2016-2018
//
//////////////////////////////////////////////////////////////////////////
//
// ZPatcherCurrentVersion.h
// This file holds the current version of the patch file being created and
// the supported Patch Operations
//
//////////////////////////////////////////////////////////////////////////

#ifndef _ZPATCHERCURRENTVERSION_H_
#define _ZPATCHERCURRENTVERSION_H_

namespace ZPatcher
{
	enum EZPatcher_Versions
	{
		ZPatch_Version_1	= 1,		// Initial version
		ZPatch_Version_2	= 2,		// Added file structure and size to the patch file
		ZPatch_Version_3	= 3,		// Upgraded LZMA SDK
		ZPatch_MAX			= ZPatch_Version_3,
	};

	static const unsigned char ZPatcher_Version = ZPatch_MAX;

	enum PatchOperation
	{
		Patch_File_Delete		= 1,	// Delete an existing file (Removed on the new version)
		Patch_File_Add			= 2,	// Add a file contained in the patch
		Patch_File_Replace		= 3,	// Replace a file with the one contained in the patch
		Patch_Dir_Add			= 4,	// Add a Directory that is new in the new version
		Patch_MAX				= Patch_Dir_Add,
	};
}


#endif // _ZPATCHERCURRENTVERSION_H_
