/***************************************************************************/
/* RSC IDENTIFIER: LAMBERT_1
 *
 * ABSTRACT
 *
 *    This component provides conversions between Geodetic coordinates
 *    (latitude and longitude in radians) and Lambert Conformal Conic
 *    (1 parallel) projection coordinates (easting and northing in meters) defined
 *    by one standard parallel.  
 *
 * ERROR HANDLING
 *
 *    This component checks parameters for valid values.  If an invalid value
 *    is found the error code is combined with the current error code using
 *    the bitwise or.  This combining allows multiple error codes to be
 *    returned. The possible error codes are:
 *
 *       LAMBERT_1_NO_ERROR           : No errors occurred in function
 *       LAMBERT_1_LAT_ERROR          : Latitude outside of valid range
 *                                      (-90 to 90 degrees)
 *       LAMBERT_1_LON_ERROR          : Longitude outside of valid range
 *                                      (-180 to 360 degrees)
 *       LAMBERT_1_EASTING_ERROR      : Easting outside of valid range
 *                                      (depends on ellipsoid and projection
 *                                     parameters)
 *       LAMBERT_1_NORTHING_ERROR     : Northing outside of valid range
 *                                      (depends on ellipsoid and projection
 *                                     parameters)
 *       LAMBERT_1_ORIGIN_LAT_ERROR   : Origin latitude outside of valid
 *                                      range (-89 59 59.0 to 89 59 59.0 degrees)
 *       LAMBERT_1_CENT_MER_ERROR     : Central meridian outside of valid range
 *                                      (-180 to 360 degrees)
 *		   LAMBERT_1_SCALE_FACTOR_ERROR : Scale factor outside of valid
 *                                      range (0.3 to 3.0)
 *       LAMBERT_1_A_ERROR            : Semi-major axis less than or equal to zero
 *       LAMBERT_1_INV_F_ERROR        : Inverse flattening outside of valid range
 *									                    (250 to 350)
 *
 *
 * REUSE NOTES
 *
 *    LAMBERT_1 is intended for reuse by any application that performs a Lambert
 *    Conformal Conic (1 parallel) projection or its inverse.
 *    
 * REFERENCES
 *
 *    Further information on LAMBERT_1 can be found in the Reuse Manual.
 *
 *    LAMBERT_1 originated from:
 *                      U.S. Army Topographic Engineering Center
 *                      Geospatial Information Division
 *                      7701 Telegraph Road
 *                      Alexandria, VA  22310-3864
 *
 * LICENSES
 *
 *    None apply to this component.
 *
 * RESTRICTIONS
 *
 *    LAMBERT_1 has no restrictions.
 *
 * ENVIRONMENT
 *
 *    LAMBERT_1 was tested and certified in the following environments:
 *
 *    1. Solaris 2.5 with GCC, version 2.8.1
 *    2. Windows 98/2000 with MS Visual C++, version 6
 *
 * MODIFICATIONS
 *
 *    Date              Description
 *    ----              -----------
 *    03-05-05          Original Code
 *
 *
 */


/***************************************************************************/
/*
 *                               INCLUDES
 */

#include <math.h>
#include "lambert_1.h"
/*
 *    math.h     - Standard C math library
 *    lambert_1.h  - Is for prototype error checking
 */


/***************************************************************************/
/*                               DEFINES
 *
 */

#define PI         3.14159265358979323e0   /* PI     */
#define PI_OVER_2  (PI / 2.0)
#define PI_OVER_4  (PI / 4.0)
#define MAX_LAT    (( PI *  89.99972222222222) / 180.0)  /* 89 59 59.0 degrees in radians */
#define TWO_PI     (2.0 * PI)
#define LAMBERT_m(clat,essin)                  (clat / sqrt(1.0 - essin * essin))
#define LAMBERT_t(lat,essin)                   tan(PI_OVER_4 - lat / 2) /				\
										            pow((1.0 - essin) / (1.0 + essin), es_OVER_2)
#define ES_SIN(sinlat)                         (es * sinlat)
#define MIN_SCALE_FACTOR  0.3
#define MAX_SCALE_FACTOR  3.0

/**************************************************************************/
/*                               GLOBAL DECLARATIONS
 *
 */

