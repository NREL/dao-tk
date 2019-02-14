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

    //_plotobjs.resize(_data->item_count(), PlotBase());
    _plotobjs.resize(4, PlotBase());

    for (size_t i = 0; i < _plotobjs.size(); i++)
        _plotobjs.at(i).Create();

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
    wxMemoryDC memdc;

    int ssize_w = (int)this->GetSize().GetWidth();
    int ssize_h = (int)this->GetSize().GetHeight();

    //validation
    ssize_w = ssize_w < 100 ? 1024 : ssize_w;
    ssize_h = ssize_h < 100 ? 711 : ssize_h;

    //assign a bitmap to the DC
    _pbit.Create(ssize_w, ssize_h);
    memdc.SelectObject(_pbit);

    int nitem = (int)_plotobjs.size();

    wxSize parsize = this->GetClientSize();
    parsize.x = parsize.x < 100 ? 1024 : parsize.x;
    parsize.y = parsize.y < 100 ? 711 : parsize.y;
    parsize.y /= (int)(nitem > 8 ? 8 : (nitem == 0 ? 1 : nitem));
    
    for (size_t i = 0; i < nitem; i++)
    {

        _plotobjs.at(i).SetPlotSize(parsize);
        
        _plotobjs.at(i).SetXAxisRange(0, _data->iteration_count() + 1);
        _plotobjs.at(i).SetImagePositionOffset({ 0., (double)(parsize.y*i)});
        //max and min, use the objective
        std::vector<double>* ppa = _data->has_item("ppa");
        if (ppa)
        {
            //std::pair<const std::vector<double>::iterator, const std::vector<double>::iterator> mm = std::minmax(ppa->begin(), ppa->end());
            auto mm = std::minmax(ppa->begin(), ppa->end());
            _plotobjs.at(i).AxesSetup(memdc, *mm.first, *mm.second);
        }   
        else
            _plotobjs.at(i).AxesSetup(memdc, 0., 1.);

        //wxColour gray("#d4d4d4");
    }

        
   //do this last
    memdc.SelectObject(wxNullBitmap);
    _pdc.DrawBitmap(_pbit, 0, 0);
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