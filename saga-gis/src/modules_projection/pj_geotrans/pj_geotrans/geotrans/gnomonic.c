/***************************************************************************/
/* RSC IDENTIFIER: GNOMONIC
 *
 * ABSTRACT
 *
 *    This component provides conversions between Geodetic coordinates
 *    (latitude and longitude in radians) and Gnomonic
 *    projection coordinates (easting and northing in meters).  This projection 
 *    employs a spherical Earth model.  The spherical radius used is the radius 
 *    of the sphere having the same area as the ellipsoid.
 *
 * ERROR HANDLING
 *
 *    This component checks parameters for valid values.  If an invalid value
 *    is found the error code is combined with the current error code using
 *    the bitwise or.  This combining allows multiple error codes to be
 *    returned. The possible error codes are:
 *
 *       GNOM_NO_ERROR           : No errors occurred in function
 *       GNOM_LAT_ERROR          : Latitude outside of valid range
 *                                     (-90 to 90 degrees)
 *       GNOM_LON_ERROR          : Longitude outside of valid range
 *                                     (-180 to 360 degrees)
 *       GNOM_EASTING_ERROR      : Easting outside of valid range
 *                                     (depends on ellipsoid and projection
 *                                     parameters)
 *       GNOM_NORTHING_ERROR     : Northing outside of valid range
 *                                     (depends on ellipsoid and projection
 *                                     parameters)
 *       GNOM_ORIGIN_LAT_ERROR   : Origin latitude outside of valid range
 *                                     (-90 to 90 degrees)
 *       GNOM_CENT_MER_ERROR     : Central meridian outside of valid range
 *                                     (-180 to 360 degrees)
 *       GNOM_A_ERROR            : Semi-major axis less than or equal to zero
 *       GNOM_INV_F_ERROR        : Inverse flattening outside of valid range
 *									                   (250 to 350)
 *
 *
 * REUSE NOTES
 *
 *    GNOMONIC is intended for reuse by any application that 
 *    performs a Gnomonic projection or its inverse.
 *
 * REFERENCES
 *
 *    Further information on GNOMONIC can be found in the Reuse Manual.
 *
 *    GNOMONIC originated from:     U.S. Army Topographic Engineering Center
 *                                  Geospatial Information Division
 *                                  7701 Telegraph Road
 *                                  Alexandria, VA  22310-3864
 *
 * LICENSES
 *
 *    None apply to this component.
 *
 * RESTRICTIONS
 *
 *    GNOMONIC has no restrictions.
 *
 * ENVIRONMENT
 *
 *    GNOMONIC was tested and certified in the following environments:
 *
 *    1. Solaris 2.5 with GCC, version 2.8.1
 *    2. MSDOS with MS Visual C++, version 6
 *
 * MODIFICATIONS
 *
 *    Date              Description
 *    ----              -----------
 *    05-22-00          Original Code
 *    
 *
 */



/***************************************************************************/
/*
 *                               INCLUDES
 */

#include <math.h>
#include "gnomonic.h"

/*
 *    math.h       - Standard C math library
 *    gnomonic.h   - Is for prototype error checking
 */


/***************************************************************************/
/*
 *                               DEFINES
 */

#define PI         3.14159265358979323e0  /* PI                            */
#define PI_OVER_2  ( PI / 2.0)                 
#define TWO_PI     ( 2.0 * PI)                 

/***************************************************************************/
/*
 *                               GLOBALS
 */
/* Ellipsoid Parameters, default to WGS 84 */
static double Gnom_a = 6378137.0;                  /* Semi-major axis of ellipsoid in meters */
static double Gnom_f = 1 / 298.257223563;          /* Flattening of ellipsoid */
static double Ra = 6371007.1810824;                /* Spherical Radius */
static double Sin_Gnom_Origin_Lat = 0.0;
static double Cos_Gnom_Origin_Lat = 1.0;

/* Gnomonic projection Parameters */
static double Gnom_Origin_Lat = 0.0;               /* Latitude of origin in radians */
static double Gnom_Origin_Long = 0.0;              /* Longitude of origin in radians */
static double Gnom_False_Northing = 0.0;           /* False northing in meters */
static double Gnom_False_Easting = 0.0;            /* False easting in meters */
static double abs_Gnom_Origin_Lat = 0.0;

static double Gnom_Delta_Northing = 40000000;
static double Gnom_Delta_Easting =  40000000;
/*
 * These state variables are for optimization purposes.  The only function
 * that should modify them is Set_Gnomonic_Parameters.
 */
/***************************************************************************/
/*
 *                              FUNCTIONS     
 */

