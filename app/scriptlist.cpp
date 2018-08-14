#include "scriptlist.h"
#include "daotk_app.h"
#include "scriptview.h"

#include <wx/listctrl.h>
#include <wx/imaglist.h>
#include <wx/filename.h>
#include <wx/splitter.h>

#include <wex/icons/down_arrow_gray_13.cpng>
#include <wex/icons/up_arrow_gray_13.cpng>
#include <wex/icons/stock_add_16.cpng>
#include <wex/icons/stock_remove_16.cpng>
#include <wex/icons/stock_yes_20.cpng>
#include <wex/icons/stock_no_20.cpng>
#include <wex/icons/stock_exec_16.cpng>
#include <wex/icons/qmark.cpng>

#ifdef __WXOSX__
#define FONTSIZE 13
#else
#define FONTSIZE 9
#endif


myDateTime myDateTime::Today()
{
    myDateTime D;
    D.ParseDateTime( wxDateTime::Today().FormatISOCombined() );

    return D;
}

wxString myDateTime::GetDisplayDate()
{
    if( this->IsLaterThan( wxDateTime::Today().ResetTime() ) )
        return this->FormatTime();
    else
        return this->FormatDate();
}

bool myDateTime::SetFromString(const std::string datestr )
{
    myDateTime D;
    D.ParseDateTime( datestr );
    if( D.IsValid() )
        (*this) = D;
    return D.IsValid();
}

ScriptList::ScriptList(wxSplitterWindow *parent, wxFileName *imagedir)
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize)
{

    m_parent = parent;

    m_imagelist = new wxImageList(16,16); //create and add image list to listctrl before adding images
        
    m_list = new wxListCtrl(this, ID_SCRIPT_LIST, wxDefaultPosition, wxDefaultSize, wxLC_LIST|wxLC_SINGLE_SEL);
    m_list->SetFont( wxFont(FONTSIZE, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL) );
    m_list->SetImageList( m_imagelist, wxIMAGE_LIST_SMALL);

    m_imagelist->Add( wxIcon( imagedir->GetPath() + "/edit.png", wxBITMAP_TYPE_PNG ) ); //ID_IMG::NONE
    m_imagelist->Add( wxIcon( imagedir->GetPath() + "/pass.png", wxBITMAP_TYPE_PNG ) ); //ID_IMG::OK
    m_imagelist->Add( wxIcon( imagedir->GetPath() + "/fail.png", wxBITMAP_TYPE_PNG ) );  //ID_IMG::FAILED
    m_imagelist->Add( wxIcon( imagedir->GetPath() + "/busy.png", wxBITMAP_TYPE_PNG ) );  //ID_IMG::WORKING

    wxBoxSizer *main = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer *button = new wxBoxSizer(wxHORIZONTAL);

    wxSize bsize = this->GetTextExtent("Add");
    bsize.Set( bsize.GetWidth() + 15, bsize.GetHeight() + 10);

    button->Add( new wxBitmapButton(this, ID_SCRIPT_ADD, wxBITMAP_PNG_FROM_DATA(stock_add_16), wxDefaultPosition, bsize), 0, wxALL, 1);
    button->Add( new wxBitmapButton(this, ID_SCRIPT_DEL, wxBITMAP_PNG_FROM_DATA(stock_remove_16), wxDefaultPosition, bsize), 0, wxALL, 1);
    button->Add( new wxBitmapButton(this, ID_SCRIPT_UP, wxBITMAP_PNG_FROM_DATA(up_arrow_gray_13), wxDefaultPosition, bsize), 0, wxALL, 1);
    button->Add( new wxBitmapButton(this, ID_SCRIPT_DOWN, wxBITMAP_PNG_FROM_DATA(down_arrow_gray_13), wxDefaultPosition, bsize), 0, wxALL, 1);
    button->AddStretchSpacer();

    main->Add( button );
    main->Add(m_list, 1, wxEXPAND|wxALL, 4);

    this->SetSizer(main);

    return;
}

