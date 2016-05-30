//////////////////////////////////////////////////////////////////////////
//
// ZPatcher - Patcher system - Part of the ZUpdater suite
// Felipe "Zoc" Silveira - (c) 2016
//
//////////////////////////////////////////////////////////////////////////
//
// ZUpdater.cpp
// Implement the functions used in the ZUpdater online patching process
//
//////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <string>
#include <vector>
#include <limits.h>
#include <io.h>

#ifdef  _WIN32
	#include <direct.h>
	#include <sstream>
	#include <windows.h>
	#include <shellapi.h>
#else
	#include <unistd.h>
#endif

#include "ApplyPatch.h"
#include "ZUpdater.h"
#include "DownloadFileWriter.h"
#include "LogSystem.h"

#include "TinyXML2.h"
#include "curl/curl.h"
#include "md5.h"



namespace ZUpdater
{
	std::vector<Patch>				g_Patches;
	std::vector<unsigned int>		g_BestPatchPath;
	uint64_t						g_BestPathFileSize	= 0;
	uint64_t						g_LatestVersion		= 0;


	bool GetSmallestUpdatePath(const uint64_t& sourceBuildNumber, const uint64_t& targetBuildNumber, std::vector<unsigned int>& path, uint64_t& pathFileSize)
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

		for (unsigned int patchIndex = 0; patchIndex < g_Patches.size(); ++patchIndex)
		{
			const Patch& patch = g_Patches[patchIndex];

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

	bool CheckForUpdates(const std::string& updateURL, const uint64_t& currentBuildNumber)
	{
		size_t length = updateURL.find_last_of('/');

		if (length == std::string::npos)
		{
			fprintf(stderr, "Invalid Update URL.\n");
			// This is bad! Our URL is malformed (no slashes in it!)
			return false;
		}

		std::string urlBase = std::string(updateURL, 0, length + 1);
		std::string fileName = std::string(updateURL, length + 1, std::string::npos);

		bool fileDownloaded = SimpleDownloadFile(updateURL);

		if (!fileDownloaded)
			return false;

		//////////////////////////////////////////////////////////////////////////
		// File downloaded, check for updates
		//////////////////////////////////////////////////////////////////////////

		int numUpdates = 0;
		std::string description;

		tinyxml2::XMLDocument document;
		document.LoadFile(fileName.c_str());

		tinyxml2::XMLHandle hDocument(&document);

		tinyxml2::XMLHandle hZUpdater					= hDocument.FirstChildElement("zupdater");
		tinyxml2::XMLHandle hBuilds						= hZUpdater.FirstChildElement("builds");

		if (hBuilds.ToElement() == NULL)
		{
			fprintf(stderr, "An error occured while attempting to parse the XML file\n");
			return false;
		}

		for (tinyxml2::XMLHandle hBuild = hBuilds.FirstChildElement("build"); true; hBuild = hBuild.NextSibling())
		{
			// Check if we're done here.
			if (hBuild.ToElement() == NULL)
			{
				break;
			}

			tinyxml2::XMLElement* versionElement		= hBuild.FirstChildElement("version").ToElement();
			tinyxml2::XMLElement* descriptionElement	= hBuild.FirstChildElement("desc").ToElement();

			if (versionElement == NULL || descriptionElement == NULL)
			{
				// If this information/tags aren't present, we assume it's a malformed document.
				return false;
			}

			uint64_t buildNumber = _strtoui64(versionElement->GetText(), nullptr, 10);

			if (buildNumber > g_LatestVersion)
			{
				g_LatestVersion = buildNumber;
			}

			if (buildNumber <= currentBuildNumber)
			{
				// We've gotten all of the newer builds so no need to keep reading.
				break;
			}

			++numUpdates;

			// While we don't currently show the description (TODO!), add it for displaying. This will need a BIG overhaul to work correctly.
			if (!description.empty())
			{
				description += "<br/>";
			}
			description += descriptionElement->GetText();

		}

		// Get the patches.

		tinyxml2::XMLHandle hPatches = hZUpdater.FirstChildElement("patches");

		for (tinyxml2::XMLHandle hPatch = hPatches.FirstChildElement("patch"); true; hPatch = hPatch.NextSibling())
		{
			if (hPatch.ToElement() == NULL)
			{
				break;
			}

			tinyxml2::XMLElement* srcVersionElement	= hPatch.FirstChildElement("source_version").ToElement();
			tinyxml2::XMLElement* dstVersionElement	= hPatch.FirstChildElement("destination_version").ToElement();

			if (dstVersionElement == NULL)
			{
				// If this information/tags aren't present, we assume it's a malformed document.
				return false;
			}

			tinyxml2::XMLElement* fileURLElement	= hPatch.FirstChildElement("file").ToElement();
			tinyxml2::XMLElement* fileSizeElement	= hPatch.FirstChildElement("size").ToElement();
			tinyxml2::XMLElement* md5Element		= hPatch.FirstChildElement("md5").ToElement();


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

			g_Patches.push_back(patch);

		}

		//////////////////////////////////////////////////////////////////////////
		// Update list processed, check for the shortest path
		//////////////////////////////////////////////////////////////////////////

		g_BestPathFileSize = 0;

		if (!GetSmallestUpdatePath(currentBuildNumber, g_LatestVersion, g_BestPatchPath, g_BestPathFileSize))
		{

			// There was no path from the build we have to the latest, so check for a
			// patch that isn't relative.

			for (unsigned int patchIndex = 0; patchIndex < g_Patches.size(); ++patchIndex)
			{
				const Patch& patch = g_Patches[patchIndex];
				if (patch.sourceBuildNumber == 0 && patch.targetBuildNumber == g_LatestVersion)
				{
					g_BestPatchPath.push_back(patchIndex);
					g_BestPathFileSize = patch.fileLength;
					break;
				}
			}

		}

		/*
		fprintf(stderr, "g_BestPatchPath.size() = %llu\r\n", g_BestPatchPath.size());
		for (uint64_t patchIndex = 0; patchIndex < g_BestPatchPath.size(); ++patchIndex)
		{
			const Patch& patch = g_Patches[g_BestPatchPath[patchIndex]];

			fprintf(stderr, "m_patchPath[%d] = %d\r\n", patchIndex, g_BestPatchPath[patchIndex]);
			fprintf(stderr, "Patch file: %s\r\n", patch.fileName.c_str());
			fprintf(stderr, "Patch length: %d\r\n", patch.fileLength);
			fprintf(stderr, "Patch md5: %s\r\n", patch.fileMD5.c_str());
			fprintf(stderr, "srcBuildNumber: %d\r\n", patch.srcBuildNumber);
			fprintf(stderr, "dstBuildNumber: %d\r\n", patch.dstBuildNumber);
			fprintf(stderr, "\r\n=========================================\r\n");
		}
		//*/
		return true;
	}

	uint64_t GetLatestVersion()
	{
		// Think about this as a pauper's version of C++ OOP :P
		return g_LatestVersion;
	}

	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////

	bool DownloadAndApplyPatch(std::string targetDirectory, std::string versionFile, uint64_t currentVersion)
	{
		for (unsigned int patchIndex = 0; patchIndex < g_BestPatchPath.size(); ++patchIndex)
		{
			const Patch& patch = g_Patches[g_BestPatchPath[patchIndex]];

			std::string updatesDirectory = "updates/";

			// Create the updates directory. No problem if the directory already exists.
			int err = 0;
			_set_errno(0);
			_mkdir(updatesDirectory.c_str());
			_get_errno(&err);
			if (err != 0 && err != EEXIST)
			{
				fprintf(stderr, "An error occurred while attempting to create the directory structure.\n");
				system("pause");
				return false;
			}


			// Split the filename from the URL
			size_t length = patch.fileURL.find_last_of('/');

			if (length == std::string::npos)
			{
				fprintf(stderr, "Invalid Update URL.\n");
				// This is bad! Our URL is malformed (no slashes in it!)
				return false;
			}

			std::string urlBase = std::string(patch.fileURL, 0, length + 1);
			std::string fileName = std::string(patch.fileURL, length + 1, std::string::npos);
			std::string localFullPath = (updatesDirectory + fileName);

			bool MD5Matches = false;
			while (!MD5Matches)
			{
				// Check if the target file already exists.
				err = 0;
				_set_errno(0);
				int fileExists = _access(localFullPath.c_str(), 0); // Will return Zero if file exists

				// If the file exists, check if it match the required MD5. If not, re-download the file.
				bool downloadFile = true;
				if (fileExists == 0)
				{
					fprintf(stdout, "\nChecking if MD5 of the file %s matches the required by the patch...\n", fileName.c_str());
					std::string fileMD5 = MD5File(localFullPath);

					fprintf(stdout, "Required MD5:\t%s\n", patch.fileMD5.c_str());
					fprintf(stdout, "File MD5:\t%s\n", fileMD5.c_str());

					if (_stricmp(fileMD5.c_str(), patch.fileMD5.c_str()) == 0)
					{
						fprintf(stdout, "Matches!\n");
						downloadFile = false;
						MD5Matches = true;
					}
					else
					{
						fprintf(stdout, "Does not match! :(\n");

						downloadFile = true;
					}
				}

				// Should we download the file? :)
				if (downloadFile)
				{
					SimpleDownloadFile(patch.fileURL, updatesDirectory);
					fprintf(stdout, "\n");
				}
			}

			// If we got here, the file is downloaded and the MD5 Matches
			fprintf(stdout, "\nApplying patch: %s \n", fileName.c_str());

			ZPatcher::InitLogSystem("./");
			if (ZPatcher::ApplyPatchFile(localFullPath, targetDirectory, currentVersion))
			{
				SaveTargetNewVersion(versionFile, patch.targetBuildNumber);

				if (!SaveTargetNewVersion(versionFile, patch.targetBuildNumber))
				{
					ZPatcher::DestroyLogSystem();
					system("pause");
					return false;
				}

				currentVersion = patch.targetBuildNumber;
				ZPatcher::DestroyLogSystem();
			}
			else
			{
				ZPatcher::DestroyLogSystem();
				system("pause");
				return false;
			}

			//////////////////////////////////////////////////////////////////////////
			// HACK! HACK! HACK! HACK! HACK!
			// TODO: Rewrite this function so that this horrendous hack isn't here!

			bool shouldRestart = false;
			if (!SelfUpdate(shouldRestart))
				return false;
			else
				if (shouldRestart)
					exit(EXIT_SUCCESS);

			//////////////////////////////////////////////////////////////////////////
		}
		return true;
	}

	//////////////////////////////////////////////////////////////////////////

	bool GetTargetCurrentVersion(const std::string& configFile, uint64_t& version)
	{
		FILE* targetFile;
		errno_t err;

		// Open source and target file
		err = fopen_s(&targetFile, configFile.c_str(), "rb");
		if (err != 0 && err != ENOENT)
		{
			const size_t buffer_size = 1024;
			char buffer[buffer_size];
			strerror_s(buffer, buffer_size, err);
			fprintf(stderr, "Error opening config file %s: %s\n", configFile.c_str(), buffer);
			return false;
		}
		else if (err == ENOENT) // File non-existent
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
			fprintf(stderr, "Error reading config file %s\n", configFile.c_str());
			return false;
		}
	}

