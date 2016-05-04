// CreatePatch.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Lzma2Encoder.h"
#include "Lzma2Decoder.h"

//////////////////////////////////////////////////////////////////////////
// Main function 

int main()
{
	using namespace ZPatcher;

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
		WritePatchFileHeader(destFile, props);
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
	if (!ReadPatchFileHeader(sourceFile, props))
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

	system("pause");

	return 0;
}
