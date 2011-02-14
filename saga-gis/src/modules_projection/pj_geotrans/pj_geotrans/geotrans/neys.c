/**********************************************************
 * Version $Id$
 *********************************************************/
/***************************************************************************/
/* RSC IDENTIFIER: NEYS
 *
 * ABSTRACT
 *
 *    This component provides conversions between Geodetic coordinates
 *    (latitude and longitude in radians) and Ney's (Modified Lambert 
 *    Conformal Conic) projection coordinates (easting and northing in meters).
 *
 * ERROR HANDLING
 *
 *    This component checks parameters for valid values.  If an invalid value
 *    is found the error code is combined with the current error code using
 *    the bitwise or.  This combining allows multiple error codes to be
 *    returned. The possible error codes are:
 *
 *       NEYS_NO_ERROR           : No errors occurred in function
 *       NEYS_LAT_ERROR          : Latitude outside of valid range
 *                                     (-90 to 90 degrees)
 *       NEYS_LON_ERROR          : Longitude outside of valid range
 *                                     (-180 to 360 degrees)
 *       NEYS_EASTING_ERROR      : Easting outside of valid range
 *                                     (depends on ellipsoid and projection
 *                                     parameters)
 *       NEYS_NORTHING_ERROR     : Northing outside of valid range
 *                                     (depends on ellipsoid and projection
 *                                     parameters)
 *       NEYS_FIRST_STDP_ERROR   : First standard parallel outside of valid
 *                                     range (±71 or ±74 degrees)
 *       NEYS_ORIGIN_LAT_ERROR   : Origin latitude outside of valid range
 *                                     (-89 59 59.0 to 89 59 59.0 degrees)
 *       NEYS_CENT_MER_ERROR     : Central meridian outside of valid range
 *                                     (-180 to 360 degrees)
 *       NEYS_A_ERROR            : Semi-major axis less than or equal to zero
 *       NEYS_INV_F_ERROR        : Inverse flattening outside of valid range
 *									                   (250 to 350)
 *
 *
 * REUSE NOTES
 *
 *    NEYS is intended for reuse by any application that performs a Ney's (Modified
 *    Lambert Conformal Conic) projection or its inverse.
 *    
 * REFERENCES
 *
 *    Further information on NEYS can be found in the Reuse Manual.
 *
 *    NEYS originated from:
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
 *    NEYS has no restrictions.
 *
 * ENVIRONMENT
 *
 *    NEYS was tested and certified in the following environments:
 *
 *    1. Solaris 2.5 with GCC, version 2.8.1
 *    2. Windows 95 with MS Visual C++, version 6
 *
 * MODIFICATIONS
 *
 *    Date              Description
 *    ----              -----------
 *    8-4-00            Original Code
 *
 *
 *
 */


/***************************************************************************/
/*
 *                               INCLUDES
 */

#include <math.h>
#include "lambert.h"
#include "neys.h"
/*
 *    math.h     - Standard C math library
 *    lambert.h  - Is used to convert lambert conformal conic coordinates
 *    neys.h     - Is for prototype error checking
 */


/***************************************************************************/
/*                               DEFINES
 *
 */

#define PI            3.14159265358979323e0   /* PI     */
#define PI_OVER_2     (PI / 2.0)
#define TWO_PI        (2.0 * PI)
#define SEVENTY_ONE   (71.0 * PI / 180.0)      /* 71 degrees     */
#define SEVENTY_FOUR  (74.0 * PI / 180.0)      /* 74 degrees     */
#define MAX_LAT       (89.99972222222222 * PI / 180.0) /* 89 59 59.0 degrees     */

/**************************************************************************/
/*                               GLOBAL DECLARATIONS
 *
 */

/* Ellipsoid Parameters, default to WGS 84  */
static double Neys_a = 6378137.0;               /* Semi-major axis of ellipsoid, in meters */
static double Neys_f = 1 / 298.257223563;       /* Flattening of ellipsoid */

/* Ney's projection Parameters */
static double Neys_Std_Parallel_1 = SEVENTY_ONE;     /* Lower std. parallel, in radians */
static double Neys_Std_Parallel_2 = MAX_LAT;         /* Upper std. parallel, in radians */
static double Neys_Origin_Lat = (80.0 * PI / 180.0); /* Latitude of origin, in radians */
static double Neys_Origin_Long = 0.0;                /* Longitude of origin, in radians */
static double Neys_False_Northing = 0.0;             /* False northing, in meters */
static double Neys_False_Easting = 0.0;              /* False easting, in meters */

