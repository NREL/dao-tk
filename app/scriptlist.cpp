#include "scriptlist.h"

#include <wx/listctrl.h>
#include <wx/imaglist.h>
#include <wex/icons/down_arrow_gray_13.cpng>
#include <wex/icons/up_arrow_gray_13.cpng>
#include <wex/icons/stock_add_16.cpng>
#include <wex/icons/stock_remove_16.cpng>
#include <wex/icons/stock_yes_20.cpng>
#include <wex/icons/stock_no_20.cpng>


ScriptList::ScriptList(wxWindow *parent)
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize)
{

    wxImageList imlist;
    imlist.Add( wxBITMAP_PNG_FROM_DATA(stock_yes_20) );
    imlist.Add( wxBITMAP_PNG_FROM_DATA(stock_no_20) );
    
    m_list = new wxListCtrl(this, ID_SCRIPT_LIST, wxDefaultPosition, wxDefaultSize, wxLC_REPORT|wxLC_SMALL_ICON);
    m_list->SetImageList( &imlist, wxIMAGE_LIST_NORMAL);

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

    
    m_list->AppendColumn( "File" );
    m_list->AppendColumn( "Modified" );

    // long itemid = m_list->InsertItem(0, 0);
    // m_list->SetItem(itemid, 0, "A new file");
    // m_list->SetItem(itemid, 1, "89/12");

    // itemid = m_list->InsertItem(1, 1);
    // m_list->SetItem(itemid, 0, "An old file");
    // m_list->SetItem(itemid, 1, "2587");

    return;
}

void ScriptList::Add(const char *scriptpath)
{
    /* 
    Adds a new path to the script list
    */
}

void ScriptList::Delete(size_t sel)
{

}

void ScriptList::SortById()
{

}

void ScriptList::MoveItemInList(size_t sel, bool is_up)
{

}

void ScriptList::OnScriptSelected( wxListEvent &evt )
{
    int ind = evt.GetSelection();
}

void ScriptList::OnCommand( wxCommandEvent &evt )
{
    switch( evt.GetId() )
    {
        case ID_SCRIPT_ADD:
        break;
        case ID_SCRIPT_DEL:
        break;
        case ID_SCRIPT_UP:
        break;
        case ID_SCRIPT_DOWN:
        break;
        default:
        break;
    }
}

BEGIN_EVENT_TABLE(ScriptList, wxPanel)
    EVT_LIST_ITEM_SELECTED(ID_SCRIPT_LIST, ScriptList::OnScriptSelected )
    EVT_BUTTON(ID_SCRIPT_ADD, ScriptList::OnCommand )
    EVT_BUTTON(ID_SCRIPT_DEL, ScriptList::OnCommand )
    EVT_BUTTON(ID_SCRIPT_UP, ScriptList::OnCommand )
    EVT_BUTTON(ID_SCRIPT_DOWN, ScriptList::OnCommand )
END_EVENT_TABLE()