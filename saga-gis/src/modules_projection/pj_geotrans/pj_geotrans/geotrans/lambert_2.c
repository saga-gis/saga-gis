/**********************************************************
 * Version $Id$
 *********************************************************/
/***************************************************************************/
/* RSC IDENTIFIER: LAMBERT_2
 *
 * ABSTRACT
 *
 *    This component provides conversions between Geodetic coordinates
 *    (latitude and longitude in radians) and Lambert Conformal Conic
 *    (2 parallel) projection coordinates (easting and northing in meters) defined
 *    by two standard parallels.  When both standard parallel parameters
 *    are set to the same latitude value, the result is a Lambert 
 *    Conformal Conic projection with one standard parallel at the 
 *    specified latitude.
 *
 * ERROR HANDLING
 *
 *    This component checks parameters for valid values.  If an invalid value
 *    is found the error code is combined with the current error code using
 *    the bitwise or.  This combining allows multiple error codes to be
 *    returned. The possible error codes are:
 *
 *       LAMBERT_2_NO_ERROR           : No errors occurred in function
 *       LAMBERT_2_LAT_ERROR          : Latitude outside of valid range
 *                                      (-90 to 90 degrees)
 *       LAMBERT_2_LON_ERROR          : Longitude outside of valid range
 *                                      (-180 to 360 degrees)
 *       LAMBERT_2_EASTING_ERROR      : Easting outside of valid range
 *                                      (depends on ellipsoid and projection
 *                                      parameters)
 *       LAMBERT_2_NORTHING_ERROR     : Northing outside of valid range
 *                                      (depends on ellipsoid and projection
 *                                      parameters)
 *       LAMBERT_2_FIRST_STDP_ERROR   : First standard parallel outside of valid
 *                                      range (-89 59 59.0 to 89 59 59.0 degrees)
 *       LAMBERT_2_SECOND_STDP_ERROR  : Second standard parallel outside of valid
 *                                      range (-89 59 59.0 to 89 59 59.0 degrees)
 *       LAMBERT_2_ORIGIN_LAT_ERROR   : Origin latitude outside of valid range
 *                                      (-89 59 59.0 to 89 59 59.0 degrees)
 *       LAMBERT_2_CENT_MER_ERROR     : Central meridian outside of valid range
 *                                      (-180 to 360 degrees)
 *       LAMBERT_2_A_ERROR            : Semi-major axis less than or equal to zero
 *       LAMBERT_2_INV_F_ERROR        : Inverse flattening outside of valid range
 *									    (250 to 350)
 *       LAMBERT_2_HEMISPHERE_ERROR   : Standard parallels cannot be opposite latitudes
 *       LAMBERT_2_FIRST_SECOND_ERROR : The 1st & 2nd standard parallels cannot
 *                                      both be 0
 *
 *
 * REUSE NOTES
 *
 *    LAMBERT_2 is intended for reuse by any application that performs a Lambert
 *    Conformal Conic (2 parallel) projection or its inverse.
 *    
 * REFERENCES
 *
 *    Further information on LAMBERT_2 can be found in the Reuse Manual.
 *
 *    LAMBERT_2 originated from:
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
 *    LAMBERT_2 has no restrictions.
 *
 * ENVIRONMENT
 *
 *    LAMBERT_2 was tested and certified in the following environments:
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
#include "lambert_2.h"
#include "lambert_1.h"
/*
 *    math.h     - Standard C math library
 *    lambert.h  - Is for prototype error checking
 *    lambert_1.h  - Is called to do conversion
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
#define LAMBERT_t(lat,essin)                   tan(PI_OVER_4 - lat / 2) *				\
										            pow((1.0 + essin) / (1.0 - essin), es_OVER_2)
#define ES_SIN(sinlat)                         (es * sinlat)

/**************************************************************************/
/*                               GLOBAL DECLARATIONS
 *
 */

/* Ellipsoid Parameters, default to WGS 84  */
static double Lambert_a = 6378137.0;                    /* Semi-major axis of ellipsoid, in meters */
static double Lambert_f = 1 / 298.257223563;            /* Flattening of ellipsoid */
static double es = 0.081819190842621;                   /* Eccentricity of ellipsoid */
static double es_OVER_2 = 0.040909595421311;            /* Eccentricity / 2.0 */
static double Lambert_lat0 = 0.78669154042193;          /* Calculated origin latitude */
static double Lambert_k0 = 0.99620424745181;            /* Calculated scale factor */
static double Lambert_false_northing = 8204.2214438468; /* Calculated false northing */

 /* Lambert_Conformal_Conic projection Parameters */
