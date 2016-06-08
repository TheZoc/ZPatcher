//////////////////////////////////////////////////////////////////////////
//
// ZLauncher - Patcher system - Part of the ZUpdater suite
// Felipe "Zoc" Silveira - (c) 2016
//
//////////////////////////////////////////////////////////////////////////
//
// ZLauncherThread.cpp
// Header file for the worker thread used in the launcher process
//
//////////////////////////////////////////////////////////////////////////

#ifndef _ZLAUNCHERTHREAD_H_
#define _ZLAUNCHERTHREAD_H_

#include <wx/thread.h>
#include <wx/event.h>
#include <vector>
#include "ZLauncherFrame.h"

//////////////////////////////////////////////////////////////////////////
// Declare the events used to update the main frame

wxDECLARE_EVENT(wxEVT_COMMAND_UPDATE_PROGRESS_BAR, wxThreadEvent);
wxDECLARE_EVENT(wxEVT_COMMAND_UPDATE_PROGRESS_TEXT, wxThreadEvent);

wxDECLARE_EVENT(wxEVT_COMMAND_HTML_SET_CONTENT, wxThreadEvent);
wxDECLARE_EVENT(wxEVT_COMMAND_HTML_LOAD_PAGE, wxThreadEvent);

wxDECLARE_EVENT(wxEVT_COMMAND_ENABLE_LAUNCH_BUTTON, wxThreadEvent);


//////////////////////////////////////////////////////////////////////////
// Auxiliary Patch structure

struct Patch
{
	uint64_t					sourceBuildNumber;
	uint64_t					targetBuildNumber;

	std::string					fileURL;
	DWORD						fileLength;
	std::string					fileMD5;
};

//////////////////////////////////////////////////////////////////////////
// Implement the worker thread

class ZLauncherThread : public wxThread
{
private:
	ZLauncherThread(); // Not allowed to be called

public:
	ZLauncherThread(ZLauncherFrame *handler, const wxString& updateURL, const wxString& versionFile, const wxString& targetDirectory);
	~ZLauncherThread();
	
protected:
	virtual ExitCode Entry();
	ZLauncherFrame*	m_pHandler;

	std::string					m_updateURL;
	std::string					m_versionFile;
	std::string					m_targetDirectory;

	std::string					m_ApplicationName;
	uint64_t					m_LocalCurrentVersion;
	uint64_t					m_LatestVersion;
	uint64_t					m_BestPathDownloadSize;
	std::vector<Patch>			m_Patches;
	std::vector<unsigned int>	m_BestPatchPath;
	std::string					m_UpdateDescription;


	//////////////////////////////////////////////////////////////////////////
	// This functions are everything the thread needs to get the updates applied correctly.
	// Those functions 

	/**
	* Download the Update XML from the server and process it. The output is stored in m_Patches.
	* Returns if file was downloaded and processed successfully
	*/
	bool CheckForUpdates(const std::string& updateURL, const uint64_t& currentBuildNumber);

	/**
	* Returns the latest version fetched from the XML.
	*/
	uint64_t GetLatestVersion();

	/**
	* With all the patches stored in g_Patches, find the best path to download updates, using the file download size as the value that should be minimized.
	* Returns true if there is a path from the source version to the target version (i.e. if the update is possible)
	*/
	bool GetSmallestUpdatePath(const uint64_t& sourceBuildNumber, const uint64_t& targetBuildNumber, std::vector<unsigned int>& path, uint64_t& pathFileSize);

	/**
	* Returns true if it was able to determine the current version stored in configFile.
	* The found version is returned in the "version" variable.
	* NOTE: If no file is found, it returns true and version gets the value 0 (i.e. No previous version found, perform full installation)
	*/
	bool GetTargetCurrentVersion(const std::string& configFile, uint64_t& version);

	/**
	* Calculate the MD5 hash of a file
	*/
	std::string MD5File(std::string fileName);

	/**
	* Updates given configFile to store the supplied version
	*/
	bool SaveTargetNewVersion(const std::string& configFile, const uint64_t& version);

	/**
	* Download file, from given URL, without renaming, to given targetPath.
	* Note: The filename to be saved is the last part (i.e. after last slash) of given URL. Complex URLs might be a problem for this function.
	*/
	bool SimpleDownloadFile(const std::string& URL, const std::string& targetPath = "./");

#ifdef _WIN32

	//////////////////////////////////////////////////////////////////////////
	// Windows specific stuff

	/**
	* If the updater finds a file with it's own name with an appended 'a' character (e.g. For Zupdater.exe, searches for ZUpdatera.exe),
	* it will replace itself with the updated found version and restart the application.
	* it will return true if no write error happened.
	* Please note, YOU (yes, you!) need to explicitly finish the application after calling this function and it returns updateFound == true
	*/
	bool SelfUpdate(bool &updateFound);

#endif // _WIN32

};

#endif // _ZLAUNCHERTHREAD_H_
