/***************************************************************************/
/* RSC IDENTIFIER: ORTHOGRAPHIC
 *
 * ABSTRACT
 *
 *    This component provides conversions between Geodetic coordinates
 *    (latitude and longitude in radians) and Orthographic projection 
 *    coordinates (easting and northing in meters).  The Orthographic
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
 *          ORTH_NO_ERROR           : No errors occurred in function
 *          ORTH_LAT_ERROR          : Latitude outside of valid range
 *                                      (-90 to 90 degrees)
 *          ORTH_LON_ERROR          : Longitude outside of valid range
 *                                      (-180 to 360 degrees)
 *          ORTH_EASTING_ERROR      : Easting outside of valid range
 *                                      (False_Easting +/- ~6,500,000 m,
 *                                       depending on ellipsoid parameters)
 *          ORTH_NORTHING_ERROR     : Northing outside of valid range
 *                                      (False_Northing +/- ~6,500,000 m,
 *                                       depending on ellipsoid parameters)
 *          ORTH_RADIUS_ERROR       : Coordinates too far from pole,
 *                                      depending on ellipsoid and
 *                                      projection parameters
 *          ORTH_ORIGIN_LAT_ERROR   : Origin latitude outside of valid range
 *                                      (-90 to 90 degrees)
 *          ORTH_CENT_MER_ERROR     : Central meridian outside of valid range
 *                                      (-180 to 360 degrees)
 *          ORTH_A_ERROR            : Semi-major axis less than or equal to zero
 *          ORTH_INV_F_ERROR        : Inverse flattening outside of valid range
 *								  	                  (250 to 350)
 *
 * REUSE NOTES
 *
 *    ORTHOGRAPHIC is intended for reuse by any application that performs a
 *    Orthographic projection or its inverse.
 *
 * REFERENCES
 *
 *    Further information on ORTHOGRAPHIC can be found in the Reuse Manual.
 *
 *    ORTHOGRAPHIC originated from :  U.S. Army Topographic Engineering Center
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
 *    ORTHOGRAPHIC has no restrictions.
 *
 * ENVIRONMENT
 *
 *    ORTHOGRAPHIC was tested and certified in the following environments:
 *
 *    1. Solaris 2.5 with GCC, version 2.8.1
 *    2. Windows 95 with MS Visual C++, version 6
 *
 * MODIFICATIONS
 *
 *    Date              Description
 *    ----              -----------
 *    15-06-99          Original Code
 *
 */


/***************************************************************************/
/*
 *                               INCLUDES
 */

#include <math.h>
#include "orthogr.h"

/*
 *    math.h    - Standard C math library
 *    orthogr.h - Is for prototype error checking
 */


/***************************************************************************/
/*
 *                               DEFINES
 */

#define PI         3.14159265358979323e0  /* PI                            */
#define PI_OVER_2  ( PI / 2.0)                 
#define MAX_LAT    ( (PI * 90) / 180.0 )  /* 90 degrees in radians   */
#define TWO_PI     (2.0 * PI)                  


/***************************************************************************/
/*
 *                               GLOBALS
 */

/* Ellipsoid Parameters, default to WGS 84 */
static double Orth_a = 6378137.0;                      /* Semi-major axis of ellipsoid in meters */
static double Orth_f = 1 / 298.257223563;              /* Flattening of ellipsoid */
static double es2 = 0.0066943799901413800;             /* Eccentricity (0.08181919084262188000) squared         */
static double es4 = 4.4814723452405e-005;              /* es2 * es2 */
static double es6 = 3.0000678794350e-007;              /* es4 * es2 */
static double Ra = 6371007.1810824;                    /* Spherical Radius */

/* Orthographic projection Parameters */
static double Orth_Origin_Lat = 0.0;                   /* Latitude of origin in radians     */
static double Orth_Origin_Long = 0.0;                  /* Longitude of origin in radians    */
static double Orth_False_Easting = 0.0;
static double Orth_False_Northing = 0.0;
static double Sin_Orth_Origin_Lat = 0.0;               /* sin(Orth_Origin_Lat) */
static double Cos_Orth_Origin_Lat = 1.0;               /* cos(Orth_Origin_Lat) */
/*
 * These state variables are for optimization purposes.  The only function
 * that should modify them is Set_Orthographic_Parameters.
 */


/***************************************************************************/
/*
 *                              FUNCTIONS
 */


