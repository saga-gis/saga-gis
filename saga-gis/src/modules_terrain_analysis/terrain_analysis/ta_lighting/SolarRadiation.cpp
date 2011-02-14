
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
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
	CSG_Parameter	*pNode_1, *pNode_2, *pNode_3;

	//-----------------------------------------------------
	Set_Name		(_TL("Potential Incoming Solar Radiation"));

	Set_Author		(SG_T("O.Conrad (c) 2010"));

	Set_Description	(_TW(
		"Calculation of potential incoming solar radiation (insolation).\n"
		"\n"
		"References:\n<ul>"
		"<li>Boehner, J., Antonic, O. (2009): Land Surface Parameters Specific to Topo-Climatology. in Hengl, T. & Reuter, H.I. [Eds.]: Geomorphometry - Concepts, Software, Applications.</li>"
		"<li>Oke, T.R. (1988): Boundary Layer Climates. London, Taylor & Francis.</li>"
		"<li>Wilson, J.P., Gallant, J.C. [Eds.] (2000): Terrain Analysis - Principles and Applications. New York, John Wiley & Sons, Inc.</li>"
		"</ul>\n"

		"\n*) Most options should do well, but TAPES-G based diffuse irradiance calculation ('Atmospheric Effects' methods 2 and 3) needs further revision!"
	));

	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL	, "GRD_DEM"			, _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "GRD_SVF"			, _TL("Sky View Factor"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		NULL	, "GRD_VAPOUR"		, _TL("Water Vapour Pressure [mbar]"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		NULL	, "GRD_LAT"			, _TL("Latitude [degree]"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		NULL	, "GRD_LON"			, _TL("Longitude [degree]"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		NULL	, "GRD_DIRECT"		, _TL("Direct Insolation"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		NULL	, "GRD_DIFFUS"		, _TL("Diffuse Insolation"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		NULL	, "GRD_TOTAL"		, _TL("Total Insolation"),
		_TL("Total insolation, the sum of direct and diffuse incoming solar radiation."),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		NULL	, "GRD_RATIO"		, _TL("Direct to Diffuse Ratio"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		NULL	, "DURATION"		, _TL("Duration of Insolation"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		NULL	, "SUNRISE"			, _TL("Sunrise"),
		_TL("time of sunrise/sunset is only calculated if time span is set to single day"),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		NULL	, "SUNSET"			, _TL("Sunset"),
		_TL("time of sunrise/sunset is only calculated if time span is set to single day"),
		PARAMETER_OUTPUT_OPTIONAL
	);

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
			SG_T("kWh / m\xb2"),
			SG_T("kJ / m\xb2"),
			SG_T("J / cm\xb2")
		), 0
	);

	Parameters.Add_Choice(
		NULL	, "UPDATE"			, _TL("Update"),
		_TL("show direct insolation for each time step."),
		CSG_String::Format(SG_T("%s|%s|%s|"),
			_TL("do not update"),
			_TL("update, colour stretch for each time step"),
			_TL("update, fixed colour stretch")
		), 0
	);

	//-----------------------------------------------------
	pNode_1	= Parameters.Add_Node(
		NULL	, "NODE_TIME"		, _TL("Time"),
		_TL("")
	);

	Parameters.Add_Choice(
		pNode_1	, "PERIOD"			, _TL("Time Period"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|"),
			_TL("moment"),
			_TL("day"),
			_TL("range of days")
		), 1
	);

	Parameters.Add_Value(
		pNode_1	, "MOMENT"			, _TL("Moment [h]"),
		_TL(""),
		PARAMETER_TYPE_Double		, 12.0, 0.0, true, 24.0, true
	);

	Parameters.Add_Range(
		pNode_1	, "HOUR_RANGE"		, _TL("Time Span [h]"),
		_TL("Time span used for the calculation of daily radiation sums."),
		 0.0, 24.0, 0.0	, true, 24.0, true
	);

	Parameters.Add_Value(
		pNode_1	, "DHOUR"			, _TL("Time Resolution [h]: Day"),
		_TL("Time step size for a day's calculation given in hours."),
		PARAMETER_TYPE_Double		, 0.5, 0.0, true, 24.0, true
	);

	Parameters.Add_Value(
		pNode_1	, "DDAYS"			, _TL("Time Resolution [d]: Range of Days"),
		_TL("Time step size for a range of days calculation given in days."),
		PARAMETER_TYPE_Int			, 5, 1, true
	);

	//-----------------------------------------------------
	pNode_2	= Parameters.Add_Node(
		pNode_1	, "NODE_DAY_A"		, _TL("Day of Year"),
		_TL("")
	);

	Parameters.Add_Choice(
		pNode_2	, "DAY_A"			, _TL("Day of Month"),
		_TL(""),
		SG_T("1|2|3|4|5|6|7|8|9|10|11|12|13|14|15|16|17|18|19|20|21|22|23|24|25|26|27|28|29|30|31|"), 20
	);

	Parameters.Add_Choice(
		pNode_2	, "MON_A"			, _TL("Month"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|"),
			_TL("January")	, _TL("February")	, _TL("March")		, _TL("April")	, _TL("May")		, _TL("June"),
			_TL("July")		, _TL("August")		, _TL("September")	, _TL("October"), _TL("November")	, _TL("December")
		), 2
	);

	pNode_2	= Parameters.Add_Node(
		pNode_1	, "NODE_DAY_B"		, _TL("Day of Year (End of Range)"),
		_TL("")
	);

	Parameters.Add_Choice(
		pNode_2	, "DAY_B"			, _TL("Day of Month"),
		_TL(""),
		SG_T("1|2|3|4|5|6|7|8|9|10|11|12|13|14|15|16|17|18|19|20|21|22|23|24|25|26|27|28|29|30|31|"), 20
	);

	Parameters.Add_Choice(
		pNode_2	, "MON_B"			, _TL("Month"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|"),
			_TL("January")	, _TL("February")	, _TL("March")		, _TL("April")	, _TL("May")		, _TL("June"),
			_TL("July")		, _TL("August")		, _TL("September")	, _TL("October"), _TL("November")	, _TL("December")
		), 3
	);

	//-----------------------------------------------------
	pNode_1	= Parameters.Add_Node(
		NULL	, "NODE_METHOD"		, _TL("Atmospheric Effects"),
		_TL("")
	);

	Parameters.Add_Choice(
		pNode_1	, "METHOD"			, _TL("Atmospheric Effects"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|"),
			_TL("Height of Atmosphere and Vapour Pressure"),
			_TL("Air Pressure, Water and Dust Content"),
			_TL("Lumped Atmospheric Transmittance")
		), 0
	);

	pNode_2	= Parameters.Add_Node(
		pNode_1	, "NODE_SADO"		, _TL("Height of Atmosphere and Vapour Pressure"),
		_TL("")
	);

	Parameters.Add_Value(
		pNode_2	, "ATMOSPHERE"		, _TL("Height of Atmosphere [m]"),
		_TL(""),
		PARAMETER_TYPE_Double		, 12000.0, 0.0, true
	);

	Parameters.Add_Value(
		pNode_2	, "VAPOUR"			, _TL("Water Vapour Pressure [mbar]"),
		_TL("This value is used as constant if no vapour pressure grid is given."),
		PARAMETER_TYPE_Double		, 10, 0.0, true
	);

	//-----------------------------------------------------
	pNode_2	= Parameters.Add_Node(
		pNode_1	, "NODE_COMPONENTS"	, _TL("Air Pressure, Water and Dust Content"),
		_TL("")
	);

	Parameters.Add_Value(
		pNode_2	, "PRESSURE"		, _TL("Atmospheric Pressure [mbar]"),
		_TL(""),
		PARAMETER_TYPE_Double, 1013, 0.0, true
	);

	Parameters.Add_Value(
		pNode_2	, "WATER"			, _TL("Water Content [cm]"),
		_TL("Water content of a vertical slice of atmosphere in cm: 1.5 to 1.7, average=1.68"),
		PARAMETER_TYPE_Double, 1.68, 0.0, true
	);

	Parameters.Add_Value(
		pNode_2	, "DUST"			, _TL("Dust [ppm]"),
		_TL("Dust factor: 100 ppm (standard)"),
		PARAMETER_TYPE_Double, 100, 0.0, true
	);

	//-----------------------------------------------------
	pNode_2	= Parameters.Add_Node(
		pNode_1	, "NODE_LUMPED"		, _TL("Lumped Atmospheric Transmittance"),
		_TL("")
	);

	Parameters.Add_Value(
		pNode_2	, "LUMPED"			, _TL("Lumped Atmospheric Transmittance [Percent]"),
		_TL("The transmittance of the atmosphere, usually between 60 and 80 percent."),
		PARAMETER_TYPE_Double, 70, 0.0, true, 100.0, true
	);

	//-----------------------------------------------------
	pNode_1	= Parameters.Add_Node(
		NULL	, "NODE_LOCATION"	, _TL("Location"),
		_TL("Location settings to be used, if no latitude/longitude grids are given.")
	);

	Parameters.Add_Value(
		pNode_1	, "LATITUDE"		, _TL("Latitude"),
		_TL(""),
		PARAMETER_TYPE_Degree		, 53.0, -90.0, true, 90.0, true
	);

	pNode_2	= Parameters.Add_Value(
		pNode_1	, "BENDING"			, _TL("Planetery Bending"),
		_TL(""),
		PARAMETER_TYPE_Bool			, false
	);

	Parameters.Add_Value(
		pNode_2	, "RADIUS"			, _TL("Planetary Radius"),
		_TL(""),
		PARAMETER_TYPE_Double		, 6366737.96, 0.0, true
	);

	pNode_3	= Parameters.Add_Choice(
		pNode_2	, "LAT_OFFSET"		, _TL("Latitude relates to grid's..."),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|"),
			_TL("bottom"),
			_TL("center"),
			_TL("top"),
			_TL("user defined reference")
		), 3
	);

	Parameters.Add_Value(
		pNode_3	, "LAT_REF_USER"	, _TL("Latitude (user defined reference)"),
		_TL(""),
		PARAMETER_TYPE_Double		, 0.0
	);

	pNode_3	= Parameters.Add_Choice(
		pNode_2	, "LON_OFFSET"		, _TL("Local time relates to grid's..."),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|"),
			_TL("left"),
			_TL("center"),
			_TL("right"),
			_TL("user defined reference")
		), 1
	);

	Parameters.Add_Value(
		pNode_3	, "LON_REF_USER"	, _TL("Local Time (user defined reference)"),
		_TL(""),
		PARAMETER_TYPE_Double		, 0.0
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSolarRadiation::On_Execute(void)
{
	int		x, y;

	//-----------------------------------------------------
	m_pDEM			= Parameters("GRD_DEM")		->asGrid();
	m_pVapour		= Parameters("GRD_VAPOUR")	->asGrid();
	m_pSVF			= Parameters("GRD_SVF")		->asGrid();

	m_pDirect		= Parameters("GRD_DIRECT")	->asGrid();
	m_pDiffus		= Parameters("GRD_DIFFUS")	->asGrid();
	m_pTotal		= Parameters("GRD_TOTAL")	->asGrid();
	m_pRatio		= Parameters("GRD_RATIO")	->asGrid();
	m_pDuration		= Parameters("DURATION")	->asGrid();
	m_pSunrise		= Parameters("SUNRISE")		->asGrid();
	m_pSunset		= Parameters("SUNSET")		->asGrid();

	m_bUpdate		= Parameters("UPDATE")		->asInt();

	m_Solar_Const	= Parameters("SOLARCONST")	->asDouble() / 1000.0;	// >> [kW / m²]
	m_bLocalSVF		= Parameters("LOCALSVF")	->asBool();

	m_Method		= Parameters("METHOD")		->asInt();
	m_Atmosphere	= Parameters("ATMOSPHERE")	->asDouble();
	m_Vapour		= Parameters("VAPOUR")		->asDouble();
	m_Transmittance	= Parameters("LUMPED")		->asDouble() / 100.0;	// percent to ratio
	m_Pressure		= Parameters("PRESSURE")	->asDouble();
	m_Water			= Parameters("WATER")		->asDouble();
	m_Dust			= Parameters("DUST")		->asDouble();

	m_Latitude		= Parameters("LATITUDE")	->asDouble() * M_DEG_TO_RAD;
	m_bBending		= Parameters("BENDING")		->asBool();

	m_Time			= Parameters("PERIOD")		->asInt();
	m_dHour			= Parameters("DHOUR")		->asDouble();
	m_dDays			= Parameters("DDAYS")		->asInt();

	//-----------------------------------------------------
	switch( m_Time )
	{
	case 0:	// moment
		m_Hour_A		= Parameters("MOMENT")->asDouble();
		m_Day_A			= Parameters("DAY_A")->asInt() + 1 + Get_Day_of_Year(Parameters("MON_A")->asInt());
		break;

	case 1:	// day
		m_Hour_A		= Parameters("HOUR_RANGE")->asRange()->Get_LoVal();
		m_Hour_B		= Parameters("HOUR_RANGE")->asRange()->Get_HiVal();
		m_Day_A			= Parameters("DAY_A")->asInt() + 1 + Get_Day_of_Year(Parameters("MON_A")->asInt());
		m_Day_B			= m_Day_A;
		break;

	case 2:	// range of days
		m_Hour_A		= Parameters("HOUR_RANGE")->asRange()->Get_LoVal();
		m_Hour_B		= Parameters("HOUR_RANGE")->asRange()->Get_HiVal();
		m_Day_A			= Parameters("DAY_A")->asInt() + 1 + Get_Day_of_Year(Parameters("MON_A")->asInt());
		m_Day_B			= Parameters("DAY_B")->asInt() + 1 + Get_Day_of_Year(Parameters("MON_B")->asInt());

		if( m_Day_B < m_Day_A )
		{
			m_Day_B	+= 365;
		}
		break;
	}

	//-----------------------------------------------------
	CSG_Colors	Colors(100, SG_COLORS_YELLOW_RED, true);

	Colors.Set_Ramp(SG_GET_RGB(  0,   0,  64), SG_GET_RGB(255, 159,   0),  0, 50);
	Colors.Set_Ramp(SG_GET_RGB(255, 159,   0), SG_GET_RGB(255, 255, 255), 50, 99);

	DataObject_Set_Colors(m_pDirect  , Colors);
	DataObject_Set_Colors(m_pDiffus  , Colors);
	DataObject_Set_Colors(m_pTotal   , Colors);
	DataObject_Set_Colors(m_pRatio   , 100, SG_COLORS_RED_GREY_BLUE	, true);
	DataObject_Set_Colors(m_pDuration, 100, SG_COLORS_YELLOW_RED	, true);
	DataObject_Set_Colors(m_pSunrise , 100, SG_COLORS_YELLOW_RED	, false);
	DataObject_Set_Colors(m_pSunset  , 100, SG_COLORS_YELLOW_RED	, true);

	//-----------------------------------------------------
	if( m_pDuration )	m_pDuration->Assign_NoData();
	if( m_pSunrise  )	m_pSunrise ->Assign_NoData();
	if( m_pSunset   )	m_pSunset  ->Assign_NoData();

	if( m_pDuration )	m_pDuration->Set_Unit(_TL("h"));
	if( m_pSunrise  )	m_pSunrise ->Set_Unit(_TL("h"));
	if( m_pSunset   )	m_pSunset  ->Set_Unit(_TL("h"));

	//-----------------------------------------------------
	Process_Set_Text(_TL("initialising gradient..."));

	m_Shade .Create(*Get_System(), SG_DATATYPE_Byte);
	m_Slope .Create(*Get_System(), SG_DATATYPE_Float);
	m_Aspect.Create(*Get_System(), SG_DATATYPE_Float);

	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
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
	if( m_bBending )
	{
		Process_Set_Text(_TL("initialising planetary bending..."));

		m_Lat			.Create(*Get_System(), SG_DATATYPE_Float);
		m_Lon			.Create(*Get_System(), SG_DATATYPE_Float);
		m_Sol_Height	.Create(*Get_System(), SG_DATATYPE_Float);
		m_Sol_Azimuth	.Create(*Get_System(), SG_DATATYPE_Float);

		//-------------------------------------------------
		if( Parameters("GRD_LAT")->asGrid() || Parameters("GRD_LON")->asGrid() )
		{
			if( Parameters("GRD_LAT")->asGrid() )
			{
				m_Lat	= *Parameters("GRD_LAT")->asGrid();
				m_Lat	*= M_DEG_TO_RAD;
			}

			if( Parameters("GRD_LON")->asGrid() )
			{
				m_Lon	= *Parameters("GRD_LON")->asGrid();
				m_Lon	*= M_DEG_TO_RAD;
			}
		}

		//-------------------------------------------------
		else
		{
			double	d, dx, dy, dxA, dyA;

			d	= M_DEG_TO_RAD / (Parameters("RADIUS")->asDouble() * M_PI / 180.0);

			switch( Parameters("LON_OFFSET")->asInt() )
			{
			case 0:	dxA	= Get_System()->Get_Extent().Get_XMin();	break;	// left
			case 1:	dxA	= Get_System()->Get_Extent().Get_XCenter();	break;	// center
			case 2:	dxA	= Get_System()->Get_Extent().Get_XMax();	break;	// right
			case 3:	dxA	= Parameters("LON_REF_USER")->asDouble();	break;	// user defined coordinate
			}

			switch( Parameters("LAT_OFFSET")->asInt() )
			{
			case 0:	dyA	= Get_System()->Get_Extent().Get_YMin();	break;	// bottom
			case 1:	dyA	= Get_System()->Get_Extent().Get_YCenter();	break;	// center
			case 2:	dyA	= Get_System()->Get_Extent().Get_YMax();	break;	// top
			case 3:	dyA	= Parameters("LAT_REF_USER")->asDouble();	break;	// user defined coordinate
			}

			dxA	 = d * (Get_XMin() - dxA);
			dyA	 = d * (Get_YMin() - dyA) + m_Latitude;
			d	*= Get_Cellsize();

			for(y=0, dy=dyA; y<Get_NY() && Set_Progress(y); y++, dy+=d)
			{
				for(x=0, dx=dxA; x<Get_NX(); x++, dx+=d)
				{
					m_Lat.Set_Value(x, y, dy);
					m_Lon.Set_Value(x, y, dx);
				}
			}
		}
	}

	//-----------------------------------------------------
	if( Get_Insolation() )
	{
		Finalise();

		return( true );
	}

	Finalise();

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSolarRadiation::Finalise(void)
{
	double		dUnit;
	CSG_String	Unit;

	//-----------------------------------------------------
	if( m_Time == 0 )	// moment
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
		dUnit	= 3.6;
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
		for(int i=0; i<Get_NCells(); i++)
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
	m_Shade			.Destroy();
	m_Slope			.Destroy();
	m_Aspect		.Destroy();

	m_Lat			.Destroy();
	m_Lon			.Destroy();
	m_Sol_Height	.Destroy();
	m_Sol_Azimuth	.Destroy();

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
	if( m_Time == 0 )						// Moment
	{
		m_pDirect->Assign(0.0);
		m_pDiffus->Assign(0.0);

		Get_Insolation(m_Day_A, m_Hour_A);
	}

	//-----------------------------------------------------
	else if( m_Time == 1 )					// One Day
	{
		Get_Insolation(m_Day_A);
	}

	//-----------------------------------------------------
	else if( m_Day_B - m_Day_A <= m_dDays )	// Range of Days (one representative calculation)
	{
		Get_Insolation(m_Day_A + m_dDays / 2);

		m_pDirect->Multiply(m_Day_B - m_Day_A);
		m_pDiffus->Multiply(m_Day_B - m_Day_A);
	}

	//-----------------------------------------------------
	else									// Range of Days (more than one time step)
	{
		CSG_Grid	Direct, Diffus;

		Direct.Create(*Get_System(), SG_DATATYPE_Float);
		Diffus.Create(*Get_System(), SG_DATATYPE_Float);

		Direct.Assign(0.0);
		Diffus.Assign(0.0);

		for(int Day=m_Day_A+m_dDays/2; Day<=m_Day_B && Process_Get_Okay(false); Day+=m_dDays)
		{
			Get_Insolation(Day);

			SG_UI_Progress_Lock(true);

			Direct.Add(*m_pDirect);
			Diffus.Add(*m_pDiffus);

			SG_UI_Progress_Lock(false);
		}

		m_pDirect->Assign(&Direct);
		m_pDiffus->Assign(&Diffus);

		m_pDirect->Multiply(m_dDays);
		m_pDiffus->Multiply(m_dDays);
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSolarRadiation::Get_Insolation(int Day)
{
	//-----------------------------------------------------
	double		Range	= 0.000001 + cos(M_DEG_TO_RAD * fabs(m_Latitude)) * sin(M_PI * ((80 + Day) % 365) / 365.0);
	CSG_Grid	Direct;

	if( m_bUpdate )
	{
		if( m_bUpdate == 2 )
		{
			DataObject_Update(m_pDirect, 0.0, Range, SG_UI_DATAOBJECT_SHOW);
		}
		else
		{
			DataObject_Update(m_pDirect, SG_UI_DATAOBJECT_SHOW);
		}

		Direct.Create(*Get_System(), SG_DATATYPE_Float);
	}

	//-----------------------------------------------------
	bool		bDay, bWasDay	= false;

	m_pDirect->Assign(0.0);
	m_pDiffus->Assign(0.0);

	for(double Hour=m_Hour_A; Hour<=m_Hour_B && Set_Progress(Hour - m_Hour_A, m_Hour_B - m_Hour_A); Hour+=m_dHour)
	{
		Process_Set_Text(CSG_String::Format(SG_T("%s %d(%d-%d), %s %02d:%02d"), _TL("day"), Day, m_Day_A, m_Day_B, _TL("local time"), (int)Hour, (int)(60.0 * fmod(Hour, 1.0))));

		SG_UI_Progress_Lock(true);

		bDay	= Get_Insolation(Day, Hour);

		if( m_bUpdate && (bDay || bWasDay) )
		{
			bWasDay	= bDay;

			if( m_bUpdate == 2 )
			{
				DataObject_Update(m_pDirect, 0.0, Range, SG_UI_DATAOBJECT_SHOW);
			}
			else
			{
				DataObject_Update(m_pDirect, SG_UI_DATAOBJECT_SHOW);
			}

			if( bDay )
			{
				Direct	+= *m_pDirect;

				m_pDirect->Assign(0.0);
			}
		}

		SG_UI_Progress_Lock(false);
	}

	//-----------------------------------------------------
	if( m_bUpdate )
	{
		m_pDirect->Assign(&Direct);
	}

	m_pDirect->Multiply(m_dHour);
	m_pDiffus->Multiply(m_dHour);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSolarRadiation::Get_Insolation(int Day, double Hour)
{
	double	Sol_Height, Sol_Azimuth;

	//-----------------------------------------------------
	if( m_bBending )
	{
		bool	bDayLight	= false;

		for(int y=0; y<Get_NY() && Set_Progress(y); y++)
		{
			for(int x=0; x<Get_NX(); x++)
			{
				if( Get_Solar_Position(Day, Hour, m_Lat.asDouble(x, y), m_Lon.asDouble(x, y), Sol_Height, Sol_Azimuth) )
				{
					bDayLight	= true;
				}

				m_Sol_Height .Set_Value(x, y, Sol_Height);
				m_Sol_Azimuth.Set_Value(x, y, Sol_Azimuth);
			}
		}

		if( bDayLight )
		{
			return( Get_Insolation(Sol_Height, Sol_Azimuth, Hour) );
		}
	}

	//-----------------------------------------------------
	else
	{
		if( Get_Solar_Position(Day, Hour, m_Latitude, 0.0, Sol_Height, Sol_Azimuth) )
		{
			return( Get_Insolation(Sol_Height, Sol_Azimuth, Hour) );
		}
	}

	//-----------------------------------------------------
	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSolarRadiation::Get_Insolation(double Sol_Height, double Sol_Azimuth, double Hour)
{
	double	Direct, Diffus;

	Get_Shade(Sol_Height, Sol_Azimuth);

	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			if( m_pDEM->is_NoData(x, y) )
			{
				m_pDirect->Set_NoData(x, y);
				m_pDiffus->Set_NoData(x, y);
			}
			else
			{
				if( m_bBending )
				{
					Sol_Height	= m_Sol_Height .asDouble(x, y);
					Sol_Azimuth	= m_Sol_Azimuth.asDouble(x, y);
				}

				if( Get_Irradiance(x, y, Sol_Height, Sol_Azimuth, Direct, Diffus) )
				{
					m_pDirect->Add_Value(x, y, Direct);
					m_pDiffus->Add_Value(x, y, Diffus);

					if( Direct > 0.0 )
					{
						if( m_pDuration )
						{
							if( m_pDuration->is_NoData(x, y) )
							{
								m_pDuration	->Set_Value(x, y, m_dHour);
							}
							else
							{
								m_pDuration	->Add_Value(x, y, m_dHour);
							}
						}

						if( m_pSunrise && (m_pSunrise->is_NoData(x, y) || m_pSunrise->asDouble(x, y) > Hour) )
						{
							m_pSunrise	->Set_Value(x, y, Hour);
						}

						if( m_pSunset )
						{
							m_pSunset	->Set_Value(x, y, Hour);
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
inline double CSolarRadiation::Get_Air_Mass(double Sol_Height)
{
	static const double	Air_Mass[32]	=	// Optical air mass in 1 degree increments for zenith angles >=60 [after LIST, 1968; p. 422]
	{
		2.00,  2.06,  2.12,  2.19,  2.27,  2.36,  2.45,  2.55, 2.65,  2.77,  2.90,  3.05,  3.21,  3.39,  3.59,  3.82,
		4.07,  4.37,  4.72,  5.12,  5.60,  6.18,  6.88,  7.77, 8.90, 10.39, 12.44, 15.36, 19.79, 26.96, 26.96, 26.96
	};

	//-------------------------------------------------
	double	Zenith	= M_PI_090 - Sol_Height;

	if( Zenith <= 60.0 * M_DEG_TO_RAD )
	{
		return( 1.0 / cos(Zenith) );
	}
	else
	{
		double	z	= M_RAD_TO_DEG * Zenith - 60.0;
		int		i	= (int)z;

		return( Air_Mass[i] + (z - i) * (Air_Mass[i + 1] - Air_Mass[i]) );
	}
}

//---------------------------------------------------------
inline bool CSolarRadiation::Get_Irradiance(int x, int y, double Sol_Height, double Sol_Azimuth, double &Direct, double &Diffus)
{
	if( Sol_Height <= 0.0 )
	{
		return( false );
	}

	//-----------------------------------------------------
	double	Elevation, Slope, Solar_Angle;

	Elevation	= m_pDEM->asDouble(x, y);
	Slope		= m_Slope.asDouble(x, y);
	Solar_Angle	= m_Shade.asInt(x, y) ? 0.0 : cos(Slope) * cos(Sol_Height - M_PI_090) + sin(Slope) * sin(M_PI_090 - Sol_Height) * cos(Sol_Azimuth - m_Aspect.asDouble(x, y));

	//-----------------------------------------------------
	if( m_Method == 0 )	// Boehner
	{
		double	A, E, Vapour;

		Vapour	= m_pVapour && !m_pVapour->is_NoData(x, y) ? m_pVapour->asDouble(x, y) : m_Vapour;
		Vapour	= Vapour > 0.0 ? sqrt(Vapour) : 0.0;
		E		= 0.9160 - 0.05125 * Vapour;
		A		= 0.4158 + 0.03990 * Vapour;

		Direct	= pow(E, (1.0 - Elevation / m_Atmosphere) / sin(Sol_Height));

		Diffus	= m_Atmosphere / (m_Atmosphere - Elevation) * (0.0001165 * SG_Get_Square(M_RAD_TO_DEG * Sol_Height) - 0.0152 * M_RAD_TO_DEG * Sol_Height + A);
		Diffus	= Direct * sin(Sol_Height) * (1.0 / (1.0 - Diffus) - 1.0);
	}

	//-----------------------------------------------------
	else				// TAPES
	{
		double	Air_Mass	= Get_Air_Mass(Sol_Height) * (m_Pressure / pow(10.0, Elevation * 5.4667E-05)) / 1013.0;

		//-------------------------------------------------
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

		//-------------------------------------------------
		else				// Lumped Atmospheric Transmittance
		{
			Direct	= pow(m_Transmittance, Air_Mass);
			Diffus	= 0.271 - 0.294 * Direct;
		}
	}

	//-----------------------------------------------------
	Direct	= Solar_Angle <= 0.0 ? 0.0 : Solar_Angle * Direct * m_Solar_Const;

	double	SVF	= m_pSVF && !m_pSVF->is_NoData(x, y) ? m_pSVF->asDouble(x, y) : m_bLocalSVF ? (1.0 + cos(Slope)) / 2.0 : 1.0;

	Diffus	= m_Solar_Const * Diffus * SVF;

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
bool CSolarRadiation::Get_Shade(double Sol_Height, double Sol_Azimuth)
{
	m_Shade.Assign(0.0);

	if( !m_bBending )
	{
		int		i, x, y;
		double	dx, dy, dz;

		Get_Shade_Params(Sol_Height, Sol_Azimuth, dx, dy, dz);

		for(i=0; i<Get_NCells() && Set_Progress_NCells(i); i++)
		{
			if( m_pDEM->Get_Sorted(i, x, y) && !Get_Shade_Complete(x, y) )
			{
				Set_Shade(x, y, dx, dy, dz);
			}
		}
	}

	//-----------------------------------------------------
	else
	{
		int		i, x, y, iLock;

		for(i=0, iLock=1; i<Get_NCells() && Set_Progress_NCells(i); i++, iLock++)
		{
			if( m_pDEM->Get_Sorted(i, x, y) && !Get_Shade_Complete(x, y) )
			{
				if( iLock >= 255 )
					iLock	= 1;

				if( iLock == 1 )
					Lock_Create();

				Set_Shade_Bended(x, y, iLock);
			}
		}
	}

	return( true );
}

//---------------------------------------------------------
void CSolarRadiation::Set_Shade(int x, int y, double dx, double dy, double dz)
{
	for(double ix=x+0.5, iy=y+0.5, iz=m_pDEM->asDouble(x, y); ; )
	{
		x	= (int)(ix	+= dx);
		y	= (int)(iy	+= dy);
					iz	-= dz;

		if( !is_InGrid(x, y) || m_pDEM->asDouble(x, y) > iz )
		{
			return;
		}

		m_Shade.Set_Value(x, y, 1);
	}
}

//---------------------------------------------------------
void CSolarRadiation::Set_Shade_Bended(int x, int y, char iLock)
{
	double	dx, dy, dz;

	Get_Shade_Params(m_Sol_Height.asDouble(x, y), m_Sol_Azimuth.asDouble(x, y), dx, dy, dz);

	for(double ix=x+0.5, iy=y+0.5, iz=m_pDEM->asDouble(x, y); ; )
	{
		x	= (int)(ix	+= dx);
		y	= (int)(iy	+= dy);
					iz	-= dz;

		if( !is_InGrid(x, y) || m_pDEM->asDouble(x, y) > iz || Lock_Get(x, y) == iLock )
		{
			return;
		}

		m_Shade.Set_Value(x, y, 1);

		//---------------------------------------------
		Lock_Set(x, y, iLock);

		Get_Shade_Params(m_Sol_Height.asDouble(x, y), m_Sol_Azimuth.asDouble(x, y), dx, dy, dz);
	}
}

//---------------------------------------------------------
inline bool CSolarRadiation::Get_Shade_Complete(int x, int y)
{
	if( m_Shade.asInt(x, y) == 1 )
	{
		for(int iy=y-1; iy<=y+1; iy++)
		{
			for(int ix=x-1; ix<x+1; ix++)
			{
				if( is_InGrid(ix, iy) && m_Shade.asInt(ix, iy) == 0 )
				{
					return( false );
				}
			}
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
inline void CSolarRadiation::Get_Shade_Params(double Sol_Height, double Sol_Azimuth, double &dx, double &dy, double &dz)
{
	dz	= Sol_Azimuth + M_PI_180;
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

	dz	= tan(Sol_Height) * sqrt(dx*dx + dy*dy) * Get_Cellsize();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline int CSolarRadiation::Get_Day_of_Year(int Month)
{
	static const int	Day[13]	= {	0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 };

	return( Month < 0 ? 0 : Month > 12 ? Day[12] : Day[Month] );
}

//---------------------------------------------------------
inline bool CSolarRadiation::Get_Solar_Position(int Day, double Hour, double LAT, double LON, double &Sol_Height, double &Sol_Azimuth)
{
	static const double	ECLIPTIC_OBL	= M_DEG_TO_RAD * 23.43999;	// obliquity of ecliptic

	int		i;

	double	JD, T, M, L, X, Y, Z, R, UTime,
			DEC, RA, theta, tau,
			Month, Year		= 2000;

	//-----------------------------------------------------
	Day	%= 365;

	for(Month=1, i=0; i<=12; i++)
	{
		if( Day < Get_Day_of_Year(i) )
		{
			Month	= i;
			Day		-= Get_Day_of_Year(i - 1);
			break;
		}
	}

	if( Month < 1 || Month > 12 )	// nur Tag (1 - 365) zaehlt...
	{
		Month		= 1;
	}

	if( Month <= 2 )
	{
		Month		+= 12;
		Year		-= 1;
	}


	//-----------------------------------------------------

//	UTime		= Hour - LON * 12.0 / M_PI;
	UTime		= Hour;


	//-----------------------------------------------------
	// 1. Julian Date...

	JD			= (int)(365.25 * Year) + (int)(30.6001 * (Month + 1)) - 15 + 1720996.5 + Day + UTime / 24.0;
	T			= (JD - 2451545.0 ) / 36525.0;	// number of Julian centuries since 2000/01/01 at 12 UT (JD = 2451545.0)


	//-----------------------------------------------------
	// 2. Solar Coordinates (according to: Jean Meeus: Astronomical Algorithms), accuracy of 0.01 degree

	M			= M_DEG_TO_RAD * (357.52910 + 35999.05030 * T - 0.0001559 * T * T - 0.00000048 * T * T * T);	// mean anomaly
	L			= M_DEG_TO_RAD * (	(280.46645 + 36000.76983 * T + 0.0003032 * T * T)							// mean longitude
							+	(	(1.914600 - 0.004817 * T - 0.000014  * T * T) * sin(M)
								+	(0.019993 - 0.000101 * T) * sin(2 * M) + 0.000290 * sin(3 * M)				// true longitude
								)
							);


	//-----------------------------------------------------
	// 3. convert ecliptic longitude to right ascension RA and declination delta

	X			= cos(L);
	Y			= cos(ECLIPTIC_OBL) * sin(L);
	Z			= sin(ECLIPTIC_OBL) * sin(L);
	R			= sqrt(1.0 - Z*Z); 

	DEC			= atan2(Z, R);
	RA			= 2.0 * atan2(Y, (X + R));


	//-----------------------------------------------------
	// 4. compute sidereal time (degree) at Greenwich local sidereal time at longitude (Degree)

	theta		= LON + M_DEG_TO_RAD * (280.46061837 + 360.98564736629 * (JD - 2451545.0) + T*T * (0.000387933 - T / 38710000.0));


	//-----------------------------------------------------
	// 5. compute local hour angle (degree)

	tau			= theta - RA;


	//-----------------------------------------------------
	// 6. convert (tau, delta) to horizon coordinates (h, az) of the observer

	Sol_Height	= asin ( sin(LAT) * sin(DEC) + cos(LAT) * cos(DEC) * cos(tau) );

	Sol_Azimuth	= atan2( -sin(tau) * cos(DEC), cos(LAT) * sin(DEC) - sin(LAT) * cos(DEC) * cos(tau) );
//	Sol_Azimuth	= atan2( -sin(Tau), cos(LAT) * tan(DEC) - sin(LAT) * cos(Tau) );	// previous formula gives same result but is better because of division by zero effects...

	return( Sol_Height > 0.0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
