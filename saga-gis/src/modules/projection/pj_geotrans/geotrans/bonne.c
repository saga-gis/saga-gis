/**********************************************************
 * Version $Id: bonne.c 911 2011-02-14 16:38:15Z reklov_w $
 *********************************************************/
/***************************************************************************/
/* RSC IDENTIFIER: BONNE
 *
 * ABSTRACT
 *
 *    This component provides conversions between Geodetic coordinates
 *    (latitude and longitude in radians) and Bonne projection coordinates
 *    (easting and northing in meters).
 *
 * ERROR HANDLING
 *
 *    This component checks parameters for valid values.  If an invalid value
 *    is found, the error code is combined with the current error code using 
 *    the bitwise or.  This combining allows multiple error codes to be
 *    returned. The possible error codes are:
 *
 *          BONN_NO_ERROR           : No errors occurred in function
 *          BONN_LAT_ERROR          : Latitude outside of valid range
 *                                      (-90 to 90 degrees)
 *          BONN_LON_ERROR          : Longitude outside of valid range
 *                                      (-180 to 360 degrees)
 *          BONN_EASTING_ERROR      : Easting outside of valid range
 *                                      (False_Easting +/- ~20,500,000 m,
 *                                       depending on ellipsoid parameters
 *                                       and Origin_Latitude)
 *          BONN_NORTHING_ERROR     : Northing outside of valid range
 *                                      (False_Northing +/- ~23,500,000 m,
 *                                       depending on ellipsoid parameters
 *                                       and Origin_Latitude)
 *          BONN_ORIGIN_LAT_ERROR   : Origin latitude outside of valid range
 *                                      (-90 to 90 degrees)
 *          BONN_CENT_MER_ERROR     : Central meridian outside of valid range
 *                                      (-180 to 360 degrees)
 *          BONN_A_ERROR            : Semi-major axis less than or equal to zero
 *          BONN_INV_F_ERROR        : Inverse flattening outside of valid range
 *									                    (250 to 350)
 *
 * REUSE NOTES
 *
 *    BONNE is intended for reuse by any application that performs a
 *    Bonne projection or its inverse.
 *
 * REFERENCES
 *
 *    Further information on BONNE can be found in the Reuse Manual.
 *
 *    BONNE originated from :  U.S. Army Topographic Engineering Center
 *                             Geospatial Information Division
 *                             7701 Telegraph Road
 *                             Alexandria, VA  22310-3864
 *
 * LICENSES
 *
 *    None apply to this component.
 *
 * RESTRICTIONS
 *
 *    BONNE has no restrictions.
 *
 * ENVIRONMENT
 *
 *    BONNE was tested and certified in the following environments:
 *
 *    1. Solaris 2.5 with GCC 2.8.1
 *    2. MS Windows 95 with MS Visual C++ 6
 *
 * MODIFICATIONS
 *
 *    Date              Description
 *    ----              -----------
 *    04-16-99          Original Code
 *
 */


/***************************************************************************/
/*
 *                               INCLUDES
 */
#include <math.h>
#include "bonne.h"
#include "sinusoid.h"

/*
 *    math.h  - Is needed to call the math functions (sqrt, pow, exp, log,
 *                 sin, cos, tan, and atan).
 *    bonne.h - Is for prototype error checking.
 *    sinusoid.h - Is called when the origin latitude is zero.
 */


/***************************************************************************/
/*
 *                               DEFINES
 */

#define PI         3.14159265358979323e0  /* PI                            */
#define PI_OVER_2  (PI / 2.0)                 
#define TWO_PI     (2.0 * PI)                 
#define BONN_m(coslat,sinlat)                   (coslat/sqrt(1.0 - es2*sinlat*sinlat))
#define BONN_M(c0lat,c1s2lat,c2s4lat,c3s6lat)   (Bonn_a*(c0lat-c1s2lat+c2s4lat-c3s6lat))
#define COEFF_TIMES_BONN_SIN(coeff,x,latit)	    (coeff*(sin(x * latit)))
#define FLOAT_EQ(x,v,epsilon)   (((v - epsilon) < x) && (x < (v + epsilon)))


/***************************************************************************/
/*
 *                               GLOBALS
 */
