// CreatePatch.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Lzma2Encoder.h"
#include "Lzma2Decoder.h"
#include "FileUtils.h"
#include "CreatePatch.h"

int main(int argc, char* argv[])
{
	using namespace ZPatcher;

	if (argc < 4)
	{
		fprintf(stderr, "CreatePatch.exe <old version directory> <new version directory> <output patch file>\n");
		exit(EXIT_SUCCESS);
	}

	// Store our targets
	std::string oldDirectory = argv[1];
	std::string newDirectory = argv[2];
	std::string outputFilename = argv[3];

	// Make sure all directories are represented in the same format
	NormalizeFileName(oldDirectory);
	NormalizeFileName(newDirectory);
	NormalizeFileName(outputFilename);

	PatchFileList_t* patchFileList = GetDifferences(oldDirectory, newDirectory);

	CreatePatchFile(outputFilename, newDirectory, patchFileList);

	system("pause");

	exit(EXIT_SUCCESS);

}


//////////////////////////////////////////////////////////////////////////
// Main function 


/*
int main()
{
std::vector<std::string> fileList;

ZPatcher::GetFilesInDirectory(".", "", fileList);

for (std::vector<std::string>::iterator itr = fileList.begin(); itr < fileList.end(); ++itr)
{
fprintf(stderr, "%s\n", itr->c_str());
}

system("pause");
}
*/


/*
int main()
{
using namespace ZPatcher;

FILE* sourceFile;
FILE* destFile;

errno_t err = 0;

char sourceName[] = _T("a.jpg");
char compressedName[] = _T("test.zoc");
char destName[] = _T("b.jpg");

// 	char sourceName[] = _T("a.txt");
// 	char compressedName[] = _T("txt.zoc");
// 	char destName[] = _T("b.txt");

err = fopen_s(&sourceFile, sourceName, _T("rb"));
assert(err == 0);
err = fopen_s(&destFile, compressedName, _T("wb"));
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

err = fopen_s(&sourceFile, compressedName, _T("rb"));
assert(err == 0);
err = fopen_s(&destFile, destName, _T("wb"));
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

//*/
