
#ifndef __iterplotview_h
#define __iterplotview_h

#include <wx/wx.h>

class IterPlotView : public wxPanel
{
	void OnCommand(wxCommandEvent &);

	DECLARE_EVENT_TABLE();
public:
    IterPlotView(wxWindow *parent);


};


#endif
