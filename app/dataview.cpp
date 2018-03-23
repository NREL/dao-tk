#include <wx/wx.h>
#include <wex/numeric.h>

#include "dataview.h"

enum {
	ID_ELEMENT_LIST = wxID_HIGHEST + 198,
	ID_NONE
};

BEGIN_EVENT_TABLE(DataView, wxPanel)
EVT_LISTBOX(ID_ELEMENT_LIST, DataView::OnCommand)
END_EVENT_TABLE()


DataView::DataView(wxWindow *parent)
	: wxPanel(parent)
{

	wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
	sizer->Add(new wxStaticText(this, wxID_ANY, "Data view text"), 0, wxLEFT | wxRIGHT, 5);
	SetSizer(sizer);
}

DataView::~DataView()
{
	/* nothing to do */
}

void DataView::OnCommand(wxCommandEvent &evt)
{
	return;
}