void ScriptList::UpdateScriptList()
{
    m_list->DeleteAllItems();

    m_list->SetItemCount((long)m_scripts.size());

    wxSize maxextent(-1,-1);
    for(int i=0; i<(int)m_scripts.size(); i++)
    {

        int index = m_list->InsertItem((long)i, (int)( m_scripts.at(i).status) );

        wxArrayString spath = wxSplit(m_scripts.at(i).path, '/');
        int psize = spath.Last().size();

        int pmax=25, dmax=8;

        wxString truncpath = m_scripts.at(i).path;
        if(psize < pmax-1)
            truncpath = ".." + truncpath.SubString(truncpath.length()-(pmax-2), truncpath.length() );
        else if( truncpath.length() > pmax )
            truncpath = truncpath.SubString(0,pmax-2) + "..";

        wxString prettypath = wxString::Format( 
            wxString::Format("%s-%ds%s-3s%s-%ds%s-2s%s-%ds", "%", pmax, "%", "%", dmax, "%", "%", dmax),
                                truncpath.c_str(),
                                "",
                                m_scripts.at(i).modified.GetDisplayDate().c_str(),
                                "",
                                m_scripts.at(i).created.GetDisplayDate().c_str()
                             );

        m_list->SetItemText(index, prettypath);
        
        wxSize thisextent = m_list->GetTextExtent(prettypath);
        if(thisextent.GetWidth() > maxextent.GetWidth())
            maxextent = thisextent;
    }
    m_parent->SetSashPosition(maxextent.GetWidth(), true);
}

void ScriptList::Add()
{
    /* 
    Adds a new path to the script list
    */
   wxFileDialog dlg(this, "Add Script", wxEmptyString, wxEmptyString,
		"LK Script Files (*.lk)|*.lk",
		wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_CHANGE_DIR);

	if (dlg.ShowModal() == wxID_OK)
    {
        wxStructStat structstat;
        wxStat( dlg.GetPath(), &structstat);
        

        ScriptListObject S;
        S.path = dlg.GetPath();
        S.modified.ParseISOCombined( wxDateTime(structstat.st_mtime).FormatISOCombined() );
        S.created.ParseISOCombined( wxDateTime(structstat.st_ctime).FormatISOCombined() );
        S.status = ScriptList::ID_IMG::NONE;

        m_scripts.push_back( S );

        UpdateScriptList();
    }
}

void ScriptList::Delete(size_t sel)
{
    m_list->DeleteItem( sel );
    m_scripts.erase( m_scripts.begin() + sel );

    UpdateScriptList();
}

void ScriptList::SortById()
{

}

void ScriptList::MoveItemInList(size_t sel, bool is_up)
{

}

void ScriptList::OnScriptSelected( wxListEvent & )
{
    long index = m_list->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);

    if( index != wxNOT_FOUND )
    {
        if( MainWindow::Instance().GetScriptViewForm()->CloseDoc() )
            MainWindow::Instance().GetScriptViewForm()->Load( m_scripts.at(index).path);
    }

}

void ScriptList::OnCommand( wxCommandEvent &evt )
{
    int sel=-1;
    sel = m_list->GetNextItem(sel, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED); 
    if( sel < -1 )
        return;

    switch( evt.GetId() )
    {
        case ID_SCRIPT_ADD:
            Add();
        break;
        case ID_SCRIPT_DEL:
            Delete(sel);
            break;
        case ID_SCRIPT_UP:
        case ID_SCRIPT_DOWN:
            MoveItemInList( sel, evt.GetId() == ID_SCRIPT_UP );
            break;
        default:
        break;
    }
}

std::vector< ScriptListObject >* ScriptList::GetList()
{
    return &m_scripts;
}

BEGIN_EVENT_TABLE(ScriptList, wxPanel)
    EVT_LIST_ITEM_SELECTED(ID_SCRIPT_LIST, ScriptList::OnScriptSelected )
    EVT_BUTTON(ID_SCRIPT_ADD, ScriptList::OnCommand )
    EVT_BUTTON(ID_SCRIPT_DEL, ScriptList::OnCommand )
    EVT_BUTTON(ID_SCRIPT_UP, ScriptList::OnCommand )
    EVT_BUTTON(ID_SCRIPT_DOWN, ScriptList::OnCommand )
END_EVENT_TABLE()