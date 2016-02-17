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
//                 WKSP_Data_Control.cpp                 //
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
#include <wx/image.h>
#include <wx/imaglist.h>

#include <saga_api/saga_api.h>

#include "res_commands.h"
#include "res_controls.h"
#include "res_images.h"

#include "active.h"

#include "wksp_data_control.h"
#include "wksp_data_manager.h"
#include "wksp_data_layers.h"

#include "wksp_table_manager.h"
#include "wksp_table.h"

#include "wksp_shapes_manager.h"
#include "wksp_shapes_type.h"
#include "wksp_shapes.h"

#include "wksp_tin_manager.h"
#include "wksp_tin.h"

#include "wksp_grid_manager.h"
#include "wksp_grid_system.h"
#include "wksp_grid.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
enum
{
	IMG_DATA_MANAGER	= 1,

	IMG_TABLE_MANAGER,
	IMG_TABLE,

	IMG_SHAPES_MANAGER,
	IMG_SHAPES_POINT,
	IMG_SHAPES_POINTS,
	IMG_SHAPES_LINE,
	IMG_SHAPES_POLYGON,

	IMG_TIN_MANAGER,
	IMG_TIN,

	IMG_POINTCLOUD_MANAGER,
	IMG_POINTCLOUD,

	IMG_GRID_MANAGER,
	IMG_GRID_SYSTEM,
	IMG_GRID
};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
IMPLEMENT_CLASS(CWKSP_Data_Control, CWKSP_Base_Control)

//---------------------------------------------------------
BEGIN_EVENT_TABLE(CWKSP_Data_Control, CWKSP_Base_Control)
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Data_Control	*g_pData_Ctrl	= NULL;


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Data_Control::CWKSP_Data_Control(wxWindow *pParent)
	: CWKSP_Base_Control(pParent, ID_WND_WKSP_DATA)
{
	g_pData_Ctrl		= this;

	m_bUpdate_Selection	= false;

	SetWindowStyle(wxTR_HAS_BUTTONS|wxTR_MULTIPLE);

	//-----------------------------------------------------
	IMG_ADD_TO_TREECTRL(ID_IMG_WKSP_DATA_MANAGER);

	IMG_ADD_TO_TREECTRL(ID_IMG_WKSP_TABLE_MANAGER);
	IMG_ADD_TO_TREECTRL(ID_IMG_WKSP_TABLE);

	IMG_ADD_TO_TREECTRL(ID_IMG_WKSP_SHAPES_MANAGER);
	IMG_ADD_TO_TREECTRL(ID_IMG_WKSP_SHAPES_POINT);
	IMG_ADD_TO_TREECTRL(ID_IMG_WKSP_SHAPES_POINTS);
	IMG_ADD_TO_TREECTRL(ID_IMG_WKSP_SHAPES_LINE);
	IMG_ADD_TO_TREECTRL(ID_IMG_WKSP_SHAPES_POLYGON);

	IMG_ADD_TO_TREECTRL(ID_IMG_WKSP_TIN_MANAGER);
	IMG_ADD_TO_TREECTRL(ID_IMG_WKSP_TIN);

	IMG_ADD_TO_TREECTRL(ID_IMG_WKSP_POINTCLOUD_MANAGER);
	IMG_ADD_TO_TREECTRL(ID_IMG_WKSP_POINTCLOUD);

	IMG_ADD_TO_TREECTRL(ID_IMG_WKSP_GRID_MANAGER);
	IMG_ADD_TO_TREECTRL(ID_IMG_WKSP_GRID_SYSTEM);
	IMG_ADD_TO_TREECTRL(ID_IMG_WKSP_GRID);

	//-----------------------------------------------------
	_Set_Manager(new CWKSP_Data_Manager);
}

