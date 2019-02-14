
#ifndef __iterplot_h
#define __iterplot_h

#include <wx/wx.h>
#include "plot_base.h"
#include "project.h"

class IterationPlot : public wxScrolledWindow
{
protected:

	void OnCommand(wxCommandEvent &);

	//DECLARE_EVENT_TABLE();
    void OnPaint(wxPaintEvent &event);
    PlotBase _plotobj;
    wxBitmap _pbit; //bitmap containing the current plot

public:
    IterationPlot(wxPanel *parent,
        ordered_hash_vector* data,
        const wxWindowID id = wxID_ANY,
        const wxPoint pos = wxDefaultPosition,
        const wxSize size = wxDefaultSize,
        long style = wxHSCROLL | wxVSCROLL | wxFULL_REPAINT_ON_RESIZE | wxBG_STYLE_CUSTOM);
    void DoPaint(wxDC &_pdc);
};


#endif
