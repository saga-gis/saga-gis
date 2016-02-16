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
//                  WKSP_Base_Item.cpp                   //
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

#include "active.h"
#include "active_parameters.h"

#include "wksp_base_item.h"
#include "wksp_base_manager.h"

#include "wksp_module_control.h"
#include "wksp_data_control.h"
#include "wksp_map_control.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxString CWKSP_Base_Item::Get_Type_Name(TWKSP_Item Type)
{
	switch( Type )
	{
	case WKSP_ITEM_Module_Manager:		return( _TL("Tool Manager") );
	case WKSP_ITEM_Module_Group:		return( _TL("Tool Set") );
	case WKSP_ITEM_Module_Library:		return( _TL("Tool Library") );
	case WKSP_ITEM_Module:				return( _TL("Tool") );

	case WKSP_ITEM_Data_Manager:		return( _TL("Data Manager") );

	case WKSP_ITEM_Table_Manager:		return( _TL("Table Manager") );
	case WKSP_ITEM_Table:				return( _TL("Table") );

	case WKSP_ITEM_Shapes_Manager:		return( _TL("Shapes Manager") );
	case WKSP_ITEM_Shapes_Type:			return( _TL("Shape Type Manager") );
	case WKSP_ITEM_Shapes:				return( _TL("Shapes") );

	case WKSP_ITEM_TIN_Manager:			return( _TL("TIN Manager") );
	case WKSP_ITEM_TIN:					return( _TL("TIN") );

	case WKSP_ITEM_PointCloud_Manager:	return( _TL("Point Cloud Manager") );
	case WKSP_ITEM_PointCloud:			return( _TL("Point Cloud") );

	case WKSP_ITEM_Grid_Manager:		return( _TL("Grid Manager") );
	case WKSP_ITEM_Grid_System:			return( _TL("Grid System") );
	case WKSP_ITEM_Grid:				return( _TL("Grid") );

	case WKSP_ITEM_Map_Manager:			return( _TL("Map Manager") );
	case WKSP_ITEM_Map:					return( _TL("Map") );
	case WKSP_ITEM_Map_Layer:			return( _TL("Map Layer") );

	default:							return( _TL("unkown") );
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Base_Item::CWKSP_Base_Item(void)
{
	m_bManager	= false;
	m_pManager	= NULL;
	m_ID		= 0;

	m_Parameters.Create(this, _TL(""), _TL(""));
	m_Parameters.Set_Callback_On_Parameter_Changed(&Parameter_Callback);
}

//---------------------------------------------------------
CWKSP_Base_Item::~CWKSP_Base_Item(void)
{
	if( g_pACTIVE && g_pACTIVE->Get_Active() == this )
	{
		g_pACTIVE->Set_Active(NULL);
	}

	if( m_pManager )
	{
		m_pManager->Del_Item(this);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CWKSP_Base_Item::Get_Index(void)
{
	int		iItem;

	if( m_pManager )
	{
		for(iItem=0; iItem<m_pManager->Get_Count(); iItem++)
		{
			if( this == m_pManager->Get_Item(iItem) )
			{
				return( iItem );
			}
		}
	}

	return( -1 );
}

//---------------------------------------------------------
CWKSP_Base_Control * CWKSP_Base_Item::Get_Control(void)
{
	switch( Get_Type() )
	{
    default:
        return( NULL );

	case WKSP_ITEM_Module_Manager:
	case WKSP_ITEM_Module_Group:
	case WKSP_ITEM_Module_Library:
	case WKSP_ITEM_Module:
		return( g_pModule_Ctrl );

	case WKSP_ITEM_Data_Manager:
	case WKSP_ITEM_Table_Manager:
	case WKSP_ITEM_Table:
	case WKSP_ITEM_Shapes_Manager:
	case WKSP_ITEM_Shapes_Type:
	case WKSP_ITEM_Shapes:
	case WKSP_ITEM_TIN_Manager:
	case WKSP_ITEM_TIN:
	case WKSP_ITEM_PointCloud_Manager:
	case WKSP_ITEM_PointCloud:
	case WKSP_ITEM_Grid_Manager:
	case WKSP_ITEM_Grid_System:
	case WKSP_ITEM_Grid:
		return( g_pData_Ctrl );

	case WKSP_ITEM_Map_Manager:
	case WKSP_ITEM_Map:
	case WKSP_ITEM_Map_Layer:
	case WKSP_ITEM_Map_Graticule:
	case WKSP_ITEM_Map_BaseMap:
		return( g_pMap_Ctrl );
	}
}

//---------------------------------------------------------
bool CWKSP_Base_Item::is_Selected(void)
{
	CWKSP_Base_Control	*pControl	= Get_Control();

	return( pControl != NULL && pControl->IsSelected(GetId()) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Base_Item::On_Command(int Cmd_ID)
{
	return( false );
}

//---------------------------------------------------------
bool CWKSP_Base_Item::On_Command_UI(wxUpdateUIEvent &event)
{
	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameters * CWKSP_Base_Item::Get_Parameters(void)
{
	return( m_Parameters.Get_Count() > 0 ? &m_Parameters : NULL );
}

//---------------------------------------------------------
CSG_Parameter * CWKSP_Base_Item::Get_Parameter(const CSG_String &Identifier)
{
	return( m_Parameters.Get_Parameter(Identifier) );
}

//---------------------------------------------------------
void CWKSP_Base_Item::On_Create_Parameters(void)
{}

//---------------------------------------------------------
void CWKSP_Base_Item::Parameters_Changed(void)
{
	if( Get_Control() && GetId().IsOk() )
	{
		Get_Control()->SetItemText(GetId(), Get_Name());
	}

	if( g_pACTIVE )
	{
		g_pACTIVE->Update(this, false);
	}
}

//---------------------------------------------------------
int CWKSP_Base_Item::Parameter_Callback(CSG_Parameter *pParameter, int Flags)
{
	if( pParameter && pParameter->Get_Owner() && pParameter->Get_Owner()->Get_Owner() )
	{
		return ((CWKSP_Base_Item *)pParameter->Get_Owner()->Get_Owner())->
			On_Parameter_Changed(pParameter->Get_Owner(), pParameter, Flags);
	}

	if( g_pACTIVE )
	{
		return g_pACTIVE->Get_Parameters()->Update_Parameters(pParameter->Get_Owner(), false);
	}

	return( 0 );
}

//---------------------------------------------------------
int CWKSP_Base_Item::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter, int Flags)
{
	return( 1 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
