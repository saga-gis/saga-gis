	
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

	wxMenu *pOpen_File = new wxMenu;
	CMD_Menu_Add_Item(pOpen_File, false, ID_CMD_TABLE_OPEN);
	CMD_Menu_Add_Item(pOpen_File, false, ID_CMD_SHAPES_OPEN);
	CMD_Menu_Add_Item(pOpen_File, false, ID_CMD_POINTCLOUD_OPEN);
	CMD_Menu_Add_Item(pOpen_File, false, ID_CMD_TIN_OPEN);
	CMD_Menu_Add_Item(pOpen_File, false, ID_CMD_GRID_OPEN);
	CMD_Menu_Add_Item(pOpen_File, false, ID_CMD_GRIDS_OPEN);

	wxMenu *pClipboard = new wxMenu;
	CMD_Menu_Add_Item(pClipboard, false, ID_CMD_DATA_CLIPBOARD_PASTE_TABLE);
	CMD_Menu_Add_Item(pClipboard, false, ID_CMD_DATA_CLIPBOARD_PASTE_IMAGE);

	CMD_Menu_Add_Item(m_pMenu, false, ID_CMD_DATA_OPEN);
	m_pMenu->AppendSeparator();
	m_pMenu->Append(ID_CMD_DATA_FIRST      , _TL("Project"     ), m_Project.Create(ID_CMD_DATA_PROJECT_RECENT_FIRST));
	m_pMenu->AppendSubMenu(pOpen_File      , _TL("Open File"   ));
	m_pMenu->Append(ID_CMD_DATA_FILE_RECENT, _TL("Recent Files"), m_Files  .Create(ID_CMD_DATA_FILE_RECENT_FIRST   ));
	m_pMenu->AppendSubMenu(pClipboard      , _TL("Clipboard"   ));
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
		||  m_Files  .Open(Cmd_ID)
	);
}

//---------------------------------------------------------
void CWKSP_Data_Menu_Files::Recent_Add(int DataType, const wxString &File)
{
	CWKSP_Data_Menu_File *pMenu = m_bUpdate ? _Get_Menu(DataType) : NULL;

	if( pMenu )
	{
		pMenu->Add(File); pMenu->Update();
	}
}

//---------------------------------------------------------
void CWKSP_Data_Menu_Files::Recent_Del(int DataType, const wxString &File)
{
	CWKSP_Data_Menu_File *pMenu = m_bUpdate ? _Get_Menu(DataType) : NULL;

	if( pMenu )
	{
		pMenu->Del(File); pMenu->Update();
	}
}

//---------------------------------------------------------
bool CWKSP_Data_Menu_Files::Recent_Get(int DataType, wxArrayString &Files, bool bAppend)
{
	return( _Get_Menu(DataType) && _Get_Menu(DataType)->Get(Files, bAppend) );
}

//---------------------------------------------------------
int CWKSP_Data_Menu_Files::Recent_Count(int DataType)
{
	return( _Get_Menu(DataType) ? _Get_Menu(DataType)->Count() : 0 );
}

//---------------------------------------------------------
inline CWKSP_Data_Menu_File * CWKSP_Data_Menu_Files::_Get_Menu(int DataType)
{
	switch( DataType )
	{
	case SG_DATAOBJECT_TYPE_Undefined: return( &m_Project );
	default                          : return( &m_Files   );
	}
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
