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
#include "treeline.h"

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool	SG_Grid_Get_Geographic_Coordinates	(CSG_Grid *pGrid, CSG_Grid *pLon, CSG_Grid *pLat)
{
	bool	bResult	= false;

	if( pGrid && pGrid->is_Valid() && pGrid->Get_Projection().is_Okay() && (pLon || pLat) )
	{
		CSG_Grid Lon; if( !pLon ) { pLon = &Lon; } pLon->Create(pGrid->Get_System());
		CSG_Grid Lat; if( !pLat ) { pLat = &Lat; } pLat->Create(pGrid->Get_System());

		SG_RUN_TOOL(bResult, "pj_proj4", 17,	// geographic coordinate grids
				SG_TOOL_PARAMETER_SET("GRID", pGrid)
			&&	SG_TOOL_PARAMETER_SET("LON" , pLon )
			&&	SG_TOOL_PARAMETER_SET("LAT" , pLat )
		)
	}

	return( bResult );
}


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

	double	SWC	= S0 && S1 ? m_Soil.Get_Capacity(0) + m_Soil.Get_Capacity(1) : 0.0;

	for(int iDay=0; iDay<365; iDay++)
	{
		if( T[iDay] >= m_DT_min
		&&  (!Snow || Snow[iDay] <= 0.0)
		&&  (!(S0 && S1) || (S0[iDay] > 0.0 || S1[iDay] >= m_SW_min * m_Soil.Get_Capacity(1))) )
		{
			m_T_Season	+= T[iDay];
		}
	}

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
	Parameters.Add_Grid_List(NULL, "T"   , _TL("Mean Temperature"   ), _TL(""), PARAMETER_INPUT);
	Parameters.Add_Grid_List(NULL, "TMIN", _TL("Minimum Temperature"), _TL(""), PARAMETER_INPUT);
	Parameters.Add_Grid_List(NULL, "TMAX", _TL("Maximum Temperature"), _TL(""), PARAMETER_INPUT);
	Parameters.Add_Grid_List(NULL, "P"   , _TL("Precipitation"      ), _TL(""), PARAMETER_INPUT);

	//-----------------------------------------------------
	Parameters.Add_Grid_or_Const(NULL,
		"SWC"			, _TL("Soil Water Capacity of Profile"),
		_TL("Total soil water capacity (mm H2O)."),
		220.0, 0.0, true
	);

	Parameters.Add_Double(Parameters("SWC"),
		"SWC_SURFACE"	, _TL("Top Soil Water Capacity"),
		_TL(""),
		10.0, 0.0, true
	);

	Parameters.Add_Double(Parameters("SWC"),
		"SW1_RESIST"	, _TL("Transpiration Resistance"),
		_TL(""),
		1.0, 0.1, true
	);

	Parameters.Add_Double(NULL,
		"LAT_DEF"		, _TL("Default Latitude"),
		_TL(""),
		50.0, -90.0, true, 90.0, true
	);

	//-----------------------------------------------------
	Parameters.Add_Grid(NULL,
		"SMT"		, _TL("Mean Temperature"),
		_TL("Mean temperature of the growing season."),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(NULL,
		"LGS"		, _TL("Length"),
		_TL("Number of days of the growing season."),
		PARAMETER_OUTPUT, true, SG_DATATYPE_Short
	);

	Parameters.Add_Grid(NULL,
		"TLH"		, _TL("Tree Line Height"),
		_TL("Estimated relative tree line height."),
		PARAMETER_OUTPUT_OPTIONAL
	);

	//-----------------------------------------------------
	Parameters.Add_Double(NULL,
		"DT_MIN"		, _TL("Threshold Temperature"),
		_TL("Threshold temperature (C) that constrains the growing season."),
		0.9
	);

	Parameters.Add_Double(NULL,
		"SW_MIN"		, _TL("Minimum Soil Water Content (Percent)"),
		_TL(""),
		2.0, 0.0, true
	);

	Parameters.Add_Int(NULL,
		"LGS_MIN"		, _TL("Minimum Length"),
		_TL("Minimum length (days) of the growing season."),
		94, 1, true
	);

	Parameters.Add_Double(NULL,
		"SMT_MIN"		, _TL("Minimum Mean Temperature"),
		_TL("Minimum mean temperature (C) for all days of the growing season."),
		6.4
	);

	Parameters.Add_Double(NULL,
		"TLH_MAX_DIFF"	, _TL("Maximum Tree Line Height Difference"),
		_TL(""),
		1000.0, 0.0, true
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

	if( pT   ->Get_Count() != 12
	||  pTmin->Get_Count() != 12
	||  pTmax->Get_Count() != 12
	||  pP   ->Get_Count() != 12 )
	{
		SG_UI_Msg_Add_Error(_TL("there has to be one input grid for each month"));

		return( false );
	}

	//-----------------------------------------------------
	double	Lat_Def	= Parameters("LAT_DEF")->asDouble();

	CSG_Grid Lat; CSG_Grid *pLat = SG_Grid_Get_Geographic_Coordinates(pT->asGrid(0), NULL, &Lat) ? &Lat : NULL;

	//-----------------------------------------------------
	double		SWC_Def	= Parameters("SWC")->asDouble();
	CSG_Grid	*pSWC	= Parameters("SWC")->asGrid();

	m_Model.Get_Soil().Set_Capacity     (0, Parameters("SWC_SURFACE")->asDouble());
	m_Model.Get_Soil().Set_ET_Resistance(1, Parameters("SW1_RESIST" )->asDouble());

	//-----------------------------------------------------
	CSG_Grid	*pSMT	= Parameters("SMT")->asGrid();
	CSG_Grid	*pLGS	= Parameters("LGS")->asGrid();
	CSG_Grid	*pTLH	= Parameters("TLH")->asGrid();

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
				else
				{
					pSMT->Set_NoData(x, y);
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
			else
			{
				pLGS->Set_NoData(x, y);
				pSMT->Set_NoData(x, y);

				if( pTLH )
				{
					pTLH->Set_NoData(x, y);
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
	Parameters.Add_Grid_List(NULL, "T"   , _TL("Mean Temperature"   ), _TL(""), PARAMETER_INPUT);
	Parameters.Add_Grid_List(NULL, "TMIN", _TL("Minimum Temperature"), _TL(""), PARAMETER_INPUT);
	Parameters.Add_Grid_List(NULL, "TMAX", _TL("Maximum Temperature"), _TL(""), PARAMETER_INPUT);
	Parameters.Add_Grid_List(NULL, "P"   , _TL("Precipitation"      ), _TL(""), PARAMETER_INPUT);

	//-----------------------------------------------------
	Parameters.Add_Grid_or_Const(NULL,
		"SWC"			, _TL("Soil Water Capacity of Profile"),
		_TL("Total soil water capacity (mm H2O)."),
		220.0, 0.0, true
	);

	Parameters.Add_Double(Parameters("SWC"),
		"SWC_SURFACE"	, _TL("Top Soil Water Capacity"),
		_TL(""),
		10.0, 0.0, true
	);

	Parameters.Add_Double(Parameters("SWC"),
		"SW1_RESIST"	, _TL("Transpiration Resistance"),
		_TL(""),
		1.0, 0.1, true
	);

	Parameters.Add_Double(NULL,
		"LAT_DEF"		, _TL("Default Latitude"),
		_TL(""),
		50.0, -90.0, true, 90.0, true
	);

	//-----------------------------------------------------
	Parameters.Add_Table(NULL,
		"SUMMARY"	, _TL("Summary"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Table(NULL,
		"DAILY"		, _TL("Daily"),
		_TL(""),
		PARAMETER_OUTPUT
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CWater_Balance_Interactive::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
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

	if( m_pT   ->Get_Count() != 12
	||  m_pTmin->Get_Count() != 12
	||  m_pTmax->Get_Count() != 12
	||  m_pP   ->Get_Count() != 12 )
	{
		SG_UI_Msg_Add_Error(_TL("there has to be one input grid for each month"));

		return( false );
	}

	//-----------------------------------------------------
	m_Lat_Def	= Parameters("LAT_DEF")->asDouble();
	m_pLat		= SG_Grid_Get_Geographic_Coordinates(m_pT->asGrid(0), NULL, &m_Lat) ? &m_Lat : NULL;

	//-----------------------------------------------------
	m_SWC_Def	= Parameters("SWC")->asDouble();
	m_pSWC		= Parameters("SWC")->asGrid();

	m_Model.Get_Soil().Set_Capacity     (0, Parameters("SWC_SURFACE")->asDouble());
	m_Model.Get_Soil().Set_ET_Resistance(1, Parameters("SW1_RESIST" )->asDouble());

	//-----------------------------------------------------
	m_pSummary	= Parameters("SUMMARY")->asTable();
	m_pSummary->Destroy();
	m_pSummary->Set_Name(CSG_String::Format("%s [%s]", _TL("Tree Growth"), _TL("Summary")));
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
	m_pDaily->Set_Name(CSG_String::Format("%s [%s]", _TL("Tree Line"), _TL("Climate")));
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
	if( Mode != TOOL_INTERACTIVE_LDOWN )
	{
		return( false );
	}

	//-----------------------------------------------------
	int	x, y;

	if( !Get_System()->Get_World_to_Grid(x, y, ptWorld) || !Get_System()->is_InGrid(x, y) )
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
