/***************************************************************************/
/* RSC IDENTIFIER: CASSINI
 *
 * ABSTRACT
 *
 *    This component provides conversions between Geodetic coordinates
 *    (latitude and longitude in radians) and Cassini projection coordinates
 *    (easting and northing in meters).
 *
 * ERROR HANDLING
 *
 *    This component checks parameters for valid values.  If an invalid value
 *    is found, the error code is combined with the current error code using 
 *    the bitwise or.  This combining allows multiple error codes to be
 *    returned. The possible error codes are:
 *
 *          CASS_NO_ERROR           : No errors occurred in function
 *          CASS_LAT_ERROR          : Latitude outside of valid range
 *                                      (-90 to 90 degrees)
 *          CASS_LON_ERROR          : Longitude outside of valid range
 *                                      (-180 to 360 degrees)
 *          CASS_EASTING_ERROR      : Easting outside of valid range
 *                                      (False_Easting +/- ~20,000,000 m,
 *                                       depending on ellipsoid parameters
 *                                       and Origin_Latitude)
 *          CASS_NORTHING_ERROR     : Northing outside of valid range
 *                                      (False_Northing +/- ~57,000,000 m,
 *                                       depending on ellipsoid parameters
 *                                       and Origin_Latitude)
 *          CASS_ORIGIN_LAT_ERROR   : Origin latitude outside of valid range
 *                                      (-90 to 90 degrees)
 *          CASS_CENT_MER_ERROR     : Central meridian outside of valid range
 *                                      (-180 to 360 degrees)
 *          CASS_A_ERROR            : Semi-major axis less than or equal to zero
 *          CASS_INV_F_ERROR        : Inverse flattening outside of valid range
 *									                    (250 to 350)
 *			CASS_LON_WARNING        : Distortion will result if longitude is more
 *                                      than 4 degrees from the Central Meridian
 *
 * REUSE NOTES
 *
 *    CASSINI is intended for reuse by any application that performs a
 *    Cassini projection or its inverse.
 *
 * REFERENCES
 *
 *    Further information on CASSINI can be found in the Reuse Manual.
 *
 *    CASSINI originated from :  U.S. Army Topographic Engineering Center
 *                               Geospatial Information Division
 *                               7701 Telegraph Road
 *                               Alexandria, VA  22310-3864
 *
 * LICENSES
 *
 *    None apply to this component.
 *
 * RESTRICTIONS
 *
 *    CASSINI has no restrictions.
 *
 * ENVIRONMENT
 *
 *    CASSINI was tested and certified in the following environments:
 *
 *    1. Solaris 2.5 with GCC 2.8.1
 *    2. MS Windows 95 with MS Visual C++ 6
 *
 * MODIFICATIONS
 *
 *    Date              Description
 *    ----              -----------
 *    04/16/99          Original Code
 *
 */


/***************************************************************************/
/*
 *                               INCLUDES
 */

#include <math.h>
#include "cassini.h"

/*
 *    math.h    - Standard C math library
 *    cassini.h - Is for prototype error checking.
 */


/***************************************************************************/
/*
 *                               DEFINES
 */

#define PI         3.14159265358979323e0  /* PI                            */
#define PI_OVER_2  ( PI / 2.0)                    
#define TWO_PI     (2.0 * PI)                  
#define CASS_M(c0lat, c1s2lat, c2s4lat, c3s6lat)  (Cass_a*(c0lat-c1s2lat+c2s4lat-c3s6lat))
#define CASS_RD(sinlat)                           (sqrt(1.0 - es2 * (sinlat * sinlat)))
#define CASS_COEFF_TIMES_SIN(coeff, x, latit)	  (coeff * (sin (x * latit)))
#define FLOAT_EQ(x,v,epsilon)   (((v - epsilon) < x) && (x < (v + epsilon)))
#define THIRTY_ONE  (31.0 * PI / 180)	  /* 31 degrees in radians */


/***************************************************************************/
/*
 *                               GLOBALS
 */

/* Ellipsoid Parameters, default to WGS 84 */
static double Cass_a = 6378137.0;                      /* Semi-major axis of ellipsoid in meters */
static double Cass_f = 1 / 298.257223563;              /* Flattening of ellipsoid */
static double es2 = 0.0066943799901413800;             /* Eccentricity (0.08181919084262188000) squared  */
static double es4 =4.4814723452405e-005;               /* es2 * es2 */
static double es6 = 3.0000678794350e-007;              /* es4 * es2 */
static double M0 = 0.0;
static double c0 = .99832429845280;                    /* 1 - es2 / 4.0 - 3.0 * es4 / 64.0 - 5.0 * es6 / 256.0 */
static double c1 = .0025146070605187;                  /* 3.0 * es2 / 8.0 + 3.0 * es4 / 32.0 + 45.0 * es6 / 1024.0 */
static double c2 = 2.6390465943377e-006;               /* 15.0 * es4 / 256.0 + 45.0 * es6 / 1024.0 */
static double c3 = 3.4180460865959e-009;               /* 35.0 * es6 / 3072.0 */
static double One_Minus_es2 = .99330562000986;         /* 1.0 - es2 */
static double a0 = .0025188265843907;                  /* 3.0 * e1 / 2.0 - 27.0 * e3 / 32.0 */
static double a1 = 3.7009490356205e-006;               /* 21.0 * e2 / 16.0 - 55.0 * e4 / 32.0 */
static double a2 = 7.4478137675038e-009;               /* 151.0 * e3 / 96.0 */
static double a3 = 1.7035993238596e-011;               /* 1097.0 * e4 /512.0 */

