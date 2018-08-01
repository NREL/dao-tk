/*******************************************************************************************************
*  Copyright 2017 Alliance for Sustainable Energy, LLC
*
*  NOTICE: This software was developed at least in part by Alliance for Sustainable Energy, LLC
*  (�Alliance�) under Contract No. DE-AC36-08GO28308 with the U.S. Department of Energy and the U.S.
*  The Government retains for itself and others acting on its behalf a nonexclusive, paid-up,
*  irrevocable worldwide license in the software to reproduce, prepare derivative works, distribute
*  copies to the public, perform publicly and display publicly, and to permit others to do so.
*
*  Redistribution and use in source and binary forms, with or without modification, are permitted
*  provided that the following conditions are met:
*
*  1. Redistributions of source code must retain the above copyright notice, the above government
*  rights notice, this list of conditions and the following disclaimer.
*
*  2. Redistributions in binary form must reproduce the above copyright notice, the above government
*  rights notice, this list of conditions and the following disclaimer in the documentation and/or
*  other materials provided with the distribution.
*
*  3. The entire corresponding source code of any redistribution, with or without modification, by a
*  research entity, including but not limited to any contracting manager/operator of a United States
*  National Laboratory, any institution of higher learning, and any non-profit organization, must be
*  made publicly available under this license for as long as the redistribution is made available by
*  the research entity.
*
*  4. Redistribution of this software, without modification, must refer to the software by the same
*  designation. Redistribution of a modified version of this software (i) may not refer to the modified
*  version by the same designation, or by any confusingly similar designation, and (ii) must refer to
*  the underlying software originally provided by Alliance as �System Advisor Model� or �SAM�. Except
*  to comply with the foregoing, the terms �System Advisor Model�, �SAM�, or any confusingly similar
*  designation may not be used to refer to any modified version of this software or any modified
*  version of the underlying software originally provided by Alliance without the prior written consent
*  of Alliance.
*
*  5. The name of the copyright holder, contributors, the United States Government, the United States
*  Department of Energy, or any of their employees may not be used to endorse or promote products
*  derived from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
*  IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
*  FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER,
*  CONTRIBUTORS, UNITED STATES GOVERNMENT OR UNITED STATES DEPARTMENT OF ENERGY, NOR ANY OF THEIR
*  EMPLOYEES, BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
*  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
*  IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
*  THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <algorithm>

#include <wx/wx.h>
#include <wx/mdi.h>
#include <wx/config.h>
#include <wx/busyinfo.h>
#include <wx/print.h>
#include <wx/printdlg.h>
#include <wx/scrolwin.h>
#include <wx/clipbrd.h>
#include <wx/busyinfo.h>
#include <wx/filename.h>
#include <wx/tokenzr.h>
#include <wx/statline.h>
#include <wx/tglbtn.h>
#include <wx/splitter.h>
#include <wx/grid.h>

#include <wex/plot/plplotctrl.h>
#include <wex/plot/pllineplot.h>
#include <wex/plot/plbarplot.h>
#include <wex/plot/plhistplot.h>
#include <wex/dview/dvplotctrl.h>
#include <wex/dview/dvtimeseriesdataset.h>
#include <wex/extgrid.h>
#include <wex/numeric.h>

#include "dataview.h"
#include "project.h"

#ifdef __WXOSX__
#define FONTSIZE 13
#else
#define FONTSIZE 10
#endif

class DataView::Table : public wxGridTableBase
{
public:

	Table()
	{
		m_vt_ref = 0;
		m_attr = new wxGridCellAttr;
		m_attr->SetBackgroundColour( wxColour( 240,240,240 ) );
		m_attr->SetTextColour( "navy" );
		m_attr->SetFont( wxFont(FONTSIZE, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL) );
	}

	virtual ~Table()
	{		
		m_attr->DecRef();
		m_vt_ref = 0;
	}

	

	
    virtual wxGridCellAttr *GetAttr(int , int col,
                                    wxGridCellAttr::wxAttrKind  )
	{
		if (col >= 0 && col < (int)m_items.Count())
		{
			if (!m_vt_ref) return NULL;
			lk::vardata_t *v = m_vt_ref->at( (const char*)m_items[col].c_str() );
			if (!v) return NULL;

			if (v->type() != lk::vardata_t::VECTOR) return NULL;

			m_attr->IncRef();
			return m_attr;
		}
		else
			return NULL;
	}

	void Detach()
	{
		m_vt_ref = 0;
	}
	
	virtual int GetNumberRows()
	{
		int max0 = 0;
		for (int i=0;i<(int)m_items.Count();i++)
		{
			if (!m_vt_ref) continue;
			lk::vardata_t *v = m_vt_ref->at( (const char*)m_items[i].c_str() );
			if (!v) continue;

			int len = 1;
			if (v->type() == lk::vardata_t::VECTOR) 
				if( v->vec()->empty() )
					len = 0;
				else
					if(v->vec()->front().type() == lk::vardata_t::VECTOR)
						len = v->vec()->front().vec()->size();
					else
						len = v->vec()->size();

			if (len > max0) max0 = len;
		}
		return max0;
	}

	virtual int GetNumberCols()
	{
		return m_items.Count();
	}

	virtual bool IsEmptyCell(int row, int col)
	{
		if ( col < 0 || col >= (int)m_items.Count() || row < 0 ) return true;
		
		if (!m_vt_ref) return true;
		lk::vardata_t *v = m_vt_ref->at( (const char*)m_items[col].c_str() );
		if (!v) return true;

		if ( v->type() == lk::vardata_t::STRING && row >= 1 ) return true;

		if (v->type() == lk::vardata_t::VECTOR) 
			if( row >= (int)v->vec()->size())
				return true;

		return false;
	}

	virtual wxString GetValue( int row, int col )
	{
		if (m_vt_ref && col >= 0 && col < (int)m_items.Count())
		{
			lk::vardata_t *v = m_vt_ref->at( (const char*)m_items[col].c_str() );
			if (!v) return "<lookup error>";

			if (v->type() == lk::vardata_t::STRING && row == 0) return wxString(v->as_string().c_str());
			
			else if (v->type() == lk::vardata_t::NUMBER && row == 0) return wxString::Format("%lf", v->as_number() );
			
			else if (v->type() == lk::vardata_t::VECTOR && row < (int)v->vec()->size())
			{
				if ( v->vec()->empty() )
					return "";

				if (
						v->type() == lk::vardata_t::VECTOR 
						&& v->vec()->front().type() != lk::vardata_t::VECTOR 
						&& row < (int)v->vec()->size() 
					)
					return wxString::Format("%lf", v->vec()->at(row).as_number() );
				
				else if ( v->vec()->front().type() == lk::vardata_t::VECTOR) 
				{
					wxString ret;
					for (int j=0;j<(int)v->vec()->front().vec()->size();j++)
					{
						ret += wxString::Format("%*lf", 13, v->vec()->at(row).vec()->at(j).as_number() );
					}

					return ret;
				}
			}

			// else if (v->type == SSC_TABLE && (unsigned int)row < v->table.size())
			// {
			// 	int k = 0;
			// 	const char *key = v->table.first();
			// 	while (key != 0)
			// 	{
			// 		if (k++ == row) break;
			// 		key = v->table.next();
			// 	}
			// 	return ".{'" + wxString(key) + "'}";
			// }
		}
		
		return wxEmptyString;
	}

	virtual wxString GetColLabelValue(int col)
	{
		if (col >= 0 && col < (int)m_items.Count())
		{

			if (!m_vt_ref) return m_items[col];
			else return m_items[col];
		}
		else
			return "<unknown>";
	}
	
	void SetData( const wxArrayString &items, lk::varhash_t *vt, bool )
	{
		m_items = items;
		m_vt_ref = vt;
	}

	virtual void SetValue(int,int,const wxString &)
	{
		/* nothing to do */
	}

