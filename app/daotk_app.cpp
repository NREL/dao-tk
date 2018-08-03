
#include <wx/wx.h>
#include <wx/frame.h>
#include <wx/simplebook.h>
#include <wx/filename.h>
#include <wx/log.h>
#include <wx/stdpaths.h>
#include <wx/busyinfo.h>

#ifdef __WXMSW__
#include <wex/mswfatal.h>
#endif

#include "../app/menu.cpng"
#include "../app/notes_white.cpng"

#include <wex/easycurl.h>
#include <wex/metro.h>
#include <wex/icons/cirplus.cpng>
#include <wex/icons/qmark.cpng>
#include <wex/utils.h>

#include "scripting.h"
#include "dataview.h"
#include "scriptview.h"
#include "logview.h"
#include "daotk_app.h"

int version_major = 0;
int version_minor = 0;
int version_micro = 0;

class CustomThemeProvider : public wxMetroThemeProvider
{
public:
	virtual ~CustomThemeProvider() { }
	virtual wxColour Colour(int id)
	{
		switch (id)
		{
		case wxMT_FOREGROUND: return wxColour(255, 121, 38);
		case wxMT_HOVER: return wxColour(201, 87, 16);
		case wxMT_DIMHOVER: return wxColour(0, 102, 18);
		default:
			return wxMetroThemeProvider::Colour(id);
			/*
			case wxMT_BACKGROUND:  return *wxWHITE;
			case wxMT_HOVER: return wxColour( 0, 88, 153 );
			case wxMT_DIMHOVER: return wxColour( 0, 107, 186 );
			case wxMT_LIGHTHOVER: return wxColour( 231, 232, 238 );
			case wxMT_ACCENT: return wxColour( 255, 143, 50 );
			case wxMT_TEXT: return wxColour( 135, 135, 135 );
			case wxMT_ACTIVE: return wxColour( 0, 114, 198 );
			case wxMT_SELECT:  return wxColour(193,210,238);
			case wxMT_HIGHLIGHT: return wxColour(224,232,246);
			*/
		}
	}
};


IMPLEMENT_APP(MyApp)

static wxArrayString g_appArgs;			//any arguments after the applicaton open
void MyApp::OnFatalException()
{
#ifdef __WXMSW__
	wxMSWHandleApplicationFatalException();
#endif
}

bool MyApp::OnInit()
{


#ifdef __WXMSW__
	bool is64 = (sizeof(void*) == 8);
	wxMSWSetupExceptionHandler("DAOToolkit",
		wxString::Format("%d.%d.%d (%d bit)", version_major, version_minor, version_micro, is64 ? 64 : 32),
		"mike.wagner@nrel.gov");
#endif

	wxEasyCurl::Initialize();

	for (int i = 0; i<argc; i++)
		g_appArgs.Add(argv[i]);

	wxInitAllImageHandlers();

	wxMetroTheme::SetTheme(new CustomThemeProvider);

	MainWindow *mw = new MainWindow;
	mw->Show();
	if (g_appArgs.size() > 1)
		mw->LoadScript(g_appArgs[1]);

	return true;
}

int MyApp::OnExit()
{
	return wxApp::OnExit();
}

enum {
	ID_MAIN_MENU = wxID_HIGHEST + 123, ID_TABS,
	ID_NEW_SCRIPT, ID_OPEN_SCRIPT
};



BEGIN_EVENT_TABLE(MainWindow, wxFrame)
EVT_BUTTON(ID_MAIN_MENU, MainWindow::OnCommand)
EVT_LISTBOX(ID_TABS, MainWindow::OnCaseTabChange)
EVT_CLOSE(MainWindow::OnClose)
EVT_MENU(wxID_ABOUT, MainWindow::OnCommand)
EVT_MENU(wxID_HELP, MainWindow::OnCommand)
EVT_MENU(wxID_NEW, MainWindow::OnCommand)
EVT_MENU(wxID_OPEN, MainWindow::OnCommand)
EVT_MENU(wxID_SAVE, MainWindow::OnCommand)
EVT_MENU(wxID_SAVEAS, MainWindow::OnCommand)
EVT_MENU(wxID_CLOSE, MainWindow::OnCommand)
EVT_MENU(wxID_EXIT, MainWindow::OnCommand)
EVT_BUTTON(wxID_HELP, MainWindow::OnCommand)
END_EVENT_TABLE()

