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
#include "CreatePatchFrame.h"
#include "VisualCreatePatch.h"
#include <wx/app.h>
#include <string>
#include <codecvt>
#include "CreatePatch.h"

wxIMPLEMENT_APP(VisualCreatePatch);

bool VisualCreatePatch::OnInit()
{
	if (!wxApp::OnInit())
		return false;

	CreatePatchFrame* f = new CreatePatchFrame(nullptr);
	f->SetIcon(wxICON(frame_icon));
	f->Show(true);

	f->m_txtOldDirectory->SetLabelText(m_oldDirectory);
	f->m_txtNewDirectory->SetLabelText(m_newDirectory);
	f->m_txtPatchFile->SetLabelText(m_outputFilename);

	f->DoStartCreatePatchThread(m_oldDirectory, m_newDirectory, m_outputFilename);


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
