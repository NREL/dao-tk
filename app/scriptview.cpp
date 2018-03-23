#include <wx/wx.h>
#include <wex/numeric.h>

#include "scriptview.h"

enum {
	ID_ELEMENT_LIST = wxID_HIGHEST + 198,
	ID_NONE
};

BEGIN_EVENT_TABLE(ScriptView, wxPanel)
EVT_LISTBOX(ID_ELEMENT_LIST, ScriptView::OnCommand)
END_EVENT_TABLE()


ScriptView::ScriptView(wxWindow *parent)
	: wxPanel(parent)
{

	wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
	sizer->Add(new wxStaticText(this, wxID_ANY, "X bins:"), 0, wxLEFT | wxRIGHT , 5);
	SetSizer(sizer);
}

ScriptView::~ScriptView()
{
	/* nothing to do */
}

void ScriptView::OnCommand(wxCommandEvent &evt)
{
	return;
}