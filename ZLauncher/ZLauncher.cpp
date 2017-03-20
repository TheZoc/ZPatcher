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

	//////////////////////////////////////////////////////////////////////////
	// Run the launcher!
	ZLauncherFrame* f = new ZLauncherFrame(nullptr);
	f->SetLaunchExecutableName(launcherExecutableName);

#ifdef _WIN32
	f->SetIcon(wxICON(frame_icon));
#elif __APPLE__
	// TODO: Remove this hardcode
	f->SetIcon(wxIcon("ZLauncher.icns"));
//#else __linux__
//	// TODO: Find out how this works in Linux!
#endif

#ifdef _WIN32
	//////////////////////////////////////////////////////////////////////////
	// Make wxWebView use the latest installed Internet Explorer/Edge version
	//////////////////////////////////////////////////////////////////////////

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
	wxRegKey rk(wxRegKey::HKCU, wxT("Software\\Microsoft\\Internet Explorer\\Main\\FeatureControl\\FEATURE_BROWSER_EMULATION"));

	// List here: https://support.microsoft.com/en-us/help/969393/information-about-internet-explorer-versions
	switch (IEVersion)
	{
	case 11:
		rk.SetValue(ExecName, 11001);
		break;
	case 10:
		rk.SetValue(ExecName, 10001);
		break;
	case 9:
		rk.SetValue(ExecName, 9999);
		break;
	case 8:
		rk.SetValue(ExecName, 8888);
		break;
	case 7:
	default:
		rk.SetValue(ExecName, 7000);
		break;
	}
#endif

	f->Show(true);
	f->DoStartCreatePatchThread(updateURL, versionFile, targetDirectory);

	return true;
}

