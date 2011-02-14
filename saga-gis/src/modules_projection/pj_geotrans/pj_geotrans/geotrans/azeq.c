/**********************************************************
 * Version $Id$
 *********************************************************/
/***************************************************************************/
/* RSC IDENTIFIER: AZIMUTHAL EQUIDISTANT
 *
 * ABSTRACT
 *
 *    This component provides conversions between Geodetic coordinates
 *    (latitude and longitude in radians) and Azimuthal Equidistant
 *    projection coordinates (easting and northing in meters).  This projection 
 *    employs a spherical Earth model.  The spherical radius used is the radius of 
 *    the sphere having the same area as the ellipsoid.
 *
 * ERROR HANDLING
 *
 *    This component checks parameters for valid values.  If an invalid value
 *    is found the error code is combined with the current error code using
 *    the bitwise or.  This combining allows multiple error codes to be
 *    returned. The possible error codes are:
 *
 *       AZEQ_NO_ERROR           : No errors occurred in function
 *       AZEQ_LAT_ERROR          : Latitude outside of valid range
 *                                     (-90 to 90 degrees)
 *       AZEQ_LON_ERROR          : Longitude outside of valid range
 *                                     (-180 to 360 degrees)
 *       AZEQ_EASTING_ERROR      : Easting outside of valid range
 *                                     (depends on ellipsoid and projection
 *                                     parameters)
 *       AZEQ_NORTHING_ERROR     : Northing outside of valid range
 *                                     (depends on ellipsoid and projection
 *                                     parameters)
 *       AZEQ_ORIGIN_LAT_ERROR   : Origin latitude outside of valid range
 *                                     (-90 to 90 degrees)
 *       AZEQ_CENT_MER_ERROR     : Central meridian outside of valid range
 *                                     (-180 to 360 degrees)
 *       AZEQ_A_ERROR            : Semi-major axis less than or equal to zero
 *       AZEQ_INV_F_ERROR        : Inverse flattening outside of valid range
 *									                   (250 to 350)
 *       AZEQ_PROJECTION_ERROR   : Point is plotted as a circle of radius PI * Ra
 *
 *
 * REUSE NOTES
 *
 *    AZIMUTHAL EQUIDISTANT is intended for reuse by any application that 
 *    performs an Azimuthal Equidistant projection or its inverse.
 *
 * REFERENCES
 *
 *    Further information on AZIMUTHAL EQUIDISTANT can be found in the Reuse Manual.
 *
 *    AZIMUTHAL EQUIDISTANT originated from:     U.S. Army Topographic Engineering Center
 *                                               Geospatial Information Division
 *                                               7701 Telegraph Road
 *                                               Alexandria, VA  22310-3864
 *
 * LICENSES
 *
 *    None apply to this component.
 *
 * RESTRICTIONS
 *
 *    AZIMUTHAL EQUIDISTANT has no restrictions.
 *
 * ENVIRONMENT
 *
 *    AZIMUTHAL EQUIDISTANT was tested and certified in the following environments:
 *
 *    1. Solaris 2.5 with GCC, version 2.8.1
 *    2. MSDOS with MS Visual C++, version 6
 *
 * MODIFICATIONS
 *
 *    Date              Description
 *    ----              -----------
 *    05-19-00          Original Code
 *    
 *
 */



/***************************************************************************/
/*
 *                               INCLUDES
 */

#include <math.h>
#include "azeq.h"

/*
 *    math.h     - Standard C math library
 *    azeq.h   - Is for prototype error checking
 */


/***************************************************************************/
/*
 *                               DEFINES
 */

#define PI         3.14159265358979323e0  /* PI                            */
#define PI_OVER_2  ( PI / 2.0)                 
#define TWO_PI     ( 2.0 * PI)                 
#define ONE        (1.0 * PI / 180)       /* 1 degree in radians           */

/***************************************************************************/
/*
 *                               GLOBALS
 */
/* Ellipsoid Parameters, default to WGS 84 */
static double Azeq_a = 6378137.0;                  /* Semi-major axis of ellipsoid in meters */
static double Azeq_f = 1 / 298.257223563;          /* Flattening of ellipsoid */
static double Ra = 6371007.1810824;                /* Spherical Radius */
static double Sin_Azeq_Origin_Lat = 0.0;
static double Cos_Azeq_Origin_Lat = 1.0;

