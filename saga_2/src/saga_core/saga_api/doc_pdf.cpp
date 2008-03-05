
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//           Application Programming Interface           //
//                                                       //
//                  Library: SAGA_API                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                     doc_pdf.cpp                       //
//                                                       //
//                 Copyright (C) 2005 by                 //
//              Victor Olaya, Olaf Conrad                //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'.                              //
//                                                       //
// This library is free software; you can redistribute   //
// it and/or modify it under the terms of the GNU Lesser //
// General Public License as published by the Free       //
// Software Foundation, version 2.1 of the License.      //
//                                                       //
// This library is distributed in the hope that it will  //
// be useful, but WITHOUT ANY WARRANTY; without even the //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU Lesser General Public //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU Lesser     //
// General Public License along with this program; if    //
// not, write to the Free Software Foundation, Inc.,     //
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
#ifndef _SAGA_DONOTUSE_HARU


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <hpdf.h>

#include "doc_pdf.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define PDF_PAGE_WIDTH_A4		595.276
#define PDF_PAGE_HEIGHT_A4		841.89

#define PDF_PAGE_HEIGHT_A4_M	0.297

#define PDF_PAGE_WIDTH_A3		 PDF_PAGE_HEIGHT_A4
#define PDF_PAGE_HEIGHT_A3		(PDF_PAGE_HEIGHT_A4 * sqrt(2.0))

#define PDF_METER_TO_POINT		(PDF_PAGE_HEIGHT_A4 / PDF_PAGE_HEIGHT_A4_M)
#define PDF_POINT_TO_METER		(PDF_PAGE_HEIGHT_A4_M / PDF_PAGE_HEIGHT_A4)

//---------------------------------------------------------
#define PDF_GET_R(c)			(float)(SG_GET_R(c) / 255.0)
#define PDF_GET_G(c)			(float)(SG_GET_G(c) / 255.0)
#define PDF_GET_B(c)			(float)(SG_GET_B(c) / 255.0)


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Doc_PDF::CSG_Doc_PDF(void)
{
	m_pPDF	= NULL;

	Layout_Set_Box_Space(5, false);

	Open();
}

//---------------------------------------------------------
CSG_Doc_PDF::CSG_Doc_PDF(TSG_PDF_Page_Size Size, int Orientation, const SG_Char *Title)
{
	m_pPDF	= NULL;

	Layout_Set_Box_Space(5, false);

	Open(Size, Orientation, Title);
}

