//////////////////////////////////////////////////////////////////////////
//
// ZPatcher - Patcher system - Part of the ZUpdater suite
// Felipe "Zoc" Silveira - (c) 2016
//
//////////////////////////////////////////////////////////////////////////
//
// Lzma2Decoder.h
// Header that defines the functions used to decode our patch files
//
//////////////////////////////////////////////////////////////////////////


#ifndef _LZMA2DECODER_H_
#define _LZMA2DECODER_H_

#include "ZPatcherCurrentVersion.h"
#include "Lzma2Dec.h"

namespace ZPatcher
{
	// Initializes the LZMA2 Decoder and return its handle
	CLzma2Dec* InitLzma2Decoder(const Byte &props);

	// Destroys the LZMA2 Decoder handle
	void FreeLzma2Decoder(CLzma2Dec* decoder);

	// Read the patch file header, returns if it's valid or not and assign the Lzma2 Properties byte to Lzma2Properties param
	bool ReadPatchFileHeader(FILE* source, Byte &Lzma2Properties);

	// Read the modified file information inside the patch file. Return the modified file name and the operation
	void GetFileinfo(FILE* patchFile, std::string& fileName, Byte& operation);

	// Decompress the file data stored in the patch file (only call this in the correct operations!)
	void FileDecompress(CLzma2Dec* decoder, FILE* source, FILE* dest);
}

#endif // _LZMA2DECODER_H_
