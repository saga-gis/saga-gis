
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
//                 WKSP_Layer_Legend.cpp                 //
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
#include <saga_gdi/sgdi_helper.h>

#include "helper.h"

#include "wksp_shapes_point.h"
#include "wksp_shapes_line.h"
#include "wksp_shapes_polygon.h"
#include "wksp_tin.h"
#include "wksp_pointcloud.h"
#include "wksp_grid.h"
#include "wksp_grids.h"

#include "wksp_layer_classify.h"
#include "wksp_layer_legend.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define ZOOMED(x)			((int)(m_Zoom * (double)x))

//---------------------------------------------------------
#define SPACE_VERTICAL		ZOOMED(5)
#define BOX_WIDTH			ZOOMED(30)
#define BOX_HEIGHT			ZOOMED(12)
#define BOX_SPACE			ZOOMED(2)
#define TICK_WIDTH			ZOOMED(5)
#define METRIC_HEIGHT		ZOOMED(200)
#define SIZE_HEIGHT			ZOOMED(100)

//---------------------------------------------------------
#define FONT_SIZE_TITLE		ZOOMED(10)
#define FONT_SIZE_SUBTITLE	ZOOMED(8)
#define FONT_SIZE_LABEL		ZOOMED(8)

//---------------------------------------------------------
#define METRIC_POS_V(Value)	(  y    + METRIC_HEIGHT - (int)((double)METRIC_HEIGHT * m_pClassify->Get_MetricToRelative((Value) / zFactor)))
#define METRIC_POS_H(Value)	(m_xBox +                 (int)((double)METRIC_HEIGHT * m_pClassify->Get_MetricToRelative((Value) / zFactor)))
#define METRIC_GET_STRING(z, dz)		wxString::Format(wxT("%.*f"), dz >= 1.0 ? 0 : 1 + (int)fabs(log10(dz)), z)

