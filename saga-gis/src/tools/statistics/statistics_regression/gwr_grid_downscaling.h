/**********************************************************
 * Version $Id: gwr_grid_downscaling.h 1549 2012-11-29 16:38:50Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                 statistics_regression                 //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 gwr_grid_downscaling.h                //
//                                                       //
//                 Copyright (C) 2013 by                 //
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
#ifndef HEADER_INCLUDED__gwr_grid_downscaling_H
#define HEADER_INCLUDED__gwr_grid_downscaling_H

//---------------------------------------------------------
#include "MLB_Interface.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CGWR_Grid_Downscaling : public CSG_Tool_Grid
{
public:
	CGWR_Grid_Downscaling(void);

	virtual CSG_String				Get_MenuPath			(void)	{ return(_TL("A:Spatial and Geostatistics|Geographically Weighted Regression")); }


protected:

	virtual int						On_Parameters_Enable	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	virtual bool					On_Execute				(void);


private:

	int								m_nPredictors;

	CSG_Grid_Cell_Addressor			m_Search;

	CSG_Grid						*m_pDependent, **m_pPredictors, **m_pModel, *m_pQuality, *m_pResiduals;


	bool							Set_Model				(double x, double y, double &Value, double &Residual);
	bool							Set_Model				(void);

	bool							Get_Model				(void);
	bool							Get_Model				(int x, int y, CSG_Regression_Weighted &Model);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__gwr_grid_downscaling_H
