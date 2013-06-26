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

	m_Name.Printf(wxT("%02d. %s"), ++iMap, _TL("Map"));

	m_pView			= NULL;
	m_pView_3D		= NULL;
	m_pLayout		= NULL;
	m_pLayout_Info	= new CVIEW_Layout_Info(this);

	m_bScaleBar		= g_pMaps->Get_Parameter("SCALE_BAR")->asBool();
	m_bSynchronise	= false;
	m_Img_bSave		= false;

	On_Create_Parameters();
}

//---------------------------------------------------------
CWKSP_Map::~CWKSP_Map(void)
{
	View_Show			(false);
	View_3D_Show		(false);
	View_Layout_Show	(false);

	delete(m_pLayout_Info);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
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

	s.Append(wxString::Format(wxT("<b>%s</b><table border=\"0\">"),
		_TL("Map")
	));

	s.Append(wxString::Format(wxT("<tr><td>%s</td><td>%s</td></tr>"),
		_TL("Name")					, m_Name.c_str()
	));

	s.Append(wxString::Format(wxT("<tr><td>%s</td><td>%d</td></tr>"),
		_TL("Layers")					, Get_Count()
	));

	s.Append(wxT("</table>"));

	return( s );
}

//---------------------------------------------------------
wxMenu * CWKSP_Map::Get_Menu(void)
{
	wxMenu	*pMenu;

	pMenu	= new wxMenu(_TL("Map"));

	CMD_Menu_Add_Item(pMenu, false, ID_CMD_WKSP_ITEM_CLOSE);
	CMD_Menu_Add_Item(pMenu,  true, ID_CMD_MAPS_SHOW);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_MAPS_SAVE_IMAGE);
	CMD_Menu_Add_Item(pMenu,  true, ID_CMD_MAPS_SAVE_IMAGE_ON_CHANGE);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_MAPS_SAVE_TO_CLIPBOARD);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_MAPS_SAVE_TO_CLIPBOARD_LEGEND);
	CMD_Menu_Add_Item(pMenu,  true, ID_CMD_MAPS_3D_SHOW);
	CMD_Menu_Add_Item(pMenu,  true, ID_CMD_MAPS_LAYOUT_SHOW);
	CMD_Menu_Add_Item(pMenu,  true, ID_CMD_MAP_SYNCHRONIZE);
	CMD_Menu_Add_Item(pMenu,  true, ID_CMD_MAP_SCALEBAR);

	return( pMenu );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
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

	case ID_CMD_MAP_SCALEBAR:
		Set_ScaleBar(!m_bScaleBar);
		break;

	case ID_CMD_MAP_SYNCHRONIZE:
		Set_Synchronising(!m_bSynchronise);
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

	case ID_CMD_MAP_SCALEBAR:
		event.Check(is_ScaleBar());
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
void CWKSP_Map::On_Create_Parameters(void)
{
	CSG_Parameter	*pNode_0, *pNode_1;

	//-----------------------------------------------------
	pNode_0	= m_Parameters.Add_Node(
		NULL	, "NODE_GENERAL"	, _TL("General"),
		_TL("")
	);

	m_Parameters.Add_String(
		pNode_0	, "NAME"			, _TL("Name"),
		_TL(""),
		&m_Name
	);

	m_Parameters.Add_Value(
		pNode_0	, "GOTO_NEWLAYER"	, _TL("Zoom to added layer"),
		_TL(""),
		PARAMETER_TYPE_Bool, g_pMaps->Get_Parameter("GOTO_NEWLAYER")->asBool()
	);

	//-----------------------------------------------------
	pNode_0	= m_Parameters.Add_Node(
		NULL	, "NODE_FRAME"		, _TL("Frame"),
		_TL("")
	);

	m_Parameters.Add_Value(
		pNode_0	, "FRAME_SHOW"		, _TL("Show"),
		_TL(""),
		PARAMETER_TYPE_Bool, g_pMaps->Get_Parameter("FRAME_SHOW")->asBool()
	);

	m_Parameters.Add_Value(
		pNode_0	, "FRAME_WIDTH"		, _TL("Width"),
		_TL(""),
		PARAMETER_TYPE_Int, g_pMaps->Get_Parameter("FRAME_WIDTH")->asInt(), 5, true
	);

	//-----------------------------------------------------
	pNode_0	= m_Parameters.Add_Node(
		NULL	, "NODE_PRINT"		, _TL("Print Layout"),
		_TL("")
	);

	m_Parameters.Add_Choice(
		pNode_0	, "PRINT_LEGEND"	, _TL("Show Legend"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("no"),
			_TL("yes")
		), 1
	);

	m_Parameters.Add_Value(
		pNode_0	, "PRINT_RESOLUTION"	, _TL("Display Resolution"),
		_TL(""),
		PARAMETER_TYPE_Int, 2, 1, true
	);

	pNode_1	= m_Parameters.Add_Node(
		pNode_0	, "NODE_PRINT_FRAME"	, _TL("Frame"),
		_TL("")
	);

	m_Parameters.Add_Value(
		pNode_1	, "PRINT_FRAME_SHOW"	, _TL("Show"),
		_TL(""),
		PARAMETER_TYPE_Bool, true
	);

	m_Parameters.Add_Value(
		pNode_1	, "PRINT_FRAME_WIDTH"	, _TL("Width"),
		_TL(""),
		PARAMETER_TYPE_Int, 7, 5, true
	);

	m_Parameters.Add_Value(
		pNode_0	, "PRINT_SCALE_SHOW"	, _TL("Show Scale"),
		_TL(""),
		PARAMETER_TYPE_Bool, true
	);

	//-----------------------------------------------------
	m_Img_Parms.Set_Name(_TL("Save Map as Image..."));

	pNode_0	= m_Img_Parms.Add_Node(NULL, "NODE_MAP", _TL("Map"), _TL(""));

	m_Img_Parms.Add_Value(
		pNode_0	, "NX"	, _TL("Map Width [Pixels]"),
		_TL(""),
		PARAMETER_TYPE_Int, 800	, 1, true
	);

	m_Img_Parms.Add_Value(
		pNode_0	, "NY"	, _TL("Map Height [Pixels]"),
		_TL(""),
		PARAMETER_TYPE_Int, 600	, 1, true
	);

	m_Img_Parms.Add_Value(
		pNode_0	, "FR"	, _TL("Frame Width [Pixels]"),
		_TL(""),
		PARAMETER_TYPE_Int, 20	, 0, true
	);

	m_Img_Parms.Add_Value(
		pNode_0	, "REF"	, _TL("Save Georeference (world file)"),
		_TL(""),
		PARAMETER_TYPE_Bool, 1
	);

	m_Img_Parms.Add_Value(
		pNode_0	, "KML"	, _TL("Save KML file"),
		_TL(""),
		PARAMETER_TYPE_Bool, 1
	);

	pNode_0	= m_Img_Parms.Add_Node(NULL, "NODE_LEGEND", _TL("Legend"), _TL(""));

	m_Img_Parms.Add_Value(
		pNode_0	, "LG"	, _TL("Save"),
		_TL(""),
		PARAMETER_TYPE_Bool, 1
	);

	m_Img_Parms.Add_Value(
		pNode_0	, "LZ"	, _TL("Zoom"),
		_TL(""),
		PARAMETER_TYPE_Double, 1.0, 0, true
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
	return( m_Parameters("PRINT_LEGEND")->asInt() );
}

//---------------------------------------------------------
int CWKSP_Map::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter, int Flags)
{
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

	CWKSP_Base_Manager::Parameters_Changed();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Map_Layer * CWKSP_Map::Find_Layer(CWKSP_Layer *pLayer)
{
	for(int i=0; i<Get_Count(); i++)
	{
		if( pLayer == Get_Layer(i)->Get_Layer() )
		{
			return( Get_Layer(i) );
		}
	}

	return( NULL );
}

//---------------------------------------------------------
int CWKSP_Map::Get_Layer(CWKSP_Layer *pLayer)
{
	for(int i=0; i<Get_Count(); i++)
	{
		if( pLayer == Get_Layer(i)->Get_Layer() )
		{
			return( i );
		}
	}

	return( -1 );
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

		return( pItem );
	}

	return( NULL );
}

