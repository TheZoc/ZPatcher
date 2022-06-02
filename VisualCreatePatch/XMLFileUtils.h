#pragma once

#include "CreatePatch.h"

class XMLFileUtils
{
public:
	static void SaveXMLPatchFile(const ZPatcher::PatchFileList_t* patchFileList, const std::string& outputPatchFilename);
	static bool LoadXMLPatchFile(const std::string& inputPatchFilename, ZPatcher::PatchFileList_t* patchFileList);
};
