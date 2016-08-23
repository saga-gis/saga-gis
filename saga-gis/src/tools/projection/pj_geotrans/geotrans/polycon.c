/**********************************************************
 * Version $Id: polycon.c 911 2011-02-14 16:38:15Z reklov_w $
 *********************************************************/
/***************************************************************************/
/* RSC IDENTIFIER: POLYCONIC
 *
 * ABSTRACT
 *
 *    This component provides conversions between Geodetic coordinates 
 *    (latitude and longitude in radians) and Polyconic projection coordinates
 *    (easting and northing in meters).
 *
 * ERROR HANDLING
 *
 *    This component checks parameters for valid values.  If an invalid value
 *    is found, the error code is combined with the current error code using 
 *    the bitwise or.  This combining allows multiple error codes to be
 *    returned. The possible error codes are:
 *
 *          POLY_NO_ERROR           : No errors occurred in function
 *          POLY_LAT_ERROR          : Latitude outside of valid range
 *                                      (-90 to 90 degrees)
 *          POLY_LON_ERROR          : Longitude outside of valid range
 *                                      (-180 to 360 degrees)
 *          POLY_EASTING_ERROR      : Easting outside of valid range
 *                                      (False_Easting +/- ~20,500,000 m,
 *                                       depending on ellipsoid parameters
 *                                       and Origin_Latitude)
 *          POLY_NORTHING_ERROR     : Northing outside of valid range
 *                                      (False_Northing +/- ~15,500,000 m,
 *                                       depending on ellipsoid parameters
 *                                       and Origin_Latitude)
 *          POLY_ORIGIN_LAT_ERROR   : Origin latitude outside of valid range
 *                                      (-90 to 90 degrees)
 *          POLY_CENT_MER_ERROR     : Central meridian outside of valid range
 *                                      (-180 to 360 degrees)
 *          POLY_A_ERROR            : Semi-major axis less than or equal to zero
 *          POLAR_INV_F_ERROR       : Inverse flattening outside of valid range
 *								  	                  (250 to 350)
 *		    POLY_LON_WARNING        : Distortion will result if longitude is more
 *                                     than 90 degrees from the Central Meridian
 *
 * REUSE NOTES
 *
 *    POLYCONIC is intended for reuse by any application that performs a
 *    Polyconic projection or its inverse.
 *    
 * REFERENCES
 *
 *    Further information on POLYCONIC can be found in the Reuse Manual.
 *
 *    POLYCONIC originated from :  U.S. Army Topographic Engineering Center
 *                                 Geospatial Information Division
 *                                 7701 Telegraph Road
 *                                 Alexandria, VA  22310-3864
 *
 * LICENSES
 *
 *    None apply to this component.
 *
 * RESTRICTIONS
 *
 *    POLYCONIC has no restrictions.
 *
 * ENVIRONMENT
 *
 *    POLYCONIC was tested and certified in the following environments:
 *
 *    1. Solaris 2.5 with GCC, version 2.8.1
 *    2. Windows 95 with MS Visual C++, version 6
 *
 * MODIFICATIONS
 *
 *    Date              Description
 *    ----              -----------
 *    10-06-99          Original Code
 *
 */


/***************************************************************************/
/*
 *                               INCLUDES
 */

#include <math.h>
#include "polycon.h"

/*
 *    math.h     - Standard C math library
 *    polycon.h - Is for prototype error checking
 */


/***************************************************************************/
/*
 *                               DEFINES
 */

#define PI         3.14159265358979323e0  /* PI                            */
#define PI_OVER_2  ( PI / 2.0)                 
#define TWO_PI     (2.0 * PI)                  
#define POLY_COEFF_TIMES_SIN(coeff, x, latit)         (coeff * (sin (x * latit)))
#define POLY_M(c0lat,c1s2lat,c2s4lat,c3s6lat)   (Poly_a*(c0lat - c1s2lat + c2s4lat - c3s6lat)) 
#define FLOAT_EQ(x,v,epsilon)   (((v - epsilon) < x) && (x < (v + epsilon)))
#define FOURTY_ONE (41.0 * PI / 180)     /* 41 degrees in radians          */