//---------------------------------------------------------
bool CWKSP_Map::Update(CWKSP_Layer *pLayer, bool bMapOnly)
{
	bool	bRefresh	= false;

	for(int i=0; i<Get_Count(); i++)
	{
		if( Get_Layer(i)->Get_Layer()->Update(pLayer) )
		{
			bRefresh	= true;

			if( !bMapOnly )
			{
				Get_Layer(i)->Parameters_Changed();
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Map::_Set_Extent(const CSG_Rect &Extent)
{
	if( Extent.Get_XRange() > 0.0 && Extent.Get_YRange() > 0.0 )
	{
		View_Refresh(true);

		if( m_bSynchronise )
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
	if( Get_Count() > 0 )
	{
		CSG_Rect	Extent(Get_Layer(0)->Get_Layer()->Get_Extent());

		for(int i=1; i<Get_Count(); i++)
		{
			Extent.Union(Get_Layer(i)->Get_Layer()->Get_Extent());
		}

		Set_Extent(Extent);
	}
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
void CWKSP_Map::Set_ScaleBar(bool bOn)
{
	m_bScaleBar	= bOn;

	if( m_pView )
	{
		m_pView->Refresh_Map();
		m_pView->Ruler_Refresh();
	}
}

//---------------------------------------------------------
void CWKSP_Map::Set_Synchronising(bool bOn)
{
	m_bSynchronise	= bOn;

	if( m_bSynchronise )
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
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Map::On_Delete(CWKSP_Map_Layer *pLayer)
{
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Map::View_Closes(wxMDIChildFrame *pView)
{
	if( wxDynamicCast(pView, CVIEW_Map) != NULL )
	{
		m_pView		= NULL;
	}

	if( wxDynamicCast(pView, CVIEW_Map_3D) != NULL )
	{
		m_pView_3D	= NULL;
	}

	if( wxDynamicCast(pView, CVIEW_Layout) != NULL )
	{
		m_pLayout	= NULL;
	}
}

//---------------------------------------------------------
void CWKSP_Map::View_Refresh(bool bMapOnly)
{
	if( !bMapOnly && g_pACTIVE && g_pACTIVE->Get_Legend() )
	{
		g_pACTIVE->Get_Legend()->Refresh(true);
	}

	if( m_pView )
	{
		m_pView->Refresh_Map();
	}

	if( m_pView_3D )
	{
		m_pView_3D->On_Source_Changed();
	}

	if( m_pLayout )
	{
		m_pLayout->Refresh_Layout();
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
			m_pView	= new CVIEW_Map(this, Get_Frame_Width());
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
		delete(m_pView);
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
		m_pView_3D	= new CVIEW_Map_3D(this);
	}
	else if( !bShow && m_pView_3D )
	{
		m_pView_3D->Destroy();
		delete(m_pView_3D);
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
		m_pLayout	= new CVIEW_Layout(m_pLayout_Info);
	}
	else if( !bShow && m_pLayout )
	{
		m_pLayout->Destroy();
		delete(m_pLayout);
	}
}

//---------------------------------------------------------
void CWKSP_Map::View_Layout_Toggle(void)
{
	View_Layout_Show( m_pLayout == NULL );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Rect CWKSP_Map::Get_World(wxRect rClient)
{
	double		d, dWorld, dClient;
	TSG_Rect	Extent;

	Extent	= Get_Extent().m_rect;

	dClient	= (double)rClient.GetHeight()	/ (double)rClient.GetWidth();
	dWorld	= Get_Extent().Get_YRange()		/ Get_Extent().Get_XRange();

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
CSG_Point CWKSP_Map::Get_World(wxRect rClient, wxPoint ptClient)
{
	double		d;
	CSG_Rect	rWorld(Get_World(rClient));

	ptClient.y	= rClient.GetHeight() - ptClient.y;
	d			= rWorld.Get_XRange() / (double)rClient.GetWidth();

	return( CSG_Point(
		rWorld.Get_XMin() + ptClient.x * d,
		rWorld.Get_YMin() + ptClient.y * d)
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define MASK_R	255
#define MASK_G	0
#define MASK_B	255

//---------------------------------------------------------
bool CWKSP_Map::Get_Image(wxImage &Image, CSG_Rect &rWorld)
{
	if( Image.GetWidth() > 0 && Image.GetHeight() > 0 )
	{
		wxBitmap	BMP(Image);
		wxMemoryDC	dc;

		dc.SelectObject(BMP);
		Draw_Map(dc, 1.0, wxRect(0, 0, Image.GetWidth(), Image.GetHeight()), false, SG_GET_RGB(MASK_R, MASK_G, MASK_B));
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

	Draw_Map(dc, 1.0, r, false);

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
void CWKSP_Map::SaveAs_Image_To_Memory(int nx, int ny)
{
	if( nx < 1 || ny < 1 )
		return;

	CSG_Rect		Extent(Get_Extent());

	CSG_Parameters	P(NULL, _TL("Save To Memory Grid"), _TL(""));

	P.Add_Value(NULL, "CELLSIZE", _TL("Cellsize"), _TL(""), PARAMETER_TYPE_Double, Extent.Get_XRange() / (double)nx, 0.0, true);

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
			P("COLORS_TYPE")->Set_Value(3);

			g_pData->Set_Parameters(pGrid, &P);
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

		file.SetName(wxString::Format(wxT("%s_%03d"), fn.GetName().c_str(), m_Img_Count++));

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

	Draw_Map(dc, 1.0, r, false);
	Draw_Frame(dc, r, Frame);

	dc.SelectObject(wxNullBitmap);
	BMP.SaveFile(file, (wxBitmapType)type);

	if( m_Img_Parms("REF")->asBool() )
	{
		CSG_File	Stream;
		wxFileName	fn(file);

		switch( type )
		{
		default:					fn.SetExt(wxT("world"));	break;
		case wxBITMAP_TYPE_BMP:		fn.SetExt(wxT("bpw"));		break;
		case wxBITMAP_TYPE_GIF:		fn.SetExt(wxT("gfw"));		break;
		case wxBITMAP_TYPE_JPEG:	fn.SetExt(wxT("jgw"));		break;
		case wxBITMAP_TYPE_PNG:		fn.SetExt(wxT("pgw"));		break;
		case wxBITMAP_TYPE_PCX:		fn.SetExt(wxT("pxw"));		break;
		case wxBITMAP_TYPE_TIF:		fn.SetExt(wxT("tfw"));		break; 
		}

		if( Stream.Open(fn.GetFullPath().wx_str(), SG_FILE_W, false) )
		{
			CSG_Rect	rWorld(Get_World(r));
			double		d	= rWorld.Get_XRange() / r.GetWidth();

			Stream.Printf(SG_T("%.10f\n%.10f\n%.10f\n%.10f\n%.10f\n%.10f\n"),
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

		fn.SetExt(wxT("kml"));

		if( Stream.Open(fn.GetFullPath().wx_str(), SG_FILE_W, false) )
		{
			CSG_Rect	rWorld(Get_World(r));
			double		d	= rWorld.Get_XRange() / r.GetWidth();

			fn.Assign(file);

			Stream.Printf(SG_T("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"));
			Stream.Printf(SG_T("<kml xmlns=\"http://www.opengis.net/kml/2.2\">\n"));
			Stream.Printf(SG_T("  <Folder>\n"));
			Stream.Printf(SG_T("    <name>Maps exported from SAGA</name>\n"));
			Stream.Printf(SG_T("    <description>System for Automated Geoscientific Analyses - www.saga-gis.org</description>\n"));
			Stream.Printf(SG_T("    <GroundOverlay>\n"));
			Stream.Printf(SG_T("      <name>%s</name>\n")				, Get_Name().wx_str());
			Stream.Printf(SG_T("      <description>%s</description>\n")	, Get_Description().wx_str());
			Stream.Printf(SG_T("      <Icon>\n"));
			Stream.Printf(SG_T("        <href>%s</href>\n")				, fn.GetFullName().wx_str());
			Stream.Printf(SG_T("      </Icon>\n"));
			Stream.Printf(SG_T("      <LatLonBox>\n"));
			Stream.Printf(SG_T("        <north>%f</north>\n")			, rWorld.Get_YMax() + Frame * d);
			Stream.Printf(SG_T("        <south>%f</south>\n")			, rWorld.Get_YMin() - Frame * d);
			Stream.Printf(SG_T("        <east>%f</east>\n")				, rWorld.Get_XMax() + Frame * d);
			Stream.Printf(SG_T("        <west>%f</west>\n")				, rWorld.Get_XMin() - Frame * d);
			Stream.Printf(SG_T("        <rotation>0.0</rotation>\n"));
			Stream.Printf(SG_T("      </LatLonBox>\n"));
			Stream.Printf(SG_T("    </GroundOverlay>\n"));
			Stream.Printf(SG_T("  </Folder>\n"));
			Stream.Printf(SG_T("</kml>\n"));
		}
	}

	if( m_Img_Parms("LG")->asBool() && Get_Legend_Size(s, 1.0, m_Img_Parms("LZ")->asDouble()) )
	{
		wxFileName	fn(file);
		file	= fn.GetName();
		file.Append(wxT("_legend"));
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
//														 //
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

		Draw_Map(dc, Get_Extent(), 1.0, r, false);

		dc.SelectObject(wxNullBitmap);

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
void CWKSP_Map::Draw_Map(wxDC &dc, double Zoom, const wxRect &rClient, bool bEdit, int Background)
{
	Draw_Map(dc, Get_World(rClient), Zoom, rClient, bEdit, Background);
}

//---------------------------------------------------------
void CWKSP_Map::Draw_Map(wxDC &dc, const CSG_Rect &rWorld, double Zoom, const wxRect &rClient, bool bEdit, int Background)
{
	CWKSP_Map_DC	dc_Map(rWorld, rClient, Zoom, Background);

	for(int i=Get_Count()-1; i>=0; i--)
	{
		if( Get_Layer(i)->do_Show() && Get_Layer(i)->Get_Layer()->do_Show(Get_Extent()) )
		{
			Get_Layer(i)->Get_Layer()->Draw(dc_Map, bEdit && Get_Layer(i)->Get_Layer() == Get_Active_Layer());
		}
	}

	if( m_bScaleBar )
	{
		double	dScale	= 0.4 * rWorld.Get_XRange();
		wxRect	r, rScale(10, rClient.GetHeight() - 25, (int)(0.5 + dc_Map.xWorld2DC(rWorld.Get_XMin() + dScale)), 20);

		dc_Map.dc.SetPen(wxPen(*wxWHITE));
		dc_Map.dc.SetTextForeground(*wxWHITE);

		r	= rScale; r.Offset( 0,  1); Draw_Scale(dc_Map.dc, r, 0.0, dScale, true, true, true, true);
		r	= rScale; r.Offset( 1,  1); Draw_Scale(dc_Map.dc, r, 0.0, dScale, true, true, true, true);
		r	= rScale; r.Offset( 1,  0); Draw_Scale(dc_Map.dc, r, 0.0, dScale, true, true, true, true);
		r	= rScale; r.Offset( 1, -1); Draw_Scale(dc_Map.dc, r, 0.0, dScale, true, true, true, true);
		r	= rScale; r.Offset( 0, -1); Draw_Scale(dc_Map.dc, r, 0.0, dScale, true, true, true, true);
		r	= rScale; r.Offset(-1, -1); Draw_Scale(dc_Map.dc, r, 0.0, dScale, true, true, true, true);
		r	= rScale; r.Offset(-1,  0); Draw_Scale(dc_Map.dc, r, 0.0, dScale, true, true, true, true);
		r	= rScale; r.Offset(-1,  1); Draw_Scale(dc_Map.dc, r, 0.0, dScale, true, true, true, true);

		dc_Map.dc.SetPen(wxPen(*wxBLACK));
		dc_Map.dc.SetTextForeground(*wxBLACK);

		Draw_Scale(dc_Map.dc, rScale, 0.0, dScale, true, true, true, true);
	}

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

	r		= wxRect(rMap.GetLeft()    , rMap.GetBottom()   , rMap.GetWidth(), Width);
	Draw_Scale(dc, r, 0.0              , rWorld.Get_XRange(), true , true , true);

	r		= wxRect(rFrame.GetLeft()  , rMap.GetTop()      , Width, rMap.GetHeight());
	Draw_Scale(dc, r, rWorld.Get_YMin(), rWorld.Get_YMax()  , false, false, false);

	r		= wxRect(rMap.GetRight()   , rMap.GetTop()      , Width, rMap.GetHeight());
	Draw_Scale(dc, r, 0.0              , rWorld.Get_YRange(), false, false, true);

	Draw_Edge(dc, EDGE_STYLE_SIMPLE, rFrame.GetLeft(), rFrame.GetTop(), rFrame.GetRight(), rFrame.GetBottom());
}

//---------------------------------------------------------
bool CWKSP_Map::Draw_Legend(wxDC &dc, double Zoom_Map, double Zoom, wxPoint Position, wxSize *pSize)
{
	int		i, n;
	wxSize	s, Size;

	Size.x		= 0;
	Size.y		= 0;

	for(i=0, n=0; i<Get_Count(); i++)
	{
		if( Get_Layer(i)->Get_Layer()->do_Legend() )
		{
			n++;

			Get_Layer(i)->Get_Layer()->Get_Legend()->Draw(dc, Zoom, Zoom_Map, Position, &s);

			if( Get_Layer(i)->Get_Layer()->Get_Legend()->Get_Orientation() == LEGEND_VERTICAL )
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

	if( pSize )
	{
		*pSize	= Size;
	}

	return( n > 0 );
}

//---------------------------------------------------------
bool CWKSP_Map::Get_Legend_Size(wxSize &Size, double Zoom_Map, double Zoom)
{
	wxBitmap	bmp(10, 10);
	wxMemoryDC	dc(bmp);

	return( Draw_Legend(dc, Zoom_Map, Zoom, wxPoint(0, 0), &Size) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
