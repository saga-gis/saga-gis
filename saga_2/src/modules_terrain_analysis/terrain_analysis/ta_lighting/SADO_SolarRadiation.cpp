
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                     ta_lighting                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                SADO_SolarRadiation.cpp                //
//                                                       //
//                 Copyright (C) 2006 by                 //
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
#include "SADO_SolarRadiation.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSADO_SolarRadiation::CSADO_SolarRadiation(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name		(_TL("Insolation"));

	Set_Author		(_TL("Copyrights (c) 2006 by Olaf Conrad"));

	Set_Description	(_TL(
		"Calculation of incoming solar radiation (insolation). "
		"Based on the SADO (System for the Analysis of Discrete Surfaces) routines "
		"developed by Boehner & Trachinow. "
		"\n\n"
		"References:\n"
	));

	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL	, "GRD_DEM"			, _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT
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
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		NULL	, "GRD_DIFFUS"		, _TL("Diffuse Insolation"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		NULL	, "GRD_TOTAL"		, _TL("Total Insolation"),
		_TL("Total insolation, the sum of direct and diffuse incoming solar radiation."),
		PARAMETER_OUTPUT_OPTIONAL
	);

	//-----------------------------------------------------
	Parameters.Add_Value(
		NULL	, "BHORIZON"		, _TL("Horizontal Projection"),
		_TL(""),
		PARAMETER_TYPE_Bool			, false
	);

	Parameters.Add_Value(
		NULL	, "SOLARCONST"		, _TL("Solar Constant [J]"),
		_TL(""),
		PARAMETER_TYPE_Double		, 8.164, 0.0, true
	);

	Parameters.Add_Value(
		NULL	, "ATMOSPHERE"		, _TL("Height of Atmosphere [m]"),
		_TL(""),
		PARAMETER_TYPE_Double		, 12000.0, 0.0, true
	);

	Parameters.Add_Value(
		NULL	, "VAPOUR"			, _TL("Water Vapour Pressure [mbar]"),
		_TL("This value is used as constant if no vapour pressure grid is given."),
		PARAMETER_TYPE_Double		, 10, 0.0, true
	);

	//-----------------------------------------------------
	pNode	= Parameters.Add_Node(
		NULL	, "NODE_TIME"		, _TL("Time"),
		""
	);

	Parameters.Add_Choice(
		pNode	, "PERIOD"			, _TL("Time Period"),
		_TL(""),

		CSG_String::Format("%s|%s|%s|%s|",
			_TL("moment"),
			_TL("day"),
			_TL("range of days"),
			_TL("same moment for a range of days")
		), 1
	);

	Parameters.Add_Value(
		pNode	, "DHOUR"			, _TL("Daily Time Resolution [h]"),
		_TL("Time resolution for a day's calculation."),
		PARAMETER_TYPE_Double		, 1.0, 0.0, true, 24.0, true
	);

	Parameters.Add_Value(
		pNode	, "DDAYS"			, _TL("Range of Days Time Resolution [days]"),
		_TL("Time resolution for a range of days calculation."),
		PARAMETER_TYPE_Int			, 1, 1, true
	);

	Parameters.Add_Value(
		pNode	, "DAY_A"			, _TL("Day of Year"),
		_TL(""),
		PARAMETER_TYPE_Int			, 81
	);

	Parameters.Add_Value(
		pNode	, "DAY_B"			, _TL("Day of Year (Range End)"),
		_TL(""),
		PARAMETER_TYPE_Int			, 87
	);

	Parameters.Add_Value(
		pNode	, "MOMENT"			, _TL("Moment [h]"),
		_TL(""),
		PARAMETER_TYPE_Double		, 12.0, 0.0, true, 24.0, true
	);

	//-----------------------------------------------------
	pNode	= Parameters.Add_Node(
		NULL	, "NODE_LOCATION"	, _TL("Location"),
		""
	);

	Parameters.Add_Value(
		pNode	, "LATITUDE"		, _TL("Latitude"),
		_TL(""),
		PARAMETER_TYPE_Degree		, 53.0, -90.0, true, 90.0, true
	);

	Parameters.Add_Value(
		pNode	, "BENDING"			, _TL("Planetery Bending"),
		_TL(""),
		PARAMETER_TYPE_Bool			, false
	);

	Parameters.Add_Value(
		pNode	, "RADIUS"			, _TL("Planetary Radius"),
		_TL(""),
		PARAMETER_TYPE_Double		, 6366737.96, 0.0, true
	);

	Parameters.Add_Choice(
		pNode	, "LAT_OFFSET"		, _TL("Latitude relates to grid's..."),
		_TL(""),

		CSG_String::Format("%s|%s|%s|%s|",
			_TL("bottom"),
			_TL("center"),
			_TL("top"),
			_TL("user defined reference")
		), 3
	);

	Parameters.Add_Choice(
		pNode	, "LON_OFFSET"		, _TL("Local time relates to grid's..."),
		_TL(""),

		CSG_String::Format("%s|%s|%s|%s|",
			_TL("left"),
			_TL("center"),
			_TL("right"),
			_TL("user defined reference")
		), 1
	);

	Parameters.Add_Value(
		pNode	, "LAT_REF_USER"	, _TL("Latitude (user defined reference)"),
		_TL(""),
		PARAMETER_TYPE_Double		, 0.0
	);

	Parameters.Add_Value(
		pNode	, "LON_REF_USER"	, _TL("Local Time (user defined reference)"),
		_TL(""),
		PARAMETER_TYPE_Double		, 0.0
	);

	//-----------------------------------------------------
	pNode	= Parameters.Add_Node(
		NULL	, "NODE_UPDATEVIEW"	, _TL("Update View"),
		""
	);

	Parameters.Add_Value(
		pNode	, "UPD_DIRECT"		, _TL("Direct Insolation"),
		_TL(""),
		PARAMETER_TYPE_Bool			, false
	);

	Parameters.Add_Value(
		pNode	, "UPD_DIFFUS"		, _TL("Diffuse Insolation"),
		_TL(""),
		PARAMETER_TYPE_Bool			, false
	);

	Parameters.Add_Value(
		pNode	, "UPD_TOTAL"		, _TL("Total Insolation"),
		_TL(""),
		PARAMETER_TYPE_Bool			, false
	);
}

