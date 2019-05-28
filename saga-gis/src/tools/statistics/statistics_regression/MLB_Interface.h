
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
//                    TLB_Interface.h                    //
//                                                       //
//                 Copyright (C) 2003 by                 //
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
#ifndef HEADER_INCLUDED__statistics_regression_H
#define HEADER_INCLUDED__statistics_regression_H


///////////////////////////////////////////////////////////
//														 //
//				Include the SAGA-API here				 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <saga_api/saga_api.h>


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define GWR_Add_References(bLogistic)	\
	Add_Reference("Fotheringham, S.A., Brunsdon, C., Charlton, M.", "2002",\
		"Geographically Weighted Regression: the analysis of spatially varying relationships",\
		"John Wiley & Sons.",\
		SG_T("http://onlinelibrary.wiley.com/doi/10.1111/j.1538-4632.2003.tb01114.x/abstract"), SG_T("online"));\
	Add_Reference("Fotheringham, S.A., Charlton, M., Brunsdon, C.", "1998",\
		"Geographically weighted regression: a natural evolution of the expansion method for spatial data analysis",\
		"Environment and Planning A 30(11), 1905–1927.",\
		SG_T("http://www.envplan.com/abstract.cgi?id=a301905"), SG_T("online"));\
	Add_Reference("Lloyd, C.", "2010",\
		"Spatial Data Analysis - An Introduction for GIS Users",\
		"Oxford, 206p.");\
	if( bLogistic ) {\
	Add_Reference("Zhang, D., Ren, N., and Hou, X.", "2018",\
		"An improved logistic regression model based on a spatially weighted technique (ILRBSWT v1.0) and its application to mineral prospectivity mapping",\
		"Geosci. Model Dev., 11, 2525-2539.",\
		SG_T("https://doi.org/10.5194/gmd-11-2525-2018"), SG_T("doi:10.5194/gmd-11-2525-2018"));\
	}

//---------------------------------------------------------
double	GWR_Fit_To_Density(CSG_Shapes *pPoints, double Bandwidth, int Rounding = 1);


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__statistics_regression_H