private:
    wxGridCellAttr *m_attr;
	lk::varhash_t *m_vt_ref;
	wxArrayString m_items;
};



enum { ID_COPY_CLIPBOARD = 2315,
	   ID_LIST,
	   ID_SHOW_STATS,
	   ID_SELECT_ALL,
	   ID_UNSELECT_ALL,
	   ID_DVIEW,
	   ID_POPUP_STATS,
	   ID_POPUP_PLOT_BAR,
	   ID_POPUP_PLOT_LINE,
	   ID_GRID };

BEGIN_EVENT_TABLE( DataView, wxPanel )
	EVT_BUTTON( ID_COPY_CLIPBOARD, DataView::OnCommand )
	EVT_BUTTON( ID_UNSELECT_ALL, DataView::OnCommand )
	EVT_BUTTON( ID_SELECT_ALL, DataView::OnCommand )
	EVT_BUTTON( ID_UNSELECT_ALL, DataView::OnCommand )
	EVT_BUTTON( ID_SHOW_STATS, DataView::OnCommand )
	EVT_BUTTON( ID_DVIEW, DataView::OnCommand )
	EVT_CHECKLISTBOX( ID_LIST, DataView::OnVarListCheck )
	EVT_LISTBOX_DCLICK( ID_LIST, DataView::OnVarListDClick )
	EVT_GRID_CMD_LABEL_RIGHT_CLICK( ID_GRID, DataView::OnGridLabelRightClick )
	EVT_GRID_CMD_LABEL_LEFT_DCLICK( ID_GRID, DataView::OnGridLabelDoubleClick )
	
	EVT_MENU( ID_POPUP_STATS, DataView::OnPopup )
	EVT_MENU( ID_POPUP_PLOT_BAR, DataView::OnPopup )
	EVT_MENU( ID_POPUP_PLOT_LINE, DataView::OnPopup )

