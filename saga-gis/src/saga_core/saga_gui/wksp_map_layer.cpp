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
//                  WKSP_Map_Layer.cpp                   //
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
#include <wx/window.h>

#include <saga_api/saga_api.h>

#include "res_commands.h"

#include "wksp_data_manager.h"
#include "wksp_layer.h"
#include "wksp_layer_classify.h"
#include "wksp_map.h"
#include "wksp_map_layer.h"

#include "wksp_grid.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Map_Layer::CWKSP_Map_Layer(CWKSP_Layer *pLayer)
{
	m_pLayer		= pLayer;

	m_bShow			= true;

	m_bFitColors	= false;
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxString CWKSP_Map_Layer::Get_Name(void)
{
	return( m_bShow ? m_pLayer->Get_Name() : wxString::Format(wxT("[%s]"), m_pLayer->Get_Name().c_str()) );
}

//---------------------------------------------------------
wxString CWKSP_Map_Layer::Get_Description(void)
{
	return( m_pLayer->Get_Description() );
}

//---------------------------------------------------------
wxMenu * CWKSP_Map_Layer::Get_Menu(void)
{
	wxMenu	*pMenu, *pMenu_Edit;

	pMenu	= new wxMenu(m_pLayer->Get_Name());

	CMD_Menu_Add_Item(pMenu, false, ID_CMD_WKSP_ITEM_CLOSE);
	CMD_Menu_Add_Item(pMenu,  true, ID_CMD_MAPS_LAYER_SHOW);
	pMenu->AppendSeparator();
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_MAPS_MOVE_TOP);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_MAPS_MOVE_UP);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_MAPS_MOVE_DOWN);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_MAPS_MOVE_BOTTOM);

	switch( m_pLayer->Get_Type() )
	{
	default:
		break;

	case WKSP_ITEM_Grid:
		pMenu->AppendSeparator();
		CMD_Menu_Add_Item(pMenu, true, ID_CMD_MAPS_GRID_FITCOLORS);
		break;
	}

	if( (pMenu_Edit = m_pLayer->Edit_Get_Menu()) != NULL )
	{
		pMenu->AppendSeparator();
		pMenu->Append(ID_CMD_WKSP_FIRST, _TL("Edit"), pMenu_Edit);
	}

	return( pMenu );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Map_Layer::On_Command(int Cmd_ID)
{
	switch( Cmd_ID )
	{
	default:
		return( CWKSP_Base_Item::On_Command(Cmd_ID) );

	case ID_CMD_SHAPES_EDIT_SHAPE:
	case ID_CMD_SHAPES_EDIT_ADD_SHAPE:
	case ID_CMD_SHAPES_EDIT_DEL_SHAPE:
	case ID_CMD_SHAPES_EDIT_ADD_PART:
	case ID_CMD_SHAPES_EDIT_DEL_PART:
	case ID_CMD_SHAPES_EDIT_DEL_POINT:
	case ID_CMD_SHAPES_EDIT_SEL_CLEAR:
	case ID_CMD_SHAPES_EDIT_SEL_INVERT:
		return( m_pLayer->On_Command(Cmd_ID) );

	case ID_CMD_WKSP_ITEM_RETURN:
	case ID_CMD_MAPS_LAYER_SHOW:
		m_bShow	= !m_bShow;
		((wxTreeCtrl *)Get_Control())->SetItemText(GetId(), Get_Name());
		((CWKSP_Map *)Get_Manager())->View_Refresh(true);
		break;

	case ID_CMD_MAPS_MOVE_TOP:
		if( Get_Manager()->Move_Top(this) )
			((CWKSP_Map *)Get_Manager())->View_Refresh(false);
		break;

	case ID_CMD_MAPS_MOVE_BOTTOM:
		if( Get_Manager()->Move_Bottom(this) )
			((CWKSP_Map *)Get_Manager())->View_Refresh(false);
		break;

	case ID_CMD_MAPS_MOVE_UP:
		if( Get_Manager()->Move_Up(this) )
			((CWKSP_Map *)Get_Manager())->View_Refresh(false);
		break;

	case ID_CMD_MAPS_MOVE_DOWN:
		if( Get_Manager()->Move_Down(this) )
			((CWKSP_Map *)Get_Manager())->View_Refresh(false);
		break;

	case ID_CMD_MAPS_GRID_FITCOLORS:
		if( (m_bFitColors = !m_bFitColors) == true )
		{
			Fit_Colors(((CWKSP_Map *)Get_Manager())->Get_Extent());
		}
		break;
	}

	return( true );
}

//---------------------------------------------------------
bool CWKSP_Map_Layer::On_Command_UI(wxUpdateUIEvent &event)
{
	switch( event.GetId() )
	{
	default:
		return( CWKSP_Base_Item::On_Command_UI(event) );

	case ID_CMD_SHAPES_EDIT_SHAPE:
	case ID_CMD_SHAPES_EDIT_ADD_SHAPE:
	case ID_CMD_SHAPES_EDIT_DEL_SHAPE:
	case ID_CMD_SHAPES_EDIT_ADD_PART:
	case ID_CMD_SHAPES_EDIT_DEL_PART:
	case ID_CMD_SHAPES_EDIT_DEL_POINT:
	case ID_CMD_SHAPES_EDIT_SEL_CLEAR:
	case ID_CMD_SHAPES_EDIT_SEL_INVERT:
		return( m_pLayer->On_Command_UI(event) );

	case ID_CMD_MAPS_LAYER_SHOW:
		event.Check(m_bShow);
		break;

	case ID_CMD_MAPS_MOVE_TOP:
	case ID_CMD_MAPS_MOVE_UP:
		event.Enable(Get_Index() > 0);
		break;

	case ID_CMD_MAPS_MOVE_DOWN:
	case ID_CMD_MAPS_MOVE_BOTTOM:
		event.Enable(Get_Index() < Get_Manager()->Get_Count() - 1);
		break;

	case ID_CMD_MAPS_GRID_FITCOLORS:
		event.Check(m_bFitColors);
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
CSG_Parameters * CWKSP_Map_Layer::Get_Parameters(void)
{
	return( m_pLayer->Get_Parameters() );
}

//---------------------------------------------------------
void CWKSP_Map_Layer::Parameters_Changed(void)
{
	m_pLayer->Parameters_Changed();

	CWKSP_Base_Item::Parameters_Changed();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define	FIT_OVERLAY_GRID_COLORS(band, extent)	{\
	CWKSP_Grid	*pGrid	= (CWKSP_Grid *)g_pData->Get(m_pLayer->Get_Parameter(band)->asGrid());\
	if( pGrid && m_pLayer->Get_Parameter(band)->is_Enabled() )\
	{	pGrid->Fit_Color_Range(extent);	}\
}

//---------------------------------------------------------
bool CWKSP_Map_Layer::Fit_Colors(const CSG_Rect &rWorld)
{
	if( m_bFitColors )
	{
		if( m_pLayer->Get_Type() == WKSP_ITEM_Grid )
		{
			((CWKSP_Grid *)m_pLayer)->Fit_Color_Range(rWorld);

			if( m_pLayer->Get_Parameter("COLORS_TYPE")->asInt() == CLASSIFY_OVERLAY )
			{
				FIT_OVERLAY_GRID_COLORS("OVERLAY_R", rWorld);
				FIT_OVERLAY_GRID_COLORS("OVERLAY_G", rWorld);
				FIT_OVERLAY_GRID_COLORS("OVERLAY_B", rWorld);
			}

			return( true );
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
