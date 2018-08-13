
#include <wx/wx.h>
#include <wx/frame.h>
#include <wx/simplebook.h>
#include <wx/filename.h>
#include <wx/log.h>
#include <wx/stdpaths.h>
#include <wx/busyinfo.h>
#include <wx/splitter.h>

#ifdef __WXMSW__
#include <wex/mswfatal.h>
#endif
#include <cstdio>

#include "../app/menu.cpng"
#include "../app/notes_white.cpng"

#include <wex/easycurl.h>
#include <wex/metro.h>
#include <wex/icons/cirplus.cpng>
#include <wex/icons/qmark.cpng>
#include <wex/utils.h>

#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/reader.h>
#include <rapidjson/error/en.h>
// #include <rapidjson/filewritestream.h>
// #include <rapidjson/filereadstream.h>

#include "scripting.h"
#include "dataview.h"
#include "scriptview.h"
#include "scriptlist.h"
#include "logview.h"
#include "daotk_app.h"

int version_major = 0;
int version_minor = 0;
int version_micro = 0;

namespace rjs = rapidjson;

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
	ID_NEW_SCRIPT, ID_OPEN_SCRIPT, ID_RUN_SCRIPT, 
	ID_SAVE_SCRIPT, ID_SAVE_SCRIPT_AS
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
EVT_MENU(ID_NEW_SCRIPT, MainWindow::OnCommand)
EVT_MENU(ID_OPEN_SCRIPT, MainWindow::OnCommand)
EVT_MENU(ID_SAVE_SCRIPT, MainWindow::OnCommand)
EVT_MENU(ID_SAVE_SCRIPT_AS, MainWindow::OnCommand)
EVT_MENU(ID_RUN_SCRIPT, MainWindow::OnCommand)
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
	entries.push_back(wxAcceleratorEntry(wxACCEL_CTRL|wxACCEL_SHIFT, 'o', ID_OPEN_SCRIPT));
	entries.push_back(wxAcceleratorEntry(wxACCEL_CTRL|wxACCEL_SHIFT, 'n', ID_NEW_SCRIPT));
	entries.push_back(wxAcceleratorEntry(wxACCEL_CTRL|wxACCEL_SHIFT, 's', ID_SAVE_SCRIPT));
	entries.push_back(wxAcceleratorEntry(wxACCEL_NORMAL, WXK_F5, ID_RUN_SCRIPT));
	entries.push_back(wxAcceleratorEntry(wxACCEL_NORMAL, WXK_F1, wxID_HELP));
	SetAcceleratorTable(wxAcceleratorTable(entries.size(), &entries[0]));

	
	wxSplitterWindow *splitwin = new wxSplitterWindow(m_notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_LIVE_UPDATE ); 
	splitwin->SetMinimumPaneSize(210);

	wxSplitterWindow *splitscript = new wxSplitterWindow(splitwin, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_LIVE_UPDATE ); 

	m_tabList->Append("Script");
	m_ScriptViewForm = new ScriptView(splitscript);
	m_ScriptList = new ScriptList(splitscript);
	splitscript->SplitVertically(m_ScriptList, m_ScriptViewForm, 180);

	m_LogViewForm = new LogView(splitwin);

	m_notebook->AddPage(splitwin, "Script");

	splitwin->SplitHorizontally(splitscript, m_LogViewForm, 390);

	m_tabList->Append("Data");
	m_DataViewForm = new DataView(m_notebook, m_image_dir.GetFullPath().c_str() );
	m_DataViewForm->SetDataObject( m_project.GetMergedData() );
	m_notebook->AddPage(m_DataViewForm, "Data");


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

