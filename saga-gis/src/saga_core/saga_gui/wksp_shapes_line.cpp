
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
//                 WKSP_Shapes_Line.cpp                  //
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
//                Germany                                //
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <saga_gdi/sgdi_helper.h>

#include "wksp_layer_classify.h"

#include "wksp_shapes_line.h"


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Shapes_Line::CWKSP_Shapes_Line(CSG_Shapes *pShapes)
	: CWKSP_Shapes(pShapes)
{
	On_Create_Parameters();

	DataObject_Changed();
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Shapes_Line::On_Create_Parameters(void)
{
	CWKSP_Shapes::On_Create_Parameters();

	//-----------------------------------------------------
	// Display...

	m_Parameters.Add_Choice("NODE_DISPLAY",
		"DISPLAY_POINTS"       , _TL("Show Vertices"),
		_TL(""),
		CSG_String::Format("%s|%s|%s",
			_TL("no"),
			_TL("yes"),
			_TL("with label")
		), 0
	);

	PenList_Add("NODE_DISPLAY",
		"LINE_STYLE"           , _TL("Line Style"),
		_TL("")
	);


	//-----------------------------------------------------
	// Size...

	m_Parameters.Add_Choice("NODE_SIZE",
		"SIZE_FIELD"           , _TL("Attribute"),
		_TL(""),
		"<not set>"
	);

	m_Parameters.Add_Range("SIZE_FIELD",
		"SIZE_RANGE"           , _TL("Size Range"),
		_TL(""),
		0, 10, 0, true
	);

	m_Parameters.Add_Int("SIZE_FIELD",
		"SIZE_DEFAULT"         , _TL("Default Size"),
		_TL(""),
		1, 1, true
	);

	m_Parameters.Add_Choice("NODE_SIZE",
		"SIZE_TYPE"            , _TL("Size relates to..."),
		_TL(""),
		CSG_String::Format("%s|%s",
			_TL("Screen"),
			_TL("Map Units")
		), 0
	);


	//-----------------------------------------------------
	// Labeling...

	m_Parameters.Add_Choice("LABEL_FIELD",
		"LABEL_STYLE"          , _TL("Style"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s",
			_TL("one label at the polyline centroid"),
			_TL("one label at the central vertex of each part"),
			_TL("one aligned label at the central vertex of each part"),
			_TL("along line labeling")
		), 3
	);

	m_Parameters.Add_Choice("LABEL_STYLE",
		"LABEL_ALIGN"          , _TL("Align"),
		_TL(""),
		CSG_String::Format("%s|%s|%s",
			_TL("top"),
			_TL("center"),
			_TL("bottom")
		), 1
	);

	m_Parameters.Add_Choice("LABEL_STYLE",
		"LABEL_ORIENT"         , _TL("Orientation"),
		_TL(""),
		CSG_String::Format("%s|%s",
			_TL("left"),
			_TL("right")
		), 1
	);

	m_Parameters.Add_Double("LABEL_STYLE",
		"LABEL_FREQUENCY"      , _TL("Along Line Frequency"),
		_TL("The distance between labels specified as multiples of the label's text width."),
		10., 0., true
	);


	//-----------------------------------------------------
	// Boundary Effect...

	m_Parameters.Add_Choice("NODE_DISPLAY",
		"BOUNDARY_EFFECT"      , _TL("Boundary Effect"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s|%s|%s|%s|%s|%s|%s",
			_TL("none"),
			_TL("all sides"),
			_TL("top"),
			_TL("top left"),
			_TL("left"),
			_TL("bottom left"),
			_TL("bottom"),
			_TL("bottom right"),
			_TL("right"),
			_TL("top right")
		), 0
	);

	m_Parameters.Add_Color("BOUNDARY_EFFECT",
		"BOUNDARY_EFFECT_COLOR", _TL("Color"),
		_TL(""),
		SG_GET_RGB(255, 255, 255)
	);
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Shapes_Line::On_DataObject_Changed(void)
{
	Set_Fields_Choice(m_Parameters("SIZE_FIELD"), true, true, true);

	CWKSP_Shapes::On_DataObject_Changed();
}

//---------------------------------------------------------
void CWKSP_Shapes_Line::On_Parameters_Changed(void)
{
	CWKSP_Shapes::On_Parameters_Changed();

	//-----------------------------------------------------
	m_Size_Type  = m_Parameters("SIZE_TYPE")->asInt();

	if(	(m_iSize = Get_Fields_Choice(m_Parameters("SIZE_FIELD"))) < 0
	 || (m_dSize = Get_Shapes()->Get_Maximum(m_iSize) - (m_Size_Min = Get_Shapes()->Get_Minimum(m_iSize))) <= 0. )
	{
		m_iSize  = -1;
		m_Size   = m_Parameters("SIZE_DEFAULT")->asInt();
	}
	else
	{
		m_Size   = (int)m_Parameters("SIZE_RANGE")->asRange()->Get_Min();
		m_dSize  =     (m_Parameters("SIZE_RANGE")->asRange()->Get_Max() - m_Size) / m_dSize;
	}

	//-----------------------------------------------------
	m_Pen = wxPen(m_pClassify->Get_Unique_Color(), (int)m_Size, (wxPenStyle)m_Line_Style);

	m_bVertices = m_Parameters("DISPLAY_POINTS")->asInt();

	//-----------------------------------------------------
	m_Effect_Color = m_Parameters("BOUNDARY_EFFECT_COLOR")->asColor();

	switch( m_Parameters("BOUNDARY_EFFECT")->asInt() )
	{
	default: m_Effect = TEXTEFFECT_NONE       ; break;
	case  1: m_Effect = TEXTEFFECT_FRAME      ; break;
	case  2: m_Effect = TEXTEFFECT_TOP        ; break;
	case  3: m_Effect = TEXTEFFECT_TOPLEFT    ; break;
	case  4: m_Effect = TEXTEFFECT_LEFT       ; break;
	case  5: m_Effect = TEXTEFFECT_BOTTOMLEFT ; break;
	case  6: m_Effect = TEXTEFFECT_BOTTOM     ; break;
	case  7: m_Effect = TEXTEFFECT_BOTTOMRIGHT; break;
	case  8: m_Effect = TEXTEFFECT_RIGHT      ; break;
	case  9: m_Effect = TEXTEFFECT_TOPRIGHT   ; break;
	}
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CWKSP_Shapes_Line::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter, int Flags)
{
	if( Flags & PARAMETER_CHECK_ENABLE )
	{
		if(	pParameter->Cmp_Identifier("SIZE_FIELD") )
		{
			int Value = Get_Fields_Choice(pParameter);

			pParameters->Set_Enabled("SIZE_RANGE"     , Value >= 0);
			pParameters->Set_Enabled("SIZE_DEFAULT"   , Value <  0);
		}

		if(	pParameter->Cmp_Identifier("LABEL_STYLE") )
		{
			pParameters->Set_Enabled("LABEL_ALIGN"    , pParameter->asInt() == 2 || pParameter->asInt() == 3);
			pParameters->Set_Enabled("LABEL_ORIENT"   , pParameter->asInt() == 2 || pParameter->asInt() == 3);
			pParameters->Set_Enabled("LABEL_FREQUENCY", pParameter->asInt() == 3);
		}

		if(	pParameter->Cmp_Identifier("BOUNDARY_EFFECT") )
		{
			pParameters->Set_Enabled("BOUNDARY_EFFECT_COLOR", pParameter->asInt() != 0);
		}
	}

	return( CWKSP_Shapes::On_Parameter_Changed(pParameters, pParameter, Flags) );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Shapes_Line::Get_Style_Size(int &min_Size, int &max_Size, double &min_Value, double &dValue, wxString *pName)
{
	if( m_iSize >= 0 )
	{
		min_Size  = (int)(m_Size);
		max_Size  = (int)(m_Size + ((Get_Shapes()->Get_Maximum(m_iSize) - m_Size_Min) * m_dSize));
		min_Value = m_Size_Min;
		dValue    = m_dSize;

		if( pName )
		{
			pName->Printf(Get_Shapes()->Get_Field_Name(m_iSize));
		}

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Shapes_Line::Draw_Initialize(CSG_Map_DC &dc_Map, int Flags)
{
	m_Label_Style  = m_Parameters("LABEL_STYLE"    )->asInt();
	m_Label_Freq   = m_Parameters("LABEL_FREQUENCY")->asInt();
	m_Label_Orient = m_Parameters("LABEL_ORIENT"   )->asInt();

	switch( m_Parameters("LABEL_ALIGN")->asInt() )
	{
	default: m_Label_Align = TEXTALIGN_TOPLEFT   ; break;
	case  1: m_Label_Align = TEXTALIGN_CENTERLEFT; break;
	case  2: m_Label_Align = TEXTALIGN_BOTTOMLEFT; break;
	}

	m_Pen.SetStyle((wxPenStyle)(m_Line_Style = PenList_Get_Style("LINE_STYLE")));

	dc_Map.SetPen(m_Pen);
}

//---------------------------------------------------------
void CWKSP_Shapes_Line::Draw_Shape(CSG_Map_DC &dc_Map, CSG_Shape *pShape, int Flags)
{
	wxPen Pen(m_Pen);

	//-----------------------------------------------------
	if( (Flags & LAYER_DRAW_FLAG_SELECTION) != 0 )
	{
		Pen.SetColour(m_Sel_Color);
		Pen.SetWidth (m_Size + ((Flags & LAYER_DRAW_FLAG_HIGHLIGHT) != 0 ? 2 : 0));
	}

	//-----------------------------------------------------
	else
	{
		int Color;

		if( !Get_Class_Color(pShape, Color) && !m_bNoData )
		{
			return;
		}

		Pen.SetColour(SG_GET_R(Color), SG_GET_G(Color), SG_GET_B(Color));

		//-------------------------------------------------
		double Size = m_Size;

		if( m_iSize >= 0 )
		{
			if( pShape->is_NoData(m_iSize) )
			{
				if( !m_bNoData )
				{
					return;
				}
			}
			else
			{
				Size += m_dSize * (pShape->asDouble(m_iSize) - m_Size_Min);
			}
		}

		switch( m_Size_Type )
		{
		default: Size *= dc_Map.Scale()   ; break;
		case  1: Size *= dc_Map.World2DC(); break;
		}

		Pen.SetWidth(Size < 0 ? 0 : (int)(0.5 + Size));
	}

	//-----------------------------------------------------
	if( m_Effect )
	{
		wxColour Color = Pen.GetColour(); Pen.SetColour(m_Effect_Color); dc_Map.SetPen(Pen);

		if( m_Effect & TEXTEFFECT_TOP         )	_Draw_Shape(dc_Map, pShape,  0, -1);
		if( m_Effect & TEXTEFFECT_TOPLEFT     )	_Draw_Shape(dc_Map, pShape, -1, -1);
		if( m_Effect & TEXTEFFECT_LEFT        )	_Draw_Shape(dc_Map, pShape, -1,  0);
		if( m_Effect & TEXTEFFECT_BOTTOMLEFT  )	_Draw_Shape(dc_Map, pShape, -1,  1);
		if( m_Effect & TEXTEFFECT_BOTTOM      )	_Draw_Shape(dc_Map, pShape,  0,  1);
		if( m_Effect & TEXTEFFECT_BOTTOMRIGHT )	_Draw_Shape(dc_Map, pShape,  1,  1);
		if( m_Effect & TEXTEFFECT_RIGHT       )	_Draw_Shape(dc_Map, pShape,  1,  0);
		if( m_Effect & TEXTEFFECT_TOPRIGHT    )	_Draw_Shape(dc_Map, pShape,  1, -1);

		Pen.SetColour(Color);
	}

	//-----------------------------------------------------
	dc_Map.SetPen(Pen);

	_Draw_Shape(dc_Map, pShape);

	dc_Map.SetPen(m_Pen);
}

//---------------------------------------------------------
void CWKSP_Shapes_Line::_Draw_Shape(CSG_Map_DC &dc_Map, CSG_Shape *pShape, int xOffset, int yOffset)
{
	for(int iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
	{
		if( pShape->Get_Point_Count(iPart) > 1 )
		{
			TSG_Point_Int A = dc_Map.World2DC(pShape->Get_Point(0, iPart)); A.x += xOffset; A.y += yOffset;

			for(int iPoint=1; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
			{
				TSG_Point_Int B = A; A = dc_Map.World2DC(pShape->Get_Point(iPoint, iPart)); A.x += xOffset; A.y += yOffset;

				dc_Map.DrawLine(A.x, A.y, B.x, B.y);
			}
		}
	}
}

//---------------------------------------------------------
#define GET_ANGLE(a, b)	M_RAD_TO_DEG * (b.x != a.x ? M_PI_180 - atan2((double)(b.y - a.y), (double)(b.x - a.x)) : (b.y > a.y ? M_PI_270 : (b.y < a.y ? M_PI_090 : 0.0)))

//---------------------------------------------------------
void CWKSP_Shapes_Line::Draw_Label(CSG_Map_DC &dc_Map, CSG_Shape *pShape, const wxString &Label)
{
	CSG_Shape_Line *pLine = (CSG_Shape_Line *)pShape;

	switch( m_Label_Style )
	{
	//-----------------------------------------------------
	case  0: // one label at the polyline centroid
		{
			TSG_Point_Int C = dc_Map.World2DC(pLine->Get_Centroid());

			dc_Map.DrawText(TEXTALIGN_CENTER, C.x, C.y, Label, m_Label.Effect, m_Label.Effect_Color, m_Label.Effect_Size);
		}
		break;

	//-----------------------------------------------------
	case  1: // one label at the central vertex of each part
	case  2: // one aligned label at the central vertex of each part
		{
			for(int iPart=0; iPart<pLine->Get_Part_Count(); iPart++)
			{
				double d = 0., c = 0.5 * pLine->Get_Length(iPart);

				for(int i=1, j=0; d<c && i<pLine->Get_Point_Count(iPart); i++, j++)
				{
					d += SG_Get_Distance(pLine->Get_Point(i, iPart), pLine->Get_Point(j, iPart));

					if( d >= c )
					{
						if( m_Label_Style == 1 )
						{
							TSG_Point A = pLine->Get_Point(i, iPart);
							TSG_Point B = pLine->Get_Point(j, iPart);

							A.x += 0.5 * (B.x - A.x);
							A.y += 0.5 * (B.y - A.y);

							TSG_Point_Int C = dc_Map.World2DC(A);

							dc_Map.DrawText(TEXTALIGN_CENTER, C.x, C.y, Label, m_Label.Effect, m_Label.Effect_Color, m_Label.Effect_Size);
						}
						else
						{
							TSG_Point_Int A = dc_Map.World2DC(pLine->Get_Point(i, iPart));
							TSG_Point_Int B = dc_Map.World2DC(pLine->Get_Point(j, iPart));

							if( m_Label_Orient == 0 )
							{
								dc_Map.DrawText(m_Label_Align, A.x, A.y, GET_ANGLE(B, A), Label, m_Label.Effect, m_Label.Effect_Color, m_Label.Effect_Size);
							}
							else
							{
								dc_Map.DrawText(m_Label_Align, B.x, B.y, GET_ANGLE(A, B), Label, m_Label.Effect, m_Label.Effect_Color, m_Label.Effect_Size);
							}
						}
					}
				}
			}
		}
		break;

	//-----------------------------------------------------
	case  3: // along line labeling
		{
			wxCoord Width, Height; dc_Map.Get_DC().GetTextExtent(Label, &Width, &Height);

			for(int iPart=0; iPart<pLine->Get_Part_Count(); iPart++)
			{
				if( pLine->Get_Point_Count(iPart) < 2 || Width > dc_Map.World2DC() * pLine->Get_Length(iPart) )
				{
					continue;
				}

				bool bLabel = false; double d = 0.; TSG_Point_Int B, A = dc_Map.World2DC(pLine->Get_Point(0, iPart));

				for(int iPoint=1; iPoint<pLine->Get_Point_Count(iPart); iPoint++)
				{
					//-------------------------------------
					if( !bLabel )
					{
						B = A; A = dc_Map.World2DC(pLine->Get_Point(iPoint, iPart));

						if( (d += SG_Get_Distance(A.x, A.y, B.x, B.y)) > m_Label_Freq * Width )
						{
							B = A; bLabel = true;
						}
					}

					//-------------------------------------
					else
					{
						A = dc_Map.World2DC(pLine->Get_Point(iPoint, iPart));

						if( SG_Get_Distance(A.x, A.y, B.x, B.y) > Width )
						{
							d = 0.; bLabel = false;

							if( m_Label_Orient == 0 )
							{
								dc_Map.DrawText(m_Label_Align, A.x, A.y, GET_ANGLE(B, A), Label, m_Label.Effect, m_Label.Effect_Color, m_Label.Effect_Size);
							}
							else
							{
								dc_Map.DrawText(m_Label_Align, B.x, B.y, GET_ANGLE(A, B), Label, m_Label.Effect, m_Label.Effect_Color, m_Label.Effect_Size);
							}
						}
					}
				}
			}
		}
		break;
	}
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Shapes_Line::Edit_Do_Mouse_Move_Draw(bool bMouseDown)
{
	return( CWKSP_Shapes::Edit_Do_Mouse_Move_Draw(bMouseDown) || (m_Edit.pShape && m_Edit.Part >= 0 && (m_Edit.Point < 0 || bMouseDown)) );
}

//---------------------------------------------------------
void CWKSP_Shapes_Line::Edit_Shape_Draw_Move(wxDC &dc, const CSG_Rect &rWorld, const wxPoint &Point)
{
	if( m_Edit.pShape && m_Edit.Part >= 0 )
	{
		int nPoints = m_Edit.pShape->Get_Point_Count(m_Edit.Part);

		if( m_Edit.Point < 0 )
		{
			if( nPoints > 0 )
			{
				CWKSP_Shapes::Edit_Shape_Draw_Move(dc, rWorld, Point,
					m_Edit.pShape->Get_Point(nPoints - 1, m_Edit.Part)
				);
			}
		}
		else
		{
			if( nPoints > 1 )
			{
				if( m_Edit.Point > 0 )
				{
					CWKSP_Shapes::Edit_Shape_Draw_Move(dc, rWorld, Point,
						m_Edit.pShape->Get_Point(m_Edit.Point - 1, m_Edit.Part)
					);
				}

				if( m_Edit.Point < nPoints - 1 )
				{
					CWKSP_Shapes::Edit_Shape_Draw_Move(dc, rWorld, Point,
						m_Edit.pShape->Get_Point(m_Edit.Point + 1, m_Edit.Part)
					);
				}
			}

			CWKSP_Shapes::Edit_Shape_Draw_Move(dc, rWorld, Point);
		}
	}
}

//---------------------------------------------------------
void CWKSP_Shapes_Line::Edit_Shape_Draw(CSG_Map_DC &dc_Map)
{
	if( m_Edit.pShape )
	{
		if( m_Edit.bGleam )
		{
			dc_Map.SetPen(wxPen(m_Edit.Color, 3));
			dc_Map.Get_DC().SetLogicalFunction(wxINVERT);

			_Draw_Shape(dc_Map, m_Edit.pShape);

			dc_Map.Get_DC().SetLogicalFunction(wxCOPY);
		}

		dc_Map.SetPen(wxPen(m_Edit.Color));

		_Draw_Shape(dc_Map, m_Edit.pShape);

		CWKSP_Shapes::Edit_Shape_Draw(dc_Map);
	}
}

//---------------------------------------------------------
int CWKSP_Shapes_Line::Edit_Shape_HitTest(const CSG_Point &Point, double max_Dist, int &pos_iPart, int &pos_iPoint)
{
	int Result = CWKSP_Shapes::Edit_Shape_HitTest(Point, max_Dist, pos_iPart, pos_iPoint);

	if( Result == 0 && m_Edit.pShape )
	{
		for(int iPart=0; iPart<m_Edit.pShape->Get_Part_Count(); iPart++)
		{
			CSG_Point A = m_Edit.pShape->Get_Point(0, iPart);

			for(int iPoint=1; iPoint<m_Edit.pShape->Get_Point_Count(iPart); iPoint++)
			{
				CSG_Point C, B = m_Edit.pShape->Get_Point(iPoint, iPart); double d = SG_Get_Nearest_Point_On_Line(Point, A, B, C, true); A = B;

				if( d >= 0. && (0. > max_Dist || d < max_Dist) )
				{
					Result = 2; max_Dist = d; pos_iPoint = iPoint; pos_iPart = iPart;
				}
			}
		}
	}

	return( Result );
}

//---------------------------------------------------------
void CWKSP_Shapes_Line::Edit_Snap_Point_ToLine(const CSG_Point &Point, CSG_Point &snap_Point, double &snap_Dist, CSG_Shape *pShape)
{
	for(int iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
	{
		CSG_Point A = pShape->Get_Point(0, iPart);

		for(int iPoint=1; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
		{
			CSG_Point C, B = pShape->Get_Point(iPoint, iPart); double d = SG_Get_Nearest_Point_On_Line(Point, A, B, C, true); A = B;

			if( d >= 0. && d < snap_Dist )
			{
				snap_Dist = d; snap_Point = C;
			}
		}
	}
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