	bool SaveTargetNewVersion(const std::string& configFile, const uint64_t& version)
	{
		FILE* targetFile;
		errno_t err;

		// Open source and target file
		err = fopen_s(&targetFile, configFile.c_str(), "wb");
		if (err != 0)
		{
			const size_t buffer_size = 1024;
			char buffer[buffer_size];
			strerror_s(buffer, buffer_size, err);
			fprintf(stderr, "Error opening config file for writing %s: %s\n", configFile.c_str(), buffer);
			return false;
		}

		size_t elementsWritten = fwrite(&version, sizeof(uint64_t), 1, targetFile);

		fclose(targetFile);

		if (elementsWritten != 1)
		{
			fprintf(stderr, "Error reading config file %s\n", configFile.c_str());
			return false;
		}

		return true;
	}
	
	//////////////////////////////////////////////////////////////////////////

	std::string MD5File(std::string fileName)
	{
		FILE* targetFile;
		errno_t err;

		// Open source and target file
		err = fopen_s(&targetFile, fileName.c_str(), "rb");
		if (err != 0)
		{
			const size_t buffer_size = 1024;
			char buffer[buffer_size];
			strerror_s(buffer, buffer_size, err);
			fprintf(stderr, "Error opening %s: %s\n", fileName.c_str(), buffer);
			return 0;
		}

		const uint64_t buffer_size = 1 << 16;
		unsigned char readBuffer[buffer_size];
		size_t bytesRead;


		md5_state_t state;
		md5_byte_t digest[16];
		char hex_output[16 * 2 + 1];

		// Read file and generate MD5 hash
		md5_init(&state);

		while (bytesRead = fread(readBuffer, 1, buffer_size, targetFile)) 
			md5_append(&state, (const md5_byte_t*)readBuffer, static_cast<int>(bytesRead));

		fclose(targetFile); // Close the file. It won't be needed anymore.

		md5_finish(&state, digest);

		for (int di = 0; di < 16; ++di)
			snprintf(hex_output + di * 2, 3, "%02x", digest[di]);



		return hex_output;
	}

