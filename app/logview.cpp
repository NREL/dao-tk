#include <wx/wx.h>
#include <wx/clipbrd.h>
#include <wex/numeric.h>

#include "logview.h"
#include "iterplot.h"

enum A {
	ID_ELEMENT_LIST = wxID_HIGHEST + 198,
	ID_LOG_CLEAR,
	ID_LOG_COPY,
	ID_NONE
};

BEGIN_EVENT_TABLE(LogView, wxPanel)
EVT_LISTBOX(ID_ELEMENT_LIST, LogView::OnCommand)
EVT_BUTTON(ID_LOG_CLEAR, LogView::OnCommand)
EVT_BUTTON(ID_LOG_COPY, LogView::OnCommand)
END_EVENT_TABLE()


LogView::LogView(wxWindow *parent)
	: wxPanel(parent)
{

	wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer *top_sizer = new wxBoxSizer(wxHORIZONTAL);

	top_sizer->Add(new wxButton(this, ID_LOG_CLEAR, "Clear log", wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT), 0, wxALL | wxEXPAND, 2);
	top_sizer->Add(new wxButton(this, ID_LOG_COPY, "Copy log", wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT), 0, wxALL | wxEXPAND, 2);

	m_TextLog = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize,
		wxTE_READONLY | wxTE_MULTILINE | wxHSCROLL | wxTE_DONTWRAP | wxBORDER_NONE);
	m_TextLog->SetFont(wxFont(10, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "consolas"));
	m_TextLog->SetForegroundColour(*wxBLUE);

	sizer->Add(top_sizer);
	sizer->Add(m_TextLog, 1, wxEXPAND | wxALL, 5);
	SetSizer(sizer);
}

LogView::~LogView()
{
	/* nothing to do */
}

void LogView::OnCommand(wxCommandEvent &evt)
{
	switch (evt.GetId())
	{
	case ID_ELEMENT_LIST:
		break;
	case ID_LOG_CLEAR:
		ClearLog();
		break;
	case ID_LOG_COPY:
		if (wxTheClipboard->Open())
		{
			// This data objects are held by the clipboard,
			// so do not delete them in the app.
			wxTheClipboard->SetData(new wxTextDataObject(m_TextLog->GetValue()));
			wxTheClipboard->Close();
		}
		break;
	case ID_NONE:
		break;
	default:
		break;
	}
}

void LogView::Log(const wxString &text, bool wnl)
{
	if (wnl) m_TextLog->AppendText(text + "\n");
	else m_TextLog->AppendText(text);
}

void LogView::ClearLog()
{
	m_TextLog->Clear();
}

