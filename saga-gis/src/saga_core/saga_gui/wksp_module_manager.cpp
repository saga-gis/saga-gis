/**********************************************************
 * Version $Id$
 *********************************************************/

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
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
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

#include "saga.h"

#include "res_commands.h"
#include "res_dialogs.h"

#include "helper.h"

#include "wksp_module_control.h"
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
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Module_Manager::CWKSP_Module_Manager(void)
{
	g_pModules		= this;

	m_pMenu_Modules	= new CWKSP_Menu_Modules;

	//-----------------------------------------------------
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	pNode	= m_Parameters.Add_Node(NULL, "NODE_GENERAL", _TL("General"), _TL(""));

	m_Parameters.Add_Choice(
		pNode	, "START_LOGO"		, _TL("Show Logo at Start Up"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|"),
			_TL("do not show"),
			_TL("only during start up phase"),
			_TL("20 seconds"),
			_TL("until user closes it")
		), 1
	);

	m_Parameters.Add_Value(
		pNode	, "PROCESS_UPDATE"	, _TL("Process Update Frequency [milliseconds]"),
		_TL(""),
		PARAMETER_TYPE_Int	, 0, 0, true
	);

	m_Parameters.Add_Value(
		pNode	, "SAVE_CONFIG"		, _TL("Save Configuration"),
		_TL("disabling might help to shut down faster, particularly if started from slow devices"),
		PARAMETER_TYPE_Bool	, true
	);

#ifdef _OPENMP
	m_Parameters.Add_Value(
		pNode	, "OMP_THREADS_MAX"	, _TL("Number of CPU Cores [# physical processors]"),
		_TW("Number of processors to use for parallelization. Should be set to the number "
		    "of physical processors, and not to the total number of physical and logical processors "
			"on systems supporting hyper-threading."),
		PARAMETER_TYPE_Int	, SG_OMP_Get_Max_Num_Threads(), 1, true, SG_OMP_Get_Max_Num_Procs(), true
	);
#endif

	//-----------------------------------------------------
	pNode	= m_Parameters.Add_Node(NULL, "NODE_MODULES", _TL("Tools"), _TL(""));

	m_Parameters.Add_Value(
		pNode	, "BEEP"			, _TL("Beep when finished"),
		_TL(""),
		PARAMETER_TYPE_Bool	, false
	);

	m_Parameters.Add_Choice(
		pNode	, "HELP_SOURCE"		, _TL("Tool Description Source"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("built-in"),
			_TL("online")
		), 0
	);

	//-----------------------------------------------------
	pNode	= m_Parameters.Add_Node(NULL, "NODE_FILES", _TL("Files"), _TL(""));

	m_Parameters.Add_Value(
		pNode	, "LNG_OLDSTYLE"	, _TL("Old Style Namings"),
		_TL("Use old style namings (e.g. 'modules' instead of 'tools'). Ignored if translation file is used. You need to restart SAGA to apply the changes."),
		PARAMETER_TYPE_Bool, false
	);

	m_Parameters.Add_FilePath(
		pNode	, "LNG_FILE_DIC"	, _TL("Language Translations"),
		_TL("Dictionary for translations from built-in (English) to local language (editable text table). You need to restart SAGA to apply the changes."),
		CSG_String::Format(SG_T("%s|*.lng|%s|*.txt|%s|*.*"),
			_TL("Dictionary Files (*.lng)"),
			_TL("Text Table (*.txt)"),
			_TL("All Files")
		)
	);

	m_Parameters.Add_FilePath(
		pNode	, "CRS_FILE_SRS"		, _TL("CRS Database"),
		_TL("Database with Coordinate Reference System (CRS) definitions. A restart of SAGA is required to have the changes take effect!"),
		CSG_String::Format(SG_T("%s|*.srs|%s|*.*"),
			_TL("Spatial Reference System Files (*.srs)"),
			_TL("All Files")
		)
	);

	m_Parameters.Add_FilePath(
		pNode	, "CRS_FILE_DIC"		, _TL("CRS Dictionary"),
		_TL("Dictionary for Proj.4/OGC WKT translations. A restart of SAGA is required to have the changes take effect!"),
		CSG_String::Format(SG_T("%s|*.dic|%s|*.*"),
			_TL("Dictionary Files (*.dic)"),
			_TL("All Files")
		)
	);

	m_Parameters.Add_FilePath(
		pNode	, "TOOL_MENUS"			, _TL("User defined tool menus"),
		_TL("User defined tool menus."),
		CSG_String::Format(SG_T("%s|*.xml|%s|*.*"),
			_TL("XML Files (*.xml)"),
			_TL("All Files")
		)
	);
}

