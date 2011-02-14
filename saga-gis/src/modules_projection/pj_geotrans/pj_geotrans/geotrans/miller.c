/***************************************************************************/
/* RSC IDENTIFIER: MILLER
 *
 * ABSTRACT
 *
 *    This component provides conversions between Geodetic coordinates
 *    (latitude and longitude in radians) and Miller Cylindrical projection 
 *    coordinates (easting and northing in meters).  The Miller Cylindrical
 *    projection employs a spherical Earth model.  The Spherical Radius
 *    used is the the radius of the sphere having the same area as the
 *    ellipsoid.
 *
 * ERROR HANDLING
 *
 *    This component checks parameters for valid values.  If an invalid value
 *    is found, the error code is combined with the current error code using 
 *    the bitwise or.  This combining allows multiple error codes to be
 *    returned. The possible error codes are:
 *
 *          MILL_NO_ERROR           : No errors occurred in function
 *          MILL_LAT_ERROR          : Latitude outside of valid range
 *                                      (-90 to 90 degrees)
 *          MILL_LON_ERROR          : Longitude outside of valid range
 *                                      (-180 to 360 degrees)
 *          MILL_EASTING_ERROR      : Easting outside of valid range
 *                                      (False_Easting +/- ~20,000,000 m,
 *                                       depending on ellipsoid parameters)
 *          MILL_NORTHING_ERROR     : Northing outside of valid range
 *                                      (False_Northing +/- ~14,000,000 m,
 *                                       depending on ellipsoid parameters)
 *          MILL_CENT_MER_ERROR     : Central meridian outside of valid range
 *                                      (-180 to 360 degrees)
 *          MILL_A_ERROR            : Semi-major axis less than or equal to zero
 *          MILL_INV_F_ERROR        : Inverse flattening outside of valid range
 *									                    (250 to 350)
 *
 * REUSE NOTES
 *
 *    MILLER is intended for reuse by any application that performs a
 *    Miller Cylindrical projection or its inverse.
 *
 * REFERENCES
 *
 *    Further information on MILLER can be found in the Reuse Manual.
 *
 *    MILLER originated from :  U.S. Army Topographic Engineering Center
 *                                Geospatial Information Division
 *                                7701 Telegraph Road
 *                                Alexandria, VA  22310-3864
 *
 * LICENSES
 *
 *    None apply to this component.
 *
 * RESTRICTIONS
 *
 *    MILLER has no restrictions.
 *
 * ENVIRONMENT
 *
 *    MILLER was tested and certified in the following environments:
 *
 *    1. Solaris 2.5 with GCC 2.8.1
 *    2. Windows 95 with MS Visual C++ 6
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
#include "miller.h"

/*
 *    math.h   - Standard C math library
 *    miller.h - Is for prototype error checking
 */


/***************************************************************************/
/*
 *                               DEFINES
 */

#define PI         3.14159265358979323e0  /* PI                            */
#define PI_OVER_2  ( PI / 2.0)                
#define TWO_PI     (2.0 * PI)                  

/***************************************************************************/
/*
 *                               GLOBALS
 */

/* Ellipsoid Parameters, default to WGS 84 */
static double Mill_a = 6378137.0;                      /* Semi-major axis of ellipsoid in meters */
static double Mill_f = 1 / 298.257223563;              /* Flattening of ellipsoid */
static double es2 = 0.0066943799901413800;             /* Eccentricity (0.08181919084262188000) squared         */
static double es4 = 4.4814723452405e-005;              /* es2 * es2	*/
static double es6 = 3.0000678794350e-007;              /* es4 * es2   */
static double Ra = 6371007.1810824;                    /* Spherical Radius */

