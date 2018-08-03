
/*******************************************************************************************************
*  Copyright 2018 Alliance for Sustainable Energy, LLC
*
*  NOTICE: This software was developed at least in part by Alliance for Sustainable Energy, LLC
*  ("Alliance") under Contract No. DE-AC36-08GO28308 with the U.S. Department of Energy and the U.S.
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
*  the underlying software originally provided by Alliance as "Solar Power tower Integrated Layout and 
*  Optimization Tool" or "SolarPILOT". Except to comply with the foregoing, the terms "Solar Power 
*  tower Integrated Layout and Optimization Tool", "SolarPILOT", or any confusingly similar
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


#include <wx/imaglist.h>
#include <wx/treebase.h>

#include "treeselect.h"

//******* VarTreeView control *************

VarTreeView::VarTreeView( wxWindow *parent, int id, wxString imagedir, const wxPoint &pos, const wxSize &size)
    : wxTreeCtrl(parent, id, pos, size, wxTR_HAS_BUTTONS|wxTR_NO_LINES|wxTR_SINGLE|wxTR_HIDE_ROOT)
{
    bCheckMode = true;
    wxImageList *images = new wxImageList( 16, 16 );
    images->Add(wxBitmap(imagedir+"checkbox_false_16.png", wxBITMAP_TYPE_PNG));
    images->Add(wxBitmap(imagedir+"checkbox_true_16.png", wxBITMAP_TYPE_PNG));
    AssignImageList( images );
}

void VarTreeView::Check(const wxTreeItemId &item, bool b)
{
    SetItemImage(item, b ? ICON_CHECK_TRUE : ICON_CHECK_FALSE);
}

bool VarTreeView::IsChecked(const wxTreeItemId &item)
{
    return (GetItemImage(item)==ICON_CHECK_TRUE);
}

void VarTreeView::EnableCheckMode(bool b)
{
    bCheckMode = b;
}

bool VarTreeView::IsCheckMode()
{
    return bCheckMode;
}

void VarTreeView::OnLClick(wxMouseEvent &evt)
{
    int flags=0;
    wxTreeItemId item = HitTest(evt.GetPosition(), flags);
    if (!item.IsOk()||!bCheckMode)
    {
        evt.Skip();
        return;
    }

    int state = GetItemImage(item);
    if (state == ICON_CHECK_TRUE || state == ICON_CHECK_FALSE)
    {
        SetItemImage(item, 1-state);
        // wxTreeEvent tree_evt( ::wxEVT_COMMAND_TREE_ITEM_ACTIVATED , this, item );
        wxTreeEvent tree_evt( ::wxEVT_TREE_STATE_IMAGE_CLICK , this, item );
        tree_evt.SetPoint( evt.GetPosition() );
        tree_evt.SetLabel( GetItemText(item) );
        ProcessEvent( tree_evt );
    }
    else
        evt.Skip();
}
BEGIN_EVENT_TABLE(VarTreeView, wxTreeCtrl)
    EVT_LEFT_DOWN(VarTreeView::OnLClick)
END_EVENT_TABLE()
//---------------------------------

/* ******** VarTreeTextCtrl ************** */


BEGIN_EVENT_TABLE(VarTreeTextCtrl,wxTextCtrl)
    EVT_KILL_FOCUS(VarTreeTextCtrl::OnLoseFocus)
    EVT_SET_FOCUS(VarTreeTextCtrl::OnSetFocus)
END_EVENT_TABLE()

VarTreeTextCtrl::VarTreeTextCtrl( wxWindow *parent, int id, const wxPoint &pos, const wxSize &size, long style)
    : wxTextCtrl(parent, id, "", pos, size, wxTE_PROCESS_ENTER|style)
{
    /* nothing to do */
    bSendOnFocus = true;
}

void VarTreeTextCtrl::OnLoseFocus(wxFocusEvent &evt)
{
    if (bSendOnFocus && m_origVal != GetValue())
    {
        wxCommandEvent enterpress(wxEVT_COMMAND_TEXT_ENTER, this->GetId() );
        enterpress.SetEventObject(this);
        enterpress.SetString(GetValue());
        GetEventHandler()->ProcessEvent(enterpress);
    }
    evt.Skip();
}

void VarTreeTextCtrl::OnSetFocus(wxFocusEvent &evt)
{
    m_origVal = GetValue();
    evt.Skip();
}
