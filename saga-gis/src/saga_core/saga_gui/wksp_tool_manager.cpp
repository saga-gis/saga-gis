
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
//                 wksp_tool_manager.cpp                 //
//                                                       //
//          Copyright (C) 2005 by Olaf Conrad            //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'. SAGA is free software; you   //
// can redistribute it and/or modify it under the terms  //
// of the GNU General Public License as published by the //
// Free Software Foundation, either version 2 of the     //
// License, or (at your option) any later version.       //
//                                                       //
// SAGA is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU General Public        //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU General    //
// Public License along with this program; if not, see   //
// <http://www.gnu.org/licenses/>.                       //
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
#include <wx/dir.h>
#include <wx/filename.h>

#include <saga_api/saga_api.h>

#include "saga.h"
#include "saga_frame.h"

#include "res_commands.h"
#include "res_dialogs.h"

#include "dlg_parameters.h"

#include "helper.h"

#include "wksp_tool_control.h"
#include "wksp_tool_manager.h"
#include "wksp_tool_library.h"
#include "wksp_tool_menu.h"
#include "wksp_tool.h"

#include "wksp_data_manager.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Tool_Manager	*g_pTools	= NULL;


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Tool_Manager::CWKSP_Tool_Manager(void)
{
	g_pTools		= this;

	m_pMenu_Tools	= new CWKSP_Tool_Menu;

	//-----------------------------------------------------
	m_Parameters.Add_Node("", "NODE_GENERAL", _TL("General"), _TL(""));

	m_Parameters.Add_Bool("NODE_GENERAL",
		"START_LOGO"	, _TL("Show Logo at Start Up"),
		_TL(""),
		true
	);

	m_Parameters.Add_Int("NODE_GENERAL",
		"PROCESS_UPDATE", _TL("Process Update Frequency [milliseconds]"),
		_TL(""),
		0, 0, true
	);

	m_Parameters.Add_Bool("NODE_GENERAL",
		"SAVE_CONFIG"	, _TL("Save Configuration"),
		_TL("disabling might help to shut down faster, particularly if started from slow devices"),
		true
	);

#ifdef _OPENMP
	m_Parameters.Add_Int("NODE_GENERAL",
		"OMP_THREADS_MAX", _TL("Number of CPU Cores [# physical processors]"),
		_TW("Number of processors to use for parallelization. Should be set to the number "
		    "of physical processors, and not to the total number of physical and logical processors "
			"on systems supporting hyper-threading."),
		SG_OMP_Get_Max_Num_Threads(), 1, true, SG_OMP_Get_Max_Num_Procs(), true
	);
#endif

	//-----------------------------------------------------
	m_Parameters.Add_Node("", "NODE_TOOLS", _TL("Tools"), _TL(""));

	m_Parameters.Add_Bool("NODE_TOOLS",
		"BEEP"			, _TL("Beep when finished"),
		_TL(""),
		false
	);

	m_Parameters.Add_Choice("NODE_TOOLS",
		"HELP_SOURCE"	, _TL("Tool Description Source"),
		_TL(""),
		CSG_String::Format("%s|%s",
			_TL("built-in"),
			_TL("online")
		), 0
	);

	//-----------------------------------------------------
	m_Parameters.Add_Node("", "NODE_FILES", _TL("Files"), _TL(""));

	m_Parameters.Add_Bool("NODE_FILES",
		"LNG_OLDSTYLE"	, _TL("Old Style Namings"),
		_TL("Use old style namings (e.g. 'modules' instead of 'tools'). Ignored if translation file is used. You need to restart SAGA to apply the changes."),
		false
	);

	m_Parameters.Add_FilePath("NODE_FILES",
		"LNG_FILE_DIC"	, _TL("Language Translations"),
		_TL("Dictionary for translations from built-in (English) to local language (editable text table, utf-8 encoded). You need to restart SAGA to apply the changes."),
		CSG_String::Format("%s|*.lng;*.txt|%s (*.lng)|*.lng|%s (*.txt)|*.txt|%s|*.*",
			_TL("Recognized Files"),
			_TL("Dictionary Files"),
			_TL("Text Table"),
			_TL("All Files")
		)
	);

	m_Parameters.Add_FilePath("NODE_FILES",
		"CRS_FILE_SRS"	, _TL("CRS Database"),
		_TL("Database with Coordinate Reference System (CRS) definitions. You need to restart SAGA to apply the changes."),
		CSG_String::Format("%s (*.srs)|*.srs|%s|*.*",
			_TL("Spatial Reference System Files"),
			_TL("All Files")
		)
	);

	m_Parameters.Add_FilePath("NODE_FILES",
		"CRS_FILE_DIC"	, _TL("CRS Dictionary"),
		_TL("Dictionary for Proj.4/OGC WKT translations. You need to restart SAGA to apply the changes."),
		CSG_String::Format("%s (*.dic)|*.dic|%s|*.*",
			_TL("Dictionary Files"),
			_TL("All Files")
		)
	);

	m_Parameters.Add_FilePath("NODE_FILES",
		"TOOL_MENUS"	, _TL("User defined tool menus"),
		_TL("User defined tool menus."),
		CSG_String::Format("%s|*.xml|%s|*.*",
			_TL("XML Files (*.xml)"),
			_TL("All Files")
		)
	);

	//-----------------------------------------------------
	m_Parameters.Add_Node("", "NODE_LOOK", _TL("Appearance"), _TL(""));

	m_Parameters.Add_Int("NODE_LOOK",
		"LOOK_TB_SIZE"	, _TL("Tool Bar Button Size"),
		_TL("Tool bar button sizes. You need to restart SAGA to apply the changes."),
		16, 16, true
	);

	m_Parameters.Add_Int("NODE_LOOK",
		"FLOAT_PRECISION", _TL("Floating Point Precision"),
		_TL("Sets the (max) precision used when floating point values are rendered as text in settings controls. The value set to -1 means infinite precision."),
		10, -1, true
	);
}