long Set_Gnomonic_Parameters(double a,
                             double f,
                             double Origin_Latitude,
                             double Central_Meridian,
                             double False_Easting,
                             double False_Northing)
{ /* BEGIN Set_Gnomonic_Parameters */
/*
 * The function Set_Gnomonic_Parameters receives the ellipsoid parameters and
 * projection parameters as inputs, and sets the corresponding state
 * variables.  If any errors occur, the error code(s) are returned by the function, 
 * otherwise GNOM_NO_ERROR is returned.
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

  double es2, es4, es6;
  double inv_f = 1 / f;
  long Error_Code = GNOM_NO_ERROR;

  if (a <= 0.0)
  { /* Semi-major axis must be greater than zero */
    Error_Code |= GNOM_A_ERROR;
  }
  if ((inv_f < 250) || (inv_f > 350))
  { /* Inverse flattening must be between 250 and 350 */
    Error_Code |= GNOM_INV_F_ERROR;
  }
  if ((Origin_Latitude < -PI_OVER_2) || (Origin_Latitude > PI_OVER_2))
  { /* origin latitude out of range */
    Error_Code |= GNOM_ORIGIN_LAT_ERROR;
  }
  if ((Central_Meridian < -PI) || (Central_Meridian > TWO_PI))
  { /* origin longitude out of range */
    Error_Code |= GNOM_CENT_MER_ERROR;
  }
  if (!Error_Code)
  { /* no errors */
    Gnom_a = a;
    Gnom_f = f;
    es2 = 2 * Gnom_f - Gnom_f * Gnom_f;
    es4 = es2 * es2;
    es6 = es4 * es2;
    /* spherical radius */
    Ra = Gnom_a * (1.0 - es2 / 6.0 - 17.0 * es4 / 360.0 - 67.0 * es6 / 3024.0);
    Gnom_Origin_Lat = Origin_Latitude;
    Sin_Gnom_Origin_Lat = sin(Gnom_Origin_Lat);
    Cos_Gnom_Origin_Lat = cos(Gnom_Origin_Lat);
    abs_Gnom_Origin_Lat = fabs(Gnom_Origin_Lat);
    if (Central_Meridian > PI)
      Central_Meridian -= TWO_PI;
    Gnom_Origin_Long = Central_Meridian;
    Gnom_False_Northing = False_Northing;
    Gnom_False_Easting = False_Easting;
  } /* End if(!Error_Code) */
  return (Error_Code);
} /* End Set_Gnomonic_Parameters */


