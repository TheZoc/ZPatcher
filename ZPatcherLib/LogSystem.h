//////////////////////////////////////////////////////////////////////////
//
// ZPatcher - Patcher system - Part of the ZUpdater suite
// Felipe "Zoc" Silveira - (c) 2016-2022
//
//////////////////////////////////////////////////////////////////////////
//
// LogSystem.h
// Simple log system for the patcher
//
//////////////////////////////////////////////////////////////////////////

#ifndef _LOGSYSTEM_H_
#define _LOGSYSTEM_H_

#include <string>

namespace ZPatcher
{
	enum LogLevel
	{
		LOG,
		LOG_WARNING,
		LOG_ERROR,
		LOG_FATAL,
	};

	// Set the current Active log file to be used by all Log() calls.
	bool SetActiveLog(const std::string& logName);

	// Add a line to the Active log file - Use SetActiveLog() to set the active log file. Format is the same as printf()
	void Log(LogLevel level, const char* format, ...);

	// Add a line to the Target log file. Format is the same as printf()
 	void LogEx(std::string logName, LogLevel level, const char* format, ...);
}

#endif //_LOGSYSTEM_H_
