///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun 17 2015)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <wx/dcbuffer.h>
#include <sstream>
#include <iomanip>
#include "ZLauncherFrame.h"
#include "ZLauncherThread.h"
#include "DownloadFileWriter.h"

//////////////////////////////////////////////////////////////////////////

static const wxString g_ResourceDirectory = "./ZLauncherRes/";

#define DARK_COLORS

#ifdef DARK_COLORS

static const wxString g_BackgroundImage = (g_ResourceDirectory + "bg-dark.png");

#define APPLICATION_BACKGROUND	wxColour( 40, 40, 40 )
#define COMPONENT_BACKGROUND	wxColour( 30, 30, 30 )
#define COMPONENT_TEXT_COLOR	wxColour( 195, 195, 195 )

static const char* g_htmlHeader = "<html><head><style type=\"text/css\">body {color: #c3c3c3; background-color: #1E1E1E; scrollbar-base-color: #1E1E1E; scrollbar-face-color: #1E1E1E; scrollbar-3dlight-color: #858585; scrollbar-highlight-color: #858585; scrollbar-shadow-color: #858585; scrollbar-dark-shadow-color: #858585; scrollbar-track-color: #282828; scrollbar-arrow-color: #C3C3C3;}</style></head><body>";

#else

static const wxString g_BackgroundImage = wxEmptyString;

#define APPLICATION_BACKGROUND	wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW )
#define COMPONENT_BACKGROUND	wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW )
#define COMPONENT_TEXT_COLOR	wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOWTEXT )

static const char* g_htmlHeader = "<html><head><style type=\"text/css\">body {color: #000000; background-color: #FFFFFF; }</style></head><body>";

#endif


///////////////////////////////////////////////////////////////////////////