//---------------------------------------------------------
CSG_Doc_PDF::~CSG_Doc_PDF(void)
{
	Close();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
const SG_Char * CSG_Doc_PDF::Get_Version(void)
{
	static CSG_String	s	= CSG_String::Format(SG_T("Haru Free PDF Library, Version %s"), HPDF_VERSION_TEXT);

	return( s );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Doc_PDF::Open(const SG_Char *Title)
{
	return( Open(PDF_PAGE_SIZE_A4, PDF_PAGE_ORIENTATION_PORTRAIT, Title) );
}

//---------------------------------------------------------
bool CSG_Doc_PDF::Open(TSG_PDF_Page_Size Size, int Orientation, const SG_Char *Title)
{
	if( Close() )
	{
		m_pPDF	= HPDF_New(NULL, NULL);

		m_pOutline_Last_Level_0 = NULL;
		m_pOutline_Last_Level_1 = NULL;
		m_pOutline_Last_Level_2 = NULL;

		Set_Size_Page(Size, Orientation);

		if( Title && *Title )
		{
			Add_Page_Title(Title);
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Doc_PDF::Close(void)
{
	if( m_pPDF )
	{
		try
		{
			HPDF_Free(m_pPDF);
		}
		catch(...)
		{
			return( false );
		}
	}

	m_pPDF		= NULL;
	m_pPage		= NULL;
	m_nPages	= 0;

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Doc_PDF::Save(const SG_Char *FileName)
{
	if( m_pPDF && m_nPages > 0 && SG_Dir_Create(SG_File_Get_Path(FileName)) )
	{
		try
		{
			return( HPDF_SaveToFile(m_pPDF, SG_STR_SGTOMB(FileName)) == HPDF_OK );
		}
		catch(...)
		{}
	}

	SG_UI_Msg_Add_Error(LNG("[ERR] Could not save PDF file."));

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double CSG_Doc_PDF::Get_Page_To_Meter(void)
{
	return( PDF_POINT_TO_METER );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
struct _HPDF_Dict_Rec * CSG_Doc_PDF::_Get_Font(TSG_PDF_Font_Type Font)
{
	switch( Font )
	{
	default:
	case PDF_FONT_Helvetica:				return( HPDF_GetFont(m_pPDF, "Helvetica"			, NULL) );
	case PDF_FONT_Helvetica_Bold:			return( HPDF_GetFont(m_pPDF, "Helvetica-Bold"		, NULL) );
	case PDF_FONT_Helvetica_Oblique:		return( HPDF_GetFont(m_pPDF, "Helvetica-Oblique"	, NULL) );
	case PDF_FONT_Helvetica_BoldOblique:	return( HPDF_GetFont(m_pPDF, "Helvetica-BoldOblique", NULL) );
	case PDF_FONT_Times_Roman:				return( HPDF_GetFont(m_pPDF, "Times-Roman"			, NULL) );
	case PDF_FONT_Times_Bold:				return( HPDF_GetFont(m_pPDF, "Times-Bold"			, NULL) );
	case PDF_FONT_Times_Italic:				return( HPDF_GetFont(m_pPDF, "Times-Italic"			, NULL) );
	case PDF_FONT_Times_BoldItalic:			return( HPDF_GetFont(m_pPDF, "Times-BoldItalic"		, NULL) );
	case PDF_FONT_Courier:					return( HPDF_GetFont(m_pPDF, "Courier"				, NULL) );
	case PDF_FONT_Courier_Bold:				return( HPDF_GetFont(m_pPDF, "Courier-Bold"			, NULL) );
	case PDF_FONT_Courier_Oblique:			return( HPDF_GetFont(m_pPDF, "Courier-Oblique"		, NULL) );
	case PDF_FONT_Courier_BoldOblique:		return( HPDF_GetFont(m_pPDF, "Courier-BoldOblique"	, NULL) );
	case PDF_FONT_Symbol:					return( HPDF_GetFont(m_pPDF, "Symbol"				, NULL) );
	case PDF_FONT_ZapfDingbats:				return( HPDF_GetFont(m_pPDF, "ZapfDingbats"			, NULL) );
	}

	return( NULL );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Doc_PDF::Layout_Add_Box(double xMin_Percent, double yMin_Percent, double xMax_Percent, double yMax_Percent, const SG_Char *ID)
{
	if( m_Boxes_Percent.Add(xMin_Percent, yMin_Percent, xMax_Percent, yMax_Percent) )
	{
		m_Boxes.Add();
		m_Boxes_ID.Add(ID);

		_Layout_Set_Box(m_Boxes.Get_Count() - 1);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Doc_PDF::Layout_Add_Box(const CSG_Rect &Box_Percent, const SG_Char *ID)
{
	return( Layout_Add_Box(Box_Percent.Get_XMin(), Box_Percent.Get_YMin(), Box_Percent.Get_XMax(), Box_Percent.Get_YMax(), ID) );
}

//---------------------------------------------------------
const CSG_Rect & CSG_Doc_PDF::Layout_Get_Box(const SG_Char *ID)
{
	for(int i=0; i<m_Boxes_ID.Get_Count(); i++)
	{
		if( !m_Boxes_ID[i].Cmp(ID) )
		{
			return( Layout_Get_Box(i) );
		}
	}

	return( Layout_Get_Box(-1) );
}

//---------------------------------------------------------
void CSG_Doc_PDF::Layout_Set_Box_Space(double Space, bool bPercent)
{
	m_Boxes_Space_bPercent	= bPercent;
	m_Boxes_Space			= Space;
}

//---------------------------------------------------------
void CSG_Doc_PDF::_Layout_Set_Boxes(void)
{
	for(int i=0; i<m_Boxes.Get_Count(); i++)
	{
		_Layout_Set_Box(i);
	}
}

//---------------------------------------------------------
void CSG_Doc_PDF::_Layout_Set_Box(int iBox)
{
	if( iBox >= 0 && iBox < m_Boxes.Get_Count() && iBox < m_Boxes_Percent.Get_Count() )
	{
		m_Boxes[iBox].Assign(
			m_Size_Margins.Get_XMin() + 0.01 * m_Boxes_Percent[iBox].Get_XMin() * m_Size_Margins.Get_XRange(),
			m_Size_Margins.Get_YMax() - 0.01 * m_Boxes_Percent[iBox].Get_YMax() * m_Size_Margins.Get_YRange(),
			m_Size_Margins.Get_XMin() + 0.01 * m_Boxes_Percent[iBox].Get_XMax() * m_Size_Margins.Get_XRange(),
			m_Size_Margins.Get_YMax() - 0.01 * m_Boxes_Percent[iBox].Get_YMin() * m_Size_Margins.Get_YRange()
		);

		m_Boxes[iBox].Deflate(m_Boxes_Space, m_Boxes_Space_bPercent);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Doc_PDF::_Fit_Rectangle(double &x, double &y, double &dx, double &dy, double XToY_Ratio, bool bShrink)
{
	if( XToY_Ratio != 0.0 && dx != 0.0 && dy != 0.0 )
	{
		if( XToY_Ratio > dx / dy )
		{
			if( bShrink )
			{
				XToY_Ratio	 = dx / XToY_Ratio;
				y			+= (dy - XToY_Ratio) / 2.0;
				dy			 = XToY_Ratio;
			}
			else
			{
				XToY_Ratio	 = dy * XToY_Ratio;
				x			-= (dx - XToY_Ratio) / 2.0;
				dx			 = XToY_Ratio;
			}
		}
		else
		{
			if( bShrink )
			{
				XToY_Ratio	 = dy * XToY_Ratio;
				x			+= (dx - XToY_Ratio) / 2.0;
				dx			 = XToY_Ratio;
			}
			else
			{
				XToY_Ratio	 = dx / XToY_Ratio;
				y			-= (dy - XToY_Ratio) / 2.0;
				dy			 = XToY_Ratio;
			}
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Doc_PDF::_Fit_Rectangle(CSG_Rect &r, double XToY_Ratio, bool bShrink)
{
	double	x, y, dx, dy;

	x	= r.Get_XMin();
	y	= r.Get_YMin();
	dx	= r.Get_XRange();
	dy	= r.Get_YRange();

	if( _Fit_Rectangle(x, y, dx, dy, XToY_Ratio, bShrink) )
	{
		r.Assign(x, y, x + dx, y + dy);

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
bool CSG_Doc_PDF::_Add_Outline_Item(const SG_Char *Title, struct _HPDF_Dict_Rec *pPage, TSG_PDF_Title_Level Level)
{
	if( m_pPDF && pPage )
	{
		HPDF_Outline	pOutlineItem	= NULL;

		switch( Level )
		{
		case PDF_TITLE:
			pOutlineItem	= m_pOutline_Last_Level_0	= HPDF_CreateOutline(m_pPDF, NULL, SG_STR_SGTOMB(Title), NULL);
			break;

		case PDF_TITLE_01:
			if( m_pOutline_Last_Level_0 )
			{
				pOutlineItem	= m_pOutline_Last_Level_1	= HPDF_CreateOutline(m_pPDF, m_pOutline_Last_Level_0, SG_STR_SGTOMB(Title), NULL);
			}
			break;

		case PDF_TITLE_02:
			if( m_pOutline_Last_Level_1 )
			{
				pOutlineItem	= m_pOutline_Last_Level_2	= HPDF_CreateOutline(m_pPDF, m_pOutline_Last_Level_1, SG_STR_SGTOMB(Title), NULL);
			}
			break;

		case PDF_TITLE_NONE:
			if( m_pOutline_Last_Level_2 )
			{
				pOutlineItem	= HPDF_CreateOutline(m_pPDF, m_pOutline_Last_Level_2, SG_STR_SGTOMB(Title), NULL);
			}
			break;

		}

		if( pOutlineItem )
		{
			if( pPage )
			{
				struct _HPDF_Array_Rec	*pDestination	= HPDF_Page_CreateDestination(pPage);

				HPDF_Destination_SetFit		(pDestination);
				HPDF_Outline_SetDestination	(pOutlineItem, pDestination);
			}

			return( true );
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Doc_PDF::Add_Outline_Item(const SG_Char *Title)
{
	return( _Add_Outline_Item(Title, m_pPage, _Get_Lowest_Outline_Level()) );
}

//---------------------------------------------------------
TSG_PDF_Title_Level CSG_Doc_PDF::_Get_Lowest_Outline_Level(void)
{
	if( m_pOutline_Last_Level_2 )
	{
		return( PDF_TITLE_NONE );
	}
	else if( m_pOutline_Last_Level_1 )
	{
		return( PDF_TITLE_02 );
	}
	else if( m_pOutline_Last_Level_0 )
	{
		return( PDF_TITLE_01 );
	}

	return( PDF_TITLE );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Doc_PDF::Add_Page(void)
{
	return( Add_Page(m_Size_Paper.Get_XRange(), m_Size_Paper.Get_YRange()) );
}

//---------------------------------------------------------
bool CSG_Doc_PDF::Add_Page(TSG_PDF_Page_Size Size, int Orientation)
{
	if( Add_Page() )
	{
		return( Set_Size_Page(Size, Orientation) );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Doc_PDF::Add_Page(double Width, double Height)
{
	if( m_pPDF )
	{
		m_nPages++;

		m_pPage		= HPDF_AddPage(m_pPDF);

		Set_Size_Page(Width, Height);

		HPDF_Page_SetRGBStroke	(m_pPage, 0.0, 0.0, 0.0);
		HPDF_Page_SetRGBFill	(m_pPage, 1.0, 1.0, 1.0);

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
bool CSG_Doc_PDF::Set_Size_Page(TSG_PDF_Page_Size Size, int Orientation)
{
	double	Width, Height;

	switch( Size )
	{
	case PDF_PAGE_SIZE_A4:	default:
		Width	= PDF_PAGE_WIDTH_A4;
		Height	= PDF_PAGE_HEIGHT_A4;
		break;

	case PDF_PAGE_SIZE_A3:
		Width	= PDF_PAGE_WIDTH_A3;
		Height	= PDF_PAGE_HEIGHT_A3;
		break;
	}

	//-----------------------------------------------------
	if( Orientation == PDF_PAGE_ORIENTATION_LANDSCAPE )
	{
		double	a;

		a		= Width;
		Width	= Height;
		Height	= a;
	}

	return( Set_Size_Page(Width, Height) );
}

//---------------------------------------------------------
bool CSG_Doc_PDF::Set_Size_Page(double Width, double Height)
{
	if( Width > 0.0 && Height > 0.0 )
	{
		m_Size_Paper.Assign(0.0, 0.0, Width, Height);

		m_Size_Margins	= m_Size_Paper;
		m_Size_Margins.Deflate(10.0, false);

		_Layout_Set_Boxes();

		if( m_pPage )
		{
			HPDF_Page_SetWidth	(m_pPage, (float)m_Size_Paper.Get_XRange());
			HPDF_Page_SetHeight	(m_pPage, (float)m_Size_Paper.Get_YRange());
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
bool CSG_Doc_PDF::Add_Page_Title(const SG_Char *Title, TSG_PDF_Title_Level Level, TSG_PDF_Page_Size Size, int Orientation)
{
	bool	bLine, bDestination, bPage;
	int		FontSize;

	//-----------------------------------------------------
	if( Add_Page() )
	{
		if( m_nPages % 2 - 1 )
		{
			Add_Page();
		}

		if( Size != PDF_PAGE_SIZE_PREVIOUS || Orientation != PDF_PAGE_ORIENTATION_PREVIOUS )
		{
			Set_Size_Page(Size, Orientation);
		}

		//-------------------------------------------------
		switch( Level )
		{
		case PDF_TITLE:
			FontSize		= 26;
			bLine			= true;
			bDestination	= true;
			bPage			= true;
			break;

		case PDF_TITLE_01:
			FontSize		= 22;
			bLine			= true;
			bDestination	= false;
			bPage			= true;
			break;

		case PDF_TITLE_02:
			FontSize		= 20;
			bLine			= false;
			bDestination	= false;
			bPage			= false;
			break;
		}

		_Add_Outline_Item(Title, m_pPage, Level);

		//-------------------------------------------------
		Draw_Text(Get_Margins().Get_XCenter(), Get_Margins().Get_YCenter(), Title, FontSize, PDF_STYLE_TEXT_ALIGN_H_CENTER, 0.0, SG_GET_RGB(0, 0, 0));

		//-------------------------------------------------
		if( bLine )
		{
			double	y	= Get_Margins().Get_YCenter() - 25.0;

			Draw_Line(Get_Margins().Get_XMin(), y, Get_Margins().Get_XMax(), y, 5, SG_GET_RGB(0, 0, 0), PDF_STYLE_LINE_END_ROUND);
		}

		if( bPage )
		{
			Add_Page();
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
bool CSG_Doc_PDF::_Set_Style_FillStroke(int Style, int Fill_Color, int Line_Color, int Line_Width)
{
	if( Is_Ready_To_Draw() )
	{
		if( Style & PDF_STYLE_POLYGON_STROKE )
		{
			if( Style & PDF_STYLE_LINE_END_ROUND )
			{
				HPDF_Page_SetLineCap(m_pPage, HPDF_ROUND_END);
			}
			else if( Style & PDF_STYLE_LINE_END_SQUARE )
			{
				HPDF_Page_SetLineCap(m_pPage, HPDF_PROJECTING_SCUARE_END);
			}
			else // if( Style & PDF_STYLE_LINE_END_BUTT )
			{
				HPDF_Page_SetLineCap(m_pPage, HPDF_BUTT_END);
			}

			if( Style & PDF_STYLE_LINE_JOIN_ROUND )
			{
				HPDF_Page_SetLineJoin(m_pPage, HPDF_ROUND_JOIN);
			}
			else if( Style & PDF_STYLE_LINE_JOIN_BEVEL )
			{
				HPDF_Page_SetLineJoin(m_pPage, HPDF_BEVEL_JOIN);
			}
			else // if( Style & PDF_STYLE_LINE_JOIN_MITER )
			{
				HPDF_Page_SetLineJoin(m_pPage, HPDF_MITER_JOIN);
			}

			HPDF_Page_SetRGBStroke	(m_pPage, PDF_GET_R(Line_Color), PDF_GET_G(Line_Color), PDF_GET_B(Line_Color));

			HPDF_Page_SetLineWidth	(m_pPage, (float)Line_Width);
		}

		//-------------------------------------------------
		if( Style & PDF_STYLE_POLYGON_FILL )
		{
			HPDF_Page_SetRGBFill(m_pPage, PDF_GET_R(Fill_Color), PDF_GET_G(Fill_Color), PDF_GET_B(Fill_Color));
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Doc_PDF::Draw_Point(double x, double y, double Width, int Style, int Fill_Color, int Line_Color, int Line_Width)
{
	if( Is_Ready_To_Draw() )
	{
		Width	/= 2.0;

		//-------------------------------------------------
	//	if( Style & PDF_STYLE_POINT_CIRCLE )
		{
		}
	//	else // if( Style & PDF_STYLE_POINT_SQUARE )
		{
			return( Draw_Rectangle(x - Width, y - Width, x + Width, y + Width, Style, Fill_Color, Line_Color, Line_Width) );
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Doc_PDF::Draw_Line(double xa, double ya, double xb, double yb, int Width, int Color, int Style)
{
	CSG_Points	Points;

	Points.Add(xa, ya);
	Points.Add(xb, yb);

	return( Draw_Line(Points, Width, Color, Style) );
}

bool CSG_Doc_PDF::Draw_Line(CSG_Points &Points, int Width, int Color, int Style)
{
	if( Points.Get_Count() > 1 && _Set_Style_FillStroke(Style|PDF_STYLE_POLYGON_STROKE, 0, Color, Width) )
	{
		HPDF_Page_MoveTo(m_pPage, (float)Points[0].x, (float)Points[0].y);

		for(int i=1; i<Points.Get_Count(); i++)
		{
			HPDF_Page_LineTo(m_pPage, (float)Points[i].x, (float)Points[i].y);
		}

		//-------------------------------------------------
		HPDF_Page_Stroke(m_pPage);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Doc_PDF::Draw_Rectangle(double xa, double ya, double xb, double yb, int Style, int Fill_Color, int Line_Color, int Line_Width)
{
	CSG_Points	Points;

	Points.Add(xa, ya);
	Points.Add(xb, ya);
	Points.Add(xb, yb);
	Points.Add(xa, yb);

	return( Draw_Polygon(Points, Style, Fill_Color, Line_Color, Line_Width) );
}

bool CSG_Doc_PDF::Draw_Rectangle(const CSG_Rect &r, int Style, int Fill_Color, int Line_Color, int Line_Width)
{
	return( Draw_Rectangle(r.Get_XMin(), r.Get_YMin(), r.Get_XMax(), r.Get_YMax(), Style, Fill_Color, Line_Color, Line_Width) );
}

//---------------------------------------------------------
bool CSG_Doc_PDF::Draw_Polygon(CSG_Points &Points, int Style, int Fill_Color, int Line_Color, int Line_Width)
{
	if( Points.Get_Count() > 2 && _Set_Style_FillStroke(Style, Fill_Color, Line_Color, Line_Width) )
	{
		HPDF_Page_MoveTo(m_pPage, (float)Points[0].x, (float)Points[0].y);

		for(int i=1; i<Points.Get_Count(); i++)
		{
			HPDF_Page_LineTo(m_pPage, (float)Points[i].x, (float)Points[i].y);
		}

		HPDF_Page_ClosePath(m_pPage);

		//-------------------------------------------------
		if( Style & PDF_STYLE_POLYGON_FILL && Style & PDF_STYLE_POLYGON_STROKE )
		{
			HPDF_Page_EofillStroke(m_pPage);
		}
		else if( Style & PDF_STYLE_POLYGON_FILL )
		{
			HPDF_Page_Eofill(m_pPage);
		}
		else // if( Style & PDF_STYLE_POLYGON_STROKE )
		{
			HPDF_Page_Stroke(m_pPage);
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
bool CSG_Doc_PDF::Draw_Text(double x, double y, CSG_Strings &Text, int Size, int Style, double Angle, int Color, TSG_PDF_Font_Type Font)
{
	if( Is_Ready_To_Draw() && Text.Get_Count() > 0 )
	{
		for(int i=0; i<Text.Get_Count(); i++, y-=Size)
		{
			_Draw_Text(x, y, Text[i], Size, Style, Angle, Color, Font);
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Doc_PDF::Draw_Text(double x, double y, const SG_Char *Text, int Size, int Style, double Angle, int Color, TSG_PDF_Font_Type Font)
{
	int				n;
	CSG_String		String(Text);
	CSG_Strings	Strings;

	while( (n = String.Find('\n')) >= 0 )
	{
		Strings.Add(String.Left(n));
		String.Remove(0, n + 1);
	}

	if( Strings.Get_Count() > 0 )
	{
		if( String.Length() > 0 )
		{
			Strings.Add(String);
		}

		return( Draw_Text(x, y, Strings, Size, Style, Angle, Color, Font) );
	}

	return( _Draw_Text(x, y, Text, Size, Style, Angle, Color, Font) );
}

//---------------------------------------------------------
bool CSG_Doc_PDF::_Draw_Text(double x, double y, const SG_Char *Text, int Size, int Style, double Angle, int Color, TSG_PDF_Font_Type Font)
{
	float	Width, Height;

	if( m_pPage && Text && *Text != '\0' )
	{
		double	ax, ay, bx, by;

		HPDF_Page_SetFontAndSize(m_pPage, _Get_Font(Font), (float)Size);

		HPDF_Page_SetTextRenderingMode(m_pPage, HPDF_FILL_THEN_STROKE);
		HPDF_Page_SetLineWidth	(m_pPage, 0);
		HPDF_Page_SetRGBStroke	(m_pPage, PDF_GET_R(Color), PDF_GET_G(Color), PDF_GET_B(Color));
		HPDF_Page_SetRGBFill	(m_pPage, PDF_GET_R(Color), PDF_GET_G(Color), PDF_GET_B(Color));

		Width	= HPDF_Page_TextWidth(m_pPage, SG_STR_SGTOMB(Text)) * (float)cos(Angle);
		Height	= HPDF_Page_TextWidth(m_pPage, SG_STR_SGTOMB(Text)) * (float)sin(Angle) + Size;

		//-------------------------------------------------
		if( Style & PDF_STYLE_TEXT_ALIGN_H_CENTER )
		{
			ax	= x - HPDF_Page_TextWidth(m_pPage, SG_STR_SGTOMB(Text)) / 2.0;
		}
		else if( Style & PDF_STYLE_TEXT_ALIGN_H_RIGHT )
		{
			ax	= x - HPDF_Page_TextWidth(m_pPage, SG_STR_SGTOMB(Text));
		}
		else
		{
			ax	= x;
		}

		bx	= ax + HPDF_Page_TextWidth(m_pPage, SG_STR_SGTOMB(Text));

		if( Style & PDF_STYLE_TEXT_ALIGN_V_CENTER )
		{
			ay	= y - Size / 2.0;
		}
		else if( Style & PDF_STYLE_TEXT_ALIGN_V_TOP )
		{
			ay	= y - Size;
		}
		else
		{
			ay	= y;
		}

		by	= ay;

		//-------------------------------------------------
		HPDF_Page_BeginText(m_pPage);

		if( Angle != 0.0 )
		{
			double	dSin, dCos, dx, dy;

			dSin	= sin(-Angle);
			dCos	= cos(-Angle),

			dx	= ax - x,
			dy	= ay - y;
			ax	= x + dCos * dx + dSin * dy;
			ay	= y - dSin * dx + dCos * dy;

			dx	= bx - x,
			dy	= by - y;
			bx	= x + dCos * dx + dSin * dy;
			by	= y - dSin * dx + dCos * dy;

			dSin	= sin(Angle);
			dCos	= cos(Angle),

			HPDF_Page_SetTextMatrix(m_pPage, (float)dCos, (float)dSin, -(float)dSin, (float)dCos, (float)ax, (float)ay);
		}
		else
		{
			HPDF_Page_MoveTextPos(m_pPage, (float)ax, (float)ay);
		}

		HPDF_Page_ShowText(m_pPage, SG_STR_SGTOMB(Text));
		HPDF_Page_EndText(m_pPage);

		//-------------------------------------------------
		if( Style & PDF_STYLE_TEXT_UNDERLINE )
		{
			double	dx, dy;

			dy	=  0.2 * Size * (ax - bx) / SG_Get_Distance(ax, ay, bx, by);
			dx	= -0.2 * Size * (ay - by) / SG_Get_Distance(ax, ay, bx, by);

			Draw_Line(ax + dx, ay + dy, bx + dx, by + dy, (int)(0.1 * Size), Color);
		}

		if( Style & PDF_STYLE_TEXT_STROKE )
		{
			double	dx, dy;

			dy	= -0.4 * Size * (ax - bx) / SG_Get_Distance(ax, ay, bx, by);
			dx	=  0.4 * Size * (ay - by) / SG_Get_Distance(ax, ay, bx, by);

			Draw_Line(ax + dx, ay + dy, bx + dx, by + dy, (int)(0.1 * Size), Color);
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
bool CSG_Doc_PDF::Draw_Image(double x, double y, double dx, double dy, const SG_Char *FileName)
{
	bool		bKeepRatio	= true;
	double		nx, ny;
	HPDF_Image	pImage		= NULL;

	//-----------------------------------------------------
	if( m_pPage && SG_File_Exists(FileName) && dx > 0.0 && dy > 0.0 )
	{
		if( SG_File_Cmp_Extension(FileName, SG_T("png")) )
		{
	try	{	pImage	= HPDF_LoadPngImageFromFile (m_pPDF, SG_STR_SGTOMB(FileName));	}	catch(...)	{}
		}
		else if( SG_File_Cmp_Extension(FileName, SG_T("jpg")) )
		{
	try	{	pImage	= HPDF_LoadJpegImageFromFile(m_pPDF, SG_STR_SGTOMB(FileName));	}	catch(...)	{}
		}
	}

	//-----------------------------------------------------
	if( pImage && (nx = HPDF_Image_GetWidth(pImage)) > 0 && (ny = HPDF_Image_GetHeight(pImage)) > 0 )
	{
		if( bKeepRatio )
		{
			_Fit_Rectangle(x, y, dx, dy, nx / ny, true);
		}

		HPDF_Page_DrawImage(m_pPage, pImage, (float)x, (float)y, (float)dx, (float)dy);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Doc_PDF::Draw_Image(const CSG_Rect &r, const SG_Char *FileName)
{
	return( Draw_Image(r.Get_XMin(), r.Get_YMin(), r.Get_XRange(), r.Get_YRange(), FileName) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Doc_PDF::Draw_Graticule(const CSG_Rect &r, const CSG_Rect &rWorld, int Size)
{
	if( Is_Ready_To_Draw() )
	{
		CSG_Rect	rRuler, rFrame(r);

		rFrame.Inflate(Size, false);

		Draw_Rectangle(rFrame, PDF_STYLE_POLYGON_STROKE);
		Draw_Rectangle(r     , PDF_STYLE_POLYGON_STROKE);

		rRuler.Assign(r.Get_XMin(), r.Get_YMax(), r.Get_XMax(), rFrame.Get_YMax());
		_Draw_Ruler(rRuler, rWorld.Get_XMin(), rWorld.Get_XMax()  , true , true , false);

		rRuler.Assign(r.Get_XMin(), r.Get_YMin(), r.Get_XMax(), rFrame.Get_YMin());
		_Draw_Ruler(rRuler, 0.0              , rWorld.Get_XRange(), true , true , true);

		rRuler.Assign(r.Get_XMin(), r.Get_YMin(), rFrame.Get_XMin(), r.Get_YMax());
		_Draw_Ruler(rRuler, rWorld.Get_YMin(), rWorld.Get_YMax()  , false, false, false);

		rRuler.Assign(r.Get_XMax(), r.Get_YMin(), rFrame.Get_XMax(), r.Get_YMax());
		_Draw_Ruler(rRuler, 0.0              , rWorld.Get_YRange(), false, false, true);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
#define RULER_TEXT_SPACE	4

//---------------------------------------------------------
bool CSG_Doc_PDF::_Draw_Ruler(const CSG_Rect &r, double zMin, double zMax, bool bHorizontal, bool bAscendent, bool bTickAtTop)
{
	int			Decimals, FontSize;
	double		xOff, yOff, Width, Height, Height_Tick, z, dz, zToDC, zDC, zPos, tPos;
	CSG_String	s;

	//-----------------------------------------------------
	Width	= bHorizontal ? r.Get_XRange() : r.Get_YRange();
	Height	= bHorizontal ? r.Get_YRange() : r.Get_XRange();

	if( Is_Ready_To_Draw() && zMin < zMax && Width > 0 && Height > 0 )
	{
		xOff		= r.Get_XMin();
		yOff		= r.Get_YMax();

		FontSize	= (int)(0.45 * (double)Height);
		HPDF_Page_SetFontAndSize(m_pPage, _Get_Font(PDF_FONT_DEFAULT), (float)FontSize);

		Height_Tick	= (int)(0.3 * (double)Height);

		//-------------------------------------------------
		zToDC		= (double)Width / (zMax - zMin);

		dz			= pow(10.0, floor(log10(zMax - zMin)) - 1.0);
		Decimals	= dz >= 1.0 ? 0 : (int)fabs(log10(dz));

		s.Printf(SG_T("%.*f"), Decimals, zMax);
		zDC			= HPDF_Page_TextWidth(m_pPage, SG_STR_SGTOMB(s));
		while( zToDC * dz < zDC + RULER_TEXT_SPACE )
		{
			dz	*= 2;
		}

		//-------------------------------------------------
		z			= dz * floor(zMin / dz);
		if( z < zMin )	z	+= dz;

		for(; z<=zMax; z+=dz)
		{
			s.Printf(SG_T("%.*f"), Decimals, z);

			zDC	= bAscendent ? zToDC * (z - zMin) : Width - zToDC * (z - zMin);

			if( bHorizontal )
			{
				zPos	= xOff + zDC;

				if( bTickAtTop )
				{
					tPos	= yOff;
					Draw_Line(zPos, tPos - Height_Tick, zPos, yOff);
					Draw_Text(zPos, tPos - Height_Tick, s, FontSize, PDF_STYLE_TEXT_ALIGN_H_CENTER|PDF_STYLE_TEXT_ALIGN_V_TOP, 0.0);
				}
				else
				{
					tPos	= yOff - Height;
					Draw_Line(zPos, tPos + Height_Tick, zPos, tPos);
					Draw_Text(zPos, tPos + Height_Tick, s, FontSize, PDF_STYLE_TEXT_ALIGN_H_CENTER|PDF_STYLE_TEXT_ALIGN_V_BOTTOM, 0.0);
				}
			}
			else
			{
				zPos	= yOff - zDC;

				if( bTickAtTop )
				{
					tPos	= xOff;
					Draw_Line(tPos + Height_Tick, zPos, tPos, zPos);
					Draw_Text(tPos + Height_Tick, zPos, s, FontSize, PDF_STYLE_TEXT_ALIGN_H_CENTER|PDF_STYLE_TEXT_ALIGN_V_TOP, M_DEG_TO_RAD * 90.0);
				}
				else
				{
					tPos	= xOff + Height;
					Draw_Line(tPos - Height_Tick, zPos, tPos, zPos);
					Draw_Text(tPos - Height_Tick, zPos, s, FontSize, PDF_STYLE_TEXT_ALIGN_H_CENTER|PDF_STYLE_TEXT_ALIGN_V_BOTTOM, M_DEG_TO_RAD * 90.0);
				}
			}
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
bool CSG_Doc_PDF::_Draw_Table(CSG_Rect r, CSG_Table *pTable, int iRecord, int nRecords, double CellHeight, double HeaderHeightRel)
{
	int				iField, dyFont, dyFont_Hdr;
	double			xPos, yPos, dxPos, dyPos, dyPos_Hdr, xSpace, ySpace, ySpace_Hdr;
	bool			bAddHeader;
	CSG_Table_Record	*pRecord;

	if( pTable && pTable->is_Valid() && iRecord < pTable->Get_Record_Count() )
	{
	//	Draw_Text(r.Get_XMin(), r.Get_YMax(), pTable->Get_Name(), 16);	// the table's name should not be drawn here...

		dxPos		= r.Get_XRange() / pTable->Get_Field_Count();

		if( HeaderHeightRel < 1.0 )
			HeaderHeightRel	= 1.0;

		dyPos		= CellHeight > 0.0 ? CellHeight : r.Get_YRange() / (nRecords + HeaderHeightRel);
		dyPos_Hdr	= dyPos * HeaderHeightRel;

		dyFont		= (int)(0.75 * dyPos);
		dyFont_Hdr	= (int)(0.75 * dyPos_Hdr);

		ySpace		= 0.1 * dyPos;
		ySpace_Hdr	= 0.1 * dyPos_Hdr;
		xSpace		= ySpace;

		nRecords	+= iRecord;

		if( nRecords > pTable->Get_Record_Count() )
		{
			nRecords	= pTable->Get_Record_Count();
		}

		//-------------------------------------------------
		for(yPos=r.Get_YMax(), bAddHeader=true; iRecord<nRecords; iRecord++, yPos-=dyPos)
		{
			if( yPos < r.Get_YMin() - dyPos )
			{
				Add_Page();

				yPos		= r.Get_YMax();
				bAddHeader	= true;
			}

			if( bAddHeader )
			{
				for(iField=0, xPos=r.Get_XMin(); iField<pTable->Get_Field_Count(); iField++, xPos+=dxPos)
				{
					Draw_Rectangle(xPos, yPos, xPos + dxPos, yPos - dyPos_Hdr, PDF_STYLE_POLYGON_FILLSTROKE, SG_COLOR_GREY_LIGHT, SG_COLOR_BLACK, 0);
					Draw_Text(xPos + xSpace, yPos - ySpace_Hdr, pTable->Get_Field_Name(iField), dyFont_Hdr, PDF_STYLE_TEXT_ALIGN_H_LEFT|PDF_STYLE_TEXT_ALIGN_V_TOP);
				}

				yPos		-= dyPos_Hdr;
				bAddHeader	= false;
			}

			//---------------------------------------------
			pRecord	= pTable->Get_Record(iRecord);

			for(iField=0, xPos=r.Get_XMin(); iField<pTable->Get_Field_Count(); iField++, xPos+=dxPos)
			{
				Draw_Rectangle(xPos, yPos, xPos + dxPos, yPos - dyPos, PDF_STYLE_POLYGON_STROKE, SG_COLOR_WHITE, SG_COLOR_BLACK, 0);
				Draw_Text(xPos + xSpace, yPos - ySpace, pRecord->asString(iField), dyFont, PDF_STYLE_TEXT_ALIGN_H_LEFT|PDF_STYLE_TEXT_ALIGN_V_TOP);
			}
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Doc_PDF::Draw_Table(const CSG_Rect &r, CSG_Table *pTable, double CellHeight, double HeaderHeightRel)
{
	if( pTable && pTable->is_Valid() )
	{
		return( _Draw_Table(r, pTable, 0, pTable->Get_Record_Count(), CellHeight, HeaderHeightRel) );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Doc_PDF::Draw_Table(const CSG_Rect &r, CSG_Table *pTable, int nColumns, double CellHeight, double HeaderHeightRel)
{
	if( pTable && pTable->is_Valid() && nColumns > 1 )
	{
		int			nRecords;
		double		dx, dxSpace;
		CSG_Rect	rColumn(r), rTable(r);

		dxSpace		= 4;
		rTable.Inflate(dxSpace, 0.0, false);
		dx			= rTable.Get_XRange() / (double)nColumns;
		nRecords	= pTable->Get_Record_Count() / nColumns;

		for(int i=0, iRecord=0; i<nColumns; i++, iRecord+=nRecords)
		{
			rColumn.m_rect.xMin	= rTable.Get_XMin() + (i + 0) * dx + dxSpace;
			rColumn.m_rect.xMax	= rTable.Get_XMin() + (i + 1) * dx - dxSpace;

			_Draw_Table(rColumn, pTable, iRecord, nRecords, CellHeight, HeaderHeightRel);
		}

		return( true );
	}

	return( Draw_Table(r, pTable, CellHeight, HeaderHeightRel) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Doc_PDF::_Draw_Shape(CSG_Rect r, CSG_Shape *pShape, double xMin, double yMin, double World2PDF, int Style, int Fill_Color, int Line_Color, int Line_Width, int Point_Width)
{
	if( pShape && pShape->is_Valid() )
	{
		int			iPart, iPoint;
		TSG_Point	Point;
		CSG_Points	Points;

		for(iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
		{
			Points.Clear();

			for(iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
			{
				Point	= pShape->Get_Point(iPoint, iPart);
				Point.x	= r.Get_XMin() + World2PDF * (Point.x - xMin);
				Point.y	= r.Get_YMin() + World2PDF * (Point.y - yMin);
				Points.Add(Point.x, Point.y);
			}

			switch( pShape->Get_Type() )
			{
			case SHAPE_TYPE_Point:
			case SHAPE_TYPE_Points:
				for(iPoint=0; iPoint<Points.Get_Count(); iPoint++)
				{
					Draw_Point(Points[iPoint].x, Points[iPoint].y, Point_Width, Style, Fill_Color, Line_Color, Line_Width);
				}
				break;

			case SHAPE_TYPE_Line:
				Draw_Line(Points, Line_Width, Line_Color, Style);
				break;

			case SHAPE_TYPE_Polygon:
				if( ((CSG_Shape_Polygon *)pShape)->is_Lake(iPart) )
				{
					Draw_Polygon(Points, PDF_STYLE_POLYGON_FILLSTROKE, SG_COLOR_WHITE, Line_Color, Line_Width);
				}
				else
				{
					Draw_Polygon(Points, Style, Fill_Color, Line_Color, Line_Width);
				}
				break;
			}
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Doc_PDF::Draw_Shape(const CSG_Rect &r, CSG_Shape *pShape, int Style, int Fill_Color, int Line_Color, int Line_Width, CSG_Rect *prWorld)
{
	double		World2PDF;
	CSG_Rect	rWorld, rFit;

	if( pShape && pShape->is_Valid() && r.Get_XRange() > 0.0 && r.Get_YRange() > 0.0 )
	{
		rWorld	= prWorld ? *prWorld : pShape->Get_Extent();

		if( rWorld.Get_XRange() > 0.0 && rWorld.Get_YRange() > 0.0 )
		{
			_Fit_Rectangle(rFit = r, rWorld.Get_XRange() / rWorld.Get_YRange(), true);

			World2PDF	= rFit.Get_XRange() / rWorld.Get_XRange();

			_Draw_Shape(rFit, pShape, rWorld.Get_XMin(), rWorld.Get_YMin(), World2PDF, Style, Fill_Color, Line_Color, Line_Width, Line_Width);

			return( true );
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Doc_PDF::Draw_Shapes(const CSG_Rect &r, CSG_Shapes *pShapes, int Style, int Fill_Color, int Line_Color, int Line_Width, CSG_Rect *prWorld)
{
	double		World2PDF;
	CSG_Rect	rWorld, rFit;

	if( pShapes && pShapes->is_Valid() && r.Get_XRange() > 0.0 && r.Get_YRange() > 0.0 )
	{
		rWorld	= prWorld ? *prWorld : pShapes->Get_Extent();

		if( rWorld.Get_XRange() > 0.0 && rWorld.Get_YRange() > 0.0 )
		{
			_Fit_Rectangle(rFit = r, rWorld.Get_XRange() / rWorld.Get_YRange(), true);

			World2PDF	= rFit.Get_XRange() / rWorld.Get_XRange();

			for(int iShape=0; iShape<pShapes->Get_Count(); iShape++)
			{
				_Draw_Shape(rFit, pShapes->Get_Shape(iShape), rWorld.Get_XMin(), rWorld.Get_YMin(), World2PDF, Style, Fill_Color, Line_Color, Line_Width, Line_Width);
			}

			return( true );
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Doc_PDF::Draw_Grid(const CSG_Rect &r, CSG_Grid *pGrid, const CSG_Colors &Colors, double zMin, double zMax, int Style, CSG_Rect *prWorld)
{
	double		x, y, z, d, xFit, yFit, dFit;
	CSG_Rect	rWorld, rFit;

	if( Is_Ready_To_Draw() && pGrid && pGrid->is_Valid() && pGrid->Get_ZRange() > 0.0 )
	{
		rWorld	= prWorld ? *prWorld : pGrid->Get_Extent();

		if( rWorld.Get_XRange() > 0.0 && rWorld.Get_YRange() > 0.0 )
		{
			_Fit_Rectangle(rFit = r, rWorld.Get_XRange() / rWorld.Get_YRange(), true);

			dFit	= 1.0;
			d		= dFit * (rWorld.Get_XRange() / rFit.Get_XRange());

			if( zMin >= zMax )
			{
				zMin	= pGrid->Get_ZMin();
				zMax	= pGrid->Get_ZMax();
			}

			zMax	= Colors.Get_Count() / (zMax - zMin);

			for(y=rWorld.Get_YMin(), yFit=rFit.Get_YMin(); yFit<rFit.Get_YMax() && y<pGrid->Get_Extent().Get_YMax(); y++, yFit+=dFit, y+=d)
			{
				for(x=rWorld.Get_XMin(), xFit=rFit.Get_XMin(); xFit<rFit.Get_XMax() && x<pGrid->Get_Extent().Get_XMax(); x++, xFit+=dFit, x+=d)
				{
					if( pGrid->Get_Value(x, y, z) )
					{
						Draw_Point(xFit, yFit, dFit, PDF_STYLE_POINT_SQUARE|PDF_STYLE_POLYGON_FILL, Colors.Get_Color((int)(zMax * (z - zMin))));
					}
				}
			}

			return( true );
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_Doc_PDF::Draw_Curve(CSG_Points &Data, const CSG_Rect &r, int iGraphType)
{
/*
	int i;
    int iMag;
	int iNumLines;
	int iNumData;
    float fMinLine = 0;
	float fWidth;
	float fMax, fMin;
	float fStep;
    float fX, fY;
	float fMaxX, fMinX;
	float fOffsetX;
	CSG_String sValue;
	CSG_Points Points;

	fMin = fMax = Data[0].y;

	for (i = 0; i < Data.Get_Count(); i++)
	{
		if (Data[i].y > fMax)
		{
			fMax = Data[i].y;
		}
		if (Data[i].y < fMin)
		{
			fMin = Data[i].y;
		}
		if (iGraphType == PDF_GRAPH_TYPE_BARS){
			fMin = M_GET_MIN(0, fMin);
		}
    }

	if (fMin != fMax){
		iMag = (int) (log(fMax - fMin) / log(10.0));
		fStep = (float) ((int) pow(10.0, (double) iMag));
		if (fStep == 0)
		{
			fStep = 1.;
		}
		if (fMin < 0) 
		{
			iMag = (int) (log(fabs(fMin)) / log(10.0));
			fMinLine = -(int) pow(10.0, (double) iMag);
		}
		else 
		{
			fMinLine=(int)(((int)(fMin/fStep)) *fStep);
		}

		iNumLines = (int) ((fMax - fMin) / fStep);

		while (iNumLines < 8) 
		{
			fStep = fStep / 2.0;
			iNumLines = (int) ((fMax - fMin) / fStep);
		}
		iNumLines = (int) ((fMax - fMinLine) / fStep);
	}
	else{
		if (fMin > 0)
		{
			fStep = fMin = 0;
			iNumLines = 1;
			fMinLine = fMax;
		}
		else if (fMin < 0)
		{
			fStep = 0;
			fMin = fMin - 1;
			fMinLine = fMax;
		}
		else
		{
			return;
		}		
	}

	for (i = 0; i < iNumLines; i++) 
	{
        fY = r.Get_YMax() - ((fMinLine + fStep * i - fMin) / (fMax - fMin)) * r.Get_YRange();
        if (fY <= r.Get_YMax() && fY >= r.Get_YMin()) 
		{
			fY = HPDF_Page_Height() - fY;
        }
    }

	if (iGraphType == PDF_GRAPH_TYPE_BARS)
	{
		fWidth = (float) r.Get_XRange() / (float) (Data.Get_Count());
	    for (i = 0; i < Data.Get_Count(); i++)
		{
	        fX = r.Get_XMin() + i * fWidth;
			fY = r.Get_YMax();
			fY = HPDF_Page_Height() - fY;
			Draw_Rectangle(fX, fY, fX + fWidth,
					fY - r.Get_YRange() * ((Data[i].y - fMin) / (fMax - fMin)),
					PDF_STYLE_POLYGON_FILLSTROKE, 0x660000);
	    }
	}
	else
	{
		fWidth = (float) r.Get_XRange() / (float) (Data.Get_Count() - 1);
		fY = r.Get_YMin() + r.Get_YRange()
				- r.Get_YRange() * ((Data[0].y - fMin) / (fMax - fMin));
		fY = HPDF_Page_Height() - fY;
		Points.Add(r.Get_XMin(), fY);
		for (i = 1; i < Data.Get_Count(); i++)
		{
			fY = r.Get_YMin() + r.Get_YRange()
					- r.Get_YRange() * ((Data[i].y - fMin) / (fMax - fMin));
			fY = HPDF_Page_Height() - fY;
			Points.Add(r.Get_XMin() + i * fWidth, fY);
	    }
		Draw_Line(Points, 3, 0x660000);
	}

    fMaxX = Data[Data.Get_Count() - 1].x;
    fMinX = Data[0].x;

	if (fMaxX != fMinX)
	{
		iMag = (int) (log(fMaxX - fMinX) / log(10.0));
		fStep = (float) pow(10.0, (double) iMag);

		iNumData = (int) ((fMaxX-fMinX) / fStep);

		while (iNumData < 3)
		{
			fStep = fStep / 2.0;
			iNumData = (int) ((fMaxX-fMinX) / fStep);
		}
		fMinLine = (int)(((int)(fMinX/fStep)) * fStep);
		while (fMinLine<fMinX)
		{
			fMinLine+=fStep;
		}
		fOffsetX = fMinX-fMinLine;

		fY = HPDF_Page_Height() - r.Get_YMin() - r.Get_YRange();
		for (i = 0; i < iNumData; i++)
		{
			fX = r.Get_XMin() + ((fStep * (float) i-fOffsetX) / (fMaxX-fMinX)) * r.Get_XRange();
			if (fX >= r.Get_XMin() && fX < r.Get_XMin() + r.Get_XRange()){
				if (fabs(fStep * (float)i +fMinLine) > 100000)
				{
					sValue = SG_Get_String(fStep * (float)i +fMinLine);
				}
				else
				{
					sValue = SG_Get_String(fStep * (float)i + fMinLine, 2, true);
				}
				Draw_Text(fX, fY, sValue, 9, PDF_STYLE_TEXT_ALIGN_H_RIGHT);
				Draw_Line(fX, fY, fX, fY - 10, 1);
			}
		}
    }

	Draw_Line(r.Get_XMin(), HPDF_Page_Height() - r.Get_YMin(),
			r.Get_XMin(), HPDF_Page_Height() - r.Get_YMax(), 4);
	Draw_Line(r.Get_XMin(), HPDF_Page_Height() - r.Get_YMax(),
			r.Get_XMax(), HPDF_Page_Height() - r.Get_YMax(), 4);
*/
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#else // #ifndef _SAGA_DONOTUSE_HARU

#include "doc_pdf.h"

CSG_Doc_PDF::CSG_Doc_PDF(void)															{	m_pPDF	= NULL;	}
CSG_Doc_PDF::CSG_Doc_PDF(TSG_PDF_Page_Size Size, int Orientation, const SG_Char *Title)	{	m_pPDF	= NULL;	}
CSG_Doc_PDF::~CSG_Doc_PDF(void)															{}
void CSG_Doc_PDF::Layout_Set_Box_Space(double Space, bool bPercent)						{}
void CSG_Doc_PDF::_Layout_Set_Boxes(void)												{}
void CSG_Doc_PDF::_Layout_Set_Box(int iBox)												{}
double CSG_Doc_PDF::Get_Page_To_Meter(void)												{	return( 1.0 );	}
struct _HPDF_Dict_Rec * CSG_Doc_PDF::_Get_Font(TSG_PDF_Font_Type Font)					{	return( NULL );	}
const CSG_Rect & CSG_Doc_PDF::Layout_Get_Box(const SG_Char *ID)							{	return( m_Size_Margins );	}
TSG_PDF_Title_Level CSG_Doc_PDF::_Get_Lowest_Outline_Level(void)						{	return( PDF_TITLE_NONE );	}
bool CSG_Doc_PDF::Open(const SG_Char *Title)											{	return( false );	}
bool CSG_Doc_PDF::Open(TSG_PDF_Page_Size Size, int Orientation, const SG_Char *Title)	{	return( false );	}
bool CSG_Doc_PDF::Close(void)															{	return( false );	}
bool CSG_Doc_PDF::Save(const SG_Char *FileName)											{	return( false );	}
bool CSG_Doc_PDF::Layout_Add_Box(double xMin_Percent, double yMin_Percent, double xMax_Percent, double yMax_Percent, const SG_Char *ID)	{	return( false );	}
bool CSG_Doc_PDF::Layout_Add_Box(const CSG_Rect &Box_Percent, const SG_Char *ID)														{	return( false );	}
bool CSG_Doc_PDF::_Fit_Rectangle(double &x, double &y, double &dx, double &dy, double XToY_Ratio, bool bShrink)							{	return( false );	}
bool CSG_Doc_PDF::_Fit_Rectangle(CSG_Rect &r, double XToY_Ratio, bool bShrink)															{	return( false );	}
bool CSG_Doc_PDF::_Add_Outline_Item(const SG_Char *Title, struct _HPDF_Dict_Rec *pPage, TSG_PDF_Title_Level Level)						{	return( false );	}
bool CSG_Doc_PDF::Add_Outline_Item(const SG_Char *Title)																				{	return( false );	}
bool CSG_Doc_PDF::Add_Page(void)																																						{	return( false );	}
bool CSG_Doc_PDF::Add_Page(TSG_PDF_Page_Size Size, int Orientation)																														{	return( false );	}
bool CSG_Doc_PDF::Add_Page(double Width, double Height)																																{	return( false );	}
bool CSG_Doc_PDF::Set_Size_Page(TSG_PDF_Page_Size Size, int Orientation)																													{	return( false );	}
bool CSG_Doc_PDF::Set_Size_Page(double Width, double Height)																															{	return( false );	}
bool CSG_Doc_PDF::Add_Page_Title(const SG_Char *Title, TSG_PDF_Title_Level Level, TSG_PDF_Page_Size Size, int Orientation)																		{	return( false );	}
bool CSG_Doc_PDF::_Set_Style_FillStroke(int Style, int Fill_Color, int Line_Color, int Line_Width)																					{	return( false );	}
bool CSG_Doc_PDF::Draw_Point(double x, double y, double Width, int Style, int Fill_Color, int Line_Color, int Line_Width)																{	return( false );	}
bool CSG_Doc_PDF::Draw_Line(double xa, double ya, double xb, double yb, int Width, int Color, int Style)																				{	return( false );	}
bool CSG_Doc_PDF::Draw_Line(CSG_Points &Points, int Width, int Color, int Style)																									{	return( false );	}
bool CSG_Doc_PDF::Draw_Rectangle(double xa, double ya, double xb, double yb, int Style, int Fill_Color, int Line_Color, int Line_Width)												{	return( false );	}
bool CSG_Doc_PDF::Draw_Rectangle(const CSG_Rect &r, int Style, int Fill_Color, int Line_Color, int Line_Width)																		{	return( false );	}
bool CSG_Doc_PDF::Draw_Polygon(CSG_Points &Points, int Style, int Fill_Color, int Line_Color, int Line_Width)																		{	return( false );	}
bool CSG_Doc_PDF::Draw_Text(double x, double y, CSG_Strings &Text, int Size, int Style, double Angle, int Color, TSG_PDF_Font_Type Font)												{	return( false );	}
bool CSG_Doc_PDF::Draw_Text(double x, double y, const SG_Char *Text, int Size, int Style, double Angle, int Color, TSG_PDF_Font_Type Font)													{	return( false );	}
bool CSG_Doc_PDF::_Draw_Text(double x, double y, const SG_Char *Text, int Size, int Style, double Angle, int Color, TSG_PDF_Font_Type Font)													{	return( false );	}
bool CSG_Doc_PDF::Draw_Image(double x, double y, double dx, double dy, const SG_Char *FileName)																							{	return( false );	}
bool CSG_Doc_PDF::Draw_Image(const CSG_Rect &r, const SG_Char *FileName)																												{	return( false );	}
bool CSG_Doc_PDF::Draw_Graticule(const CSG_Rect &r, const CSG_Rect &rWorld, int Size)																								{	return( false );	}
bool CSG_Doc_PDF::_Draw_Ruler(const CSG_Rect &r, double zMin, double zMax, bool bHorizontal, bool bAscendent, bool bTickAtTop)														{	return( false );	}
bool CSG_Doc_PDF::_Draw_Table(CSG_Rect r, CSG_Table *pTable, int iRecord, int nRecords, double CellHeight, double HeaderHeightRel)														{	return( false );	}
bool CSG_Doc_PDF::Draw_Table(const CSG_Rect &r, CSG_Table *pTable, double CellHeight, double HeaderHeightRel)																			{	return( false );	}
bool CSG_Doc_PDF::Draw_Table(const CSG_Rect &r, CSG_Table *pTable, int nColumns, double CellHeight, double HeaderHeightRel)																{	return( false );	}
bool CSG_Doc_PDF::_Draw_Shape(CSG_Rect r, CSG_Shape *pShape, double xMin, double yMin, double World2PDF, int Style, int Fill_Color, int Line_Color, int Line_Width, int Point_Width)	{	return( false );	}
bool CSG_Doc_PDF::Draw_Shape(const CSG_Rect &r, CSG_Shape *pShape, int Style, int Fill_Color, int Line_Color, int Line_Width, CSG_Rect *prWorld)										{	return( false );	}
bool CSG_Doc_PDF::Draw_Shapes(const CSG_Rect &r, CSG_Shapes *pShapes, int Style, int Fill_Color, int Line_Color, int Line_Width, CSG_Rect *prWorld)									{	return( false );	}
bool CSG_Doc_PDF::Draw_Grid(const CSG_Rect &r, CSG_Grid *pGrid, const CSG_Colors &Colors, double zMin, double zMax, int Style, CSG_Rect *prWorld)											{	return( false );	}
void CSG_Doc_PDF::Draw_Curve(CSG_Points &Data, const CSG_Rect &r, int iGraphType)	{}

#endif	// #else // #ifndef _SAGA_DONOTUSE_HARU


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifdef _SAGA_UNICODE

bool CSG_Doc_PDF::Layout_Add_Box(double xMin_Percent, double yMin_Percent, double xMax_Percent, double yMax_Percent, const char *ID)
{	return( Layout_Add_Box(xMin_Percent, yMin_Percent, xMax_Percent, yMax_Percent, SG_STR_MBTOSG(ID)) );	}

bool CSG_Doc_PDF::Layout_Add_Box(const CSG_Rect &Box_Percent, const char *ID)
{	return( Layout_Add_Box(Box_Percent, SG_STR_MBTOSG(ID)) );	}

const CSG_Rect & CSG_Doc_PDF::Layout_Get_Box(const char *ID)
{	return( Layout_Get_Box(SG_STR_MBTOSG(ID)) );	}

#endif


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
