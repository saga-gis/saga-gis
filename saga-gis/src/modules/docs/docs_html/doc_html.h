/**********************************************************
 * Version $Id: doc_html.h 1921 2014-01-09 10:24:11Z oconrad $
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
//-------------------------------------------------------//
//                                                       //
//                      doc_html.h                       //
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
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__docs_html__doc_html_H
#define HEADER_INCLUDED__docs_html__doc_html_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "doc_svg.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define HTML_GRAPH_TYPE_BARS					1
#define HTML_GRAPH_TYPE_LINES					2

#define HTML_COLOR_RANDOM						-2


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class docs_html_EXPORT CSG_Doc_HTML
{
public:
	CSG_Doc_HTML(void);
	virtual ~CSG_Doc_HTML(void);

	void						Open						(const SG_Char *Title);
	bool						Save						(const SG_Char *FileName);

	void						AddCurve					(const SG_Char *Filename,
															CSG_Points &Points,
															const SG_Char *Description,
															int iGraphType,
															bool bIncludeTableData);
	void						AddTable					(const SG_Char ***Table,
															int iRows,
															int iCols,
															const SG_Char *Description);
	void						AddTable					(CSG_Table *pTable);
	void						AddParagraph				(const SG_Char *Text);
	void						AddLineBreak				();
	void						AddHeader					(const SG_Char *Text, int iOrder);
	void						AddHyperlink				(const SG_Char *Text, const SG_Char *URL);
	CSG_String					GetHyperlinkCode			(const SG_Char *Text, const SG_Char *URL);

	void						AddImage					(const SG_Char *Filename);
	void						AddThumbnail				(const SG_Char *Filename,
															int iWidth,
															bool bIsPercent);
	void						AddThumbnails				(const SG_Char **Text,
															int iImages,
															int iThumbnailsPerRow);
	void						StartUnorderedList			(void);
	void						StartOrderedList			(void);
	void						CloseUnorderedList			(void);
	void						CloseOrderedList			(void);
	void						AddListElement				(const SG_Char *Text);
	void						AddOrderedList				(const SG_Char **Text,
															int iElement);
	void						AddUnorderedList			(const SG_Char **Text,
															int iElement);

	bool						Draw_Shapes					(CSG_Shapes *pShapes,
															const SG_Char *Filename,
															int Fill_Color, 
															int Line_Color, 
															int Line_Width);

private:

	CSG_String					m_sHTMLCode;

	bool						_Draw_Shape					(CSG_Doc_SVG &SVG,
															CSG_Shape *pShape,
															CSG_Rect GlobalRect,
															int Fill_Color, 
															int Line_Color, 
															int Line_Width, 
															int Point_Width);

	void						_AddBicolumTable			(CSG_Points *pData);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__docs_html__doc_html_H
