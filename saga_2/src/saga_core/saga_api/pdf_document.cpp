
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
//                  pdf_document.cpp                     //
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
#include <wx/filename.h>

#if defined(_SAGA_MSW)
	#define XMD_H
#endif
#if defined(_SAGA_MSW) && !defined(_SAGA_MINGW)
	#define HAVE_BOOLEAN
#endif
#include <libharu.h>

#include "pdf_document.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define PDF_PAGE_HEIGHT_A4_M	0.297

#define PDF_PAGE_WIDTH_A3		 PDF_PAGE_HEIGHT_A4
#define PDF_PAGE_HEIGHT_A3		(PDF_PAGE_HEIGHT_A4 * sqrt(2.0))

#define PDF_METER_TO_POINT		(PDF_PAGE_HEIGHT_A4 / PDF_PAGE_HEIGHT_A4_M)
#define PDF_POINT_TO_METER		(PDF_PAGE_HEIGHT_A4_M / PDF_PAGE_HEIGHT_A4)


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPDF_Document::CPDF_Document(void)
{
	m_pPDF	= NULL;

	Layout_Set_Box_Space(5, false);

	Open();
}

//---------------------------------------------------------
CPDF_Document::CPDF_Document(TPDF_Page_Size Size, int Orientation, const char *Title)
{
	m_pPDF	= NULL;

	Layout_Set_Box_Space(5, false);

	Open(Size, Orientation, Title);
}