static MainWindow *g_mainWindow = 0;

MainWindow::MainWindow()
	: wxFrame(0, wxID_ANY, "DAOToolkit",
		wxDefaultPosition, wxSize(1100, 700))
{
#ifdef __WXMSW__
	SetIcon(wxICON(appicon));
#endif

	if (g_mainWindow != 0)
		wxMessageBox("internal error - only one main window can exist!");
	else
		g_mainWindow = this;

	//Set up directories
    m_working_dir.SetPath( ::wxGetCwd() ); 
    m_install_dir.SetPath( wxPathOnly( wxStandardPaths::Get().GetExecutablePath()) ); // + "/../";
    m_install_dir.RemoveLastDir();

    m_image_dir.SetPath( m_install_dir.GetPath() ); 
	m_image_dir.AppendDir("deploy");
    m_image_dir.AppendDir("rs");
    m_help_dir.SetPath( m_install_dir.GetPath() );
    m_help_dir.AppendDir("rs");
    m_help_dir.AppendDir("help");
    m_local_dir.SetPath( wxPathOnly(wxStandardPaths::Get().GetUserLocalDataDir()) );
    m_local_dir.AppendDir("dao-tk");

	wxBoxSizer *tools = new wxBoxSizer(wxHORIZONTAL);
	tools->Add(m_mainMenuButton = new wxMetroButton(this, ID_MAIN_MENU, wxEmptyString, wxBITMAP_PNG_FROM_DATA(menu), wxDefaultPosition, wxDefaultSize /*, wxMB_DOWNARROW */), 0, wxALL | wxEXPAND, 0);
	m_tabList = new wxMetroTabList(this, ID_TABS, wxDefaultPosition, wxDefaultSize);
	tools->Add(m_tabList, 1, wxALL | wxEXPAND, 0);
	tools->Add(new wxMetroButton(this, wxID_HELP, wxEmptyString, wxBITMAP_PNG_FROM_DATA(qmark), wxDefaultPosition, wxDefaultSize), 0, wxALL | wxEXPAND, 0);

	m_notebook = new wxSimplebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);

	m_statusLabel = new wxStaticText(this, wxID_ANY, "Status");
	m_progressBar = new wxGauge( this, wxID_ANY, 100 );
	
	wxBoxSizer *sz_stat = new wxBoxSizer( wxHORIZONTAL );
	sz_stat->Add( m_progressBar, 1, wxALL|wxEXPAND, 3 );
	sz_stat->Add( m_statusLabel, 5, wxALL|wxEXPAND, 3 );

	wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
	sizer->Add(tools, 0, wxALL | wxEXPAND, 0);
	sizer->Add(m_notebook, 1, wxALL | wxEXPAND, 0);
	sizer->Add(sz_stat, 0, wxALL|wxEXPAND, 0 );

	SetSizer(sizer);

	std::vector<wxAcceleratorEntry> entries;
	entries.push_back(wxAcceleratorEntry(wxACCEL_CTRL, 'o', wxID_OPEN));
	entries.push_back(wxAcceleratorEntry(wxACCEL_CTRL, 's', wxID_SAVE));
	entries.push_back(wxAcceleratorEntry(wxACCEL_CTRL, 'w', wxID_CLOSE));
	entries.push_back(wxAcceleratorEntry(wxACCEL_NORMAL, WXK_F1, wxID_HELP));
	SetAcceleratorTable(wxAcceleratorTable(entries.size(), &entries[0]));

	m_tabList->Append("Script");
	m_ScriptViewForm = new ScriptView(m_notebook);
	m_notebook->AddPage(m_ScriptViewForm, "Script");

	m_tabList->Append("Data");
	m_DataViewForm = new DataView(m_notebook, m_image_dir.GetFullPath().c_str() );
	m_DataViewForm->SetDataObject( m_project.GetMergedData() );
	m_notebook->AddPage(m_DataViewForm, "Data");

	m_tabList->Append("Log");
	m_LogViewForm = new LogView(m_notebook);
	m_notebook->AddPage(m_LogViewForm, "Log");

	UpdateFrameTitle();
}

