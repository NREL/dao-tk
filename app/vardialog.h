#ifndef _VAR_DIALOG
#define _VAR_DIALOG

#include "project.h"

#include <wx/wx.h>

#include <vector>
#include <string>

class wxComboBox;
class wxTextCtrl;
class wxHtmlWindow;

class VariableDialog : public wxFrame
{
    
    wxTextCtrl *m_searchtext;
    wxComboBox *m_searchselect;
    wxHtmlWindow *m_html;
    lk::varhash_t *m_variable_data;


public:
    VariableDialog(wxWindow *parent, lk::varhash_t* variable_data, int id = -1, long style = wxDEFAULT_FRAME_STYLE,
                wxSize size=wxDefaultSize, wxPoint position=wxDefaultPosition);

    void OnCommand(wxCommandEvent &);

    void UpdateHelp(const char* filter, const char* type);


    DECLARE_EVENT_TABLE()
};


#endif
