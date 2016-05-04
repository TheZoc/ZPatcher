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
	CLzma2Dec* InitLzma2Decoder(const Byte &props);
	void FreeLzma2Decoder(CLzma2Dec* decoder);

	bool ReadPatchFileHeader(FILE* source, Byte &Lzma2Properties);

	void GetFileinfo(FILE* source, std::wstring& fileName);
	void FileDecompress(CLzma2Dec* decoder, FILE* source, FILE* dest);
}

#endif // _LZMA2DECODER_H_
