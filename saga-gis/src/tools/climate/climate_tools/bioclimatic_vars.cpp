
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
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
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
	Set_Name		(_TL("Bioclimatic Variables"));

	Set_Author		("O.Conrad (c) 2016");

	Set_Description	(_TW(
		"This tool calculates biogically meaningful variables from "
		"monthly climate data (mean, minimum and maximum temperature "
		"and precipitation), as provided e.g. by the <a href=\"http://worldclim.org\">"
		"WorldClim - Global Climate Data</a> project.\n"
		"<p>"
		"The implementation follows the definitions given by Jeremy van der Wal at "
		"<a href=\"https://rforge.net/doc/packages/climates/bioclim.html\">BioClim - Bioclimatic Variables</a>:<ol>"
		"<li><b>Annual Mean Temperature:</b>"
		" The mean of all the monthly mean temperatures."
		" Each monthly mean temperature is the mean of that month's maximum and minimum temperature.</li>"
		"<li><b>Mean Diurnal Range:</b>"
		" The annual mean of all the monthly diurnal temperature ranges."
		" Each monthly diurnal range is the difference between that month's maximum and minimum temperature.</li>"
		"<li><b>Isothermality:</b>"
		" The mean diurnal range (parameter 2) divided by the annual temperature range (parameter 7).</li>"
		"<li><b>Temperature Seasonality:</b>"
		" returns either<ul><li>"
		" the temperature coefficient of variation as the standard deviation of the monthly mean temperatures"
		" expressed as a percentage of the mean of those temperatures (i.e. the annual mean). For this calculation,"
		" the mean in degrees Kelvin is used. This avoids the possibility of having to divide by zero,"
		" but does mean that the values are usually quite small.</li><li>"
		" the standard deviation of the monthly mean temperatures.</li></ul>"
		"<li><b>Maximum Temperature of Warmest Period:</b>"
		" The highest temperature of any monthly maximum temperature.</li>"
		"<li><b>Minimum Temperature of Coldest Period:</b>"
		" The lowest temperature of any monthly minimum temperature.</li>"
		"<li><b>Temperature Annual Range:</b>"
		" The difference between the Maximum Temperature of Warmest Period"
		" and the Minimum Temperature of Coldest Period.</li>"
		"<li><b>Mean Temperature of Wettest Quarter:</b>"
		" The wettest quarter of the year is determined (to the nearest month),"
		" and the mean temperature of this period is calculated.</li>"
		"<li><b>Mean Temperature of Driest Quarter:</b>"
		" The driest quarter of the year is determined (to the nearest month),"
		" and the mean temperature of this period is calculated.</li>"
		"<li><b>Mean Temperature of Warmest Quarter:</b>"
		" The warmest quarter of the year is determined (to the nearest month),"
		" and the mean temperature of this period is calculated.</li>"
		"<li><b>Mean Temperature of Coldest Quarter:</b>"
		" The coldest quarter of the year is determined (to the nearest month),"
		" and the mean temperature of this period is calculated.</li>"
		"<li><b>Annual Precipitation:</b>"
		" The sum of all the monthly precipitation estimates.</li>"
		"<li><b>Precipitation of Wettest Period:</b>"
		" The precipitation of the wettest month.</li>"
		"<li><b>Precipitation of Driest Period:</b>"
		" The precipitation of the driest month.</li>"
		"<li><b>Precipitation Seasonality:</b>"
		" The Coefficient of Variation is the standard deviation of"
		" the monthly precipitation estimates expressed as a percentage of the"
		" mean of those estimates (i.e. the annual mean).</li>"
		"<li><b>Precipitation of Wettest Quarter:</b>"
		" The wettest quarter of the year is determined (to the nearest month),"
		" and the total precipitation over this period is calculated.</li>"
		"<li><b>Precipitation of Driest Quarter:</b>"
		" The driest quarter of the year is determined (to the nearest month),"
		" and the total precipitation over this period is calculated.</li>"
		"<li><b>Precipitation of Warmest Quarter:</b>"
		" The warmest quarter of the year is determined (to the nearest month),"
		" and the total precipitation over this period is calculated.</li>"
		"<li><b>Precipitation of Coldest Quarter:</b>"
		" The coldest quarter of the year is determined (to the nearest month),"
		" and the total precipitation over this period is calculated.</li>"
		"</ol></p><p>"
		"The quarterly parameters are not aligned to any calendar quarters. "
		"BioClim's definition of a quarter is any consecutive 3 months. "
		"For example, the driest quarter will be the 3 consecutive months that "
		"are drier than any other set of 3 consecutive months."
		"</p>"
	));

	//-----------------------------------------------------
	Parameters.Add_Grid_List("",
		"TMEAN"	, _TL("Mean Temperature"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid_List("",
		"TMIN"	, _TL("Minimum Temperature"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid_List("",
		"TMAX"	, _TL("Maximum Temperature"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid_List("",
		"P"		, _TL("Precipitation"),
		_TL(""),
		PARAMETER_INPUT
	);

	//-----------------------------------------------------
	for(int i=0; i<NVARS; i++)
	{
		Parameters.Add_Grid("", CSG_String::Format("BIO_%02d", i + 1), Vars[i][0], Vars[i][1], PARAMETER_OUTPUT);
	}

	#define ADD_TIME_GRID(id, name)	Parameters.Add_Grid("", id, name,\
		_TL("Result is the number of the 2nd of three consecutive months (Dec-Jan-Feb=1, Jan-Feb-Mar=2, ...)."),\
		PARAMETER_OUTPUT_OPTIONAL, true, SG_DATATYPE_Char)

	ADD_TIME_GRID("QUARTER_COLDEST", _TL("Coldest Quarter"));
	ADD_TIME_GRID("QUARTER_WARMEST", _TL("Warmest Quarter"));
	ADD_TIME_GRID("QUARTER_DRIEST" , _TL("Driest Quarter" ));
	ADD_TIME_GRID("QUARTER_WETTEST", _TL("Wettest Quarter"));

	Parameters.Add_Choice("",
		"SEASONALITY", _TL("Temperature Seasonality"),
		_TL(""),
		CSG_String::Format("%s|%s|",
			_TL("Coefficient of Variation"),
			_TL("Standard Deviation")
		), 1
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CBioclimatic_Vars::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CBioclimatic_Vars::On_Execute(void)
{
	m_pT    = Parameters("TMEAN")->asGridList();
	m_pTmin = Parameters("TMIN" )->asGridList();
	m_pTmax = Parameters("TMAX" )->asGridList();
	m_pP    = Parameters("P"    )->asGridList();

	if( m_pT   ->Get_Grid_Count() != 12
	||  m_pTmin->Get_Grid_Count() != 12
	||  m_pTmax->Get_Grid_Count() != 12
	||  m_pP   ->Get_Grid_Count() != 12 )
	{
		Error_Set(_TL("there has to be one input grid for each month"));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Colors	Colors(10);

	Colors.Set_Color(0, 254, 135, 000);
	Colors.Set_Color(1, 254, 194, 063);
	Colors.Set_Color(2, 254, 254, 126);
	Colors.Set_Color(3, 231, 231, 227);
	Colors.Set_Color(4, 132, 222, 254);
	Colors.Set_Color(5, 042, 163, 239);
	Colors.Set_Color(6, 000, 105, 224);
	Colors.Set_Color(7, 000, 047, 210);
	Colors.Set_Color(8, 000, 001, 156);
	Colors.Set_Color(9, 000, 000, 103);

	for(int i=0; i<NVARS; i++)
	{
		m_pVars[i]	= Parameters(CSG_String::Format("BIO_%02d", i + 1))->asGrid();

		if( i > 10 )	// Precipitation
		{
			DataObject_Set_Colors(m_pVars[i], Colors);
		}
	}

	Set_Quarter_Classes(m_pVars[NVARS + 0] = Parameters("QUARTER_COLDEST")->asGrid());
	Set_Quarter_Classes(m_pVars[NVARS + 1] = Parameters("QUARTER_WARMEST")->asGrid());
	Set_Quarter_Classes(m_pVars[NVARS + 2] = Parameters("QUARTER_DRIEST" )->asGrid());
	Set_Quarter_Classes(m_pVars[NVARS + 3] = Parameters("QUARTER_WETTEST")->asGrid());

	m_Seasonality	= Parameters("SEASONALITY")->asInt();

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
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
void CBioclimatic_Vars::Set_Quarter_Classes(CSG_Grid *pGrid)
{
	CSG_Parameter	*pLUT	= DataObject_Get_Parameter(pGrid, "LUT");

	if( pLUT && pLUT->asTable() )
	{
		const CSG_String Month[12] = {
			"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
		};

		pLUT->asTable()->Del_Records();

		for(int i=0, j=11, k=10; i<12; k=j, j=i++)
		{
			CSG_Table_Record	*pClass	= pLUT->asTable()->Add_Record();

			#define GET_VALUE(c) 255. * (0.5 + 0.5 * cos(M_PI * (j / 6. +  c * 2. / 3.)))

			pClass->Set_Value(0, SG_GET_RGB(GET_VALUE(1), GET_VALUE(2), GET_VALUE(0)));
			pClass->Set_Value(1, Month[k] + "-" + Month[j] + "-" + Month[i]);
			pClass->Set_Value(2, Month[k] + "-" + Month[j] + "-" + Month[i]);
			pClass->Set_Value(3, i + 1);
			pClass->Set_Value(4, i + 1);
		}

		DataObject_Set_Parameter(pGrid, pLUT);
		DataObject_Set_Parameter(pGrid, "COLORS_TYPE", 1);	// Classified
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CBioclimatic_Vars::Set_NoData(int x, int y)
{
	for(int i=0; i<NVARS+4; i++)
	{
		SG_GRID_PTR_SAFE_SET_NODATA(m_pVars[i], x, y);
	}
}

//---------------------------------------------------------
bool CBioclimatic_Vars::Set_Variables(int x, int y)
{
	CSG_Vector	T(12), Tmin(12), Tmax(12), P(12), dTD(12), T3(12), P3(12);
	
	//-----------------------------------------------------
	for(int i=0; i<12; i++)
	{
		if( m_pT   ->Get_Grid(i)->is_NoData(x, y)
		||  m_pTmin->Get_Grid(i)->is_NoData(x, y)
		||  m_pTmax->Get_Grid(i)->is_NoData(x, y)
		||  m_pP   ->Get_Grid(i)->is_NoData(x, y) )
		{
			return( false );
		}

		T   [i]	= m_pT   ->Get_Grid(i)->asDouble(x, y);
		Tmin[i]	= m_pTmin->Get_Grid(i)->asDouble(x, y);
		Tmax[i]	= m_pTmax->Get_Grid(i)->asDouble(x, y);
		P   [i]	= m_pP   ->Get_Grid(i)->asDouble(x, y);
		dTD [i]	= Tmax[i] - Tmin[i];
	}

	//-----------------------------------------------------
	int		T3min = 0, T3max = 0, P3min = 0, P3max = 0;

	for(int i=0, j=11, k=10; i<12; k=j, j=i++)
	{	// find the minima and maxima for three consecutive months' mean temperatures and precipitation sums
		T3[i]	= (T[i] + T[j] + T[k]) / 3.; // mean temperature
		P3[i]	= (P[i] + P[j] + P[k])     ; // precipitation sum

		if( T3[i] < T3[T3min] ) { T3min = i; }
		if( T3[i] > T3[T3max] ) { T3max = i; }
		if( P3[i] < P3[P3min] ) { P3min = i; }
		if( P3[i] > P3[P3max] ) { P3max = i; }
	}

	//-----------------------------------------------------
	CSG_Simple_Statistics sT(T), sTmin(Tmin), sTmax(Tmax), sP(P), sdTD(dTD);

	// Annual Mean Temperature
	SG_GRID_PTR_SAFE_SET_VALUE(m_pVars[ 0], x, y, sT.Get_Mean());

	// Mean Diurnal Range (Mean of monthly (max temp - min temp))
	SG_GRID_PTR_SAFE_SET_VALUE(m_pVars[ 1], x, y, sdTD.Get_Mean());

	// Isothermality (BIO2/BIO7) (* 100)
	if( sTmax.Get_Maximum() - sTmin.Get_Minimum() > 0. )
	{
		SG_GRID_PTR_SAFE_SET_VALUE(m_pVars[ 2], x, y, 100. * sdTD.Get_Mean() / (sTmax.Get_Maximum() - sTmin.Get_Minimum()));
	}
	else
	{
		SG_GRID_PTR_SAFE_SET_NODATA(m_pVars[ 2], x, y);
	}

	// Temperature Seasonality
	if( m_Seasonality == 0 )
	{	// standard deviation of the mean temperatures expressed as a percentage of the mean of those temperatures (i.e. the annual mean)
		SG_GRID_PTR_SAFE_SET_VALUE(m_pVars[ 3], x, y, 100. * sT.Get_StdDev() / (sT.Get_Mean() + 273.15));
	}
	else
	{	// standard deviation * 100
		SG_GRID_PTR_SAFE_SET_VALUE(m_pVars[ 3], x, y, 100. * sT.Get_StdDev());
	}

	// Max Temperature of Warmest Month
	SG_GRID_PTR_SAFE_SET_VALUE(m_pVars[ 4], x, y, sTmax.Get_Maximum());

	// Min Temperature of Coldest Month
	SG_GRID_PTR_SAFE_SET_VALUE(m_pVars[ 5], x, y, sTmin.Get_Minimum());

	// Temperature Annual Range (BIO5-BIO6)
	SG_GRID_PTR_SAFE_SET_VALUE(m_pVars[ 6], x, y, sTmax.Get_Maximum() - sTmin.Get_Minimum());

	// Mean Temperature of Wettest Quarter
	SG_GRID_PTR_SAFE_SET_VALUE(m_pVars[ 7], x, y, T3[P3max]);

	// Mean Temperature of Driest Quarter
	SG_GRID_PTR_SAFE_SET_VALUE(m_pVars[ 8], x, y, T3[P3min]);

	// Mean Temperature of Warmest Quarter
	SG_GRID_PTR_SAFE_SET_VALUE(m_pVars[ 9], x, y, T3[T3max]);

	// Mean Temperature of Coldest Quarter
	SG_GRID_PTR_SAFE_SET_VALUE(m_pVars[10], x, y, T3[T3min]);

	// Annual Precipitation
	SG_GRID_PTR_SAFE_SET_VALUE(m_pVars[11], x, y, sP.Get_Sum());

	// Precipitation of Wettest Month
	SG_GRID_PTR_SAFE_SET_VALUE(m_pVars[12], x, y, sP.Get_Maximum());

	// Precipitation of Driest Month
	SG_GRID_PTR_SAFE_SET_VALUE(m_pVars[13], x, y, sP.Get_Minimum());

	// Precipitation Seasonality (Coefficient of Variation)
	SG_GRID_PTR_SAFE_SET_VALUE(m_pVars[14], x, y, sP.Get_StdDev() * 100. / sP.Get_Mean());

	// Precipitation of Wettest Quarter
	SG_GRID_PTR_SAFE_SET_VALUE(m_pVars[15], x, y, P3[P3max]);

	// Precipitation of Driest Quarter
	SG_GRID_PTR_SAFE_SET_VALUE(m_pVars[16], x, y, P3[P3min]);

	// Precipitation of Warmest Quarter
	SG_GRID_PTR_SAFE_SET_VALUE(m_pVars[17], x, y, P3[T3max]);

	// Precipitation of Coldest Quarter
	SG_GRID_PTR_SAFE_SET_VALUE(m_pVars[18], x, y, P3[T3min]);

	// Number of Coldest Quarter
	SG_GRID_PTR_SAFE_SET_VALUE(m_pVars[19], x, y, 1 + T3min);

	// Number of Warmest Quarter
	SG_GRID_PTR_SAFE_SET_VALUE(m_pVars[20], x, y, 1 + T3max);

	// Number of Driest Quarter
	SG_GRID_PTR_SAFE_SET_VALUE(m_pVars[21], x, y, 1 + P3min);

	// Number of Wettest Quarter
	SG_GRID_PTR_SAFE_SET_VALUE(m_pVars[22], x, y, 1 + P3max);

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
