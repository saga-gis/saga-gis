
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library:                     //
//                     CliffMetrics                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   profile_crossings.h                 //
//                                                       //
//                 Copyright (C) 2020 by                 //
//               Andres Payo & Olaf Conrad               //
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
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__profile_crossings_H
#define HEADER_INCLUDED__profile_crossings_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <saga_api/saga_api.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CProfile_Crossings : public CSG_Tool  
{
public:
	CProfile_Crossings(void);


protected:

	virtual bool			On_Execute			(void);


private:

	bool					Get_Crossing		(CSG_Shapes *pLines_A, CSG_Shapes *pLines_B, CSG_Shapes *pCrossings_AB, int Attributes, CSG_Shapes *pDistances_AB, double disSeaward );

	bool					Add_Attributes		(CSG_Shapes *pCrossings, CSG_Shapes *pLines);
	bool					Set_Attributes		(CSG_Shape  *pCrossing , CSG_Shape  *pLine, int &Offset);

	bool					Set_Crossing		(const TSG_Point &Crossing, CSG_Shape *pA, CSG_Shape *pB, CSG_Shape *pCrossing, int Attributes);

	bool					Set_Distance		(const TSG_Point StartPoint, const TSG_Point Crossing, CSG_Shape *pA, CSG_Shape *pB, CSG_Shape *pDistances, double disSeaward);
	bool					Set_Attributes      (CSG_Shape  *pDistances, double dDistance, int &Offset);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__profile_crossings_H