/* Ellipsoid Parameters, default to WGS 84 */
static double Bonn_a = 6378137.0;                 /* Semi-major axis of ellipsoid in meters */
static double Bonn_f = 1 / 298.257223563;         /* Flattening of ellipsoid */
static double es2 = 0.0066943799901413800;        /* Eccentricity (0.08181919084262188000) squared  */
static double es4 = 4.4814723452405e-005;         /* es2 * es2 */
static double es6 = 3.0000678794350e-007;         /* es4 * es2 */
static double M1 = 4984944.3782319;               /* Bonn_M(Bonna,c0lat,c1s2lat,c2s4lat,c3s6lat) */
static double m1 = .70829317069372;               /* Bonn_m(coslat,sinlat,es2) */
static double c0 = .99832429845280;               /* 1 - es2 / 4.0 - 3.0 * es4 / 64.0 - 5.0 * es6 / 256.0 */
static double c1 = .0025146070605187;             /* 3.0 * es2 / 8.0 + 3.0 * es4 / 32.0 + 45.0 * es6 / 1024.0 */
static double c2 = 2.6390465943377e-006;          /* 15.0 * es4 / 256.0 + 45.0 * es6 / 1024.0 */
static double c3 = 3.4180460865959e-009;          /* 35.0 * es6 / 3072.0 */
static double a0 = .0025188265843907;             /* 3.0 * e1 / 2.0 - 27.0 * e3 / 32.0 */
static double a1 = 3.7009490356205e-006;          /* 21.0 * e2 / 16.0 - 55.0 * e4 / 32.0 */
static double a2 = 7.4478137675038e-009;          /* 151.0 * e3 / 96.0 */
static double a3 = 1.7035993238596e-011;          /* 1097.0 * e4 / 512.0 */


/* Bonne projection Parameters */
static double  Bonn_Origin_Lat = ((45 * PI) / 180.0);           /* Latitude of origin in radians     */
static double  Bonn_Origin_Long = 0.0;                          /* Longitude of origin in radians    */
static double  Bonn_False_Northing = 0.0;                       /* False northing in meters          */
static double  Bonn_False_Easting = 0.0;                        /* False easting in meters           */
static double  Sin_Bonn_Origin_Lat = .70710678118655;           /* sin(Bonn_Origin_Lat)              */
static double  Bonn_am1sin = 6388838.2901211;                   /* Bonn_a * m1 / Sin_Bonn_Origin_Lat */

/* Maximum variance for easting and northing values for WGS 84.
 */
static double  Bonn_Max_Easting = 20027474.0;
static double  Bonn_Min_Easting = -20027474.0;
static double  Bonn_Delta_Northing = 20003932.0;
/*
 * These state variables are for optimization purposes.  The only function
 * that should modify them is Set_Bonne_Parameters.
 */


/***************************************************************************/
/*
 *                              FUNCTIONS     
 */


