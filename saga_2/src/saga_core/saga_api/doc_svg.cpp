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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "doc_svg.h"
#include <time.h>

//---------------------------------------------------------
#define SVG_CODE_OPENING_1	"<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"\
							"<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\" "\
							"\"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">\n"\
							"<svg version=\"1.1\" width=\""

#define SVG_CODE_OPENING_2	"\"\n xmlns=\"http://www.w3.org/2000/svg\">\n"

#define SVG_CODE_CLOSING	"</svg>"


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
	m_sSVGCode.Append("\" height=\"");
	m_sSVGCode.Append(SG_Get_String(iHeight,0));
	m_sSVGCode.Append(SVG_CODE_OPENING_2);

	m_iWidth = iWidth;
	m_iHeight = iHeight;

}

bool CSG_Doc_SVG::Save(const char *Filename)
{

	m_sSVGCode.Append(SVG_CODE_CLOSING);

	FILE	*Stream;

	Stream	= fopen(Filename, "w");

	if( Stream )
	{
		fprintf(Stream, "%s", m_sSVGCode.c_str());
		fclose(Stream);
		return true;
	}
	
	return false;

}

void CSG_Doc_SVG::_AddAttribute(const char *Attribute, 
							   const char *Value)
{

		m_sSVGCode.Append(Attribute);
		m_sSVGCode.Append("=\"");
		m_sSVGCode.Append(Value);
		m_sSVGCode.Append("\" ");		

}

void CSG_Doc_SVG::_AddAttribute(const char *Attribute, 
							   int iValue)
{

	_AddAttribute(Attribute, SG_Get_String(iValue, 0));
}

void CSG_Doc_SVG::_AddAttribute(const char *Attribute, 
							   double dValue)
{

	_AddAttribute(Attribute, SG_Get_String(dValue, 2));

}

void CSG_Doc_SVG::Draw_Circle(double x, 
							 double y, 
							 double Radius, 
							 int Fill_Color, 
							 int Line_Color, 
							 double Line_Width, 
							 const char *Unit)
{
	
	CSG_String sWidth;

	m_sSVGCode.Append("<circle ");
	_AddAttribute("cx", x);
	_AddAttribute("cy", y);
	_AddAttribute("r", Radius);
	sWidth.Append(SG_Get_String(Line_Width,2));
	sWidth.Append(Unit);
	_AddAttribute("stroke-width", sWidth);
	_AddAttribute("stroke", _Get_SVGColor(Line_Color));
	_AddAttribute("fill", _Get_SVGColor(Fill_Color));
	m_sSVGCode.Append("/>\n");

}

void CSG_Doc_SVG::Draw_LinkedCircle(double x, 
								 double y, 
								 double Radius, 
								 const char *Link,
								 int Fill_Color, 
								 int Line_Color, 
								 double Line_Width, 
								 const char *Unit)
{
	
	CSG_String sWidth;
	CSG_String sLink;

	m_sSVGCode.Append("<circle ");
	_AddAttribute("cx", x);
	_AddAttribute("cy", y);
	_AddAttribute("r", Radius);
	sWidth.Append(SG_Get_String(Line_Width,2));
	sWidth.Append(Unit);
	_AddAttribute("stroke-width", sWidth);
	_AddAttribute("stroke", _Get_SVGColor(Line_Color));
	_AddAttribute("fill", _Get_SVGColor(Fill_Color));
	sLink = "window.open('";
	sLink.Append(Link);
	sLink.Append("')");
	_AddAttribute("onclick", sLink);
	m_sSVGCode.Append("/>\n");

}

void CSG_Doc_SVG::Draw_Line(double xa, 
						   double ya, 
						   double xb, 
						   double yb, 
						   double Width, 
						   const char *Unit, 
						   int Color)
{

	CSG_String sWidth;

	m_sSVGCode.Append("<line ");
	_AddAttribute("x1", xa);
	_AddAttribute("x2", xb);
	_AddAttribute("y1", ya);
	_AddAttribute("y2", yb);
	sWidth.Append(SG_Get_String(Width,2));
	sWidth.Append(Unit);
	_AddAttribute("stroke-width", sWidth);	_AddAttribute("stroke", _Get_SVGColor(Color));
	m_sSVGCode.Append("/>\n");

}

void CSG_Doc_SVG::Draw_Line(CSG_Points &Points, 
						   double Width, 
						   const char *Unit, 
						   int Color)
{
	int i;
	CSG_String sPoints;
	CSG_String sWidth;
	
	for (i = 0; i < Points.Get_Count(); i++)
	{
		sPoints.Append(SG_Get_String(Points.Get_X(i),2));
		sPoints.Append(",");
		sPoints.Append(SG_Get_String(Points.Get_Y(i),2));
		sPoints.Append(" ");
	}
	m_sSVGCode.Append("<polyline ");
	_AddAttribute("points", sPoints);	
	sWidth.Append(SG_Get_String(Width,2));
	sWidth.Append(Unit);
	_AddAttribute("stroke-width", sWidth);
	_AddAttribute("stroke", _Get_SVGColor(Color));
	_AddAttribute("fill", "none");
	m_sSVGCode.Append("/>\n");

}