/* Cassini projection Parameters */
static double Cass_Origin_Lat = 0.0;                   /* Latitude of origin in radians     */
static double Cass_Origin_Long = 0.0;                  /* Longitude of origin in radians    */
static double Cass_False_Northing = 0.0;               /* False northing in meters          */
static double Cass_False_Easting = 0.0;                /* False easting in meters           */

/* Maximum variance for easting and northing values for WGS 84.
 */
static double Cass_Min_Easting = -20037508.4;
static double Cass_Max_Easting = 20037508.4;
static double Cass_Min_Northing = -56575846.0;
static double Cass_Max_Northing = 56575846.0;
/*
 * These state variables are for optimization purposes.  The only function
 * that should modify them is Set_Cassini_Parameters.
 */


/***************************************************************************/
/*
 *                              FUNCTIONS     
 */


long Set_Cassini_Parameters(double a,
                            double f,
                            double Origin_Latitude,
                            double Central_Meridian,
                            double False_Easting,
                            double False_Northing)
{ /* Begin Set_Cassini_Parameters */
/*
 * The function Set_Cassini_Parameters receives the ellipsoid parameters and
 * Cassini projection parameters as inputs, and sets the corresponding state
 * variables.  If any errors occur, the error code(s) are returned by the 
 * function, otherwise CASS_NO_ERROR is returned.
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

  double j,three_es4;
  double x, e1, e2, e3, e4;
  double lat, sin2lat, sin4lat, sin6lat;
  double temp;
  double inv_f = 1 / f;
  long Error_Code = CASS_NO_ERROR;

  if (a <= 0.0)
  { /* Semi-major axis must be greater than zero */
    Error_Code |= CASS_A_ERROR;
  }
  if ((inv_f < 250) || (inv_f > 350))
  { /* Inverse flattening must be between 250 and 350 */
    Error_Code |= CASS_INV_F_ERROR;
  }
  if ((Origin_Latitude < -PI_OVER_2) || (Origin_Latitude > PI_OVER_2))
  { /* origin latitude out of range */
    Error_Code |= CASS_ORIGIN_LAT_ERROR;
  }
  if ((Central_Meridian < -PI) || (Central_Meridian > TWO_PI))
  { /* origin longitude out of range */
    Error_Code |= CASS_CENT_MER_ERROR;
  }
  if (!Error_Code)
  { /* no errors */
    Cass_a = a;
    Cass_f = f;
    Cass_Origin_Lat = Origin_Latitude;
    if (Central_Meridian > PI)
      Central_Meridian -= TWO_PI;
    Cass_Origin_Long = Central_Meridian;
    Cass_False_Northing = False_Northing;
    Cass_False_Easting = False_Easting;
    es2 = 2 * Cass_f - Cass_f * Cass_f;
    es4 = es2 * es2;
    es6 = es4 * es2;
    j = 45.0 * es6 / 1024.0;
    three_es4 = 3.0 * es4;
    c0 = 1 - es2 / 4.0 - three_es4 / 64.0 - 5.0 * es6 / 256.0; 
    c1 = 3.0 * es2 /8.0 + three_es4 / 32.0 + j;
    c2 = 15.0 * es4 / 256.0 + j;
    c3 = 35.0 * es6 / 3072.0;
    lat = c0 * Cass_Origin_Lat;
    sin2lat = CASS_COEFF_TIMES_SIN(c1, 2.0, Cass_Origin_Lat);
    sin4lat = CASS_COEFF_TIMES_SIN(c2, 4.0, Cass_Origin_Lat);
    sin6lat = CASS_COEFF_TIMES_SIN(c3, 6.0, Cass_Origin_Lat);
    M0 = CASS_M(lat, sin2lat, sin4lat, sin6lat);

    One_Minus_es2 = 1.0 - es2;
    x = sqrt (One_Minus_es2);
    e1 = (1 - x) / (1 + x);
    e2 = e1 * e1;
    e3 = e2 * e1;
    e4 = e3 * e1;
    a0 = 3.0 * e1 / 2.0 - 27.0 * e3 / 32.0;
    a1 = 21.0 * e2 / 16.0 - 55.0 * e4 / 32.0;
    a2 = 151.0 * e3 / 96.0;
    a3 = 1097.0 * e4 /512.0;

    if (Cass_Origin_Long > 0)
    {
      Convert_Geodetic_To_Cassini(THIRTY_ONE, Cass_Origin_Long - PI, &temp, &Cass_Max_Northing);
      Convert_Geodetic_To_Cassini(-THIRTY_ONE, Cass_Origin_Long - PI, &temp, &Cass_Min_Northing); 
      Cass_Max_Easting = 19926188.9;
      Cass_Min_Easting = -20037508.4;
    }
    else if (Cass_Origin_Long < 0)
    {
      Convert_Geodetic_To_Cassini(THIRTY_ONE, PI + Cass_Origin_Long, &temp, &Cass_Max_Northing); 
      Convert_Geodetic_To_Cassini(-THIRTY_ONE, PI + Cass_Origin_Long, &temp, &Cass_Min_Northing);
      Cass_Max_Easting = 20037508.4;
      Cass_Min_Easting = -19926188.9;
    }
    else
    {
      Convert_Geodetic_To_Cassini(THIRTY_ONE, PI, &temp, &Cass_Max_Northing); 
      Convert_Geodetic_To_Cassini(-THIRTY_ONE, PI , &temp, &Cass_Min_Northing);
      Cass_Max_Easting = 20037508.4;
      Cass_Min_Easting = -20037508.4;
    }

  } /* End if(!Error_Code) */
  return (Error_Code);
} /* End Set_Cassini_Parameters */


