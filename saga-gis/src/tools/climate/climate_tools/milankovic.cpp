/**********************************************************
 * Version $Id: milankovic.cpp 911 2011-02-14 16:38:15Z reklov_w $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                       Lectures                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    milankovic.cpp                     //
//                                                       //
//                 Copyright (C) 2012 by                 //
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "milankovic.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define SG_NINT(x)	((int)(x < 0.0 ? x - 0.5 : x + 0.5))


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CSG_Solar_Position
{
public:
	CSG_Solar_Position(void);
	CSG_Solar_Position(long Year);


	static bool			Get_Orbital_Parameters	(int Year, double &Eccentricity, double &Obliquity, double &Perihelion);
	
	bool				Get_Orbital_Position	(double Day, double &SIND, double &COSD, double &SUNDIS, double &SUNLON, double &SUNLAT, double &EQTIME);

	bool				Set_Solar_Constant		(double SolarConst);
	double				Get_Solar_Constant		(void)		{	return( m_SolarConst );	}

	bool				Set_Year				(int Year);
	int					Get_Year				(void)		{	return( m_Year );	}

	bool				Set_Day					(double Day);
	double				Get_Day					(void)		{	return( m_Day          );	}
	double				Get_Sun_Distance		(void)		{	return( m_SunDist      );	}
	double				Get_Sun_Lon				(void)		{	return( m_SunLon       );	}
	double				Get_Sun_Lat				(void)		{	return( m_SunLat       );	}
	double				Get_Zenith				(void)		{	return( asin(m_sinDec) );	}

	double				Get_Daily_Radiation		(double Latitude);

	bool				Get_CosZenith			(double RLAT, double SIND, double COSD, double &COSZT, double &COSZS);


private:

	int					m_Year;
	
	double				m_Day;

	double				m_Eccentricity, m_Obliquity, m_Perihelion, m_SolarConst,
						m_sinDec, m_cosDec, m_SunDist, m_SunLon, m_SunLat, m_EqTime;


};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Solar_Position::CSG_Solar_Position(void)
{
	m_SolarConst	= 1367.0;
	m_Year			= 2000;
	m_Day			= 0.0;

	Get_Orbital_Parameters(m_Year, m_Eccentricity, m_Obliquity, m_Perihelion);
	Get_Orbital_Position  (m_Day, m_sinDec, m_cosDec, m_SunDist, m_SunLon, m_SunLat, m_EqTime);
}

//---------------------------------------------------------
CSG_Solar_Position::CSG_Solar_Position(long Year)
{
	m_SolarConst	= 1367.0;
	m_Year			= Year;
	m_Day			= 0.0;

	Get_Orbital_Parameters(m_Year, m_Eccentricity, m_Obliquity, m_Perihelion);
	Get_Orbital_Position  (m_Day, m_sinDec, m_cosDec, m_SunDist, m_SunLon, m_SunLat, m_EqTime);
}

//---------------------------------------------------------
bool CSG_Solar_Position::Set_Year(int Year)
{
	if( m_Year != Year )
	{
		m_Year	= Year;

		return( Get_Orbital_Parameters(m_Year, m_Eccentricity, m_Obliquity, m_Perihelion) );
	}

	return( true );
}

//---------------------------------------------------------
bool CSG_Solar_Position::Set_Day(double Day)
{
	if( m_Day != Day )
	{
		m_Day	= Day;

		return( Get_Orbital_Position(m_Day, m_sinDec, m_cosDec, m_SunDist, m_SunLon, m_SunLat, m_EqTime) );
	}

	return( true );
}

//---------------------------------------------------------
double CSG_Solar_Position::Get_Daily_Radiation(double Latitude)
{
	double	cosZT, cosZS;

	Get_CosZenith(Latitude, m_sinDec, m_cosDec, cosZT, cosZS);
	
	return( Get_Solar_Constant() * cosZT / (m_SunDist*m_SunDist) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
// ORBPAR calculates the three orbital parameters as a function of
// YEAR.  The source of these calculations is: Andre L. Berger,
// 1978, "Long-Term Variations of Daily Insolation and Quaternary
// Climatic Changes", JAS, v.35, p.2362.  Also useful is: Andre L.
// Berger, May 1978, "A Simple Algorithm to Compute Long Term
// Variations of Daily Insolation", published by Institut
// D'Astronomie de Geophysique, Universite Catholique de Louvain,
// Louvain-la Neuve, No. 18.
//
// Tables and equations refer to the first reference (JAS).  The
// corresponding table or equation in the second reference is
// enclosed in parentheses.  The coefficients used in this
// subroutine are slightly more precise than those used in either
// of the references.  The generated orbital parameters are precise
// within plus or minus 1000000 years from present.
//
// Input:  YEAR  	= years A.D. are positive, B.C. are negative
// Output: ECCEN 	= eccentricity of orbital ellipse
//         OBLIQ 	= latitude of Tropic of Cancer in radians
//         OMEGVP	= longitude of perihelion	=
//                  = spatial angle from vernal equinox to perihelion
//                    in radians with sun as angle vertex
//
//---------------------------------------------------------
bool CSG_Solar_Position::Get_Orbital_Parameters(int YEAR, double &ECCEN, double &OBLIQ, double &OMEGVP)
{
	// Table 1 (2).  Obliquity relative to mean ecliptic of date: OBLIQD
	const double	TABLE1[47][3]	=
	{
		{	-2462.2214466, 31.609974, 251.9025	},
		{	 -857.3232075, 32.620504, 280.8325	},
		{	 -629.3231835, 24.172203, 128.3057	},
		{	 -414.2804924, 31.983787, 292.7252	},
		{	 -311.7632587, 44.828336,  15.3747	},
		{	  308.9408604, 30.973257, 263.7951	},
		{	 -162.5533601, 43.668246, 308.4258	},
		{	 -116.1077911, 32.246691, 240.0099	},
		{	  101.1189923, 30.599444, 222.9725	},
		{	  -67.6856209, 42.681324, 268.7809	},
		{	   24.9079067, 43.836462, 316.7998	},
		{	   22.5811241, 47.439436, 319.6024	},
		{	  -21.1648355, 63.219948, 143.8050	},
		{	  -15.6549876, 64.230478, 172.7351	},
		{	   15.3936813,  1.010530,  28.9300	},
		{	   14.6660938,  7.437771, 123.5968	},
		{	  -11.7273029, 55.782177,  20.2082	},
		{	   10.2742696,   .373813,  40.8226	},
		{	    6.4914588, 13.218362, 123.4722	},
		{	    5.8539148, 62.583231, 155.6977	},
		{	   -5.4872205, 63.593761, 184.6277	},
		{	   -5.4290191, 76.438310, 267.2772	},
		{	    5.1609570, 45.815258,  55.0196	},
		{	    5.0786314,  8.448301, 152.5268	},
		{	   -4.0735782, 56.792707,  49.1382	},
		{	    3.7227167, 49.747842, 204.6609	},
		{	    3.3971932, 12.058272,  56.5233	},
		{	   -2.8347004, 75.278220, 200.3284	},
		{	   -2.6550721, 65.241008, 201.6651	},
		{	   -2.5717867, 64.604291, 213.5577	},
		{	   -2.4712188,  1.647247,  17.0374	},
		{	    2.4625410,  7.811584, 164.4194	},
		{	    2.2464112, 12.207832,  94.5422	},
		{	   -2.0755511, 63.856665, 131.9124	},
		{	   -1.9713669, 56.155990,  61.0309	},
		{	   -1.8813061, 77.448840, 296.2073	},
		{	   -1.8468785,  6.801054, 135.4894	},
		{	    1.8186742, 62.209418, 114.8750	},
		{	    1.7601888, 20.656133, 247.0691	},
		{	   -1.5428851, 48.344406, 256.6114	},
		{	    1.4738838, 55.145460,  32.1008	},
		{	   -1.4593669, 69.000539, 143.6804	},
		{	    1.4192259, 11.071350,  16.8784	},
		{	   -1.1818980, 74.291298, 160.6835	},
		{	    1.1756474, 11.047742,  27.5932	},
		{	   -1.1316126,  0.636717, 348.1074	},
		{	    1.0896928, 12.844549,  82.6496	}
	};

	// Table 4 (1).  Fundamental elements of the ecliptic: ECCEN sin(pi)
	const double	TABLE4[19][3]	=
	{
		{	 .01860798,  4.207205,  28.620089	},
		{	 .01627522,  7.346091, 193.788772	},
		{	-.01300660, 17.857263, 308.307024	},
		{	 .00988829, 17.220546, 320.199637	},
		{	-.00336700, 16.846733, 279.376984	},
		{	 .00333077,  5.199079,  87.195000	},
		{	-.00235400, 18.231076, 349.129677	},
		{	 .00140015, 26.216758, 128.443387	},
		{	 .00100700,  6.359169, 154.143880	},
		{	 .00085700, 16.210016, 291.269597	},
		{	 .00064990,  3.065181, 114.860583	},
		{	 .00059900, 16.583829, 332.092251	},
		{	 .00037800, 18.493980, 296.414411	},
		{	-.00033700,  6.190953, 145.769910	},
		{	 .00027600, 18.867793, 337.237063	},
		{	 .00018200, 17.425567, 152.092288	},
		{	-.00017400,  6.186001, 126.839891	},
		{	-.00012400, 18.417441, 210.667199	},
		{	 .00001250,  0.667863,  72.108838	}
	};

	// Table 5 (3).  General precession in longitude: psi
	const double	TABLE5[78][3]	=
	{
		{	7391.0225890, 31.609974, 251.9025	},
		{	2555.1526947, 32.620504, 280.8325	},
		{	2022.7629188, 24.172203, 128.3057	},
		{  -1973.6517951,  0.636717, 348.1074	},
		{	1240.2321818, 31.983787, 292.7252	},
		{	 953.8679112,  3.138886, 165.1686	},
		{	-931.7537108, 30.973257, 263.7951	},
		{	 872.3795383, 44.828336,  15.3747	},
		{	 606.3544732,  0.991874,  58.5749	},
		{	-496.0274038,  0.373813,  40.8226	},
		{	 456.9608039, 43.668246, 308.4258	},
		{	 346.9462320, 32.246691, 240.0099	},
		{	-305.8412902, 30.599444, 222.9725	},
		{	 249.6173246,  2.147012, 106.5937	},
		{	-199.1027200, 10.511172, 114.5182	},
		{	 191.0560889, 42.681324, 268.7809	},
		{	-175.2936572, 13.650058, 279.6869	},
		{	 165.9068833,  0.986922,  39.6448	},
		{	 161.1285917,  9.874455, 126.4108	},
		{	 139.7878093, 13.013341, 291.5795	},
		{	-133.5228399,  0.262904, 307.2848	},
		{	 117.0673811,  0.004952,  18.9300	},
		{	 104.6907281,  1.142024, 273.7596	},
		{	  95.3227476, 63.219948, 143.8050	},
		{	  86.7824524,  0.205021, 191.8927	},
		{	  86.0857729,  2.151964, 125.5237	},
		{	  70.5893698, 64.230478, 172.7351	},
		{	 -69.9719343, 43.836462, 316.7998	},
		{	 -62.5817473, 47.439436, 319.6024	},
		{	  61.5450059,  1.384343,  69.7526	},
		{	 -57.9364011,  7.437771, 123.5968	},
		{	  57.1899832, 18.829299, 217.6432	},
		{	 -57.0236109,  9.500642,  85.5882	},
		{	 -54.2119253,  0.431696, 156.2147	},
		{	  53.2834147,  1.160090,  66.9489	},
		{	  52.1223575, 55.782177,  20.2082	},
		{	 -49.0059908, 12.639528, 250.7568	},
		{	 -48.3118757,  1.155138,  48.0188	},
		{	 -45.4191685,  0.168216,   8.3739	},
		{	 -42.2357920,  1.647247,  17.0374	},
		{	 -34.7971099, 10.884985, 155.3409	},
		{	  34.4623613,  5.610937,  94.1709	},
		{	 -33.8356643, 12.658184, 221.1120	},
		{	  33.6689362,  1.010530,  28.9300	},
		{	 -31.2521586,  1.983748, 117.1498	},
		{	 -30.8798701, 14.023871, 320.5095	},
		{	  28.4640769,  0.560178, 262.3602	},
		{	 -27.1960802,  1.273434, 336.2148	},
		{	  27.0860736, 12.021467, 233.0046	},
		{	 -26.3437456, 62.583231, 155.6977	},
		{	  24.7253740, 63.593761, 184.6277	},
		{	  24.6732126, 76.438310, 267.2772	},
		{	  24.4272733,  4.280910,  78.9281	},
		{	  24.0127327, 13.218362, 123.4722	},
		{	  21.7150294, 17.818769, 188.7132	},
		{	 -21.5375347,  8.359495, 180.1364	},
		{	  18.1148363, 56.792707,  49.1382	},
		{	 -16.9603104,  8.448301, 152.5268	},
		{	 -16.1765215,  1.978796,  98.2198	},
		{	  15.5567653,  8.863925,  97.4808	},
		{	  15.4846529,  0.186365, 221.5376	},
		{	  15.2150632,  8.996212, 168.2438	},
		{	  14.5047426,  6.771027, 161.1199	},
		{	 -14.3873316, 45.815258,  55.0196	},
		{	  13.1351419, 12.002811, 262.6495	},
		{	  12.8776311, 75.278220, 200.3284	},
		{	  11.9867234, 65.241008, 201.6651	},
		{	  11.9385578, 18.870667, 294.6547	},
		{	  11.7030822, 22.009553,  99.8233	},
		{	  11.6018181, 64.604291, 213.5577	},
		{	 -11.2617293, 11.498094, 154.1631	},
		{	 -10.4664199,  0.578834, 232.7153	},
		{	  10.4333970,  9.237738, 138.3034	},
		{	 -10.2377466, 49.747842, 204.6609	},
		{	  10.1934446,  2.147012, 106.5938	},
		{	 -10.1280191,  1.196895, 250.4676	},
		{	  10.0289441,  2.133898, 332.3345	},
		{	 -10.0034259,  0.173168,  27.3039	}
	};

	int		YM1950	= YEAR - 1950;
	int		i;
	double	ARG;

	//-----------------------------------------------------
	// Obliquity from Table 1 (2):
	//   OBLIQ#	= 23.320556 (degrees)           Equation 5.5 (15)
	//   OBLIQD	= OBLIQ# + sum[A cos(ft+delta)] Equation 1 (5)

	double	SUMC	= 0;

	for(i=0; i<47; i++)	// Do 110 I=1,47
	{
		ARG		= M_DEG_TO_RAD * (YM1950 * TABLE1[i][1] / 3600 + TABLE1[i][2]);
		SUMC	= SUMC + TABLE1[i][0] * cos(ARG);
	}

//	OBLIQD	= 23.320556 + SUMC / 3600;
	OBLIQ	= M_DEG_TO_RAD * (23.320556 + SUMC / 3600);

	//-----------------------------------------------------
	// Eccentricity from Table 4 (1):
	//   ECCEN sin(pi)	= sum[M sin(gt+beta)]	     Equation 4 (1)
	//   ECCEN cos(pi)	= sum[M cos(gt+beta)]	     Equation 4 (1)
	//   ECCEN	= ECCEN sqrt[sin(pi)^2 + cos(pi)^2]

	double	ESINPI	= 0;
	double	ECOSPI	= 0;

	for(i=0; i<19; i++)	// Do 210 I=1,19
	{
		ARG		= M_DEG_TO_RAD * (YM1950 * TABLE4[i][1] / 3600 + TABLE4[i][2]);
		ESINPI	= ESINPI + TABLE4[i][0] * sin(ARG);
		ECOSPI	= ECOSPI + TABLE4[i][0] * cos(ARG);
	}

	ECCEN	= sqrt(ESINPI*ESINPI + ECOSPI*ECOSPI);

	//-----------------------------------------------------
	// Perihelion from Equation 4,6,7 (9) and Table 4,5 (1,3):
	//   PSI#	= 50.439273 (seconds of degree)          Equation 7.5 (16)
	//   ZETA	=  3.392506 (degrees)                    Equation 7.5 (17)
	//   PSI	= PSI# t + ZETA + sum[F sin(ft+delta)]   Equation 7 (9)
	//   PIE	= atan[ECCEN sin(pi) / ECCEN cos(pi)]
	//   OMEGVP	= PIE + PSI + 3.14159                    Equation 6 (4.5)

	double	PIE		= atan2(ESINPI, ECOSPI);
	double	FSINFD	= 0;

	for(i=0; i<78; i++)	// Do 310 I=1,78
	{
		ARG		= M_DEG_TO_RAD * (YM1950 * TABLE5[i][1] / 3600 + TABLE5[i][2]);
		FSINFD	= FSINFD + TABLE5[i][0] * sin(ARG);
	}

	double	PSI	= M_DEG_TO_RAD*(3.392506 + (YM1950 * 50.439273 + FSINFD) / 3600);

	OMEGVP	= fmod(PIE + PSI + .5 * M_PI_360, M_PI_360);

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
// ORBIT receives orbital parameters and time of year, and returns
// distance from Sun, declination angle, and Sun's overhead position.
// Reference for following calculations is:  V.M.Blanco and
// S.W.McCuskey, 1961, "Basic Physics of the Solar System", pages
// 135 - 151.  Existence of Moon and heavenly bodies other than
// Earth and Sun are ignored.  Earth is assumed to be spherical.
//
// Program author: Gary L. Russell 2008/09/22
// Angles, longitude and latitude are measured in radians.
//
// Input: ECCEN       = eccentricity of the orbital ellipse
//        OBLIQ       = latitude of Tropic of Cancer
//        OMEGVP      = longitude of perihelion (sometimes Pi is added)      =
//                    = spatial angle from vernal equinox to perihelion
//                      with Sun as angle vertex
//        DAY         = days measured since 2000 January 1, hour 0
//
// Constants: EDAYzY  = tropical year      = Earth days per year      = 365.2425
//            VE2000  = days from 2000 January 1, hour 0 until vernal
//                      equinox of year 2000      = 31 + 29 + 19 + 7.5/24
//
// Intermediate quantities:
//    BSEMI      = semi minor axis in units of semi major axis
//   PERIHE      = perihelion in days since 2000 January 1, hour 0
//                 in its annual revolution about Sun
//       TA      = true anomaly      = spatial angle from perihelion to
//                 current location with Sun as angle vertex
//       EA      = eccentric anomaly      = spatial angle measured along
//                 eccentric circle (that circumscribes Earth's orbit)
//                 from perihelion to point above (or below) Earth's
//                 absisca (where absisca is directed from center of
//                 eccentric circle to perihelion)
//       MA      = mean anomaly      = temporal angle from perihelion to
//                 current time in units of 2*Pi per tropical year
//   TAofVE      = TA(VE)      = true anomaly of vernal equinox      = - OMEGVP
//   EAofVE      = EA(VE)      = eccentric anomaly of vernal equinox
//   MAofVE      = MA(VE)      = mean anomaly of vernal equinox
//   SLNORO      = longitude of Sun in Earth's nonrotating reference frame
//   VEQLON      = longitude of Greenwich Meridion in Earth's nonrotating
//                 reference frame at vernal equinox
//   ROTATE      = change in longitude in Earth's nonrotating reference
//                 frame from point's location on vernal equinox to its
//                 current location where point is fixed on rotating Earth
//   SLMEAN      = longitude of fictitious mean Sun in Earth's rotating
//                 reference frame (normal longitude and latitude)
//
// Output: SIND  = sin(SUNLAT) = sine of declination angle
//         COSD  = cos(SUNLAT) = cosine of the declination angle
//       SUNDIS  = distance to Sun in units of semi major axis
//       SUNLON  = longitude of point on Earth directly beneath Sun
//       SUNLAT  = latitude of point on Earth directly beneath Sun
//       EQTIME  = Equation of Time      =
//               = longitude of fictitious mean Sun minus SUNLON
//
// From the above reference:
// (4-54): [1 - ECCEN*cos(EA)]*[1 + ECCEN*cos(TA)]      = (1 - ECCEN^2)
// (4-55): tan(TA/2)    = sqrt[(1+ECCEN)/(1-ECCEN)]*tan(EA/2)
// Yield:  tan(EA)      = sin(TA)*sqrt(1-ECCEN^2) / [cos(TA) + ECCEN]
//    or:  tan(TA)      = sin(EA)*sqrt(1-ECCEN^2) / [cos(EA) - ECCEN]
//
//---------------------------------------------------------
bool CSG_Solar_Position::Get_Orbital_Position(double DAY, double &SIND, double &COSD, double &SUNDIS, double &SUNLON, double &SUNLAT, double &EQTIME)
{
	const double	EDAYzY	= 365.2425;
	const double	VE2000	= 79.3125;

	double	ECCEN	= m_Eccentricity;
	double	OBLIQ	= m_Obliquity;
	double	OMEGVP	= m_Perihelion;

	//-----------------------------------------------------
	// Determine EAofVE from geometry: tan(EA)	= b*sin(TA) / [e+cos(TA)]
	// Determine MAofVE from Kepler's equation: MA	= EA - e*sin(EA)
	// Determine MA knowing time from vernal equinox to current day
	double	BSEMI, TAofVE, EAofVE, MAofVE, MA;

	BSEMI 	= sqrt(1.0 - ECCEN*ECCEN);
	TAofVE	= -OMEGVP;
	EAofVE	= atan2(BSEMI*sin(TAofVE), ECCEN+cos(TAofVE));
	MAofVE	= EAofVE - ECCEN*sin(EAofVE);
//	PERIHE	= VE2000 - MAofVE*EDAYzY/M_PI_360
	MA	= fmod(M_PI_360*(DAY-VE2000)/EDAYzY + MAofVE, M_PI_360);
	
	//-----------------------------------------------------
	// Numerically invert Kepler's equation: MA	= EA - e*sin(EA)

	double	dEA, EA	= MA + ECCEN*(sin(MA) + ECCEN*sin(2*MA)/2);

	do
	{
		EA	+= (dEA	= (MA - EA + ECCEN * sin(EA)) / (1.0 - ECCEN * cos(EA)));
	}
	while( fabs(dEA) > 0.1 ); // 1.0e-10 ); // 1d-10 );

	//-----------------------------------------------------
	// Calculate distance to Sun and true anomaly
	SUNDIS	= 1.0 - ECCEN * cos(EA);

	double	TA	= atan2(BSEMI * sin(EA), cos(EA) - ECCEN);

	//-----------------------------------------------------
	// Change reference frame to be nonrotating reference frame, angles
	// fixed according to stars, with Earth at center and positive x
	// axis be ray from Earth to Sun were Earth at vernal equinox, and
	// x-y plane be Earth's equatorial plane.  Distance from current Sun
	// to this x axis is SUNDIS sin(TA-TAofVE).  At vernal equinox, Sun
	// is located at (SUNDIS,0,0).  At other times, Sun is located at:
	//
	// SUN	= (SUNDIS cos(TA-TAofVE),
	//	   SUNDIS sin(TA-TAofVE) cos(OBLIQ),
	//	   SUNDIS sin(TA-TAofVE) sin(OBLIQ))

	double	SUNX, SUNY, SLNORO;

	SIND  	= sin(TA - TAofVE) * sin(OBLIQ);
	COSD  	= sqrt(1 - SIND*SIND);
	SUNX  	= cos(TA - TAofVE);
	SUNY  	= sin(TA - TAofVE) * cos(OBLIQ);
	SLNORO	= atan2(SUNY, SUNX);

	//-----------------------------------------------------
	// Determine Sun location in Earth's rotating reference frame
	// (normal longitude and latitude)

	double	VEQLON, ROTATE;

	VEQLON 	= M_PI_360 * VE2000 - M_PI_180 + MAofVE - TAofVE;  // !  modulo 2*Pi;
	ROTATE 	= M_PI_360 * (DAY-VE2000)*(EDAYzY+1)/EDAYzY;
	SUNLON 	= fmod(SLNORO - ROTATE - VEQLON, M_PI_360);
	if( SUNLON > M_PI_180 )
		SUNLON	= SUNLON - M_PI_360;
	SUNLAT	= asin(sin(TA - TAofVE) * sin(OBLIQ));

	//-----------------------------------------------------
	// Determine longitude of fictitious mean Sun
	// Calculate Equation of Time

	double	SLMEAN;

	SLMEAN	= M_PI_180 - M_PI_360*(DAY - floor(DAY));
	EQTIME	= fmod(SLMEAN - SUNLON, M_PI_360);
	if( EQTIME > M_PI_180 )
		EQTIME	= EQTIME - M_PI_360;

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
// COSZIJ calculates the daily average cosine of the zenith angle
// weighted by time and weighted by sunlight.
//
// Input: RLAT   = latitude (degrees)
//   SIND,COSD   = sine and cosine of the declination angle
//
// Output: COSZT = sum(cosZ*dT) / sum(dT)
//         COSZS = sum(cosZ*cosZ*dT) / sum(cosZ*dT)
//
// Intern: DAWN  = time of DAWN (temporal radians) at mean local time
//         DUSK  = time of DUSK (temporal radians) at mean local time
//
//---------------------------------------------------------
bool CSG_Solar_Position::Get_CosZenith(double RLAT, double SIND, double COSD, double &COSZT, double &COSZS)
{
	if( 0 )	// bDegree
	{
		RLAT	*= M_DEG_TO_RAD;
	}

	double	SINJ = sin(RLAT);
	double	COSJ = cos(RLAT);
	double	SJSD = SINJ*SIND;
	double	CJCD = COSJ*COSD;

	if( SJSD+CJCD <= 0.0 )
	{
		// Constant nighttime at this latitude
	//	DAWN	= 999999;
	//	DUSK	= 999999;
		COSZT	= 0.0;
		COSZS	= 0.0;
	}
	else if( SJSD-CJCD >= 0.0 )
	{
		// Constant daylight at this latitude
		double	ECOSZ, QCOSZ;
	//	DAWN	= -999999;
	//	DUSK	= -999999;
		ECOSZ	= SJSD*M_PI_360;
		QCOSZ	= SJSD*ECOSZ + .5*CJCD*CJCD*M_PI_360;
		COSZT	= SJSD;  // !  = ECOSZ/M_PI_360
		COSZS	= QCOSZ/ECOSZ;
	}
	else
	{
		// Compute DAWN and DUSK (at local time) and their sines
		double	CDUSK, DUSK, SDUSK, S2DUSK;
		CDUSK	= -SJSD/CJCD;
		DUSK	= acos(CDUSK);
		SDUSK	= sqrt(CJCD*CJCD-SJSD*SJSD) / CJCD;
		S2DUSK	= 2*SDUSK*CDUSK;
	//	DAWN	= -DUSK;
	//	SDAWN	= -SDUSK;
	//	S2DAWN	= -S2DUSK;

		// Nighttime at initial and final times with daylight in between
		double	ECOSZ, QCOSZ;
		ECOSZ	= SJSD*(2*DUSK) + CJCD*(2*SDUSK);
		QCOSZ	= SJSD*ECOSZ + CJCD*(SJSD*(2*SDUSK)
				+ .5*CJCD*(2*DUSK + .5*(2*S2DUSK)));
		COSZT	= ECOSZ/M_PI_360;
		COSZS	= QCOSZ/ECOSZ;
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define GET_DESCRIPTION	"Orbital parameters used here are based on the work of Andre L. Berger "\
	"and its implementation from the NASA Goddard Institute for Space Studies (GISS). "\
	"Berger's orbital parameters are considered to be valid for approximately 1 million years. "

//---------------------------------------------------------
#define GET_REFERENCES	Add_Reference(\
	"Berger, A.L.", "1978", "Long Term Variations of Daily Insolation and Quaternary Climatic Changes", \
	"Journal of the Atmospheric Sciences, volume 35(12), 2362-2367."\
); Add_Reference(\
	"Berger, A.L.", "1978", "A Simple Algorithm to Compute Long Term Variations of Daily or Monthly Insolation", \
	"Institut d'Astronomie et de Geophysique, Universite Catholique de Louvain, Louvain-la-Neuve, No. 18."\
);


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CMilankovic::CMilankovic(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Earth's Orbital Parameters"));

	Set_Author		("O.Conrad (c) 2012");

	Set_Description	(_TW(
		GET_DESCRIPTION
	));

	GET_REFERENCES;

	//-----------------------------------------------------
	Parameters.Add_Table("",
		"ORBPAR", _TL("Earth's Orbital Parameters"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Double("",
		"START"	, _TL("Start [ka]"),
		_TL(""),
		-200.
	);

	Parameters.Add_Double("",
		"STOP"	, _TL("Stop [ka]"),
		_TL(""),
		2.
	);

	Parameters.Add_Double("",
		"STEP"	, _TL("Step [ka]"),
		_TL(""),
		1., 0.001, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CMilankovic::On_Execute(void)
{
	//-----------------------------------------------------
	int	Start	= (int)(Parameters("START")->asDouble() * 1000.0);
	int	Stop	= (int)(Parameters("STOP" )->asDouble() * 1000.0);
	int	Step	= (int)(Parameters("STEP" )->asDouble() * 1000.0);

	CSG_Table	*pOrbit	= Parameters("ORBPAR")->asTable();

	pOrbit->Destroy();
	pOrbit->Set_Name(_TL("Earth's Orbital Parameters"));
	pOrbit->Set_NoData_Value(-9999999);
	pOrbit->Add_Field(_TL("Year" 	           ), SG_DATATYPE_Int   );
	pOrbit->Add_Field(_TL("Eccentricity"       ), SG_DATATYPE_Double);
	pOrbit->Add_Field(_TL("Obliquity"          ), SG_DATATYPE_Double);
	pOrbit->Add_Field(_TL("Perihelion"         ), SG_DATATYPE_Double);
	pOrbit->Add_Field(_TL("Climatic Precession"), SG_DATATYPE_Double);

	//-----------------------------------------------------
	for(int year=Start; year<=Stop && Set_Progress(year - Start, Stop - Start); year+=Step)
	{
		double	eccentricity, obliquity, perihelion;

		if( CSG_Solar_Position::Get_Orbital_Parameters(year, eccentricity, obliquity, perihelion) )
		{
			CSG_Table_Record	*pRecord	= pOrbit->Add_Record();

			pRecord->Set_Value(0, year);
			pRecord->Set_Value(1, eccentricity);
			pRecord->Set_Value(2, obliquity);
			pRecord->Set_Value(3, perihelion);
			pRecord->Set_Value(4, -eccentricity * sin(perihelion));
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
CMilankovic_SR_Location::CMilankovic_SR_Location(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Annual Course of Daily Insolation"));

	Set_Author		("O.Conrad (c) 2012");

	Set_Description	(_TW(
		GET_DESCRIPTION
	));

	GET_REFERENCES;

	//-----------------------------------------------------
	Parameters.Add_Table("",
		"SOLARRAD"	, _TL("Solar Radiation"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Double("",
		"START"		, _TL("Start [ka]"),
		_TL(""),
		-200.
	);

	Parameters.Add_Double("",
		"STOP"		, _TL("Stop [ka]"),
		_TL(""),
		2.
	);

	Parameters.Add_Double("",
		"STEP"		, _TL("Step [ka]"),
		_TL(""),
		1, 0.001, true
	);

	Parameters.Add_Double("",
		"LAT"		, _TL("Latitude [Degree]"),
		_TL(""),
		53.0, -90.0, true, 90.0, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CMilankovic_SR_Location::On_Execute(void)
{
	//-----------------------------------------------------
	int		Start	= (int)(Parameters("START")->asDouble() * 1000.0);
	int		Stop	= (int)(Parameters("STOP" )->asDouble() * 1000.0);
	int		Step	= (int)(Parameters("STEP" )->asDouble() * 1000.0);

	double	Lat		= Parameters("LAT")->asDouble() * M_DEG_TO_RAD;

	CSG_Table	*pRadiation	= Parameters("SOLARRAD")->asTable();

	pRadiation->Destroy();
	pRadiation->Set_Name(_TL("Annual Course of Daily Insolation"));
	pRadiation->Set_NoData_Value(-9999999);
	pRadiation->Add_Field(_TL("Day")        , SG_DATATYPE_Int);
	pRadiation->Add_Field(_TL("Zenith")     , SG_DATATYPE_Double);
	pRadiation->Add_Field(_TL("Culmination"), SG_DATATYPE_Double);
	pRadiation->Add_Field(_TL("Radiation")  , SG_DATATYPE_Double);

	//-----------------------------------------------------
	for(int year=Start; year<=Stop && Set_Progress(year - Start, Stop - Start); year+=Step)
	{
		Process_Set_Text(CSG_String::Format(SG_T("%s: %d"), _TL("Year"), year));

		CSG_Solar_Position	Position(year);

		pRadiation->Del_Records();

		for(int day=1; day<=365 && Process_Get_Okay(); day++)
		{
			if( Position.Set_Day(day) )
			{
				CSG_Table_Record	*pRecord	= pRadiation->Add_Record();

				pRecord->Set_Value(0, day);
				pRecord->Set_Value(1, Position.Get_Zenith() * M_RAD_TO_DEG);
				pRecord->Set_Value(2, 90 - fabs(Lat - Position.Get_Zenith()) * M_RAD_TO_DEG);
				pRecord->Set_Value(3, Position.Get_Daily_Radiation(Lat));
			}
		}

		DataObject_Update(pRadiation);
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
CMilankovic_SR_Day_Location::CMilankovic_SR_Day_Location(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Daily Insolation over Latitude"));

	Set_Author		("O.Conrad (c) 2012");

	Set_Description	(_TW(
		GET_DESCRIPTION
	));

	GET_REFERENCES;

	//-----------------------------------------------------
	Parameters.Add_Table("",
		"SOLARRAD"	, _TL("Solar Radiation"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Double("",
		"START"		, _TL("Start [ka]"),
		_TL(""),
		-200.
	);

	Parameters.Add_Double("",
		"STOP"		, _TL("Stop [ka]"),
		_TL(""),
		2.
	);

	Parameters.Add_Double("",
		"STEP"		, _TL("Step [ka]"),
		_TL(""),
		1., 0.001, true
	);

	Parameters.Add_Int("",
		"DLAT"		, _TL("Latitude Increment [Degree]"),
		_TL(""),
		5, 1, true, 90, true
	);

	Parameters.Add_Int("",
		"DAY"		, _TL("Day of Year"),
		_TL(""),
		181, 0, true, 366, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CMilankovic_SR_Day_Location::On_Execute(void)
{
	int		iLat, nLat;

	//-----------------------------------------------------
	int		Start	= (int)(Parameters("START")->asDouble() * 1000.0);
	int		Stop	= (int)(Parameters("STOP" )->asDouble() * 1000.0);
	int		Step	= (int)(Parameters("STEP" )->asDouble() * 1000.0);

	int		Day		= Parameters("DAY")->asInt();

	double	dLat	= Parameters("DLAT")->asDouble() * M_DEG_TO_RAD;

	CSG_Table	*pRadiation	= Parameters("SOLARRAD")->asTable();

	pRadiation->Destroy();
	pRadiation->Set_NoData_Value(-9999999);
	pRadiation->Add_Field(_TL("Year")     , SG_DATATYPE_Int);

	nLat	= (int)(M_PI_180 / dLat);

	for(iLat=0; iLat<=nLat; iLat++)
	{
		pRadiation->Add_Field(CSG_String::Format(SG_T("LAT_%.1f"), -90.0 + iLat * M_RAD_TO_DEG * dLat), SG_DATATYPE_Double);
	}

	//-----------------------------------------------------
	for(int year=Start; year<=Stop && Set_Progress(year - Start, Stop - Start); year+=Step)
	{
		Process_Set_Text(CSG_String::Format(SG_T("%s: %d"), _TL("Year"), year));

		CSG_Solar_Position	Position(year);

		double	sinDec, cosDec, Distance, SunLon, SunLat, EqTime, cosZT, cosZS;

		if( Position.Get_Orbital_Position(Day, sinDec, cosDec, Distance, SunLon, SunLat, EqTime) )
		{
			CSG_Table_Record	*pRecord	= pRadiation->Add_Record();

			pRecord->Set_Value(0, year);

			for(iLat=0; iLat<=nLat; iLat++)
			{
				Position.Get_CosZenith(-M_PI_090 + iLat * dLat, sinDec, cosDec, cosZT, cosZS);

				pRecord->Set_Value(1 + iLat, 1367 * cosZT / (Distance*Distance));
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
CMilankovic_SR_Monthly_Global::CMilankovic_SR_Monthly_Global(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Monthly Global by Latitude"));

	Set_Author		("O.Conrad (c) 2012");

	Set_Description	(_TW(
		GET_DESCRIPTION
	));

	GET_REFERENCES;

	//-----------------------------------------------------
	Parameters.Add_Table("",
		"SOLARRAD"	, _TL("Solar Radiation"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Table("",
		"ALBEDO"	, _TL("Albedo"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Table_Field("ALBEDO",
		"FIELD"		, _TL("Field"),
		_TL("")
	);

	Parameters.Add_Double("",
		"YEAR"		, _TL("Year [ka]"),
		_TL(""),
		2.
	);

	Parameters.Add_Int("",
		"DLAT"		, _TL("Latitude Increment [Degree]"),
		_TL(""),
		5, 1, true, 90, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CMilankovic_SR_Monthly_Global::On_Execute(void)
{
	const int	nDays [12]	= {	31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31	};
	const char	*Month[12]	= {	"JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"	};

	int		iMonth, iDay, aDay, bDay, iLat, Lat;

	//-----------------------------------------------------
	int		Year	= (int)(Parameters("YEAR")->asDouble() * 1000.0);
	int		dLat	= Parameters("DLAT")->asInt();

	CSG_Matrix	SR(12, 181);
	CSG_Vector	SRYear(181), SRGlobal(12), Area(181); // , Albedo(181);

	//-----------------------------------------------------
	CSG_Table	*pAlbedo	= Parameters("ALBEDO")->asTable();
	int			fAlbedo		= Parameters("FIELD") ->asInt();

	if( pAlbedo && pAlbedo->Get_Record_Count() != 181 )
	{
		Message_Add(_TL("warning: albedo is ignored"));

		pAlbedo	= NULL;
	}

	//-----------------------------------------------------
	for(iLat=0, Lat=-90; iLat<=180; iLat++, Lat++)
	{
		double	LATN	= iLat == 180 ?  M_PI / 2.0 : (Lat + 0.5) * M_DEG_TO_RAD;
		double	LATS	= iLat ==   0 ? -M_PI / 2.0 : (Lat - 0.5) * M_DEG_TO_RAD;

		Area[iLat]	= 0.5 * (sin(LATN) - sin(LATS));
	}

	//-----------------------------------------------------
	CSG_Solar_Position	Position(Year);

	for(iMonth=0, bDay=1; iMonth<12; iMonth++)
	{
		aDay	= bDay;	bDay	+= nDays[iMonth];

		for(iDay=aDay; iDay<bDay; iDay++)
		{
			Position.Set_Day(iDay);

			for(iLat=0, Lat=-90; iLat<=180; iLat++, Lat++)
			{
				double	s	= Position.Get_Daily_Radiation(M_DEG_TO_RAD * Lat);

				if( pAlbedo )
				{
					s	*= (1.0 - pAlbedo->Get_Record(iLat)->asDouble(fAlbedo));
				}

				SR[iLat][iMonth]	+= s;
			}
		}

		for(iLat=0; iLat<=180; iLat++)
		{
			SR[iLat][iMonth]	/= nDays[iMonth];
			SRGlobal[iMonth]	+= SR[iLat][iMonth] * Area[iLat];
			SRYear[iLat]		+= SR[iLat][iMonth];
		}
	}

	//-----------------------------------------------------
	CSG_Table			*pRadiation	= Parameters("SOLARRAD")->asTable();
	CSG_Table_Record	*pRecord;

	pRadiation->Destroy();
	pRadiation->Set_Name(CSG_String::Format(SG_T("%s [%d]"), _TL("Global Irradiation"), Year));
	pRadiation->Set_NoData_Value(-9999999);
	pRadiation->Add_Field(_TL("Lat"), SG_DATATYPE_String);

	for(iMonth=0; iMonth<12; iMonth++)
	{
		pRadiation->Add_Field(Month[iMonth], SG_DATATYPE_Double);
	}

	pRadiation->Add_Field(_TL("Annual"), SG_DATATYPE_Double);

	for(iLat=0, Lat=-90; iLat<=180; iLat+=dLat, Lat+=dLat)
	{
		pRecord	= pRadiation->Add_Record();
		pRecord->Set_Value(0, Lat);

		for(iMonth=0; iMonth<12; iMonth++)
		{
			pRecord->Set_Value(1 + iMonth, SR[iLat][iMonth]);
		}

		pRecord->Set_Value(1 + 12, SRYear[iLat] / 12.0);
	}

	pRecord	= pRadiation->Add_Record();
	pRecord->Set_Value(0, SG_T("global"));

	double	SRGLAN	= 0.0;

	for(iMonth=0; iMonth<12; iMonth++)
	{
		pRecord->Set_Value(1 + iMonth, SRGlobal[iMonth]);

		SRGLAN	+= SRGlobal[iMonth];
	}

	pRecord->Set_Value(1 + 12, SRGLAN / 12.0);

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
