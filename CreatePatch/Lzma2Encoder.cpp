//////////////////////////////////////////////////////////////////////////
//
// ZPatcher - Patcher system - Part of the ZUpdater suite
// Felipe "Zoc" Silveira - (c) 2016
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

CLzma2EncHandle ZPatcher::InitLzma2Encoder()
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

void ZPatcher::WriteFileInfo(FILE* dest, const std::wstring& fileName)
{
	unsigned long fileNameLen = static_cast<unsigned long>(fileName.length());

	fwrite(&fileNameLen, sizeof(unsigned long), 1, dest);
	fwrite(fileName.c_str(), sizeof(wchar_t), fileNameLen, dest);
}

void ZPatcher::FileCompress(CLzma2EncHandle hLzma2Enc, FILE* source, FILE* dest)
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

