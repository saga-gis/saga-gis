/**********************************************************
 * Version $Id: gw_multi_regression_grid.h 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                 statistics_regression                 //
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

	virtual CSG_String			Get_MenuPath			(void)	{	return( _TL("R:GWR") );	}


protected:

	virtual int					On_Parameter_Changed	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);
	virtual int					On_Parameters_Enable	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	virtual bool				On_Execute				(void);


private:

	int							m_nPredictors, m_nPoints_Min, m_nPoints_Max, m_Direction;

	CSG_Grid					**m_pPredictors, **m_pModel, *m_pQuality;

	CSG_Grid_System				m_dimModel;

	double						m_Radius;

	CSG_Distance_Weighting		m_Weighting;

	CSG_PRQuadTree				m_Search;

	CSG_Shapes					m_Points;


	bool						Initialize				(CSG_Shapes *pPoints, int iDependent, CSG_Parameter_Grid_List *pPredictors);
	void						Finalize				(void);

	bool						Get_Model				(void);
	bool						Get_Model				(int x, int y, CSG_Regression_Weighted &Model);

	bool						Set_Model				(void);
	bool						Set_Model				(double x, double y, double &Value);

	bool						Set_Residuals			(void);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__gw_multi_regression_grid_H
