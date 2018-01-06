//////////////////////////////////////////////////////////////////////////
//
// VisualCreatePatch - Patcher system - Part of the ZUpdater suite
// Felipe "Zoc" Silveira - (c) 2016-2018
//
//////////////////////////////////////////////////////////////////////////
//
// CreatePatchThread.cpp
// Header file for the worker thread used in the patch creation process
//
//////////////////////////////////////////////////////////////////////////

#ifndef _CREATEPATCHTHREAD_H_
#define _CREATEPATCHTHREAD_H_

#include <wx/thread.h>
#include <wx/event.h>
#include "CreatePatch.h"
#include "CreatePatchFrame.h"

//////////////////////////////////////////////////////////////////////////
// Declare the events used to update the main frame

wxDECLARE_EVENT(wxEVT_COMMAND_UPDATE_COMPARISON_PROGRESSBAR, wxThreadEvent);
wxDECLARE_EVENT(wxEVT_COMMAND_UPDATE_COMPARISON_TEXT, wxThreadEvent);

wxDECLARE_EVENT(wxEVT_COMMAND_UPDATE_PATCH_PROCCESS_PROGRESSBAR, wxThreadEvent);
wxDECLARE_EVENT(wxEVT_COMMAND_UPDATE_PATCH_PROCCESS_COMPARISON_TEXT, wxThreadEvent);

wxDECLARE_EVENT(wxEVT_COMMAND_UPDATE_FILE_PROCCESS_PROGRESSBAR, wxThreadEvent);
wxDECLARE_EVENT(wxEVT_COMMAND_UPDATE_FILE_PROCCESS_TEXT, wxThreadEvent);

//////////////////////////////////////////////////////////////////////////
// Implement the worker thread

class CreatePatchThread : public wxThread
{
public:
	CreatePatchThread(CreatePatchFrame* handler);
	~CreatePatchThread();
	
	wxString m_oldDirectory;
	wxString m_newDirectory;
	wxString m_outputFilename;

protected:
	virtual ExitCode Entry();
	CreatePatchFrame*	m_pHandler;
	ZPatcher::PatchFileList_t*	m_pPatchFileList;
};

#endif // _CREATEPATCHTHREAD_H_
