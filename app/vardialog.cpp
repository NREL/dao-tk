#include "vardialog.h"

#include <wx/wxhtml.h>
#include <wx/combo.h>
#include <wx/bmpbuttn.h>

#include <wex/icons/stock_cancel_20.cpng>

enum {ID_SEARCHSELECT=wxID_HIGHEST+154, ID_SEARCHTEXT, ID_SEARCHCLEAR};

VariableDialog::VariableDialog(wxWindow *parent, int id, long style, wxSize size, wxPoint position)
    : wxFrame(parent, id, "Variable information", position, size, style)
{

    std::vector< std::string> searchchoices = { "All", "Name", "Description", "Type" };
    wxArrayString sca; 
    for (size_t i = 0; i < searchchoices.size(); i++)
        sca.push_back(searchchoices[i]);

    m_searchtext = new wxTextCtrl(this, ID_SEARCHTEXT, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTC_LEFT);
    m_searchselect = new wxComboBox(this, ID_SEARCHSELECT, searchchoices[0], 
                    wxDefaultPosition, wxDefaultSize, sca, wxCB_DROPDOWN | wxCB_READONLY);
    
    m_html = new wxHtmlWindow(this, wxID_ANY);

    wxBoxSizer *mainsizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer *topsizer = new wxBoxSizer(wxHORIZONTAL);

    topsizer->Add(m_searchtext, 1, wxEXPAND | wxALL, 2);
    topsizer->Add(new wxBitmapButton(this, ID_SEARCHCLEAR, wxIcon("") ), 0, wxALL, 2);
    topsizer->Add(m_searchselect, 0, wxALL, 2);

    mainsizer->Add(topsizer);
    mainsizer->Add(m_html, 1, wxEXPAND | wxALL, 2);

    this->SetSizer(mainsizer);
    this->Layout();
}


void VariableDialog::OnCommand(wxCommandEvent &evt)
{

    switch (evt.GetId())
    {
    case ID_SEARCHTEXT:
    {
        wxString filter = m_searchtext->GetValue();
        wxString filtertype = m_searchselect->GetValue();
        UpdateHelp(filter, filtertype);
        break;
    }
    case ID_SEARCHSELECT:
        break;
    case ID_SEARCHCLEAR:
        break;
    }

}

void VariableDialog::UpdateHelp(const char* filter, const char* type)
{
    std::string filtered_text = "<html><head></head><body>";
    std::string sfilter(filter);
    std::string stype(type);
        
    //do search here and append doc items  -- "All", "Name", "Description", "Type"
       
    if (stype == "Name")
    {

    }
    else if (stype == "Description")
    {

    }
    else if (stype == "Type")
    {

    }
    else // (stype == "" || stype == "All")
    {

    }

    filtered_text.append("</body></html>");


    m_html->SetPage(filtered_text);
}


BEGIN_EVENT_TABLE( VariableDialog, wxFrame )
    EVT_TEXT(ID_SEARCHTEXT, VariableDialog::OnCommand)
    EVT_COMBOBOX(ID_SEARCHSELECT, VariableDialog::OnCommand)
    EVT_BUTTON(ID_SEARCHCLEAR, VariableDialog::OnCommand)
END_EVENT_TABLE()