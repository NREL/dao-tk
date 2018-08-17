#include "vardialog.h"

#include <wx/wxhtml.h>
#include <wx/combo.h>
#include <wx/bmpbuttn.h>

#include <wex/icons/stock_cancel_20.cpng>

enum {ID_SEARCHSELECT=wxID_HIGHEST+154, ID_SEARCHTEXT, ID_SEARCHCLEAR};

VariableDialog::VariableDialog(wxWindow *parent, std::vector< void* > vargroups, int id, long style, wxSize size, wxPoint position)
    : wxFrame(parent, id, "Variable information", position, size, style)
{

    m_variable_data.clear();
    for( std::vector<void*>::iterator vg = vargroups.begin(); vg != vargroups.end(); vg ++)
        m_variable_data.push_back( static_cast<lk::varhash_t*>( *vg ) );

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

    UpdateHelp("", "All");

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
    wxString sfilter(filter);
    std::string stype(type);

    sfilter.MakeLower();        //lowercase
    
        
    //do search here and append doc items  -- "All", "Name", "Description", "Type"
    for (std::vector< lk::varhash_t* >::iterator grit = m_variable_data.begin(); grit != m_variable_data.end(); grit++)
    {
        
        for (lk::varhash_t::iterator vit = (*grit)->begin(); vit != (*grit)->end(); vit++)
        {
            if (vit == (*grit)->begin())
            {
                std::string groupname = (static_cast<data_base*>((*grit)->begin()->second)->group);
                std::string groupfirst = wxSplit(groupname, '|').front();
                filtered_text.append("<hr style=\"height:30px\"><br><h1>");
                filtered_text.append(groupfirst);
                filtered_text.append("</h1>");
            }

            data_base* v = static_cast<data_base*>(vit->second);

            if( v->nice_name.empty() )
                continue;

            if (sfilter.empty())
            {
                filtered_text.append(v->doc.formatted_doc);
                continue;
            }

            if (stype == "Name")
            {
                if (v->name.find(sfilter.c_str(), 0) != std::string::npos)
                    filtered_text.append(v->doc.formatted_doc);
            }
            else if (stype == "Description")
            {
                if (v->doc.description.find(sfilter.c_str(), 0) != std::string::npos)
                    filtered_text.append(v->doc.formatted_doc);
            }
            else if (stype == "Type")
            {
                switch (v->type())
                {
                case data_base::NUMBER:
                    if (wxString("number double float integer long").Find(sfilter) != wxNOT_FOUND)
                        filtered_text.append(v->doc.formatted_doc);
                    break;
                case data_base::STRING:
                    if (wxString("string character text").Find(sfilter) != wxNOT_FOUND)
                        filtered_text.append(v->doc.formatted_doc);
                    break;
                case data_base::VECTOR:
                    if (wxString("vector array matrix list").Find(sfilter) != wxNOT_FOUND)
                        filtered_text.append(v->doc.formatted_doc);
                    break;
                case data_base::HASH:
                    if (wxString("table hash map").Find(sfilter) != wxNOT_FOUND)
                        filtered_text.append(v->doc.formatted_doc);
                    break;
                default:
                    break;
                }
            }
            else // (stype == "" || stype == "All")
            {
                if (v->doc.formatted_doc.find(sfilter.c_str(), 0) != std::string::npos)
                    filtered_text.append(v->doc.formatted_doc);
            }
        }
    }
    filtered_text.append("</body></html>");


    m_html->SetPage(filtered_text);
}


BEGIN_EVENT_TABLE( VariableDialog, wxFrame )
    EVT_TEXT(ID_SEARCHTEXT, VariableDialog::OnCommand)
    EVT_COMBOBOX(ID_SEARCHSELECT, VariableDialog::OnCommand)
    EVT_BUTTON(ID_SEARCHCLEAR, VariableDialog::OnCommand)
END_EVENT_TABLE()