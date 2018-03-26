#ifndef __dao_h
#define __dao_h

#include <wx/frame.h>

//#define ST_CONSOLE_APP      //define this if we want to compile as a console application (no gui)

class wxSimplebook;
class wxPanel;
class wxMetroButton;
class wxMetroTabList;
class ScriptView;
class DataView;
class LogView;


class MyApp : public wxApp
{
public:
	virtual void OnFatalException();
	virtual bool OnInit();
	virtual int OnExit();
};

DECLARE_APP(MyApp)


class MainWindow : public wxFrame
{
public:
	MainWindow();
	virtual ~MainWindow();
	static MainWindow &Instance();
	void UpdateFrameTitle();

	void ClearLog();
	void Log(const wxString &, bool wnl = true);

	bool LoadScript(const wxString &file);

	void Save();
	void SaveAs();

protected:
	void OnClose(wxCloseEvent &);
	void OnCommand(wxCommandEvent &);
	void OnCaseTabChange(wxCommandEvent &);

private:
	wxMetroButton * m_mainMenuButton;
	wxMetroTabList *m_tabList;
	wxSimplebook *m_notebook;

	ScriptView *m_ScriptViewForm;
	DataView *m_DataViewForm;
	LogView *m_LogViewForm;

	wxString m_fileName;

	DECLARE_EVENT_TABLE();
};


#endif
