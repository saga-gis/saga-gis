
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
//             temperature_lapse_downscaling.cpp         //
//                                                       //
//                 Copyright (C) 2021 by                 //
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
#include "temperature_lapse_downscaling.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTemperature_Lapse_Downscaling::CTemperature_Lapse_Downscaling(void)
{
	Set_Name		(_TL("Lapse Rate Based Temperature Downscaling"));

	Set_Author		("O.Conrad (c) 2021");

	Set_Description	(_TW(
		"The Lapse Rate Based Temperature Downscaling is quite simple, "
		"but might perform well for mountainous regions, where the "
		"altitudinal gradient is the main driver for local temperature "
		"variation. First, a given lapse rate is used to estimate sea "
		"level temperatures from elevation and temperature data at a "
		"coarse resolution. Second, the same lapse rate is used to "
		"estimate the terrain surface temperature using higher resoluted "
		"elevation data and the spline interpolated sea level temperatures "
		"from the previous step. "
		"The lapse rates can be defined as one constant value valid for the "
		"whole area of interest, or as varying value as defined by an "
		"additional input grid. Alternatively a constant lapse rate can be "
		"estimated from the coarse resolution input with a regression analysis. "
	));

	//-----------------------------------------------------
	Parameters.Add_Grid_System("", "LORES_GRID_SYSTEM"    , _TL("Coarse Resolution"    ), _TL(""));
	Parameters.Add_Grid("LORES_GRID_SYSTEM", "LORES_DEM"  , _TL("Elevation"            ), _TL(""), PARAMETER_INPUT);
	Parameters.Add_Grid("LORES_GRID_SYSTEM", "LORES_T"    , _TL("Temperature"          ), _TL(""), PARAMETER_INPUT);
	Parameters.Add_Grid("LORES_GRID_SYSTEM", "LORES_LAPSE", _TL("Lapse Rate"           ), _TL(""), PARAMETER_INPUT);
	Parameters.Add_Grid("LORES_GRID_SYSTEM", "LORES_SLT"  , _TL("Sea Level Temperature"), _TL(""), PARAMETER_OUTPUT_OPTIONAL);

	Parameters.Add_Grid_System("", "HIRES_GRID_SYSTEM"    , _TL("High Resolution"      ), _TL(""));
	Parameters.Add_Grid("HIRES_GRID_SYSTEM", "HIRES_DEM"  , _TL("Elevation"            ), _TL(""), PARAMETER_INPUT);
	Parameters.Add_Grid("HIRES_GRID_SYSTEM", "HIRES_T"    , _TL("Temperature"          ), _TL(""), PARAMETER_OUTPUT);

	Parameters.Add_Choice("",
		"LAPSE_METHOD"	, _TL("Lapse Rate"),
		_TL(""),
		CSG_String::Format("%s|%s|%s",
			_TL("constant lapse rate"),
			_TL("constant lapse rate from regression"),
			_TL("varying lapse rate from grid")
		), 1
	);

	Parameters.Add_Double("LAPSE_METHOD",
		"CONST_LAPSE"	, _TL("Lapse Rate"),
		_TL("Constant lapse rate in degree of temperature per 100 meter."),
		0.6
	);

	Parameters.Add_Choice("LAPSE_METHOD",
		"REGRS_LAPSE"	, _TL("Regression"),
		_TL(""),
		CSG_String::Format("%s|%s|%s",
			_TL("elevation"),
			_TL("elevation and position"),
			_TL("elevation and position (2nd order polynom)")
		), 2
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CTemperature_Lapse_Downscaling::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("LAPSE_METHOD") )
	{
		pParameters->Set_Enabled("CONST_LAPSE", pParameter->asInt() == 0);
		pParameters->Set_Enabled("REGRS_LAPSE", pParameter->asInt() == 1);
		pParameters->Set_Enabled("LORES_LAPSE", pParameter->asInt() == 2);
	}

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTemperature_Lapse_Downscaling::On_Execute(void)
{
	CSG_Grid_System	LoRes(*Parameters("LORES_GRID_SYSTEM")->asGrid_System());

	CSG_Grid *pLoRes_Z   = Parameters("LORES_DEM")->asGrid();
	CSG_Grid *pLoRes_T   = Parameters("LORES_T"  )->asGrid();
	CSG_Grid *pLoRes_SLT = Parameters("LORES_SLT")->asGrid(), LoRes_SLT;

	if( !pLoRes_SLT )
	{
		pLoRes_SLT = &LoRes_SLT; pLoRes_SLT->Create(LoRes);
	}

	//-----------------------------------------------------
	double dT_const = 0.006; CSG_Grid *pLoRes_dT = NULL;

	switch( Parameters("LAPSE_METHOD")->asInt() )
	{
	default: // constant lapse rate
		dT_const	= Parameters("CONST_LAPSE")->asDouble() / 100.;
		break;

	case  1: // constant lapse rate from regression
	//	Process_Set_Text("%s...", _TL("Regression"));

		if( !Get_Regression(pLoRes_T, pLoRes_Z, dT_const) )
		{
			return( false );
		}
		break;

	case  2: // varying lapse rate from grid
		pLoRes_dT	= Parameters("LORES_LAPSE")->asGrid();
		break;
	}

	//-----------------------------------------------------
	for(int y=0; y<LoRes.Get_NY() && Set_Progress(y, LoRes.Get_NY()); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<LoRes.Get_NX(); x++)
		{
			if( !pLoRes_Z->is_NoData(x, y) && !pLoRes_T->is_NoData(x, y) && !(pLoRes_dT && pLoRes_dT->is_NoData(x, y)) )
			{
				double	dT	= pLoRes_dT ? pLoRes_dT->asDouble(x, y) : dT_const;

				pLoRes_SLT->Set_Value(x, y, pLoRes_T->asDouble(x, y) + dT * pLoRes_Z->asDouble(x, y));
			}
			else
			{
				pLoRes_SLT->Set_NoData(x, y);
			}
		}
	}

	//-----------------------------------------------------
	CSG_Grid_System	HiRes(*Parameters("HIRES_GRID_SYSTEM")->asGrid_System());

	CSG_Grid *pHiRes_DEM = Parameters("HIRES_DEM")->asGrid();
	CSG_Grid *pHiRes_T   = Parameters("HIRES_T"  )->asGrid();

	pHiRes_T->Fmt_Name("%s [%s]", pLoRes_T->Get_Name(), _TL("downscaled"));

	for(int y=0; y<HiRes.Get_NY() && Set_Progress(y, HiRes.Get_NY()); y++)
	{
		double	py	= HiRes.Get_YMin() + y * HiRes.Get_Cellsize();

		#pragma omp parallel for
		for(int x=0; x<HiRes.Get_NX(); x++)
		{
			double	px	= HiRes.Get_XMin() + x * HiRes.Get_Cellsize(), T, dT;

			if( !pHiRes_DEM->is_NoData(x, y) && pLoRes_SLT->Get_Value(px, py, T) && (!pLoRes_dT || pLoRes_dT->Get_Value(px, py, dT)) )
			{
				if( pLoRes_dT )
				{
					dT	/= 100.; // degree per 100m -> degree per meter
				}
				else
				{
					dT	= dT_const;
				}

				pHiRes_T->Set_Value(x, y, T - dT * pHiRes_DEM->asDouble(x, y));
			}
			else
			{
				pHiRes_T->Set_NoData(x, y);
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTemperature_Lapse_Downscaling::Get_Regression(CSG_Grid *pT, CSG_Grid *pZ, double &dT)
{
	int	Method	= Parameters("REGRS_LAPSE")->asInt();

	CSG_Vector	Sample;

	switch( Method )
	{
	default: Sample.Create(2); break; // T = a + b*Z
	case  1: Sample.Create(4); break; // T = a + b*Z + c*X + d*Y
	case  2: Sample.Create(6); break; // T = a + b*Z + c*X + d*Y + e*X*X + f*Y*Y
	}

	const SG_Char *Vars[] = { SG_T("T"), SG_T("Z"), SG_T("X"), SG_T("Y"), SG_T("X^2"), SG_T("Y^2") };

	CSG_Strings	Names(Sample.Get_N(), Vars);

	//-----------------------------------------------------
	CSG_Matrix	Samples;

	CSG_Grid_System	System(pT->Get_System());

	for(int y=0; y<System.Get_NY(); y++)
	{
		for(int x=0; x<System.Get_NX(); x++)
		{
			if( !pT->is_NoData(x, y) && !pZ->is_NoData(x, y) )
			{
				switch( Method )
				{
				case  2: // T = a + b*Z + c*X + d*Y + e*X*X + f*Y*Y
					Sample[5]	= y*y;
					Sample[4]	= x*x;

				case  1: // T = a + b*Z + c*X + d*Y
					Sample[3]	= y;
					Sample[2]	= x;

				default: // T = a + b*Z
					Sample[1]	= pZ->asDouble(x, y);
					Sample[0]	= pT->asDouble(x, y);
				}

				Samples.Add_Row(Sample);
			}
		}
	}

	//-----------------------------------------------------
	CSG_Regression_Multiple	Regression;

	if( !Regression.Get_Model(Samples, &Names) )
	{
		Error_Set(_TL("Regression failed"));

		return( false );
	}

	Message_Add(Regression.Get_Info(), false);

	dT	= -Regression.Get_RCoeff(0);

	Message_Fmt("\n\n%s: %g", _TL("Constant lapse rate from regression"), dT * 100.);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
