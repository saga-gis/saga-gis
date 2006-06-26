
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
//                   html_document.cpp                   //
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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
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

#include "html_document.h"

#define GRAPH_WIDTH		700
#define GRAPH_HEIGHT	350
#define MAP_WIDTH		700.
#define MAP_HEIGHT		700.
#define OFFSET_X		50
#define OFFSET_Y		50

CAPI_String OPENING_HTML_CODE_1 ("<html>\n"
								"<head>"
								"<title>");

CAPI_String OPENING_HTML_CODE_2 ("</title>\n"
								"<meta http-equiv=\"Content-Type\" content=\"text/html; charset=iso-8859-1\">\n"
								"</head>\n"
								"<body bgcolor=\"#FFFFFF\" text=\"#000000\">\n");

CAPI_String CLOSING_HTML_CODE ("</body>\n"
									"</html>");

CHTML_Document::CHTML_Document(){}

CHTML_Document::~CHTML_Document(){}

void CHTML_Document::Open(const char *Title)
{

	m_sHTMLCode.Clear();

	m_sHTMLCode.Append(OPENING_HTML_CODE_1);
	m_sHTMLCode.Append(Title);
	m_sHTMLCode.Append(OPENING_HTML_CODE_2);

}

bool CHTML_Document::Save(const char *Filename)
{

	m_sHTMLCode.Append(CLOSING_HTML_CODE);

	FILE	*Stream;

	Stream	= fopen(Filename, "w");

	if( Stream )
	{
		fprintf(Stream, "%s", m_sHTMLCode.c_str());
		fclose(Stream);
		return true;
	}

	return false;

}

void CHTML_Document::AddParagraph(const char *Text)
{

	m_sHTMLCode.Append("<p align=\"left\">");
	m_sHTMLCode.Append(Text);
	m_sHTMLCode.Append("</p>\n");

}

void CHTML_Document::AddLineBreak()
{

	m_sHTMLCode.Append("</br>");	

}
void CHTML_Document::AddHeader(const char *Text,
							int iOrder)
{

	m_sHTMLCode.Append("<h");
	m_sHTMLCode.Append(API_Get_String(iOrder,0));
	m_sHTMLCode.Append(" align=\"left\">");
	m_sHTMLCode.Append(Text);
	m_sHTMLCode.Append("</h");
	m_sHTMLCode.Append(API_Get_String(iOrder,0));
	m_sHTMLCode.Append(">\n");

}

void CHTML_Document::AddHyperlink(const char *Text,
								const char *URL)
{

	m_sHTMLCode.Append("<a href=\"");
	m_sHTMLCode.Append(URL);
	m_sHTMLCode.Append("\">\n");
	m_sHTMLCode.Append(Text);
	m_sHTMLCode.Append("\n</a>");

}

CAPI_String CHTML_Document::GetHyperlinkCode(const char *Text,
									const char *URL)
{
	CAPI_String s;

	s.Append("<a href=\"");
	s.Append(URL);
	s.Append("\">\n");
	s.Append(Text);
	s.Append("\n</a>");

	return s;

}
void CHTML_Document::AddImage(const char *Filename)
{

	m_sHTMLCode.Append("<img src=\"");
	m_sHTMLCode.Append(Filename);
	m_sHTMLCode.Append("\">\n");

}

void CHTML_Document::AddThumbnail(const char *Filename,
								int iWidth,
								bool bIsPercent)
{

	m_sHTMLCode.Append("<a href=\"");
	m_sHTMLCode.Append(Filename);
	m_sHTMLCode.Append("\">\n");
    m_sHTMLCode.Append("<img src=\"");
    m_sHTMLCode.Append(Filename);
    m_sHTMLCode.Append("\" width=");
    m_sHTMLCode.Append(API_Get_String(iWidth,0));
    if (bIsPercent)
    {
		m_sHTMLCode.Append("%");
	}
    m_sHTMLCode.Append("></a><br><br>\n");

}

