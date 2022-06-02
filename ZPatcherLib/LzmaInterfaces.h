//////////////////////////////////////////////////////////////////////////
//
// ZPatcher - Patcher system - Part of the ZUpdater suite
// Felipe "Zoc" Silveira - (c) 2016-2018
//
//////////////////////////////////////////////////////////////////////////
//
// LzmaAllocators.h
// Header that defines the functions used to allocate and free memory both
// in LZMA2 Encoder and Decoder
//
//////////////////////////////////////////////////////////////////////////

#ifndef _LZMAALLOCATORS_H_
#define _LZMAALLOCATORS_H_

#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <string>

#ifdef _WIN32
	#include <malloc.h>
#endif

#include "7zFile.h"

namespace ZPatcher
{
	//////////////////////////////////////////////////////////////////////////
	// File Read Struct

	typedef struct
	{
		ISeqInStream SeqInStream;
		FILE* file;

	} ISeqInStreamPlus;

	SRes SeqInStreamPlus_Read(const ISeqInStream* p, void* buf, size_t* size);

	//////////////////////////////////////////////////////////////////////////
	// File Write Struct

	typedef struct
	{
		ISeqOutStream SeqOutStream;
		FILE* file;
		uint64_t BytesWritten;
	} ISeqOutStreamPlus;

	size_t SeqOutStreamPlus_Write(const ISeqOutStream* p, const void* buf, size_t originalSize);

	//////////////////////////////////////////////////////////////////////////
	// Progress Struct

	typedef struct
	{
		ICompressProgress CompressProgress;
		int64_t TotalSize;
		std::string FileName;
	} ICompressProgressPlus;

	SRes OnProgress(ICompressProgress* p, UInt64 inSize, UInt64 outSize);

	void PrintProgressBar(const float Percentage, UInt64 CurrentDownload);

	//////////////////////////////////////////////////////////////////////////
	// Updated LZMA interfaces
	//
	// In the latest version of LZMA SDK, a set of parameters were converted
	// from void pointers to named struct pointers.
	// This typedef aids in keeping it human-readable when using the already
	// existing structs to the new LZMA SDK.

	typedef SRes(*CompressProgressCallback)(const ICompressProgress *p, UInt64 inSize, UInt64 outSize);
}

#endif // _LZMAALLOCATORS_H_
