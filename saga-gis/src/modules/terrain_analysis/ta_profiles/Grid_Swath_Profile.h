/**********************************************************
 * Version $Id: Grid_Swath_Profile.h 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                     ta_profiles                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  Grid_Swath_Profile.h                 //
//                                                       //
//                 Copyright (C) 2005 by                 //
//                      Olaf Conrad                      //
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
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__Grid_Swath_Profile_H
#define HEADER_INCLUDED__Grid_Swath_Profile_H

//---------------------------------------------------------
#include "MLB_Interface.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CGrid_Swath_Profile : public CSG_Module_Grid_Interactive
{
public:
	CGrid_Swath_Profile(void);
	virtual ~CGrid_Swath_Profile(void);


protected:

	virtual bool				On_Execute			(void);
	virtual bool				On_Execute_Finish	(void);
	virtual bool				On_Execute_Position	(CSG_Point ptWorld, TSG_Module_Interactive_Mode Mode);


private:

	bool						m_bAdd;

	double						m_Width;

	CSG_Shapes					*m_pPoints, *m_pLine;

	CSG_Grid					*m_pDEM;

	CSG_Parameter_Grid_List		*m_pValues;


	bool						Set_Profile	(void);
	bool						Set_Profile	(CSG_Point A, CSG_Point B, CSG_Point Left, CSG_Point Right);

	bool						Add_Point	(CSG_Point Point, CSG_Point Left, CSG_Point Right, CSG_Point Step);
	bool						Add_Swath	(CSG_Table_Record *pRecord, int iEntry, CSG_Grid *pGrid, CSG_Point Left, CSG_Point Right, CSG_Point Step);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__Grid_Swath_Profile_H