void CHTML_Document::AddThumbnails(const char **Filename,
								int iImages,
								int iThumbnailsPerRow)
{

	int i,j;
	int iImage	= 0;
	int iRows	= (int)ceil((double)iImages / (double)iThumbnailsPerRow);
	int iWidth	= (int)(100.0 / (double)iThumbnailsPerRow);

    m_sHTMLCode.Append("<table width=\"99%\" style=\"background-color:transparent;\" border=0 cellspacing=0 cellpadding=2 >\n");
	for (i=0; i<iRows; i++)
	{
		m_sHTMLCode.Append("<tr>\n");

		for (j=0; j<iThumbnailsPerRow; j++){
			m_sHTMLCode.Append("<td width=\"");
			m_sHTMLCode.Append(API_Get_String(iWidth));
			m_sHTMLCode.Append("%\" align=\"center\">");
			AddThumbnail(Filename[iImage], 100, true);
			iImage++;
			if (iImage >= iImages){
				break;
			}
			m_sHTMLCode.Append("</td>");
		}
		m_sHTMLCode.Append("\n</tr>\n");
	}

	m_sHTMLCode.Append("\n</table>\n");

}

void CHTML_Document::StartUnorderedList()
{

	m_sHTMLCode.Append("<ul>\n");

}

void CHTML_Document::StartOrderedList()
{

	m_sHTMLCode.Append("<ol>\n");

}

void CHTML_Document::CloseUnorderedList()
{

	m_sHTMLCode.Append("</ul>\n");

}

void CHTML_Document::CloseOrderedList()
{

	m_sHTMLCode.Append("</ol>\n");

}

void CHTML_Document::AddListElement(const char *Text)
{

	m_sHTMLCode.Append("<li>");
	m_sHTMLCode.Append(Text);
	m_sHTMLCode.Append("</li>\n");

}

void CHTML_Document::AddOrderedList(const char **Text, int iElements)
{

	StartOrderedList();
	for (int i = 0; i < iElements; i++)
	{
		AddListElement(Text[i]);
	}
	CloseOrderedList();

}

void CHTML_Document::AddUnorderedList(const char **Text, int iElements)
{

	StartUnorderedList();
	for (int i = 0; i < iElements; i++)
	{
		AddListElement(Text[i]);
	}
	CloseUnorderedList();

}

void CHTML_Document::AddCurve(const char *Filename,
								CAPI_dPoints &Data,
								const char *Description,
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
	CAPI_String sValue;
	CAPI_String sTableFilename;
	CAPI_dPoints Points;
	CSVG_Graph SVG;

	m_sHTMLCode.Append("<object type=\"image/svg+xml\" width=\"");
	m_sHTMLCode.Append(API_Get_String(GRAPH_WIDTH + OFFSET_X, 0));
	m_sHTMLCode.Append("\" height=\"");
	m_sHTMLCode.Append(API_Get_String(GRAPH_HEIGHT + OFFSET_Y, 0));
	m_sHTMLCode.Append("\" data=\"file://");
	m_sHTMLCode.Append(Filename);
	m_sHTMLCode.Append("\"></object><br>\n");

	m_sHTMLCode.Append("<p align=\"center\"><i>");
	m_sHTMLCode.Append(Description);
	m_sHTMLCode.Append("</i></p>\n");

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
			SVG.Draw_Text(OFFSET_X - 10, fY, API_Get_String(fMinLine + fStep * i, 1), 0, "Verdana", 8, "", SVG_ALIGNMENT_Right);
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
		SVG.Draw_Line(Points, 4, "", 0x660000);
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
					sValue = API_Get_String(fStep * i +fMinLine);
				}
				else
				{
					sValue = API_Get_String(fStep * i + fMinLine, 2, true);
				}
				SVG.Draw_Text(fX, GRAPH_HEIGHT + 10, sValue, 0, "Verdana", 8, "", SVG_ALIGNMENT_Center);
				SVG.Draw_Line(fX, GRAPH_HEIGHT, fX, GRAPH_HEIGHT - 5);
			}
		}
    }

	SVG.Draw_Line(OFFSET_X, 0, OFFSET_X, GRAPH_HEIGHT, 4);
	SVG.Draw_Line(OFFSET_X, GRAPH_HEIGHT, OFFSET_X + GRAPH_WIDTH, GRAPH_HEIGHT, 4);

	SVG.Save(Filename);

	if (bIncludeTableData)
	{
		CHTML_Document HTMLDoc;
		HTMLDoc.Open(LNG("Data Table"));

		sTableFilename = Filename;
		sTableFilename.Append(".htm");

		m_sHTMLCode.Append("<a href=\"file://");
		m_sHTMLCode.Append(sTableFilename);
		m_sHTMLCode.Append("\">");
		m_sHTMLCode.Append(LNG("Data Table"));
		m_sHTMLCode.Append("</a><br><br>\n");

		HTMLDoc._AddBicolumTable(&Data);
		HTMLDoc.Save(sTableFilename);

	}


}//method

