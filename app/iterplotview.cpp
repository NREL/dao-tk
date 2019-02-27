#include <wx/wx.h>
#include <wx/clipbrd.h>
#include <wx/spinctrl.h>

#include "iterplotview.h"
#include "iterplot.h"
#include "project.h"

enum A {
    ID_REFRESH = wxID_HIGHEST + 222,
    ID_CHECK_ALL,
    ID_UNCHECK_ALL,
    ID_NORMALIZE,
    ID_SCROLL_UP,
    ID_SCROLL_DOWN,
    ID_NPLOT_UP,
    ID_NPLOT_DOWN,
    ID_NONE
};

BEGIN_EVENT_TABLE(IterPlotView, wxPanel)
EVT_BUTTON(ID_REFRESH, IterPlotView::OnCommand)
EVT_BUTTON(ID_CHECK_ALL, IterPlotView::OnCommand)
EVT_BUTTON(ID_UNCHECK_ALL, IterPlotView::OnCommand)
EVT_BUTTON(ID_SCROLL_UP, IterPlotView::OnCommand)
EVT_BUTTON(ID_SCROLL_DOWN, IterPlotView::OnCommand)
EVT_BUTTON(ID_NPLOT_UP, IterPlotView::OnCommand)
EVT_BUTTON(ID_NPLOT_DOWN, IterPlotView::OnCommand)
EVT_CHECKBOX(ID_NORMALIZE, IterPlotView::OnCommand)
END_EVENT_TABLE()


IterPlotView::IterPlotView(wxWindow *parent, Project* project)
    : wxPanel(parent)
{
    m_project_ptr = project;
    m_nplotview_max = 20;
    m_firstplot = 0;
    m_nplotview = 6;

    wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);

    wxBoxSizer *top_sizer = new wxBoxSizer(wxHORIZONTAL);

    top_sizer->Add(new wxButton(this, ID_REFRESH, "Refresh data", wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT), 0, wxALL | wxEXPAND, 2);
    top_sizer->Add(new wxButton(this, ID_CHECK_ALL, "Check all", wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT), 0, wxALL | wxEXPAND, 2);
    top_sizer->Add(new wxButton(this, ID_UNCHECK_ALL, "Uncheck all", wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT), 0, wxALL | wxEXPAND, 2);
    top_sizer->Add(new wxCheckBox(this, ID_NORMALIZE, "Normalize plots"), 0, wxALL | wxEXPAND, 2);
    top_sizer->Add(new wxButton(this, ID_SCROLL_UP, "^", wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT), 0, wxALL | wxEXPAND, 2);
    top_sizer->Add(new wxButton(this, ID_SCROLL_DOWN, "v", wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT), 0, wxALL | wxEXPAND, 2);
    top_sizer->Add(new wxButton(this, ID_NPLOT_UP, "+", wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT), 0, wxALL | wxEXPAND, 2);
    top_sizer->Add(new wxButton(this, ID_NPLOT_DOWN, "-", wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT), 0, wxALL | wxEXPAND, 2);

    m_iterplot = new IterationPlot(this, &m_project_ptr->m_optimization_outputs.iteration_history.hash_vector);

    sizer->Add(top_sizer);
    sizer->Add(m_iterplot, 1, wxEXPAND | wxALL, 5);

    SetSizer(sizer);
}

void IterPlotView::OnCommand(wxCommandEvent &evt)
{
    switch (evt.GetId())
    {
    case ID_REFRESH:
        break;
    case ID_CHECK_ALL:
        break;
    case ID_UNCHECK_ALL:
        break;
    case ID_NORMALIZE:
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
