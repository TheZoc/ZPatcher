//////////////////////////////////////////////////////////////////////////
//
// ZLauncher - Patcher system - Part of the ZUpdater suite
// Felipe "Zoc" Silveira - (c) 2016
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
	
	//////////////////////////////////////////////////////////////////////////
	// Simple config section
	wxString	updateURL				= wxT("https://raw.githubusercontent.com/TheZoc/ZPatcher/master/tests/zpatcher_test.xml");
	wxString	versionFile				= wxT("zpatcher_test.zversion");
	wxString	targetDirectory			= wxT("./");
	wxString	launcherExecutableName	= wxT("Example.exe");

#ifdef _WIN32
	//////////////////////////////////////////////////////////////////////////
	// Make wxWebView use the latest installed Internet Explorer/Edge version
	UseLatestIEVersion();
#endif

	//////////////////////////////////////////////////////////////////////////
	// Run the launcher!
	ZLauncherFrame* f = new ZLauncherFrame(nullptr);
	f->SetLaunchExecutableName(launcherExecutableName);

#ifdef _WIN32
	f->SetIcon(wxICON(frame_icon));
#elif __APPLE__
	// TODO: Remove this hardcoded value
	f->SetIcon(wxIcon("ZLauncher.icns"));
//#else __linux__
//	// TODO: Find out how this works in Linux!
#endif

	f->Show(true);
	f->DoStartCreatePatchThread(updateURL, versionFile, targetDirectory);

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
