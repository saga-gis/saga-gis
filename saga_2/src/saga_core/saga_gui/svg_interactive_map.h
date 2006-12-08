
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    User Interface                     //
//                                                       //
//                    Program: SAGA                      //
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
#include <saga_api/doc_svg.h>

#include "wksp_map.h"
#include "wksp_shapes.h"
#include "wksp_grid.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CSVG_Interactive_Map : public CSG_Doc_SVG
{
public:
	CSVG_Interactive_Map(void);
	virtual ~CSVG_Interactive_Map(void);

	void				Create_From_Map				(CWKSP_Map *pMap, CSG_Shapes *pIndexLayer, const wxChar *Filename);


private:

	CSG_String			m_Directory;
	double				m_dWidth;


	void				_Write_Code					(const wxChar *FileName, const wxChar *Code);

	void				_Add_Opening				(CSG_Rect r);
	const wxChar *		_Get_Opening_Code_1			(void);
	const wxChar *		_Get_Opening_Code_2			(void);

	void				_Add_CheckBoxes				(CWKSP_Map *pMap);
	void				_Add_Shapes					(CWKSP_Shapes *pLayer);
	bool				_Add_Shape					(CSG_Shape *pShape, int Fill_Color, int Line_Color, double Line_Width, double Point_Width);
	void				_Add_Grid					(CWKSP_Grid *pLayer);
	void				_Add_ReferenceMap			(CSG_Shapes *pIndexLayer, CSG_Rect r);
	void				_Add_Label					(const wxChar* Label, CSG_Shape *pShape, double dSize, const wxChar* Unit);

	const wxChar *		_Get_Code_1					(void);
	const wxChar *		_Get_Code_2					(void);
	const wxChar *		_Get_Code_3					(void);
	const wxChar *		_Get_Code_4					(void);
	const wxChar *		_Get_Code_5					(void);
	const wxChar *		_Get_Code_Closing_1			(void);

	const wxChar *		_Get_Code_CheckBox			(void);
	const wxChar *		_Get_Code_MapApp			(void);
	const wxChar *		_Get_Code_Timer				(void);
	const wxChar *		_Get_Code_Slider			(void);
	const wxChar *		_Get_Code_Helper			(void);
	const wxChar *		_Get_Code_Buttons			(void);
	const wxChar *		_Get_Code_Navigation_1		(void);
	const wxChar *		_Get_Code_Navigation_2		(void);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__svg_interactive_map_H
