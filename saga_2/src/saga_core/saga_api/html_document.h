
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
//                   html_document.h                     //
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

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__SAGA_API__html_document_H
#define HEADER_INCLUDED__SAGA_API__html_document_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "shapes.h"
#include "svg_graph.h"

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
class SAGA_API_DLL_EXPORT CHTML_Document
{
public:
	CHTML_Document(void);
	virtual ~CHTML_Document(void);

	void						Open						(const char *Title);
	bool						Save						(const char *FileName);

	void						AddCurve					(const char *Filename,
															CAPI_dPoints &Points,
															const char *Description,
															int iGraphType,
															bool bIncludeTableData);
	void						AddTable					(const char ***Table,
															int iRows,
															int iCols,
															const char *Description);
	void						AddTable					(CTable *pTable);
	void						AddParagraph				(const char *Text);
	void						AddLineBreak				();
	void						AddHeader					(const char *Text, int iOrder);
	void						AddHyperlink				(const char *Text, const char *URL);
	CAPI_String					GetHyperlinkCode			(const char *Text, const char *URL);

	void						AddImage					(const char *Filename);
	void						AddThumbnail				(const char *Filename,
															int iWidth,
															bool bIsPercent);
	void						AddThumbnails				(const char **Text,
															int iImages,
															int iThumbnailsPerRow);
	void						StartUnorderedList			(void);
	void						StartOrderedList			(void);
	void						CloseUnorderedList			(void);
	void						CloseOrderedList			(void);
	void						AddListElement				(const char *Text);
	void						AddOrderedList				(const char **Text,
															int iElement);
	void						AddUnorderedList			(const char **Text,
															int iElement);

	bool						Draw_Shapes					(CShapes *pShapes,
															const char *Filename,
															int Fill_Color, 
															int Line_Color, 
															int Line_Width);

private:

	CAPI_String					m_sHTMLCode;

	bool						_Draw_Shape					(CSVG_Graph &SVG,
															CShape *pShape,
															CGEO_Rect GlobalRect,
															int Fill_Color, 
															int Line_Color, 
															int Line_Width, 
															int Point_Width);

	void						_AddBicolumTable			(CAPI_dPoints *pData);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__SAGA_API__html_document_H