void Get_Gnomonic_Parameters(double *a,
                             double *f,
                             double *Origin_Latitude,
                             double *Central_Meridian,
                             double *False_Easting,
                             double *False_Northing)
{ /* Begin Get_Gnomonic_Parameters */
/*
 * The function Get_Gnomonic_Parameters returns the current ellipsoid
 * parameters and Gnomonic projection parameters.
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

  *a = Gnom_a;
  *f = Gnom_f;
  *Origin_Latitude = Gnom_Origin_Lat;
  *Central_Meridian = Gnom_Origin_Long;
  *False_Easting = Gnom_False_Easting;
  *False_Northing = Gnom_False_Northing;
  return;
} /* End Get_Gnomonic_Parameters */


long Convert_Geodetic_To_Gnomonic (double Latitude,
                                   double Longitude,
                                   double *Easting,
                                   double *Northing)
{ /* Begin Convert_Geodetic_To_Gnomonic */
/*
 * The function Convert_Geodetic_To_Gnomonic converts geodetic (latitude and
 * longitude) coordinates to Gnomonic projection (easting and northing)
 * coordinates, according to the current ellipsoid and Gnomonic projection
 * parameters.  If any errors occur, the error code(s) are returned by the
 * function, otherwise GNOM_NO_ERROR is returned.
 *
 *    Latitude          : Latitude (phi) in radians           (input)
 *    Longitude         : Longitude (lambda) in radians       (input)
 *    Easting           : Easting (X) in meters               (output)
 *    Northing          : Northing (Y) in meters              (output)
 */

  double dlam;       /* Longitude - Central Meridan */
  double cos_c;      
  double k_prime;    /* scale factor */
  double Ra_kprime;
  double slat = sin(Latitude);
  double clat = cos(Latitude);
  double Ra_cotlat;
  double sin_dlam, cos_dlam;
  double temp_Easting, temp_Northing;
  long Error_Code = GNOM_NO_ERROR;

  if ((Latitude < -PI_OVER_2) || (Latitude > PI_OVER_2))
  { /* Latitude out of range */
    Error_Code |= GNOM_LAT_ERROR;
  }
  if ((Longitude < -PI) || (Longitude > TWO_PI))
  { /* Longitude out of range */
    Error_Code |= GNOM_LON_ERROR;
  }
  dlam = Longitude - Gnom_Origin_Long;
  sin_dlam = sin(dlam);
  cos_dlam = cos(dlam);
  cos_c = Sin_Gnom_Origin_Lat * slat + Cos_Gnom_Origin_Lat * clat * cos_dlam;
  if (cos_c <= 1.0e-10)
  {  /* Point is out of view.  Will return longitude out of range message
    since no point out of view is implemented.  */
    Error_Code |= GNOM_LON_ERROR;
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
    if (fabs(abs_Gnom_Origin_Lat - PI_OVER_2) < 1.0e-10)
    {
      Ra_cotlat = Ra * (clat / slat);
      temp_Easting = Ra_cotlat * sin_dlam;
      temp_Northing = Ra_cotlat * cos_dlam;
      if (Gnom_Origin_Lat >= 0.0)
      {
        *Easting = temp_Easting + Gnom_False_Easting;
        *Northing = -1.0 * temp_Northing + Gnom_False_Northing;
      }
      else
      {
        *Easting = -1.0 * temp_Easting + Gnom_False_Easting;
        *Northing = temp_Northing + Gnom_False_Northing;
      }
    }
    else if (abs_Gnom_Origin_Lat <= 1.0e-10)
    {
      *Easting = Ra * tan(dlam) + Gnom_False_Easting;
      *Northing = Ra * tan(Latitude) / cos_dlam + Gnom_False_Northing;
    }
    else
    {
      k_prime = 1 / cos_c;
      Ra_kprime = Ra * k_prime;
      *Easting = Ra_kprime * clat * sin_dlam + Gnom_False_Easting;
      *Northing = Ra_kprime * (Cos_Gnom_Origin_Lat * slat - Sin_Gnom_Origin_Lat * clat * cos_dlam) + Gnom_False_Northing;
    }
  }
  return (Error_Code);
} /* End Convert_Geodetic_To_Gnomonic */


long Convert_Gnomonic_To_Geodetic(double Easting,
                                  double Northing,
                                  double *Latitude,
                                  double *Longitude)
{ /* Begin Convert_Gnomonic_To_Geodetic */
/*
 * The function Convert_Gnomonic_To_Geodetic converts Gnomonic projection
 * (easting and northing) coordinates to geodetic (latitude and longitude)
 * coordinates, according to the current ellipsoid and Gnomonic projection
 * coordinates.  If any errors occur, the error code(s) are returned by the
 * function, otherwise GNOM_NO_ERROR is returned.
 *
 *    Easting           : Easting (X) in meters                  (input)
 *    Northing          : Northing (Y) in meters                 (input)
 *    Latitude          : Latitude (phi) in radians              (output)
 *    Longitude         : Longitude (lambda) in radians          (output)
 */

  double dx, dy;
  double rho;
  double c;
  double sin_c, cos_c;
  double dy_sinc;
  long Error_Code = GNOM_NO_ERROR; 

  if ((Easting < (Gnom_False_Easting - Gnom_Delta_Easting)) 
      || (Easting > (Gnom_False_Easting + Gnom_Delta_Easting)))
  { /* Easting out of range  */
    Error_Code |= GNOM_EASTING_ERROR;
  }
  if ((Northing < (Gnom_False_Northing - Gnom_Delta_Northing)) 
      || (Northing > (Gnom_False_Northing + Gnom_Delta_Northing)))
  { /* Northing out of range */
    Error_Code |= GNOM_NORTHING_ERROR;
  }
  if (!Error_Code)
  {
    dy = Northing - Gnom_False_Northing;
    dx = Easting - Gnom_False_Easting;
    rho = sqrt(dx * dx + dy * dy);
    if (fabs(rho) <= 1.0e-10)
    {
      *Latitude = Gnom_Origin_Lat;
      *Longitude = Gnom_Origin_Long;
    }
    else
    {
      c = atan(rho / Ra);
      sin_c = sin(c);
      cos_c = cos(c);
      dy_sinc = dy * sin_c;
      *Latitude = asin((cos_c * Sin_Gnom_Origin_Lat) + ((dy_sinc * Cos_Gnom_Origin_Lat) / rho));
      if (fabs(abs_Gnom_Origin_Lat - PI_OVER_2) < 1.0e-10)
      {
        if (Gnom_Origin_Lat >= 0.0)
          *Longitude = Gnom_Origin_Long + atan2(dx, -dy);
        else
          *Longitude = Gnom_Origin_Long + atan2(dx, dy);
      }
      else
        *Longitude = Gnom_Origin_Long + atan2((dx * sin_c), (rho * Cos_Gnom_Origin_Lat * cos_c - dy_sinc * Sin_Gnom_Origin_Lat));
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
} /* End Convert_Gnomonic_To_Geodetic */



