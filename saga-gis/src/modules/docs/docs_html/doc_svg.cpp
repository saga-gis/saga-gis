/**********************************************************
 * Version $Id: doc_svg.cpp 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                       docs_html                       //
//                                                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                     doc_svg.cpp                       //
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "doc_svg.h"
#include <time.h>

//---------------------------------------------------------
#define SVG_CODE_OPENING_1	SG_T("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n")\
							SG_T("<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\" ")\
							SG_T("\"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">\n")\
							SG_T("<svg version=\"1.1\" width=\"")

#define SVG_CODE_OPENING_2	SG_T("\"\n xmlns=\"http://www.w3.org/2000/svg\">\n")

#define SVG_CODE_CLOSING	SG_T("</svg>")

//---------------------------------------------------------
const SG_Char *g_Unit	= SG_T("");


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Doc_SVG::CSG_Doc_SVG(){}

CSG_Doc_SVG::~CSG_Doc_SVG(){}

void CSG_Doc_SVG::Open(int iWidth, int iHeight)
{

	m_sSVGCode.Append(SVG_CODE_OPENING_1);
	m_sSVGCode.Append(SG_Get_String(iWidth,0));
	m_sSVGCode.Append(SG_T("\" height=\""));
	m_sSVGCode.Append(SG_Get_String(iHeight,0));
	m_sSVGCode.Append(SVG_CODE_OPENING_2);

	m_iWidth = iWidth;
	m_iHeight = iHeight;

}

bool CSG_Doc_SVG::Save(const SG_Char *Filename)
{

	m_sSVGCode.Append(SVG_CODE_CLOSING);

	CSG_File	Stream(Filename, SG_FILE_W, false);

	if( Stream.is_Open() )
	{
		Stream.Printf(SG_T("%s"), m_sSVGCode.c_str());
		return true;
	}
	
	return false;

}

void CSG_Doc_SVG::_AddAttribute(const SG_Char *Attribute, 
							   const SG_Char *Value)
{

		m_sSVGCode.Append(Attribute);
		m_sSVGCode.Append(SG_T("=\""));
		m_sSVGCode.Append(Value);
		m_sSVGCode.Append(SG_T("\" "));		

}

void CSG_Doc_SVG::_AddAttribute(const SG_Char *Attribute, 
							   int iValue)
{

	_AddAttribute(Attribute, SG_Get_String(iValue, 0));
}

void CSG_Doc_SVG::_AddAttribute(const SG_Char *Attribute, 
							   double dValue)
{

	_AddAttribute(Attribute, SG_Get_String(dValue, 2));

}

void CSG_Doc_SVG::Draw_Circle(double x, 
							 double y, 
							 double Radius, 
							 int Fill_Color, 
							 int Line_Color, 
							 double Line_Width)
{
	
	CSG_String sWidth;

	m_sSVGCode.Append(SG_T("<circle "));
	_AddAttribute(SG_T("cx"), x);
	_AddAttribute(SG_T("cy"), y);
	_AddAttribute(SG_T("r"), Radius);
	sWidth.Append(SG_Get_String(Line_Width,2));
	sWidth.Append(g_Unit);
	_AddAttribute(SG_T("stroke-width"), sWidth);
	_AddAttribute(SG_T("stroke"), _Get_SVGColor(Line_Color));
	_AddAttribute(SG_T("fill"), _Get_SVGColor(Fill_Color));
	m_sSVGCode.Append(SG_T("/>\n"));

}

void CSG_Doc_SVG::Draw_LinkedCircle(double x, 
								 double y, 
								 double Radius, 
								 const SG_Char *Link,
								 int Fill_Color, 
								 int Line_Color, 
								 double Line_Width)
{
	
	CSG_String sWidth;
	CSG_String sLink;

	m_sSVGCode.Append(SG_T("<circle "));
	_AddAttribute(SG_T("cx"), x);
	_AddAttribute(SG_T("cy"), y);
	_AddAttribute(SG_T("r"), Radius);
	sWidth.Append(SG_Get_String(Line_Width,2));
	sWidth.Append(g_Unit);
	_AddAttribute(SG_T("stroke-width"), sWidth);
	_AddAttribute(SG_T("stroke"), _Get_SVGColor(Line_Color));
	_AddAttribute(SG_T("fill"), _Get_SVGColor(Fill_Color));
	sLink = SG_T("window.open('");
	sLink.Append(Link);
	sLink.Append(SG_T("')"));
	_AddAttribute(SG_T("onclick"), sLink);
	m_sSVGCode.Append(SG_T("/>\n"));

}

void CSG_Doc_SVG::Draw_Line(double xa, 
						   double ya, 
						   double xb, 
						   double yb, 
						   double Width, 
						   int Color)
{

	CSG_String sWidth;

	m_sSVGCode.Append(SG_T("<line "));
	_AddAttribute(SG_T("x1"), xa);
	_AddAttribute(SG_T("x2"), xb);
	_AddAttribute(SG_T("y1"), ya);
	_AddAttribute(SG_T("y2"), yb);
	sWidth.Append(SG_Get_String(Width,2));
	sWidth.Append(g_Unit);
	_AddAttribute(SG_T("stroke-width"), sWidth);	_AddAttribute(SG_T("stroke"), _Get_SVGColor(Color));
	m_sSVGCode.Append(SG_T("/>\n"));

}

void CSG_Doc_SVG::Draw_Line(CSG_Points &Points, 
						   double Width, 
						   int Color)
{
	int i;
	CSG_String sPoints;
	CSG_String sWidth;
	
	for (i = 0; i < Points.Get_Count(); i++)
	{
		sPoints.Append(SG_Get_String(Points.Get_X(i),2));
		sPoints.Append(SG_T(","));
		sPoints.Append(SG_Get_String(Points.Get_Y(i),2));
		sPoints.Append(SG_T(" "));
	}
	m_sSVGCode.Append(SG_T("<polyline "));
	_AddAttribute(SG_T("points"), sPoints);	
	sWidth.Append(SG_Get_String(Width,2));
	sWidth.Append(g_Unit);
	_AddAttribute(SG_T("stroke-width"), sWidth);
	_AddAttribute(SG_T("stroke"), _Get_SVGColor(Color));
	_AddAttribute(SG_T("fill"), SG_T("none"));
	m_sSVGCode.Append(SG_T("/>\n"));

}

void CSG_Doc_SVG::Draw_Rectangle(double xa, 
								double ya, 
								double xb, 
								double yb, 
								int Fill_Color, 
								int Line_Color, 
								double Line_Width)
{
	CSG_Points	Points;

	Points.Add(xa, ya);
	Points.Add(xb, ya);
	Points.Add(xb, yb);
	Points.Add(xa, yb);

	Draw_Polygon(Points, Fill_Color, Line_Color, Line_Width);
}

void CSG_Doc_SVG::Draw_Rectangle(const CSG_Rect &r, 
								int Fill_Color, 
								int Line_Color, 
								double Line_Width)
{
	Draw_Rectangle(r.Get_XMin(), r.Get_YMin(), r.Get_XMax(), r.Get_YMax(), Fill_Color, Line_Color, Line_Width) ;
}

void CSG_Doc_SVG::Draw_Polygon(CSG_Points &Points, 
							  int Fill_Color, 
							  int Line_Color, 
							  double Line_Width)
{
	if( Points.Get_Count() > 2 )
	{
		int i;
		CSG_String sPoints;
		CSG_String sWidth;
		
		for (i = 0; i < Points.Get_Count(); i++)
		{
			sPoints.Append(SG_Get_String(Points.Get_X(i),2));
			sPoints.Append(SG_T(","));
			sPoints.Append(SG_Get_String(Points.Get_Y(i),2));
			sPoints.Append(SG_T(" "));
		}
		m_sSVGCode.Append(SG_T("<polygon "));
		_AddAttribute(SG_T("points"), sPoints);
		sWidth.Append(SG_Get_String(Line_Width,2));
		sWidth.Append(g_Unit);
		_AddAttribute(SG_T("stroke-width"), sWidth);				
		_AddAttribute(SG_T("stroke"), _Get_SVGColor(Line_Color));
		_AddAttribute(SG_T("fill"), _Get_SVGColor(Fill_Color));
		m_sSVGCode.Append(SG_T("/>\n"));
	}
}

void CSG_Doc_SVG::Draw_LinkedPolygon(CSG_Points &Points, 
							  const SG_Char* Link,
							  int Fill_Color, 
							  int Line_Color, 
							  double Line_Width)
{
	if( Points.Get_Count() > 2 )
	{
		int i;
		CSG_String sPoints;
		CSG_String sWidth;
		CSG_String sLink;
		
		for (i = 0; i < Points.Get_Count(); i++)
		{
			sPoints.Append(SG_Get_String(Points.Get_X(i),2));
			sPoints.Append(SG_T(","));
			sPoints.Append(SG_Get_String(Points.Get_Y(i),2));
			sPoints.Append(SG_T(" "));
		}
		m_sSVGCode.Append(SG_T("<polygon "));
		_AddAttribute(SG_T("points"), sPoints);
		sWidth.Append(SG_Get_String(Line_Width,2));
		sWidth.Append(g_Unit);
		_AddAttribute(SG_T("stroke-width"), sWidth);				
		_AddAttribute(SG_T("stroke"), _Get_SVGColor(Line_Color));
		_AddAttribute(SG_T("fill"), _Get_SVGColor(Fill_Color));
		sLink = SG_T("window.open('");
		sLink.Append(Link);
		sLink.Append(SG_T("')"));
		_AddAttribute(SG_T("onclick"), sLink);
		m_sSVGCode.Append(SG_T("/>\n"));
	}
}

void CSG_Doc_SVG::Draw_Text(double x, 
						 double y, 
						 const SG_Char *Text, 
						 int Color, 
						 const SG_Char* Font, 
						 double dSize,
						 TSG_SVG_Alignment iAlignment)
{

	CSG_String sAlignments[] = {SG_T(""), SG_T("middle"), SG_T("end")};
	CSG_String sSize;
	
	sSize.Append(SG_Get_String(dSize,2));
	sSize.Append(g_Unit);
	
	m_sSVGCode.Append(SG_T("<text "));
	_AddAttribute(SG_T("x"), x);	
	_AddAttribute(SG_T("y"), y);
	_AddAttribute(SG_T("font-family"), Font);
	_AddAttribute(SG_T("fill"), _Get_SVGColor(Color));
	_AddAttribute(SG_T("font-size"), sSize);
	_AddAttribute(SG_T("text-anchor"), sAlignments[iAlignment]);
	m_sSVGCode.Append(SG_T(">\n"));
	m_sSVGCode.Append(Text);
	m_sSVGCode.Append(SG_T("</text>"));

}

CSG_String CSG_Doc_SVG::_Get_SVGColor(int iColor)
{

	CSG_String s;

	if (iColor == SG_COLOR_NONE)
	{
		return SG_T("none");
	}
	else if (iColor == SG_COLOR_RANDOM)
	{
		s.Append(SG_T("rgb("));
		s.Append(SG_Get_String((int)(255.0 * (double)rand() / (double)RAND_MAX),0));
		s.Append(SG_T(","));
		s.Append(SG_Get_String((int)(255.0 * (double)rand() / (double)RAND_MAX),0));
		s.Append(SG_T(","));
		s.Append(SG_Get_String((int)(255.0 * (double)rand() / (double)RAND_MAX),0));
		s.Append(SG_T(")"));

		return s;
	}
	else
	{
		s.Append(SG_T("rgb("));
		s.Append(SG_Get_String(SG_GET_R(iColor),0));
		s.Append(SG_T(","));
		s.Append(SG_Get_String(SG_GET_G(iColor),0));
		s.Append(SG_T(","));
		s.Append(SG_Get_String(SG_GET_B(iColor),0));
		s.Append(SG_T(")"));

		return s;
	}

}
