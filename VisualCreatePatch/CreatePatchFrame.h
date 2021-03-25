//////////////////////////////////////////////////////////////////////////
//
// VisualCreatePatch - Patcher system - Part of the ZUpdater suite
// Felipe "Zoc" Silveira - (c) 2016-2018
//
//////////////////////////////////////////////////////////////////////////
//
// CreatePatchFrame.cpp
// Header file for the frame with the create patch progress and related events
//
//////////////////////////////////////////////////////////////////////////

#ifndef __CREATEPATCHFRAME_H__
#define __CREATEPATCHFRAME_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/statline.h>
#include <wx/gauge.h>
#include <wx/textctrl.h>
#include <wx/gbsizer.h>
#include <wx/frame.h>
#include <wx/dcbuffer.h>

#include "7zTypes.h"

class CreatePatchThread;

///////////////////////////////////////////////////////////////////////////////
/// Class CreatePatchFrame
///////////////////////////////////////////////////////////////////////////////
class CreatePatchFrame : public wxFrame 
{
//	private:
//	protected:
	public:
		wxStaticText* m_txtOldLabel;
		wxStaticText* m_txtOldDirectory;
		wxStaticText* m_txtNewLabel;
		wxStaticText* m_txtNewDirectory;
		wxStaticText* m_txtPatchLabel;
		wxStaticText* m_txtPatchFile;
		wxStaticLine* m_staticline1;
		wxGauge* m_progressComparison;
		wxStaticText* m_txtComparisonLabel;
		wxTextCtrl* m_txtComparison;
		wxStaticLine* m_staticline2;
		wxStaticText* m_txtFiles;
		wxTextCtrl* m_txtPatchProcessed;
		wxGauge* m_progressPatchProcessed;
		wxStaticText* m_txtProcessing;
		wxTextCtrl* m_txtProcessingFile;
		wxGauge* m_progressFileProccess;
	
	public:
		CreatePatchFrame( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("CreatePatch : ZPatcher v2.0 beta"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 530,325 ), long style = wxCAPTION|wxCLOSE_BOX|wxRESIZE_BORDER|wxSYSTEM_MENU );
		~CreatePatchFrame();

public:
	void DoStartCreatePatchThread(wxString oldDirectory, wxString newDirectory, wxString outputFileName);

	void OnComparisonProgressUpdate(wxThreadEvent& evt);
	void OnComparisonTextUpdate(wxThreadEvent& evt);

	void OnPatchProcessedProgressUpdate(wxThreadEvent& evt);
	void OnPatchProcessedTextUpdate(wxThreadEvent& evt);

	void OnFileProcessedProgressUpdate(wxThreadEvent& evt);
	void OnFileProcessedTextUpdate(wxThreadEvent& evt);

	void OnClose(wxCloseEvent& evt);
	
	static void UpdateComparisonDisplay(const float& Percentage, const uint64_t& leftAmount, const uint64_t& rightAmount);
	static void UpdatePatchProcessedDisplay(const float& Percentage, const uint64_t& leftAmount, const uint64_t& rightAmount);
	static void UpdateFileProcessedDisplay(const float& Percentage, std::string fileName);

	static SRes OnLZMAProgress(const ICompressProgress* p, UInt64 inSize, UInt64 outSize);

protected:
	friend class VisualCreatePatch;		// Allows to check if m_pThread is set to null, to end the program
	friend class CreatePatchThread;		// Allow it to access our m_pThread
	CreatePatchThread* m_pThread;		// Our thread pointer
	wxCriticalSection m_pThreadCS;		// protects the m_pThread pointer
};

#endif //__CREATEPATCHFRAME_H__
