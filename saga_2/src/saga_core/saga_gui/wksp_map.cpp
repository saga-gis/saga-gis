
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
CWKSP_Map::CWKSP_Map(void)
{
	_Create_Parameters();

	m_pView			= NULL;
	m_pView_3D		= NULL;
	m_pLayout		= NULL;
	m_pLayout_Info	= new CVIEW_Layout_Info(this);
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
	return( wxString::Format("%02d. %s", 1 + Get_Index(), LNG("[CAP] Map")) );
}

//---------------------------------------------------------
wxString CWKSP_Map::Get_Description(void)
{
	return( wxString::Format("%02d. %s", 1 + Get_Index(), LNG("[CAP] Map")) );
}

//---------------------------------------------------------
wxMenu * CWKSP_Map::Get_Menu(void)
{
	wxMenu	*pMenu;

	pMenu	= new wxMenu(LNG("[CAP] Map"));

	CMD_Menu_Add_Item(pMenu, false, ID_CMD_WKSP_ITEM_CLOSE);
	CMD_Menu_Add_Item(pMenu,  true, ID_CMD_MAPS_SHOW);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_MAPS_SAVE_IMAGE);
#ifndef _SAGA_DONOTUSE_HARU
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_MAPS_SAVE_PDF_INDEXED);
#endif
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

	case ID_CMD_MAPS_SAVE_PDF_INDEXED:
		SaveAs_PDF_Indexed();
		break;

	case ID_CMD_MAPS_SAVE_INTERACTIVE_SVG:
		SaveAs_Interactive_SVG();
		break;
	
	case ID_CMD_MAPS_SYNCHRONIZE:
		Synchronize_Extents();
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
	CParameter	*pNode_0, *pNode_1;

	//-----------------------------------------------------
	m_Parameters.Create(this, "", "");
	m_Parameters.Set_Callback_On_Parameter_Changed(&_On_Parameter_Changed);

	//-----------------------------------------------------
	pNode_0	= m_Parameters.Add_Node(
		NULL	, "NODE_FRAME"		, LNG("[CAP] Frame"),
		""
	);

	m_Parameters.Add_Value(
		pNode_0	, "FRAME_SHOW"		, LNG("[CAP] Show"),
		"",
		PARAMETER_TYPE_Bool, true
	);

	m_Parameters.Add_Value(
		pNode_0	, "FRAME_WIDTH"		, LNG("[CAP] Width"),
		"",
		PARAMETER_TYPE_Int, 17, 5, true
	);

	//-----------------------------------------------------
	pNode_0	= m_Parameters.Add_Node(
		NULL	, "NODE_PRINT"		, LNG("[CAP] Print Layout"),
		""
	);

	m_Parameters.Add_Choice(
		pNode_0	, "PRINT_LEGEND"	, LNG("[CAP] Show Legend"),
		"",
		wxString::Format("%s|%s|",
			LNG("no"),
			LNG("yes")
		), 1
	);

	m_Parameters.Add_Value(
		pNode_0	, "PRINT_RESOLUTION"	, LNG("[CAP] Display Resolution"),
		"",
		PARAMETER_TYPE_Int, 2, 1, true
	);

	pNode_1	= m_Parameters.Add_Node(
		pNode_0	, "NODE_PRINT_FRAME"	, LNG("[CAP] Frame"),
		""
	);

	m_Parameters.Add_Value(
		pNode_1	, "PRINT_FRAME_SHOW"	, LNG("[CAP] Show"),
		"",
		PARAMETER_TYPE_Bool, true
	);

	m_Parameters.Add_Value(
		pNode_1	, "PRINT_FRAME_WIDTH"	, LNG("[CAP] Width"),
		"",
		PARAMETER_TYPE_Int, 7, 5, true
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
int CWKSP_Map::_On_Parameter_Changed(CParameter *pParameter)
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
int CWKSP_Map::On_Parameter_Changed(CParameters *pParameters, CParameter *pParameter)
{
	return( 1 );
}

//---------------------------------------------------------
void CWKSP_Map::Parameters_Changed(void)
{
	if( m_pView )
	{
		m_pView->Ruler_Set_Width(Get_Frame_Width());
	}

	View_Refresh(false);
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
			m_Extent	= pLayer->Get_Extent();
		}

		Set_Extent(pLayer->Get_Extent());

		Add_Item(pItem = new CWKSP_Map_Layer(pLayer));

		Move_Top(pItem);

		return( pItem );
	}

	return( NULL );
}

