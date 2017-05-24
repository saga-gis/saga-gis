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
//              WKSP_PointCloud_Manager.cpp              //
//                                                       //
//          Copyright (C) 2009 by Olaf Conrad            //
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <saga_api/saga_api.h>

#include "res_commands.h"

#include "wksp_data_manager.h"

#include "wksp_map_manager.h"

#include "wksp_pointcloud_manager.h"
#include "wksp_pointcloud.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_PointCloud_Manager::CWKSP_PointCloud_Manager(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxString CWKSP_PointCloud_Manager::Get_Name(void)
{
	return( _TL("PointCloud") );
}

//---------------------------------------------------------
wxString CWKSP_PointCloud_Manager::Get_Description(void)
{
	wxString	s;

	s.Printf(wxT("<b>%s</b>:%d<br>"), _TL("PointCloud"), Get_Count());

	return( s );
}

//---------------------------------------------------------
wxMenu * CWKSP_PointCloud_Manager::Get_Menu(void)
{
	wxMenu	*pMenu	= new wxMenu(_TL("PointCloud"));

	CMD_Menu_Add_Item(pMenu, false, ID_CMD_POINTCLOUD_OPEN);

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
CWKSP_PointCloud * CWKSP_PointCloud_Manager::Get_Data(CSG_PointCloud *pObject)
{
	for(int i=0; i<Get_Count(); i++)
	{
		if( pObject == Get_Data(i)->Get_Object() )
		{
			return( Get_Data(i) );
		}
	}

	return( NULL );
}

//---------------------------------------------------------
CWKSP_PointCloud * CWKSP_PointCloud_Manager::Add_Data(CSG_PointCloud *pObject)
{
	CWKSP_PointCloud	*pItem	= Get_Data(pObject);

	if( pItem == NULL && pObject != NULL )
	{
		Add_Item(pItem = new CWKSP_PointCloud(pObject));
	}

	return( pItem );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