long Set_Orthographic_Parameters(double a,
                                 double f,
                                 double Origin_Latitude,
                                 double Central_Meridian,
                                 double False_Easting,
                                 double False_Northing)
{ /* BEGIN Set_Orthographic_Parameters */
/*
 * The function Set_Orthographic_Parameters receives the ellipsoid parameters and
 * projection parameters as inputs, and sets the corresponding state
 * variables.  If any errors occur, the error code(s) are returned by the function, 
 * otherwise ORTH_NO_ERROR is returned.
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

  double inv_f = 1 / f;
  long Error_Code = ORTH_NO_ERROR;

  if (a <= 0.0)
  { /* Semi-major axis must be greater than zero */
    Error_Code |= ORTH_A_ERROR;
  }
  if ((inv_f < 250) || (inv_f > 350))
  { /* Inverse flattening must be between 250 and 350 */
    Error_Code |= ORTH_INV_F_ERROR;
  }
  if ((Origin_Latitude < -PI_OVER_2) || (Origin_Latitude > PI_OVER_2))
  { /* origin latitude out of range */
    Error_Code |= ORTH_ORIGIN_LAT_ERROR;
  }
  if ((Central_Meridian < -PI) || (Central_Meridian > TWO_PI))
  { /* origin longitude out of range */
    Error_Code |= ORTH_CENT_MER_ERROR;
  }
  if (!Error_Code)
  { /* no errors */
    Orth_a = a;
    Orth_f = f;
    es2 = 2 * Orth_f - Orth_f * Orth_f;
    es4 = es2 * es2;
    es6 = es4 * es2;
    Ra = Orth_a * (1.0 - es2 / 6.0 - 17.0 * es4 / 360.0 - 67.0 * es6 /3024.0);
    Orth_Origin_Lat = Origin_Latitude;
    Sin_Orth_Origin_Lat = sin(Orth_Origin_Lat);
    Cos_Orth_Origin_Lat = cos(Orth_Origin_Lat);
    if (Central_Meridian > PI)
      Central_Meridian -= TWO_PI;
    Orth_Origin_Long = Central_Meridian;
    Orth_False_Easting = False_Easting;
    Orth_False_Northing = False_Northing;

  } /* END OF if(!Error_Code) */
  return (Error_Code);
} /* END OF Set_Orthographic_Parameters */


