//////////////////////////////////////////////////////////////////////////
//
// ZLauncher - Patcher system - Part of the ZUpdater suite
// Felipe "Zoc" Silveira - (c) 2016
//
//////////////////////////////////////////////////////////////////////////
//
// ZLauncher.h
// Header for the entry point for the launcher
//
//////////////////////////////////////////////////////////////////////////

#ifndef _VISUALCREATEPATCH_H_
#define _VISUALCREATEPATCH_H_

#include <wx/wxprec.h>
#include <wx/cmdline.h>

struct ZLauncherConfig
{
	std::string UpdateURL;
	std::string VersionFile;
	std::string TargetDirectory;
	std::string LaunchExecutable;
};

class VisualCreatePatch : public wxApp
{
protected:
	ZLauncherConfig m_Config;

public:
	virtual bool OnInit();

	virtual bool ParseConfigFile(wxString ConfigFileName);

#ifdef _WIN32
	// Make wxWebView use the latest installed Internet Explorer/Edge version on Windows
	void UseLatestIEVersion();
#endif

};


#endif // _VISUALCREATEPATCH_H_
