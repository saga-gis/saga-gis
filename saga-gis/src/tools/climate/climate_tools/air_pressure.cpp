
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                     climate_tools                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   air_pressure.cpp                    //
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
#include "air_pressure.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CAirPressure_Scaling::CAirPressure_Scaling(void)
{
	Set_Name		(_TL("Air Pressure Adjustment"));

	Set_Author		("O.Conrad (c) 2020");

	Set_Description	(_TW(
		"This tool adjusts air pressure values to the elevation "
		"using the barometric formula. Default values refer to "
		"the international standard atmosphere. "
	));

	//-----------------------------------------------------
	Parameters.Add_Grid_or_Const("", "P", _TL("Air Pressure"          ), _TL("[hPa]"    ), 1013.25  ,    0.  , false, 0., false, false);
	Parameters.Add_Grid_or_Const("", "Z", _TL("Air Pressure Elevation"), _TL("[m]"      ),    0.    ,    0.  , false, 0., false, false);
	Parameters.Add_Grid_or_Const("", "T", _TL("Temperature"           ), _TL("[Celsius]"),    0.    , -273.15, false, 0., false, false);
	Parameters.Add_Grid_or_Const("", "L", _TL("Temperature Lapse Rate"), _TL("[K/m]"    ),    0.0065,    0.  , false, 0., false, false);

	Parameters.Add_Grid("",
		"DEM"		, _TL("Elevation"),
		_TL("[m]"),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid("",
		"P_ADJ"		, _TL("Adjusted Air Pressure"),
		_TL(""),
		PARAMETER_OUTPUT
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CAirPressure_Scaling::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CAirPressure_Scaling::On_Execute(void)
{
	CSG_Grid *pP = Parameters("P")->asGrid(); double P = Parameters("P")->asDouble();
	CSG_Grid *pZ = Parameters("Z")->asGrid(); double Z = Parameters("Z")->asDouble();
	CSG_Grid *pT = Parameters("T")->asGrid(); double T = Parameters("T")->asDouble();
	CSG_Grid *pL = Parameters("L")->asGrid(); double L = Parameters("L")->asDouble();

	CSG_Grid *pDEM  = Parameters("DEM"  )->asGrid();
	CSG_Grid *pPadj	= Parameters("P_ADJ")->asGrid();

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		double	py	= Get_YMin() + y * Get_Cellsize();

		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			double	Padj = -1.;

			if( !pDEM->is_NoData(x, y) )
			{
				double	px	= Get_XMin() + x * Get_Cellsize(),
					_P = P, _Z = Z, _T = T, _L = L;

				if( (!pP || pP->Get_Value(px, py, _P))
				&&  (!pZ || pZ->Get_Value(px, py, _Z))
				&&  (!pT || pT->Get_Value(px, py, _T))
				&&  (!pL || pL->Get_Value(px, py, _L)) )
				{
					Padj	= _P * pow(1. - (_L * (pDEM->asDouble(x, y) - _Z) / (273.15 + _T)), 5.255);
				}
			}

			if( Padj < 0. )
			{
				pPadj->Set_NoData(x, y);
			}
			else
			{
				pPadj->Set_Value(x, y, Padj);
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