/* Ellipsoid Parameters, default to WGS 84  */
static double Lambert_1_a = 6378137.0;               /* Semi-major axis of ellipsoid, in meters */
static double Lambert_1_f = 1 / 298.257223563;       /* Flattening of ellipsoid */
static double es = 0.08181919084262188000;           /* Eccentricity of ellipsoid */
static double es_OVER_2 = .040909595421311;          /* Eccentricity / 2.0 */
static double Lambert_1_n = 0.70710678118655;        /* Ratio of angle between meridians */
static double Lambert_1_rho0 = 6388838.2901212;      /* Height above ellipsoid */
static double Lambert_1_rho_olat = 6388838.2901211;
static double Lambert_1_t0 = 0.41618115138974;

/* Lambert_Conformal_Conic projection Parameters */
static double Lambert_1_Origin_Lat = (45 * PI / 180);   /* Latitude of origin in radians */
static double Lambert_1_Origin_Long = 0.0;              /* Longitude of origin, in radians */
static double Lambert_1_False_Northing = 0.0;           /* False northing, in meters */
static double Lambert_1_False_Easting = 0.0;            /* False easting, in meters */
static double Lambert_1_Scale_Factor = 1.0;             /* Scale Factor */


/* Maximum variance for easting and northing values for WGS 84. */
static double Lambert_Delta_Easting = 40000000.0;
static double Lambert_Delta_Northing = 40000000.0;

/* These state variables are for optimization purposes. The only function
 * that should modify them is Set_Lambert_1_Parameters.         */


/************************************************************************/
/*                              FUNCTIONS
 *
 */

long Set_Lambert_1_Parameters(double a,
                              double f,
                              double Origin_Latitude,
                              double Central_Meridian,
                              double False_Easting,
                              double False_Northing,
                              double Scale_Factor)

{ /* BEGIN Set_Lambert_1_Parameters */
/*
 * The function Set_Lambert_1_Parameters receives the ellipsoid parameters and
 * Lambert Conformal Conic (1 parallel) projection parameters as inputs, and sets the
 * corresponding state variables.  If any errors occur, the error code(s)
 * are returned by the function, otherwise LAMBERT_1_NO_ERROR is returned.
 *
 *   a                   : Semi-major axis of ellipsoid, in meters   (input)
 *   f                   : Flattening of ellipsoid				     (input)
 *   Origin_Latitude     : Latitude of origin, in radians            (input)
 *   Central_Meridian    : Longitude of origin, in radians           (input)
 *   False_Easting       : False easting, in meters                  (input)
 *   False_Northing      : False northing, in meters                 (input)
 *   Scale_Factor        : Projection scale factor                   (input) 
 *
 */

  double es2;
  double es_sin;
  double m0;
  double inv_f = 1 / f;
  long Error_Code = LAMBERT_1_NO_ERROR;

  if (a <= 0.0)
  { /* Semi-major axis must be greater than zero */
    Error_Code |= LAMBERT_1_A_ERROR;
  }
  if ((inv_f < 250) || (inv_f > 350))
  { /* Inverse flattening must be between 250 and 350 */
    Error_Code |= LAMBERT_1_INV_F_ERROR;
  }
  if (((Origin_Latitude < -MAX_LAT) || (Origin_Latitude > MAX_LAT)) ||
       (Origin_Latitude == 0))
  { /* Origin Latitude out of range */
    Error_Code |= LAMBERT_1_ORIGIN_LAT_ERROR;
  }
  if ((Central_Meridian < -PI) || (Central_Meridian > TWO_PI))
  { /* Origin Longitude out of range */
    Error_Code |= LAMBERT_1_CENT_MER_ERROR;
  }
  if ((Scale_Factor < MIN_SCALE_FACTOR) || (Scale_Factor > MAX_SCALE_FACTOR))
  {
    Error_Code |= LAMBERT_1_SCALE_FACTOR_ERROR;
  }

  if (!Error_Code)
  { /* no errors */

    Lambert_1_a = a;
    Lambert_1_f = f;
    Lambert_1_Origin_Lat = Origin_Latitude;
    if (Central_Meridian > PI)
      Central_Meridian -= TWO_PI;
    Lambert_1_Origin_Long = Central_Meridian;
    Lambert_1_False_Easting = False_Easting;
    Lambert_1_False_Northing = False_Northing;
    Lambert_1_Scale_Factor = Scale_Factor;

    es2 = 2.0 * Lambert_1_f - Lambert_1_f * Lambert_1_f;
    es = sqrt(es2);
    es_OVER_2 = es / 2.0;

    Lambert_1_n = sin(Lambert_1_Origin_Lat);

    es_sin = ES_SIN(sin(Lambert_1_Origin_Lat));
    m0 = LAMBERT_m(cos(Lambert_1_Origin_Lat), es_sin);
    Lambert_1_t0 = LAMBERT_t(Lambert_1_Origin_Lat, es_sin);

    Lambert_1_rho0 = Lambert_1_a * Lambert_1_Scale_Factor * m0 / Lambert_1_n;

    Lambert_1_rho_olat = Lambert_1_rho0;
  }
  return (Error_Code);
} /* END OF Set_Lambert_1_Parameters */


