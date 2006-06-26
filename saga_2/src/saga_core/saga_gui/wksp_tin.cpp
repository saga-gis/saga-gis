
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
//                    WKSP_TIN.cpp                       //
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
#include "res_commands.h"

#include "helper.h"

#include "wksp_map_control.h"

#include "wksp_layer_classify.h"

#include "wksp_tin.h"
#include "wksp_table.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_TIN::CWKSP_TIN(CTIN *pTIN)
	: CWKSP_Layer(pTIN)
{
	m_pTIN		= pTIN;
	m_pTable	= new CWKSP_Table(&m_pTIN->Get_Table(), this);

	m_Edit_Attributes.Destroy();
	m_Edit_Attributes.Add_Field(LNG("[CAP] Name") , TABLE_FIELDTYPE_String);
	m_Edit_Attributes.Add_Field(LNG("[CAP] Value"), TABLE_FIELDTYPE_String);

	Create_Parameters();
}

//---------------------------------------------------------
CWKSP_TIN::~CWKSP_TIN(void)
{
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxString CWKSP_TIN::Get_Name(void)
{
	return( wxString::Format("%02d. %s", 1 + Get_ID(), m_pTIN->Get_Name()) );
}

//---------------------------------------------------------
wxString CWKSP_TIN::Get_Description(void)
{
	wxString	s;

	//-----------------------------------------------------
	s.Append(wxString::Format("<b>%s</b><table border=\"0\">",
		LNG("[CAP] T.I.N.")
	));

	s.Append(wxString::Format("<tr><td>%s</td><td>%s</td></tr>",
		LNG("[CAP] Name")					, m_pTIN->Get_Name()
	));

	s.Append(wxString::Format("<tr><td>%s</td><td>%s</td></tr>",
		LNG("[CAP] File")					, m_pTIN->Get_File_Path()
	));

	s.Append(wxString::Format("<tr><td>%s</td><td>%d</td></tr>",
		LNG("[CAP] Points")					, m_pTIN->Get_Point_Count()
	));

	s.Append("</table>");

	//-----------------------------------------------------
	s.Append(wxString::Format("<hr><b>%s</b>", LNG("[CAP] Table Description")));
	s.Append(Get_TableInfo_asHTML(&m_pTIN->Get_Table()));

	//-----------------------------------------------------
	s.Append(wxString::Format("<hr><b>%s</b><font size=\"-1\">", LNG("[CAP] Data History")));
	s.Append(m_pTIN->Get_History().Get_HTML());
	s.Append(wxString::Format("</font"));

	//-----------------------------------------------------
	return( s );
}

//---------------------------------------------------------
wxMenu * CWKSP_TIN::Get_Menu(void)
{
	wxMenu	*pMenu;

	pMenu	= new wxMenu(m_pTIN->Get_Name());

	CMD_Menu_Add_Item(pMenu, false, ID_CMD_WKSP_ITEM_CLOSE);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_TIN_SHOW);

	return( pMenu );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_TIN::On_Command(int Cmd_ID)
{
//	switch( Cmd_ID )
	{
//	default:
		return( CWKSP_Layer::On_Command(Cmd_ID) );
	}

//	return( true );
}

//---------------------------------------------------------
bool CWKSP_TIN::On_Command_UI(wxUpdateUIEvent &event)
{
//	switch( event.GetId() )
	{
//	default:
		return( CWKSP_Layer::On_Command_UI(event) );
	}

//	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_TIN::On_Create_Parameters(void)
{
	//-----------------------------------------------------
	// General...

	m_Parameters.Add_Choice(
		m_Parameters("NODE_METRIC")		, "COLORS_ATTRIB"			, LNG("[CAP] Color"),
		"",
		""
	);

	//-----------------------------------------------------
	m_Parameters.Add_Value(
		m_Parameters("NODE_DISPLAY")	, "DISPLAY_POINTS"			, LNG("[CAP] Show Points"),
		"",
		PARAMETER_TYPE_Bool, false
	);

	//-----------------------------------------------------
	m_Parameters.Add_Value(
		m_Parameters("NODE_DISPLAY")	, "DISPLAY_EDGES"			, LNG("[CAP] Show Edges"),
		"",
		PARAMETER_TYPE_Bool, true
	);

	//-----------------------------------------------------
	m_Parameters.Add_Value(
		m_Parameters("NODE_DISPLAY")	, "DISPLAY_TRANSPARENCY"	, LNG("[CAP] Transparency [%]"),
		"",
		PARAMETER_TYPE_Double, 0.0, 0.0, true, 100.0, true
	);

}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_TIN::On_DataObject_Changed(void)
{
	int			i;
	wxString	sChoices;

	for(i=0; i<m_pTIN->Get_Table().Get_Field_Count(); i++)
	{
		sChoices.Append(wxString::Format("%s|", m_pTIN->Get_Table().Get_Field_Name(i)));
	}

	m_Parameters("COLORS_ATTRIB")->asChoice()->Set_Items(sChoices);
}

//---------------------------------------------------------
void CWKSP_TIN::On_Parameters_Changed(void)
{
	if( (m_Color_Field = m_Parameters("COLORS_ATTRIB")->asInt()) >= m_pTIN->Get_Table().Get_Field_Count() )
	{
		m_Color_Field	= -1;
	}

	long	DefColor	= m_Parameters("UNISYMBOL_COLOR")->asColor();
	m_Color_Pen		= wxColour(COLOR_GET_R(DefColor), COLOR_GET_G(DefColor), COLOR_GET_B(DefColor));
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CWKSP_TIN::On_Parameter_Changed(CParameters *pParameters, CParameter *pParameter)
{
	int		zField;

	if( !strcmp(pParameter->Get_Identifier(), "COLORS_ATTRIB") )
	{
		zField	= pParameter->asInt();

		pParameters->Get_Parameter("METRIC_ZRANGE")->asRange()->Set_Range(
			m_pTIN->Get_Table().Get_MinValue(zField),
			m_pTIN->Get_Table().Get_MaxValue(zField)
		);
	}

	return( 1 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxString CWKSP_TIN::Get_Value(CGEO_Point ptWorld, double Epsilon)
{
	return( "" );
}

//---------------------------------------------------------
double CWKSP_TIN::Get_Value_Range(void)
{
	if( m_Color_Field >= 0 )
	{
		return(
			  m_pTIN->Get_Table().Get_MaxValue(m_Color_Field)
			- m_pTIN->Get_Table().Get_MinValue(m_Color_Field)
		);
	}
	else
	{
		return( 0.0 );
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_TIN::asImage(CGrid *pImage)
{
	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_TIN::On_Edit_On_Mouse_Up(CGEO_Point Point, double ClientToWorld, int Key)
{
	CGEO_Rect	rWorld(m_Edit_Mouse_Down, Point);

	if( rWorld.Get_XRange() == 0.0 && rWorld.Get_YRange() == 0.0 )
	{
		rWorld.Inflate(2.0 * ClientToWorld, false);
	}

	return( true );
}

//---------------------------------------------------------
bool CWKSP_TIN::On_Edit_Set_Attributes(void)
{
	return( true );
}

//---------------------------------------------------------
TGEO_Rect CWKSP_TIN::On_Edit_Get_Extent(void)
{
	return( m_pTIN->Get_Extent() );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_TIN::On_Draw(CWKSP_Map_DC &dc_Map, bool bEdit)
{
	if( Get_Extent().Intersects(dc_Map.m_rWorld) != INTERSECTION_None )
	{
		if( m_Color_Field >= 0 )
		{
			_Draw_Triangles	(dc_Map);
		}

		if( m_Parameters("DISPLAY_EDGES")	->asBool() )
		{
			_Draw_Edges		(dc_Map);
		}

		if( m_Parameters("DISPLAY_POINTS")	->asBool() )
		{
			_Draw_Points	(dc_Map);
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_TIN::_Draw_Points(CWKSP_Map_DC &dc_Map)
{
	int			i;
	TAPI_iPoint	Point;

	for(i=0; i<m_pTIN->Get_Point_Count(); i++)
	{
		Point	= dc_Map.World2DC(m_pTIN->Get_Point(i)->Get_Point());

		dc_Map.dc.DrawCircle(Point.x, Point.y, 5);
	}
}

//---------------------------------------------------------
void CWKSP_TIN::_Draw_Edges(CWKSP_Map_DC &dc_Map)
{
	int			i;
	CTIN_Edge	*pEdge;
	TAPI_iPoint	Point[2];

	for(i=0; i<m_pTIN->Get_Edge_Count(); i++)
	{
		pEdge	= m_pTIN->Get_Edge(i);

		Point[0]	= dc_Map.World2DC(pEdge->Get_Point(0)->Get_Point());
		Point[1]	= dc_Map.World2DC(pEdge->Get_Point(1)->Get_Point());

		dc_Map.dc.DrawLine(Point[0].x, Point[0].y, Point[1].x, Point[1].y);
	}
}

//---------------------------------------------------------
void CWKSP_TIN::_Draw_Triangles(CWKSP_Map_DC &dc_Map)
{
	int				iTriangle, iPoint;
	TAPI_iPoint		Point;
	TPoint			p[3];
	CTIN_Triangle	*pTriangle;

	if( dc_Map.IMG_Draw_Begin(m_Parameters("DISPLAY_TRANSPARENCY")->asDouble() / 100.0) )
	{
		for(iTriangle=0; iTriangle<m_pTIN->Get_Triangle_Count(); iTriangle++)
		{
			pTriangle	= m_pTIN->Get_Triangle(iTriangle);

			if( dc_Map.m_rWorld.Intersects(pTriangle->Get_Extent()) != INTERSECTION_None )
			{
				for(iPoint=0; iPoint<3; iPoint++)
				{
					Point		= dc_Map.World2DC(pTriangle->Get_Point(iPoint)->Get_Point());
					p[iPoint].x	= Point.x;
					p[iPoint].y	= Point.y;
					p[iPoint].z	= pTriangle->Get_Point(iPoint)->Get_Record()->asDouble(m_Color_Field);
				}

				_Draw_Triangle(dc_Map, p);
			}
		}

		dc_Map.IMG_Draw_End();
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define SORT_POINTS_Y(a, b)	if( p[a].y < p[b].y ) {	pp = p[a]; p[a] = p[b]; p[b] = pp;	}
#define SORT_POINTS_X(a, b)	if( p[a].x < p[b].x ) {	pp = p[a]; p[a] = p[b]; p[b] = pp;	}

#define DRAW_PIXEL(x, y, z)	if( m_pClassify->Get_Class_Color_byValue(z, Color) )	{	dc_Map.IMG_Set_Pixel(x, y, Color);	}

//---------------------------------------------------------
void CWKSP_TIN::_Draw_Triangle(CWKSP_Map_DC &dc_Map, TPoint p[3])
{
	int		i, j, y, y_j, Color;
	double	x, x_a, dx, dx_a, dy, z, z_a, dz, dz_a;
	TPoint	pp;

	//-----------------------------------------------------
	SORT_POINTS_Y(1, 0);
	SORT_POINTS_Y(2, 0);
	SORT_POINTS_Y(2, 1);

	//-----------------------------------------------------
	if( p[2].y == p[0].y )
	{
		if( p[0].y >= 0 && p[0].y < dc_Map.m_rDC.GetHeight() )
		{
			SORT_POINTS_X(1, 0);
			SORT_POINTS_X(2, 0);
			SORT_POINTS_X(2, 1);

			//---------------------------------------------
			if( p[2].x == p[0].x )
			{
				if(	p[0].x >= 0 && p[0].x < dc_Map.m_rDC.GetWidth() )
				{
					DRAW_PIXEL(p[0].x, p[0].y, p[0].z > p[1].z
						? (p[0].z > p[2].z ? p[0].z : p[2].z)
						: (p[1].z > p[2].z ? p[1].z : p[2].z)
					);
				}
			}

			//---------------------------------------------
			else
			{
				_Draw_Triangle_Line(dc_Map, p[0].x, p[1].x, p[0].y, p[0].z, p[1].z);
				_Draw_Triangle_Line(dc_Map, p[1].x, p[2].x, p[0].y, p[1].z, p[2].z);
			}
		}
	}

	//-----------------------------------------------------
	else if( !((p[0].y < 0 && p[2].y < 0) || (p[0].y >= dc_Map.m_rDC.GetHeight() && p[2].y >= dc_Map.m_rDC.GetHeight())) )
	{
		dy		=  p[2].y - p[0].y;
		dx_a	= (p[2].x - p[0].x) / dy;
		dz_a	= (p[2].z - p[0].z) / dy;
		x_a		=  p[0].x;
		z_a		=  p[0].z;

		for(i=0, j=1; i<2; i++, j++)
		{
			if( (dy	=  p[j].y - p[i].y) > 0.0 )
			{
				dx		= (p[j].x - p[i].x) / dy;
				dz		= (p[j].z - p[i].z) / dy;
				x		=  p[i].x;
				z		=  p[i].z;

				if( (y = p[i].y) < 0 )
				{
					x		-= y * dx;
					z		-= y * dz;
					y		 = 0;
					x_a		 = p[0].x - p[0].y * dx_a;
					z_a		 = p[0].z - p[0].y * dz_a;
				}

				if( (y_j = p[j].y) > dc_Map.m_rDC.GetHeight() )
				{
					y_j		= dc_Map.m_rDC.GetHeight();
				}

				for( ; y<y_j; y++, x+=dx, z+=dz, x_a+=dx_a, z_a+=dz_a)
				{
					if( x < x_a )
					{
						_Draw_Triangle_Line(dc_Map, (int)x, (int)x_a, y, z, z_a);
					}
					else
					{
						_Draw_Triangle_Line(dc_Map, (int)x_a, (int)x, y, z_a, z);
					}
				}
			}
		}
	}
}

//---------------------------------------------------------
inline void CWKSP_TIN::_Draw_Triangle_Line(CWKSP_Map_DC &dc_Map, int xa, int xb, int y, double za, double zb)
{
	int		Color;
	double	dz;

	if( (dz = xb - xa) > 0.0 )
	{
		dz	= (zb - za) / dz;

		if( xa < 0 )
		{
			za	-= dz * xa;
			xa	 = 0;
		}

		if( xb >= dc_Map.m_rDC.GetWidth() )
		{
			xb	= dc_Map.m_rDC.GetWidth() - 1;
		}

		for(int x=xa; x<=xb; x++, za+=dz)
		{
			DRAW_PIXEL(x, y, za);
		}
	}
	else if( xa >= 0 && xa < dc_Map.m_rDC.GetWidth() )
	{
		DRAW_PIXEL(xa, y, za);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
