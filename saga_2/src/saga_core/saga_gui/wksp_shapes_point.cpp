
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
//                 WKSP_Shapes_Point.cpp                 //
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
#include <wx/filename.h>

#include "res_commands.h"
#include "res_images.h"

#include "helper.h"
#include "dc_helper.h"

#include "wksp_layer_classify.h"

#include "wksp_shapes_point.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Shapes_Point::CWKSP_Shapes_Point(CSG_Shapes *pShapes)
	: CWKSP_Shapes(pShapes)
{
	Create_Parameters();
}

//---------------------------------------------------------
CWKSP_Shapes_Point::~CWKSP_Shapes_Point(void)
{
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Shapes_Point::On_Create_Parameters(void)
{
	CWKSP_Shapes::On_Create_Parameters();

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

	m_Parameters.Add_Choice(
		m_Parameters("NODE_DISPLAY")	, "DISPLAY_SYMBOL_TYPE"		, LNG("[CAP] Symbol Type"),
		LNG(""),

		CSG_String::Format(wxT("%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|"),
			LNG("circle"),
			LNG("square"),
			LNG("rhombus"),
			LNG("triangle (up)"),
			LNG("triangle (down)"),
			LNG("circle with square"),
			LNG("circle with rhombus"),
			LNG("circle with triangle (up)"),
			LNG("circle with triangle (down)"),
			LNG("circle in square"),
			LNG("circle in rhombus"),
			LNG("circle in triangle (up)"),
			LNG("circle in triangle (down)"),
			LNG("image")
		), 0
	);

	m_Parameters.Add_FilePath(
		m_Parameters("NODE_DISPLAY")	, "DISPLAY_SYMBOL_IMAGE"	, LNG("[CAP] Symbol Image"),
		LNG(""),
		CSG_String::Format(
			wxT("%s|*.bmp;*.ico;*.gif;*.jpg;*.jif;*.jpeg;*.pcx;*.png;*.pnm;*.tif;*.tiff;*.xpm|")
			wxT("%s (*.bmp)|*.bmp|")
			wxT("%s (*.jpg)|*.jpg|")
			wxT("%s (*.png)|*.png|")
			wxT("%s (*.pcx)|*.pcx|")
			wxT("%s (*.xpm)|*.xpm|")
			wxT("%s (*.tif)|*.tif;*.tiff|")
			wxT("%s (*.gif)|*.gif|")
			wxT("%s|*.*"),
			LNG("Image Files"),
			LNG("Windows or OS/2 Bitmap"),
			LNG("JPEG - JFIF Compliant"),
			LNG("Portable Network Graphics"),
			LNG("Zsoft Paintbrush"),
			LNG("X11 Pixel Map"),
			LNG("Tagged Image File Format"),
			LNG("CompuServe Graphics Interchange"),
			LNG("All Files")
		)
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
		PARAMETER_TYPE_Double, 5, 0, true
	);

	m_Parameters.Add_Range(
		m_Parameters("NODE_SIZE")	, "SIZE_RANGE"		, LNG("[CAP] Size Range"),
		LNG(""),
		2, 10, 0, true
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Shapes_Point::On_DataObject_Changed(void)
{
	CWKSP_Shapes::On_DataObject_Changed();

	_AttributeList_Set(m_Parameters("SIZE_ATTRIB"), true);
}

//---------------------------------------------------------
void CWKSP_Shapes_Point::On_Parameters_Changed(void)
{
	CWKSP_Shapes::On_Parameters_Changed();

	//-----------------------------------------------------
	m_Symbol_Type	= m_Parameters("DISPLAY_SYMBOL_TYPE")->asInt();

	if( !wxFileName::FileExists(m_Parameters("DISPLAY_SYMBOL_IMAGE")->asString())
	||	!m_Symbol.LoadFile(m_Parameters("DISPLAY_SYMBOL_IMAGE")->asString()) )
	{
		m_Symbol	= IMG_Get_Image(ID_IMG_DEFAULT);
	}

	//-----------------------------------------------------
	m_Size_Type		= m_Parameters("SIZE_TYPE")->asInt();

	if(	(m_iSize	= m_Parameters("SIZE_ATTRIB")->asInt()) >= m_pShapes->Get_Table().Get_Field_Count()
	||	(m_dSize	= m_pShapes->Get_Table().Get_MaxValue(m_iSize) - (m_Size_Min = m_pShapes->Get_Table().Get_MinValue(m_iSize))) <= 0.0 )
	{
		m_iSize		= -1;
		m_Size		= m_Parameters("SIZE_DEFAULT")->asDouble();
	}
	else
	{
		m_Size		=  m_Parameters("SIZE_RANGE")->asRange()->Get_LoVal();
		m_dSize		= (m_Parameters("SIZE_RANGE")->asRange()->Get_HiVal() - m_Size) / m_dSize;
	}

	//-----------------------------------------------------
	Get_Style(m_Pen, m_Brush, m_bOutline);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CWKSP_Shapes_Point::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	CWKSP_Shapes::On_Parameter_Changed(pParameters, pParameter);

	//-----------------------------------------------------
	if(	!SG_STR_CMP(pParameter->Get_Identifier(), wxT("COLORS_FONT")) )
	{
		int		zField	= pParameters->Get_Parameter("COLORS_ATTRIB")->asInt();

		pParameters->Get_Parameter("METRIC_ZRANGE")->asRange()->Set_Range(
			m_pShapes->Get_Table().Get_MinValue(zField),
			m_pShapes->Get_Table().Get_MaxValue(zField)
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
bool CWKSP_Shapes_Point::Get_Style(wxPen &Pen, wxBrush &Brush, bool &bOutline, wxString *pName)
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
			pName->Printf(m_pShapes->Get_Table().Get_Field_Name(m_iColor));
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CWKSP_Shapes_Point::Get_Style_Size(int &min_Size, int &max_Size, double &min_Value, double &dValue, wxString *pName)
{
	if( m_iSize >= 0 )
	{
		min_Size	= (int)(m_Size);
		max_Size	= (int)(m_Size + (m_pShapes->Get_Table().Get_MaxValue(m_iSize) - m_Size_Min) * m_dSize);
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
inline void CWKSP_Shapes_Point::_Draw_Initialize(CWKSP_Map_DC &dc_Map)
{
	dc_Map.dc.SetBrush	(m_Brush);
	dc_Map.dc.SetPen	(m_Pen);
}

//---------------------------------------------------------
inline bool CWKSP_Shapes_Point::_Draw_Initialize(CWKSP_Map_DC &dc_Map, int &Size, CSG_Shape *pShape, bool bSelection)
{
	//-----------------------------------------------------
	double	dSize	= m_iSize < 0 ? m_Size : m_Size + (pShape->Get_Record()->asDouble(m_iSize) - m_Size_Min) * m_dSize;

	switch( m_Size_Type )
	{
	default:
	case 0:	dSize	*= dc_Map.m_Scale;		break;
	case 1:	dSize	*= dc_Map.m_World2DC;	break;
	}

	Size	= (int)(0.5 + dSize);

	//-----------------------------------------------------
	if( Size > 0 )
	{
		if( bSelection )
		{
			dc_Map.dc.SetBrush	(wxBrush(wxColour(255, 255, 0), wxSOLID));
			dc_Map.dc.SetPen	(wxPen	(wxColour(255,   0, 0), pShape == m_pShapes->Get_Selection(0) ? 2 : 1, wxSOLID));
		}
		else if( !bSelection && m_iColor >= 0 )
		{
			int		Color	= m_pClassify->Get_Class_Color_byValue(pShape->Get_Record()->asDouble(m_iColor));

			wxBrush	Brush(m_Brush);
			Brush.SetColour(SG_GET_R(Color), SG_GET_G(Color), SG_GET_B(Color));
			dc_Map.dc.SetBrush(Brush);

			if( !m_bOutline )
			{
				wxPen	Pen(m_Pen);
				Pen.SetColour(SG_GET_R(Color), SG_GET_G(Color), SG_GET_B(Color));
				dc_Map.dc.SetPen(Pen);
			}
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
void CWKSP_Shapes_Point::_Draw_Shape(CWKSP_Map_DC &dc_Map, CSG_Shape *pShape, bool bSelection)
{
	int		Size;

	//-----------------------------------------------------
	if( _Draw_Initialize(dc_Map, Size, pShape, bSelection) )
	{
		TSG_Point_Int	p(dc_Map.World2DC(pShape->Get_Point(0)));

		Draw_Symbol(dc_Map.dc, p.x, p.y, Size);

		//-------------------------------------------------
		if( bSelection )
		{
			_Draw_Initialize(dc_Map);
		}
	}
}

//---------------------------------------------------------
void CWKSP_Shapes_Point::_Draw_Label(CWKSP_Map_DC &dc_Map, CSG_Shape *pShape)
{
	TSG_Point_Int	p(dc_Map.World2DC(pShape->Get_Point(0)));

	Draw_Text(dc_Map.dc, TEXTALIGN_TOPCENTER, p.x, p.y, pShape->Get_Record()->asString(m_iLabel, -2));
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define DRAW_CIRCLE(d)		{	int		s	= (int)((d) * size);\
	dc.DrawCircle(x, y, s);\
}

#define DRAW_SQUARE			{	int		s	= (int)(0.7071067812 * size);\
	dc.DrawRectangle(x - s, y - s, 2 * s, 2 * s);\
}

#define DRAW_RHOMBUS		{	wxPoint	p[4];\
	p[0].y	= p[2].y	= y;\
	p[1].x	= p[3].x	= x;\
	p[0].x				= x - size;\
	p[2].x				= x + size;\
	p[1].y				= y - size;\
	p[3].y				= y + size;\
	dc.DrawPolygon(4, p);\
}

#define DRAW_TRIANGLE_UP	{	wxPoint	p[3];\
	p[0].y	= p[1].y	= y + (size / 2);\
	p[2].y				= y - (size - 1);\
	p[0].x				= x - (int)(0.8660254038 * size);\
	p[1].x				= x + (int)(0.8660254038 * size);\
	p[2].x				= x;\
	dc.DrawPolygon(3, p);\
}

#define DRAW_TRIANGLE_DOWN	{	wxPoint	p[3];\
	p[0].y	= p[1].y	= y - (size / 2);\
	p[2].y				= y + (size - 1);\
	p[0].x				= x - (int)(0.8660254038 * size);\
	p[1].x				= x + (int)(0.8660254038 * size);\
	p[2].x				= x;\
	dc.DrawPolygon(3, p);\
}

//---------------------------------------------------------
void CWKSP_Shapes_Point::Draw_Symbol(wxDC &dc, int x, int y, int size)
{
	switch( m_Symbol_Type )
	{
	default:
	case 0:		// circle
		DRAW_CIRCLE(1.0);
		break;

	case 1:		// square
		DRAW_SQUARE;
		break;

	case 2:		// rhombus
		DRAW_RHOMBUS;
		break;

	case 3:		// triangle up
		DRAW_TRIANGLE_UP;
		break;

	case 4:		// triangle down
		DRAW_TRIANGLE_DOWN;
		break;

	case 5:		// square in circle
		DRAW_CIRCLE(1.0);
		DRAW_SQUARE;
		break;

	case 6:		// rhombus in circle
		DRAW_CIRCLE(1.1);
		DRAW_RHOMBUS;
		break;

	case 7:		// triangle up in circle
		DRAW_CIRCLE(1.0);
		DRAW_TRIANGLE_UP;
		break;

	case 8:		// triangle down in circle
		DRAW_CIRCLE(1.0);
		DRAW_TRIANGLE_DOWN;
		break;

	case 9:		// circle in square
		DRAW_SQUARE;
		DRAW_CIRCLE(0.7);
		break;

	case 10:	// circle in rhombus
		DRAW_RHOMBUS;
		DRAW_CIRCLE(0.5 * sqrt(2.0));
		break;

	case 11:	// circle in triangle up
		DRAW_TRIANGLE_UP;
		DRAW_CIRCLE(0.5);
		break;

	case 12:	// circle in triangle down
		DRAW_TRIANGLE_DOWN;
		DRAW_CIRCLE(0.5);
		break;

	case 13:	// image
		{
			double	d	= (double)m_Symbol.GetWidth() / (double)m_Symbol.GetHeight();
			int		sx, sy;

			if( d > 1.0 )
			{
				sx	= size;
				sy	= (int)(0.5 + size / d);
			}
			else
			{
				sx	= (int)(0.5 + size * d);
				sy	= size;
			}

			if( sx > 0 && sy > 0 )
			{
				dc.DrawBitmap(wxBitmap(m_Symbol.Scale(2 * sx, 2 * sy)), x - sx, y - sy, true);
			}
		}
		break;
	}	
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxMenu * CWKSP_Shapes_Point::On_Edit_Get_Menu(void)
{
	wxMenu	*pMenu;

	pMenu	= new wxMenu;

	CMD_Menu_Add_Item(pMenu, true , ID_CMD_SHAPES_EDIT_SHAPE);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_SHAPES_EDIT_ADD_SHAPE);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_SHAPES_EDIT_DEL_SHAPE);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_SHAPES_EDIT_SEL_INVERT);

	return( pMenu );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
