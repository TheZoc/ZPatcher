//////////////////////////////////////////////////////////////////////////
//
// VisualCreatePatch - Patcher system - Part of the ZUpdater suite
// Felipe "Zoc" Silveira - (c) 2016
//
//////////////////////////////////////////////////////////////////////////
//
// VisualCreatePatch.cpp
// Implement the entry point for the visual application to create patch
//
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <string>
#include "CreatePatchFrame.h"
#include "VisualCreatePatch.h"
#include "CreatePatch.h"

wxIMPLEMENT_APP(VisualCreatePatch);

bool VisualCreatePatch::OnInit()
{
	if (!wxApp::OnInit())
		return false;

	// Allow us to process Idle events
	Connect(wxID_ANY, wxEVT_IDLE, wxIdleEventHandler(VisualCreatePatch::OnIdle));

	m_pFrame = new CreatePatchFrame(nullptr);

#ifdef _WIN32
	m_pFrame->SetIcon(wxICON(frame_icon));
#elif __APPLE__
	// TODO: Remove this hardcode
	m_pFrame->SetIcon(wxIcon("VisualCreatePatch.icns"));
//#else __linux__
//	// TODO: Find out how this works in linux!
#endif

	m_pFrame->Show(true);

	m_pFrame->m_txtOldDirectory->SetLabelText(m_oldDirectory);
	m_pFrame->m_txtNewDirectory->SetLabelText(m_newDirectory);
	m_pFrame->m_txtPatchFile->SetLabelText(m_outputFilename);

	m_pFrame->DoStartCreatePatchThread(m_oldDirectory, m_newDirectory, m_outputFilename);

	return true;
}

void VisualCreatePatch::OnInitCmdLine(wxCmdLineParser& parser)
{
	parser.SetDesc(g_cmdLineDesc);

	// must refuse '/' as parameter starter or cannot use "/path" style paths
	parser.SetSwitchChars(wxT("-"));
}

bool VisualCreatePatch::OnCmdLineParsed(wxCmdLineParser& parser)
{
	if (!parser.Found(wxT("o"), &m_oldDirectory)) return false;
	if (!parser.Found(wxT("n"), &m_newDirectory)) return false;
	if (!parser.Found(wxT("p"), &m_outputFilename)) return false;

	return true;
}

void VisualCreatePatch::OnIdle(wxIdleEvent& event)
{
	// Check if our thread has ended - if so, automatically close the program
	if (m_pFrame->m_pThread == nullptr)
		wxQueueEvent(wxTheApp->GetTopWindow()->GetEventHandler(), new wxCloseEvent(wxEVT_CLOSE_WINDOW));

	// Ask for the next Idle event
	event.RequestMore();
}