void Get_Cassini_Parameters(double *a,
                            double *f,
                            double *Origin_Latitude,
                            double *Central_Meridian,
                            double *False_Easting,
                            double *False_Northing)
{ /* Begin Get_Cassini_Parameters */
/*
 * The function Get_Cassini_Parameters returns the current ellipsoid
 * parameters, Cassini projection parameters.
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

  *a = Cass_a;
  *f = Cass_f;
  *Origin_Latitude = Cass_Origin_Lat;
  *Central_Meridian = Cass_Origin_Long;
  *False_Easting = Cass_False_Easting;
  *False_Northing = Cass_False_Northing;
  return;
} /* End Get_Cassini_Parameters */


long Convert_Geodetic_To_Cassini (double Latitude,
                                  double Longitude,
                                  double *Easting,
                                  double *Northing)
{ /* Begin Convert_Geodetic_To_Cassini */
/*
 * The function Convert_Geodetic_To_Cassini converts geodetic (latitude and
 * longitude) coordinates to Cassini projection (easting and northing)
 * coordinates, according to the current ellipsoid and Cassini projection
 * parameters.  If any errors occur, the error code(s) are returned by the
 * function, otherwise CASS_NO_ERROR is returned.
 *
 *    Latitude          : Latitude (phi) in radians           (input)
 *    Longitude         : Longitude (lambda) in radians       (input)
 *    Easting           : Easting (X) in meters               (output)
 *    Northing          : Northing (Y) in meters              (output)
 */

  double lat, sin2lat, sin4lat, sin6lat;
  double RD;
  double tlat = tan(Latitude);
  double clat = cos(Latitude);
  double slat = sin(Latitude);
  double dlam;                      /* Longitude - Central Meridan */
  double NN;
  double TT;
  double AA, A2, A3, A4, A5;
  double CC;
  double MM;
  long Error_Code = CASS_NO_ERROR;

  if ((Latitude < -PI_OVER_2) || (Latitude > PI_OVER_2))
  { /* Latitude out of range */
    Error_Code |= CASS_LAT_ERROR;
  }
  if ((Longitude < -PI) || (Longitude > TWO_PI))
  { /* Longitude out of range */
    Error_Code |= CASS_LON_ERROR;
  }
  if (!Error_Code)
  { /* no errors */
    dlam = Longitude - Cass_Origin_Long;

    if (fabs(dlam) > (4.0 * PI / 180))
    { /* Distortion will result if Longitude is more than 4 degrees from the Central Meridian */
      Error_Code |= CASS_LON_WARNING;
    }

    if (dlam > PI)
    {
      dlam -= TWO_PI;
    }
    if (dlam < -PI)
    {
      dlam += TWO_PI;
    }
    RD = CASS_RD(slat);
    NN = Cass_a / RD;
    TT = tlat * tlat;
    AA = dlam * clat;
    A2 = AA * AA;
    A3 = AA * A2;
    A4 = AA * A3;
    A5 = AA * A4;
    CC = es2 * clat * clat / One_Minus_es2;
    lat = c0 * Latitude;
    sin2lat = CASS_COEFF_TIMES_SIN(c1, 2.0, Latitude);
    sin4lat = CASS_COEFF_TIMES_SIN(c2, 4.0, Latitude);
    sin6lat = CASS_COEFF_TIMES_SIN(c3, 6.0, Latitude);
    MM = CASS_M(lat, sin2lat, sin4lat, sin6lat);

    *Easting = NN * (AA - (TT * A3 / 6.0) - (8.0 - TT + 8.0 * CC) *
                     (TT * A5 / 120.0)) + Cass_False_Easting;
    *Northing = MM - M0 + NN * tlat * ((A2 / 2.0) + (5.0 - TT +
                                                     6.0 * CC) * A4 / 24.0) + Cass_False_Northing;
  }
  return (Error_Code);
} /* End Convert_Geodetic_To_Cassini */


long Convert_Cassini_To_Geodetic(double Easting,
                                 double Northing,
                                 double *Latitude,
                                 double *Longitude)
{ /* Begin Convert_Cassini_To_Geodetic */
/*
 * The function Convert_Cassini_To_Geodetic converts Cassini projection
 * (easting and northing) coordinates to geodetic (latitude and longitude)
 * coordinates, according to the current ellipsoid and Cassini projection
 * coordinates.  If any errors occur, the error code(s) are returned by the
 * function, otherwise CASS_NO_ERROR is returned.
 *
 *    Easting           : Easting (X) in meters                  (input)
 *    Northing          : Northing (Y) in meters                 (input)
 *    Latitude          : Latitude (phi) in radians              (output)
 *    Longitude         : Longitude (lambda) in radians          (output)
 */

  double dx;     /* Delta easting - Difference in easting (easting-FE)      */
  double dy;     /* Delta northing - Difference in northing (northing-FN)   */
  double mu1;
  double sin2mu, sin4mu, sin6mu, sin8mu;
  double M1;
  double phi1;
  double tanphi1, sinphi1, cosphi1;
  double T1, T;
  double N1;
  double RD, R1;
  double DD, D2, D3, D4, D5;
  const double epsilon = 1.0e-1;
  long Error_Code = CASS_NO_ERROR;

  if ((Easting < (Cass_False_Easting + Cass_Min_Easting))
      || (Easting > (Cass_False_Easting + Cass_Max_Easting)))
  { /* Easting out of range */
    Error_Code |= CASS_EASTING_ERROR;
  }
  if ((Northing < (Cass_False_Northing + Cass_Min_Northing - epsilon))
      || (Northing > (Cass_False_Northing + Cass_Max_Northing + epsilon)))
  { /* Northing out of range */
    Error_Code |= CASS_NORTHING_ERROR;
  }
  if (!Error_Code)
  { /* no errors */
    dy = Northing - Cass_False_Northing;
    dx = Easting - Cass_False_Easting;
    M1 = M0 + dy;
    mu1 = M1 / (Cass_a * c0); 

    sin2mu = CASS_COEFF_TIMES_SIN(a0, 2.0, mu1);
    sin4mu = CASS_COEFF_TIMES_SIN(a1, 4.0, mu1);
    sin6mu = CASS_COEFF_TIMES_SIN(a2, 6.0, mu1);
    sin8mu = CASS_COEFF_TIMES_SIN(a3, 8.0, mu1);
    phi1 = mu1 + sin2mu + sin4mu + sin6mu + sin8mu;

    if (FLOAT_EQ(phi1,PI_OVER_2,.00001))
    {
      *Latitude =  PI_OVER_2;
      *Longitude = Cass_Origin_Long;
    }
    else if (FLOAT_EQ(phi1,-PI_OVER_2,.00001))
    {
      *Latitude = -PI_OVER_2;
      *Longitude = Cass_Origin_Long;
    }
    else
    {
      tanphi1 = tan(phi1);
      sinphi1 = sin(phi1);
      cosphi1 = cos(phi1);
      T1 = tanphi1 * tanphi1;
      RD = CASS_RD(sinphi1);
      N1 = Cass_a / RD;
      R1 = N1 * One_Minus_es2 / (RD * RD);
      DD = dx / N1;
      D2 = DD * DD;
      D3 = D2 * DD;
      D4 = D3 * DD;
      D5 = D4 * DD;
      T = (1.0 + 3.0 * T1);
      *Latitude = phi1 - (N1 * tanphi1 / R1) * (D2 / 2.0 - T * D4 / 24.0);

      *Longitude = Cass_Origin_Long + (DD - T1 * D3 / 3.0 + T * T1 * D5 / 15.0) / cosphi1;

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
    if (fabs(*Longitude - Cass_Origin_Long) > (4.0 * PI / 180))
    { /* Distortion will result if Longitude is more than 4 degrees from the Central Meridian */
      Error_Code |= CASS_LON_WARNING;
    }
  }
  return (Error_Code);
} /* End Convert_Cassini_To_Geodetic */