//---------------------------------------------------------
#define BOX_STYLE_RECT		0x01
#define BOX_STYLE_LINE		0x02
#define BOX_STYLE_SYMB		0x04
#define BOX_STYLE_OUTL		0x08
#define BOX_STYLE_FILL		0x10


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
enum
{
	FONT_TITLE		= 0,
	FONT_SUBTITLE,
	FONT_LABEL
};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Layer_Legend::CWKSP_Layer_Legend(CWKSP_Layer *pLayer)
{
	m_pLayer		= pLayer;
	m_pClassify		= pLayer->Get_Classifier();

	m_Orientation	= LEGEND_VERTICAL;
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Layer_Legend::Set_Orientation(int Orientation)
{
	m_Orientation	= Orientation == LEGEND_VERTICAL ? LEGEND_VERTICAL : LEGEND_HORIZONTAL;
}

//---------------------------------------------------------
wxSize CWKSP_Layer_Legend::Get_Size(double Zoom, double Zoom_Map)
{
	wxMemoryDC	dc;

	Draw(dc, Zoom, Zoom_Map, wxPoint(0, 0), NULL);

	return( m_Size );
}

//---------------------------------------------------------
void CWKSP_Layer_Legend::Draw(wxDC &dc, double Zoom, double Zoom_Map, wxPoint Position, wxSize *pSize)
{
	//-----------------------------------------------------
	m_Zoom		= Zoom > 0.0 ? Zoom : 1.0;
	m_Position	= Position;
	m_Size		= wxSize(BOX_WIDTH, 0);

	m_Zoom_Map	= Zoom_Map;

	//-----------------------------------------------------
	m_oldPen	= dc.GetPen  ();
	m_oldBrush	= dc.GetBrush();
	m_oldFont	= dc.GetFont ();

	//-----------------------------------------------------
	m_xBox		= m_Position.x;
	m_dxBox		= BOX_WIDTH;
	m_xTick		= m_xBox	+ m_dxBox;
	m_dxTick	= TICK_WIDTH;
	m_xText		= m_xTick	+ m_dxTick;

	//-----------------------------------------------------
	_Draw_Title(dc, FONT_TITLE, m_pLayer->Get_Object()->Get_Name());

	//-----------------------------------------------------
	switch( m_pLayer->Get_Type() )
	{
	case WKSP_ITEM_Shapes      :
		switch( ((CWKSP_Shapes *)m_pLayer)->Get_Shapes()->Get_Type() )
		{
		case SHAPE_TYPE_Point  :
		case SHAPE_TYPE_Points : _Draw_Point     (dc, (CWKSP_Shapes_Point   *)m_pLayer); break;
		case SHAPE_TYPE_Line   : _Draw_Line      (dc, (CWKSP_Shapes_Line    *)m_pLayer); break;
		case SHAPE_TYPE_Polygon: _Draw_Polygon   (dc, (CWKSP_Shapes_Polygon *)m_pLayer); break;
		default                : break;
		}
		break;

	case WKSP_ITEM_TIN         : _Draw_TIN       (dc, (CWKSP_TIN            *)m_pLayer); break;
	case WKSP_ITEM_PointCloud  : _Draw_PointCloud(dc, (CWKSP_PointCloud     *)m_pLayer); break;
	case WKSP_ITEM_Grid        : _Draw_Grid      (dc, (CWKSP_Grid           *)m_pLayer); break;
	case WKSP_ITEM_Grids       : _Draw_Grids     (dc, (CWKSP_Grids          *)m_pLayer); break;
	default                    : break;
	}

	//-----------------------------------------------------
	dc.SetPen  (m_oldPen  );
	dc.SetBrush(m_oldBrush);
	dc.SetFont (m_oldFont );

	//-----------------------------------------------------
	if( pSize )
	{
		*pSize	= m_Size;
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline void CWKSP_Layer_Legend::_Set_Size(int xSet, int yAdd)
{
	if( m_Size.x < xSet )
	{
		m_Size.x	= xSet;
	}

	m_Position.y	+= yAdd;
	m_Size.y		+= yAdd;
}

//---------------------------------------------------------
inline void CWKSP_Layer_Legend::_Set_Font(wxDC &dc, int Style)
{
	wxFont	Font;

	switch( Style )
	{
	default:
	case FONT_LABEL   : Font.Create(FONT_SIZE_LABEL   , wxFONTFAMILY_SWISS, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_NORMAL); break;
	case FONT_SUBTITLE: Font.Create(FONT_SIZE_SUBTITLE, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD  ); break;
	case FONT_TITLE   : Font.Create(FONT_SIZE_TITLE   , wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD  ); break;
	}

	dc.SetFont(Font);
}

//---------------------------------------------------------
inline void CWKSP_Layer_Legend::_Draw_Title(wxDC &dc, int Style, wxString Text)
{
	if( Text.Length() > 0 )
	{
		wxCoord	dx_Text, dy_Text;

		_Set_Font(dc, Style);

		Draw_Text(dc, TEXTALIGN_TOPLEFT, m_Position.x, m_Position.y, Text);

		dc.GetTextExtent(Text, &dx_Text, &dy_Text);

		dy_Text			+= SPACE_VERTICAL;

		_Set_Size(dx_Text, dy_Text);
	}
}

//---------------------------------------------------------
inline void CWKSP_Layer_Legend::_Draw_Label(wxDC &dc, int y, wxString Text, int yAlign)
{
	wxCoord	dx_Text, dy_Text;

	switch( yAlign )
	{
	case TEXTALIGN_TOP:	default:
		yAlign	= TEXTALIGN_TOPLEFT;
		break;

	case TEXTALIGN_YCENTER:
		yAlign	= TEXTALIGN_CENTERLEFT;
		break;

	case TEXTALIGN_BOTTOM:
		yAlign	= TEXTALIGN_BOTTOMLEFT;
		break;
	}

	_Set_Font(dc, FONT_LABEL);

	Draw_Text(dc, yAlign, m_xText, y, Text);

	dc.GetTextExtent(Text, &dx_Text, &dy_Text);

	_Set_Size((m_xText - m_xBox) + dx_Text, 0);
}

//---------------------------------------------------------
inline void CWKSP_Layer_Legend::_Draw_Box(wxDC &dc, int y, int dy, int Style, int LineColor, int FillColor, wxString Text)
{
	if( (Style & BOX_STYLE_OUTL) != 0 )
	{
		wxPen	Pen		= dc.GetPen();
		Pen		.SetColour(Get_Color_asWX(LineColor));
		dc		.SetPen   (Pen);
	}

	if( (Style & BOX_STYLE_FILL) != 0 )
	{
		wxBrush	Brush	= dc.GetBrush();
		Brush	.SetColour(Get_Color_asWX(FillColor));
		dc		.SetBrush (Brush);
	}

	if( !Text.IsEmpty() )
	{
		_Draw_Label(dc, y, Text, TEXTALIGN_TOP);
	}

	_Set_Size(0, dy);

	dy	-= BOX_SPACE;

	//-----------------------------------------------------
	if( (Style & BOX_STYLE_RECT) != 0 )
	{
		dc.DrawRectangle(m_xBox, y, m_dxBox, dy);
	}

	if( (Style & BOX_STYLE_LINE) != 0 )
	{
		dc.DrawLine(m_xBox                  , y + dy / 2, m_xBox +     m_dxBox / 4, y);
		dc.DrawLine(m_xBox +     m_dxBox / 4, y         , m_xBox + 3 * m_dxBox / 4, y + dy);
		dc.DrawLine(m_xBox + 3 * m_dxBox / 4, y + dy    , m_xBox +     m_dxBox    , y + dy / 2);
	}
	
	if( (Style & BOX_STYLE_SYMB) != 0 )
	{
		((CWKSP_Shapes_Point *)m_pLayer)->Draw_Symbol(dc, m_xBox + m_dxBox / 2, y + dy / 2, dy / 2);
	}
}

//---------------------------------------------------------
inline void CWKSP_Layer_Legend::_Draw_Box(wxDC &dc, int y, int dy, int Style, int iClass)
{
	int	LineColor	= Get_Color_asInt(dc.GetTextForeground());
	int	FillColor	= Get_Color_asInt(dc.GetBrush().IsOk() ? dc.GetBrush().GetColour() : dc.GetTextBackground());

	wxString	Text;

	if( iClass >= 0 )
	{
		if( (Style & BOX_STYLE_OUTL) == 0 )
		{
			LineColor	= m_pClassify->Get_Class_Color(iClass);
		}

		if( (Style & BOX_STYLE_FILL) != 0 )
		{
			FillColor	= m_pClassify->Get_Class_Color(iClass);
		}

		Text	= m_pClassify->Get_Class_Name(iClass);
	}

	_Draw_Box(dc, y, dy, Style, LineColor, FillColor, Text);
}

//---------------------------------------------------------
void CWKSP_Layer_Legend::_Draw_Boxes(wxDC &dc, int y, int Style, double zFactor)
{
	switch( m_pClassify->Get_Mode() )
	{
	case CLASSIFY_GRADUATED:
	case CLASSIFY_SHADE    :
		_Draw_Continuum(dc, m_Position.y, zFactor);
		break;

	default:
		for(int iClass=m_pClassify->Get_Class_Count()-1; iClass>=0; iClass--, y+=BOX_HEIGHT)
		{
			_Draw_Box(dc, y, BOX_HEIGHT, Style, iClass);
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
void CWKSP_Layer_Legend::_Draw_Point(wxDC &dc, CWKSP_Shapes_Point *pLayer)
{
	int			size_Min, size_Max;
	double		size_Min_Value, size_dValue;
	wxString	size_Name;

	//-----------------------------------------------------
	dc.SetBrush(pLayer->Get_Def_Brush());
	dc.SetPen  (pLayer->Get_Def_Pen  ());

	if( pLayer->Get_Style_Size(size_Min, size_Max, size_Min_Value, size_dValue, &size_Name) )
	{
		_Draw_Title(dc, FONT_SUBTITLE, size_Name);

		int		iClass, nClasses, dy, y;
		double	iSize, dySize;

		nClasses	= (int)((double)SIZE_HEIGHT / (double)BOX_HEIGHT);
		dySize		= (double)(size_Max - size_Min) / (double)nClasses;

		for(iClass=nClasses, iSize=size_Max; iClass>=0; iClass--, iSize-=dySize)
		{
			dy	= (int)(2.0 * m_Zoom_Map * iSize) + BOX_SPACE;
			y	= m_Position.y + SIZE_HEIGHT - iClass * BOX_HEIGHT;

			pLayer->Draw_Symbol(dc, m_xBox + m_dxBox / 2, y + dy / 2, dy / 2);

			_Draw_Label(dc, y, wxString::Format(wxT("%f"), size_Min_Value + (iSize - size_Min) / size_dValue), TEXTALIGN_TOP);
		}

		_Set_Size(0, SIZE_HEIGHT + dc.GetFont().GetPointSize());

		if( m_pClassify->Get_Mode() == CLASSIFY_SINGLE )
		{
			return;
		}
	}

	//-----------------------------------------------------
	_Draw_Title(dc, FONT_SUBTITLE, pLayer->Get_Name_Attribute());
	_Draw_Boxes(dc, m_Position.y, pLayer->Get_Outline() ? BOX_STYLE_RECT|BOX_STYLE_FILL|BOX_STYLE_OUTL : BOX_STYLE_RECT|BOX_STYLE_FILL);
}

//---------------------------------------------------------
void CWKSP_Layer_Legend::_Draw_Line(wxDC &dc, CWKSP_Shapes_Line *pLayer)
{
	//-----------------------------------------------------
	int			size_Min, size_Max;
	double		size_Min_Value, size_dValue;
	wxString	size_Name;

	if( pLayer->Get_Style_Size(size_Min, size_Max, size_Min_Value, size_dValue, &size_Name) )
	{
		_Draw_Title(dc, FONT_SUBTITLE, size_Name);

		wxPen Pen	= pLayer->Get_Def_Pen();

		for(int iSize=size_Min; iSize<=size_Max; iSize++)
		{
			Pen.SetWidth(iSize);
			dc.SetPen(Pen);

			int	y	= m_Position.y;

			_Draw_Box	(dc, y, BOX_HEIGHT, BOX_STYLE_LINE|BOX_STYLE_OUTL, -1);
			_Draw_Label	(dc, y, wxString::Format(wxT("%f"), size_Min_Value + (iSize - size_Min) / size_dValue), TEXTALIGN_TOP);
		}

		if( m_pClassify->Get_Mode() == CLASSIFY_SINGLE )
		{
			return;
		}
	}

	//-----------------------------------------------------
	dc.SetPen  (pLayer->Get_Def_Pen());

	_Draw_Title(dc, FONT_SUBTITLE, pLayer->Get_Name_Attribute());
	_Draw_Boxes(dc, m_Position.y, BOX_STYLE_LINE);
}

//---------------------------------------------------------
void CWKSP_Layer_Legend::_Draw_Polygon(wxDC &dc, CWKSP_Shapes_Polygon *pLayer)
{
	_Draw_Title(dc, FONT_SUBTITLE, pLayer->Get_Name_Attribute());
	_Draw_Boxes(dc, m_Position.y, pLayer->Get_Outline() ? BOX_STYLE_RECT|BOX_STYLE_FILL|BOX_STYLE_OUTL : BOX_STYLE_RECT|BOX_STYLE_FILL);
}

//---------------------------------------------------------
void CWKSP_Layer_Legend::_Draw_TIN(wxDC &dc, CWKSP_TIN *pLayer)
{
	_Draw_Title(dc, FONT_SUBTITLE, pLayer->Get_Name_Attribute());
	_Draw_Boxes(dc, m_Position.y, BOX_STYLE_RECT|BOX_STYLE_FILL|BOX_STYLE_OUTL);
}

//---------------------------------------------------------
void CWKSP_Layer_Legend::_Draw_PointCloud(wxDC &dc, CWKSP_PointCloud *pLayer)
{
	_Draw_Title(dc, FONT_SUBTITLE, pLayer->Get_Name_Attribute());
	_Draw_Boxes(dc, m_Position.y, BOX_STYLE_RECT|BOX_STYLE_FILL|BOX_STYLE_OUTL);
}

//---------------------------------------------------------
void CWKSP_Layer_Legend::_Draw_Grid(wxDC &dc, CWKSP_Grid *pLayer)
{
	switch( m_pClassify->Get_Mode() )
	{
	case CLASSIFY_GRADUATED:
	case CLASSIFY_DISCRETE:
	case CLASSIFY_SHADE:
		if( !pLayer->Get_Grid()->Get_Unit().is_Empty() )
		{
			_Draw_Title(dc, FONT_SUBTITLE, wxString::Format("[%s]", pLayer->Get_Grid()->Get_Unit().c_str()));
		}

	default:
		_Draw_Boxes(dc, m_Position.y, BOX_STYLE_RECT|BOX_STYLE_FILL|BOX_STYLE_OUTL);
		break;

	case CLASSIFY_RGB:
	case CLASSIFY_OVERLAY:
		_Draw_Layer_Image(dc, m_Position.y, pLayer);
		break;
	}
}

//---------------------------------------------------------
void CWKSP_Layer_Legend::_Draw_Grids(wxDC &dc, CWKSP_Grids *pLayer)
{
	switch( m_pClassify->Get_Mode() )
	{
	case CLASSIFY_GRADUATED:
	case CLASSIFY_DISCRETE:
	case CLASSIFY_SHADE:
		if( *pLayer->Get_Grids()->Get_Unit() )
		{
			_Draw_Title(dc, FONT_SUBTITLE, wxString::Format("[%s]", pLayer->Get_Grids()->Get_Unit()));
		}

	default:
		_Draw_Boxes(dc, m_Position.y, BOX_STYLE_RECT|BOX_STYLE_FILL|BOX_STYLE_OUTL);
		break;

	case CLASSIFY_RGB:
	case CLASSIFY_OVERLAY:
	//	_Draw_Layer_Image(dc, m_Position.y, pLayer);
		_Draw_Box(dc, m_Position.y, BOX_HEIGHT, BOX_STYLE_RECT|BOX_STYLE_FILL|BOX_STYLE_OUTL, 0, SG_GET_RGB(255,   0,   0), pLayer->Get_Grid(0)->Get_Name());
		_Draw_Box(dc, m_Position.y, BOX_HEIGHT, BOX_STYLE_RECT|BOX_STYLE_FILL|BOX_STYLE_OUTL, 0, SG_GET_RGB(  0, 255,   0), pLayer->Get_Grid(1)->Get_Name());
		_Draw_Box(dc, m_Position.y, BOX_HEIGHT, BOX_STYLE_RECT|BOX_STYLE_FILL|BOX_STYLE_OUTL, 0, SG_GET_RGB(  0,   0, 255), pLayer->Get_Grid(2)->Get_Name());
		break;
	}
}

//---------------------------------------------------------
void CWKSP_Layer_Legend::_Draw_Layer_Image(wxDC &dc, int ay, CWKSP_Layer *pLayer)
{
	int		nx, ny;

	if( pLayer->Get_Extent().Get_XRange() > pLayer->Get_Extent().Get_YRange() )
	{
		nx	= m_dxBox;
		ny	= (int)(pLayer->Get_Extent().Get_YRange() / (pLayer->Get_Extent().Get_XRange() / nx));
	}
	else
	{
		ny	= m_dxBox;
		nx	= (int)(pLayer->Get_Extent().Get_XRange() / (pLayer->Get_Extent().Get_YRange() / ny));
	}

	dc.DrawBitmap(pLayer->Get_Thumbnail(nx, ny), m_xBox, ay, false);

	Draw_Edge(dc, EDGE_STYLE_SIMPLE, m_xBox, ay, m_xBox + nx, ay + ny);

	//-----------------------------------------------------
	_Set_Size(0, ny);

	ny	+= BOX_SPACE;
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Layer_Legend::_Draw_Continuum(wxDC &dc, int y, double zFactor)
{
	//-----------------------------------------------------
	double	zMin	= m_pClassify->Get_RelativeToMetric(0.0);
	double	zMax	= m_pClassify->Get_RelativeToMetric(1.0);

	//-----------------------------------------------------
	if( zMin >= zMax )
	{
		if( m_pLayer->Get_Value_Range() > 0.0 )
		{
			_Draw_Box(dc, y, BOX_HEIGHT, BOX_STYLE_RECT|BOX_STYLE_FILL|BOX_STYLE_OUTL, m_pClassify->Get_Class_Count() - 1);
		}

		_Draw_Box(dc, y + BOX_HEIGHT, BOX_HEIGHT, BOX_STYLE_RECT|BOX_STYLE_FILL|BOX_STYLE_OUTL, 0);
	}

	//-----------------------------------------------------
	else
	{
		int		dxFont, dyFont;
		double	yToDC, dz;

		dc.SetPen(dc.GetTextForeground());

		_Set_Font(dc, FONT_LABEL);
		dc.GetTextExtent(wxString::Format(wxT("01234567")), &dxFont, &dyFont);

		zMin	*= zFactor;
		zMax	*= zFactor;
		yToDC	= METRIC_HEIGHT / (zMax - zMin);
		dz		= pow(10.0, floor(log10(zMax - zMin)) - 1.0);

		while( yToDC * dz < dyFont )	dz	*= 2.0;

		yToDC	= METRIC_HEIGHT / (double)m_pClassify->Get_Class_Count();

		if( m_Orientation == LEGEND_VERTICAL )
		{
			_Draw_Continuum_V(dc, y, yToDC, zMin, zMax, zFactor, dz, dyFont);
		}
		else
		{
			_Draw_Continuum_H(dc, y, yToDC, zMin, zMax, zFactor, dz, dyFont);
		}
	}
}

//---------------------------------------------------------
void CWKSP_Layer_Legend::_Draw_Continuum_V(wxDC &dc, int y, double yToDC, double zMin, double zMax, double zFactor, double dz, int dyFont)
{
	CSG_Colors Colors(*m_pClassify->Get_Metric_Colors());

	Colors.Set_Count(METRIC_HEIGHT);

	wxPen oldPen(dc.GetPen());

	for(int i=0, iy=y+METRIC_HEIGHT; i<Colors.Get_Count(); i++, iy--)
	{
		dc.SetPen(wxPen(Get_Color_asWX(Colors[i])));

		dc.DrawLine(m_xBox, iy, m_xTick, iy);
	}

	dc.SetPen(oldPen);

	Draw_Edge(dc, EDGE_STYLE_SIMPLE, m_xBox, y, m_xTick, y + METRIC_HEIGHT);

	//-----------------------------------------------------
	int iy, jy; double z;

	switch( m_pClassify->Get_Metric_Mode() )
	{
	case METRIC_MODE_NORMAL:	default:
		z	= dz * floor(zMin / dz);
		if( z < zMin )	z	+= dz;

		for(; z<=zMax; z+=dz)
		{
			iy	= METRIC_POS_V(z);
			dc.DrawLine(m_xTick, iy, m_xText, iy);

			_Draw_Label(dc, iy, METRIC_GET_STRING(z, dz), TEXTALIGN_YCENTER);
		}
		break;

	case METRIC_MODE_LOGUP:
		jy	= METRIC_POS_V(zMax) + (1 + dyFont);
		z	= dz * floor(zMax / dz);
		if( z > zMax )	z	-= dz;

		for(; z>=zMin; z-=dz)
		{
			iy	= METRIC_POS_V(z);
			dc.DrawLine(m_xTick, iy, m_xText, iy);

			if( abs(jy - iy) >= (int)(0.75 * dyFont) )
			{
				jy	= iy;
				_Draw_Label(dc, iy, METRIC_GET_STRING(z, dz), TEXTALIGN_YCENTER);
			}

			while( dz >= 10. && abs(iy - METRIC_POS_V(z - dz)) > 2 * dyFont )
				dz	*= 0.1;
		}
		break;

	case METRIC_MODE_LOGDOWN:
		jy	= METRIC_POS_V(zMin) - (1 + dyFont);
		z	= dz * floor(zMin / dz);
		if( z < zMin )	z	+= dz;

		for(; z<=zMax; z+=dz)
		{
			iy	= METRIC_POS_V(z);
			dc.DrawLine(m_xTick, iy, m_xText, iy);

			if( abs(jy - iy) >= (int)(0.75 * dyFont) )
			{
				jy	= iy;
				_Draw_Label(dc, iy, METRIC_GET_STRING(z, dz), TEXTALIGN_YCENTER);
			}

			while( dz >= 10. && abs(iy - METRIC_POS_V(z + dz)) > 2 * dyFont )
				dz	*= 0.1;
		}
		break;
	}

	//-----------------------------------------------------
	_Set_Size(0, METRIC_HEIGHT + dc.GetFont().GetPointSize());
}

//---------------------------------------------------------
void CWKSP_Layer_Legend::_Draw_Continuum_H(wxDC &dc, int y, double yToDC, double zMin, double zMax, double zFactor, double dz, int dyFont)
{
	CSG_Colors Colors(*m_pClassify->Get_Metric_Colors());

	Colors.Set_Count(METRIC_HEIGHT);

	wxPen oldPen(dc.GetPen());

	int yTick = y + m_dxBox;
	int yText = y + m_dxBox + m_dxTick;

	for(int i=0, ix=m_xBox; i<Colors.Get_Count(); i++, ix++)
	{
		dc.SetPen(wxPen(Get_Color_asWX(Colors[i])));

		dc.DrawLine(ix, y, ix, yTick);
	}

	dc.SetPen(oldPen);

	Draw_Edge(dc, EDGE_STYLE_SIMPLE, m_xBox, y, m_xBox + METRIC_HEIGHT, yTick);

	//-----------------------------------------------------
	int iy, jy, sx, sy; double z; wxString s;

	switch( m_pClassify->Get_Metric_Mode() )
	{
	case METRIC_MODE_NORMAL:	default:
		jy	= METRIC_POS_H(zMin) - METRIC_HEIGHT;
		z	= dz * floor(zMin / dz);
		if( z < zMin )	z	+= dz;

		for(; z<=zMax; z+=dz)
		{
			iy	= METRIC_POS_H(z);
			dc.DrawLine(iy, yTick, iy, yText);

			s	= METRIC_GET_STRING(z, dz);
			dc.GetTextExtent(s, &sx, &sy);

			if( abs(jy - iy) >= (int)(1.25 * sx) )
			{
				jy	= iy;
				Draw_Text(dc, TEXTALIGN_TOPCENTER, iy, yText, s);
			}
		}
		break;

	case METRIC_MODE_LOGUP:
		jy	= METRIC_POS_H(zMax) + METRIC_HEIGHT;
		z	= dz * floor(zMax / dz);
		if( z > zMax )	z	-= dz;

		for(; z>=zMin; z-=dz)
		{
			iy	= METRIC_POS_H(z);
			dc.DrawLine(iy, yTick, iy, yText);

			s	= METRIC_GET_STRING(z, dz);
			dc.GetTextExtent(s, &sx, &sy);

			if( abs(jy - iy) >= (int)(1.25 * sx) )
			{
				jy	= iy;
				Draw_Text(dc, TEXTALIGN_TOPCENTER, iy, yText, s);
			}

			while( abs(iy - METRIC_POS_H(z - dz)) > 2 * sx )
				dz	*= 0.1;
		}
		break;

	case METRIC_MODE_LOGDOWN:
		jy	= METRIC_POS_H(zMin) - METRIC_HEIGHT;
		z	= dz * floor(zMin / dz);
		if( z < zMin )	z	+= dz;

		for(; z<=zMax; z+=dz)
		{
			iy	= METRIC_POS_H(z);
			dc.DrawLine(iy, yTick, iy, yText);

			s	= METRIC_GET_STRING(z, dz);
			dc.GetTextExtent(s, &sx, &sy);

			if( abs(jy - iy) >= (int)(1.25 * sx) )
			{
				jy	= iy;
				Draw_Text(dc, TEXTALIGN_TOPCENTER, iy, yText, s);
			}

			while( abs(iy - METRIC_POS_H(z + dz)) > 2 * sx )
				dz	*= 0.1;
		}
		break;
	}

	//-----------------------------------------------------
	_Set_Size(METRIC_HEIGHT + m_dxBox, m_dxBox + m_dxTick + dyFont);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
