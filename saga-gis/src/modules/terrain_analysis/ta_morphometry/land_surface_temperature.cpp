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

	Set_Author	(SG_T("(c) 2008 by O.Conrad"));

	Set_Description(_TW(
		"References:\n"
		"Bohner, J., Antonic, O. (2008): "
		"'Land-suface parameters specific to topo-climatology'. "
		"in: Hengl, T., Reuter, H. (Eds.): 'Geomorphometry - Concepts, Software, Applications', in press\n"
	));


	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL	, "DEM"			, _TL("Elevation [m]"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "SWR"			, _TL("Short Wave Radiation [kW/m2]"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "LAI"			, _TL("Leaf Area Index"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "LST"			, _TL("Land Surface Temperature [Deg.Celsius]"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Value(
		NULL	, "Z_REFERENCE"	, _TL("Elevation at Reference Station [m]"),
		_TL(""),
		PARAMETER_TYPE_Double	, 0.0
	);

	Parameters.Add_Value(
		NULL	, "T_REFERENCE"	, _TL("Temperature at Reference Station [Deg.Celsius]"),
		_TL(""),
		PARAMETER_TYPE_Double	, 0.0
	);

	Parameters.Add_Value(
		NULL	, "T_GRADIENT"	, _TL("Temperature Gradient [Deg.Celsius/km]"),
		_TL(""),
		PARAMETER_TYPE_Double	, 6.5
	);

	Parameters.Add_Value(
		NULL	, "C_FACTOR"	, _TL("C Factor"),
		_TL(""),
		PARAMETER_TYPE_Double	, 1.0
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CLand_Surface_Temperature::On_Execute(void)
{
	double		Z_reference, T_reference, T_gradient, C_Factor, LAI_max, Z, SWR, LAI, LST;
	CSG_Grid	*pDEM, *pSWR, *pLAI, *pLST;

	//-----------------------------------------------------
	pDEM		= Parameters("DEM")			->asGrid();
	pSWR		= Parameters("SWR")			->asGrid();
	pLAI		= Parameters("LAI")			->asGrid();
	pLST		= Parameters("LST")			->asGrid();

	Z_reference	= Parameters("Z_REFERENCE")	->asDouble();
	T_reference	= Parameters("T_REFERENCE")	->asDouble();
	T_gradient	= Parameters("T_GRADIENT")	->asDouble();
	C_Factor	= Parameters("C_FACTOR")	->asDouble();

	LAI_max		= pLAI->Get_ZMax();

	//-----------------------------------------------------
	if( LAI_max > 0.0 )
	{
		for(int y=0; y<Get_NY() && Set_Progress(y); y++)
		{
			for(int x=0; x<Get_NX(); x++)
			{
				if( pDEM->is_NoData(x, y) || pSWR->is_NoData(x, y) || pLAI->is_NoData(x, y) || (SWR = pSWR->asDouble(x, y)) <= 0.0 )
				{
					pLST->Set_NoData(x, y);
				}
				else
				{
					Z	= pDEM->asDouble(x, y);
					SWR	= pSWR->asDouble(x, y);
					LAI	= pLAI->asDouble(x, y);

					LST	= T_reference
						- (T_gradient * (Z - Z_reference)) / 1000.0
						+ C_Factor * (SWR - 1.0 / SWR) * (1.0 - LAI / LAI_max);

					pLST->Set_Value(x, y, LST);
				}
			}
		}

		return( true );
	}

	//-----------------------------------------------------
	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
