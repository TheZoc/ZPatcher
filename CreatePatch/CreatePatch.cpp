// CreatePatch.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <malloc.h>
#include <vector>
#include <assert.h>
#include <stdio.h>
#include "7zTypes.h"
#include "LzmaLib.h"
#include "Lzma2Enc.h"
#include "Lzma2Dec.h"

#define PATCH_VERSION 1

//////////////////////////////////////////////////////////////////////////
// Forward declaration

void FileCompress(CLzma2EncHandle hLzma, FILE* source, FILE* dest);
void FileDecompress(CLzma2Dec* decoder, FILE* source, FILE* dest);
void PrintProgressBar(const int Percentage, UInt64 CurrentDownload);

void WriteFileHeader(FILE* dest, Byte &Lzma2Properties);
void WriteFileInfo(FILE* dest, const std::wstring& fileName);
CLzma2EncHandle InitLzma2Encoder();
void DestroyLzma2EncHandle(CLzma2EncHandle hLzma2Enc);

bool ReadFileHeader(FILE* source, Byte &Lzma2Properties);
void GetFileinfo(FILE* source, std::wstring& fileName);
CLzma2Dec* InitLzma2Decoder(const Byte &props);
void FreeLzma2Decoder(CLzma2Dec* decoder);


//////////////////////////////////////////////////////////////////////////
// Allocators

static void* LzmaAlloc(void *p, size_t size) { return malloc(size); }
static void LzmaFree(void *p, void *address) { free(address); }
static ISzAlloc LzmaSzAlloc = { &LzmaAlloc, &LzmaFree };

//////////////////////////////////////////////////////////////////////////
// Progress Struct

typedef struct
{
	ICompressProgress CompressProgress;
	__int64 TotalSize;
} ICompressProgressPlus;


SRes OnProgress(void *p, UInt64 inSize, UInt64 outSize)
{
	ICompressProgressPlus* progress = reinterpret_cast<ICompressProgressPlus*>(p);

	PrintProgressBar((int)(((float)inSize / (float)progress->TotalSize)*100.0f), outSize);
	//	fprintf(stderr, "Progress: %llu / %llu / %llu \n", inSize, progress->TotalSize, outSize);

	return SZ_OK;
	/* Returns: result. (result != SZ_OK) means break.
	Value (UInt64)(Int64)-1 for size means unknown value. */
}

//////////////////////////////////////////////////////////////////////////
// File Read Struct

typedef struct
{
	ISeqInStream SeqInStream;
	FILE* file;

} ISeqInStreamPlus;


SRes SeqInStreamPlus_Read(void* p, void* buf, size_t* size)
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

//////////////////////////////////////////////////////////////////////////
// File Write Struct

typedef struct
{
	ISeqOutStream SeqOutStream;
	FILE* file;

} ISeqOutStreamPlus;

size_t SeqOutStreamPlus_Write(void* p, const void* buf, size_t originalSize)
{
	if (originalSize == 0)
		return SZ_OK;

	ISeqOutStreamPlus *pp = (ISeqOutStreamPlus*)p;
	size_t bytesWritten = 0;

	bytesWritten = fwrite(buf, 1, originalSize, pp->file);

	assert(bytesWritten == originalSize);

	return bytesWritten;

	// Zoc (2016-05-03): Handle if we get an error.
	//	return ferror(pp->file);
}

//////////////////////////////////////////////////////////////////////////
// Main function 

int main()
{
	FILE* sourceFile;
	FILE* destFile;

	errno_t err = 0;

	wchar_t sourceName[] = _T("a.jpg");
	wchar_t compressedName[] = _T("test.zoc");
	wchar_t destName[] = _T("b.jpg");

	// 	wchar_t sourceName[] = _T("a.txt");
	// 	wchar_t compressedName[] = _T("txt.zoc");
	// 	wchar_t destName[] = _T("b.txt");

	err = _wfopen_s(&sourceFile, sourceName, _T("rb"));
	assert(err == 0);
	err = _wfopen_s(&destFile, compressedName, _T("wb"));
	assert(err == 0);

	CLzma2EncHandle hLzma2Enc = InitLzma2Encoder();

	{
		// This makes props a local variable
		Byte props = Lzma2Enc_WriteProperties(hLzma2Enc);
		WriteFileHeader(destFile, props);
	}

	WriteFileInfo(destFile, destName);
	FileCompress(hLzma2Enc, sourceFile, destFile);
	DestroyLzma2EncHandle(hLzma2Enc);

	fclose(sourceFile);
	fclose(destFile);

	fprintf(stderr, "\nFile compression completed.\n");

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	err = _wfopen_s(&sourceFile, compressedName, _T("rb"));
	assert(err == 0);
	err = _wfopen_s(&destFile, destName, _T("wb"));
	assert(err == 0);

	Byte props;
	if (!ReadFileHeader(sourceFile, props))
	{
		fprintf(stderr, "\nFile decompression failed - Invalid header.\n");
		system("pause");
		return 1;
	}

	CLzma2Dec* decoder = InitLzma2Decoder(props);

	std::wstring outFileName;
	GetFileinfo(sourceFile, outFileName);

	fprintf(stderr, "\nDecompressing file %ls.\n", outFileName.c_str());

	FileDecompress(decoder, sourceFile, destFile);

	fclose(sourceFile);
	fclose(destFile);

	fprintf(stderr, "\nFile decompression completed.\n");
	//*/

	system("pause");

	return 0;
}

