#ifndef __scriptlist_h
#define __scriptlist_h

#include <wx/wx.h>
#include <wx/datetime.h>
#include <lk/absyn.h>
#include <lk/env.h>

#include <vector>
#include <string>

class wxListCtrl;
class wxListEvent;
class wxImageList;
class wxFileName;
class wxSplitterWindow;

class myDateTime : public wxDateTime
{
public:
    bool SetFromString(const std::string datstr);
    wxString GetDisplayDate(); 
    static myDateTime Today();
};

struct ScriptListObject
{
    std::string path;
    myDateTime modified;
    myDateTime created;
    unsigned char status;   //see ID_INT struct in ScriptList for options

    ScriptListObject()
    {
        path.clear();
        modified.SetToCurrent();
        created.SetToCurrent();
        status = 0;
    };
};

enum {ID_SCRIPT_LIST=wxID_HIGHEST+1, ID_SCRIPT_UP, ID_SCRIPT_DOWN, ID_SCRIPT_DEL, ID_SCRIPT_ADD};

class ScriptList : public wxPanel
{
    wxListCtrl *m_list;
    std::vector< ScriptListObject > m_scripts;
    wxImageList *m_imagelist;
    wxSplitterWindow *m_parent;

    void OnScriptSelected( wxListEvent &);
    void OnCommand( wxCommandEvent &);

public:

    struct ID_IMG { enum A {NONE=0, OK, FAIL, WORKING}; };
    
    ScriptList(wxSplitterWindow *parent, wxFileName *imagedir);

    void Add();
    void Delete(size_t sel);
    void SortById();
    void MoveItemInList(size_t sel, bool is_up);
    void UpdateScriptList();
    

    std::vector< ScriptListObject >* GetList();

    DECLARE_EVENT_TABLE()
};



#endif