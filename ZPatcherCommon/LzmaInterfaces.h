//////////////////////////////////////////////////////////////////////////
//
// ZPatcher - Patcher system - Part of the ZUpdater suite
// Felipe "Zoc" Silveira - (c) 2016
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

#include <malloc.h>
#include <stdio.h>
#include "7zFile.h"

namespace ZPatcher
{
	//////////////////////////////////////////////////////////////////////////
	// Allocators

	static void* LzmaAlloc(void *p, size_t size) { return malloc(size); }
	static void LzmaFree(void *p, void *address) { free(address); }
	static ISzAlloc LzmaSzAlloc = { &LzmaAlloc, &LzmaFree };

	//////////////////////////////////////////////////////////////////////////
	// File Read Struct

	typedef struct
	{
		ISeqInStream SeqInStream;
		FILE* file;

	} ISeqInStreamPlus;

	SRes SeqInStreamPlus_Read(void* p, void* buf, size_t* size);

	//////////////////////////////////////////////////////////////////////////
	// File Write Struct

	typedef struct
	{
		ISeqOutStream SeqOutStream;
		FILE* file;

	} ISeqOutStreamPlus;

	size_t SeqOutStreamPlus_Write(void* p, const void* buf, size_t originalSize);

	//////////////////////////////////////////////////////////////////////////
	// Progress Struct

	typedef struct
	{
		ICompressProgress CompressProgress;
		__int64 TotalSize;
	} ICompressProgressPlus;

	SRes OnProgress(void *p, UInt64 inSize, UInt64 outSize);

	void PrintProgressBar(const int Percentage, UInt64 CurrentDownload);

}

#endif // _LZMAALLOCATORS_H_
