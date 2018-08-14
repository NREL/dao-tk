#ifndef __dao_h
#define __dao_h

#include <wx/frame.h>
#include <wx/filename.h>

#include "project.h"

//#define ST_CONSOLE_APP      //define this if we want to compile as a console application (no gui)

class wxSimplebook;
class wxPanel;
class wxMetroButton;
class wxMetroTabList;
class ScriptView;
class ScriptList;
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
private:
	wxFileName 
        m_recent_file_list,    //File containing the list of recent files
        m_settings_file,
        m_working_dir, //The working directory
        m_install_dir, //the head installation directory
        m_image_dir, //the directory containing resource images
        m_local_dir,    //The location to store edited app files
        m_help_dir;    //the location of the help directory

	wxMetroButton * m_mainMenuButton;
	wxMetroTabList *m_tabList;
	wxSimplebook *m_notebook;

	ScriptView *m_ScriptViewForm;
	ScriptList *m_ScriptList;
	DataView *m_DataViewForm;
	LogView *m_LogViewForm;

	wxStaticText *m_statusLabel;
	wxGauge *m_progressBar;

	wxString m_fileName;

	Project m_project;

	DECLARE_EVENT_TABLE();

public:
	MainWindow();
	virtual ~MainWindow();
	static MainWindow &Instance();
	void UpdateFrameTitle();
	Project *GetProject();
	ScriptView *GetScriptViewForm();
	
	bool UpdateIsStopFlagSet();
	void SetProgress( int percent, const wxString &msg = wxEmptyString );
	void UpdateDataTable();

	void ClearLog();
	void Log(const wxString &, bool wnl = true);
	void SyntaxLog(const wxString &);

	bool LoadScript(const wxString &file);

	void Save();
	void SaveAs();

	bool Open();
	bool Load(const wxString &);

protected:
	void OnClose(wxCloseEvent &);
	void OnCommand(wxCommandEvent &);
	void OnCaseTabChange(wxCommandEvent &);

};


#endif
