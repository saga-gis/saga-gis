/**********************************************************
 * Version $Id: svg_interactive_map.h 1922 2014-01-09 10:28:46Z oconrad $
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
//              SVG_Interactive_map.cpp                  //
//                                                       //
//          Copyright (C) 2005 by Victor Olaya           //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'. SAGA is free software; you   //
// can redistribute it and/or modify it under the terms  //
// of the GNU General Public License as published by the //
// Free Software Foundation; version 2 of the License.   //
//                                                       //
// SAGA is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU General Public        //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU General    //
// Public License along with this program; if not,       //
// write to the Free Software Foundation, Inc.,          //
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

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef _HEADER_INCLUDED__SAGA_GUI__svg_interactive_map_H
#define _HEADER_INCLUDED__SAGA_GUI__svg_interactive_map_H


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
class CSVG_Interactive_Map : public CSG_Doc_SVG, public CSG_Tool
{
public:
	CSVG_Interactive_Map(void);
	virtual ~CSVG_Interactive_Map(void);

protected:

	virtual bool		On_Execute(void);


private:

	CSG_String			m_Directory;
	double				m_dWidth;


	bool				Create_From_Map				(CSG_Parameter_Shapes_List *pList, CSG_Shapes *pIndexLayer, const SG_Char *Filename);

	void				_Write_Code					(const CSG_String &FileName, const CSG_String &Code);

	void				_Add_Opening				(CSG_Rect r);

	void				_Add_CheckBoxes				(CSG_Parameter_Shapes_List *pList);
	void				_Add_Shapes					(CSG_Shapes *pLayer);
	bool				_Add_Shape					(CSG_Shape *pShape, int Fill_Color, int Line_Color, double Line_Width, double Point_Width);
	void				_Add_Grid					(CSG_Grid *pLayer);
	void				_Add_ReferenceMap			(CSG_Shapes *pIndexLayer, CSG_Rect r);
	void				_Add_Label					(const SG_Char* Label, CSG_Shape *pShape, double dSize, const SG_Char* Unit);

	const char *		_Get_Opening_Code_1			(void);
	const char *		_Get_Opening_Code_2			(void);

	const char *		_Get_Code_1					(void);
	const char *		_Get_Code_2					(void);
	const char *		_Get_Code_3					(void);
	const char *		_Get_Code_4					(void);
	const char *		_Get_Code_5					(void);
	const char *		_Get_Code_Closing_1			(void);

	const char *		_Get_Code_CheckBox			(void);
	const char *		_Get_Code_MapApp			(void);
	const char *		_Get_Code_Timer				(void);
	const char *		_Get_Code_Slider			(void);
	const char *		_Get_Code_Helper			(void);
	const char *		_Get_Code_Buttons			(void);
	const char *		_Get_Code_Navigation_1		(void);
	const char *		_Get_Code_Navigation_2		(void);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__svg_interactive_map_H
