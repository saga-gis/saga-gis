
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
	Create_Parameters();
}

//---------------------------------------------------------
CWKSP_Shapes_Line::~CWKSP_Shapes_Line(void)
{
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
		m_Parameters("NODE_DISPLAY"), "DISPLAY_POINTS"	, LNG("[CAP] Show Vertices"),
		LNG(""),
		PARAMETER_TYPE_Bool, false
	);


	//-----------------------------------------------------
	// Size...

	m_Parameters.Add_Node(
		NULL						, "NODE_SIZE"		, LNG("[CAP] Display: Size"),
		LNG("")
	);

	_AttributeList_Add(
		m_Parameters("NODE_SIZE")	, "SIZE_ATTRIB"		, LNG("[CAP] Attribute"),
		LNG("")
	);

	m_Parameters.Add_Choice(
		m_Parameters("NODE_SIZE")	, "SIZE_TYPE"		, LNG("[CAP] Size relates to..."),
		LNG(""),
		wxString::Format(wxT("%s|%s|"),
			LNG("[VAL] Screen"),
			LNG("[VAL] Map Units")
		), 0
	);

	m_Parameters.Add_Value(
		m_Parameters("NODE_SIZE")	, "SIZE_DEFAULT"	, LNG("[CAP] Default Size"),
		LNG(""),
		PARAMETER_TYPE_Int, 0, 0, true
	);

	m_Parameters.Add_Range(
		m_Parameters("NODE_SIZE")	, "SIZE_RANGE"		, LNG("[CAP] Size Range"),
		LNG(""),
		0, 10, 0, true
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

	_AttributeList_Set(m_Parameters("SIZE_ATTRIB"), true);
}

