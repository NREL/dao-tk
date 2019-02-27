#include <wx/wx.h>
#include <wx/clipbrd.h>

#include "iterplotview.h"
#include "iterplot.h"
#include "project.h"

enum A {
    ID_REFRESH = wxID_HIGHEST + 222,
    ID_CHECK_ALL,
    ID_UNCHECK_ALL,
    ID_NORMALIZE,
    ID_NONE
};

BEGIN_EVENT_TABLE(IterPlotView, wxPanel)
EVT_BUTTON(ID_REFRESH, IterPlotView::OnCommand)
EVT_BUTTON(ID_CHECK_ALL, IterPlotView::OnCommand)
EVT_BUTTON(ID_UNCHECK_ALL, IterPlotView::OnCommand)
EVT_CHECKBOX(ID_NORMALIZE, IterPlotView::OnCommand)
END_EVENT_TABLE()


IterPlotView::IterPlotView(wxWindow *parent, Project* project)
    : wxPanel(parent)
{
    m_project_ptr = project;

    wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);

    wxBoxSizer *top_sizer = new wxBoxSizer(wxHORIZONTAL);

    top_sizer->Add(new wxButton(this, ID_REFRESH, "Refresh data", wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT), 0, wxALL | wxEXPAND, 2);
    top_sizer->Add(new wxButton(this, ID_CHECK_ALL, "Check all", wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT), 0, wxALL | wxEXPAND, 2);
    top_sizer->Add(new wxButton(this, ID_UNCHECK_ALL, "Uncheck all", wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT), 0, wxALL | wxEXPAND, 2);
    top_sizer->Add(new wxCheckBox(this, ID_NORMALIZE, "Normalize plots"), 0, wxALL | wxEXPAND, 2);

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
    default:
        break;
    }
}

void IterPlotView::UpdateDataFromProject()
{
    /* 
    Update the plot display based on the referenced project 
    */
    
    m_iterplot->SetData(&m_project_ptr->m_optimization_outputs.iteration_history.hash_vector);

}