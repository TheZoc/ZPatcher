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
#include "ZLauncher.h"
#include "ZLauncherFrame.h"

wxIMPLEMENT_APP(VisualCreatePatch);

bool VisualCreatePatch::OnInit()
{
	if (!wxApp::OnInit())
		return false;

	wxInitAllImageHandlers();
	
	//////////////////////////////////////////////////////////////////////////
	// Simple config section
	wxString	updateURL				= wxT("http://www.example.org/example.xml");
	wxString	versionFile				= wxT("example.zversion");
	wxString	targetDirectory			= wxT("./");
	wxString	launcherExecutableName	= wxT("Example.exe");

	//////////////////////////////////////////////////////////////////////////
	// Run the launcher!
	ZLauncherFrame* f = new ZLauncherFrame(nullptr);
	f->SetLaunchExecutableName(launcherExecutableName);
	f->SetIcon(wxICON(frame_icon));
	f->Show(true);

	f->DoStartCreatePatchThread(updateURL, versionFile, targetDirectory);

	return true;
}

