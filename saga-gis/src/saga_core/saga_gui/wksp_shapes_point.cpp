
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
#define SYMBOL_TYPE_Image		13
#define SYMBOL_TYPE_Beachball	14


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Shapes_Point::CWKSP_Shapes_Point(CSG_Shapes *pShapes)
	: CWKSP_Shapes(pShapes)
{
	On_Create_Parameters();

	DataObject_Changed();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Shapes_Point::On_Create_Parameters(void)
{
	CWKSP_Shapes::On_Create_Parameters();

	BrushList_Add("NODE_DISPLAY",
		"DISPLAY_BRUSH"		, _TL("Fill Style"),
		_TL("")
	);

	m_Parameters.Add_Bool("NODE_DISPLAY",
		"OUTLINE"			, _TL("Outline"),
		_TL(""),
		true
	);

	m_Parameters.Add_Color("OUTLINE",
		"OUTLINE_COLOR"		, _TL("Color"),
		_TL(""),
		SG_GET_RGB(0, 0, 0)
	);

	m_Parameters.Add_Int("OUTLINE",
		"OUTLINE_SIZE"		, _TL("Size"),
		_TL(""),
		1, 1, true
	);

	m_Parameters.Add_Choice("NODE_DISPLAY",
		"DISPLAY_SYMBOL_TYPE"	, _TL("Symbol Type"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s",
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
			_TL("image"),
			_TL("beachball plot")
		), 0
	);

	m_Parameters.Add_FilePath("DISPLAY_SYMBOL_TYPE",
		"DISPLAY_SYMBOL_IMAGE"	, _TL("Image Symbol File"),
		_TL(""),
		CSG_String::Format(
			"%s|*.bmp;*.ico;*.gif;*.jpg;*.jif;*.jpeg;*.pcx;*.png;*.pnm;*.tif;*.tiff;*.xpm|"
			"%s (*.bmp)|*.bmp|"
			"%s (*.jpg)|*.jpg;*.jif;*.jpeg|"
			"%s (*.png)|*.png|"
			"%s (*.pcx)|*.pcx|"
			"%s (*.xpm)|*.xpm|"
			"%s (*.tif)|*.tif;*.tiff|"
			"%s (*.gif)|*.gif|"
			"%s|*.*",
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

	m_Parameters.Add_Choice("DISPLAY_SYMBOL_TYPE", "BEACHBALL_STRIKE", _TL("Strike"), _TL(""), _TL("<default>"));
	m_Parameters.Add_Choice("DISPLAY_SYMBOL_TYPE", "BEACHBALL_DIP"   , _TL("Dip"   ), _TL(""), _TL("<default>"));
	m_Parameters.Add_Choice("DISPLAY_SYMBOL_TYPE", "BEACHBALL_RAKE"  , _TL("Rake"  ), _TL(""), _TL("<default>"));


	//-----------------------------------------------------
	// Images...

	m_Parameters.Add_Choice("NODE_DISPLAY",
		"IMAGE_FIELD"			, _TL("Image Field"),
		_TL("Field that provides file paths (either absolute or relative to this data set) to images to be displayed besides the points."),
		_TL("<default>")
	);

	m_Parameters.Add_Choice("IMAGE_FIELD",
		"IMAGE_ALIGN_X"			, _TL("Horizontal Align"),
		_TL(""),
		CSG_String::Format("%s|%s|%s",
			_TL("left"),
			_TL("center"),
			_TL("right")
		), 0
	);

	m_Parameters.Add_Choice("IMAGE_FIELD",
		"IMAGE_ALIGN_Y"			, _TL("Vertical Align"),
		_TL(""),
		CSG_String::Format("%s|%s|%s",
			_TL("top"),
			_TL("center"),
			_TL("bottom")
		), 0
	);

	m_Parameters.Add_Double("IMAGE_FIELD",
		"IMAGE_OFFSET"			, _TL("Offset"),
		_TL("Offset distance to symbol relative to symbol's size."),
		1., 0., true
	);

	m_Parameters.Add_Double("IMAGE_FIELD",
		"IMAGE_SCALE"			, _TL("Size Scaling"),
		_TL("Scales image size relative to symbol's size."),
		10., 0.1, true
	);

	m_Parameters.Add_Choice("IMAGE_FIELD",
		"IMAGE_FIT"				, _TL("Fit Size to..."),
		_TL(""),
		CSG_String::Format("%s|%s|%s",
			_TL("overall"),
			_TL("width"),
			_TL("height")
		), 0
	);


	//-----------------------------------------------------
	// Label...

	m_Parameters.Add_Choice("LABEL_ATTRIB",
		"LABEL_ANGLE_ATTRIB", _TL("Rotation by Attribute"),
		_TL(""),
		_TL("<default>")
	);

	m_Parameters.Add_Double("LABEL_ANGLE_ATTRIB",
		"LABEL_ANGLE"		, _TL("Default Rotation"),
		_TL("rotation clockwise in degree"),
		0.0, -360.0, true, 360.0, true
	);

	m_Parameters.Add_Choice("LABEL_ATTRIB",
		"LABEL_ALIGN_X"		, _TL("Horizontal Align"),
		_TL(""),
		CSG_String::Format("%s|%s|%s",
			_TL("left"),
			_TL("center"),
			_TL("right")
		), 1
	);

	m_Parameters.Add_Choice("LABEL_ATTRIB",
		"LABEL_ALIGN_Y"		, _TL("Vertical Align"),
		_TL(""),
		CSG_String::Format("%s|%s|%s",
			_TL("top"),
			_TL("center"),
			_TL("bottom")
		), 0
	);

	m_Parameters.Add_Double("LABEL_ATTRIB",
		"LABEL_OFFSET"		, _TL("Offset"),
		_TL("Offset distance to symbol (either screen or map units)."),
		0., 0., true
	);


	//-----------------------------------------------------
	// Size...

	m_Parameters.Add_Choice("NODE_SIZE",
		"SIZE_TYPE"			, _TL("Size relates to..."),
		_TL(""),
		CSG_String::Format("%s|%s",
			_TL("Screen"),
			_TL("Map Units")
		), 0
	);

	m_Parameters.Add_Choice("NODE_SIZE",
		"SIZE_ATTRIB"		, _TL("Attribute"),
		_TL(""),
		_TL("<default>")
	);

	m_Parameters.Add_Choice("SIZE_ATTRIB",
		"SIZE_SCALE"		, _TL("Attribute Values"),
		_TL(""),
		CSG_String::Format("%s|%s",
			_TL("no scaling"),
			_TL("scale to size range")
		), 1
	);

	m_Parameters.Add_Range("SIZE_ATTRIB",
		"SIZE_RANGE"		, _TL("Size Range"),
		_TL(""),
		2, 10, 0, true
	);

	m_Parameters.Add_Double("SIZE_ATTRIB",
		"SIZE_DEFAULT"		, _TL("Default Size"),
		_TL(""),
		5, 0, true
	);


	//-----------------------------------------------------
	// Edit...

	m_Parameters.Add_Color("NODE_SELECTION",
		"SEL_COLOR_FILL"	, _TL("Fill Color"),
		_TL(""),
		SG_GET_RGB(255, 255, 0)
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Shapes_Point::On_DataObject_Changed(void)
{
	AttributeList_Set(m_Parameters("IMAGE_FIELD"       ),  true);
	AttributeList_Set(m_Parameters("BEACHBALL_STRIKE"  ), false);
	AttributeList_Set(m_Parameters("BEACHBALL_DIP"     ), false);
	AttributeList_Set(m_Parameters("BEACHBALL_RAKE"    ), false);
	AttributeList_Set(m_Parameters("SIZE_ATTRIB"       ),  true);
	AttributeList_Set(m_Parameters("LABEL_ANGLE_ATTRIB"),  true);

	CWKSP_Shapes::On_DataObject_Changed();
}

//---------------------------------------------------------
void CWKSP_Shapes_Point::On_Parameters_Changed(void)
{
	CWKSP_Shapes::On_Parameters_Changed();

	//-----------------------------------------------------
	m_Symbol_Type	= m_Parameters("DISPLAY_SYMBOL_TYPE")->asInt();

	if( m_Symbol_Type == SYMBOL_TYPE_Image )
	{
		if( !SG_File_Exists   (m_Parameters("DISPLAY_SYMBOL_IMAGE")->asString())
		||	!m_Symbol.LoadFile(m_Parameters("DISPLAY_SYMBOL_IMAGE")->asString()) )
		{
			m_Symbol	= IMG_Get_Image(ID_IMG_DEFAULT);
		}
	}

	if( m_Symbol_Type == SYMBOL_TYPE_Beachball )
	{
		m_Beachball[0]	= m_Parameters("BEACHBALL_STRIKE")->asInt();
		m_Beachball[1]	= m_Parameters("BEACHBALL_DIP"   )->asInt();
		m_Beachball[2]	= m_Parameters("BEACHBALL_RAKE"  )->asInt();
	}

	//-----------------------------------------------------
	m_Image_Field	= m_Parameters("IMAGE_FIELD"  )->asInt();

	if( m_Image_Field >= Get_Shapes()->Get_Field_Count() )
	{
		m_Image_Field	= -1;
	}

	switch( m_Parameters("IMAGE_ALIGN_X")->asInt() )
	{
	default: m_Image_Align  = TEXTALIGN_LEFT   ; break;
	case  1: m_Image_Align  = TEXTALIGN_XCENTER; break;
	case  2: m_Image_Align  = TEXTALIGN_RIGHT  ; break;
	}

	switch( m_Parameters("IMAGE_ALIGN_Y")->asInt() )
	{
	default: m_Image_Align |= TEXTALIGN_TOP    ; break;
	case  1: m_Image_Align |= TEXTALIGN_YCENTER; break;
	case  2: m_Image_Align |= TEXTALIGN_BOTTOM ; break;
	}

	m_Image_Offset	= m_Parameters("IMAGE_OFFSET")->asDouble();
	m_Image_Scale	= m_Parameters("IMAGE_SCALE" )->asDouble();
	m_Image_Fit		= m_Parameters("IMAGE_FIT"   )->asInt();

	//-----------------------------------------------------
	m_Size_Type		= m_Parameters("SIZE_TYPE" )->asInt();
	m_Size_Scale	= m_Parameters("SIZE_SCALE")->asInt();

	if(	(m_iSize	= m_Parameters("SIZE_ATTRIB")->asInt()) >= Get_Shapes()->Get_Field_Count()
	||	(m_dSize	= Get_Shapes()->Get_Maximum(m_iSize) - (m_Size_Min = Get_Shapes()->Get_Minimum(m_iSize))) <= 0.0 )
	{
		m_iSize		= -1;
		m_Size		= m_Parameters("SIZE_DEFAULT")->asDouble();
		m_dSize		= 0.0;
		m_Size_Min	= 0.0;
	}
	else
	{
		m_Size		=  m_Parameters("SIZE_RANGE")->asRange()->Get_Min();
		m_dSize		= (m_Parameters("SIZE_RANGE")->asRange()->Get_Max() - m_Size) / m_dSize;
	}

	//-----------------------------------------------------
	switch( m_Parameters("LABEL_ALIGN_X")->asInt() )
	{
	default: m_Label_Align  = TEXTALIGN_LEFT   ; break;
	case  1: m_Label_Align  = TEXTALIGN_XCENTER; break;
	case  2: m_Label_Align  = TEXTALIGN_RIGHT  ; break;
	}

	switch( m_Parameters("LABEL_ALIGN_Y")->asInt() )
	{
	default: m_Label_Align |= TEXTALIGN_TOP    ; break;
	case  1: m_Label_Align |= TEXTALIGN_YCENTER; break;
	case  2: m_Label_Align |= TEXTALIGN_BOTTOM ; break;
	}

	m_Label_Angle	= m_Parameters("LABEL_ANGLE" )->asDouble();

	if( (m_iLabel_Angle	= m_Parameters("LABEL_ANGLE_ATTRIB")->asInt()) >= Get_Shapes()->Get_Field_Count() )
	{
		m_iLabel_Angle	= -1;
	}

	//-----------------------------------------------------
	m_bOutline	= m_Parameters("OUTLINE")->asBool();
	m_Pen		= wxPen(!m_bOutline ? m_pClassify->Get_Unique_Color() : Get_Color_asWX(m_Parameters("OUTLINE_COLOR")->asColor()), m_Parameters("OUTLINE_SIZE")->asInt(), wxPENSTYLE_SOLID);
	m_Brush		= wxBrush(m_pClassify->Get_Unique_Color(), BrushList_Get_Style("DISPLAY_BRUSH"));
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CWKSP_Shapes_Point::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter, int Flags)
{
	//-----------------------------------------------------
	if( Flags & PARAMETER_CHECK_VALUES )
	{
		if(	pParameter->Cmp_Identifier("COLORS_FONT") )
		{
			Set_Metrics(
				(*pParameters)("METRIC_ATTRIB")->asInt(),
				(*pParameters)("METRIC_NORMAL")->asInt()
			);

			(*pParameters)("METRIC_ZRANGE")->asRange()->Set_Range(
				m_Metrics.Get_Minimum(),
				m_Metrics.Get_Maximum()
			);
		}
	}

	//-----------------------------------------------------
	if( Flags & PARAMETER_CHECK_ENABLE )
	{
		if(	pParameter->Cmp_Identifier("DISPLAY_SYMBOL_TYPE") )
		{
			int		Value	= pParameter->asInt();

			pParameters->Set_Enabled("DISPLAY_SYMBOL_IMAGE", Value == SYMBOL_TYPE_Image    );
			pParameters->Set_Enabled("BEACHBALL_STRIKE"    , Value == SYMBOL_TYPE_Beachball);
			pParameters->Set_Enabled("BEACHBALL_DIP"       , Value == SYMBOL_TYPE_Beachball);
			pParameters->Set_Enabled("BEACHBALL_RAKE"      , Value == SYMBOL_TYPE_Beachball);
		}

		if(	pParameter->Cmp_Identifier("LABEL_ATTRIB") )
		{
			bool	Value	= pParameter->asInt() < Get_Shapes()->Get_Field_Count();

			pParameters->Set_Enabled("LABEL_ANGLE_ATTRIB"  , Value);
			pParameters->Set_Enabled("LABEL_ANGLE"         , Value);
			pParameters->Set_Enabled("LABEL_ALIGN_X"       , Value);
			pParameters->Set_Enabled("LABEL_ALIGN_Y"       , Value);
		}

		if(	pParameter->Cmp_Identifier("LABEL_ANGLE_ATTRIB") )
		{
			bool	Value	= pParameter->asInt() >= Get_Shapes()->Get_Field_Count();

			pParameters->Set_Enabled("LABEL_ANGLE"         , Value);
		}

		if(	pParameter->Cmp_Identifier("SIZE_ATTRIB") )
		{
			bool	Value	= pParameter->asInt() < Get_Shapes()->Get_Field_Count();

			pParameters->Set_Enabled("SIZE_SCALE"          , Value ==  true);
			pParameters->Set_Enabled("SIZE_RANGE"          , Value ==  true);
			pParameters->Set_Enabled("SIZE_DEFAULT"        , Value == false);
		}

		if(	pParameter->Cmp_Identifier("IMAGE_FIELD") )
		{
			pParameter->Set_Children_Enabled(pParameter->asInt() < Get_Shapes()->Get_Field_Count());
		}
	}

	return( CWKSP_Shapes::On_Parameter_Changed(pParameters, pParameter, Flags) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Shapes_Point::Get_Style_Size(int &min_Size, int &max_Size, double &min_Value, double &dValue, wxString *pName)
{
	if( m_iSize >= 0 )
	{
		min_Size	= (int)(m_Size);
		max_Size	= (int)(m_Size + (Get_Shapes()->Get_Maximum(m_iSize) - m_Size_Min) * m_dSize);
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
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline void CWKSP_Shapes_Point::Draw_Initialize(CWKSP_Map_DC &dc_Map, int Flags)
{
	dc_Map.dc.SetBrush(m_Brush);
	dc_Map.dc.SetPen  (m_Pen  );

	m_Sel_Color_Fill	= Get_Color_asWX(m_Parameters("SEL_COLOR_FILL")->asInt());

	m_Symbol_Type	= m_Parameters("DISPLAY_SYMBOL_TYPE")->asInt();

	if( (Flags & LAYER_DRAW_FLAG_THUMBNAIL) != 0 )
	{
		if( m_Symbol_Type == SYMBOL_TYPE_Image
		||  m_Symbol_Type == SYMBOL_TYPE_Beachball )
		{
			m_Symbol_Type	= 0;
		}
	}
}

//---------------------------------------------------------
inline bool CWKSP_Shapes_Point::Draw_Initialize(CWKSP_Map_DC &dc_Map, int &Size, CSG_Shape *pShape, int Selection)
{
	//-----------------------------------------------------
	if( m_Brush.IsTransparent() && !m_bOutline && !Selection )
	{
		return( false );	// nothing to draw !
	}

	//-----------------------------------------------------
	if( Selection )
	{
		dc_Map.dc.SetBrush(wxBrush(m_Sel_Color_Fill, m_Brush.GetStyle()));
		dc_Map.dc.SetPen(wxPen(m_Sel_Color, Selection == 1 ? 2 : 0, wxPENSTYLE_SOLID));
	}
	else
	{
		int		Color;

		if( !Get_Class_Color(pShape, Color) && !m_bNoData )
		{
			return( false );
		}

		if( !m_Brush.IsTransparent() )
		{
			wxBrush	Brush(m_Brush);	Brush.SetColour(SG_GET_R(Color), SG_GET_G(Color), SG_GET_B(Color)); dc_Map.dc.SetBrush(Brush);
		}

		if( !m_bOutline )
		{
			wxPen	Pen  (m_Pen  );	Pen  .SetColour(SG_GET_R(Color), SG_GET_G(Color), SG_GET_B(Color)); dc_Map.dc.SetPen(Pen);
		}
	}

	//-----------------------------------------------------
	m_Label_Offset	= m_Parameters("LABEL_OFFSET")->asDouble();

	if( m_Label_Offset > 0. )
	{
		switch( m_Parameters("LABEL_ATTRIB_SIZE_TYPE")->asInt() )
		{
		default: m_Label_Offset *= dc_Map.m_Scale   ; break;
		case  1: m_Label_Offset *= dc_Map.m_World2DC; break;
		}
	}

	//-----------------------------------------------------
	double	dSize	= m_iSize < 0 ? m_Size : pShape->asDouble(m_iSize);

	if( m_Size_Scale != 0 )	// scale to size range
	{
		dSize	= (dSize - m_Size_Min) * m_dSize + m_Size;
	}

	switch( m_Size_Type )
	{
	default: dSize *= dc_Map.m_Scale   ; break;
	case  1: dSize *= dc_Map.m_World2DC; break;
	}

	return( (Size = (int)(0.5 + dSize)) > 0 );
}

//---------------------------------------------------------
void CWKSP_Shapes_Point::Draw_Shape(CWKSP_Map_DC &dc_Map, CSG_Shape *pShape, int Selection)
{
	if( m_iSize < 0 || !pShape->is_NoData(m_iSize) )
	{
		int		Size;

		if( Draw_Initialize(dc_Map, Size, pShape, Selection) )
		{
			TSG_Point_Int	p(dc_Map.World2DC(pShape->Get_Point(0)));

			//---------------------------------------------
			if( m_Symbol_Type == SYMBOL_TYPE_Beachball )
			{
				if( !pShape->is_NoData(m_Beachball[0])
				&&  !pShape->is_NoData(m_Beachball[1])
				&&  !pShape->is_NoData(m_Beachball[2])	)
				{
					_Beachball_Draw(dc_Map.dc, p.x, p.y, Size,
						pShape->asDouble(m_Beachball[0]),
						pShape->asDouble(m_Beachball[1]),
						pShape->asDouble(m_Beachball[2])
					);
				}
			}

			//---------------------------------------------
			else if( m_Symbol_Type == SYMBOL_TYPE_Image )
			{
				double	d	= (double)m_Symbol.GetWidth() / (double)m_Symbol.GetHeight();

				int	sx	= d >  1.0 ? Size : (int)(0.5 + Size * d);
				int	sy	= d <= 1.0 ? Size : (int)(0.5 + Size / d);

				if( sx > 0 && sy > 0 )
				{
					wxRect	r(p.x - sx, p.y - sy, 2 * sx, 2 * sy);

					if( Selection )
					{
						r.Inflate(1);

						dc_Map.dc.DrawRectangle(r);
					}

					dc_Map.dc.DrawBitmap(wxBitmap(m_Symbol.Scale(2 * sx, 2 * sy)), p.x - sx, p.y - sy, true);
				}
			}

			//---------------------------------------------
			else
			{
				Draw_Symbol(dc_Map.dc, p.x, p.y, Size);
			}

			//---------------------------------------------
			if( m_Image_Field >= 0 )
			{
				_Image_Draw(dc_Map.dc, p.x, p.y, Size, pShape->asString(m_Image_Field));
			}

			//---------------------------------------------
			if( Selection )
			{
				dc_Map.dc.SetBrush(m_Brush);
				dc_Map.dc.SetPen  (m_Pen  );
			}
		}
	}
}

//---------------------------------------------------------
void CWKSP_Shapes_Point::Draw_Label(CWKSP_Map_DC &dc_Map, CSG_Shape *pShape, const wxString &Label)
{
	TSG_Point_Int	p(dc_Map.World2DC(pShape->Get_Point(0)));

	if( m_Label_Offset > 0. )
	{
		if( (m_Label_Align  & TEXTALIGN_LEFT   ) != 0 ) p.x += m_Label_Offset; else
		if( (m_Label_Align  & TEXTALIGN_RIGHT  ) != 0 ) p.x -= m_Label_Offset;

		if( (m_Label_Align  & TEXTALIGN_TOP    ) != 0 ) p.y += m_Label_Offset; else
		if( (m_Label_Align  & TEXTALIGN_BOTTOM ) != 0 ) p.y -= m_Label_Offset;
	}

	double	Angle	= m_iLabel_Angle < 0 ? m_Label_Angle : pShape->asDouble(m_iLabel_Angle);

	Draw_Text(dc_Map.dc, m_Label_Align, p.x, p.y, Angle, Label, m_Label_Eff, m_Label_Eff_Color, m_Label_Eff_Size);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define DRAW_CIRCLE(d)		{	int		s	= (int)((d) * Size);\
	dc.DrawCircle(x, y, s);\
}

//---------------------------------------------------------
#define DRAW_SQUARE			{	int		s	= (int)(0.7071067812 * Size);\
	dc.DrawRectangle(x - s, y - s, 2 * s, 2 * s);\
}

//---------------------------------------------------------
#define DRAW_RHOMBUS		{	wxPoint	p[4];\
	p[0].y	= p[2].y	= y;\
	p[1].x	= p[3].x	= x;\
	p[0].x				= x - Size;\
	p[2].x				= x + Size;\
	p[1].y				= y - Size;\
	p[3].y				= y + Size;\
	dc.DrawPolygon(4, p);\
}

//---------------------------------------------------------
#define DRAW_TRIANGLE_UP	{	wxPoint	p[3];\
	p[0].y	= p[1].y	= y + (Size / 2);\
	p[2].y				= y - (Size - 1);\
	p[0].x				= x - (int)(0.8660254038 * Size);\
	p[1].x				= x + (int)(0.8660254038 * Size);\
	p[2].x				= x;\
	dc.DrawPolygon(3, p);\
}

//---------------------------------------------------------
#define DRAW_TRIANGLE_DOWN	{	wxPoint	p[3];\
	p[0].y	= p[1].y	= y - (Size / 2);\
	p[2].y				= y + (Size - 1);\
	p[0].x				= x - (int)(0.8660254038 * Size);\
	p[1].x				= x + (int)(0.8660254038 * Size);\
	p[2].x				= x;\
	dc.DrawPolygon(3, p);\
}

//---------------------------------------------------------
void CWKSP_Shapes_Point::Draw_Symbol(wxDC &dc, int x, int y, int Size)
{
	switch( m_Symbol_Type )
	{
	default: DRAW_CIRCLE(1.0)                      ; break;	// circle
	case  1: DRAW_SQUARE                           ; break;	// square
	case  2: DRAW_RHOMBUS                          ; break;	// rhombus
	case  3: DRAW_TRIANGLE_UP                      ; break;	// triangle up
	case  4: DRAW_TRIANGLE_DOWN                    ; break;	// triangle down
	case  5: DRAW_CIRCLE(1.0)  ; DRAW_SQUARE       ; break;	// square in circle
	case  6: DRAW_CIRCLE(1.1)  ; DRAW_RHOMBUS      ; break;	// rhombus in circle
	case  7: DRAW_CIRCLE(1.0)  ; DRAW_TRIANGLE_UP  ; break;	// triangle up in circle
	case  8: DRAW_CIRCLE(1.0)  ; DRAW_TRIANGLE_DOWN; break;	// triangle down in circle
	case  9: DRAW_SQUARE       ; DRAW_CIRCLE(0.7)  ; break;	// circle in square
	case 10: DRAW_RHOMBUS      ; DRAW_CIRCLE(0.7)  ; break;	// circle in rhombus
	case 11: DRAW_TRIANGLE_UP  ; DRAW_CIRCLE(0.5)  ; break;	// circle in triangle up
	case 12: DRAW_TRIANGLE_DOWN; DRAW_CIRCLE(0.5)  ; break;	// circle in triangle down
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Shapes_Point::_Image_Draw(wxDC &dc, int x, int y, int size, const wxString &file)
{
	wxFileName	fn(file);

	if( fn.IsRelative() )
	{
		fn.MakeAbsolute(SG_File_Get_Path(Get_Shapes()->Get_File_Name()).c_str());
	}

	wxImage	Symbol;

	if( fn.Exists() && Symbol.LoadFile(fn.GetFullPath()) )
	{
		double	s	= m_Image_Scale  * size;
		double	o	= m_Image_Offset * size;

		int		sx	= Symbol.GetWidth ();
		int		sy	= Symbol.GetHeight();

		double	d	= (double)sx / (double)sy;

		switch( m_Image_Fit )
		{
		default:
			sx	= d >  1.0 ? s : (int)(0.5 + s * d);
			sy	= d <= 1.0 ? s : (int)(0.5 + s / d);
			break;

		case  1:	// width
			sx	= s;
			sy	= (int)(0.5 + s / d);
			break;

		case  2:	// height
			sx	= (int)(0.5 + s * d);
			sy	= s;
			break;
		}

		if( sx > 1 && sy > 1 )
		{
			if( m_Image_Align & TEXTALIGN_LEFT    ) { x += o     ; } else
			if( m_Image_Align & TEXTALIGN_XCENTER ) { x -= sx / 2; } else
			if( m_Image_Align & TEXTALIGN_RIGHT   ) { x	-= o + sx; }

			if( m_Image_Align & TEXTALIGN_TOP     ) { y += o     ; } else
			if( m_Image_Align & TEXTALIGN_YCENTER ) { y -= sy / 2; } else
			if( m_Image_Align & TEXTALIGN_BOTTOM  ) { y -= o + sy; }

			dc.DrawBitmap(wxBitmap(Symbol.Scale(sx, sy)), x, y, true);
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Shapes_Point::_Beachball_Draw(wxDC &dc, int x, int y, int size, double strike, double dip, double rake)
{
	const double	dArc	= 5.0;

	strike	*= M_DEG_TO_RAD;
	dip		*= M_DEG_TO_RAD;
	rake	*= M_DEG_TO_RAD;

	CSG_Shapes	Plot(SHAPE_TYPE_Polygon);

	Plot.Add_Shape();	// 0, unit circle

	for(double a=0.0; a<M_PI_360; a+=dArc*M_DEG_TO_RAD)
	{
		Plot.Get_Shape(0)->Add_Point(sin(a), cos(a));
	}

	CSG_Vector	N(3);

	N[0] = 0; N[1] = 0; N[2] = 1;

	SG_VectorR3_Rotate(N, 1, dip);
	SG_VectorR3_Rotate(N, 2, strike);

	_Beachball_Get_Plane(Plot.Add_Shape(), Plot.Get_Shape(0), N);	// 1, fault plane

	N[0] = 0; N[1] = -1; N[2] = 0;

	rake = fmod(rake, M_PI_360); if( rake < -M_PI_180 ) rake += M_PI_360; else if( rake > M_PI_180 ) rake -= M_PI_360;

	SG_VectorR3_Rotate(N, 2, -rake);
	SG_VectorR3_Rotate(N, 1, dip);
	SG_VectorR3_Rotate(N, 2, strike);

	_Beachball_Get_Plane(Plot.Add_Shape(), Plot.Get_Shape(0), N);	// 2, auxiliary plane

	//-----------------------------------------------------
	SG_Polygon_Intersection(Plot.Get_Shape(0), Plot.Get_Shape(1), Plot.Add_Shape());	// 3
	SG_Polygon_Difference  (Plot.Get_Shape(0), Plot.Get_Shape(1), Plot.Add_Shape());	// 4

	CSG_Shape_Polygon	*pPlot[2];

	if( rake < 0.0 )
	{
		SG_Polygon_Difference  (Plot.Get_Shape(3), Plot.Get_Shape(2), pPlot[0] = (CSG_Shape_Polygon *)Plot.Add_Shape());
		SG_Polygon_Intersection(Plot.Get_Shape(4), Plot.Get_Shape(2), pPlot[1] = (CSG_Shape_Polygon *)Plot.Add_Shape());
	}
	else
	{
		SG_Polygon_Intersection(Plot.Get_Shape(3), Plot.Get_Shape(2), pPlot[0] = (CSG_Shape_Polygon *)Plot.Add_Shape());
		SG_Polygon_Difference  (Plot.Get_Shape(4), Plot.Get_Shape(2), pPlot[1] = (CSG_Shape_Polygon *)Plot.Add_Shape());
	}

	//-----------------------------------------------------
	dc.DrawCircle(x, y, size);

	wxBrush	b = dc.GetBrush(); dc.SetBrush(*wxWHITE_BRUSH);

	for(int i=0; i<2; i++)
	{
		// SG_Polygon_Offset(pPlot[i], -0.01, dArc);
		_Beachball_Get_Scaled(pPlot[i], x,  y, size);

		if( pPlot[i]->Get_Area() > 1 )
		{
			wxPoint	*Points	= new wxPoint[pPlot[i]->Get_Point_Count(0)];

			for(int iPoint=0; iPoint<pPlot[i]->Get_Point_Count(0); iPoint++)
			{
				TSG_Point	p		= pPlot[i]->Get_Point(iPoint, 0);
				Points[iPoint].x	= (int)(p.x + 0.5);
				Points[iPoint].y	= (int)(p.y + 0.5);
			}

			dc.DrawPolygon(pPlot[i]->Get_Point_Count(0), Points);

			delete[](Points);
		}
	}

	dc.SetBrush(b);
}

//---------------------------------------------------------
void CWKSP_Shapes_Point::_Beachball_Get_Plane(CSG_Shape *pPlane, CSG_Shape *pCircle, const CSG_Vector &Normal)
{
	CSG_Vector	Ez(3);	Ez[2]	=  1.0;

	double	Slope	= Normal.Get_Angle(Ez);
	double	Azimuth	= M_PI_090 + atan2(Normal[1], Normal[0]);

	if( Slope > M_PI_090 )
	{
		Azimuth	+= M_PI_180;
		Slope	 = M_PI_090 - (Slope - M_PI_090);
	}

	TSG_Point	A, C;

	A.x	= sin(Azimuth - M_PI_090);	// strike
	A.y	= cos(Azimuth - M_PI_090);

	if( Slope < M_PI_090 )
	{
		double	d	= -2. * tan(Slope / 2.);	// stereographic projection

		C.x	= d * sin(Azimuth);
		C.y	= d * cos(Azimuth);

		pPlane->Add_Part(((CSG_Shape_Polygon *)pCircle)->Get_Part(0));

		_Beachball_Get_Scaled(pPlane, C.x, C.y, SG_Get_Distance(A, C));
	}
	else
	{
		A.x	*= 1.1;	A.y	*= 1.1;

		pPlane->Add_Point(-A.x, -A.y);
		pPlane->Add_Point( A.x,  A.y);
		pPlane->Add_Point( A.x - A.y,  A.y + A.x);
		pPlane->Add_Point(-A.x - A.y, -A.y + A.x);
	}
}

//---------------------------------------------------------
void CWKSP_Shapes_Point::_Beachball_Get_Scaled(CSG_Shape *pShape, double x, double y, double size)
{
	for(int iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
	{
		for(int iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
		{
			TSG_Point	Point	= pShape->Get_Point(iPoint, iPart);

			Point.x	= x + size * Point.x;
			Point.y	= y - size * Point.y;

			pShape->Set_Point(Point, iPoint, iPart);
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