//---------------------------------------------------------
CPDF_Document::~CPDF_Document(void)
{
	Close();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPDF_Document::Open(const char *Title)
{
	return( Open(PDF_PAGE_SIZE_A4, PDF_PAGE_ORIENTATION_PORTRAIT, Title) );
}

//---------------------------------------------------------
bool CPDF_Document::Open(TPDF_Page_Size Size, int Orientation, const char *Title)
{
	if( Close() )
	{
		m_pPDF	= new PdfDoc();
		m_pPDF	->NewDoc();

		m_pLastLevel0OutlineItem = NULL;
		m_pLastLevel1OutlineItem = NULL;
		m_pLastLevel2OutlineItem = NULL;

		Set_Size_Page(Size, Orientation);

		PdfType1FontDef	*pFontDef	= new PdfHelveticaFontDef();
		m_pPDF	->AddType1Font(pFontDef, "WinAnsiEncoding", new PdfWinAnsiEncoding());
		m_Font_Default	= pFontDef->FontName();

		if( Title && *Title != '\0' )
		{
			Add_Page_Title(Title);
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CPDF_Document::Close(void)
{
	if( m_pPDF )
	{
		try
		{
			m_pPDF->FreeDoc();
			delete(m_pPDF);
		}
		catch(...)
		{
			return( false );
		}
	}

	m_pPDF		= NULL;
	m_pPage		= NULL;
	m_pCanvas	= NULL;
	m_nPages	= 0;

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPDF_Document::Save(const char *FileName)
{
	if( m_pPDF && m_nPages > 0 && API_Directory_Make(API_Extract_File_Path(FileName)) )
	{
		try
		{
			m_pPDF->WriteToFile(FileName);
		}
		catch(...)
		{
			return ( false );
		}

		return( true );
	}

	API_Callback_Message_Add_Error(LNG("[ERR] Could not save PDF file."));

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double CPDF_Document::Get_Page_To_Meter(void)
{
	return( PDF_POINT_TO_METER );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
const char * CPDF_Document::_Get_Font_Name(TPDF_Font_Type Font)
{
	return( "WinAnsiEncoding" );

	switch( Font )
	{
	case PDF_FONT_Helvetica:				return( "Helvetica" );
	case PDF_FONT_Helvetica_Bold:			return( "Helvetica-Bold" );
	case PDF_FONT_Helvetica_Oblique:		return( "Helvetica-Oblique" );
	case PDF_FONT_Helvetica_BoldOblique:	return( "Helvetica-BoldOblique" );
	case PDF_FONT_Times_Roman:				return( "Times-Roman" );
	case PDF_FONT_Times_Bold:				return( "Times-Bold" );
	case PDF_FONT_Times_Italic:				return( "Times-Italic" );
	case PDF_FONT_Times_BoldItalic:			return( "Times-BoldItalic" );
	case PDF_FONT_Courier:					return( "Courier" );
	case PDF_FONT_Courier_Bold:				return( "Courier-Bold" );
	case PDF_FONT_Courier_Oblique:			return( "Courier-Oblique" );
	case PDF_FONT_Courier_BoldOblique:		return( "Courier-BoldOblique" );
	case PDF_FONT_Symbol:					return( "Symbol" );
	case PDF_FONT_ZapfDingbats:				return( "ZapfDingbats" );
	}

	return( m_Font_Default );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPDF_Document::Layout_Add_Box(double xMin_Percent, double yMin_Percent, double xMax_Percent, double yMax_Percent, const char *ID)
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
bool CPDF_Document::Layout_Add_Box(const CGEO_Rect &Box_Percent, const char *ID)
{
	return( Layout_Add_Box(Box_Percent.Get_XMin(), Box_Percent.Get_YMin(), Box_Percent.Get_XMax(), Box_Percent.Get_YMax(), ID) );
}

//---------------------------------------------------------
const CGEO_Rect & CPDF_Document::Layout_Get_Box(const char *ID)
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
void CPDF_Document::Layout_Set_Box_Space(double Space, bool bPercent)
{
	m_Boxes_Space_bPercent	= bPercent;
	m_Boxes_Space			= Space;
}

//---------------------------------------------------------
void CPDF_Document::_Layout_Set_Boxes(void)
{
	for(int i=0; i<m_Boxes.Get_Count(); i++)
	{
		_Layout_Set_Box(i);
	}
}

//---------------------------------------------------------
void CPDF_Document::_Layout_Set_Box(int iBox)
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
bool CPDF_Document::_Fit_Rectangle(double &x, double &y, double &dx, double &dy, double XToY_Ratio, bool bShrink)
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
bool CPDF_Document::_Fit_Rectangle(CGEO_Rect &r, double XToY_Ratio, bool bShrink)
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
bool CPDF_Document::_Add_Outline_Item(const char *Title, PdfPage *pPage, TPDF_Title_Level Level)
{
	if( m_pPDF && pPage )
	{
		PdfOutlineItem	*pOutlineItem	= NULL;

		switch( Level )
		{
		case PDF_TITLE:
			pOutlineItem	= m_pLastLevel0OutlineItem	= new PdfOutlineItem(m_pPDF->Outlines());
			break;

		case PDF_TITLE_01:
			if( m_pLastLevel0OutlineItem )
			{
				pOutlineItem	= m_pLastLevel1OutlineItem	= new PdfOutlineItem(m_pLastLevel0OutlineItem);
			}
			break;

		case PDF_TITLE_02:
			if( m_pLastLevel1OutlineItem )
			{
				pOutlineItem	= m_pLastLevel2OutlineItem	= new PdfOutlineItem(m_pLastLevel1OutlineItem);
			}
			break;

		case PDF_TITLE_NONE:
			if( m_pLastLevel2OutlineItem )
			{
				pOutlineItem	= new PdfOutlineItem(m_pLastLevel2OutlineItem);
			}
			break;

		}

		if( pOutlineItem )
		{
			pOutlineItem	->SetTitle(Title);

			if( pPage )
			{
				PdfDestination	*pDestination;

				pDestination	= new PdfDestination(pPage);
				pDestination	->SetFit();
				pOutlineItem	->SetDestination(pDestination);
			}

			return( true );
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CPDF_Document::Add_Outline_Item(const char *Title)
{
	return( _Add_Outline_Item(Title, m_pPage, _Get_Lowest_Level_Outline_Item()) );
}

//---------------------------------------------------------
TPDF_Title_Level CPDF_Document::_Get_Lowest_Level_Outline_Item(void)
{
	if( m_pLastLevel2OutlineItem )
	{
		return( PDF_TITLE_NONE );
	}
	else if( m_pLastLevel1OutlineItem )
	{
		return( PDF_TITLE_02 );
	}
	else if( m_pLastLevel0OutlineItem )
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
bool CPDF_Document::Add_Page(void)
{
	return( Add_Page(m_Size_Paper.Get_XRange(), m_Size_Paper.Get_YRange()) );
}

//---------------------------------------------------------
bool CPDF_Document::Add_Page(TPDF_Page_Size Size, int Orientation)
{
	if (Add_Page())
	{
		return (Set_Size_Page(Size, Orientation));
	}
	else
	{
		return false;
	}
}

//---------------------------------------------------------
bool CPDF_Document::Add_Page(double Width, double Height)
{
	if( m_pPDF )
	{
		m_nPages++;

		m_pPage		= m_pPDF->AddPage();

		Set_Size_Page(Width, Height);
		Set_Size_Page(m_Size_Paper.Get_XRange(), m_Size_Paper.Get_YRange());

		m_pCanvas	= m_pPage->Canvas();

		m_pCanvas	->SetRGBStroke	(  0,   0,   0);
		m_pCanvas	->SetRGBFill	(255, 255, 255);

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
bool CPDF_Document::Set_Size_Page(TPDF_Page_Size Size, int Orientation)
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
bool CPDF_Document::Set_Size_Page(double Width, double Height)
{
	if( (Width > 0.0 && Height > 0.0) )
	{
		m_Size_Paper	.Assign(0.0, 0.0, Width, Height);

		m_Size_Margins	= m_Size_Paper;
		m_Size_Margins.Deflate(10.0, false);

		_Layout_Set_Boxes();

		if( m_pPage )
		{
			m_pPage->SetSize((int)m_Size_Paper.Get_XRange(), (int)m_Size_Paper.Get_YRange());
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
bool CPDF_Document::Add_Page_Title(const char *Title, TPDF_Title_Level Level, TPDF_Page_Size Size, int Orientation)
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
		Draw_Text(Get_Margins().Get_XCenter(), Get_Margins().Get_YCenter(), Title, FontSize, PDF_STYLE_TEXT_ALIGN_H_CENTER, 0.0, COLOR_GET_RGB(0, 0, 0));

		//-------------------------------------------------
		if( bLine )
		{
			double	y	= Get_Margins().Get_YCenter() - 25.0;

			Draw_Line(Get_Margins().Get_XMin(), y, Get_Margins().Get_XMax(), y, 5, COLOR_GET_RGB(0, 0, 0), PDF_STYLE_LINE_END_ROUND);
		}

		if( bDestination )
		{
			PdfDestination	*pDestination	= new PdfDestination(m_pPage);
			pDestination->SetFit();
			m_pPDF->Catalog()->SetOpenAction(pDestination);
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
bool CPDF_Document::_Set_Style_FillStroke(int Style, int Fill_Color, int Line_Color, int Line_Width)
{
	if( Is_Ready_To_Draw() )
	{
		if( Style & PDF_STYLE_POLYGON_STROKE )
		{
			if( Style & PDF_STYLE_LINE_END_ROUND )
			{
				m_pCanvas->SetLineCap(PDF_ROUND_END);
			}
			else if( Style & PDF_STYLE_LINE_END_SQUARE )
			{
				m_pCanvas->SetLineCap(PDF_PROJECTING_SCUARE_END);
			}
			else // if( Style & PDF_STYLE_LINE_END_BUTT )
			{
				m_pCanvas->SetLineCap(PDF_BUTT_END);
			}

			if( Style & PDF_STYLE_LINE_JOIN_ROUND )
			{
				m_pCanvas->SetLineJoin(PDF_ROUND_JOIN);
			}
			else if( Style & PDF_STYLE_LINE_JOIN_BEVEL )
			{
				m_pCanvas->SetLineJoin(PDF_BEVEL_JOIN);
			}
			else // if( Style & PDF_STYLE_LINE_JOIN_MITER )
			{
				m_pCanvas->SetLineJoin(PDF_MITER_JOIN);
			}

			m_pCanvas->SetRGBStroke(COLOR_GET_R(Line_Color), COLOR_GET_G(Line_Color), COLOR_GET_B(Line_Color));

			m_pCanvas->SetLineWidth(Line_Width);
		}

		//-------------------------------------------------
		if( Style & PDF_STYLE_POLYGON_FILL )
		{
			m_pCanvas->SetRGBFill(COLOR_GET_R(Fill_Color), COLOR_GET_G(Fill_Color), COLOR_GET_B(Fill_Color));
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CPDF_Document::Draw_Point(double x, double y, double Width, int Style, int Fill_Color, int Line_Color, int Line_Width)
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
bool CPDF_Document::Draw_Line(double xa, double ya, double xb, double yb, int Width, int Color, int Style)
{
	CAPI_dPoints	Points;

	Points.Add(xa, ya);
	Points.Add(xb, yb);

	return( Draw_Line(Points, Width, Color, Style) );
}

bool CPDF_Document::Draw_Line(CAPI_dPoints &Points, int Width, int Color, int Style)
{
	if( Points.Get_Count() > 1 && _Set_Style_FillStroke(Style|PDF_STYLE_POLYGON_STROKE, 0, Color, Width) )
	{
		m_pCanvas->MoveTo(Points[0].x, Points[0].y);

		for(int i=1; i<Points.Get_Count(); i++)
		{
			m_pCanvas->LineTo(Points[i].x, Points[i].y);
		}

		//-------------------------------------------------
		m_pCanvas->Stroke();

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CPDF_Document::Draw_Rectangle(double xa, double ya, double xb, double yb, int Style, int Fill_Color, int Line_Color, int Line_Width)
{
	CAPI_dPoints	Points;

	Points.Add(xa, ya);
	Points.Add(xb, ya);
	Points.Add(xb, yb);
	Points.Add(xa, yb);

	return( Draw_Polygon(Points, Style, Fill_Color, Line_Color, Line_Width) );
}

bool CPDF_Document::Draw_Rectangle(const CGEO_Rect &r, int Style, int Fill_Color, int Line_Color, int Line_Width)
{
	return( Draw_Rectangle(r.Get_XMin(), r.Get_YMin(), r.Get_XMax(), r.Get_YMax(), Style, Fill_Color, Line_Color, Line_Width) );
}

//---------------------------------------------------------
bool CPDF_Document::Draw_Polygon(CAPI_dPoints &Points, int Style, int Fill_Color, int Line_Color, int Line_Width)
{
	if( Points.Get_Count() > 2 && _Set_Style_FillStroke(Style, Fill_Color, Line_Color, Line_Width) )
	{
		m_pCanvas->MoveTo(Points[0].x, Points[0].y);

		for(int i=1; i<Points.Get_Count(); i++)
		{
			m_pCanvas->LineTo(Points[i].x, Points[i].y);
		}

		m_pCanvas->ClosePath();

		//-------------------------------------------------
		if( Style & PDF_STYLE_POLYGON_FILL && Style & PDF_STYLE_POLYGON_STROKE )
		{
			m_pCanvas->EofillStroke();
		}
		else if( Style & PDF_STYLE_POLYGON_FILL )
		{
			m_pCanvas->Eofill();
		}
		else // if( Style & PDF_STYLE_POLYGON_STROKE )
		{
			m_pCanvas->Stroke();
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
bool CPDF_Document::Draw_Text(double x, double y, CAPI_Strings &Text, int Size, int Style, double Angle, int Color, TPDF_Font_Type Font)
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
bool CPDF_Document::Draw_Text(double x, double y, const char *Text, int Size, int Style, double Angle, int Color, TPDF_Font_Type Font)
{
	int				n;
	CAPI_String		String(Text);
	CAPI_Strings	Strings;

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
bool CPDF_Document::_Draw_Text(double x, double y, const char *Text, int Size, int Style, double Angle, int Color, TPDF_Font_Type Font)
{

	float Width, Height;

	if( m_pCanvas && Text && *Text != '\0' )
	{
		double	ax, ay, bx, by;

		m_pCanvas->SetFontAndSize(_Get_Font_Name(Font), Size);

		m_pCanvas->SetTextRenderingMode(PDF_FILL_THEN_STROKE);
		m_pCanvas->SetLineWidth(0);
		m_pCanvas->SetRGBStroke(COLOR_GET_R(Color), COLOR_GET_G(Color), COLOR_GET_B(Color));
		m_pCanvas->SetRGBFill  (COLOR_GET_R(Color), COLOR_GET_G(Color), COLOR_GET_B(Color));

		Width = m_pCanvas->TextWidth(Text) * cos(Angle);
		Height = m_pCanvas->TextWidth(Text) * sin(Angle) + Size;

		//-------------------------------------------------
		if( Style & PDF_STYLE_TEXT_ALIGN_H_CENTER )
		{
			ax	= x - m_pCanvas->TextWidth(Text) / 2.0;
		}
		else if( Style & PDF_STYLE_TEXT_ALIGN_H_RIGHT )
		{
			ax	= x - m_pCanvas->TextWidth(Text);
		}
		else
		{
			ax	= x;
		}

		bx	= ax + m_pCanvas->TextWidth(Text);

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
		m_pCanvas->BeginText();

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

			m_pCanvas->SetTextMatrix(dCos, dSin, -dSin, dCos, ax, ay);
		}
		else
		{
			m_pCanvas->MoveTextPos(ax, ay);
		}

		m_pCanvas->ShowText(Text);
		m_pCanvas->EndText();

		//-------------------------------------------------
		if( Style & PDF_STYLE_TEXT_UNDERLINE )
		{
			double	dx, dy;

			dy	=  0.2 * Size * (ax - bx) / sqrt(MAT_Square(ax - bx) + MAT_Square(ay - by));
			dx	= -0.2 * Size * (ay - by) / sqrt(MAT_Square(ax - bx) + MAT_Square(ay - by));

			Draw_Line(ax + dx, ay + dy, bx + dx, by + dy, (int)(0.1 * Size), Color);
		}

		if( Style & PDF_STYLE_TEXT_STROKE )
		{
			double	dx, dy;

			dy	= -0.4 * Size * (ax - bx) / sqrt(MAT_Square(ax - bx) + MAT_Square(ay - by));
			dx	=  0.4 * Size * (ay - by) / sqrt(MAT_Square(ax - bx) + MAT_Square(ay - by));

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
bool CPDF_Document::Draw_Image(double x, double y, double dx, double dy, const char *FileName)
{
	bool		bKeepRatio	= true;
	PdfImage	*pImage		= NULL;

	//-----------------------------------------------------
	if( m_pCanvas && FileName && *FileName != '\0' && wxFileExists(FileName) && dx > 0.0 && dy > 0.0 )
	{
		if( API_Cmp_File_Extension(FileName, "png") )
		{
			PdfPngImage		*pPNG;

			pPNG	= new PdfPngImage(m_pPDF);
		//	pPNG	->LoadFromFile(FileName);
	try	{	pPNG	->LoadFromFile(FileName);	}	catch(...)	{}

			if( !pPNG->IsValidObject() )
			{
				delete(pPNG);
			}
			else
			{
				pImage	= pPNG;
			}
		}
		else if( API_Cmp_File_Extension(FileName, "jpg") )
		{
			PdfJpegImage	*pJPG;

			pJPG	= new PdfJpegImage(m_pPDF);
		//	pJPG	->LoadFromFile(FileName);
	try	{	pJPG	->LoadFromFile(FileName);	}	catch(...)	{}

			if( !pJPG->IsValidObject() )
			{
				delete(pJPG);
			}
			else
			{
				pImage	= pJPG;
			}
		}
	}

	//-----------------------------------------------------
	if( pImage )
	{
		if( bKeepRatio )
		{
			_Fit_Rectangle(x, y, dx, dy, pImage->Width() / pImage->Height(), true);
		}

		pImage->AddFilter(PDF_FILTER_DEFLATE);
		m_pPDF->AddXObject(pImage);

		m_pCanvas->GSave();
		m_pCanvas->Concat(dx, 0, 0, dy, x, y);
		m_pCanvas->ExecuteXObject(pImage);
		m_pCanvas->GRestore();

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CPDF_Document::Draw_Image(const CGEO_Rect &r, const char *FileName)
{
	return( Draw_Image(r.Get_XMin(), r.Get_YMin(), r.Get_XRange(), r.Get_YRange(), FileName) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPDF_Document::Draw_Graticule(const CGEO_Rect &r, const CGEO_Rect &rWorld, int Size)
{
	if( Is_Ready_To_Draw() )
	{
		CGEO_Rect	rRuler, rFrame(r);

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
bool CPDF_Document::_Draw_Ruler(const CGEO_Rect &r, double zMin, double zMax, bool bHorizontal, bool bAscendent, bool bTickAtTop)
{
	int			Decimals, FontSize;
	double		xOff, yOff, Width, Height, Height_Tick, z, dz, zToDC, zDC, zPos, tPos;
	CAPI_String	s;

	//-----------------------------------------------------
	Width	= bHorizontal ? r.Get_XRange() : r.Get_YRange();
	Height	= bHorizontal ? r.Get_YRange() : r.Get_XRange();

	if( Is_Ready_To_Draw() && zMin < zMax && Width > 0 && Height > 0 )
	{
		xOff		= r.Get_XMin();
		yOff		= r.Get_YMax();

		FontSize	= (int)(0.45 * (double)Height);
		m_pCanvas->SetFontAndSize(_Get_Font_Name(PDF_FONT_DEFAULT), FontSize);

		Height_Tick	= (int)(0.3 * (double)Height);

		//-------------------------------------------------
		zToDC		= (double)Width / (zMax - zMin);

		dz			= pow(10.0, floor(log10(zMax - zMin)) - 1.0);
		Decimals	= dz >= 1.0 ? 0 : (int)fabs(log10(dz));

		s.Printf("%.*f", Decimals, zMax);
		zDC			= m_pCanvas->TextWidth(s.c_str());
		while( zToDC * dz < zDC + RULER_TEXT_SPACE )
		{
			dz	*= 2;
		}

		//-------------------------------------------------
		z			= dz * floor(zMin / dz);
		if( z < zMin )	z	+= dz;

		for(; z<=zMax; z+=dz)
		{
			s.Printf("%.*f", Decimals, z);

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
bool CPDF_Document::_Draw_Table(CGEO_Rect r, CTable *pTable, int iRecord, int nRecords, double CellHeight, double HeaderHeightRel)
{
	int				iField, dyFont, dyFont_Hdr;
	double			xPos, yPos, dxPos, dyPos, dyPos_Hdr, xSpace, ySpace, ySpace_Hdr;
	bool			bAddHeader;
	CTable_Record	*pRecord;

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
					Draw_Rectangle(xPos, yPos, xPos + dxPos, yPos - dyPos_Hdr, PDF_STYLE_POLYGON_FILLSTROKE, COLOR_DEF_GREY_LIGHT, COLOR_DEF_BLACK, 0);
					Draw_Text(xPos + xSpace, yPos - ySpace_Hdr, pTable->Get_Field_Name(iField), dyFont_Hdr, PDF_STYLE_TEXT_ALIGN_H_LEFT|PDF_STYLE_TEXT_ALIGN_V_TOP);
				}

				yPos		-= dyPos_Hdr;
				bAddHeader	= false;
			}

			//---------------------------------------------
			pRecord	= pTable->Get_Record(iRecord);

			for(iField=0, xPos=r.Get_XMin(); iField<pTable->Get_Field_Count(); iField++, xPos+=dxPos)
			{
				Draw_Rectangle(xPos, yPos, xPos + dxPos, yPos - dyPos, PDF_STYLE_POLYGON_STROKE, COLOR_DEF_WHITE, COLOR_DEF_BLACK, 0);
				Draw_Text(xPos + xSpace, yPos - ySpace, pRecord->asString(iField), dyFont, PDF_STYLE_TEXT_ALIGN_H_LEFT|PDF_STYLE_TEXT_ALIGN_V_TOP);
			}
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CPDF_Document::Draw_Table(const CGEO_Rect &r, CTable *pTable, double CellHeight, double HeaderHeightRel)
{
	if( pTable && pTable->is_Valid() )
	{
		return( _Draw_Table(r, pTable, 0, pTable->Get_Record_Count(), CellHeight, HeaderHeightRel) );
	}

	return( false );
}

//---------------------------------------------------------
bool CPDF_Document::Draw_Table(const CGEO_Rect &r, CTable *pTable, int nColumns, double CellHeight, double HeaderHeightRel)
{
	if( pTable && pTable->is_Valid() && nColumns > 1 )
	{
		int			nRecords;
		double		dx, dxSpace;
		CGEO_Rect	rColumn(r), rTable(r);

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
bool CPDF_Document::_Draw_Shape(CGEO_Rect r, CShape *pShape, double xMin, double yMin, double World2PDF, int Style, int Fill_Color, int Line_Color, int Line_Width, int Point_Width)
{
	if( pShape && pShape->is_Valid() )
	{
		int				iPart, iPoint;
		TGEO_Point		Point;
		CAPI_dPoints	Points;

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
				if( ((CShape_Polygon *)pShape)->is_Lake(iPart) )
				{
					Draw_Polygon(Points, PDF_STYLE_POLYGON_FILLSTROKE, COLOR_DEF_WHITE, Line_Color, Line_Width);
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
bool CPDF_Document::Draw_Shape(const CGEO_Rect &r, CShape *pShape, int Style, int Fill_Color, int Line_Color, int Line_Width, CGEO_Rect *prWorld)
{
	double		World2PDF;
	CGEO_Rect	rWorld, rFit;

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
bool CPDF_Document::Draw_Shapes(const CGEO_Rect &r, CShapes *pShapes, int Style, int Fill_Color, int Line_Color, int Line_Width, CGEO_Rect *prWorld)
{
	double		World2PDF;
	CGEO_Rect	rWorld, rFit;

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
bool CPDF_Document::Draw_Grid(const CGEO_Rect &r, CGrid *pGrid, const CColors &Colors, double zMin, double zMax, int Style, CGEO_Rect *prWorld)
{
	double		x, y, z, d, xFit, yFit, dFit;
	CGEO_Rect	rWorld, rFit;

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
/*void CPDF_Document::Draw_Curve(CAPI_dPoints &Data,
							   const CGEO_Rect &r,
								int iGraphType){

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
	CAPI_String sValue;
	CAPI_dPoints Points;

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
			fY = m_pCanvas->Height() - fY;
        }
    }

	if (iGraphType == PDF_GRAPH_TYPE_BARS)
	{
		fWidth = (float) r.Get_XRange() / (float) (Data.Get_Count());
	    for (i = 0; i < Data.Get_Count(); i++)
		{
	        fX = r.Get_XMin() + i * fWidth;
			fY = r.Get_YMax();
			fY = m_pCanvas->Height() - fY;
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
		fY = m_pCanvas->Height() - fY;
		Points.Add(r.Get_XMin(), fY);
		for (i = 1; i < Data.Get_Count(); i++)
		{
			fY = r.Get_YMin() + r.Get_YRange()
					- r.Get_YRange() * ((Data[i].y - fMin) / (fMax - fMin));
			fY = m_pCanvas->Height() - fY;
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

		fY = m_pCanvas->Height() - r.Get_YMin() - r.Get_YRange();
		for (i = 0; i < iNumData; i++)
		{
			fX = r.Get_XMin() + ((fStep * (float) i-fOffsetX) / (fMaxX-fMinX)) * r.Get_XRange();
			if (fX >= r.Get_XMin() && fX < r.Get_XMin() + r.Get_XRange()){
				if (fabs(fStep * (float)i +fMinLine) > 100000)
				{
					sValue = API_Get_String(fStep * (float)i +fMinLine);
				}
				else
				{
					sValue = API_Get_String(fStep * (float)i + fMinLine, 2, true);
				}
				Draw_Text(fX, fY, sValue, 9, PDF_STYLE_TEXT_ALIGN_H_RIGHT);
				Draw_Line(fX, fY, fX, fY - 10, 1);
			}
		}
    }

	Draw_Line(r.Get_XMin(), m_pCanvas->Height() - r.Get_YMin(),
			r.Get_XMin(), m_pCanvas->Height() - r.Get_YMax(), 4);
	Draw_Line(r.Get_XMin(), m_pCanvas->Height() - r.Get_YMax(),
			r.Get_XMax(), m_pCanvas->Height() - r.Get_YMax(), 4);

}//method*/


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#else // #ifndef _SAGA_DONOTUSE_HARU

#include "pdf_document.h"

CPDF_Document::CPDF_Document(void)														{	m_pPDF	= NULL;	}
CPDF_Document::CPDF_Document(TPDF_Page_Size Size, int Orientation, const char *Title)	{	m_pPDF	= NULL;	}
CPDF_Document::~CPDF_Document(void)														{}
void CPDF_Document::Layout_Set_Box_Space(double Space, bool bPercent)					{}
void CPDF_Document::_Layout_Set_Boxes(void)												{}
void CPDF_Document::_Layout_Set_Box(int iBox)											{}
double CPDF_Document::Get_Page_To_Meter(void)											{	return( 1.0 );	}
const char * CPDF_Document::_Get_Font_Name(TPDF_Font_Type Font)							{	return( "-" );	}
const CGEO_Rect & CPDF_Document::Layout_Get_Box(const char *ID)							{	return( m_Size_Margins );	}
TPDF_Title_Level CPDF_Document::_Get_Lowest_Level_Outline_Item(void)					{	return( PDF_TITLE_NONE );	}
bool CPDF_Document::Open(const char *Title)												{	return( false );	}
bool CPDF_Document::Open(TPDF_Page_Size Size, int Orientation, const char *Title)		{	return( false );	}
bool CPDF_Document::Close(void)															{	return( false );	}
bool CPDF_Document::Save(const char *FileName)											{	return( false );	}
bool CPDF_Document::Layout_Add_Box(double xMin_Percent, double yMin_Percent, double xMax_Percent, double yMax_Percent, const char *ID)	{	return( false );	}
bool CPDF_Document::Layout_Add_Box(const CGEO_Rect &Box_Percent, const char *ID)														{	return( false );	}
bool CPDF_Document::_Fit_Rectangle(double &x, double &y, double &dx, double &dy, double XToY_Ratio, bool bShrink)						{	return( false );	}
bool CPDF_Document::_Fit_Rectangle(CGEO_Rect &r, double XToY_Ratio, bool bShrink)														{	return( false );	}
bool CPDF_Document::_Add_Outline_Item(const char *Title, PdfPage *pPage, TPDF_Title_Level Level)										{	return( false );	}
bool CPDF_Document::Add_Outline_Item(const char *Title)																					{	return( false );	}
bool CPDF_Document::Add_Page(void)																																						{	return( false );	}
bool CPDF_Document::Add_Page(TPDF_Page_Size Size, int Orientation)																														{	return( false );	}
bool CPDF_Document::Add_Page(double Width, double Height)																																{	return( false );	}
bool CPDF_Document::Set_Size_Page(TPDF_Page_Size Size, int Orientation)																													{	return( false );	}
bool CPDF_Document::Set_Size_Page(double Width, double Height)																															{	return( false );	}
bool CPDF_Document::Add_Page_Title(const char *Title, TPDF_Title_Level Level, TPDF_Page_Size Size, int Orientation)																		{	return( false );	}
bool CPDF_Document::_Set_Style_FillStroke(int Style, int Fill_Color, int Line_Color, int Line_Width)																					{	return( false );	}
bool CPDF_Document::Draw_Point(double x, double y, double Width, int Style, int Fill_Color, int Line_Color, int Line_Width)																{	return( false );	}
bool CPDF_Document::Draw_Line(double xa, double ya, double xb, double yb, int Width, int Color, int Style)																				{	return( false );	}
bool CPDF_Document::Draw_Line(CAPI_dPoints &Points, int Width, int Color, int Style)																									{	return( false );	}
bool CPDF_Document::Draw_Rectangle(double xa, double ya, double xb, double yb, int Style, int Fill_Color, int Line_Color, int Line_Width)												{	return( false );	}
bool CPDF_Document::Draw_Rectangle(const CGEO_Rect &r, int Style, int Fill_Color, int Line_Color, int Line_Width)																		{	return( false );	}
bool CPDF_Document::Draw_Polygon(CAPI_dPoints &Points, int Style, int Fill_Color, int Line_Color, int Line_Width)																		{	return( false );	}
bool CPDF_Document::Draw_Text(double x, double y, CAPI_Strings &Text, int Size, int Style, double Angle, int Color, TPDF_Font_Type Font)												{	return( false );	}
bool CPDF_Document::Draw_Text(double x, double y, const char *Text, int Size, int Style, double Angle, int Color, TPDF_Font_Type Font)													{	return( false );	}
bool CPDF_Document::_Draw_Text(double x, double y, const char *Text, int Size, int Style, double Angle, int Color, TPDF_Font_Type Font)													{	return( false );	}
bool CPDF_Document::Draw_Image(double x, double y, double dx, double dy, const char *FileName)																							{	return( false );	}
bool CPDF_Document::Draw_Image(const CGEO_Rect &r, const char *FileName)																												{	return( false );	}
bool CPDF_Document::Draw_Graticule(const CGEO_Rect &r, const CGEO_Rect &rWorld, int Size)																								{	return( false );	}
bool CPDF_Document::_Draw_Ruler(const CGEO_Rect &r, double zMin, double zMax, bool bHorizontal, bool bAscendent, bool bTickAtTop)														{	return( false );	}
bool CPDF_Document::_Draw_Table(CGEO_Rect r, CTable *pTable, int iRecord, int nRecords, double CellHeight, double HeaderHeightRel)														{	return( false );	}
bool CPDF_Document::Draw_Table(const CGEO_Rect &r, CTable *pTable, double CellHeight, double HeaderHeightRel)																			{	return( false );	}
bool CPDF_Document::Draw_Table(const CGEO_Rect &r, CTable *pTable, int nColumns, double CellHeight, double HeaderHeightRel)																{	return( false );	}
bool CPDF_Document::_Draw_Shape(CGEO_Rect r, CShape *pShape, double xMin, double yMin, double World2PDF, int Style, int Fill_Color, int Line_Color, int Line_Width, int Point_Width)	{	return( false );	}
bool CPDF_Document::Draw_Shape(const CGEO_Rect &r, CShape *pShape, int Style, int Fill_Color, int Line_Color, int Line_Width, CGEO_Rect *prWorld)										{	return( false );	}
bool CPDF_Document::Draw_Shapes(const CGEO_Rect &r, CShapes *pShapes, int Style, int Fill_Color, int Line_Color, int Line_Width, CGEO_Rect *prWorld)									{	return( false );	}
bool CPDF_Document::Draw_Grid(const CGEO_Rect &r, CGrid *pGrid, const CColors &Colors, double zMin, double zMax, int Style, CGEO_Rect *prWorld)											{	return( false );	}

#endif	// #else // #ifndef _SAGA_DONOTUSE_HARU


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
