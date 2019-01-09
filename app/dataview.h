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

#ifndef __dataview_h
#define __dataview_h

#include <vector>

#include <wx/panel.h>
#include <wx/checklst.h>
#include <wx/treebase.h>
#include <wx/grid.h>

#include <lk/env.h>
#include <ssc/sscapi.h>

class wxExtGridCtrl;
class VarTreeView;
class variable;

class DataView : public wxPanel
{
public:

	class Table; // forward

	DataView( wxWindow *parent, const char* imagedir );
	virtual ~DataView() { m_vt = NULL; }

	void SetDataObject( lk::varhash_t *vt ) { m_vt = vt; UpdateView(); }
	ssc_data_t GetDataObject() { return m_vt; }

	void UpdateView();	
	void UpdateGrid();
	virtual void Freeze();
	virtual void Thaw();


	std::vector<int> GetColumnWidths();
	void SetColumnWidths( const std::vector<int> &cwl );
	wxArrayString GetSelections();
	void SetSelections(const wxArrayString &sel, const wxArrayString &labels);

	wxString GetSelection();

	 void ShowStats( wxString name=wxEmptyString );

private:
	void OnTextSearch( wxCommandEvent &evt );
	void OnCommand(wxCommandEvent &evt);
	void OnVarListCheck(wxTreeEvent &evt);
	void OnVarListDClick(wxCommandEvent &evt);
	void OnPopup( wxCommandEvent &evt);

	void OnGridLabelRightClick(wxGridEvent &evt);
	void OnGridLabelDoubleClick(wxGridEvent &evt);

	bool m_frozen;
	wxExtGridCtrl *m_grid;
	Table *m_grid_table;
	// wxCheckListBox *m_varlist;
	VarTreeView *m_varlist;
	wxTreeItemId m_root;
	// wxFont m_data_font;

	// wxTreeItemId m_root_item;
	// std::vector<wxTreeItemId> m_tree_items;
	wxArrayString m_names;
	wxArrayString m_selections;

	wxString m_popup_var_name;

	lk::varhash_t *m_vt;

	DECLARE_EVENT_TABLE();
};


class wxExtGridCtrl;
//class wxNumericCtrl;

class StatDialog: public wxDialog
{
public:
	StatDialog(wxWindow *parent, const wxString &title);

	void Compute( std::vector<lk::vardata_t> &val );

private:
	wxExtGridCtrl *grdMonthly;
	wxTextCtrl *numSumOver1000;
	wxTextCtrl *numSum;
	wxTextCtrl *numMax;
	wxTextCtrl *numMean;
	wxTextCtrl *numMin;
};

class TableViewDialog : public wxDialog
{
    variable *_vardata;
    wxGrid *_grid;
    int _type;
    wxKeyboardState _keyboardstate;

    void OnCommand(wxCommandEvent &evt);
    void OnCopy(wxKeyEvent &evt);

    DECLARE_EVENT_TABLE();
public:
    enum {TVD_TABLE, TVD_VECTHASH, TVD_ARRAY};

    TableViewDialog(wxWindow *parent, variable* vardata, wxString title, int id=wxID_ANY, long style=wxDEFAULT_DIALOG_STYLE);
};


#endif
