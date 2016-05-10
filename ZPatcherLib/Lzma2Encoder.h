//////////////////////////////////////////////////////////////////////////
//
// ZPatcher - Patcher system - Part of the ZUpdater suite
// Felipe "Zoc" Silveira - (c) 2016
//
//////////////////////////////////////////////////////////////////////////
//
// Lzma2Encoder.h
// Header that defines the functions used to encode our patch files
//
//////////////////////////////////////////////////////////////////////////


#ifndef _LZMA2ENCODER_H_
#define _LZMA2ENCODER_H_

#include "ZPatcherCurrentVersion.h"
#include "Lzma2Enc.h"
#include <stdio.h>
#include <string>

namespace ZPatcher
{
	// Initializes the LZMA2 Encoder and returns it's handle
	CLzma2EncHandle InitLzma2Encoder();

	// Destroys the LZMA2 Encoder handle
	void DestroyLzma2EncHandle(CLzma2EncHandle hLzma2Enc);

	// Write the Patch File Header, including the LZMA2 Properties byte
	void WritePatchFileHeader(FILE* dest, Byte &Lzma2Properties);

	// Write the modified file information to the patch file, including the operation
	void WriteFileInfo(FILE* dest, const Byte& operation, const std::string& fileName);

	// Write the modified file data to the patch file.
	void WriteCompressedFile(CLzma2EncHandle hLzma2Enc, FILE* source, FILE* dest);

	// Write the modified file data to the patch file, uses the source file name as param, instead of the file handle.
	void WriteCompressedFile(CLzma2EncHandle hLzma2Enc, std::string& sourceFileName, FILE* dest);
}

#endif // _LZMA2ENCODER_H_
