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

///////////////////////////////////////////////////////////////////////////

ZLauncherFrame::ZLauncherFrame( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxSize( 500,300 ), wxDefaultSize );
	this->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW ) );
	
	wxGridBagSizer* gridBagSizerFrame;
	gridBagSizerFrame = new wxGridBagSizer( 0, 0 );
	gridBagSizerFrame->SetFlexibleDirection( wxBOTH );
	gridBagSizerFrame->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	gridBagSizerFrame->SetMinSize( wxSize( 800,600 ) ); 
	wxGridBagSizer* gridBagSizerBody;
	gridBagSizerBody = new wxGridBagSizer( 0, 0 );
	gridBagSizerBody->SetFlexibleDirection( wxBOTH );
	gridBagSizerBody->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_htmlWin = new wxHtmlWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHW_SCROLLBAR_AUTO|wxSIMPLE_BORDER );
	m_htmlWin->SetMinSize( wxSize( 400,300 ) );

	gridBagSizerBody->Add( m_htmlWin, wxGBPosition( 0, 0 ), wxGBSpan( 1, 1 ), wxALL|wxEXPAND, 5 );

	gridBagSizerBody->AddGrowableRow( 0 );
	
	gridBagSizerFrame->Add( gridBagSizerBody, wxGBPosition( 0, 0 ), wxGBSpan( 1, 2 ), wxEXPAND, 5 );
	
	wxGridBagSizer* drigBagSizerFooter;
	drigBagSizerFooter = new wxGridBagSizer( 0, 0 );
	drigBagSizerFooter->SetFlexibleDirection( wxBOTH );
	drigBagSizerFooter->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_txtProgress = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY|wxNO_BORDER );
	m_txtProgress->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW ) );
	
	drigBagSizerFooter->Add( m_txtProgress, wxGBPosition( 0, 0 ), wxGBSpan( 1, 1 ), wxALIGN_BOTTOM|wxEXPAND|wxLEFT|wxRIGHT|wxTOP, 5 );
	
	m_progress = new wxGauge( this, wxID_ANY, 100, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL );
	m_progress->SetValue( 0 ); 
	m_progress->SetMinSize( wxSize( 500,25 ) );
	
	drigBagSizerFooter->Add( m_progress, wxGBPosition( 1, 0 ), wxGBSpan( 1, 1 ), wxALIGN_TOP|wxALL|wxBOTTOM|wxEXPAND|wxLEFT|wxRIGHT, 5 );
	
	
	drigBagSizerFooter->AddGrowableCol( 0 );
	drigBagSizerFooter->AddGrowableRow( 0 );
	
	gridBagSizerFrame->Add( drigBagSizerFooter, wxGBPosition( 1, 0 ), wxGBSpan( 1, 1 ), wxEXPAND, 5 );
	
	m_btnLaunch = new wxButton( this, wxID_ANY, wxT("Launch"), wxDefaultPosition, wxDefaultSize, 0 );
	m_btnLaunch->Enable( false );
	
	gridBagSizerFrame->Add( m_btnLaunch, wxGBPosition( 1, 1 ), wxGBSpan( 1, 1 ), wxALIGN_BOTTOM|wxALL, 5 );
	
	gridBagSizerFrame->AddGrowableCol( 0 );
	gridBagSizerFrame->AddGrowableRow( 0 );
	
	this->SetSizer( gridBagSizerFrame );
	this->Layout();
	this->Centre( wxBOTH );

	m_backgroundImg.LoadFile("bg.tga", wxBITMAP_TYPE_TGA);
	SetBackgroundStyle(wxBG_STYLE_PAINT);

	Bind(wxEVT_PAINT, &ZLauncherFrame::PaintEvent, this);

	Bind(wxEVT_COMMAND_UPDATE_PROGRESS_BAR, &ZLauncherFrame::OnProgressBarUpdate, this);
	Bind(wxEVT_COMMAND_UPDATE_PROGRESS_TEXT, &ZLauncherFrame::OnProgressTextUpdate, this);

	Bind(wxEVT_COMMAND_HTML_SET_CONTENT, &ZLauncherFrame::OnHTMLSetContent, this);
	Bind(wxEVT_COMMAND_HTML_LOAD_PAGE, &ZLauncherFrame::OnHTMLSetContent, this);

	Bind(wxEVT_COMMAND_ENABLE_LAUNCH_BUTTON, &ZLauncherFrame::OnEnableLaunchButton, this);

	Bind(wxEVT_CLOSE_WINDOW, &ZLauncherFrame::OnClose, this);
}

ZLauncherFrame::~ZLauncherFrame()
{
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
		dc.SetBackground(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
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
	m_htmlWin->SetPage(evt.GetString());
}

void ZLauncherFrame::OnHTMLLoadPage(wxThreadEvent& evt)
{
	m_htmlWin->LoadPage(evt.GetString());
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
	HTMLSetContentEvent->SetString(html);
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

