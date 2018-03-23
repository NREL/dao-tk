
#ifndef __scriptview_h
#define __scriptview_h

#include <wx/wx.h>

class ScriptView : public wxPanel
{
public:
	ScriptView(wxWindow *parent);
	virtual ~ScriptView();

private:

	void OnCommand(wxCommandEvent &);

	DECLARE_EVENT_TABLE();
};


#endif