END_EVENT_TABLE()


DataView::DataView( wxWindow *parent ) 
	: wxPanel( parent ),
  	m_frozen(false),
	m_grid_table(0),
	m_root_item(0),
	m_vt(0)
{
	wxBoxSizer *tb_sizer = new wxBoxSizer(wxHORIZONTAL);
	tb_sizer->Add( new wxButton(this, ID_SELECT_ALL, "Select all", wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT), 0, wxALL|wxEXPAND, 2);
	tb_sizer->Add( new wxButton(this, ID_UNSELECT_ALL, "Unselect all", wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT), 0, wxALL|wxEXPAND, 2);
	tb_sizer->Add( new wxButton( this, ID_COPY_CLIPBOARD, "Copy to clipboard", wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT), 0, wxEXPAND|wxALL, 2);
	tb_sizer->Add( new wxButton( this, ID_SHOW_STATS, "Show stats...", wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT), 0, wxEXPAND|wxALL, 2);
	tb_sizer->Add( new wxButton( this, ID_DVIEW, "Timeseries graph...", wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT), 0, wxEXPAND|wxALL, 2);
	tb_sizer->AddStretchSpacer(1);

	wxSplitterWindow *splitwin = new wxSplitterWindow(this, wxID_ANY, 
		wxDefaultPosition, wxDefaultSize, wxSP_LIVE_UPDATE ); 
	splitwin->SetMinimumPaneSize(210);

	m_varlist = new wxCheckListBox( splitwin, ID_LIST );
	m_varlist->SetFont( wxFont(FONTSIZE, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL) );
	
	m_grid = new wxExtGridCtrl(splitwin, ID_GRID);
	m_grid->SetFont( wxFont(FONTSIZE, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL) );
	m_grid->EnableEditing(false);
	m_grid->EnableCopyPaste(false);
	m_grid->DisableDragCell();
	m_grid->DisableDragRowSize();
	m_grid->DisableDragColMove();
	m_grid->DisableDragGridSize();
	m_grid->SetDefaultCellAlignment( wxALIGN_RIGHT, wxALIGN_CENTER );
	m_grid->SetRowLabelAlignment( wxALIGN_LEFT, wxALIGN_CENTER );

	splitwin->SplitVertically(m_varlist, m_grid, 390);


	wxBoxSizer *szv_main = new wxBoxSizer(wxVERTICAL);
	szv_main->Add( tb_sizer, 0, wxALL|wxEXPAND, 2 );
	//szv_main->Add( new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL ), 0, wxALL|wxEXPAND, 1);
	szv_main->Add( splitwin, 1, wxALL|wxEXPAND, 0 );

	SetSizer( szv_main );

}	

std::vector<int> DataView::GetColumnWidths()
{
	std::vector<int> list;
	for (int i=0;i<m_grid->GetNumberCols();i++)
		list.push_back( m_grid->GetColSize( i ) );
	return list;
}

void DataView::SetColumnWidths( const std::vector<int> &cwl )
{
	for (int i=0;i<(int)cwl.size() && i<m_grid->GetNumberCols();i++)
		m_grid->SetColSize( i, cwl[i] );
}

