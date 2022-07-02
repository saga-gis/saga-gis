
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                    statistics_grid                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//              GSGrid_Zonal_Statistics.h                //
//                                                       //
//              Copyright (C) 2005-2022 by               //
//                    Volker Wichmann                    //
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
#ifndef HEADER_INCLUDED__GSGrid_Zonal_Statistics_H
#define HEADER_INCLUDED__GSGrid_Zonal_Statistics_H

//---------------------------------------------------------
#include <saga_api/saga_api.h>

#include <limits>
#include <map>
#include <vector>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CGSGrid_Zonal_Statistics : public CSG_Tool_Grid
{
public:
	CGSGrid_Zonal_Statistics(void);
	virtual ~CGSGrid_Zonal_Statistics(void);


protected:

	virtual bool		On_Execute		(void);


private:

    typedef struct {
        sLong   n       =  0;
        double  min     =  std::numeric_limits<double>::max();
        double  max     = -std::numeric_limits<double>::max();
        double  sum     =  0.0;
        double  sum_2   =  0.0;     // sum squared, with aspect it is used to store the y-component (x-component is stored in sum)
        std::vector<sLong>  cells;  // cells belonging to UCU, only written for first entry
    }STATS;

    void    _Set_Stats(std::map<std::vector<int>, std::vector<STATS> > &mapUCUs, std::vector<int> &vCategories, int i, double val, bool bAspect);
    void    _Create_Field(CSG_Table *pTable, CSG_String sFieldName, CSG_String sSuffix, TSG_Data_Type Type, bool bShortNames);
};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__GSGrid_Zonal_Statistics_H
