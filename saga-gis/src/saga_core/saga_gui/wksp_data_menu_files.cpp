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
#include <wx/string.h>
#include <wx/menu.h>

#include <saga_api/saga_api.h>

#include "res_commands.h"

#include "wksp_data_menu_files.h"
#include "wksp_data_menu_file.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Data_Menu_Files::CWKSP_Data_Menu_Files(void)
{
	m_pFMProjects		= new CWKSP_Data_Menu_File(DATAOBJECT_TYPE_Undefined);
	m_pFMTables			= new CWKSP_Data_Menu_File(DATAOBJECT_TYPE_Table);
	m_pFMShapes			= new CWKSP_Data_Menu_File(DATAOBJECT_TYPE_Shapes);
	m_pFMTINs			= new CWKSP_Data_Menu_File(DATAOBJECT_TYPE_TIN);
	m_pFMPointClouds	= new CWKSP_Data_Menu_File(DATAOBJECT_TYPE_PointCloud);
	m_pFMGrids			= new CWKSP_Data_Menu_File(DATAOBJECT_TYPE_Grid);

	m_bUpdate			= true;

	m_Menus				= NULL;
	m_nMenus			= 0;
}

//---------------------------------------------------------
CWKSP_Data_Menu_Files::~CWKSP_Data_Menu_Files(void)
{
	delete(m_pFMProjects);
	delete(m_pFMTables);
	delete(m_pFMShapes);
	delete(m_pFMTINs);
	delete(m_pFMPointClouds);
	delete(m_pFMGrids);

	if( m_Menus )
	{
		SG_Free(m_Menus);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Data_Menu_Files::Add(wxMenu *pMenu)
{
	pMenu->Append(ID_CMD_DATA_FIRST      , _TL("[MNU] Project")    , m_pFMProjects   ->Create());

	pMenu->AppendSeparator();
	pMenu->Append(ID_CMD_TABLES_FIRST    , _TL("[MNU] Table")      , m_pFMTables     ->Create());
	pMenu->Append(ID_CMD_SHAPES_FIRST    , _TL("[MNU] Shapes")     , m_pFMShapes     ->Create());
	pMenu->Append(ID_CMD_TIN_FIRST       , _TL("[MNU] TIN")        , m_pFMTINs       ->Create());
	pMenu->Append(ID_CMD_POINTCLOUD_FIRST, _TL("[MNU] Point Cloud"), m_pFMPointClouds->Create());
	pMenu->Append(ID_CMD_GRIDS_FIRST     , _TL("[MNU] Grid")       , m_pFMGrids      ->Create());

	pMenu->AppendSeparator();
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_FRAME_QUIT);

	//-----------------------------------------------------
	m_Menus	= (wxMenu **)SG_Realloc(m_Menus, (m_nMenus + 1) * sizeof(wxMenu *));
	m_Menus[m_nMenus++]	= pMenu;
}

//---------------------------------------------------------
void CWKSP_Data_Menu_Files::Del(wxMenu *pMenu)
{
	for(int i=0; i<m_nMenus; i++)
	{
		if( m_Menus[i] == pMenu )
		{
			m_nMenus--;

			for( ; i<m_nMenus; i++)
			{
				m_Menus[i]	= m_Menus[i + 1];
			}

			m_Menus	= (wxMenu **)SG_Realloc(m_Menus, m_nMenus * sizeof(wxMenu *));
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Data_Menu_Files::Recent_Open(int Cmd_ID)
{
	return(	m_pFMProjects   ->Open(Cmd_ID)
		||	m_pFMTables     ->Open(Cmd_ID)
		||	m_pFMShapes     ->Open(Cmd_ID)
		||	m_pFMTINs       ->Open(Cmd_ID)
		||	m_pFMPointClouds->Open(Cmd_ID)
		||	m_pFMGrids      ->Open(Cmd_ID)
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
	case DATAOBJECT_TYPE_Undefined:		return( m_pFMProjects );
	case DATAOBJECT_TYPE_Table:			return( m_pFMTables );
	case DATAOBJECT_TYPE_Shapes:		return( m_pFMShapes );
	case DATAOBJECT_TYPE_TIN:			return( m_pFMTINs );
	case DATAOBJECT_TYPE_PointCloud:	return( m_pFMPointClouds );
	case DATAOBJECT_TYPE_Grid:			return( m_pFMGrids );
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
	for(int i=0; i<m_nMenus; i++)
	{
		_Update(m_Menus[i]);
	}
}

//---------------------------------------------------------
void CWKSP_Data_Menu_Files::_Update(wxMenu *pMenu)
{
	m_pFMProjects	->Update(pMenu->FindItem(ID_CMD_DATA_FIRST      )->GetSubMenu());
	m_pFMTables		->Update(pMenu->FindItem(ID_CMD_TABLES_FIRST    )->GetSubMenu());
	m_pFMShapes		->Update(pMenu->FindItem(ID_CMD_SHAPES_FIRST    )->GetSubMenu());
	m_pFMTINs		->Update(pMenu->FindItem(ID_CMD_TIN_FIRST       )->GetSubMenu());
	m_pFMPointClouds->Update(pMenu->FindItem(ID_CMD_POINTCLOUD_FIRST)->GetSubMenu());
	m_pFMGrids		->Update(pMenu->FindItem(ID_CMD_GRIDS_FIRST     )->GetSubMenu());
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
