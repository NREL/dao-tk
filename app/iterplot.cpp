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
    SetData(data);
}

void IterationPlot::SetData(ordered_hash_vector* data)
{
    _data = data;
    if (_data->item_count() == 0)
        return;


    _plotobjs.resize(_data->item_count(), PlotBase());
    //_plotobjs.resize(4, PlotBase());

    for (size_t i = 0; i < _plotobjs.size(); i++)
        _plotobjs.at(i).Create();
    Update();
    Refresh();
}

void IterationPlot::OnPaint(wxPaintEvent &event)
{
    //Use a memory DC with a bitmap, and catch the wxEraseBackground event to prevent flicker

    wxPaintDC _pdc(this);
    DoPaint(_pdc);
    wxYieldIfNeeded();
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
        

        //std::vector<double>* ppa = _data->has_item("ppa");
        svd_pair datpair = _data->at_index(i);
        double datmin = 9e9;
        double datmax = -9e9;
        for (size_t j = 0; j < datpair.second.size(); j++)
        {
            datmin = datpair.second.at(j) < datmin ? datpair.second.at(j) : datmin;
            datmax = datpair.second.at(j) > datmax ? datpair.second.at(j) : datmax;
        }
        //max and min
        datmin = datmin - (datmax - datmin)*.05;
        datmax = datmax + (datmax - datmin)*.05;
        if (datmin == datmax)
        {
            datmax = datmin + 1.;
        }
        //axes limits
        _plotobjs.at(i).SetXAxisRange(0, _data->iteration_count() + 1);
        _plotobjs.at(i).SetXAxisRange(0, datpair.second.size() + 1);
        _plotobjs.at(i).SetYAxisRange(datmin, datmax);
        //plot positioning
        _plotobjs.at(i).SetImagePositionOffset({ 0., (double)(parsize.y*i)});
        _plotobjs.at(i).AxesSetup(memdc, datmin, datmax);
        //draw the data
        _plotobjs.at(i).DrawSeries(memdc, datpair.second, datpair.first );

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