/**********************************************************
 * Version $Id: treeline.cpp 1380 2012-04-26 12:02:19Z reklov_w $
 *********************************************************/

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
//                     treeline.cpp                      //
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
// Free Software Foundation; version >=2 of the License. //
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "treeline.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CCT_Growing_Season::CCT_Growing_Season(void)
{
	m_DT_min	= 0.9;
	m_LGS_min	= 94;
	m_SMT_min	= 6.4;
	m_SW_min	= 0.2 / 100.0;
}

//---------------------------------------------------------
CCT_Growing_Season::CCT_Growing_Season(const CCT_Growing_Season &Copy)
	: CCT_Water_Balance(Copy)
{
	m_DT_min	= Copy.m_DT_min;
	m_LGS_min	= Copy.m_LGS_min;
	m_SMT_min	= Copy.m_SMT_min;
	m_SW_min	= Copy.m_SW_min;
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCT_Growing_Season::Set_DT_min(double Value)
{
	m_DT_min	= Value;

	return( true );
}

//---------------------------------------------------------
bool CCT_Growing_Season::Set_LGS_min(int Value)
{
	m_LGS_min	= Value;

	return( true );
}

//---------------------------------------------------------
bool CCT_Growing_Season::Set_SMT_min(double Value)
{
	m_SMT_min	= Value;

	return( true );
}

//---------------------------------------------------------
bool CCT_Growing_Season::Set_SW_min(double Value)
{
	if( Value >= 0.0 )
	{
		m_SW_min	= Value;

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCT_Growing_Season::Calculate(double SWC, double Latitude)
{
	CCT_Water_Balance::Calculate(SWC, Latitude);

	return( Get_T_Season(m_Daily[DAILY_T], m_Snow, m_Soil.Get_SW_0(), m_Soil.Get_SW_1()) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCT_Growing_Season::Calculate(double SWC, double Latitude, double &Height, double maxDiff)
{
	Set_Soil_Capacity(SWC);

	//-----------------------------------------------------
	double	dHi, dLo;

	if( is_Growing(SWC, Latitude, 0.0) )
	{
		dLo	= 0.0;	// below tree line

		if( is_Growing(SWC, Latitude, dHi = maxDiff) == true )
		{
			Height	= dHi;

			return( true );
		}
	}
	else
	{
		dHi	= 0.0;	// above tree line

		if( is_Growing(SWC, Latitude, dLo = -maxDiff) == false )
		{
			Height	= dLo;

			return( true );
		}
	}

	//-----------------------------------------------------
	while( dHi - dLo > 10.0 )
	{
		if( is_Growing(SWC, Latitude, Height = dLo + (dHi - dLo) / 2.0) )	// below tree line
		{
			dLo	= Height;
		}
		else			// above tree line
		{
			dHi	= Height;
		}
	}

	//-----------------------------------------------------
	Height	= dLo + (dHi - dLo) / 2.0;

	return( true );
}

//---------------------------------------------------------
bool CCT_Growing_Season::is_Growing(double SWC, double Latitude, double Height)
{
	const double	lapsrate	= -0.0055;	// adiabatic air temperature decline coefficient [°K / m]

	//-----------------------------------------------------
	// 1. Temperature

	CSG_Vector	T	= m_Monthly[MONTHLY_T];	T	+= lapsrate * Height;

	CT_Get_Daily_Splined(m_Daily[DAILY_T], T);

	if( !Get_T_Season(m_Daily[DAILY_T]) )
	{
		return( false );	// above tree line
	}

	//-----------------------------------------------------
	// 2. Snow cover

	CT_Get_Daily_Precipitation(m_Daily[DAILY_P], m_Monthly[MONTHLY_P], T);

	m_Snow.Calculate(m_Daily[DAILY_T], m_Daily[DAILY_P]);

	if( !Get_T_Season(m_Daily[DAILY_T], m_Snow) )
	{
		return( false );	// above tree line
	}

	//-----------------------------------------------------
	// 3. Soil water

	CSG_Vector	Tmin	= m_Monthly[MONTHLY_Tmin];	Tmin	+= lapsrate * Height;
	CSG_Vector	Tmax	= m_Monthly[MONTHLY_Tmax];	Tmax	+= lapsrate * Height;

	m_Soil.Calculate(m_Daily[DAILY_T], m_Daily[DAILY_P], Set_ETpot(Latitude, Tmin, Tmax), m_Snow);

	if( !Get_T_Season(m_Daily[DAILY_T], m_Snow, m_Soil.Get_SW_0(), m_Soil.Get_SW_1()) )
	{
		return( false );	// above tree line
	}

	//-----------------------------------------------------
	return( true );	// below tree line
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCT_Growing_Season::Get_T_Season(const double *T, const double *Snow, const double *S0, const double *S1)
{
	m_T_Season.Create();

	m_GDay_First = m_GDay_Last = -1;	// invalidate first and last growing day

	int	i;	bool	*bGrowing	= new bool[365];

	for(i=0; i<365; i++)	// 1. identify growing days
	{
		bGrowing[i]	= T[i] >= m_DT_min && (!Snow || Snow[i] <= 0.0) && (!(S0 && S1) || (S0[i] > 0.0 || (S1[i] > 0.0 && S1[i] >= m_SW_min * m_Soil.Get_Capacity(1))));
	}

	for(i=0; i<365; i++)	// 2. evaluate growing days
	{
		if( bGrowing[i] )
		{
			m_T_Season	+= T[i];

			if( m_GDay_First < 0 && !bGrowing[(365 + i - 1) % 365] )	// is the previous day a not growing day ?
			{
				m_GDay_First	= i;
			}

			if( m_GDay_Last  < 0 && !bGrowing[(365 + i + 1) % 365] )	// is the following day a not growing day ?
			{
				m_GDay_Last		= i;
			}
		}
	}

	delete[](bGrowing);

	return( m_T_Season.Get_Count() >= m_LGS_min && m_T_Season.Get_Mean() >= m_SMT_min );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTree_Growth::CTree_Growth(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Tree Growth Season"));

	Set_Author		("O.Conrad, L.Landschreiber (c) 2016");

	Set_Description	(_TW(
		"The 'Tree Growth Season' tool estimates the potential number of days "
		"suitable for tree growth as well as the average temperature for these days. "
		"The estimation needs monthly data of mean, minimum, and maximum temperature "
		"and precipitation. Internally a soil water balance model is run on a daily basis. "
		"Using the given thresholds a relative tree line height can optionally be estimated."
	));

	Add_Reference("Paulsen, J. / Körner, C.", "2014",
		"A climate-based model to predict potential treeline position around the globe",
		"Alpine Botany, 124:1, 1–12. doi:10.1007/s00035-014-0124-0.",
		SG_T("http://link.springer.com/article/10.1007%2Fs00035-014-0124-0"), _TL("online")
	);

	//-----------------------------------------------------
	Parameters.Add_Grid_List("", "T"   , _TL("Mean Temperature"   ), _TL(""), PARAMETER_INPUT);
	Parameters.Add_Grid_List("", "TMIN", _TL("Minimum Temperature"), _TL(""), PARAMETER_INPUT);
	Parameters.Add_Grid_List("", "TMAX", _TL("Maximum Temperature"), _TL(""), PARAMETER_INPUT);
	Parameters.Add_Grid_List("", "P"   , _TL("Precipitation"      ), _TL(""), PARAMETER_INPUT);

	//-----------------------------------------------------
	Parameters.Add_Grid_or_Const("",
		"SWC"			, _TL("Soil Water Capacity of Profile"),
		_TL("Total soil water capacity (mm H2O)."),
		220.0, 0.0, true
	);

	Parameters.Add_Double("SWC",
		"SWC_SURFACE"	, _TL("Top Soil Water Capacity"),
		_TL(""),
		30.0, 0.0, true
	);

	Parameters.Add_Double("SWC",
		"SW1_RESIST"	, _TL("Transpiration Resistance"),
		_TL(""),
		0.5, 0.01, true
	);

	Parameters.Add_Double("",
		"LAT_DEF"		, _TL("Default Latitude"),
		_TL(""),
		0, -90, true, 90, true
	);

	//-----------------------------------------------------
	Parameters.Add_Grid("",
		"SMT"		, _TL("Mean Temperature"),
		_TL("Mean temperature of the growing season."),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid("",
		"LGS"		, _TL("Length"),
		_TL("Number of days of the growing season."),
		PARAMETER_OUTPUT, true, SG_DATATYPE_Short
	);

	Parameters.Add_Grid("",
		"FIRST"	, _TL("First Growing Day"),
		_TL("First growing day of the year (1-365)."),
		PARAMETER_OUTPUT_OPTIONAL, true, SG_DATATYPE_Short
	);

	Parameters.Add_Grid("",
		"LAST"	, _TL("Last Growing Day"),
		_TL("Last degree day of the year (1-365)."),
		PARAMETER_OUTPUT_OPTIONAL, true, SG_DATATYPE_Short
	);

	Parameters.Add_Grid("",
		"TLH"		, _TL("Tree Line Height"),
		_TL("Estimated relative tree line height."),
		PARAMETER_OUTPUT_OPTIONAL
	);

	//-----------------------------------------------------
	Parameters.Add_Double("",
		"DT_MIN"		, _TL("Threshold Temperature"),
		_TL("Threshold temperature (C) that constrains the growing season."),
		0.9
	);

	Parameters.Add_Double("",
		"SW_MIN"		, _TL("Minimum Soil Water Content (Percent)"),
		_TL(""),
		2.0, 0.0, true
	);

	Parameters.Add_Int("",
		"LGS_MIN"		, _TL("Minimum Length"),
		_TL("Minimum length (days) of the growing season."),
		94, 1, true
	);

	Parameters.Add_Double("",
		"SMT_MIN"		, _TL("Minimum Mean Temperature"),
		_TL("Minimum mean temperature (C) for all days of the growing season."),
		6.4
	);

	Parameters.Add_Double("",
		"TLH_MAX_DIFF"	, _TL("Maximum Tree Line Height Difference"),
		_TL(""),
		3000.0, 0.0, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CTree_Growth::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTree_Growth::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Parameter_Grid_List	*pT   	= Parameters("T"   )->asGridList();
	CSG_Parameter_Grid_List	*pTmin	= Parameters("TMIN")->asGridList();
	CSG_Parameter_Grid_List	*pTmax	= Parameters("TMAX")->asGridList();
	CSG_Parameter_Grid_List	*pP   	= Parameters("P"   )->asGridList();

	if( pT   ->Get_Grid_Count() != 12
	||  pTmin->Get_Grid_Count() != 12
	||  pTmax->Get_Grid_Count() != 12
	||  pP   ->Get_Grid_Count() != 12 )
	{
		SG_UI_Msg_Add_Error(_TL("there has to be one input grid for each month"));

		return( false );
	}

	//-----------------------------------------------------
	double	Lat_Def	= Parameters("LAT_DEF")->asDouble();

	CSG_Grid Lat; CSG_Grid *pLat = SG_Grid_Get_Geographic_Coordinates(pT->Get_Grid(0), NULL, &Lat) ? &Lat : NULL;

	//-----------------------------------------------------
	double		SWC_Def	= Parameters("SWC")->asDouble();
	CSG_Grid	*pSWC	= Parameters("SWC")->asGrid();

	m_Model.Get_Soil().Set_Capacity     (0, Parameters("SWC_SURFACE")->asDouble());
	m_Model.Get_Soil().Set_ET_Resistance(1, Parameters("SW1_RESIST" )->asDouble());

	//-----------------------------------------------------
	CSG_Grid	*pSMT	= Parameters("SMT"  )->asGrid();
	CSG_Grid	*pLGS	= Parameters("LGS"  )->asGrid();
	CSG_Grid	*pFirst	= Parameters("FIRST")->asGrid();
	CSG_Grid	*pLast	= Parameters("LAST" )->asGrid();
	CSG_Grid	*pTLH	= Parameters("TLH"  )->asGrid();

	DataObject_Set_Colors(pLGS, 11, SG_COLORS_GREEN_GREY_BLUE, true);
	DataObject_Set_Colors(pTLH, 11, SG_COLORS_GREEN_GREY_BLUE, true);

	//-----------------------------------------------------
	double	maxDiff	= Parameters("TLH_MAX_DIFF")->asDouble();

	m_Model.Set_DT_min (Parameters( "DT_MIN")->asDouble());
	m_Model.Set_SMT_min(Parameters("SMT_MIN")->asDouble());
	m_Model.Set_LGS_min(Parameters("LGS_MIN")->asInt   ());
	m_Model.Set_SW_min (Parameters( "SW_MIN")->asDouble() / 100.0);

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
#ifndef _DEBUG
		#pragma omp parallel for
#endif
		for(int x=0; x<Get_NX(); x++)
		{
			SG_GRID_PTR_SAFE_SET_NODATA(pLGS  , x, y);
			SG_GRID_PTR_SAFE_SET_NODATA(pSMT  , x, y);
			SG_GRID_PTR_SAFE_SET_NODATA(pFirst, x, y);
			SG_GRID_PTR_SAFE_SET_NODATA(pLast , x, y);
			SG_GRID_PTR_SAFE_SET_NODATA(pTLH  , x, y);

			CCT_Growing_Season	Model(m_Model);	// copy model setup

			if( Model.Set_Monthly(CCT_Water_Balance::MONTHLY_T   , x, y, pT   )
			&&  Model.Set_Monthly(CCT_Water_Balance::MONTHLY_Tmin, x, y, pTmin)
			&&  Model.Set_Monthly(CCT_Water_Balance::MONTHLY_Tmax, x, y, pTmax)
			&&  Model.Set_Monthly(CCT_Water_Balance::MONTHLY_P   , x, y, pP   ) )
			{
				double	Lat	= pLat && !pLat->is_NoData(x, y) ? pLat->asDouble(x, y) : Lat_Def;
				double	SWC	= pSWC && !pSWC->is_NoData(x, y) ? pSWC->asDouble(x, y) : SWC_Def;

				Model.Calculate(SWC, Lat);

				pLGS->Set_Value(x, y, Model.Get_LGS());

				if( Model.Get_LGS() > 0 )
				{
					pSMT->Set_Value(x, y, Model.Get_SMT());
				}

				if( pFirst && Model.Get_GDay_First() >= 0 )
				{
					pFirst->Set_Value(x, y, 1 + Model.Get_GDay_First());
				}

				if( pLast  && Model.Get_GDay_Last () >= 0 )
				{
					pLast ->Set_Value(x, y, 1 + Model.Get_GDay_Last());
				}

				if( pTLH )
				{
					double	Height;

					if( Model.Calculate(SWC, Lat, Height, maxDiff) )
					{
						pTLH->Set_Value(x, y, Height);
					}
					else
					{
						pTLH->Set_NoData(x, y);
					}
				}
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
CWater_Balance::CWater_Balance(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Soil Water Balance"));

	Set_Author		("O.Conrad, L.Landschreiber (c) 2016");

	Set_Description	(_TW(
		"This tool calculates the water balance for the selected position on a daily basis. "
		"Needed input is monthly data of mean, minimum, and maximum temperature as well as precipitation."
	));

	Add_Reference("Paulsen, J. / Körner, C.", "2014",
		"A climate-based model to predict potential treeline position around the globe",
		"Alpine Botany, 124:1, 1–12. doi:10.1007/s00035-014-0124-0.",
		SG_T("http://link.springer.com/article/10.1007%2Fs00035-014-0124-0"), _TL("online")
	);

	//-----------------------------------------------------
	Parameters.Add_Grid_List("", "T"   , _TL("Mean Temperature"   ), _TL(""), PARAMETER_INPUT);
	Parameters.Add_Grid_List("", "TMIN", _TL("Minimum Temperature"), _TL(""), PARAMETER_INPUT);
	Parameters.Add_Grid_List("", "TMAX", _TL("Maximum Temperature"), _TL(""), PARAMETER_INPUT);
	Parameters.Add_Grid_List("", "P"   , _TL("Precipitation"      ), _TL(""), PARAMETER_INPUT);

	//-----------------------------------------------------
	Parameters.Add_Grid_or_Const("",
		"SWC"			, _TL("Soil Water Capacity of Profile"),
		_TL("Total soil water capacity (mm H2O)."),
		220.0, 0.0, true
	);

	Parameters.Add_Double("SWC",
		"SWC_SURFACE"	, _TL("Top Soil Water Capacity"),
		_TL(""),
		30.0, 0.0, true
	);

	Parameters.Add_Double("SWC",
		"SW1_RESIST"	, _TL("Transpiration Resistance"),
		_TL(""),
		0.5, 0.01, true
	);

	Parameters.Add_Double("",
		"LAT_DEF"		, _TL("Default Latitude"),
		_TL(""),
		0, -90, true, 90, true
	);

	//-----------------------------------------------------
	Parameters.Add_Grids("", "SNOW" , _TL("Snow Depth"              ), _TL(""), PARAMETER_OUTPUT_OPTIONAL);
	Parameters.Add_Grids("", "ETP"  , _TL("Evapotranspiration"      ), _TL(""), PARAMETER_OUTPUT_OPTIONAL);
	Parameters.Add_Grids("", "SW_0" , _TL("Soil Water (Upper Layer)"), _TL(""), PARAMETER_OUTPUT_OPTIONAL);
	Parameters.Add_Grids("", "SW_1" , _TL("Soil Water (Lower Layer)"), _TL(""), PARAMETER_OUTPUT_OPTIONAL);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CWater_Balance::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWater_Balance::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Parameter_Grid_List	*pT   	= Parameters("T"   )->asGridList();
	CSG_Parameter_Grid_List	*pTmin	= Parameters("TMIN")->asGridList();
	CSG_Parameter_Grid_List	*pTmax	= Parameters("TMAX")->asGridList();
	CSG_Parameter_Grid_List	*pP   	= Parameters("P"   )->asGridList();

	if( pT   ->Get_Grid_Count() != 12
	||  pTmin->Get_Grid_Count() != 12
	||  pTmax->Get_Grid_Count() != 12
	||  pP   ->Get_Grid_Count() != 12 )
	{
		SG_UI_Msg_Add_Error(_TL("there has to be one input grid for each month"));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Grids	*pSnow	= Parameters("SNOW")->asGrids();
	CSG_Grids	*pETP	= Parameters("ETP" )->asGrids();
	CSG_Grids	*pSW_0	= Parameters("SW_0")->asGrids();
	CSG_Grids	*pSW_1	= Parameters("SW_1")->asGrids();

	if( !pSnow && !pETP && !pSW_0 && !pSW_1 )
	{
		SG_UI_Msg_Add_Error(_TL("no output has been specified"));

		return( false );
	}

	#define CREATE_DAILY_GRIDS(pGrids, Name)	if( pGrids ) { if( !pGrids->Create(Get_System(), 365) )\
		{	SG_UI_Msg_Add_Error(_TL("failed to create grid collection")); return( false ); } else\
		{	pGrids->Set_Name(Name); }\
	};

	CREATE_DAILY_GRIDS(pSnow, _TL("Snow Depth"              ));
	CREATE_DAILY_GRIDS(pETP , _TL("Evapotranspiration"      ));
	CREATE_DAILY_GRIDS(pSW_0, _TL("Soil Water (Upper Layer)"));
	CREATE_DAILY_GRIDS(pSW_1, _TL("Soil Water (Lower Layer)"));

	//-----------------------------------------------------
	double	Lat_Def	= Parameters("LAT_DEF")->asDouble();

	CSG_Grid Lat; CSG_Grid *pLat = SG_Grid_Get_Geographic_Coordinates(pT->Get_Grid(0), NULL, &Lat) ? &Lat : NULL;

	//-----------------------------------------------------
	double		SWC_Def	= Parameters("SWC")->asDouble();
	CSG_Grid	*pSWC	= Parameters("SWC")->asGrid();

	m_Model.Get_Soil().Set_Capacity     (0, Parameters("SWC_SURFACE")->asDouble());
	m_Model.Get_Soil().Set_ET_Resistance(1, Parameters("SW1_RESIST" )->asDouble());

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
#ifndef _DEBUG
		#pragma omp parallel for
#endif
		for(int x=0; x<Get_NX(); x++)
		{
			CCT_Water_Balance	Model(m_Model);	// copy model setup

			if( Model.Set_Monthly(CCT_Water_Balance::MONTHLY_T   , x, y, pT   )
			&&  Model.Set_Monthly(CCT_Water_Balance::MONTHLY_Tmin, x, y, pTmin)
			&&  Model.Set_Monthly(CCT_Water_Balance::MONTHLY_Tmax, x, y, pTmax)
			&&  Model.Set_Monthly(CCT_Water_Balance::MONTHLY_P   , x, y, pP   ) )
			{
				double	Lat	= pLat && !pLat->is_NoData(x, y) ? pLat->asDouble(x, y) : Lat_Def;
				double	SWC	= pSWC && !pSWC->is_NoData(x, y) ? pSWC->asDouble(x, y) : SWC_Def;

				Model.Calculate(SWC, Lat);

				for(int iDay=0; iDay<365; iDay++)
				{
					SG_GRIDS_PTR_SAFE_SET_VALUE(pSnow, x, y, iDay, Model.Get_Snow (iDay));
					SG_GRIDS_PTR_SAFE_SET_VALUE(pETP , x, y, iDay, Model.Get_ETpot(iDay));
					SG_GRIDS_PTR_SAFE_SET_VALUE(pSW_0, x, y, iDay, Model.Get_SW_0 (iDay));
					SG_GRIDS_PTR_SAFE_SET_VALUE(pSW_1, x, y, iDay, Model.Get_SW_1 (iDay));
				}
			}
			else
			{
				for(int iDay=0; iDay<365; iDay++)
				{
					SG_GRIDS_PTR_SAFE_SET_NODATA(pSnow, x, y, iDay);
					SG_GRIDS_PTR_SAFE_SET_NODATA(pETP , x, y, iDay);
					SG_GRIDS_PTR_SAFE_SET_NODATA(pSW_0, x, y, iDay);
					SG_GRIDS_PTR_SAFE_SET_NODATA(pSW_1, x, y, iDay);
				}
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
CWater_Balance_Interactive::CWater_Balance_Interactive(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Soil Water Balance"));

	Set_Author		("O.Conrad, L.Landschreiber (c) 2018");

	Set_Description	(_TW(
		"This tool calculates the water balance for the selected position on a daily basis. "
		"Needed input is monthly data of mean, minimum, and maximum temperature as well as precipitation."
	));

	Add_Reference("Paulsen, J. / Körner, C.", "2014",
		"A climate-based model to predict potential treeline position around the globe",
		"Alpine Botany, 124:1, 1–12. doi:10.1007/s00035-014-0124-0.",
		SG_T("http://link.springer.com/article/10.1007%2Fs00035-014-0124-0"), _TL("online")
	);

	//-----------------------------------------------------
	Parameters.Add_Grid_List("", "T"   , _TL("Mean Temperature"   ), _TL(""), PARAMETER_INPUT);
	Parameters.Add_Grid_List("", "TMIN", _TL("Minimum Temperature"), _TL(""), PARAMETER_INPUT);
	Parameters.Add_Grid_List("", "TMAX", _TL("Maximum Temperature"), _TL(""), PARAMETER_INPUT);
	Parameters.Add_Grid_List("", "P"   , _TL("Precipitation"      ), _TL(""), PARAMETER_INPUT);

	//-----------------------------------------------------
	Parameters.Add_Grid_or_Const("",
		"SWC"			, _TL("Soil Water Capacity of Profile"),
		_TL("Total soil water capacity (mm H2O)."),
		220.0, 0.0, true
	);

	Parameters.Add_Double("SWC",
		"SWC_SURFACE"	, _TL("Top Soil Water Capacity"),
		_TL(""),
		30.0, 0.0, true
	);

	Parameters.Add_Double("SWC",
		"SW1_RESIST"	, _TL("Transpiration Resistance"),
		_TL(""),
		0.5, 0.01, true
	);

	Parameters.Add_Double("",
		"LAT_DEF"		, _TL("Default Latitude"),
		_TL(""),
		50, -90, true, 90, true
	);

	//-----------------------------------------------------
	Parameters.Add_Table("",
		"SUMMARY"	, _TL("Summary"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Table("",
		"DAILY"		, _TL("Daily"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	//-----------------------------------------------------
	Set_Drag_Mode(TOOL_INTERACTIVE_DRAG_NONE);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CWater_Balance_Interactive::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	return( CSG_Tool_Grid_Interactive::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWater_Balance_Interactive::On_Execute(void)
{
	//-----------------------------------------------------
	m_pT   	= Parameters("T"   )->asGridList();
	m_pTmin	= Parameters("TMIN")->asGridList();
	m_pTmax	= Parameters("TMAX")->asGridList();
	m_pP   	= Parameters("P"   )->asGridList();

	if( m_pT   ->Get_Grid_Count() != 12
	||  m_pTmin->Get_Grid_Count() != 12
	||  m_pTmax->Get_Grid_Count() != 12
	||  m_pP   ->Get_Grid_Count() != 12 )
	{
		SG_UI_Msg_Add_Error(_TL("there has to be one input grid for each month"));

		return( false );
	}

	//-----------------------------------------------------
	m_Lat_Def	= Parameters("LAT_DEF")->asDouble();
	m_pLat		= SG_Grid_Get_Geographic_Coordinates(m_pT->Get_Grid(0), NULL, &m_Lat) ? &m_Lat : NULL;

	//-----------------------------------------------------
	m_SWC_Def	= Parameters("SWC")->asDouble();
	m_pSWC		= Parameters("SWC")->asGrid();

	m_Model.Get_Soil().Set_Capacity     (0, Parameters("SWC_SURFACE")->asDouble());
	m_Model.Get_Soil().Set_ET_Resistance(1, Parameters("SW1_RESIST" )->asDouble());

	//-----------------------------------------------------
	m_pSummary	= Parameters("SUMMARY")->asTable();
	m_pSummary->Destroy();
	m_pSummary->Set_Name("%s [%s]", _TL("Tree Growth"), _TL("Summary"));
	m_pSummary->Add_Field("NAME" , SG_DATATYPE_String);
	m_pSummary->Add_Field("VALUE", SG_DATATYPE_Double);
	m_pSummary->Add_Record()->Set_Value(0, _TL("X"));
	m_pSummary->Add_Record()->Set_Value(0, _TL("Y"));
	m_pSummary->Add_Record()->Set_Value(0, _TL("Latitude"));
	m_pSummary->Add_Record()->Set_Value(0, _TL("Length of Growing Season"));
	m_pSummary->Add_Record()->Set_Value(0, _TL("Mean Temperature"));
	m_pSummary->Add_Record()->Set_Value(0, _TL("Tree Line Height"));

	//-----------------------------------------------------
	m_pDaily	= Parameters("DAILY")->asTable();
	m_pDaily->Destroy();
	m_pDaily->Set_Name("%s [%s]", _TL("Tree Line"), _TL("Climate"));
	m_pDaily->Add_Field("T"   , SG_DATATYPE_Double);
	m_pDaily->Add_Field("P"   , SG_DATATYPE_Double);
	m_pDaily->Add_Field("SNOW", SG_DATATYPE_Double);
	m_pDaily->Add_Field("ETP" , SG_DATATYPE_Double);
	m_pDaily->Add_Field("SW_0", SG_DATATYPE_Double);
	m_pDaily->Add_Field("SW_1", SG_DATATYPE_Double);
	m_pDaily->Set_Record_Count(365);

	//-----------------------------------------------------
	return( true );
}

//---------------------------------------------------------
bool CWater_Balance_Interactive::On_Execute_Finish(void)
{
	m_Lat.Destroy();

	return( true );
}

//---------------------------------------------------------
bool CWater_Balance_Interactive::On_Execute_Position(CSG_Point ptWorld, TSG_Tool_Interactive_Mode Mode)
{
	if( Mode != TOOL_INTERACTIVE_LDOWN && Mode != TOOL_INTERACTIVE_MOVE_LDOWN )
	{
		return( false );
	}

	//-----------------------------------------------------
	int	x, y;

	if( !Get_System().Get_World_to_Grid(x, y, ptWorld) || !Get_System().is_InGrid(x, y) )
	{
		return( false );
	}

	//-----------------------------------------------------
	CCT_Water_Balance	Model(m_Model);	// copy model setup

	if( Model.Set_Monthly(CCT_Water_Balance::MONTHLY_T   , x, y, m_pT   )
	&&  Model.Set_Monthly(CCT_Water_Balance::MONTHLY_Tmin, x, y, m_pTmin)
	&&  Model.Set_Monthly(CCT_Water_Balance::MONTHLY_Tmax, x, y, m_pTmax)
	&&  Model.Set_Monthly(CCT_Water_Balance::MONTHLY_P   , x, y, m_pP   ) )
	{
		double	Latitude	= m_pLat && !m_pLat->is_NoData(x, y) ? m_pLat->asDouble(x, y) : m_Lat_Def;
		double	SWC			= m_pSWC && !m_pSWC->is_NoData(x, y) ? m_pSWC->asDouble(x, y) : m_SWC_Def;

		Model.Calculate(SWC, Latitude);

		for(int iDay=0; iDay<365; iDay++)
		{
			CSG_Table_Record	*pRecord	= m_pDaily->Get_Record(iDay);

			pRecord->Set_Value(0, Model.Get_T    (iDay));
			pRecord->Set_Value(1, Model.Get_P    (iDay));
			pRecord->Set_Value(2, Model.Get_Snow (iDay));
			pRecord->Set_Value(3, Model.Get_ETpot(iDay));
			pRecord->Set_Value(4, Model.Get_SW_0 (iDay));
			pRecord->Set_Value(5, Model.Get_SW_1 (iDay));
		}

		//-------------------------------------------------
		m_pSummary->Get_Record(0)->Set_Value(1, ptWorld.Get_X());
		m_pSummary->Get_Record(1)->Set_Value(1, ptWorld.Get_Y());
		m_pSummary->Get_Record(2)->Set_Value(1, Latitude);

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
