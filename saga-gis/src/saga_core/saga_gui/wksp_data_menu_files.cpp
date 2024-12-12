	
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
//               WKSP_Data_Menu_Files.cpp                //
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
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <wx/menu.h>
#include <wx/filename.h>

#include <saga_api/saga_api.h>

#include "res_commands.h"

#include "wksp_data_menu_files.h"
#include "wksp_data_menu_file.h"

#include "saga_frame.h"


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Data_Menu_Files::CWKSP_Data_Menu_Files(void)
{
	m_bUpdate = true;
	m_pMenu   = new wxMenu;

	wxMenu *pOpenFiles = new wxMenu;
	CMD_Menu_Add_Item(pOpenFiles, false, ID_CMD_TABLE_OPEN     );
	CMD_Menu_Add_Item(pOpenFiles, false, ID_CMD_SHAPES_OPEN    );
	CMD_Menu_Add_Item(pOpenFiles, false, ID_CMD_POINTCLOUD_OPEN);
	CMD_Menu_Add_Item(pOpenFiles, false, ID_CMD_TIN_OPEN       );
	CMD_Menu_Add_Item(pOpenFiles, false, ID_CMD_GRID_OPEN      );
	CMD_Menu_Add_Item(pOpenFiles, false, ID_CMD_GRIDS_OPEN     );

	wxMenu *pClipboard = new wxMenu;
	CMD_Menu_Add_Item(pClipboard, false, ID_CMD_DATA_CLIPBOARD_PASTE_TABLE);
	CMD_Menu_Add_Item(pClipboard, false, ID_CMD_DATA_CLIPBOARD_PASTE_IMAGE);

	CMD_Menu_Add_Item(m_pMenu, false, ID_CMD_DATA_OPEN);
	m_pMenu->Append(ID_CMD_DATA_FOLDER_RECENT, _TL("Recent Folders"), m_Folder .Create(CWKSP_Data_Menu_File::Recent_Type::Folder ));
	m_pMenu->Append(ID_CMD_DATA_FILE_RECENT  , _TL("Recent Data"   ), m_Data   .Create(CWKSP_Data_Menu_File::Recent_Type::Data   ));
	m_pMenu->AppendSeparator();
	m_pMenu->Append(ID_CMD_DATA_PROJECT      , _TL("Project"       ), m_Project.Create(CWKSP_Data_Menu_File::Recent_Type::Project));
	m_pMenu->AppendSubMenu(pOpenFiles        , _TL("Data"          ));
	m_pMenu->AppendSubMenu(pClipboard        , _TL("Clipboard"     ));
	m_pMenu->AppendSeparator();
	CMD_Menu_Add_Item(m_pMenu, false, ID_CMD_FRAME_QUIT);
}

//---------------------------------------------------------
CWKSP_Data_Menu_Files::~CWKSP_Data_Menu_Files(void)
{}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Data_Menu_Files::Recent_Open(int Cmd_ID)
{
	return(	m_Project.Open(Cmd_ID)
		||  m_Folder .Open(Cmd_ID)
		||  m_Data   .Open(Cmd_ID)
	);
}

//---------------------------------------------------------
void CWKSP_Data_Menu_Files::Recent_Add(CWKSP_Data_Menu_File::Recent_Type Type, const wxString &File)
{
	if( m_bUpdate )
	{
		switch( Type )
		{
		case CWKSP_Data_Menu_File::Recent_Type::Project: m_Project.Add(File, true); break;
		case CWKSP_Data_Menu_File::Recent_Type::Data   : m_Data   .Add(File, true); break; default: return;
		}

		m_Folder.Add(wxFileName(File).GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR), true);
	}
}

//---------------------------------------------------------
void CWKSP_Data_Menu_Files::Recent_Del(CWKSP_Data_Menu_File::Recent_Type Type, const wxString &File)
{
	if( m_bUpdate )
	{
		switch( Type )
		{
		case CWKSP_Data_Menu_File::Recent_Type::Project: m_Project.Del(File, true); break;
		case CWKSP_Data_Menu_File::Recent_Type::Data   : m_Data   .Del(File, true); break; default: return;
		}

		m_Folder.Del(wxFileName(File).GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR), true);
	}
}

//---------------------------------------------------------
bool CWKSP_Data_Menu_Files::Recent_Get(CWKSP_Data_Menu_File::Recent_Type Type, wxArrayString &Files, bool bAppend)
{
	return( _Get_Menu(Type) && _Get_Menu(Type)->Get(Files, bAppend) );
}

//---------------------------------------------------------
int CWKSP_Data_Menu_Files::Recent_Count(CWKSP_Data_Menu_File::Recent_Type Type)
{
	return( _Get_Menu(Type) ? _Get_Menu(Type)->Count() : 0 );
}

//---------------------------------------------------------
inline CWKSP_Data_Menu_File * CWKSP_Data_Menu_Files::_Get_Menu(CWKSP_Data_Menu_File::Recent_Type Type)
{
	switch( Type )
	{
	case CWKSP_Data_Menu_File::Recent_Type::Project: return( &m_Project );
	case CWKSP_Data_Menu_File::Recent_Type::Folder : return( &m_Folder  );
	case CWKSP_Data_Menu_File::Recent_Type::Data   : return( &m_Data    );
	default: return( NULL );
	}
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
