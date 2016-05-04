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

namespace ZPatcher
{
	CLzma2EncHandle InitLzma2Encoder();
	void DestroyLzma2EncHandle(CLzma2EncHandle hLzma2Enc);

	void WritePatchFileHeader(FILE* dest, Byte &Lzma2Properties);

	void WriteFileInfo(FILE* dest, const Byte& operation, const std::string& fileName);

	void WriteCompressedFile(CLzma2EncHandle hLzma2Enc, FILE* source, FILE* dest);
	void WriteCompressedFile(CLzma2EncHandle hLzma2Enc, std::string& sourceFileName, FILE* dest);
}

#endif // _LZMA2ENCODER_H_
