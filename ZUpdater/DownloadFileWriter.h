//////////////////////////////////////////////////////////////////////////
//
// ZPatcher - Patcher system - Part of the ZUpdater suite
// Felipe "Zoc" Silveira - (c) 2016
//
//////////////////////////////////////////////////////////////////////////
//
// DownloadFileWriter.h
// Class to download a file from an URL and save to disk
//
//////////////////////////////////////////////////////////////////////////


#ifndef DOWNLOADFILEWRITER_H
#define DOWNLOADFILEWRITER_H

#include "curl/curl.h"

#ifdef WIN32
#include <windows.h>
#endif


// Zoc (2016-04-17): It is mandatory to call curl_global_init() before using DownloadFileWriter!
class DownloadFileWriter
{
public:
	DownloadFileWriter*		m_pSelf;
	CURL*					m_CurlHandle;
	double					m_LastRunTime;
	char					m_CurlErrorMessage[CURL_ERROR_SIZE];

	// Zoc (2016-04-26): File to write
	FILE*					m_hDestFile;



public:
	DownloadFileWriter();

	int						PrepareFileToWrite(const char* DestFile);
	int						PrepareCurlHandle();
	void					SetupTransfer(const char* URL);
	CURLcode				StartDownload();
	void					FinishDownload();

	static size_t			Callback_WriteFile(void* Buffer, size_t ElementSize, size_t NumElements, void* DownloadFileWriterPointer);
	static int				TransferInfo(void *p, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow);
	static void				PrintProgressBar(const int Percentage, curl_off_t CurrentDownload);

};

#endif // DOWNLOADFILEWRITER_H
