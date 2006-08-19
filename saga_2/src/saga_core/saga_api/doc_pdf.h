
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
//                      doc_pdf.h                        //
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
#ifndef HEADER_INCLUDED__SAGA_API__doc_pdf_H
#define HEADER_INCLUDED__SAGA_API__doc_pdf_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "shapes.h"
#include "grid.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define PDF_TABLE_CELL_HEIGHT_DEFAULT		20.0
#define PDF_TABLE_HEADER_HEIGHT				40.0

//---------------------------------------------------------
#define PDF_PAGE_ORIENTATION_PREVIOUS		-1
#define PDF_PAGE_ORIENTATION_PORTRAIT		0
#define PDF_PAGE_ORIENTATION_LANDSCAPE		1

//---------------------------------------------------------
#define PDF_STYLE_POINT_CIRCLE				0x00000001
#define PDF_STYLE_POINT_SQUARE				0x00000002

#define PDF_STYLE_LINE_END_BUTT				0x00000004
#define PDF_STYLE_LINE_END_ROUND			0x00000008
#define PDF_STYLE_LINE_END_SQUARE			0x00000010
#define PDF_STYLE_LINE_JOIN_MITER			0x00000020
#define PDF_STYLE_LINE_JOIN_ROUND			0x00000040
#define PDF_STYLE_LINE_JOIN_BEVEL			0x00000080

#define PDF_STYLE_POLYGON_STROKE			0x00000100
#define PDF_STYLE_POLYGON_FILL				0x00000200
#define PDF_STYLE_POLYGON_FILLSTROKE		(PDF_STYLE_POLYGON_STROKE|PDF_STYLE_POLYGON_FILL)

//---------------------------------------------------------
#define PDF_STYLE_TEXT_ALIGN_H_LEFT			0x00000001
#define PDF_STYLE_TEXT_ALIGN_H_CENTER		0x00000002
#define PDF_STYLE_TEXT_ALIGN_H_RIGHT		0x00000004

#define PDF_STYLE_TEXT_ALIGN_V_TOP			0x00000008
#define PDF_STYLE_TEXT_ALIGN_V_CENTER		0x00000010
#define PDF_STYLE_TEXT_ALIGN_V_BOTTOM		0x00000020

#define PDF_STYLE_TEXT_UNDERLINE			0x00000040
#define PDF_STYLE_TEXT_STROKE				0x00000080

//----------------------------------------------------------
#define PDF_GRAPH_TYPE_BARS					1
#define PDF_GRAPH_TYPE_LINES				2


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
typedef enum EPDF_Font_Type
{
	PDF_FONT_DEFAULT	= 0,
	PDF_FONT_Helvetica,
	PDF_FONT_Helvetica_Bold,
	PDF_FONT_Helvetica_Oblique,
	PDF_FONT_Helvetica_BoldOblique,
	PDF_FONT_Times_Roman,
	PDF_FONT_Times_Bold,
	PDF_FONT_Times_Italic,
	PDF_FONT_Times_BoldItalic,
	PDF_FONT_Courier,
	PDF_FONT_Courier_Bold,
	PDF_FONT_Courier_Oblique,
	PDF_FONT_Courier_BoldOblique,
	PDF_FONT_Symbol,
	PDF_FONT_ZapfDingbats
}
TPDF_Font_Type;

//---------------------------------------------------------
typedef enum EPDF_Page_Size
{
	PDF_PAGE_SIZE_A4		= 1,
	PDF_PAGE_SIZE_A3,
	PDF_PAGE_SIZE_PREVIOUS
}
TPDF_Page_Size;