//---------------------------------------------------------
CWKSP_Data_Control::~CWKSP_Data_Control(void)
{
	_Del_Item(m_pManager, true);

	g_pData_Ctrl	= NULL;
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline int CWKSP_Data_Control::_Get_Image_ID(CWKSP_Base_Item *pItem)
{
	if( pItem )
	{
		switch( pItem->Get_Type() )
		{
		default:
			break;

		case WKSP_ITEM_Data_Manager:		return( IMG_DATA_MANAGER );
		case WKSP_ITEM_Table_Manager:		return( IMG_TABLE_MANAGER );
		case WKSP_ITEM_Table:				return( IMG_TABLE );
		case WKSP_ITEM_Shapes_Manager:		return( IMG_SHAPES_MANAGER );

		case WKSP_ITEM_Shapes_Type:
		case WKSP_ITEM_Shapes:
			switch( pItem->Get_Type() == WKSP_ITEM_Shapes
				?	((CWKSP_Shapes *)pItem)->Get_Shapes()->Get_Type()
				:	((CWKSP_Shapes_Type *)pItem)->Get_Shape_Type() )
			{
			default:
			case SHAPE_TYPE_Point:			return( IMG_SHAPES_POINT );
			case SHAPE_TYPE_Points:			return( IMG_SHAPES_POINTS );
			case SHAPE_TYPE_Line:			return( IMG_SHAPES_LINE );
			case SHAPE_TYPE_Polygon:		return( IMG_SHAPES_POLYGON );
			}

		case WKSP_ITEM_TIN_Manager:			return( IMG_TIN_MANAGER );
		case WKSP_ITEM_TIN:					return( IMG_TIN );
		case WKSP_ITEM_PointCloud_Manager:	return( IMG_POINTCLOUD_MANAGER );
		case WKSP_ITEM_PointCloud:			return( IMG_POINTCLOUD );
		case WKSP_ITEM_Grid_Manager:		return( IMG_GRID_MANAGER );
		case WKSP_ITEM_Grid_System:			return( IMG_GRID_SYSTEM );
		case WKSP_ITEM_Grid:				return( IMG_GRID );
		}
	}

	return( 0 );
}

//---------------------------------------------------------
void CWKSP_Data_Control::Add_Item(CWKSP_Base_Manager *pManager, CWKSP_Base_Item *pItem)
{
	if( pManager && pItem )
	{
		if( pManager == Get_Manager() )
		{
			_Add_Item(pItem, _Get_Image_ID(pItem), _Get_Image_ID(pItem), true);

			return;
		}

		AppendItem	(pManager->GetId(), pItem->Get_Name(), _Get_Image_ID(pItem), _Get_Image_ID(pItem), pItem);
		SortChildren(pManager->GetId());
		Expand		(pManager->GetId());

		//-------------------------------------------------
		if( g_pData_Buttons )
		{
			g_pData_Buttons->Update_Buttons();
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Data_Control::Close(bool bSilent)
{
	return( _Del_Item(m_pManager, bSilent) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CWKSP_Data_Control::Get_Selection_Count(void)
{
	wxArrayTreeItemIds	IDs;

	return( GetSelections(IDs) );
}

//---------------------------------------------------------
CWKSP_Base_Item * CWKSP_Data_Control::Get_Item_Selected(void)
{
	if( m_bUpdate_Selection )
	{
		return( NULL );
	}

	if( Get_Manager()->MultiSelect_Check() )
	{
		return( m_pManager );
	}

	wxArrayTreeItemIds	IDs;

	if( GetSelections(IDs) == 1 )
	{
		return( (CWKSP_Base_Item *)GetItemData(IDs[0]) );
	}

	return( NULL );
}

//---------------------------------------------------------
bool CWKSP_Data_Control::Set_Item_Selected(CWKSP_Base_Item *pItem, bool bKeepMultipleSelection)
{
	if( !pItem || !pItem->GetId().IsOk() || pItem->Get_Control() != this )
	{
		return( false );
	}

	if( bKeepMultipleSelection )
	{
		ToggleItemSelection(pItem->GetId());
	}
	else
	{
		m_bUpdate_Selection	= true;
		SelectItem(pItem->GetId());
		m_bUpdate_Selection	= false;

		wxArrayTreeItemIds	IDs;
				
		if( GetSelections(IDs) > 1 )
		{
			for(size_t i=0; i<IDs.Count(); i++)
			{
				if( IDs[i] != pItem->GetId() )
				{
					UnselectItem(IDs[i]);
				}
			}
		}
	}

	g_pACTIVE->Set_Active(Get_Item_Selected());

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
