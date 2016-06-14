/**********************************************************
 * Version $Id: bioclimatic_vars.cpp 1380 2012-04-26 12:02:19Z reklov_w $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                     climate_tools                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  bioclimatic_vars.cpp                 //
//                                                       //
//                 Copyright (C) 2016 by                 //
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
#include "bioclimatic_vars.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
const CSG_String	Vars[NVARS][2]	=
{
	{	_TL("Annual Mean Temperature"             ), _TL("")	},
	{	_TL("Mean Diurnal Range"                  ), _TL("")	},
	{	_TL("Isothermality"                       ), _TL("")	},
	{	_TL("Temperature Seasonality"             ), _TL("")	},
	{	_TL("Maximum Temperature of Warmest Month"), _TL("")	},
	{	_TL("Minimum Temperature of Coldest Month"), _TL("")	},
	{	_TL("Temperature Annual Range"            ), _TL("")	},
	{	_TL("Mean Temperature of Wettest Quarter" ), _TL("")	},
	{	_TL("Mean Temperature of Driest Quarter"  ), _TL("")	},
	{	_TL("Mean Temperature of Warmest Quarter" ), _TL("")	},
	{	_TL("Mean Temperature of Coldest Quarter" ), _TL("")	},
	{	_TL("Annual Precipitation"                ), _TL("")	},
	{	_TL("Precipitation of Wettest Month"      ), _TL("")	},
	{	_TL("Precipitation of Driest Month"       ), _TL("")	},
	{	_TL("Precipitation Seasonality"           ), _TL("")	},
	{	_TL("Precipitation of Wettest Quarter"    ), _TL("")	},
	{	_TL("Precipitation of Driest Quarter"     ), _TL("")	},
	{	_TL("Precipitation of Warmest Quarter"    ), _TL("")	},
	{	_TL("Precipitation of Coldest Quarter"    ), _TL("")	}
};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CBioclimatic_Vars::CBioclimatic_Vars(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Bioclimatic Variables"));

	Set_Author		("O.Conrad (c) 2016");

	Set_Description	(_TW(
		""
	));

	//-----------------------------------------------------
	Parameters.Add_Grid_List(
		NULL	, "TMEAN"		, _TL("Mean Temperature"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid_List(
		NULL	, "TMIN"		, _TL("Minimum Temperature"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid_List(
		NULL	, "TMAX"		, _TL("Maximum Temperature"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid_List(
		NULL	, "P"			, _TL("Precipitation"),
		_TL(""),
		PARAMETER_INPUT
	);

	//-----------------------------------------------------
	for(int i=0; i<NVARS; i++)
	{
		Parameters.Add_Grid(NULL, CSG_String::Format("BIO_%02d", i + 1), Vars[i][0], Vars[i][1], PARAMETER_OUTPUT);
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CBioclimatic_Vars::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	return( CSG_Module_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CBioclimatic_Vars::On_Execute(void)
{
	//-----------------------------------------------------
	m_pT		= Parameters("TMEAN")->asGridList();
	m_pTmin		= Parameters("TMIN" )->asGridList();
	m_pTmax		= Parameters("TMAX" )->asGridList();
	m_pP		= Parameters("P"    )->asGridList();

	if( m_pT   ->Get_Count() != 12
	||  m_pTmin->Get_Count() != 12
	||  m_pTmax->Get_Count() != 12
	||  m_pP   ->Get_Count() != 12 )
	{
		Error_Set(_TL("there has to be one input grid for each month"));

		return( false );
	}

	for(int i=0; i<NVARS; i++)
	{
		m_pVars[i]	= Parameters(CSG_String::Format("BIO_%02d", i + 1))->asGrid();
	}

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
	//	#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			if( !Set_Variables(x, y) )
			{
				Set_NoData(x, y);
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
void CBioclimatic_Vars::Set_NoData(int x, int y)
{
	for(int i=0; i<NVARS; i++)
	{
		SG_GRID_PTR_SAFE_SET_NODATA(m_pVars[i], x, y);
	}
}

//---------------------------------------------------------
bool CBioclimatic_Vars::Set_Variables(int x, int y)
{
	int		i;

	CSG_Vector	T(12), Tmin(12), Tmax(12), P(12), dTD(12), T4(12), P4(12);
	
	//-----------------------------------------------------
	for(i=0; i<12; i++)
	{
		if( m_pT   ->asGrid(i)->is_NoData(x, y)
		||  m_pTmin->asGrid(i)->is_NoData(x, y)
		||  m_pTmax->asGrid(i)->is_NoData(x, y)
		||  m_pP   ->asGrid(i)->is_NoData(x, y) )
		{
			return( false );
		}

		T   [i]	= m_pT   ->asGrid(i)->asDouble(x, y);
		Tmin[i]	= m_pTmin->asGrid(i)->asDouble(x, y);
		Tmax[i]	= m_pTmax->asGrid(i)->asDouble(x, y);
		P   [i]	= m_pP   ->asGrid(i)->asDouble(x, y);
		dTD [i]	= Tmax[i] - Tmin[i];
	}

	//-----------------------------------------------------
	int		T4min = 0, T4max = 0, P4min = 0, P4max = 0;

	for(i=0; i<12; i++)
	{
		T4[i]	= (T[i] + T[(i + 1) % 12] + T[(i + 13) % 12]) / 3;

		if( T4[i] < T4[T4min] )	{	T4min	= i;	}
		if( T4[i] > T4[T4max] )	{	T4max	= i;	}

		P4[i]	= (P[i] + P[(i + 1) % 12] + P[(i + 13) % 12]);

		if( P4[i] < P4[P4min] )	{	P4min	= i;	}
		if( P4[i] > P4[P4max] )	{	P4max	= i;	}
	}

	//-----------------------------------------------------
	CSG_Simple_Statistics sT(T), sTmin(Tmin), sTmax(Tmax), sP(P), sdTD(dTD);

	// Annual Mean Temperature
	SG_GRID_PTR_SAFE_SET_VALUE(m_pVars[ 0], x, y, sT.Get_Mean());

	// Mean Diurnal Range (Mean of monthly (max temp - min temp))
	SG_GRID_PTR_SAFE_SET_VALUE(m_pVars[ 1], x, y, sdTD.Get_Mean());

	// Isothermality (BIO2/BIO7) (* 100)
	if( sTmax.Get_Maximum() - sTmin.Get_Minimum() > 0.0 )
	{
		SG_GRID_PTR_SAFE_SET_VALUE(m_pVars[ 2], x, y, 100.0 * sdTD.Get_Mean() / (sTmax.Get_Maximum() - sTmin.Get_Minimum()));
	}
	else
	{
		SG_GRID_PTR_SAFE_SET_NODATA(m_pVars[ 2], x, y);
	}

	// Temperature Seasonality (standard deviation *100)
	SG_GRID_PTR_SAFE_SET_VALUE(m_pVars[ 3], x, y, 100.0 * sT.Get_StdDev());

	// Max Temperature of Warmest Month
	SG_GRID_PTR_SAFE_SET_VALUE(m_pVars[ 4], x, y, sTmax.Get_Maximum());

	// Min Temperature of Coldest Month
	SG_GRID_PTR_SAFE_SET_VALUE(m_pVars[ 5], x, y, sTmin.Get_Minimum());

	// Temperature Annual Range (BIO5-BIO6)
	SG_GRID_PTR_SAFE_SET_VALUE(m_pVars[ 6], x, y, sTmax.Get_Maximum() - sTmin.Get_Minimum());

	// Mean Temperature of Wettest Quarter
	SG_GRID_PTR_SAFE_SET_VALUE(m_pVars[ 7], x, y, T4[P4max]);

	// Mean Temperature of Driest Quarter
	SG_GRID_PTR_SAFE_SET_VALUE(m_pVars[ 8], x, y, T4[P4min]);

	// Mean Temperature of Warmest Quarter
	SG_GRID_PTR_SAFE_SET_VALUE(m_pVars[ 9], x, y, T4[T4max]);

	// Mean Temperature of Coldest Quarter
	SG_GRID_PTR_SAFE_SET_VALUE(m_pVars[10], x, y, T4[T4min]);

	// Annual Precipitation
	SG_GRID_PTR_SAFE_SET_VALUE(m_pVars[11], x, y, sP.Get_Mean());

	// Precipitation of Wettest Month
	SG_GRID_PTR_SAFE_SET_VALUE(m_pVars[12], x, y, sP.Get_Maximum());

	// Precipitation of Driest Month
	SG_GRID_PTR_SAFE_SET_VALUE(m_pVars[13], x, y, sP.Get_Minimum());

	// Precipitation Seasonality (Coefficient of Variation)
	SG_GRID_PTR_SAFE_SET_VALUE(m_pVars[14], x, y, sP.Get_StdDev() * 100.0 /sP.Get_Mean());

	// Precipitation of Wettest Quarter
	SG_GRID_PTR_SAFE_SET_VALUE(m_pVars[15], x, y, P4[P4max]);

	// Precipitation of Driest Quarter
	SG_GRID_PTR_SAFE_SET_VALUE(m_pVars[16], x, y, P4[P4min]);

	// Precipitation of Warmest Quarter
	SG_GRID_PTR_SAFE_SET_VALUE(m_pVars[17], x, y, P4[T4max]);

	// Precipitation of Coldest Quarter
	SG_GRID_PTR_SAFE_SET_VALUE(m_pVars[18], x, y, P4[T4min]);

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
/*
Source: Jeremy VanDerWal, Bioclim - Bioclimatic Variables (https://rforge.net/doc/packages/climates/bioclim.html)

The quarterly parameters are not aligned to any calendar quarters. BIOCLIM's definition of a quarter is any 13 consecutive weeks, (or any consecutive 3 months if running with a monthly time step). For example, the driest quarter will be the 13 consecutive weeks that are drier than any other set of 13 consecutive weeks.

    Annual Mean Temperature
    The mean of all the weekly mean temperatures. Each weekly mean temperature is the mean of that week's maximum and minimum temperature.

    Mean Diurnal Range(Mean(period max-min))
    The mean of all the weekly diurnal temperature ranges. Each weekly diurnal range is the difference between that week's maximum and minimum temperature.

    Isothermality 2/7
    The mean diurnal range (parameter 2) divided by the Annual Temperature Range (parameter 7).

    Temperature Seasonality
    ANUCLIM (cov=TRUE) returns the temperature Coefficient of Variation (C of V) as the standard deviation of the weekly mean temperatures expressed as a percentage of the mean of those temperatures (i.e. the annual mean). For this calculation, the mean in degrees Kelvin is used. This avoids the possibility of having to divide by zero, but does mean that the values are usually quite small.
    Worldclim (cov=FALSE) returns the the standard deviation of the weekly mean temperatures.

    Max Temperature of Warmest Period
    The highest temperature of any weekly maximum temperature.

    Min Temperature of Coldest Period
    The lowest temperature of any weekly minimum temperature.

    Temperature Annual Range (5-6)
    The difference between the Max Temperature of Warmest Period and the Min Temperature of Coldest Period.

    Mean Temperature of Wettest Quarter
    The wettest quarter of the year is determined (to the nearest week), and the mean temperature of this period is calculated.

    Mean Temperature of Driest Quarter
    The driest quarter of the year is determined (to the nearest week), and the mean temperature of this period is calculated.

    Mean Temperature of Warmest Quarter
    The warmest quarter of the year is determined (to the nearest week), and the mean temperature of this period is calculated.

    Mean Temperature of Coldest Quarter
    The coldest quarter of the year is determined (to the nearest week), and the mean temperature of this period is calculated.

    Annual Precipitation
    The sum of all the monthly precipitation estimates.

    Precipitation of Wettest Period
    The precipitation of the wettest week or month, depending on the time step.

    Precipitation of Driest Period
    The precipitation of the driest week or month, depending on the time step.

    Precipitation Seasonality(C of V)
    The Coefficient of Variation (C of V) is the standard deviation of the weekly precipitation estimates expressed as a percentage of the mean of those estimates (i.e. the annual mean).

    Precipitation of Wettest Quarter
    The wettest quarter of the year is determined (to the nearest week), and the total precipitation over this period is calculated.

    Precipitation of Driest Quarter
    The driest quarter of the year is determined (to the nearest week), and the total precipitation over this period is calculated.

    Precipitation of Warmest Quarter
    The warmest quarter of the year is determined (to the nearest week), and the total precipitation over this period is calculated.

    Precipitation of Coldest Quarter
    The coldest quarter of the year is determined (to the nearest week), and the total precipitation over this period is calculated.
/**/


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
