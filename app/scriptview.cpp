#include <wx/wx.h>
#include <wex/numeric.h>

#include "scriptview.h"
#include "daotk_app.h"
#include <wex/lkscript.h>
#include <wex/dview/dvplotctrl.h>
#include <wex/dview/dvtimeseriesdataset.h>
#include <wx/busyinfo.h>

#include <lk/lex.h>
#include <lk/parse.h>
#include <lk/eval.h>
#include <lk/invoke.h>
#include <lk/stdlib.h>

#include "scripting.h"
#include "vardialog.h"

void Output(const wxString &text)
{
	MainWindow::Instance().Log(text, false);
}

void Output(const char *fmt, ...)
{
	static char buf[2048];
	va_list ap;
	va_start(ap, fmt);
#if defined(_MSC_VER)||defined(_WIN32)
	_vsnprintf(buf, 2046, fmt, ap);
#else
	vsnprintf(buf, 2046, fmt, ap);
#endif
	va_end(ap);
	Output(wxString(buf));
}

void ClearOutput()
{
	MainWindow::Instance().ClearLog();
}

//----------------------------------------------------------------
static lk::fcall_t *daotk_functions()
{
	static lk::fcall_t st[] = {
		_test,
		_initialize,
		_generate_solarfield,
		_power_cycle,
		_simulate_optical,
		_simulate_solarfield,
		_simulate_performance,
		_var,
		0 };

	return (lk::fcall_t*)st;
}
//----------------------------------------------------------------


enum { ID_CODEEDITOR = wxID_HIGHEST + 1, ID_RUN, ID_HELP, ID_VARS };

class MyScriptCtrl : public wxLKScriptCtrl
{
public:
	MyScriptCtrl(wxWindow *parent, int id = wxID_ANY)
		: wxLKScriptCtrl(parent, id, wxDefaultPosition, wxDefaultSize, wxLK_STDLIB_ALL | wxLK_STDLIB_SOUT)
	{
	}

	virtual void OnOutput(const wxString &tt)
	{
		Output(tt);
	}
	virtual void OnSyntaxCheck(int, const wxString &err)
	{
		ClearOutput();
		Output(err);
	}
};

ScriptView::ScriptView(wxWindow *parent)
	: wxPanel(parent)
{

	wxBoxSizer *szdoc = new wxBoxSizer(wxHORIZONTAL);
	szdoc->Add(new wxButton(this, wxID_NEW, "New", wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT), 0, wxALL | wxEXPAND, 2);
	szdoc->Add(new wxButton(this, wxID_OPEN, "Open", wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT), 0, wxALL | wxEXPAND, 2);
	szdoc->Add(new wxButton(this, wxID_SAVE, "Save", wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT), 0, wxALL | wxEXPAND, 2);
	szdoc->Add(new wxButton(this, wxID_SAVEAS, "Save as", wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT), 0, wxALL | wxEXPAND, 2);
	szdoc->Add(new wxButton(this, wxID_FIND, "Find", wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT), 0, wxALL | wxEXPAND, 2);
	szdoc->Add(new wxButton(this, wxID_FORWARD, "Find next", wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT), 0, wxALL | wxEXPAND, 2);
	szdoc->Add(new wxButton(this, ID_HELP, "Help", wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT), 0, wxALL | wxEXPAND, 2);
	szdoc->Add(new wxButton(this, ID_RUN, "Run", wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT), 0, wxALL | wxEXPAND, 2);
    szdoc->Add(new wxButton(this, ID_VARS, "Variables", wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT), 0, wxALL | wxEXPAND, 2);
	szdoc->Add(m_stopButton = new wxButton(this, wxID_STOP, "Stop", wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT), 0, wxALL | wxEXPAND, 2);
	szdoc->AddStretchSpacer();
	m_stopButton->SetForegroundColour(*wxRED);
	m_stopButton->Hide();

	m_editor = new MyScriptCtrl(this, ID_CODEEDITOR);

	m_editor->RegisterLibrary(daotk_functions(), "DAOToolkit Functions", this);

	wxBoxSizer *szedit = new wxBoxSizer(wxVERTICAL);
	szedit->Add(szdoc, 0, wxALL | wxEXPAND, 2);
	szedit->Add(m_editor, 1, wxALL | wxEXPAND, 0);
	szedit->Add(m_statusLabel = new wxStaticText(this, wxID_ANY, wxEmptyString), 0, wxALL | wxEXPAND, 0);

	SetSizer(szedit);

	m_editor->SetFocus();
}

ScriptView::~ScriptView()
{
}

