//////////////////////////////////////////////////////////////////////////
//
// ZPatcher - Patcher system - Part of the ZUpdater suite
// Felipe "Zoc" Silveira - (c) 2016-2018
//
//////////////////////////////////////////////////////////////////////////
//
// Lzma2Encoder.cpp
// Implements the functions used to encode our files
//
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Lzma2Enc.h"
#include "Lzma2Encoder.h"
#include "LzmaInterfaces.h"
#include "LogSystem.h"
#include <cstdint>
#include <cerrno>
#include <cassert>

#ifdef _WIN32
	#define ftell64 _ftelli64
	#define fseek64 _fseeki64
#else
 	#define ftell64 ftell
	#define fseek64 fseek
#endif

CLzma2EncHandle ZPatcher::InitLzma2Encoder()
{
	CLzma2EncHandle enc = Lzma2Enc_Create(&LzmaSzAlloc, &LzmaSzAlloc);
	assert(enc);

	CLzma2EncProps props;
	Lzma2EncProps_Init(&props);
	props.lzmaProps.writeEndMark = 0;
	props.lzmaProps.level = 9;
	props.lzmaProps.dictSize = 1 << 27;

	SRes res = Lzma2Enc_SetProps(enc, &props);
	assert(res == SZ_OK);

	return enc;
}

void ZPatcher::DestroyLzma2EncHandle(CLzma2EncHandle hLzma2Enc)
{
	Lzma2Enc_Destroy(hLzma2Enc);
}

void ZPatcher::WritePatchFileHeader(FILE* dest, Byte &Lzma2Properties)
{
	Byte version = ZPatcher_Version;
	Byte header[9] = { 'Z', 'P', 'A', 'T', 'C', 'H', '\u001A', version, Lzma2Properties };

	fwrite(header, 1, 9, dest);
}

void ZPatcher::WriteFileInfo(FILE* dest, const Byte& operation, const std::string& fileName)
{
	uint64_t fileNameLen = static_cast<uint64_t>(fileName.length());

	fwrite(&operation, sizeof(Byte), 1, dest);
	fwrite(&fileNameLen, sizeof(uint64_t), 1, dest);
	fwrite(fileName.c_str(), sizeof(char), fileNameLen, dest);
}

uint64_t ZPatcher::WriteCompressedFile(CLzma2EncHandle hLzma2Enc, FILE* source, FILE* dest, ICompressProgressPlus LZMAProgressCallbackPlus)
{
	ISeqInStreamPlus inputHandler = { { &SeqInStreamPlus_Read }, source };
	ISeqOutStreamPlus outputHandler = { { &SeqOutStreamPlus_Write }, dest, 0 };

	SRes res = Lzma2Enc_Encode2(hLzma2Enc, (ISeqOutStream*)&outputHandler, nullptr, nullptr, (ISeqInStream*)&inputHandler, nullptr, 0, (ICompressProgress*)&LZMAProgressCallbackPlus);

	if (res != SZ_OK)
	{
		fprintf(stderr, "Error in LZMA2Enc_Encode: Returned value was different from SZ_OK.");
		return 0;
	}

	return outputHandler.BytesWritten;
}

bool ZPatcher::WriteCompressedFile(CLzma2EncHandle hLzma2Enc, std::string& sourceFileName, FILE* dest, ICompressProgress LZMAProgressCallback)
{
	FILE* sourceFile;

	errno = 0;
	sourceFile = fopen(sourceFileName.c_str(), "rb");
	if (errno != 0)
	{
		Log(LOG_FATAL, "Error opening file \"%s\" to read updated data: %s", sourceFileName.c_str(), strerror(errno));
		fprintf(stderr, "Error opening file \"%s\" to read updated data: %s", sourceFileName.c_str(), strerror(errno));
		return false;
	}

	// Setup our data structure
	fseek64(sourceFile, 0LL, SEEK_END);
	int64_t	srcFileSize = ftell64(sourceFile);
	rewind(sourceFile);
	ICompressProgressPlus LZMAProgressCallbackPlus = { LZMAProgressCallback, srcFileSize, sourceFileName };

	int64_t		compressedFileSizePosition	= ftell64(dest);	// Not using uint64_t here because ftell64 returns a signed value
	uint64_t	compressedFileSize			= 0;

	fwrite(&compressedFileSize, 1, sizeof(uint64_t), dest);

	compressedFileSize = WriteCompressedFile(hLzma2Enc, sourceFile, dest, LZMAProgressCallbackPlus);

	fseek64(dest, compressedFileSizePosition, SEEK_SET);
	fwrite(&compressedFileSize, 1, sizeof(uint64_t), dest);
	fseek64(dest, 0LL, SEEK_END);

	fclose(sourceFile);
	return (compressedFileSize > 0);
}
