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

#include "../libs/rapidxml-1.13/rapidxml.hpp"
#include "../libs/rapidxml-1.13/rapidxml_utils.hpp"


#ifdef _WIN32
// Allows us to set the correct version of Internet Explorer for changelog display
#include <wx/msw/registry.h>
#endif

wxIMPLEMENT_APP(ZLancher);

bool ZLancher::OnInit()
{
	if (!wxApp::OnInit())
		return false;

	wxInitAllImageHandlers();

	// Initialize Config colors now wx is set up
	m_Config.ApplicationBackground = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);
	m_Config.ProgressBarTextBackground = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);
	m_Config.ProgressBarTextForeground = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);

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
	ZLauncherFrame* f = new ZLauncherFrame(m_Config, nullptr);

#ifdef _WIN32
	f->SetIcon(wxICON(frame_icon));
#elif __APPLE__
	// TODO: Remove this hardcoded value
	f->SetIcon(wxIcon("ZLauncher.icns"));
//#else __linux__
//	// TODO: Find out how this works in Linux!
#endif

	f->Show(true);
	f->DoStartCreatePatchThread();

	return true;
}

bool ZLancher::ParseConfigFile(wxString ConfigFileName)
{
	ZPatcher::SetActiveLog("ZLauncher");

	ZPatcher::Log(ZPatcher::LOG, "Opening %s config file.", ConfigFileName.ToStdString().c_str());

	rapidxml::xml_document<> document;
	rapidxml::file<> xmlFile(ConfigFileName.c_str());
	try
	{
		document.parse<0>(xmlFile.data());
	}
	catch (const rapidxml::parse_error& e)
	{
		ZPatcher::Log(ZPatcher::LOG_FATAL, "An error occurred while attempting to open the XML config file for reading: %s", e.what());
		return false;
	}

	// Get the ZLauncher opening tag
	rapidxml::xml_node<>* zlauncherNode = document.first_node("ZLauncher");
	if (!zlauncherNode)
	{
		ZPatcher::Log(ZPatcher::LOG_FATAL, "An error occurred while attempting to parse the XML file: Missing or Invalid ZLauncher tag.");
		return false;
	}

	// Get the Config tag, child of ZLauncher
	rapidxml::xml_node<>* configNode = zlauncherNode->first_node("Config");
	if (!configNode)
	{
		ZPatcher::Log(ZPatcher::LOG_FATAL, "An error occurred while attempting to parse the XML file: Missing or Invalid Config tag.");
		return false;
	}
	
	// Get all configuration data here
	rapidxml::xml_node<>* updateURLNode			= configNode->first_node("UpdateURL");
	rapidxml::xml_node<>* versionFileNode		= configNode->first_node("VersionFile");
	rapidxml::xml_node<>* targetDirectoryNode	= configNode->first_node("TargetDirectory");
	rapidxml::xml_node<>* launchExecutableNode	= configNode->first_node("LaunchExecutable");

	// Slightly slower, WAY easier to read and maintain
	bool bCoreConfigsPresent = true;
	if (!updateURLNode)			{ ZPatcher::Log(ZPatcher::LOG_FATAL, "An error occurred while attempting to parse the XML file: Missing or Invalid UpdateURL tag.");		bCoreConfigsPresent = false; }
	if (!versionFileNode)		{ ZPatcher::Log(ZPatcher::LOG_FATAL, "An error occurred while attempting to parse the XML file: Missing or Invalid VersionFile tag.");		bCoreConfigsPresent = false; }
	if (!targetDirectoryNode)	{ ZPatcher::Log(ZPatcher::LOG_FATAL, "An error occurred while attempting to parse the XML file: Missing or Invalid TargetDirectory tag.");	bCoreConfigsPresent = false; }
	if (!launchExecutableNode)	{ ZPatcher::Log(ZPatcher::LOG_FATAL, "An error occurred while attempting to parse the XML file: Missing or Invalid LaunchExecutable tag.");	bCoreConfigsPresent = false; }

	if (!bCoreConfigsPresent)
		return false;

	// Fetch all the data and store it in the member struct variable 
	m_Config.UpdateURL			= updateURLNode->value();
	m_Config.VersionFile		= versionFileNode->value();
	m_Config.TargetDirectory	= targetDirectoryNode->value();
	m_Config.LaunchExecutable	= launchExecutableNode->value();

	// Optional config
	rapidxml::xml_node<>* BackgroundImageNode			= configNode->first_node("BackgroundImage");
	rapidxml::xml_node<>* appBackgroundNode				= configNode->first_node("AppBackground");
	rapidxml::xml_node<>* progressBarTextBackgroundNode	= configNode->first_node("ProgressBarTextBackground");
	rapidxml::xml_node<>* progressBarTextForegroundNode	= configNode->first_node("ProgressBarTextForeground");

	auto ExtractSingleColorFunc = [](const rapidxml::xml_node<>* colorNode) -> uint8_t
	{
		if (colorNode)
		{
			const int32_t color = strtol(colorNode->value(), nullptr, 10);
			if (errno == 0)
				return color;
		}
		return 0;
	};

	auto ExtractColorFunc = [&ExtractSingleColorFunc](const rapidxml::xml_node<>* targetNode) -> wxColour
	{
		if (targetNode)
		{
			const uint8_t r = ExtractSingleColorFunc(targetNode->first_node("r"));
			const uint8_t g = ExtractSingleColorFunc(targetNode->first_node("g"));
			const uint8_t b = ExtractSingleColorFunc(targetNode->first_node("b"));
			return wxColour(r, g, b);
		}
		return wxColour(0, 0, 0);
	};

	if (BackgroundImageNode)			m_Config.BackgroundImage			= BackgroundImageNode->value();
	if (appBackgroundNode)				m_Config.ApplicationBackground		= ExtractColorFunc(appBackgroundNode);
	if (progressBarTextBackgroundNode)	m_Config.ProgressBarTextBackground	= ExtractColorFunc(progressBarTextBackgroundNode);
	if (progressBarTextForegroundNode)	m_Config.ProgressBarTextForeground	= ExtractColorFunc(progressBarTextForegroundNode);

	return true;
}

#ifdef _WIN32
void ZLancher::UseLatestIEVersion()
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
