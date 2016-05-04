//////////////////////////////////////////////////////////////////////////
//
// ZPatcher - Patcher system - Part of the ZUpdater suite
// Felipe "Zoc" Silveira - (c) 2016
//
//////////////////////////////////////////////////////////////////////////
//
// Lzma2Decoder.cpp
// Implements the functions used to decode our files
//
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Lzma2Dec.h"
#include "Lzma2Decoder.h"
#include "LzmaInterfaces.h"

CLzma2Dec* ZPatcher::InitLzma2Decoder(const Byte &props)
{
	CLzma2Dec* dec = static_cast<CLzma2Dec*>(malloc(sizeof(CLzma2Dec)));
	Lzma2Dec_Construct(dec);

	SRes res = Lzma2Dec_Allocate(dec, props, &LzmaSzAlloc);
	assert(res == SZ_OK);

	Lzma2Dec_Init(dec);

	return dec;
}

void ZPatcher::FreeLzma2Decoder(CLzma2Dec* decoder)
{
	Lzma2Dec_Free(decoder, &LzmaSzAlloc);
}

bool ZPatcher::ReadPatchFileHeader(FILE* source, Byte &Lzma2Properties)
{
	Byte header[9];
	fread(header, 1, 9, source);

	const Byte version = ZPatcher_Version;

	if (header[0] != 'Z' ||
		header[1] != 'P' ||
		header[2] != 'A' ||
		header[3] != 'T' ||
		header[4] != 'C' ||
		header[5] != 'H' ||
		header[6] != '\u001A')
	{
		return false;
	}

	// It's possible to support multiple versions in future.
	if (header[7] != version)
		return false;

	Lzma2Properties = header[8];

	return true;
}

void ZPatcher::GetFileinfo(FILE* source, std::string& fileName)
{
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	// Discover how to handle this byte! ;)
	// Added here to replicate the Encoder functionality
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	Byte operation;
	fread(&operation, sizeof(Byte), 1, source);

	unsigned long fileNameLen;
	fread(&fileNameLen, sizeof(unsigned long), 1, source);

	fileName.clear();
	fileName.resize(fileNameLen + 1, _T('\0'));
	fread(&fileName[0], sizeof(char), fileNameLen, source);

	// TODO: Add file operation here
}

void ZPatcher::FileDecompress(CLzma2Dec* decoder, FILE* source, FILE* dest)
{
	ELzmaStatus status;

	const SizeT buffer_size = 1 << 16;

	Byte sourceBuffer[buffer_size];
	Byte destBuffer[buffer_size];
	SizeT sourceLen = 0;
	SizeT destLen = buffer_size;
	__int64 sourceFilePos = _ftelli64(source);

	int loop = 0;

	while (true)
	{
		sourceLen = fread(sourceBuffer, 1, buffer_size, source);

		SRes res = Lzma2Dec_DecodeToBuf(decoder, destBuffer, &destLen, sourceBuffer, &sourceLen, LZMA_FINISH_ANY, &status);
		assert(res == SZ_OK);

		fwrite(destBuffer, 1, destLen, dest);

		if (res == SZ_OK && status == LZMA_STATUS_FINISHED_WITH_MARK)
			break;

		sourceFilePos += sourceLen;
		res = _fseeki64(source, sourceFilePos, SEEK_SET);
		assert(res == 0);
	}
}
