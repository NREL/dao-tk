
#ifndef __iterplot_h
#define __iterplot_h 1

#include <wx/wx.h>
#include "plot_base.h"
#include "project.h"

class IterationPlot : public wxScrolledWindow
{
protected:
    
    PlotBase _plotobj;
    wxBitmap _pbit; //bitmap containing the current plot
    
    ordered_hash_vector* _data;

	void OnCommand(wxCommandEvent &);
    void OnPaint(wxPaintEvent &event);
    void OnEraseBackground(wxEraseEvent &evt);

	DECLARE_EVENT_TABLE()

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
