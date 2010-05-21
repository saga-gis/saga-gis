
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    User Interface                     //
//                                                       //
//                    Program: SAGA                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                WKSP_Module_Manager.cpp                //
//                                                       //
//          Copyright (C) 2005 by Olaf Conrad            //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'. SAGA is free software; you   //
// can redistribute it and/or modify it under the terms  //
// of the GNU General Public License as published by the //
// Free Software Foundation; version 2 of the License.   //
//                                                       //
// SAGA is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU General Public        //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU General    //
// Public License along with this program; if not,       //
// write to the Free Software Foundation, Inc.,          //
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
// USA.                                                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <wx/dir.h>
#include <wx/filename.h>

#include <saga_api/saga_api.h>
#include <saga_odbc/saga_odbc.h>

#include "saga.h"

#include "res_commands.h"
#include "res_dialogs.h"

#include "helper.h"

#include "wksp_module_manager.h"
#include "wksp_module_library.h"
#include "wksp_module_menu.h"
#include "wksp_module.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Module_Manager	*g_pModules	= NULL;


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Module_Manager::CWKSP_Module_Manager(void)
{
	g_pModules	= this;

	m_pMenu		= new CWKSP_Module_Menu;

	//-----------------------------------------------------
	m_Parameters.Create(this, LNG(""), LNG(""));

	m_Parameters.Add_Value(
		NULL	, "BEEP"		, LNG("[CAP] Beep when finished"),
		LNG(""),
		PARAMETER_TYPE_Bool	, true
	);

	m_Parameters.Add_Choice(
		NULL	, "START_LOGO"	, LNG("Show Logo at Start Up"),
		LNG(""),
		CSG_String::Format(wxT("%s|%s|%s|%s|"),
			LNG("do not show"),
			LNG("only during start up phase"),
			LNG("20 seconds"),
			LNG("until user closes it")
		), 1
	);

	m_Parameters.Add_Value(
		NULL	, "PROC_FREQ"	, LNG("Process Update Frequency [milliseconds]"),
		LNG(""),
		PARAMETER_TYPE_Int	, 0, 0, true
	);
}

