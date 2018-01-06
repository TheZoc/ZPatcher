//////////////////////////////////////////////////////////////////////////
//
// ZLauncher - Patcher system - Part of the ZUpdater suite
// Felipe "Zoc" Silveira - (c) 2016-2018
//
//////////////////////////////////////////////////////////////////////////
//
// ZLauncher.cpp
// Implement the entry point for the launcher
//
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <wx/stdpaths.h>
#include "ZLauncher.h"
#include "ZLauncherFrame.h"
#include "LogSystem.h"
#include "tinyxml2.h"
#include <wx/stdpaths.h>

#ifdef _WIN32
// Allows us to set the correct version of Internet Explorer for changelog display
#include <wx/msw/registry.h>
#endif

wxIMPLEMENT_APP(VisualCreatePatch);

bool VisualCreatePatch::OnInit()
{
	if (!wxApp::OnInit())
		return false;

	wxInitAllImageHandlers();

	// The configuration XML file is the same as the executable. On Windows, it replaces the .exe extension
	wxString ConfigPath = wxStandardPaths::Get().GetExecutablePath();

#ifdef _WIN32
	// Since only Windows actively uses extensions for the executable, remove it.
	ConfigPath = ConfigPath.BeforeLast('.');
#endif
	ConfigPath += ".xml";

	// Load the configuration file
	if (!ParseConfigFile(ConfigPath))
	{
		wxMessageBox(wxString::Format("Error Loading XML configuration file:\n\n%s\n\nMake sure it exists and has the appropriate format.", ConfigPath), "Missing file - Exiting...", wxOK | wxICON_ERROR);
		return false;
	}

#ifdef _WIN32
	//////////////////////////////////////////////////////////////////////////
	// Make wxWebView use the latest installed Internet Explorer/Edge version
	UseLatestIEVersion();
#endif

	//////////////////////////////////////////////////////////////////////////
	// Run the launcher!
	ZLauncherFrame* f = new ZLauncherFrame(nullptr);
	f->SetLaunchExecutableName(m_Config.LaunchExecutable);

#ifdef _WIN32
	f->SetIcon(wxICON(frame_icon));
#elif __APPLE__
	// TODO: Remove this hardcoded value
	f->SetIcon(wxIcon("ZLauncher.icns"));
//#else __linux__
//	// TODO: Find out how this works in Linux!
#endif

	f->Show(true);
	f->DoStartCreatePatchThread(m_Config.UpdateURL, m_Config.VersionFile, m_Config.TargetDirectory);

	return true;
}