//---------------------------------------------------------
CSADO_SolarRadiation::~CSADO_SolarRadiation(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSADO_SolarRadiation::On_Execute(void)
{
	//-----------------------------------------------------
	m_pDEM			= Parameters("GRD_DEM")		->asGrid();
	m_pVP			= Parameters("GRD_VAPOUR")	->asGrid();

	m_pSumDirect	= Parameters("GRD_DIRECT")	->asGrid();
	m_bUpdateDirect	= Parameters("UPD_DIRECT")	->asBool() && m_pSumDirect;
	m_pSumDiffus	= Parameters("GRD_DIFFUS")	->asGrid();
	m_bUpdateDiffus	= Parameters("UPD_DIFFUS")	->asBool() && m_pSumDiffus;
	m_pSumTotal		= Parameters("GRD_TOTAL")	->asGrid();
	m_bUpdateTotal	= Parameters("UPD_TOTAL")	->asBool() && m_pSumTotal;

	m_Solar_Const	= Parameters("SOLARCONST")	->asDouble();
	m_Atmosphere	= Parameters("ATMOSPHERE")	->asDouble();
	m_VP			= Parameters("VAPOUR")		->asDouble();
	m_bHorizon		= Parameters("BHORIZON")	->asBool();

	m_Latitude		= Parameters("LATITUDE")	->asDouble() * M_DEG_TO_RAD;
	m_bBending		= Parameters("BENDING")		->asBool();

	m_Hour			= Parameters("MOMENT")		->asDouble();
	m_dHour			= Parameters("DHOUR")		->asDouble();
	m_dDays			= Parameters("DDAYS")		->asInt();
	m_Day_A			= Parameters("DAY_A")		->asInt();
	m_Day_B			= Parameters("DAY_B")		->asInt();

	switch( Parameters("PERIOD")->asInt() )
	{
	case 0:	// moment
		m_bMoment		= true;
		m_bUpdateDirect	= m_bUpdateDiffus	= m_bUpdateTotal	= false;
		break;

	case 1:	// day
		m_bMoment		= false;
		m_Day_B			= m_Day_A;
		m_Hour			= 0.0;
		break;

	case 2:	// range of days
		m_bMoment		= false;
		m_Hour			= 0.0;
		break;

	case 3:	// same moment for a range of days
		m_bMoment		= false;
		m_dHour			= 24.0;
		break;
	}

	//-----------------------------------------------------
	if( !m_pSumDirect && !m_pSumDiffus && !m_pSumTotal )
	{
		Message_Dlg(_TL("No output grid has been choosen."), Get_Name());
	}
	else if( m_pSumDirect == m_pDEM || m_pSumDiffus == m_pDEM || m_pSumTotal == m_pDEM )
	{
		Message_Dlg(_TL("Output must not overwrite elevation grid."), Get_Name());
	}
	else
	{
		return( Get_Insolation() );
	}

	//-----------------------------------------------------
	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSADO_SolarRadiation::Initialise(void)
{
	int		x, y;

	Process_Set_Text(_TL("initialising..."));

	//-----------------------------------------------------
	CSG_Colors	c(100, SG_COLORS_YELLOW_RED, true);

	c.Set_Ramp(SG_GET_RGB(  0,   0,  64), SG_GET_RGB(255, 159,   0),  0, 50);
	c.Set_Ramp(SG_GET_RGB(255, 159,   0), SG_GET_RGB(255, 255, 255), 50, 99);

	if( m_pSumDirect )
	{
		m_pSumDirect->Assign(0.0);
		m_pSumDirect->Set_Unit(_TL("Joule"));
		DataObject_Set_Colors(m_pSumDirect, c);

		if( m_bUpdateDirect )
		{
			m_TmpDirect.Create(*Get_System(), GRID_TYPE_Float);
			DataObject_Update(m_pSumDirect, true);
		}
	}

	if( m_pSumDiffus )
	{
		m_pSumDiffus->Assign(0.0);
		m_pSumDiffus->Set_Unit(_TL("Joule"));
		DataObject_Set_Colors(m_pSumDiffus, c);

		if( m_bUpdateDiffus )
		{
			m_TmpDiffus.Create(*Get_System(), GRID_TYPE_Float);
			DataObject_Update(m_pSumDiffus, true);
		}
	}

	if( m_pSumTotal )
	{
		m_pSumTotal ->Assign(0.0);
		m_pSumTotal ->Set_Unit(_TL("Joule"));
		DataObject_Set_Colors(m_pSumTotal , c);

		if( m_bUpdateTotal )
		{
			m_TmpTotal.Create(*Get_System(), GRID_TYPE_Float);
			DataObject_Update(m_pSumTotal , true);
		}
	}

	//-----------------------------------------------------
	Process_Set_Text(_TL("initialising gradient..."));

	m_Shade .Create(*Get_System(), GRID_TYPE_Byte);
	m_Slope .Create(*Get_System(), GRID_TYPE_Float);
	m_Aspect.Create(*Get_System(), GRID_TYPE_Float);

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

		CSG_Grid	*pLat	= Parameters("GRD_LAT")->asGrid(),
				*pLon	= Parameters("GRD_LON")->asGrid();

		m_Lat		.Create(*Get_System(), GRID_TYPE_Float);
		m_Lon		.Create(*Get_System(), GRID_TYPE_Float);
		m_Decline	.Create(*Get_System(), GRID_TYPE_Float);
		m_Azimuth	.Create(*Get_System(), GRID_TYPE_Float);

		if( pLat || pLon )
		{
			if( pLat )
			{
				m_Lat	= *pLat;
				m_Lat	*= M_DEG_TO_RAD;
			}

			if( pLon )
			{
				m_Lon	= *pLon;
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
	return( true );
}

//---------------------------------------------------------
bool CSADO_SolarRadiation::Finalise(double SumFactor)
{
	//-----------------------------------------------------
	if( m_pSumDirect )
	{
		if( m_bUpdateDirect )	m_pSumDirect->Assign(&m_TmpDirect);
		if( SumFactor != 1.0 )	*m_pSumDirect	*= SumFactor;
	}

	if( m_pSumDiffus )
	{
		if( m_bUpdateDiffus )	m_pSumDiffus->Assign(&m_TmpDiffus);
		if( SumFactor != 1.0 )	*m_pSumDiffus	*= SumFactor;
	}

	if( m_pSumTotal )
	{
		if( m_bUpdateTotal )	m_pSumTotal->Assign(&m_TmpTotal);
		if( SumFactor != 1.0 )	*m_pSumTotal	*= SumFactor;
	}

	//-----------------------------------------------------
	m_Shade		.Destroy();
	m_Slope		.Destroy();
	m_Aspect	.Destroy();

	m_Lat		.Destroy();
	m_Lon		.Destroy();
	m_Decline	.Destroy();
	m_Azimuth	.Destroy();

	m_TmpDirect	.Destroy();
	m_TmpDiffus	.Destroy();
	m_TmpTotal	.Destroy();

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSADO_SolarRadiation::Get_Insolation(void)
{
	//-----------------------------------------------------
	if( Initialise() )
	{
		if( m_bMoment )
		{
			Get_Insolation(m_Day_A, m_Hour);

			Finalise();
		}

		//-------------------------------------------------
		else
		{
			for(int Day=m_Day_A; Day<=m_Day_B && Process_Get_Okay(false); Day+=m_dDays)
			{
				for(double Hour=m_Hour; Hour<24.0 && Process_Get_Okay(false); Hour+=m_dHour)
				{
					Process_Set_Text(CSG_String::Format("%s: %d(%d-%d), %s: %f", _TL("Day"), Day, m_Day_A, m_Day_B, _TL("Hour"), Hour));

					if( m_bUpdateDirect )	m_pSumDirect->Assign(0.0);
					if( m_bUpdateDiffus )	m_pSumDiffus->Assign(0.0);
					if( m_bUpdateTotal )	m_pSumTotal ->Assign(0.0);

					if( Get_Insolation(Day, Hour) )
					{
						if( m_bUpdateDirect )
						{
							m_TmpDirect	+= *m_pSumDirect;
							DataObject_Update(m_pSumDirect);
						}

						if( m_bUpdateDiffus )
						{
							m_TmpDiffus	+= *m_pSumDiffus;
							DataObject_Update(m_pSumDiffus);
						}

						if( m_bUpdateTotal )
						{
							m_TmpTotal	+= *m_pSumTotal;
							DataObject_Update(m_pSumTotal);
						}
					}
				}
			}

			Finalise(m_dHour / (24.0 * (1 + m_Day_B - m_Day_A)));	// *m_pSumDirect	*= m_dHour / D->size();
		}
	}

	//-----------------------------------------------------
	return( true );
}

//---------------------------------------------------------
bool CSADO_SolarRadiation::Get_Insolation(int Day, double Hour)
{
	double	Azimuth, Decline;

	if( m_bBending )
	{
		bool	bGo	= false;

		for(int y=0; y<Get_NY() && Set_Progress(y); y++)
		{
			for(int x=0; x<Get_NX(); x++)
			{
				Get_Solar_Position(Day, Hour, m_Lat.asDouble(x, y), m_Lon.asDouble(x, y), Decline, Azimuth);

				m_Azimuth.Set_Value(x, y, Azimuth);
				m_Decline.Set_Value(x, y, Decline);

				if( Decline > 0.0 )
				{
					bGo	= true;
				}
			}
		}

		if( bGo )
		{
			return( Set_Insolation(Decline, Azimuth) );
		}
	}
	else if( Get_Solar_Position(Day, Hour, m_Latitude, 0.0, Decline, Azimuth) )
	{
		return( Set_Insolation(Decline, Azimuth) );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline double CSADO_SolarRadiation::Get_Vapour_Exponent(double VapourPressure)
{
	return( 0.916  - (VapourPressure > 0.0 ? 0.05125 * sqrt(VapourPressure) : 0.0) );
}

//---------------------------------------------------------
inline double CSADO_SolarRadiation::Get_Vapour_A(double VapourPressure)
{
	return( 0.4158 + (VapourPressure > 0.0 ? 0.0399  * sqrt(VapourPressure) : 0.0) );
}

//---------------------------------------------------------
bool CSADO_SolarRadiation::Set_Insolation(double Decline, double Azimuth)
{
	int		x, y;
	double	a, e, Direct, Diffus;

	Get_Shade(Decline, Azimuth);

	e	= Get_Vapour_Exponent	(m_VP);
	a	= Get_Vapour_A			(m_VP);

	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			if( m_pDEM->is_NoData(x, y) )
			{
				if( m_pSumDirect )	m_pSumDirect->Set_NoData(x, y);
				if( m_pSumDiffus )	m_pSumDiffus->Set_NoData(x, y);
				if( m_pSumTotal )	m_pSumTotal ->Set_NoData(x, y);
			}
			else
			{
				if( m_pVP != NULL )
				{
					e	= Get_Vapour_Exponent	(m_pVP->asDouble(x, y));
					a	= Get_Vapour_A			(m_pVP->asDouble(x, y));
				}

				if( m_bBending )
				{
					Azimuth	= m_Azimuth.asDouble(x, y);
					Decline	= m_Decline.asDouble(x, y);
				}

				if( Decline > 0.0 )
				{
					Direct	= Get_Solar_Direct(x, y, Decline, Azimuth, e);
					Diffus	= Get_Solar_Diffus(x, y, Decline, a      , e);

					if( m_pSumDirect )	m_pSumDirect->Add_Value(x, y, Direct);
					if( m_pSumDiffus )	m_pSumDiffus->Add_Value(x, y, Diffus);
					if( m_pSumTotal  )	m_pSumTotal ->Add_Value(x, y, Direct + Diffus);
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
inline double CSADO_SolarRadiation::Get_Solar_Reduction(double Elevation, double Decline, double Reduction)
{
	return( Decline > 0.0 ? (m_Solar_Const * pow(Reduction, (1.0 - Elevation / m_Atmosphere) / sin(Decline))) : 0.0 );
}

//---------------------------------------------------------
inline double CSADO_SolarRadiation::Get_Solar_Direct(int x, int y, double Decline, double Azimuth, double Exponent)
{
	if( m_Shade.asInt(x, y) == 0 )
	{
		double	Angle, Slope;

		Slope	= m_bHorizon ? 0.0 : m_Slope.asDouble(x, y);

		Angle	= cos(Slope) * cos(Decline - M_PI_090)
				+ sin(Slope) * sin(M_PI_090 - Decline)
				* cos(Azimuth - m_Aspect.asDouble(x, y));

		if( Angle > 0.0 )
		{
			return( Angle * Get_Solar_Reduction(m_pDEM->asDouble(x, y), Decline, Exponent) );
		}
	}

	return( 0.0 );
}

//---------------------------------------------------------
inline double CSADO_SolarRadiation::Get_Solar_Diffus(int x, int y, double Decline, double A, double Exponent)
{
	double	Dh, Ih, d, Slope, srad;

	Ih		= Get_Solar_Reduction(m_pDEM->asDouble(x, y), Decline, Exponent) * sin(Decline);
//	Decline	*= M_RAD_TO_DEG;	// Fehler in SADO ????!!!!!
	d		= m_Atmosphere / (m_Atmosphere - m_pDEM->asDouble(x, y))
			* (0.0001165 * Decline*Decline - 0.0152 * Decline + A);
	d		= 1.0 - d;
	Dh		= Ih / d - Ih;

	Slope	= m_bHorizon ? 0.0 : m_Slope.asDouble(x, y);

	srad	= (Dh + Dh * cos(Slope)) / 2.0;

	return( srad < 0.0 || srad > m_Solar_Const ? m_Solar_Const : srad );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSADO_SolarRadiation::Get_Shade(double Decline, double Azimuth)
{
	//-----------------------------------------------------
	m_Shade.Assign(0.0);

	if( !m_bBending )
	{
		int		i, x, y;
		double	dx, dy, dz;

		Get_Shade_Params(Decline, Azimuth, dx, dy, dz);

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
void CSADO_SolarRadiation::Set_Shade(int x, int y, double dx, double dy, double dz)
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
void CSADO_SolarRadiation::Set_Shade_Bended(int x, int y, char iLock)
{
	double	dx, dy, dz;

	Get_Shade_Params(m_Decline.asDouble(x, y), m_Azimuth.asDouble(x, y), dx, dy, dz);

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

		Get_Shade_Params(m_Decline.asDouble(x, y), m_Azimuth.asDouble(x, y), dx, dy, dz);
	}
}

//---------------------------------------------------------
inline bool CSADO_SolarRadiation::Get_Shade_Complete(int x, int y)
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
inline void CSADO_SolarRadiation::Get_Shade_Params(double Decline, double Azimuth, double &dx, double &dy, double &dz)
{
	dz	= Azimuth + M_PI_180;
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

	dz	= tan(Decline) * sqrt(dx*dx + dy*dy) * Get_Cellsize();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline bool CSADO_SolarRadiation::Get_Solar_Position(int Day, double Hour, double LAT, double LON, double &Declination, double &Azimuth)
{
	static const int	Day2Month[13]	= {	0, 31, 49, 80, 109, 140, 170, 201, 232, 272, 303, 333, 366 };

	static const double	ECLIPTIC_OBL	= M_DEG_TO_RAD * 23.43999;	// obliquity of ecliptic

	int		i;

	double	JD, T, M, L, X, Y, Z, R, UTime,
			DEC, RA, theta, tau,
			Month, Year		= 2000;

	//-----------------------------------------------------

	for(Month=1, i=0; i<=12; i++)
	{
		if( Day < Day2Month[i] )
		{
			Month	= i;
			Day		-= Day2Month[i - 1];
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
	L			= M_DEG_TO_RAD * (	(280.46645 + 36000.76983 * T + 0.0003032 * T * T)						// mean longitude
							+	(	(1.914600 - 0.004817 * T - 0.000014  * T * T) * sin(M)
								+	(0.019993 - 0.000101 * T) * sin(2 * M) + 0.000290 * sin(3 * M)		// true longitude
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

	Declination	= asin ( sin(LAT) * sin(DEC) + cos(LAT) * cos(DEC) * cos(tau) );

	Azimuth		= atan2( -sin(tau) * cos(DEC), cos(LAT) * sin(DEC) - sin(LAT) * cos(DEC) * cos(tau) );
//	Azimuth		= atan2( -sin(Tau), cos(LAT) * tan(DEC) - sin(LAT) * cos(Tau) );	// previous formula gives same result but is better because of division by zero effects...

	return( Declination > 0.0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

/*/---------------------------------------------------------
#define CHECK_SHADOW(X, Y)	if( (d = SG_Get_Distance(x, y, X, Y) * Get_Cellsize()) > 0.0 )\
							{\
								if( (t = (m_pDEM->asDouble(X, Y) - z) / d) > tMax )\
								{\
									tMax	= t;\
								}\
								\
								if( (d = zMax / d) < tanDec || d < tMax )\
								{\
									return( false );\
								}\
								else if( tMax > tanDec )\
								{\
									return( true );\
								}\
							}

//---------------------------------------------------------
bool CSADO_SolarRadiation::Get_Solar_Shadow(int x, int y, double Decline, double Azimuth)
{
	static double	Zenith	= 89.9999 * M_DEG_TO_RAD;

	//-----------------------------------------------------
	if( Decline >= Zenith || Decline > m_Slope.Get_ZMax() )
	{
		return( false );
	}

	if( Decline <= 0.0 )
	{
		return( true );
	}

	//-----------------------------------------------------
	bool	bGo;
	int		dx, nx, ix, iy, iy_Last, jy;
	double	dy, d, t, tMax, z, zMax, tanDec;

	//-----------------------------------------------------
	Azimuth	= fmod(Azimuth, M_PI_360);	if( Azimuth < 0.0 )	Azimuth	+= M_PI_360;

	if( Azimuth < M_PI_180 )
	{
		dx	= 1;
		nx	= Get_NX() - 1;
	}
	else
	{
		dx	= -1;
		nx	= 0;
	}

	Azimuth	= M_PI_090 - Azimuth;

	if(		 Azimuth	==  M_PI_090 )
			 Azimuth	=   89.9999 * M_DEG_TO_RAD;
	else if( Azimuth	== -M_PI_090 )
			 Azimuth	= - 89.9999 * M_DEG_TO_RAD;
	else if( Azimuth	== -M_PI_180 )
			 Azimuth	= -179.9999 * M_DEG_TO_RAD;
	else if( Azimuth	== -M_PI_270)
			 Azimuth	= -269.9999 * M_DEG_TO_RAD;

	dy		= tan(Azimuth);

	//-----------------------------------------------------
	z		= m_pDEM->asDouble(x, y);
	zMax	= m_pDEM->Get_ZMax();
	tanDec	= tan(Decline);
	tMax	= 0.0;
  
	//-----------------------------------------------------
	for(ix=x, iy=y, bGo=true; ix!=nx && bGo; ix+=dx)
	{
		iy_Last	= iy;
		iy		= y + (int)floor(dy * (ix - x + (dx > 0 ? 0.5 : -0.5)));

		if( iy < 0 )
		{
			bGo	= false;
			iy	= 0;
		}
		else if( iy >= Get_NY() )
		{
			bGo	= false;
			iy	= Get_NY() - 1;
		}

		//-------------------------------------------------
		if( iy_Last < iy )
		{
			for(jy=iy_Last+1; jy<=iy; jy++)
			{
				CHECK_SHADOW(ix, jy);
			}
		}
		else if( iy_Last > iy )
		{
			for(jy=iy_Last; jy>iy; jy--)
			{
				CHECK_SHADOW(ix, jy);
			}
		}
		else if( iy_Last == iy ) 
		{
			CHECK_SHADOW(ix, iy);
		}
	}

	return( false );
}/**/


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
