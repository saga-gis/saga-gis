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
//                     WKSP_Map.cpp                      //
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
#include <wx/dcmemory.h>
#include <wx/filename.h>
#include <wx/clipbrd.h>
#include <wx/dataobj.h>

#include "res_commands.h"
#include "res_dialogs.h"

#include "helper.h"
#include "dc_helper.h"

#include "active.h"
#include "active_legend.h"

#include "wksp_map_control.h"
#include "wksp_map_manager.h"
#include "wksp_map.h"
#include "wksp_map_layer.h"
#include "wksp_map_graticule.h"
#include "wksp_map_basemap.h"
#include "wksp_map_buttons.h"

#include "wksp_layer_legend.h"
#include "wksp_shapes.h"
#include "wksp_data_manager.h"

#include "view_map.h"
#include "view_map_control.h"
#include "view_map_3d.h"
#include "view_layout.h"
#include "view_layout_info.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define LEGEND_SPACE	10


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Rect	CWKSP_Map_Extents::m_Dummy	= CSG_Rect(0, 0, 1, 1);

//---------------------------------------------------------
CWKSP_Map_Extents::CWKSP_Map_Extents(void)
{
	m_iExtent	= -1;
	m_nExtents	= 0;
}

//---------------------------------------------------------
CSG_Rect CWKSP_Map_Extents::Set_Back(void)
{
	if( m_iExtent < 0 )
		return( m_Dummy );

	if( !is_First() )
	{
		m_iExtent--;
	}

	return( Get_Rect(m_iExtent) );
}

//---------------------------------------------------------
CSG_Rect CWKSP_Map_Extents::Set_Forward(void)
{
	if( m_iExtent < 0 )
		return( m_Dummy );

	if( !is_Last() )
	{
		m_iExtent++;
	}

	return( Get_Rect(m_iExtent) );
}

