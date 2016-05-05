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
	CreateDirectoryTree(logFile);

	errno_t err = 0;
	err = fopen_s(&g_LogSystem, logFile.c_str(), "wb");
	assert(err == 0);
}

std::string ZPatcher::BuildHumanTimeStamp()
{
	using std::chrono::system_clock;
	time_t tt = system_clock::to_time_t(system_clock::now());

	tm timeinfo;
	localtime_s(&timeinfo, &tt);

	std::string humanTimestamp;
	humanTimestamp = (timeinfo.tm_year + 1900);
	humanTimestamp += "-";
	humanTimestamp += (timeinfo.tm_mon + 1);
	humanTimestamp += "-";
	humanTimestamp += (timeinfo.tm_mday);
	humanTimestamp += "-";
	humanTimestamp += (timeinfo.tm_hour);
	humanTimestamp += "-";
	humanTimestamp += (timeinfo.tm_min);
	humanTimestamp += "-";
	humanTimestamp += (timeinfo.tm_sec);

	return humanTimestamp;
}

void ZPatcher::Log(const char* format, ...)
{
	if (g_LogSystem == nullptr)
		return;

	va_list args;
	va_start(args, format);

	fprintf_s(g_LogSystem, "%s > ", BuildHumanTimeStamp().c_str());
	fprintf_s(g_LogSystem, format, args);
	fprintf_s(g_LogSystem, "\n");
}

void ZPatcher::DestroyLogSystem()
{
	fclose(g_LogSystem);
}