//---------------------------------------------------------
typedef enum EPDF_Title_Level
{
	PDF_TITLE	= 0,
	PDF_TITLE_01,
	PDF_TITLE_02,
	PDF_TITLE_NONE
}
TPDF_Title_Level;


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CDoc_PDF
{
public:
	CDoc_PDF(void);
	CDoc_PDF(TPDF_Page_Size Size, int Orientation = PDF_PAGE_ORIENTATION_PORTRAIT, const char *Title = NULL);
	virtual ~CDoc_PDF(void);

	bool						Open						(TPDF_Page_Size Size = PDF_PAGE_SIZE_A4, int Orientation = PDF_PAGE_ORIENTATION_PORTRAIT, const char *Title = NULL);
	bool						Open						(const char *Title);
	bool						Close						(void);

	bool						Save						(const char *FileName);

	bool						Is_Open						(void)	{	return( m_pPDF    != NULL );	}
	bool						Is_Ready_To_Draw			(void)	{	return( m_pCanvas != NULL );	}

	static double				Get_Page_To_Meter			(void);

	bool						Add_Page					(void);
	bool						Add_Page					(TPDF_Page_Size Size, int Orientation = PDF_PAGE_ORIENTATION_PORTRAIT);
	bool						Add_Page					(double Width, double Height);

	bool						Add_Page_Title				(const char *Title, TPDF_Title_Level Level = PDF_TITLE, TPDF_Page_Size Size = PDF_PAGE_SIZE_PREVIOUS, int Orientation = PDF_PAGE_ORIENTATION_PREVIOUS);
	bool						Add_Outline_Item			(const char *Title);

	bool						Set_Size_Page				(TPDF_Page_Size Size, int Orientation = PDF_PAGE_ORIENTATION_PORTRAIT);
	bool						Set_Size_Page				(double Width, double Height);
	const CSG_Rect &			Get_Size_Page				(void)	{	return( m_Size_Paper );		}
	const CSG_Rect &			Get_Margins					(void)	{	return( m_Size_Margins );	}

	bool						Layout_Add_Box				(double xMin_Percent, double yMin_Percent, double xMax_Percent, double yMax_Percent, const char *ID = NULL);
	bool						Layout_Add_Box				(const CSG_Rect &Box_Percent, const char *ID = NULL);
	const CSG_Rect &			Layout_Get_Box				(int iBox)	{	return( iBox >= 0 && iBox < m_Boxes.Get_Count() ? m_Boxes[iBox] : m_Size_Margins );	}
	const CSG_Rect &			Layout_Get_Box				(const char *ID);
	void						Layout_Set_Box_Space		(double Space, bool bPercent);

	bool						Draw_Point					(double x, double y, double Width, int Style = PDF_STYLE_POINT_SQUARE|PDF_STYLE_POLYGON_FILLSTROKE, int Fill_Color = SG_COLOR_WHITE, int Line_Color = SG_COLOR_BLACK, int Line_Width = 0);
	bool						Draw_Line					(double xa, double ya, double xb, double yb	, int Width = 0, int Color = SG_COLOR_BLACK, int Style = 0);
	bool						Draw_Line					(CSG_Points &Points							, int Width = 0, int Color = SG_COLOR_BLACK, int Style = 0);
	bool						Draw_Rectangle				(double xa, double ya, double xb, double yb	, int Style = PDF_STYLE_POLYGON_FILLSTROKE, int Fill_Color = SG_COLOR_WHITE, int Line_Color = SG_COLOR_BLACK, int Line_Width = 0);
	bool						Draw_Rectangle				(const CSG_Rect &r							, int Style = PDF_STYLE_POLYGON_FILLSTROKE, int Fill_Color = SG_COLOR_WHITE, int Line_Color = SG_COLOR_BLACK, int Line_Width = 0);
	bool						Draw_Polygon				(CSG_Points &Points							, int Style = PDF_STYLE_POLYGON_FILLSTROKE, int Fill_Color = SG_COLOR_GREEN, int Line_Color = SG_COLOR_BLACK, int Line_Width = 0);
	bool						Draw_Text					(double x, double y, const char   *Text, int Size, int Style = PDF_STYLE_TEXT_ALIGN_H_LEFT|PDF_STYLE_TEXT_ALIGN_V_TOP, double Angle = 0.0, int Color = SG_COLOR_BLACK, TPDF_Font_Type Font = PDF_FONT_DEFAULT);
	bool						Draw_Text					(double x, double y, CSG_Strings  &Text, int Size, int Style = PDF_STYLE_TEXT_ALIGN_H_LEFT|PDF_STYLE_TEXT_ALIGN_V_TOP, double Angle = 0.0, int Color = SG_COLOR_BLACK, TPDF_Font_Type Font = PDF_FONT_DEFAULT);
	bool						Draw_Image					(double x, double y, double dx, double dy	, const char *FileName);
	bool						Draw_Image					(const CSG_Rect &r							, const char *FileName);

	bool						Draw_Graticule				(const CSG_Rect &r, const CSG_Rect &rWorld, int Size = 10);
	void						Draw_Curve					(CSG_Points &Data, const CSG_Rect &r, int iGraphType);

	bool						Draw_Table					(const CSG_Rect &r, CTable *pTable					, double CellHeight = 0.0, double HeaderHeightRel = 1.5);
	bool						Draw_Table					(const CSG_Rect &r, CTable *pTable, int nColumns	, double CellHeight = 0.0, double HeaderHeightRel = 1.5);

	bool						Draw_Shape					(const CSG_Rect &r, CShape  *pShape , int Style = PDF_STYLE_POLYGON_FILLSTROKE, int Fill_Color = SG_COLOR_GREEN, int Line_Color = SG_COLOR_BLACK, int Line_Width = 0, CSG_Rect *prWorld = NULL);
	bool						Draw_Shapes					(const CSG_Rect &r, CShapes *pShapes, int Style = PDF_STYLE_POLYGON_FILLSTROKE, int Fill_Color = SG_COLOR_GREEN, int Line_Color = SG_COLOR_BLACK, int Line_Width = 0, CSG_Rect *prWorld = NULL);

	bool						Draw_Grid					(const CSG_Rect &r, CGrid *pGrid, const CSG_Colors &Colors, double zMin = 0.0, double zMax = 0.0, int Style = 0, CSG_Rect *prWorld = NULL);


protected:

	bool						_Fit_Rectangle				(double &x, double &y, double &dx, double &dy, double XToY_Ratio, bool bShrink);
	bool						_Fit_Rectangle				(CSG_Rect &r, double XToY_Ratio, bool bShrink);

	bool						_Add_Outline_Item			(const char *Title, class PdfPage *pPage, TPDF_Title_Level Level = PDF_TITLE);

	bool						_Draw_Ruler					(const CSG_Rect &r, double zMin, double zMax, bool bHorizontal, bool bAscendent, bool bTickAtTop);
	bool						_Draw_Text					(double x, double y, const char *Text, int Size, int Style, double Angle, int Color, TPDF_Font_Type Font);
	bool						_Draw_Table					(CSG_Rect r, CTable *pTable, int iRecord, int nRecords, double CellHeight, double HeaderHeightRel);
	bool						_Draw_Shape					(CSG_Rect r, CShape *pShape, double xMin, double yMin, double World2PDF, int Style = PDF_STYLE_POLYGON_FILLSTROKE, int Fill_Color = SG_COLOR_GREEN, int Line_Color = SG_COLOR_BLACK, int Line_Width = 0, int Point_Width = 5);

	TPDF_Title_Level			_Get_Lowest_Level_Outline_Item	(void);


private:

	bool						m_Boxes_Space_bPercent;

	int							m_nPages;

	double						m_Boxes_Space;

	CSG_Rect					m_Size_Paper, m_Size_Margins;

	CSG_Rects					m_Boxes_Percent, m_Boxes;

	CSG_String					m_Font_Default;

	CSG_Strings					m_Boxes_ID;

	class PdfDoc				*m_pPDF;

	class PdfPage				*m_pPage;

	class PdfContents			*m_pCanvas;

	class PdfOutlineItem		*m_pLastLevel0OutlineItem, *m_pLastLevel1OutlineItem, *m_pLastLevel2OutlineItem;


	const char *				_Get_Font_Name				(TPDF_Font_Type Font);

	void						_Layout_Set_Boxes			(void);
	void						_Layout_Set_Box				(int iBox);

	bool						_Set_Style_FillStroke		(int Style, int Fill_Color, int Line_Color, int Line_Width);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__SAGA_API__doc_pdf_H
