//////////////////////////////////////////////////////////////////////////
//
// VisualCreatePatch - Patcher system - Part of the ZUpdater suite
// Felipe "Zoc" Silveira - (c) 2016-2018
//
//////////////////////////////////////////////////////////////////////////
//
// CreatePatchFrame.cpp
// Implement frame with the create patch progress and related events
//
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CreatePatchFrame.h"
#include "CreatePatchThread.h"
#include "CreatePatch.h"
#include "LzmaInterfaces.h"

///////////////////////////////////////////////////////////////////////////

CreatePatchFrame::CreatePatchFrame( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	this->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW ) );
	
	wxGridBagSizer* gridBagSizer;
	gridBagSizer = new wxGridBagSizer( 0, 0 );
	gridBagSizer->SetFlexibleDirection( wxBOTH );
	gridBagSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	gridBagSizer->SetMinSize( wxSize( 500,-1 ) ); 
	m_txtOldLabel = new wxStaticText( this, wxID_ANY, wxT("Old version directory:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_txtOldLabel->Wrap( -1 );
	gridBagSizer->Add( m_txtOldLabel, wxGBPosition( 0, 0 ), wxGBSpan( 1, 1 ), wxALL, 5 );
	
	m_txtOldDirectory = new wxStaticText( this, wxID_ANY, wxT("-undefined-"), wxDefaultPosition, wxDefaultSize, 0 );
	m_txtOldDirectory->Wrap( -1 );
	gridBagSizer->Add( m_txtOldDirectory, wxGBPosition( 0, 1 ), wxGBSpan( 1, 1 ), wxALL, 5 );
	
	m_txtNewLabel = new wxStaticText( this, wxID_ANY, wxT("New version directory:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_txtNewLabel->Wrap( -1 );
	gridBagSizer->Add( m_txtNewLabel, wxGBPosition( 1, 0 ), wxGBSpan( 1, 1 ), wxALL, 5 );
	
	m_txtNewDirectory = new wxStaticText( this, wxID_ANY, wxT("-undefined-"), wxDefaultPosition, wxDefaultSize, 0 );
	m_txtNewDirectory->Wrap( -1 );
	gridBagSizer->Add( m_txtNewDirectory, wxGBPosition( 1, 1 ), wxGBSpan( 1, 1 ), wxALL, 5 );
	
	m_txtPatchLabel = new wxStaticText( this, wxID_ANY, wxT("Output Patch File:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_txtPatchLabel->Wrap( -1 );
	gridBagSizer->Add( m_txtPatchLabel, wxGBPosition( 2, 0 ), wxGBSpan( 1, 1 ), wxALL, 5 );
	
	m_txtPatchFile = new wxStaticText( this, wxID_ANY, wxT("-undefined-"), wxDefaultPosition, wxDefaultSize, 0 );
	m_txtPatchFile->Wrap( -1 );
	gridBagSizer->Add( m_txtPatchFile, wxGBPosition( 2, 1 ), wxGBSpan( 1, 1 ), wxALL, 5 );
	
	m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	gridBagSizer->Add( m_staticline1, wxGBPosition( 3, 0 ), wxGBSpan( 1, 2 ), wxEXPAND | wxALL, 5 );
	
	m_progressComparison = new wxGauge( this, wxID_ANY, 100, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL|wxGA_SMOOTH );
	m_progressComparison->SetValue( 0 ); 
	m_progressComparison->SetMinSize( wxSize( 500,25 ) );
	
	gridBagSizer->Add( m_progressComparison, wxGBPosition( 4, 0 ), wxGBSpan( 1, 2 ), wxALL|wxEXPAND, 5 );
	
	m_txtComparisonLabel = new wxStaticText( this, wxID_ANY, wxT("Files Compared:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_txtComparisonLabel->Wrap( -1 );
	gridBagSizer->Add( m_txtComparisonLabel, wxGBPosition( 5, 0 ), wxGBSpan( 1, 1 ), wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	m_txtComparison = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY|wxNO_BORDER );
	m_txtComparison->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW ) );

	gridBagSizer->Add( m_txtComparison, wxGBPosition( 5, 1 ), wxGBSpan( 1, 1 ), wxALL|wxEXPAND, 5 );
	
	m_staticline2 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	gridBagSizer->Add( m_staticline2, wxGBPosition( 6, 0 ), wxGBSpan( 1, 2 ), wxEXPAND | wxALL, 5 );
	
	m_txtFiles = new wxStaticText( this, wxID_ANY, wxT("Files Processed:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_txtFiles->Wrap( -1 );
	gridBagSizer->Add( m_txtFiles, wxGBPosition( 7, 0 ), wxGBSpan( 1, 1 ), wxALL, 5 );
	
	m_txtPatchProcessed = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY|wxNO_BORDER );
	m_txtPatchProcessed->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW ) );

	gridBagSizer->Add( m_txtPatchProcessed, wxGBPosition( 7, 1 ), wxGBSpan( 1, 1 ), wxALL|wxEXPAND, 5 );
	
	m_progressPatchProcessed = new wxGauge( this, wxID_ANY, 100, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL );
	m_progressPatchProcessed->SetValue( 0 ); 
	m_progressPatchProcessed->SetMinSize( wxSize( 500,25 ) );
	
	gridBagSizer->Add( m_progressPatchProcessed, wxGBPosition( 8, 0 ), wxGBSpan( 1, 2 ), wxALL|wxEXPAND, 5 );
	
	m_txtProcessing = new wxStaticText( this, wxID_ANY, wxT("Processing File:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_txtProcessing->Wrap( -1 );
	gridBagSizer->Add( m_txtProcessing, wxGBPosition( 9, 0 ), wxGBSpan( 1, 1 ), wxALL, 5 );
	
	m_txtProcessingFile = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY|wxNO_BORDER );
	m_txtProcessingFile->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW ) );
	gridBagSizer->Add( m_txtProcessingFile, wxGBPosition( 9, 1 ), wxGBSpan( 1, 1 ), wxALL|wxEXPAND, 5 );
	
	m_progressFileProccess = new wxGauge( this, wxID_ANY, 100, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL );
	m_progressFileProccess->SetValue( 0 ); 
	m_progressFileProccess->SetMinSize( wxSize( 500,25 ) );
	
	gridBagSizer->Add( m_progressFileProccess, wxGBPosition( 10, 0 ), wxGBSpan( 1, 2 ), wxALL|wxEXPAND, 5 );

	gridBagSizer->AddGrowableCol( 1 );
	
	this->SetSizer( gridBagSizer );
	this->Layout();
	this->Centre( wxBOTH );

	Bind(wxEVT_COMMAND_UPDATE_COMPARISON_PROGRESSBAR, &CreatePatchFrame::OnComparisonProgressUpdate, this);
	Bind(wxEVT_COMMAND_UPDATE_COMPARISON_TEXT, &CreatePatchFrame::OnComparisonTextUpdate, this);

	Bind(wxEVT_COMMAND_UPDATE_PATCH_PROCCESS_PROGRESSBAR, &CreatePatchFrame::OnPatchProcessedProgressUpdate, this);
	Bind(wxEVT_COMMAND_UPDATE_PATCH_PROCCESS_COMPARISON_TEXT, &CreatePatchFrame::OnPatchProcessedTextUpdate, this);

	Bind(wxEVT_COMMAND_UPDATE_FILE_PROCCESS_PROGRESSBAR, &CreatePatchFrame::OnFileProcessedProgressUpdate, this);
	Bind(wxEVT_COMMAND_UPDATE_FILE_PROCCESS_TEXT, &CreatePatchFrame::OnFileProcessedTextUpdate, this);

	Bind(wxEVT_CLOSE_WINDOW, &CreatePatchFrame::OnClose, this);

#ifndef __APPLE__
	SetDoubleBuffered(true);
#endif
}

CreatePatchFrame::~CreatePatchFrame()
{

}

void CreatePatchFrame::DoStartCreatePatchThread(wxString oldDirectory, wxString newDirectory, wxString outputFileName)
{
	m_pThread = new CreatePatchThread(this);
	m_pThread->m_oldDirectory	= oldDirectory;
	m_pThread->m_newDirectory	= newDirectory;
	m_pThread->m_outputFilename	= outputFileName;

	if (m_pThread->Run() != wxTHREAD_NO_ERROR)
	{
		// TODO: Handle this appropriately!
		wxLogError("Can't create the thread!");
		delete m_pThread;
		m_pThread = NULL;
	}

	// From now on, the state of the pointer would be unreliable, since this is a detached thread.
	// OnThreadExit() will set the pointer to nullptr, to avoid problems with this (hence, the "friend class" statement)
}

void CreatePatchFrame::OnComparisonProgressUpdate(wxThreadEvent& evt)
{
	m_progressComparison->SetValue(evt.GetInt());
}

void CreatePatchFrame::OnComparisonTextUpdate(wxThreadEvent& evt)
{
	m_txtComparison->SetValue(evt.GetString());
}

void CreatePatchFrame::OnPatchProcessedProgressUpdate(wxThreadEvent& evt)
{
	m_progressPatchProcessed->SetValue(evt.GetInt());
}

void CreatePatchFrame::OnPatchProcessedTextUpdate(wxThreadEvent& evt)
{
	m_txtPatchProcessed->SetValue(evt.GetString());
}

void CreatePatchFrame::OnFileProcessedProgressUpdate(wxThreadEvent& evt)
{
	m_progressFileProccess->SetValue(evt.GetInt());
}

void CreatePatchFrame::OnFileProcessedTextUpdate(wxThreadEvent& evt)
{
	m_txtProcessingFile->SetValue(evt.GetString());
}

void CreatePatchFrame::OnClose(wxCloseEvent& evt)
{
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

void CreatePatchFrame::UpdateComparisonDisplay(const float& Percentage, const uint64_t& leftAmount, const uint64_t& rightAmount)
{
	wxThreadEvent* updateProgressBarEvent = new wxThreadEvent(wxEVT_COMMAND_UPDATE_COMPARISON_PROGRESSBAR);
	updateProgressBarEvent->SetInt(static_cast<int>(Percentage));
	wxQueueEvent(wxTheApp->GetTopWindow()->GetEventHandler(), updateProgressBarEvent);

	wxThreadEvent* updateProgressTextEvent = new wxThreadEvent(wxEVT_COMMAND_UPDATE_COMPARISON_TEXT);
	updateProgressTextEvent->SetString(wxString::Format("%llu / %llu", leftAmount, rightAmount));
	wxQueueEvent(wxTheApp->GetTopWindow()->GetEventHandler(), updateProgressTextEvent);
}

void CreatePatchFrame::UpdatePatchProcessedDisplay(const float& Percentage, const uint64_t& leftAmount, const uint64_t& rightAmount)
{
	wxThreadEvent* updateProgressBarEvent = new wxThreadEvent(wxEVT_COMMAND_UPDATE_PATCH_PROCCESS_PROGRESSBAR);
	updateProgressBarEvent->SetInt(static_cast<int>(Percentage));
	wxQueueEvent(wxTheApp->GetTopWindow()->GetEventHandler(), updateProgressBarEvent);

	wxThreadEvent* updateProgressTextEvent = new wxThreadEvent(wxEVT_COMMAND_UPDATE_PATCH_PROCCESS_COMPARISON_TEXT);
	updateProgressTextEvent->SetString(wxString::Format("%llu / %llu", leftAmount, rightAmount));
	wxQueueEvent(wxTheApp->GetTopWindow()->GetEventHandler(), updateProgressTextEvent);
}

void CreatePatchFrame::UpdateFileProcessedDisplay(const float& Percentage, std::string fileName)
{

	wxThreadEvent* updateFileProccesedPBarEvent = new wxThreadEvent(wxEVT_COMMAND_UPDATE_FILE_PROCCESS_PROGRESSBAR);
	updateFileProccesedPBarEvent->SetInt(static_cast<int>(Percentage));
	wxQueueEvent(wxTheApp->GetTopWindow()->GetEventHandler(), updateFileProccesedPBarEvent);

	wxThreadEvent* updateFileProccesedTextEvent = new wxThreadEvent(wxEVT_COMMAND_UPDATE_FILE_PROCCESS_TEXT);
	wxString wxFileName(fileName.c_str(), wxConvUTF8);
	updateFileProccesedTextEvent->SetString(wxFileName);
	wxQueueEvent(wxTheApp->GetTopWindow()->GetEventHandler(), updateFileProccesedTextEvent);
}

SRes CreatePatchFrame::OnLZMAProgress(void *p, UInt64 inSize, UInt64 outSize)
{
	ZPatcher::ICompressProgressPlus* progress = reinterpret_cast<ZPatcher::ICompressProgressPlus*>(p);
	UpdateFileProcessedDisplay((((float)inSize / (float)progress->TotalSize)*100.0f), progress->FileName);
	return SZ_OK;
}