ZLauncherFrame::ZLauncherFrame( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	wxImage::SetDefaultLoadFlags(wxImage::GetDefaultLoadFlags() & ~wxImage::Load_Verbose);

	this->SetSizeHints( wxSize( 500,300 ), wxDefaultSize );
	this->SetBackgroundColour( APPLICATION_BACKGROUND );
	
	// GridBagSizer for the whole frame
	wxGridBagSizer* gridBagSizerFrame;
	gridBagSizerFrame = new wxGridBagSizer( 0, 0 );
	gridBagSizerFrame->SetFlexibleDirection( wxBOTH );
	gridBagSizerFrame->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	gridBagSizerFrame->SetMinSize( wxSize( 800,600 ) ); 

	// GridBagSizer For the "Body"  (HTML Window, etc.)
	wxGridBagSizer* gridBagSizerBody;
	gridBagSizerBody = new wxGridBagSizer( 0, 0 );
	gridBagSizerBody->SetFlexibleDirection( wxBOTH );
	gridBagSizerBody->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	// HTML Viewer
	m_htmlWin = wxWebView::New(this, wxID_ANY, wxWebViewDefaultURLStr, wxDefaultPosition, wxDefaultSize, wxWebViewBackendDefault, wxSIMPLE_BORDER);
	m_htmlWin->SetMinSize( wxSize( 400,300 ) );
	gridBagSizerBody->Add( m_htmlWin, wxGBPosition( 0, 0 ), wxGBSpan( 1, 1 ), wxALL|wxEXPAND, 5 );

	// GridBagSizer for the right side of the window
	wxGridBagSizer* gridBagSizerRight;
	gridBagSizerRight = new wxGridBagSizer( 0, 0 );
	gridBagSizerRight->SetFlexibleDirection( wxBOTH );
	gridBagSizerRight->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	gridBagSizerRight->Add( 0, 0, wxGBPosition( 1, 0 ), wxGBSpan( 1, 1 ), wxEXPAND, 5 );

	// Close Button (Top Right)
	m_CloseButtonImg_Normal.LoadFile(g_ResourceDirectory + "CloseButton_Normal.png", wxBITMAP_TYPE_PNG);
	m_CloseButtonImg_Disabled.LoadFile(g_ResourceDirectory + "CloseButton_Disabled.png", wxBITMAP_TYPE_PNG);
	m_CloseButtonImg_Pressed.LoadFile(g_ResourceDirectory + "CloseButton_Pressed.png", wxBITMAP_TYPE_PNG);
	m_CloseButtonImg_Focus.LoadFile(g_ResourceDirectory + "CloseButton_Focus.png", wxBITMAP_TYPE_PNG);
	m_CloseButtonImg_Hover.LoadFile(g_ResourceDirectory + "CloseButton_Hover.png", wxBITMAP_TYPE_PNG);
	m_btnClose = new wxButton(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE | wxBU_EXACTFIT | wxBU_NOTEXT);
	m_btnClose->SetBitmap(m_CloseButtonImg_Normal);
	m_btnClose->SetBitmapDisabled(m_CloseButtonImg_Disabled);
	m_btnClose->SetBitmapPressed(m_CloseButtonImg_Pressed);
	m_btnClose->SetBitmapFocus(m_CloseButtonImg_Focus);
	m_btnClose->SetBitmapCurrent(m_CloseButtonImg_Hover);
	m_btnClose->SetBackgroundColour(APPLICATION_BACKGROUND);
	m_btnClose->Enable(true);

	gridBagSizerRight->Add( m_btnClose, wxGBPosition( 0, 1 ), wxGBSpan( 1, 1 ), wxALL, 5 );
	gridBagSizerRight->AddGrowableCol( 0 );
	gridBagSizerRight->AddGrowableRow( 1 );

	gridBagSizerBody->Add( gridBagSizerRight, wxGBPosition( 0, 1 ), wxGBSpan( 1, 1 ), wxALIGN_RIGHT|wxEXPAND, 5 );
	gridBagSizerBody->AddGrowableCol( 1 );
	gridBagSizerBody->AddGrowableRow( 0 );
	
	gridBagSizerFrame->Add( gridBagSizerBody, wxGBPosition( 0, 0 ), wxGBSpan( 1, 2 ), wxEXPAND, 5 );
	
	// Footer Area (Progress Bar and Launch button)
	wxGridBagSizer* gridBagSizerFooter;
	gridBagSizerFooter = new wxGridBagSizer( 0, 0 );
	gridBagSizerFooter->SetFlexibleDirection( wxBOTH );
	gridBagSizerFooter->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	// Text for the Progress Bar
	m_txtProgress = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY|wxNO_BORDER );
	m_txtProgress->SetForegroundColour( COMPONENT_TEXT_COLOR );
	m_txtProgress->SetBackgroundColour( APPLICATION_BACKGROUND );
	
	gridBagSizerFooter->Add( m_txtProgress, wxGBPosition( 0, 0 ), wxGBSpan( 1, 1 ), wxALIGN_BOTTOM|wxEXPAND|wxLEFT|wxRIGHT|wxTOP, 5 );

	// Progress Bar
	m_progress = new wxGauge( this, wxID_ANY, 100, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL );
	m_progress->SetValue( 0 );
	m_progress->SetMinSize( wxSize( 500,25 ) );
	
	gridBagSizerFooter->Add( m_progress, wxGBPosition( 1, 0 ), wxGBSpan( 1, 1 ), wxALIGN_TOP|wxALL|wxBOTTOM|wxEXPAND|wxLEFT|wxRIGHT, 5 );
	gridBagSizerFooter->AddGrowableCol( 0 );
	gridBagSizerFooter->AddGrowableRow( 0 );
	
	gridBagSizerFrame->Add( gridBagSizerFooter, wxGBPosition( 1, 0 ), wxGBSpan( 1, 1 ), wxEXPAND, 5 );
	
	// Launch Button
	m_LaunchButtonImg_Normal.LoadFile(g_ResourceDirectory + "LaunchButton_Normal.png", wxBITMAP_TYPE_PNG);
	m_LaunchButtonImg_Disabled.LoadFile(g_ResourceDirectory + "LaunchButton_Disabled.png", wxBITMAP_TYPE_PNG);
	m_LaunchButtonImg_Pressed.LoadFile(g_ResourceDirectory + "LaunchButton_Pressed.png", wxBITMAP_TYPE_PNG);
	m_LaunchButtonImg_Focus.LoadFile(g_ResourceDirectory + "LaunchButton_Focus.png", wxBITMAP_TYPE_PNG);
	m_LaunchButtonImg_Hover.LoadFile(g_ResourceDirectory + "LaunchButton_Hover.png", wxBITMAP_TYPE_PNG);
	m_btnLaunch = new wxButton( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE | wxBU_EXACTFIT | wxBU_NOTEXT);
	m_btnLaunch->SetBitmap(m_LaunchButtonImg_Normal);
	m_btnLaunch->SetBitmapDisabled(m_LaunchButtonImg_Disabled);
	m_btnLaunch->SetBitmapPressed(m_LaunchButtonImg_Pressed);
	m_btnLaunch->SetBitmapFocus(m_LaunchButtonImg_Focus);
	m_btnLaunch->SetBitmapCurrent(m_LaunchButtonImg_Hover);
	m_btnLaunch->SetBackgroundColour(APPLICATION_BACKGROUND);
	m_btnLaunch->Enable( false );

	gridBagSizerFrame->Add( m_btnLaunch, wxGBPosition( 1, 1 ), wxGBSpan( 1, 1 ), wxALIGN_BOTTOM|wxALL, 5 );
	gridBagSizerFrame->AddGrowableCol( 0 );
	gridBagSizerFrame->AddGrowableRow( 0 );
	
	this->SetSizer( gridBagSizerFrame );
	this->Layout();

	this->Centre( wxBOTH );

	// Set brakcground image, if any.
	SetBackgroundStyle(wxBG_STYLE_PAINT);
	if (g_BackgroundImage != wxEmptyString)
		m_backgroundImg.LoadFile(g_BackgroundImage, wxBITMAP_TYPE_PNG);


	// Bind Message Events
	Bind(wxEVT_PAINT, &ZLauncherFrame::PaintEvent, this);

	Bind(wxEVT_COMMAND_UPDATE_PROGRESS_BAR, &ZLauncherFrame::OnProgressBarUpdate, this);
	Bind(wxEVT_COMMAND_UPDATE_PROGRESS_TEXT, &ZLauncherFrame::OnProgressTextUpdate, this);

	Bind(wxEVT_COMMAND_HTML_SET_CONTENT, &ZLauncherFrame::OnHTMLSetContent, this);
	Bind(wxEVT_COMMAND_HTML_LOAD_PAGE, &ZLauncherFrame::OnHTMLLoadPage, this);

	Bind(wxEVT_COMMAND_ENABLE_LAUNCH_BUTTON, &ZLauncherFrame::OnEnableLaunchButton, this);

	Bind(wxEVT_CLOSE_WINDOW, &ZLauncherFrame::OnClose, this);

	// Bind Button Events
	m_btnClose->Bind(wxEVT_BUTTON, &ZLauncherFrame::OnCloseButtonClicked, this);
	m_btnLaunch->Bind(wxEVT_BUTTON, &ZLauncherFrame::OnLaunchButtonClicked, this);

}

