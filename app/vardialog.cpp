#include "vardialog.h"
#include "daotk_app.h"

#include <wx/wxhtml.h>
#include <wx/combo.h>
#include <wx/bmpbuttn.h>

#include <wex/icons/stock_cancel_20.cpng>

enum {ID_SEARCHSELECT=wxID_HIGHEST+154, ID_SEARCHTEXT, ID_SEARCHCLEAR, ID_VAR_HELP,
        ID_NAV_BACK, ID_NAV_FORWARD, ID_NAV_TOP, ID_NAV_NEXT_SECTION, ID_NAV_PREV_SECTION
    };

VariableDialog::VariableDialog(wxWindow *parent, std::vector< void* > vargroups, std::string imgpath, int id, long style, wxSize size, wxPoint position)
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

    m_searchtext = new wxTextCtrl(main_panel, ID_SEARCHTEXT, wxEmptyString, wxDefaultPosition, wxSize(250,30) );
    m_searchtext->SetForegroundColour( wxColour("#888") );
    m_searchselect = new wxComboBox(main_panel, ID_SEARCHSELECT, searchchoices[0],
                    wxDefaultPosition, wxDefaultSize, sca, wxCB_DROPDOWN | wxCB_READONLY);
    
    m_html = new wxHtmlWindow(main_panel, ID_VAR_HELP);

    wxBoxSizer *mainsizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer *topsizer = new wxBoxSizer(wxHORIZONTAL);
    // ID_NAV_BACK, ID_NAV_FORWARD, ID_NAV_TOP, ID_NAV_NEXT_SECTION
    wxBitmapButton *bmtmp;
    topsizer->Add(bmtmp = new wxBitmapButton(main_panel, ID_NAV_BACK, wxIcon(imgpath+"/arrow-left.png", wxBITMAP_TYPE_PNG) ), 0, wxBU_EXACTFIT|wxALL, 2);
    bmtmp->SetToolTip("History back");
    topsizer->Add(bmtmp = new wxBitmapButton(main_panel, ID_NAV_FORWARD, wxIcon(imgpath+"/arrow-right.png", wxBITMAP_TYPE_PNG) ), 0, wxBU_EXACTFIT|wxALL, 2);
    bmtmp->SetToolTip("History forward");
    topsizer->Add(bmtmp = new wxBitmapButton(main_panel, ID_NAV_TOP, wxIcon(imgpath+"/arrow-up-double.png", wxBITMAP_TYPE_PNG) ), 0, wxBU_EXACTFIT|wxALL, 2);
    bmtmp->SetToolTip("Top of page");
    topsizer->Add(bmtmp = new wxBitmapButton(main_panel, ID_NAV_NEXT_SECTION, wxIcon(imgpath+"/arrow-down.png", wxBITMAP_TYPE_PNG) ), 0, wxBU_EXACTFIT|wxALL, 2);
    bmtmp->SetToolTip("Next group");
    topsizer->Add(bmtmp = new wxBitmapButton(main_panel, ID_NAV_PREV_SECTION, wxIcon(imgpath+"/arrow-up.png", wxBITMAP_TYPE_PNG) ), 0, wxBU_EXACTFIT|wxALL, 2);
    bmtmp->SetToolTip("Previous group");
    topsizer->AddStretchSpacer();

    // topsizer->Add(new wxStaticText(main_panel, wxID_ANY, "Search..."), 0, wxALL|wxALIGN_BOTTOM, 2);
    topsizer->Add(m_searchtext, 0, wxALL|wxALIGN_BOTTOM, 2);
    topsizer->Add(new wxBitmapButton(main_panel, ID_SEARCHCLEAR, wxBITMAP_PNG_FROM_DATA(stock_cancel_20) ), 0, wxALL|wxALIGN_BOTTOM, 2);
    topsizer->Add(m_searchselect, 0, wxALL|wxALIGN_BOTTOM, 2);

    mainsizer->Add(topsizer, 0, wxEXPAND, 0);
    mainsizer->Add(m_html, 1, wxEXPAND | wxALL, 2);

    UpdateHelp("", "All");

    main_panel->SetSizer(mainsizer);

    wxBoxSizer *windowsizer = new wxBoxSizer(wxVERTICAL);
    windowsizer->Add(main_panel, 1, wxEXPAND, 0);
    this->SetSizer(windowsizer);

    this->Layout();

    m_searchtext->SetFocus();
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
    {
        m_searchtext->SetValue("");
        wxString filtertype = m_searchselect->GetValue();
        UpdateHelp("", filtertype.c_str());
        break;
    }
    case ID_NAV_BACK:
        m_html->HistoryBack();
        break;
    case ID_NAV_FORWARD:
        m_html->HistoryForward();
        break;
    case ID_NAV_NEXT_SECTION:
    case ID_NAV_PREV_SECTION:
    {
        wxString anchor = m_html->GetOpenedAnchor();
        int a_ind = std::find( m_anchor_list.begin(), m_anchor_list.end(), anchor.ToStdString().c_str() ) - m_anchor_list.begin();
        if ( evt.GetId() == ID_NAV_NEXT_SECTION )
        {
            if( a_ind > m_anchor_list.size()-2 )
                break;
            m_html->LoadPage("#"+m_anchor_list.at(a_ind+1) );
        }
        else
        {
            if( a_ind < 1 )
                break;
            m_html->LoadPage("#"+m_anchor_list.at(a_ind-1) );
        }
        
    }
        break;
    case ID_NAV_TOP:
        m_html->LoadPage("#top");
        break;
    }

}

