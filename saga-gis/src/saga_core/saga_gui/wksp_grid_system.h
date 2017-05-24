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
//                  WKSP_Grid_System.h                   //
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
#ifndef _HEADER_INCLUDED__SAGA_GUI__WKSP_Grid_System_H
#define _HEADER_INCLUDED__SAGA_GUI__WKSP_Grid_System_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <saga_api/saga_api.h>

#include "wksp_base_manager.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CWKSP_Grid_System : public CWKSP_Base_Manager
{
public:
	CWKSP_Grid_System(const CSG_Grid_System &System);

	virtual TWKSP_Item				Get_Type			(void)		{	return( WKSP_ITEM_Grid_System );	}

	virtual wxString				Get_Name			(void);
	virtual wxString				Get_Description		(void);

	virtual wxMenu *				Get_Menu			(void);

	class CWKSP_Data_Item *			Get_Data			(int iItem)	{	return( (CWKSP_Data_Item *)Get_Item(iItem) );	}

	class CWKSP_Grid *				Get_Grid			(CSG_Grid *pGrid);
	class CWKSP_Grid *				Add_Grid			(CSG_Grid *pGrid);

	class CWKSP_Grids *				Get_Grids			(CSG_Grids *pGrids);
	class CWKSP_Grids *				Add_Grids			(CSG_Grids *pGrids);

	const CSG_Grid_System &			Get_System			(void)		{	return( m_System );	}


private:

	CSG_Grid_System					m_System;

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__WKSP_Grid_System_H
