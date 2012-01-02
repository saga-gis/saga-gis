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
{
}

//---------------------------------------------------------
CWKSP_Grid_Manager::~CWKSP_Grid_Manager(void)
{
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxString CWKSP_Grid_Manager::Get_Name(void)
{
	return( _TL("[CAP] Grids") );
}

//---------------------------------------------------------
wxString CWKSP_Grid_Manager::Get_Description(void)
{
	wxString	s;

	s.Printf(wxT("<b>%s</b><br>%d %s"), _TL("[CAP] Grids"), Get_Count(), Get_Count() == 1 ? _TL("[TXT] grid system") : _TL("[TXT] grid systems"));

	return( s );
}

//---------------------------------------------------------
wxMenu * CWKSP_Grid_Manager::Get_Menu(void)
{
	wxMenu	*pMenu;

	pMenu	= new wxMenu(_TL("[CAP] Grids"));

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
bool CWKSP_Grid_Manager::On_Command(int Cmd_ID)
{
	switch( Cmd_ID )
	{
	default:
		return( CWKSP_Base_Manager::On_Command(Cmd_ID) );

	case ID_CMD_WKSP_ITEM_RETURN:
		break;
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Grid_System * CWKSP_Grid_Manager::Get_System(CSG_Grid_System *pSystem)
{
	if( pSystem != NULL )
	{
		for(int i=0; i<Get_Count(); i++)
		{
			if( pSystem->is_Equal(*Get_System(i)->Get_System()) )
			{
				return( Get_System(i) );
			}
		}
	}

	return( NULL );
}

//---------------------------------------------------------
CWKSP_Grid_System * CWKSP_Grid_Manager::_Get_System(CSG_Grid_System *pSystem)
{
	CWKSP_Grid_System	*pItem;

	if( pSystem )
	{
		if( (pItem = Get_System(pSystem)) == NULL )
		{
			Add_Item(pItem = new CWKSP_Grid_System(*pSystem));
		}

		return( pItem );
	}

	return( NULL );
}

//---------------------------------------------------------
bool CWKSP_Grid_Manager::Exists(CSG_Grid_System *pSystem)
{
	return( Get_System(pSystem) != NULL );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Grid * CWKSP_Grid_Manager::Get_Grid(CSG_Grid *pGrid)
{
	for(int i=0; i<Get_Count(); i++)
	{
		if( Get_System(i)->Get_Grid(pGrid) )
		{
			return( Get_System(i)->Get_Grid(pGrid) );
		}
	}

	return( NULL );
}

//---------------------------------------------------------
bool CWKSP_Grid_Manager::Exists(CSG_Grid *pGrid)
{
	return( Get_Grid(pGrid) != NULL );
}

//---------------------------------------------------------
CWKSP_Grid * CWKSP_Grid_Manager::Add(CSG_Grid *pGrid)
{
	if( pGrid && pGrid->is_Valid() && !Exists(pGrid) )
	{
		return( _Get_System((CSG_Grid_System *)&pGrid->Get_System())->Add(pGrid) );
	}

	return( NULL );
}

//---------------------------------------------------------
CSG_Grid * CWKSP_Grid_Manager::Get_byFileName(const wxString &File_Name)
{
	for(int i=0; i<Get_Count(); i++)
	{
		for(int j=0; j<Get_System(i)->Get_Count(); j++)
		{
			if( !File_Name.Cmp(Get_System(i)->Get_Grid(j)->Get_Grid()->Get_File_Name()) )
			{
				return( Get_System(i)->Get_Grid(j)->Get_Grid() );
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
bool CWKSP_Grid_Manager::Update(CSG_Grid *pGrid, CSG_Parameters *pParameters)
{
	CWKSP_Grid	*pItem;

	if( (pItem = Get_Grid(pGrid)) != NULL )
	{
		pItem->DataObject_Changed(pParameters);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_Grid_Manager::Update_Views(CSG_Grid *pGrid)
{
	CWKSP_Grid	*pItem;

	if( (pItem = Get_Grid(pGrid)) != NULL )
	{
		pItem->Update_Views(false);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_Grid_Manager::Show(CSG_Grid *pGrid, int Map_Mode)
{
	CWKSP_Grid	*pItem;

	if( (pItem = Get_Grid(pGrid)) != NULL )
	{
		switch( Map_Mode )
		{
		case SG_UI_DATAOBJECT_SHOW:
			return( pItem->Show() );

		case SG_UI_DATAOBJECT_SHOW_NEW_MAP:
			g_pMaps->Add(pItem, NULL);

		case SG_UI_DATAOBJECT_SHOW_LAST_MAP:
			return( pItem->Show(g_pMaps->Get_Map(g_pMaps->Get_Count() - 1)) );
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_Grid_Manager::asImage(CSG_Grid *pGrid, CSG_Grid *pImage)
{
	CWKSP_Grid	*pItem;

	if( (pItem = Get_Grid(pGrid)) != NULL )
	{
		return( pItem->asImage(pImage) );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Grid_Manager::Get_Colors(CSG_Grid *pGrid, CSG_Colors *pColors)
{
	CWKSP_Grid	*pItem;

	if( (pItem = Get_Grid(pGrid)) != NULL )
	{
		return( pItem->Get_Colors(pColors) );
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_Grid_Manager::Set_Colors(CSG_Grid *pGrid, CSG_Colors *pColors)
{
	CWKSP_Grid	*pItem;

	if( (pItem = Get_Grid(pGrid)) != NULL )
	{
		pItem->DataObject_Changed(pColors);

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
