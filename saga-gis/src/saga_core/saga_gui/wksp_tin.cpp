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

#include "wksp_data_manager.h"

#include "wksp_tin.h"
#include "wksp_table.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_TIN::CWKSP_TIN(CSG_TIN *pTIN)
	: CWKSP_Layer(pTIN)
{
	m_pTable	= new CWKSP_Table(pTIN);

	m_Edit_Attributes.Destroy();
	m_Edit_Attributes.Add_Field(_TL("Name" ), SG_DATATYPE_String);
	m_Edit_Attributes.Add_Field(_TL("Value"), SG_DATATYPE_String);

	//-----------------------------------------------------
	On_Create_Parameters();

	DataObject_Changed();
}

//---------------------------------------------------------
CWKSP_TIN::~CWKSP_TIN(void)
{
	delete(m_pTable);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxString CWKSP_TIN::Get_Description(void)
{
	wxString	s;

	//-----------------------------------------------------
	s	+= wxString::Format("<b>%s</b><table border=\"0\">", _TL("TIN"));

	s	+= "<table border=\"0\">";

	DESC_ADD_STR(_TL("Name"            ), m_pObject->Get_Name());
	DESC_ADD_STR(_TL("Description"     ), m_pObject->Get_Description());
	DESC_ADD_STR(_TL("File"            ), SG_File_Exists(m_pObject->Get_File_Name()) ? m_pObject->Get_File_Name() : _TL("memory"));
	DESC_ADD_STR(_TL("Modified"        ), m_pObject->is_Modified() ? _TL("yes") : _TL("no"));
	DESC_ADD_STR(_TL("Projection"      ), m_pObject->Get_Projection().Get_Description().c_str());
	DESC_ADD_FLT(_TL("West"            ), asTIN()->Get_Extent().Get_XMin());
	DESC_ADD_FLT(_TL("East"            ), asTIN()->Get_Extent().Get_XMax());
	DESC_ADD_FLT(_TL("West-East"       ), asTIN()->Get_Extent().Get_XRange());
	DESC_ADD_FLT(_TL("South"           ), asTIN()->Get_Extent().Get_YMin());
	DESC_ADD_FLT(_TL("North"           ), asTIN()->Get_Extent().Get_YMax());
	DESC_ADD_FLT(_TL("South-North"     ), asTIN()->Get_Extent().Get_YRange());
	DESC_ADD_INT(_TL("Number of Points"), asTIN()->Get_Node_Count());

	s	+= "</table>";

	s	+= Get_TableInfo_asHTML(asTIN());

	//-----------------------------------------------------
//	s	+= wxString::Format(wxT("<hr><b>%s</b><font size=\"-1\">"), _TL("Data History"));
//	s	+= asTIN()->Get_History().Get_HTML();
//	s	+= wxString::Format(wxT("</font"));

	//-----------------------------------------------------
	return( s );
}

//---------------------------------------------------------
wxMenu * CWKSP_TIN::Get_Menu(void)
{
	wxMenu	*pMenu	= new wxMenu(m_pObject->Get_Name());

	CMD_Menu_Add_Item(pMenu, false, ID_CMD_WKSP_ITEM_CLOSE);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_TIN_SHOW);

	pMenu->AppendSeparator();
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_DATA_PROJECTION);

	if( m_pObject->Get_MetaData().Get_Children_Count() > 0 )
		CMD_Menu_Add_Item(pMenu, false, ID_CMD_DATA_METADATA);

	pMenu->AppendSeparator();
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_WKSP_ITEM_SETTINGS_COPY);

	pMenu->AppendSeparator();
	wxMenu	*pTable	= new wxMenu(_TL("Table"));
	CMD_Menu_Add_Item(pTable,  true, ID_CMD_TABLE_SHOW);
	CMD_Menu_Add_Item(pTable,  true, ID_CMD_TABLE_DIAGRAM);
	CMD_Menu_Add_Item(pTable, false, ID_CMD_TABLE_SCATTERPLOT);
	pMenu->Append(ID_CMD_WKSP_FIRST, _TL("Attributes"), pTable);

	return( pMenu );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_TIN::On_Command(int Cmd_ID)
{
	switch( Cmd_ID )
	{
	default:
		return( CWKSP_Layer::On_Command(Cmd_ID) );

	case ID_CMD_TABLE_SHOW:
		m_pTable->Toggle_View();
		break;

	case ID_CMD_TABLE_DIAGRAM:
		m_pTable->Toggle_Diagram();
		break;

	case ID_CMD_TABLE_SCATTERPLOT:
		Add_ScatterPlot();
		break;
	}

	return( true );
}

