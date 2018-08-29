
#ifndef __scriptview_h
#define __scriptview_h

#include <wx/wx.h>
#include <lk/absyn.h>
#include <lk/env.h>

class wxLKScriptCtrl;

class ScriptView : public wxPanel
{
private:
	wxLKScriptCtrl * m_editor;
	wxStaticText *m_statusLabel;
	wxString m_fileName;
	wxButton *m_stopButton;
	wxString m_lastFindStr;
public:
	ScriptView(wxWindow *parent);
	virtual ~ScriptView();

	wxString GetFileName() { return m_fileName; }
	void OnCommand(wxCommandEvent &evt);
	void Open();
	bool Save();
	bool SaveAs();
	bool CloseDoc();
	bool Write(const wxString &file);
	bool Load(const wxString &file);
	void Exec();
    void CreateVariableDialog();


	wxLKScriptCtrl *GetEditor() { return m_editor; }

	DECLARE_EVENT_TABLE()
};


#endif
