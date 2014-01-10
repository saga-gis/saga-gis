/**********************************************************
 * Version $Id: doc_html.cpp 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                       docs_html                       //
//                                                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                     doc_html.cpp                      //
//                                                       //
//                 Copyright (C) 2005 by                 //
//                      Victor Olaya                     //
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
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
// USA.                                                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    contact:    Victor Olaya                           //
//                                                       //
//    e-mail:     volaya@saga-gis.org                    //
//                                                       //
///////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "doc_html.h"

//---------------------------------------------------------
#define GRAPH_WIDTH		700
#define GRAPH_HEIGHT	350
#define MAP_WIDTH		700.
#define MAP_HEIGHT		700.
#define OFFSET_X		50
#define OFFSET_Y		50

//---------------------------------------------------------
#define HTML_CODE_OPENING_1	SG_T("<html>\n<head><title>")

#define HTML_CODE_OPENING_2	SG_T("</title>\n")\
	SG_T("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=iso-8859-1\">\n")\
	SG_T("</head>\n")\
	SG_T("<body bgcolor=\"#FFFFFF\" text=\"#000000\">\n")

#define HTML_CODE_CLOSING	SG_T("</body>\n</html>")


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Doc_HTML::CSG_Doc_HTML(){}

CSG_Doc_HTML::~CSG_Doc_HTML(){}

void CSG_Doc_HTML::Open(const SG_Char *Title)
{

	m_sHTMLCode.Clear();

	m_sHTMLCode.Append(HTML_CODE_OPENING_1);
	m_sHTMLCode.Append(Title);
	m_sHTMLCode.Append(HTML_CODE_OPENING_2);

}

bool CSG_Doc_HTML::Save(const SG_Char *Filename)
{

	m_sHTMLCode.Append(HTML_CODE_CLOSING);

	CSG_File	Stream(Filename, SG_FILE_W, false);

	if( Stream.is_Open() )
	{
		Stream.Printf(SG_T("%s"), m_sHTMLCode.c_str());
		return true;
	}

	return false;

}

void CSG_Doc_HTML::AddParagraph(const SG_Char *Text)
{

	m_sHTMLCode.Append(SG_T("<p align=\"left\">"));
	m_sHTMLCode.Append(Text);
	m_sHTMLCode.Append(SG_T("</p>\n"));

}

void CSG_Doc_HTML::AddLineBreak()
{

	m_sHTMLCode.Append(SG_T("</br>"));	

}
void CSG_Doc_HTML::AddHeader(const SG_Char *Text,
							int iOrder)
{

	m_sHTMLCode.Append(SG_T("<h"));
	m_sHTMLCode.Append(SG_Get_String(iOrder,0));
	m_sHTMLCode.Append(SG_T(" align=\"left\">"));
	m_sHTMLCode.Append(Text);
	m_sHTMLCode.Append(SG_T("</h"));
	m_sHTMLCode.Append(SG_Get_String(iOrder,0));
	m_sHTMLCode.Append(SG_T(">\n"));

}

void CSG_Doc_HTML::AddHyperlink(const SG_Char *Text,
								const SG_Char *URL)
{

	m_sHTMLCode.Append(SG_T("<a href=\""));
	m_sHTMLCode.Append(URL);
	m_sHTMLCode.Append(SG_T("\">\n"));
	m_sHTMLCode.Append(Text);
	m_sHTMLCode.Append(SG_T("\n</a>"));

}

CSG_String CSG_Doc_HTML::GetHyperlinkCode(const SG_Char *Text,
									const SG_Char *URL)
{
	CSG_String s;

	s.Append(SG_T("<a href=\""));
	s.Append(URL);
	s.Append(SG_T("\">\n"));
	s.Append(Text);
	s.Append(SG_T("\n</a>"));

	return s;

}
void CSG_Doc_HTML::AddImage(const SG_Char *Filename)
{

	m_sHTMLCode.Append(SG_T("<img src=\""));
	m_sHTMLCode.Append(Filename);
	m_sHTMLCode.Append(SG_T("\">\n"));

}

void CSG_Doc_HTML::AddThumbnail(const SG_Char *Filename,
								int iWidth,
								bool bIsPercent)
{

	m_sHTMLCode.Append(SG_T("<a href=\""));
	m_sHTMLCode.Append(Filename);
	m_sHTMLCode.Append(SG_T("\">\n"));
    m_sHTMLCode.Append(SG_T("<img src=\""));
    m_sHTMLCode.Append(Filename);
    m_sHTMLCode.Append(SG_T("\" width="));
    m_sHTMLCode.Append(SG_Get_String(iWidth,0));
    if (bIsPercent)
    {
		m_sHTMLCode.Append(SG_T("%"));
	}
    m_sHTMLCode.Append(SG_T("></a><br><br>\n"));

}

void CSG_Doc_HTML::AddThumbnails(const SG_Char **Filename,
								int iImages,
								int iThumbnailsPerRow)
{

	int i,j;
	int iImage	= 0;
	int iRows	= (int)ceil((double)iImages / (double)iThumbnailsPerRow);
	int iWidth	= (int)(100.0 / (double)iThumbnailsPerRow);

    m_sHTMLCode.Append(SG_T("<table width=\"99%\" style=\"background-color:transparent;\" border=0 cellspacing=0 cellpadding=2 >\n"));
	for (i=0; i<iRows; i++)
	{
		m_sHTMLCode.Append(SG_T("<tr>\n"));

		for (j=0; j<iThumbnailsPerRow; j++){
			m_sHTMLCode.Append(SG_T("<td width=\""));
			m_sHTMLCode.Append(SG_Get_String(iWidth));
			m_sHTMLCode.Append(SG_T("%\" align=\"center\">"));
			AddThumbnail(Filename[iImage], 100, true);
			iImage++;
			if (iImage >= iImages){
				break;
			}
			m_sHTMLCode.Append(SG_T("</td>"));
		}
		m_sHTMLCode.Append(SG_T("\n</tr>\n"));
	}

	m_sHTMLCode.Append(SG_T("\n</table>\n"));

}

void CSG_Doc_HTML::StartUnorderedList()
{

	m_sHTMLCode.Append(SG_T("<ul>\n"));

}

void CSG_Doc_HTML::StartOrderedList()
{

	m_sHTMLCode.Append(SG_T("<ol>\n"));

}

void CSG_Doc_HTML::CloseUnorderedList()
{

	m_sHTMLCode.Append(SG_T("</ul>\n"));

}

void CSG_Doc_HTML::CloseOrderedList()
{

	m_sHTMLCode.Append(SG_T("</ol>\n"));

}

void CSG_Doc_HTML::AddListElement(const SG_Char *Text)
{

	m_sHTMLCode.Append(SG_T("<li>"));
	m_sHTMLCode.Append(Text);
	m_sHTMLCode.Append(SG_T("</li>\n"));

}

void CSG_Doc_HTML::AddOrderedList(const SG_Char **Text, int iElements)
{

	StartOrderedList();
	for (int i = 0; i < iElements; i++)
	{
		AddListElement(Text[i]);
	}
	CloseOrderedList();

}

void CSG_Doc_HTML::AddUnorderedList(const SG_Char **Text, int iElements)
{

	StartUnorderedList();
	for (int i = 0; i < iElements; i++)
	{
		AddListElement(Text[i]);
	}
	CloseUnorderedList();

}

void CSG_Doc_HTML::AddCurve(const SG_Char *Filename,
								CSG_Points &Data,
								const SG_Char *Description,
								int iGraphType,
								bool bIncludeTableData)
{

	int i;
    int iMag;
	int iNumLines;
    double fMinLine = 0;
	double fWidth;
	double fMax, fMin;
	double fRange, fRangeX;
	double fStep;
    double fX, fY, fY2;
	double fMaxX, fMinX;
	CSG_String sValue;
	CSG_String sTableFilename;
	CSG_Points Points;
	CSG_Doc_SVG SVG;

	m_sHTMLCode.Append(SG_T("<object type=\"image/svg+xml\" width=\""));
	m_sHTMLCode.Append(SG_Get_String(GRAPH_WIDTH + OFFSET_X, 0));
	m_sHTMLCode.Append(SG_T("\" height=\""));
	m_sHTMLCode.Append(SG_Get_String(GRAPH_HEIGHT + OFFSET_Y, 0));
	m_sHTMLCode.Append(SG_T("\" data=\"file://"));
	m_sHTMLCode.Append(Filename);
	m_sHTMLCode.Append(SG_T("\"></object><br>\n"));

	m_sHTMLCode.Append(SG_T("<p align=\"center\"><i>"));
	m_sHTMLCode.Append(Description);
	m_sHTMLCode.Append(SG_T("</i></p>\n"));

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
		if (iGraphType == HTML_GRAPH_TYPE_BARS){
			fMin = M_GET_MIN(0, fMin);
		}
    }

	fRange = fMax - fMin;

	fMaxX = Data[Data.Get_Count() - 1].x;
    fMinX = Data[0].x;

	fRangeX = fMaxX - fMinX;

	SVG.Open(GRAPH_WIDTH + OFFSET_X, GRAPH_HEIGHT + OFFSET_Y);

	if (fMin != fMax){
		iMag	= (int) (log(fMax - fMin) / log(10.0));
		fStep	= (int) pow(10.0, (double) iMag);
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
        fY = (fMinLine + fStep * i - fMin) / (fRange) * GRAPH_HEIGHT;
		fY = GRAPH_HEIGHT - fY;
        if (fY >= 0 && fY <= GRAPH_HEIGHT)
		{
			SVG.Draw_Line(OFFSET_X - 10, fY, GRAPH_WIDTH + OFFSET_X, fY);
			SVG.Draw_Text(OFFSET_X - 10, fY, SG_Get_String(fMinLine + fStep * i, 1), 0, SG_T("Verdana"), 8, SVG_ALIGNMENT_Right);
        }
    }

	if (iGraphType == HTML_GRAPH_TYPE_BARS)
	{
		fWidth = GRAPH_WIDTH / (double) (Data.Get_Count()) ;
	    for (i = 0; i < Data.Get_Count(); i++)
		{
	        fX = OFFSET_X + i * fWidth;
			fY = GRAPH_HEIGHT;
			fY2 = (Data.Get_Y(i) - fMin) / fRange * GRAPH_HEIGHT;
			fY2 = GRAPH_HEIGHT - fY2;
			SVG.Draw_Rectangle(fX, fY, fX + fWidth, fY2, 0x660000, 0x000000);
	    }
	}
	else
	{
		for (i = 0; i < Data.Get_Count(); i++)
		{
			fX = OFFSET_X + (Data.Get_X(i) - fMinX) / fRangeX * GRAPH_WIDTH;
			fY = (Data.Get_Y(i) - fMin) / fRange * GRAPH_HEIGHT;
			fY = GRAPH_HEIGHT - fY;
			Points.Add(fX, fY);
	    }
		SVG.Draw_Line(Points, 4, 0x660000);
	}

	if (fMaxX != fMinX)
	{
		iMag = (int) (log(fMaxX - fMinX) / log(10.0));
		fStep = (double) pow(10.0, (double) iMag);

		iNumLines = (int) ((fMaxX -fMinX) / fStep);

		while (iNumLines < 5)
		{
			fStep = fStep / 2.0;
			iNumLines = (int) ((fMaxX-fMinX) / fStep);
		}
		fMinLine = (int)(((int)(fMinX/fStep)) * fStep);
		while (fMinLine <fMinX)
		{
			fMinLine+=fStep;
		}

		for (i = 0; i < iNumLines + 1; i++)
		{
			if (iGraphType == HTML_GRAPH_TYPE_BARS)
			{
				fX = OFFSET_X + (fMinLine + fStep * i - fMinX) / fRangeX * (GRAPH_WIDTH - fWidth);
			}
			else
			{
				fX = OFFSET_X + (fMinLine + fStep * i - fMinX) / fRangeX * GRAPH_WIDTH;
			}
			if (fX >= OFFSET_X && fX <= GRAPH_WIDTH + OFFSET_X){
				if (fabs(fStep * i +fMinLine) > 100000)
				{
					sValue = SG_Get_String(fStep * i +fMinLine);
				}
				else
				{
					sValue = SG_Get_String(fStep * i + fMinLine, 2, true);
				}
				SVG.Draw_Text(fX, GRAPH_HEIGHT + 10, sValue, 0, SG_T("Verdana"), 8, SVG_ALIGNMENT_Center);
				SVG.Draw_Line(fX, GRAPH_HEIGHT, fX, GRAPH_HEIGHT - 5);
			}
		}
    }

	SVG.Draw_Line(OFFSET_X, 0, OFFSET_X, GRAPH_HEIGHT, 4);
	SVG.Draw_Line(OFFSET_X, GRAPH_HEIGHT, OFFSET_X + GRAPH_WIDTH, GRAPH_HEIGHT, 4);

	SVG.Save(Filename);

	if (bIncludeTableData)
	{
		CSG_Doc_HTML HTMLDoc;
		HTMLDoc.Open(_TL("Data Table"));

		sTableFilename = Filename;
		sTableFilename.Append(SG_T(".htm"));

		m_sHTMLCode.Append(SG_T("<a href=\"file://"));
		m_sHTMLCode.Append(sTableFilename);
		m_sHTMLCode.Append(SG_T("\">"));
		m_sHTMLCode.Append(_TL("Data Table"));
		m_sHTMLCode.Append(SG_T("</a><br><br>\n"));

		HTMLDoc._AddBicolumTable(&Data);
		HTMLDoc.Save(sTableFilename);

	}


}//method

void CSG_Doc_HTML::_AddBicolumTable(CSG_Points *pData)
{

	int i;

    m_sHTMLCode.Append(SG_T("<table width=\"99%\" style=\"background-color:transparent;\" border=0 cellspacing=0 cellpadding=2 >\n"));
	m_sHTMLCode.Append(
		SG_T("<tr bgcolor=\"#CCCCCC\">\n")
		SG_T("<td width=\"50%\" align=\"center\">X</td><td width=\"50%\" align=\"center\">Y</td>\n")
		SG_T("</tr>\n")
	);

	for (i=0; i<pData->Get_Count(); i++)
	{
		m_sHTMLCode.Append(SG_T("<tr>\n"));
		m_sHTMLCode.Append(SG_T("<td width=\"50%\" align=\"center\">"));
		m_sHTMLCode.Append(SG_Get_String(pData->Get_X(i),2));
		m_sHTMLCode.Append(SG_T("</td>"));
		m_sHTMLCode.Append(SG_T("<td width=\"50%\" align=\"center\">"));
		m_sHTMLCode.Append(SG_Get_String(pData->Get_Y(i),2));
		m_sHTMLCode.Append(SG_T("</td>"));
		m_sHTMLCode.Append(SG_T("\n</tr>\n"));
	}

	m_sHTMLCode.Append(SG_T("\n</table>\n"));
	m_sHTMLCode.Append(SG_T("<p align=\"center\"><i>"));
	m_sHTMLCode.Append(_TL("Data Table"));
	m_sHTMLCode.Append(SG_T("</i></p>\n"));

}

void CSG_Doc_HTML::AddTable(const SG_Char ***Table,
								int iRows,
								int iCols,
								const SG_Char *Description)
{

	int i,j;
	int iWidth = (int) (100/iCols);

    m_sHTMLCode.Append(SG_T("<table width=\"99%\" style=\"background-color:transparent;\" border=0 cellspacing=0 cellpadding=2 >\n"));
	for (i=0; i<iRows; i++)
	{
		if (i==0)
		{
			m_sHTMLCode.Append(SG_T("<tr bgcolor=\"#CCCCCC\">\n"));
		}
		else
		{
			m_sHTMLCode.Append(SG_T("<tr>\n"));
		}
		for (j=0; j<iCols; j++){
			m_sHTMLCode.Append(SG_T("<td width=\""));
			m_sHTMLCode.Append(SG_Get_String(iWidth,0));
			m_sHTMLCode.Append(SG_T("%\" align=\"center\">"));
			try
			{
				m_sHTMLCode.Append(Table[i][j]);
			}
			catch(...){}
			m_sHTMLCode.Append(SG_T("</td>"));
		}
		m_sHTMLCode.Append(SG_T("\n</tr>\n"));
	}

	m_sHTMLCode.Append(SG_T("\n</table>\n"));
	m_sHTMLCode.Append(SG_T("<p align=\"center\"><i>"));
	m_sHTMLCode.Append(Description);
	m_sHTMLCode.Append(SG_T("</i></p>\n"));

}

void CSG_Doc_HTML::AddTable(CSG_Table *pTable)
{

	int i,j;
	int iWidth = (int) (100/pTable->Get_Field_Count());

    m_sHTMLCode.Append(SG_T("<table width=\"99%\" style=\"background-color:transparent;\" border=0 cellspacing=0 cellpadding=2 >\n"));

	m_sHTMLCode.Append(SG_T("<tr bgcolor=\"#CCCCCC\">\n"));
	for (i=0; i<pTable->Get_Field_Count(); i++){
		m_sHTMLCode.Append(SG_T("<td width=\""));
		m_sHTMLCode.Append(SG_Get_String(iWidth,0));
		m_sHTMLCode.Append(SG_T("%\" align=\"center\">"));
		m_sHTMLCode.Append(pTable->Get_Field_Name(i));
		m_sHTMLCode.Append(SG_T("</td>"));
	}
	m_sHTMLCode.Append(SG_T("\n</tr>\n"));

	for (i=0; i<pTable->Get_Record_Count(); i++)
	{
		m_sHTMLCode.Append(SG_T("<tr>\n"));
		for (j=0; j<pTable->Get_Field_Count(); j++){
			m_sHTMLCode.Append(SG_T("<td width=\""));
			m_sHTMLCode.Append(SG_Get_String(iWidth,0));
			m_sHTMLCode.Append(SG_T("%\" align=\"center\">"));
			try
			{
				m_sHTMLCode.Append(pTable->Get_Record(i)->asString(j));
			}
			catch(...){}
			m_sHTMLCode.Append(SG_T("</td>"));
		}
		m_sHTMLCode.Append(SG_T("\n</tr>\n"));
	}

	m_sHTMLCode.Append(SG_T("\n</table>\n"));
	m_sHTMLCode.Append(SG_T("<p align=\"center\"><i>"));
	m_sHTMLCode.Append(pTable->Get_Name());
	m_sHTMLCode.Append(SG_T("</i></p>\n"));

}

bool CSG_Doc_HTML::_Draw_Shape(CSG_Doc_SVG &SVG, CSG_Shape *pShape, CSG_Rect GlobalRect, int Fill_Color, int Line_Color, int Line_Width, int Point_Width)
{
	if( pShape && pShape->is_Valid() )
	{
		int			iPart, iPoint;
		double		x,y;
		TSG_Point	Point;
		CSG_Points	Points;
		double		dWidth, dHeight;
		double		dOffsetX, dOffsetY;

		if (GlobalRect.Get_XRange() / GlobalRect.Get_YRange() > MAP_WIDTH / MAP_HEIGHT)
		{
			dWidth	= MAP_WIDTH * .95;
			dHeight	= MAP_HEIGHT / GlobalRect.Get_XRange() * GlobalRect.Get_YRange();
		}
		else{
			dHeight	= MAP_HEIGHT * .95;
			dWidth	= MAP_WIDTH / GlobalRect.Get_YRange() * GlobalRect.Get_XRange();
		}

		dOffsetX = (MAP_WIDTH  - dWidth)  / 2.;
		dOffsetY = (MAP_HEIGHT - dHeight) / 2.;

		SVG.Draw_Rectangle(0,0,MAP_WIDTH,MAP_HEIGHT,SG_COLOR_NONE,SG_COLOR_BLACK,1);

		for(iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
		{
			Points.Clear();

			for(iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
			{
				Point = pShape->Get_Point(iPoint, iPart);
				x = dOffsetX + (Point.x - GlobalRect.Get_XMin()) / GlobalRect.Get_XRange() * dWidth;
				y = (Point.y - GlobalRect.Get_YMin()) / GlobalRect.Get_YRange() * dHeight;
				y = MAP_HEIGHT - y - dOffsetY;
				Points.Add(x, y);
			}

			switch( pShape->Get_Type() )
			{
			case SHAPE_TYPE_Point:
			case SHAPE_TYPE_Points:
				for(iPoint=0; iPoint<Points.Get_Count(); iPoint++)
				{
					SVG.Draw_Circle(Points[iPoint].x, Points[iPoint].y, Point_Width, Fill_Color, Line_Color, Line_Width);
				}
				break;

			case SHAPE_TYPE_Line:
				SVG.Draw_Line(Points, Line_Width, Line_Color);
				break;

			case SHAPE_TYPE_Polygon:
				if( ((CSG_Shape_Polygon *)pShape)->is_Lake(iPart) )
				{
					SVG.Draw_Polygon(Points, SG_COLOR_WHITE, Line_Color, Line_Width);
				}
				else
				{
					SVG.Draw_Polygon(Points, Fill_Color, Line_Color, Line_Width);
				}
				break;
			}
		}

		return( true );
	}

	return( false );
}

bool CSG_Doc_HTML::Draw_Shapes(CSG_Shapes *pShapes, const SG_Char *Filename, int Fill_Color, int Line_Color, int Line_Width)
{

	CSG_Doc_SVG	SVG;
	CSG_Rect	r;

	if( pShapes && pShapes->is_Valid())
	{
		r = pShapes->Get_Extent();
		SVG.Open((int)MAP_WIDTH, (int)MAP_HEIGHT);

		for(int iShape=0; iShape<pShapes->Get_Count(); iShape++)
		{
			_Draw_Shape(SVG, pShapes->Get_Shape(iShape), r, Fill_Color, Line_Color, 1, 3);
		}

		SVG.Save(Filename);

		m_sHTMLCode.Append(SG_T("<center>\n<object type=\"image/svg+xml\" width=\""));
		m_sHTMLCode.Append(SG_Get_String(MAP_WIDTH, 0));
		m_sHTMLCode.Append(SG_T("\" height=\""));
		m_sHTMLCode.Append(SG_Get_String(MAP_HEIGHT, 0));
		m_sHTMLCode.Append(SG_T("\" data=\"file://"));
		m_sHTMLCode.Append(Filename);
		m_sHTMLCode.Append(SG_T("\"></object></center><br>\n"));

		return( true );

	}

	return( false );
}
