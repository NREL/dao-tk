
#include <lk/env.h>
#include <fstream>
#include <algorithm> 

#include "scripting.h"


static bool LKInfoCallback(void *data)
{
	DAOTKScriptWindow *frame = static_cast<DAOTKScriptWindow*>(data);
	if (frame != NULL)
	{
			frame->AddOutput("message");
			wxYieldIfNeeded();
	}
	return true;
};


static void _test(lk::invoke_t &cxt)
{
	LK_DOC("test", "Test description.", "(void):null");

	//do something
}


static lk::fcall_t *daotk_functions()
{
	static lk::fcall_t st[] = {
		_test,
		0 };

	return (lk::fcall_t*)st;
}


//--------------------- scripting --------------------------------------
DAOTKScriptWindow::DAOTKScriptWindow(wxWindow *parent, int id)
	: wxLKScriptWindow(parent, id)
{
	GetEditor()->RegisterLibrary(daotk_functions(), "DAO-Tk Functions");
	//set a callback in ssc if needed
}

void DAOTKScriptWindow::OnHelp()
{
}

void DAOTKScriptWindow::OnScriptStarted()
{
}

void DAOTKScriptWindow::OnScriptStopped()
{
}

DAOTKScriptWindowFactory::DAOTKScriptWindowFactory()
{
	// nothing to do
}

DAOTKScriptWindowFactory::~DAOTKScriptWindowFactory()
{
	// nothing to do
}

wxLKScriptWindow *DAOTKScriptWindowFactory::Create()
{
	wxLKScriptWindow *sw = new DAOTKScriptWindow(0, wxID_ANY);
#ifdef __WXMSW__
	sw->SetIcon(wxICON(appicon));
#endif    
	return sw;
}
