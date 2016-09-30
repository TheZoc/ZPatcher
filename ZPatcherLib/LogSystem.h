//////////////////////////////////////////////////////////////////////////
//
// ZPatcher - Patcher system - Part of the ZUpdater suite
// Felipe "Zoc" Silveira - (c) 2016
//
//////////////////////////////////////////////////////////////////////////
//
// LogSystem.h
// Simple log system for the patcher
//
//////////////////////////////////////////////////////////////////////////

#ifndef _LOGSYSTEM_H_
#define _LOGSYSTEM_H_

#include <cstdio>
#include <string>
#include <map>

namespace ZPatcher
{
	// Map that holds all the open log files.
	extern std::map<std::string, FILE*> g_NewLogSystem;

	// This specifies the directory that will receive the log files
	extern std::string g_LogDirectory;

	enum LogLevel
	{
		LOG,
		LOG_WARNING,
		LOG_ERROR,
		LOG_FATAL,
	};

	// This will create the directory structure defined in 
	// This function will create a Log/ directory and place files in there.
	//No logging will be done if this isn't called.
	bool InitNewLogFile(const std::string& logName);

	bool SetActiveLog(const std::string& logName);

	// Build a human-readable timestamp in the format yyyy-mm-dd-hh-mm-ss
	std::string BuildHumanTimeStamp();

	// Add a line to the target log file. The format should be the same as the one used in printf
	void Log(std::string logName, LogLevel level, const char* format, ...);

	// Add a line to the ACTIVE log file. Use SetActiveLog() to set the active log file.
	void Log(LogLevel level, const char* format, ...);

	// Close the log file
	void DestroyLogSystem();
}

#endif //_LOGSYSTEM_H_
#pragma once
