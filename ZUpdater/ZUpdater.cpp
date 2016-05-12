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

#include "ZUpdater.h"
#include "DownloadFileWriter.h"

#include "TinyXML2.h"
#include "curl/curl.h"
#include "md5.h"

namespace ZUpdater
{
	std::vector<Patch>				g_Patches;
	std::vector<unsigned int>		g_BestPatchPath;
	unsigned long long				g_BestPathFileSize	= 0;
	unsigned long long				g_LatestVersion		= 0;

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

		const unsigned long long buffer_size = 1 << 16;
		unsigned char readBuffer[buffer_size];
		size_t bytesRead;

		MD5 fileMD5;

		while (bytesRead = fread(readBuffer, 1, buffer_size, targetFile))
			fileMD5.update(readBuffer, static_cast<MD5::size_type>(bytesRead));

		fileMD5.finalize();

		fclose(targetFile);

		return fileMD5.hexdigest();
	}

	bool GetUpdatePath(const unsigned long long& sourceBuildNumber, const unsigned long long& targetBuildNumber, std::vector<unsigned int>& path, unsigned long long& pathFileSize)
	{
		// Our current path is a path from the start to the destination builds.
		if (sourceBuildNumber == targetBuildNumber)
		{
			return true;
		}

		// Zoc (2016-04-21): Ahhh, the good, old "Depth First Search". Since this is a small graph, it's fine to use this.
		// The cost to be minimized is the download total filesize. This is a brute-force method that explores all paths.
		std::vector<unsigned int> bestPath;
		unsigned long long bestPathFileSize = ULLONG_MAX;

		for (unsigned int patchIndex = 0; patchIndex < g_Patches.size(); ++patchIndex)
		{
			const Patch& patch = g_Patches[patchIndex];

			if (patch.sourceBuildNumber == sourceBuildNumber)
			{
				std::vector<unsigned int> testPath;
				testPath.push_back(patchIndex);

				unsigned long long testPathFileSize = patch.fileLength;

				// Find the best path recursively
				if (GetUpdatePath(patch.targetBuildNumber, targetBuildNumber, testPath, testPathFileSize))
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

	bool CheckForUpdates(const std::string& updateURL, const unsigned long long& currentBuildNumber)
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

			unsigned long long buildNumber = _strtoui64(versionElement->GetText(), nullptr, 10);

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

		if (!GetUpdatePath(currentBuildNumber, g_LatestVersion, g_BestPatchPath, g_BestPathFileSize))
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
		for (unsigned long long patchIndex = 0; patchIndex < g_BestPatchPath.size(); ++patchIndex)
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

		fprintf(stdout, "Downloading File: %s to %s.\n", fileName.c_str(), targetPath.c_str());

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

	bool GetTargetCurrentVersion(const std::string& configFile, unsigned long long& version)
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

	bool SaveTargetNewVersion(const std::string& configFile, const unsigned long long& version)
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

		size_t elementsWritten = fwrite(&version, sizeof(unsigned long long), 1, targetFile);

		fclose(targetFile);

		if (elementsWritten != 1)
		{
			fprintf(stderr, "Error reading config file %s\n", configFile.c_str());
			return false;
		}

		return true;
	}

}