wxArrayString DataView::GetSelections()
{
	return m_selections;
}

void DataView::SetSelections(const wxArrayString &sel)
{
	m_selections = sel;
	size_t i=0;
	while (i < m_selections.Count())
	{
		if ( m_names.Index( m_selections[i] ) == wxNOT_FOUND )
			m_selections.RemoveAt(i);
		else
		{
			i++;
		}
	}

	for (unsigned int idx=0;idx<m_names.Count();idx++)
		m_varlist->Check( idx, (m_selections.Index( m_names[idx] ) >= 0) );	
}

static void SortByLabels(wxArrayString &names, wxArrayString &labels)
{
	// sort the selections by labels
	wxString buf;
	int count = (int)labels.Count();
	for (int i=0;i<count-1;i++)
	{
		int smallest = i;

		for (int j=i+1;j<count;j++)
			if ( labels[j] < labels[smallest] )
				smallest = j;

		// swap
		buf = labels[i];
		labels[i] = labels[smallest];
		labels[smallest] = buf;

		buf = names[i];
		names[i] = names[smallest];
		names[smallest] = buf;

	}
}

void DataView::Freeze()
{
	m_frozen = true;	
}

void DataView::Thaw()
{
	m_frozen = false;
	UpdateView();
}

void DataView::UpdateView()
{
	if ( m_frozen ) return;

	wxArrayString sel_list = m_selections;

	m_names.Clear();

	m_varlist->Clear();

	if (m_vt != NULL)
	{
		int padto = 0;
		
		std::vector< lk_string > varnames;
		for(lk::varhash_t::iterator it = m_vt->begin(); it != m_vt->end(); it++ )
		{
			varnames.push_back( it->first );
			int len = (static_cast<data_base*>(it->second))->nice_name.length();
			if (len > padto) padto = len;
		}
		padto += 2;


		wxArrayString labels;
		for( int ni=0; ni<varnames.size(); ni++)
		{
			lk_string name = varnames.at(ni);


			if (lk::vardata_t *v = m_vt->at(name))
			{
				m_names.Add( name );
				wxString label = wxString::Format( "%" + wxString::Format("-%ds",padto) ,( static_cast<data_base*>(v) )->nice_name );

				// for (int j=0;j< padto-(int)label.length();j++)
				// 	label += " ";
				
				int labellen = label.length();

				label += wxString(v->typestr());
				if (v->type() == lk::vardata_t::NUMBER)
					label += " " + wxString::Format("%lg", v->as_number() );				
				else if (v->type() == lk::vardata_t::STRING)
					label += " " + wxString(v->as_string().c_str());
				else if (v->type() == lk::vardata_t::VECTOR)
					if( v->vec()->empty() )
						label += wxString::Format( " [%d]", (int)v->vec()->size() );
					else
						if( v->vec()->front().type() == lk::vardata_t::VECTOR)
							label += wxString::Format(" [%d,%d]", (int)v->vec()->size(), (int)v->vec()->front().vec()->size() );
						else
							label += wxString::Format( " [%d]", (int)v->vec()->size() );
   				labels.Add( label );
			}

		}

		m_varlist->Freeze();
		SortByLabels(m_names, labels );
		for (int i=0;i<(int)m_names.Count();i++)
		{
			int idx = m_varlist->Append( labels[i]);
			m_varlist->Check( idx, false );
		}
		m_varlist->Thaw();
	}
	
	SetSelections( sel_list );
	UpdateGrid();
}
	
void DataView::UpdateGrid()
{
	std::vector<int> cwl = GetColumnWidths();
	m_grid->Freeze();
	
	if (m_grid_table) m_grid_table->Detach();

	m_grid_table = new DataView::Table;
	m_grid_table->SetData( m_selections, m_vt, true );
	m_grid->SetTable( m_grid_table, true );
	m_grid->SetRowLabelSize(60);
	//m_grid->SetColLabelSize( wxGRID_AUTOSIZE );
	m_grid->Thaw();
	
	m_grid->Layout();
	m_grid->GetParent()->Layout();
	SetColumnWidths(cwl);
	m_grid->ForceRefresh();
//	m_grid->AutoSizeColumns();

}