void Get_Lambert_1_Parameters(double *a,
                              double *f,
                              double *Origin_Latitude,
                              double *Central_Meridian,
                              double *False_Easting,
                              double *False_Northing,
                              double *Scale_Factor)

{ /* BEGIN Get_Lambert_1_Parameters */
/*                         
 * The function Get_Lambert_1_Parameters returns the current ellipsoid
 * parameters and Lambert Conformal Conic (1 parallel) projection parameters.
 *
 *   a                   : Semi-major axis of ellipsoid, in meters   (output)
 *   f                   : Flattening of ellipsoid					 (output)
 *   Origin_Latitude     : Latitude of origin, in radians            (output)
 *   Central_Meridian    : Longitude of origin, in radians           (output)
 *   False_Easting       : False easting, in meters                  (output)
 *   False_Northing      : False northing, in meters                 (output)
 *   Scale_Factor        : Projection scale factor                   (output) 
 */


  *a = Lambert_1_a;
  *f = Lambert_1_f;
  *Origin_Latitude = Lambert_1_Origin_Lat;
  *Central_Meridian = Lambert_1_Origin_Long;
  *False_Easting = Lambert_1_False_Easting;
  *False_Northing = Lambert_1_False_Northing;
  *Scale_Factor = Lambert_1_Scale_Factor;
  return;
} /* END OF Get_Lambert_1_Parameters */


long Convert_Geodetic_To_Lambert_1 (double Latitude,
                                    double Longitude,
                                    double *Easting,
                                    double *Northing)

{ /* BEGIN Convert_Geodetic_To_Lambert_1 */
/*
 * The function Convert_Geodetic_To_Lambert_1 converts Geodetic (latitude and
 * longitude) coordinates to Lambert Conformal Conic (1 parallel) projection (easting
 * and northing) coordinates, according to the current ellipsoid and
 * Lambert Conformal Conic (1 parallel) projection parameters.  If any errors occur, the
 * error code(s) are returned by the function, otherwise LAMBERT_NO_ERROR is
 * returned.
 *
 *    Latitude         : Latitude, in radians                         (input)
 *    Longitude        : Longitude, in radians                        (input)
 *    Easting          : Easting (X), in meters                       (output)
 *    Northing         : Northing (Y), in meters                      (output)
 */

  double t;
  double rho;
  double dlam;
  double theta;
  long  Error_Code = LAMBERT_1_NO_ERROR;

  if ((Latitude < -PI_OVER_2) || (Latitude > PI_OVER_2))
  {  /* Latitude out of range */
    Error_Code|= LAMBERT_1_LAT_ERROR;
  }
  if ((Longitude < -PI) || (Longitude > TWO_PI))
  {  /* Longitude out of range */
    Error_Code|= LAMBERT_1_LON_ERROR;
  }

  if (!Error_Code)
  { /* no errors */

    if (fabs(fabs(Latitude) - PI_OVER_2) > 1.0e-10)
    {
      t = LAMBERT_t(Latitude, ES_SIN(sin(Latitude)));
      rho = Lambert_1_rho0 * pow(t / Lambert_1_t0, Lambert_1_n);
    }
    else
    {
      if ((Latitude * Lambert_1_n) <= 0)
      { /* Point can not be projected */
        Error_Code |= LAMBERT_1_LAT_ERROR;
        return (Error_Code);
      }
      rho = 0.0;
    }

    dlam = Longitude - Lambert_1_Origin_Long;

    if (dlam > PI)
    {
      dlam -= TWO_PI;
    }
    if (dlam < -PI)
    {
      dlam += TWO_PI;
    }

    theta = Lambert_1_n * dlam;

    *Easting = rho * sin(theta) + Lambert_1_False_Easting;
    *Northing = Lambert_1_rho_olat - rho * cos(theta) + Lambert_1_False_Northing;

  }
  return (Error_Code);
} /* END OF Convert_Geodetic_To_Lambert */



long Convert_Lambert_1_To_Geodetic (double Easting,
                                    double Northing,
                                    double *Latitude,
                                    double *Longitude)

