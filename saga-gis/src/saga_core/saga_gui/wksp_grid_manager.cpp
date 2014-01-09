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
//                 WKSP_Grid_Manager.cpp                 //
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
#include <saga_api/saga_api.h>

#include "res_commands.h"

#include "wksp_data_manager.h"

#include "wksp_map_manager.h"

#include "wksp_grid_manager.h"
#include "wksp_grid_system.h"
#include "wksp_grid.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Grid_Manager::CWKSP_Grid_Manager(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxString CWKSP_Grid_Manager::Get_Name(void)
{
	return( _TL("Grids") );
}

//---------------------------------------------------------
wxString CWKSP_Grid_Manager::Get_Description(void)
{
	wxString	s;

	s.Printf(wxT("<b>%s</b><br>%d %s"), _TL("Grids"), Get_Count(), Get_Count() == 1 ? _TL("grid system") : _TL("grid systems"));

	return( s );
}

//---------------------------------------------------------
wxMenu * CWKSP_Grid_Manager::Get_Menu(void)
{
	wxMenu	*pMenu	= new wxMenu(_TL("Grids"));

	CMD_Menu_Add_Item(pMenu, false, ID_CMD_GRIDS_OPEN);

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
CWKSP_Grid_System * CWKSP_Grid_Manager::Get_System(const CSG_Grid_System &System)
{
	for(int i=0; i<Get_Count(); i++)
	{
		if( System == ((CWKSP_Grid_System *)Get_Item(i))->Get_System() )
		{
			return( (CWKSP_Grid_System *)Get_Item(i) );
		}
	}

	return( NULL );
}

//---------------------------------------------------------
CWKSP_Grid_System * CWKSP_Grid_Manager::_Get_System(const CSG_Grid_System &System)
{
	CWKSP_Grid_System	*pItem	= NULL;

	if( (pItem = Get_System(System)) == NULL )
	{
		Add_Item(pItem = new CWKSP_Grid_System(System));
	}

	return( pItem );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Grid * CWKSP_Grid_Manager::Get_Data(CSG_Grid *pObject)
{
	CWKSP_Grid	*pItem	= NULL;

	for(int i=0; !pItem && i<Get_Count(); i++)
	{
		pItem	= ((CWKSP_Grid_System *)Get_Item(i))->Get_Data(pObject);
	}

	return( pItem );
}

//---------------------------------------------------------
CWKSP_Grid * CWKSP_Grid_Manager::Add_Data(CSG_Grid *pObject)
{
	CWKSP_Grid	*pItem	= Get_Data(pObject);

	if( pItem == NULL && pObject != NULL && pObject->is_Valid() )
	{
		pItem	= _Get_System(pObject->Get_System())->Add_Data(pObject);
	}

	return( pItem );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