void ScriptView::OnCommand(wxCommandEvent &evt)
{
	switch (evt.GetId())
	{
	case wxID_NEW:
		CloseDoc();
		break;
	case wxID_OPEN:
		Open();
		break;
	case wxID_SAVE:
		Save();
		break;
	case wxID_SAVEAS:
		SaveAs();
		break;
	case wxID_UNDO: m_editor->Undo(); break;
	case wxID_REDO: m_editor->Redo(); break;
	case wxID_CUT: m_editor->Cut(); break;
	case wxID_COPY: m_editor->Copy(); break;
	case wxID_PASTE: m_editor->Paste(); break;
	case wxID_SELECTALL: m_editor->SelectAll(); break;
	case wxID_FIND:
		m_editor->ShowFindReplaceDialog(); break;
	case wxID_FORWARD:
		m_editor->FindNext(); break;
	case ID_HELP:
		m_editor->ShowHelpDialog(this);
		break;
	case ID_RUN:
		Exec();
		break;
    case ID_VARS:
        CreateVariableDialog();
	case wxID_STOP:
		m_editor->Stop();
		m_stopButton->Hide();
		Layout();
		break;
	}
}

void ScriptView::Open()
{
	CloseDoc();
	wxFileDialog dlg(this, "Open", wxEmptyString, wxEmptyString,
		"LK Script Files (*.lk)|*.lk",
		wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_CHANGE_DIR);

	if (dlg.ShowModal() == wxID_OK)
		if (!Load(dlg.GetPath()))
			wxMessageBox("Could not load file:\n\n" + dlg.GetPath());
}

bool ScriptView::Save()
{
	if (m_fileName.IsEmpty())
		return SaveAs();
	else
		return Write(m_fileName);
}

bool ScriptView::SaveAs()
{
	wxFileDialog dlg(this, "Save as...",
		wxPathOnly(m_fileName),
		wxFileNameFromPath(m_fileName),
		"LK Script Files (*.lk)|*.lk", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
	if (dlg.ShowModal() == wxID_OK)
		return Write(dlg.GetPath());
	else
		return false;
}


bool ScriptView::CloseDoc()
{
	if (m_editor->IsScriptRunning())
	{
		if (wxYES == wxMessageBox("A script is running. Cancel it?", "Query", wxYES_NO))
			m_editor->Stop();

		return false;
	}

	if (m_editor->GetModify())
	{
		Raise();
		wxString id = m_fileName.IsEmpty() ? "untitled" : m_fileName;
		int result = wxMessageBox("Script modified. Save it?\n\n" + id, "Query", wxYES_NO | wxCANCEL);
		if (result == wxCANCEL
			|| (result == wxYES && !Save()))
			return false;
	}

	m_editor->SetText(wxEmptyString);
	m_editor->EmptyUndoBuffer();
	m_editor->SetSavePoint();
	m_fileName.Clear();
	m_statusLabel->SetLabel(m_fileName);
	return true;
}

bool ScriptView::Write(const wxString &file)
{
	wxBusyInfo info("Saving script file...");
	wxMilliSleep(120);

	if (((wxStyledTextCtrl*)m_editor)->SaveFile(file))
	{
		m_fileName = file;
		m_statusLabel->SetLabel(m_fileName);
		return true;
	}
	else return false;
}


bool ScriptView::Load(const wxString &file)
{
	FILE *fp = fopen(file.c_str(), "r");
	if (fp)
	{
		wxString str;
		char buf[1024];
		while (fgets(buf, 1023, fp) != 0)
			str += buf;

		fclose(fp);
		m_editor->SetText(str);
		m_editor->EmptyUndoBuffer();
		m_editor->SetSavePoint();
		m_fileName = file;
		m_statusLabel->SetLabel(m_fileName);
		return true;
	}
	else return false;
}

void ScriptView::Exec()
{
	ClearOutput();
	m_stopButton->Show();
	Layout();
	wxGetApp().Yield(true);

	wxLKSetToplevelParentForPlots(&MainWindow::Instance());
	wxLKSetPlotTarget(NULL);


	wxString work_dir;
	if (!m_fileName.IsEmpty())
		work_dir = wxPathOnly(m_fileName);

	m_editor->SetWorkDir(work_dir);

	m_editor->Execute();

	if (m_stopButton->IsShown())
	{
		m_stopButton->Hide();
		Layout();
	}
}

void ScriptView::CreateVariableDialog()
{
    VariableDialog *dlg = new VariableDialog(this, wxID_ANY, wxDEFAULT_FRAME_STYLE);
    dlg->Show();
    
    return;
}

BEGIN_EVENT_TABLE(ScriptView, wxPanel)
EVT_BUTTON(wxID_NEW, ScriptView::OnCommand)
EVT_BUTTON(wxID_OPEN, ScriptView::OnCommand)
EVT_BUTTON(wxID_SAVE, ScriptView::OnCommand)
EVT_BUTTON(wxID_SAVEAS, ScriptView::OnCommand)
EVT_BUTTON(wxID_HELP, ScriptView::OnCommand)

EVT_BUTTON(wxID_FIND, ScriptView::OnCommand)
EVT_BUTTON(wxID_FORWARD, ScriptView::OnCommand)


EVT_BUTTON(ID_RUN, ScriptView::OnCommand)

EVT_BUTTON(wxID_STOP, ScriptView::OnCommand)
EVT_BUTTON(ID_RUN, ScriptView::OnCommand)
EVT_BUTTON(ID_HELP, ScriptView::OnCommand)

END_EVENT_TABLE()