MainWindow::~MainWindow() {
	g_mainWindow = 0;
}

MainWindow &MainWindow::Instance()
{
	if (!g_mainWindow)
	{
		wxMessageBox("internal error - no mainwindow instance defined");
		::exit(-1);
	}

	return *g_mainWindow;
}

Project *MainWindow::GetProject()
{
    return &m_project;
}

void MainWindow::Log(const wxString &text, bool wnl)
{
	m_LogViewForm->Log(text, wnl);
	wxString ltext(text);
	ltext.Truncate(150);
	m_statusLabel->SetLabel(ltext);
}

void MainWindow::ClearLog()
{
	m_LogViewForm->ClearLog();
}

bool MainWindow::LoadScript(const wxString &file)
{
	return m_ScriptViewForm->Load(file);
}

void MainWindow::OnClose(wxCloseEvent &evt)
{
	Raise();
	if (!m_ScriptViewForm->CloseDoc())
	{
		evt.Veto();
		return;
	}

	// destroy the window
#ifndef DAO_CONSOLE_APP
	wxGetApp().ScheduleForDestruction(this);
#endif
}

void MainWindow::Save()
{
	if (m_fileName.IsEmpty())
	{
		SaveAs();
		return;
	}

	if (!m_ScriptViewForm->Save())
		wxMessageBox("Error writing project to disk:\n\n" + m_fileName, "Notice", wxOK, this);
}

void MainWindow::SaveAs()
{
	wxFileDialog dlg(this, "Save DAO-Tk script file as", wxPathOnly(m_fileName),
		m_fileName, "DAO-Tk Project File (*.dtk)|*.dtk",
		wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
	if (dlg.ShowModal() == wxID_OK)
	{
		m_fileName = dlg.GetPath();
		Save();
	}
}


void MainWindow::OnCommand(wxCommandEvent &evt)
{
	switch (evt.GetId())
	{
	case wxID_NEW:
	case wxID_CLOSE:
		m_ScriptViewForm->CloseDoc();
		break;
	case wxID_SAVEAS:
		SaveAs();
		break;
	case wxID_SAVE:
		Save();
		break;
	case wxID_OPEN:
	{
		m_ScriptViewForm->Open();
		break;
	}
	case ID_MAIN_MENU:
	{
		wxPoint p = m_mainMenuButton->ClientToScreen(wxPoint(0, m_mainMenuButton->GetClientSize().y));
		wxMetroPopupMenu menu;
		menu.Append(wxID_NEW, "New project\tCtrl-N");
		menu.Append(wxID_OPEN, "Open project\tCtrl-O");
		menu.Append(wxID_SAVE, "Save\tCtrl-S");
		menu.Append(wxID_SAVEAS, "Save as...");
		menu.AppendSeparator();
		menu.Append(wxID_CLOSE, "Close\tCtrl-W");
		menu.Append(wxID_EXIT, "Quit");
		menu.Popup(this, p);
	}
	break;
	case wxID_EXIT:
		Close();
		break;
	case wxID_HELP:
		//ShowHelpTopic("home");
		break;

	};
}

void MainWindow::OnCaseTabChange(wxCommandEvent &evt)
{
	m_notebook->SetSelection(evt.GetSelection());
}


void MainWindow::UpdateFrameTitle()
{
	bool is64 = (sizeof(void*) == 8);
	wxString title = "DAOToolkit " + wxString::Format("%d.%d.%d (%d bit)", version_major, version_minor, version_micro, is64 ? 64 : 32);
	if (!m_fileName.IsEmpty())	title += ": " + m_fileName;
	else title += ": untitled";

	if (m_modified) title += " *";

	if (GetTitle() != title)
		SetTitle(title);
}

void MainWindow::SetProgress( int percent, const wxString &msg )
{
	m_progressBar->SetValue( percent );
	m_statusLabel->SetLabel(msg);
}

void MainWindow::UpdateDataTable()
{
	m_DataViewForm->UpdateView();
}

bool MainWindow::UpdateIsStopFlagSet()
{
	return m_ScriptViewForm->GetEditor()->IsStopFlagSet();
}