void Get_Orthographic_Parameters(double *a,
                                 double *f,
                                 double *Origin_Latitude,
                                 double *Central_Meridian,
                                 double *False_Easting,
                                 double *False_Northing)
{ /* BEGIN Get_Orthographic_Parameters */
/*
 * The function Get_Orthographic_Parameters returns the current ellipsoid
 * parameters and Orthographic projection parameters.
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

  *a = Orth_a;
  *f = Orth_f;
  *Origin_Latitude = Orth_Origin_Lat;
  *Central_Meridian = Orth_Origin_Long;
  *False_Easting = Orth_False_Easting;
  *False_Northing = Orth_False_Northing;
  return;
} /* END OF Get_Orthographic_Parameters */


long Convert_Geodetic_To_Orthographic (double Latitude,
                                       double Longitude,
                                       double *Easting,
                                       double *Northing)

{ /* BEGIN Convert_Geodetic_To_Orthographic */
/*
 * The function Convert_Geodetic_To_Orthographic converts geodetic (latitude and
 * longitude) coordinates to Orthographic projection (easting and northing)
 * coordinates, according to the current ellipsoid and Orthographic projection
 * parameters.  If any errors occur, the error code(s) are returned by the
 * function, otherwise ORTH_NO_ERROR is returned.
 *
 *    Latitude          : Latitude (phi) in radians           (input)
 *    Longitude         : Longitude (lambda) in radians       (input)
 *    Easting           : Easting (X) in meters               (output)
 *    Northing          : Northing (Y) in meters              (output)
 */

  double slat = sin(Latitude);
  double clat = cos(Latitude);
  double dlam;                        /* Longitude - Central Meridan */
  double clat_cdlam;
  double cos_c;                       /* Value used to determine whether the point is beyond
                                   viewing.  If zero or positive, the point is within view.  */
  long   Error_Code = ORTH_NO_ERROR;

  if ((Latitude < -PI_OVER_2) || (Latitude > PI_OVER_2))
  {  /* Latitude out of range */
    Error_Code |= ORTH_LAT_ERROR;
  }
  if ((Longitude < -PI) || (Longitude > TWO_PI))
  {  /* Longitude out of range */
    Error_Code|= ORTH_LON_ERROR;
  }
  dlam = Longitude - Orth_Origin_Long;
  clat_cdlam = clat * cos(dlam);
  cos_c = Sin_Orth_Origin_Lat * slat + Cos_Orth_Origin_Lat * clat_cdlam;
  if (cos_c < 0.0)
  {  /* Point is out of view.  Will return longitude out of range message
  since no point out of view is implemented.  */
    Error_Code |= ORTH_LON_ERROR;
  }

  if (!Error_Code)
  { /* no errors */
    if (dlam > PI)
    {
      dlam -= TWO_PI;
    }
    if (dlam < -PI)
    {
      dlam += TWO_PI;
    }
    *Easting = Ra * clat * sin(dlam) + Orth_False_Easting;
    *Northing = Ra * (Cos_Orth_Origin_Lat * slat - Sin_Orth_Origin_Lat * clat_cdlam) +
                Orth_False_Northing;
  }
  return (Error_Code);

} /* END OF Convert_Geodetic_To_Orthographic */


long Convert_Orthographic_To_Geodetic(double Easting,
                                      double Northing,
                                      double *Latitude,
                                      double *Longitude)
{ /* BEGIN Convert_Orthographic_To_Geodetic */
/*
 * The function Convert_Orthographic_To_Geodetic converts Orthographic projection
 * (easting and northing) coordinates to geodetic (latitude and longitude)
 * coordinates, according to the current ellipsoid and Orthographic projection
 * coordinates.  If any errors occur, the error code(s) are returned by the
 * function, otherwise ORTH_NO_ERROR is returned.
 *
 *    Easting           : Easting (X) in meters                  (input)
 *    Northing          : Northing (Y) in meters                 (input)
 *    Latitude          : Latitude (phi) in radians              (output)
 *    Longitude         : Longitude (lambda) in radians          (output)
 */

  double cc;
  double cos_cc, sin_cc;
  double rho;
  double dx, dy;
  double temp;
  double rho_OVER_Ra;
  long Error_Code = ORTH_NO_ERROR;

  if ((Easting > (Orth_False_Easting + Ra)) ||
      (Easting < (Orth_False_Easting - Ra)))
  { /* Easting out of range */
    Error_Code |= ORTH_EASTING_ERROR;
  }
  if ((Northing > (Orth_False_Northing + Ra)) ||
      (Northing < (Orth_False_Northing - Ra)))
  { /* Northing out of range */
    Error_Code |= ORTH_NORTHING_ERROR;
  }
  if (!Error_Code)
  {
    temp = sqrt(Easting * Easting + Northing * Northing);     

    if ((temp > (Orth_False_Easting + Ra)) || (temp > (Orth_False_Northing + Ra)) ||
        (temp < (Orth_False_Easting - Ra)) || (temp < (Orth_False_Northing - Ra)))
    { /* Point is outside of projection area */
      Error_Code |= ORTH_RADIUS_ERROR;
    }
  }

  if (!Error_Code)
  {
    dx = Easting - Orth_False_Easting;
    dy = Northing - Orth_False_Northing;
    rho = sqrt(dx * dx + dy * dy);
    if (rho == 0.0)
    {
      *Latitude = Orth_Origin_Lat;
      *Longitude = Orth_Origin_Long;
    }
    else
    {
      rho_OVER_Ra = rho / Ra;

      if (rho_OVER_Ra > 1.0)
        rho_OVER_Ra = 1.0;
      else if (rho_OVER_Ra < -1.0)
        rho_OVER_Ra = -1.0;

      cc = asin(rho_OVER_Ra);
      cos_cc = cos(cc);
      sin_cc = sin(cc);
      *Latitude = asin(cos_cc * Sin_Orth_Origin_Lat + (dy * sin_cc * Cos_Orth_Origin_Lat / rho));

      if (Orth_Origin_Lat == MAX_LAT)
        *Longitude = Orth_Origin_Long + atan2(dx, -dy);
      else if (Orth_Origin_Lat == -MAX_LAT)
       *Longitude = Orth_Origin_Long + atan2(dx, dy);
      else
        *Longitude = Orth_Origin_Long + atan2(dx * sin_cc, (rho *
                                                            Cos_Orth_Origin_Lat * cos_cc - dy * Sin_Orth_Origin_Lat * sin_cc));

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
} /* END OF Convert_Orthographic_To_Geodetic */

