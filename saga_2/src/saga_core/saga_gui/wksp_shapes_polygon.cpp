
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
//                WKSP_Shapes_Polygon.cpp                //
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
#include "helper.h"
#include "dc_helper.h"

#include "wksp_layer_classify.h"

#include "wksp_shapes_polygon.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Shapes_Polygon::CWKSP_Shapes_Polygon(CSG_Shapes *pShapes)
	: CWKSP_Shapes(pShapes)
{
	Create_Parameters();
}

//---------------------------------------------------------
CWKSP_Shapes_Polygon::~CWKSP_Shapes_Polygon(void)
{
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Shapes_Polygon::On_Create_Parameters(void)
{
	CWKSP_Shapes::On_Create_Parameters();


	//-----------------------------------------------------
	// Display...

	_BrushList_Add(
		m_Parameters("NODE_DISPLAY")	, "DISPLAY_BRUSH"			, LNG("[CAP] Fill Style"),
		LNG("")
	);

	m_Parameters.Add_Value(
		m_Parameters("NODE_DISPLAY")	, "DISPLAY_OUTLINE"			, LNG("[CAP] Outline"),
		LNG(""),
		PARAMETER_TYPE_Bool, true
	);

	m_Parameters.Add_Value(
		m_Parameters("NODE_DISPLAY")	, "DISPLAY_OUTLINE_COLOR"	, LNG("[CAP] Outline Color"),
		LNG(""),
		PARAMETER_TYPE_Color, SG_GET_RGB(0, 0, 0)
	);

	m_Parameters.Add_Value(
		m_Parameters("NODE_DISPLAY")	, "DISPLAY_OUTLINE_SIZE"	, LNG("[CAP] Outline Size"),
		LNG(""),
		PARAMETER_TYPE_Int, 0, 0, true
	);

	m_Parameters.Add_Value(
		m_Parameters("NODE_DISPLAY")	, "DISPLAY_POINTS"			, LNG("[CAP] Show Vertices"),
		LNG(""),
		PARAMETER_TYPE_Bool, false
	);

	m_Parameters.Add_Value(
		m_Parameters("NODE_DISPLAY")	, "DISPLAY_CENTROID"		, LNG("[CAP] Show Centroid"),
		LNG(""),
		PARAMETER_TYPE_Bool, false
	);

	m_Parameters.Add_Value(
		m_Parameters("NODE_SELECTION")	, "EDIT_SEL_COLOR_FILL"		, LNG("[CAP] Fill Color"),
		LNG(""),
		PARAMETER_TYPE_Color, SG_GET_RGB(255, 255, 0)
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Shapes_Polygon::On_DataObject_Changed(void)
{
	CWKSP_Shapes::On_DataObject_Changed();
}

//---------------------------------------------------------
void CWKSP_Shapes_Polygon::On_Parameters_Changed(void)
{
	CWKSP_Shapes::On_Parameters_Changed();

	//-----------------------------------------------------
	Get_Style(m_Pen, m_Brush, m_bOutline);

	m_bPoints	= m_Parameters("DISPLAY_POINTS")	->asBool();
	m_bCentroid	= m_Parameters("DISPLAY_CENTROID")	->asBool();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CWKSP_Shapes_Polygon::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
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
bool CWKSP_Shapes_Polygon::Get_Style(wxPen &Pen, wxBrush &Brush, bool &bOutline, wxString *pName)
{
	bOutline	= m_Parameters("DISPLAY_OUTLINE")->asBool();
	Brush		= wxBrush(m_Def_Color, _BrushList_Get_Style(m_Parameters("DISPLAY_BRUSH")->asInt()));
	Pen			= wxPen(!bOutline ? m_Def_Color : Get_Color_asWX(m_Parameters("DISPLAY_OUTLINE_COLOR")->asColor()), m_Parameters("DISPLAY_OUTLINE_SIZE")->asInt(), wxSOLID);

	if( pName )
	{
		if(	m_iColor < 0 || m_pClassify->Get_Mode() == CLASSIFY_UNIQUE )
		{
			pName->Clear();
		}
		else
		{
			pName->Printf(m_pShapes->Get_Field_Name(m_iColor));
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Shapes_Polygon::_Draw_Initialize(CWKSP_Map_DC &dc_Map)
{
	dc_Map.dc.SetBrush(m_Brush);
	dc_Map.dc.SetPen(m_Pen);

	m_Sel_Color_Fill	= Get_Color_asWX(m_Parameters("EDIT_SEL_COLOR_FILL")->asInt());
}

//---------------------------------------------------------
void CWKSP_Shapes_Polygon::_Draw_Shape(CWKSP_Map_DC &dc_Map, CSG_Shape *pShape, bool bSelection)
{
	//-----------------------------------------------------
	if( bSelection )
	{
		dc_Map.dc.SetBrush(wxBrush(m_Sel_Color_Fill, wxSOLID));
		dc_Map.dc.SetPen(wxPen(m_Sel_Color, pShape == m_pShapes->Get_Selection(0) ? 2 : 1, wxSOLID));

		dc_Map.Draw_Polygon((CSG_Shape_Polygon *)pShape);

		dc_Map.dc.SetBrush(m_Brush);
		dc_Map.dc.SetPen(m_Pen);
	}
	else if( m_iColor >= 0 )
	{
		int		Color	= m_pClassify->Get_Class_Color_byValue(pShape->asDouble(m_iColor));

		m_Brush.SetColour(SG_GET_R(Color), SG_GET_G(Color), SG_GET_B(Color));
		dc_Map.dc.SetBrush(m_Brush);

		if( !m_bOutline )
		{
			m_Pen.SetColour(SG_GET_R(Color), SG_GET_G(Color), SG_GET_B(Color));
			dc_Map.dc.SetPen(m_Pen);
		}

		dc_Map.Draw_Polygon((CSG_Shape_Polygon *)pShape);
	}

	if( m_bCentroid )
	{
		TSG_Point	Point	= ((CSG_Shape_Polygon *)pShape)->Get_Centroid();

		dc_Map.dc.DrawCircle((int)dc_Map.xWorld2DC(Point.x), (int)dc_Map.yWorld2DC(Point.y), 2);
	}

	if( m_bPoints )
	{
		_Draw_Polygon_Points(dc_Map, (CSG_Shape_Polygon *)pShape);
	}
}

//---------------------------------------------------------
void CWKSP_Shapes_Polygon::_Draw_Label(CWKSP_Map_DC &dc_Map, CSG_Shape *pShape)
{
	int			iPart;
	TSG_Point	p;

	if( 0 )
	{
//		p	= dc_Map.World2DC(((CSG_Shape_Polygon *)pShape)->Get_Centroid());
		p	= ((CSG_Shape_Polygon *)pShape)->Get_Centroid();
		p.x	= dc_Map.xWorld2DC(p.x);
		p.y	= dc_Map.yWorld2DC(p.y);

		Draw_Text(dc_Map.dc, TEXTALIGN_CENTER, (int)p.x, (int)p.y, pShape->asString(m_iLabel, -2));
	}
	else
	{
		for(iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
		{
			if( !((CSG_Shape_Polygon *)pShape)->is_Lake(iPart) )
			{
				p	= ((CSG_Shape_Polygon *)pShape)->Get_Centroid(iPart);
				p.x	= dc_Map.xWorld2DC(p.x);
				p.y	= dc_Map.yWorld2DC(p.y);

				Draw_Text(dc_Map.dc, TEXTALIGN_CENTER, (int)p.x, (int)p.y, pShape->asString(m_iLabel, -2));
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
void CWKSP_Shapes_Polygon::_Edit_Shape_Draw_Move(wxDC &dc, CSG_Rect rWorld, double ClientToWorld, wxPoint Point)
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
				DRAW_MOVELINE(0);

				if( nPoints > 1 )
					DRAW_MOVELINE(nPoints - 1);
			}
		}
		else
		{
			if( nPoints > 1 )
			{
				DRAW_MOVELINE(m_Edit_iPoint > 0 ? m_Edit_iPoint - 1 : nPoints - 1);

				if( nPoints > 2 )
					DRAW_MOVELINE(m_Edit_iPoint >= nPoints - 1 ? 0 : m_Edit_iPoint + 1);
			}

			CWKSP_Shapes::_Edit_Shape_Draw_Move(dc, rWorld, ClientToWorld, Point);
		}
	}
}

//---------------------------------------------------------
void CWKSP_Shapes_Polygon::_Edit_Shape_Draw(CWKSP_Map_DC &dc_Map)
{
	int			iPart, iPoint;
	TSG_Point_Int	ptA, ptB;

	if( m_Edit_pShape )
	{
		dc_Map.dc.SetPen(wxPen(m_Edit_Color));

		for(iPart=0; iPart<m_Edit_pShape->Get_Part_Count(); iPart++)
		{
			if( m_Edit_pShape->Get_Point_Count(iPart) > 2 )
			{
				ptA		= dc_Map.World2DC(m_Edit_pShape->Get_Point(m_Edit_pShape->Get_Point_Count(iPart) - 1, iPart));

				for(iPoint=0; iPoint<m_Edit_pShape->Get_Point_Count(iPart); iPoint++)
				{
					ptB		= ptA;
					ptA		= dc_Map.World2DC(m_Edit_pShape->Get_Point(iPoint, iPart));

					dc_Map.dc.DrawLine(ptA.x, ptA.y, ptB.x, ptB.y);
				}
			}
			else if( m_Edit_pShape->Get_Point_Count(iPart) == 2 )
			{
				ptA		= dc_Map.World2DC(m_Edit_pShape->Get_Point(0, iPart));
				ptB		= dc_Map.World2DC(m_Edit_pShape->Get_Point(1, iPart));

				dc_Map.dc.DrawLine(ptA.x, ptA.y, ptB.x, ptB.y);
			}
		}

		CWKSP_Shapes::_Edit_Shape_Draw(dc_Map);
	}
}

//---------------------------------------------------------
int CWKSP_Shapes_Polygon::_Edit_Shape_HitTest(CSG_Point pos_Point, double max_Dist, int &pos_iPart, int &pos_iPoint)
{
	int			Result, iPart, iPoint;
	double		d;
	TSG_Point	A, B, Point, hit_Point;

	Result	= CWKSP_Shapes::_Edit_Shape_HitTest(pos_Point, max_Dist, pos_iPart, pos_iPoint);

	if( Result == 0 && m_Edit_pShape )
	{
		for(iPart=0; iPart<m_Edit_pShape->Get_Part_Count(); iPart++)
		{
			B	= m_Edit_pShape->Get_Point(m_Edit_pShape->Get_Point_Count(iPart) - 1, iPart);

			for(iPoint=0; iPoint<m_Edit_pShape->Get_Point_Count(iPart); iPoint++)
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
void CWKSP_Shapes_Polygon::_Edit_Snap_Point_ToLine(CSG_Point pos_Point, CSG_Point &snap_Point, double &snap_Dist, CSG_Shape *pShape)
{
	int			iPart, iPoint;
	double		d;
	TSG_Point	A, B, Point;

	for(iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
	{
		B	= pShape->Get_Point(pShape->Get_Point_Count(iPart) - 1, iPart);

		for(iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
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
void CWKSP_Shapes_Polygon::_Draw_Polygon_Points(CWKSP_Map_DC &dc_Map, CSG_Shape_Polygon *pPolygon)
{
	int			iPart, iPoint;
	TSG_Point	Point;

	dc_Map.dc.SetPen(*wxBLACK_PEN);
	dc_Map.dc.SetBrush(*wxWHITE_BRUSH);

	for(iPart=0; iPart<pPolygon->Get_Part_Count(); iPart++)
	{
		if( pPolygon->Get_Point_Count(iPart) > 2 )
		{
			for(iPoint=0; iPoint<pPolygon->Get_Point_Count(iPart); iPoint++)
			{
				Point	= pPolygon->Get_Point(iPoint, iPart);

				dc_Map.dc.DrawCircle((int)dc_Map.xWorld2DC(Point.x), (int)dc_Map.yWorld2DC(Point.y), 2);
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
