
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
//                   air_humidity.cpp                    //
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
#include "air_humidity.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
/**
* Saturation pressure is calculated using the formulas according
* to Lowe & Ficke (Method == 1) or Magnus after Sonntag (1990)
* (default). Temperature has to be [°C] and should not be less
* than -50°C.
*/
double	CT_Get_Vapor_Pressure_at_Saturation	(double T, int Method = 0)
{
	double	Ew, Ei;

	switch( Method )
	{
	default: // Magnus after Sonntag (1990)
		Ew = 6.112 * exp(17.62 * T / (243.12 + T));
		Ei = 6.112 * exp(22.46 * T / (272.62 + T));
		break;

	case  1: // Lowe & Ficke (1974)
		Ew = 6.107799961 + T * (4.436518521e-1 + T * (1.428945805e-2 + T * (2.650648471e-4 + T * (3.031240396e-6 + T * (2.034080948e-8 + T * 6.136820929e-11)))));
		Ei = 6.109177956 + T * (5.034698970e-1 + T * (1.886013408e-2 + T * (4.176223716e-4 + T * (5.824720280e-6 + T * (4.838803174e-8 + T * 1.838826904e-10)))));
		break;
	}

	return( Ew < Ei ? Ew : Ei );
}

//---------------------------------------------------------
/**
* Estimates dew point temperature from actual temperature [°C]
* and water vapor pressure [hPa]. Method for saturation pressure
* calculation is either Lowe & Ficke (Method == 1) or Magnus
* (default).
*/
double	CT_Get_Dew_Point_Temperature(double VP, int Method = 0, double Epsilon = 0.01)
{
	if( VP <= 0. || Epsilon <= 0. )
	{
		return( -999. );
	}

	double T = 0., dT = 10.; int Direction = 0;

	while( dT > Epsilon && fabs(T) < 100. )
	{
		double VPsat = CT_Get_Vapor_Pressure_at_Saturation(T, Method);
			
		if( VPsat > VP )
		{
			if( Direction < 0 ) { dT /= 2.; } Direction =  1; T -= dT;
		}
		else if( VPsat < VP )
		{
			if( Direction > 0 ) { dT /= 2.; } Direction = -1; T += dT;
		}
		else
		{
			return( T );
		}
	}

	return( T );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CAirHumidity_Conversion::CAirHumidity_Conversion(void)
{
	Set_Name		(_TL("Air Humidity Conversions"));

	Set_Author		("O.Conrad (c) 2021");

	Set_Description	(_TW(
		"Conversions of air moisture content between various units. "
	));

	//-----------------------------------------------------
	Parameters.Add_Grid_or_Const("", "T"        , _TL("Temperature"           ), _TL("[Celsius]"),   25.  , -273.15, true,   0., false);
	Parameters.Add_Grid_or_Const("", "P"        , _TL("Air Pressure"          ), _TL("[hPa]"    ), 1013.25,    0.  , true,   0., false);

	Parameters.Add_Grid_or_Const("",  "IN_VP"   , _TL("Vapor Pressure"        ), _TL("[hPa]"    ),   15.  ,    0.  , true,   0., false);
	Parameters.Add_Grid_or_Const("",  "IN_SH"   , _TL("Specific Humidity"     ), _TL("[g/kg]"   ),   10.  ,    0.  , true,   0., false);
	Parameters.Add_Grid_or_Const("",  "IN_RH"   , _TL("Relative Humidity"     ), _TL("[%]"      ),   50.  ,    0.  , true, 100.,  true);
	Parameters.Add_Grid_or_Const("",  "IN_DP"   , _TL("Dew Point"             ), _TL("[Celsius]"),   14.  , -273.15, true,   0., false);

	Parameters.Add_Grid         ("", "OUT_VPSAT", _TL("Saturation Pressure"   ), _TL("[hPa]"    ), PARAMETER_OUTPUT_OPTIONAL);
	Parameters.Add_Grid         ("", "OUT_VP"   , _TL("Vapor Pressure"        ), _TL("[hPa]"    ), PARAMETER_OUTPUT_OPTIONAL);
	Parameters.Add_Grid         ("", "OUT_VPDIF", _TL("Vapor Pressure Deficit"), _TL("[hPa]"    ), PARAMETER_OUTPUT_OPTIONAL);
	Parameters.Add_Grid         ("", "OUT_RH"   , _TL("Relative Humidity"     ), _TL("[%]"      ), PARAMETER_OUTPUT_OPTIONAL);
	Parameters.Add_Grid         ("", "OUT_SH"   , _TL("Specific Humidity"     ), _TL("[g/kg]"   ), PARAMETER_OUTPUT_OPTIONAL);
	Parameters.Add_Grid         ("", "OUT_DP"   , _TL("Dew Point"             ), _TL("[Celsius]"), PARAMETER_OUTPUT_OPTIONAL);
	Parameters.Add_Grid         ("", "OUT_DPDIF", _TL("Dew Point Difference"  ), _TL("[Celsius]"), PARAMETER_OUTPUT_OPTIONAL);

	Parameters.Add_Choice("",
		"CONVERSION"	, _TL("Conversion from..."),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s",
			_TL("Vapor Pressure"),
			_TL("Specific Humidity"),
			_TL("Relative Humidity"),
			_TL("Dew Point")
		), 0
	);

	Parameters.Add_Choice("",
		"VPSAT_METHOD"	, _TL("Saturation Pressure"),
		_TL("Formula used to estimate vapor pressure at saturation."),
		CSG_String::Format("%s|%s",
			_TL("Magnus"),
			_TL("Lowe & Ficke")
		), 0
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CAirHumidity_Conversion::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("CONVERSION") )
	{
		pParameters->Set_Enabled( "IN_VP", pParameter->asInt() == 0);
		pParameters->Set_Enabled( "IN_SH", pParameter->asInt() == 1);
		pParameters->Set_Enabled( "IN_RH", pParameter->asInt() == 2);
		pParameters->Set_Enabled( "IN_DP", pParameter->asInt() == 3);

		pParameters->Set_Enabled("OUT_VP", pParameter->asInt() != 0);
		pParameters->Set_Enabled("OUT_SH", pParameter->asInt() != 1);
		pParameters->Set_Enabled("OUT_RH", pParameter->asInt() != 2);
		pParameters->Set_Enabled("OUT_DP", pParameter->asInt() != 3);
	}

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CAirHumidity_Conversion::On_Execute(void)
{
	const double Mass_H2O = 18.01534; // [g/mol] molar mass of water
	const double Mass_Air = 28.9644 ; // [g/mol] molar mass of dry air

	//-----------------------------------------------------
	int	VPsat_Method = Parameters("VPSAT_METHOD")->asInt();
	int	Conversion   = Parameters("CONVERSION"  )->asInt();

	CSG_Grid *pT = Parameters("T")->asGrid(); double Tconst = Parameters("T")->asDouble();
	CSG_Grid *pP = Parameters("P")->asGrid(); double Pconst = Parameters("P")->asDouble();

	CSG_Grid *pValue; double Value;

	switch( Conversion )
	{
	default: pValue = Parameters("IN_VP")->asGrid(); Value = Parameters("IN_VP")->asDouble(); break;
	case  1: pValue = Parameters("IN_SH")->asGrid(); Value = Parameters("IN_SH")->asDouble(); break;
	case  2: pValue = Parameters("IN_RH")->asGrid(); Value = Parameters("IN_RH")->asDouble(); break;
	case  3: pValue = Parameters("IN_DP")->asGrid(); Value = Parameters("IN_DP")->asDouble(); break;
	}

	CSG_Grid *pVP    = Conversion == 0 ? NULL : Parameters("OUT_VP")->asGrid();
	CSG_Grid *pSH    = Conversion == 1 ? NULL : Parameters("OUT_SH")->asGrid();
	CSG_Grid *pRH    = Conversion == 2 ? NULL : Parameters("OUT_RH")->asGrid();
	CSG_Grid *pDP    = Conversion == 3 ? NULL : Parameters("OUT_DP")->asGrid();

	CSG_Grid *pVPdif = Parameters("OUT_VPDIF")->asGrid();
	CSG_Grid *pDPdif = Parameters("OUT_DPDIF")->asGrid();
	CSG_Grid *pVPsat = Parameters("OUT_VPSAT")->asGrid();

	if( pVPsat ) { pVPsat->Set_Unit(SG_T("hPa"    )); }
	if( pVP    ) { pVP   ->Set_Unit(SG_T("hPa"    )); }
	if( pVPdif ) { pVPdif->Set_Unit(SG_T("hPa"    )); }
	if( pSH    ) { pSH   ->Set_Unit(SG_T("g/kg"   )); }
	if( pRH    ) { pRH   ->Set_Unit(SG_T("%"      )); }
	if( pDP    ) { pDP   ->Set_Unit(SG_T("Celsius")); }
	if( pDPdif ) { pDPdif->Set_Unit(SG_T("Celsius")); }

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			if( (pT     && pT    ->is_NoData(x, y))
			||  (pP     && pP    ->is_NoData(x, y))
			||  (pValue && pValue->is_NoData(x, y)) )
			{
				if( pVPsat ) { pVPsat->Set_NoData(x, y); }
				if( pVP    ) { pVP   ->Set_NoData(x, y); }
				if( pVPdif ) { pVPdif->Set_NoData(x, y); }
				if( pSH    ) { pSH   ->Set_NoData(x, y); }
				if( pRH    ) { pRH   ->Set_NoData(x, y); }
				if( pDP    ) { pDP   ->Set_NoData(x, y); }
				if( pDPdif ) { pDPdif->Set_NoData(x, y); }
			}
			else
			{
				double	T     = pT     ? pT    ->asDouble(x, y) : Tconst;
				double	P     = pP     ? pP    ->asDouble(x, y) : Pconst;
				double	Value = pValue ? pValue->asDouble(x, y) : Value;

				double	VP, VMR, SH, VPsat = CT_Get_Vapor_Pressure_at_Saturation(T, VPsat_Method);

				if( VPsat > P ) // should never happen!
				{
					VPsat = P;
				}

				switch( Conversion )
				{
				default: // Vapor Pressure
					VP  = Value;
					VMR = VP / P; // volume mixing ratio
					SH  = VMR * Mass_H2O / (VMR * Mass_H2O + (1. - VMR) * Mass_Air);
					break;

				case  1: // Specific Humidity
					SH  = Value / 1000.; // [g/kg] -> [kg/kg]
					VMR = Mass_Air / (Mass_Air - Mass_H2O * (1. - 1. / SH)); // volume mixing ratio
					VP  = VMR * P; if( VP > VPsat ) { VP = VPsat; }
					break;

				case  2: // Relative Humidity
					VP  = VPsat * Value / 100.; if( VP > VPsat ) { VP = VPsat; }
					VMR = VP / P; // volume mixing ratio
					SH  = VMR * Mass_H2O / (VMR * Mass_H2O + (1. - VMR) * Mass_Air);
					break;

				case  3: // Dew Point Temperature
					VP  = CT_Get_Vapor_Pressure_at_Saturation(Value); if( VP > VPsat ) { VP = VPsat; }
					VMR = VP / P; // volume mixing ratio
					SH  = VMR * Mass_H2O / (VMR * Mass_H2O + (1. - VMR) * Mass_Air);
					break;
				}

				double DP = pDP || pDPdif ? CT_Get_Dew_Point_Temperature(VP, VPsat_Method, 0.001) : 0.; // time consuming, only estimate if necessary

				if( pVPsat ) { pVPsat->Set_Value(x, y, VPsat            ); }
				if( pVP    ) { pVP   ->Set_Value(x, y, VP               ); }
				if( pVPdif ) { pVPdif->Set_Value(x, y, VPsat - VP       ); }
				if( pSH    ) { pSH   ->Set_Value(x, y, 1000. * SH       ); } // [kg/kg] -> [g/kg]
				if( pRH    ) { pRH   ->Set_Value(x, y, 100. * VP / VPsat); }
				if( pDP    ) { pDP   ->Set_Value(x, y, DP               ); }
				if( pDPdif ) { pDPdif->Set_Value(x, y, T - DP           ); }
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
