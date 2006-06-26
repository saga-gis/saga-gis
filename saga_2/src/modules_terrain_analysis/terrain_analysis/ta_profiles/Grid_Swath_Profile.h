
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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
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
class CGrid_Swath_Profile : public CModule_Grid_Interactive
{
public:
	CGrid_Swath_Profile(void);
	virtual ~CGrid_Swath_Profile(void);


protected:

	virtual bool				On_Execute			(void);
	virtual bool				On_Execute_Position	(CGEO_Point ptWorld, TModule_Interactive_Mode Mode);


private:

	bool						m_bAdd;

	double						m_Width;

	CShapes						*m_pPoints, *m_pLine;

	CGrid						*m_pDEM;

	CParameter_Grid_List		*m_pValues;


	bool						Set_Profile	(void);
	bool						Set_Profile	(CGEO_Point A, CGEO_Point B, CGEO_Point Left, CGEO_Point Right);

	bool						Add_Point	(CGEO_Point Point, CGEO_Point Left, CGEO_Point Right, CGEO_Point Step);
	bool						Add_Swath	(CTable_Record *pRecord, int iEntry, CGrid *pGrid, CGEO_Point Left, CGEO_Point Right, CGEO_Point Step);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__Grid_Swath_Profile_H
