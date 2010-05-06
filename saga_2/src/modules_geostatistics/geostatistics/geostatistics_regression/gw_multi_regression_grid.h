
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//               geostatistics_regression                //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//              gw_multi_regression_grid.h               //
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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
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
#ifndef HEADER_INCLUDED__gw_multi_regression_grid_H
#define HEADER_INCLUDED__gw_multi_regression_grid_H

//---------------------------------------------------------
#include "MLB_Interface.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CGW_Multi_Regression_Grid : public CSG_Module_Grid
{
public:
	CGW_Multi_Regression_Grid(void);


protected:

	virtual bool				On_Execute				(void);


private:

	int							m_nPoints_Min, m_nPoints_Max, m_Mode;

	double						m_Radius;

	CSG_Distance_Weighting		m_Weighting;

	CSG_PRQuadTree				m_Search;

	CSG_Vector					m_z, m_w;

	CSG_Matrix					m_y;

	CSG_Shapes					m_Points;

	CSG_Grid					*m_pRegression, *m_pQuality;
	
	CSG_Parameter_Grid_List		*m_pPredictors, *m_pSlopes;


	bool						Initialize				(void);
	void						Finalize				(void);

	int							Set_Variables			(int x, int y);

	bool						Get_Regression			(int x, int y);

	bool						Set_Residuals			(void);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__gw_multi_regression_grid_H