void DataView::OnCommand(wxCommandEvent &evt)
{
	switch(evt.GetId())
	{
	case ID_SHOW_STATS:
		ShowStats();
		break;
	case ID_DVIEW:
		{
			wxDialog dlg(this, -1, "Timeseries Viewer", wxDefaultPosition, wxSize(900,600), wxRESIZE_BORDER|wxDEFAULT_DIALOG_STYLE);
			wxDVPlotCtrl *dv = new wxDVPlotCtrl( &dlg );
			wxBoxSizer *sz = new wxBoxSizer(wxVERTICAL);
			sz->Add( dv, 1, wxALL|wxEXPAND, 0 );
			sz->Add( dlg.CreateButtonSizer(wxOK), 0, wxALL|wxEXPAND, 0 );
			dlg.SetSizer(sz);
			
			std::vector<double> da(8760);
			int iadded = 0;
			for (size_t i=0;i<m_selections.Count();i++)
			{
				lk::vardata_t *v = m_vt->at( (const char*) m_selections[i].c_str() );
				if ( v != 0 
					&& v->type() == lk::vardata_t::VECTOR
					&& v->vec()->size() == 8760)
				{
					for (int k=0;k<8760;k++)
						da[k] = v->vec()->at(k).as_number();

					dv->AddDataSet(  new wxDVArrayDataSet( m_selections[i], da ) );
					iadded++;
				}
			}
			
			if (iadded == 0)
				wxMessageBox("Please check one or more array variables with 8760 values to show in the timeseries viewer.");
			else
			{
				dv->SelectDataOnBlankTabs();
				dlg.ShowModal();
			}

		}
		break;
	case ID_SELECT_ALL:
		{
			m_selections.Clear();
			for(lk::varhash_t::iterator it = m_vt->begin(); it != m_vt->end(); it++ )
				m_selections.Add( it->first );
			UpdateView();
		}
		break;
	case ID_UNSELECT_ALL:
		m_selections.Clear();
		UpdateView();
		break;
	case ID_COPY_CLIPBOARD:
		m_grid->Copy( m_grid->NumCellsSelected() == 1, true);
		break;
	}
}

void DataView::OnVarListCheck(wxCommandEvent &evt)
{
	int idx = evt.GetSelection();
	if (idx >= 0  && idx < (int)m_names.Count())
	{
		wxString var = m_names[idx];
		
		if (m_varlist->IsChecked(idx) && m_selections.Index(var) == wxNOT_FOUND)
			m_selections.Add( var );

		if (!m_varlist->IsChecked(idx) && m_selections.Index(var) != wxNOT_FOUND)
			m_selections.Remove( var );
	}

	UpdateGrid();
}

void DataView::OnVarListDClick(wxCommandEvent &)
{
	ShowStats( wxEmptyString );
}

wxString DataView::GetSelection()
{
	int n = m_varlist->GetSelection();
	if (n >= 0 && n < (int)m_names.Count())
		return m_names[n];
	else
		return wxEmptyString;
}

void DataView::ShowStats( wxString name )
{
	if (name.IsEmpty()) name = GetSelection();
	if (name.IsEmpty()) return;

	if (m_vt)
	{
		lk::vardata_t *v = m_vt->at((const char*) name.c_str() );
		if (!v || v->type() != lk::vardata_t::VECTOR)
		{
			wxMessageBox("variable not found or not of array type.");
			return;
		}

		StatDialog dlg(this, "Stats for: " + name);
		dlg.Compute( *v->vec() );
		dlg.ShowModal();
	}
}

void DataView::OnGridLabelRightClick(wxGridEvent &evt)
{
	int col = evt.GetCol();
	if (col < 0 || col >= (int)m_selections.Count()) return;
	
	m_popup_var_name = m_selections[col];

	wxMenu popup;
	popup.Append( ID_POPUP_STATS, "Statistics...");
	popup.Append( ID_POPUP_PLOT_BAR, "Bar plot (array only)" );
	popup.Append( ID_POPUP_PLOT_LINE, "Line plot (array only)" );

	m_grid->PopupMenu( &popup, evt.GetPosition() );
}

void DataView::OnGridLabelDoubleClick(wxGridEvent &evt)
{
	int col = evt.GetCol();
	if (col < 0 || col >= (int)m_selections.Count()) return;
	ShowStats( m_selections[col] );
}