/* Maximum variance for easting and northing values for WGS 84. */
static double Neys_Delta_Easting = 40000000.0;
static double Neys_Delta_Northing = 40000000.0;

/* These state variables are for optimization purposes. The only function
 * that should modify them is Set_Neys_Parameters.         */

/************************************************************************/
/*                              FUNCTIONS
 *
 */

long Set_Neys_Parameters(double a,
                         double f,
                         double Origin_Latitude,
                         double Central_Meridian,
                         double Std_Parallel_1,
                         double False_Easting,
                         double False_Northing)

{ /* BEGIN Set_Neys_Parameters */
/*
 * The function Set_Neys_Parameters receives the ellipsoid parameters and
 * Ney's (Modified Lambert Conformal Conic) projection parameters as inputs, and sets the
 * corresponding state variables.  If any errors occur, the error code(s)
 * are returned by the function, otherwise NEYS_NO_ERROR is returned.
 *
 *   a                   : Semi-major axis of ellipsoid, in meters   (input)
 *   f                   : Flattening of ellipsoid						       (input)
 *   Origin_Latitude     : Latitude of origin, in radians            (input)
 *   Central_Meridian    : Longitude of origin, in radians           (input)
 *   Std_Parallel_1      : First standard parallel, in radians       (input)
 *   False_Easting       : False easting, in meters                  (input)
 *   False_Northing      : False northing, in meters                 (input)
 */

  double inv_f = 1 / f;
  long Error_Code = NEYS_NO_ERROR;

  if (a <= 0.0)
  { /* Semi-major axis must be greater than zero */
    Error_Code |= NEYS_A_ERROR;
  }
  if ((inv_f < 250) || (inv_f > 350))
  { /* Inverse flattening must be between 250 and 350 */
    Error_Code |= NEYS_INV_F_ERROR;
  }
  if ((Origin_Latitude < -MAX_LAT) || (Origin_Latitude > MAX_LAT))
  { /* Origin Latitude out of range */
    Error_Code |= NEYS_ORIGIN_LAT_ERROR;
  }
  if ((Std_Parallel_1 != SEVENTY_ONE) && (Std_Parallel_1 != SEVENTY_FOUR))
  { /* First Standard Parallel invalid */
    Error_Code |= NEYS_FIRST_STDP_ERROR;
  }
  if ((Central_Meridian < -PI) || (Central_Meridian > TWO_PI))
  { /* Origin Longitude out of range */
    Error_Code |= NEYS_CENT_MER_ERROR;
  }

  if (!Error_Code)
  { /* no errors */
    Neys_a = a;
    Neys_f = f;

    Neys_Origin_Lat = Origin_Latitude;
    if (Neys_Origin_Lat >= 0)
    {
      Neys_Std_Parallel_1 = Std_Parallel_1;
      Neys_Std_Parallel_2 = MAX_LAT;
    }
    else
    {
      Neys_Std_Parallel_1 = -Std_Parallel_1;
      Neys_Std_Parallel_2 = -MAX_LAT;
    }

    if (Central_Meridian > PI)
      Central_Meridian -= TWO_PI;
    Neys_Origin_Long = Central_Meridian;
    Neys_False_Easting = False_Easting;
    Neys_False_Northing = False_Northing;

    Set_Lambert_Parameters(Neys_a, Neys_f, Neys_Origin_Lat, Neys_Origin_Long,
                           Neys_Std_Parallel_1, Neys_Std_Parallel_2, 
                           Neys_False_Easting, Neys_False_Northing);   
  }
  return (Error_Code);
}  /* END of Set_Neys_Parameters  */


void Get_Neys_Parameters(double *a,
                         double *f,
                         double *Origin_Latitude,
                         double *Central_Meridian,
                         double *Std_Parallel_1,
                         double *False_Easting,
                         double *False_Northing)

