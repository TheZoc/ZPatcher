//////////////////////////////////////////////////////////////////////////
//
// VisualCreatePatch - Patcher system - Part of the ZUpdater suite
// Felipe "Zoc" Silveira - (c) 2016
//
//////////////////////////////////////////////////////////////////////////
//
// CreatePatchThread.cpp
// Implement worker thread for the patch creation process
//
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "wx/thread.h"
#include "CreatePatchFrame.h"
#include "CreatePatchThread.h"
#include "LogSystem.h"
#include "FileUtils.h"

//////////////////////////////////////////////////////////////////////////
// Defube the events used to update the create patch frame

wxDEFINE_EVENT(wxEVT_COMMAND_UPDATE_COMPARISON_PROGRESSBAR, wxThreadEvent);
wxDEFINE_EVENT(wxEVT_COMMAND_UPDATE_COMPARISON_TEXT, wxThreadEvent);

wxDEFINE_EVENT(wxEVT_COMMAND_UPDATE_PATCH_PROCCESS_PROGRESSBAR, wxThreadEvent);
wxDEFINE_EVENT(wxEVT_COMMAND_UPDATE_PATCH_PROCCESS_COMPARISON_TEXT, wxThreadEvent);

wxDEFINE_EVENT(wxEVT_COMMAND_UPDATE_FILE_PROCCESS_PROGRESSBAR, wxThreadEvent);
wxDEFINE_EVENT(wxEVT_COMMAND_UPDATE_FILE_PROCCESS_TEXT, wxThreadEvent);

//////////////////////////////////////////////////////////////////////////

CreatePatchThread::CreatePatchThread(CreatePatchFrame *handler)
	: wxThread(wxTHREAD_DETACHED)
{
	m_pHandler = handler;
	m_pPatchFileList = nullptr;
}

CreatePatchThread::~CreatePatchThread()
{
	wxCriticalSectionLocker enter(m_pHandler->m_pThreadCS);

	if (m_pPatchFileList)
		delete(m_pPatchFileList);

	// the thread is being destroyed; make sure not to leave dangling pointers around
	m_pHandler->m_pThread = NULL;
}

wxThread::ExitCode CreatePatchThread::Entry()
{
	using namespace ZPatcher;

	// Check through execution if we should destroy the thread and exit
	if (TestDestroy()) { ZPatcher::DestroyLogSystem(); return (wxThread::ExitCode)0; }

	SetActiveLog("VisualCreatePatch");

	std::string oldDirectory = m_oldDirectory.ToStdString();
	NormalizeFileName(oldDirectory);

	std::string newDirectory = m_newDirectory.ToStdString();
	NormalizeFileName(newDirectory);

	std::string outputFilename = m_outputFilename.ToStdString();
	NormalizeFileName(outputFilename);

	if (TestDestroy()) { ZPatcher::DestroyLogSystem(); return (wxThread::ExitCode)0; }

	// First, create the list of files to be added to the patch
	m_pPatchFileList = GetDifferences(oldDirectory, newDirectory, &CreatePatchFrame::UpdateComparisonDisplay);

	if (TestDestroy()) { ZPatcher::DestroyLogSystem(); return (wxThread::ExitCode)0; }

	// Then, create the patch file.
	// This is ugly, since there is no way to check inside CreatePatch() if the thread was destroyed. Since this is a proof of concept, it will do for now :)
	CreatePatchFile(outputFilename, newDirectory, m_pPatchFileList, &CreatePatchFrame::UpdatePatchProcessedDisplay, { &CreatePatchFrame::OnLZMAProgress } );

	ZPatcher::DestroyLogSystem();
	return (wxThread::ExitCode)0;     // success
}