long Set_Bonne_Parameters(double a,
                          double f,
                          double Origin_Latitude,
                          double Central_Meridian,
                          double False_Easting,
                          double False_Northing)
{ /* Begin Set_Bonne_Parameters */
/*
 * The function Set_Bonne_Parameters receives the ellipsoid parameters and
 * Bonne projection parameters as inputs, and sets the corresponding state
 * variables.  If any errors occur, the error code(s) are returned by the 
 * function, otherwise BONN_NO_ERROR is returned.
 *
 *    a                 : Semi-major axis of ellipsoid, in meters   (input)
 *    f                 : Flattening of ellipsoid                   (input)
 *    Origin_Latitude   : Latitude in radians at which the          (input)
 *                          point scale factor is 1.0
 *    Central_Meridian  : Longitude in radians at the center of     (input)
 *                          the projection
 *    False_Easting     : A coordinate value in meters assigned to the
 *                          central meridian of the projection.     (input)
 *    False_Northing    : A coordinate value in meters assigned to the
 *                          origin latitude of the projection       (input)
 */

  double j, three_es4;
  double x,e1,e2,e3,e4;
  double clat; 
  double sin2lat, sin4lat, sin6lat, lat;
  double inv_f = 1 / f;
  long Error_Code = BONN_NO_ERROR;

  if (a <= 0.0)
  { /* Semi-major axis must be greater than zero */
    Error_Code |= BONN_A_ERROR;
  }
  if ((inv_f < 250) || (inv_f > 350))
  { /* Inverse flattening must be between 250 and 350 */
	 Error_Code |= BONN_INV_F_ERROR;
  } 
  if ((Origin_Latitude < -PI_OVER_2) || (Origin_Latitude > PI_OVER_2))
  { /* origin latitude out of range */
    Error_Code |= BONN_ORIGIN_LAT_ERROR;
  }
  if ((Central_Meridian < -PI) || (Central_Meridian > TWO_PI))
  { /* origin longitude out of range */
    Error_Code |= BONN_CENT_MER_ERROR;
  }
  if (!Error_Code)
  { /* no errors */
    Bonn_a = a;
    Bonn_f = f;
    Bonn_Origin_Lat = Origin_Latitude;
    if (Central_Meridian > PI)
      Central_Meridian -= TWO_PI;
    Bonn_Origin_Long = Central_Meridian;
    Bonn_False_Northing = False_Northing;
    Bonn_False_Easting = False_Easting;
    if (Bonn_Origin_Lat == 0.0)
		{
			if (Bonn_Origin_Long > 0)
			{
				Bonn_Max_Easting = 19926189.0;
				Bonn_Min_Easting = -20037509.0;
			}
			else if (Bonn_Origin_Long < 0)
			{
				Bonn_Max_Easting = 20037509.0;
				Bonn_Min_Easting = -19926189.0;
			}
			else
			{
				Bonn_Max_Easting = 20037509.0;
				Bonn_Min_Easting = -20037509.0;
			}
			Bonn_Delta_Northing = 10001966.0;
			Set_Sinusoidal_Parameters(Bonn_a, Bonn_f, Bonn_Origin_Long, Bonn_False_Easting, Bonn_False_Northing);
		}
		else
		{
			Sin_Bonn_Origin_Lat = sin(Bonn_Origin_Lat);

			es2 = 2 * Bonn_f - Bonn_f * Bonn_f;
			es4 = es2 * es2;
			es6 = es4 * es2;
			j = 45.0 * es6 / 1024.0;
			three_es4 = 3.0 * es4;
			c0 = 1 - es2 / 4.0 - three_es4 / 64.0 - 5.0 * es6 / 256.0;
			c1 = 3.0 * es2 / 8.0 + three_es4 / 32.0 + j;
			c2 = 15.0 * es4 / 256.0 + j;
			c3 = 35.0 * es6 / 3072.0;

			clat = cos(Bonn_Origin_Lat);
			m1 = BONN_m(clat, Sin_Bonn_Origin_Lat);

			lat = c0 * Bonn_Origin_Lat;
			sin2lat = COEFF_TIMES_BONN_SIN(c1, 2.0, Bonn_Origin_Lat);
			sin4lat = COEFF_TIMES_BONN_SIN(c2, 4.0, Bonn_Origin_Lat);
			sin6lat = COEFF_TIMES_BONN_SIN(c3, 6.0, Bonn_Origin_Lat);
			M1 = BONN_M(lat, sin2lat, sin4lat, sin6lat);

			x = sqrt (1.0 - es2);
			e1 = (1.0 - x) / (1.0 + x);
			e2 = e1 * e1;
			e3 = e2 * e1;
			e4 = e3 * e1;
			a0 = 3.0 * e1 / 2.0 - 27.0 * e3 / 32.0;
			a1 = 21.0 * e2 / 16.0 - 55.0 * e4 / 32.0;
			a2 = 151.0 * e3 / 96.0;
			a3 = 1097.0 * e4 / 512.0;
			if (Sin_Bonn_Origin_Lat == 0.0)
				Bonn_am1sin = 0.0;
			else
				Bonn_am1sin = Bonn_a * m1 / Sin_Bonn_Origin_Lat;

			Bonn_Max_Easting = 20027474.0;
			Bonn_Min_Easting = -20027474.0;
			Bonn_Delta_Northing = 20003932.0;

		}

  } /* End if(!Error_Code) */
  return (Error_Code);
} /* End Set_Bonne_Parameters */