static double Lambert_Std_Parallel_1 = (40 * PI / 180); /* Lower std. parallel, in radians */
static double Lambert_Std_Parallel_2 = (50 * PI / 180); /* Upper std. parallel, in radians */
static double Lambert_Origin_Lat = (45 * PI / 180);     /* Latitude of origin, in radians */
static double Lambert_Origin_Long = 0.0;                /* Longitude of origin, in radians */
static double Lambert_False_Northing = 0.0;             /* False northing, in meters */
static double Lambert_False_Easting = 0.0;              /* False easting, in meters */

/* Maximum variance for easting and northing values for WGS 84. */
static double Lambert_Delta_Easting = 40000000.0;
static double Lambert_Delta_Northing = 40000000.0;

/* These state variables are for optimization purposes. The only function
 * that should modify them is Set_Lambert_Parameters.         */


/************************************************************************/
/*                              FUNCTIONS
 *
 */

long Set_Lambert_2_Parameters(double a,
                              double f,
                              double Origin_Latitude,
                              double Central_Meridian,
                              double Std_Parallel_1,
                              double Std_Parallel_2,
                              double False_Easting,
                              double False_Northing)

{ /* BEGIN Set_Lambert_2_Parameters */
/*
 * The function Set_Lambert_2_Parameters receives the ellipsoid parameters and
 * Lambert Conformal Conic (2 parallel) projection parameters as inputs, and sets the
 * corresponding state variables.  If any errors occur, the error code(s)
 * are returned by the function, otherwise LAMBERT_2_NO_ERROR is returned.
 *
 *   a                   : Semi-major axis of ellipsoid, in meters   (input)
 *   f                   : Flattening of ellipsoid				     (input)
 *   Central_Meridian    : Longitude of origin, in radians           (input)
 *   Std_Parallel_1      : First standard parallel, in radians       (input)
 *   Std_Parallel_2      : Second standard parallel, in radians      (input)
 *   False_Easting       : False easting, in meters                  (input)
 *   False_Northing      : False northing, in meters                 (input)
 *
 *   Note that when the two standard parallel parameters are both set to the 
 *   same latitude value, the result is a Lambert Conformal Conic projection 
 *   with one standard parallel at the specified latitude.
 */

  double es2;
  double es_sin;
  double t0;
  double t1;
  double t2;
  double t_olat;
  double m0;
  double m1;
  double m2;
  double m_olat;
  double n;                                /* Ratio of angle between meridians */
  double const_value;
  double inv_f = 1 / f;
  long Error_Code = LAMBERT_2_NO_ERROR;

  if (a <= 0.0)
  { /* Semi-major axis must be greater than zero */
    Error_Code |= LAMBERT_2_A_ERROR;
  }
  if ((inv_f < 250) || (inv_f > 350))
  { /* Inverse flattening must be between 250 and 350 */
    Error_Code |= LAMBERT_2_INV_F_ERROR;
  }
  if ((Origin_Latitude < -MAX_LAT) || (Origin_Latitude > MAX_LAT))
  { /* Origin Latitude out of range */
    Error_Code |= LAMBERT_2_ORIGIN_LAT_ERROR;
  }
  if ((Std_Parallel_1 < -MAX_LAT) || (Std_Parallel_1 > MAX_LAT))
  { /* First Standard Parallel out of range */
    Error_Code |= LAMBERT_2_FIRST_STDP_ERROR;
  }
  if ((Std_Parallel_2 < -MAX_LAT) || (Std_Parallel_2 > MAX_LAT))
  { /* Second Standard Parallel out of range */
    Error_Code |= LAMBERT_2_SECOND_STDP_ERROR;
  }
  if ((Std_Parallel_1 == 0) && (Std_Parallel_2 == 0))
  { /* First & Second Standard Parallels are both 0 */
    Error_Code |= LAMBERT_2_FIRST_SECOND_ERROR;
  }
  if (Std_Parallel_1 == -Std_Parallel_2)
  { /* Parallels are the negation of each other */
    Error_Code |= LAMBERT_2_HEMISPHERE_ERROR;
  }
  if ((Central_Meridian < -PI) || (Central_Meridian > TWO_PI))
  { /* Origin Longitude out of range */
    Error_Code |= LAMBERT_2_CENT_MER_ERROR;
  }

  if (!Error_Code)
  { /* no errors */

    Lambert_a = a;
    Lambert_f = f;
    Lambert_Origin_Lat = Origin_Latitude;
    Lambert_Std_Parallel_1 = Std_Parallel_1;
    Lambert_Std_Parallel_2 = Std_Parallel_2;
    if (Central_Meridian > PI)
      Central_Meridian -= TWO_PI;
    Lambert_Origin_Long = Central_Meridian;
    Lambert_False_Easting = False_Easting;
    Lambert_False_Northing = False_Northing;

    if (fabs(Lambert_Std_Parallel_1 - Lambert_Std_Parallel_2) > 1.0e-10)
    {
      es2 = 2 * Lambert_f - Lambert_f * Lambert_f;
      es = sqrt(es2);
      es_OVER_2 = es / 2.0;

      es_sin = ES_SIN(sin(Lambert_Origin_Lat));
      m_olat = LAMBERT_m(cos(Lambert_Origin_Lat), es_sin);
      t_olat = LAMBERT_t(Lambert_Origin_Lat, es_sin);

      es_sin = ES_SIN(sin(Lambert_Std_Parallel_1));
      m1 = LAMBERT_m(cos(Lambert_Std_Parallel_1), es_sin);
      t1 = LAMBERT_t(Lambert_Std_Parallel_1, es_sin);

      es_sin = ES_SIN(sin(Lambert_Std_Parallel_2));
      m2 = LAMBERT_m(cos(Lambert_Std_Parallel_2), es_sin);
      t2 = LAMBERT_t(Lambert_Std_Parallel_2, es_sin);

      n = log(m1 / m2) / log(t1 / t2);

      Lambert_lat0 = asin(n);
    
      es_sin = ES_SIN(sin(Lambert_lat0));
      m0 = LAMBERT_m(cos(Lambert_lat0), es_sin);
      t0 = LAMBERT_t(Lambert_lat0, es_sin);

      Lambert_k0 = (m1 / m0) * (pow(t0 / t1, n));

      const_value = ((Lambert_a * m2) / (n * pow(t2, n)));
      
      Lambert_false_northing = (const_value * pow(t_olat, n)) - (const_value * pow(t0, n)) + Lambert_False_Northing;
    }
    else
    {
      Lambert_lat0 = Lambert_Std_Parallel_1;
      Lambert_k0 = 1.0;
      Lambert_false_northing = Lambert_False_Northing;
    }

    Set_Lambert_1_Parameters(Lambert_a, Lambert_f, Lambert_lat0, Lambert_Origin_Long, Lambert_False_Easting, Lambert_false_northing, Lambert_k0);
  }
  return (Error_Code);
} /* END OF Set_Lambert_2_Parameters */


