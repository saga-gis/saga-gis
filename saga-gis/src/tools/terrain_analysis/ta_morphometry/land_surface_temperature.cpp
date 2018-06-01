/**********************************************************
 * Version $Id: land_surface_temperature.cpp 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                    ta_morphometry                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//              Land_Surface_Temperature.cpp             //
//                                                       //
//                 Copyright (C) 2008 by                 //
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
//    e-mail:     conrad@geowiss.uni-hamburg.de          //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Hamburg                  //
//                Bundesstr. 55                          //
//                20146 Hamburg                          //
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
#include "land_surface_temperature.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CLand_Surface_Temperature::CLand_Surface_Temperature(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Land Surface Temperature"));

	Set_Author		("O.Conrad (c) 2008");

	Set_Description(_TW(
		"Temperature estimation at each grid point as a function of temperature, "
		"temperature lapse rate and elevation for a reference station. "
		"Further optional input is the Leaf Area Index (LAI) and the "
		"short-wave radiation ratio, which relates the irradiance including "
		"terrain effects to that calculated for a flat, horizontal plane. "
		"See Wilson & Gallant (2000) for more details. "
	));

	Add_Reference(
		"Boehner, J., Antonic, O.", "2009",
		"Land-surface parameters specific to topo-climatology",
		"In: Hengl, T., Reuter, H. (Eds.): Geomorphometry - Concepts, Software, Applications. Developments in soil science, 33, 195-226."
	);

	Add_Reference(
		"Wilson, J. P., Gallant, J. C.", "2000",
		"Secondary Topographic Attributes",
		"In: Wilson, J. P., Gallant, J. C. (Eds.): Terrain analysis - Principles and Applications, 87-131."
	);

	//-----------------------------------------------------
	Parameters.Add_Grid("",
		"DEM"			, _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid("",
		"SWR"			, _TL("Short Wave Radiation Ratio"),
		_TL("The ratio of daily total short-wave irradiance on sloping sites compared to horizontal sites."),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Grid("",
		"LAI"			, _TL("Leaf Area Index"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Double("LAI",
		"LAI_MAX"		, _TL("Maximum LAI"),
		_TL(""),
		8.0, 0.01, true
	);

	Parameters.Add_Grid("",
		"LST"			, _TL("Land Surface Temperature"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Node("",
		"REFERENCE"		, _TL("Reference Station"),
		_TL("")
	);

	Parameters.Add_Double("REFERENCE",
		"Z_REFERENCE"	, _TL("Elevation"),
		_TL(""),
		0.0
	);

	Parameters.Add_Double("REFERENCE",
		"T_REFERENCE"	, _TL("Temperature"),
		_TL("Temperature at reference station in degree Celsius."),
		0.0
	);

	Parameters.Add_Double("",
		"T_GRADIENT"	, _TL("Temperature Lapse Rate"),
		_TL("Vertical temperature gradient in degree Celsius per 100 meter."),
		0.65
	);

	Parameters.Add_Double("",
		"C_FACTOR"		, _TL("C Factor"),
		_TL(""),
		1.0
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CLand_Surface_Temperature::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), "LAI") )
	{
		pParameters->Set_Enabled("LAI_MAX", pParameter->asDataObject() != NULL);
	}

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CLand_Surface_Temperature::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Grid	*pDEM	= Parameters("DEM")->asGrid();
	CSG_Grid	*pSWR	= Parameters("SWR")->asGrid();
	CSG_Grid	*pLAI	= Parameters("LAI")->asGrid();
	CSG_Grid	*pLST	= Parameters("LST")->asGrid();

	double	Z_reference	= Parameters("Z_REFERENCE")->asDouble();
	double	T_reference	= Parameters("T_REFERENCE")->asDouble();
	double	T_gradient	= Parameters("T_GRADIENT" )->asDouble() / 100.0;	// lapse rate per meter
	double	C_Factor	= Parameters("C_FACTOR"   )->asDouble();
	double	LAI_max		= Parameters("LAI_MAX"    )->asDouble();

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			if( pDEM->is_NoData(x, y) || (pLAI && pLAI->is_NoData(x, y)) || (pSWR && (pSWR->is_NoData(x, y) || pSWR->asDouble(x, y) <= 0.0)) )
			{
				pLST->Set_NoData(x, y);
			}
			else
			{
				double	C	= C_Factor;

				if( pSWR )
				{
					C	*= pSWR->asDouble(x, y) - 1.0 / pSWR->asDouble(x, y);
				}

				if( pLAI )
				{
					C	*= 1.0 - pLAI->asDouble(x, y) / LAI_max;
				}

				double	T	= T_reference - T_gradient * (pDEM->asDouble(x, y) - Z_reference);

				pLST->Set_Value(x, y, T + C);
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