void VariableDialog::UpdateHelp(const char* filter, const char* type)
{
    std::string filtered_text = "<html><head></head><body><a name=\"top\"></a>";
    std::string body_text;
    std::vector< std::string > all_group_names;
    wxString sfilter(filter);
    std::string stype(type);

    sfilter.MakeLower();        //lowercase
    
        
    //do search here and append doc items  -- "All", "Name", "Description", "Type"
    m_anchor_list.clear();
    m_anchor_list.push_back("top");

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
                wxString grouplab = groupname;
                grouplab.Replace(" ","");
                grouplab.Replace("|","_");

                group_formatted.append( wxString::Format(
                        "<a name=\"%s\"></a><table style=\"width:100%;background-color:#444;fontsize:+3;\">"
                        "<tr><td><font size=\"+4\" color=\"#fff\">%s</font></td></tr></table>",
                        grouplab.ToStdString().c_str(),
                    groupfirst.c_str() ).ToStdString() );

                all_group_names.push_back( groupname + "#" + grouplab.ToStdString());
                m_anchor_list.push_back( grouplab.ToStdString());
            }

            if (sfilter.empty())
            {
                filtered_group_items.append(v->doc.formatted_doc);
                continue;
            }

            wxString searchstring;

            if (stype == "Name")
            {
                searchstring = v->name;
            }
            else if (stype == "Description")
            {
                searchstring = v->doc.description;
            }
            else if (stype == "Type")
            {
                switch (v->type())
                {
                case data_base::NUMBER:
                    searchstring = wxString("number double float integer long");
                    break;
                case data_base::STRING:
                    searchstring = wxString("string character text");
                    break;
                case data_base::VECTOR:
                    searchstring = wxString("vector array matrix list");
                    break;
                case data_base::HASH:
                    searchstring = wxString("table hash map");
                    break;
                default:
                    break;
                }
            }
            else // (stype == "" || stype == "All")
            {
                searchstring = v->doc.formatted_doc;
            }
            
            //make sure all terms are included in the listed items
            wxArrayString sfilter_terms = wxSplit(sfilter, ' ');
            bool all_ok = true;
            for(int si=0; si<sfilter_terms.size(); si++)
            {
                if (searchstring.MakeLower().Find(sfilter_terms[si]) == wxNOT_FOUND)
                {
                    all_ok = false;
                    break;
                }
            }
            if( all_ok )
                filtered_group_items.append(v->doc.formatted_doc);
            
        }

        if ( ! filtered_group_items.empty() )
        {
            body_text.append(group_formatted);
            body_text.append(filtered_group_items);
        }
    }

    //always construct the header
    filtered_text.append("<table style=\"width:80%;background-color:#FF7926;padding:5px;border:2px;\"><tr><td><font size=\"+1\" color=\"#000\"><tr>");
    for(size_t i=0; i<all_group_names.size(); i++)
    {
        wxArrayString groupparse = wxSplit( all_group_names.at(i), '#');
        filtered_text.append( wxString::Format("<td><a href=\"#%s\">%s</a></td>",
                                groupparse[1].c_str(),
                                groupparse[0].c_str()
                            ).ToStdString() );

    }
    filtered_text.append("</tr></table>");

    //add the filtered body text
    filtered_text.append( body_text );

    filtered_text.append("</body></html>");

    m_html->SetPage(filtered_text);
    m_html->LoadPage("#"+m_anchor_list.front());
}

void VariableDialog::OnHtmlEvent(wxHtmlLinkEvent &evt)
{
    wxString link = evt.GetLinkInfo().GetHref();

    if( link.Find("id?") > -1 )
    {
        wxArrayString idparse = wxSplit(link.c_str(), '?') ;
        if( idparse.size() != 2 )
            return;
        wxString var = idparse.back();

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
    EVT_COMMAND_SET_FOCUS(ID_SEARCHTEXT, VariableDialog::OnCommand)
    EVT_COMMAND_KILL_FOCUS(ID_SEARCHTEXT, VariableDialog::OnCommand)
    EVT_TEXT(ID_SEARCHTEXT, VariableDialog::OnCommand)
    EVT_COMBOBOX(ID_SEARCHSELECT, VariableDialog::OnCommand)
    EVT_BUTTON(ID_SEARCHCLEAR, VariableDialog::OnCommand)
    EVT_BUTTON(ID_NAV_BACK, VariableDialog::OnCommand)
    EVT_BUTTON(ID_NAV_FORWARD, VariableDialog::OnCommand)
    EVT_BUTTON(ID_NAV_TOP, VariableDialog::OnCommand)
    EVT_BUTTON(ID_NAV_NEXT_SECTION, VariableDialog::OnCommand)
    EVT_BUTTON(ID_NAV_PREV_SECTION, VariableDialog::OnCommand)
    // EVT_HTML_LINK_CLICKED(ID_VAR_HELP, VariableDialog::OnHtmlEvent)
END_EVENT_TABLE()