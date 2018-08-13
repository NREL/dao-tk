#ifndef __scriptlist_h
#define __scriptlist_h

#include <wx/wx.h>
#include <lk/absyn.h>
#include <lk/env.h>

#include <vector>
#include <string>

class wxListCtrl;
class wxListEvent;

struct ScriptListObject
{
    std::string path;
    int id;

    ScriptListObject()
    {
        path.clear();
        id=-1;
    };
};

enum {ID_SCRIPT_LIST=wxID_HIGHEST+1, ID_SCRIPT_UP, ID_SCRIPT_DOWN, ID_SCRIPT_DEL, ID_SCRIPT_ADD};

class ScriptList : public wxPanel
{
    wxListCtrl *m_list;
    std::vector< ScriptListObject > m_scripts;

    void OnScriptSelected( wxListEvent &);
    void OnCommand( wxCommandEvent &);

public:

    ScriptList(wxWindow *parent);

    void Add(const char *scriptpath);
    void Delete(size_t sel);
    void SortById();
    void MoveItemInList(size_t sel, bool is_up);

    DECLARE_EVENT_TABLE()
};



#endif