//---------------------------------------------------------
CWKSP_Module_Manager::~CWKSP_Module_Manager(void)
{
	delete(m_pMenu);

	g_pModules	= NULL;
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Module_Manager::Initialise(void)
{
	_Config_Read();

	if( Get_Count() == 0 )
	{
#if defined(_SAGA_LINUX)
	if( _Open_Directory(wxT(MODULE_LIBRARY_PATH)) == 0 )
#endif
		_Open_Directory(g_pSAGA->Get_App_Path(), true);
	}

	return( true );
}

//---------------------------------------------------------
bool CWKSP_Module_Manager::Finalise(void)
{
	SG_ODBC_Get_Connection_Manager().Destroy();

	_Config_Write();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxString CWKSP_Module_Manager::Get_Name(void)
{
	return( LNG("[CAP] Module Libraries") );
}

//---------------------------------------------------------
wxString CWKSP_Module_Manager::Get_Description(void)
{
	wxString	s;

	s.Printf(wxT("<b>%s: %d</b><br><br>%s %d<br>"),
		LNG("[CAP] Module Libraries")	, Get_Count(),
		LNG("[CAP] Modules")			, Get_Items_Count()
	);

	return( s );
}

//---------------------------------------------------------
wxMenu * CWKSP_Module_Manager::Get_Menu(void)
{
	wxMenu	*pMenu;

	pMenu	= new wxMenu(LNG("[CAP] Module Libraries"));

	CMD_Menu_Add_Item(pMenu, false, ID_CMD_MODULES_OPEN);

	if( Get_Count() > 0 )
	{
		CMD_Menu_Add_Item(pMenu, false, ID_CMD_WKSP_ITEM_CLOSE);
		CMD_Menu_Add_Item(pMenu, false, ID_CMD_MODULES_SAVE_HTML);
		pMenu->AppendSeparator();
		CMD_Menu_Add_Item(pMenu, false, ID_CMD_WKSP_ITEM_SEARCH);
	}

	return( pMenu );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Module_Manager::On_Command(int Cmd_ID)
{
	switch( Cmd_ID )
	{
	default:
		return( CWKSP_Base_Manager::On_Command(Cmd_ID) );

	case ID_CMD_MODULES_OPEN:
		Open();
		break;

	case WXK_F2:
	case ID_CMD_MODULES_SAVE_HTML:
		_Make_HTML_Docs();
		break;

	case WXK_F3:
		break;
	}

	return( true );
}

//---------------------------------------------------------
bool CWKSP_Module_Manager::On_Command_UI(wxUpdateUIEvent &event)
{
	switch( event.GetId() )
	{
	default:
		return( CWKSP_Base_Manager::On_Command_UI(event) );

	case ID_CMD_WKSP_ITEM_CLOSE:
		event.Enable(Get_Count() > 0 && g_pModule == NULL);
		break;
	}

	return( true );
}

//---------------------------------------------------------
void CWKSP_Module_Manager::On_Execute(wxCommandEvent &event)
{
	CWKSP_Module	*pModule;

	if( (pModule = Get_Module_byID(m_pMenu->Get_ID_Translated(event.GetId()))) != NULL )
	{
		pModule->On_Command(ID_CMD_WKSP_ITEM_RETURN);
	}
}

//---------------------------------------------------------
void CWKSP_Module_Manager::On_Execute_UI(wxUpdateUIEvent &event)
{
	if( g_pModule )
	{
		if( g_pModule->Get_Menu_ID() == m_pMenu->Get_ID_Translated(event.GetId()) )
		{
			event.Enable(true);
			event.Check(true);
		}
		else
		{
			event.Enable(false);
			event.Check(false);
		}
	}
	else
	{
		event.Enable(true);
		event.Check(false);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Module_Manager::Parameters_Changed(void)
{
	g_pSAGA->Process_Set_Frequency(m_Parameters("PROC_FREQ")->asInt());

	CWKSP_Base_Item::Parameters_Changed();
}

//---------------------------------------------------------
bool CWKSP_Module_Manager::Do_Beep(void)
{
	return( m_Parameters("BEEP")->asBool() );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define CFG_LIBS	wxT("/LIBS")
#define CFG_LIBF	wxT("LIB_%03d")

//---------------------------------------------------------
void CWKSP_Module_Manager::_Config_Read(void)
{
	bool		bValue;
	long		lValue;
	wxString	sValue;

	if( CONFIG_Read(wxT("/MODULES"), wxT("BEEP")		, bValue) )
	{
		m_Parameters("BEEP")		->Set_Value(bValue);
	}

	if( CONFIG_Read(wxT("/MODULES"), wxT("START_LOGO")	, lValue) )
	{
		m_Parameters("START_LOGO")	->Set_Value((int)lValue);
	}

	if( CONFIG_Read(wxT("/MODULES"), wxT("PROC_FREQ")	, lValue) )
	{
		m_Parameters("PROC_FREQ")	->Set_Value((int)lValue);
	}

	for(int i=0; CONFIG_Read(CFG_LIBS, wxString::Format(CFG_LIBF, i), sValue); i++)
	{
		Open(sValue);
	}

	m_pMenu->Update();
}

//---------------------------------------------------------
void CWKSP_Module_Manager::_Config_Write(void)
{
	CONFIG_Write(wxT("/MODULES")	, wxT("BEEP")		,		m_Parameters("BEEP")		->asBool());
	CONFIG_Write(wxT("/MODULES")	, wxT("START_LOGO")	, (long)m_Parameters("START_LOGO")	->asInt());
	CONFIG_Write(wxT("/MODULES")	, wxT("PROC_FREQ")	, (long)m_Parameters("PROC_FREQ")	->asInt());

	CONFIG_Delete(CFG_LIBS);

	for(int i=0; i<Get_Count(); i++)
	{
		CONFIG_Write(CFG_LIBS, wxString::Format(CFG_LIBF, i), Get_Library(i)->Get_File_Name());
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CWKSP_Module_Manager::_Open_Directory(const wxChar *sDirectory, bool bOnlySubDirectories)
{
	int			nOpened	= 0;
	wxDir		Dir;
	wxString	FileName;

	if( Dir.Open(sDirectory) )
	{
		if( !bOnlySubDirectories && Dir.GetFirst(&FileName, wxEmptyString, wxDIR_FILES) )
		{
			do
			{	if( FileName.Find(wxT("saga_api")) < 0 && FileName.Find(wxT("saga_gdi")) < 0 && FileName.Find(wxT("wx")) < 0 && FileName.Find(wxT("mingw")) < 0 )
				if( Open(SG_File_Make_Path(Dir.GetName(), FileName, NULL)) )
				{
					nOpened++;
				}
			}
			while( Dir.GetNext(&FileName) );
		}

		if( Dir.GetFirst(&FileName, wxEmptyString, wxDIR_DIRS) )
		{
			do
			{
				if( FileName.CmpNoCase(wxT("dll")) )
				{
					nOpened	+= _Open_Directory(SG_File_Make_Path(Dir.GetName(), FileName, NULL));
				}
			}
			while( Dir.GetNext(&FileName) );
		}
	}

	return( nOpened );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Module_Manager::Open(void)
{
	wxArrayString	File_Paths;

	if( DLG_Open(File_Paths, ID_DLG_MODULES_OPEN) )
	{
		MSG_General_Add_Line();

		for(size_t i=0; i<File_Paths.GetCount(); i++)
		{
			Open(File_Paths[i]);
		}

		m_pMenu->Update();
	}
}

//---------------------------------------------------------
bool CWKSP_Module_Manager::Open(const wxChar *File_Name)
{
	CWKSP_Module_Library	*pLibrary;

	//-----------------------------------------------------
	if( SG_File_Cmp_Extension(File_Name, wxT("mlb"))
	||	SG_File_Cmp_Extension(File_Name, wxT("dll"))
	||	SG_File_Cmp_Extension(File_Name, wxT("so" )) )
	{
		MSG_General_Add(wxString::Format(wxT("%s: %s..."), LNG("[MSG] Load library"), File_Name), true, true);

		//-------------------------------------------------
		for(int i=0; i<Get_Count(); i++)
		{
			if( SG_STR_CMP(File_Name, Get_Library(i)->Get_File_Name()) == 0 )
			{
				MSG_Error_Add(wxString::Format(wxT("%s\n%s"), File_Name, LNG("[ERR] Library has already been loaded")), true);
				MSG_General_Add(LNG("[MSG] has already been loaded"), false);

				return( false );
			}
		}

		//-------------------------------------------------
		pLibrary	= new CWKSP_Module_Library(File_Name);

		if( pLibrary->is_Valid() )
		{
			Add_Item(pLibrary);

			MSG_General_Add(LNG("[MSG] okay"), false, false, SG_UI_MSG_STYLE_SUCCESS);

			return( true );
		}

		delete(pLibrary);

		MSG_General_Add(LNG("[MSG] failed"), false, false, SG_UI_MSG_STYLE_FAILURE);
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Module_Manager::Exists(CWKSP_Module *pModule)
{
	for(int i=0; i<Get_Count(); i++)
	{
		if( Get_Library(i)->Exists(pModule) )
		{
			return( true );
		}
	}

	return( false );
}

//---------------------------------------------------------
CWKSP_Module * CWKSP_Module_Manager::Get_Module_byID(int CMD_ID)
{
	CWKSP_Module	*pModule;

	for(int i=0; i<Get_Count(); i++)
	{
		if( (pModule = Get_Library(i)->Get_Module_byID(CMD_ID)) != NULL )
		{
			return( pModule );
		}
	}

	return( NULL );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Module_Manager::_Make_HTML_Docs(void)
{
	CSG_Parameters	Options(NULL, LNG("Create HTML Documentation"), LNG(""));

	Options.Add_FilePath(NULL, "DIR", LNG("Choose Documentation Folder"), LNG(""), NULL, NULL, true, true);

	if( !DLG_Parameters(&Options) )
	{
		return;
	}

	//-----------------------------------------------------
	bool			bDirectory;
	CSG_File		Stream_Module, Stream_Lib, Stream_Libs, Stream_List;
	wxString		LibName, Directory, Main, s;
	wxFileName		FileName;

	MSG_General_Add(wxString::Format(wxT("%s..."), LNG("Creating module documentation files")), true, true);

	bDirectory	= wxDirExists(Options("DIR")->asString());
	Directory	= bDirectory ? Options("DIR")->asString() : SG_File_Get_Path(g_pSAGA->Get_App_Path()).c_str();

	//-----------------------------------------------------
	FileName.AssignDir	(Directory);
	FileName.SetExt		(wxT("html"));
	FileName.SetName	(wxT("index"));

	Stream_Libs.Open(FileName.GetFullPath().c_str(), SG_FILE_W, false);
	Stream_Libs.Printf(SG_T("<html><head><title>SAGA - System for Automated Geoscientific Analyses</title></head><body>"));
	Stream_Libs.Printf(SG_T("<h1><a href=\"http://www.saga-gis.org\">SAGA - System for Automated Geoscientific Analyses</a></h1>"));
	Stream_Libs.Printf(SG_T("<h2>%s</h2>\n<ul>\n"), LNG("Module Library Descriptions"));

	Main		= FileName.GetFullPath();

	//-----------------------------------------------------
	for(int i=0; i<Get_Count() && PROGRESSBAR_Set_Position(i, Get_Count()); i++)
	{
		LibName				= SG_File_Get_Name(Get_Library(i)->Get_File_Name(), false).c_str();
		FileName.AssignDir	(bDirectory ? Directory.c_str() : SG_File_Get_Path(Get_Library(i)->Get_File_Name()).c_str());
		FileName.AppendDir	(LibName);
		FileName.SetExt		(wxT("html"));

		if( wxDirExists(FileName.GetPath()) || wxMkdir(FileName.GetPath()) )
		{
			//---------------------------------------------
			// create a frame

			FileName.SetName(wxT("index"));

			if( Stream_Lib.Open(FileName.GetFullPath().c_str(), SG_FILE_W, false) )
			{
				if( Stream_Libs.is_Open() )
				{
					s	= Get_FilePath_Relative(Directory.c_str(), FileName.GetFullPath().c_str()).c_str();	if( s[0] == '\\' )	s	= s.AfterFirst('\\');
					Stream_Libs.Printf(wxT("<li><a href=\"%s\">%s</a></li>\n"), s.c_str(), Get_Library(i)->Get_Name().c_str());
				}

				Stream_Lib.Printf(SG_T("<html><head><title>SAGA - System for Automated Geoscientific Analyses</title></head>"));
				Stream_Lib.Printf(SG_T("<frameset cols=\"200,*\" frameborder=\"0\" framespacing=\"0\" border=\"0\">"));
				Stream_Lib.Printf(SG_T("  <frame frameborder=\"0\" noresize src=\"modules.html\" name=\"MODULES\">"));
				Stream_Lib.Printf(SG_T("  <frame frameborder=\"0\" noresize src=\"%s.html\" name=\"CONTENT\">")   , LibName.c_str());
				Stream_Lib.Printf(SG_T("</frameset></html>"));
			}

			//---------------------------------------------
			// write the modules

			if( bDirectory )
				s	= wxT("./../index");
			else
				s	= Get_FilePath_Relative(Main.c_str(), FileName.GetFullPath().c_str()).c_str();	if( s[0] == '\\' )	s	= s.AfterFirst('\\');

			FileName.SetName(wxT("modules"));
			Stream_List.Open(FileName.GetFullPath().c_str(), SG_FILE_W, false);
			Stream_List.Printf(SG_T("<body bgcolor=\"#CCCCCC\">"));
			Stream_List.Printf(SG_T("<b><a target=\"_top\"    href=\"http://www.saga-gis.org\">SAGA</a></b><hr>"));
			Stream_List.Printf(SG_T("<b><a target=\"_top\"    href=\"%s.html\">%s</a></b><hr>"), s.c_str(), LNG("Library Overview"));
			Stream_List.Printf(SG_T("<b><a target=\"CONTENT\" href=\"%s.html\">%s</a></b><hr><ul>"), LibName.c_str(), Get_Library(i)->Get_Name().c_str());

			FileName.SetName(LibName);

			if( Stream_Lib.Open(FileName.GetFullPath().c_str(), SG_FILE_W, false) )
			{
				Stream_Lib.Printf(wxT("%s<hr><ul>"), Get_Library(i)->Get_Description().c_str());

				for(int j=0; j<Get_Library(i)->Get_Count(); j++)
				{
					FileName.SetName(wxString::Format(wxT("%s_%02d"), LibName.c_str(), Get_Library(i)->Get_Module(j)->Get_Index()));

					if( Stream_Module.Open(FileName.GetFullPath().c_str(), SG_FILE_W, false) )
					{
						Stream_Module.Printf(wxT("%s"), Get_Library(i)->Get_Module(j)->Get_Description().c_str());

						Stream_Lib .Printf(wxT("<li><a target=\"CONTENT\" href=\"%s\">%s</a></li>"), FileName.GetFullName().c_str(), Get_Library(i)->Get_Module(j)->Get_Name().c_str());
						Stream_List.Printf(wxT("<li><a target=\"CONTENT\" href=\"%s\">%s</a></li>"), FileName.GetFullName().c_str(), Get_Library(i)->Get_Module(j)->Get_Name().c_str());
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	if( Stream_Libs.is_Open() )
	{
		Stream_Libs.Printf(wxT("</ul>"));
	}

	PROCESS_Set_Okay(true);

	MSG_General_Add(LNG("okay"), false, false, SG_UI_MSG_STYLE_SUCCESS);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