//---------------------------------------------------------
CWKSP_Tool_Manager::~CWKSP_Tool_Manager(void)
{
	delete(m_pMenu_Tools);

	g_pTools	= NULL;
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define CFG_LIBS	"/LIBS"
#define CFG_LIBF	"LIB_%03d"

//---------------------------------------------------------
bool CWKSP_Tool_Manager::Initialise(void)
{
	CONFIG_Read("/TOOLS", "DLG_INFO", CDLG_Parameters::m_bInfo);

	CONFIG_Read("/TOOLS", &m_Parameters);

	CONFIG_Do_Save(m_Parameters("SAVE_CONFIG")->asBool());

	g_pSAGA->Process_Set_Frequency(m_Parameters("PROCESS_UPDATE")->asInt());

	#ifdef _OPENMP
		SG_OMP_Set_Max_Num_Threads(m_Parameters("OMP_THREADS_MAX")->asInt());
	#endif

	//-----------------------------------------------------
	#ifdef _SAGA_MSW
		wxString	Default_Path(g_pSAGA->Get_App_Path());
	#else
		wxString	Default_Path(MODULE_LIBRARY_PATH);
	#endif

	//-----------------------------------------------------
	wxString Version; bool bCompatible = CONFIG_Read("/VERSION", "SAGA" , Version) && !Version.Cmp(SAGA_VERSION);

	#ifdef __GNUC__
	if( bCompatible ) // gcc builds: don't load stored libraries when there is no abi compatibility assured!
	{
		long	Number;

		if( !CONFIG_Read("/VERSION", "GNUC" , Number) || Number != __GNUC__ )
		{
			bCompatible	= false;
		}
		#ifdef __GNUC_MINOR__
			else if( !CONFIG_Read("/VERSION", "GNUC_MINOR" , Number) || Number != __GNUC_MINOR__ )
			{
				bCompatible	= false;
			}
			#ifdef __GNUC_PATCHLEVEL__
				else if( !CONFIG_Read("/VERSION", "GNUC_PATCHLEVEL" , Number) || Number != __GNUC_PATCHLEVEL__ )
				{
					bCompatible	= false;
				}
			#endif
		#endif
	}
	#endif

	//-----------------------------------------------------
	if( bCompatible )
	{
		wxString	Library;

		for(int i=0; CONFIG_Read(CFG_LIBS, wxString::Format(CFG_LIBF, i), Library); i++)
		{
			if( !wxFileExists(Library) )
			{
				wxFileName	fn(Library);

				fn.MakeAbsolute(Default_Path);

				Library	= fn.GetFullPath();
			}

			SG_UI_Progress_Lock(true);
			SG_Get_Tool_Library_Manager().Add_Library(&Library);
			SG_UI_Progress_Lock(false);
		}
	}

	//-----------------------------------------------------
	if( SG_Get_Tool_Library_Manager().Get_Count() == 0 )
	{
		On_Command(ID_CMD_TOOL_RELOAD);
	}

	_Update(false);

	return( true );
}

//---------------------------------------------------------
bool CWKSP_Tool_Manager::Finalise(void)
{
	CONFIG_Write("/VERSION", "SAGA", SAGA_VERSION);

	#ifdef __GNUC__
		CONFIG_Write("/VERSION", "GNUC", (long)__GNUC__);
		#ifdef __GNUC_MINOR__
			CONFIG_Write("/VERSION", "GNUC_MINOR", (long)__GNUC_MINOR__);
			#ifdef __GNUC_PATCHLEVEL__
				CONFIG_Write("/VERSION", "GNUC_PATCHLEVEL", (long)__GNUC_PATCHLEVEL__);
			#endif
		#endif
	#endif

	CONFIG_Write("/TOOLS", "DLG_INFO", CDLG_Parameters::m_bInfo);

	CONFIG_Write("/TOOLS", &m_Parameters);

	//-----------------------------------------------------
	#ifdef _SAGA_MSW
		wxString	Default_Path(g_pSAGA->Get_App_Path());
	#else
		wxString	Default_Path(MODULE_LIBRARY_PATH);
	#endif

	CONFIG_Delete(CFG_LIBS);

	for(int i=0, n=0; i<Get_Count(); i++)
	{
		CWKSP_Tool_Group	*pGroup	= Get_Group(i);

		for(int j=0; j<pGroup->Get_Count(); j++)
		{
			CSG_Tool_Library	*pLibrary	= pGroup->Get_Library(j)->Get_Library();

			if( pLibrary->Get_Type() == TOOL_CHAINS )
			{
				for(int j=0; j<pLibrary->Get_Count(); j++)
				{
					CONFIG_Write(CFG_LIBS, wxString::Format(CFG_LIBF, n++),
						Get_FilePath_Relative(Default_Path.c_str(), pLibrary->Get_File_Name(j).c_str())
					);
				}
			}
			else
			{
				CONFIG_Write(CFG_LIBS, wxString::Format(CFG_LIBF, n++),
					Get_FilePath_Relative(Default_Path.c_str(), pLibrary->Get_File_Name().c_str())
				);
			}
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CWKSP_Tool_Manager::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter, int Flags)
{
	//-----------------------------------------------------
	if( Flags & PARAMETER_CHECK_VALUES )
	{
		if( g_pSAGA_Frame && g_pData )
		{
			if( pParameter->Cmp_Identifier("LNG_OLDSTYLE")
			||  pParameter->Cmp_Identifier("LNG_FILE_DIC")
			||  pParameter->Cmp_Identifier("CRS_FILE_SRS")
			||  pParameter->Cmp_Identifier("CRS_FILE_DIC")
			||  pParameter->Cmp_Identifier("LOOK_TB_SIZE") )
			{
				if( DLG_Message_Confirm(_TL("Restart now ?"), _TL("Restart SAGA to apply the changes")) && g_pData->Close(true) )
				{
					m_Parameters.Assign_Values(pParameters);

					g_pSAGA_Frame->Close(true);
				}
			}
		}
	}

	//-----------------------------------------------------
	return( CWKSP_Base_Manager::On_Parameter_Changed(pParameters, pParameter, Flags) );
}

//---------------------------------------------------------
void CWKSP_Tool_Manager::Parameters_Changed(void)
{
	CWKSP_Base_Item::Parameters_Changed();

	if( m_Parameters("SAVE_CONFIG")->asBool() == false )
	{
		CONFIG_Write("/TOOLS", &m_Parameters);
	}

	CONFIG_Do_Save(m_Parameters("SAVE_CONFIG")->asBool());

	g_pSAGA->Process_Set_Frequency(m_Parameters("PROCESS_UPDATE")->asInt());

#ifdef _OPENMP
	SG_OMP_Set_Max_Num_Threads(m_Parameters("OMP_THREADS_MAX")->asInt());
#endif

	m_pMenu_Tools->Update();
}

//---------------------------------------------------------
bool CWKSP_Tool_Manager::Do_Beep(void)
{
	return( m_Parameters("BEEP")->asBool() );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxString CWKSP_Tool_Manager::Get_Name(void)
{
	return( _TL("Tool Libraries") );
}

//---------------------------------------------------------
wxString CWKSP_Tool_Manager::Get_Description(void)
{
	return( SG_Get_Tool_Library_Manager().Get_Summary().c_str() );
}

//---------------------------------------------------------
wxMenu * CWKSP_Tool_Manager::Get_Menu(void)
{
	wxMenu	*pMenu	= new wxMenu(Get_Name());

	CMD_Menu_Add_Item(pMenu, false, ID_CMD_TOOL_OPEN);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_TOOL_RELOAD);

	if( Get_Count() > 0 )
	{
		CMD_Menu_Add_Item(pMenu, false, ID_CMD_WKSP_ITEM_CLOSE);
		pMenu->AppendSeparator();
		CMD_Menu_Add_Item(pMenu, false, ID_CMD_WKSP_ITEM_SEARCH);
		pMenu->AppendSeparator();
		CMD_Menu_Add_Item(pMenu, false, ID_CMD_TOOL_SAVE_DOCS);
	}

	return( pMenu );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Tool_Manager::On_Command(int Cmd_ID)
{
	switch( Cmd_ID )
	{
	default:
		return( CWKSP_Base_Manager::On_Command(Cmd_ID) );

	case ID_CMD_TOOL_OPEN:
		Open();
		break;

	case ID_CMD_TOOL_RELOAD:
		#if defined(_SAGA_LINUX)
			SG_Get_Tool_Library_Manager().Add_Directory(CSG_String(MODULE_LIBRARY_PATH), false);
			SG_Get_Tool_Library_Manager().Add_Directory(SG_File_Make_Path(CSG_String(SHARE_PATH), SG_T("toolchains")), false);
		#else
			SG_Get_Tool_Library_Manager().Add_Directory(CSG_String(&g_pSAGA->Get_App_Path()) + "/tools", false);
		#endif
			_Update(false);
		break;

	case ID_CMD_TOOL_SEARCH:
		{
			CWKSP_Base_Item	*pItem	= g_pTool_Ctrl->Search_Item(_TL("Run Tool"), WKSP_ITEM_Tool);

			if( pItem && pItem->GetId().IsOk() && pItem->Get_Type() == WKSP_ITEM_Tool )
			{
				((CWKSP_Tool *)pItem)->Execute(true);
			}
		}
		break;

	case ID_CMD_TOOL_SAVE_DOCS:
		{
			wxString	Path;

			if( DLG_Directory(Path, _TL("Create Tool Description Files")) )
			{
				MSG_General_Add(wxString::Format(SG_T("%s..."), _TL("Create Tool Description Files")), true, true);

				SG_Get_Tool_Library_Manager().Get_Summary(&Path);

				MSG_General_Add(_TL("okay"), false, false, SG_UI_MSG_STYLE_SUCCESS);
			}
		}
		break;
	}

	return( true );
}

//---------------------------------------------------------
bool CWKSP_Tool_Manager::On_Command_UI(wxUpdateUIEvent &event)
{
	switch( event.GetId() )
	{
	default:
		return( CWKSP_Base_Manager::On_Command_UI(event) );

	case ID_CMD_WKSP_ITEM_CLOSE:
	case ID_CMD_TOOL_SEARCH:
	case ID_CMD_TOOL_SAVE_DOCS:
		event.Enable(Get_Count() > 0 && g_pTool == NULL);
		break;
	}

	return( true );
}

//---------------------------------------------------------
void CWKSP_Tool_Manager::On_Execute(wxCommandEvent &event)
{
	CWKSP_Tool	*pTool;

	if( (pTool = Get_Tool_byID(m_pMenu_Tools->Get_ID_Translated(event.GetId()))) != NULL )
	{
		pTool->On_Command(ID_CMD_WKSP_ITEM_RETURN);
	}
}

//---------------------------------------------------------
void CWKSP_Tool_Manager::On_Execute_UI(wxUpdateUIEvent &event)
{
	if( g_pTool )
	{
		if( g_pTool->Get_Menu_ID() == m_pMenu_Tools->Get_ID_Translated(event.GetId()) )
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
CWKSP_Tool_Group * CWKSP_Tool_Manager::Get_Group(const wxString &Group)
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
CWKSP_Tool_Library * CWKSP_Tool_Manager::Get_Library(CSG_Tool_Library *pLibrary)
{
	for(int i=0; i<Get_Count(); i++)
	{
		CWKSP_Tool_Group	*pGroup	= Get_Group(i);

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
bool CWKSP_Tool_Manager::_Update(bool bSyncToCtrl)
{
	if( !bSyncToCtrl )
	{
	//	Get_Control()->Freeze();

		for(int i=SG_Get_Tool_Library_Manager().Get_Count()-1; i>=0; i--)
		{
			CSG_Tool_Library	*pLibrary	= SG_Get_Tool_Library_Manager().Get_Library(i);
			CWKSP_Tool_Library	*pItem	= Get_Library(pLibrary);

			if( !pItem )
			{
				CWKSP_Tool_Group	*pGroup	= Get_Group(pLibrary->Get_Category().c_str());

				if( !pGroup )
				{
					Add_Item(pGroup = new CWKSP_Tool_Group(pLibrary->Get_Category().c_str()));
				}

				pGroup->Add_Library(pLibrary);
			}
			else if( pItem->Get_Library()->Get_Type() == TOOL_CHAINS )
			{
				pItem->Update();
			}
		}

	//	Get_Control()->Thaw();
	}

	m_pMenu_Tools->Update();

	return( true );
}

//---------------------------------------------------------
bool CWKSP_Tool_Manager::Update(void)
{
	return( _Update(true) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxMenu * CWKSP_Tool_Manager::Get_Menu_Tools(void)
{
	return( m_pMenu_Tools->Get_Menu() );
}

//---------------------------------------------------------
void CWKSP_Tool_Manager::Set_Recently_Used(CWKSP_Tool *pTool)
{
	m_pMenu_Tools->Set_Recent(pTool);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Tool_Manager::Open(void)
{
	wxArrayString	File_Paths;

	if( DLG_Open(File_Paths, ID_DLG_TOOL_OPEN) )
	{
		MSG_General_Add_Line();

		bool	bUpdate	= false;

		for(size_t i=0; i<File_Paths.GetCount(); i++)
		{
			if( SG_Get_Tool_Library_Manager().Add_Library(&File_Paths[i]) )
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
bool CWKSP_Tool_Manager::Open(const wxString &File_Name)
{
	if( SG_Get_Tool_Library_Manager().Add_Library(&File_Name) )
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
bool CWKSP_Tool_Manager::Exists(CWKSP_Tool *pTool)
{
	for(int i=0; i<Get_Count(); i++)
	{
		CWKSP_Tool_Group	*pGroup	= Get_Group(i);

		for(int j=0; j<pGroup->Get_Count(); j++)
		{
			if( pGroup->Get_Library(j)->Get_Tool(pTool) != NULL )
			{
				return( true );
			}
		}
	}

	return( false );
}

//---------------------------------------------------------
CWKSP_Tool * CWKSP_Tool_Manager::Get_Tool_byID(int CMD_ID)
{
	CWKSP_Tool	*pTool;

	for(int i=0; i<Get_Count(); i++)
	{
		CWKSP_Tool_Group	*pGroup	= Get_Group(i);

		for(int j=0; j<pGroup->Get_Count(); j++)
		{
			if( (pTool = pGroup->Get_Library(j)->Get_Tool_byID(CMD_ID)) != NULL )
			{
				return( pTool );
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
CWKSP_Tool_Group::CWKSP_Tool_Group(const wxString &Name)
{
	m_Name	= Name;
}

//---------------------------------------------------------
CWKSP_Tool_Group::~CWKSP_Tool_Group(void)
{}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define SUMMARY_ADD_STR(label, value)	wxString::Format("<tr><td valign=\"top\"><b>%s</b></td><td valign=\"top\">%s</td></tr>", label, value)
#define SUMMARY_ADD_INT(label, value)	wxString::Format("<tr><td valign=\"top\"><b>%s</b></td><td valign=\"top\">%d</td></tr>", label, value)

//---------------------------------------------------------
wxString CWKSP_Tool_Group::Get_Description(void)
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
wxMenu * CWKSP_Tool_Group::Get_Menu(void)
{
	wxMenu	*pMenu	= new wxMenu(Get_Name());

	CMD_Menu_Add_Item(pMenu, false, ID_CMD_TOOL_OPEN);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_TOOL_RELOAD);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_WKSP_ITEM_CLOSE);
	pMenu->AppendSeparator();
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_WKSP_ITEM_SEARCH);

	return( pMenu );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Tool_Group::On_Command(int Cmd_ID)
{
//	switch( Cmd_ID )
	{
//	default:
		return( CWKSP_Base_Manager::On_Command(Cmd_ID) );
	}
}

//---------------------------------------------------------
bool CWKSP_Tool_Group::On_Command_UI(wxUpdateUIEvent &event)
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
bool CWKSP_Tool_Group::Add_Library(CSG_Tool_Library *pLibrary)
{
	return( pLibrary && Add_Item(new CWKSP_Tool_Library(pLibrary)) );
}

//---------------------------------------------------------
CWKSP_Tool_Library * CWKSP_Tool_Group::Get_Library(CSG_Tool_Library *pLibrary)
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
