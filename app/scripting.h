#ifndef _DAO_SCRIPTING
#define _DAO_SCRIPTING

#include <wex/lkscript.h>


class DAOTKScriptWindowFactory : public wxLKScriptWindowFactory
{
public:
	DAOTKScriptWindowFactory();
	virtual ~DAOTKScriptWindowFactory();
	virtual wxLKScriptWindow *Create();
};


class DAOTKScriptWindow : public wxLKScriptWindow
{
public:
	DAOTKScriptWindow(wxWindow *parent, int id = wxID_ANY);

protected:
	virtual void OnScriptStarted();
	virtual void OnScriptStopped();
	virtual void OnHelp();

};




#endif