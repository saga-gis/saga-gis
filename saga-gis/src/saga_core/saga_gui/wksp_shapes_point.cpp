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
	Initialise();
}

//---------------------------------------------------------
CWKSP_Shapes_Point::~CWKSP_Shapes_Point(void)
{}


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
		m_Parameters("NODE_DISPLAY")	, "DISPLAY_BRUSH"			, _TL("[CAP] Fill Style"),
		_TL("")
	);

	m_Parameters.Add_Value(
		m_Parameters("NODE_DISPLAY")	, "OUTLINE"					, _TL("[CAP] Outline"),
		_TL(""),
		PARAMETER_TYPE_Bool, true
	);

	m_Parameters.Add_Value(
		m_Parameters("OUTLINE")			, "OUTLINE_COLOR"			, _TL("[CAP] Color"),
		_TL(""),
		PARAMETER_TYPE_Color, SG_GET_RGB(0, 0, 0)
	);

	m_Parameters.Add_Value(
		m_Parameters("OUTLINE")			, "OUTLINE_SIZE"			, _TL("[CAP] Size"),
		_TL(""),
		PARAMETER_TYPE_Int, 0, 0, true
	);

	m_Parameters.Add_Choice(
		m_Parameters("NODE_DISPLAY")	, "DISPLAY_SYMBOL_TYPE"		, _TL("[CAP] Symbol Type"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|"),
			_TL("circle"),
			_TL("square"),
			_TL("rhombus"),
			_TL("triangle (up)"),
			_TL("triangle (down)"),
			_TL("circle with square"),
			_TL("circle with rhombus"),
			_TL("circle with triangle (up)"),
			_TL("circle with triangle (down)"),
			_TL("circle in square"),
			_TL("circle in rhombus"),
			_TL("circle in triangle (up)"),
			_TL("circle in triangle (down)"),
			_TL("image")
		), 0
	);

	m_Parameters.Add_FilePath(
		m_Parameters("DISPLAY_SYMBOL_TYPE")	, "DISPLAY_SYMBOL_IMAGE"	, _TL("[CAP] Symbol Image"),
		_TL(""),
		CSG_String::Format(
			SG_T("%s|*.bmp;*.ico;*.gif;*.jpg;*.jif;*.jpeg;*.pcx;*.png;*.pnm;*.tif;*.tiff;*.xpm|")
			SG_T("%s (*.bmp)|*.bmp|")
			SG_T("%s (*.jpg)|*.jpg;*.jif;*.jpeg|")
			SG_T("%s (*.png)|*.png|")
			SG_T("%s (*.pcx)|*.pcx|")
			SG_T("%s (*.xpm)|*.xpm|")
			SG_T("%s (*.tif)|*.tif;*.tiff|")
			SG_T("%s (*.gif)|*.gif|")
			SG_T("%s|*.*"),
			_TL("Image Files"),
			_TL("Windows or OS/2 Bitmap"),
			_TL("JPEG - JFIF Compliant"),
			_TL("Portable Network Graphics"),
			_TL("Zsoft Paintbrush"),
			_TL("X11 Pixel Map"),
			_TL("Tagged Image File Format"),
			_TL("CompuServe Graphics Interchange"),
			_TL("All Files")
		)
	);


	//-----------------------------------------------------
	// Label...

	_AttributeList_Add(
		m_Parameters("LABEL_ATTRIB")	, "LABEL_ANGLE_ATTRIB"	, _TL("[CAP] Rotation by Attribute"),
		_TL("")
	);

	m_Parameters.Add_Value(
		m_Parameters("LABEL_ANGLE_ATTRIB"), "LABEL_ANGLE"		, _TL("[CAP] Default Rotation"),
		_TL("rotation clockwise in degree"),
		PARAMETER_TYPE_Double, 0.0, -360.0, true, 360.0, true
	);

	m_Parameters.Add_Choice(
		m_Parameters("LABEL_ATTRIB")	, "LABEL_ALIGN_X"		, _TL("[CAP] Horizontal Align"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|"),
			_TL("left"),
			_TL("center"),
			_TL("right")
		), 1
	);

	m_Parameters.Add_Choice(
		m_Parameters("LABEL_ATTRIB")	, "LABEL_ALIGN_Y"		, _TL("[CAP] Vertical Align"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|"),
			_TL("top"),
			_TL("center"),
			_TL("bottom")
		), 0
	);


	//-----------------------------------------------------
	// Size...

	m_Parameters.Add_Choice(
		m_Parameters("NODE_SIZE")		, "SIZE_TYPE"		, _TL("[CAP] Size relates to..."),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("[VAL] Screen"),
			_TL("[VAL] Map Units")
		), 0
	);

	_AttributeList_Add(
		m_Parameters("NODE_SIZE")		, "SIZE_ATTRIB"		, _TL("[CAP] Attribute"),
		_TL("")
	);

	m_Parameters.Add_Choice(
		m_Parameters("SIZE_ATTRIB")		, "SIZE_SCALE"		, _TL("[CAP] Attribute Values"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("[VAL] no scaling"),
			_TL("[VAL] scale to size range")
		), 1
	);

	m_Parameters.Add_Range(
		m_Parameters("SIZE_ATTRIB")		, "SIZE_RANGE"		, _TL("[CAP] Size Range"),
		_TL(""),
		2, 10, 0, true
	);

	m_Parameters.Add_Value(
		m_Parameters("SIZE_ATTRIB")		, "SIZE_DEFAULT"	, _TL("[CAP] Default Size"),
		_TL(""),
		PARAMETER_TYPE_Double, 5, 0, true
	);


	//-----------------------------------------------------
	// Edit...

	m_Parameters.Add_Value(
		m_Parameters("NODE_SELECTION")	, "SEL_COLOR_FILL"	, _TL("[CAP] Fill Color"),
		_TL(""),
		PARAMETER_TYPE_Color, SG_GET_RGB(255, 255, 0)
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

	_AttributeList_Set(m_Parameters("SIZE_ATTRIB")			, true);
	_AttributeList_Set(m_Parameters("LABEL_ANGLE_ATTRIB")	, true);
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
	m_Size_Type		= m_Parameters("SIZE_TYPE") ->asInt();
	m_Size_Scale	= m_Parameters("SIZE_SCALE")->asInt();

	if(	(m_iSize	= m_Parameters("SIZE_ATTRIB")->asInt()) >= m_pShapes->Get_Field_Count()
	||	(m_dSize	= m_pShapes->Get_Maximum(m_iSize) - (m_Size_Min = m_pShapes->Get_Minimum(m_iSize))) <= 0.0 )
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
	m_Label_Angle	= m_Parameters("LABEL_ANGLE")->asDouble();

	if( (m_iLabel_Angle	= m_Parameters("LABEL_ANGLE_ATTRIB")->asInt()) >= m_pShapes->Get_Field_Count() )
	{
		m_iLabel_Angle	= -1;
	}

	switch( m_Parameters("LABEL_ALIGN_X")->asInt() )
	{
	case 0:	m_Label_Align	 = TEXTALIGN_LEFT;		break;
	case 1:	m_Label_Align	 = TEXTALIGN_XCENTER;	break;
	case 2:	m_Label_Align	 = TEXTALIGN_RIGHT;		break;
	}

	switch( m_Parameters("LABEL_ALIGN_Y")->asInt() )
	{
	case 0:	m_Label_Align	|= TEXTALIGN_TOP;		break;
	case 1:	m_Label_Align	|= TEXTALIGN_YCENTER;	break;
	case 2:	m_Label_Align	|= TEXTALIGN_BOTTOM;	break;
	}

	//-----------------------------------------------------
	m_bOutline	= m_Parameters("OUTLINE")->asBool();
	m_Pen		= wxPen(!m_bOutline ? m_Def_Color : Get_Color_asWX(m_Parameters("OUTLINE_COLOR")->asColor()), m_Parameters("OUTLINE_SIZE")->asInt(), wxSOLID);
	m_Brush		= wxBrush(m_Def_Color, _BrushList_Get_Style(m_Parameters("DISPLAY_BRUSH")->asInt()));
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CWKSP_Shapes_Point::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter, int Flags)
{
	//-----------------------------------------------------
	if( Flags & PARAMETER_CHECK_VALUES )
	{
		if(	!SG_STR_CMP(pParameter->Get_Identifier(), wxT("COLORS_FONT")) )
		{
			int		zField	= pParameters->Get_Parameter("METRIC_ATTRIB")->asInt();

			pParameters->Get_Parameter("METRIC_ZRANGE")->asRange()->Set_Range(
				m_pShapes->Get_Minimum(zField),
				m_pShapes->Get_Maximum(zField)
			);
		}
	}

	//-----------------------------------------------------
	if( Flags & PARAMETER_CHECK_ENABLE )
	{
		if(	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("DISPLAY_SYMBOL_TYPE")) )
		{
			pParameters->Get_Parameter("DISPLAY_SYMBOL_IMAGE")->Set_Enabled(pParameter->asInt() == pParameter->asChoice()->Get_Count() - 1);
		}

		if(	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("LABEL_ATTRIB")) )
		{
			bool	Value	= pParameter->asInt() < m_pShapes->Get_Field_Count();

			pParameters->Get_Parameter("LABEL_ANGLE_ATTRIB")->Set_Enabled(Value);
			pParameters->Get_Parameter("LABEL_ANGLE"       )->Set_Enabled(Value);
			pParameters->Get_Parameter("LABEL_ALIGN_X"     )->Set_Enabled(Value);
			pParameters->Get_Parameter("LABEL_ALIGN_Y"     )->Set_Enabled(Value);
		}

		if(	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("SIZE_ATTRIB")) )
		{
			bool	Value	= pParameter->asInt() < m_pShapes->Get_Field_Count();

			pParameters->Get_Parameter("SIZE_SCALE"  )->Set_Enabled(Value == true);
			pParameters->Get_Parameter("SIZE_RANGE"  )->Set_Enabled(Value == true);
			pParameters->Get_Parameter("SIZE_DEFAULT")->Set_Enabled(Value == false);
		}

		if(	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("LABEL_ANGLE_ATTRIB")) )
		{
			pParameters->Get_Parameter("LABEL_ANGLE")->Set_Enabled(pParameter->asInt() >= m_pShapes->Get_Field_Count());
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
wxString CWKSP_Shapes_Point::Get_Name_Attribute(void)
{
	return(	m_iColor < 0 || m_pClassify->Get_Mode() == CLASSIFY_UNIQUE ? SG_T("") : m_pShapes->Get_Field_Name(m_iColor) );
}

//---------------------------------------------------------
bool CWKSP_Shapes_Point::Get_Style_Size(int &min_Size, int &max_Size, double &min_Value, double &dValue, wxString *pName)
{
	if( m_iSize >= 0 )
	{
		min_Size	= (int)(m_Size);
		max_Size	= (int)(m_Size + (m_pShapes->Get_Maximum(m_iSize) - m_Size_Min) * m_dSize);
		min_Value	= m_Size_Min;
		dValue		= m_dSize;

		if( pName )
		{
			pName->Printf(m_pShapes->Get_Field_Name(m_iSize));
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

	m_Sel_Color_Fill	= Get_Color_asWX(m_Parameters("SEL_COLOR_FILL")->asInt());
}

//---------------------------------------------------------
inline bool CWKSP_Shapes_Point::_Draw_Initialize(CWKSP_Map_DC &dc_Map, int &Size, CSG_Shape *pShape, bool bSelection)
{
	//-----------------------------------------------------
	double	dSize;

	if( m_iSize < 0 )	// default size
	{
		dSize	= m_Size;
	}
	else				// size by attribute
	{
		if( m_Size_Scale == 0 )	// take value as is
		{
			dSize	= pShape->asDouble(m_iSize);
		}
		else					// scale to size range
		{
			dSize	= (pShape->asDouble(m_iSize) - m_Size_Min) * m_dSize + m_Size;
		}
	}

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
			dc_Map.dc.SetBrush	(wxBrush(m_Sel_Color_Fill	, wxSOLID));
			dc_Map.dc.SetPen	(wxPen	(m_Sel_Color     , 0, wxSOLID));
		}
		else
		{
			int		Color;

			if( _Get_Class_Color(pShape, Color) )
			{
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
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
void CWKSP_Shapes_Point::_Draw_Shape(CWKSP_Map_DC &dc_Map, CSG_Shape *pShape, bool bSelection)
{
	if( m_iSize >= 0 && pShape->is_NoData(m_iSize) )
	{
		return;
	}

	//-----------------------------------------------------
	int		Size;

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
	wxString		s(pShape->asString(m_iLabel, m_Label_Prec));	s.Trim(true).Trim(false);

	double	Angle	= m_iLabel_Angle < 0 ? m_Label_Angle : pShape->asDouble(m_iLabel_Angle);

	Draw_Text(dc_Map.dc, m_Label_Align, p.x, p.y, Angle, s, m_Label_Eff, m_Label_Eff_Color);
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
