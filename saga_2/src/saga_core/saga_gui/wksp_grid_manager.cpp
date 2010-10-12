
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
	return( LNG("[CAP] Grids") );
}

//---------------------------------------------------------
wxString CWKSP_Grid_Manager::Get_Description(void)
{
	wxString	s;

	s.Printf(wxT("<b>%s</b><br>%d %s"), LNG("[CAP] Grids"), Get_Count(), Get_Count() == 1 ? LNG("[TXT] grid system") : LNG("[TXT] grid systems"));

	return( s );
}

//---------------------------------------------------------
wxMenu * CWKSP_Grid_Manager::Get_Menu(void)
{
	wxMenu	*pMenu;

	pMenu	= new wxMenu(LNG("[CAP] Grids"));

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
#include <wx/filename.h>

#include "wksp_module_manager.h"
#include "wksp_module_library.h"
#include "wksp_module.h"

//---------------------------------------------------------
bool CWKSP_Grid_Manager::Open_GDAL(const wxChar *File_Name)
{
	CSG_Module	*pImport	= NULL;

	for(int i=0; i<g_pModules->Get_Count() && !pImport; i++)
	{
		wxFileName	fName(g_pModules->Get_Library(i)->Get_File_Name());

		if( !fName.GetName().Cmp(SG_T("io_gdal")) )
		{
			pImport	= g_pModules->Get_Library(i)->Get_Module(0)->Get_Module();	// GDAL_Import
		}
	}

	if( !pImport
	||	!pImport->Get_Parameters()->Set_Parameter(SG_T("FILES"), PARAMETER_TYPE_FilePath, File_Name)
	||	!pImport->Execute() )
	{
		return( false );
	}

	CSG_Parameter_Grid_List	*pGrids	= pImport->Get_Parameters()->Get_Parameter(SG_T("GRIDS"))->asGridList();

	for(int i=0; i<pGrids->Get_Count(); i++)
	{
		SG_UI_DataObject_Add(pGrids->asGrid(i), SG_UI_DATAOBJECT_UPDATE_ONLY);
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
CSG_Grid * CWKSP_Grid_Manager::Get_byFileName(const wxChar *File_Name)
{
	CSG_String	s(File_Name);

	for(int i=0; i<Get_Count(); i++)
	{
		for(int j=0; j<Get_System(i)->Get_Count(); j++)
		{
			if( !s.Cmp(Get_System(i)->Get_Grid(j)->Get_Grid()->Get_File_Name()) )
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
