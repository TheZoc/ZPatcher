//////////////////////////////////////////////////////////////////////////
//
// ZPatcher - Patcher system - Part of the ZUpdater suite
// Felipe "Zoc" Silveira - (c) 2016
//
//////////////////////////////////////////////////////////////////////////
//
// LogSystem.cpp
// Simple log system for the patcher
//
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <cstring>
#include <cassert>
#include <cstdarg>
#include <cerrno>
#include <ctime>
#include <chrono>
#include "LogSystem.h"
#include "FileUtils.h"

FILE* ZPatcher::g_LogSystem = NULL;



void ZPatcher::InitLogSystem(const std::string& location)
{
	std::string logFile = location;

	if (logFile.back() != '/') // Check if it has a trailing slash. If it doesn't, add it.
		logFile += "/";

	logFile += "Logs/ZPatcher-";
	logFile += BuildHumanTimeStamp();
	logFile += ".log";

	NormalizeFileName(logFile);
	CreateDirectoryTree(logFile);

	if (g_LogSystem == nullptr)
	{
		errno = 0;
		g_LogSystem = fopen(logFile.c_str(), "wb");
		if (errno != 0)
		{
			int error = errno;
			fprintf(stderr, "Unable to open log file %s for writing: %s", location.c_str(), strerror(error));
			return;
		}
	}
	else
	{
		Log(LOG_ERROR, "Attempt to initialize log system failed - it was already initialized.");
	}
}

std::string ZPatcher::BuildHumanTimeStamp()
{
	using std::chrono::system_clock;
	time_t tt = system_clock::to_time_t(system_clock::now());

	tm timeinfo;

#ifdef _WIN32
	localtime_s(&timeinfo, &tt);
#else
	localtime_r(&tt, &timeinfo);
#endif

	std::string humanTimestamp;

	char buffer[16];
	sprintf(buffer, "%02d", timeinfo.tm_year + 1900);
	humanTimestamp += buffer;
	humanTimestamp += "-";
	sprintf(buffer, "%02d", timeinfo.tm_mon + 1);
	humanTimestamp += buffer;
	humanTimestamp += "-";
	sprintf(buffer, "%02d", timeinfo.tm_mday);
	humanTimestamp += buffer;
	humanTimestamp += "-";
	sprintf(buffer, "%02d", timeinfo.tm_hour);
	humanTimestamp += buffer;
	humanTimestamp += "-";
	sprintf(buffer, "%02d", timeinfo.tm_min);
	humanTimestamp += buffer;
	humanTimestamp += "-";
	sprintf(buffer, "%02d", timeinfo.tm_sec);
	humanTimestamp += buffer;

	return humanTimestamp;
}

void ZPatcher::Log(LogLevel level, const char* format, ...)
{
	if (g_LogSystem == nullptr)
		return;

	va_list args;
	va_start(args, format);

	fprintf(g_LogSystem, "[%s] ", BuildHumanTimeStamp().c_str());

	switch (level)
	{
	case LOG:
		fprintf(g_LogSystem, "[Log] ");
		break;
	case LOG_WARNING:
		fprintf(g_LogSystem, "[Warning] ");
		break;
	case LOG_ERROR:
		fprintf(g_LogSystem, "[ERROR] ");
		break;
	case LOG_FATAL:
		fprintf(g_LogSystem, "[FATAL] ");
		break;
	default:
		fprintf(g_LogSystem, "[???] ");
		break;
	}
	fprintf(g_LogSystem, "> ");
	vfprintf(g_LogSystem, format, args);
	fprintf(g_LogSystem, "\n");
	fflush(g_LogSystem);
}

void ZPatcher::DestroyLogSystem()
{
	if (g_LogSystem)
		fclose(g_LogSystem);

	g_LogSystem = nullptr;
}