void CHTML_Document::_AddBicolumTable(CAPI_dPoints *pData)
{

	int i;

    m_sHTMLCode.Append("<table width=\"99%\" style=\"background-color:transparent;\" border=0 cellspacing=0 cellpadding=2 >\n");
	m_sHTMLCode.Append("<tr bgcolor=\"#CCCCCC\">\n"
						"<td width=\"50%\" align=\"center\">X</td><td width=\"50%\" align=\"center\">Y</td>\n"
						"</tr>\n");

	for (i=0; i<pData->Get_Count(); i++)
	{
		m_sHTMLCode.Append("<tr>\n");
		m_sHTMLCode.Append("<td width=\"50%\" align=\"center\">");
		m_sHTMLCode.Append(API_Get_String(pData->Get_X(i),2));
		m_sHTMLCode.Append("</td>");
		m_sHTMLCode.Append("<td width=\"50%\" align=\"center\">");
		m_sHTMLCode.Append(API_Get_String(pData->Get_Y(i),2));
		m_sHTMLCode.Append("</td>");
		m_sHTMLCode.Append("\n</tr>\n");
	}

	m_sHTMLCode.Append("\n</table>\n");
	m_sHTMLCode.Append("<p align=\"center\"><i>");
	m_sHTMLCode.Append(LNG("Data Table"));
	m_sHTMLCode.Append("</i></p>\n");

}

void CHTML_Document::AddTable(const char ***Table,
								int iRows,
								int iCols,
								const char *Description)
{

	int i,j;
	int iWidth = (int) (100/iCols);

    m_sHTMLCode.Append("<table width=\"99%\" style=\"background-color:transparent;\" border=0 cellspacing=0 cellpadding=2 >\n");
	for (i=0; i<iRows; i++)
	{
		if (i==0)
		{
			m_sHTMLCode.Append("<tr bgcolor=\"#CCCCCC\">\n");
		}
		else
		{
			m_sHTMLCode.Append("<tr>\n");
		}
		for (j=0; j<iCols; j++){
			m_sHTMLCode.Append("<td width=\"");
			m_sHTMLCode.Append(API_Get_String(iWidth,0));
			m_sHTMLCode.Append("%\" align=\"center\">");
			try
			{
				m_sHTMLCode.Append(Table[i][j]);
			}
			catch(...){}
			m_sHTMLCode.Append("</td>");
		}
		m_sHTMLCode.Append("\n</tr>\n");
	}

	m_sHTMLCode.Append("\n</table>\n");
	m_sHTMLCode.Append("<p align=\"center\"><i>");
	m_sHTMLCode.Append(Description);
	m_sHTMLCode.Append("</i></p>\n");

}