//---------------------------------------------------------
bool CWKSP_Map_Extents::Add_Extent(const CSG_Rect &Extent, bool bReset)
{
	if( Extent.Get_XRange() > 0.0 && Extent.Get_YRange() > 0.0 )
	{
		if( bReset )
		{
			m_iExtent	= -1;
			m_nExtents	= 0;
		}

		if( Extent != Get_Extent() )
		{
			m_iExtent	++;
			m_nExtents	= m_iExtent + 1;

			if( m_nExtents > Get_Count() )
			{
				Add(Extent);
			}
			else
			{
				Get_Rect(m_iExtent)	= Extent;
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
CWKSP_Map::CWKSP_Map(void)
{
	static int	iMap	= 0;

	m_Name.Printf("%02d. %s", ++iMap, _TL("Map"));

	m_pView			= NULL;
	m_pView_3D		= NULL;
	m_pLayout		= NULL;
	m_pLayout_Info	= new CVIEW_Layout_Info(this);

	m_Img_bSave		= false;

	On_Create_Parameters();
}

//---------------------------------------------------------
CWKSP_Map::~CWKSP_Map(void)
{
	if( m_pView    )	m_pView   ->Do_Destroy();
	if( m_pView_3D )	m_pView_3D->Do_Destroy();
	if( m_pLayout  )	m_pLayout ->Do_Destroy();

	delete(m_pLayout_Info);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxString CWKSP_Map::Get_Name(void)
{
	return( m_Name );
}

//---------------------------------------------------------
wxString CWKSP_Map::Get_Description(void)
{
	wxString	s;

	//-----------------------------------------------------
	s	+= wxString::Format("<h4>%s</h4>", _TL("Map"));

	s	+= "<table border=\"0\">";

	DESC_ADD_STR(_TL("Name"             ), m_Name.c_str());
	DESC_ADD_INT(_TL("Layers"           ), Get_Count());
	DESC_ADD_STR(_TL("Coordinate System"), m_Projection.Get_Description().c_str());

	s	+= "</table>";

	//-----------------------------------------------------
	return( s );
}

//---------------------------------------------------------
wxMenu * CWKSP_Map::Get_Menu(void)
{
	wxMenu	*pMenu;

	pMenu	= new wxMenu(_TL("Map"));

	CMD_Menu_Add_Item(pMenu, false, ID_CMD_WKSP_ITEM_CLOSE);
	CMD_Menu_Add_Item(pMenu,  true, ID_CMD_MAPS_SHOW);
	pMenu->AppendSeparator();
	CMD_Menu_Add_Item(pMenu,  true, ID_CMD_MAPS_3D_SHOW);
	CMD_Menu_Add_Item(pMenu,  true, ID_CMD_MAPS_LAYOUT_SHOW);
	pMenu->AppendSeparator();
	CMD_Menu_Add_Item(pMenu,  true, ID_CMD_MAPS_SCALEBAR);
//	CMD_Menu_Add_Item(pMenu,  true, ID_CMD_MAP_NORTH_ARROW);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_MAPS_GRATICULE_ADD);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_MAPS_BASEMAP_ADD);
	CMD_Menu_Add_Item(pMenu,  true, ID_CMD_MAPS_SYNCHRONIZE);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_MAPS_PROJECTION);
	pMenu->AppendSeparator();
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_MAPS_SAVE_IMAGE);
	CMD_Menu_Add_Item(pMenu,  true, ID_CMD_MAPS_SAVE_IMAGE_ON_CHANGE);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_MAPS_SAVE_TO_CLIPBOARD);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_MAPS_SAVE_TO_CLIPBOARD_LEGEND);

	return( pMenu );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Map::On_Command(int Cmd_ID)
{
	switch( Cmd_ID )
	{
	default:
		return( CWKSP_Base_Manager::On_Command(Cmd_ID) );

	case ID_CMD_WKSP_ITEM_RETURN:
		View_Show(true);
		break;

	case ID_CMD_MAPS_SAVE_IMAGE:
		SaveAs_Image();
		break;

	case ID_CMD_MAPS_SAVE_IMAGE_ON_CHANGE:
		SaveAs_Image_On_Change();
		break;

	case ID_CMD_MAPS_SAVE_TO_CLIPBOARD:
		SaveAs_Image_Clipboard(false);
		break;

	case ID_CMD_MAPS_SAVE_TO_CLIPBOARD_LEGEND:
		SaveAs_Image_Clipboard(true);
		break;

	case ID_CMD_MAPS_SCALEBAR:
		Set_ScaleBar(!m_Parameters("SCALE_SHOW")->asBool());
		break;

	case ID_CMD_MAPS_SYNCHRONIZE:
		Set_Synchronising(!m_Parameters("SYNC_MAPS")->asBool());
		break;

	case ID_CMD_MAPS_GRATICULE_ADD:
		Add_Graticule();
		break;

	case ID_CMD_MAPS_BASEMAP_ADD:
		Add_BaseMap();
		break;

	case ID_CMD_MAPS_PROJECTION:
		Set_Projection();
		break;

	case ID_CMD_MAPS_SHOW:
		View_Toggle();
		break;

	case ID_CMD_MAPS_3D_SHOW:
		View_3D_Toggle();
		break;

	case ID_CMD_MAPS_LAYOUT_SHOW:
		View_Layout_Toggle();
		break;
	}

	return( true );
}

//---------------------------------------------------------
bool CWKSP_Map::On_Command_UI(wxUpdateUIEvent &event)
{
	switch( event.GetId() )
	{
	default:
		return( CWKSP_Base_Manager::On_Command_UI(event) );

	case ID_CMD_MAPS_SHOW:
		event.Check(m_pView != NULL);
		break;

	case ID_CMD_MAPS_3D_SHOW:
		event.Check(m_pView_3D != NULL);
		break;

	case ID_CMD_MAPS_LAYOUT_SHOW:
		event.Check(m_pLayout != NULL);
		break;

	case ID_CMD_MAPS_SAVE_IMAGE_ON_CHANGE:
		event.Check(is_Image_Save_Mode());
		break;

	case ID_CMD_MAPS_SCALEBAR:
		event.Check(is_ScaleBar());
		break;

	case ID_CMD_MAPS_GRATICULE_ADD:
	case ID_CMD_MAPS_BASEMAP_ADD:
		event.Enable(Get_Count() > 0 && m_Projection.is_Okay());
		break;
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Map::On_Create_Parameters(void)
{
	///////////////////////////////////////////////////////
	//-----------------------------------------------------
	m_Parameters.Add_Node(NULL,
		"NODE_GENERAL"	, _TL("General"),
		_TL("")
	);

	m_Parameters.Add_String(m_Parameters("NODE_GENERAL"),
		"NAME"			, _TL("Name"),
		_TL(""),
		&m_Name
	);

	m_Parameters.Add_Bool(m_Parameters("NODE_GENERAL"),
		"GOTO_NEWLAYER"	, _TL("Zoom to Added Layer"),
		_TL(""),
		g_pMaps->Get_Parameter("GOTO_NEWLAYER")->asBool()
	);

	m_Parameters.Add_Bool(m_Parameters("NODE_GENERAL"),
		"SYNC_MAPS"		, _TL("Synchronize Map Extents"),
		_TL(""),
		false
	);

	//-----------------------------------------------------
	m_Parameters.Add_Bool(m_Parameters("NODE_GENERAL"),
		"GCS_POSITION"	, _TL("Position as Geographic Coordinates"),
		_TL("Display mouse position in status bar as geographic coordinates."),
		false
	);

	//-----------------------------------------------------
	m_Parameters.Add_Bool(m_Parameters("NODE_GENERAL"),
		"SEL_EXTENT"	, _TL("Show Extent"),
		_TL("Display selected extent in map."),
		false
	);

	m_Parameters.Add_Color(m_Parameters("SEL_EXTENT"),
		"SEL_COLOUR"	, _TL("Colour"),
		_TL(""),
		SG_GET_RGB(222, 222, 222)
	);

	m_Parameters.Add_Double(m_Parameters("SEL_EXTENT"),
		"SEL_TRANSP"	, _TL("Transparency [%]"),
		_TL(""),
		50.0, 0.0, true, 100.0, true
	);

	//-----------------------------------------------------
	m_Parameters.Add_Bool(m_Parameters("NODE_GENERAL"),
		"FRAME_SHOW"	, _TL("Frame"),
		_TL(""),
		g_pMaps->Get_Parameter("FRAME_SHOW")->asBool()
	);

	m_Parameters.Add_Int(m_Parameters("FRAME_SHOW"),
		"FRAME_WIDTH"	, _TL("Size"),
		_TL(""),
		g_pMaps->Get_Parameter("FRAME_WIDTH")->asInt(), 5, true
	);

	//-----------------------------------------------------
	m_Parameters.Add_Bool(m_Parameters("NODE_GENERAL"),
		"NORTH_SHOW"	, _TL("North Arrow"),
		_TL(""),
		false
	);

	m_Parameters.Add_Double(m_Parameters("NORTH_SHOW"),
		"NORTH_ANGLE"	, _TL("Direction"),
		_TL(""),
		0.0, -180.0, true, 360.0, true
	);

	m_Parameters.Add_Double(m_Parameters("NORTH_SHOW"),
		"NORTH_SIZE"	, _TL("Size"),
		_TL("Size given as percentage of map size"),
		 5.0, 1.0, true, 100.0, true
	);

	m_Parameters.Add_Double(m_Parameters("NORTH_SHOW"),
		"NORTH_OFFSET_X", _TL("Horizontal Offset"),
		_TL("Offset given as percentage of map size"),
		 5.0, 0.0, true, 100.0, true
	);

	m_Parameters.Add_Double(m_Parameters("NORTH_SHOW"),
		"NORTH_OFFSET_Y", _TL("Vertical Offset"),
		_TL("Offset given as percentage of map size"),
		90.0, 0.0, true, 100.0, true
	);

	m_Parameters.Add_Bool(m_Parameters("NORTH_SHOW"),
		"NORTH_EXTENT"	, _TL("Relate to Extent"),
		_TL("Relate position and size to selected map extent."),
		false
	);

	//-----------------------------------------------------
	m_Parameters.Add_Bool(m_Parameters("NODE_GENERAL"),
		"SCALE_SHOW"	, _TL("Scale Bar"),
		_TL(""),
		g_pMaps->Get_Parameter("SCALE_BAR")->asBool()
	);

	m_Parameters.Add_Choice(m_Parameters("SCALE_SHOW"),
		"SCALE_STYLE"	, _TL("Style"),
		_TL(""),
		CSG_String::Format("%s|%s|",
			_TL("scale line"),
			_TL("alternating scale bar")
		), 1
	);

	m_Parameters.Add_Bool(m_Parameters("SCALE_SHOW"),
		"SCALE_UNIT"	, _TL("Unit"),
		_TL(""),
		true
	);

	m_Parameters.Add_Double(m_Parameters("SCALE_SHOW"),
		"SCALE_WIDTH"	, _TL("Width"),
		_TL("Width given as percentage of map size"),
		40.0, 1.0, true, 100.0, true
	);

	m_Parameters.Add_Double(m_Parameters("SCALE_SHOW"),
		"SCALE_HEIGHT"	, _TL("Height"),
		_TL("Height given as percentage of map size"),
		 4.0, 0.1, true, 100.0, true
	);

	m_Parameters.Add_Double(m_Parameters("SCALE_SHOW"),
		"SCALE_OFFSET_X", _TL("Horizontal Offset"),
		_TL("Offset given as percentage of map size"),
		 5.0, 0.0, true, 100.0, true
	);

	m_Parameters.Add_Double(m_Parameters("SCALE_SHOW"),
		"SCALE_OFFSET_Y", _TL("Vertical Offset"),
		_TL("Offset given as percentage of map size"),
		7.5, 0.0, true, 100.0, true
	);

	m_Parameters.Add_Bool(m_Parameters("SCALE_SHOW"),
		"SCALE_EXTENT"	, _TL("Relate to Extent"),
		_TL("Relate position and size to selected map extent."),
		false
	);

	//-----------------------------------------------------
	m_Parameters.Add_Node(NULL,
		"NODE_PRINT"		, _TL("Print Layout"),
		_TL("")
	);

	m_Parameters.Add_Bool(m_Parameters("NODE_PRINT"),
		"PRINT_LEGEND"		, _TL("Legend"),
		_TL(""),
		true
	);

	m_Parameters.Add_Bool(m_Parameters("NODE_PRINT"),
		"PRINT_SCALE_SHOW"	, _TL("Scale"),
		_TL(""),
		true
	);

	m_Parameters.Add_Int(m_Parameters("NODE_PRINT"),
		"PRINT_RESOLUTION"	, _TL("Display Resolution"),
		_TL(""),
		2, 1, true
	);

	m_Parameters.Add_Node(m_Parameters("NODE_PRINT"),
		"NODE_PRINT_FRAME"	, _TL("Frame"),
		_TL("")
	);

	m_Parameters.Add_Bool(m_Parameters("NODE_PRINT_FRAME"),
		"PRINT_FRAME_SHOW"	, _TL("Show"),
		_TL(""),
		true
	);

	m_Parameters.Add_Int(m_Parameters("NODE_PRINT_FRAME"),
		"PRINT_FRAME_WIDTH"	, _TL("Width"),
		_TL(""),
		7, 5, true
	);

	///////////////////////////////////////////////////////
	//-----------------------------------------------------
	m_Img_Parms.Set_Name(_TL("Save Map as Image..."));

	m_Img_Parms.Add_Node(NULL,
		"NODE_MAP"	, _TL("Map"),
		_TL("")
	);

	m_Img_Parms.Add_Int(m_Img_Parms("NODE_MAP"),
		"NX"		, _TL("Map Width [Pixels]"),
		_TL(""),
		800, 1, true
	);

	m_Img_Parms.Add_Int(m_Img_Parms("NODE_MAP"),
		"NY"		, _TL("Map Height [Pixels]"),
		_TL(""),
		600, 1, true
	);

	m_Img_Parms.Add_Int(m_Img_Parms("NODE_MAP"),
		"FR"		, _TL("Frame Width [Pixels]"),
		_TL(""),
		  20, 0, true
	);

	m_Img_Parms.Add_Bool(m_Img_Parms("NODE_MAP"),
		"REF"		, _TL("Save Georeference (world file)"),
		_TL(""),
		true
	);

	m_Img_Parms.Add_Bool(m_Img_Parms("NODE_MAP"),
		"KML"		, _TL("Save KML file"),
		_TL(""),
		true
	);

	m_Img_Parms.Add_Node(NULL,
		"NODE_LEGEND", _TL("Legend"),
		_TL("")
	);

	m_Img_Parms.Add_Bool(m_Img_Parms("NODE_LEGEND"),
		"LG"		, _TL("Save"),
		_TL(""),
		true
	);

	m_Img_Parms.Add_Double(m_Img_Parms("NODE_LEGEND"),
		"LZ"		, _TL("Zoom"),
		_TL(""),
		1.0, 0.0, true
	);
}

//---------------------------------------------------------
int CWKSP_Map::Get_Frame_Width(void)
{
	return( m_Parameters("FRAME_SHOW")->asBool() ? m_Parameters("FRAME_WIDTH")->asInt() : 0 );
}

//---------------------------------------------------------
int CWKSP_Map::Get_Print_Resolution(void)
{
	return( m_Parameters("PRINT_RESOLUTION")->asInt() );
}

//---------------------------------------------------------
int CWKSP_Map::Get_Print_Frame(void)
{
	return( m_Parameters("PRINT_FRAME_SHOW")->asBool() ? m_Parameters("PRINT_FRAME_WIDTH")->asInt() : 0 );
}

//---------------------------------------------------------
int CWKSP_Map::Get_Print_Legend(void)
{
	return( m_Parameters("PRINT_LEGEND")->asBool() ? 1 : 0 );
}

//---------------------------------------------------------
int CWKSP_Map::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter, int Flags)
{
	if( Flags & PARAMETER_CHECK_ENABLE )
	{
		if(	!SG_STR_CMP(pParameter->Get_Identifier(), "SEL_EXTENT") )
		{
			pParameters->Get_Parameter("SEL_COLOUR"    )->Set_Enabled(pParameter->asBool());
			pParameters->Get_Parameter("SEL_TRANSP"    )->Set_Enabled(pParameter->asBool());
		}

		if(	!SG_STR_CMP(pParameter->Get_Identifier(), "FRAME_SHOW") )
		{
			pParameters->Get_Parameter("FRAME_WIDTH"   )->Set_Enabled(pParameter->asBool());
		}

		if(	!SG_STR_CMP(pParameter->Get_Identifier(), "NORTH_SHOW") )
		{
			pParameters->Get_Parameter("NORTH_ANGLE"   )->Set_Enabled(pParameter->asBool());
			pParameters->Get_Parameter("NORTH_SIZE"    )->Set_Enabled(pParameter->asBool());
			pParameters->Get_Parameter("NORTH_OFFSET_X")->Set_Enabled(pParameter->asBool());
			pParameters->Get_Parameter("NORTH_OFFSET_Y")->Set_Enabled(pParameter->asBool());
			pParameters->Get_Parameter("NORTH_EXTENT"  )->Set_Enabled(pParameter->asBool());
		}

		if(	!SG_STR_CMP(pParameter->Get_Identifier(), "SCALE_SHOW") )
		{
			pParameters->Get_Parameter("SCALE_STYLE"   )->Set_Enabled(pParameter->asBool());
			pParameters->Get_Parameter("SCALE_UNIT"    )->Set_Enabled(pParameter->asBool());
			pParameters->Get_Parameter("SCALE_WIDTH"   )->Set_Enabled(pParameter->asBool());
			pParameters->Get_Parameter("SCALE_HEIGHT"  )->Set_Enabled(pParameter->asBool());
			pParameters->Get_Parameter("SCALE_OFFSET_X")->Set_Enabled(pParameter->asBool());
			pParameters->Get_Parameter("SCALE_OFFSET_Y")->Set_Enabled(pParameter->asBool());
			pParameters->Get_Parameter("SCALE_EXTENT"  )->Set_Enabled(pParameter->asBool());
		}
	}

	return( CWKSP_Base_Manager::On_Parameter_Changed(pParameters, pParameter, Flags) );
}

//---------------------------------------------------------
void CWKSP_Map::Parameters_Changed(void)
{
	m_Name	= m_Parameters("NAME")->asString();

	if( m_pView )
	{
		m_pView->SetTitle(m_Name);

		m_pView->Ruler_Set_Width(Get_Frame_Width());
	}

	View_Refresh(false);

	Set_Synchronising(m_Parameters("SYNC_MAPS")->asBool());

	CWKSP_Base_Manager::Parameters_Changed();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Map::Update(CWKSP_Layer *pLayer, bool bMapOnly)
{
	bool	bRefresh	= false;

	for(int i=0; i<Get_Count(); i++)
	{
		if( Get_Item(i)->Get_Type() == WKSP_ITEM_Map_Layer && ((CWKSP_Map_Layer *)Get_Item(i))->Get_Layer()->Update(pLayer) )
		{
			bRefresh	= true;

			if( !bMapOnly )
			{
				Get_Item(i)->Parameters_Changed();
			}
		}
	}

	if( bRefresh )
	{
		if( m_pView )
		{
			View_Refresh(bMapOnly);
		}

		_Img_Save_On_Change();

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
int CWKSP_Map::Get_Layer(CWKSP_Layer *pLayer)
{
	for(int i=0; i<Get_Count(); i++)
	{
		if( Get_Item(i)->Get_Type() == WKSP_ITEM_Map_Layer && ((CWKSP_Map_Layer *)Get_Item(i))->Get_Layer() == pLayer )
		{
			return( i );
		}
	}

	return( -1 );
}

//---------------------------------------------------------
CWKSP_Map_Layer * CWKSP_Map::Find_Layer(CWKSP_Layer *pLayer)
{
	for(int i=0; i<Get_Count(); i++)
	{
		if( Get_Item(i)->Get_Type() == WKSP_ITEM_Map_Layer && ((CWKSP_Map_Layer *)Get_Item(i))->Get_Layer() == pLayer )
		{
			return( (CWKSP_Map_Layer *)Get_Item(i) );
		}
	}

	return( NULL );
}

//---------------------------------------------------------
CWKSP_Map_Layer * CWKSP_Map::Add_Layer(CWKSP_Layer *pLayer)
{
	CWKSP_Map_Layer	*pItem;

	if( Get_Layer(pLayer) < 0 )
	{
		if( Get_Count() == 0 )
		{
			Set_Extent(pLayer->Get_Extent());
		}

		if( Get_Count() == 0 || (m_Parameters("GOTO_NEWLAYER")->asBool() && pLayer->Get_Extent().Get_XRange() > 0.0 && pLayer->Get_Extent().Get_YRange() > 0.0) )
		{
			Set_Extent(pLayer->Get_Extent());
		}

		if( Get_Count() == 0 )
		{
			m_Parameters("NAME")->Set_Value(pLayer->Get_Name().wx_str());

			Parameters_Changed();
		}

		Add_Item(pItem = new CWKSP_Map_Layer(pLayer));

		Move_Top(pItem);

		if( !m_Projection.is_Okay() && pLayer->Get_Object()->Get_Projection().is_Okay() )
		{
			m_Projection	= pLayer->Get_Object()->Get_Projection();
		}

		return( pItem );
	}

	return( NULL );
}

//---------------------------------------------------------
CWKSP_Map_Graticule * CWKSP_Map::Add_Graticule(CSG_MetaData *pEntry)
{
	CWKSP_Map_Graticule	*pItem;

	if( (Get_Count() > 0 && m_Projection.is_Okay()) || pEntry )
	{
		g_pMaps->Add(this);

		Add_Item(pItem = new CWKSP_Map_Graticule(pEntry));

		Move_Top(pItem);

		View_Refresh(true);

		return( pItem );
	}

	return( NULL );
}

//---------------------------------------------------------
class CWKSP_Map_BaseMap * CWKSP_Map::Add_BaseMap(CSG_MetaData *pEntry)
{
	CWKSP_Map_BaseMap	*pItem;

	if( (Get_Count() > 0 && m_Projection.is_Okay()) || pEntry )
	{
		g_pMaps->Add(this);

		Add_Item(pItem = new CWKSP_Map_BaseMap(pEntry));

		if( !pEntry )
		{
			pItem->Dlg_Parameters();
		}

		switch( pItem->Get_Parameter("POSITION")->asInt() )
		{
		default: Move_Bottom(pItem); break;
		case  0: Move_Top   (pItem); break;
		}

		View_Refresh(true);

		return( pItem );
	}

	return( NULL );
}

//---------------------------------------------------------
CWKSP_Base_Item * CWKSP_Map::Add_Copy(CWKSP_Base_Item *pItem)
{
	if( pItem )
	{
		if( pItem->Get_Type() == WKSP_ITEM_Map_Layer )
		{
			return( Add_Layer(((CWKSP_Map_Layer *)pItem)->Get_Layer()) );
		}

		if( pItem->Get_Type() == WKSP_ITEM_Map_Graticule )
		{
			CWKSP_Map_Graticule	*pCopy	= Add_Graticule();

			pCopy->Get_Parameters()->Assign_Values(pItem->Get_Parameters());

			return( pCopy );
		}

		if( pItem->Get_Type() == WKSP_ITEM_Map_BaseMap )
		{
			CSG_MetaData	Settings;

			pItem->Get_Parameters()->Serialize(Settings, true);

			CWKSP_Map_BaseMap	*pCopy	= Add_BaseMap(&Settings);

		//	pItem->Get_Parameters()->Assign_Values(pItem->Get_Parameters());

			return( pCopy );
		}
	}

	return( NULL );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Map::_Set_Extent(const CSG_Rect &Extent)
{
	if( Extent.Get_XRange() > 0.0 && Extent.Get_YRange() > 0.0 )
	{
		View_Refresh(true);

		for(int i=0; i<Get_Count(); i++)
		{
			if( Get_Item(i)->Get_Type() == WKSP_ITEM_Map_Layer )
			{
				((CWKSP_Map_Layer *)Get_Item(i))->Fit_Colors(Extent);
			}
		}

		if( m_Parameters("SYNC_MAPS")->asBool() )
		{
			_Synchronise_Extents();
		}
	}
}

//---------------------------------------------------------
void CWKSP_Map::Set_Extent(const CSG_Rect &Extent, bool bReset)
{
	CSG_Rect	r(Extent);

	if( r.Get_XRange() == 0.0 )
	{
		r.m_rect.xMin	-= 1.0;
		r.m_rect.xMax	+= 1.0;
	}

	if( r.Get_YRange() == 0.0 )
	{
		r.m_rect.yMin	-= 1.0;
		r.m_rect.yMax	+= 1.0;
	}

	if( m_Extents.Add_Extent(r, bReset) )
	{
		_Set_Extent(r);
	}
}

//---------------------------------------------------------
void CWKSP_Map::Set_Extent(void)
{
	CSG_Parameters	p(NULL, _TL("Map Extent"), _TL(""));

	p.Add_Range(NULL, "X", _TL("West-East")		, _TL(""), Get_Extent().Get_XMin(), Get_Extent().Get_XMax());
	p.Add_Range(NULL, "Y", _TL("South-North")	, _TL(""), Get_Extent().Get_YMin(), Get_Extent().Get_YMax());

	if( DLG_Parameters(&p) )
	{
		Set_Extent(CSG_Rect(
			p("X")->asRange()->Get_LoVal(),
			p("Y")->asRange()->Get_LoVal(),
			p("X")->asRange()->Get_HiVal(),
			p("Y")->asRange()->Get_HiVal())
		);
	}
}

//---------------------------------------------------------
void CWKSP_Map::Set_Extent_Full(void)
{
	CSG_Rect	Extent;

	for(int i=0, n=0; i<Get_Count(); i++)
	{
		if( Get_Item(i)->Get_Type() == WKSP_ITEM_Map_Layer )
		{
			CWKSP_Layer	*pLayer	= ((CWKSP_Map_Layer *)Get_Item(i))->Get_Layer();

			if( n++ == 0 )
			{
				Extent.Assign(pLayer->Get_Extent());
			}
			else
			{
				Extent.Union (pLayer->Get_Extent());
			}
		}
	}

	Set_Extent(Extent);
}

//---------------------------------------------------------
void CWKSP_Map::Set_Extent_Active(void)
{
	if( Get_Active_Layer() )
	{
		Set_Extent(Get_Active_Layer()->Get_Extent());
	}
}

//---------------------------------------------------------
void CWKSP_Map::Set_Extent_Selection(void)
{
	if( Get_Active_Layer() )
	{
		Set_Extent(Get_Active_Layer()->Edit_Get_Extent());
	}
}

//---------------------------------------------------------
bool CWKSP_Map::Set_Extent_Back(bool bCheck_Only)
{
	if( !m_Extents.is_First() )
	{
		if( !bCheck_Only )
		{
			_Set_Extent(m_Extents.Set_Back());
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_Map::Set_Extent_Forward(bool bCheck_Only)
{
	if( !m_Extents.is_Last() )
	{
		if( !bCheck_Only )
		{
			_Set_Extent(m_Extents.Set_Forward());
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_Map::is_North_Arrow(void)
{
	return( m_Parameters("NORTH_SHOW")->asBool() );
}

void CWKSP_Map::Set_North_Arrow(bool bOn)
{
	if( m_Parameters("NORTH_SHOW")->asBool() != bOn )
	{
		m_Parameters("NORTH_SHOW")->Set_Value(bOn ? 1 : 0);

		if( m_pView )
		{
			m_pView->Do_Update();
		}
	}
}

//---------------------------------------------------------
bool CWKSP_Map::is_ScaleBar(void)
{
	return( m_Parameters("SCALE_SHOW")->asBool() );
}

void CWKSP_Map::Set_ScaleBar(bool bOn)
{
	if( m_Parameters("SCALE_SHOW")->asBool() != bOn )
	{
		m_Parameters("SCALE_SHOW")->Set_Value(bOn ? 1 : 0);

		if( m_pView )
		{
			m_pView->Do_Update();
			m_pView->Ruler_Refresh();
		}
	}
}

//---------------------------------------------------------
bool CWKSP_Map::is_Synchronising(void)
{
	return( m_Parameters("SYNC_MAPS")->asBool() );
}

void CWKSP_Map::Set_Synchronising(bool bOn)
{
	if( m_Parameters("SYNC_MAPS")->asBool() != bOn )
	{
		m_Parameters("SYNC_MAPS")->Set_Value(bOn ? 1 : 0);
	}

	if( bOn )
	{
		_Synchronise_Extents();
	}
}

//---------------------------------------------------------
void CWKSP_Map::_Synchronise_Extents(void)
{
	for(int i=0; i<Get_Manager()->Get_Count(); i++)
	{
		if( Get_Manager()->Get_Item(i) != this )
		{
			((CWKSP_Map_Manager *)Get_Manager())->Get_Map(i)->Set_Extent(Get_Extent());
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Map::Set_Mouse_Position(const TSG_Point &Point)
{
	((CWKSP_Map_Manager *)Get_Manager())->Set_Mouse_Position(Point, m_Projection);
}

//---------------------------------------------------------
void CWKSP_Map::Set_CrossHair(const TSG_Point &Point, const CSG_Projection &Projection)
{
	if( m_pView )
	{
		if( !Projection.is_Okay() )
		{
			m_pView->Get_Map_Control()->Set_CrossHair(Point);
		}
		else if( m_Projection.is_Okay() )
		{
			TSG_Point	p	= Point;

			if( Get_Projected(Projection, m_Projection, p) )
			{
				if( m_pView )
				{
					m_pView->Get_Map_Control()->Set_CrossHair(p);
				}
			}
		}
	}
}

//---------------------------------------------------------
void CWKSP_Map::Set_CrossHair_Off(void)
{
	if( m_pView )
	{
		m_pView->Get_Map_Control()->Set_CrossHair_Off();
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Map::Set_Projection(void)
{
	CSG_Tool	*pTool	= SG_Get_Tool_Library_Manager().Get_Tool("pj_proj4", 15);	// CCRS_Picker

	if(	pTool )
	{
		pTool->Settings_Push();

		if( pTool->Get_Parameters()->Set_Parameter("CRS_PROJ4", m_Projection.Get_Proj4())
		&&	pTool->On_Before_Execution() && DLG_Parameters(pTool->Get_Parameters()) )
		{
			pTool->Execute();

			m_Projection.Assign(pTool->Get_Parameters()->Get("CRS_PROJ4")->asString(), SG_PROJ_FMT_Proj4);

			View_Refresh(false);
		}

		pTool->Settings_Pop();
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Map::View_Opened(MDI_ChildFrame *pView)
{
    if( wxDynamicCast(pView, CVIEW_Map   ) != NULL )    {	m_pView		= (CVIEW_Map    *)pView;	return( true );	}
    if( wxDynamicCast(pView, CVIEW_Map_3D) != NULL )	{	m_pView_3D	= (CVIEW_Map_3D *)pView;	return( true );	}
	if( wxDynamicCast(pView, CVIEW_Layout) != NULL )	{	m_pLayout	= (CVIEW_Layout *)pView;	return( true );	}

    return( false );
}

//---------------------------------------------------------
void CWKSP_Map::View_Closes(MDI_ChildFrame *pView)
{
	if( pView == m_pView    )	m_pView		= NULL;
	if( pView == m_pView_3D )	m_pView_3D	= NULL;
	if( pView == m_pLayout  )	m_pLayout	= NULL;
}

//---------------------------------------------------------
void CWKSP_Map::View_Refresh(bool bMapOnly)
{
	if( m_pView    )	m_pView   ->Do_Update();
	if( m_pView_3D )	m_pView_3D->Do_Update();
	if( m_pLayout  )	m_pLayout ->Do_Update();

	if( !bMapOnly && g_pACTIVE )
	{
		g_pACTIVE->Update_Description();

		if( g_pACTIVE->Get_Legend() )
		{
			g_pACTIVE->Get_Legend()->Refresh(true);
		}
	}

	_Set_Thumbnail();

	if( g_pMap_Buttons )
	{
		g_pMap_Buttons->Refresh();
	}
}

//---------------------------------------------------------
void CWKSP_Map::View_Show(bool bShow)
{
	if( bShow )
	{
		if( !m_pView )
		{
			new CVIEW_Map(this, Get_Frame_Width());
		}
		else
		{
			View_Refresh(false);

			m_pView->Activate();
		}
	}
	else if( m_pView )
	{
		m_pView->Destroy();
	}
}

//---------------------------------------------------------
void CWKSP_Map::View_Toggle(void)
{
	View_Show( m_pView == NULL );
}

//---------------------------------------------------------
void CWKSP_Map::View_3D_Show(bool bShow)
{
	if( bShow && !m_pView_3D )
	{
		new CVIEW_Map_3D(this);
	}
	else if( !bShow && m_pView_3D )
	{
		m_pView_3D->Destroy();
	}
}

//---------------------------------------------------------
void CWKSP_Map::View_3D_Toggle(void)
{
	View_3D_Show( m_pView_3D == NULL );
}

//---------------------------------------------------------
void CWKSP_Map::View_Layout_Show(bool bShow)
{
	if( bShow && !m_pLayout )
	{
		new CVIEW_Layout(m_pLayout_Info);
	}
	else if( !bShow && m_pLayout )
	{
		m_pLayout->Destroy();
	}
}

//---------------------------------------------------------
void CWKSP_Map::View_Layout_Toggle(void)
{
	View_Layout_Show( m_pLayout == NULL );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Rect CWKSP_Map::Get_World(wxRect rClient)
{
	double		d, dWorld, dClient;
	TSG_Rect	Extent;

	Extent	= Get_Extent().m_rect;

	dClient	= (double)rClient.GetHeight() / (double)rClient.GetWidth();
	dWorld	= Get_Extent().Get_YRange() / Get_Extent().Get_XRange();

	if( dWorld > dClient )
	{
		d			= (Get_Extent().Get_XRange() - Get_Extent().Get_YRange() / dClient) / 2.0;
		Extent.xMin	+= d;
		Extent.xMax	-= d;
	}
	else
	{
		d			= (Get_Extent().Get_YRange() - Get_Extent().Get_XRange() * dClient) / 2.0;
		Extent.yMin	+= d;
		Extent.yMax	-= d;
	}

	return( CSG_Rect(Extent) );
}

//---------------------------------------------------------
CSG_Point CWKSP_Map::Get_World(wxRect rClient, wxPoint Point)
{
	CSG_Rect	rWorld(Get_World(rClient));

	double	d	= rWorld.Get_XRange() / (double)rClient.GetWidth();

	return( CSG_Point(
		rWorld.Get_XMin() + d *                        Point.x,
		rWorld.Get_YMin() + d * (rClient.GetHeight() - Point.y)
	));
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define MASK_R	254
#define MASK_G	255
#define MASK_B	255

//---------------------------------------------------------
bool CWKSP_Map::Get_Image(wxImage &Image, CSG_Rect &rWorld)
{
	if( Image.GetWidth() > 0 && Image.GetHeight() > 0 )
	{
		wxBitmap	BMP(Image);
		wxMemoryDC	dc;

		dc.SelectObject(BMP);
		Draw_Map(dc, 1.0, wxRect(0, 0, Image.GetWidth(), Image.GetHeight()), LAYER_DRAW_FLAG_NOEDITS, SG_GET_RGB(MASK_R, MASK_G, MASK_B));
		dc.SelectObject(wxNullBitmap);

		rWorld	= Get_World(wxRect(0, 0, Image.GetWidth(), Image.GetHeight()));
		Image	= BMP.ConvertToImage();
		Image.SetMaskColour(MASK_R, MASK_G, MASK_B);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
void CWKSP_Map::SaveAs_Image(void)
{
	//-----------------------------------------------------
	if( View_Get() && View_Get()->Get_Map_Control() )
	{
		wxSize	s(View_Get()->Get_Map_Control()->GetClientSize());

		m_Img_Parms("NX")->Set_Value(s.x);
		m_Img_Parms("NY")->Set_Value(s.y);
	}

	if( DLG_Image_Save(m_Img_File, m_Img_Type) && DLG_Parameters(&m_Img_Parms) )
	{
		_Img_Save(m_Img_File, m_Img_Type);
	}
}

//---------------------------------------------------------
void CWKSP_Map::SaveAs_Image_Clipboard(bool bLegend)
{
	if( bLegend == false )
	{
		SaveAs_Image_Clipboard(
			Get_Manager()->Get_Parameter("CLIP_NX"   )->asInt(),
			Get_Manager()->Get_Parameter("CLIP_NY"   )->asInt(),
			Get_Manager()->Get_Parameter("CLIP_FRAME")->asInt()
		);

		return;
	}

	//-----------------------------------------------------
	// draw a legend...

	Set_Buisy_Cursor(true);

	int			Frame	= Get_Manager()->Get_Parameter("CLIP_LEGEND_FRAME")->asInt();
	double		Scale	= Get_Manager()->Get_Parameter("CLIP_LEGEND_SCALE")->asDouble();
	wxSize		s;
	wxBitmap	BMP;
	wxMemoryDC	dc;

	if( Get_Legend_Size(s, 1.0, Scale) )
	{
		s.x	+= 2 * Frame;
		s.y	+= 2 * Frame;

		BMP.Create(s.GetWidth(), s.GetHeight());
		dc.SelectObject(BMP);
		dc.SetBackground(*wxWHITE_BRUSH);
		dc.Clear();

		if( Frame > 0 )
		{
			dc.SetPen(Get_Color_asWX(Get_Manager()->Get_Parameter("CLIP_LEGEND_COLOR")->asInt()));
			Draw_Edge(dc, EDGE_STYLE_SIMPLE, 0, 0, s.x - 1, s.y - 1);
		}

		Draw_Legend(dc, 1.0, Scale, wxPoint(Frame, Frame));

		dc.SelectObject(wxNullBitmap);

		if( wxTheClipboard->Open() )
		{
			wxBitmapDataObject	*pBMP	= new wxBitmapDataObject;
			pBMP->SetBitmap(BMP);
			wxTheClipboard->SetData(pBMP);
			wxTheClipboard->Close();
		}
	}

	Set_Buisy_Cursor(false);
}

//---------------------------------------------------------
void CWKSP_Map::SaveAs_Image_Clipboard(int nx, int ny, int frame)
{
	Set_Buisy_Cursor(true);

	wxSize		s;
	wxRect		r;
	wxBitmap	BMP;
	wxMemoryDC	dc;

//	if( frame < 0 )	frame	= Get_Frame_Width();
	if( frame < 5 )	frame	= 0;

	r		= wxRect(0, 0, nx + 2 * frame, ny + 2 * frame);

	BMP.Create(r.GetWidth(), r.GetHeight());
	r.Deflate(frame);
	dc.SelectObject(BMP);
	dc.SetBackground(*wxWHITE_BRUSH);
	dc.Clear();

	Draw_Map(dc, 1.0, r, LAYER_DRAW_FLAG_NOEDITS);

	if( frame > 0 )
	{
		Draw_Frame(dc, r, frame);
	}

	dc.SelectObject(wxNullBitmap);

	if( wxTheClipboard->Open() )
	{
		wxBitmapDataObject	*pBMP	= new wxBitmapDataObject;
		pBMP->SetBitmap(BMP);
		wxTheClipboard->SetData(pBMP);
		wxTheClipboard->Close();
	}

	Set_Buisy_Cursor(false);
}

//---------------------------------------------------------
void CWKSP_Map::SaveAs_Image_To_KMZ(int nx, int ny)
{
	if( nx < 1 || ny < 1 )
	{
		return;
	}

	//-----------------------------------------------------
	CSG_Rect		Extent(Get_Extent());

	CSG_Parameters	P(NULL, _TL("Save Map to KMZ"), _TL(""));

	P.Add_FilePath(NULL, "FILE"    , _TL("File"    ), _TL(""), CSG_String::Format("%s|*.kmz|%s|*.*", _TL("KMZ Files"), _TL("All Files")), NULL, true);
	P.Add_Double  (NULL, "CELLSIZE", _TL("Cellsize"), _TL(""), SG_Get_Rounded_To_SignificantFigures(Extent.Get_XRange() / (double)nx, 2), 0.0, true);
	P.Add_Bool    (NULL, "LOAD"    , _TL("Load"    ), _TL(""), true);

	if( !DLG_Parameters(&P) || P("CELLSIZE")->asDouble() <= 0.0 )
	{
		return;
	}

	wxFileName	FileName(P("FILE")->asString());

	if( !FileName.IsOk() )
	{
		FileName.AssignTempFileName("saga_map");
		FileName.SetExt("kmz");

		if( !FileName.IsOk() )
		{
			DLG_Message_Show_Error(_TL("invalid file name"), _TL("Save Map to KMZ"));

			return;
		}
	}

	//-----------------------------------------------------
	nx	= Extent.Get_XRange() / P("CELLSIZE")->asDouble();
	ny	= Extent.Get_YRange() / P("CELLSIZE")->asDouble();

	wxImage		Image(nx, ny);

	if( !Get_Image(Image, Extent) )
	{
		return;
	}

	//-----------------------------------------------------
	CSG_Grid	Map(SG_DATATYPE_Int, Image.GetWidth(), Image.GetHeight(), Extent.Get_XRange() / (double)Image.GetWidth(), Extent.Get_XMin(), Extent.Get_YMin());

	Map.Set_Name(Get_Name().wx_str());
	Map.Set_NoData_Value(16711935);
	Map.Get_Projection().Create(m_Projection);

	for(int y=0, yy=Map.Get_NY()-1; y<Map.Get_NY(); y++, yy--)
	{
		for(int x=0; x<Map.Get_NX(); x++)
		{
			Map.Set_Value(x, y, SG_GET_RGB(Image.GetRed(x, yy), Image.GetGreen(x, yy), Image.GetBlue(x, yy)));
		}
	}

	//-----------------------------------------------------
	bool	bResult;

	SG_RUN_TOOL(bResult, "io_grid_image", 2,
			SG_TOOL_PARAMETER_SET("GRID"     , &Map)
		&&	SG_TOOL_PARAMETER_SET("FILE"     , FileName.GetFullPath().wc_str())
		&&	SG_TOOL_PARAMETER_SET("COLOURING", 4)	// rgb coded values
		&&	SG_TOOL_PARAMETER_SET("OUTPUT"   , 2)	// kmz file
	);

	if( bResult && P("LOAD")->asBool() )
	{
		Open_Application(FileName.GetFullPath());
	}
}

//---------------------------------------------------------
void CWKSP_Map::SaveAs_Image_To_Memory(int nx, int ny)
{
	if( nx < 1 || ny < 1 )
		return;

	CSG_Rect		Extent(Get_Extent());

	CSG_Parameters	P(NULL, _TL("Save To Memory Grid"), _TL(""));

	P.Add_Double(NULL, "CELLSIZE", _TL("Cellsize"), _TL(""), Extent.Get_XRange() / (double)nx, 0.0, true);

	if( !DLG_Parameters(&P) || P("CELLSIZE")->asDouble() <= 0.0 )
		return;

	nx	= Extent.Get_XRange() / P("CELLSIZE")->asDouble();
	ny	= Extent.Get_YRange() / P("CELLSIZE")->asDouble();

	wxImage		Image(nx, ny);

	if( Get_Image(Image, Extent) )
	{
		CSG_Grid	*pGrid	= SG_Create_Grid(SG_DATATYPE_Int, Image.GetWidth(), Image.GetHeight(), Extent.Get_XRange() / (double)Image.GetWidth(), Extent.Get_XMin(), Extent.Get_YMin());

		pGrid->Set_Name(Get_Name().wx_str());
		pGrid->Set_NoData_Value(16711935);
		pGrid->Get_Projection().Create(m_Projection);

		for(int y=0, yy=pGrid->Get_NY()-1; y<pGrid->Get_NY(); y++, yy--)
		{
			for(int x=0; x<pGrid->Get_NX(); x++)
			{
				pGrid->Set_Value(x, y, SG_GET_RGB(Image.GetRed(x, yy), Image.GetGreen(x, yy), Image.GetBlue(x, yy)));
			}
		}

		g_pData->Add(pGrid);
		g_pData->Get_Parameters(pGrid, &P);

		if( P("COLORS_TYPE") )
		{
			P("COLORS_TYPE")->Set_Value(6);	// RGB

			g_pData->Set_Parameters(pGrid, &P);
		}
	}
}

//---------------------------------------------------------
void CWKSP_Map::SaveAs_Image_To_Grid(CSG_Grid &Grid, int Size)
{
	if( Size < 1 )
		return;

	CSG_Rect	Extent(Get_Extent());
	wxImage		Image;

	if( Extent.Get_XRange() > Extent.Get_YRange() )
	{
		Image.Create(Size, Size * Extent.Get_YRange() / Extent.Get_XRange());
	}
	else
	{
		Image.Create(Size * Extent.Get_XRange() / Extent.Get_YRange(), Size);
	}

	if( Get_Image(Image, Extent) )
	{
		Grid.Create(SG_DATATYPE_Int, Image.GetWidth(), Image.GetHeight(), Extent.Get_XRange() / (double)Image.GetWidth(), Extent.Get_XMin(), Extent.Get_YMin());
		Grid.Set_NoData_Value(SG_GET_RGB(MASK_R, MASK_G, MASK_B));

		for(int y=0, yy=Grid.Get_NY()-1; y<Grid.Get_NY(); y++, yy--)
		{
			for(int x=0; x<Grid.Get_NX(); x++)
			{
				Grid.Set_Value(x, y, SG_GET_RGB(Image.GetRed(x, yy), Image.GetGreen(x, yy), Image.GetBlue(x, yy)));
			}
		}
	}
}

//---------------------------------------------------------
void CWKSP_Map::SaveAs_Image_On_Change(void)
{
	if( m_Img_bSave )
	{
		m_Img_bSave	= false;
	}
	else if( DLG_Image_Save(m_Img_File, m_Img_Type) && DLG_Parameters(&m_Img_Parms) )
	{
		m_Img_bSave	= true;
		m_Img_Count	= 0;
	}
}

//---------------------------------------------------------
void CWKSP_Map::_Img_Save_On_Change(void)
{
	if( m_Img_bSave )
	{
		wxFileName	fn(m_Img_File), file(m_Img_File);

		file.SetName(wxString::Format("%s_%03d", fn.GetName().c_str(), m_Img_Count++));

		_Img_Save(file.GetFullPath(), m_Img_Type);
	}
}

//---------------------------------------------------------
void CWKSP_Map::_Img_Save(wxString file, int type)
{
	int			nx, ny, Frame;
	wxSize		s;
	wxRect		r;
	wxBitmap	BMP;
	wxMemoryDC	dc;

	Set_Buisy_Cursor(true);

	nx		= m_Img_Parms("NX")->asInt();
	ny		= m_Img_Parms("NY")->asInt();
	Frame	= m_Img_Parms("FR")->asInt();	if( Frame < 5 )	Frame	= 0;
	r		= wxRect(0, 0, nx + 2 * Frame, ny + 2 * Frame);

	BMP.Create(r.GetWidth(), r.GetHeight());
	r.Deflate(Frame);
	dc.SelectObject(BMP);
	dc.SetBackground(*wxWHITE_BRUSH);
	dc.Clear();

	Draw_Map(dc, 1.0, r, LAYER_DRAW_FLAG_NOEDITS);
	Draw_Frame(dc, r, Frame);

	dc.SelectObject(wxNullBitmap);
	BMP.SaveFile(file, (wxBitmapType)type);

	if( m_Img_Parms("REF")->asBool() )
	{
		CSG_File	Stream;
		wxFileName	fn(file);

		switch( type )
		{
		default                : fn.SetExt("world"); break;
		case wxBITMAP_TYPE_BMP : fn.SetExt("bpw"  ); break;
		case wxBITMAP_TYPE_GIF : fn.SetExt("gfw"  ); break;
		case wxBITMAP_TYPE_JPEG: fn.SetExt("jgw"  ); break;
		case wxBITMAP_TYPE_PNG : fn.SetExt("pgw"  ); break;
		case wxBITMAP_TYPE_PCX : fn.SetExt("pxw"  ); break;
		case wxBITMAP_TYPE_TIF : fn.SetExt("tfw"  ); break;
		}

		if( Stream.Open(fn.GetFullPath().wx_str(), SG_FILE_W, false) )
		{
			CSG_Rect	rWorld(Get_World(r));
			double		d	= rWorld.Get_XRange() / r.GetWidth();

			Stream.Printf("%.10f\n%.10f\n%.10f\n%.10f\n%.10f\n%.10f\n",
				d, 0.0, 0.0,-d,
				rWorld.Get_XMin() - Frame * d,
				rWorld.Get_YMax() + Frame * d
			);
		}
	}

	if( m_Img_Parms("KML")->asBool() )
	{
		CSG_File	Stream;
		wxFileName	fn(file);

		fn.SetExt("kml");

		if( Stream.Open(fn.GetFullPath().wx_str(), SG_FILE_W, false) )
		{
			CSG_Rect	rWorld(Get_World(r));
			double		d	= rWorld.Get_XRange() / r.GetWidth();

			fn.Assign(file);

			Stream.Printf("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
			Stream.Printf("<kml xmlns=\"http://www.opengis.net/kml/2.2\">\n");
			Stream.Printf("  <Folder>\n");
			Stream.Printf("    <name>Maps exported from SAGA</name>\n");
			Stream.Printf("    <description>System for Automated Geoscientific Analyses - www.saga-gis.org</description>\n");
			Stream.Printf("    <GroundOverlay>\n");
			Stream.Printf("      <name>%s</name>\n"              , Get_Name().wx_str());
			Stream.Printf("      <description>%s</description>\n", Get_Description().wx_str());
			Stream.Printf("      <Icon>\n");
			Stream.Printf("        <href>%s</href>\n"            , fn.GetFullName().wx_str());
			Stream.Printf("      </Icon>\n");
			Stream.Printf("      <LatLonBox>\n");
			Stream.Printf("        <north>%f</north>\n"          , rWorld.Get_YMax() + Frame * d);
			Stream.Printf("        <south>%f</south>\n"          , rWorld.Get_YMin() - Frame * d);
			Stream.Printf("        <east>%f</east>\n"            , rWorld.Get_XMax() + Frame * d);
			Stream.Printf("        <west>%f</west>\n"            , rWorld.Get_XMin() - Frame * d);
			Stream.Printf("        <rotation>0.0</rotation>\n");
			Stream.Printf("      </LatLonBox>\n");
			Stream.Printf("    </GroundOverlay>\n");
			Stream.Printf("  </Folder>\n");
			Stream.Printf("</kml>\n");
		}
	}

	if( m_Img_Parms("LG")->asBool() && Get_Legend_Size(s, 1.0, m_Img_Parms("LZ")->asDouble()) )
	{
		wxFileName	fn(file);
		file	= fn.GetName() + "_legend";
		fn.SetName(file);
		file	= fn.GetFullPath();

		BMP.Create(s.GetWidth(), s.GetHeight());
		dc.SelectObject(BMP);
		dc.SetBackground(*wxWHITE_BRUSH);
		dc.Clear();

		Draw_Legend(dc, 1.0, m_Img_Parms("LZ")->asDouble(), wxPoint(0, 0));

		dc.SelectObject(wxNullBitmap);
		BMP.SaveFile(file, (wxBitmapType)type);
	}

	Set_Buisy_Cursor(false);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
const wxBitmap & CWKSP_Map::Get_Thumbnail(int dx, int dy)
{
	if( dx > 0 && dy > 0 && (!m_Thumbnail.IsOk() || m_Thumbnail.GetWidth() != dx || m_Thumbnail.GetHeight() != dy) )
	{
		m_Thumbnail.Create(dx, dy);

		_Set_Thumbnail();
	}

	return( m_Thumbnail );
}

//---------------------------------------------------------
bool CWKSP_Map::_Set_Thumbnail(void)
{
	if( m_Thumbnail.IsOk() && m_Thumbnail.GetWidth() > 0 && m_Thumbnail.GetHeight() > 0 )
	{
		wxMemoryDC		dc;
		wxRect			r(0, 0, m_Thumbnail.GetWidth(), m_Thumbnail.GetHeight());

		dc.SelectObject(m_Thumbnail);
		dc.SetBackground(*wxWHITE_BRUSH);
		dc.Clear();

		Draw_Map(dc, Get_Extent(), 1.0, r, LAYER_DRAW_FLAG_NOEDITS|LAYER_DRAW_FLAG_NOLABELS);

		dc.SelectObject(wxNullBitmap);

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Map::Draw_Map(wxDC &dc, double Zoom, const wxRect &rClient, int Flags, int Background)
{
	Draw_Map(dc, Get_World(rClient), Zoom, rClient, Flags, Background);
}

//---------------------------------------------------------
void CWKSP_Map::Draw_Map(wxDC &dc, const CSG_Rect &rWorld, double Zoom, const wxRect &rClient, int Flags, int Background)
{
	CWKSP_Map_DC	dc_Map(rWorld, rClient, Zoom, Background);

	int	Flag_Labels	= !(Flags & LAYER_DRAW_FLAG_NOLABELS) ? 0 : LAYER_DRAW_FLAG_NOLABELS;

	//-----------------------------------------------------
	for(int i=Get_Count()-1; i>=0; i--)
	{
		switch( Get_Item(i)->Get_Type() )
		{
		case WKSP_ITEM_Map_Layer:
			{
				CWKSP_Map_Layer	*pLayer	= (CWKSP_Map_Layer *)Get_Item(i);

				if( pLayer->do_Show() && pLayer->Get_Layer()->do_Show(Get_Extent()) )
				{
					pLayer->Get_Layer()->Draw(dc_Map, !(Flags & LAYER_DRAW_FLAG_NOEDITS) && pLayer->Get_Layer() == Get_Active_Layer() ? Flags : Flag_Labels);
				}
			}
			break;

		case WKSP_ITEM_Map_Graticule:
			{
				CWKSP_Map_Graticule	*pLayer	= (CWKSP_Map_Graticule *)Get_Item(i);

				if( pLayer->do_Show() )//&& pLayer->Get_Graticule(Get_Extent()) )
				{
					pLayer->Draw(dc_Map);
				}
			}
			break;

		case WKSP_ITEM_Map_BaseMap:
			{
				CWKSP_Map_BaseMap	*pLayer	= (CWKSP_Map_BaseMap *)Get_Item(i);

				if( pLayer->do_Show() )//&& pLayer->Get_Graticule(Get_Extent()) )
				{
					pLayer->Draw(dc_Map);
				}
			}
			break;

		default:
			break;
		}
	}

	//-----------------------------------------------------
	Draw_Extent     (dc_Map, rWorld, rClient);
	Draw_ScaleBar   (dc_Map, rWorld, rClient);
	Draw_North_Arrow(dc_Map, rWorld, rClient);

	//-----------------------------------------------------
	dc_Map.Draw(dc);
}

//---------------------------------------------------------
void CWKSP_Map::Draw_Frame(wxDC &dc, wxRect rMap, int Width)
{
	Draw_Frame(dc, Get_World(rMap), rMap, Width);
}

void CWKSP_Map::Draw_Frame(wxDC &dc, const CSG_Rect &rWorld, wxRect rMap, int Width)
{
	wxRect		r, rFrame(rMap);

	Draw_Edge(dc, EDGE_STYLE_SIMPLE, rMap.GetLeft(), rMap.GetTop(), rMap.GetRight(), rMap.GetBottom());

	rFrame.Inflate(Width);

	r		= wxRect(rMap.GetLeft()    , rFrame.GetTop()    , rMap.GetWidth(), Width);
	Draw_Scale(dc, r, rWorld.Get_XMin(), rWorld.Get_XMax()  , true , true , false);

	r		= wxRect(rFrame.GetLeft()  , rMap.GetTop()      , Width, rMap.GetHeight());
	Draw_Scale(dc, r, rWorld.Get_YMin(), rWorld.Get_YMax()  , false, false, false);

	if( is_ScaleBar() )
	{
		r		= wxRect(rMap.GetLeft()    , rMap.GetBottom()   , rMap.GetWidth(), Width);
		Draw_Scale(dc, r, rWorld.Get_XMin(), rWorld.Get_XMax()  , true , true , false);

		r		= wxRect(rMap.GetRight()   , rMap.GetTop()      , Width, rMap.GetHeight());
		Draw_Scale(dc, r, rWorld.Get_YMin(), rWorld.Get_YMax()  , false, false, false);
	}
	else
	{
		r		= wxRect(rMap.GetLeft()    , rMap.GetBottom()   , rMap.GetWidth(), Width);
		Draw_Scale(dc, r, 0.0              , rWorld.Get_XRange(), true , true , true);

		r		= wxRect(rMap.GetRight()   , rMap.GetTop()      , Width, rMap.GetHeight());
		Draw_Scale(dc, r, 0.0              , rWorld.Get_YRange(), false, false, true);
	}

	Draw_Edge(dc, EDGE_STYLE_SIMPLE, rFrame.GetLeft(), rFrame.GetTop(), rFrame.GetRight(), rFrame.GetBottom());
}

//---------------------------------------------------------
bool CWKSP_Map::Draw_Legend(wxDC &dc, double Zoom_Map, double Zoom, wxPoint Position, wxSize *pSize)
{
	wxSize	s, Size(0, 0);

	for(int i=0; i<Get_Count(); i++)
	{
		if( Get_Item(i)->Get_Type() == WKSP_ITEM_Map_Layer )
		{
			CWKSP_Layer	*pLayer	= ((CWKSP_Map_Layer *)Get_Item(i))->Get_Layer();

			if( pLayer->do_Legend() )
			{
				pLayer->Get_Legend()->Draw(dc, Zoom, Zoom_Map, Position, &s);

				if( 1 )	// m_pLayout->Get_Legend()->Get_Orientation() == LEGEND_VERTICAL )
				{
					s.y			+= (int)(Zoom * LEGEND_SPACE);
					Position.y	+= s.y;
					Size.y		+= s.y;

					if( Size.x < s.x )
						Size.x	= s.x;
				}
				else
				{
					s.x			+= (int)(Zoom * LEGEND_SPACE);
					Position.x	+= s.x;
					Size.x		+= s.x;

					if( Size.y < s.y )
						Size.y	= s.y;
				}
			}
		}
	}

	if( pSize )
	{
		*pSize	= Size;
	}

	return( Size.GetX() > 0 || Size.GetY() > 0 );
}

//---------------------------------------------------------
bool CWKSP_Map::Get_Legend_Size(wxSize &Size, double Zoom_Map, double Zoom)
{
	wxBitmap	bmp(10, 10);
	wxMemoryDC	dc(bmp);

	return( Draw_Legend(dc, Zoom_Map, Zoom, wxPoint(0, 0), &Size) );
}

//---------------------------------------------------------
bool CWKSP_Map::Draw_North_Arrow(CWKSP_Map_DC &dc_Map, const CSG_Rect &rWorld, const wxRect &rClient)
{
	if( !m_Parameters("NORTH_SHOW")->asBool() )
	{
		return( true );
	}

	const double	Arrow[3][2]	= { { 0.0, 1.0 }, { 0.5, -1.0 }, { 0.0, -0.50 } };

	wxRect	r	= !m_Parameters("NORTH_EXTENT")->asBool() ? wxRect(0, 0, rClient.GetWidth(), rClient.GetHeight()) : wxRect(
		(int)dc_Map.xWorld2DC(Get_Extent().Get_XMin()),
		(int)dc_Map.yWorld2DC(Get_Extent().Get_YMax()),
		(int)(dc_Map.m_World2DC * Get_Extent().Get_XRange()),
		(int)(dc_Map.m_World2DC * Get_Extent().Get_YRange())
	);

	double	cos_a	= cos(-m_Parameters("NORTH_ANGLE")->asDouble() * M_DEG_TO_RAD);
	double	sin_a	= sin(-m_Parameters("NORTH_ANGLE")->asDouble() * M_DEG_TO_RAD);
	double	scale	= m_Parameters("NORTH_SIZE")->asDouble() * 0.01 * M_GET_MIN(r.GetWidth(), r.GetHeight());

	int		xOff	= r.GetX() + (int)(0.5 +                 m_Parameters("NORTH_OFFSET_X")->asDouble() * 0.01 * r.GetWidth ());
	int		yOff	= r.GetY() + (int)(0.5 + r.GetHeight() - m_Parameters("NORTH_OFFSET_Y")->asDouble() * 0.01 * r.GetHeight());

	for(int side=0; side<=1; side++)
	{
		wxPoint	Points[3];

		for(int i=0; i<3; i++)
		{
			double	x	= scale * Arrow[i][0] * (side ? 1 : -1);
			double	y	= scale * Arrow[i][1];

			Points[i].x	= xOff + (int)(0.5 + cos_a * x - sin_a * y);
			Points[i].y	= yOff - (int)(0.5 + sin_a * x + cos_a * y);
		}

		if( side == 0 )
		{
		//	dc_Map.dc.SetPen     (wxPen  (*wxWHITE, 3));
		//	dc_Map.dc.DrawLines  (3, Points);

			dc_Map.dc.SetPen     (wxPen  (*wxBLACK, 0));
			dc_Map.dc.SetBrush   (wxBrush(*wxBLACK));
			dc_Map.dc.DrawPolygon(3, Points);
            dc_Map.dc.DrawPolygon(3, Points);
		}
		else
		{
			dc_Map.dc.SetPen     (wxPen  (*wxBLACK, 0));
			dc_Map.dc.SetBrush   (wxBrush(*wxWHITE));
			dc_Map.dc.DrawPolygon(3, Points);
		//	dc_Map.dc.DrawLines  (3, Points);
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CWKSP_Map::Draw_ScaleBar(CWKSP_Map_DC &dc_Map, const CSG_Rect &rWorld, const wxRect &rClient)
{
	if( !m_Parameters("SCALE_SHOW")->asBool() )
	{
		return( true );
	}

	double	dWidth	= 0.01 * m_Parameters("SCALE_WIDTH")->asDouble();

	wxRect	r	= !m_Parameters("SCALE_EXTENT")->asBool() ? wxRect(0, 0, rClient.GetWidth(), rClient.GetHeight()) : wxRect(
		(int) dc_Map .xWorld2DC  (Get_Extent().Get_XMin  ()),
		(int) dc_Map .yWorld2DC  (Get_Extent().Get_YMax  ()),
		(int)(dc_Map.m_World2DC * Get_Extent().Get_XRange()),
		(int)(dc_Map.m_World2DC * Get_Extent().Get_YRange())
	);

	r	= wxRect(
		(int)(0.5 + r.GetWidth () * 0.01 * m_Parameters("SCALE_OFFSET_X")->asDouble()) + r.GetX(), r.GetY() + r.GetHeight() -
		(int)(0.5 + r.GetHeight() * 0.01 * m_Parameters("SCALE_OFFSET_Y")->asDouble()),
		(int)(0.5 + r.GetWidth () * dWidth),
		(int)(0.5 + r.GetHeight() * 0.01 * m_Parameters("SCALE_HEIGHT"  )->asDouble())
	);

	dWidth	*= rWorld.Get_XRange();

	CSG_String	Unit;

	if( m_Projection.is_Okay() && m_Parameters("SCALE_UNIT")->asBool() )
	{
		Unit	= SG_Get_Projection_Unit_Name(m_Projection.Get_Unit(), true);

		if( Unit.is_Empty() )	Unit	= m_Projection.Get_Unit_Name();

		if( m_Projection.Get_Unit() == SG_PROJ_UNIT_Meter && dWidth > 10000.0 )
		{
			Unit	 = SG_Get_Projection_Unit_Name(SG_PROJ_UNIT_Kilometer, true);
			dWidth	/= 1000.0;
		}
	}

	int	Style	= SCALE_STYLE_LINECONN|SCALE_STYLE_GLOOMING;

	if( m_Parameters("SCALE_STYLE")->asInt() == 1 )
		Style	|= SCALE_STYLE_BLACKWHITE;

	Draw_Scale(dc_Map.dc, r, 0.0, dWidth, SCALE_HORIZONTAL, SCALE_TICK_TOP, Style, Unit.c_str());

	return( true );
}

//---------------------------------------------------------
bool CWKSP_Map::Draw_Extent(CWKSP_Map_DC &dc_Map, const CSG_Rect &rWorld, const wxRect &rClient)
{
	if( !m_Parameters("SEL_EXTENT")->asBool() || rWorld == Get_Extent() )
	{
		return( true );
	}

	if( dc_Map.IMG_Draw_Begin(m_Parameters("SEL_TRANSP")->asDouble() / 100.0) )
	{
		int	Colour	= m_Parameters("SEL_COLOUR")->asColor();

		wxRect	r(0, 0, rClient.GetWidth(), rClient.GetHeight()); r.Inflate(1);

		if( rWorld.Get_XRange() > Get_Extent().Get_XRange() )
		{
			int	d	= (int)(0.5 + dc_Map.m_World2DC * (rWorld.Get_XRange() - Get_Extent().Get_XRange()) / 2.0);

			dc_Map.IMG_Set_Rect(r.GetLeft (), r.GetTop(), r.GetLeft () + d, r.GetBottom(), Colour);
			dc_Map.IMG_Set_Rect(r.GetRight(), r.GetTop(), r.GetRight() - d, r.GetBottom(), Colour);
		}
		else
		{
			int	d	= (int)(0.5 + dc_Map.m_World2DC * (rWorld.Get_YRange() - Get_Extent().Get_YRange()) / 2.0);

			dc_Map.IMG_Set_Rect(r.GetLeft(), r.GetTop   (), r.GetRight(), r.GetTop   () + d, Colour);
			dc_Map.IMG_Set_Rect(r.GetLeft(), r.GetBottom(), r.GetRight(), r.GetBottom() - d, Colour);
		}

		dc_Map.IMG_Draw_End();
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