//---------------------------------------------------------
bool CWKSP_Map::Update(CWKSP_Layer *pLayer, bool bMapOnly)
{
	if( m_pView && Get_Layer(pLayer) >= 0 )
	{
		View_Refresh(bMapOnly);

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
void CWKSP_Map::Set_Extent(TSG_Rect Extent)
{
	if( Extent.xMax - Extent.xMin == 0.0 )
	{
		Extent.xMin	-= 0.01 * m_Extent.Get_XRange();
		Extent.xMax	+= 0.01 * m_Extent.Get_XRange();
	}

	if( Extent.yMax - Extent.yMin == 0.0 )
	{
		Extent.yMin	-= 0.01 * m_Extent.Get_YRange();
		Extent.yMax	+= 0.01 * m_Extent.Get_YRange();
	}

	if( !(m_Extent == Extent) )
	{
		m_Extent_Last	= m_Extent;
		m_Extent		= Extent;

		View_Refresh(true);
	}
}

//---------------------------------------------------------
void CWKSP_Map::Set_Extent(void)
{
	CParameters	p;

	p.Create(NULL, LNG("[CAP] Map Extent"), LNG(""));
	p.Add_Range(NULL, "X", LNG("West-East")		, LNG(""), m_Extent.Get_XMin(), m_Extent.Get_XMax());
	p.Add_Range(NULL, "Y", LNG("South-North")	, LNG(""), m_Extent.Get_YMin(), m_Extent.Get_YMax());

	if( DLG_Parameters(&p) )
	{
		CSG_Rect	Extent(	p("X")->asRange()->Get_LoVal(),
							p("Y")->asRange()->Get_LoVal(),
							p("X")->asRange()->Get_HiVal(),
							p("Y")->asRange()->Get_HiVal()	);

		Set_Extent(Extent);
	}
}

//---------------------------------------------------------
void CWKSP_Map::Set_Extent_Last(void)
{
	Set_Extent(m_Extent_Last);
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
void CWKSP_Map::Synchronize_Extents(void)
{
	for(int i=0; i<Get_Manager()->Get_Count(); i++)
	{
		if( Get_Manager()->Get_Item(i) != this )
		{
			((CWKSP_Map_Manager *)Get_Manager())->Get_Map(i)->Set_Extent(m_Extent);
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

	Extent	= m_Extent.m_rect;

	dClient	= (double)rClient.GetHeight()	/ (double)rClient.GetWidth();
	dWorld	= m_Extent.Get_YRange()			/ m_Extent.Get_XRange();

	if( dWorld > dClient )
	{
		d			= (m_Extent.Get_XRange() - m_Extent.Get_YRange() / dClient) / 2.0;
		Extent.xMin	+= d;
		Extent.xMax	-= d;
	}
	else
	{
		d			= (m_Extent.Get_YRange() - m_Extent.Get_XRange() * dClient) / 2.0;
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
	int			nx, ny, Frame, type;
	wxSize		s;
	wxRect		r;
	wxString	file;
	wxBitmap	BMP;
	wxMemoryDC	dc;
	CParameters	Parms;
	CParameter	*pNode;

	//-----------------------------------------------------
	Parms.Set_Name(LNG("[CAP] Save Map as Image..."));

	pNode	= Parms.Add_Node(NULL, "NODE_MAP", LNG("Map"), "");

	Parms.Add_Value(
		pNode	, "NX"	, LNG("[PRM] Map Width [Pixels]"),
		"",
		PARAMETER_TYPE_Int, 800	, 1, true
	);

	Parms.Add_Value(
		pNode	, "NY"	, LNG("[PRM] Map Height [Pixels]"),
		"",
		PARAMETER_TYPE_Int, 600	, 1, true
	);

	Parms.Add_Value(
		pNode	, "FR"	, LNG("[PRM] Frame Width [Pixels]"),
		"",
		PARAMETER_TYPE_Int, 20	, 0, true
	);

	pNode	= Parms.Add_Node(NULL, "NODE_LEGEND", LNG("[PRM] Legend"), "");

	Parms.Add_Value(
		pNode	, "LG"	, LNG("[PRM] Save"),
		"",
		PARAMETER_TYPE_Bool, 1
	);

	Parms.Add_Value(
		pNode	, "LZ"	, LNG("[PRM] Zoom"),
		"",
		PARAMETER_TYPE_Double, 1.0, 0, true
	);

	//-----------------------------------------------------
	if( DLG_Image_Save(file, type) && DLG_Parameters(&Parms) )
	{
		Set_Buisy_Cursor(true);

		nx		= Parms("NX")->asInt();
		ny		= Parms("NY")->asInt();
		Frame	= Parms("FR")->asInt();
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

		if( Parms("LG")->asBool() && Get_Legend_Size(s, 1.0, Parms("LZ")->asDouble()) )
		{
			wxFileName	fn(file);
			file	= fn.GetName();
			file.Append("_legend");
			fn.SetName(file);
			file	= fn.GetFullPath();

			BMP.Create(s.GetWidth(), s.GetHeight());
			dc.SelectObject(BMP);
			dc.SetBackground(*wxWHITE_BRUSH);
			dc.Clear();

			Draw_Legend(dc, 1.0, Parms("LZ")->asDouble(), wxPoint(0, 0));

			dc.SelectObject(wxNullBitmap);
			BMP.SaveFile(file, (wxBitmapType)type);
		}

		Set_Buisy_Cursor(false);
	}
}

//---------------------------------------------------------
void CWKSP_Map::SaveAs_PDF_Indexed(void)
{
	static CParameters	Parameters(NULL, LNG("[CAP] Save to PDF"), LNG(""), NULL, false);

	//-----------------------------------------------------
	if( Parameters.Get_Count() == 0 )
	{
		Parameters.Add_FilePath(
			NULL	, "FILENAME"	, LNG("[FLD] PDF Document"),
			LNG(""),
			wxString::Format("%s%s",
			LNG("PDF Documents (*.pdf)|*.pdf|"),
			LNG("All Files|*.*")), NULL, true, false
		);

		Parameters.Add_Choice(
			NULL	, "PAPER_SIZE"	, LNG("[FLD] Paper Format"),
			LNG(""),
			LNG("A4 Portrait|"
			"A4 Landscape|"
			"A3 Portrait|"
			"A3 Landscape|")
		);

		Parameters.Add_String(
			NULL	, "NAME"		, LNG("[FLD] Title"),
			LNG(""),
			""
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
			"All Recognised Files|*.png;*.jpg|"
			"PNG Files|*.png|"
			"JPG Files|*.jpg|"
			"All Files|*.*",
			NULL, false, false
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
		CShapes			*pShapes;
	//	CGrid			*pGrid;
		CDoc_PDF	PDF;

		MSG_General_Add(wxString::Format("%s...", LNG("[MSG] Save to PDF")), true, true);

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

		MSG_General_Add(bResult ? LNG("[MSG] okay") : LNG("[MSG] failed"), false, false);
	}
}

//---------------------------------------------------------
void CWKSP_Map::Draw_PDF(CDoc_PDF *pPDF, const char *FilePath_Maps, int Image_ID, const char *FileName_Icon, const char *Title, CSG_Rect rWorld, bool bRoundScale, int iField, CShapes *pShapes)
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
		FileName	= SG_File_Make_Path(FilePath_Maps, wxString::Format("image_%03d", Image_ID + 1), "png");
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

		Description.Append(wxString::Format("%d. %s\n", Image_ID + 2, LNG("Map")));

		if( pShapes && Image_ID >= 0 && Image_ID < pShapes->Get_Count() )
		{
			switch( pShapes->Get_Type() )
			{
			default:
				break;

			case SHAPE_TYPE_Line:
				d	= ((CShape_Line    *)pShapes->Get_Shape(Image_ID))->Get_Length();
				e	= d > 1000.0 ? 1000.0 : 1.0;
				s	= d > 1000.0 ? "km"   : "m";
				Description.Append(wxString::Format("%s: %f%s\n", LNG("Length")		, d / e, s.c_str()));
				break;

			case SHAPE_TYPE_Polygon:
				d	= ((CShape_Polygon *)pShapes->Get_Shape(Image_ID))->Get_Area();
				e	= d > 1000000.0 ? 1000000.0 : (d > 10000.0 ? 10000.0 : 1.0);
				s	= d > 1000000.0 ? "km²"     : (d > 10000.0 ? "ha"    : "m²");
				Description.Append(wxString::Format("%s: %f%s\n", LNG("Area")		, d / e, s.c_str()));

				d	= ((CShape_Polygon *)pShapes->Get_Shape(Image_ID))->Get_Perimeter();
				e	= d > 1000.0 ? 1000.0 : 1.0;
				s	= d > 1000.0 ? "km"   : "m";
				Description.Append(wxString::Format("%s: %f%s\n", LNG("Perimeter")	, d / e, s.c_str()));

				Description.Append(wxString::Format("%s: %d\n"  , LNG("Parts")		, ((CShape_Polygon *)pShapes->Get_Shape(Image_ID))->Get_Part_Count()));
				break;
			}

			if( iField >= 0 && iField < pShapes->Get_Table().Get_Field_Count() )
			{
				Description.Append(wxString::Format("%s: %s\n", pShapes->Get_Table().Get_Field_Name(iField), pShapes->Get_Shape(Image_ID)->Get_Record()->asString(iField)));
			}
		}

		Description.Append(wxString::Format("%s 1:%s", LNG("Scale"), SG_Get_String(Scale, 2).c_str()));

		pPDF->Draw_Text(rBox.Get_XMin(), rBox.Get_YMax(), Description, 12, PDF_STYLE_TEXT_ALIGN_H_LEFT|PDF_STYLE_TEXT_ALIGN_V_TOP);
	}
}

//---------------------------------------------------------
void CWKSP_Map::SaveAs_Interactive_SVG(void)
{
	static CParameters	Parameters(NULL, LNG("[CAP] Save as Interactive SVG"), LNG(""), NULL, false);

	//-----------------------------------------------------
	if( Parameters.Get_Count() == 0 )
	{
		Parameters.Add_FilePath(
			NULL	, "FILENAME"	, LNG("[FLD] SVG File"),
			LNG(""),
			wxString::Format("%s%s",
			LNG("Standard Vector Graphics (SVG)(*.svg)|*.svg|"),
			LNG("All Files|*.*")), NULL, true, false
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
		CShapes					*pIndexLayer;
		CSVG_Interactive_Map	SVG;

		MSG_General_Add(wxString::Format("%s...", LNG("[MSG] Save as Interactive SVG")), true, true);

		pIndexLayer		= Parameters("SHAPES")		->asShapes();
		FileName		= Parameters("FILENAME")	->asString();

		SVG.Create_From_Map(this, pIndexLayer, FileName);

		MSG_General_Add(LNG("[MSG] okay"), false, false);
	}
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
	CWKSP_Layer		*pLayer;
	CWKSP_Map_DC	dc_Map(rWorld, rClient, Zoom, Background);

	for(int i=Get_Count()-1; i>=0; i--)
	{
		pLayer	= Get_Layer(i)->Get_Layer();

		if( pLayer->do_Show(Get_Extent()) )
		{
			pLayer->Draw(dc_Map, bEdit && pLayer == Get_Active_Layer());
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
