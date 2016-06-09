//////////////////////////////////////////////////////////////////////////
//
// ZLauncher - Patcher system - Part of the ZUpdater suite
// Felipe "Zoc" Silveira - (c) 2016
//
//////////////////////////////////////////////////////////////////////////
//
// ZLauncherThread.cpp
// Implement worker thread used in the launcher process
//
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <string>
#include <sstream>
#include <errno.h>
#include "curl/curl.h"
#include "wx/thread.h"
#include "tinyxml2.h"
#include "ZLauncherFrame.h"
#include "ZLauncherThread.h"
#include "LogSystem.h"
#include "FileUtils.h"
#include "LogSystem.h"
#include "DownloadFileWriter.h"
#include "md5.h"
#include "ApplyPatch.h"

//////////////////////////////////////////////////////////////////////////
// Define the events used to update the create patch frame

wxDEFINE_EVENT(wxEVT_COMMAND_UPDATE_PROGRESS_BAR, wxThreadEvent);
wxDEFINE_EVENT(wxEVT_COMMAND_UPDATE_PROGRESS_TEXT, wxThreadEvent);

wxDEFINE_EVENT(wxEVT_COMMAND_HTML_SET_CONTENT, wxThreadEvent);
wxDEFINE_EVENT(wxEVT_COMMAND_HTML_LOAD_PAGE, wxThreadEvent);

wxDEFINE_EVENT(wxEVT_COMMAND_ENABLE_LAUNCH_BUTTON, wxThreadEvent);

//////////////////////////////////////////////////////////////////////////

ZLauncherThread::ZLauncherThread(ZLauncherFrame *handler, const wxString& updateURL, const wxString& versionFile, const wxString& targetDirectory)
	: wxThread(wxTHREAD_DETACHED)
{
	m_pHandler = handler;

	m_updateURL = updateURL;
	m_versionFile = versionFile;
	m_targetDirectory = targetDirectory;
}

ZLauncherThread::~ZLauncherThread()
{
	wxCriticalSectionLocker enter(m_pHandler->m_pThreadCS);

	// the thread is being destroyed; make sure not to leave dangling pointers around
	m_pHandler->m_pThread = NULL;
}