void CHTML_Document::AddTable(CTable *pTable)
{

	int i,j;
	int iWidth = (int) (100/pTable->Get_Field_Count());

    m_sHTMLCode.Append("<table width=\"99%\" style=\"background-color:transparent;\" border=0 cellspacing=0 cellpadding=2 >\n");

	m_sHTMLCode.Append("<tr bgcolor=\"#CCCCCC\">\n");
	for (i=0; i<pTable->Get_Field_Count(); i++){
		m_sHTMLCode.Append("<td width=\"");
		m_sHTMLCode.Append(API_Get_String(iWidth,0));
		m_sHTMLCode.Append("%\" align=\"center\">");
		m_sHTMLCode.Append(pTable->Get_Field_Name(i));
		m_sHTMLCode.Append("</td>");
	}
	m_sHTMLCode.Append("\n</tr>\n");

	for (i=0; i<pTable->Get_Record_Count(); i++)
	{
		m_sHTMLCode.Append("<tr>\n");
		for (j=0; j<pTable->Get_Field_Count(); j++){
			m_sHTMLCode.Append("<td width=\"");
			m_sHTMLCode.Append(API_Get_String(iWidth,0));
			m_sHTMLCode.Append("%\" align=\"center\">");
			try
			{
				m_sHTMLCode.Append(pTable->Get_Record(i)->asString(j));
			}
			catch(...){}
			m_sHTMLCode.Append("</td>");
		}
		m_sHTMLCode.Append("\n</tr>\n");
	}

	m_sHTMLCode.Append("\n</table>\n");
	m_sHTMLCode.Append("<p align=\"center\"><i>");
	m_sHTMLCode.Append(pTable->Get_Name());
	m_sHTMLCode.Append("</i></p>\n");

}

bool CHTML_Document::_Draw_Shape(CSVG_Graph &SVG, CShape *pShape, CGEO_Rect GlobalRect, int Fill_Color, int Line_Color, int Line_Width, int Point_Width)
{
	if( pShape && pShape->is_Valid() )
	{
		int				iPart, iPoint;
		double			x,y;
		TGEO_Point		Point;
		CAPI_dPoints	Points;
		double			dWidth, dHeight;
		double			dOffsetX, dOffsetY;

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

		SVG.Draw_Rectangle(0,0,MAP_WIDTH,MAP_HEIGHT,COLOR_DEF_NONE,COLOR_DEF_BLACK,1,"%");

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
					SVG.Draw_Circle(Points[iPoint].x, Points[iPoint].y, Point_Width, Fill_Color, Line_Color, Line_Width, "%");
				}
				break;

			case SHAPE_TYPE_Line:
				SVG.Draw_Line(Points, Line_Width, "", Line_Color);
				break;

			case SHAPE_TYPE_Polygon:
				if( ((CShape_Polygon *)pShape)->is_Lake(iPart) )
				{
					SVG.Draw_Polygon(Points, COLOR_DEF_WHITE, Line_Color, Line_Width, "");
				}
				else
				{
					SVG.Draw_Polygon(Points, Fill_Color, Line_Color, Line_Width, "");
				}
				break;
			}
		}

		return( true );
	}

	return( false );
}

bool CHTML_Document::Draw_Shapes(CShapes *pShapes, const char *Filename, int Fill_Color, int Line_Color, int Line_Width)
{

	CSVG_Graph SVG;
	CGEO_Rect r;

	if( pShapes && pShapes->is_Valid())
	{
		r = pShapes->Get_Extent();
		SVG.Open((int)MAP_WIDTH, (int)MAP_HEIGHT);

		for(int iShape=0; iShape<pShapes->Get_Count(); iShape++)
		{
			_Draw_Shape(SVG, pShapes->Get_Shape(iShape), r, Fill_Color, Line_Color, 1, 3);
		}

		SVG.Save(Filename);

		m_sHTMLCode.Append("<center>\n<object type=\"image/svg+xml\" width=\"");
		m_sHTMLCode.Append(API_Get_String(MAP_WIDTH, 0));
		m_sHTMLCode.Append("\" height=\"");
		m_sHTMLCode.Append(API_Get_String(MAP_HEIGHT, 0));
		m_sHTMLCode.Append("\" data=\"file://");
		m_sHTMLCode.Append(Filename);
		m_sHTMLCode.Append("\"></object></center><br>\n");

		return( true );

	}

	return( false );
}
