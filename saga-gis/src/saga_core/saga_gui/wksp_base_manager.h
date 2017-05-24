/**********************************************************
 * Version $Id$
 *********************************************************/

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
//                 WKSP_Base_Manager.h                   //
//                                                       //
//          Copyright (C) 2005 by Olaf Conrad            //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'. SAGA is free software; you   //
// can redistribute it and/or modify it under the terms  //
// of the GNU General Public License as published by the //
// Free Software Foundation, either version 2 of the     //
// License, or (at your option) any later version.       //
//                                                       //
// SAGA is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU General Public        //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU General    //
// Public License along with this program; if not, see   //
// <http://www.gnu.org/licenses/>.                       //
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
#ifndef _HEADER_INCLUDED__SAGA_GUI__WKSP_Base_Manager_H
#define _HEADER_INCLUDED__SAGA_GUI__WKSP_Base_Manager_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "wksp_base_item.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CWKSP_Base_Manager : public CWKSP_Base_Item
{
	friend class CWKSP_Base_Item;

public:
	CWKSP_Base_Manager(void);
	virtual ~CWKSP_Base_Manager(void);

	int							Get_Count		(void)		{	return( m_nItems );	}

	CWKSP_Base_Item *			Get_Item		(int iItem)	{	return( iItem >= 0 && iItem < m_nItems ? m_Items[iItem] : NULL );	}

	int							Get_Items_Count	(void);

	bool						Move_Top		(CWKSP_Base_Item *pItem);
	bool						Move_Bottom		(CWKSP_Base_Item *pItem);
	bool						Move_Up			(CWKSP_Base_Item *pItem);
	bool						Move_Down		(CWKSP_Base_Item *pItem);
	bool						Move_To			(CWKSP_Base_Item *pItem, CWKSP_Base_Item *pItem_Dst);


protected:

	bool						Add_Item		(CWKSP_Base_Item *pItem);
	bool						Del_Item		(CWKSP_Base_Item *pItem);
	bool						Del_Item		(int iItem);


private:

	int							m_nItems, m_Item_ID;

	CWKSP_Base_Item				**m_Items;

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__WKSP_Base_Manager_H
