
#ifndef __iterplotview_h
#define __iterplotview_h

#include <wx/wx.h>

class IterationPlot;
class Project;

class IterPlotView : public wxPanel
{
	void OnCommand(wxCommandEvent &);

    IterationPlot* m_iterplot;
    Project* m_project_ptr;
    int m_firstplot;
    int m_nplotview;
    int m_nplotview_max;

	DECLARE_EVENT_TABLE();
public:
    IterPlotView(wxWindow *parent, Project* project, wxString imagedir);
    void UpdateDataFromProject();

};


#endif