/* Azimuthal Equidistant projection Parameters */
static double Azeq_Origin_Lat = 0.0;               /* Latitude of origin in radians */
static double Azeq_Origin_Long = 0.0;              /* Longitude of origin in radians */
static double Azeq_False_Northing = 0.0;           /* False northing in meters */
static double Azeq_False_Easting = 0.0;            /* False easting in meters */
static double abs_Azeq_Origin_Lat = 0.0;

static double Azeq_Delta_Northing = 19903915.0;
static double Azeq_Delta_Easting = 19903915.0;
/*
 * These state variables are for optimization purposes.  The only function
 * that should modify them is Set_Azimuthal_Equidistant_Parameters.
 */
/***************************************************************************/
/*
 *                              FUNCTIONS     
 */

long Set_Azimuthal_Equidistant_Parameters(double a,
                                          double f,
                                          double Origin_Latitude,
                                          double Central_Meridian,
                                          double False_Easting,
                                          double False_Northing)
{ /* BEGIN Set_Azimuthal_Equidistant_Parameters */
/*
 * The function Set_Azimuthal_Equidistant_Parameters receives the ellipsoid parameters and
 * projection parameters as inputs, and sets the corresponding state
 * variables.  If any errors occur, the error code(s) are returned by the function, 
 * otherwise AZEQ_NO_ERROR is returned.
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

  double es2, es4, es6;
  double inv_f = 1 / f;
  double temp_Northing = 0.0;
  long Error_Code = AZEQ_NO_ERROR;

  if (a <= 0.0)
  { /* Semi-major axis must be greater than zero */
    Error_Code |= AZEQ_A_ERROR;
  }
  if ((inv_f < 250) || (inv_f > 350))
  { /* Inverse flattening must be between 250 and 350 */
    Error_Code |= AZEQ_INV_F_ERROR;
  }
  if ((Origin_Latitude < -PI_OVER_2) || (Origin_Latitude > PI_OVER_2))
  { /* origin latitude out of range */
    Error_Code |= AZEQ_ORIGIN_LAT_ERROR;
  }
  if ((Central_Meridian < -PI) || (Central_Meridian > TWO_PI))
  { /* origin longitude out of range */
    Error_Code |= AZEQ_CENT_MER_ERROR;
  }
  if (!Error_Code)
  { /* no errors */
    Azeq_a = a;
    Azeq_f = f;
    es2 = 2 * Azeq_f - Azeq_f * Azeq_f;
    es4 = es2 * es2;
    es6 = es4 * es2;
    /* spherical radius */
    Ra = Azeq_a * (1.0 - es2 / 6.0 - 17.0 * es4 / 360.0 - 67.0 * es6 / 3024.0);
    Azeq_Origin_Lat = Origin_Latitude;
    Sin_Azeq_Origin_Lat = sin(Azeq_Origin_Lat);
    Cos_Azeq_Origin_Lat = cos(Azeq_Origin_Lat);
    abs_Azeq_Origin_Lat = fabs(Azeq_Origin_Lat);
    if (Central_Meridian > PI)
      Central_Meridian -= TWO_PI;
    Azeq_Origin_Long = Central_Meridian;
    Azeq_False_Northing = False_Northing;
    Azeq_False_Easting = False_Easting;

    if (fabs(abs_Azeq_Origin_Lat - PI_OVER_2) < 1.0e-10)
    {
      Azeq_Delta_Northing = 20015110.0;
      Azeq_Delta_Easting = 20015110.0;
    }
    else if (abs_Azeq_Origin_Lat >= 1.0e-10)
    {
      if (Azeq_Origin_Long > 0.0)
      {
        Convert_Geodetic_To_Azimuthal_Equidistant(-Azeq_Origin_Lat, 
                                                  (Azeq_Origin_Long - PI - ONE), &Azeq_Delta_Easting, &temp_Northing);
      }
      else
      {
        Convert_Geodetic_To_Azimuthal_Equidistant(-Azeq_Origin_Lat, 
                                                  (Azeq_Origin_Long + PI - ONE), &Azeq_Delta_Easting, &temp_Northing);
      }
      Azeq_Delta_Northing = 19903915.0;
    }
    else
    {
      Azeq_Delta_Northing = 19903915.0;
      Azeq_Delta_Easting = 19903915.0;
    }
  } /* End if(!Error_Code) */
  return (Error_Code);
} /* End Set_Azimuthal_Equidistant_Parameters */


