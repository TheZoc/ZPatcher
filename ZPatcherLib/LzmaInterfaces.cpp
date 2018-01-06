//////////////////////////////////////////////////////////////////////////
//
// ZPatcher - Patcher system - Part of the ZUpdater suite
// Felipe "Zoc" Silveira - (c) 2016-2018
//
//////////////////////////////////////////////////////////////////////////
//
// LzmaInterfaces.h
// Implement the functions used in LZMA structures
//
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LzmaInterfaces.h"

SRes ZPatcher::SeqInStreamPlus_Read(void* p, void* buf, size_t* size)
{
	size_t originalSize = *size;
	if (originalSize == 0)
		return SZ_OK;

	ISeqInStreamPlus *pp = (ISeqInStreamPlus*)p;

	*size = fread(buf, 1, originalSize, pp->file);
	if (*size == originalSize)
		return SZ_OK;

	return ferror(pp->file);
}

size_t ZPatcher::SeqOutStreamPlus_Write(void* p, const void* buf, size_t originalSize)
{
	if (originalSize == 0)
		return SZ_OK;

	ISeqOutStreamPlus *pp = (ISeqOutStreamPlus*)p;
	size_t bytesWritten = 0;

	bytesWritten = fwrite(buf, 1, originalSize, pp->file);
	pp->BytesWritten += bytesWritten;

//	assert(bytesWritten == originalSize);

	return bytesWritten;

	// Zoc (2016-05-03): Handle if we get an error.
	//	return ferror(pp->file);
}

SRes ZPatcher::OnProgress(void *p, UInt64 inSize, UInt64 outSize)
{
	ICompressProgressPlus* progress = reinterpret_cast<ICompressProgressPlus*>(p);

	PrintProgressBar((((float)inSize / (float)progress->TotalSize)*100.0f), outSize);

	return SZ_OK;
	/* Returns: result. (result != SZ_OK) means break.
	Value (UInt64)(Int64)-1 for size means unknown value. */
}


void ZPatcher::PrintProgressBar(const float Percentage, UInt64 CurrentFileSize)
{
	const int progressMaxSize = 10;

	int num = fprintf(stdout, " %0.2f %%", Percentage);

	if (num < progressMaxSize)
		while (num < progressMaxSize)
		{
			fprintf(stdout, " ");
			++num;
		}

	for (int i = 0; i < progressMaxSize; ++i)
		fprintf(stdout, "\b");

	fflush(stdout);
	/*

	int barWidth = 80;

	fprintf(stderr, "\xd[");

	int pos = (int)(barWidth * Percentage / 100.0f);
	for (int i = 0; i < barWidth; ++i)
	{
		if (i < pos) fprintf(stderr, "=");
		else if (i == pos) fprintf(stderr, ">");
		else fprintf(stderr, " ");
	}
	fprintf(stderr, "] ");

	if (CurrentFileSize / 1048576.0f > 1.0f)
	{
		fprintf(stderr, "%0.2f MiB     ", CurrentFileSize / 1048576.0f);
	}
	else if (CurrentFileSize / 1024.0f > 1.0f)
	{
		fprintf(stderr, "%0.2f KiB     ", CurrentFileSize / 1024.0f);
	}
	else
	{
		fprintf(stderr, "%llu B   ", CurrentFileSize);
	}
	*/


	// 	if (Percentage == 100)
	// 		fprintf(stderr, "\n");

}
