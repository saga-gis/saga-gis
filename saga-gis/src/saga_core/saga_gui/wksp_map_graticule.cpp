/**********************************************************
 * Version $Id: wksp_map_graticule.cpp 1921 2014-01-09 10:24:11Z oconrad $
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
//                wksp_map_graticule.cpp                 //
//                                                       //
//          Copyright (C) 2014 by Olaf Conrad            //
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
//                University of Hamburg                  //
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

#include "helper.h"
#include "dc_helper.h"

#include "res_commands.h"

#include "wksp_map.h"
#include "wksp_map_graticule.h"
#include "wksp_map_dc.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Map_Graticule::CWKSP_Map_Graticule(CSG_MetaData *pEntry)
{
	m_bShow		= true;

	//-----------------------------------------------------
	CSG_Parameter	*pNode, *pNode_1;

	m_Parameters.Set_Name      ("GRATICULE");
	m_Parameters.Set_Identifier("GRATICULE");

	//-----------------------------------------------------
	pNode	= m_Parameters.Add_Node(NULL, "NODE_GENERAL"	,_TL("General")	, _TL(""));

	m_Parameters.Add_String(
		pNode	, "NAME"		, _TL("Name"),
		_TL(""),
		_TL("Graticule")
	);

	pNode_1	= m_Parameters.Add_Choice(
		pNode	, "INTERVAL"	, _TL("Interval"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("fixed interval"),
			_TL("fitted interval")
		), 1
	);

	m_Parameters.Add_Value(
		pNode_1	, "FIXED"		, _TL("Fixed Interval (Degree)"),
		_TL(""),
		PARAMETER_TYPE_Double, 5.0, 0.0, true, 20.0
	);

	m_Parameters.Add_Value(
		pNode_1	, "FITTED"		, _TL("Number of Intervals"),
		_TL(""),
		PARAMETER_TYPE_Int, 5, 1, true
	);

	m_Parameters.Add_Value(
		pNode	, "RESOLUTION"	, _TL("Minimum Resolution (Degree)"),
		_TL(""),
		PARAMETER_TYPE_Double, 0.5, 0.0, true
	);

	//-----------------------------------------------------
	pNode_1	= m_Parameters.Add_Value(
		pNode	, "SHOW_ALWAYS"	, _TL("Show at all scales"),
		_TL(""),
		PARAMETER_TYPE_Bool, true
	);

	m_Parameters.Add_Range(
		pNode_1	, "SHOW_RANGE"	, _TL("Scale Range"),
		_TL("only show within scale range; values are given as extent measured in map units"),
		100.0, 1000.0, 0.0, true
	);

	//-----------------------------------------------------
	pNode	= m_Parameters.Add_Node(NULL, "NODE_DISPLAY"	,_TL("Display")	, _TL(""));

	m_Parameters.Add_Value(
		pNode	, "COLOR"		, _TL("Color"),
		_TL(""),
		PARAMETER_TYPE_Color, SG_COLOR_GREY
	);

	m_Parameters.Add_Value(
		pNode	, "SIZE"		, _TL("Size"),
		_TL(""),
		PARAMETER_TYPE_Int, 0, 0, true
	);

	m_Parameters.Add_Value(
		pNode	, "TRANSPARENCY", _TL("Transparency [%]"),
		_TL(""),
		PARAMETER_TYPE_Double, 0.0, 0.0, true, 100.0, true
	);

	m_Parameters.Add_Choice(
		pNode	, "LINE_STYLE"	, _TL("Line Style"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|"),
			_TL("Solid style"),
			_TL("Dotted style"),
			_TL("Long dashed style"),
			_TL("Short dashed style"), 
			_TL("Dot and dash style"),
			_TL("Backward diagonal hatch"),
			_TL("Cross-diagonal hatch"),
			_TL("Forward diagonal hatch"),
			_TL("Cross hatch"),
			_TL("Horizontal hatch"),
			_TL("Vertical hatch")
		//	_TL("Use the stipple bitmap")
		//	_TL("Use the user dashes")
		//	_TL("No pen is used")
		), 4
	);

	//-----------------------------------------------------
	pNode_1	= m_Parameters.Add_Value(
		pNode	, "LABEL"		, _TL("Label"),
		_TL(""),
		PARAMETER_TYPE_Bool, true
	);

	m_Parameters.Add_Font(
		pNode_1	, "LABEL_FONT"	, _TL("Font"),
		_TL("")
	);

	m_Parameters.Add_Value(
		pNode_1	, "LABEL_SIZE"	, _TL("Size"),
		_TL("Font size given as percentage of map size."),
		PARAMETER_TYPE_Double, 2, 0.0, true, 10.0, true
	);

	m_Parameters.Add_Choice(
		pNode_1	, "LABEL_EFFECT", _TL("Boundary Effect"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|"),
			_TL("none"),
			_TL("full frame"),
			_TL("top"),
			_TL("top left"),
			_TL("left"),
			_TL("bottom left"),
			_TL("bottom"),
			_TL("bottom right"),
			_TL("right"),
			_TL("top right")
		), 1
	);

	m_Parameters.Add_Value(
		pNode_1	, "LABEL_EFFCOL", _TL("Boundary Effect Color"),
		_TL(""),
		PARAMETER_TYPE_Color, SG_GET_RGB(255, 255, 255)
	);

	//-----------------------------------------------------
	if( pEntry )
	{
		Load(*pEntry);
	}
}

//---------------------------------------------------------
CWKSP_Map_Graticule::~CWKSP_Map_Graticule(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Map_Graticule::Load(CSG_MetaData &Entry)
{
	return( m_Parameters.Serialize(Entry, false) );
}

//---------------------------------------------------------
bool CWKSP_Map_Graticule::Save(CSG_MetaData &Entry)
{
	return( m_Parameters.Serialize(*Entry.Add_Child("GRATICULE"), true) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxString CWKSP_Map_Graticule::Get_Name(void)
{
	wxString	Name(m_Parameters("NAME")->asString());

	if( !m_bShow )
	{
		return( "[" + Name + "]" );
	}

	return( Name );
}

//---------------------------------------------------------
wxString CWKSP_Map_Graticule::Get_Description(void)
{
	wxString	s;

	//-----------------------------------------------------
	s	+= wxString::Format("<h4>%s</h4>", _TL("Graticule"));

	s	+= "<table border=\"0\">";

	DESC_ADD_STR(_TL("Name"      ), m_Parameters("NAME")->asString());
	DESC_ADD_STR(_TL("Projection"), Get_Map()->Get_Projection().Get_Description().c_str());

	s	+= wxT("</table>");

	//-----------------------------------------------------
	return( s );
}

//---------------------------------------------------------
wxMenu * CWKSP_Map_Graticule::Get_Menu(void)
{
	wxMenu	*pMenu	= new wxMenu(m_Parameters("NAME")->asString());

	CMD_Menu_Add_Item(pMenu, false, ID_CMD_WKSP_ITEM_CLOSE);
	CMD_Menu_Add_Item(pMenu,  true, ID_CMD_MAPS_LAYER_SHOW);
	pMenu->AppendSeparator();
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_MAPS_MOVE_TOP);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_MAPS_MOVE_UP);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_MAPS_MOVE_DOWN);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_MAPS_MOVE_BOTTOM);

	return( pMenu );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Map_Graticule::On_Command(int Cmd_ID)
{
	switch( Cmd_ID )
	{
	default:
		return( CWKSP_Base_Item::On_Command(Cmd_ID) );

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
	}

	return( true );
}

//---------------------------------------------------------
bool CWKSP_Map_Graticule::On_Command_UI(wxUpdateUIEvent &event)
{
	switch( event.GetId() )
	{
	default:
		return( CWKSP_Base_Item::On_Command_UI(event) );

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
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CWKSP_Map_Graticule::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter, int Flags)
{
	if( Flags & PARAMETER_CHECK_ENABLE )
	{
		if(	!SG_STR_CMP(pParameter->Get_Identifier(), "INTERVAL") )
		{
			pParameters->Set_Enabled("FIXED"       , pParameter->asInt() == 0);
			pParameters->Set_Enabled("FITTED"      , pParameter->asInt() == 1);
		}

		if(	!SG_STR_CMP(pParameter->Get_Identifier(), "SHOW_ALWAYS") )
		{
			pParameters->Set_Enabled("SHOW_RANGE"  , pParameter->asBool() == false);
		}

		if(	!SG_STR_CMP(pParameter->Get_Identifier(), "LABEL") )
		{
			pParameters->Set_Enabled("LABEL_FONT"  , pParameter->asBool());
			pParameters->Set_Enabled("LABEL_SIZE"  , pParameter->asBool());
			pParameters->Set_Enabled("LABEL_EFFECT", pParameter->asBool());
			pParameters->Set_Enabled("LABEL_EFFCOL", pParameter->asBool());
		}
	}

	return( CWKSP_Base_Item::On_Parameter_Changed(pParameters, pParameter, Flags) );
}

//---------------------------------------------------------
void CWKSP_Map_Graticule::Parameters_Changed(void)
{
	CWKSP_Base_Item::Parameters_Changed();

	Get_Map()->View_Refresh(true);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Map_Graticule::Get_Graticule(const CSG_Rect &Extent)
{
	bool	bResult	= false;

	m_Graticule  .Create(SHAPE_TYPE_Line );
	m_Coordinates.Create(SHAPE_TYPE_Point);

	CSG_Module	*pModule	= SG_Get_Module_Library_Manager().Get_Module("pj_proj4", 14);

	if(	pModule && Get_Map()->Get_Projection().is_Okay() )
	{
		SG_UI_Msg_Lock     (true);
		SG_UI_Progress_Lock(true);

		pModule->Settings_Push();

		if( pModule->Set_Parameter("XMIN"      , Extent.Get_XMin())
		&&  pModule->Set_Parameter("XMAX"      , Extent.Get_XMax())
		&&  pModule->Set_Parameter("YMIN"      , Extent.Get_YMin())
		&&  pModule->Set_Parameter("YMAX"      , Extent.Get_YMax())
		&&  pModule->Set_Parameter("INTERVAL"  , m_Parameters("INTERVAL"))
		&&  pModule->Set_Parameter("FIXED"     , m_Parameters("FIXED"))
		&&  pModule->Set_Parameter("FITTED"    , m_Parameters("FITTED"))
		&&  pModule->Set_Parameter("RESOLUTION", m_Parameters("RESOLUTION"))
		&&  pModule->Set_Parameter("GRATICULE" , &m_Graticule)
		&&  pModule->Set_Parameter("COORDS"    , &m_Coordinates)
		&&  pModule->Set_Parameter("CRS_PROJ4" , Get_Map()->Get_Projection().Get_Proj4())
		&&  pModule->On_Before_Execution() && pModule->Execute() )
		{
			bResult	= true;
		}

		pModule->Settings_Pop();

		SG_UI_Msg_Lock     (false);
		SG_UI_Progress_Lock(false);
	}

	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Map_Graticule::Draw(CWKSP_Map_DC &dc_Map)
{
	if( !Get_Graticule(dc_Map.m_rWorld) || m_Graticule.Get_Count() <= 0 )
	{
		return( false );
	}

	if( !m_Parameters("SHOW_ALWAYS")->asBool() )
	{
		CSG_Parameter_Range	*pRange	= m_Parameters("SHOW_RANGE")->asRange();
		double	dRange	= dc_Map.m_rWorld.Get_XRange() > dc_Map.m_rWorld.Get_YRange() ? dc_Map.m_rWorld.Get_XRange() : dc_Map.m_rWorld.Get_YRange();

		if( dRange < pRange->Get_LoVal() || pRange->Get_HiVal() < dRange )
		{
			return( false );
		}
	}

	//-----------------------------------------------------
	CWKSP_Map_DC	*pDC	= m_Parameters("TRANSPARENCY")->asDouble() > 0.0 ? new CWKSP_Map_DC(dc_Map.m_rWorld, dc_Map.m_rDC, dc_Map.m_Scale, SG_GET_RGB(255, 255, 255)) : NULL;
	CWKSP_Map_DC	&dc		= pDC ? *pDC : dc_Map;

	//-----------------------------------------------------
	wxPen	Pen(m_Parameters("COLOR")->asColor(), m_Parameters("SIZE")->asInt());

	switch( m_Parameters("LINE_STYLE")->asInt() )
	{
	default:
	case  0:	Pen.SetStyle(wxPENSTYLE_SOLID           );	break; // Solid style.
	case  1:	Pen.SetStyle(wxPENSTYLE_DOT             );	break; // Dotted style.
	case  2:	Pen.SetStyle(wxPENSTYLE_LONG_DASH       );	break; // Long dashed style.
	case  3:	Pen.SetStyle(wxPENSTYLE_SHORT_DASH      );	break; // Short dashed style.
	case  4:	Pen.SetStyle(wxPENSTYLE_DOT_DASH        );	break; // Dot and dash style.
	case  5:	Pen.SetStyle(wxPENSTYLE_BDIAGONAL_HATCH );	break; // Backward diagonal hatch.
	case  6:	Pen.SetStyle(wxPENSTYLE_CROSSDIAG_HATCH );	break; // Cross-diagonal hatch.
	case  7:	Pen.SetStyle(wxPENSTYLE_FDIAGONAL_HATCH );	break; // Forward diagonal hatch.
	case  8:	Pen.SetStyle(wxPENSTYLE_CROSS_HATCH     );	break; // Cross hatch.
	case  9:	Pen.SetStyle(wxPENSTYLE_HORIZONTAL_HATCH);	break; // Horizontal hatch.
	case 10:	Pen.SetStyle(wxPENSTYLE_VERTICAL_HATCH  );	break; // Vertical hatch.
//	case 11:	Pen.SetStyle(wxPENSTYLE_STIPPLE         );	break; // Use the stipple bitmap. 
//	case 12:	Pen.SetStyle(wxPENSTYLE_USER_DASH       );	break; // Use the user dashes: see wxPen::SetDashes.
//	case 13:	Pen.SetStyle(wxPENSTYLE_TRANSPARENT     );	break; // No pen is used.
	}

	dc.dc.SetPen(Pen);

	//-----------------------------------------------------
	for(int iLine=0; iLine<m_Graticule.Get_Count(); iLine++)
	{
		CSG_Shape	*pLine	= m_Graticule.Get_Shape(iLine);

		for(int iPart=0; iPart<pLine->Get_Part_Count(); iPart++)
		{
			if( pLine->Get_Point_Count(iPart) > 1 )
			{
				TSG_Point_Int	B, A	= dc.World2DC(pLine->Get_Point(0, iPart));

				for(int iPoint=1; iPoint<pLine->Get_Point_Count(iPart); iPoint++)
				{
					B		= A;
					A		= dc.World2DC(pLine->Get_Point(iPoint, iPart));

					dc.dc.DrawLine(A.x, A.y, B.x, B.y);
				}
			}
		}
	}

	//-----------------------------------------------------
	if( m_Parameters("LABEL")->asBool() )
	{
		int	Size	= (int)(0.5 + 0.01 * m_Parameters("LABEL_SIZE")->asDouble()
		*	( dc.m_rDC.GetWidth() < dc.m_rDC.GetHeight()
			? dc.m_rDC.GetWidth() : dc.m_rDC.GetHeight() )
		);

		if( Size > 2 )
		{
			int			Effect;
			wxColour	Effect_Color	= Get_Color_asWX(m_Parameters("LABEL_EFFCOL")->asInt());
			wxFont		Font	= Get_Font(m_Parameters("LABEL_FONT"));

			Font.SetPointSize(Size);

			dc.dc.SetFont(Font);
			dc.dc.SetTextForeground(m_Parameters("LABEL_FONT")->asColor());

			switch( m_Parameters("LABEL_EFFECT")->asInt() )
			{
			default:	Effect	= TEXTEFFECT_NONE;			break;
			case 1:		Effect	= TEXTEFFECT_FRAME;			break;
			case 2:		Effect	= TEXTEFFECT_TOP;			break;
			case 3:		Effect	= TEXTEFFECT_TOPLEFT;		break;
			case 4:		Effect	= TEXTEFFECT_LEFT;			break;
			case 5:		Effect	= TEXTEFFECT_BOTTOMLEFT;	break;
			case 6:		Effect	= TEXTEFFECT_BOTTOM;		break;
			case 7:		Effect	= TEXTEFFECT_BOTTOMRIGHT;	break;
			case 8:		Effect	= TEXTEFFECT_RIGHT;			break;
			case 9:		Effect	= TEXTEFFECT_TOPRIGHT;		break;
			}

			for(int iPoint=0; iPoint<m_Coordinates.Get_Count(); iPoint++)
			{
				CSG_Shape	*pPoint	= m_Coordinates.Get_Shape(iPoint);

				TSG_Point_Int	p(dc.World2DC(pPoint->Get_Point(0)));
				wxString		Type(pPoint->asString(0));

				int	Align	= !Type.Cmp("LAT_MIN") ? TEXTALIGN_CENTERLEFT
							: !Type.Cmp("LAT_MAX") ? TEXTALIGN_CENTERRIGHT
							: !Type.Cmp("LON_MIN") ? TEXTALIGN_BOTTOMCENTER
							: !Type.Cmp("LON_MAX") ? TEXTALIGN_TOPCENTER
							: TEXTALIGN_CENTER;

				Draw_Text(dc.dc, Align, p.x, p.y, 0.0, pPoint->asString(1), Effect, Effect_Color);
			}
		}
	}

	//-----------------------------------------------------
	if( pDC )
	{
		dc_Map.Draw_DC(dc, m_Parameters("TRANSPARENCY")->asDouble() / 100.0);

		delete(pDC);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
