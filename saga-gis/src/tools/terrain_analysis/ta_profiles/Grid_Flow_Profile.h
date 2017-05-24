/**********************************************************
 * Version $Id: Grid_Flow_Profile.h 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                     ta_profiles                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  Grid_Flow_Profile.h                  //
//                                                       //
//                 Copyright (C) 2004 by                 //
//                      Olaf Conrad                      //
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
#ifndef HEADER_INCLUDED__Grid_Flow_Profile_H
#define HEADER_INCLUDED__Grid_Flow_Profile_H

//---------------------------------------------------------
#include "MLB_Interface.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CGrid_Flow_Profile : public CSG_Tool_Grid_Interactive
{
public:
	CGrid_Flow_Profile(void);


protected:

	virtual bool				On_Execute			(void);

	virtual bool				On_Execute_Position	(CSG_Point ptWorld, TSG_Tool_Interactive_Mode Mode);


private:

	CSG_Shapes					*m_pPoints, *m_pLines;
	
	CSG_Shape					*m_pLine;

	CSG_Grid					*m_pDEM;

	CSG_Parameter_Grid_List		*m_pValues;


	bool						Set_Profile			(TSG_Point ptWorld);

	bool						Add_Point			(int x, int y);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__Grid_Flow_Profile_H
