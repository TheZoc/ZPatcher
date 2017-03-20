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

class VisualCreatePatch : public wxApp
{
public:
	virtual bool OnInit();

#ifdef _WIN32
	// Make wxWebView use the latest installed Internet Explorer/Edge version on Windows
	void UseLatestIEVersion();
#endif

};


#endif // _VISUALCREATEPATCH_H_
