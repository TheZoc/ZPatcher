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
#include <cerrno>
#include <cstring>
#include "DownloadFileWriter.h"
#include "LogSystem.h"

DownloadFileWriter::DownloadFileWriter()
{
	m_pSelf								= nullptr;
	m_CurlHandle						= nullptr;
	m_LastUpdateTime						= 0;
	m_CurlErrorMessage[0]				= '\0';

	m_pTransferInfoFunc					= nullptr;
	m_hDestFile							= nullptr;
	m_DestFileName						= "";
}

int DownloadFileWriter::PrepareFileToWrite(const std::string& DestFile)
{
	errno = 0;
	m_hDestFile = fopen(DestFile.c_str(), "wb");
	if (errno != 0)
	{
		ZPatcher::Log(ZPatcher::LOG_FATAL, "Error opening file \"%s\" to write download data: %s", DestFile.c_str(), strerror(errno));
		return 1;
	}

	m_DestFileName = DestFile;
	return 0;
}

int DownloadFileWriter::PrepareCurlHandle()
{
	m_CurlHandle = curl_easy_init();
	if (!m_CurlHandle)
	{
		ZPatcher::Log(ZPatcher::LOG_FATAL, "Error initializing curl");
		return -1;
	}

	return 0;
}

void DownloadFileWriter::SetTransferInfoFunction(TransferInfoFunc func)
{
	m_pTransferInfoFunc = func;
}

void DownloadFileWriter::SetupTransfer(const std::string& URL)
{
	curl_easy_setopt( m_CurlHandle,	CURLOPT_URL,				URL.c_str()																		);
	curl_easy_setopt( m_CurlHandle,	CURLOPT_USERAGENT,			"ZLauncher/1.0.0"																);

	curl_easy_setopt( m_CurlHandle, CURLOPT_FOLLOWLOCATION,		1L																				);
	curl_easy_setopt( m_CurlHandle, CURLOPT_MAXREDIRS,			10																				);

	curl_easy_setopt( m_CurlHandle,	CURLOPT_WRITEFUNCTION,		&DownloadFileWriter::Callback_WriteFile											);
	curl_easy_setopt( m_CurlHandle,	CURLOPT_WRITEDATA,			(void*)this																		);

	curl_easy_setopt(m_CurlHandle, CURLOPT_XFERINFOFUNCTION,	m_pTransferInfoFunc ? m_pTransferInfoFunc : &DownloadFileWriter::TransferInfo	); // Pay attention! This uses a ternary if!
	curl_easy_setopt( m_CurlHandle,	CURLOPT_XFERINFODATA,		this																			);
	curl_easy_setopt( m_CurlHandle,	CURLOPT_NOPROGRESS,			0L																				);

	curl_easy_setopt( m_CurlHandle,	CURLOPT_ERRORBUFFER,		&m_CurlErrorMessage																);
}

CURLcode DownloadFileWriter::StartDownload()
{
	CURLcode CurlResult = CURLE_OK;
	CurlResult = curl_easy_perform(m_CurlHandle);

	if (CurlResult != CURLE_OK)
	{
		ZPatcher::Log(ZPatcher::LOG_FATAL, "curl_easy_perform() failed: %s\nError Message: %s", curl_easy_strerror(CurlResult), m_CurlErrorMessage);
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

CURL* DownloadFileWriter::GetCurlHandle()
{
	return m_CurlHandle;
}

std::string DownloadFileWriter::GetDestFileName()
{
	return m_DestFileName;
}

double DownloadFileWriter::GetLastUpdateTime()
{
	return m_LastUpdateTime;
}

void DownloadFileWriter::SetLastUpdateTime(const double& lastUpdate)
{
	m_LastUpdateTime = lastUpdate;
}

size_t DownloadFileWriter::Callback_WriteFile(void* Buffer, size_t ElementSize, size_t NumElements, void* DownloadFileWriterPointer)
{
	size_t ElementsWritten = 0;
	ElementsWritten = fwrite(Buffer, ElementSize, NumElements, ((DownloadFileWriter*)DownloadFileWriterPointer)->m_hDestFile);
	return ElementsWritten;
}

void DownloadFileWriter::PrintProgressBar(const int Percentage, curl_off_t CurrentDownload)
{
	static const int bufferSize = 15;
	char buffer[bufferSize];

	// Write file size remaining to a buffer first, so the number of characters
	//   can be subtracted from the maximum progress bar size
	int charsForProgress = 0;
	if (CurrentDownload / 1048576.0f > 1.0f)
	{
		charsForProgress = snprintf(buffer, bufferSize, "%0.2f MiB ", CurrentDownload / 1048576.0f);
	}
	else// if (CurrentDownload / 1024.0f > 1.0f)
	{
		charsForProgress = snprintf(buffer, bufferSize, "%0.2f KiB ", CurrentDownload / 1024.0f);
	}


	static const int fixedChars = 3; // Number of static progress bar chars ("[" and "] ")
	int barWidth = 80 - fixedChars - charsForProgress;
#ifdef  _WIN32
	fprintf(stdout, "\xd[");
#else
	// http://stackoverflow.com/a/6774395
	fprintf(stdout, "\r\033[K[");
#endif

	int pos = (int)(barWidth * Percentage / 100.0f);
	for (int i = 0; i < barWidth; ++i)
	{
		if (i < pos) fprintf(stdout, "=");
		else if (i == pos) fprintf(stdout, ">");
		else fprintf(stdout, " ");
	}
	fprintf(stdout, "] ");

	fputs(buffer, stdout);

	fflush(stdout);
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