wxThread::ExitCode ZLauncherThread::Entry()
{
	// TestDestroy() checks if the thread should be cancelled/destroyed
	if (TestDestroy()) {
		ZPatcher::DestroyLogSystem();
		return (wxThread::ExitCode)0; }

	ZPatcher::InitLogSystem("./", "ZLauncher");
	std::ostringstream tmphtml;

	//////////////////////////////////////////////////////////////////////////
	// Get the current version
	if (TestDestroy()) { ZPatcher::DestroyLogSystem(); return (wxThread::ExitCode)0; }
	if (!GetTargetCurrentVersion(m_versionFile, m_LocalCurrentVersion))
	{
		ZLauncherFrame::UpdateProgress(0, "An error occurred while getting current application version. See the log for details.");
		ZPatcher::DestroyLogSystem();
		return (wxThread::ExitCode)0;
	}

	tmphtml << "<b>Current Version: " << m_LocalCurrentVersion << "</b>";
	ZLauncherFrame::HTMLSetContent(tmphtml.str());

	//////////////////////////////////////////////////////////////////////////
	// Check the Updates URL for the XML file with updates and process it.
	if (TestDestroy()) { ZPatcher::DestroyLogSystem(); return (wxThread::ExitCode)0; }
	if (!CheckForUpdates(m_updateURL, m_LocalCurrentVersion))
	{
		ZLauncherFrame::UpdateProgress(0, "An error occurred while checking for updates. See the log for details.");
		ZPatcher::DestroyLogSystem();
		return (wxThread::ExitCode)0;
	}

	//////////////////////////////////////////////////////////////////////////
	// CheckForUpdates() adds a bunch of new information. Add everything here.
	tmphtml.str("");
	tmphtml.clear();
	tmphtml << "<h1>" << m_ApplicationName << "</h1>";
	tmphtml << "<b>Current Version: " << m_LocalCurrentVersion << "</b>";
	tmphtml << "<br /><b>Latest Version: " << GetLatestVersion() << "</b>";
	tmphtml << m_UpdateDescription;
	ZLauncherFrame::HTMLSetContent(tmphtml.str());

	if (m_LatestVersion > m_LocalCurrentVersion)
		ZLauncherFrame::UpdateProgress(0, "Preparing patch download.");


	//////////////////////////////////////////////////////////////////////////
	// Create the directory where the updates will be downloaded.
	if (TestDestroy()) { ZPatcher::DestroyLogSystem(); return (wxThread::ExitCode)0; }
	std::string updatesDirectory = "updates/";
	errno = 0;
	ZPatcher::CreateDirectoryTree(updatesDirectory);
	if (errno != 0 && errno != EEXIST)
	{
		ZPatcher::Log(ZPatcher::LOG_FATAL, "An error occurred while attempting to create the directory structure: %s", updatesDirectory.c_str(), strerror(errno));
		ZLauncherFrame::UpdateProgress(0, "An error occurred while attempting to create the directory structure. See the log for details.");
		ZPatcher::DestroyLogSystem();
		return (wxThread::ExitCode)0;
	}


#ifdef _WIN32
	//////////////////////////////////////////////////////////////////////////
	// Now this is an ugly hack for windows that checks if the patcher was updated.
	// Do this once before starting to apply the patch and one after each patch.

	bool shouldRestart = false;
	if (!SelfUpdate(shouldRestart))
	{
		ZPatcher::Log(ZPatcher::LOG_FATAL, "Error trying to perform a self update.");
		return false;
	}
	else
		if (shouldRestart)
		{
			ZPatcher::DestroyLogSystem();
			wxQueueEvent(wxTheApp->GetTopWindow()->GetEventHandler(), new wxCloseEvent(wxEVT_CLOSE_WINDOW));
			return (wxThread::ExitCode)0;
		}
#endif

	//////////////////////////////////////////////////////////////////////////
	// Do the update loop!
	for (unsigned int patchIndex = 0; patchIndex < m_BestPatchPath.size(); ++patchIndex)
	{
		// Check if we should stop before each file download
		if (TestDestroy()) { ZPatcher::DestroyLogSystem(); return (wxThread::ExitCode)0; }

		const Patch& patch = m_Patches[m_BestPatchPath[patchIndex]];

		// Split the filename from the URL
		size_t length = patch.fileURL.find_last_of('/');
		if (length == std::string::npos)
		{
			// This is bad! Our URL is malformed (no slashes in it!)
			ZPatcher::Log(ZPatcher::LOG_FATAL, "Invalid Update URL: %s", patch.fileURL.c_str());
			return (wxThread::ExitCode)0;
;
		}

		// Adjust the path accordingly
		std::string urlBase = std::string(patch.fileURL, 0, length + 1);
		std::string fileName = std::string(patch.fileURL, length + 1, std::string::npos);
		std::string localFullPath = (updatesDirectory + fileName);

		ZPatcher::DestroyLogSystem();
		// We're going to store a log for each patch applied.
		ZPatcher::InitLogSystem("./", "ZLauncher_" + fileName);

		// Check if we already have a file downloaded with the same name as the patch.
		// If the file exists, check if the MD5 hash matches. If not, re-download the file.
		bool MD5Matches = false;
		while (!MD5Matches)
		{
			errno = 0;
#ifdef _WIN32
			int fileExists = _access(localFullPath.c_str(), 0); // Will return Zero if file exists
#else
			int fileExists = access(localFullPath.c_str(), F_OK); // Will return Zero if file exists
#endif // _WIN32

			bool downloadFile = true;
			if (fileExists == 0)
			{
				std::string fileMD5 = MD5File(localFullPath);
#ifdef _WIN32
				if (_stricmp(fileMD5.c_str(), patch.fileMD5.c_str()) == 0)
#else
				if (strcasecmp(fileMD5.c_str(), patch.fileMD5.c_str()) == 0)
#endif
				{
					downloadFile = false;
					MD5Matches = true;
				}
				else
				{
					downloadFile = true;
				}
			}

			// Should we download the file? :)
			if (downloadFile)
			{

				if (!SimpleDownloadFile(patch.fileURL, updatesDirectory))
				{
					// User attempted to close the window, abort operation!
					ZPatcher::Log(ZPatcher::LOG_WARNING, "Download aborted by user.");
					ZPatcher::DestroyLogSystem();
					return (wxThread::ExitCode)0;
				}
			}
		}

		// This is a great place to check for a stopping condition.
		if (TestDestroy()) { ZPatcher::DestroyLogSystem(); return (wxThread::ExitCode)0; }

		m_pHandler->m_pThreadCS.Enter();

		// At this point, we have the patch file and the MD5 hash matches. Apply it!
		if (ZPatcher::ApplyPatchFile(localFullPath, m_targetDirectory, m_LocalCurrentVersion, &ZLauncherFrame::ApplyPatchProgress))
		{
			if (!SaveTargetNewVersion(m_versionFile, patch.targetBuildNumber))
			{
				ZPatcher::Log(ZPatcher::LOG_FATAL, "The patch was applied, but couldn't write the updated version info to %s", m_versionFile.c_str());
				ZPatcher::DestroyLogSystem();
				m_pHandler->m_pThreadCS.Leave();
				return (wxThread::ExitCode)0;
			}

			m_LocalCurrentVersion = patch.targetBuildNumber;
		}
		else
		{
			ZPatcher::Log(ZPatcher::LOG_FATAL, "Unable to apply the patch %s", localFullPath.c_str());
			ZPatcher::DestroyLogSystem();
			m_pHandler->m_pThreadCS.Leave();
			return (wxThread::ExitCode)0;
		}

		m_pHandler->m_pThreadCS.Leave();


#ifdef _WIN32
		//////////////////////////////////////////////////////////////////////////
		// Now this is an ugly hack for windows that checks if the patcher was updated.

		bool shouldRestart = false;
		if (!SelfUpdate(shouldRestart))
		{
			ZPatcher::Log(ZPatcher::LOG_FATAL, "Error trying to perform a self update.");
			return false;
		}
		else
			if (shouldRestart)
			{
				ZPatcher::DestroyLogSystem();
				wxQueueEvent(wxTheApp->GetTopWindow()->GetEventHandler(), new wxCloseEvent(wxEVT_CLOSE_WINDOW));
				return (wxThread::ExitCode)0;
			}
#endif
	}
	ZPatcher::DestroyLogSystem();

	ZLauncherFrame::UpdateProgress(100, "Up-to-date!");
	ZLauncherFrame::EnableLaunchButton(true);

	// Enable launch button


	return (wxThread::ExitCode)0;     // success
}

