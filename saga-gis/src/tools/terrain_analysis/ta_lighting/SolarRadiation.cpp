/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                      ta_lighting                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  SolarRadiation.cpp                   //
//                                                       //
//                 Copyright (C) 2003 by                 //
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
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
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
#include "SolarRadiation.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSolarRadiation::CSolarRadiation(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name		(_TL("Potential Incoming Solar Radiation"));

	Set_Author		("O.Conrad (c) 2010");

	Set_Description	(_TW(
		"Calculation of potential incoming solar radiation (insolation).\n"
		"Times of sunrise/sunset will only be calculated if time span is set to single day.\n"
		"\n"
		"References:\n<ul>"
		"<li>Boehner, J., Antonic, O. (2009): Land Surface Parameters Specific to Topo-Climatology. in Hengl, T. & Reuter, H.I. [Eds.]: Geomorphometry - Concepts, Software, Applications.</li>"
		"<li>Oke, T.R. (1988): Boundary Layer Climates. London, Taylor & Francis.</li>"
		"<li>Wilson, J.P., Gallant, J.C. [Eds.] (2000): Terrain Analysis - Principles and Applications. New York, John Wiley & Sons, Inc.</li>"
		"<li>Joint Research Centre: <a target=\"_blank\" href=\"http://re.jrc.ec.europa.eu/pvgis/\">GIS solar radiation database for Europe</a> and  <a target=\"_blank\" href=\"http://re.jrc.ec.europa.eu/pvgis/solres/solmod3.htm\">Solar radiation and GIS</a>.</li>"
		"<li>Hofierka, J., Suri, M. (2002): The solar radiation model for Open source GIS: implementation and applications. International GRASS users conference in Trento, Italy, September 2002. <a target=\"_blank\" href=\"http://skagit.meas.ncsu.edu/~jaroslav/trento/Hofierka_Jaroslav.pdf\">pdf</a>.</li>"
		"</ul>\n"

		"\n*) Most options should do well, but TAPES-G based diffuse irradiance calculation ('Atmospheric Effects' methods 2 and 3) needs further revision!"
	));

	//-----------------------------------------------------
	Parameters.Add_Grid         (NULL, "GRD_DEM"     , _TL("Elevation"                   ), _TL(""), PARAMETER_INPUT);
	Parameters.Add_Grid         (NULL, "GRD_SVF"     , _TL("Sky View Factor"             ), _TL(""), PARAMETER_INPUT_OPTIONAL);
	Parameters.Add_Grid_or_Const(NULL, "GRD_VAPOUR"  , _TL("Water Vapour Pressure [mbar]"), _TL(""), 10.0, 0.0, true);
	Parameters.Add_Grid_or_Const(NULL, "GRD_LINKE"   , _TL("Linke Turbidity Coefficient" ), _TL(""),  3.0, 0.0, true);
	Parameters.Add_Grid         (NULL, "GRD_DIRECT"  , _TL("Direct Insolation"           ), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid         (NULL, "GRD_DIFFUS"  , _TL("Diffuse Insolation"          ), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid         (NULL, "GRD_TOTAL"   , _TL("Total Insolation"            ), _TL(""), PARAMETER_OUTPUT_OPTIONAL);
	Parameters.Add_Grid         (NULL, "GRD_RATIO"   , _TL("Direct to Diffuse Ratio"     ), _TL(""), PARAMETER_OUTPUT_OPTIONAL);
	Parameters.Add_Grid         (NULL, "GRD_DURATION", _TL("Duration of Insolation"      ), _TL(""), PARAMETER_OUTPUT_OPTIONAL);
	Parameters.Add_Grid         (NULL, "GRD_SUNRISE" , _TL("Sunrise"                     ), _TL(""), PARAMETER_OUTPUT_OPTIONAL);
	Parameters.Add_Grid         (NULL, "GRD_SUNSET"  , _TL("Sunset"                      ), _TL(""), PARAMETER_OUTPUT_OPTIONAL);

	//-----------------------------------------------------
	Parameters.Add_Value(
		NULL	, "SOLARCONST"		, _TL("Solar Constant [W / m\xb2]"),
		_TL(""),
		PARAMETER_TYPE_Double		, 1367.0, 0.0, true
	);

	Parameters.Add_Value(
		NULL	, "LOCALSVF"		, _TL("Local Sky View Factor"),
		_TL("Use sky view factor based on local slope (after Oke 1988), if no sky viev factor grid is given."),
		PARAMETER_TYPE_Bool			, true
	);

	Parameters.Add_Choice(
		NULL	, "UNITS"			, _TL("Units"),
		_TL("Units for output radiation values."),
		CSG_String::Format(SG_T("%s|%s|%s|"),
			SG_T("kWh / m2"),
			SG_T("kJ / m2"),
			SG_T("J / cm2")
		), 0
	);

	Parameters.Add_Choice(
		NULL	, "SHADOW"			, _TL("Shadow"),
		_TL("Choose 'slim' to trace grid node's shadow, 'fat' to trace the whole cell's shadow, or ignore shadowing effects. The first is slightly faster but might show some artifacts."),
		CSG_String::Format("%s|%s|%s|",
			_TL("slim"),
			_TL("fat"),
			_TL("none")
		), 1
	);

	//-----------------------------------------------------
	pNode	= Parameters.Add_Choice(
		NULL	, "LOCATION"		, _TL("Location"),
		_TL(""),
		CSG_String::Format("%s|%s|",
			_TL("constant latitude"),
			_TL("calculate from grid system")
		), 0
	);

	Parameters.Add_Value(
		pNode	, "LATITUDE"		, _TL("Latitude"),
		_TL(""),
		PARAMETER_TYPE_Degree		, 53.0, -90.0, true, 90.0, true
	);

	//-----------------------------------------------------
	pNode	= Parameters.Add_Choice(
		NULL	, "PERIOD"			, _TL("Time Period"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|",
			_TL("moment"),
			_TL("day"),
			_TL("range of days")
		), 1
	);

	pNode	= Parameters.Add_Date(
		pNode	, "DAY"				, _TL("Day"),
		_TL(""),
		CSG_DateTime::Now().Get_JDN()
	);

	Parameters.Add_Date(
		pNode	, "DAY_STOP"		, _TL("Last Day"),
		_TL(""),
		CSG_DateTime::Now().Get_JDN()
	);

	Parameters.Add_Value(
		pNode	, "DAYS_STEP"		, _TL("Resolution [d]"),
		_TL("Time step size for a range of days calculation given in days."),
		PARAMETER_TYPE_Int			, 5, 1, true
	);

	pNode	= Parameters("PERIOD");

	Parameters.Add_Value(
		pNode	, "MOMENT"			, _TL("Moment [h]"),
		_TL(""),
		PARAMETER_TYPE_Double		, 12.0, 0.0, true, 24.0, true
	);

	Parameters.Add_Range(
		pNode	, "HOUR_RANGE"		, _TL("Time Span [h]"),
		_TL("Time span used for the calculation of daily radiation sums."),
		 0.0, 24.0, 0.0	, true, 24.0, true
	);

	Parameters.Add_Value(
		pNode	, "HOUR_STEP"		, _TL("Resolution [h]"),
		_TL("Time step size for a day's calculation given in hours."),
		PARAMETER_TYPE_Double		, 0.5, 0.0, true, 24.0, true
	);

	//-----------------------------------------------------
	pNode	= Parameters.Add_Choice(
		NULL	, "METHOD"			, _TL("Atmospheric Effects"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s|",
			_TL("Height of Atmosphere and Vapour Pressure"),
			_TL("Air Pressure, Water and Dust Content"),
			_TL("Lumped Atmospheric Transmittance"),
			_TL("Hofierka and Suri")
		), 2
	);

	Parameters.Add_Value(
		pNode	, "ATMOSPHERE"		, _TL("Height of Atmosphere [m]"),
		_TL(""),
		PARAMETER_TYPE_Double		, 12000.0, 0.0, true
	);

	Parameters.Add_Value(
		pNode	, "PRESSURE"		, _TL("Barometric Pressure [mbar]"),
		_TL(""),
		PARAMETER_TYPE_Double, 1013, 0.0, true
	);

	Parameters.Add_Value(
		pNode	, "WATER"			, _TL("Water Content [cm]"),
		_TL("Water content of a vertical slice of atmosphere in cm: 1.5 to 1.7, average=1.68"),
		PARAMETER_TYPE_Double, 1.68, 0.0, true
	);

	Parameters.Add_Value(
		pNode	, "DUST"			, _TL("Dust [ppm]"),
		_TL("Dust factor: 100 ppm (standard)"),
		PARAMETER_TYPE_Double, 100, 0.0, true
	);

	Parameters.Add_Value(
		pNode	, "LUMPED"			, _TL("Lumped Atmospheric Transmittance [Percent]"),
		_TL("The transmittance of the atmosphere, usually between 60 and 80 percent."),
		PARAMETER_TYPE_Double, 70, 0.0, true, 100.0, true
	);

	//-----------------------------------------------------
	pNode	= Parameters.Add_Choice(
		NULL	, "UPDATE"			, _TL("Update"),
		_TL("show direct insolation for each time step."),
		CSG_String::Format("%s|%s|%s|",
			_TL("do not update"),
			_TL("fit histogram stretch for each time step"),
			_TL("constant histogram stretch for all time steps")
		), 0
	);	pNode->Set_UseInCMD(false);

	Parameters.Add_Value(
		pNode	, "UPDATE_STRETCH"	, _TL("Constant Histogram Stretch"),
		_TL(""),
		PARAMETER_TYPE_Double		, 1.0, 0.0, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CSolarRadiation::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	!SG_STR_CMP(pParameter->Get_Identifier(), "GRD_DEM") && pParameter->asGrid() && pParameter->asGrid()->Get_Projection().is_Okay() )
	{
		CSG_Shapes	srcCenter(SHAPE_TYPE_Point), dstCenter(SHAPE_TYPE_Point);

		srcCenter.Get_Projection()	= pParameter->asGrid()->Get_Projection();
		srcCenter.Add_Shape()->Add_Point(pParameter->asGrid()->Get_System().Get_Extent().Get_Center());

		bool	bResult;

		SG_RUN_TOOL(bResult, "pj_proj4", 2,	// Coordinate Transformation (Shapes)
				SG_TOOL_PARAMETER_SET("SOURCE"   , &srcCenter)
			&&	SG_TOOL_PARAMETER_SET("TARGET"   , &dstCenter)
			&&	SG_TOOL_PARAMETER_SET("CRS_PROJ4", SG_T("+proj=longlat +ellps=WGS84 +datum=WGS84"))
		)

		if( bResult )
		{
			pParameters->Get_Parameter("LATITUDE")->Set_Value(dstCenter.Get_Shape(0)->Get_Point(0).y);
		}
	}

	return( CSG_Tool_Grid::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
int CSolarRadiation::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	!SG_STR_CMP(pParameter->Get_Identifier(), "UPDATE") )
	{
		pParameters->Set_Enabled("UPDATE_STRETCH", pParameter->asInt() == 2);
	}

	if(	!SG_STR_CMP(pParameter->Get_Identifier(), "LOCATION") )
	{
		pParameters->Set_Enabled("LATITUDE"      , pParameter->asInt() == 0);
	}

	if(	!SG_STR_CMP(pParameter->Get_Identifier(), "PERIOD") )
	{
		pParameters->Set_Enabled("MOMENT"        , pParameter->asInt() == 0);
		pParameters->Set_Enabled("GRD_DURATION"  , pParameter->asInt() == 1);
		pParameters->Set_Enabled("GRD_SUNRISE"   , pParameter->asInt() == 1);
		pParameters->Set_Enabled("GRD_SUNSET"    , pParameter->asInt() == 1);
		pParameters->Set_Enabled("UPDATE"        , pParameter->asInt() >= 1);
		pParameters->Set_Enabled("HOUR_RANGE"    , pParameter->asInt() >= 1);
		pParameters->Set_Enabled("HOUR_STEP"     , pParameter->asInt() >= 1);
		pParameters->Set_Enabled("DAY_STOP"      , pParameter->asInt() == 2);
		pParameters->Set_Enabled("DAYS_STEP"     , pParameter->asInt() == 2);
	}

	if(	!SG_STR_CMP(pParameter->Get_Identifier(), "METHOD") )
	{
		pParameters->Set_Enabled("GRD_VAPOUR"    , pParameter->asInt() == 0);
		pParameters->Set_Enabled("ATMOSPHERE"    , pParameter->asInt() == 0);
		pParameters->Set_Enabled("PRESSURE"      , pParameter->asInt() == 1);
		pParameters->Set_Enabled("WATER"         , pParameter->asInt() == 1);
		pParameters->Set_Enabled("DUST"          , pParameter->asInt() == 1);
		pParameters->Set_Enabled("LUMPED"        , pParameter->asInt() == 2);
		pParameters->Set_Enabled("GRD_LINKE"     , pParameter->asInt() == 3);
		pParameters->Set_Enabled("LOCALSVF"      , pParameter->asInt() != 3);
	}

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSolarRadiation::On_Execute(void)
{
	//-----------------------------------------------------
	m_pDEM			= Parameters("GRD_DEM"   )->asGrid();
	m_pSVF			= Parameters("GRD_SVF"   )->asGrid();
	m_pVapour		= Parameters("GRD_VAPOUR")->asGrid();
	m_Vapour		= Parameters("GRD_VAPOUR")->asDouble();
	m_pLinke		= Parameters("GRD_LINKE" )->asGrid();
	m_Linke			= Parameters("GRD_LINKE" )->asDouble();

	m_pDirect		= Parameters("GRD_DIRECT")->asGrid();
	m_pDiffus		= Parameters("GRD_DIFFUS")->asGrid();
	m_pTotal		= Parameters("GRD_TOTAL" )->asGrid();
	m_pRatio		= Parameters("GRD_RATIO" )->asGrid();

	m_pDuration		= NULL;
	m_pSunrise		= NULL;
	m_pSunset		= NULL;

	m_bLocalSVF		= Parameters("LOCALSVF"  )->asBool  ();
	m_Shadowing		= Parameters("SHADOW"    )->asInt   ();

	m_Method		= Parameters("METHOD"    )->asInt   ();
	m_Atmosphere	= Parameters("ATMOSPHERE")->asDouble();
	m_Transmittance	= Parameters("LUMPED"    )->asDouble() /  100.0;	// percent to ratio
	m_Pressure		= Parameters("PRESSURE"  )->asDouble();
	m_Water			= Parameters("WATER"     )->asDouble();
	m_Dust			= Parameters("DUST"      )->asDouble();

	//-----------------------------------------------------
	CSG_Colors	Colors(11, SG_COLORS_YELLOW_RED, true);

	Colors.Set_Ramp(SG_GET_RGB(  0,   0,  64), SG_GET_RGB(255, 159,   0), 0,  5);
	Colors.Set_Ramp(SG_GET_RGB(255, 159,   0), SG_GET_RGB(255, 255, 255), 5, 10);

	DataObject_Set_Colors(m_pDirect, Colors);
	DataObject_Set_Colors(m_pDiffus, Colors);
	DataObject_Set_Colors(m_pTotal , Colors);
	DataObject_Set_Colors(m_pRatio , 11, SG_COLORS_RED_GREY_BLUE, true);

	//-----------------------------------------------------
	m_Latitude	= Parameters("LATITUDE")->asDouble() * M_DEG_TO_RAD;

	if( (m_Location = Parameters("LOCATION")->asInt()) != 0 )
	{
		m_Sun_Height .Create(*Get_System());
		m_Sun_Azimuth.Create(*Get_System());

		m_Lat		 .Create(*Get_System());
		m_Lon		 .Create(*Get_System());

		SG_RUN_TOOL_ExitOnError("pj_proj4", 17,	// geographic coordinate grids
				SG_TOOL_PARAMETER_SET("GRID", m_pDEM)
			&&	SG_TOOL_PARAMETER_SET("LON" , &m_Lon)
			&&	SG_TOOL_PARAMETER_SET("LAT" , &m_Lat)
		)

		m_Lat.Set_Scaling(M_DEG_TO_RAD);
		m_Lon.Set_Scaling(M_DEG_TO_RAD, -M_DEG_TO_RAD * m_Lon.asDouble(Get_NX() / 2, Get_NY() / 2));

		Message_Add(CSG_String::Format("\n%s: %f <-> %f", _TL("Longitude"), M_RAD_TO_DEG * m_Lon.Get_Min(), M_RAD_TO_DEG * m_Lon.Get_Max()), false);
		Message_Add(CSG_String::Format("\n%s: %f <-> %f", _TL("Latitude" ), M_RAD_TO_DEG * m_Lat.Get_Min(), M_RAD_TO_DEG * m_Lat.Get_Max()), false);
	}

	//-----------------------------------------------------
	Process_Set_Text(_TL("Slopes"));

	m_Shade .Create(*Get_System());
	m_Slope .Create(*Get_System());
	m_Aspect.Create(*Get_System());

	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			double	s, a;

			if( m_pDEM->Get_Gradient(x, y, s, a) )
			{
				m_Slope .Set_Value(x, y, s);
				m_Aspect.Set_Value(x, y, a);
			}
			else
			{
				m_Slope .Set_NoData(x, y);
				m_Aspect.Set_NoData(x, y);
			}
		}
	}

	//-----------------------------------------------------
	if( Get_Insolation() )
	{
		Finalize();

		return( true );
	}

	Finalize();

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSolarRadiation::Finalize(void)
{
	double		dUnit;
	CSG_String	Unit;

	//-----------------------------------------------------
	if( Parameters("PERIOD")->asInt() == 0 )	// moment
	{
		Unit	= SG_T("W / m\xb2");
		dUnit	= 1000.0;
	}
	else switch( Parameters("UNITS")->asInt() )
	{
	case 0: default:	// [kWh / m2]
		Unit	= SG_T("kWh / m\xb2");
		dUnit	= 1.0;
		break;

	case 1:				// [kJ / m2]
		Unit	= SG_T("kJ / m\xb2");
		dUnit	= 3600.0;
		break;

	case 2:				// [Ws / cm2] = [J / cm2]
		Unit	= SG_T("J / cm\xb2");
		dUnit	= 360.0;
		break;
	}

	//-----------------------------------------------------
	m_pDirect->Set_Unit(Unit);
	m_pDirect->Multiply(dUnit);

	m_pDiffus->Set_Unit(Unit);
	m_pDiffus->Multiply(dUnit);

	if( m_pTotal )
	{
		m_pTotal->Assign(m_pDirect);
		m_pTotal->Add  (*m_pDiffus);

		m_pTotal->Set_Unit(Unit);
	}

	if( m_pRatio )
	{
		for(sLong i=0; i<Get_NCells(); i++)
		{
			if( m_pDEM->is_NoData(i) )
			{
				m_pRatio->Set_NoData(i);
			}
			else
			{
				if( m_pDiffus->asDouble(i) > 0.0 )
				{
					m_pRatio->Set_Value(i, m_pDirect->asDouble(i) / m_pDiffus->asDouble(i));
				}
			}
		}
	}

	//-----------------------------------------------------
	m_Shade      .Destroy();
	m_Slope      .Destroy();
	m_Aspect     .Destroy();
	m_Lat        .Destroy();
	m_Lon        .Destroy();
	m_Sun_Height .Destroy();
	m_Sun_Azimuth.Destroy();

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSolarRadiation::Get_Insolation(void)
{
	//-----------------------------------------------------
	switch( Parameters("PERIOD")->asInt() ? Parameters("UPDATE")->asInt() : 0 )
	{
	case 1:	DataObject_Update(m_pDirect                                               , SG_UI_DATAOBJECT_SHOW);	break;
	case 2:	DataObject_Update(m_pDirect, 0.0, Parameters("UPDATE_STRETCH")->asDouble(), SG_UI_DATAOBJECT_SHOW);	break;
	}

	//-----------------------------------------------------
	CSG_DateTime	Date(Parameters("DAY")->asDate()->Get_Date());

	switch( Parameters("PERIOD")->asInt() )
	{
	//-----------------------------------------------------
	case 0:	// Moment

		m_pDirect->Assign(0.0);
		m_pDiffus->Assign(0.0);

		return( Get_Insolation(Date, Parameters("MOMENT")->asDouble()) );

	//-----------------------------------------------------
	case 1:	// One Day

		if( !!(m_pDuration = Parameters("GRD_DURATION")->asGrid()) ) { DataObject_Set_Colors(m_pDuration, 11, SG_COLORS_YELLOW_RED,  true); m_pDuration->Assign_NoData(); m_pDuration->Set_Unit(_TL("h")); }
		if( !!(m_pSunrise  = Parameters("GRD_SUNRISE" )->asGrid()) ) { DataObject_Set_Colors(m_pSunrise , 11, SG_COLORS_YELLOW_RED, false); m_pSunrise ->Assign_NoData(); m_pSunrise ->Set_Unit(_TL("h")); }
		if( !!(m_pSunset   = Parameters("GRD_SUNSET"  )->asGrid()) ) { DataObject_Set_Colors(m_pSunset  , 11, SG_COLORS_YELLOW_RED,  true); m_pSunset  ->Assign_NoData(); m_pSunset  ->Set_Unit(_TL("h")); }

		return( Get_Insolation(Date) );

	//-----------------------------------------------------
	case 2:	// Range of Days
		{
			CSG_DateTime	Stop(Parameters("DAY_STOP")->asDate()->Get_Date());

			int	dDays	= Parameters("DAYS_STEP")->asInt();

			//---------------------------------------------
			// one representative calculation

			if( Stop.Get_JDN() - Date.Get_JDN() <= dDays )
			{
				Date.Set(Date.Get_JDN() + 0.5 * (dDays = (int)(Stop.Get_JDN() - Date.Get_JDN())));

				Get_Insolation(Date);

				m_pDirect->Multiply(1 + dDays);
				m_pDiffus->Multiply(1 + dDays);
			}

			//---------------------------------------------
			// more than one time step of days

			else
			{
				CSG_Grid	Direct, Diffus;

				Direct.Create(*Get_System(), SG_DATATYPE_Float);
				Diffus.Create(*Get_System(), SG_DATATYPE_Float);

				Direct.Assign(0.0);
				Diffus.Assign(0.0);

				CSG_TimeSpan	dStep(24.0 * dDays);

				for( ; Date<=Stop && Process_Get_Okay(false); Date+=dStep)
				{
					Get_Insolation(Date);

					SG_UI_Progress_Lock(true);

					Direct.Add(*m_pDirect);
					Diffus.Add(*m_pDiffus);

					SG_UI_Progress_Lock(false);
				}

				SG_UI_Progress_Lock(true);

				m_pDirect->Assign(&Direct);	m_pDirect->Multiply(dDays);
				m_pDiffus->Assign(&Diffus);	m_pDiffus->Multiply(dDays);

				SG_UI_Progress_Lock(false);
			}
		}

		return( true );
	}

	//-----------------------------------------------------
	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSolarRadiation::Get_Insolation(CSG_DateTime Date)
{
	SG_UI_Progress_Lock(true);

	//-----------------------------------------------------
	int		Update	= Parameters("UPDATE")->asInt();
	double	dUpdate	= Parameters("UPDATE_STRETCH")->asDouble();

	CSG_Grid	Direct;

	//-----------------------------------------------------
	bool	bWasDay	= false;

	double	Hour_A	= Parameters("HOUR_RANGE")->asRange()->Get_LoVal();
	double	Hour_B	= Parameters("HOUR_RANGE")->asRange()->Get_HiVal();
	double	dHour	= Parameters("HOUR_STEP" )->asDouble();

	m_pDirect->Assign(0.0);
	m_pDiffus->Assign(0.0);

	for(double Hour=Hour_A; Hour<=Hour_B && Set_Progress(Hour - Hour_A, Hour_B - Hour_A); Hour+=dHour)
	{
		SG_UI_Progress_Lock(false);
		bool	bDay	= Get_Insolation(Date, Hour);
		SG_UI_Progress_Lock(true);

		if( Update && (bDay || bWasDay) )
		{
			bWasDay	= bDay;

			switch( Update )
			{
			case 1:	DataObject_Update(m_pDirect              , SG_UI_DATAOBJECT_SHOW);	break;
			case 2:	DataObject_Update(m_pDirect, 0.0, dUpdate, SG_UI_DATAOBJECT_SHOW);	break;
			}

			if( bDay )
			{
				if( !Direct.is_Valid() )
				{
					Direct.Create(*m_pDirect);
				}
				else
				{
					Direct.Add   (*m_pDirect);
				}
				
				m_pDirect->Assign(0.0);
			}
		}
	}

	//-----------------------------------------------------
	if( Update )
	{
		m_pDirect->Assign(&Direct);
	}

	m_pDirect->Multiply(dHour);
	m_pDiffus->Multiply(dHour);

	SG_UI_Progress_Lock(false);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSolarRadiation::Get_Insolation(CSG_DateTime Date, double Hour)
{
	Date.Set_Hour(Hour);

	Process_Set_Text(Date.Format("%A, %d. %B %Y, %X"));

	double	JDN	= floor(Date.Get_JDN()) - 0.5 + Hour / 24.0;	// relate to UTC, avoid problems with daylight saving time

	//-----------------------------------------------------
	m_Solar_Const	= Parameters("SOLARCONST")->asDouble() / 1000.0;	// >> [kW / m²]

	m_Solar_Const	*= 1.0 + 0.03344 * cos(Date.Get_DayOfYear() * 2.0 * M_PI / 365.25 - 0.048869);	// corrected for Earth's orbit eccentricity

	//-----------------------------------------------------
	if( m_Location )
	{
		bool	bDayLight	= false;

		for(int y=0; y<Get_NY() && Process_Get_Okay(); y++)
		{
			#pragma omp parallel for
			for(int x=0; x<Get_NX(); x++)
			{
				double	Sun_Height, Sun_Azimuth;

				if( SG_Get_Sun_Position(JDN, m_Lon.asDouble(x, y), m_Lat.asDouble(x, y), Sun_Height, Sun_Azimuth) )
				{
					bDayLight	= true;
				}

				m_Sun_Height .Set_Value(x, y, Sun_Height);
				m_Sun_Azimuth.Set_Value(x, y, Sun_Azimuth);
			}
		}

		if( bDayLight )
		{
			return( Get_Insolation(0.0, 0.0, Hour) );
		}
	}

	//-----------------------------------------------------
	else
	{
		double	Sun_Height, Sun_Azimuth;

		if( SG_Get_Sun_Position(JDN, 0.0, m_Latitude, Sun_Height, Sun_Azimuth) )
		{
			return( Get_Insolation(Sun_Height, Sun_Azimuth, Hour) );
		}
	}

	//-----------------------------------------------------
	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSolarRadiation::Get_Insolation(double Sun_Height, double Sun_Azimuth, double Hour)
{
	if( !Get_Shade(Sun_Height, Sun_Azimuth) )
	{
		return( false );
	}

	double	dHour	= Parameters("HOUR_STEP")->asDouble();

	for(int y=0; y<Get_NY() && Process_Get_Okay(); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			if( m_pDEM->is_NoData(x, y) )
			{
				m_pDirect->Set_NoData(x, y);
				m_pDiffus->Set_NoData(x, y);
			}
			else
			{
				double	Direct, Diffus;

				if( Get_Irradiance(x, y,
						m_Location ? m_Sun_Height .asDouble(x, y) : Sun_Height,
						m_Location ? m_Sun_Azimuth.asDouble(x, y) : Sun_Azimuth,
						Direct, Diffus) )
				{
					m_pDirect->Add_Value(x, y, Direct);
					m_pDiffus->Add_Value(x, y, Diffus);

					if( Direct > 0.0 )
					{
						if( m_pDuration )
						{
							if( m_pDuration->is_NoData(x, y) )
							{
								m_pDuration->Set_Value(x, y, dHour);
							}
							else
							{
								m_pDuration->Add_Value(x, y, dHour);
							}
						}

						if( m_pSunrise && (m_pSunrise->is_NoData(x, y) || m_pSunrise->asDouble(x, y) > Hour) )
						{
							m_pSunrise->Set_Value(x, y, Hour);
						}

						if( m_pSunset )
						{
							m_pSunset->Set_Value(x, y, Hour);
						}
					}
				}
			}
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline double CSolarRadiation::Get_Air_Mass(double Sun_Height)
{
	static const double	Air_Mass[32]	=	// Optical air mass in 1 degree increments for zenith angles >=60 [after LIST, 1968; p. 422]
	{
		2.00,  2.06,  2.12,  2.19,  2.27,  2.36,  2.45,  2.55, 2.65,  2.77,  2.90,  3.05,  3.21,  3.39,  3.59,  3.82,
		4.07,  4.37,  4.72,  5.12,  5.60,  6.18,  6.88,  7.77, 8.90, 10.39, 12.44, 15.36, 19.79, 26.96, 26.96, 26.96
	};

	//-------------------------------------------------
	if( Sun_Height > 30.0 * M_DEG_TO_RAD )
	{
		return( 1.0 / cos(M_PI_090 - Sun_Height) );
	}

	double	z	= 30.0 - M_RAD_TO_DEG * Sun_Height;
	int		i	= (int)z;

	return( Air_Mass[i] + (z - i) * (Air_Mass[i + 1] - Air_Mass[i]) );
}

//---------------------------------------------------------
inline bool CSolarRadiation::Get_Irradiance(int x, int y, double Sun_Height, double Sun_Azimuth, double &Direct, double &Diffus)
{
	//-----------------------------------------------------
	if( Sun_Height <= 0.0 )
	{
		return( false );
	}

	double	Elevation	= m_pDEM->asDouble(x, y);
	double	Slope		= m_Slope.asDouble(x, y);
	double	Solar_Angle	= m_Shade.asDouble(x, y) > 0.0 ? 0.0 :	// solar incidence angle measured between the Sun and the inclined surface
		cos(Slope) * cos(Sun_Height - M_PI_090) + sin(Slope) * sin(M_PI_090 - Sun_Height) * cos(Sun_Azimuth - m_Aspect.asDouble(x, y));

	//-----------------------------------------------------
	if( m_Method == 3 )	// Hofierka & Suri 2002
	{
		double	h_0		= Sun_Height;	// * M_RAD_TO_DEG;	// ???!!
		double	sin_h_0	= sin(Sun_Height);
		double	d_exp	= Solar_Angle;
		double	T_LK	= m_pLinke && !m_pLinke->is_NoData(x, y) ? m_pLinke->asDouble(x, y) : m_Linke;

		//-------------------------------------------------
		double	h_0ref	= h_0 + 0.061359 * (0.1594 + 1.123 * h_0 + 0.065656 * h_0*h_0) / (1. + 28.9344 * h_0 + 277.3971 * h_0*h_0);
		double	m		= exp(-Elevation / 8434.5) / (sin_h_0 + 0.50572 * pow(h_0ref + 6.07995, -1.6364));
		double	d_R		= m <= 20.0
			? 1. / ( 6.6296 + 1.7513 * m - 0.1202 * m*m + 0.0065 * m*m*m - 0.00013 * m*m*m*m)
			: 1. / (10.4    + 0.718  * m);
		double	B_0c	= m_Solar_Const * exp(-0.8662 * T_LK * m * d_R);
		double	B_hc	= B_0c * sin_h_0;

		Direct			= B_0c * sin(d_exp);	// B_ic

		//-------------------------------------------------
		double	Tn		= -0.015843 + 0.030543 * T_LK + 0.0003797 * T_LK*T_LK;
		double	A1		=  0.26463  - 0.061581 * T_LK + 0.0031408 * T_LK*T_LK;	if( A1 * Tn < 0.0022 )	A1	= 0.0022 / Tn;
		double	A2		=  2.04020  + 0.018945 * T_LK - 0.011161  * T_LK*T_LK;
		double	A3		= -1.3025   + 0.039231 * T_LK + 0.0085079 * T_LK*T_LK;
		double	F_d		= A1 + A2 * sin_h_0 + A3 * sin_h_0*sin_h_0;
		double	D_hc	= m_Solar_Const * Tn * F_d;

		double	K_b		= B_hc / (m_Solar_Const * sin_h_0);
		double	F		= 0.5 * (1. + cos(Slope)) + (sin(Slope) - Slope * cos(Slope) - M_PI * SG_Get_Square(sin(Slope / 2.0)));

		if( m_Shade.asDouble(x, y) > 0.0 )	// shadowed surface
		{
			Diffus	= D_hc * F * 0.25227;
		}
		else if( Sun_Height >= 0.1 )		// sunlit surface, height of Sun above 5.7°
		{
			F	*= 0.00263 - 0.712 * K_b - 0.6883 * K_b*K_b;	// * N

			Diffus	= D_hc * F * (1. - K_b) + K_b * sin(d_exp) / sin_h_0;
		}
		else
		{
			double	A_LN	= Sun_Azimuth - m_Aspect.asDouble(x, y);

			//	if( A_LN < -M_PI_180 ) A_LN += M_PI_360; else if( A_LN > M_PI_180 ) A_LN -= M_PI_360;	// quatsch!!!

			F	*= 0.00263 - 0.712 * K_b - 0.6883 * K_b*K_b;	// * N

			Diffus	= D_hc * F * (1. - K_b) + K_b * sin(Slope) * cos(A_LN) / (0.1 - 0.008 * h_0);
		}

		if( m_pSVF && !m_pSVF->is_NoData(x, y) )
		{
			Diffus	*= m_pSVF->asDouble(x, y);
		}
	}

	//-----------------------------------------------------
	else
	{
		if( m_Method == 0 )	// Boehner
		{
			double	A, E, Vapour;

			Vapour	= m_pVapour && !m_pVapour->is_NoData(x, y) ? m_pVapour->asDouble(x, y) : m_Vapour;
			Vapour	= Vapour > 0.0 ? sqrt(Vapour) : 0.0;
			E		= 0.9160 - 0.05125 * Vapour;
			A		= 0.4158 + 0.03990 * Vapour;

			Direct	= pow(E, (1.0 - Elevation / m_Atmosphere) / sin(Sun_Height));

			Diffus	= m_Atmosphere / (m_Atmosphere - Elevation) * (0.0001165 * SG_Get_Square(M_RAD_TO_DEG * Sun_Height) - 0.0152 * M_RAD_TO_DEG * Sun_Height + A);
			Diffus	= Direct * sin(Sun_Height) * (1.0 / (1.0 - Diffus) - 1.0);
		}

		//-------------------------------------------------
		else				// TAPES
		{
			double	Air_Mass	= Get_Air_Mass(Sun_Height) * (m_Pressure / pow(10.0, Elevation * 5.4667E-05)) / 1013.0;

			//---------------------------------------------
			if( m_Method == 1 )	// Air Pressure, Water and Dust Content
			{
				double	AW, TW, TD, TDC;

				AW		= 1.0 - 0.077 * pow(m_Water * Air_Mass, 0.3);		// absorption by water vapour
				TW		= pow(0.975, m_Water * Air_Mass);					// scattering by water vapour	| problem (?!): fortran source differs from paper
				TD		= pow(0.950, m_Water * m_Dust / 100.0);				// scattering by dust			| problem (?!): 100ppm := 1, 300ppm := 2
				TDC		= pow(0.900, Air_Mass) + 0.026 * (Air_Mass - 1.0);	// scattering by a dust free atmosphere

				Direct	= AW * TW * TD * TDC;
				Diffus	= 0.5 * (AW - Direct);
			}

			//---------------------------------------------
			else				// Lumped Atmospheric Transmittance
			{
				Direct	= pow(m_Transmittance, Air_Mass);
				Diffus	= 0.271 - 0.294 * Direct;
			}

			if( Sun_Height < M_RAD_TO_DEG )
			{
				Diffus	*= Sun_Height;
			}
		}

		//-------------------------------------------------
		Direct	= Solar_Angle <= 0.0 ? 0.0 : Solar_Angle * Direct * m_Solar_Const;

		double	SVF	= m_pSVF && !m_pSVF->is_NoData(x, y) ? m_pSVF->asDouble(x, y) : m_bLocalSVF ? (1.0 + cos(Slope)) / 2.0 : 1.0;

		Diffus	= m_Solar_Const * Diffus * SVF;
	}

	//-----------------------------------------------------
	if( Direct < 0.0 )	{	Direct	= 0.0;	}	else if( Direct > m_Solar_Const )	{	Direct	= m_Solar_Const;	}
	if( Diffus < 0.0 )	{	Diffus	= 0.0;	}	else if( Diffus > m_Solar_Const )	{	Diffus	= m_Solar_Const;	}

	return( true );
}

//---------------------------------------------------------
/* The original TAPES-G source code for the optical air mass computation
C ==================================================================
      SUBROUTINE SOLAR(ZA,RDIRN,RDIFN,ITEST)
      COMMON/SOL1/U,D,P,TRANSM
      PARAMETER (PI=3.14159265358979323846)
      PARAMETER (DTOR=PI/180.)
      DIMENSION AM(32)
      DATA AM/2.0,2.06,2.12,2.19,2.27,2.36,2.45,2.55,2.65,2.77,2.9,
     *  3.05,3.21,3.39,3.59,3.82,4.07,4.37,4.72,5.12,5.6,6.18,6.88,
     *  7.77,8.9,10.39,12.44,15.36,19.79,26.96,26.96,26.96/
      DATA PO/1013./
C     ***************************************************************
C     AM    Optical air mass in 1 degree increments for zenith angles
C           >=60 [LIST, 1968; p. 422]
C     U     Water content of a vertical slice of atmosphere in cm:
C           1.5 to 1.7, average=1.68
C     D     Dust factor: 1=100 ppm (standard); 2=300 ppm
C     P     Barometric pressure in mb
C     PO    Standard atmospheric pressure = 1013 mb
C     TRANSM  Transmittance of the atmosphere (0.6-0.8)
C     AW    Accounts for absorption by water vapour
C     TW    Accounts for scattering by water vapour
C     TD    Accounts for scattering by dust
C     TDC   Accounts for scattering by a dust free atmosphere
C     **************************************************************
C     Compute optical air mass
C
      IF(ZA.LE.60.) THEN
         AMASS=1./COS(ZA*DTOR)
      ELSE
         Y=ZA-59.
         I=INT(Y)
         AMASS=AM(I)+(Y-FLOAT(I))*(AM(I+1)-AM(I))
      ENDIF
      AMASS2=AMASS*P/PO
C     --------------------------------------------------------------
C     Account for atmospheric effects using either a lumped atmos-
C     pheric transmittance approach (ITEST=1) or by calculating the
C     components (ITEST=2)
C
      IF(ITEST.EQ.1) THEN
         RDIRN=TRANSM**AMASS2
         RDIFN=0.271-0.294*RDIRN
      ELSE
         AW=1.0-0.077*(U*AMASS2)**0.3
         TW=0.975**(U*AMASS)
         TD=0.95**(U*D)
         TDC=0.9**AMASS2+0.026*(AMASS2-1.0)
         RDIRN=AW*TW*TD*TDC
         RDIFN=0.5*(AW-RDIRN)
      ENDIF
      RETURN
      END
C ====================================================================*/


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline bool CSolarRadiation::Get_Shade_Params(double Sun_Height, double Sun_Azimuth, double &dx, double &dy, double &dz)
{
	if( Sun_Height <= 0.0 )
	{
		return( false );
	}

	dz	= Sun_Azimuth + M_PI_180;
	dx	= sin(dz);
	dy	= cos(dz);

	if( fabs(dx) > fabs(dy) )
	{
		dy	/= fabs(dx);
		dx	= dx < 0 ? -1 : 1;
	}
	else if( fabs(dy) > fabs(dx) )
	{
		dx	/= fabs(dy);
		dy	= dy < 0 ? -1 : 1;
	}
	else
	{
		dx	= dx < 0 ? -1 : 1;
		dy	= dy < 0 ? -1 : 1;
	}

	dz	= tan(Sun_Height) * sqrt(dx*dx + dy*dy) * Get_Cellsize();

	return( true );
}

//---------------------------------------------------------
bool CSolarRadiation::Get_Shade(double Sun_Height, double Sun_Azimuth)
{
	const double	dShadow	= 0.49;

	m_Shade.Assign(0.0);

	//-----------------------------------------------------
	if( m_Location == 0 )
	{
		double	dx, dy, dz;

		if( !Get_Shade_Params(Sun_Height, Sun_Azimuth, dx, dy, dz) )
		{
			return( false );
		}

		for(int y=0; y<Get_NY() && Process_Get_Okay(); y++)
		{
			#pragma omp parallel for
			for(int x=0; x<Get_NX(); x++)
			{
				if( !m_pDEM->is_NoData(x, y) )
				{
					switch( m_Shadowing )
					{
					case 0:	// slim
						Set_Shade(x, y, m_pDEM->asDouble(x, y), dx, dy, dz);
						break;

					case 1:	// fat
						Set_Shade(x - dShadow, y - dShadow, m_pDEM->asDouble(x, y), dx, dy, dz);
						Set_Shade(x + dShadow, y - dShadow, m_pDEM->asDouble(x, y), dx, dy, dz);
						Set_Shade(x - dShadow, y + dShadow, m_pDEM->asDouble(x, y), dx, dy, dz);
						Set_Shade(x + dShadow, y + dShadow, m_pDEM->asDouble(x, y), dx, dy, dz);
						break;
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	else
	{
		for(int y=0; y<Get_NY() && Process_Get_Okay(); y++)
		{
			#pragma omp parallel for
			for(int x=0; x<Get_NX(); x++)
			{
				if( !m_pDEM->is_NoData(x, y) )
				{
					switch( m_Shadowing )
					{
					case 0:	// slim
						Set_Shade_Bended(x, y, m_pDEM->asDouble(x, y));
						break;

					case 1:	// fat
						Set_Shade_Bended(x - dShadow, y - dShadow, m_pDEM->asDouble(x, y));
						Set_Shade_Bended(x + dShadow, y - dShadow, m_pDEM->asDouble(x, y));
						Set_Shade_Bended(x - dShadow, y + dShadow, m_pDEM->asDouble(x, y));
						Set_Shade_Bended(x + dShadow, y + dShadow, m_pDEM->asDouble(x, y));
						break;
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}

//---------------------------------------------------------
void CSolarRadiation::Set_Shade(double x, double y, double z, double dx, double dy, double dz)
{
	for(x+=dx+0.5, y+=dy+0.5, z-=dz; ; x+=dx, y+=dy, z-=dz)
	{
		int	ix	= (int)x,	iy	= (int)y;

		if( !is_InGrid(ix, iy) )
		{
			return;
		}

		if( !m_pDEM->is_NoData(ix, iy) )
		{
			double	zDiff	= z - m_pDEM->asDouble(ix, iy);

			if( zDiff <= 0.0 )
			{
				return;
			}

			m_Shade.Set_Value(ix, iy, zDiff);
		}
	}
}

//---------------------------------------------------------
void CSolarRadiation::Set_Shade_Bended(double x, double y, double z)
{
	x	+= 0.5;	y	+= 0.5;

	for(int ix=(int)x, iy=(int)y; ; )
	{
		double	dx, dy, dz;

		if( !Get_Shade_Params(m_Sun_Height.asDouble(ix, iy), m_Sun_Azimuth.asDouble(ix, iy), dx, dy, dz) )
		{
			return;
		}

		x	+= dx;	ix	= (int)x;
		y	+= dy;	iy	= (int)y;
		z	-= dz;

		if( !m_pDEM->is_InGrid(ix, iy) )
		{
			return;
		}

		if( !m_pDEM->is_NoData(ix, iy) )
		{
			double	zDiff	= z - m_pDEM->asDouble(ix, iy);

			if( zDiff <= 0.0 )
			{
				return;
			}

			m_Shade.Set_Value(ix, iy, zDiff);
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
