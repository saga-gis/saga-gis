
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
//              land_surface_temperature.cpp              //
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
#include "climate_tools.h"

#include "land_surface_temperature.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CLand_Surface_Temperature::CLand_Surface_Temperature(void)
{
	Set_Name		(_TL("Land Surface Temperature"));

	Set_Author		("O.Conrad (c) 2020");

	Set_Description	(_TW(
		"This tool estimates the land surface temperature by combining global solar radiation, "
		"albedo, and the Stefan-Boltzmann Law. "
		"This is an implementation of the approach proposed by Hofierka et al. (2020). "
	));

	Add_Reference("Hofierka, J., Gallaya, M., Onacillovaa, K., Hofierka, J.jr.", "2020",
		"Physically-based land surface temperature modeling in urban areas using a 3-D city model and multispectral satellite data",
		"Urban Climate, 31, 100566.",
		SG_T("https://doi.org/10.1016/j.uclim.2019.100566"), SG_T("doi:10.1016/j.uclim.2019.100566")
	);

	//-----------------------------------------------------
	Parameters.Add_Grid_or_Const("", "IRRADIANCE", _TL("Global Irradiance"),
		_TL("[W/m2]"),
		5. , 0., true
	);

	Parameters.Add_Grid_or_Const("", "ALBEDO"    , _TL("Albedo"),
		_TL("Surface reflectance [0 <= albedo <= 1]"),
		0.5, 0., true, 1., true
	);

	Parameters.Add_Grid_or_Const("", "EMISSIVITY", _TL("Emissivity"),
		_TL("Thermal emissivity [0 <= emissivity <= 1]"),
		0.5, 0., true, 1., true
	);

	Parameters.Add_Grid_or_Const("", "CONVECTION", _TL("Convection Coefficient"),
		_TL("Convection heat transfer coefficient [W/m2/K]."),
		10., 0., true
	);

	Parameters.Add_Grid_or_Const("", "T_AIR"     , _TL("Ambient Air Temperature"),
		_TL("[Kelvin]"),
		300., 0., true
	);

	Parameters.Add_Grid_or_Const("", "T_SKY"     , _TL("Radiant Sky Temperature"),
		_TL("[Kelvin]"),
		280., 0., true
	);

	Parameters.Add_Grid_or_Const("", "T_INITIAL" , _TL("Initial Temperature Estimation"),
		_TL("Initial estimation of land surface temperature [Kelvin] (e.g., 300)."),
		300., 0., true
	);

	Parameters.Add_Grid("",
		"LST"		, _TL("Land Surface Temperature"),
		_TL("[Kelvin]"),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice("LST",
		"UNIT"		, _TL("Unit"),
		_TL(""),
		CSG_String::Format("%s|%s",
			_TL("Kelvin"),
			_TL("Celsius")
		), 0
	);

	Parameters.Add_Int("",
		"ITERATIONS", _TL("Iterations"),
		_TL(""),
		10, 1, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CLand_Surface_Temperature::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CLand_Surface_Temperature::On_Execute(void)
{
	const double	sigma	= 5.670374419e-8;	// Stefan-Boltzmann constant [W/m2/K4] (0.000000056685 in Hofierka et al. 2020)

	//-----------------------------------------------------
	CSG_Grid *pIrradiance = Parameters("IRRADIANCE")->asGrid(); double cIrradiance = Parameters("IRRADIANCE")->asDouble();
	CSG_Grid *pAlbedo     = Parameters("ALBEDO"    )->asGrid(); double cAlbedo     = Parameters("ALBEDO"    )->asDouble();
	CSG_Grid *pEmissivity = Parameters("EMISSIVITY")->asGrid(); double cEmissivity = Parameters("EMISSIVITY")->asDouble();
	CSG_Grid *pConvection = Parameters("CONVECTION")->asGrid(); double cConvection = Parameters("CONVECTION")->asDouble();
	CSG_Grid *pT_air      = Parameters("T_AIR"     )->asGrid(); double cT_air      = Parameters("T_AIR"     )->asDouble();
	CSG_Grid *pT_sky      = Parameters("T_SKY"     )->asGrid(); double cT_sky      = Parameters("T_SKY"     )->asDouble();
	CSG_Grid *pT_initial  = Parameters("T_INITIAL" )->asGrid(); double cT_initial  = Parameters("T_INITIAL" )->asDouble();

	CSG_Grid	*pLST	= Parameters("LST")->asGrid();

	pLST->Set_Unit("Kelvin");
	pLST->Set_Scaling(1., 0.);

	int	Iterations	= Parameters("ITERATIONS")->asInt();

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#ifndef _DEBUG
		#pragma omp parallel for
		#endif
		for(int x=0; x<Get_NX(); x++)
		{
			if( (pIrradiance && pIrradiance->is_NoData(x, y))
			||  (pAlbedo     && pAlbedo    ->is_NoData(x, y))
			||  (pEmissivity && pEmissivity->is_NoData(x, y))
			||  (pConvection && pConvection->is_NoData(x, y)) )
			{
				pLST->Set_NoData(x, y);

				continue;
			}

			double	Irradiance = pIrradiance ? pIrradiance->asDouble(x, y) : cIrradiance;
			double	Albedo     = pAlbedo     ? pAlbedo    ->asDouble(x, y) : cAlbedo    ;
			double	Emissivity = pEmissivity ? pEmissivity->asDouble(x, y) : cEmissivity;
			double	Convection = pConvection ? pConvection->asDouble(x, y) : cConvection;
			double	T_air      = pT_air      ? pT_air     ->asDouble(x, y) : cT_air     ;
			double	T_sky      = pT_sky      ? pT_sky     ->asDouble(x, y) : cT_sky     ;
			double	T_initial  = pT_initial  ? pT_initial ->asDouble(x, y) : cT_initial ;

			//-----------------------------------------------------
			Emissivity	= sigma * (Emissivity < 0. ? 0. : Emissivity > 1. ? 1. : Emissivity);
			Albedo		= 1. -    (Albedo     < 0. ? 0. : Albedo     > 1. ? 1. : Albedo    );

			double	c	= -Emissivity * T_sky*T_sky*T_sky*T_sky - Convection * T_air - Albedo * Irradiance;

			double	LST	= T_initial;

			LST	= (3. * Emissivity * LST*LST*LST*LST - c)
				/ (4. * Emissivity * LST*LST*LST + Convection);	// 1st iteration

			for(int i=1; i<Iterations; i++)
			{
				LST	= (3. * Emissivity * LST*LST*LST*LST - c)
					/ (4. * Emissivity * LST*LST*LST + Convection);
			}

			pLST->Set_Value(x, y, LST);
		}
	}

	//-----------------------------------------------------
	if( Parameters("UNIT")->asInt() == 1 )
	{
		pLST->Set_Unit("Celsius");
		pLST->Set_Scaling(1., -273.15);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