void DataView::OnPopup(wxCommandEvent &evt)
{
	switch(evt.GetId())
	{
	case ID_POPUP_STATS:
		ShowStats( m_popup_var_name );
		break;
	case ID_POPUP_PLOT_BAR:
	case ID_POPUP_PLOT_LINE:
		{
			if (!m_vt) return;
			lk::vardata_t *v = m_vt->at( (const char*) m_popup_var_name.c_str() );
			if (!v || v->type() != lk::vardata_t::VECTOR)
			{
				wxMessageBox("variable not found or not of array type.");
				return;
			}

			wxFrame *frm = new wxFrame(this, -1, "plot: " + m_popup_var_name, wxDefaultPosition, wxSize(500,350));

			if ( v->vec()->size() == 8760 )
			{
				wxDVPlotCtrl *dv = new wxDVPlotCtrl( frm );
				std::vector<double> da(8760);
				for (int i=0;i<8760;i++)
					da[i] = v->vec()->at(i).as_number();

				dv->AddDataSet(  new wxDVArrayDataSet( m_popup_var_name, da ) );
			}
			else
			{
				wxPLPlotCtrl *plotsurf = new wxPLPlotCtrl( frm, wxID_ANY );
			
				double minval = 1e99;
				double maxval = 1e-99;
				std::vector<wxRealPoint> pdat;
				for (int i=0;i<(int)v->vec()->size(); i++)
				{
					double vali = v->vec()->at(i).as_number();

					pdat.push_back( wxRealPoint( i+1, vali ) );
					if ( vali < minval ) minval = vali;
					if ( vali > maxval ) maxval = vali;
				}

				
				if( evt.GetId() == ID_POPUP_PLOT_LINE )
				{
					double range = maxval-minval;
					plotsurf->AddPlot( new wxPLLinePlot( pdat, m_popup_var_name ) );
					minval -= (0.05*range);
					maxval += (0.05*range);
					plotsurf->SetYAxis1( new wxPLLinearAxis( minval, maxval ) );
				}
				else
				{
					plotsurf->AddPlot( new wxPLBarPlot( pdat, 0.0, m_popup_var_name ) );
					if ( minval > 0 ) minval = 0;
					if ( maxval < 0 ) maxval = 0;
					plotsurf->SetYAxis1( new wxPLLinearAxis( minval, maxval ) );
				}

				plotsurf->SetTitle("Plot of: '" + m_popup_var_name + "'");
				plotsurf->SetXAxis1( new wxPLLinearAxis( 0, v->vec()->size()+1 ) );

			}

			frm->Show();
		}
		break;
	}
}


#include <wex/numeric.h>
#include <wex/extgrid.h>

StatDialog::StatDialog( wxWindow *parent, const wxString &title )
	 : wxDialog( parent, wxID_ANY, title, 
		wxDefaultPosition, wxSize( 640,480), 
		wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER )
{
	wxBoxSizer *sz_h1 = new wxBoxSizer( wxHORIZONTAL );
	
	sz_h1->Add( new wxStaticText( this, wxID_ANY, "Mean:" ), 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 5 );
	sz_h1->Add( numMean = new wxNumericCtrl(this) );
	sz_h1->Add( new wxStaticText( this, wxID_ANY, "Min:" ), 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 5 );
	sz_h1->Add( numMin = new wxNumericCtrl(this) );
	sz_h1->Add( new wxStaticText( this, wxID_ANY, "Max:" ), 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 5 );
	sz_h1->Add( numMax = new wxNumericCtrl(this) );

	wxBoxSizer *sz_h2 = new wxBoxSizer( wxHORIZONTAL );

	sz_h2->Add( new wxStaticText( this, wxID_ANY, "Sum:" ), 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 5 );
	sz_h2->Add( numSum = new wxNumericCtrl(this) );
	sz_h2->Add( new wxStaticText( this, wxID_ANY, "Sum/1000:" ), 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 5 );
	sz_h2->Add( numSumOver1000 = new wxNumericCtrl(this) );
	
	grdMonthly = new wxExtGridCtrl(this, wxID_ANY);
	grdMonthly->CreateGrid(12,4);
	grdMonthly->EnableEditing(false);
	grdMonthly->DisableDragCell();
	grdMonthly->DisableDragColSize();
	grdMonthly->DisableDragRowSize();
	grdMonthly->DisableDragColMove();
	grdMonthly->DisableDragGridSize();
	grdMonthly->SetRowLabelSize(23);
	grdMonthly->SetColLabelSize(23);

	wxBoxSizer *sz_main = new wxBoxSizer( wxVERTICAL );
	sz_main->Add( sz_h1 );	
	sz_main->Add( sz_h2 );
	sz_main->Add( grdMonthly, 1, wxALL|wxEXPAND, 5 );
	sz_main->Add( CreateButtonSizer( wxOK ), 0, wxALL|wxEXPAND, 5 );

	SetSizer( sz_main );
}

