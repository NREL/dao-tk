#include "vardialog.h"
#include "daotk_app.h"

#include <wx/wxhtml.h>
#include <wx/combo.h>
#include <wx/bmpbuttn.h>

#include <wex/icons/stock_cancel_20.cpng>

enum {ID_SEARCHSELECT=wxID_HIGHEST+154, ID_SEARCHTEXT, ID_SEARCHCLEAR, ID_VAR_HELP};

VariableDialog::VariableDialog(wxWindow *parent, std::vector< void* > vargroups, int id, long style, wxSize size, wxPoint position)
    : wxFrame(parent, id, "Variable information", position, size, style)
{

    m_variable_data.clear();
    for( std::vector<void*>::iterator vg = vargroups.begin(); vg != vargroups.end(); vg ++)
        m_variable_data.push_back( static_cast<lk::varhash_t*>( *vg ) );

    wxPanel* main_panel = new wxPanel(this);

    std::vector< std::string> searchchoices = { "All", "Name", "Description", "Type" };
    wxArrayString sca; 
    for (size_t i = 0; i < searchchoices.size(); i++)
        sca.push_back(searchchoices[i]);

    m_searchtext = new wxTextCtrl(main_panel, ID_SEARCHTEXT, wxEmptyString, wxDefaultPosition, wxSize(250,21) );
    m_searchselect = new wxComboBox(main_panel, ID_SEARCHSELECT, searchchoices[0],
                    wxDefaultPosition, wxDefaultSize, sca, wxCB_DROPDOWN | wxCB_READONLY);
    
    m_html = new wxHtmlWindow(main_panel, ID_VAR_HELP);

    wxBoxSizer *mainsizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer *topsizer = new wxBoxSizer(wxHORIZONTAL);

    topsizer->Add(m_searchtext, 1, wxEXPAND | wxALL, 2);
    topsizer->Add(new wxBitmapButton(main_panel, ID_SEARCHCLEAR, wxBITMAP_PNG_FROM_DATA(stock_cancel_20) ), 0, wxALL, 2);
    topsizer->Add(m_searchselect, 0, wxALL, 2);

    mainsizer->Add(topsizer);
    mainsizer->Add(m_html, 1, wxEXPAND | wxALL, 2);

    UpdateHelp("", "All");

    main_panel->SetSizer(mainsizer);

    wxBoxSizer *windowsizer = new wxBoxSizer(wxVERTICAL);
    windowsizer->Add(main_panel, 1, wxEXPAND, 0);
    this->SetSizer(windowsizer);

    this->Layout();
}


void VariableDialog::OnCommand(wxCommandEvent &evt)
{

    switch (evt.GetId())
    {
    case ID_SEARCHTEXT:
    case ID_SEARCHSELECT:
    {
        wxString filter = m_searchtext->GetValue();
        wxString filtertype = m_searchselect->GetValue();
        UpdateHelp(filter.c_str(), filtertype.c_str());
        break;
    }
    case ID_SEARCHCLEAR:
        m_searchtext->SetValue("");
        wxString filtertype = m_searchselect->GetValue();
        UpdateHelp("", filtertype.c_str());
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
        std::string filtered_group_items;
        std::string group_formatted;

        for (lk::varhash_t::iterator vit = (*grit)->begin(); vit != (*grit)->end(); vit++)
        {
            data_base* v = static_cast<data_base*>(vit->second);

            if( v->nice_name.empty() )
                continue;

            if ( group_formatted.empty() )
            {
                std::string groupname = v->group;
                std::string groupfirst = wxSplit(groupname, '|').front();
                group_formatted.append("<table style=\"width:100%;background-color:#444;fontsize:+3;\"><tr><td><font size=\"+4\" color=\"#fff\">");
                group_formatted.append(groupfirst);
                group_formatted.append("</font></td></tr></table>");
            }

            if (sfilter.empty())
            {
                filtered_group_items.append(v->doc.formatted_doc);
                continue;
            }

            if (stype == "Name")
            {
                if (v->name.find(sfilter.c_str(), 0) != std::string::npos)
                    filtered_group_items.append(v->doc.formatted_doc);
            }
            else if (stype == "Description")
            {
                if (v->doc.description.find(sfilter.c_str(), 0) != std::string::npos)
                    filtered_group_items.append(v->doc.formatted_doc);
            }
            else if (stype == "Type")
            {
                switch (v->type())
                {
                case data_base::NUMBER:
                    if (wxString("number double float integer long").Find(sfilter) != wxNOT_FOUND)
                        filtered_group_items.append(v->doc.formatted_doc);
                    break;
                case data_base::STRING:
                    if (wxString("string character text").Find(sfilter) != wxNOT_FOUND)
                        filtered_group_items.append(v->doc.formatted_doc);
                    break;
                case data_base::VECTOR:
                    if (wxString("vector array matrix list").Find(sfilter) != wxNOT_FOUND)
                        filtered_group_items.append(v->doc.formatted_doc);
                    break;
                case data_base::HASH:
                    if (wxString("table hash map").Find(sfilter) != wxNOT_FOUND)
                        filtered_group_items.append(v->doc.formatted_doc);
                    break;
                default:
                    break;
                }
            }
            else // (stype == "" || stype == "All")
            {
                if (v->doc.formatted_doc.find(sfilter.c_str(), 0) != std::string::npos)
                    filtered_group_items.append(v->doc.formatted_doc);
            }
        }

        if ( ! filtered_group_items.empty() )
        {
            filtered_text.append(group_formatted);
            filtered_text.append(filtered_group_items);
        }
    }
    filtered_text.append("</body></html>");


    m_html->SetPage(filtered_text);
}

void VariableDialog::OnHtmlEvent(wxHtmlLinkEvent &evt)
{
    wxString link = evt.GetLinkInfo().GetHref();

    if( link.Find("id?") > -1 )
    {
        wxArrayString idparse = wxSplit(link.c_str(), '?') ;
        if( idparse.size() != 2 )
            return;
        wxString var = idparse.at(1);

        for( std::vector< lk::varhash_t* >::iterator vgroup = m_variable_data.begin(); 
                                                     vgroup!= m_variable_data.end(); 
                                                     vgroup ++ )
        {
            lk::varhash_t::iterator vfind = (*vgroup)->find(var);
            if( vfind != (*vgroup)->end() )   
            {
                MainWindow::Instance().ScriptInsert( (wxString::Format("var(\"%s\")", vfind->first.c_str())).c_str() );
                return;
            }
        }
    }

    return;
}

BEGIN_EVENT_TABLE( VariableDialog, wxFrame )
    EVT_TEXT(ID_SEARCHTEXT, VariableDialog::OnCommand)
    EVT_COMBOBOX(ID_SEARCHSELECT, VariableDialog::OnCommand)
    EVT_BUTTON(ID_SEARCHCLEAR, VariableDialog::OnCommand)
    EVT_HTML_LINK_CLICKED(ID_VAR_HELP, VariableDialog::OnHtmlEvent)
END_EVENT_TABLE()