//---------------------------------------------------------
CWKSP_Module_Manager::~CWKSP_Module_Manager(void)
{
	delete(m_pMenu_Modules);

	g_pModules	= NULL;
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define CFG_LIBS	wxT("/LIBS")
#define CFG_LIBF	wxT("LIB_%03d")

//---------------------------------------------------------
bool CWKSP_Module_Manager::Initialise(void)
{
	CONFIG_Read("/MODULES", &m_Parameters);

	CONFIG_Do_Save(m_Parameters("SAVE_CONFIG")->asBool());

	g_pSAGA->Process_Set_Frequency(m_Parameters("PROCESS_UPDATE")->asInt());

#ifdef _OPENMP
	SG_OMP_Set_Max_Num_Threads(m_Parameters("OMP_THREADS_MAX")->asInt());
#endif

	//-----------------------------------------------------
	wxString	Library;

	for(int i=0; CONFIG_Read(CFG_LIBS, wxString::Format(CFG_LIBF, i), Library); i++)
	{
		if( !wxFileExists(Library) )
		{
			wxFileName	fn(Library);	fn.MakeAbsolute(g_pSAGA->Get_App_Path());

			Library	= fn.GetFullPath();
		}

		SG_Get_Module_Library_Manager().Add_Library(Library);
	}

	if( SG_Get_Module_Library_Manager().Get_Count() == 0 )
	{
#if defined(_SAGA_LINUX)
	if( (SG_Get_Module_Library_Manager().Add_Directory(SG_T(MODULE_LIBRARY_PATH), false)
	   + SG_Get_Module_Library_Manager().Add_Directory(SG_T(SHARE_PATH         ), false)) == 0 )
#endif
		SG_Get_Module_Library_Manager().Add_Directory(g_pSAGA->Get_App_Path(), true);
	}

	_Update(false);

	return( true );
}

//---------------------------------------------------------
#ifdef _SAGA_MSW
	#define GET_LIBPATH(path)	Get_FilePath_Relative(g_pSAGA->Get_App_Path(), path.c_str())
#else
	#define GET_LIBPATH(path)	path.c_str()
#endif

//---------------------------------------------------------
bool CWKSP_Module_Manager::Finalise(void)
{
	CONFIG_Write("/MODULES", &m_Parameters);

	CONFIG_Delete(CFG_LIBS);

	for(int i=0, n=0; i<Get_Count(); i++)
	{
		CWKSP_Module_Group	*pGroup	= Get_Group(i);

		for(int j=0; j<pGroup->Get_Count(); j++)
		{
			CSG_Module_Library	*pLibrary	= pGroup->Get_Library(j)->Get_Library();

			if( pLibrary->Get_Type() == MODULE_CHAINS )
			{
				for(int j=0; j<pLibrary->Get_Count(); j++)
				{
					CONFIG_Write(CFG_LIBS, wxString::Format(CFG_LIBF, n++), GET_LIBPATH(pLibrary->Get_File_Name(j)));
				}
			}
			else
			{
				CONFIG_Write(CFG_LIBS, wxString::Format(CFG_LIBF, n++), GET_LIBPATH(pLibrary->Get_File_Name()));
			}
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxString CWKSP_Module_Manager::Get_Name(void)
{
	return( _TL("Tool Libraries") );
}

//---------------------------------------------------------
wxString CWKSP_Module_Manager::Get_Description(void)
{
	return( SG_Get_Module_Library_Manager().Get_Summary().c_str() );
}

//---------------------------------------------------------
wxMenu * CWKSP_Module_Manager::Get_Menu(void)
{
	wxMenu	*pMenu;

	pMenu	= new wxMenu(_TL("Tool Libraries"));

	CMD_Menu_Add_Item(pMenu, false, ID_CMD_MODULES_OPEN);

	if( Get_Count() > 0 )
	{
		CMD_Menu_Add_Item(pMenu, false, ID_CMD_WKSP_ITEM_CLOSE);
		pMenu->AppendSeparator();
		CMD_Menu_Add_Item(pMenu, false, ID_CMD_WKSP_ITEM_SEARCH);
		pMenu->AppendSeparator();
		CMD_Menu_Add_Item(pMenu, false, ID_CMD_MODULES_SAVE_DOCS);
	}

	return( pMenu );
}


///////////////////////////////////////////////////////////
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

	case ID_CMD_MODULES_SEARCH:
		{
			CWKSP_Base_Item	*pItem	= g_pModule_Ctrl->Search_Item(_TL("Run Tool"), WKSP_ITEM_Module);

			if( pItem && pItem->GetId().IsOk() && pItem->Get_Type() == WKSP_ITEM_Module )
			{
				((CWKSP_Module *)pItem)->Execute(true);
			}
		}
		break;

	case ID_CMD_MODULES_SAVE_DOCS:
		{
			wxString	Path;

			if( DLG_Directory(Path, _TL("Create Tool Description Files")) )
			{
				MSG_General_Add(wxString::Format(SG_T("%s..."), _TL("Create Tool Description Files")), true, true);

				SG_Get_Module_Library_Manager().Get_Summary(&Path);

				MSG_General_Add(_TL("okay"), false, false, SG_UI_MSG_STYLE_SUCCESS);
			}
		}
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
	case ID_CMD_MODULES_SEARCH:
	case ID_CMD_MODULES_SAVE_DOCS:
		event.Enable(Get_Count() > 0 && g_pModule == NULL);
		break;
	}

	return( true );
}

//---------------------------------------------------------
void CWKSP_Module_Manager::On_Execute(wxCommandEvent &event)
{
	CWKSP_Module	*pModule;

	if( (pModule = Get_Module_byID(m_pMenu_Modules->Get_ID_Translated(event.GetId()))) != NULL )
	{
		pModule->On_Command(ID_CMD_WKSP_ITEM_RETURN);
	}
}

//---------------------------------------------------------
void CWKSP_Module_Manager::On_Execute_UI(wxUpdateUIEvent &event)
{
	if( g_pModule )
	{
		if( g_pModule->Get_Menu_ID() == m_pMenu_Modules->Get_ID_Translated(event.GetId()) )
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
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Module_Manager::Parameters_Changed(void)
{
	CWKSP_Base_Item::Parameters_Changed();

	if( m_Parameters("SAVE_CONFIG")->asBool() == false )
	{
		CONFIG_Write("/MODULES", &m_Parameters);
	}

	CONFIG_Do_Save(m_Parameters("SAVE_CONFIG")->asBool());

	g_pSAGA->Process_Set_Frequency(m_Parameters("PROCESS_UPDATE")->asInt());

#ifdef _OPENMP
	SG_OMP_Set_Max_Num_Threads(m_Parameters("OMP_THREADS_MAX")->asInt());
#endif

	m_pMenu_Modules->Update();
}

//---------------------------------------------------------
bool CWKSP_Module_Manager::Do_Beep(void)
{
	return( m_Parameters("BEEP")->asBool() );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Module_Group * CWKSP_Module_Manager::Get_Group(const wxString &Group)
{
	for(int i=0; i<Get_Count(); i++)
	{
		if( !Group.Cmp(Get_Group(i)->Get_Name()) )
		{
			return( Get_Group(i) );
		}
	}

	return( NULL );
}

//---------------------------------------------------------
CWKSP_Module_Library * CWKSP_Module_Manager::Get_Library(CSG_Module_Library *pLibrary)
{
	for(int i=0; i<Get_Count(); i++)
	{
		CWKSP_Module_Group	*pGroup	= Get_Group(i);

		for(int j=0; j<pGroup->Get_Count(); j++)
		{
			if( pLibrary == pGroup->Get_Library(j)->Get_Library() )
			{
				return( pGroup->Get_Library(j) );
			}
		}
	}

	return( NULL );
}

//---------------------------------------------------------
bool CWKSP_Module_Manager::_Update(bool bSyncToCtrl)
{
	if( !bSyncToCtrl )
	{
	//	Get_Control()->Freeze();

		for(int i=SG_Get_Module_Library_Manager().Get_Count()-1; i>=0; i--)
		{
			CSG_Module_Library	*pLibrary	= SG_Get_Module_Library_Manager().Get_Library(i);
			CWKSP_Module_Library	*pItem	= Get_Library(pLibrary);

			if( !pItem )
			{
				CWKSP_Module_Group	*pGroup	= Get_Group(pLibrary->Get_Category().c_str());

				if( !pGroup )
				{
					Add_Item(pGroup = new CWKSP_Module_Group(pLibrary->Get_Category().c_str()));
				}

				pGroup->Add_Library(pLibrary);
			}
			else if( pItem->Get_Library()->Get_Type() == MODULE_CHAINS )
			{
				pItem->Update();
			}
		}

	//	Get_Control()->Thaw();
	}

	m_pMenu_Modules->Update();

	return( true );
}

//---------------------------------------------------------
bool CWKSP_Module_Manager::Update(void)
{
	return( _Update(true) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxMenu * CWKSP_Module_Manager::Get_Menu_Modules(void)
{
	return( m_pMenu_Modules->Get_Menu() );
}

//---------------------------------------------------------
void CWKSP_Module_Manager::Set_Recently_Used(CWKSP_Module *pModule)
{
	m_pMenu_Modules->Set_Recent(pModule);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Module_Manager::Open(void)
{
	wxArrayString	File_Paths;

	if( DLG_Open(File_Paths, ID_DLG_MODULES_OPEN) )
	{
		MSG_General_Add_Line();

		bool	bUpdate	= false;

		for(size_t i=0; i<File_Paths.GetCount(); i++)
		{
			if( SG_Get_Module_Library_Manager().Add_Library(File_Paths[i]) )
			{
				bUpdate	= true;
			}
		}

		if( bUpdate )
		{
			_Update(false);
		}
	}
}

//---------------------------------------------------------
bool CWKSP_Module_Manager::Open(const wxString &File_Name)
{
	if( SG_Get_Module_Library_Manager().Add_Library(File_Name) )
	{
		_Update(false);

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Module_Manager::Exists(CWKSP_Module *pModule)
{
	for(int i=0; i<Get_Count(); i++)
	{
		CWKSP_Module_Group	*pGroup	= Get_Group(i);

		for(int j=0; j<pGroup->Get_Count(); j++)
		{
			if( pGroup->Get_Library(j)->Get_Module(pModule) != NULL )
			{
				return( true );
			}
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
		CWKSP_Module_Group	*pGroup	= Get_Group(i);

		for(int j=0; j<pGroup->Get_Count(); j++)
		{
			if( (pModule = pGroup->Get_Library(j)->Get_Module_byID(CMD_ID)) != NULL )
			{
				return( pModule );
			}
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
CWKSP_Module_Group::CWKSP_Module_Group(const wxString &Name)
{
	m_Name	= Name;
}

//---------------------------------------------------------
CWKSP_Module_Group::~CWKSP_Module_Group(void)
{}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define SUMMARY_ADD_STR(label, value)	wxString::Format("<tr><td valign=\"top\"><b>%s</b></td><td valign=\"top\">%s</td></tr>", label, value)
#define SUMMARY_ADD_INT(label, value)	wxString::Format("<tr><td valign=\"top\"><b>%s</b></td><td valign=\"top\">%d</td></tr>", label, value)

//---------------------------------------------------------
wxString CWKSP_Module_Group::Get_Description(void)
{
	wxString	s;

	s	+= wxString::Format("<h4>%s</h4>", _TL("Tool Libraries"));

	s	+= "<table border=\"0\">";

	s	+= SUMMARY_ADD_STR(_TL("Category" ), Get_Name().c_str());
	s	+= SUMMARY_ADD_INT(_TL("Libraries"), Get_Count());

	s	+= "</table>";

	//-----------------------------------------------------
	s	+= wxString::Format("<hr><h4>%s</h4><table border=\"0\">", _TL("Tool Libraries"));

	s	+= wxString::Format("<tr align=\"left\"><th>%s</th><th>%s</th><th>%s</th></tr>",
			_TL("Library"),
			_TL("Name"   ),
			_TL("Tools"  )
		);

	for(int i=0; i<Get_Count(); i++)
	{
		s	+= wxString::Format("<tr><td valign=\"top\"><i>%s</i></td><td valign=\"top\">%s</td><td valign=\"top\" align=\"right\">%d</td></tr>",
				Get_Library(i)->Get_Library()->Get_Library_Name().c_str(),
				Get_Library(i)->Get_Name().c_str(),
				Get_Library(i)->Get_Count()
			);
	}

	s	+= "</table>";

	return( s );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Module_Group::On_Command(int Cmd_ID)
{
//	switch( Cmd_ID )
	{
//	default:
		return( CWKSP_Base_Manager::On_Command(Cmd_ID) );
	}
}

//---------------------------------------------------------
bool CWKSP_Module_Group::On_Command_UI(wxUpdateUIEvent &event)
{
//	switch( event.GetId() )
	{
//	default:
		return( CWKSP_Base_Manager::On_Command_UI(event) );
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Module_Group::Add_Library(CSG_Module_Library *pLibrary)
{
	return( pLibrary && Add_Item(new CWKSP_Module_Library(pLibrary)) );
}

//---------------------------------------------------------
CWKSP_Module_Library * CWKSP_Module_Group::Get_Library(CSG_Module_Library *pLibrary)
{
	for(int i=0; i<Get_Count(); i++)
	{
		if( pLibrary == Get_Library(i)->Get_Library() )
		{
			return( Get_Library(i) );
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
