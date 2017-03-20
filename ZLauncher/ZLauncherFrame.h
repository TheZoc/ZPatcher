///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun 17 2015)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __ZLAUNCHERFRAME_H__
#define __ZLAUNCHERFRAME_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/webview.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/gbsizer.h>
#include <wx/textctrl.h>
#include <wx/gauge.h>
#include <wx/button.h>
#include <wx/frame.h>
#include "curl/curl.h"

class ZLauncherThread;

//////////////////////////////////////////////////////////////////////////
// Theme Files

#define RESOURCES_DIRECTORY						"./ZLauncherRes/"					// This is the directory that will hold all the assets used by the patcher
#define BACKGROUND_IMAGE						"bg-dark.png"
#define PATCH_HEADER_HTML_FILE					"PatchNotesHeader.html"
#define PATCH_HEADER_COMPATIBILITY_HTML_FILE	"PatchNotesHeaderCompat.html"		// Used in Windows only, for systems that doesn't have Microsoft Edge installed

#define CLOSE_BUTTON_NORMAL						"CloseButton_Normal.png"
#define CLOSE_BUTTON_DISABLED					"CloseButton_Disabled.png"
#define CLOSE_BUTTON_PRESSED					"CloseButton_Pressed.png"
#define CLOSE_BUTTON_FOCUS						"CloseButton_Focus.png"
#define CLOSE_BUTTON_HOVER						"CloseButton_Hover.png"

#define LAUNCH_BUTTON_NORMAL					"LaunchButton_Normal.png"
#define LAUNCH_BUTTON_DISABLED					"LaunchButton_Disabled.png"
#define LAUNCH_BUTTON_PRESSED					"LaunchButton_Pressed.png"
#define LAUNCH_BUTTON_FOCUS						"LaunchButton_Focus.png"
#define LAUNCH_BUTTON_HOVER						"LaunchButton_Hover.png"

//////////////////////////////////////////////////////////////////////////
// Header related files
extern wxString g_PatchHTMLHeaderFileName;

///////////////////////////////////////////////////////////////////////////////
/// Class ZLauncherFrame
///////////////////////////////////////////////////////////////////////////////
class ZLauncherFrame : public wxFrame
{
	private:
	
	protected:
		wxBitmap m_backgroundImg;
		wxWebView* m_htmlWin;
		wxButton* m_btnClose;
		wxTextCtrl* m_txtProgress;
		wxGauge* m_progress;
		wxButton* m_btnLaunch;
	
		wxBitmap m_LaunchButtonImg_Normal;
		wxBitmap m_LaunchButtonImg_Disabled;
		wxBitmap m_LaunchButtonImg_Pressed;
		wxBitmap m_LaunchButtonImg_Focus;
		wxBitmap m_LaunchButtonImg_Hover;

		wxBitmap m_CloseButtonImg_Normal;
		wxBitmap m_CloseButtonImg_Disabled;
		wxBitmap m_CloseButtonImg_Pressed;
		wxBitmap m_CloseButtonImg_Focus;
		wxBitmap m_CloseButtonImg_Hover;

	public:
		ZLauncherFrame( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("ZLauncher : ZPatcher v2.0 beta"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 800,600 ), long style = 0L /* wxCAPTION|wxCLOSE_BOX|wxSYSTEM_MENU */ );
		~ZLauncherFrame();
	
protected:
	wxString m_LaunchExecutableName;

public:
	//////////////////////////////////////////////////////////////////////////
	// This is the directory that will hold all the assets related to the patcher
	static const wxString GetResourcesDirectory() { return wxString(RESOURCES_DIRECTORY); }

	//////////////////////////////////////////////////////////////////////////
	// Set Launch Executable Name
	void SetLaunchExecutableName(wxString exe);

	//////////////////////////////////////////////////////////////////////////
	// Close button click
	void OnCloseButtonClicked(wxCommandEvent& WXUNUSED(evt));

	//////////////////////////////////////////////////////////////////////////
	// Close button click
	void OnLaunchButtonClicked(wxCommandEvent& WXUNUSED(evt));


	//////////////////////////////////////////////////////////////////////////
	// Background image
	void PaintEvent(wxPaintEvent & evt);
	void RenderFrame(wxDC& dc);

	//////////////////////////////////////////////////////////////////////////
	// Thread communication
	void DoStartCreatePatchThread(const wxString& updateURL, const wxString& versionFile, const wxString& targetDirectory);

	void OnProgressBarUpdate(wxThreadEvent& evt);
	void OnProgressTextUpdate(wxThreadEvent& evt);

	void OnHTMLSetContent(wxThreadEvent& evt);
	void OnHTMLLoadPage(wxThreadEvent& evt);

	void OnEnableLaunchButton(wxThreadEvent& evt);

	void OnClose(wxCloseEvent& evt);

	
	static void UpdateProgress(const float& Percentage, const wxString& txt);
	static int	TransferInfo(void *p, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow);
	static void ApplyPatchProgress(const float& Percentage);
	static void HTMLSetContent(wxString html);
	static void HTMLLoadPage(wxString url);
	static void EnableLaunchButton(bool enable);

protected:
	friend class			ZLauncherThread;			// Allow it to access our m_pThread
	ZLauncherThread*		m_pThread;					// Our thread pointer
	wxCriticalSection		m_pThreadCS;				// protects the m_pThread pointer

};

#endif //__ZLAUNCHERFRAME_H__