void Get_Lambert_2_Parameters(double *a,
                              double *f,
                              double *Origin_Latitude,
                              double *Central_Meridian,
                              double *Std_Parallel_1,
                              double *Std_Parallel_2,
                              double *False_Easting,
                              double *False_Northing)

{ /* BEGIN Get_Lambert_2_Parameters */
/*                         
 * The function Get_Lambert_2_Parameters returns the current ellipsoid
 * parameters and Lambert Conformal Conic (2 parallel) projection parameters.
 *
 *   a                   : Semi-major axis of ellipsoid, in meters   (output)
 *   f                   : Flattening of ellipsoid					 (output)
 *   Origin_Latitude     : Latitude of origin, in radians            (output)
 *   Central_Meridian    : Longitude of origin, in radians           (output)
 *   Std_Parallel_1      : First standard parallel, in radians       (output)
 *   Std_Parallel_2      : Second standard parallel, in radians      (output)
 *   False_Easting       : False easting, in meters                  (output)
 *   False_Northing      : False northing, in meters                 (output)
 */


  *a = Lambert_a;
  *f = Lambert_f;
  *Std_Parallel_1 = Lambert_Std_Parallel_1;
  *Std_Parallel_2 = Lambert_Std_Parallel_2;
  *Origin_Latitude = Lambert_Origin_Lat;
  *Central_Meridian = Lambert_Origin_Long;
  *False_Easting = Lambert_False_Easting;
  *False_Northing = Lambert_False_Northing;
  return;
} /* END OF Get_Lambert_2_Parameters */


