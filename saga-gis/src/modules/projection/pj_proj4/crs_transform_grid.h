/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                   Projection_Proj4                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  crs_transform_grid.h                 //
//                                                       //
//                 Copyright (C) 2010 by                 //
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
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__crs_transform_grid_H
#define HEADER_INCLUDED__crs_transform_grid_H

//---------------------------------------------------------
#include "crs_base.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class pj_proj4_EXPORT CCRS_Transform_Grid : public CCRS_Transform
{
public:
	CCRS_Transform_Grid(bool bList);


protected:

	virtual int					On_Parameter_Changed		(CSG_Parameters *pParameters, CSG_Parameter *pParameter);
	virtual int					On_Parameters_Enable		(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	virtual bool				On_Execute_Transformation	(void);


private:

	bool						m_bList;

	int							m_Interpolation;

	CSG_Parameters_Grid_Target	m_Grid_Target;

	CSG_Grid					m_Target_Area;


	bool						Transform					(CSG_Grid                *pGrid );
	bool						Transform					(CSG_Parameter_Grid_List *pGrids);

	bool						Transform					(CSG_Grid                *pGrid , CSG_Grid                *pTarget );
	bool						Transform					(CSG_Parameter_Grid_List *pGrids, CSG_Parameter_Grid_List *pTargets, const CSG_Grid_System &Target_System);

	bool						Transform					(CSG_Grid                *pGrid , CSG_Shapes *pPoints);
	bool						Transform					(CSG_Parameter_Grid_List *pGrids, CSG_Shapes *pPoints);

	void						Get_MinMax					(TSG_Rect &r, double x, double y);
	bool						Get_Target_System			(const CSG_Grid_System &System, bool bEdge);

	bool						Set_Target_Area				(const CSG_Grid_System &Source, const CSG_Grid_System &Target);
	bool						is_In_Target_Area			(int x, int y);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__crs_transform_grid_H