void StatDialog::Compute( std::vector<lk::vardata_t> &val )
{
static int nday[12] = {31,28,31,30,31,30,31,31,30,31,30,31}; 

	size_t len = val.size();
	// double *pvals = val.data();

	double min, max, mean, sum;
	double mmin[12],mmax[12],mmean[12],msum[12];

	size_t i,j;
	min = (double)1e19;
	max = (double)-1e19;
	mean=sum=0.0;
	for (i=0;i<12;i++)
	{
		mmin[i]=(double)1e19;
		mmax[i]=(double)-1e19;
		mmean[i]=msum[i] = 0;
	}


	for (i=0;i<len;i++)
	{
		double vali = val.at(i).as_number();

		if (vali < min) min = vali;
		if (vali > max) max = vali;
		sum += vali;
	}

	mean = sum/((double)len);

	numMin->SetValue( min );
	numMax->SetValue( max );
	numMean->SetValue( mean );
	numSum->SetValue( sum );
	numSumOver1000->SetValue( sum/1000.0 );

	size_t multiple = len / 8760;
	if ( multiple*8760 == len )
	{
		i=0;
		for (int m=0;m<12;m++)
		{
			for (int d=0;d<nday[m];d++)
			{
				for (int h=0;h<24;h++)
				{
					double aval = 0.0;
					for (j=0;j<multiple;j++)
						aval += val[i*multiple+j].as_number();


					if (aval < mmin[m]) mmin[m] = aval;
					if (aval > mmax[m]) mmax[m] = aval;
					msum[m] += aval;

					i++;
				}
			}

			mmean[m] = msum[m] / ( nday[m]*24 );
		}
	}

	grdMonthly->ResizeGrid(12,5);
	for (i=0;i<12;i++)
	{
		grdMonthly->SetCellValue( i, 0, wxString::Format("%lg", mmin[i]  )  );
		grdMonthly->SetCellValue( i, 1, wxString::Format("%lg", mmax[i]  )  );
		grdMonthly->SetCellValue( i, 2, wxString::Format("%lg", mmean[i] )  );
		grdMonthly->SetCellValue( i, 3, wxString::Format("%lg", msum[i] ) );
		grdMonthly->SetCellValue( i, 4, wxString::Format("%lg", msum[i]/1000.0f ) );
	}
	
	grdMonthly->SetRowLabelValue(0, "Jan");
	grdMonthly->SetRowLabelValue(1, "Feb");
	grdMonthly->SetRowLabelValue(2, "Mar");
	grdMonthly->SetRowLabelValue(3, "Apr");
	grdMonthly->SetRowLabelValue(4, "May");
	grdMonthly->SetRowLabelValue(5, "Jun");
	grdMonthly->SetRowLabelValue(6, "Jul");
	grdMonthly->SetRowLabelValue(7, "Aug");
	grdMonthly->SetRowLabelValue(8, "Sep");
	grdMonthly->SetRowLabelValue(9, "Oct");
	grdMonthly->SetRowLabelValue(10, "Nov");
	grdMonthly->SetRowLabelValue(11, "Dec");

	grdMonthly->SetColLabelValue(0, "Min");
	grdMonthly->SetColLabelValue(1, "Max");
	grdMonthly->SetColLabelValue(2, "Mean");
	grdMonthly->SetColLabelValue(3, "Sum");
	grdMonthly->SetColLabelValue(4, "Sum/1000");

	grdMonthly->SetRowLabelSize( 40 );
	grdMonthly->SetColLabelSize( wxGRID_AUTOSIZE );
}

