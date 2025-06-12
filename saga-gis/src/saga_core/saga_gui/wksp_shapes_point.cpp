
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

#include <saga_gdi/sgdi_helper.h>

#include "res_commands.h"
#include "res_images.h"

#include "helper.h"

#include "wksp_layer_classify.h"

#include "wksp_shapes_point.h"


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define SYMBOL_TYPE_Image     13
#define SYMBOL_TYPE_Arrow     14
#define SYMBOL_TYPE_Beachball 15


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Shapes_Point::CWKSP_Shapes_Point(CSG_Shapes *pShapes)
	: CWKSP_Shapes(pShapes)
{
	On_Create_Parameters();

	DataObject_Changed();
}


///////////////////////////////////////////////////////////
//                                                       //
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
			_TL("arrow"),
			_TL("beachball plot")
		), 0
	);

	m_Parameters.Add_FilePath("DISPLAY_SYMBOL_TYPE",
		"DISPLAY_SYMBOL_IMAGE"	, _TL("Image Symbol File"),
		_TL(""),
		CSG_String::Format(
			"%s|*.bmp;*.ico;*.gif;*.jpg;*.jif;*.jpeg;*.pcx;*.png;*.pnm;*.tif;*.tiff;*.xpm|"
			"%s (*.png)|*.png|"
			"%s (*.jpg)|*.jpg;*.jif;*.jpeg|"
			"%s (*.tif)|*.tif;*.tiff|"
			"%s (*.gif)|*.gif|"
			"%s (*.bmp)|*.bmp|"
			"%s (*.pcx)|*.pcx|"
			"%s (*.xpm)|*.xpm|"
			"%s|*.*",
			_TL("Recognized File Types"      ),
			_TL("Portable Network Graphics"  ),
			_TL("JPEG - JFIF Compliant"      ),
			_TL("Tagged Image File Format"   ),
			_TL("Graphics Interchange Format"),
			_TL("Windows or OS/2 Bitmap"     ),
			_TL("Zsoft Paintbrush"           ),
			_TL("X11 Pixel Map"              ),
			_TL("All Files"                  )
		)
	);

	m_Parameters.Add_Choice("DISPLAY_SYMBOL_TYPE", "ARROW_DIRECTION"  , _TL("Direction"  ), _TL(""), "<not set>");
	m_Parameters.Add_Int   ("ARROW_DIRECTION"    , "ARROW_OFFSET"     , _TL("Offset"     ), _TL(""), 0.);
	m_Parameters.Add_Choice("ARROW_DIRECTION"    , "ARROW_ORIENTATION", _TL("Orientation"), _TL(""), CSG_String::Format("%s|%s", _TL("clockwise"), _TL("counterclockwise")));
	m_Parameters.Add_Choice("ARROW_DIRECTION"    , "ARROW_UNIT"       , _TL("Unit"       ), _TL(""), CSG_String::Format("%s|%s", _TL("degree"), _TL("radians")));
	m_Parameters.Add_Choice("ARROW_DIRECTION"    , "ARROW_STYLE"      , _TL("Style"      ), _TL(""), CSG_String::Format("%s|%s|%s", _TL("simple"), _TL("center"), _TL("circle")));
	m_Parameters.Add_Int   ("ARROW_DIRECTION"    , "ARROW_WIDTH"      , _TL("Line Width" ), _TL(""), 2, 1, true);

	m_Parameters.Add_Choice("DISPLAY_SYMBOL_TYPE", "BEACHBALL_STRIKE" , _TL("Strike"     ), _TL(""), "<not set>");
	m_Parameters.Add_Choice("DISPLAY_SYMBOL_TYPE", "BEACHBALL_DIP"    , _TL("Dip"        ), _TL(""), "<not set>");
	m_Parameters.Add_Choice("DISPLAY_SYMBOL_TYPE", "BEACHBALL_RAKE"   , _TL("Rake"       ), _TL(""), "<not set>");


	//-----------------------------------------------------
	// Images...

	m_Parameters.Add_Choice("NODE_DISPLAY",
		"IMAGE_FIELD"			, _TL("Image Field"),
		_TL("Field that provides file paths (either absolute or relative to this data set) to images to be displayed besides the points."),
		"<not set>"
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

	m_Parameters.Add_Choice("LABEL_FIELD",
		"LABEL_ANGLE_FIELD" , _TL("Rotation by Attribute"),
		_TL(""),
		"<not set>"
	);

	m_Parameters.Add_Double("LABEL_ANGLE_FIELD",
		"LABEL_ANGLE"		, _TL("Default Rotation"),
		_TL("rotation clockwise in degree"),
		0.0, -360.0, true, 360.0, true
	);

	m_Parameters.Add_Node("LABEL_FIELD",
		"LABEL_PLACEMENT"	, _TL("Placement"),
		_TL("")
	);

	m_Parameters.Add_Choice("LABEL_PLACEMENT",
		"LABEL_PLACEMENT_X"	, _TL("Horizontal"),
		_TL(""),
		CSG_String::Format("%s|%s|%s",
			_TL("left"),
			_TL("center"),
			_TL("right")
		), 1
	);

	m_Parameters.Add_Double("LABEL_PLACEMENT_X",
		"LABEL_OFFSET_X"	, _TL("Offset"),
		_TL("Offset distance to symbol (either screen or map units)."),
		0., 0., true
	);

	m_Parameters.Add_Choice("LABEL_PLACEMENT",
		"LABEL_PLACEMENT_Y"	, _TL("Vertical"),
		_TL(""),
		CSG_String::Format("%s|%s|%s",
			_TL("above"),
			_TL("center"),
			_TL("below")
		), 2
	);

	m_Parameters.Add_Double("LABEL_PLACEMENT_Y",
		"LABEL_OFFSET_Y"	, _TL("Offset"),
		_TL("Offset distance to symbol (either screen or map units)."),
		0., 0., true
	);


	//-----------------------------------------------------
	// Size...

	m_Parameters.Add_Double("NODE_SIZE",
		"SIZE_DEFAULT"		, _TL("Size"),
		_TL(""),
		5., 0., true
	);

	m_Parameters.Add_Choice("NODE_SIZE",
		"SIZE_FIELD"		, _TL("Attribute"),
		_TL(""),
		"<not set>"
	);

	m_Parameters.Add_Choice("SIZE_FIELD",
		"SIZE_SCALE"		, _TL("Attribute Values"),
		_TL(""),
		CSG_String::Format("%s|%s",
			_TL("take as size"),
			_TL("scale to size range")
		), 1
	);

	m_Parameters.Add_Range("SIZE_FIELD",
		"SIZE_RANGE"		, _TL("Size Range"),
		_TL(""),
		2., 10., 0., true
	);

	m_Parameters.Add_Choice("NODE_SIZE",
		"SIZE_TYPE"			, _TL("Size relates to..."),
		_TL(""),
		CSG_String::Format("%s|%s",
			_TL("Screen"),
			_TL("Map Units")
		), 0
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
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Shapes_Point::On_DataObject_Changed(void)
{
	Set_Fields_Choice(m_Parameters("IMAGE_FIELD"      ), false,  true);
	Set_Fields_Choice(m_Parameters("SIZE_FIELD"       ),  true,  true);
	Set_Fields_Choice(m_Parameters("LABEL_ANGLE_FIELD"),  true,  true);
	Set_Fields_Choice(m_Parameters("ARROW_DIRECTION"  ),  true, false);
	Set_Fields_Choice(m_Parameters("BEACHBALL_STRIKE" ),  true, false);
	Set_Fields_Choice(m_Parameters("BEACHBALL_DIP"    ),  true, false);
	Set_Fields_Choice(m_Parameters("BEACHBALL_RAKE"   ),  true, false);

	CWKSP_Shapes::On_DataObject_Changed();
}

//---------------------------------------------------------
void CWKSP_Shapes_Point::On_Parameters_Changed(void)
{
	CWKSP_Shapes::On_Parameters_Changed();

	m_Symbol.Type = m_Parameters("DISPLAY_SYMBOL_TYPE")->asInt();

	switch( m_Symbol.Type )
	{
	//-----------------------------------------------------
	case SYMBOL_TYPE_Image:
		if( !SG_File_Exists         (m_Parameters("DISPLAY_SYMBOL_IMAGE")->asString())
		||	!m_Symbol.Image.LoadFile(m_Parameters("DISPLAY_SYMBOL_IMAGE")->asString()) )
		{
			m_Symbol.Image = IMG_Get_Bitmap(ID_IMG_DEFAULT).ConvertToImage();
		}
		else
		{
			m_Symbol_Image.Destroy();
		}
		break;

	//-----------------------------------------------------
	case SYMBOL_TYPE_Arrow:
		m_Arrow.Field       = Get_Fields_Choice(m_Parameters("ARROW_DIRECTION"));
		m_Arrow.Offset      = m_Parameters("ARROW_OFFSET"     )->asDouble();
		m_Arrow.Orientation = m_Parameters("ARROW_ORIENTATION")->asInt   ();
		m_Arrow.Unit        = m_Parameters("ARROW_UNIT"       )->asInt   ();
		m_Arrow.Style       = m_Parameters("ARROW_STYLE"      )->asInt   ();
		m_Arrow.Width       = m_Parameters("ARROW_WIDTH"      )->asInt   ();
		break;

	//-----------------------------------------------------
	case SYMBOL_TYPE_Beachball:
		m_Beachball.Strike  = Get_Fields_Choice(m_Parameters("BEACHBALL_STRIKE"));
		m_Beachball.Dip     = Get_Fields_Choice(m_Parameters("BEACHBALL_DIP"   ));
		m_Beachball.Rake    = Get_Fields_Choice(m_Parameters("BEACHBALL_RAKE"  ));
		break;
	}

	//-----------------------------------------------------
	m_Image.Field = m_Parameters("IMAGE_FIELD" )->asInt();

	if( m_Image.Field >= Get_Shapes()->Get_Field_Count() )
	{
		m_Image.Field = -1;
	}
	else
	{
		m_Image.Offset = m_Parameters("IMAGE_OFFSET")->asDouble();
		m_Image.Scale  = m_Parameters("IMAGE_SCALE" )->asDouble();
		m_Image.Fit    = m_Parameters("IMAGE_FIT"   )->asInt();

		switch( m_Parameters("IMAGE_ALIGN_X")->asInt() )
		{
		default: m_Image.Align  = TEXTALIGN_LEFT   ; break;
		case  1: m_Image.Align  = TEXTALIGN_XCENTER; break;
		case  2: m_Image.Align  = TEXTALIGN_RIGHT  ; break;
		}

		switch( m_Parameters("IMAGE_ALIGN_Y")->asInt() )
		{
		default: m_Image.Align |= TEXTALIGN_TOP    ; break;
		case  1: m_Image.Align |= TEXTALIGN_YCENTER; break;
		case  2: m_Image.Align |= TEXTALIGN_BOTTOM ; break;
		}
	}

	//-----------------------------------------------------
	m_Size.Unit   = m_Parameters("SIZE_TYPE" )->asInt(); // screen pixels, map units
	m_Size.Adjust = m_Parameters("SIZE_SCALE")->asInt(); // adjust to size range

	if( (m_Size.Field = Get_Fields_Choice(m_Parameters("SIZE_FIELD"))) >= 0
	&&  (m_Size.Scale = Get_Shapes()->Get_Maximum(m_Size.Field) - Get_Shapes()->Get_Minimum(m_Size.Field)) > 0. )
	{
		m_Size.Offset  =  m_Parameters("SIZE_RANGE.MIN")->asDouble();
		m_Size.Scale   = (m_Parameters("SIZE_RANGE.MAX")->asDouble() - m_Size.Offset) / m_Size.Scale;
		m_Size.Minimum = Get_Shapes()->Get_Minimum(m_Size.Field);
	}
	else
	{
		m_Size.Field   = -1;
		m_Size.Offset  = m_Parameters("SIZE_DEFAULT")->asDouble();
		m_Size.Scale   = 0.;
		m_Size.Minimum = 0.;
	}

	//-----------------------------------------------------
	switch( m_Parameters("LABEL_PLACEMENT_X")->asInt() )
	{
	default: m_Label_Point.Align  = TEXTALIGN_LEFT   ; break;
	case  1: m_Label_Point.Align  = TEXTALIGN_XCENTER; break;
	case  0: m_Label_Point.Align  = TEXTALIGN_RIGHT  ; break;
	}

	switch( m_Parameters("LABEL_PLACEMENT_Y")->asInt() )
	{
	default: m_Label_Point.Align |= TEXTALIGN_TOP    ; break;
	case  1: m_Label_Point.Align |= TEXTALIGN_YCENTER; break;
	case  0: m_Label_Point.Align |= TEXTALIGN_BOTTOM ; break;
	}

	m_Label_Point.Angle       = m_Parameters("LABEL_ANGLE")->asDouble();
	m_Label_Point.Angle_Field = Get_Fields_Choice(m_Parameters("LABEL_ANGLE_FIELD"));

	//-----------------------------------------------------
	m_bOutline = m_Parameters("OUTLINE")->asBool();
	m_Pen      = wxPen(!m_bOutline ? m_pClassify->Get_Unique_Color() : Get_Color_asWX(m_Parameters("OUTLINE_COLOR")->asColor()), !m_bOutline ? 1 : m_Parameters("OUTLINE_SIZE")->asInt(), wxPENSTYLE_SOLID);
	m_Brush    = wxBrush(m_pClassify->Get_Unique_Color(), BrushList_Get_Style("DISPLAY_BRUSH"));
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CWKSP_Shapes_Point::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter, int Flags)
{
	//-----------------------------------------------------
	if( Flags & PARAMETER_CHECK_VALUES )
	{
		if(	pParameter->Cmp_Identifier("COLORS_FONT") )
		{
			return( CWKSP_Shapes::On_Parameter_Changed(pParameters, (*pParameters)("METRIC_FIELD"), Flags) );
		}
	}

	//-----------------------------------------------------
	if( Flags & PARAMETER_CHECK_ENABLE )
	{
		if(	pParameter->Cmp_Identifier("DISPLAY_SYMBOL_TYPE") )
		{
			int Value = pParameter->asInt();

			pParameters->Set_Enabled("DISPLAY_SYMBOL_IMAGE", Value == SYMBOL_TYPE_Image    );
			pParameters->Set_Enabled("ARROW_DIRECTION"     , Value == SYMBOL_TYPE_Arrow    );
			pParameters->Set_Enabled("BEACHBALL_STRIKE"    , Value == SYMBOL_TYPE_Beachball);
			pParameters->Set_Enabled("BEACHBALL_DIP"       , Value == SYMBOL_TYPE_Beachball);
			pParameters->Set_Enabled("BEACHBALL_RAKE"      , Value == SYMBOL_TYPE_Beachball);
		}

		if(	pParameter->Cmp_Identifier("LABEL_FIELD") )
		{
			bool Value = pParameter->asInt() < Get_Shapes()->Get_Field_Count();

			pParameters->Set_Enabled("LABEL_ANGLE_FIELD"   , Value);
			pParameters->Set_Enabled("LABEL_ANGLE"         , Value);
			pParameters->Set_Enabled("LABEL_PLACEMENT"     , Value);
		}

		if(	pParameter->Cmp_Identifier("LABEL_PLACEMENT_X") )
		{
			pParameters->Set_Enabled("LABEL_OFFSET_X"      , pParameter->asInt() != 1);
		}

		if(	pParameter->Cmp_Identifier("LABEL_PLACEMENT_Y") )
		{
			pParameters->Set_Enabled("LABEL_OFFSET_Y"      , pParameter->asInt() != 1);
		}

		if(	pParameter->Cmp_Identifier("LABEL_ANGLE_FIELD") )
		{
			pParameters->Set_Enabled("LABEL_ANGLE"         , Get_Fields_Choice(pParameter) < 0);
		}

		if(	pParameter->Cmp_Identifier("SIZE_FIELD") )
		{
			int Value = Get_Fields_Choice(pParameter);

			pParameters->Set_Enabled("SIZE_SCALE"          , Value >= 0);
			pParameters->Set_Enabled("SIZE_RANGE"          , Value >= 0);
			pParameters->Set_Enabled("SIZE_DEFAULT"        , Value <  0);
		}

		if( pParameter->Cmp_Identifier("SIZT_SCALE") )
		{
			pParameters->Set_Enabled("SIZE_RANGE", pParameter->asInt() == 1);
		}

		if(	pParameter->Cmp_Identifier("IMAGE_FIELD") )
		{
			pParameter->Set_Children_Enabled(pParameter->asInt() < Get_Shapes()->Get_Field_Count());
		}
	}

	return( CWKSP_Shapes::On_Parameter_Changed(pParameters, pParameter, Flags) );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Shapes_Point::Get_Style_Size(int &minSize, int &maxSize, double &Minimum, double &Scale, wxString *pName)
{
	if( m_Size.Field >= 0 )
	{
		minSize = (int)(m_Size.Offset);
		maxSize = (int)(m_Size.Offset + m_Size.Scale * (Get_Shapes()->Get_Maximum(m_Size.Field) - m_Size.Minimum));
		Minimum = m_Size.Minimum;
		Scale   = m_Size.Scale;

		if( pName )
		{
			pName->Printf(Get_Shapes()->Get_Field_Name(m_Size.Field));
		}

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline void CWKSP_Shapes_Point::Draw_Initialize(CSG_Map_DC &dc_Map, int Flags)
{
	dc_Map.SetBrush(m_Brush);
	dc_Map.SetPen  (m_Pen  );

	m_Sel_Color_Fill = Get_Color_asWX(m_Parameters("SEL_COLOR_FILL")->asInt());

	m_Symbol.Type = m_Parameters("DISPLAY_SYMBOL_TYPE")->asInt();

	if( (Flags & LAYER_DRAW_FLAG_THUMBNAIL) != 0 )
	{
		if( m_Symbol.Type == SYMBOL_TYPE_Image
		||  m_Symbol.Type == SYMBOL_TYPE_Beachball )
		{
			m_Symbol.Type = 0;
		}
	}
}

//---------------------------------------------------------
inline bool CWKSP_Shapes_Point::Draw_Initialize(CSG_Map_DC &dc_Map, int &Size, CSG_Shape *pShape, int Flags)
{
	if( m_Brush.IsTransparent() && !m_bOutline && (Flags & LAYER_DRAW_FLAG_SELECTION) == 0 )
	{
		return( false ); // nothing to draw !
	}

	//-----------------------------------------------------
	if( (Flags & LAYER_DRAW_FLAG_SELECTION) != 0 )
	{
		dc_Map.SetBrush(wxBrush(m_Sel_Color_Fill, m_Brush.GetStyle()));
		dc_Map.SetPen(wxPen(m_Sel_Color, (Flags & LAYER_DRAW_FLAG_HIGHLIGHT) != 0 ? 2 : 0, wxPENSTYLE_SOLID));
	}
	else
	{
		int Color;

		if( !Get_Class_Color(pShape, Color) && !m_bNoData )
		{
			return( false );
		}

		if( m_Symbol.Type == SYMBOL_TYPE_Arrow && m_Arrow.Style != 2 )
		{
			if( !m_Brush.IsTransparent() )
			{
				wxBrush Brush(m_Brush); Brush.SetColour(Get_Color_asWX(Color)); dc_Map.SetBrush(Brush);
			}

			wxPen Pen(Get_Color_asWX(Color), 2); dc_Map.SetPen(Pen);
		}
		else
		{
			if( !m_Brush.IsTransparent() )
			{
				wxBrush Brush(m_Brush); Brush.SetColour(Get_Color_asWX(Color)); dc_Map.SetBrush(Brush);
			}

			if( !m_bOutline )
			{
				wxPen   Pen  (m_Pen  ); Pen  .SetColour(Get_Color_asWX(Color)); dc_Map.SetPen  (Pen  );
			}
		}
	}

	//-----------------------------------------------------
	m_Label_Point.Offset.x = m_Parameters("LABEL_OFFSET_X")->asDouble();
	m_Label_Point.Offset.y = m_Parameters("LABEL_OFFSET_Y")->asDouble();

	switch( m_Parameters("LABEL_FIELD_SIZE_TYPE")->asInt() )
	{
	default: m_Label_Point.Offset *= dc_Map.Scale   (); break;
	case  1: m_Label_Point.Offset *= dc_Map.World2DC(); break;
	}

	//-----------------------------------------------------
	double _Size = m_Size.Field < 0 ? m_Size.Offset : pShape->asDouble(m_Size.Field);

	if( m_Size.Adjust ) // adjust to size range
	{
		_Size = m_Size.Offset + m_Size.Scale * (_Size - m_Size.Minimum);
	}

	switch( m_Size.Unit )
	{
	default: _Size *= dc_Map.Scale   (); break;
	case  1: _Size *= dc_Map.World2DC(); break;
	}

	return( (Size = (int)(0.5 + _Size)) > 0 );
}

//---------------------------------------------------------
void CWKSP_Shapes_Point::Draw_Shape(CSG_Map_DC &dc_Map, CSG_Shape *pShape, int Flags)
{
	if( m_Size.Field >= 0 && pShape->is_NoData(m_Size.Field) )
	{
		return;
	}

	//-----------------------------------------------------
	int Size;

	if( !Draw_Initialize(dc_Map, Size, pShape, Flags) )
	{
		return;
	}

	TSG_Point_Int p(dc_Map.World2DC(pShape->Get_Point()));

	switch( m_Symbol.Type )
	{
	default:
		Draw_Symbol(dc_Map, p.x, p.y, Size);
		break;

	//-----------------------------------------------------
	case SYMBOL_TYPE_Image:
		{
			double d = (double)m_Symbol.Image.GetWidth() / (double)m_Symbol.Image.GetHeight();

			int sx = d >  1.0 ? Size : (int)(0.5 + Size * d);
			int sy = d <= 1.0 ? Size : (int)(0.5 + Size / d);

			if( sx > 0 && sy > 0 )
			{
				wxRect r(p.x - sx, p.y - sy, 2 * sx, 2 * sy);

				if( (Flags & LAYER_DRAW_FLAG_SELECTION) != 0 )
				{
					r.Inflate(1); dc_Map.DrawRectangle(r);
				}

				dc_Map.DrawBitmap(wxBitmap(m_Symbol.Image.Scale(2 * sx, 2 * sy)), p.x - sx, p.y - sy, true);
			}
		}
		break;

	//-----------------------------------------------------
	case SYMBOL_TYPE_Arrow:
		if( !pShape->is_NoData(m_Arrow.Field) )
		{
			_Arrow_Draw(dc_Map, p.x, p.y, Size, pShape->asDouble(m_Arrow.Field));
		}
		break;

	//-----------------------------------------------------
	case SYMBOL_TYPE_Beachball:
		if( !pShape->is_NoData(m_Beachball.Strike)
		&&  !pShape->is_NoData(m_Beachball.Dip   )
		&&  !pShape->is_NoData(m_Beachball.Rake  ) )
		{
			_Beachball_Draw(dc_Map, p.x, p.y, Size,
				pShape->asDouble(m_Beachball.Strike),
				pShape->asDouble(m_Beachball.Dip   ),
				pShape->asDouble(m_Beachball.Rake  )
			);
		}
		break;
	}

	//---------------------------------------------
	if( (Flags & LAYER_DRAW_FLAG_THUMBNAIL) == 0 && m_Image.Field >= 0 )
	{
		_Image_Draw(dc_Map, p.x, p.y, Size, pShape->asString(m_Image.Field));
	}

	//---------------------------------------------
	if( (Flags & LAYER_DRAW_FLAG_SELECTION) != 0 )
	{
		dc_Map.SetBrush(m_Brush);
		dc_Map.SetPen  (m_Pen  );
	}
}

//---------------------------------------------------------
void CWKSP_Shapes_Point::Draw_Label(CSG_Map_DC &dc_Map, CSG_Shape *pShape, const wxString &Label)
{
	TSG_Point_Int p(dc_Map.World2DC(pShape->Get_Point()));

	if( m_Label_Point.Offset.x > 0. )
	{
		if( (m_Label_Point.Align & TEXTALIGN_LEFT  ) != 0 ) p.x += m_Label_Point.Offset.x; else
		if( (m_Label_Point.Align & TEXTALIGN_RIGHT ) != 0 ) p.x -= m_Label_Point.Offset.x;
	}

	if( m_Label_Point.Offset.y > 0. )
	{
		if( (m_Label_Point.Align & TEXTALIGN_TOP   ) != 0 ) p.y += m_Label_Point.Offset.y; else
		if( (m_Label_Point.Align & TEXTALIGN_BOTTOM) != 0 ) p.y -= m_Label_Point.Offset.y;
	}

	double Angle = m_Label_Point.Angle_Field < 0 ? m_Label_Point.Angle : pShape->asDouble(m_Label_Point.Angle_Field);

	dc_Map.DrawText(m_Label_Point.Align, p.x, p.y, Angle, Label, m_Label.Effect, m_Label.Effect_Color, m_Label.Effect_Size);
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define DRAW_CIRCLE(d)         { int s = d == 1. ? Size : (int)((d) * Size);\
	dc.DrawCircle(x, y, s);\
}

//---------------------------------------------------------
#define DRAW_SQUARE(d)         { int s = d == 1. ? Size : (int)((d) * Size);\
	dc.DrawRectangle(x - s, y - s, 1 + 2 * s, 1 + 2 * s);\
}

//---------------------------------------------------------
#define DRAW_RHOMBUS(d)        { int s = d == 1. ? Size : (int)((d) * Size); wxPoint p[4];\
	p[0].y = p[2].y = y;\
	p[1].x = p[3].x = x;\
	p[0].x = x - s;\
	p[2].x = x + s;\
	p[1].y = y - s;\
	p[3].y = y + s;\
	dc.DrawPolygon(4, p);\
}

//---------------------------------------------------------
#define DRAW_TRIANGLE_UP(d)    { int s = d == 1. ? Size : (int)((d) * Size); wxPoint p[3];\
	p[0].y = p[1].y = y + (s / 2);\
	p[2].y = y - (s - 1);\
	p[0].x = x - (int)(0.8660254038 * s);\
	p[1].x = x + (int)(0.8660254038 * s);\
	p[2].x = x;\
	dc.DrawPolygon(3, p);\
}

//---------------------------------------------------------
#define DRAW_TRIANGLE_DOWN(d) { int s = d == 1. ? Size : (int)((d) * Size); wxPoint p[3];\
	p[0].y = p[1].y = y - (s / 2);\
	p[2].y = y + (s - 1);\
	p[0].x = x - (int)(0.8660254038 * s);\
	p[1].x = x + (int)(0.8660254038 * s);\
	p[2].x = x;\
	dc.DrawPolygon(3, p);\
}

//---------------------------------------------------------
void CWKSP_Shapes_Point::Draw_Symbol(wxDC &dc, int x, int y, int Size)
{
	switch( m_Symbol.Type )
	{
	default: DRAW_CIRCLE       (1.0)                         ; break; // circle
	case  1: DRAW_SQUARE       (1.0)                         ; break; // square
	case  2: DRAW_RHOMBUS      (1.0)                         ; break; // rhombus
	case  3: DRAW_TRIANGLE_UP  (1.0)                         ; break; // triangle up
	case  4: DRAW_TRIANGLE_DOWN(1.0)                         ; break; // triangle down
	case  5: DRAW_CIRCLE       (1.0); DRAW_SQUARE       (0.7); break; // square in circle
	case  6: DRAW_CIRCLE       (1.0); DRAW_RHOMBUS      (1.0); break; // rhombus in circle
	case  7: DRAW_CIRCLE       (1.0); DRAW_TRIANGLE_UP  (1.0); break; // triangle up in circle
	case  8: DRAW_CIRCLE       (1.0); DRAW_TRIANGLE_DOWN(1.0); break; // triangle down in circle
	case  9: DRAW_SQUARE       (1.0); DRAW_CIRCLE       (0.9); break; // circle in square
	case 10: DRAW_RHOMBUS      (1.0); DRAW_CIRCLE       (0.7); break; // circle in rhombus
	case 11: DRAW_TRIANGLE_UP  (1.0); DRAW_CIRCLE       (0.5); break; // circle in triangle up
	case 12: DRAW_TRIANGLE_DOWN(1.0); DRAW_CIRCLE       (0.5); break; // circle in triangle down
	}
}

//---------------------------------------------------------
void CWKSP_Shapes_Point::Draw_Symbol(CSG_Map_DC &dc, int x, int y, int Size)
{
	switch( m_Symbol.Type )
	{
	default: DRAW_CIRCLE       (1.0)                         ; break; // circle
	case  1: DRAW_SQUARE       (1.0)                         ; break; // square
	case  2: DRAW_RHOMBUS      (1.4)                         ; break; // rhombus
	case  3: DRAW_TRIANGLE_UP  (2.0)                         ; break; // triangle up
	case  4: DRAW_TRIANGLE_DOWN(2.0)                         ; break; // triangle down
	case  5: DRAW_CIRCLE       (1.0); DRAW_SQUARE       (0.7); break; // square in circle
	case  6: DRAW_CIRCLE       (1.0); DRAW_RHOMBUS      (1.0); break; // rhombus in circle
	case  7: DRAW_CIRCLE       (1.0); DRAW_TRIANGLE_UP  (1.0); break; // triangle up in circle
	case  8: DRAW_CIRCLE       (1.0); DRAW_TRIANGLE_DOWN(1.0); break; // triangle down in circle
	case  9: DRAW_SQUARE       (1.0); DRAW_CIRCLE       (1.0); break; // circle in square
	case 10: DRAW_RHOMBUS      (1.4); DRAW_CIRCLE       (1.0); break; // circle in rhombus
	case 11: DRAW_TRIANGLE_UP  (2.0); DRAW_CIRCLE       (1.0); break; // circle in triangle up
	case 12: DRAW_TRIANGLE_DOWN(2.0); DRAW_CIRCLE       (1.0); break; // circle in triangle down
	}
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Shapes_Point::_Image_Draw(CSG_Map_DC &dc_Map, int x, int y, int size, const wxString &file)
{
	wxFileName fn(file);

	if( fn.IsRelative() )
	{
		fn.MakeAbsolute(SG_File_Get_Path(Get_Shapes()->Get_File_Name()).c_str());
	}

	wxImage Image;

	if( fn.Exists() && Image.LoadFile(fn.GetFullPath()) )
	{
		double s = m_Image.Scale  * size;
		double o = m_Image.Offset * size;

		int   sx = Image.GetWidth ();
		int   sy = Image.GetHeight();

		double d = (double)sx / (double)sy;

		switch( m_Image.Fit )
		{
		default:
			sx = d >  1. ? s : (int)(0.5 + s * d);
			sy = d <= 1. ? s : (int)(0.5 + s / d);
			break;

		case  1: // width
			sx = s;
			sy = (int)(0.5 + s / d);
			break;

		case  2: // height
			sx = (int)(0.5 + s * d);
			sy = s;
			break;
		}

		if( sx > 1 && sy > 1 )
		{
			if( m_Image.Align & TEXTALIGN_LEFT    ) { x += o     ; } else
			if( m_Image.Align & TEXTALIGN_XCENTER ) { x -= sx / 2; } else
			if( m_Image.Align & TEXTALIGN_RIGHT   ) { x	-= o + sx; }

			if( m_Image.Align & TEXTALIGN_TOP     ) { y += o     ; } else
			if( m_Image.Align & TEXTALIGN_YCENTER ) { y -= sy / 2; } else
			if( m_Image.Align & TEXTALIGN_BOTTOM  ) { y -= o + sy; }

			dc_Map.DrawBitmap(wxBitmap(Image.Scale(sx, sy)), x, y, true);
		}
	}
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Shapes_Point::_Arrow_Draw(CSG_Map_DC &dc_Map, int x, int y, int size, double angle)
{
	static double Template[4][2] =
	{
		{  0.0, -1.0 },
		{  0.0,  1.0 },
		{  0.3, -0.5 },
		{ -0.3, -0.5 }
	};

	angle += m_Arrow.Offset;

	if( m_Arrow.Unit == 0 ) // degree
	{
		angle *= M_DEG_TO_RAD;
	}

	if( m_Arrow.Orientation == 0 ) // clockwise
	{
		angle *= -1.;
	}

	double sin_a = (1 + size) * sin(angle), cos_a = (1 + size) * cos(angle);

	#define ROTATE(i) {\
		x + (int)(Template[i][0] * cos_a + Template[i][1] * sin_a),\
		y + (int)(Template[i][1] * cos_a - Template[i][0] * sin_a) }

	int Arrow[4][2] =
	{
		ROTATE(0), ROTATE(1), ROTATE(2), ROTATE(3)
	};

	switch( m_Arrow.Style )
	{
	case  1: // arrow with center point
		dc_Map.DrawCircle(x, y, m_Arrow.Width);
		break;

	case  2: // arrow in circle
		dc_Map.DrawCircle(x, y, size);
		break;
	}

	dc_Map.DrawLine(Arrow[0][0], Arrow[0][1], Arrow[1][0], Arrow[1][1]);
	dc_Map.DrawLine(Arrow[0][0], Arrow[0][1], Arrow[2][0], Arrow[2][1]);
	dc_Map.DrawLine(Arrow[0][0], Arrow[0][1], Arrow[3][0], Arrow[3][1]);
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Shapes_Point::_Beachball_Draw(CSG_Map_DC &dc_Map, int x, int y, int size, double strike, double dip, double rake)
{
	const double dArc = 5.;

	strike *= M_DEG_TO_RAD; dip *= M_DEG_TO_RAD; rake *= M_DEG_TO_RAD;

	CSG_Shapes Plot(SHAPE_TYPE_Polygon);

	Plot.Add_Shape(); // 0, unit circle

	for(double a=0.; a<M_PI_360; a+=dArc*M_DEG_TO_RAD)
	{
		Plot.Get_Shape(0)->Add_Point(sin(a), cos(a));
	}

	CSG_Vector N(3);

	N[0] = 0; N[1] = 0; N[2] = 1;

	SG_VectorR3_Rotate(N, 1,    dip);
	SG_VectorR3_Rotate(N, 2, strike);

	_Beachball_Get_Plane(Plot.Add_Shape(), Plot.Get_Shape(0), N);	// 1, fault plane

	N[0] = 0; N[1] = -1; N[2] = 0;

	rake = fmod(rake, M_PI_360); if( rake < -M_PI_180 ) rake += M_PI_360; else if( rake > M_PI_180 ) rake -= M_PI_360;

	SG_VectorR3_Rotate(N, 2,  -rake);
	SG_VectorR3_Rotate(N, 1,    dip);
	SG_VectorR3_Rotate(N, 2, strike);

	_Beachball_Get_Plane(Plot.Add_Shape(), Plot.Get_Shape(0), N);	// 2, auxiliary plane

	//-----------------------------------------------------
	SG_Shape_Get_Intersection(Plot.Get_Shape(0), Plot.Get_Shape(1)->asPolygon(), Plot.Add_Shape());	// 3
	SG_Shape_Get_Difference  (Plot.Get_Shape(0), Plot.Get_Shape(1)->asPolygon(), Plot.Add_Shape());	// 4

	CSG_Shape_Polygon *pPlot[2];

	if( rake < 0. )
	{
		SG_Shape_Get_Difference  (Plot.Get_Shape(3), Plot.Get_Shape(2)->asPolygon(), pPlot[0] = (CSG_Shape_Polygon *)Plot.Add_Shape());
		SG_Shape_Get_Intersection(Plot.Get_Shape(4), Plot.Get_Shape(2)->asPolygon(), pPlot[1] = (CSG_Shape_Polygon *)Plot.Add_Shape());
	}
	else
	{
		SG_Shape_Get_Intersection(Plot.Get_Shape(3), Plot.Get_Shape(2)->asPolygon(), pPlot[0] = (CSG_Shape_Polygon *)Plot.Add_Shape());
		SG_Shape_Get_Difference  (Plot.Get_Shape(4), Plot.Get_Shape(2)->asPolygon(), pPlot[1] = (CSG_Shape_Polygon *)Plot.Add_Shape());
	}

	//-----------------------------------------------------
	dc_Map.DrawCircle(x, y, size);

	wxBrush b = dc_Map.Get_DC().GetBrush(); dc_Map.SetBrush(*wxWHITE_BRUSH);

	for(int i=0; i<2; i++)
	{
	//	SG_Shape_Get_Offset(pPlot[i], -0.01, dArc);
		_Beachball_Get_Scaled(pPlot[i], x,  y, size);

		if( pPlot[i]->Get_Area() > 1 )
		{
			wxPoint *Points = new wxPoint[pPlot[i]->Get_Point_Count(0)];

			for(int iPoint=0; iPoint<pPlot[i]->Get_Point_Count(0); iPoint++)
			{
				CSG_Point p(pPlot[i]->Get_Point(iPoint, 0));
				Points[iPoint].x = (int)(p.x + 0.5);
				Points[iPoint].y = (int)(p.y + 0.5);
			}

			dc_Map.DrawPolygon(pPlot[i]->Get_Point_Count(0), Points);

			delete[](Points);
		}
	}

	dc_Map.SetBrush(b);
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
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