ZLauncherFrame::~ZLauncherFrame()
{
}

void ZLauncherFrame::SetLaunchExecutableName(wxString exe)
{
	m_LaunchExecutableName = exe;
}

void ZLauncherFrame::OnCloseButtonClicked(wxCommandEvent& WXUNUSED(evt))
{
	wxQueueEvent(wxTheApp->GetTopWindow()->GetEventHandler(), new wxCloseEvent(wxEVT_CLOSE_WINDOW));
}

void ZLauncherFrame::OnLaunchButtonClicked(wxCommandEvent& WXUNUSED(evt))
{
	// System specific
#ifdef _WIN32
	ShellExecuteA(NULL, "open", m_LaunchExecutableName.mbc_str(), NULL, NULL, SW_SHOW);
#endif

	// Exit after launching the game/application
	wxQueueEvent(wxTheApp->GetTopWindow()->GetEventHandler(), new wxCloseEvent(wxEVT_CLOSE_WINDOW));
}

void ZLauncherFrame::PaintEvent(wxPaintEvent & evt)
{
	wxAutoBufferedPaintDC dc(this);
	RenderFrame(dc);
}

void ZLauncherFrame::RenderFrame(wxDC& dc)
{
	if (m_backgroundImg.IsOk())
	{
		dc.DrawBitmap(m_backgroundImg, 0, 0);
	}
	else
	{
		dc.SetBackground( APPLICATION_BACKGROUND );
		dc.Clear();
	}
}

