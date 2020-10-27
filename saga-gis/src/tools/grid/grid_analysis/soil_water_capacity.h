
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                    grid_analysis                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 soil_water_capacity.h                 //
//                                                       //
//                 Copyright (C) 2020 by                 //
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
//    e-mail:     oconrad@saga-gis.de                    //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__soil_water_capacity_H
#define HEADER_INCLUDED__soil_water_capacity_H


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
class CSoil_Water_Capacity : public CSG_Tool_Grid
{
public:
	CSoil_Water_Capacity(bool bGrids);

	virtual CSG_String		Get_MenuPath			(void)	{	return( _TL("Soil Analysis") );	}


protected:

	virtual int				On_Parameters_Enable	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	virtual bool			On_Execute				(void);


private:

	bool					m_bGrids;

	static double			s_Coefficients[4][12];

	CSG_Matrix				m_Coefficients;


	double					van_Genuchten			(double psi, double alpha, double n, double theta_s, double theta_r);

	bool					Get_HodnettTomasella	(void);
	bool					Get_HodnettTomasella	(double &alpha, double &n, double &theta_s, double &theta_r, double Sand, double Silt, double Clay, double Bulk, double Corg, double CEC, double pH, bool bAdjust);

	bool					Get_Toth				(void);
	bool					Get_Toth				(double &FC, double &PWP, double Silt, double Clay, double Corg);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__soil_water_capacity_H