void Get_Bonne_Parameters(double *a,
                          double *f,
                          double *Origin_Latitude,
                          double *Central_Meridian,
                          double *False_Easting,
                          double *False_Northing)
{ /* Begin Get_Bonne_Parameters */
/*
 * The function Get_Bonne_Parameters returns the current ellipsoid
 * parameters and Bonne projection parameters.
 *
 *    a                 : Semi-major axis of ellipsoid, in meters   (output)
 *    f                 : Flattening of ellipsoid                   (output)
 *    Origin_Latitude   : Latitude in radians at which the          (output)
 *                          point scale factor is 1.0
 *    Central_Meridian  : Longitude in radians at the center of     (output)
 *                          the projection
 *    False_Easting     : A coordinate value in meters assigned to the
 *                          central meridian of the projection.     (output)
 *    False_Northing    : A coordinate value in meters assigned to the
 *                          origin latitude of the projection       (output)
 */

  *a = Bonn_a;
  *f = Bonn_f;
  *Origin_Latitude = Bonn_Origin_Lat;
  *Central_Meridian = Bonn_Origin_Long;
  *False_Easting = Bonn_False_Easting;
  *False_Northing = Bonn_False_Northing;
  return;
} /* End Get_Bonne_Parameters */


long Convert_Geodetic_To_Bonne (double Latitude,
                                double Longitude,
                                double *Easting,
                                double *Northing)
{ /* Begin Convert_Geodetic_To_Bonne */
/*
 * The function Convert_Geodetic_To_Bonne converts geodetic (latitude and
 * longitude) coordinates to Bonne projection (easting and northing)
 * coordinates, according to the current ellipsoid and Bonne projection
 * parameters.  If any errors occur, the error code(s) are returned by the
 * function, otherwise BONN_NO_ERROR is returned.
 *
 *    Latitude          : Latitude (phi) in radians           (input)
 *    Longitude         : Longitude (lambda) in radians       (input)
 *    Easting           : Easting (X) in meters               (output)
 *    Northing          : Northing (Y) in meters              (output)
 */

  double dlam; /* Longitude - Central Meridan */
  double mm;
  double MM;
  double rho;
  double EE;
  double clat = cos(Latitude);
  double slat = sin(Latitude);
  double lat, sin2lat, sin4lat, sin6lat;
  long Error_Code = BONN_NO_ERROR;

  if ((Latitude < -PI_OVER_2) || (Latitude > PI_OVER_2))
  { /* Latitude out of range */
    Error_Code |= BONN_LAT_ERROR;
  }
  if ((Longitude < -PI) || (Longitude > TWO_PI))
  { /* Longitude out of range */
    Error_Code |= BONN_LON_ERROR;
  }
  if (!Error_Code)
  { /* no errors */
    if (Bonn_Origin_Lat == 0.0)
			Convert_Geodetic_To_Sinusoidal(Latitude, Longitude, Easting, Northing);
		else
		{
			dlam = Longitude - Bonn_Origin_Long;
			if (dlam > PI)
			{
				dlam -= TWO_PI;
			}
			if (dlam < -PI)
			{
				dlam += TWO_PI;
			}
			if ((Latitude - Bonn_Origin_Lat) == 0.0 && FLOAT_EQ(fabs(Latitude),PI_OVER_2,.00001))
			{
				*Easting = 0.0;
				*Northing = 0.0;
			}
			else
			{
				mm = BONN_m(clat, slat);

				lat = c0 * Latitude;
				sin2lat = COEFF_TIMES_BONN_SIN(c1, 2.0, Latitude);
				sin4lat = COEFF_TIMES_BONN_SIN(c2, 4.0, Latitude);
				sin6lat = COEFF_TIMES_BONN_SIN(c3, 6.0, Latitude);
				MM = BONN_M(lat, sin2lat, sin4lat, sin6lat);         

				rho = Bonn_am1sin + M1 - MM;
				if (rho == 0)
					EE = 0;
				else
					EE = Bonn_a * mm * dlam / rho;
				*Easting = rho * sin(EE) + Bonn_False_Easting;
				*Northing = Bonn_am1sin - rho * cos(EE) + Bonn_False_Northing;
			}
    }
  }
  return (Error_Code);
} /* End Convert_Geodetic_To_Bonne */