void CSG_Doc_SVG::Draw_Rectangle(double xa, 
								double ya, 
								double xb, 
								double yb, 
								int Fill_Color, 
								int Line_Color, 
								double Line_Width, 
								const char *Unit)
{
	CSG_Points	Points;

	Points.Add(xa, ya);
	Points.Add(xb, ya);
	Points.Add(xb, yb);
	Points.Add(xa, yb);

	Draw_Polygon(Points, Fill_Color, Line_Color, Line_Width, Unit) ;
}

void CSG_Doc_SVG::Draw_Rectangle(const CSG_Rect &r, 
								int Fill_Color, 
								int Line_Color, 
								double Line_Width, 
								const char *Unit)
{
	Draw_Rectangle(r.Get_XMin(), r.Get_YMin(), r.Get_XMax(), r.Get_YMax(), Fill_Color, Line_Color, Line_Width) ;
}

void CSG_Doc_SVG::Draw_Polygon(CSG_Points &Points, 
							  int Fill_Color, 
							  int Line_Color, 
							  double Line_Width, 
							  const char *Unit)
{
	if( Points.Get_Count() > 2 )
	{
		int i;
		CSG_String sPoints;
		CSG_String sWidth;
		
		for (i = 0; i < Points.Get_Count(); i++)
		{
			sPoints.Append(SG_Get_String(Points.Get_X(i),2));
			sPoints.Append(",");
			sPoints.Append(SG_Get_String(Points.Get_Y(i),2));
			sPoints.Append(" ");
		}
		m_sSVGCode.Append("<polygon ");
		_AddAttribute("points", sPoints);
		sWidth.Append(SG_Get_String(Line_Width,2));
		sWidth.Append(Unit);
		_AddAttribute("stroke-width", sWidth);				
		_AddAttribute("stroke", _Get_SVGColor(Line_Color));
		_AddAttribute("fill", _Get_SVGColor(Fill_Color));
		m_sSVGCode.Append("/>\n");
	}
}

void CSG_Doc_SVG::Draw_LinkedPolygon(CSG_Points &Points, 
							  const char* Link,
							  int Fill_Color, 
							  int Line_Color, 
							  double Line_Width, 
							  const char *Unit)
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
			sPoints.Append(",");
			sPoints.Append(SG_Get_String(Points.Get_Y(i),2));
			sPoints.Append(" ");
		}
		m_sSVGCode.Append("<polygon ");
		_AddAttribute("points", sPoints);
		sWidth.Append(SG_Get_String(Line_Width,2));
		sWidth.Append(Unit);
		_AddAttribute("stroke-width", sWidth);				
		_AddAttribute("stroke", _Get_SVGColor(Line_Color));
		_AddAttribute("fill", _Get_SVGColor(Fill_Color));
		sLink = "window.open('";
		sLink.Append(Link);
		sLink.Append("')");
		_AddAttribute("onclick", sLink);
		m_sSVGCode.Append("/>\n");
	}
}

void CSG_Doc_SVG::Draw_Text(double x, 
						 double y, 
						 const char *Text, 
						 int Color, 
						 const char* Font, 
						 double dSize,
						 const char* Unit,
						 TSG_SVG_Alignment iAlignment)
{

	CSG_String sAlignments[] = {"", "middle", "end"};
	CSG_String sSize;
	
	sSize.Append(SG_Get_String(dSize,2));
	sSize.Append(Unit);
	
	m_sSVGCode.Append("<text ");
	_AddAttribute("x", x);	
	_AddAttribute("y", y);
	_AddAttribute("font-family", Font);
	_AddAttribute("fill", _Get_SVGColor(Color));
	_AddAttribute("font-size", sSize);
	_AddAttribute("text-anchor", sAlignments[iAlignment]);
	m_sSVGCode.Append(">\n");
	m_sSVGCode.Append(Text);
	m_sSVGCode.Append("</text>");

}

CSG_String CSG_Doc_SVG::_Get_SVGColor(int iColor)
{

	CSG_String s;

	if (iColor == SG_COLOR_NONE)
	{
		return ("none");
	}
	else if (iColor == SG_COLOR_RANDOM)
	{
		s.Append("rgb(");
		s.Append(SG_Get_String((int)(255.0 * (double)rand() / (double)RAND_MAX),0));
		s.Append(",");
		s.Append(SG_Get_String((int)(255.0 * (double)rand() / (double)RAND_MAX),0));
		s.Append(",");
		s.Append(SG_Get_String((int)(255.0 * (double)rand() / (double)RAND_MAX),0));
		s.Append(")");

		return s;
	}
	else
	{
		s.Append("rgb(");
		s.Append(SG_Get_String(SG_GET_R(iColor),0));
		s.Append(",");
		s.Append(SG_Get_String(SG_GET_G(iColor),0));
		s.Append(",");
		s.Append(SG_Get_String(SG_GET_B(iColor),0));
		s.Append(")");

		return s;
	}

}
