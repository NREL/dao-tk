
#ifndef __dataview_h
#define __dataview_h

#include <wx/wx.h>

class DataView : public wxPanel
{
public:
	DataView(wxWindow *parent);
	virtual ~DataView();

private:

	void OnCommand(wxCommandEvent &);

	DECLARE_EVENT_TABLE();
};


#endif
