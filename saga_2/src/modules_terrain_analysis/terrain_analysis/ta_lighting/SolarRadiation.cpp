
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
#include "HillShade.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSolarRadiation::CSolarRadiation(void)
{
	CSG_Parameter	*pNode_0, *pNode_1, *pNode_2;

	//-----------------------------------------------------
	Set_Name(_TL("Incoming Solar Radiation"));

	Set_Author		(SG_T("(c) 2001 by O.Conrad"));

	Set_Description	(_TW(
		"Calculation of the potential incoming solar radiation.\n\n"
		"Reference:\n"
		"- Wilson, J.P., Gallant, J.C., (Eds.), 2000:\n"
		"    'Terrain analysis - principles and applications',\n"
		"    New York, John Wiley & Sons, Inc.")
	);


	//-----------------------------------------------------
	pNode_0	= Parameters.Add_Grid(
		NULL	, "ELEVATION"	, _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT
	);


	//-----------------------------------------------------
	pNode_0	= Parameters.Add_Grid(
		NULL	, "INSOLAT"		, _TL("Solar Radiation"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	pNode_1	= Parameters.Add_Value(
		pNode_0	, "UPDATE"		, _TL("Update View"),
		_TL("Show each time step during calculation."),
		PARAMETER_TYPE_Bool		, true
	);

	pNode_1	= Parameters.Add_Choice(
		pNode_0	, "UNIT"		, _TL("Unit"),
		_TL(""),

		CSG_String::Format(SG_T("%s|%s|"),
			_TL("kWh/m\xc2\xb2"),
			_TL("J/m\xc2\xb2")
		), 0
	);

	Parameters.Add_Grid(
		NULL	, "DURATION"	, _TL("Duration of Insolation"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		NULL	, "SUNRISE"		, _TL("Sunrise"),
		_TL("time of sunrise/sunset is only calculated if time span is set to single day"),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		NULL	, "SUNSET"		, _TL("Sunset"),
		_TL("time of sunrise/sunset is only calculated if time span is set to single day"),
		PARAMETER_OUTPUT_OPTIONAL
	);


	//-----------------------------------------------------
	pNode_0	= Parameters.Add_Node(
		NULL	, "NODE_SOLAR"	, _TL("Solar Radiation"),
		_TL("")
	);

	pNode_1	= Parameters.Add_Value(
		pNode_0	, "SOLCONST"	, _TL("Solar Constant [W/m\xc2\xb2]"),
		_TL(""),
		PARAMETER_TYPE_Double	, 1367
	);

	pNode_1	= Parameters.Add_Choice(
		pNode_0	, "METHOD"		, _TL("Atmospheric Effects"),
		_TL("Account for atmospheric effects using either a lumped atmospheric transmittance approach or by calculating the components (water and dust)."),

		CSG_String::Format(SG_T("%s|%s|"),
			_TL("Lumped atmospheric transmittance"),
			_TL("Calculating the components")
		), 0
	);

	pNode_1	= Parameters.Add_Value(
		pNode_0	, "TRANSMITT"	, _TL("Lumped Transmittance [%]"),
		_TL("The transmittance of the atmosphere (usually between 60 and 80%)"),
		PARAMETER_TYPE_Double	, 70,
		0.0		, true,
		100.0	, true
	);

	pNode_1	= Parameters.Add_Value(
		pNode_0	, "PRESSURE"	, _TL("Atmospheric Pressure [mb]"),
		_TL(""),
		PARAMETER_TYPE_Double	, 1013,
		0.0		, true
	);

	pNode_1	= Parameters.Add_Value(
		pNode_0	, "WATER"		, _TL("Water Content [cm]"),
		_TL("Water content of a vertical slice of atmosphere in cm: 1.5 to 1.7, average=1.68"),
		PARAMETER_TYPE_Double	, 1.68,
		0.0		, true
	);

	pNode_1	= Parameters.Add_Value(
		pNode_0	, "DUST"		, _TL("Dust [ppm]"),
		_TL("Dust factor: 100 ppm (standard)"),
		PARAMETER_TYPE_Double	, 100,
		0.0		, true
	);

	pNode_1	= Parameters.Add_Value(
		pNode_0	, "LATITUDE"	, _TL("Latitude [Degree]"),
		_TL(""),
		PARAMETER_TYPE_Double	, 53.5,
		-90.0	, true,
		90.0	, true
	);


	//-----------------------------------------------------
	// Daily Time Resolution...

	pNode_0	= Parameters.Add_Node(
		NULL	, "NODE_HOUR"	, _TL("Daily Time Resolution"),
		_TL("")
	);

	pNode_1	= Parameters.Add_Range(
		pNode_0	, "HOUR_RANGE"	, _TL("Time Span [h]"),
		_TL("Time span used for the calculation of daily radiation sums."),
		 0.0	, 24.0,
		 0.0	, true,
		24.0	, true
	);

	pNode_1	= Parameters.Add_Value(
		pNode_0	, "HOUR_STEP"	, _TL("Time Step [h]"),
		_TL("Time step size used for the calculation of daily radiation sums."),
		PARAMETER_TYPE_Double	, 1.0,
		0.001	, true,
		12.0	, true
	);


	//-----------------------------------------------------
	// Time Settings...

	pNode_0	= Parameters.Add_Choice(
		NULL	, "TIMESPAN"	, _TL("Simulation Time"),
		_TL(""),

		CSG_String::Format(SG_T("%s|%s|%s|"),
			_TL("Single Day"),
			_TL("Range of Days"),
			_TL("One Year")
		), 0
	);

	//-----------------------------------------------------
	pNode_1	= Parameters.Add_Node(
		pNode_0	, "NODE_SINGLE_DAY"		, _TL("Single Day"),
		_TL("")
	);

	pNode_2	= Parameters.Add_Choice(
		pNode_1	, "SINGLE_DAY_DAY"		, _TL("Day"),
		_TL(""),

		SG_T("  1|  2|  3|  4|  5|  6|  7|  8|  9| 10|")
		SG_T(" 11| 12| 13| 14| 15| 16| 17| 18| 19| 20|")
		SG_T(" 21| 22| 23| 24| 25| 26| 27| 28| 29| 30|")
		SG_T(" 31|"), 20
	);

	pNode_2	= Parameters.Add_Choice(
		pNode_1	, "SINGLE_DAY_MONTH"	, _TL("Month"),
		_TL(""),

		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|"),
			_TL("January"),
			_TL("February"),
			_TL("March"),
			_TL("April"),
			_TL("May"),
			_TL("June"),
			_TL("July"),
			_TL("August"),
			_TL("September"),
			_TL("October"),
			_TL("November"),
			_TL("December")
		), 2
	);


	//-----------------------------------------------------
	// Range of Days...

	pNode_1	= Parameters.Add_Node(
		pNode_0	, "NODE_DAY_RANGE"		, _TL("Range of Days"),
		_TL("")
	);

	pNode_2	= Parameters.Add_Range(
		pNode_1	, "DAY_RANGE"			, _TL("Time Span [day of year]"),
		_TL(""),
		   1	, 31,
		-365	, true,
		 366	, true
	);

	pNode_2	= Parameters.Add_Value(
		pNode_1	, "DAY_STEP"			, _TL("Time Step [number of days]"),
		_TL(""),
		PARAMETER_TYPE_Int, 5,
		1		, true,
		100		, true
	);
}

//---------------------------------------------------------
CSolarRadiation::~CSolarRadiation(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSolarRadiation::On_Execute(void)
{
	static const int	Month2Day[12]	= {	0, 31, 49, 80, 109, 140, 170, 201, 232, 272, 303, 333 };

	//-----------------------------------------------------
	int			Day_Step, Day_Start, Day_Stop;
	double		Latitude, Hour_Step, Hour_Start, Hour_Stop;
	CSG_Colors	Colors;

	//-----------------------------------------------------
	m_pDTM			= Parameters("ELEVATION")	->asGrid();
	m_pRadiation	= Parameters("INSOLAT")		->asGrid();
	m_pDuration		= Parameters("DURATION")	->asGrid();
	m_pSunrise		= Parameters("SUNRISE")		->asGrid();
	m_pSunset		= Parameters("SUNSET")		->asGrid();

	m_SolarConstant	= Parameters("SOLCONST")	->asDouble() / 1000.0;	// [kW / m²]

	m_Method		= Parameters("METHOD")		->asInt();
	m_Transmittance	= Parameters("TRANSMITT")	->asDouble() / 100.0;
	m_Pressure		= Parameters("PRESSURE")	->asDouble();
	m_Water			= Parameters("WATER")		->asDouble();
	m_Dust			= Parameters("DUST")		->asDouble();

	Latitude		= Parameters("LATITUDE")	->asDouble() * M_DEG_TO_RAD;

	Day_Step		= Parameters("DAY_STEP")	->asInt();
	Hour_Step		= Parameters("HOUR_STEP")	->asDouble();
	Hour_Start		= Parameters("HOUR_RANGE")	->asRange()->Get_LoVal();
	Hour_Stop		= Parameters("HOUR_RANGE")	->asRange()->Get_HiVal();

	//-----------------------------------------------------
	m_pSum			= SG_Create_Grid(m_pDTM);

	//-----------------------------------------------------
	switch( Parameters("TIMESPAN")->asInt() )
	{
	case 0:	// single day...
		Day_Start		= Parameters("SINGLE_DAY_MONTH")->asInt();
		Day_Start		= Parameters("SINGLE_DAY_DAY")	->asInt() + (Day_Start >= 0 && Day_Start < 12 ? Month2Day[Day_Start] : 0);

		if( m_pSunrise )	{	m_pSunrise->Assign_NoData();	DataObject_Set_Colors(m_pSunrise, 100, SG_COLORS_YELLOW_RED, false);	}
		if( m_pSunset  )	{	m_pSunset ->Assign_NoData();	DataObject_Set_Colors(m_pSunset , 100, SG_COLORS_YELLOW_RED, true );	}

		Execute_DailySum (Latitude, Hour_Step, Hour_Start, Hour_Stop, Day_Start);
		break;

	case 1:	// range of days...
		Day_Start		= (int)Parameters("DAY_RANGE")	->asRange()->Get_LoVal() - 1;
		Day_Stop		= (int)Parameters("DAY_RANGE")	->asRange()->Get_HiVal() - 1;

		Execute_SumOfDays(Latitude, Hour_Step, Hour_Start, Hour_Stop, Day_Step, Day_Start, Day_Stop);
		break;

	case 2:	// one year...
		Day_Start		= -10;	// winter solstitute...
		Day_Stop		= 173;	// until half year has passed...

		Execute_SumOfDays(Latitude, Hour_Step, Hour_Start, Hour_Stop, Day_Step, Day_Start, Day_Stop);

		*m_pRadiation	*= 2.0;	// two times...
		*m_pDuration	*= 2.0;	// two times...
		break;
	}

	//-----------------------------------------------------
	delete( m_pSum );

	Colors.Set_Count(100);
	Colors.Set_Ramp(SG_GET_RGB(  0,   0,  64), SG_GET_RGB(255, 159,   0),  0, 50);
	Colors.Set_Ramp(SG_GET_RGB(255, 159,   0), SG_GET_RGB(255, 255, 255), 50, 99);

	DataObject_Set_Colors(m_pRadiation, Colors);
	DataObject_Set_Colors(m_pDuration , Colors);

	m_pDuration->Set_Unit(_TL("h"));

	if( Parameters("UNIT")->asInt() == 1 )	// Joule...
	{
		*m_pRadiation	*= 10.0 / 36.0;	// 1 J = 1 Ws = 1/(60*60) Wh = 1/3600 Wh >> 1 J = 1000/3600 kWh = 10/36 kWh

		m_pRadiation->Set_Unit(_TL("J/m\xc2\xb2"));
	}
	else
	{
		m_pRadiation->Set_Unit(_TL("kWh/m\xc2\xb2"));
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
void CSolarRadiation::Execute_DailySum(double Latitude_RAD, double Hour_Step, double Hour_Start, double Hour_Stop, int Day)
{
	Get_DailySum(Latitude_RAD, Hour_Step, Hour_Start, Hour_Stop, Day);

	m_pRadiation->Assign(m_pSum);
}

//---------------------------------------------------------
void CSolarRadiation::Execute_SumOfDays(double Latitude_RAD, double Hour_Step, double Hour_Start, double Hour_Stop, int Day_Step, int Day_Start, int Day_Stop)
{
	int		Day, nDays;
	CSG_Grid	*pSumOfDays, *pDur_Total;

	//-----------------------------------------------------
	nDays	= Day_Stop - Day_Start;

	if( nDays < 1 )
	{
		Execute_DailySum(Latitude_RAD, Hour_Step, Hour_Start, Hour_Stop, Day_Start);
	}

	//-----------------------------------------------------
	else
	{
		pSumOfDays		= SG_Create_Grid(m_pDTM);
		pDur_Total		= SG_Create_Grid(m_pDTM);

		if( Day_Step < 1 || Day_Step > nDays )
		{
			Day_Step		= 1;
		}

		//-------------------------------------------------
		for(Day=Day_Start; Day<Day_Stop && Set_Progress(Day - Day_Start, nDays); Day+=Day_Step)
		{
			Get_DailySum(Latitude_RAD, Hour_Step, Hour_Start, Hour_Stop, Day, false);

			*m_pSum			*= Day_Step;
			*pSumOfDays		+= *m_pSum;

			*m_pDuration	*= Day_Step;
			*pDur_Total		+= *m_pDuration;
		}

		if( (Day_Step = Day_Stop - Day) > 0 )	// Rest...
		{
			Get_DailySum(Latitude_RAD, Hour_Step, Hour_Start, Hour_Stop, Day, false);

			*m_pSum			*= Day_Step;
			*pSumOfDays		+= *m_pSum;

			*m_pDuration	*= Day_Step;
			*pDur_Total		+= *m_pDuration;
		}

		//-------------------------------------------------
		m_pRadiation	->Assign(pSumOfDays);
		delete(pSumOfDays);

		m_pDuration		->Assign(pDur_Total);
		delete(pDur_Total);
	}
}


///////////////////////////////////////////////////////////
//														 //
//						Daily Sum						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSolarRadiation::Get_DailySum(double Latitude_RAD, double Hour_Step, double Hour_Start, double Hour_Stop, int Day, bool bProgressBar)
{
	bool		bNight;
	double		time, Sol_Height, Sol_Azimuth, Solar_Angle, RDIRN, RDIFN, d;
	CHillShade	HillShade;

	//-----------------------------------------------------
	Day			%= 366;

	if( Day < 0 )
	{
		Day		+= 366;
	}

	Hour_Start	+= Hour_Step / 2.0;

	m_pSum		->Assign(0.0);
	m_pDuration	->Assign(0.0);

	HillShade.Get_Parameters()->Set_Parameter(SG_T("ELEVATION")		, PARAMETER_TYPE_Grid	, m_pDTM);
	HillShade.Get_Parameters()->Set_Parameter(SG_T("SHADE")			, PARAMETER_TYPE_Grid	, m_pRadiation);
	HillShade.Get_Parameters()->Set_Parameter(SG_T("METHOD")		, PARAMETER_TYPE_Choice	, 3);
	HillShade.Get_Parameters()->Set_Parameter(SG_T("EXAGGERATION")	, PARAMETER_TYPE_Double	, 1.0);
	HillShade.Set_Show_Progress(false);

	//-----------------------------------------------------
	for(time=Hour_Start, bNight=false; time<Hour_Stop && is_Progress(); time+=Hour_Step)
	{
		Process_Set_Text(CSG_String::Format(_TL("%d. Day of Year, Local Time: %dh %02dm"), Day + 1, (int)time, (int)(60.0 * fmod(time, 1.0))));

		if( bProgressBar )
		{
			Set_Progress(time, 24.0);
		}
		else
		{
			Process_Get_Okay(false);
		}

		//-------------------------------------------------
		if( Get_SolarPosition(Day, time, Latitude_RAD, 0.0, Sol_Azimuth, Sol_Height, false) )
		{
			bNight	= false;

			HillShade.Get_Parameters()->Set_Parameter(SG_T("AZIMUTH")		, PARAMETER_TYPE_Double, (double)M_RAD_TO_DEG * Sol_Azimuth);
			HillShade.Get_Parameters()->Set_Parameter(SG_T("DECLINATION")	, PARAMETER_TYPE_Double, (double)M_RAD_TO_DEG * Sol_Height);
			HillShade.Execute();

			for(int n=0; n<Get_NCells(); n++)
			{
				if( m_pRadiation->is_NoData(n) )
				{
					m_pSum		->Set_NoData(n);
					m_pDuration	->Set_NoData(n);
				}
				else
				{
					Solar_Angle	= m_pRadiation->asDouble(n);

					if( Solar_Angle < M_PI_090 )
					{
						Get_SolarCorrection(M_PI_090 - Sol_Height, m_pDTM->asDouble(n), RDIRN, RDIFN);

						d	= m_SolarConstant * Hour_Step * RDIRN * cos(Solar_Angle);
						// Dir[q,a] = SConst * SunDuration[q,a] * t^m(q) * cos(AngIn[q,a]) 

						m_pSum		->Add_Value(n, d);
						m_pDuration	->Add_Value(n, Hour_Step);

						if( m_pSunrise && m_pSunrise->is_NoData(n) )
						{
							m_pSunrise->Set_Value(n, time);
						}

						if( m_pSunset && !m_pSunset->is_NoData(n) )
						{
							m_pSunset->Set_NoData(n);
						}
					}
					else
					{
						if( m_pSunset && m_pSunset->is_NoData(n) && m_pSum->asDouble(n) > 0.0 )
						{
							m_pSunset->Set_Value(n, time);
						}
					}
				}
			}

			if( Parameters("UPDATE")->asBool() )
			{
				DataObject_Update(m_pRadiation, 20 * M_DEG_TO_RAD, 90.0 * M_DEG_TO_RAD, true);
			}
		}

		//-------------------------------------------------
		else if( !bNight )
		{
			bNight	= true;

			if( Parameters("UPDATE")->asBool() )
			{
				m_pRadiation->Assign(M_PI_090);

				DataObject_Update(m_pRadiation, 0, 90.0 * M_DEG_TO_RAD, true);
			}

			if( m_pSunset )
			{
				for(int n=0; n<Get_NCells(); n++)
				{
					if( m_pSunset->is_NoData(n) && m_pSum->asDouble(n) > 0.0 )
					{
						m_pSunset->Set_Value(n, time);
					}
				}
			}
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//						Atmosphere						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSolarRadiation::Get_SolarCorrection(double ZenithAngle, double Elevation, double &RDIRN, double &RDIFN)
{
	const double	AM[32]	=	// AM    Optical air mass in 1 degree increments for zenith angles >=60 [after LIST, 1968; p. 422]
	{
		 2.00,  2.06,  2.12,  2.19,  2.27,  2.36,  2.45,  2.55,
		 2.65,  2.77,  2.90,  3.05,  3.21,  3.39,  3.59,  3.82,
		 4.07,  4.37,  4.72,  5.12,  5.60,  6.18,  6.88,  7.77,
		 8.90, 10.39, 12.44, 15.36, 19.79, 26.96, 26.96, 26.96
	};

	const double	Pressure_0	= 1013.0;	// Standard atmospheric pressure = 1013 mb

	int		i;
	double	z, AMASS, AMASS2, AW, TW, TD, TDC;

	//-----------------------------------------------------
	z		= M_RAD_TO_DEG * ZenithAngle;

	if( z <= 60.0 )
	{
		AMASS	= 1.0 / cos(ZenithAngle);
	}
	else
	{
		z		-= 60.0;
		i		= (int)z;
		AMASS	= AM[i] + (z - i) * (AM[i + 1] - AM[i]);
	}

	z		= m_Pressure / pow(10.0, Elevation * 5.4667E-05);	// P     Barometric pressure in mb
	AMASS2	= AMASS * z / Pressure_0;

	//-----------------------------------------------------
	switch( m_Method )
	{
	case 0:	default:
		RDIRN	= pow(m_Transmittance, AMASS2);
		RDIFN	= 0.271 - 0.294 * RDIRN;
		break;

	case 1:
		AW		= 1.0 - 0.077 * pow(m_Water * AMASS2, 0.3);		// AW    Accounts for absorption by water vapour
		TW		= pow(0.975, m_Water * AMASS);					// TW    Accounts for scattering by water vapour
		TD		= pow(0.950, m_Water * m_Dust / 100.0);			// TD    Accounts for scattering by dust
		TDC		= pow(0.900, AMASS2) + 0.026 * (AMASS2 - 1.0);	// TDC   Accounts for scattering by a dust free atmosphere

		RDIRN	= AW * TW * TD * TDC;
		RDIFN	= 0.5 * (AW - RDIRN);
		break;
	}

	// Optical Pathlength (solar zenith angle < 80Degree) =>
	// m(t)		= exp(-0.000118 * Elevation - 1.638 * 10e-9 * Elevation) / cos(SolarZenithAngle)
	// if( Method == 2 && Zenithangle < 80 )	// nach bla...
	// RDIRN	= pow(m_Transmittance, exp((-0.000118 - 1.638e-8 * Elevation) * Elevation) / cos(ZenithAngle));
}

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
//					Solar Position						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSolarRadiation::Get_SolarPosition(int Day, double Time, double LAT, double LON, double &Azimuth, double &Declination, bool bDegree)
{
	static const int	Day2Month[13]	= {	0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 366 };

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

//	UTime		= Time - LON * 12.0 / M_PI;
	UTime		= Time;


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
	RA			= 2 * atan2(Y, (X + R));


	//-----------------------------------------------------
	// 4. compute sidereal time (degree) at Greenwich local sidereal time at longitude (DegreeE)

	theta		= LON + M_DEG_TO_RAD * (280.46061837 + 360.98564736629 * (JD - 2451545.0) + T * T * (0.000387933 - T / 38710000.0));


	//-----------------------------------------------------
	// 5. compute local hour angle (degree)

	tau			= theta - RA;


	//-----------------------------------------------------
	// 6. convert (tau, delta) to horizon coordinates (h, az) of the observer

	Declination	= asin( sin(LAT) * sin(DEC) + cos(LAT) * cos(DEC) * cos(tau) );
	Azimuth		= atan2( -sin(tau) * cos(DEC), cos(LAT) * sin(DEC) - sin(LAT) * cos(DEC) * cos(tau) );
	//Azimuth	= atan2( -sin(Tau), cos(LAT) * tan(DEC) - sin(LAT) * cos(Tau) );	// erstere ist besser wegen division by zero effects...

	if( bDegree )
	{
		Declination	*= M_RAD_TO_DEG;
		Azimuth		*= M_RAD_TO_DEG;
	}

	return( Declination >= 0.0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
