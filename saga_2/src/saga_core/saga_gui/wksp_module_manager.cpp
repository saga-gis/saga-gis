
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
		NULL	, "BEEP"	, LNG("[CAP] Beep when finished"),
		LNG(""),
		PARAMETER_TYPE_Bool	, true
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
		if( _Open_Directory("/usr/local/lib/saga") == 0 )
#endif
		_Open_Directory(g_pSAGA->Get_App_Path());
	}

	return( true );
}

//---------------------------------------------------------
bool CWKSP_Module_Manager::Finalise(void)
{
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
		event.Enable(Get_Count() > 0);
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
	wxString	Value;

	for(int i=0; CONFIG_Read(CFG_LIBS, wxString::Format(CFG_LIBF, i), Value); i++)
	{
		Open(Value);
	}

	m_pMenu->Update();
}

//---------------------------------------------------------
void CWKSP_Module_Manager::_Config_Write(void)
{
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
int CWKSP_Module_Manager::_Open_Directory(const wxChar *sDirectory)
{
	int			nOpened	= 0;
	wxDir		Dir;
	wxString	FileName;

	if( Dir.Open(sDirectory) )
	{
		if( Dir.GetFirst(&FileName, wxEmptyString, wxDIR_FILES) )
		{
			do
			{	if( FileName.Find(wxT("saga_api")) < 0 )
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
				nOpened	+= _Open_Directory(SG_File_Make_Path(Dir.GetName(), FileName, NULL));
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
	||	SG_File_Cmp_Extension(File_Name, wxT("so")) )
	{
		MSG_General_Add(wxString::Format(wxT("%s: %s..."), LNG("[MSG] Load library"), File_Name), true, true);

		//-------------------------------------------------
		for(int i=0; i<Get_Count(); i++)
		{
			if( SG_STR_CMP(File_Name, Get_Library(i)->Get_File_Name()) == 0 )
			{
				MSG_Error_Add(wxString::Format(wxT("%s\n%s"), File_Name, LNG("[ERR] Library has already been loaded")), true);
				MSG_General_Add(LNG("[MSG] is already loaded"), false);

				return( false );
			}
		}

		//-------------------------------------------------
		pLibrary	= new CWKSP_Module_Library(File_Name);

		if( pLibrary->is_Valid() )
		{
			Add_Item(pLibrary);

			MSG_General_Add(LNG("[MSG] okay"), false);

			return( true );
		}

		delete(pLibrary);

		MSG_General_Add(LNG("[MSG] failed"), false);
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
	int			i, j;
	CSG_File	Stream, Stream_Lib;
	wxString	LibName;
	wxFileName	FileName;

	MSG_General_Add(wxString::Format(wxT("%s..."), LNG("Creating module documentation files")), true, true);

	for(i=0; i<Get_Count(); i++)
	{
		FileName.Assign		(Get_Library(i)->Get_File_Name());
		LibName				= FileName.GetName();
		FileName.AppendDir	(LibName);
		FileName.SetExt		(wxT("html"));

		if( (wxDirExists(FileName.GetPath()) || wxMkdir(FileName.GetPath())) && Stream_Lib.Open(FileName.GetFullPath().c_str(), SG_FILE_W, false) )
		{
			Stream_Lib.Printf(wxT("%s<hr>"), Get_Library(i)->Get_Description().c_str());

			for(j=0; j<Get_Library(i)->Get_Count(); j++)
			{
				FileName.SetName(wxString::Format(wxT("%s_%02d"), LibName.c_str(), Get_Library(i)->Get_Module(j)->Get_Index()));

				Stream_Lib.Printf(wxT("<a href=\"%s\">%s</a><br>"), FileName.GetFullName().c_str(), Get_Library(i)->Get_Module(j)->Get_Name().c_str());

				if( Stream.Open(FileName.GetFullPath().c_str(), SG_FILE_W, false) )
				{
					Stream.Printf(wxT("%s"), Get_Library(i)->Get_Module(j)->Get_Description().c_str());

					Stream.Close();
				}
			}

			Stream_Lib.Close();
		}
	}

	MSG_General_Add(LNG("okay"), false, false);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