/***************************************************************************/
/*
 *                               GLOBALS
 */

/* Ellipsoid Parameters, default to WGS 84 */
static double Poly_a = 6378137.0;                      /* Semi-major axis of ellipsoid in meters */
static double Poly_f = 1 / 298.257223563;              /* Flattening of ellipsoid */
static double es2 = 0.0066943799901413800;             /* Eccentricity (0.08181919084262188000) squared         */
static double es4 = 4.4814723452405e-005;              /* es2 * es2 */
static double es6 = 3.0000678794350e-007;              /* es4 * es2 */
static double M0 = 0.0;
static double c0 = .99832429845280;      /* 1 - es2 / 4.0 - 3.0 * es4 / 64.0 - 5.0 * es6 / 256.0 */
static double c1 = .0025146070605187; /* 3.0 * es2 / 8.0 + 3.0 * es4 / 32.0 + 45.0 * es6 / 1024.0 */
static double c2 = 2.6390465943377e-006;             /* 15.0 * es4 / 256.0 + 45.0 * es6 / 1024.0 */
static double c3 = 3.4180460865959e-009;               /* 35.0 * es6 / 3072.0 */

/* Polyconic projection Parameters */
static double Poly_Origin_Lat = 0.0;                   /* Latitude of origin in radians     */
static double Poly_Origin_Long = 0.0;                  /* Longitude of origin in radians    */
static double Poly_False_Northing = 0.0;               /* False northing in meters          */
static double Poly_False_Easting = 0.0;                /* False easting in meters           */

/* Maximum variance for easting and northing values for WGS 84.
 */
static double Poly_Max_Easting = 20037509.0;
static double Poly_Max_Northing = 15348215.0;
static double Poly_Min_Easting = -20037509.0;
static double Poly_Min_Northing = -15348215.0;

/*
 * These state variables are for optimization purposes.  The only function
 * that should modify them is Set_Polyconic_Parameters.
 */


/***************************************************************************/
/*
 *                              FUNCTIONS     
 */


long Set_Polyconic_Parameters(double a,
                              double f,
                              double Origin_Latitude,
                              double Central_Meridian,
                              double False_Easting,
                              double False_Northing)

