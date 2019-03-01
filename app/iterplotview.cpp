#include <wx/wx.h>
#include <wx/clipbrd.h>
#include <wx/spinctrl.h>

#include "iterplotview.h"
#include "iterplot.h"
#include "project.h"

enum A {
    ID_SCROLL_UP = wxID_HIGHEST + 222,
    ID_SCROLL_DOWN,
    ID_NPLOT_UP,
    ID_NPLOT_DOWN,
    ID_NONE
};

BEGIN_EVENT_TABLE(IterPlotView, wxPanel)
EVT_BUTTON(ID_SCROLL_UP, IterPlotView::OnCommand)
EVT_BUTTON(ID_SCROLL_DOWN, IterPlotView::OnCommand)
EVT_BUTTON(ID_NPLOT_UP, IterPlotView::OnCommand)
EVT_BUTTON(ID_NPLOT_DOWN, IterPlotView::OnCommand)
END_EVENT_TABLE()


IterPlotView::IterPlotView(wxWindow *parent, Project* project, wxString imagedir)
    : wxPanel(parent)
{
    m_project_ptr = project;
    m_nplotview_max = 20;
    m_firstplot = 0;
    m_nplotview = 6;

    wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);

    wxBoxSizer *top_sizer = new wxBoxSizer(wxHORIZONTAL);
    
    wxBitmapButton *bmtmp;
    top_sizer->Add(bmtmp = new wxBitmapButton(this, ID_SCROLL_UP, wxIcon(imagedir + "/arrow-up.png", wxBITMAP_TYPE_PNG)), 0, wxBU_EXACTFIT | wxALL, 2);
    bmtmp->SetToolTip("Scroll plots up");
    top_sizer->Add(bmtmp = new wxBitmapButton(this, ID_SCROLL_DOWN, wxIcon(imagedir + "/arrow-down.png", wxBITMAP_TYPE_PNG)), 0, wxBU_EXACTFIT | wxALL, 2);
    bmtmp->SetToolTip("Scroll plots down");
    top_sizer->Add(bmtmp = new wxBitmapButton(this, ID_NPLOT_UP, wxIcon(imagedir + "/zoom-out.png", wxBITMAP_TYPE_PNG)), 0, wxBU_EXACTFIT | wxALL, 2);
    bmtmp->SetToolTip("View more plots");
    top_sizer->Add(bmtmp = new wxBitmapButton(this, ID_NPLOT_DOWN, wxIcon(imagedir + "/zoom-in.png", wxBITMAP_TYPE_PNG)), 0, wxBU_EXACTFIT | wxALL, 2);
    bmtmp->SetToolTip("View fewer plots");

    m_iterplot = new IterationPlot(this, &m_project_ptr->m_optimization_outputs.iteration_history.hash_vector);

    sizer->Add(top_sizer);
    sizer->Add(m_iterplot, 1, wxEXPAND | wxALL, 5);

    SetSizer(sizer);
}

void IterPlotView::OnCommand(wxCommandEvent &evt)
{
    switch (evt.GetId())
    {
    case ID_NONE:
        break;
    case ID_SCROLL_UP:
        m_firstplot--;
        m_firstplot < 0 ? 0 : m_firstplot;
        break;
    case ID_SCROLL_DOWN:
    {
        m_firstplot++;
        int nitem = m_project_ptr->m_optimization_outputs.iteration_history.hash_vector.item_count();
        m_firstplot > nitem - m_nplotview - 1 ? nitem - m_nplotview - 1 : m_firstplot;
        break;
    }
    case ID_NPLOT_UP:
        m_nplotview++;
        m_nplotview > m_nplotview_max - 2 ? m_nplotview_max - 1 : m_nplotview;
        break;
    case ID_NPLOT_DOWN:
        m_nplotview--;
        m_nplotview < 1 ? 1 : m_nplotview;
    default:
        break;
    }
}

void IterPlotView::UpdateDataFromProject()
{
    /* 
    Update the plot display based on the referenced project 
    */
    ordered_hash_vector somedat = m_project_ptr->m_optimization_outputs.iteration_history.hash_vector.slice(m_firstplot, m_firstplot + m_nplotview + 1);
       
    m_iterplot->SetData(&somedat);
}