//---------------------------------------------------------
void CWKSP_Shapes_Line::On_Parameters_Changed(void)
{
	CWKSP_Shapes::On_Parameters_Changed();

	//-----------------------------------------------------
	m_Size_Type		= m_Parameters("SIZE_TYPE")->asInt();

	if(	(m_iSize	= m_Parameters("SIZE_ATTRIB")->asInt()) >= m_pShapes->Get_Table().Get_Field_Count()
	||	(m_dSize	= m_pShapes->Get_Table().Get_MaxValue(m_iSize) - (m_Size_Min = m_pShapes->Get_Table().Get_MinValue(m_iSize))) <= 0.0 )
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
	Get_Style(m_Pen);

	m_bPoints	= m_Parameters("DISPLAY_POINTS")->asBool();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CWKSP_Shapes_Line::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	CWKSP_Shapes::On_Parameter_Changed(pParameters, pParameter);

	return( 1 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Shapes_Line::Get_Style(wxPen &Pen, wxString *pName)
{
	Pen		= wxPen(m_Def_Color, (int)m_Size, wxSOLID);

	if( pName )
	{
		if(	m_iColor < 0 || m_pClassify->Get_Mode() == CLASSIFY_UNIQUE )
		{
			pName->Clear();
		}
		else
		{
			pName->Printf(m_pShapes->Get_Table().Get_Field_Name(m_iColor));
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CWKSP_Shapes_Line::Get_Style_Size(int &min_Size, int &max_Size, double &min_Value, double &dValue, wxString *pName)
{
	if( m_iSize >= 0 )
	{
		min_Size	= (int)(m_Size);
		max_Size	= (int)(m_Size + ((m_pShapes->Get_Table().Get_MaxValue(m_iSize) - m_Size_Min) * m_dSize));
		min_Value	= m_Size_Min;
		dValue		= m_dSize;

		if( pName )
		{
			pName->Printf(m_pShapes->Get_Table().Get_Field_Name(m_iSize));
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
void CWKSP_Shapes_Line::_Draw_Initialize(CWKSP_Map_DC &dc_Map)
{
	dc_Map.dc.SetPen(m_Pen);
}

//---------------------------------------------------------
void CWKSP_Shapes_Line::_Draw_Shape(CWKSP_Map_DC &dc_Map, CSG_Shape *pShape, bool bSelection)
{
	//-----------------------------------------------------
	if( bSelection )
	{
		dc_Map.dc.SetPen(wxPen(wxColour(255, 0, 0), pShape == m_pShapes->Get_Selection(0) ? 2 : 1, wxSOLID));
	}
	else if( m_iColor >= 0 || m_iSize >= 0 )
	{
		wxPen	Pen(m_Pen);

		if( m_iColor >= 0 )
		{
			int		Color	= m_pClassify->Get_Class_Color_byValue(pShape->Get_Record()->asDouble(m_iColor));
			Pen.SetColour(SG_GET_R(Color), SG_GET_G(Color), SG_GET_B(Color));
		}

		double	dSize	= m_iSize < 0 ? m_Size
						: m_Size + (pShape->Get_Record()->asDouble(m_iSize) - m_Size_Min) * m_dSize;

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
	int				iPart, iPoint;
	TSG_Point_Int	A, B;

	for(iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
	{
		if( pShape->Get_Point_Count(iPart) > 1 )
		{
			A		= dc_Map.World2DC(pShape->Get_Point(0, iPart));

			for(iPoint=1; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
			{
				B		= A;
				A		= dc_Map.World2DC(pShape->Get_Point(iPoint, iPart));

				dc_Map.dc.DrawLine(A.x, A.y, B.x, B.y);
			}
		}
	}

	if( m_bPoints )
	{
		dc_Map.dc.SetPen(*wxBLACK_PEN);
		dc_Map.dc.SetBrush(*wxWHITE_BRUSH);

		for(iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
		{
			for(iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
			{
				A		= dc_Map.World2DC(pShape->Get_Point(iPoint, iPart));
				dc_Map.dc.DrawCircle(A.x, A.y, 2);
			}
		}
	}

	//-----------------------------------------------------
	if( bSelection )
	{
		dc_Map.dc.SetPen(m_Pen);
	}
}

//---------------------------------------------------------
#define GET_ANGLE(a, b)	M_RAD_TO_DEG * (b.x != a.x ? M_PI_180 - atan2((double)(b.y - a.y), (double)(b.x - a.x)) : (b.y > a.y ? M_PI_270 : (b.y < a.y ? M_PI_090 : 0.0)))

//---------------------------------------------------------
void CWKSP_Shapes_Line::_Draw_Label(CWKSP_Map_DC &dc_Map, CSG_Shape *pShape)
{
	const int	m_Label_Freq	= 10,
				m_Label_Prec	= 2;

	bool			bLabel;
	int				iPart, iPoint;
	double			d;
	TSG_Point_Int	A, B;
	wxCoord			sx, sy;
	wxString		s(pShape->Get_Record()->asString(m_iLabel, m_Label_Prec));

	dc_Map.dc.GetTextExtent(s, &sx, &sy);

	//-----------------------------------------------------
	for(iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
	{
		if( dc_Map.m_World2DC * ((CSG_Shape_Line *)pShape)->Get_Length(iPart) > (2 * m_Label_Freq) * sx )
		{
			A		= dc_Map.World2DC(pShape->Get_Point(0, iPart));

			for(iPoint=1, d=0.0, bLabel=false; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
			{
				//-----------------------------------------
				if( !bLabel )
				{
					B		= A;
					A		= dc_Map.World2DC(pShape->Get_Point(iPoint, iPart));

					if( (d += SG_Get_Distance(A.x, A.y, B.x, B.y)) > m_Label_Freq * sx )
					{
						bLabel	= true;
						B		= A;
					}
				}

				//-----------------------------------------
				else
				{
					A		= dc_Map.World2DC(pShape->Get_Point(iPoint, iPart));

					if( SG_Get_Distance(A.x, A.y, B.x, B.y) > sx )
					{
						bLabel	= false;
						d		= 0.0;

						dc_Map.dc.DrawRotatedText(s, B.x, B.y, GET_ANGLE(A, B));

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
#define DRAW_MOVELINE(i)	{	ptWorld	= m_Edit_pShape->Get_Point(i, m_Edit_iPart);\
								dc.DrawLine(Point.x, Point.y,\
									(int)((ptWorld.x - rWorld.Get_XMin()) / ClientToWorld),\
									(int)((rWorld.Get_YMax() - ptWorld.y) / ClientToWorld));	}

//---------------------------------------------------------
void CWKSP_Shapes_Line::_Edit_Shape_Draw_Move(wxDC &dc, CSG_Rect rWorld, double ClientToWorld, wxPoint Point)
{
	int			nPoints;
	TSG_Point	ptWorld;

	if( m_Edit_pShape && m_Edit_iPart >= 0 )
	{
		nPoints	= m_Edit_pShape->Get_Point_Count(m_Edit_iPart);

		if( m_Edit_iPoint < 0 )
		{
			if( nPoints > 0 )
			{
				DRAW_MOVELINE(nPoints - 1);
			}
		}
		else
		{
			if( nPoints > 1 )
			{
				if( m_Edit_iPoint > 0 )
					DRAW_MOVELINE(m_Edit_iPoint - 1);

				if( m_Edit_iPoint < nPoints - 1 )
					DRAW_MOVELINE(m_Edit_iPoint + 1);
			}

			CWKSP_Shapes::_Edit_Shape_Draw_Move(dc, rWorld, ClientToWorld, Point);
		}
	}
}

//---------------------------------------------------------
void CWKSP_Shapes_Line::_Edit_Shape_Draw(CWKSP_Map_DC &dc_Map)
{
	int			iPart, iPoint;
	TSG_Point_Int	ptA, ptB;

	if( m_Edit_pShape )
	{
		dc_Map.dc.SetPen(*wxBLACK_PEN);

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

		CWKSP_Shapes::_Edit_Shape_Draw(dc_Map);
	}
}

//---------------------------------------------------------
int CWKSP_Shapes_Line::_Edit_Shape_HitTest(CSG_Point pos_Point, double max_Dist, int &pos_iPart, int &pos_iPoint)
{
	int			Result, iPart, iPoint;
	double		d;
	TSG_Point	A, B, Point, hit_Point;

	Result	= CWKSP_Shapes::_Edit_Shape_HitTest(pos_Point, max_Dist, pos_iPart, pos_iPoint);

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
void CWKSP_Shapes_Line::_Edit_Snap_Point_ToLine(CSG_Point pos_Point, CSG_Point &snap_Point, double &snap_Dist, CSG_Shape *pShape)
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
