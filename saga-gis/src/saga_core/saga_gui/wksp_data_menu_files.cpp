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
#include <wx/menu.h>

#include <saga_api/saga_api.h>

#include "res_commands.h"

#include "wksp_data_menu_files.h"
#include "wksp_data_menu_file.h"

#include "saga_frame.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Data_Menu_Files::CWKSP_Data_Menu_Files(void)
{
	m_bUpdate	= true;
	m_pMenu		= new wxMenu;

	CMD_Menu_Add_Item(m_pMenu, false, ID_CMD_WKSP_OPEN);
	CMD_Menu_Add_Item(m_pMenu, false, ID_CMD_WKSP_SAVE);
	CMD_Menu_Add_Item(m_pMenu, false, ID_CMD_DATA_PROJECT_NEW);

	m_pMenu->AppendSeparator();
	m_pMenu->Append(ID_CMD_DATA_FIRST      , _TL("Project")    , m_Projects   .Create(DATAOBJECT_TYPE_Undefined));
	m_pMenu->Append(ID_CMD_TABLES_FIRST    , _TL("Table")      , m_Tables     .Create(DATAOBJECT_TYPE_Table));
	m_pMenu->Append(ID_CMD_SHAPES_FIRST    , _TL("Shapes")     , m_Shapes     .Create(DATAOBJECT_TYPE_Shapes));
	m_pMenu->Append(ID_CMD_POINTCLOUD_FIRST, _TL("Point Cloud"), m_PointClouds.Create(DATAOBJECT_TYPE_PointCloud));
	m_pMenu->Append(ID_CMD_TIN_FIRST       , _TL("TIN")        , m_TINs       .Create(DATAOBJECT_TYPE_TIN));
	m_pMenu->Append(ID_CMD_GRIDS_FIRST     , _TL("Grid")       , m_Grids      .Create(DATAOBJECT_TYPE_Grid));
	m_pMenu->AppendSeparator();

	CMD_Menu_Add_Item(m_pMenu, false, ID_CMD_FRAME_QUIT);
}

//---------------------------------------------------------
CWKSP_Data_Menu_Files::~CWKSP_Data_Menu_Files(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Data_Menu_Files::Recent_Open(int Cmd_ID)
{
	return(	m_Projects   .Open(Cmd_ID)
		||	m_Tables     .Open(Cmd_ID)
		||	m_Shapes     .Open(Cmd_ID)
		||	m_TINs       .Open(Cmd_ID)
		||	m_PointClouds.Open(Cmd_ID)
		||	m_Grids      .Open(Cmd_ID)
	);
}

//---------------------------------------------------------
void CWKSP_Data_Menu_Files::Recent_Add(int DataType, const wxString &FileName)
{
	if( m_bUpdate && _Get_Menu(DataType) )
	{
		_Get_Menu(DataType)->Add(FileName);

		_Update();
	}
}

//---------------------------------------------------------
void CWKSP_Data_Menu_Files::Recent_Del(int DataType, const wxString &FileName)
{
	if( m_bUpdate && _Get_Menu(DataType) )
	{
		_Get_Menu(DataType)->Del(FileName);

		_Update();
	}
}

//---------------------------------------------------------
inline CWKSP_Data_Menu_File * CWKSP_Data_Menu_Files::_Get_Menu(int DataType)
{
	switch( DataType )
	{
	case DATAOBJECT_TYPE_Undefined:		return( &m_Projects );
	case DATAOBJECT_TYPE_Table:			return( &m_Tables );
	case DATAOBJECT_TYPE_Shapes:		return( &m_Shapes );
	case DATAOBJECT_TYPE_TIN:			return( &m_TINs );
	case DATAOBJECT_TYPE_PointCloud:	return( &m_PointClouds );
	case DATAOBJECT_TYPE_Grid:			return( &m_Grids );
	}

	return( NULL );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Data_Menu_Files::_Update(void)
{
	g_pSAGA_Frame->Freeze();
	m_Projects   .Update(m_pMenu->FindItem(ID_CMD_DATA_FIRST      )->GetSubMenu());
	m_Tables     .Update(m_pMenu->FindItem(ID_CMD_TABLES_FIRST    )->GetSubMenu());
	m_Shapes     .Update(m_pMenu->FindItem(ID_CMD_SHAPES_FIRST    )->GetSubMenu());
	m_TINs       .Update(m_pMenu->FindItem(ID_CMD_TIN_FIRST       )->GetSubMenu());
	m_PointClouds.Update(m_pMenu->FindItem(ID_CMD_POINTCLOUD_FIRST)->GetSubMenu());
	m_Grids      .Update(m_pMenu->FindItem(ID_CMD_GRIDS_FIRST     )->GetSubMenu());
	g_pSAGA_Frame->Thaw();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
