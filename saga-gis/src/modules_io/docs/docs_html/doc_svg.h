/**********************************************************
 * Version $Id$
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
//-------------------------------------------------------//
//                                                       //
//                      doc_svg.h                        //
//                                                       //
//                 Copyright (C) 2005 by                 //
//                     Victor Olaya                      //
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
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__docs_html__doc_svg_H
#define HEADER_INCLUDED__docs_html__doc_svg_H

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "MLB_Interface.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
typedef enum ESG_SVG_Alignment
{
	SVG_ALIGNMENT_Left	= 0,
	SVG_ALIGNMENT_Center,
	SVG_ALIGNMENT_Right
}
TSG_SVG_Alignment;


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class docs_html_EXPORT CSG_Doc_SVG
{
public:
	CSG_Doc_SVG(void);
	virtual ~CSG_Doc_SVG(void);

	void						Open						(int iWidth, int iHeight);
	bool						Save						(const SG_Char *FileName);

	void						Draw_Line					(double xa, double ya, double xb, double yb	, double Width = 1., int Color = SG_COLOR_BLACK);
	void						Draw_Line					(CSG_Points &Points							, double Width = 1., int Color = SG_COLOR_BLACK);
	void						Draw_Rectangle				(double xa, double ya, double xb, double yb	, int Fill_Color = SG_COLOR_WHITE, int Line_Color = SG_COLOR_BLACK, double Line_Width = 1.);
	void						Draw_Rectangle				(const CSG_Rect &r							, int Fill_Color = SG_COLOR_WHITE, int Line_Color = SG_COLOR_BLACK, double Line_Width = 1.);
	void						Draw_Polygon				(CSG_Points &Points							, int Fill_Color = SG_COLOR_GREEN, int Line_Color = SG_COLOR_BLACK, double Line_Width = 1.);
	void						Draw_LinkedPolygon			(CSG_Points &Points, const SG_Char *Link	, int Fill_Color = SG_COLOR_GREEN, int Line_Color = SG_COLOR_BLACK, double Line_Width = 1.);
	void						Draw_Circle					(double x, double y, double Radius			, int Fill_Color = SG_COLOR_RED  , int Line_Color = SG_COLOR_BLACK, double Line_Width = 1.);
	void						Draw_LinkedCircle			(double x, double y, double Radius, const SG_Char *Link, int Fill_Color = SG_COLOR_RED  , int Line_Color = SG_COLOR_BLACK, double Line_Width = 1.);

	void						Draw_Text					(double x, double y,  const SG_Char *Text, int iColor, const SG_Char* Font, double dSize, TSG_SVG_Alignment Alignment = SVG_ALIGNMENT_Center);

protected:

	CSG_String					m_sSVGCode;
	void						_AddAttribute				(const SG_Char *Attribute, const SG_Char *Value);
	void						_AddAttribute				(const SG_Char *Attribute, int Value);
	void						_AddAttribute				(const SG_Char *Attribute, double Value);
	CSG_String					_Get_SVGColor				(int iColor);

private:
	
	int							m_iWidth, m_iHeight;
};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif //#ifndef HEADER_INCLUDED__docs_html__doc_svg_H