{ /* BEGIN Set_Polyconic_Parameters */
/*
 * The function Set_Polyconic_Parameters receives the ellipsoid parameters and
 * Polyconic projection parameters as inputs, and sets the corresponding state
 * variables.  If any errors occur, the error code(s) are returned by the function, 
 * otherwise POLY_NO_ERROR is returned.
 *
 *    a                 : Semi-major axis of ellipsoid, in meters   (input)
 *    f                 : Flattening of ellipsoid						        (input)
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
  double lat, sin2lat, sin4lat, sin6lat;
  double temp;
  double inv_f = 1 / f;
  long Error_Code = POLY_NO_ERROR;

  if (a <= 0.0)
  { /* Semi-major axis must be greater than zero */
    Error_Code |= POLY_A_ERROR;
  }
  if ((inv_f < 250) || (inv_f > 350))
  { /* Inverse flattening must be between 250 and 350 */
    Error_Code |= POLY_INV_F_ERROR;
  }
  if ((Origin_Latitude < -PI_OVER_2) || (Origin_Latitude > PI_OVER_2))
  { /* origin latitude out of range */
    Error_Code |= POLY_ORIGIN_LAT_ERROR;
  }
  if ((Central_Meridian < -PI) || (Central_Meridian > TWO_PI))
  { /* origin longitude out of range */
    Error_Code |= POLY_CENT_MER_ERROR;
  }
  if (!Error_Code)
  { /* no errors */
    Poly_a = a;
    Poly_f = f;
    Poly_Origin_Lat = Origin_Latitude;
    if (Central_Meridian > PI)
      Central_Meridian -= TWO_PI;
    Poly_Origin_Long = Central_Meridian;
    Poly_False_Northing = False_Northing;
    Poly_False_Easting = False_Easting;
    es2 = 2 * Poly_f - Poly_f * Poly_f;
    es4 = es2 * es2;
    es6 = es4 * es2;

    j = 45.0 * es6 / 1024.0;
    three_es4 = 3.0 * es4;
    c0 = 1.0 - es2 / 4.0 - three_es4 / 64.0 - 5.0 * es6 / 256.0;
    c1 = 3.0 * es2 / 8.0 + three_es4 / 32.0 + j;
    c2 = 15.0 * es4 / 256.0 + j;
    c3 = 35.0 * es6 / 3072.0;

    lat = c0 * Poly_Origin_Lat;
    sin2lat = POLY_COEFF_TIMES_SIN(c1, 2.0, Poly_Origin_Lat);
    sin4lat = POLY_COEFF_TIMES_SIN(c2, 4.0, Poly_Origin_Lat);
    sin6lat = POLY_COEFF_TIMES_SIN(c3, 6.0, Poly_Origin_Lat);
    M0 = POLY_M(lat, sin2lat, sin4lat, sin6lat);

    if (Poly_Origin_Long > 0)
    {
      Convert_Geodetic_To_Polyconic(FOURTY_ONE, Poly_Origin_Long - PI, &temp, &Poly_Max_Northing);
      Convert_Geodetic_To_Polyconic(-FOURTY_ONE, Poly_Origin_Long - PI, &temp, &Poly_Min_Northing);
      Poly_Max_Easting = 19926189.0;
      Poly_Min_Easting = -20037509.0;
    }
    else if (Poly_Origin_Long < 0)
    {
      Convert_Geodetic_To_Polyconic(FOURTY_ONE, Poly_Origin_Long + PI, &temp, &Poly_Max_Northing);
      Convert_Geodetic_To_Polyconic(-FOURTY_ONE, Poly_Origin_Long + PI, &temp, &Poly_Min_Northing);
      Poly_Max_Easting = 20037509.0;
      Poly_Min_Easting = -19926189.0;
    }
    else
    {
      Convert_Geodetic_To_Polyconic(FOURTY_ONE, PI, &temp, &Poly_Max_Northing);
      Convert_Geodetic_To_Polyconic(-FOURTY_ONE, PI, &temp, &Poly_Min_Northing);
      Poly_Max_Easting = 20037509.0;
      Poly_Min_Easting = -20037509.0;
    }

  } /* END OF if(!Error_Code) */
  return (Error_Code);
} /* END OF Set_Polyconic_Parameters */

void Get_Polyconic_Parameters(double *a,
                              double *f,
                              double *Origin_Latitude,
                              double *Central_Meridian,
                              double *False_Easting,
                              double *False_Northing)

{ /* BEGIN Get_Polyconic_Parameters */
/*
 * The function Get_Polyconic_Parameters returns the current ellipsoid
 * parameters, and Polyconic projection parameters.
 *
 *    a                 : Semi-major axis of ellipsoid, in meters   (output)
 *    f                 : Flattening of ellipsoid						        (output)
 *    Origin_Latitude   : Latitude in radians at which the          (output)
 *                          point scale factor is 1.0
 *    Central_Meridian  : Longitude in radians at the center of     (output)
 *                          the projection
 *    False_Easting     : A coordinate value in meters assigned to the
 *                          central meridian of the projection.     (output)
 *    False_Northing    : A coordinate value in meters assigned to the
 *                          origin latitude of the projection       (output)
 */

  *a = Poly_a;
  *f = Poly_f;
  *Origin_Latitude = Poly_Origin_Lat;
  *Central_Meridian = Poly_Origin_Long;
  *False_Easting = Poly_False_Easting;
  *False_Northing = Poly_False_Northing;
  return;
} /* END OF Get_Polyconic_Parameters */