/* Miller projection Parameters */
static double Mill_Origin_Long = 0.0;                  /* Longitude of origin in radians    */
static double Mill_False_Easting = 0.0;
static double Mill_False_Northing = 0.0;
static double Mill_Delta_Northing = 14675058.0;
static double Mill_Max_Easting =  20015110.0;
static double Mill_Min_Easting =  -20015110.0;
/*
 * These state variables are for optimization purposes.  The only function
 * that should modify them is Set_Miller_Parameters.
 */


/***************************************************************************/
/*
 *                              FUNCTIONS
 */


long Set_Miller_Parameters(double a,
                           double f,
                           double Central_Meridian ,
                           double False_Easting,
                           double False_Northing)
{ /* Begin Set_Miller_Parameters */
/*
 * The function Set_Miller_Parameters receives the ellipsoid parameters and
 * Miller Cylindrical projcetion parameters as inputs, and sets the corresponding
 * state variables.  If any errors occur, the error code(s) are returned by the 
 * function, otherwise MILL_NO_ERROR is returned.
 *
 *    a                 : Semi-major axis of ellipsoid, in meters   (input)
 *    f                 : Flattening of ellipsoid						        (input)
 *    Central_Meridian  : Longitude in radians at the center of     (input)
 *                          the projection
 *    False_Easting     : A coordinate value in meters assigned to the
 *                          central meridian of the projection.     (input)
 *    False_Northing    : A coordinate value in meters assigned to the
 *                          origin latitude of the projection       (input)
 */

  double inv_f = 1 / f;
  long Error_Code = MILL_NO_ERROR;

  if (a <= 0.0)
  { /* Semi-major axis must be greater than zero */
    Error_Code |= MILL_A_ERROR;
  }
  if ((inv_f < 250) || (inv_f > 350))
  { /* Inverse flattening must be between 250 and 350 */
    Error_Code |= MILL_INV_F_ERROR;
  }
  if ((Central_Meridian < -PI) || (Central_Meridian > TWO_PI))
  { /* origin longitude out of range */
    Error_Code |= MILL_CENT_MER_ERROR;
  }
  if (!Error_Code)
  { /* no errors */
    Mill_a = a;
    Mill_f = f;
    es2 = 2 * Mill_f - Mill_f * Mill_f;
    es4 = es2 * es2;
    es6 = es4 * es2;
    /* spherical radius */
    Ra = Mill_a * (1.0 - es2 / 6.0 - 17.0 * es4 / 360.0 - 67.0 * es6 /3024.0);
    if (Central_Meridian > PI)
      Central_Meridian -= TWO_PI;
    Mill_Origin_Long = Central_Meridian;
    Mill_False_Easting = False_Easting;
    Mill_False_Northing = False_Northing;
    if (Mill_Origin_Long > 0)
    {
      Mill_Max_Easting = 19903915.0;
      Mill_Min_Easting = -20015110.0;
    }
    else if (Mill_Origin_Long < 0)
    {
      Mill_Max_Easting = 20015110.0;
      Mill_Min_Easting = -19903915.0;
    }
    else
    {
      Mill_Max_Easting = 20015110.0;
      Mill_Min_Easting = -20015110.0;
    }
  } /* End if(!Error_Code) */
  return (Error_Code);
} /* End Set_Miller_Parameters */


void Get_Miller_Parameters(double *a,
                           double *f,
                           double *Central_Meridian,
                           double *False_Easting,
                           double *False_Northing)
{ /* Begin Get_Miller_Parameters */
/*
 * The function Get_Miller_Parameters returns the current ellipsoid
 * parameters and Miller Cylindrical projection parameters.
 *
 *    a                 : Semi-major axis of ellipsoid, in meters   (output)
 *    f                 : Flattening of ellipsoid						        (output)
 *    Central_Meridian  : Longitude in radians at the center of     (output)
 *                          the projection
 *    False_Easting     : A coordinate value in meters assigned to the
 *                          central meridian of the projection.     (output)
 *    False_Northing    : A coordinate value in meters assigned to the
 *                          origin latitude of the projection       (output)
 */

  *a = Mill_a;
  *f = Mill_f;
  *Central_Meridian = Mill_Origin_Long;
  *False_Easting = Mill_False_Easting;
  *False_Northing = Mill_False_Northing;
  return;
} /* End Get_Miller_Parameters */