void ZLauncherFrame::DoStartCreatePatchThread(const wxString& updateURL, const wxString& versionFile, const wxString& targetDirectory)
{
	m_pThread = new ZLauncherThread(this, updateURL, versionFile, targetDirectory);

	if (m_pThread->Run() != wxTHREAD_NO_ERROR)
	{
		// TODO: Handle this appropriately!
		wxLogError("Can't create the thread!");
		delete m_pThread;
		m_pThread = NULL;
	}
}

void ZLauncherFrame::OnProgressBarUpdate(wxThreadEvent& evt)
{
	m_progress->SetValue(evt.GetInt());
}

void ZLauncherFrame::OnProgressTextUpdate(wxThreadEvent& evt)
{
	m_txtProgress->SetValue(evt.GetString());
}

void ZLauncherFrame::OnHTMLSetContent(wxThreadEvent& evt)
{
	m_htmlWin->SetPage(evt.GetString(), "");
}

void ZLauncherFrame::OnHTMLLoadPage(wxThreadEvent& evt)
{
	m_htmlWin->LoadURL(evt.GetString());
}

void ZLauncherFrame::OnEnableLaunchButton(wxThreadEvent& evt)
{
	m_btnLaunch->Enable(evt.GetInt() != 0);
}

void ZLauncherFrame::OnClose(wxCloseEvent& evt)
{

	if (evt.CanVeto() &&			// Check if this is not a "force close" event.
		!m_pThreadCS.TryEnter())	// Check if the thread is in the critical section.
	{
		wxMessageBox("The patcher is currently **applying** a patch. Please, wait until it completes.\n(It is safe to close during a download).", "Currently Applying Patch", wxOK | wxICON_WARNING, nullptr);
		evt.Veto();
		return;
	}

	// If we entered the critical section in the above check, leave it now.
	m_pThreadCS.Leave();

	{
		wxCriticalSectionLocker enter(m_pThreadCS);
		if (m_pThread)         // does the thread still exist?
		{
			wxMessageOutputDebug().Printf("MYFRAME: deleting thread");
			if (m_pThread->Delete() != wxTHREAD_NO_ERROR)
				wxLogError("Can't delete the thread!");
		}
	}       // exit from the critical section to give the thread
			// the possibility to enter its destructor
			// (which is guarded with m_pThreadCS critical section!)

	while (true)
	{
		{ // was the ~MyThread() function executed?
			wxCriticalSectionLocker enter(m_pThreadCS);
			if (!m_pThread) break;
		}
		// wait for thread completion
		wxThread::This()->Sleep(1);
	}
	Destroy();
}

void ZLauncherFrame::UpdateProgress(const float& Percentage, const wxString& txt)
{
	wxThreadEvent* updateProgressBarEvent = new wxThreadEvent(wxEVT_COMMAND_UPDATE_PROGRESS_BAR);
	updateProgressBarEvent->SetInt(0);
	wxQueueEvent(wxTheApp->GetTopWindow()->GetEventHandler(), updateProgressBarEvent);

	wxThreadEvent* updateProgressTextEvent = new wxThreadEvent(wxEVT_COMMAND_UPDATE_PROGRESS_TEXT);
	updateProgressTextEvent->SetString(txt);
	wxQueueEvent(wxTheApp->GetTopWindow()->GetEventHandler(), updateProgressTextEvent);
}

