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
#include "dc_helper.h"

#include "wksp_layer_classify.h"

#include "wksp_shapes_line.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Shapes_Line::CWKSP_Shapes_Line(CSG_Shapes *pShapes)
	: CWKSP_Shapes(pShapes)
{
	On_Create_Parameters();

	DataObject_Changed();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Shapes_Line::On_Create_Parameters(void)
{
	CWKSP_Shapes::On_Create_Parameters();

	//-----------------------------------------------------
	// Display...

	m_Parameters.Add_Value(
		m_Parameters("NODE_DISPLAY"), "DISPLAY_POINTS"	, _TL("Show Vertices"),
		_TL(""),
		PARAMETER_TYPE_Bool, false
	);

	m_Parameters.Add_Choice(
		m_Parameters("NODE_DISPLAY"), "LINE_STYLE"		, _TL("Line Style"),
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
		), 0
	);

	//-----------------------------------------------------
	// Size...

	m_Parameters.Add_Choice(
		m_Parameters("NODE_SIZE")		, "SIZE_TYPE"		, _TL("Size relates to..."),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("Screen"),
			_TL("Map Units")
		), 0
	);

	AttributeList_Add(
		m_Parameters("NODE_SIZE")		, "SIZE_ATTRIB"		, _TL("Attribute"),
		_TL("")
	);

	m_Parameters.Add_Range(
		m_Parameters("SIZE_ATTRIB")		, "SIZE_RANGE"		, _TL("Size Range"),
		_TL(""),
		0, 10, 0, true
	);

	m_Parameters.Add_Value(
		m_Parameters("SIZE_ATTRIB")		, "SIZE_DEFAULT"	, _TL("Default Size"),
		_TL(""),
		PARAMETER_TYPE_Int, 0, 0, true
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Shapes_Line::On_DataObject_Changed(void)
{
	CWKSP_Shapes::On_DataObject_Changed();

	AttributeList_Set(m_Parameters("SIZE_ATTRIB"), true);
}

//---------------------------------------------------------
void CWKSP_Shapes_Line::On_Parameters_Changed(void)
{
	CWKSP_Shapes::On_Parameters_Changed();

	//-----------------------------------------------------
	m_Size_Type		= m_Parameters("SIZE_TYPE")->asInt();

	if(	(m_iSize	= m_Parameters("SIZE_ATTRIB")->asInt()) >= Get_Shapes()->Get_Field_Count()
	||	(m_dSize	= Get_Shapes()->Get_Maximum(m_iSize) - (m_Size_Min = Get_Shapes()->Get_Minimum(m_iSize))) <= 0.0 )
	{
		m_iSize		= -1;
		m_Size		= m_Parameters("SIZE_DEFAULT")->asInt();
	}
	else
	{
		m_Size		= (int)m_Parameters("SIZE_RANGE")->asRange()->Get_LoVal();
		m_dSize		=     (m_Parameters("SIZE_RANGE")->asRange()->Get_HiVal() - m_Size) / m_dSize;
	}

	//-----------------------------------------------------
	m_Pen		= wxPen(m_Def_Color, (int)m_Size, m_Line_Style);

	m_bVertices	= m_Parameters("DISPLAY_POINTS")->asBool();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CWKSP_Shapes_Line::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter, int Flags)
{
	//-----------------------------------------------------
	if( Flags & PARAMETER_CHECK_ENABLE )
	{
		if(	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("SIZE_ATTRIB")) )
		{
			bool	Value	= pParameter->asInt() < Get_Shapes()->Get_Field_Count();

			pParameters->Get_Parameter("SIZE_RANGE"  )->Set_Enabled(Value == true);
			pParameters->Get_Parameter("SIZE_DEFAULT")->Set_Enabled(Value == false);
		}
	}

	return( CWKSP_Shapes::On_Parameter_Changed(pParameters, pParameter, Flags) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Shapes_Line::Get_Style_Size(int &min_Size, int &max_Size, double &min_Value, double &dValue, wxString *pName)
{
	if( m_iSize >= 0 )
	{
		min_Size	= (int)(m_Size);
		max_Size	= (int)(m_Size + ((Get_Shapes()->Get_Maximum(m_iSize) - m_Size_Min) * m_dSize));
		min_Value	= m_Size_Min;
		dValue		= m_dSize;

		if( pName )
		{
			pName->Printf(Get_Shapes()->Get_Field_Name(m_iSize));
		}

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
void CWKSP_Shapes_Line::Draw_Initialize(CWKSP_Map_DC &dc_Map)
{
	switch( m_Parameters("LINE_STYLE")->asInt() )
	{
	case  0:	m_Line_Style	= wxSOLID;				break; // Solid style.
	case  1:	m_Line_Style	= wxDOT;				break; // Dotted style.
	case  2:	m_Line_Style	= wxLONG_DASH;			break; // Long dashed style.
	case  3:	m_Line_Style	= wxSHORT_DASH;			break; // Short dashed style.
	case  4:	m_Line_Style	= wxDOT_DASH;			break; // Dot and dash style.
	case  5:	m_Line_Style	= wxBDIAGONAL_HATCH;	break; // Backward diagonal hatch.
	case  6:	m_Line_Style	= wxCROSSDIAG_HATCH;	break; // Cross-diagonal hatch.
	case  7:	m_Line_Style	= wxFDIAGONAL_HATCH;	break; // Forward diagonal hatch.
	case  8:	m_Line_Style	= wxCROSS_HATCH;		break; // Cross hatch.
	case  9:	m_Line_Style	= wxHORIZONTAL_HATCH;	break; // Horizontal hatch.
	case 10:	m_Line_Style	= wxVERTICAL_HATCH;		break; // Vertical hatch.
//	case 11:	m_Line_Style	= wxSTIPPLE;			break; // Use the stipple bitmap. 
//	case 12:	m_Line_Style	= wxUSER_DASH;			break; // Use the user dashes: see wxPen::SetDashes.
//	case 13:	m_Line_Style	= wxTRANSPARENT;		break; // No pen is used.
	}

	m_Pen.SetStyle(m_Line_Style);

	dc_Map.dc.SetPen(m_Pen);
}

//---------------------------------------------------------
void CWKSP_Shapes_Line::Draw_Shape(CWKSP_Map_DC &dc_Map, CSG_Shape *pShape, int Selection)
{
	if( m_iSize >= 0 && pShape->is_NoData(m_iSize) )
	{
		return;
	}

	//-----------------------------------------------------
	if( Selection )
	{
		dc_Map.dc.SetPen(wxPen(m_Sel_Color, m_Size + (Selection == 1 ? 2 : 0), m_Line_Style));
	}
	else if( m_iColor >= 0 || m_iSize >= 0 )
	{
		int		Color;
		wxPen	Pen(m_Pen);

		if( Get_Class_Color(pShape, Color) )
		{
			Pen.SetColour(SG_GET_R(Color), SG_GET_G(Color), SG_GET_B(Color));
		}

		double	dSize	= m_iSize < 0 ? m_Size
						: m_Size + (pShape->asDouble(m_iSize) - m_Size_Min) * m_dSize;

		switch( m_Size_Type )
		{
		default:
		case 0:	dSize	*= dc_Map.m_Scale;		break;
		case 1:	dSize	*= dc_Map.m_World2DC;	break;
		}

		if( dSize >= 0 )
		{
			Pen.SetWidth((int)(0.5 + dSize));
		}

		dc_Map.dc.SetPen(Pen);
	}

	//-----------------------------------------------------
	for(int iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
	{
		if( pShape->Get_Point_Count(iPart) > 1 )
		{
			TSG_Point_Int	B, A	= dc_Map.World2DC(pShape->Get_Point(0, iPart));

			for(int iPoint=1; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
			{
				B	= A;	A	= dc_Map.World2DC(pShape->Get_Point(iPoint, iPart));

				dc_Map.dc.DrawLine(A.x, A.y, B.x, B.y);
			}
		}
	}

	//-----------------------------------------------------
	if( Selection )
	{
		dc_Map.dc.SetPen(m_Pen);
	}
}

//---------------------------------------------------------
#define GET_ANGLE(a, b)	M_RAD_TO_DEG * (b.x != a.x ? M_PI_180 - atan2((double)(b.y - a.y), (double)(b.x - a.x)) : (b.y > a.y ? M_PI_270 : (b.y < a.y ? M_PI_090 : 0.0)))

//---------------------------------------------------------
void CWKSP_Shapes_Line::Draw_Label(CWKSP_Map_DC &dc_Map, CSG_Shape *pShape, const wxString &Label)
{
	wxCoord			Width, Height;

	dc_Map.dc.GetTextExtent(Label, &Width, &Height);

	m_Label_Freq	= 10;

	//-----------------------------------------------------
	for(int iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
	{
		if( dc_Map.m_World2DC * ((CSG_Shape_Line *)pShape)->Get_Length(iPart) > (2 * m_Label_Freq) * Width )
		{
			bool			bLabel	= false;
			double			d		= 0.0;
			TSG_Point_Int	B, A	= dc_Map.World2DC(pShape->Get_Point(0, iPart));

			for(int iPoint=1; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
			{
				//-----------------------------------------
				if( !bLabel )
				{
					B		= A;
					A		= dc_Map.World2DC(pShape->Get_Point(iPoint, iPart));

					if( (d += SG_Get_Distance(A.x, A.y, B.x, B.y)) > m_Label_Freq * Width )
					{
						bLabel	= true;
						B		= A;
					}
				}

				//-----------------------------------------
				else
				{
					A		= dc_Map.World2DC(pShape->Get_Point(iPoint, iPart));

					if( SG_Get_Distance(A.x, A.y, B.x, B.y) > Width )
					{
						bLabel	= false;
						d		= 0.0;

						Draw_Text(dc_Map.dc, TEXTALIGN_TOPLEFT, B.x, B.y, GET_ANGLE(A, B), Label, m_Label_Eff, m_Label_Eff_Color);

					//	dc_Map.dc.DrawRotatedText(s, B.x, B.y, GET_ANGLE(A, B));
					//	dc_Map.dc.DrawCircle(A.x, A.y, 3);	dc_Map.dc.DrawCircle(B.x, B.y, 3);
					}
				}
			}
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Shapes_Line::Edit_Shape_Draw_Move(wxDC &dc, const CSG_Rect &rWorld, const wxPoint &Point)
{
	if( m_Edit_pShape && m_Edit_iPart >= 0 )
	{
		int	nPoints	= m_Edit_pShape->Get_Point_Count(m_Edit_iPart);

		if( m_Edit_iPoint < 0 )
		{
			if( nPoints > 0 )
			{
				CWKSP_Shapes::Edit_Shape_Draw_Move(dc, rWorld, Point,
					m_Edit_pShape->Get_Point(nPoints - 1, m_Edit_iPart)
				);
			}
		}
		else
		{
			if( nPoints > 1 )
			{
				if( m_Edit_iPoint > 0 )
				{
					CWKSP_Shapes::Edit_Shape_Draw_Move(dc, rWorld, Point,
						m_Edit_pShape->Get_Point(m_Edit_iPoint - 1, m_Edit_iPart)
					);
				}

				if( m_Edit_iPoint < nPoints - 1 )
				{
					CWKSP_Shapes::Edit_Shape_Draw_Move(dc, rWorld, Point,
						m_Edit_pShape->Get_Point(m_Edit_iPoint + 1, m_Edit_iPart)
					);
				}
			}

			CWKSP_Shapes::Edit_Shape_Draw_Move(dc, rWorld, Point);
		}
	}
}

//---------------------------------------------------------
void CWKSP_Shapes_Line::Edit_Shape_Draw(CWKSP_Map_DC &dc_Map)
{
	int			iPart, iPoint;
	TSG_Point_Int	ptA, ptB;

	if( m_Edit_pShape )
	{
		dc_Map.dc.SetPen(wxPen(m_Edit_Color));

		for(iPart=0; iPart<m_Edit_pShape->Get_Part_Count(); iPart++)
		{
			if( m_Edit_pShape->Get_Point_Count(iPart) > 1 )
			{
				ptA		= dc_Map.World2DC(m_Edit_pShape->Get_Point(0, iPart));

				for(iPoint=1; iPoint<m_Edit_pShape->Get_Point_Count(iPart); iPoint++)
				{
					ptB		= ptA;
					ptA		= dc_Map.World2DC(m_Edit_pShape->Get_Point(iPoint, iPart));

					dc_Map.dc.DrawLine(ptA.x, ptA.y, ptB.x, ptB.y);
				}
			}
		}

		CWKSP_Shapes::Edit_Shape_Draw(dc_Map);
	}
}

//---------------------------------------------------------
int CWKSP_Shapes_Line::Edit_Shape_HitTest(CSG_Point pos_Point, double max_Dist, int &pos_iPart, int &pos_iPoint)
{
	int			Result, iPart, iPoint;
	double		d;
	TSG_Point	A, B, Point, hit_Point;

	Result	= CWKSP_Shapes::Edit_Shape_HitTest(pos_Point, max_Dist, pos_iPart, pos_iPoint);

	if( Result == 0 && m_Edit_pShape )
	{
		for(iPart=0; iPart<m_Edit_pShape->Get_Part_Count(); iPart++)
		{
			B	= m_Edit_pShape->Get_Point(0, iPart);

			for(iPoint=1; iPoint<m_Edit_pShape->Get_Point_Count(iPart); iPoint++)
			{
				A	= m_Edit_pShape->Get_Point(iPoint, iPart);
				d	= SG_Get_Nearest_Point_On_Line(pos_Point, A, B, Point, true);
				B	= A;

				if( d >= 0.0 && (0.0 > max_Dist || d < max_Dist) )
				{
					Result		= 2;
					max_Dist	= d;
					pos_iPoint	= iPoint;
					pos_iPart	= iPart;
					hit_Point	= Point;
				}
			}
		}

		if( Result )
		{
			pos_Point	= hit_Point;
		}
	}

	return( Result );
}

//---------------------------------------------------------
void CWKSP_Shapes_Line::Edit_Snap_Point_ToLine(CSG_Point pos_Point, CSG_Point &snap_Point, double &snap_Dist, CSG_Shape *pShape)
{
	int			iPart, iPoint;
	double		d;
	TSG_Point	A, B, Point;

	for(iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
	{
		B	= pShape->Get_Point(0, iPart);

		for(iPoint=1; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
		{
			A	= pShape->Get_Point(iPoint, iPart);
			d	= SG_Get_Nearest_Point_On_Line(pos_Point, A, B, Point, true);
			B	= A;

			if( d >= 0.0 && d < snap_Dist )
			{
				snap_Dist	= d;
				snap_Point	= Point;
			}
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
