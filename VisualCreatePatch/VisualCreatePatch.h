//////////////////////////////////////////////////////////////////////////
//
// VisualCreatePatch - Patcher system - Part of the ZUpdater suite
// Felipe "Zoc" Silveira - (c) 2016-2018
//
//////////////////////////////////////////////////////////////////////////
//
// VisualCreatePatch.h
// Header for the entry point
//
//////////////////////////////////////////////////////////////////////////

#ifndef _VISUALCREATEPATCH_H_
#define _VISUALCREATEPATCH_H_

#include <wx/wxprec.h>
#include <wx/cmdline.h>

class VisualCreatePatch : public wxApp
{
public:
	virtual bool OnInit() override;

	virtual void OnInitCmdLine(wxCmdLineParser& parser) override;
	virtual bool OnCmdLineParsed(wxCmdLineParser& parser) override;

	void OnIdle(wxIdleEvent& event);

private:
	CreatePatchFrame*	m_pFrame;
	wxString			m_oldDirectory;
	wxString			m_newDirectory;
	wxString			m_outputFilename;
	bool				m_exportXml;
	bool				m_importXml;
};

static const wxCmdLineEntryDesc g_cmdLineDesc[] =
{
	{ wxCMD_LINE_OPTION, "o", "old", "old version directory", wxCMD_LINE_VAL_STRING , wxCMD_LINE_OPTION_MANDATORY },
	{ wxCMD_LINE_OPTION, "n", "new", "new version directory", wxCMD_LINE_VAL_STRING , wxCMD_LINE_OPTION_MANDATORY },
	{ wxCMD_LINE_OPTION, "p", "patchfile", "output filename of the patch file", wxCMD_LINE_VAL_STRING , wxCMD_LINE_OPTION_MANDATORY },
	{ wxCMD_LINE_SWITCH, "x", "exportxmldiff", "create XML with the diff contents", wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL },
	{ wxCMD_LINE_SWITCH, "i", "importxmldiff", "uses the previously exported XML file to create the patch file", wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL },
	wxCMD_LINE_DESC_END
};

#endif // _VISUALCREATEPATCH_H_