//////////////////////////////////////////////////////////////////////////
bool ZLauncherThread::CheckForUpdates(const std::string& updateURL, const uint64_t& currentBuildNumber)
{
	size_t length = updateURL.find_last_of('/');

	if (length == std::string::npos)
	{
		// This is bad! Our URL is malformed (no slashes in it!)
		ZPatcher::Log(ZPatcher::LOG_FATAL, "Invalid Update URL: %s", updateURL.c_str());
		return false;
	}

	std::string urlBase = std::string(updateURL, 0, length + 1);
	std::string fileName = std::string(updateURL, length + 1, std::string::npos);

	// Inform the user we are downloading the update file and will process it
	wxThreadEvent* updateProgressTextEvent = new wxThreadEvent(wxEVT_COMMAND_UPDATE_PROGRESS_TEXT);
	updateProgressTextEvent->SetString(wxString::Format("Downloading update information..."));
	wxQueueEvent(wxTheApp->GetTopWindow()->GetEventHandler(), updateProgressTextEvent);

	bool fileDownloaded = SimpleDownloadFile(updateURL);

	if (!fileDownloaded)
		return false;

	//////////////////////////////////////////////////////////////////////////
	// File downloaded, check for updates
	//////////////////////////////////////////////////////////////////////////

	int numUpdates = 0;

	tinyxml2::XMLDocument document;
	document.LoadFile(fileName.c_str());

	tinyxml2::XMLHandle hDocument(&document);

	tinyxml2::XMLHandle hZUpdater = hDocument.FirstChildElement("zupdater");

	tinyxml2::XMLHandle hApplication = hZUpdater.FirstChildElement("application");
	if (hApplication.ToElement() == NULL)
	{
		ZPatcher::Log(ZPatcher::LOG_FATAL, "An error occurred while attempting to parse the XML file: (hApplication.ToElement() == NULL)");
		return false;
	}

	m_ApplicationName = hApplication.ToElement()->GetText();

	tinyxml2::XMLHandle hBuilds = hZUpdater.FirstChildElement("builds");

	if (hBuilds.ToElement() == NULL)
	{
		ZPatcher::Log(ZPatcher::LOG_FATAL, "An error occurred while attempting to parse the XML file: (hBuilds.ToElement() == NULL)");
		return false;
	}

	for (tinyxml2::XMLHandle hBuild = hBuilds.FirstChildElement("build"); true; hBuild = hBuild.NextSibling())
	{
		// Check if we're done here.
		if (hBuild.ToElement() == NULL)
		{
			break;
		}

		tinyxml2::XMLElement* versionElement = hBuild.FirstChildElement("version").ToElement();
		tinyxml2::XMLElement* descriptionElement = hBuild.FirstChildElement("desc").ToElement();

		if (versionElement == NULL || descriptionElement == NULL)
		{
			// If this information/tags aren't present, we assume it's a malformed document.
			return false;
		}

		uint64_t buildNumber = strtoull(versionElement->GetText(), nullptr, 10);

		if (buildNumber > m_LatestVersion)
		{
			m_LatestVersion = buildNumber;
		}

		if (buildNumber < currentBuildNumber)
		{
			// We've gotten all of the newer builds so no need to keep reading.
			break;
		}

		++numUpdates;

		// While we don't currently show the description (TODO!), add it for displaying. This will need a BIG overhaul to work correctly.
		if (!m_UpdateDescription.empty())
		{
			m_UpdateDescription += "<br/>";
		}
		m_UpdateDescription += descriptionElement->GetText();

	}

	// Get the patches.

	tinyxml2::XMLHandle hPatches = hZUpdater.FirstChildElement("patches");

	for (tinyxml2::XMLHandle hPatch = hPatches.FirstChildElement("patch"); true; hPatch = hPatch.NextSibling())
	{
		if (hPatch.ToElement() == NULL)
		{
			break;
		}

		tinyxml2::XMLElement* srcVersionElement = hPatch.FirstChildElement("source_version").ToElement();
		tinyxml2::XMLElement* dstVersionElement = hPatch.FirstChildElement("destination_version").ToElement();

		if (dstVersionElement == NULL)
		{
			// If this information/tags aren't present, we assume it's a malformed document.
			return false;
		}

		tinyxml2::XMLElement* fileURLElement = hPatch.FirstChildElement("file").ToElement();
		tinyxml2::XMLElement* fileSizeElement = hPatch.FirstChildElement("size").ToElement();
		tinyxml2::XMLElement* md5Element = hPatch.FirstChildElement("md5").ToElement();


		if (fileSizeElement == NULL || fileURLElement == NULL || md5Element == NULL)
		{
			// If this information/tags aren't present, we assume it's a malformed document.
			return false;
		}

		Patch patch;

		if (srcVersionElement != NULL)
		{
			patch.sourceBuildNumber = atoi(srcVersionElement->GetText());
		}
		else
		{
			// If a version is not specified, assume it's the base version (aka: not a patch, the entire game/application).
			patch.sourceBuildNumber = 0;
		}

		patch.targetBuildNumber = atoi(dstVersionElement->GetText());

		if (patch.sourceBuildNumber >= patch.targetBuildNumber)
		{
			// Downgrades aren't supported. By not supporting them, we can use a simple DFS to find the best patch path, avoiding loops, etc.
			continue;
		}

		const char* fileSizeText = fileSizeElement->GetText();
		if (fileSizeText != NULL)
		{
			patch.fileLength = atoi(fileSizeText);
		}
		else
		{
			patch.fileLength = 0;
		}


		patch.fileMD5 = md5Element->GetText();
		patch.fileURL = fileURLElement->GetText();

		// If the file name is not absolute, make it so.

		if (strncmp(patch.fileURL.c_str(), "http://", 7) != 0)
		{
			patch.fileURL = urlBase + patch.fileURL;
		}

		m_Patches.push_back(patch);

	}

	//////////////////////////////////////////////////////////////////////////
	// Update list processed, check for the shortest path
	//////////////////////////////////////////////////////////////////////////

	m_BestPathDownloadSize = 0;

	if (!GetSmallestUpdatePath(currentBuildNumber, m_LatestVersion, m_BestPatchPath, m_BestPathDownloadSize))
	{
		// There was no path from the build we have to the latest, so check for a
		// patch that isn't relative.
		for (unsigned int patchIndex = 0; patchIndex < m_Patches.size(); ++patchIndex)
		{
			const Patch& patch = m_Patches[patchIndex];
			if (patch.sourceBuildNumber == 0 && patch.targetBuildNumber == m_LatestVersion)
			{
				m_BestPatchPath.push_back(patchIndex);
				m_BestPathDownloadSize = patch.fileLength;
				break;
			}
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////

uint64_t ZLauncherThread::GetLatestVersion()
{
	return m_LatestVersion;
}

//////////////////////////////////////////////////////////////////////////

bool ZLauncherThread::GetSmallestUpdatePath(const uint64_t& sourceBuildNumber, const uint64_t& targetBuildNumber, std::vector<unsigned int>& path, uint64_t& pathFileSize)
{
	// Our current path is a path from the start to the destination builds.
	if (sourceBuildNumber == targetBuildNumber)
	{
		return true;
	}

	// Zoc (2016-04-21): Ahhh, the good, old "Depth First Search". Since this is a small graph, it's okay to use this.
	// TODO: Convert this to Breadth First Search? It would allow us the "downgrade" feature with the proper setup.
	// The cost to be minimized is the download total filesize. This is a brute-force method that explores all paths.
	std::vector<unsigned int> bestPath;
	uint64_t bestPathFileSize = ULLONG_MAX;

	for (unsigned int patchIndex = 0; patchIndex < m_Patches.size(); ++patchIndex)
	{
		const Patch& patch = m_Patches[patchIndex];

		if (patch.sourceBuildNumber == sourceBuildNumber)
		{
			std::vector<unsigned int> testPath;
			testPath.push_back(patchIndex);

			uint64_t testPathFileSize = patch.fileLength;

			// Find the best path recursively
			if (GetSmallestUpdatePath(patch.targetBuildNumber, targetBuildNumber, testPath, testPathFileSize))
			{
				// Check if we DO have a path. If we do, check if it's better than the one we currently have.
				if (bestPath.empty() || testPathFileSize < bestPathFileSize)
				{
					bestPath = testPath;
					bestPathFileSize = testPathFileSize;
				}
			}
		}
	}

	if (!bestPath.empty())
	{
		path.insert(path.end(), bestPath.begin(), bestPath.end());
		pathFileSize += bestPathFileSize;
		return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////

bool ZLauncherThread::GetTargetCurrentVersion(const std::string& configFile, uint64_t& version)
{
	FILE* targetFile;

	// Open target file
	errno = 0;
	targetFile = fopen(configFile.c_str(), "rb");
	if (errno != 0 && errno != ENOENT)
	{
		ZPatcher::Log(ZPatcher::LOG_FATAL, "Error opening version file \"%s\" to read version information: %s", configFile.c_str(), strerror(errno));
		return false;
	}
	else if (errno == ENOENT) // File non-existent
	{
		version = 0;
		return true;
	}

	size_t version_read;
	size_t elementsRead = fread(&version_read, sizeof(size_t), 1, targetFile);
	fclose(targetFile);

	if (elementsRead == 1)
	{
		version = version_read;
		return true;
	}
	else
	{
		ZPatcher::Log(ZPatcher::LOG_FATAL, "Error reading config file \"%s\"", configFile.c_str(), strerror(errno));
		return false;
	}
}

std::string ZLauncherThread::MD5File(std::string fileName)
{
	FILE* targetFile;

	// Open target file
	errno = 0;
	targetFile = fopen(fileName.c_str(), "rb");
	if (errno != 0)
	{
		ZPatcher::Log(ZPatcher::LOG_FATAL, "Error opening file \"%s\" to calculate MD5 Hash: %s", fileName.c_str(), strerror(errno));
		return "";
	}

	// Buffer to read the file
	const uint64_t buffer_size = 1 << 16;
	unsigned char readBuffer[buffer_size];
	size_t bytesRead;

	// MD5 hashing utilities
	md5_state_t state;
	md5_byte_t digest[16];
	char hex_output[16 * 2 + 1];

	// Read file and generate MD5 hash
	md5_init(&state);

	while ((bytesRead = fread(readBuffer, 1, buffer_size, targetFile))) 
		md5_append(&state, (const md5_byte_t*)readBuffer, static_cast<int>(bytesRead));

	fclose(targetFile); // Close the file. It won't be needed anymore.
	md5_finish(&state, digest);

	for (int di = 0; di < 16; ++di)
		snprintf(hex_output + di * 2, 3, "%02x", digest[di]);

	// Return MD5 hash
	return hex_output;
}


bool ZLauncherThread::SaveTargetNewVersion(const std::string& configFile, const uint64_t& version)
{
	FILE* targetFile;

	// Open target file
	errno = 0;
	targetFile = fopen(configFile.c_str(), "wb");
	if (errno != 0)
	{
		ZPatcher::Log(ZPatcher::LOG_FATAL, "Error opening version file \"%s\" to write version information: %s", configFile.c_str(), strerror(errno));
		return false;
	}

	size_t elementsWritten = fwrite(&version, sizeof(uint64_t), 1, targetFile);

	fclose(targetFile);

	if (elementsWritten != 1)
	{
		ZPatcher::Log(ZPatcher::LOG_FATAL, "Error writing version file \"%s\"", configFile.c_str());
		return false;
	}

	return true;
}


//////////////////////////////////////////////////////////////////////////

bool ZLauncherThread::SimpleDownloadFile(const std::string& URL, const std::string& targetPath /*= "./"*/)
{
	DownloadFileWriter			DFW(this);
	int							DFWError = 0;
	CURLcode					DFWCurlCode = CURLE_OK;

	size_t length = URL.find_last_of('/');

	if (length == std::string::npos)
	{
		// This is bad! Our URL is malformed (no slashes in it!)
		ZPatcher::Log(ZPatcher::LOG_FATAL, "Invalid Update URL: %s", URL.c_str());
		return false;
	}

	std::string urlBase = std::string(URL, 0, length + 1);
	std::string fileName = std::string(URL, length + 1, std::string::npos);

	DFWError = DFW.PrepareFileToWrite(targetPath + fileName);
	if (DFWError != 0)
	{
		ZPatcher::Log(ZPatcher::LOG_FATAL, "Error preparing to write file: \"%s\"", (targetPath + fileName).c_str());
		return false;		// Zoc (2016-04-26): TODO: Improve this!
	}

	DFWError = DFW.PrepareCurlHandle();
	if (DFWError != 0)
	{
		ZPatcher::Log(ZPatcher::LOG_FATAL, "Error preparing cURL handle.");
		return false;		// Zoc (2016-04-26): TODO: Improve this!
	}

	DFW.SetTransferInfoFunction(&ZLauncherFrame::TransferInfo);

	DFW.SetupTransfer(URL.c_str());

	ZPatcher::Log(ZPatcher::LOG, "Downloading File \"%s\" to \"%s\"", fileName.c_str(), targetPath.c_str());

	// Download our file
	DFWCurlCode = DFW.StartDownload();

	if (DFWCurlCode != CURLE_OK)
	{
		ZPatcher::Log(ZPatcher::LOG_FATAL, "Error downloading file \"%s\"", fileName.c_str());
		return false;
	}

	return true;
}

#ifdef _WIN32
//////////////////////////////////////////////////////////////////////////
// Windows specific stuff

bool ZLauncherThread::SelfUpdate(bool &updateFound)
{
	// The Batch file destruction technique was adapted from: http://www.codeproject.com/Articles/4027/Writing-a-self-destructing-exe-file

	// Get the full path to this executable and it's folder name.
	char updaterFullPath[_MAX_PATH];
	char updatedSelfExecutableName[_MAX_PATH];
	char folder[_MAX_PATH];
	char tempBatchFile[_MAX_PATH];
	char* pb;

	GetModuleFileNameA(NULL, updaterFullPath, _MAX_PATH);

	// Full path to the temporary batch file
	strcpy_s(updatedSelfExecutableName, _MAX_PATH, updaterFullPath);
	pb = strrchr(updatedSelfExecutableName, '.');
	strcpy_s(pb, 6, "a.exe");

	// Get the folder name
	strcpy_s(folder, _MAX_PATH, updaterFullPath);
	pb = strrchr(folder, '\\');
	if (pb != NULL)
		*pb = '\0';

	// Full path to the temporary batch file
	strcpy_s(tempBatchFile, _MAX_PATH, updaterFullPath);
	pb = strrchr(tempBatchFile, '.');
	strcpy_s(pb, 5, ".bat");

	_set_errno(0);
	int fileExists = _access(updatedSelfExecutableName, 0); // Will return Zero if file exists

	if (fileExists == 0)
	{
		updateFound = true;
		FILE* batchFile;
		// Open source and target file
		_set_errno(0);
		errno_t err = fopen_s(&batchFile, tempBatchFile, "wb");
		if (err != 0)
		{
			const size_t buffer_size = 1024;
			char buffer[buffer_size];
			strerror_s(buffer, buffer_size, err);
			ZPatcher::Log(ZPatcher::LOG_FATAL, "Error opening self-update file for writing %s: %s", tempBatchFile, buffer);
			return false;
		}

		std::stringstream stream;

		// Wait until the application has ended and delete it
		stream << ":Repeat" << std::endl;
		stream << "del \"" << updaterFullPath << "\"" << std::endl;
		stream << "if exist \"" << updaterFullPath << "\" goto Repeat" << std::endl;

		// Copy updated executable
		stream << "copy /b /v /y \"" << updatedSelfExecutableName << "\" \"" << updaterFullPath << "\"" << std::endl;

		// Delete original updated executable (already copied)
		stream << "del \"" << updatedSelfExecutableName << "\"" << std::endl;

		// Start a new instance of the executable
		stream << "start \"\" \"" << updaterFullPath << "\"" << std::endl;

		// Delete this batch file.
		stream << "del \"" << tempBatchFile << "\"" << std::endl;

		fwrite(stream.str().c_str(), 1, strlen(stream.str().c_str()), batchFile);

		fclose(batchFile);

		// Execute the self-destruct batch file and exit the program.
		ShellExecuteA(NULL, "open", tempBatchFile, NULL, NULL, SW_HIDE);
	}
	else
	{
		updateFound = false;
	}

	return true;
}
#endif //_WIN32
