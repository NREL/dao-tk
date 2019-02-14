#include <wx/wx.h>
#include <wx/dc.h>
#include <wx/dcgraph.h>
#include <wx/dcbuffer.h>

#include "iterplot.h"
#include "plot_base.h"

BEGIN_EVENT_TABLE(IterationPlot, wxScrolledWindow)
    EVT_PAINT(IterationPlot::OnPaint)
    EVT_ERASE_BACKGROUND(IterationPlot::OnEraseBackground)
END_EVENT_TABLE()

IterationPlot::IterationPlot(wxPanel *parent, ordered_hash_vector* data, const wxWindowID id,
    const wxPoint pos, const wxSize size, long style)
    : wxScrolledWindow(parent, id, pos, size, style)
{
    _data = data;
    _plotobj.Create();

}

void IterationPlot::OnPaint(wxPaintEvent &event)
{
    //Use a memory DC with a bitmap, and catch the wxEraseBackground event to prevent flicker

    wxPaintDC _pdc(this);
    DoPaint(_pdc);
    event.Skip();
}

void IterationPlot::DoPaint(wxDC &_pdc)
{

    //set up the canvas
    wxMemoryDC _dc;
    
    int ssize_w = (int)this->GetSize().GetWidth();
    int ssize_h = (int)this->GetSize().GetHeight();

    //validation
    ssize_w = ssize_w < 100 ? 1024 : ssize_w;
    ssize_h = ssize_h < 100 ? 711 : ssize_h;

    //assign a bitmap to the DC
    _pbit.Create(ssize_w, ssize_h);
    _dc.SelectObject(_pbit);

    wxGCDC gdc(_dc);

    _dc.SelectObject(wxNullBitmap);
    _pdc.DrawBitmap(_pbit, 0, 0);

    wxSize parsize = this->GetClientSize();
    parsize.x = parsize.x < 100 ? 1024 : parsize.x;
    parsize.y = parsize.y < 100 ? 711 : parsize.y;

    _plotobj.SetPlotSize(parsize);
}

void IterationPlot::OnEraseBackground(wxEraseEvent &) {}

void IterationPlot::OnCommand(wxCommandEvent &evt)
{
    switch (evt.GetId())
    {

    default:
        break;
    }

    return;
}