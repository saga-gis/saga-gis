/***************************************************************************/
/* RSC IDENTIFIER: MOLLWEIDE
 *
 * ABSTRACT
 *
 *    This component provides conversions between Geodetic coordinates
 *    (latitude and longitude in radians) and Mollweide projection coordinates
 *    (easting and northing in meters).  The Mollweide Pseudocylindrical
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
 *          MOLL_NO_ERROR           : No errors occurred in function
 *          MOLL_LAT_ERROR          : Latitude outside of valid range
 *                                      (-90 to 90 degrees)
 *          MOLL_LON_ERROR          : Longitude outside of valid range
 *                                      (-180 to 360 degrees)
 *          MOLL_EASTING_ERROR      : Easting outside of valid range
 *                                      (False_Easting +/- ~18,000,000 m,
 *                                       depending on ellipsoid parameters)
 *          MOLL_NORTHING_ERROR     : Northing outside of valid range
 *                                      (False_Northing +/- ~9,000,000 m,
 *                                       depending on ellipsoid parameters)
 *          MOLL_ORIGIN_LON_ERROR   : Origin longitude outside of valid range
 *                                      (-180 to 360 degrees)
 *          MOLL_A_ERROR            : Semi-major axis less than or equal to zero
 *          MOLL_INV_F_ERROR        : Inverse flattening outside of valid range
 *								  	                  (250 to 350)
 *
 * REUSE NOTES
 *
 *    MOLLWEID is intended for reuse by any application that performs a
 *    Mollweide projection or its inverse.
 *
 * REFERENCES
 *
 *    Further information on MOLLWEID can be found in the Reuse Manual.
 *
 *    MOLLWEID originated from :  U.S. Army Topographic Engineering Center
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
 *    MOLLWEID has no restrictions.
 *
 * ENVIRONMENT
 *
 *    MOLLWEID was tested and certified in the following environments:
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
#include "mollweid.h"

/*
 *    math.h     - Standard C math library
 *    mollweid.h - Is for prototype error checking
 */


/***************************************************************************/
/*
 *                               DEFINES
 */

#define PI         3.14159265358979323e0  /* PI                            */
#define PI_OVER_2  ( PI / 2.0)                 
#define MAX_LAT    ( (PI * 90) / 180.0 ) /* 90 degrees in radians    */
#define TWO_PI     (2.0 * PI)                  


/***************************************************************************/
/*
 *                               GLOBALS
 */

/* Ellipsoid Parameters, default to WGS 84 */
static double Moll_a = 6378137.0;                /* Semi-major axis of ellipsoid in meters */
static double Moll_f = 1 / 298.257223563;        /* Flattening of ellipsoid */
static double es2 = 0.0066943799901413800;       /* Eccentricity (0.08181919084262188000) squared         */
static double es4 = 4.4814723452405e-005;        /* es2 * es2	*/
static double es6 = 3.0000678794350e-007;        /* es4 * es2  */
static double Sqrt2_Ra = 9009964.7614632;        /* sqrt(2.0) * Spherical Radius(6371007.1810824) */
static double Sqrt8_Ra = 18019929.522926;        /* sqrt(8.0) * Spherical Radius(6371007.1810824) */

/* Mollweide projection Parameters */
static double Moll_Origin_Long = 0.0;            /* Longitude of origin in radians    */
static double Moll_False_Easting = 0.0;
static double Moll_False_Northing = 0.0;
static double Moll_Delta_Northing = 9009965.0;
static double Moll_Max_Easting =  18019930.0;
static double Moll_Min_Easting =  -18019930.0;

/*
 * These state variables are for optimization purposes.  The only function
 * that should modify them is Set_Mollweide_Parameters.
 */


/***************************************************************************/
/*
 *                              FUNCTIONS
 */