long Convert_Geodetic_To_Miller (double Latitude,
                                 double Longitude,
                                 double *Easting,
                                 double *Northing)

{ /* Begin Convert_Geodetic_To_Miller */
/*
 * The function Convert_Geodetic_To_Miller converts geodetic (latitude and
 * longitude) coordinates to Miller Cylindrical projection (easting and northing)
 * coordinates, according to the current ellipsoid and Miller Cylindrical projection
 * parameters.  If any errors occur, the error code(s) are returned by the
 * function, otherwise MILL_NO_ERROR is returned.
 *
 *    Latitude          : Latitude (phi) in radians           (input)
 *    Longitude         : Longitude (lambda) in radians       (input)
 *    Easting           : Easting (X) in meters               (output)
 *    Northing          : Northing (Y) in meters              (output)
 */

  double slat = sin(0.8 * Latitude);
  double dlam;     /* Longitude - Central Meridan */

  long   Error_Code = MILL_NO_ERROR;

  if ((Latitude < -PI_OVER_2) || (Latitude > PI_OVER_2))
  {  /* Latitude out of range */
    Error_Code |= MILL_LAT_ERROR;
  }
  if ((Longitude < -PI) || (Longitude > TWO_PI))
  {  /* Longitude out of range */
    Error_Code|= MILL_LON_ERROR;
  }

  if (!Error_Code)
  { /* no errors */
    dlam = Longitude - Mill_Origin_Long;
    if (dlam > PI)
    {
      dlam -= TWO_PI;
    }
    if (dlam < -PI)
    {
      dlam += TWO_PI;
    }
    *Easting = Ra * dlam + Mill_False_Easting;
    *Northing = (Ra / 1.6) * log((1.0 + slat) /
                                 (1.0 - slat)) + Mill_False_Northing;
  }
  return (Error_Code);
} /* End Convert_Geodetic_To_Miller */


long Convert_Miller_To_Geodetic(double Easting,
                                double Northing,
                                double *Latitude,
                                double *Longitude)
{ /* Begin Convert_Miller_To_Geodetic */
/*
 * The function Convert_Miller_To_Geodetic converts Miller Cylindrical projection
 * (easting and northing) coordinates to geodetic (latitude and longitude)
 * coordinates, according to the current ellipsoid and Miller Cylindrical projection
 * coordinates.  If any errors occur, the error code(s) are returned by the
 * function, otherwise MILL_NO_ERROR is returned.
 *
 *    Easting           : Easting (X) in meters                  (input)
 *    Northing          : Northing (Y) in meters                 (input)
 *    Latitude          : Latitude (phi) in radians              (output)
 *    Longitude         : Longitude (lambda) in radians          (output)
 */

  double dx, dy;
  long Error_Code = MILL_NO_ERROR;

  if ((Easting < (Mill_False_Easting + Mill_Min_Easting))
      || (Easting > (Mill_False_Easting + Mill_Max_Easting)))
  { /* Easting out of range  */
    Error_Code |= MILL_EASTING_ERROR;
  }
  if ((Northing < (Mill_False_Northing - Mill_Delta_Northing)) || 
      (Northing > (Mill_False_Northing + Mill_Delta_Northing) ))
  { /* Northing out of range */
    Error_Code |= MILL_NORTHING_ERROR;
  }

  if (!Error_Code)
  {
    dy = Northing - Mill_False_Northing;
    dx = Easting - Mill_False_Easting;
    *Latitude = atan(sinh(0.8 * dy / Ra)) / 0.8;
    *Longitude = Mill_Origin_Long + dx / Ra;

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
  return (Error_Code);
} /* End Convert_Miller_To_Geodetic */