long Convert_Geodetic_To_Polyconic (double Latitude,
                                    double Longitude,
                                    double *Easting,
                                    double *Northing)
{ /* BEGIN Convert_Geodetic_To_Polyconic */
/*
 * The function Convert_Geodetic_To_Polyconic converts geodetic (latitude and
 * longitude) coordinates to Polyconic projection (easting and northing)
 * coordinates, according to the current ellipsoid and Polyconic projection
 * parameters.  If any errors occur, the error code(s) are returned by the
 * function, otherwise POLY_NO_ERROR is returned.
 *
 *    Latitude          : Latitude (phi) in radians           (input)
 *    Longitude         : Longitude (lambda) in radians       (input)
 *    Easting           : Easting (X) in meters               (output)
 *    Northing          : Northing (Y) in meters              (output)
 */

  double slat = sin(Latitude);
  double lat, sin2lat, sin4lat, sin6lat;
  double dlam;                      /* Longitude - Central Meridan */
  double NN;
  double NN_OVER_tlat;
  double MM;
  double EE;
  long Error_Code = POLY_NO_ERROR;

  if ((Latitude < -PI_OVER_2) || (Latitude > PI_OVER_2))
  { /* Latitude out of range */
    Error_Code |= POLY_LAT_ERROR;
  }
  if ((Longitude < -PI) || (Longitude > TWO_PI))
  { /* Longitude out of range */
    Error_Code |= POLY_LON_ERROR;
  }
  if (!Error_Code)
  { /* no errors */
    dlam = Longitude - Poly_Origin_Long;
    if (fabs(dlam) > (PI / 2))
    { /* Distortion will result if Longitude is more than 90 degrees from the Central Meridian */
      Error_Code |= POLY_LON_WARNING;
    }
    if (dlam > PI)
    {
      dlam -= TWO_PI;
    }
    if (dlam < -PI)
    {
      dlam += TWO_PI;
    }
    if (Latitude == 0.0)
    {
      *Easting = Poly_a * dlam + Poly_False_Easting;
      *Northing = -M0 + Poly_False_Northing;
    }
    else
    {
      NN = Poly_a / sqrt(1.0 - es2 * (slat * slat));
      NN_OVER_tlat = NN  / tan(Latitude);
      lat = c0 * Latitude;
      sin2lat = POLY_COEFF_TIMES_SIN(c1, 2.0, Latitude);
      sin4lat = POLY_COEFF_TIMES_SIN(c2, 4.0, Latitude);
      sin6lat = POLY_COEFF_TIMES_SIN(c3, 6.0, Latitude);
      MM = POLY_M(lat, sin2lat, sin4lat, sin6lat);
      EE = dlam * slat;
      *Easting = NN_OVER_tlat * sin(EE) + Poly_False_Easting;
      *Northing = MM - M0 + NN_OVER_tlat * (1.0 - cos(EE)) +
                  Poly_False_Northing;
    }
  }
  return (Error_Code);
} /* END OF Convert_Geodetic_To_Polyconic */


