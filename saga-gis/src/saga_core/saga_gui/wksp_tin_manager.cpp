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
//                 WKSP_TIN_Manager.cpp                  //
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

#include "wksp_tin_manager.h"
#include "wksp_tin.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_TIN_Manager::CWKSP_TIN_Manager(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxString CWKSP_TIN_Manager::Get_Name(void)
{
	return( _TL("TIN") );
}

//---------------------------------------------------------
wxString CWKSP_TIN_Manager::Get_Description(void)
{
	wxString	s;

	s.Printf(wxT("<b>%s</b>:%d<br>"), _TL("TIN"), Get_Count());

	return( s );
}

//---------------------------------------------------------
wxMenu * CWKSP_TIN_Manager::Get_Menu(void)
{
	wxMenu	*pMenu	= new wxMenu(_TL("TIN"));

	CMD_Menu_Add_Item(pMenu, false, ID_CMD_TIN_OPEN);

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
CWKSP_TIN * CWKSP_TIN_Manager::Get_Data(CSG_TIN *pObject)
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
CWKSP_TIN * CWKSP_TIN_Manager::Add_Data(CSG_TIN *pObject)
{
	CWKSP_TIN	*pItem	= Get_Data(pObject);

	if( pItem == NULL && pObject != NULL )
	{
		Add_Item(pItem = new CWKSP_TIN(pObject));
	}

	return( pItem );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
