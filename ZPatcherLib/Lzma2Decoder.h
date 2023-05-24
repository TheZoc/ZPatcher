//////////////////////////////////////////////////////////////////////////
//
// ZPatcher - Patcher system - Part of the ZUpdater suite
// Felipe "Zoc" Silveira - (c) 2016-2018
//
//////////////////////////////////////////////////////////////////////////
//
// Lzma2Decoder.h
// Header that defines the functions used to decode our patch files
//
//////////////////////////////////////////////////////////////////////////


#ifndef _LZMA2DECODER_H_
#define _LZMA2DECODER_H_

#include <cstdio>
#include <string>
#include "ZPatcherCurrentVersion.h"
#include "Lzma2Dec.h"

namespace ZPatcher
{
	// Initializes the LZMA2 Decoder and return its handle
	CLzma2Dec* InitLzma2Decoder(const Byte& props);

	// Destroys the LZMA2 Decoder handle
	void DestroyLzma2Decoder(CLzma2Dec* decoder);

	// Read the patch file header, returns patch file version if it's valid and assign the Lzma2 Properties byte to Lzma2Properties param. If it's invalid, returns zero.
	Byte ReadPatchFileHeader(FILE* source, Byte& Lzma2Properties);

	// Read the modified file information inside the patch file. Return the modified file name and the operation
	void GetFileinfo(FILE* patchFile, std::string& fileName, Byte& operation);

	// Decompress the file data stored in the patch file (only call this in the correct operations!)
	bool FileDecompress(CLzma2Dec* decoder, FILE* sourceFile, FILE* destFile);

	// [Deprecated] Decompress the file data stored in the patch file. Only used when the version of the patch file is 1.
	bool FileDecompress_Version_1(CLzma2Dec* decoder, FILE* sourceFile, FILE* destFile);

	// Decompress the file data stored in the patch file (only call this in the correct operations!). This operation creates the target file. NOTE: version field is a hack to allow easy migration from patch version 1 to 2. TODO: Rewrite this in a flexible way.
	bool FileDecompress(CLzma2Dec* decoder, FILE* sourceFile, const std::string& destFileName, const Byte& version = ZPatcher_Version);
}

#endif // _LZMA2DECODER_H_