long Convert_Polyconic_To_Geodetic(double Easting,
                                   double Northing,
                                   double *Latitude,
                                   double *Longitude)
{ /* BEGIN Convert_Polyconic_To_Geodetic */
/*
 * The function Convert_Polyconic_To_Geodetic converts Polyconic projection
 * (easting and northing) coordinates to geodetic (latitude and longitude)
 * coordinates, according to the current ellipsoid and Polyconic projection
 * coordinates.  If any errors occur, the error code(s) are returned by the
 * function, otherwise POLY_NO_ERROR is returned.
 *
 *    Easting           : Easting (X) in meters                  (input)
 *    Northing          : Northing (Y) in meters                 (input)
 *    Latitude          : Latitude (phi) in radians              (output)
 *    Longitude         : Longitude (lambda) in radians          (output)
 */

  double dx;     /* Delta easting - Difference in easting (easting-FE)      */
  double dy;     /* Delta northing - Difference in northing (northing-FN)   */
  double dx_OVER_Poly_a;
  double AA;
  double BB;
  double CC = 0.0;
  double PHIn, Delta_PHI = 1.0;
  double sin_PHIn;
  double PHI, sin2PHI,sin4PHI, sin6PHI;
  double Mn, Mn_prime, Ma;
  double AA_Ma;
  double Ma2_PLUS_BB;
  double AA_MINUS_Ma;
  double tolerance = 1.0e-12;        /* approximately 1/1000th of
                               an arc second or 1/10th meter */
  long Error_Code = POLY_NO_ERROR;

  if ((Easting < (Poly_False_Easting + Poly_Min_Easting))
      || (Easting > (Poly_False_Easting + Poly_Max_Easting)))
  { /* Easting out of range */
    Error_Code |= POLY_EASTING_ERROR;
  }
  if ((Northing < (Poly_False_Northing + Poly_Min_Northing))
      || (Northing > (Poly_False_Northing + Poly_Max_Northing)))
  { /* Northing out of range */
    Error_Code |= POLY_NORTHING_ERROR;
  }
  if (!Error_Code)
  { /* no errors */
    dy = Northing - Poly_False_Northing;
    dx = Easting - Poly_False_Easting;
    dx_OVER_Poly_a = dx / Poly_a;
    if (FLOAT_EQ(dy,-M0,1))
    {
      *Latitude = 0.0;
      *Longitude = dx_OVER_Poly_a + Poly_Origin_Long;
    }
    else
    {
      AA = (M0 + dy) / Poly_a;
      BB = dx_OVER_Poly_a * dx_OVER_Poly_a + (AA * AA);
      PHIn = AA;

      while (fabs(Delta_PHI) > tolerance)
      {
        sin_PHIn = sin(PHIn);
        CC = sqrt(1.0 - es2 * sin_PHIn * sin_PHIn) * tan(PHIn);
        PHI = c0 * PHIn;
        sin2PHI = POLY_COEFF_TIMES_SIN(c1, 2.0, PHIn);
        sin4PHI = POLY_COEFF_TIMES_SIN(c2, 4.0, PHIn);
        sin6PHI = POLY_COEFF_TIMES_SIN(c3, 6.0, PHIn);
        Mn = POLY_M(PHI, sin2PHI, sin4PHI, sin6PHI);
        Mn_prime = c0 - 2.0 * c1 * cos(2.0 * PHIn) + 4.0 * c2 * cos(4.0 * PHIn) - 
                   6.0 * c3 * cos(6.0 * PHIn);  
        Ma = Mn / Poly_a;
        AA_Ma = AA * Ma;
        Ma2_PLUS_BB = Ma * Ma + BB;
        AA_MINUS_Ma = AA - Ma;
        Delta_PHI = (AA_Ma * CC + AA_MINUS_Ma - 0.5 * (Ma2_PLUS_BB) * CC) /
                    (es2 * sin2PHI * (Ma2_PLUS_BB - 2.0 * AA_Ma) /
                     4.0 * CC + (AA_MINUS_Ma) * (CC * Mn_prime - 2.0 / sin2PHI) - Mn_prime);
        PHIn -= Delta_PHI;
      }
      *Latitude = PHIn;

      if (*Latitude > PI_OVER_2)  /* force distorted values to 90, -90 degrees */
        *Latitude = PI_OVER_2;
      else if (*Latitude < -PI_OVER_2)
        *Latitude = -PI_OVER_2;

      if (FLOAT_EQ(fabs(*Latitude),PI_OVER_2,.00001) || (*Latitude == 0))
        *Longitude = Poly_Origin_Long;

      else
      {
        *Longitude = (asin(dx_OVER_Poly_a * CC)) / sin(*Latitude) +
                     Poly_Origin_Long;
      }
    }
    if (*Longitude > PI)
      *Longitude -= TWO_PI;
    if (*Longitude < -PI)
      *Longitude += TWO_PI;

    if (*Longitude > PI)  /* force distorted values to 180, -180 degrees */
      *Longitude = PI;
    else if (*Longitude < -PI)
      *Longitude = -PI;

  }
  return (Error_Code);
} /* END OF Convert_Polyconic_To_Geodetic */