//---------------------------------------------------------
bool CWKSP_TIN::On_Command_UI(wxUpdateUIEvent &event)
{
	switch( event.GetId() )
	{
	default:
		return( CWKSP_Layer::On_Command_UI(event) );

	case ID_CMD_TABLE_SHOW:
		event.Check(m_pTable->Get_View() != NULL);
		break;

	case ID_CMD_TABLE_DIAGRAM:
		event.Check(m_pTable->Get_Diagram() != NULL);
		break;
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_TIN::On_Create_Parameters(void)
{
	CWKSP_Layer::On_Create_Parameters();

	//-----------------------------------------------------
	m_Parameters.Add_Bool("NODE_DISPLAY", "DISPLAY_POINTS"  , _TL("Show Nodes"), _TL(""), false);
	m_Parameters.Add_Bool("NODE_DISPLAY", "DISPLAY_EDGES"   , _TL("Show Edges"), _TL(""),  true);
	m_Parameters.Add_Bool("NODE_DISPLAY", "DISPLAY_TRIANGES", _TL("Show Faces"), _TL(""),  true);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_TIN::On_DataObject_Changed(void)
{
	CSG_String	Choices;

	for(int i=0; i<asTIN()->Get_Field_Count(); i++)
	{
		Choices	+= CSG_String(asTIN()->Get_Field_Name(i)) + '|';
	}

	m_Parameters("METRIC_ATTRIB")->asChoice()->Set_Items(Choices);

	//-----------------------------------------------------
	CWKSP_Layer::On_DataObject_Changed();
}

//---------------------------------------------------------
void CWKSP_TIN::On_Parameters_Changed(void)
{
	CWKSP_Layer::On_Parameters_Changed();

	if( (m_fValue = m_Parameters("METRIC_ATTRIB")->asInt()) >= asTIN()->Get_Field_Count() )
	{
		m_fValue	= -1;
	}

	long	DefColor	= m_Parameters("UNISYMBOL_COLOR")->asColor();
	m_Color_Pen			= wxColour(SG_GET_R(DefColor), SG_GET_G(DefColor), SG_GET_B(DefColor));
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CWKSP_TIN::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter, int Flags)
{
	if( Flags & PARAMETER_CHECK_VALUES )
	{
		if(	pParameter->Cmp_Identifier("METRIC_ATTRIB") )
		{
			pParameters->Get_Parameter("METRIC_ZRANGE")->asRange()->Set_Range(
				asTIN()->Get_Minimum(pParameter->asInt()),
				asTIN()->Get_Maximum(pParameter->asInt())
			);
		}
	}

	return( CWKSP_Layer::On_Parameter_Changed(pParameters, pParameter, Flags) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxString CWKSP_TIN::Get_Name_Attribute(void)
{
	return(	m_fValue < 0 || m_pClassify->Get_Mode() == CLASSIFY_UNIQUE ? SG_T("") : asTIN()->Get_Field_Name(m_fValue) );
}

//---------------------------------------------------------
wxString CWKSP_TIN::Get_Value(CSG_Point ptWorld, double Epsilon)
{
	return( _TL("") );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double CWKSP_TIN::Get_Value_Minimum(void)	{	return( m_fValue < 0 ? 0.0 : asTIN()->Get_Minimum(m_fValue) );	}
double CWKSP_TIN::Get_Value_Maximum(void)	{	return( m_fValue < 0 ? 0.0 : asTIN()->Get_Maximum(m_fValue) );	}
double CWKSP_TIN::Get_Value_Range  (void)	{	return( m_fValue < 0 ? 0.0 : asTIN()->Get_Range  (m_fValue) );	}
double CWKSP_TIN::Get_Value_Mean   (void)	{	return( m_fValue < 0 ? 0.0 : asTIN()->Get_Mean   (m_fValue) );	}
double CWKSP_TIN::Get_Value_StdDev (void)	{	return( m_fValue < 0 ? 0.0 : asTIN()->Get_StdDev (m_fValue) );	}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_TIN::asImage(CSG_Grid *pImage)
{
	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
TSG_Rect CWKSP_TIN::Edit_Get_Extent(void)
{
	return( asTIN()->Get_Extent() );
}

//---------------------------------------------------------
bool CWKSP_TIN::Edit_On_Mouse_Up(CSG_Point Point, double ClientToWorld, int Key)
{
	CSG_Rect	rWorld(m_Edit_Mouse_Down, Point);

	if( rWorld.Get_XRange() == 0.0 && rWorld.Get_YRange() == 0.0 )
	{
		rWorld.Inflate(2.0 * ClientToWorld, false);
	}

	return( true );
}

//---------------------------------------------------------
bool CWKSP_TIN::Edit_Set_Attributes(void)
{
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_TIN::On_Draw(CWKSP_Map_DC &dc_Map, int Flags)
{
	if( Get_Extent().Intersects(dc_Map.m_rWorld) != INTERSECTION_None )
	{
		if( m_fValue >= 0 )
		{
			_Draw_Triangles(dc_Map);
		}

		if( m_Parameters("DISPLAY_EDGES" )->asBool() )
		{
			_Draw_Edges    (dc_Map);
		}

		if( m_Parameters("DISPLAY_POINTS")->asBool() )
		{
			_Draw_Points   (dc_Map);
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_TIN::_Draw_Points(CWKSP_Map_DC &dc_Map)
{
	for(int i=0; i<asTIN()->Get_Node_Count(); i++)
	{
		TSG_Point_Int	Point	= dc_Map.World2DC(asTIN()->Get_Node(i)->Get_Point());

		dc_Map.dc.DrawCircle(Point.x, Point.y, 5);
	}
}

//---------------------------------------------------------
void CWKSP_TIN::_Draw_Edges(CWKSP_Map_DC &dc_Map)
{
	for(int i=0; i<asTIN()->Get_Edge_Count(); i++)
	{
		TSG_Point_Int	Point[2];
		CSG_TIN_Edge	*pEdge	= asTIN()->Get_Edge(i);

		Point[0]	= dc_Map.World2DC(pEdge->Get_Node(0)->Get_Point());
		Point[1]	= dc_Map.World2DC(pEdge->Get_Node(1)->Get_Point());

		dc_Map.dc.DrawLine(Point[0].x, Point[0].y, Point[1].x, Point[1].y);
	}
}

//---------------------------------------------------------
void CWKSP_TIN::_Draw_Triangles(CWKSP_Map_DC &dc_Map)
{
	if(	m_Parameters("DISPLAY_TRIANGES")->asBool() && dc_Map.IMG_Draw_Begin(m_Parameters("DISPLAY_TRANSPARENCY")->asDouble() / 100.0) )
	{
		for(int iTriangle=0; iTriangle<asTIN()->Get_Triangle_Count(); iTriangle++)
		{
			CSG_TIN_Triangle	*pTriangle	= asTIN()->Get_Triangle(iTriangle);

			if( dc_Map.m_rWorld.Intersects(pTriangle->Get_Extent()) != INTERSECTION_None )
			{
				TPoint	p[3];

				for(int iNode=0; iNode<3; iNode++)
				{
					CSG_TIN_Node	*pNode	= pTriangle->Get_Node(iNode);
					TSG_Point_Int	Point	= dc_Map.World2DC(pNode->Get_Point());

					p[iNode].x	= Point.x;
					p[iNode].y	= Point.y;
					p[iNode].z	= pNode->asDouble(m_fValue);
				}

				_Draw_Triangle(dc_Map, p);
			}
		}

		dc_Map.IMG_Draw_End();
	}
}


///////////////////////////////////////////////////////////
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