bool VisualCreatePatch::ParseConfigFile(wxString ConfigFileName)
{
	ZPatcher::SetActiveLog("ZLauncher");
	tinyxml2::XMLDocument document;

	ZPatcher::Log(ZPatcher::LOG, "Opening %s config file.", ConfigFileName.ToStdString().c_str());

	document.LoadFile(ConfigFileName.c_str());
	ZPatcher::Log(ZPatcher::LOG, "Reading config file.");

	if (document.Error())
	{
		ZPatcher::Log(ZPatcher::LOG_FATAL, "An error occurred while attempting to open the XML file for reading.");
		return false;
	}

	tinyxml2::XMLHandle hDocument(&document);

	// Get the ZLauncher opening tag
	tinyxml2::XMLHandle hZLauncher = hDocument.FirstChildElement("ZLauncher");
	if (hZLauncher.ToElement() == NULL)
	{
		ZPatcher::Log(ZPatcher::LOG_FATAL, "An error occurred while attempting to parse the XML file: Missing or Invalid ZLauncher tag.");
		return false;
	}

	// Get the Config tag, child of ZLauncher
	tinyxml2::XMLHandle hConfig = hZLauncher.FirstChildElement("Config");
	if (hConfig.ToElement() == NULL)
	{
		ZPatcher::Log(ZPatcher::LOG_FATAL, "An error occurred while attempting to parse the XML file: Missing or Invalid Config tag.");
		return false;
	}
	
	// Get all configuration data here
	tinyxml2::XMLHandle hUpdateURL			= hConfig.FirstChildElement("UpdateURL");
	tinyxml2::XMLHandle hVersionFile		= hConfig.FirstChildElement("VersionFile");
	tinyxml2::XMLHandle hTargetDirectory	= hConfig.FirstChildElement("TargetDirectory");
	tinyxml2::XMLHandle hLaunchExecutable	= hConfig.FirstChildElement("LaunchExecutable");

	// Slightly slower, WAY easier to read and maintain
	bool bAllConfigsPresent = true;
	if (hUpdateURL.ToElement() == NULL)			{ ZPatcher::Log(ZPatcher::LOG_FATAL, "An error occurred while attempting to parse the XML file: Missing or Invalid UpdateURL tag.");		bAllConfigsPresent = false; }
	if (hVersionFile.ToElement() == NULL)		{ ZPatcher::Log(ZPatcher::LOG_FATAL, "An error occurred while attempting to parse the XML file: Missing or Invalid VersionFile tag.");		bAllConfigsPresent = false; }
	if (hTargetDirectory.ToElement() == NULL)	{ ZPatcher::Log(ZPatcher::LOG_FATAL, "An error occurred while attempting to parse the XML file: Missing or Invalid TargetDirectory tag.");	bAllConfigsPresent = false; }
	if (hLaunchExecutable.ToElement() == NULL)	{ ZPatcher::Log(ZPatcher::LOG_FATAL, "An error occurred while attempting to parse the XML file: Missing or Invalid LaunchExecutable tag.");	bAllConfigsPresent = false; }

	if (!bAllConfigsPresent)
		return false;

	// Fetch all the data and store it in the member struct variable 
	m_Config.UpdateURL			= hUpdateURL.ToElement()->GetText();
	m_Config.VersionFile		= hVersionFile.ToElement()->GetText();
	m_Config.TargetDirectory	= hTargetDirectory.ToElement()->GetText();
	m_Config.LaunchExecutable	= hLaunchExecutable.ToElement()->GetText();

	return true;
}

#ifdef _WIN32
void VisualCreatePatch::UseLatestIEVersion()
{
	enum E_InternetExplorer_RegKey
	{
		IE_7	= 7000,
		IE_8	= 8888,
		IE_9	= 9999,
		IE_10	= 10001,
		IE_EDGE	= 11001
	};

	// Get the application Executable Name
	wxString ExecPath = ::wxStandardPaths::Get().GetExecutablePath();
	wxString ExecName = ExecPath.AfterLast('\\');

	// Get the installed version of Internet Explorer
	wxRegKey IEVersionKey(wxRegKey::HKLM, wxT("SOFTWARE\\Microsoft\\Internet Explorer"));

	wxString IEFullVersion;
	if (IEVersionKey.HasValue(wxT("svcVersion")))
	{
		IEVersionKey.QueryRawValue(wxT("svcVersion"), IEFullVersion);
	}
	else if (IEVersionKey.HasValue(wxT("Version")))
	{
		IEVersionKey.QueryRawValue(wxT("Version"), IEFullVersion);
	}
	else
	{
		// TODO: Handle this appropriately!
		IEVersionKey = "7.00.6001.1800";
	}

	int IEVersion = wxAtoi(IEFullVersion.BeforeFirst('.'));

	wxLogNull nolog;   // suppress error messages
	wxRegKey fbe(wxRegKey::HKCU, wxT("Software\\Microsoft\\Internet Explorer\\Main\\FeatureControl\\FEATURE_BROWSER_EMULATION"));

	// List here: https://support.microsoft.com/en-us/help/969393/information-about-internet-explorer-versions
	long TargetValue;
	switch (IEVersion)
	{
	case 11:
		TargetValue = IE_EDGE;
		break;
	case 10:
		TargetValue = IE_10;
		break;
	case 9:
		TargetValue = IE_9;
		break;
	case 8:
		TargetValue = IE_8;
		break;
	case 7:
	default:
		TargetValue = IE_7;
		break;
	}

	if (TargetValue < IE_EDGE)
	{
		g_PatchHTMLHeaderFileName = PATCH_HEADER_COMPATIBILITY_HTML_FILE;
	}

	fbe.SetValue(ExecName, TargetValue);
}
#endif