long Set_Mollweide_Parameters(double a,
                              double f,
                              double Central_Meridian,
                              double False_Easting,
                              double False_Northing)
{ /* Begin Set_Mollweide_Parameters */
/*
 * The function Set_Mollweide_Parameters receives the ellipsoid parameters and
 * Mollweide projcetion parameters as inputs, and sets the corresponding state
 * variables.  If any errors occur, the error code(s) are returned by the 
 * function, otherwise MOLL_NO_ERROR is returned.
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

  double Ra;                       /* Spherical Radius */
  double inv_f = 1 / f;
  long Error_Code = MOLL_NO_ERROR;

  if (a <= 0.0)
  { /* Semi-major axis must be greater than zero */
    Error_Code |= MOLL_A_ERROR;
  }
  if ((inv_f < 250) || (inv_f > 350))
  { /* Inverse flattening must be between 250 and 350 */
    Error_Code |= MOLL_INV_F_ERROR;
  }
  if ((Central_Meridian < -PI) || (Central_Meridian > TWO_PI))
  { /* origin longitude out of range */
    Error_Code |= MOLL_CENT_MER_ERROR;
  }
  if (!Error_Code)
  { /* no errors */
    Moll_a = a;
    Moll_f = f;
    es2 = 2 * Moll_f - Moll_f * Moll_f;
    es4 = es2 * es2;
    es6 = es4 * es2;
    /* spherical radius */
    Ra = Moll_a * (1.0 - es2 / 6.0 - 17.0 * es4 / 360.0 - 67.0 * es6 / 3024.0);
    Sqrt2_Ra = sqrt(2.0) * Ra;
    Sqrt8_Ra = sqrt(8.0) * Ra;
    if (Central_Meridian > PI)
      Central_Meridian -= TWO_PI;
    Moll_Origin_Long = Central_Meridian;
    Moll_False_Easting = False_Easting;
    Moll_False_Northing = False_Northing;

    if (Moll_Origin_Long > 0)
    {
      Moll_Max_Easting = 17919819.0;
      Moll_Min_Easting = -18019930.0;
    }
    else if (Moll_Origin_Long < 0)
    {
      Moll_Max_Easting = 18019930.0;
      Moll_Min_Easting = -17919819.0;
    }
    else
    {
      Moll_Max_Easting = 18019930.0;
      Moll_Min_Easting = -18019930.0;
    }

  } /* End if(!Error_Code) */
  return (Error_Code);
} /* End Set_Mollweide_Parameters */