{ /* BEGIN Get_Neys_Parameters */
/*                         
 * The function Get_Neys_Parameters returns the current ellipsoid
 * parameters and Ney's (Modified Lambert Conformal Conic) projection parameters.
 *
 *   a                   : Semi-major axis of ellipsoid, in meters   (output)
 *   f                   : Flattening of ellipsoid					         (output)
 *   Origin_Latitude     : Latitude of origin, in radians            (output)
 *   Central_Meridian    : Longitude of origin, in radians           (output)
 *   Std_Parallel_1      : First standard parallel, in radians       (output)
 *   False_Easting       : False easting, in meters                  (output)
 *   False_Northing      : False northing, in meters                 (output)
 */

  *a = Neys_a;
  *f = Neys_f;
  *Origin_Latitude = Neys_Origin_Lat;
  *Central_Meridian = Neys_Origin_Long;
  *Std_Parallel_1 = Neys_Std_Parallel_1;
  *False_Easting = Neys_False_Easting;
  *False_Northing = Neys_False_Northing;
  return;
} /* END OF Get_Neys_Parameters */


long Convert_Geodetic_To_Neys(double Latitude,
                              double Longitude,
                              double *Easting,
                              double *Northing)

{ /* BEGIN Convert_Geodetic_To_Neys */
/*
 * The function Convert_Geodetic_To_Neys converts Geodetic (latitude and
 * longitude) coordinates to Ney's (Modified Lambert Conformal Conic) projection 
 * (easting and northing) coordinates, according to the current ellipsoid and
 * Ney's (Modified Lambert Conformal Conic) projection parameters.  If any errors 
 * occur, the error code(s) are returned by the function, otherwise NEYS_NO_ERROR is
 * returned.
 *
 *    Latitude         : Latitude, in radians                         (input)
 *    Longitude        : Longitude, in radians                        (input)
 *    Easting          : Easting (X), in meters                       (output)
 *    Northing         : Northing (Y), in meters                      (output)
 */

  long temp_Error = 0;
  long Error_Code = NEYS_NO_ERROR;

  if ((Latitude < -PI_OVER_2) || (Latitude > PI_OVER_2))
  {  /* Latitude out of range */
    Error_Code|= NEYS_LAT_ERROR;
  }
  if ((Longitude < -PI) || (Longitude > TWO_PI))
  {  /* Longitude out of range */
    Error_Code|= NEYS_LON_ERROR;
  }

  if (!Error_Code)
  { /* no errors */

    temp_Error = Convert_Geodetic_To_Lambert(Latitude, Longitude, Easting, Northing);
    
    if (temp_Error & LAMBERT_LAT_ERROR)
      Error_Code |= NEYS_LAT_ERROR;

  }
  return (Error_Code);
} /* END OF Convert_Geodetic_To_Neys */


long Convert_Neys_To_Geodetic(double Easting,
                              double Northing,
                              double *Latitude,
                              double *Longitude)

{ /* BEGIN Convert_Neys_To_Geodetic */
/*
 * The function Convert_Neys_To_Geodetic converts Ney's (Modified Lambert Conformal
 * Conic) projection (easting and northing) coordinates to Geodetic (latitude) 
 * and longitude) coordinates, according to the current ellipsoid and Ney's 
 * (Modified Lambert Conformal Conic) projection parameters.  If any errors occur,
 * the error code(s) are returned by the function, otherwise NEYS_NO_ERROR
 * is returned.
 *
 *    Easting          : Easting (X), in meters                       (input)
 *    Northing         : Northing (Y), in meters                      (input)
 *    Latitude         : Latitude, in radians                         (output)
 *    Longitude        : Longitude, in radians                        (output)
 */

  long temp_Error = 0;
  long Error_Code = NEYS_NO_ERROR;

  if ((Easting < (Neys_False_Easting - Neys_Delta_Easting))
      ||(Easting > (Neys_False_Easting + Neys_Delta_Easting)))
  { /* Easting out of range  */
    Error_Code |= NEYS_EASTING_ERROR;
  }
  if ((Northing < (Neys_False_Northing - Neys_Delta_Northing))
      || (Northing > (Neys_False_Northing + Neys_Delta_Northing)))
  { /* Northing out of range */
    Error_Code |= NEYS_NORTHING_ERROR;
  }

  if (!Error_Code)
  { /* no errors */

    temp_Error = Convert_Lambert_To_Geodetic(Easting, Northing, Latitude, Longitude);
   
    if (temp_Error & LAMBERT_EASTING_ERROR)
      Error_Code |= NEYS_EASTING_ERROR;
    if (temp_Error & LAMBERT_NORTHING_ERROR)
      Error_Code |= NEYS_NORTHING_ERROR;

  }
  return (Error_Code);
} /* END OF Convert_Neys_To_Geodetic */