void MainWindow::SyntaxLog(const wxString &text)
{
	m_ScriptViewForm->SyntaxText(text);
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

	// if (!m_ScriptViewForm->Save())
	FILE *fp = fopen(m_fileName.c_str(), "w");
	if( fp == NULL )
	{
		wxMessageBox("Error writing project to disk:\n\n" + m_fileName, "Notice", wxOK, this);
		fclose(fp);
		return;
	}

	lk::varhash_t *all_data = GetProject()->GetMergedData();

	
	rjs::Document D;
	D.SetObject();
	rjs::Document::AllocatorType &alloc = D.GetAllocator();

	unsigned int cest=0;	//counter to estimate required file size

	for( lk::varhash_t::iterator it = all_data->begin(); it != all_data->end(); it++ )
	{
		data_base *v = static_cast< data_base* >( it->second );

		if( v->name.empty() || it->first.empty() )
			continue;

		rjs::Value jv(rjs::kObjectType);
		
		rjs::Value jdata;

		switch(v->type)
		{
			case lk::vardata_t::NUMBER:
			{
				double vnum = v->as_number();
				if( vnum != vnum )
					continue;	//don't save invalid data

				int scale;
				jdata.SetInt( double_scale(vnum, &scale) );
				jv.AddMember("s", rjs::Value().SetInt(scale), alloc);
				cest += 10;
			}
			break;
			case lk::vardata_t::STRING:
			{
				std::string strval = v->as_string();
				char *buf = new char[strval.length()];
				sprintf(buf, "%s", strval.c_str());

				jdata.SetString(buf, strval.length(), alloc );

				delete buf;
				cest += strval.length();
			}
			break;
			case lk::vardata_t::VECTOR:
			{
				jdata.SetArray();

				//determine whether this is a 1D or 2D array
				int nr = v->vec()->size();
				int nc = 0;

				bool invalid_data = false;

				for( size_t i=0; i<nr; i++)
				{
					if( nc > 0 )
					{

						for( size_t j=0; j < nc; j++)
						{
							double dval = v->vec()->at(i).vec()->at(j).as_number();
							if( dval != dval )
							{
								invalid_data = true;
								break;			//if any nan's are present, don't save
							}
							int scale;
							jdata.PushBack( double_scale(dval, &scale), alloc );
							jdata.PushBack( scale, alloc );
						}
						if( invalid_data )
							break;

						cest += nc*10;
					}
					else
					{
						double dval = v->vec()->at(i).as_number();
						if( dval != dval )
						{
							invalid_data = true;
							break;			//if any nan's are present, don't save
						}
						int scale;
						jdata.PushBack( double_scale(dval, &scale), alloc );
						jdata.PushBack( scale, alloc );
						cest += 10;
					}
				}				
				if( invalid_data )
					continue;

				if( nr > 0)
				{
					if( v->vec()->front().type() == lk::vardata_t::VECTOR )
					{
						nc = v->vec()->front().vec()->size();
						if( nc > 0 )
						{
							rjs::Value jshape(rjs::kArrayType);
							jshape.PushBack(nr, alloc);
							jshape.PushBack(nc, alloc);

							jv.AddMember("ms", jshape, alloc);
						}
					}
				}
			}
		}
		//add the name
		rjs::Value jname(rjs::kStringType);
		jname.Set(v->name.c_str());
		jv.AddMember("n", jname, alloc);
		cest += v->name.size();

		//add to the data structure
		jv.AddMember("d", jdata, alloc);
		D.AddMember((rjs::Value::StringRefType)(v->name.c_str()), jv, alloc);
	}

	int initsize=2;
	while(initsize < cest)
		initsize *= 2;

	rjs::StringBuffer stringbuffer(0, initsize);
	rjs::Writer< rjs::StringBuffer > writer(stringbuffer);
	writer.SetMaxDecimalPlaces(4);
	D.Accept(writer);

	// std::string output = stringbuffer.GetString();
	
	fprintf(fp, "%s", stringbuffer.GetString() );

	fclose(fp);
	
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

bool MainWindow::Open()
{
	// Close();
	wxFileDialog dlg(this, "Open", wxEmptyString, wxEmptyString,
		"DAO-Tk Project Files (*.dtk)|*.dtk",
		wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_CHANGE_DIR);

	if (dlg.ShowModal() == wxID_OK)
		if (!Load(dlg.GetPath()))
		{
			wxMessageBox("Could not load file:\n\n" + dlg.GetPath());
			return false;
		}
	return true;
}


bool MainWindow::Load(const wxString &file)
{
	FILE *fp = fopen(file.c_str(), "r");
	if (fp)
	{
		wxString str;
		str.reserve((int)1e6);
		char buf[1024];
		while (fgets(buf, 1023, fp) != 0)
			str += buf;

		fclose(fp);

		m_fileName = file;
		
		rjs::Document D;
		
		rjs::ParseResult pres = D.Parse(str.c_str());

		if( !pres )
		{
        	wxMessageBox( wxString::Format("\nDTK File Parse Error (%d): %s\n",
						 pres.Offset(), rjs::GetParseError_En(pres.Code()) ), "File error", wxICON_ERROR);
			return false;
		}


		// ------------ transfer data into project structure -------------------
		lk::varhash_t *all_data = GetProject()->GetMergedData();

		for( lk::varhash_t::iterator it = all_data->begin(); it != all_data->end(); it++ )
		{
			data_base *v = static_cast< data_base* >( it->second );

			//if the item doesn't have a name, we can't load data for it
			if( v->name.empty() || it->first.empty() )
				continue;

			//try to find the item
			if( !D.HasMember(v->name.c_str()) )
				continue;

			rjs::Value &jv = D[v->name.c_str()];

			switch(v->type)
			{
				case lk::vardata_t::NUMBER:
				{
					int inum = jv.HasMember("d") ? jv["d"].GetInt() : 0;
					int scale = jv.HasMember("s") ? jv["s"].GetInt() : 0;
					
					double vnum = double_unscale(inum, scale);

					v->assign( vnum );
				}
				break;
				case lk::vardata_t::STRING:
				{
					v->assign( jv.HasMember("d") ? jv["d"].GetString() : "" );
				}
				break;
				case lk::vardata_t::VECTOR:
				{
					//determine whether this is a 1D or 2D array
					int nr, nc=0;
					if( jv.HasMember("ms") ) 		//matrix shape
					{
						//matrix
						int ms[2];
						ms[0] = jv["ms"].GetArray()[0].GetInt();
						ms[1] = jv["ms"].GetArray()[1].GetInt();

						if( jv["d"].GetArray().Size() != ms[0]*ms[1]*4)
						{
							m_LogViewForm->Log( wxString::Format("NOTICE: Data array size error when loading \"%s\" from file.", v->nice_name.c_str()).c_str(), true );
							continue;
						}

						v->empty_vector();
						v->vec()->resize( nr );

						int ind=0;
						rjs::GenericArray<false, rjs::Value> A = jv["d"].GetArray();

						for( int i=0; i<nr; i++ )
						{
							lk::vardata_t &vi = v->vec()->at(i);
							vi.empty_vector();
							vi.vec()->resize(nc);

							for( int j=0; j<nc; j++ )
							{
								vi.vec()->at(j).assign( double_unscale(A[ind].GetInt(), A[ind+1].GetInt() ) );
								ind += 2;
							}
						}
						
					}
					else
					{
						//vector
						rjs::GenericArray<false, rjs::Value> A = jv["d"].GetArray();
						int nr = A.Size();

						if( nr % 2 != 0)
						{
							m_LogViewForm->Log( wxString::Format("NOTICE: Data array size error when loading \"%s\" from file.", v->nice_name.c_str()).c_str(), true );
							continue;
						}

						nr = nr/2;

						v->empty_vector();
						v->vec()->resize(nr);

						int ind=0;
						for( int i=0; i<nr; i++ )
						{
							v->vec()->at(i).assign( double_unscale(A[ind].GetInt(), A[ind+1].GetInt() ) );
							ind += 2;
						}
					}
				}
			}
		}		

		UpdateDataTable();
		SetProgress(0, wxString::Format("Successfully loaded \"%s\"", m_fileName.c_str()) );

		return true;
	}
	else return false;

}

void MainWindow::OnCommand(wxCommandEvent &evt)
{
	switch (evt.GetId())
	{
	case ID_MAIN_MENU:
	{
		wxPoint p = m_mainMenuButton->ClientToScreen(wxPoint(0, m_mainMenuButton->GetClientSize().y));
		wxMetroPopupMenu menu;
		menu.Append(wxID_NEW, "New project\tCtrl-N");
		menu.Append(wxID_OPEN, "Open project\tCtrl-O");
		menu.Append(wxID_SAVE, "Save project\tCtrl-S");
		menu.Append(wxID_SAVEAS, "Save project as...");
		menu.AppendSeparator();
		menu.Append(ID_NEW_SCRIPT, "New script\tCtrl+Shift-N");
		menu.Append(ID_OPEN_SCRIPT, "Open script\tCtrl+Shift-O");
		menu.Append(ID_SAVE_SCRIPT, "Save script\tCtrl+Shift-S");
		menu.Append(ID_SAVE_SCRIPT_AS, "Save script as...");
		menu.Append(ID_RUN_SCRIPT, "Run script\tF5");
		menu.AppendSeparator();
		menu.Append(wxID_CLOSE, "Close project\tCtrl-W");
		menu.Append(wxID_EXIT, "Quit");
		menu.Popup(this, p);
	}
	break;
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
		Open();
		break;
	case wxID_EXIT:
		Close();
		break;
	case wxID_HELP:
		//ShowHelpTopic("home");
		break;
	case ID_RUN_SCRIPT:
		m_ScriptViewForm->Exec();
		break;
	case ID_NEW_SCRIPT:
		m_ScriptViewForm->CloseDoc();
		break;
	case ID_OPEN_SCRIPT:
		m_ScriptViewForm->Open();
		break;
	case ID_SAVE_SCRIPT:
		m_ScriptViewForm->Save();
		break;
	case ID_SAVE_SCRIPT_AS:
		m_ScriptViewForm->SaveAs();
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