	//////////////////////////////////////////////////////////////////////////

	bool SimpleDownloadFile(const std::string& URL, const std::string& targetPath)
	{
		DownloadFileWriter			DFW;
		int							DFWError = 0;
		CURLcode					DFWCurlCode = CURLE_OK;

		size_t length = URL.find_last_of('/');

		if (length == std::string::npos)
		{
			fprintf(stderr, "Invalid Update URL.\n");
			// This is bad! Our URL is malformed (no slashes in it!)
			return false;
		}

		std::string urlBase = std::string(URL, 0, length + 1);
		std::string fileName = std::string(URL, length + 1, std::string::npos);

		DFWError = DFW.PrepareFileToWrite((targetPath + fileName).c_str());
		if (DFWError != 0)
		{
			fprintf(stderr, "Error preparing to write file: %s.\n", (targetPath + fileName).c_str());
			return false;		// Zoc (2016-04-26): TODO: Improve this!
		}

		DFWError = DFW.PrepareCurlHandle();
		if (DFWError != 0)
		{
			fprintf(stderr, "Error preparing cURL handle.\n");
			return false;		// Zoc (2016-04-26): TODO: Improve this!
		}

		DFW.SetupTransfer(URL.c_str());

		fprintf(stdout, "Downloading File: %s to %s\n", fileName.c_str(), targetPath.c_str());

		// Download our file
		DFWCurlCode = DFW.StartDownload();

		if (DFWCurlCode != CURLE_OK)
		{
			fprintf(stderr, "Error downloading update XML file.\n");
			return false;
		}

		fprintf(stdout, "\n");

		return true;
	}

	//////////////////////////////////////////////////////////////////////////
	// Windows specific stuff
	bool SelfUpdate(bool &updateFound)
	{
		// Get the ful path to this executable and it's folder name.
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
			FILE* batchFile;
			// Open source and target file
			_set_errno(0);
			errno_t err = fopen_s(&batchFile, tempBatchFile, "wb");
			if (err != 0)
			{
				const size_t buffer_size = 1024;
				char buffer[buffer_size];
				strerror_s(buffer, buffer_size, err);
				fprintf(stderr, "Error opening self-update file for writing %s: %s\n", tempBatchFile, buffer);
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

		return true;
	}
}
