//////////////////////////////////////////////////////////////////////////
//
// ZPatcher - Patcher system - Part of the ZUpdater suite
// Felipe "Zoc" Silveira - (c) 2016
//
//////////////////////////////////////////////////////////////////////////
//
// DownloadFileWriter.cpp
// Class to download a file from an URL and save to disk
//
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DownloadFileWriter.h"

// #define STOP_DOWNLOAD_AFTER_THIS_MANY_BYTES         500000
// #define MINIMAL_PROGRESS_FUNCTIONALITY_INTERVAL     3

DownloadFileWriter::DownloadFileWriter()
{
	m_pSelf								= nullptr;
	m_CurlHandle						= nullptr;
	m_hDestFile							= nullptr;
	m_LastRunTime						= 0;
	m_CurlErrorMessage[0]				= '\0';
}

int DownloadFileWriter::PrepareFileToWrite(const char* DestFile)
{

	errno_t err;


	err = fopen_s(&m_hDestFile, DestFile, "wb");
	if (err != 0)
	{
		const size_t buffer_size = 1024;
		char buffer[buffer_size];
		strerror_s(buffer, buffer_size, err);
		fprintf(stderr, "Error opening file \"%s\" to reading for writing: %s\n", DestFile, buffer);
		return 1;
	}
	return 0;
}

int DownloadFileWriter::PrepareCurlHandle()
{
	m_CurlHandle = curl_easy_init();
	if (!m_CurlHandle)
	{
		fprintf(stderr, "Error initializing curl\n");
		return -1;
	}

	return 0;
}

void DownloadFileWriter::SetupTransfer(const char* URL)
{
	curl_easy_setopt( m_CurlHandle,	CURLOPT_URL,				URL										);
	curl_easy_setopt( m_CurlHandle,	CURLOPT_USERAGENT,			"ZUpdater/0.1.0"						);
//	curl_easy_setopt( m_CurlHandle,	CURLOPT_TIMEOUT,			0										);

	curl_easy_setopt( m_CurlHandle,	CURLOPT_WRITEFUNCTION,		&DownloadFileWriter::Callback_WriteFile	);
	curl_easy_setopt( m_CurlHandle,	CURLOPT_WRITEDATA,			(void*)this								);

	curl_easy_setopt( m_CurlHandle,	CURLOPT_XFERINFOFUNCTION,	&DownloadFileWriter::TransferInfo		);
	curl_easy_setopt( m_CurlHandle,	CURLOPT_XFERINFODATA,		this									);
	curl_easy_setopt( m_CurlHandle,	CURLOPT_NOPROGRESS,			0L										);

	curl_easy_setopt( m_CurlHandle,	CURLOPT_ERRORBUFFER,		&m_CurlErrorMessage						);
}

CURLcode DownloadFileWriter::StartDownload()
{
	CURLcode CurlResult = CURLE_OK;
	CurlResult = curl_easy_perform(m_CurlHandle);

	if (CurlResult != CURLE_OK)
	{
		fprintf(stderr, "\n\ncurl_easy_perform() failed: %s\nError Message: %s\n", curl_easy_strerror(CurlResult), m_CurlErrorMessage);
	}

	FinishDownload();

	return CurlResult;
}

void DownloadFileWriter::FinishDownload()
{
	if (m_hDestFile)
	{
		fclose(m_hDestFile);
	}
	m_hDestFile = nullptr;

	curl_easy_cleanup(m_CurlHandle);
}

size_t DownloadFileWriter::Callback_WriteFile(void* Buffer, size_t ElementSize, size_t NumElements, void* DownloadFileWriterPointer)
{
	size_t ElementsWritten = 0;
	ElementsWritten = fwrite(Buffer, ElementSize, NumElements, ((DownloadFileWriter*)DownloadFileWriterPointer)->m_hDestFile);
	return ElementsWritten;
}

void DownloadFileWriter::PrintProgressBar(const int Percentage, curl_off_t CurrentDownload)
{
	int barWidth = 80;
	fprintf(stdout, "\xd[");

	int pos = (int)(barWidth * Percentage / 100.0f);
	for (int i = 0; i < barWidth; ++i)
	{
		if (i < pos) fprintf(stdout, "=");
		else if (i == pos) fprintf(stdout, ">");
		else fprintf(stdout, " ");
	}
	fprintf(stdout, "] ");

	if (CurrentDownload / 1048576.0f > 1.0f)
	{
		fprintf(stdout, "%0.2f MiB   ", CurrentDownload / 1048576.0f);
	}
	else// if (CurrentDownload / 1024.0f > 1.0f)
	{
		fprintf(stdout, "%0.2f KiB   ", CurrentDownload / 1024.0f);
	}
}

int DownloadFileWriter::TransferInfo(void *p, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow)
{
	DownloadFileWriter* filewriter = (DownloadFileWriter*)p;
	CURL *curl = filewriter->m_CurlHandle;
	double curtime = 0;

	curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &curtime);

// 	under certain circumstances it may be desirable for certain functionality
// 	to only run every N seconds, in order to do this the transaction time can
// 	be used
// 	if ((curtime - filewriter->m_LastRunTime) >= MINIMAL_PROGRESS_FUNCTIONALITY_INTERVAL)
// 	{
// 		filewriter->m_LastRunTime = curtime;
// 		fprintf(stderr, "\r\nTOTAL TIME: %f\n", curtime);
// 	}

	int percentage = (dltotal == 0) ? 0 : (int)(((float)dlnow / (float)dltotal) * 100);

	PrintProgressBar(percentage, dlnow);

	return 0;
}