{ /* BEGIN Convert_Lambert_1_To_Geodetic */
/*
 * The function Convert_Lambert_1_To_Geodetic converts Lambert Conformal
 * Conic (1 parallel) projection (easting and northing) coordinates to Geodetic
 * (latitude and longitude) coordinates, according to the current ellipsoid
 * and Lambert Conformal Conic (1 parallel) projection parameters.  If any errors occur,
 * the error code(s) are returned by the function, otherwise LAMBERT_NO_ERROR
 * is returned.
 *
 *    Easting          : Easting (X), in meters                       (input)
 *    Northing         : Northing (Y), in meters                      (input)
 *    Latitude         : Latitude, in radians                         (output)
 *    Longitude        : Longitude, in radians                        (output)
 */


  double dx;
  double dy;
  double rho;
  double rho_olat_MINUS_dy;
  double t;
  double PHI;
  double es_sin;
  double tempPHI = 0.0;
  double theta = 0.0;
  double tolerance = 4.85e-10;
  int count = 30;
  long Error_Code = LAMBERT_1_NO_ERROR;

  if ((Easting < (Lambert_1_False_Easting - Lambert_Delta_Easting))
      ||(Easting > (Lambert_1_False_Easting + Lambert_Delta_Easting)))
  { /* Easting out of range  */
    Error_Code |= LAMBERT_1_EASTING_ERROR;
  }
  if ((Northing < (Lambert_1_False_Northing - Lambert_Delta_Northing))
      || (Northing > (Lambert_1_False_Northing + Lambert_Delta_Northing)))
  { /* Northing out of range */
    Error_Code |= LAMBERT_1_NORTHING_ERROR;
  }

  if (!Error_Code)
  { /* no errors */

    dy = Northing - Lambert_1_False_Northing;
    dx = Easting - Lambert_1_False_Easting;
    rho_olat_MINUS_dy = Lambert_1_rho_olat - dy;
    rho = sqrt(dx * dx + (rho_olat_MINUS_dy) * (rho_olat_MINUS_dy));

    if (Lambert_1_n < 0.0)
    {
      rho *= -1.0;
      dx *= -1.0;
      rho_olat_MINUS_dy *= -1.0;
    }

    if (rho != 0.0)
    {
      theta = atan2(dx, rho_olat_MINUS_dy) / Lambert_1_n;
      t = Lambert_1_t0 * pow(rho / Lambert_1_rho0, 1 / Lambert_1_n);
      PHI = PI_OVER_2 - 2.0 * atan(t);
      while (fabs(PHI - tempPHI) > tolerance && count)
      {
        tempPHI = PHI;
        es_sin = ES_SIN(sin(PHI));
        PHI = PI_OVER_2 - 2.0 * atan(t * pow((1.0 - es_sin) / (1.0 + es_sin), es_OVER_2));
        count --;
      }

      if(!count)
        return Error_Code |= LAMBERT_1_NORTHING_ERROR;

      *Latitude = PHI;
      *Longitude = theta + Lambert_1_Origin_Long;

      if (fabs(*Latitude) < 2.0e-7)  /* force lat to 0 to avoid -0 degrees */
        *Latitude = 0.0;
      if (*Latitude > PI_OVER_2)  /* force distorted values to 90, -90 degrees */
        *Latitude = PI_OVER_2;
      else if (*Latitude < -PI_OVER_2)
        *Latitude = -PI_OVER_2;

      if (*Longitude > PI)
      {
        if (*Longitude - PI < 3.5e-6) 
          *Longitude = PI;
        else
          *Longitude -= TWO_PI;
      }
      if (*Longitude < -PI)
      {
        if (fabs(*Longitude + PI) < 3.5e-6)
          *Longitude = -PI;
        else
          *Longitude += TWO_PI;
      }

      if (fabs(*Longitude) < 2.0e-7)  /* force lon to 0 to avoid -0 degrees */
        *Longitude = 0.0;
      if (*Longitude > PI)  /* force distorted values to 180, -180 degrees */
        *Longitude = PI;
      else if (*Longitude < -PI)
        *Longitude = -PI;
    }
    else
    {
      if (Lambert_1_n > 0.0)
        *Latitude = PI_OVER_2;
      else
        *Latitude = -PI_OVER_2;
      *Longitude = Lambert_1_Origin_Long;
    }
  }
  return (Error_Code);
} /* END OF Convert_Lambert_1_To_Geodetic */



