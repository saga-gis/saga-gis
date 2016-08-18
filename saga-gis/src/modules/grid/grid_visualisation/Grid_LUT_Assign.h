/**********************************************************
 * Version $Id: grid_lut_assign.h 2155 2014-06-11 14:00:23Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                  Grid_Visualisation                   //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   grid_lut_assign.h                   //
//                                                       //
//                 Copyright (C) 2015 by                 //
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
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__grid_lut_assign_H
#define HEADER_INCLUDED__grid_lut_assign_H


///////////////////////////////////////////////////////////
//														 //
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "MLB_Interface.h"


///////////////////////////////////////////////////////////
//														 //
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CGrid_LUT_Assign : public CSG_Tool_Grid  
{
public:
	CGrid_LUT_Assign(void);


protected:

	virtual bool		On_Execute				(void);

};


///////////////////////////////////////////////////////////
//														 //
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__grid_lut_assign_H
