//////////////////////////////////////////////////////////////////////////
//
// ZPatcher - Patcher system - Part of the ZUpdater suite
// Felipe "Zoc" Silveira - (c) 2016-2022
//
//////////////////////////////////////////////////////////////////////////
//
// LogSystem.cpp
// Simple log system for the patcher
//
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <cstdarg>
#include <cstring>
#include <cerrno>
#include <chrono>
#include <ctime>
#include <map>
#include <memory>
#include "LogSystem.h"
#include "FileUtils.h"


// Map that holds all the open log files. CloseLog is a Deleter that will close the file handle when destroying the program.
struct CloseLog { void operator()(FILE* f) { if (f) { fclose(f); } } };
static std::map<std::string, std::unique_ptr<FILE, CloseLog>> g_NewLogSystem;

// This specifies the directory that will receive the log files. TODO: Make this more flexible in the future.
static std::string g_LogDirectory = "./Logs/";

// Current active log name
static std::string ActiveLogName;

/**
 * Build a human-readable timestamp in the format yyyy-mm-dd-hh-mm-ss
 */
static std::string BuildHumanTimeStamp();

/**
 * This function will create the log directory, defined by g_LogDirectory
 * and prepare the file handle to receive the log data.
 */
static bool InitNewLogFile(const std::string& logName);

/**
 * The function that actually does the logging.
 * This should not be called directly.
 */
static void DoLog(const std::string& logName, ZPatcher::LogLevel level, const char* format, va_list args);

static std::string BuildHumanTimeStamp()
{
	using std::chrono::system_clock;
	time_t tt = system_clock::to_time_t(system_clock::now());

	tm timeinfo;

#ifdef _WIN32
	localtime_s(&timeinfo, &tt);
#else
	localtime_r(&tt, &timeinfo);
#endif

	constexpr size_t BUFFER_SIZE = 20;
	char humanTimestamp[BUFFER_SIZE];
	snprintf(humanTimestamp, BUFFER_SIZE, "%02d-%02d-%02d-%02d-%02d-%02d", timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);

	return { humanTimestamp };
}

static bool InitNewLogFile(const std::string& logName)
{
	// Prepare the filename of the log and location that will receive it.
	std::string logFile = g_LogDirectory;

	if (g_LogDirectory.back() != '/') // Check if it has a trailing slash. If it doesn't, add it.
		logFile += "/";

	logFile += BuildHumanTimeStamp() + " - " + logName + ".log";

	ZPatcher::NormalizeFileName(logFile);
	ZPatcher::CreateDirectoryTree(logFile, false);

	if (g_NewLogSystem[logName] == nullptr)
	{
		errno = 0;
		g_NewLogSystem[logName] = std::unique_ptr<FILE, CloseLog>(fopen(logFile.c_str(), "wb"));
		if (errno != 0)
		{
			const int error = errno;
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

static void DoLog(const std::string& logName, ZPatcher::LogLevel level, const char* format, va_list args)
{
	using namespace ZPatcher;

	if (logName.empty())
	{
		fprintf(stderr, "Error attempting to create log entry: The log name can't be empty!!!");
		return;
	}

	if (g_NewLogSystem[logName] == nullptr)
		if (!InitNewLogFile(logName))
				return;

	FILE* targetLog = g_NewLogSystem[logName].get();

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

bool ZPatcher::SetActiveLog(const std::string& logName)
{
	if (logName.empty())
	{
		fprintf(stderr, "Error attempting to set the active log: The log name can't be empty!");
		return false;
	}

	ActiveLogName = logName;
	return true;
}

void ZPatcher::Log(LogLevel level, const char* format, ...)
{
	if (ActiveLogName.empty())
	{
		fprintf(stderr, "Error attempting to log to active log: No active log set!");
		return;
	}

	va_list args;
	va_start(args, format);
	DoLog(ActiveLogName, level, format, args);
	va_end(args);
}

void ZPatcher::LogEx(std::string logName, LogLevel level, const char* format, ...)
{
	if (logName.empty())
	{
		fprintf(stderr, "Error attempting to log entry: The log name can't be empty!");
		return;
	}

	va_list args;
	va_start(args, format);
	DoLog(logName, level, format, args);
	va_end(args);
}