int ZLauncherFrame::TransferInfo(void *p, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow)
{
	DownloadFileWriter* filewriter = (DownloadFileWriter*)p;
	CURL *curl = filewriter->GetCurlHandle();
	double curtime = 0;
	double speed = 0;

	curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &curtime);
	curl_easy_getinfo(curl, CURLINFO_SPEED_DOWNLOAD, &speed);

	int percentage = (dltotal == 0) ? 0 : (int)(((float)dlnow / (float)dltotal) * 100);
	
	std::ostringstream oss_speed;
	oss_speed << std::fixed << std::setprecision(2);
	if (speed > 1048576)
	{
		speed /= 1048576;
		oss_speed <<  speed << " MiB/s";
	}
	else if (speed > 1024)
	{
		speed /= 1024;
		oss_speed << speed << " KiB/s";
	}
	else
	{
		oss_speed << speed << " B/s";
	}

	wxThreadEvent* updateProgressBarEvent = new wxThreadEvent(wxEVT_COMMAND_UPDATE_PROGRESS_BAR);
	updateProgressBarEvent->SetInt(percentage);
	wxQueueEvent(wxTheApp->GetTopWindow()->GetEventHandler(), updateProgressBarEvent);

   	if ((curtime - filewriter->GetLastUpdateTime()) >= 0.1f)
   	{
		filewriter->SetLastUpdateTime(curtime);
		wxThreadEvent* updateProgressTextEvent = new wxThreadEvent(wxEVT_COMMAND_UPDATE_PROGRESS_TEXT);
		updateProgressTextEvent->SetString(wxString::Format("Downloading %s (%s)", filewriter->GetDestFileName(), oss_speed.str().c_str()));
		wxQueueEvent(wxTheApp->GetTopWindow()->GetEventHandler(), updateProgressTextEvent);
	}

	// Check if we should stop the thread and cancel it here!
	if (filewriter->GetParentThread() && filewriter->GetParentThread()->TestDestroy())
	{
		return 1;
	}

	return 0;
}

void ZLauncherFrame::ApplyPatchProgress(const float& Percentage)
{
	wxThreadEvent* updateProgressTextEvent = new wxThreadEvent(wxEVT_COMMAND_UPDATE_PROGRESS_TEXT);
	updateProgressTextEvent->SetString(wxT("Applying Patch, Please Wait..."));
	wxQueueEvent(wxTheApp->GetTopWindow()->GetEventHandler(), updateProgressTextEvent);

	wxThreadEvent* updateProgressBarEvent = new wxThreadEvent(wxEVT_COMMAND_UPDATE_PROGRESS_BAR);
	updateProgressBarEvent->SetInt(Percentage);
	wxQueueEvent(wxTheApp->GetTopWindow()->GetEventHandler(), updateProgressBarEvent);
}

void ZLauncherFrame::HTMLSetContent(wxString html)
{
	wxThreadEvent* HTMLSetContentEvent = new wxThreadEvent(wxEVT_COMMAND_HTML_SET_CONTENT);

	wxString htmlContent;
	htmlContent = g_htmlHeader;
	htmlContent += html;

	HTMLSetContentEvent->SetString(htmlContent);
	wxQueueEvent(wxTheApp->GetTopWindow()->GetEventHandler(), HTMLSetContentEvent);
}

void ZLauncherFrame::HTMLLoadPage(wxString url)
{
	wxThreadEvent* HTMLSetContentEvent = new wxThreadEvent(wxEVT_COMMAND_HTML_LOAD_PAGE);
	HTMLSetContentEvent->SetString(url);
	wxQueueEvent(wxTheApp->GetTopWindow()->GetEventHandler(), HTMLSetContentEvent);
}

void ZLauncherFrame::EnableLaunchButton(bool enable)
{
	wxThreadEvent* ButtonLaunchEnableEvent = new wxThreadEvent(wxEVT_COMMAND_ENABLE_LAUNCH_BUTTON);
	ButtonLaunchEnableEvent->SetInt(static_cast<int>(enable));
	wxQueueEvent(wxTheApp->GetTopWindow()->GetEventHandler(), ButtonLaunchEnableEvent);
}