long Convert_Geodetic_To_Lambert_2 (double Latitude,
                                    double Longitude,
                                    double *Easting,
                                    double *Northing)

{ /* BEGIN Convert_Geodetic_To_Lambert_2 */
/*
 * The function Convert_Geodetic_To_Lambert_2 converts Geodetic (latitude and
 * longitude) coordinates to Lambert Conformal Conic (2 parallel) projection (easting
 * and northing) coordinates, according to the current ellipsoid and
 * Lambert Conformal Conic (2 parallel) projection parameters.  If any errors occur, the
 * error code(s) are returned by the function, otherwise LAMBERT_2_NO_ERROR is
 * returned.
 *
 *    Latitude         : Latitude, in radians                         (input)
 *    Longitude        : Longitude, in radians                        (input)
 *    Easting          : Easting (X), in meters                       (output)
 *    Northing         : Northing (Y), in meters                      (output)
 */

  long  Error_Code = LAMBERT_2_NO_ERROR;

  if ((Latitude < -PI_OVER_2) || (Latitude > PI_OVER_2))
  {  /* Latitude out of range */
    Error_Code|= LAMBERT_2_LAT_ERROR;
  }
  if ((Longitude < -PI) || (Longitude > TWO_PI))
  {  /* Longitude out of range */
    Error_Code|= LAMBERT_2_LON_ERROR;
  }

  if (!Error_Code)
  { /* no errors */
    Set_Lambert_1_Parameters(Lambert_a, Lambert_f, Lambert_lat0, Lambert_Origin_Long, Lambert_False_Easting, Lambert_false_northing, Lambert_k0);
    Error_Code = Convert_Geodetic_To_Lambert_1(Latitude, Longitude, Easting, Northing);
  }
  return (Error_Code);
} /* END OF Convert_Geodetic_To_Lambert_2 */



long Convert_Lambert_2_To_Geodetic (double Easting,
                                    double Northing,
                                    double *Latitude,
                                    double *Longitude)

{ /* BEGIN Convert_Lambert_2_To_Geodetic */
/*
 * The function Convert_Lambert_2_To_Geodetic converts Lambert Conformal
 * Conic (2 parallel) projection (easting and northing) coordinates to Geodetic
 * (latitude and longitude) coordinates, according to the current ellipsoid
 * and Lambert Conformal Conic (2 parallel) projection parameters.  If any errors occur,
 * the error code(s) are returned by the function, otherwise LAMBERT_2_NO_ERROR
 * is returned.
 *
 *    Easting          : Easting (X), in meters                       (input)
 *    Northing         : Northing (Y), in meters                      (input)
 *    Latitude         : Latitude, in radians                         (output)
 *    Longitude        : Longitude, in radians                        (output)
 */


  long Error_Code = LAMBERT_2_NO_ERROR;

  if ((Easting < (Lambert_False_Easting - Lambert_Delta_Easting))
      ||(Easting > (Lambert_False_Easting + Lambert_Delta_Easting)))
  { /* Easting out of range  */
    Error_Code |= LAMBERT_2_EASTING_ERROR;
  }
  if ((Northing < (Lambert_false_northing - Lambert_Delta_Northing))
      || (Northing > (Lambert_false_northing + Lambert_Delta_Northing)))
  { /* Northing out of range */
    Error_Code |= LAMBERT_2_NORTHING_ERROR;
  }

  if (!Error_Code)
  { /* no errors */
    Set_Lambert_1_Parameters(Lambert_a, Lambert_f, Lambert_lat0, Lambert_Origin_Long, Lambert_False_Easting, Lambert_false_northing, Lambert_k0);
    Error_Code = Convert_Lambert_1_To_Geodetic(Easting, Northing, Latitude, Longitude);
  }
  return (Error_Code);
} /* END OF Convert_Lambert_2_To_Geodetic */



