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
//               Grid_ProfileFromPoints.h                //
//                                                       //
//              Copyright (C) 2004 - 2022 by             //
//            Victor Olaya and Volker Wichmann           //
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
//    e-mail:     wichmann@laserdata                     //
//                                                       //
//    contact:    Volker Wichmann                        //
//                LASERDATA GmbH                         //
//                Management and analysis of             //
//                laserscanning data                     //
//                Innsbruck, Austria                     //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__Grid_Profile_From_Points_H
#define HEADER_INCLUDED__Grid_Profile_From_Points_H

//---------------------------------------------------------
#include "MLB_Interface.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CProfileFromPoints : public CSG_Tool_Grid
{
public:
	CProfileFromPoints(void);
	virtual ~CProfileFromPoints(void);

protected:
	virtual bool    On_Execute(void);

private:

    enum
    {
        F_ID	= 0,
        F_DIST,
        F_DIST_SURF,
        F_X,
        F_Y,
        F_Z,
        F_VALUES
    };

    CSG_Grid                *m_pGrid;
    CSG_Parameter_Grid_List *m_pValues;
    CSG_Table               *m_pProfile;

    bool    Set_Profile(const TSG_Point &A, const TSG_Point &B, bool bLastPoint);
    bool    Add_Point(const TSG_Point &Point);
};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__Grid_Profile_From_Points_H
