
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

#include <saga_api/doc_pdf.h>
#include "svg_interactive_map.h"

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

	m_Name.Printf(wxT("%02d. %s"), ++iMap, LNG("[CAP] Map"));

	_Create_Parameters();

	m_pView			= NULL;
	m_pView_3D		= NULL;
	m_pLayout		= NULL;
	m_pLayout_Info	= new CVIEW_Layout_Info(this);

	m_bSynchronise	= false;
	m_Img_bSave		= false;
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
		LNG("[CAP] Map")
	));

	s.Append(wxString::Format(wxT("<tr><td>%s</td><td>%s</td></tr>"),
		LNG("[CAP] Name")					, m_Name.c_str()
	));

	s.Append(wxString::Format(wxT("<tr><td>%s</td><td>%d</td></tr>"),
		LNG("[CAP] Layers")					, Get_Count()
	));

	s.Append(wxT("</table>"));

	return( s );
}

//---------------------------------------------------------
wxMenu * CWKSP_Map::Get_Menu(void)
{
	wxMenu	*pMenu;

	pMenu	= new wxMenu(LNG("[CAP] Map"));

	CMD_Menu_Add_Item(pMenu, false, ID_CMD_WKSP_ITEM_CLOSE);
	CMD_Menu_Add_Item(pMenu,  true, ID_CMD_MAPS_SHOW);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_MAPS_SAVE_IMAGE);
	CMD_Menu_Add_Item(pMenu,  true, ID_CMD_MAPS_SAVE_IMAGE_ON_CHANGE);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_MAPS_SAVE_TO_CLIPBOARD);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_MAPS_SAVE_TO_CLIPBOARD_LEGEND);
	if( CSG_Doc_PDF::Get_Version() != NULL )
	{
		CMD_Menu_Add_Item(pMenu, false, ID_CMD_MAPS_SAVE_PDF_INDEXED);
	}
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_MAPS_SAVE_INTERACTIVE_SVG);
	CMD_Menu_Add_Item(pMenu,  true, ID_CMD_MAPS_3D_SHOW);
	CMD_Menu_Add_Item(pMenu,  true, ID_CMD_MAPS_LAYOUT_SHOW);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_MAPS_SYNCHRONIZE);

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

	case ID_CMD_MAPS_SAVE_PDF_INDEXED:
		SaveAs_PDF_Indexed();
		break;

	case ID_CMD_MAPS_SAVE_INTERACTIVE_SVG:
		SaveAs_Interactive_SVG();
		break;
	
	case ID_CMD_MAPS_SYNCHRONIZE:
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
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Map::_Create_Parameters(void)
{
	CSG_Parameter	*pNode_0, *pNode_1;

	//-----------------------------------------------------
	m_Parameters.Create(this, LNG(""), LNG(""));
	m_Parameters.Set_Callback_On_Parameter_Changed(&_On_Parameter_Changed);

	//-----------------------------------------------------
	pNode_0	= m_Parameters.Add_Node(
		NULL	, "NODE_GENERAL"	, LNG("[CAP] General"),
		LNG("")
	);

	m_Parameters.Add_String(
		pNode_0	, "NAME"			, LNG("[CAP] Name"),
		LNG(""),
		m_Name.c_str()
	);

	m_Parameters.Add_Value(
		pNode_0	, "GOTO_NEWLAYER"	, LNG("[CAP] Zoom to added layer"),
		LNG(""),
		PARAMETER_TYPE_Bool, g_pMaps->Get_Parameters()->Get_Parameter("GOTO_NEWLAYER")->asBool()
	);

	//-----------------------------------------------------
	pNode_0	= m_Parameters.Add_Node(
		NULL	, "NODE_FRAME"		, LNG("[CAP] Frame"),
		LNG("")
	);

	m_Parameters.Add_Value(
		pNode_0	, "FRAME_SHOW"		, LNG("[CAP] Show"),
		LNG(""),
		PARAMETER_TYPE_Bool, g_pMaps->Get_Parameters()->Get_Parameter("FRAME_SHOW")->asBool()
	);

	m_Parameters.Add_Value(
		pNode_0	, "FRAME_WIDTH"		, LNG("[CAP] Width"),
		LNG(""),
		PARAMETER_TYPE_Int, g_pMaps->Get_Parameters()->Get_Parameter("FRAME_WIDTH")->asInt(), 5, true
	);

	//-----------------------------------------------------
	pNode_0	= m_Parameters.Add_Node(
		NULL	, "NODE_PRINT"		, LNG("[CAP] Print Layout"),
		LNG("")
	);

	m_Parameters.Add_Choice(
		pNode_0	, "PRINT_LEGEND"	, LNG("[CAP] Show Legend"),
		LNG(""),
		wxString::Format(wxT("%s|%s|"),
			LNG("no"),
			LNG("yes")
		), 1
	);

	m_Parameters.Add_Value(
		pNode_0	, "PRINT_RESOLUTION"	, LNG("[CAP] Display Resolution"),
		LNG(""),
		PARAMETER_TYPE_Int, 2, 1, true
	);

	pNode_1	= m_Parameters.Add_Node(
		pNode_0	, "NODE_PRINT_FRAME"	, LNG("[CAP] Frame"),
		LNG("")
	);

	m_Parameters.Add_Value(
		pNode_1	, "PRINT_FRAME_SHOW"	, LNG("[CAP] Show"),
		LNG(""),
		PARAMETER_TYPE_Bool, true
	);

	m_Parameters.Add_Value(
		pNode_1	, "PRINT_FRAME_WIDTH"	, LNG("[CAP] Width"),
		LNG(""),
		PARAMETER_TYPE_Int, 7, 5, true
	);

	//-----------------------------------------------------
	m_Img_Parms.Set_Name(LNG("[CAP] Save Map as Image..."));

	pNode_0	= m_Img_Parms.Add_Node(NULL, "NODE_MAP", LNG("Map"), LNG(""));

	m_Img_Parms.Add_Value(
		pNode_0	, "NX"	, LNG("[PRM] Map Width [Pixels]"),
		LNG(""),
		PARAMETER_TYPE_Int, 800	, 1, true
	);

	m_Img_Parms.Add_Value(
		pNode_0	, "NY"	, LNG("[PRM] Map Height [Pixels]"),
		LNG(""),
		PARAMETER_TYPE_Int, 600	, 1, true
	);

	m_Img_Parms.Add_Value(
		pNode_0	, "FR"	, LNG("[PRM] Frame Width [Pixels]"),
		LNG(""),
		PARAMETER_TYPE_Int, 20	, 0, true
	);

	m_Img_Parms.Add_Value(
		pNode_0	, "REF"	, LNG("[PRM] Save Georeference (world file)"),
		LNG(""),
		PARAMETER_TYPE_Bool, 1
	);

	m_Img_Parms.Add_Value(
		pNode_0	, "KML"	, LNG("[PRM] Save KML file"),
		LNG(""),
		PARAMETER_TYPE_Bool, 1
	);

	pNode_0	= m_Img_Parms.Add_Node(NULL, "NODE_LEGEND", LNG("[PRM] Legend"), LNG(""));

	m_Img_Parms.Add_Value(
		pNode_0	, "LG"	, LNG("[PRM] Save"),
		LNG(""),
		PARAMETER_TYPE_Bool, 1
	);

	m_Img_Parms.Add_Value(
		pNode_0	, "LZ"	, LNG("[PRM] Zoom"),
		LNG(""),
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
int CWKSP_Map::_On_Parameter_Changed(CSG_Parameter *pParameter)
{
	if( pParameter && pParameter->Get_Owner() && pParameter->Get_Owner()->Get_Owner() )
	{
		return( ((CWKSP_Map *)pParameter->Get_Owner()->Get_Owner())->
			On_Parameter_Changed(pParameter->Get_Owner(), pParameter)
		);
	}

	return( 0 );
}

//---------------------------------------------------------
int CWKSP_Map::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	return( 1 );
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

		if( Get_Count() == 0 || m_Parameters("GOTO_NEWLAYER")->asBool() )
		{
			Set_Extent(pLayer->Get_Extent());
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
	int		iLayer;
	
	if( (iLayer = Get_Layer(pLayer)) >= 0 )
	{
		if( !bMapOnly )
		{
			Get_Layer(iLayer)->Parameters_Changed();
		}

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
	if( m_Extents.Add_Extent(Extent, bReset) )
	{
		_Set_Extent(Extent);
	}
}

//---------------------------------------------------------
void CWKSP_Map::Set_Extent(void)
{
	CSG_Parameters	p(NULL, LNG("[CAP] Map Extent"), LNG(""));

	p.Add_Range(NULL, "X", LNG("West-East")		, LNG(""), Get_Extent().Get_XMin(), Get_Extent().Get_XMax());
	p.Add_Range(NULL, "Y", LNG("South-North")	, LNG(""), Get_Extent().Get_YMin(), Get_Extent().Get_YMax());

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
			Get_Manager()->Get_Parameters()->Get_Parameter("CLIP_NX")	->asInt(),
			Get_Manager()->Get_Parameters()->Get_Parameter("CLIP_NY")	->asInt(),
			Get_Manager()->Get_Parameters()->Get_Parameter("CLIP_FRAME")->asInt()
		);

		return;
	}

	//-----------------------------------------------------
	// draw a legend...

	Set_Buisy_Cursor(true);

	int			Frame	= Get_Manager()->Get_Parameters()->Get_Parameter("CLIP_LEGEND_FRAME")->asInt();
	double		Scale	= Get_Manager()->Get_Parameters()->Get_Parameter("CLIP_LEGEND_SCALE")->asDouble();
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
			dc.SetPen(Get_Color_asWX(Get_Manager()->Get_Parameters()->Get_Parameter("CLIP_LEGEND_COLOR")->asInt()));
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
	Draw_Frame(dc, r, frame);

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

		if( Stream.Open(fn.GetFullPath().c_str(), SG_FILE_W, false) )
		{
			CSG_Rect	rWorld(Get_World(r));
			double		d	= rWorld.Get_XRange() / r.GetWidth();

			Stream.Printf(wxT("%.10f\n%.10f\n%.10f\n%.10f\n%.10f\n%.10f\n"),
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

		if( Stream.Open(fn.GetFullPath().c_str(), SG_FILE_W, false) )
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
			Stream.Printf(SG_T("      <name>%s</name>\n")				, Get_Name());
			Stream.Printf(SG_T("      <description>%s</description>\n")	, Get_Description());
			Stream.Printf(SG_T("      <Icon>\n"));
			Stream.Printf(SG_T("        <href>%s</href>\n")				, fn.GetFullName().c_str());
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

//---------------------------------------------------------
void CWKSP_Map::SaveAs_PDF_Indexed(void)
{
	static CSG_Parameters	Parameters(NULL, LNG("[CAP] Save to PDF"), LNG(""), NULL, false);

	//-----------------------------------------------------
	if( Parameters.Get_Count() == 0 )
	{
		Parameters.Add_FilePath(
			NULL	, "FILENAME"	, LNG("[FLD] PDF Document"),
			LNG(""),
			wxString::Format(
				wxT("%s (*.pdf)|*pdf|")
				wxT("%s|*.*"),
				LNG("PDF Files"),
				LNG("All Files")
			), NULL, true, false
		);

		Parameters.Add_Choice(
			NULL	, "PAPER_SIZE"	, LNG("[FLD] Paper Format"),
			LNG(""),

			CSG_String::Format(wxT("%s|%s|%s|%s|"),
				LNG("A4 Portrait"),
				LNG("A4 Landscape"),
				LNG("A3 Portrait"),
				LNG("A3 Landscape")
			)
		);

		Parameters.Add_String(
			NULL	, "NAME"		, LNG("[FLD] Title"),
			LNG(""),
			LNG("")
		);

		Parameters.Add_Shapes(
			NULL	, "SHAPES"		, LNG("[FLD] Shapes"),
			LNG(""),
			PARAMETER_INPUT_OPTIONAL
		);

		Parameters.Add_Table_Field(
			Parameters("SHAPES")	, "FIELD"		, LNG("[FLD] Attribute"),
			LNG("")
		);

	//	Parameters.Add_Grid(
	//		NULL	, "GRID"		, LNG("[FLD] Grid"),
	//		LNG(""),
	//		PARAMETER_INPUT_OPTIONAL
	//	);

		Parameters.Add_FilePath(
			NULL	, "FILEICON"	, LNG("[FLD] Icon"),
			LNG(""),

			CSG_String::Format(wxT("%s|*.png;*.jpg|%s|*.png|%s|*.jpg|%s|*.*"),
				LNG("All Recognised Files"),
				LNG("PNG Files"),
				LNG("JPG Files"),
				LNG("All Files")
			), NULL, false, false
		);

		Parameters.Add_Value(
			NULL	, "ROUNDSCALE"	, LNG("[FLD] Round Scale"),
			LNG(""),
			PARAMETER_TYPE_Bool, true
		);
	}

	//-----------------------------------------------------
	if( DLG_Parameters(&Parameters) )
	{
		bool			bResult, bRoundScale;
		int				iField;
		CSG_String		Name, FileName, FileName_Icon, FilePath_Maps;
		CSG_Rect		rOverview, rMap;
		CSG_Shapes			*pShapes;
	//	CSG_Grid			*pGrid;
		CSG_Doc_PDF	PDF;

		MSG_General_Add(wxString::Format(wxT("%s..."), LNG("[MSG] Save to PDF")), true, true);

		bResult			= false;
		Name			= Parameters("NAME")		->asString();	if( Name.Length() < 1 )	Name	=  LNG("Maps");
		FileName		= Parameters("FILENAME")	->asString();
		FileName_Icon	= Parameters("FILEICON")	->asString();
		pShapes			= Parameters("SHAPES")		->asShapes();
	//	pGrid			= Parameters("GRID")		->asGrid();
		iField			= Parameters("FIELD")		->asInt();
		bRoundScale		= Parameters("ROUNDSCALE")	->asBool();

		switch( Parameters("PAPER_SIZE")->asInt() )
		{
		default:
		case 0:	PDF.Open(PDF_PAGE_SIZE_A4, PDF_PAGE_ORIENTATION_PORTRAIT , Name);	break;	// A4 Portrait
		case 1:	PDF.Open(PDF_PAGE_SIZE_A4, PDF_PAGE_ORIENTATION_LANDSCAPE, Name);	break;	// A4 Landscape
		case 2:	PDF.Open(PDF_PAGE_SIZE_A3, PDF_PAGE_ORIENTATION_PORTRAIT , Name);	break;	// A3 Portrait
		case 3:	PDF.Open(PDF_PAGE_SIZE_A3, PDF_PAGE_ORIENTATION_LANDSCAPE, Name);	break;	// A3 Landscape
		}

		//-------------------------------------------------
		if( PDF.Is_Open() )
		{
			PDF.Layout_Set_Box_Space(5, false);

			PDF.Layout_Add_Box(  0.0,   0.0, 100.0,  75.0, "MAP");
			PDF.Layout_Add_Box(  0.0,  75.0,  50.0, 100.0, "DIVISIONS");
			PDF.Layout_Add_Box( 50.0,  75.0,  60.0,  80.0, "ICON");
			PDF.Layout_Add_Box( 60.0,  75.0, 100.0,  80.0, "TITLE");
			PDF.Layout_Add_Box( 50.0,  80.0, 100.0, 100.0, "DESCRIPTION");

			FilePath_Maps	= SG_File_Make_Path(SG_File_Get_Path(FileName), SG_File_Get_Name(FileName, false));
			rOverview		= pShapes ? pShapes->Get_Extent() : Get_Extent();
		//	rOverview		= pShapes ? pShapes->Get_Extent() : (pGrid ? pGrid->Get_Extent() : Get_Extent());

			//---------------------------------------------
		//	PDF.Draw_Text		(PDF.Layout_Get_Box("TITLE").Get_XMin(), PDF.Layout_Get_Box("TITLE").Get_YCenter(), LNG("This is a Test!!!"), 24);
		//	PDF.Draw_Rectangle	(PDF.Layout_Get_Box("DIVISIONS"));
		//	PDF.Draw_Grid		(PDF.Layout_Get_Box("DIVISIONS"), Parameters("GRID")->asGrid(), CSG_Colors(), 0.0, 0.0, 0, &rOverview);
		//	PDF.Draw_Shapes		(PDF.Layout_Get_Box("DIVISIONS"), pShapes, PDF_STYLE_POLYGON_STROKE, SG_COLOR_GREEN, SG_COLOR_BLACK, 1, &rOverview);
		//	PDF.Draw_Graticule	(PDF.Layout_Get_Box("DIVISIONS"), rOverview);

			//---------------------------------------------
			Draw_PDF(&PDF, FilePath_Maps, -1, FileName_Icon, Name, rOverview, bRoundScale, iField, pShapes);

			if( pShapes )
			{
				for(int i=0; i<pShapes->Get_Count() && SG_UI_Process_Set_Progress(i, pShapes->Get_Count()); i++)
				{
					Draw_PDF(&PDF, FilePath_Maps, i, FileName_Icon, Name, pShapes->Get_Shape(i)->Get_Extent(), bRoundScale, iField, pShapes);
				}
			}

			//---------------------------------------------
			PROCESS_Set_Okay(true);
			Set_Buisy_Cursor(true);
			bResult	= PDF.Save(FileName);
			Set_Buisy_Cursor(false);
		}

		MSG_General_Add(bResult ? LNG("[MSG] okay") : LNG("[MSG] failed"), false, false, bResult ? SG_UI_MSG_STYLE_SUCCESS : SG_UI_MSG_STYLE_FAILURE);
	}
}

//---------------------------------------------------------
void CWKSP_Map::Draw_PDF(CSG_Doc_PDF *pPDF, const wxChar *FilePath_Maps, int Image_ID, const wxChar *FileName_Icon, const wxChar *Title, CSG_Rect rWorld, bool bRoundScale, int iField, CSG_Shapes *pShapes)
{
	int			FrameSize_1	= 20, FrameSize_2	= 10;
	double		d, e, Scale, Ratio;
	CSG_String	FileName, Description, s;
	CSG_Rect	rBox;
	wxRect		rBMP;
	wxBitmap	BMP;
	wxMemoryDC	dc;

	if( pPDF && rWorld.Get_XRange() > 0.0 && rWorld.Get_YRange() > 0.0 && pPDF->Add_Page() )
	{
		rWorld.Inflate(5.0, true);

		//-------------------------------------------------
		rBox	= pPDF->Layout_Get_Box("MAP");
		rBox.Deflate(FrameSize_1, false);

		rBMP	= wxRect(0, 0, (int)rBox.Get_XRange(), (int)rBox.Get_YRange());
		BMP.Create(rBMP.GetWidth(), rBMP.GetHeight());
		dc.SelectObject(BMP);
		dc.SetBackground(*wxWHITE_BRUSH);
		dc.Clear();

		if( bRoundScale )
		{
			Scale	= rWorld.Get_XRange() / (pPDF->Get_Page_To_Meter() * (double)rBMP.GetWidth());

		//	if( Scale > 1000 )
		//	{
				Ratio	= ((ceil(Scale / 1000.)) / (Scale / 1000.) - 1);
				rWorld.Inflate(Ratio * 100, true);
		//	}
		}

		Scale	= rWorld.Get_XRange() / (pPDF->Get_Page_To_Meter() * rBMP.GetWidth());

		Draw_Map(dc, rWorld, 1.0, rBMP, false);
		dc.SelectObject(wxNullBitmap);
		SG_Dir_Create(FilePath_Maps);
		FileName	= SG_File_Make_Path(FilePath_Maps, wxString::Format(wxT("image_%03d"), Image_ID + 1), wxT("png"));
		BMP.SaveFile(FileName.c_str(), wxBITMAP_TYPE_PNG);

		pPDF->Draw_Image	(rBox, FileName);
		pPDF->Draw_Graticule(rBox, rWorld, FrameSize_1);

		//-------------------------------------------------
		rBox	= pPDF->Layout_Get_Box("ICON");

		if( FileName_Icon )
		{
			pPDF->Draw_Image(rBox, FileName_Icon);
		}
		else
		{
			pPDF->Draw_Rectangle(rBox);
		}

		//-------------------------------------------------
		rBox	= pPDF->Layout_Get_Box("DIVISIONS");

		pPDF->Draw_Rectangle(rBox);

		if( pShapes )
		{
			CSG_Rect	rShapes(pShapes->Get_Extent());

			rShapes.Inflate(5.0, true);
			rBox.Deflate(FrameSize_2, false);

			pPDF->Draw_Graticule(rBox, rShapes, FrameSize_2);
			pPDF->Draw_Shapes(rBox, pShapes, PDF_STYLE_POLYGON_FILLSTROKE, SG_COLOR_GREEN, SG_COLOR_BLACK, 0, &rShapes);

			if( Image_ID >= 0 && Image_ID < pShapes->Get_Count() )
			{
				pPDF->Draw_Shape(rBox, pShapes->Get_Shape(Image_ID), PDF_STYLE_POLYGON_FILLSTROKE, SG_COLOR_YELLOW, SG_COLOR_RED, 1, &rShapes);
			}
		}

		//-------------------------------------------------
		rBox	= pPDF->Layout_Get_Box("TITLE");

		pPDF->Draw_Text(rBox.Get_XMin(), rBox.Get_YCenter(), Title, 20, PDF_STYLE_TEXT_ALIGN_H_LEFT|PDF_STYLE_TEXT_ALIGN_V_CENTER|PDF_STYLE_TEXT_UNDERLINE);

		//-------------------------------------------------
		rBox	= pPDF->Layout_Get_Box("DESCRIPTION");

		Description.Append(wxString::Format(wxT("%d. %s\n"), Image_ID + 2, LNG("Map")));

		if( pShapes && Image_ID >= 0 && Image_ID < pShapes->Get_Count() )
		{
			switch( pShapes->Get_Type() )
			{
			default:
				break;

			case SHAPE_TYPE_Line:
				d	= ((CSG_Shape_Line    *)pShapes->Get_Shape(Image_ID))->Get_Length();
				e	= d > 1000.0 ? 1000.0    : 1.0;
				s	= d > 1000.0 ? wxT("km") : wxT("m");
				Description.Append(wxString::Format(wxT("%s: %f%s\n"), LNG("Length")	, d / e, s.c_str()));
				break;

			case SHAPE_TYPE_Polygon:
				d	= ((CSG_Shape_Polygon *)pShapes->Get_Shape(Image_ID))->Get_Area();
				e	= d > 1000000.0 ? 1000000.0  : (d > 10000.0 ? 10000.0   : 1.0);
				s	= d > 1000000.0 ? wxT("km\xc2\xb2") : (d > 10000.0 ? wxT("ha") : wxT("m\xc2\xb2"));
				Description.Append(wxString::Format(wxT("%s: %f%s\n"), LNG("Area")		, d / e, s.c_str()));

				d	= ((CSG_Shape_Polygon *)pShapes->Get_Shape(Image_ID))->Get_Perimeter();
				e	= d > 1000.0 ? 1000.0    : 1.0;
				s	= d > 1000.0 ? wxT("km") : wxT("m");
				Description.Append(wxString::Format(wxT("%s: %f%s\n"), LNG("Perimeter")	, d / e, s.c_str()));

				Description.Append(wxString::Format(wxT("%s: %d\n")  , LNG("Parts")		, ((CSG_Shape_Polygon *)pShapes->Get_Shape(Image_ID))->Get_Part_Count()));
				break;
			}

			if( iField >= 0 && iField < pShapes->Get_Field_Count() )
			{
				Description.Append(wxString::Format(wxT("%s: %s\n"), pShapes->Get_Field_Name(iField), pShapes->Get_Shape(Image_ID)->asString(iField)));
			}
		}

		Description.Append(wxString::Format(wxT("%s 1:%s"), LNG("Scale"), SG_Get_String(Scale, 2).c_str()));

		pPDF->Draw_Text(rBox.Get_XMin(), rBox.Get_YMax(), Description, 12, PDF_STYLE_TEXT_ALIGN_H_LEFT|PDF_STYLE_TEXT_ALIGN_V_TOP);
	}
}

//---------------------------------------------------------
void CWKSP_Map::SaveAs_Interactive_SVG(void)
{
	static CSG_Parameters	Parameters(NULL, LNG("[CAP] Save As Interactive SVG"), LNG(""), NULL, false);

	//-----------------------------------------------------
	if( Parameters.Get_Count() == 0 )
	{
		Parameters.Add_FilePath(
			NULL	, "FILENAME"	, LNG("[FLD] SVG File"),
			LNG(""),
			wxString::Format(wxT("%s|*.svg|%s|*.*"),
				LNG("SVG - Scalable Vector Graphics Files (*.svg)"),
				LNG("All Files")
			), NULL, true, false
		);

		Parameters.Add_Shapes(
			NULL	, "SHAPES"		, LNG("[FLD] Index Layer"),
			LNG(""),
			PARAMETER_INPUT_OPTIONAL
		);
	}

	//-----------------------------------------------------
	if( DLG_Parameters(&Parameters) )
	{
		CSG_String				FileName;
		CSG_Shapes					*pIndexLayer;
		CSVG_Interactive_Map	SVG;

		MSG_General_Add(wxString::Format(wxT("%s..."), LNG("[MSG] Save as Interactive SVG")), true, true);

		pIndexLayer		= Parameters("SHAPES")		->asShapes();
		FileName		= Parameters("FILENAME")	->asString();

		SVG.Create_From_Map(this, pIndexLayer, FileName);

		MSG_General_Add(LNG("[MSG] okay"), false, false, SG_UI_MSG_STYLE_SUCCESS);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
const wxBitmap & CWKSP_Map::Get_Thumbnail(int dx, int dy)
{
	if( dx > 0 && m_Thumbnail.GetWidth()  != dx
	&&	dy > 0 && m_Thumbnail.GetHeight() != dy )
	{
		m_Thumbnail.Create(dx, dy);

		_Set_Thumbnail();
	}

	return( m_Thumbnail );
}

//---------------------------------------------------------
bool CWKSP_Map::_Set_Thumbnail(void)
{
	if( m_Thumbnail.GetWidth() > 0 && m_Thumbnail.GetHeight() > 0 )
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
bool CWKSP_Map::Draw_Legend(wxDC &dc, double Zoom_Map, double Zoom, wxPoint Position, wxSize *pSize, int Layout)
{
	bool	bVertical;
	int		i, n;
	wxSize	s, Size;

	Size.x		= 0;
	Size.y		= 0;

	bVertical	= (Layout & LEGEND_LAYOUT_VERTICAL) != 0;

	for(i=0, n=0; i<Get_Count(); i++)
	{
		if( Get_Layer(i)->Get_Layer()->do_Legend() )
		{
			n++;

			Get_Layer(i)->Get_Layer()->Get_Legend()->Draw(dc, Zoom, Zoom_Map, Position, &s, bVertical);

			if( bVertical )
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
bool CWKSP_Map::Get_Legend_Size(wxSize &Size, double Zoom_Map, double Zoom, int Layout)
{
	wxMemoryDC	dc;

	return( Draw_Legend(dc, Zoom_Map, Zoom, wxPoint(0, 0), &Size, Layout) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
