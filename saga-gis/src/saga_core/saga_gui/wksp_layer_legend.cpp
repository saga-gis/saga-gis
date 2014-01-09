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
#include "helper.h"
#include "dc_helper.h"

#include "wksp_shapes_point.h"
#include "wksp_shapes_line.h"
#include "wksp_shapes_polygon.h"
#include "wksp_tin.h"
#include "wksp_pointcloud.h"
#include "wksp_grid.h"

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
	m_oldPen	= dc.GetPen();
	m_oldBrush	= dc.GetBrush();
	m_oldFont	= dc.GetFont();

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
	default:	break;

	case WKSP_ITEM_Shapes:
		switch( ((CWKSP_Shapes *)m_pLayer)->Get_Shapes()->Get_Type() )
		{
		default:	break;

		case SHAPE_TYPE_Point:
		case SHAPE_TYPE_Points:	_Draw_Point		(dc, (CWKSP_Shapes_Point   *)m_pLayer);	break;
		case SHAPE_TYPE_Line:	_Draw_Line		(dc, (CWKSP_Shapes_Line    *)m_pLayer);	break;
		case SHAPE_TYPE_Polygon:_Draw_Polygon	(dc, (CWKSP_Shapes_Polygon *)m_pLayer);	break;
		}
		break;

	case WKSP_ITEM_TIN:			_Draw_TIN       (dc, (CWKSP_TIN            *)m_pLayer);	break;
	case WKSP_ITEM_PointCloud:	_Draw_PointCloud(dc, (CWKSP_PointCloud     *)m_pLayer);	break;
	case WKSP_ITEM_Grid:		_Draw_Grid      (dc, (CWKSP_Grid           *)m_pLayer);	break;
	}

	//-----------------------------------------------------
	dc.SetPen	(m_oldPen);
	dc.SetBrush	(m_oldBrush);
	dc.SetFont	(m_oldFont);

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
	switch( Style )
	{
	case FONT_TITLE:
		dc.SetFont(wxFont(FONT_SIZE_TITLE   , wxSWISS, wxNORMAL, wxBOLD, false));
		break;

	case FONT_SUBTITLE:
		dc.SetFont(wxFont(FONT_SIZE_SUBTITLE, wxSWISS, wxNORMAL, wxBOLD));
		break;

	case FONT_LABEL:	default:
		dc.SetFont(wxFont(FONT_SIZE_LABEL   , wxSWISS, wxITALIC, wxNORMAL));
		break;
	}
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
inline void CWKSP_Layer_Legend::_Draw_Box(wxDC &dc, int y, int dy, int Style, int iClass)
{
	if( iClass >= 0 )
	{
		if( (Style & BOX_STYLE_OUTL) == 0 )
		{
			wxPen	Pen		= dc.GetPen();
			Pen		.SetColour(Get_Color_asWX(m_pClassify->Get_Class_Color(iClass)));
			dc		.SetPen   (Pen);
		}

		if( (Style & BOX_STYLE_FILL) != 0 )
		{
			wxBrush	Brush	= dc.GetBrush();
			Brush	.SetColour(Get_Color_asWX(m_pClassify->Get_Class_Color(iClass)));
			dc		.SetBrush (Brush);
		}

		_Draw_Label(dc, y, m_pClassify->Get_Class_Name(iClass), TEXTALIGN_TOP);
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
void CWKSP_Layer_Legend::_Draw_Boxes(wxDC &dc, int y, int Style, double zFactor)
{
	switch( m_pClassify->Get_Mode() )
	{
	case CLASSIFY_GRADUATED:
	case CLASSIFY_SHADE:
		if( METRIC_HEIGHT < m_pClassify->Get_Class_Count() * BOX_HEIGHT )
		{
			_Draw_Continuum(dc, m_Position.y, zFactor);

			return;
		}
	}

	for(int iClass=m_pClassify->Get_Class_Count()-1; iClass>=0; iClass--, y+=BOX_HEIGHT)
	{
		_Draw_Box(dc, y, BOX_HEIGHT, Style, iClass);
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

		if( m_pClassify->Get_Mode() == CLASSIFY_UNIQUE )
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

		if( m_pClassify->Get_Mode() == CLASSIFY_UNIQUE )
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
	case CLASSIFY_METRIC:
	case CLASSIFY_SHADE:
		if( pLayer->Get_Grid()->Get_Unit() && *pLayer->Get_Grid()->Get_Unit() )
		{
			_Draw_Title(dc, FONT_SUBTITLE, wxString::Format(wxT("[%s]"), pLayer->Get_Grid()->Get_Unit()));
		}

	default:
		_Draw_Boxes(dc, m_Position.y, BOX_STYLE_RECT|BOX_STYLE_FILL|BOX_STYLE_OUTL, pLayer->Get_Grid()->Get_ZFactor());
		break;

	case CLASSIFY_RGB:
	case CLASSIFY_OVERLAY:
		_Draw_Grid_Image(dc, m_Position.y, pLayer->Get_Grid());
		break;
	}
}

//---------------------------------------------------------
void CWKSP_Layer_Legend::_Draw_Grid_Image(wxDC &dc, int ay, CSG_Grid *pGrid)
{
	int		x, y, nx, ny, Color;
	double	d, dx, dy;
	wxImage	img;

	//-----------------------------------------------------
	if( pGrid->Get_NX() > pGrid->Get_NY() )
	{
		nx	= m_dxBox;
		d	= (int)(pGrid->Get_NX() / nx);
		ny	= (int)(pGrid->Get_NY() / d);
	}
	else
	{
		ny	= m_dxBox;
		d	= (int)(pGrid->Get_NY() / ny);
		nx	= (int)(pGrid->Get_NX() / d);
	}

	img.Create(nx, ny);

	for(y=0, dy=0.0; y<ny; y++, dy+=d)
	{
		for(x=0, dx=0.0; x<nx; x++, dx+=d)
		{
			Color	= m_pClassify->Get_Class_Color_byValue(pGrid->asDouble((int)dx, (int)dy));
			img.SetRGB(x, ny - 1 - y, SG_GET_R(Color), SG_GET_G(Color), SG_GET_B(Color));
		}
	}

	dc.DrawBitmap(wxBitmap(img), m_xBox, ay, false);

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

		dc.SetPen(*wxBLACK_PEN);

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
	int		i, iy, jy;
	double	z;

	//-----------------------------------------------------
	for(i=0, z=y+METRIC_HEIGHT; i<m_pClassify->Get_Class_Count(); i++, z-=yToDC)
	{
		Draw_FillRect(dc, Get_Color_asWX(m_pClassify->Get_Class_Color(i)), m_xBox, (int)(z), m_xTick, (int)(z - yToDC));
	}

	Draw_Edge(dc, EDGE_STYLE_SIMPLE, m_xBox, y, m_xTick, y + METRIC_HEIGHT);

	//-----------------------------------------------------
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

			while( abs(iy - METRIC_POS_V(z - dz)) > 2 * dyFont )
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

			while( abs(iy - METRIC_POS_V(z + dz)) > 2 * dyFont )
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
	int			i, iy, jy, yTick, yText, sx, sy;
	double		z;
	wxString	s;

	yTick	= y + m_dxBox;
	yText	= y + m_dxBox + m_dxTick;

	//-----------------------------------------------------
	for(i=0, z=m_xBox; i<m_pClassify->Get_Class_Count(); i++, z+=yToDC)
	{
		Draw_FillRect(dc, Get_Color_asWX(m_pClassify->Get_Class_Color(i)), (int)(z), y, (int)(z + yToDC), yTick);
	}

	Draw_Edge(dc, EDGE_STYLE_SIMPLE, m_xBox, y, m_xBox + METRIC_HEIGHT, yTick);

	//-----------------------------------------------------
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