void Get_Mollweide_Parameters(double *a,
                              double *f,
                              double *Central_Meridian,
                              double *False_Easting,
                              double *False_Northing)
{ /* Begin Get_Mollweide_Parameters */
/*
 * The function Get_Mollweide_Parameters returns the current ellipsoid
 * parameters and Mollweide projection parameters.
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

  *a = Moll_a;
  *f = Moll_f;
  *Central_Meridian = Moll_Origin_Long;
  *False_Easting = Moll_False_Easting;
  *False_Northing = Moll_False_Northing;
  return;
} /* End Get_Mollweide_Parameters */


long Convert_Geodetic_To_Mollweide (double Latitude,
                                    double Longitude,
                                    double *Easting,
                                    double *Northing)

{ /* Begin Convert_Geodetic_To_Mollweide */
/*
 * The function Convert_Geodetic_To_Mollweide converts geodetic (latitude and
 * longitude) coordinates to Mollweide projection (easting and northing)
 * coordinates, according to the current ellipsoid and Mollweide projection
 * parameters.  If any errors occur, the error code(s) are returned by the
 * function, otherwise MOLL_NO_ERROR is returned.
 *
 *    Latitude          : Latitude (phi) in radians           (input)
 *    Longitude         : Longitude (lambda) in radians       (input)
 *    Easting           : Easting (X) in meters               (output)
 *    Northing          : Northing (Y) in meters              (output)
 */

  double PI_Sin_Latitude = PI * sin(Latitude);
  double dlam;                                  /* Longitude - Central Meridan */
  double theta;
  double theta_primed = Latitude;
  double delta_theta_primed = 0.1745329;        /* arbitrarily initialized to 10 deg */
  double dtp_tolerance = 4.85e-10;              /* approximately 1/1000th of
                                                 an arc second or 1/10th meter */
  long   Error_Code = MOLL_NO_ERROR;

  if ((Latitude < -PI_OVER_2) || (Latitude > PI_OVER_2))
  {  /* Latitude out of range */
    Error_Code |= MOLL_LAT_ERROR;
  }
  if ((Longitude < -PI) || (Longitude > TWO_PI))
  {  /* Longitude out of range */
    Error_Code|= MOLL_LON_ERROR;
  }

  if (!Error_Code)
  { /* no errors */
    dlam = Longitude - Moll_Origin_Long;
    if (dlam > PI)
    {
      dlam -= TWO_PI;
    }
    if (dlam < -PI)
    {
      dlam += TWO_PI;
    }
    while (fabs(delta_theta_primed) > dtp_tolerance)
    {
      delta_theta_primed = -(theta_primed + sin(theta_primed) -
                             PI_Sin_Latitude) / (1.0 + cos(theta_primed));
      theta_primed += delta_theta_primed;
    }
    theta = theta_primed / 2.0;
    *Easting = (Sqrt8_Ra / PI ) * dlam * cos(theta) +
               Moll_False_Easting;
    *Northing = Sqrt2_Ra * sin(theta) + Moll_False_Northing;

  }
  return (Error_Code);

} /* End Convert_Geodetic_To_Mollweide */


long Convert_Mollweide_To_Geodetic(double Easting,
                                   double Northing,
                                   double *Latitude,
                                   double *Longitude)
{ /* Begin Convert_Mollweide_To_Geodetic */
/*
 * The function Convert_Mollweide_To_Geodetic converts Mollweide projection
 * (easting and northing) coordinates to geodetic (latitude and longitude)
 * coordinates, according to the current ellipsoid and Mollweide projection
 * coordinates.  If any errors occur, the error code(s) are returned by the
 * function, otherwise MOLL_NO_ERROR is returned.
 *
 *    Easting           : Easting (X) in meters                  (input)
 *    Northing          : Northing (Y) in meters                 (input)
 *    Latitude          : Latitude (phi) in radians              (output)
 *    Longitude         : Longitude (lambda) in radians          (output)
 */

  double dx, dy;
  double theta = 0.0;
  double two_theta;
  double i;

  long Error_Code = MOLL_NO_ERROR;

  if ((Easting < (Moll_False_Easting + Moll_Min_Easting))
      || (Easting > (Moll_False_Easting + Moll_Max_Easting)))
  { /* Easting out of range  */
    Error_Code |= MOLL_EASTING_ERROR;
  }
  if ((Northing < (Moll_False_Northing - Moll_Delta_Northing)) || 
      (Northing >(Moll_False_Northing + Moll_Delta_Northing) ))
  { /* Northing out of range */
    Error_Code |= MOLL_NORTHING_ERROR;
  }

  if (!Error_Code)
  {
    dy = Northing - Moll_False_Northing;
    dx = Easting - Moll_False_Easting;
    i = dy / Sqrt2_Ra;
    if (fabs(i) > 1.0)
    {
      *Latitude = MAX_LAT;
      if (Northing < 0.0)
        *Latitude *= -1.0;
    }

    else
    {
      theta = asin(i);
      two_theta = 2.0 * theta;
      *Latitude = asin((two_theta + sin(two_theta)) / PI);

      if (*Latitude > PI_OVER_2)  /* force distorted values to 90, -90 degrees */
        *Latitude = PI_OVER_2;
      else if (*Latitude < -PI_OVER_2)
        *Latitude = -PI_OVER_2;

    }
    if (fabs(fabs(*Latitude) - MAX_LAT) < 1.0e-10)
      *Longitude = Moll_Origin_Long;
    else
      *Longitude = Moll_Origin_Long + PI * dx /
                   (Sqrt8_Ra * cos(theta));

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

} /* End Convert_Mollweide_To_Geodetic */

