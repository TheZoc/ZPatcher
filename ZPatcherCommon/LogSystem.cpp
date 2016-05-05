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
#include <string>
#include <assert.h>
#include <stdarg.h> 
#include <chrono>
#include <ctime>
#include "LogSystem.h"
#include "FileUtils.h"

FILE* ZPatcher::g_LogSystem = NULL;



void ZPatcher::InitLogSystem(const std::string& location)
{
	std::string logFile = location + "/Logs/ZPatcher-";
	logFile += BuildHumanTimeStamp();
	logFile += ".log";

	NormalizeFileName(logFile);
	CreateDirectoryTree(logFile);

	errno_t err = 0;
	err = fopen_s(&g_LogSystem, logFile.c_str(), "wb");
	assert(err == 0);

	Log(LOG, "Initializing log system");
}

std::string ZPatcher::BuildHumanTimeStamp()
{
	using std::chrono::system_clock;
	time_t tt = system_clock::to_time_t(system_clock::now());

	tm timeinfo;
	localtime_s(&timeinfo, &tt);

	std::string humanTimestamp ;
	
	char buffer[16];
	sprintf_s(buffer, 16, "%02d", timeinfo.tm_year + 1900);
	humanTimestamp += buffer;
	humanTimestamp += "-";
	sprintf_s(buffer, 16, "%02d", timeinfo.tm_mon + 1);
	humanTimestamp += buffer;
	humanTimestamp += "-";
	sprintf_s(buffer, 16, "%02d", timeinfo.tm_mday);
	humanTimestamp += buffer;
	humanTimestamp += "-";
	sprintf_s(buffer, 16, "%02d", timeinfo.tm_hour);
	humanTimestamp += buffer;
	humanTimestamp += "-";
	sprintf_s(buffer, 16, "%02d", timeinfo.tm_min);
	humanTimestamp += buffer;
	humanTimestamp += "-";
	sprintf_s(buffer, 16, "%02d", timeinfo.tm_sec);
	humanTimestamp += buffer;
	return humanTimestamp;
}

void ZPatcher::Log(LogLevel level, const char* format, ...)
{
	if (g_LogSystem == nullptr)
		return;

	va_list args;
	va_start(args, format);

	fprintf_s(g_LogSystem, "[%s] ", BuildHumanTimeStamp().c_str());

	switch (level)
	{
	case LOG:
		fprintf_s(g_LogSystem, "[Log] ");
		break;
	case LOG_WARNING:
		fprintf_s(g_LogSystem, "[Warning] ");
		break;
	case LOG_ERROR:
		fprintf_s(g_LogSystem, "[ERROR] ");
		break;
	case LOG_FATAL:
		fprintf_s(g_LogSystem, "[FATAL] ");
		break;
	default:
		fprintf_s(g_LogSystem, "[???] ");
		break;
	}
	fprintf_s(g_LogSystem, "> ");
	vfprintf_s(g_LogSystem, format, args);
	fprintf_s(g_LogSystem, "\n");
}

void ZPatcher::DestroyLogSystem()
{
	fclose(g_LogSystem);
}

