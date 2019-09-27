
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                    climate_tools                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                soil_water_balance.cpp                 //
//                                                       //
//                 Copyrights (C) 2019                   //
//                     Olaf Conrad                       //
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
#include "soil_water_balance.h"

#include "climate_tools.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSoil_Water_Balance::CSoil_Water_Balance(void)
{
	Set_Name		(_TL("Soil Water Balance (Days)"));

	Set_Author		("O.Conrad (c) 2019");

	Set_Description	(_TW(
		"A Simple Soil Water Balance Model"
	));

	Add_Reference("Paulsen, J. / Körner, C.", "2014",
		"A climate-based model to predict potential treeline position around the globe",
		"Alpine Botany, 124:1, 1–12. doi:10.1007/s00035-014-0124-0.",
		SG_T("http://link.springer.com/article/10.1007%2Fs00035-014-0124-0"), _TL("online")
	);

	//-----------------------------------------------------
	Parameters.Add_Grid_List("", "TAVG", _TL("Mean Temperature"   ), _TL(""), PARAMETER_INPUT);
	Parameters.Add_Grid_List("", "TMIN", _TL("Minimum Temperature"), _TL(""), PARAMETER_INPUT);
	Parameters.Add_Grid_List("", "TMAX", _TL("Maximum Temperature"), _TL(""), PARAMETER_INPUT);
	Parameters.Add_Grid_List("", "PSUM", _TL("Precipitation"      ), _TL(""), PARAMETER_INPUT);

	//-----------------------------------------------------
	Parameters.Add_Grid("", "SNOW" , _TL("Snow Depth"              ), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid("", "SW_0" , _TL("Soil Water (Upper Layer)"), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid("", "SW_1" , _TL("Soil Water (Lower Layer)"), _TL(""), PARAMETER_OUTPUT);

	//-----------------------------------------------------
	Parameters.Add_Grid("",
		"LAT_GRID"		, _TL("Latitude"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Double("LAT_GRID",
		"LAT_CONST"		, _TL("Default Latitude"),
		_TL(""),
		50., -90., true, 90., true
	);

	//-----------------------------------------------------
	Parameters.Add_Grid_or_Const("",
		"SWC"			, _TL("Soil Water Capacity of Profile"),
		_TL("Total soil water capacity (mm H2O)."),
		220., 0., true
	);

	Parameters.Add_Double("SWC",
		"SWC_SURFACE"	, _TL("Top Soil Water Capacity"),
		_TL(""),
		30., 0., true
	);

	Parameters.Add_Double("SWC",
		"SWT_RESIST"	, _TL("Transpiration Resistance"),
		_TL(""),
		0.5, 0.01, true
	);

	//-----------------------------------------------------
	Parameters.Add_Bool("",
		"RESET"			, _TL("Reset"),
		_TL(""),
		true
	);

	Parameters.Add_Date("",
		"DAY"		, _TL("Start Day"),
		_TL("")
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CSoil_Water_Balance::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSoil_Water_Balance::Initialize(void)
{
	m_pTavg	= Parameters("TAVG")->asGridList();
	m_pTmin	= Parameters("TMIN")->asGridList();
	m_pTmax	= Parameters("TMAX")->asGridList();
	m_pPsum	= Parameters("PSUM")->asGridList();

	if( m_pTavg->Get_Grid_Count() < 1
	||  m_pTmin->Get_Grid_Count() < 1
	||  m_pTmax->Get_Grid_Count() < 1
	||  m_pPsum->Get_Grid_Count() < 1 )
	{
		Error_Set(_TL("missing input data"));

		return( false );
	}

	//-----------------------------------------------------
	m_Lat_const	= Parameters("LAT_CONST")->asDouble();
	m_pLat_Grid	= Parameters("LAT_GRID" )->asGrid  ();

	if( !m_pLat_Grid )
	{
		m_pLat_Grid	= SG_Grid_Get_Geographic_Coordinates(m_pTavg->Get_Grid(0), NULL, &m_Lat_Grid) ? &m_Lat_Grid : NULL;
	}

	//-----------------------------------------------------
	m_SWC	= Parameters("SWC")->asDouble();
	m_pSWC	= Parameters("SWC")->asGrid  ();
	m_SWC_0	= Parameters("SWC_SURFACE")->asDouble();

	m_SWT_Resist[0]	= 0.;
	m_SWT_Resist[1]	= Parameters("SWT_RESIST")->asDouble();

	//-----------------------------------------------------
	m_pSnow		= Parameters("SNOW")->asGrid();
	m_pSW[0]	= Parameters("SW_0")->asGrid();
	m_pSW[1]	= Parameters("SW_1")->asGrid();

	if( Parameters("RESET")->asBool() )
	{
		m_pSnow ->Assign(0.);

		#pragma omp parallel for
		for(sLong i=0; i<Get_NCells(); i++)
		{
			double	SWCtotal	= 0.75 * (m_pSWC && !m_pSWC->is_NoData(i) ? m_pSWC->asDouble(i) : m_SWC);

			m_pSW[0]->Set_Value(i, 0.1 * SWCtotal);
			m_pSW[1]->Set_Value(i, 0.9 * SWCtotal);
		}
	}

	//-----------------------------------------------------
	return( true );
}

//---------------------------------------------------------
bool CSoil_Water_Balance::Finalize(void)
{
	m_Lat_Grid.Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSoil_Water_Balance::On_Execute(void)
{
	if( !Initialize() )
	{
		Finalize();

		return( false );
	}

	//-----------------------------------------------------
	int	nDays	= m_pTavg->Get_Grid_Count();

	if( nDays > m_pTmin->Get_Grid_Count() ) nDays = m_pTmin->Get_Grid_Count();
	if( nDays > m_pTmax->Get_Grid_Count() ) nDays = m_pTmax->Get_Grid_Count();
	if( nDays > m_pPsum->Get_Grid_Count() ) nDays = m_pPsum->Get_Grid_Count();

	if( nDays < 1 )
	{
		Error_Set(_TL("there has to be a minimum of one day's input for each weather variable"));

		Finalize();

		return( false );
	}

	CSG_DateTime	Date	= Parameters("DAY")->asDate()->Get_Date();

	//-----------------------------------------------------
	for(int Day=0; Day<nDays && Process_Get_Okay(); Day++, Date+=CSG_TimeSpan(24.))
	{
		Process_Set_Text(Date.Format(CSG_String::Format("%s: %%Y-%%m-%%d [%d/%d]", _TL("Date"), Day + 1, nDays)));

		for(int y=0; y<Get_NY() && Set_Progress(y); y++)
		{
			#ifndef _DEBUG
			#pragma omp parallel for
			#endif
			for(int x=0; x<Get_NX(); x++)
			{
				if( !Set_Day(x, y, Day, Date) )
				{
					m_pSnow ->Set_NoData(x, y);
					m_pSW[0]->Set_NoData(x, y);
					m_pSW[1]->Set_NoData(x, y);
				}
			}
		}
	}

	//-----------------------------------------------------
	Finalize();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSoil_Water_Balance::Get_SW_Capacity(int x, int y, double SWC[2])
{
	SWC[0]	=  20.;
	SWC[1]	= 200.;

	double	SWCtotal	= m_pSWC && !m_pSWC->is_NoData(x, y) ? m_pSWC->asDouble(x, y) : m_SWC;

	if( SWCtotal < SWC[0] )
	{
		SWC[0]	= SWCtotal;
		SWC[1]	= 0.;
	}
	else
	{
		SWC[1]	= SWCtotal - SWC[0];
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
/**
* updates the snow storage on a daily base and returns the change.
*/
//---------------------------------------------------------
inline double CSoil_Water_Balance::Get_Snow_Storage(int x, int y, double T, double P)
{
	double	Snow	= m_pSnow->asDouble(x, y);

	//-----------------------------------------------------
	if( T <= 0. )
	{
		m_pSnow->Add_Value(x, y, P);

		return( P );
	}

	//-----------------------------------------------------
	if( Snow <= 0. )
	{
		return( 0. );
	}

	//-----------------------------------------------------
	// if( Snow > 0. && T > 0. )...

	double	dSnow	= T * (0.84 + 0.125 * P);	// according to: Paulsen, J. & Körner, C. (2014): A climate-based model to predict potential snow_cover position around the globe. Alpine Botany.

	if( dSnow > Snow )
	{
		m_pSnow->Set_Value(x, y, 0.);

		return( -Snow );
	}

	m_pSnow->Set_Value(x, y, Snow - dSnow);

	return( -dSnow );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline bool CSoil_Water_Balance::Get_Weather(int x, int y, int Day, const CSG_DateTime &Date, double &T, double &P, double &ETpot)
{
	if( m_pTavg->Get_Grid(Day)->is_NoData(x, y)
	||  m_pTmin->Get_Grid(Day)->is_NoData(x, y)
	||  m_pTmax->Get_Grid(Day)->is_NoData(x, y)
	||  m_pPsum->Get_Grid(Day)->is_NoData(x, y)	)
	{
		return( false );
	}

	T	= m_pTavg->Get_Grid(Day)->asDouble(x, y);
	P	= m_pPsum->Get_Grid(Day)->asDouble(x, y);

	ETpot	= CT_Get_ETpot_Hargreave(T,
		m_pTmin->Get_Grid(Day)->asDouble(x, y),
		m_pTmax->Get_Grid(Day)->asDouble(x, y),
		Date.Get_DayOfYear(), m_pLat_Grid ? m_pLat_Grid->asDouble(x, y) : m_Lat_const
	);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSoil_Water_Balance::Set_Day(int x, int y, int Day, const CSG_DateTime &Date)
{
	double	T, P, ETpot;

	if( !Get_Weather(x, y, Day, Date, T, P, ETpot) )
	{
		return( false );
	}

	double	dSnow	= Get_Snow_Storage(x, y, T, P);

	if( T <= 0. )
	{
		return( true );
	}

	//---------------------------------------------
	// upper soil layer

	double	SW[2];

	SW[0]	= m_pSW[0]->asDouble(x, y);
	SW[1]	= m_pSW[1]->asDouble(x, y);

	double	dSW	= P - dSnow;

	if( m_pSnow->asDouble(x, y) <= 0. )
	{
		dSW	-= ETpot;
	}

	SW[0]	+= dSW;

	double	SWC[2];	Get_SW_Capacity(x, y, SWC);

	if( SW[0] > SWC[0] )	// more water in upper soil layer than its capacity
	{
		dSW		= SW[0] - SWC[0];
		SW[0]	= SWC[0];
	}
	else if( SW[0] < 0. )	// evapotranspiration exceeds available water
	{	// positive: runoff, negative: loss by evapotranspiration not covered by upper layer, with water loss resistance;
		dSW		= SWC[1] > 0. ? SW[0] * pow(SW[1] / SWC[1], m_SWT_Resist[1]) : 0.;
		SW[0]	= 0.;
	}
	else
	{
		dSW		= 0.;
	}

	//---------------------------------------------
	// lower soil layer

	SW[1]	+= dSW;

	if( SW[1] > SWC[1] )	// more water in lower soil layer than its capacity
	{
		SW[1]	= SWC[1];
	}
	else if( SW[1] < 0. )	// evapotranspiration exceeds available water
	{
		SW[1]	= 0.;
	}

	//---------------------------------------------
	m_pSW[0]->Set_Value(x, y, SW[0]);
	m_pSW[1]->Set_Value(x, y, SW[1]);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