void Get_Azimuthal_Equidistant_Parameters(double *a,
                                          double *f,
                                          double *Origin_Latitude,
                                          double *Central_Meridian,
                                          double *False_Easting,
                                          double *False_Northing)
{ /* Begin Get_Azimuthal_Equidistant_Parameters */
/*
 * The function Get_Azimuthal_Equidistant_Parameters returns the current ellipsoid
 * parameters and Azimuthal Equidistant projection parameters.
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

  *a = Azeq_a;
  *f = Azeq_f;
  *Origin_Latitude = Azeq_Origin_Lat;
  *Central_Meridian = Azeq_Origin_Long;
  *False_Easting = Azeq_False_Easting;
  *False_Northing = Azeq_False_Northing;
  return;
} /* End Get_Azimuthal_Equidistant_Parameters */


long Convert_Geodetic_To_Azimuthal_Equidistant (double Latitude,
                                                double Longitude,
                                                double *Easting,
                                                double *Northing)
{ /* Begin Convert_Geodetic_To_Azimuthal_Equidistant */
/*
 * The function Convert_Geodetic_To_Azimuthal_Equidistant converts geodetic (latitude and
 * longitude) coordinates to Azimuthal Equidistant projection (easting and northing)
 * coordinates, according to the current ellipsoid and Azimuthal Equidistant projection
 * parameters.  If any errors occur, the error code(s) are returned by the
 * function, otherwise AZEQ_NO_ERROR is returned.
 *
 *    Latitude          : Latitude (phi) in radians           (input)
 *    Longitude         : Longitude (lambda) in radians       (input)
 *    Easting           : Easting (X) in meters               (output)
 *    Northing          : Northing (Y) in meters              (output)
 */

  double dlam;       /* Longitude - Central Meridan */
  double k_prime;    /* scale factor */
  double c;          /* angular distance from center */
  double slat = sin(Latitude);
  double clat = cos(Latitude);
  double cos_c;
  double sin_dlam, cos_dlam;
  double Ra_kprime;
  double Ra_PI_OVER_2_Lat;
  long Error_Code = AZEQ_NO_ERROR;

  if ((Latitude < -PI_OVER_2) || (Latitude > PI_OVER_2))
  { /* Latitude out of range */
    Error_Code |= AZEQ_LAT_ERROR;
  }
  if ((Longitude < -PI) || (Longitude > TWO_PI))
  { /* Longitude out of range */
    Error_Code |= AZEQ_LON_ERROR;
  }
  if (!Error_Code)
  { /* no errors */
    dlam = Longitude - Azeq_Origin_Long;
    if (dlam > PI)
    {
      dlam -= TWO_PI;
    }
    if (dlam < -PI)
    {
      dlam += TWO_PI;
    }

    sin_dlam = sin(dlam);
    cos_dlam = cos(dlam);
    if (fabs(abs_Azeq_Origin_Lat - PI_OVER_2) < 1.0e-10)
    {
      if (Azeq_Origin_Lat >= 0.0)
      {
        Ra_PI_OVER_2_Lat = Ra * (PI_OVER_2 - Latitude);
        *Easting = Ra_PI_OVER_2_Lat * sin_dlam + Azeq_False_Easting;
        *Northing = -1.0 * (Ra_PI_OVER_2_Lat * cos_dlam) + Azeq_False_Northing;
      }
      else
      {
        Ra_PI_OVER_2_Lat = Ra * (PI_OVER_2 + Latitude);
        *Easting = Ra_PI_OVER_2_Lat * sin_dlam + Azeq_False_Easting;
        *Northing = Ra_PI_OVER_2_Lat * cos_dlam + Azeq_False_Northing;
      }
    }
    else if (abs_Azeq_Origin_Lat <= 1.0e-10)
    {
      cos_c = clat * cos_dlam;
      if (fabs(fabs(cos_c) - 1.0) < 1.0e-14)
      {
        if (cos_c >= 0.0)
        {
          *Easting = Azeq_False_Easting;
          *Northing = Azeq_False_Northing;
        }
        else
        {
          /* if cos_c == -1 */
          Error_Code |= AZEQ_PROJECTION_ERROR;
        }
      }
      else
      {
        c = acos(cos_c);
        k_prime = c / sin(c);
        Ra_kprime = Ra * k_prime;
        *Easting = Ra_kprime * clat * sin_dlam + Azeq_False_Easting;
        *Northing = Ra_kprime * slat + Azeq_False_Northing;
      }
    }
    else
    {
      cos_c = (Sin_Azeq_Origin_Lat * slat) + (Cos_Azeq_Origin_Lat * clat * cos_dlam);
      if (fabs(fabs(cos_c) - 1.0) < 1.0e-14)
      {
        if (cos_c >= 0.0)
        {
          *Easting = Azeq_False_Easting;
          *Northing = Azeq_False_Northing;
        }
        else
        {
          /* if cos_c == -1 */
          Error_Code |= AZEQ_PROJECTION_ERROR;
        }
      }
      else
      {
        c = acos(cos_c);
        k_prime = c / sin(c);
        Ra_kprime = Ra * k_prime;
        *Easting = Ra_kprime * clat * sin_dlam + Azeq_False_Easting;
        *Northing = Ra_kprime * (Cos_Azeq_Origin_Lat * slat - Sin_Azeq_Origin_Lat * clat * cos_dlam) + Azeq_False_Northing;
      }
    }
  }
  return (Error_Code);
} /* End Convert_Geodetic_To_Azimuthal_Equidistant */


long Convert_Azimuthal_Equidistant_To_Geodetic(double Easting,
                                               double Northing,
                                               double *Latitude,
                                               double *Longitude)
{ /* Begin Convert_Azimuthal_Equidistant_To_Geodetic */
/*
 * The function Convert_Azimuthal_Equidistant_To_Geodetic converts Azimuthal_Equidistant projection
 * (easting and northing) coordinates to geodetic (latitude and longitude)
 * coordinates, according to the current ellipsoid and Azimuthal_Equidistant projection
 * coordinates.  If any errors occur, the error code(s) are returned by the
 * function, otherwise AZEQ_NO_ERROR is returned.
 *
 *    Easting           : Easting (X) in meters                  (input)
 *    Northing          : Northing (Y) in meters                 (input)
 *    Latitude          : Latitude (phi) in radians              (output)
 *    Longitude         : Longitude (lambda) in radians          (output)
 */

  double dx, dy;
  double rho;        /* height above ellipsoid */
  double c;          /* angular distance from center */
  double sin_c, cos_c, dy_sinc;
  long Error_Code = AZEQ_NO_ERROR; 

  if ((Easting < (Azeq_False_Easting - Azeq_Delta_Easting)) 
      || (Easting > (Azeq_False_Easting + Azeq_Delta_Easting)))
  { /* Easting out of range  */
    Error_Code |= AZEQ_EASTING_ERROR;
  }
  if ((Northing < (Azeq_False_Northing - Azeq_Delta_Northing)) 
      || (Northing > (Azeq_False_Northing + Azeq_Delta_Northing)))
  { /* Northing out of range */
    Error_Code |= AZEQ_NORTHING_ERROR;
  }

  if (!Error_Code)
  {
    dy = Northing - Azeq_False_Northing;
    dx = Easting - Azeq_False_Easting;
    rho = sqrt(dx * dx + dy * dy);
    if (fabs(rho) <= 1.0e-10)
    {
      *Latitude = Azeq_Origin_Lat;
      *Longitude = Azeq_Origin_Long;
    }
    else
    {
      c = rho / Ra;
      sin_c = sin(c);
      cos_c = cos(c);
      dy_sinc = dy * sin_c;
      *Latitude = asin((cos_c * Sin_Azeq_Origin_Lat) + ((dy_sinc * Cos_Azeq_Origin_Lat) / rho));
      if (fabs(abs_Azeq_Origin_Lat - PI_OVER_2) < 1.0e-10)
      {
        if (Azeq_Origin_Lat >= 0.0)
          *Longitude = Azeq_Origin_Long + atan2(dx, -dy);
        else
          *Longitude = Azeq_Origin_Long + atan2(dx, dy);
      }
      else
        *Longitude = Azeq_Origin_Long + atan2((dx * sin_c), ((rho * Cos_Azeq_Origin_Lat * cos_c) - (dy_sinc * Sin_Azeq_Origin_Lat)));
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
  return (Error_Code);
} /* End Convert_Azimuthal_Equidistant_To_Geodetic */

