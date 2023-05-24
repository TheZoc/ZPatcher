//////////////////////////////////////////////////////////////////////////
//
// VisualCreatePatch - Patcher system - Part of the ZUpdater suite
// Felipe "Zoc" Silveira - (c) 2016-2018
//
//////////////////////////////////////////////////////////////////////////
//
// CreatePatchThread.cpp
// Implement worker thread for the patch creation process
//
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "wx/thread.h"
#include "wx/msgdlg.h"
#include "CreatePatchFrame.h"
#include "CreatePatchThread.h"
#include "LogSystem.h"
#include "FileUtils.h"
#include "XMLFileUtils.h"


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
	, m_exportXml(false)
	, m_importXml(false)
	, m_pHandler(handler)
	, m_pPatchFileList(nullptr)
{}

CreatePatchThread::~CreatePatchThread()
{
	wxCriticalSectionLocker enter(m_pHandler->m_pThreadCS);

	if (m_pPatchFileList)
		delete(m_pPatchFileList);

	// the thread is being destroyed; make sure not to leave dangling pointers around
	m_pHandler->m_pThread = nullptr;
}

wxThread::ExitCode CreatePatchThread::Entry()
{
	using namespace ZPatcher;

	// Check through execution if we should destroy the thread and exit
	if (TestDestroy()) { return (wxThread::ExitCode)0; }

	SetActiveLog("VisualCreatePatch");

	const std::string oldDirectory = m_oldDirectory.ToStdString();
	const std::string newDirectory = m_newDirectory.ToStdString();
	const std::string outputFilename = m_outputFilename.ToStdString();

	if (TestDestroy()) { return (wxThread::ExitCode)0; }

	if (m_importXml)
	{
		// Read an XML file instead of calling GetDifferences()
		m_pPatchFileList = new ZPatcher::PatchFileList_t();
		if (!XMLFileUtils::LoadXMLPatchFile(outputFilename + ".xml", m_pPatchFileList))
		{
			wxMessageBox(wxT("Error attempting to load " + outputFilename + ".xml\nCheck the logs for more details."), wxT("Error"), wxICON_ERROR);
			return (wxThread::ExitCode)0; // success
		}
	}
	else
	{
		// First, create the list of files to be added to the patch
		m_pPatchFileList = GetDifferencesEx(oldDirectory, newDirectory, &CreatePatchFrame::UpdateComparisonDisplay);
	}

	if (TestDestroy()) { return (wxThread::ExitCode)0; }

	if (m_exportXml)
	{
		// Export the XML with the diff data
		XMLFileUtils::SaveXMLPatchFile(m_pPatchFileList, outputFilename + ".xml");
		return (wxThread::ExitCode)0; // success
	}
	else
	{
		// Then, create the patch file.
		// This is ugly, since there is no way to check inside CreatePatch() if the thread was destroyed. Check if there's a better way to do this.
		CreatePatchFileEx(outputFilename, newDirectory, m_pPatchFileList, &CreatePatchFrame::UpdatePatchProcessedDisplay, reinterpret_cast<LZMA_ICompressProgress>(&CreatePatchFrame::OnLZMAProgress));
	}

	return (wxThread::ExitCode)0;     // success
}
