
#ifndef __logview_h
#define __logview_h

#include <wx/wx.h>

class LogView : public wxPanel
{
public:
	LogView(wxWindow *parent);
	virtual ~LogView();

	void Log(const wxString &text, bool wnl);
	void ClearLog();

private:

	wxTextCtrl * m_TextLog;

	void OnCommand(wxCommandEvent &);

	DECLARE_EVENT_TABLE();
};


#endif
