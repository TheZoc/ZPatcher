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
#include <cstdint>
#include <cassert>
#include <cerrno>
#include "Lzma2Dec.h"
#include "Lzma2Decoder.h"
#include "LzmaInterfaces.h"
#include "LogSystem.h"

#ifdef _WIN32
	#define ftell64 _ftelli64
	#define fseek64 _fseeki64
#else
 	#define ftell64 ftell
	#define fseek64 fseek
#endif

CLzma2Dec* ZPatcher::InitLzma2Decoder(const Byte& props)
{
	CLzma2Dec* dec = static_cast<CLzma2Dec*>(malloc(sizeof(CLzma2Dec)));
	Lzma2Dec_Construct(dec);

	SRes res = Lzma2Dec_Allocate(dec, props, &LzmaSzAlloc);
	assert(res == SZ_OK);

	return dec;
}

void ZPatcher::DestroyLzma2Decoder(CLzma2Dec* decoder)
{
	Lzma2Dec_Free(decoder, &LzmaSzAlloc);
}

Byte ZPatcher::ReadPatchFileHeader(FILE* source, Byte& Lzma2Properties)
{
	Byte header[9];
	fread(header, 1, 9, source);

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

	const Byte version = header[7];

	// This is a hackish way to allow easy switch from zpatch file version 1 to version 2. TODO: Make this more flexible. Consider restructuring this into fully featured C++ class
	if (version < 1 || version > ZPatcher_Version)
		return 0;

	Lzma2Properties = header[8];

	return version;
}

void ZPatcher::GetFileinfo(FILE* patchFile, std::string& fileName, Byte& operation)
{
	fread(&operation, sizeof(Byte), 1, patchFile);

	uint64_t fileNameLen;
	fread(&fileNameLen, sizeof(uint64_t), 1, patchFile);

	fileName.clear();
	fileName.resize(fileNameLen, '\0');
	fread(&fileName[0], sizeof(char), fileNameLen, patchFile);
}

bool ZPatcher::FileDecompress(CLzma2Dec* decoder, FILE* sourceFile, FILE* destFile)
{
	ELzmaStatus status;

	const SizeT buffer_size = 1 << 16;

	Byte sourceBuffer[buffer_size];
	Byte destBuffer[buffer_size];
	SizeT sourceLen = 0;
	SizeT destLen = buffer_size;

	// We must reinitialize every time we want a decode a new file.
	Lzma2Dec_Init(decoder);

	// Get the compressed size of the file
	uint64_t CompressedSize;
	fread(&CompressedSize, 1, sizeof(uint64_t), sourceFile);
	uint64_t RemainingCompressedData = CompressedSize;

	int64_t sourceFilePos = ftell64(sourceFile);

	while (CompressedSize > 0)
	{
		SRes res;

		if (CompressedSize > buffer_size)
		{
			sourceLen = fread(sourceBuffer, 1, buffer_size, sourceFile);
			res = Lzma2Dec_DecodeToBuf(decoder, destBuffer, &destLen, sourceBuffer, &sourceLen, LZMA_FINISH_ANY, &status);	
		}
		else
		{
			sourceLen = fread(sourceBuffer, 1, RemainingCompressedData, sourceFile);
			res = Lzma2Dec_DecodeToBuf(decoder, destBuffer, &destLen, sourceBuffer, &sourceLen, LZMA_FINISH_END, &status);
		}

		assert(res == SZ_OK);

		fwrite(destBuffer, 1, destLen, destFile);

		CompressedSize -= sourceLen;
		sourceFilePos  += sourceLen;

		res = fseek64(sourceFile, sourceFilePos, SEEK_SET);
		assert(res == 0);

// 		if (res == SZ_OK && status == LZMA_STATUS_FINISHED_WITH_MARK)
// 		{
// 			Log(LOG_FATAL, "Found LZMA Status Finished with Mark!");
// 			break;
// 		}
	}

	return true;
}

// TODO: Remove this ASAP. Rewrite the decompression routines to allow easy decompression of multiple patch file versions. To future me: Consider using a C++ class for decompression.
bool ZPatcher::FileDecompress_Version_1(CLzma2Dec* decoder, FILE* sourceFile, FILE* destFile)
{
	ELzmaStatus status;

	const SizeT buffer_size = 1 << 16;

	Byte sourceBuffer[buffer_size];
	Byte destBuffer[buffer_size];
	SizeT sourceLen = 0;
	SizeT destLen = buffer_size;
	int64_t sourceFilePos = ftell64(sourceFile);

	// We must reinitialize every time we want a decode a new file.
	Lzma2Dec_Init(decoder);

	while (true)
	{
		sourceLen = fread(sourceBuffer, 1, buffer_size, sourceFile);

		SRes res = Lzma2Dec_DecodeToBuf(decoder, destBuffer, &destLen, sourceBuffer, &sourceLen, LZMA_FINISH_ANY, &status);
		assert(res == SZ_OK);

		fwrite(destBuffer, 1, destLen, destFile);

		sourceFilePos += sourceLen;
		res = fseek64(sourceFile, sourceFilePos, SEEK_SET);
		assert(res == 0);

		if (res == SZ_OK && status == LZMA_STATUS_FINISHED_WITH_MARK)
			break;
	}

	return true;
}

bool ZPatcher::FileDecompress(CLzma2Dec* decoder, FILE* sourceFile, const std::string& destFileName, const Byte& version)
{
	FILE* destFile;

	errno = 0;
	destFile = fopen(destFileName.c_str(), "wb");
	if (errno != 0)
	{
		Log(LOG_FATAL, "Error opening file \"%s\" to write updated data: %s", destFileName.c_str(), strerror(errno));
		return false;
	}

	Log(LOG, "Writing File: %s", destFileName.c_str());

	bool success = false;

	if (version == ZPatch_Version_1)
	{
		success = FileDecompress_Version_1(decoder, sourceFile, destFile);
	}
	else
	{
		success = FileDecompress(decoder, sourceFile, destFile);
	}

	fclose(destFile);

	return success;
}
