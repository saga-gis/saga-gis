
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
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <wx/window.h>

#include <saga_api/saga_api.h>
#include <saga_gdi/sgdi_helper.h>

#include "helper.h"

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
	m_bShow	= true;

	//-----------------------------------------------------
	m_Parameters.Set_Name      ("GRATICULE");
	m_Parameters.Set_Identifier("GRATICULE");

	//-----------------------------------------------------
	m_Parameters.Add_Node("", "NODE_GENERAL", _TL("General"), _TL(""));

	m_Parameters.Add_String("NODE_GENERAL",
		"NAME"		, _TL("Name"),
		_TL(""),
		_TL("Graticule")
	);

	m_Parameters.Add_Choice("NODE_GENERAL",
		"INTERVAL"	, _TL("Interval"),
		_TL(""),
		CSG_String::Format("%s|%s",
			_TL("fixed interval"),
			_TL("fitted interval")
		), 1
	);

	m_Parameters.Add_Double("INTERVAL",
		"FIXED"		, _TL("Fixed Interval (Degree)"),
		_TL(""),
		5.0, 0.0, true, 20.0
	);

	m_Parameters.Add_Int("INTERVAL",
		"FITTED"	, _TL("Number of Intervals"),
		_TL(""),
		5, 1, true
	);

	m_Parameters.Add_Double("NODE_GENERAL",
		"RESOLUTION", _TL("Minimum Resolution (Degree)"),
		_TL(""),
		0.5, 0.0, true
	);

	//-----------------------------------------------------
	m_Parameters.Add_Bool("NODE_GENERAL",
		"SHOW_ALWAYS", _TL("Show at all scales"),
		_TL(""),
		true
	);

	m_Parameters.Add_Range("SHOW_ALWAYS",
		"SHOW_RANGE", _TL("Scale Range"),
		_TL("only show within scale range; values are given as extent measured in map units"),
		100.0, 1000.0, 0.0, true
	);

	//-----------------------------------------------------
	m_Parameters.Add_Node("", "NODE_DISPLAY", _TL("Display"), _TL(""));

	m_Parameters.Add_Color("NODE_DISPLAY",
		"COLOR"			, _TL("Color"),
		_TL(""),
		SG_COLOR_GREY
	);

	m_Parameters.Add_Int("NODE_DISPLAY",
		"SIZE"			, _TL("Size"),
		_TL(""),
		0, 0, true
	);

	m_Parameters.Add_Double("NODE_DISPLAY",
		"TRANSPARENCY"	, _TL("Transparency [%]"),
		_TL(""),
		0.0, 0.0, true, 100.0, true
	);

	m_Parameters.Add_Choice("NODE_DISPLAY",
		"LINE_STYLE"	, _TL("Line Style"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s",
			_TL("Solid style"            ),
			_TL("Dotted style"           ),
			_TL("Long dashed style"      ),
			_TL("Short dashed style"     ), 
			_TL("Dot and dash style"     ),
			_TL("Backward diagonal hatch"),
			_TL("Cross-diagonal hatch"   ),
			_TL("Forward diagonal hatch" ),
			_TL("Cross hatch"            ),
			_TL("Horizontal hatch"       ),
			_TL("Vertical hatch"         )
		//	_TL("Use the stipple bitmap" )
		//	_TL("Use the user dashes"    )
		//	_TL("No pen is used"         )
		), 4
	);

	//-----------------------------------------------------
	m_Parameters.Add_Bool("NODE_DISPLAY",
		"LABEL"			, _TL("Label"),
		_TL(""),
		true
	);

	m_Parameters.Add_Choice("LABEL",
		"LABEL_UNITS"	, _TL("Units"),
		_TL(""),
		CSG_String::Format("%s|%s|%s",
			_TL("Decimal Degrees"),
			_TL("Degrees, Minutes, Seconds"),
			_TL("Degrees, Minutes, Seconds (significant)")
		), 0
	);

	m_Parameters.Add_Int("LABEL",
		"LABEL_DECIMALS", _TL("Decimals"),
		_TL("Maximum number of decimals of a second to be printed."),
		2, 1, true
	);

	m_Parameters.Add_Font("LABEL",
		"LABEL_FONT"	, _TL("Font"),
		_TL("")
	);

	m_Parameters.Add_Double("LABEL",
		"LABEL_SIZE"	, _TL("Size"),
		_TL("Font size given as percentage of map size."),
		2.0, 0.0, true, 10.0, true
	);

	m_Parameters.Add_Choice("LABEL",
		"LABEL_EFFECT"	, _TL("Boundary Effect"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s|%s|%s|%s|%s|%s|%s",
			_TL("none"        ),
			_TL("full frame"  ),
			_TL("top"         ),
			_TL("top left"    ),
			_TL("left"        ),
			_TL("bottom left" ),
			_TL("bottom"      ),
			_TL("bottom right"),
			_TL("right"       ),
			_TL("top right"   )
		), 1
	);

	m_Parameters.Add_Color("LABEL",
		"LABEL_EFFCOL"	, _TL("Boundary Effect Color"),
		_TL(""),
		SG_GET_RGB(255, 255, 255)
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
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxString CWKSP_Map_Graticule::Get_Name(void)
{
	wxString	Name(m_Parameters("NAME")->asString());

	return( !m_bShow ? "* " + Name : Name );
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
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CWKSP_Map_Graticule::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter, int Flags)
{
	if( Flags & PARAMETER_CHECK_ENABLE )
	{
		if(	pParameter->Cmp_Identifier("INTERVAL") )
		{
			pParameters->Set_Enabled("FIXED"       , pParameter->asInt() == 0);
			pParameters->Set_Enabled("FITTED"      , pParameter->asInt() == 1);
		}

		if(	pParameter->Cmp_Identifier("SHOW_ALWAYS") )
		{
			pParameters->Set_Enabled("SHOW_RANGE"  , pParameter->asBool() == false);
		}

		if(	pParameter->Cmp_Identifier("LABEL") )
		{
			pParameter->Set_Children_Enabled(pParameter->asBool());
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
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Map_Graticule::Get_Graticule(const CSG_Rect &Extent)
{
	bool	bResult	= false;

	m_Graticule  .Create(SHAPE_TYPE_Line );
	m_Coordinates.Create(SHAPE_TYPE_Point);

	CSG_Tool	*pTool	= SG_Get_Tool_Library_Manager().Create_Tool("pj_proj4", 14);

	if(	pTool && Get_Map()->Get_Projection().is_Okay() )
	{
		SG_UI_ProgressAndMsg_Lock(true);

		pTool->Set_Manager(NULL);

		bResult	=  pTool->Set_Parameter("XMIN"      , Extent.Get_XMin())
				&& pTool->Set_Parameter("XMAX"      , Extent.Get_XMax())
				&& pTool->Set_Parameter("YMIN"      , Extent.Get_YMin())
				&& pTool->Set_Parameter("YMAX"      , Extent.Get_YMax())
				&& pTool->Set_Parameter("INTERVAL"  , m_Parameters("INTERVAL"  ))
				&& pTool->Set_Parameter("FIXED"     , m_Parameters("FIXED"     ))
				&& pTool->Set_Parameter("FITTED"    , m_Parameters("FITTED"    ))
				&& pTool->Set_Parameter("RESOLUTION", m_Parameters("RESOLUTION"))
				&& pTool->Set_Parameter("GRATICULE" , &m_Graticule)
				&& pTool->Set_Parameter("COORDS"    , &m_Coordinates)
				&& pTool->Set_Parameter("CRS_PROJ4" , Get_Map()->Get_Projection().Get_Proj4())
				&& pTool->On_Before_Execution()
				&& pTool->Execute();

		SG_UI_ProgressAndMsg_Lock(false);
	}

	SG_Get_Tool_Library_Manager().Delete_Tool(pTool);

	return( bResult );
}


///////////////////////////////////////////////////////////
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

		if( dRange < pRange->Get_Min() || pRange->Get_Max() < dRange )
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
			int			Effect, Units	= m_Parameters("LABEL_UNITS")->asInt(), Decimals = m_Parameters("LABEL_DECIMALS")->asInt();
			wxColour	Effect_Color	= Get_Color_asWX(m_Parameters("LABEL_EFFCOL")->asInt());
			wxFont		Font	= Get_Font(m_Parameters("LABEL_FONT"));

			Font.SetPointSize(Size);

			dc.dc.SetFont(Font);
			dc.dc.SetTextForeground(m_Parameters("LABEL_FONT")->asColor());

			switch( m_Parameters("LABEL_EFFECT")->asInt() )
			{
			default:	Effect	= TEXTEFFECT_NONE       ;	break;
			case  1:	Effect	= TEXTEFFECT_FRAME      ;	break;
			case  2:	Effect	= TEXTEFFECT_TOP        ;	break;
			case  3:	Effect	= TEXTEFFECT_TOPLEFT    ;	break;
			case  4:	Effect	= TEXTEFFECT_LEFT       ;	break;
			case  5:	Effect	= TEXTEFFECT_BOTTOMLEFT ;	break;
			case  6:	Effect	= TEXTEFFECT_BOTTOM     ;	break;
			case  7:	Effect	= TEXTEFFECT_BOTTOMRIGHT;	break;
			case  8:	Effect	= TEXTEFFECT_RIGHT      ;	break;
			case  9:	Effect	= TEXTEFFECT_TOPRIGHT   ;	break;
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

				CSG_String	Coordinate(Get_Unit(pPoint, Units, Decimals, (Align & TEXTALIGN_YCENTER) != 0));

				Draw_Text(dc.dc, Align, p.x, p.y, 0.0, Coordinate.c_str(), Effect, Effect_Color);
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


//---------------------------------------------------------
CSG_String CWKSP_Map_Graticule::Get_Unit(CSG_Shape *pPoint, int Units, int Decimals, bool bLatitude)
{
	double	Value	= pPoint->asDouble(1);

	const SG_Char	*D	= SG_T("\xb0");

	const SG_Char	*C	= bLatitude
		? (Value < 0. ? SG_T("S") : SG_T("N"))
		: (Value > 0. ? SG_T("E") : SG_T("W"));

	CSG_String	String;

	//-----------------------------------------------------
	if( Units == 0 )	// decimal degrees
	{
	//	String.Printf("%s%s", pPoint->asString(1) + (Value < 0 ? 1 : 0), D); // no! label already includes degree symbol
		String.Printf("%s", pPoint->asString(1) + (Value < 0 ? 1 : 0));
	}

	//-----------------------------------------------------
	else				// degrees, minutes, seconds
	{
		if( Value < 0. )
		{
			Value	= -Value;
		}

		Value	= fmod(Value, 360.);
		int d	= (int)Value;
		Value	= (Value - d) * 60.;
		int h	= (int)Value;
		Value	= (Value - h) * 60.;
		int s	= (int)Value;
		Value	= (Value - s);

		//-------------------------------------------------
		if( Value > 0. )	// floating point remainder
		{
			int	n	= SG_Get_Significant_Decimals(Value, Decimals);

			String.Printf("%d%s%02d'%02d.%d''", d, D, h, s, (int)(Value * pow(10, n)));
		}
		else if( s > 0 || Units == 1 )
		{
			String.Printf("%d%s%02d'%02d''"   , d, D, h, s);
		}
		else if( h > 0 )
		{
			String.Printf("%d%s%02d'"         , d, D, h);
		}
		else
		{
			String.Printf("%d%s"              , d, D);
		}
	}

	String	+= C;

	return( String );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