void WriteFileHeader(FILE* dest, Byte &Lzma2Properties)
{
	Byte version = PATCH_VERSION;
	Byte header[9] = { 'Z', 'P', 'A', 'T', 'C', 'H', '\u001A', version, Lzma2Properties };

	fwrite(header, 1, 9, dest);
}

void WriteFileInfo(FILE* dest, const std::wstring& fileName)
{
	unsigned long fileNameLen = static_cast<unsigned long>(fileName.length());

	fwrite(&fileNameLen, sizeof(unsigned long), 1, dest);
	fwrite(fileName.c_str(), sizeof(wchar_t), fileNameLen, dest);
}

CLzma2EncHandle InitLzma2Encoder()
{
	CLzma2EncHandle enc = Lzma2Enc_Create(&LzmaSzAlloc, &LzmaSzAlloc);
	assert(enc);

	CLzma2EncProps props;
	Lzma2EncProps_Init(&props);
	props.lzmaProps.writeEndMark = 1;
	props.lzmaProps.level = 9;
	props.lzmaProps.dictSize = 1 << 27;

	SRes res = Lzma2Enc_SetProps(enc, &props);
	assert(res == SZ_OK);

	return enc;
}

void DestroyLzma2EncHandle(CLzma2EncHandle hLzma2Enc)
{
	Lzma2Enc_Destroy(hLzma2Enc);
}

void FileCompress(CLzma2EncHandle hLzma2Enc, FILE* source, FILE* dest)
{
	_fseeki64(source, 0LL, SEEK_END);
	__int64 srcFileSize = _ftelli64(source);
	_fseeki64(source, 0LL, SEEK_SET);

	ICompressProgressPlus ProgressCallback = { &OnProgress, srcFileSize };
	ISeqInStreamPlus inputHandler = { &SeqInStreamPlus_Read, source };
	ISeqOutStreamPlus outputHandler = { &SeqOutStreamPlus_Write, dest };

	SRes res = Lzma2Enc_Encode(hLzma2Enc, (ISeqOutStream*)&outputHandler, (ISeqInStream*)&inputHandler, (ICompressProgress*)&ProgressCallback);
	assert(res == SZ_OK);
}

//////////////////////////////////////////////////////////////////////////

bool ReadFileHeader(FILE* source, Byte &Lzma2Properties)
{
	Byte header[9];
	fread(header, 1, 9, source);

	const Byte version = PATCH_VERSION;

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

void GetFileinfo(FILE* source, std::wstring& fileName)
{
	unsigned long fileNameLen;
	fread(&fileNameLen, sizeof(unsigned long), 1, source);

	fileName.clear();
	fileName.resize(fileNameLen + 1, _T('\0'));
	fread(&fileName[0], sizeof(wchar_t), fileNameLen, source);

	// TODO: Add file operation here
}

CLzma2Dec* InitLzma2Decoder(const Byte &props)
{
	CLzma2Dec* dec = static_cast<CLzma2Dec*>(malloc(sizeof(CLzma2Dec)));
	Lzma2Dec_Construct(dec);

	SRes res = Lzma2Dec_Allocate(dec, props, &LzmaSzAlloc);
	assert(res == SZ_OK);

	Lzma2Dec_Init(dec);

	return dec;
}


void FreeLzma2Decoder(CLzma2Dec* decoder)
{
	Lzma2Dec_Free(decoder, &LzmaSzAlloc);
}

void FileDecompress(CLzma2Dec* decoder, FILE* source, FILE* dest)
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



void PrintProgressBar(const int Percentage, UInt64 currentFileSize)
{
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

	if (currentFileSize / 1048576.0f > 1.0f)
	{
		fprintf(stderr, "%0.2f MiB     ", currentFileSize / 1048576.0f);
	}
	else if (currentFileSize / 1024.0f > 1.0f)
	{
		fprintf(stderr, "%0.2f KiB     ", currentFileSize / 1024.0f);
	}
	else
	{
		fprintf(stderr, "%llu B   ", currentFileSize);
	}

	// 	if (Percentage == 100)
	// 		fprintf(stderr, "\n");
}
