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
	Set_Name	(_TL("Land Surface Temperature"));

	Set_Author	("O.Conrad (c) 2008");

	Set_Description(_TW(
		""
	));

	Add_Reference("Boehner, J., Antonic, O.", "2009", "Land-surface parameters specific to topo-climatology",
		"In: Hengl, T., Reuter, H. (Eds.): Geomorphometry - Concepts, Software, Applications. Developments in soil science, 33, 195-226."
	);

	//-----------------------------------------------------
	Parameters.Add_Grid("",
		"DEM"			, _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid("",
		"SWR"			, _TL("Short Wave Radiation"),
		_TL("[kW / m2]"),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid("",
		"LAI"			, _TL("Leaf Area Index"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid("",
		"LST"			, _TL("Land Surface Temperature"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Double("",
		"Z_REFERENCE"	, _TL("Elevation at Reference Station"),
		_TL(""),
		0.0
	);

	Parameters.Add_Double("",
		"T_REFERENCE"	, _TL("Temperature at Reference Station"),
		_TL("Temperature at reference station in degree Celsius."),
		0.0
	);

	Parameters.Add_Double("",
		"T_GRADIENT"	, _TL("Lapse Rate"),
		_TL("Vertical temperature gradient in degree Celsius per 100 meter."),
		0.6
	);

	Parameters.Add_Double("",
		"C_FACTOR"		, _TL("C Factor"),
		_TL(""),
		1.0
	);

	Parameters.Add_Double("",
		"LAI_MAX"		, _TL("C Factor"),
		_TL(""),
		8.0, 0.01, true
	);
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
	double	T_gradient	= Parameters("T_GRADIENT" )->asDouble() / 100.0;
	double	C_Factor	= Parameters("C_FACTOR"   )->asDouble();
	double	LAI_max		= Parameters("LAI_MAX"    )->asDouble();

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			if( pDEM->is_NoData(x, y) || pSWR->is_NoData(x, y) || pLAI->is_NoData(x, y) || pSWR->asDouble(x, y) <= 0.0 )
			{
				pLST->Set_NoData(x, y);
			}
			else
			{
				double	T	= T_reference - T_gradient * (pDEM->asDouble(x, y) - Z_reference);

				double	SWR	= pSWR->asDouble(x, y);
				double	LAI	= pLAI->asDouble(x, y);

				pLST->Set_Value(x, y, T + C_Factor * (SWR - 1.0 / SWR) * (1.0 - LAI / LAI_max));
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
