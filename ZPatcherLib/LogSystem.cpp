//////////////////////////////////////////////////////////////////////////
//
// ZPatcher - Patcher system - Part of the ZUpdater suite
// Felipe "Zoc" Silveira - (c) 2016-2018
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

// Temporary new global variable, avoiding conflict with the old system for now.
std::map<std::string, FILE*> ZPatcher::g_NewLogSystem;

// For now, the log directory will be specified here. 
std::string ZPatcher::g_LogDirectory = "./Logs/";

static std::string ActiveLogName = "";

bool ZPatcher::SetActiveLog(const std::string& logName)
{
	if (logName == "")
	{
		fprintf(stderr, "Error attempting to set the active log: The log name can't be empty!");
		return false;
	}

	ActiveLogName = logName;
	return true;
}

bool ZPatcher::InitNewLogFile(const std::string& logName)
{
	// Prepare the filename of the log and location that will receive it.
	std::string logFile = g_LogDirectory;

	if (g_LogDirectory.back() != '/') // Check if it has a trailing slash. If it doesn't, add it.
		logFile += "/";

	logFile += "Logs/";
	logFile += BuildHumanTimeStamp();
	logFile += " - ";
	logFile += logName;
	logFile += ".log";

	NormalizeFileName(logFile);
	CreateDirectoryTree(logFile, false);

	if (g_NewLogSystem[logName] == nullptr)
	{
		errno = 0;
		g_NewLogSystem[logName] = fopen(logFile.c_str(), "wb");
		if (errno != 0)
		{
			int error = errno;
			fprintf(stderr, "Unable to open log file \"%s\" for writing: %s", logFile.c_str(), strerror(error));
			return false;
		}
	}
	else
	{
		// If this happens, there's something wrong with the code. Print out an error!
		fprintf(stderr, "Attempt to initialize log file \"%s\" failed - it was already initialized.", logFile.c_str());
		return false;
	}
	
	return true;
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

// We don't want anyone calling this form somewhere else.
static void DoLog(std::string logName, ZPatcher::LogLevel level, const char* format, va_list args)
{
	using namespace ZPatcher;

	if (logName == "")
	{
		fprintf(stderr, "Error attempting to create log entry: The log name can't be empty!!!");
		return;
	}

	if (g_NewLogSystem[logName] == nullptr)
		if (!InitNewLogFile(logName))
				return;

	FILE* targetLog = g_NewLogSystem[logName];

	fprintf(targetLog, "[%s] ", BuildHumanTimeStamp().c_str());

	switch (level)
	{
	case LOG:
		fprintf(targetLog, "[Log] ");
		break;
	case LOG_WARNING:
		fprintf(targetLog, "[Warning] ");
		break;
	case LOG_ERROR:
		fprintf(targetLog, "[ERROR] ");
		break;
	case LOG_FATAL:
		fprintf(targetLog, "[FATAL] ");
		break;
	default:
		fprintf(targetLog, "[???] ");
		break;
	}
	fprintf(targetLog, "> ");
	vfprintf(targetLog, format, args);
	fprintf(targetLog, "\n");
	fflush(targetLog);
}

void ZPatcher::Log(std::string logName, LogLevel level, const char* format, ...)
{
	if (logName == "")
	{
		fprintf(stderr, "Error attempting to log entry: The log name can't be empty!");
		return;
	}

	va_list args;
	va_start(args, format);
	DoLog(logName, level, format, args);
	va_end(args);
}

void ZPatcher::Log(LogLevel level, const char* format, ...)
{
	if (ActiveLogName == "")
	{
		fprintf(stderr, "Error attempting to log to active log: No active log set!");
		return;
	}

	va_list args;
	va_start(args, format);
	DoLog(ActiveLogName, level, format, args);
	va_end(args);
}

void ZPatcher::DestroyLogSystem()
{
	for(auto &logEntry : g_NewLogSystem)
		if (logEntry.second)
		{
			fclose(logEntry.second);
			logEntry.second = nullptr;
		}
}