long Convert_Bonne_To_Geodetic(double Easting,
                               double Northing,
                               double *Latitude,
                               double *Longitude)
{ /* Begin Convert_Bonne_To_Geodetic */
/*
 * The function Convert_Bonne_To_Geodetic converts Bonne projection
 * (easting and northing) coordinates to geodetic (latitude and longitude)
 * coordinates, according to the current ellipsoid and Bonne projection
 * coordinates.  If any errors occur, the error code(s) are returned by the
 * function, otherwise BONN_NO_ERROR is returned.
 *
 *    Easting           : Easting (X) in meters                  (input)
 *    Northing          : Northing (Y) in meters                 (input)
 *    Latitude          : Latitude (phi) in radians              (output)
 *    Longitude         : Longitude (lambda) in radians          (output)
 */

  double dx;     /* Delta easting - Difference in easting (easting-FE)      */
  double dy;     /* Delta northing - Difference in northing (northing-FN)   */
  double mu;
  double MM;
  double mm;
  double am1sin_dy;
  double rho;
  double sin2mu, sin4mu, sin6mu, sin8mu;
  double clat, slat;
  long Error_Code = BONN_NO_ERROR;

  if ((Easting < (Bonn_False_Easting + Bonn_Min_Easting))
      || (Easting > (Bonn_False_Easting + Bonn_Max_Easting)))
  { /* Easting out of range */
    Error_Code |= BONN_EASTING_ERROR;
  }
  if ((Northing < (Bonn_False_Northing - Bonn_Delta_Northing))
      || (Northing > (Bonn_False_Northing + Bonn_Delta_Northing)))
  { /* Northing out of range */
    Error_Code |= BONN_NORTHING_ERROR;
  }
  if (!Error_Code)
  { /* no errors */
    if (Bonn_Origin_Lat == 0.0)
			Convert_Sinusoidal_To_Geodetic(Easting, Northing, Latitude, Longitude);
		else
		{
			dy = Northing - Bonn_False_Northing;
			dx = Easting - Bonn_False_Easting;
			am1sin_dy = Bonn_am1sin - dy;
			rho = sqrt(dx * dx + am1sin_dy * am1sin_dy);
			if (Bonn_Origin_Lat < 0.0)
				rho = -rho;
			MM = Bonn_am1sin + M1 - rho;

			mu = MM / (Bonn_a * c0); 
			sin2mu = COEFF_TIMES_BONN_SIN(a0, 2.0, mu);
			sin4mu = COEFF_TIMES_BONN_SIN(a1, 4.0, mu);
			sin6mu = COEFF_TIMES_BONN_SIN(a2, 6.0, mu);
			sin8mu = COEFF_TIMES_BONN_SIN(a3, 8.0, mu);
			*Latitude = mu + sin2mu + sin4mu + sin6mu + sin8mu;

			if (FLOAT_EQ(fabs(*Latitude),PI_OVER_2,.00001))
			{
				*Longitude = Bonn_Origin_Long;
			}
			else
			{
				clat = cos(*Latitude);
				slat = sin(*Latitude);
				mm = BONN_m(clat, slat);

				if (Bonn_Origin_Lat < 0.0)
				{
					dx = -dx;
					am1sin_dy = -am1sin_dy;
				}
				*Longitude = Bonn_Origin_Long + rho * (atan2(dx, am1sin_dy)) /
										 (Bonn_a * mm);
			}

			if (*Latitude > PI_OVER_2)  /* force distorted values to 90, -90 degrees */
				*Latitude = PI_OVER_2;
			else if (*Latitude < -PI_OVER_2)
				*Latitude = -PI_OVER_2;

			if (*Longitude > PI)
				*Longitude -= TWO_PI;
			if (*Longitude < -PI)
				*Longitude += TWO_PI;

			if (*Longitude > PI)  /* force distorted values to 180, -180 degrees */
				*Longitude = PI;
			else if (*Longitude < -PI)
				*Longitude = -PI;
		}
  }
  return (Error_Code);
} /* End Convert_Bonne_To_Geodetic */
