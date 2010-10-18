/***************************************************************************/
/* RSC IDENTIFIER: STEREOGRAPHIC 
 *
 *
 * ABSTRACT
 *
 *    This component provides conversions between geodetic (latitude and
 *    longitude) coordinates and Stereographic (easting and northing) 
 *    coordinates.
 *
 * ERROR HANDLING
 *
 *    This component checks parameters for valid values.  If an invalid 
 *    value is found the error code is combined with the current error code 
 *    using the bitwise or.  This combining allows multiple error codes to 
 *    be returned. The possible error codes are:
 *
 *          STEREO_NO_ERROR           : No errors occurred in function
 *          STEREO_LAT_ERROR          : Latitude outside of valid range
 *                                      (-90 to 90 degrees)
 *          STEREO_LON_ERROR          : Longitude outside of valid range
 *                                      (-180 to 360 degrees) 
 *          STEREO_ORIGIN_LAT_ERROR   : Origin latitude outside of valid range
 *                                      (-90 to 90 degrees)
 *          STEREO_CENT_MER_ERROR     : Central meridian outside of valid range
 *                                      (-180 to 360 degrees)
 *          STEREO_EASTING_ERROR      : Easting outside of valid range,
 *                                      (False_Easting +/- ~1,460,090,226 m,
 *                                       depending on ellipsoid and projection
 *                                       parameters)
 *          STEREO_NORTHING_ERROR     : Northing outside of valid range,
 *                                      (False_Northing +/- ~1,460,090,226 m,
 *                                       depending on ellipsoid and projection
 *                                       parameters)
 *          STEREO_A_ERROR            : Semi-major axis less than or equal to zero
 *          STEREO_INV_F_ERROR        : Inverse flattening outside of valid range
 *								  	                  (250 to 350)
 *
 *
 * REUSE NOTES
 *
 *    STEREOGRAPHIC is intended for reuse by any application that  
 *    performs a Stereographic projection.
 *
 *
 * REFERENCES
 *
 *    Further information on STEREOGRAPHIC can be found in the
 *    Reuse Manual.
 *
 *
 *    STEREOGRAPHIC originated from :
 *                                U.S. Army Topographic Engineering Center
 *                                Geospatial Information Division
 *                                7701 Telegraph Road
 *                                Alexandria, VA  22310-3864
 *
 *
 * LICENSES
 *
 *    None apply to this component.
 *
 *
 * RESTRICTIONS
 *
 *    STEREOGRAPHIC has no restrictions.
 *
 *
 * ENVIRONMENT
 *
 *    STEREOGRAPHIC was tested and certified in the following
 *    environments:
 *
 *    1. Solaris 2.5 with GCC, version 2.8.1
 *    2. Window 95 with MS Visual C++, version 6
 *
 *
 * MODIFICATIONS
 *
 *    Date              Description
 *    ----              -----------
 *    7/26/00           Original Code
 *
 */


/************************************************************************/
/*
 *                               INCLUDES
 */

#include <math.h>
#include "stereogr.h"

/*
 *    math.h      - Standard C math library
 *    stereogr.h  - Is for prototype error checking
 */


/************************************************************************/
/*                               DEFINES
 *
 */


#define PI           3.14159265358979323e0       /* PI     */
#define PI_OVER_2    (PI / 2.0)           
#define PI_OVER_4    (PI / 4.0)           
#define TWO_PI       (2.0 * PI)
#define ONE          (1.0 * PI / 180.0)         /* One degree */

/************************************************************************/
/*                           GLOBAL DECLARATIONS
 *
 */
/* Ellipsoid Parameters, default to WGS 84  */
static double Stereo_a = 6378137.0;                    /* Semi-major axis of ellipsoid, in meters  */
static double Stereo_f = 1 / 298.257223563;            /* Flattening of ellipsoid  */
static double Stereo_Ra = 6371007.1810824;             /* Spherical Radius */
static double Two_Stereo_Ra = 12742014.3621648;        /* 2 * Spherical Radius */
static long Stereo_At_Pole = 0;                        /* Flag variable */

/* Stereographic projection Parameters */
static double Stereo_Origin_Lat = 0.0;                 /* Latitude of origin, in radians */
static double Stereo_Origin_Long = 0.0;                /* Longitude of origin, in radians */
static double Stereo_False_Easting = 0.0;              /* False easting, in meters */
static double Stereo_False_Northing = 0.0;             /* False northing, in meters */
static double Sin_Stereo_Origin_Lat = 0.0;             /* sin(Stereo_Origin_Lat) */
static double Cos_Stereo_Origin_Lat = 1.0;             /* cos(Stereo_Origin_Lat) */

/* Maximum variance for easting and northing values for WGS 84. */
static double Stereo_Delta_Easting = 1460090226.0;
static double Stereo_Delta_Northing = 1460090226.0;

/* These state variables are for optimization purposes. The only function
 * that should modify them is Set_Stereographic_Parameters.         
 */


/************************************************************************/
/*                              FUNCTIONS
 *
 */

long Set_Stereographic_Parameters (double a,
                                   double f,
                                   double Origin_Latitude,
                                   double Central_Meridian,
                                   double False_Easting,
                                   double False_Northing)

{  /* BEGIN Set_Stereographic_Parameters   */
/*  
 *  The function Set_Stereographic_Parameters receives the ellipsoid
 *  parameters and Stereograpic projection parameters as inputs, and
 *  sets the corresponding state variables.  If any errors occur, error
 *  code(s) are returned by the function, otherwise STEREO_NO_ERROR is returned.
 *
 *  a                : Semi-major axis of ellipsoid, in meters         (input)
 *  f                : Flattening of ellipsoid					               (input)
 *  Origin_Latitude  : Latitude, in radians, at the center of          (input)
 *                       the projection
 *  Central_Meridian : Longitude, in radians, at the center of         (input)
 *                       the projection
 *  False_Easting    : Easting (X) at center of projection, in meters  (input)
 *  False_Northing   : Northing (Y) at center of projection, in meters (input)
 */

  double es2, es4, es6;
  double temp = 0;
  double inv_f = 1 / f;
  long Error_Code = STEREO_NO_ERROR;

  if (a <= 0.0)
  { /* Semi-major axis must be greater than zero */
    Error_Code |= STEREO_A_ERROR;
  }
  if ((inv_f < 250) || (inv_f > 350))
  { /* Inverse flattening must be between 250 and 350 */
    Error_Code |= STEREO_INV_F_ERROR;
  }
  if ((Origin_Latitude < -PI_OVER_2) || (Origin_Latitude > PI_OVER_2))
  { /* origin latitude out of range */
    Error_Code |= STEREO_ORIGIN_LAT_ERROR;
  }
  if ((Central_Meridian < -PI) || (Central_Meridian > TWO_PI))
  { /* origin longitude out of range */
    Error_Code |= STEREO_CENT_MER_ERROR;
  }
  if (!Error_Code)
  { /* no errors */
    Stereo_a = a;
    Stereo_f = f;
    es2 = 2 * Stereo_f - Stereo_f * Stereo_f;
    es4 = es2 * es2;
    es6 = es4 * es2;
    Stereo_Ra = Stereo_a * (1.0 - es2 / 6.0 - 17.0 * es4 / 360.0 - 67.0 * es6 /3024.0);
    Two_Stereo_Ra = 2.0 * Stereo_Ra;
    Stereo_Origin_Lat = Origin_Latitude;
    Sin_Stereo_Origin_Lat = sin(Stereo_Origin_Lat);
    Cos_Stereo_Origin_Lat = cos(Stereo_Origin_Lat);
    if (Central_Meridian > PI)
      Central_Meridian -= TWO_PI;
    Stereo_Origin_Long = Central_Meridian;
    Stereo_False_Easting = False_Easting;
    Stereo_False_Northing = False_Northing;
    if(fabs(fabs(Stereo_Origin_Lat) - PI_OVER_2) < 1.0e-10)
      Stereo_At_Pole = 1;
    else
      Stereo_At_Pole = 0;

    if ((Stereo_At_Pole) || (fabs(Stereo_Origin_Lat) < 1.0e-10))
    {
      Stereo_Delta_Easting = 1460090226.0;
    }
    else
    {
      if (Stereo_Origin_Long <= 0)
        Convert_Geodetic_To_Stereographic(-Stereo_Origin_Lat, PI + Stereo_Origin_Long - ONE, &Stereo_Delta_Easting, &temp);
      else
        Convert_Geodetic_To_Stereographic(-Stereo_Origin_Lat, Stereo_Origin_Long - PI - ONE, &Stereo_Delta_Easting, &temp);
    }

  } /* END OF if(!Error_Code) */
  return (Error_Code);
} /* END OF Set_Stereographic_Parameters */

void Get_Stereographic_Parameters(double *a,
                                  double *f,
                                  double *Origin_Latitude,
                                  double *Central_Meridian,
                                  double *False_Easting,
                                  double *False_Northing)

{ /* BEGIN Get_Stereographic_Parameters */
/*
 * The function Get_Stereographic_Parameters returns the current ellipsoid
 * parameters and Stereographic projection parameters.
 *
 *    a                 : Semi-major axis of ellipsoid, in meters   (output)
 *    f                 : Flattening of ellipsoid						        (output)
 *    Origin_Latitude   : Latitude, in radians, at the center of    (output)
 *                          the projection
 *    Central_Meridian  : Longitude, in radians, at the center of   (output)
 *                          the projection
 *    False_Easting     : A coordinate value, in meters, assigned to the
 *                          central meridian of the projection.     (output)
 *    False_Northing    : A coordinate value, in meters, assigned to the
 *                          origin latitude of the projection       (output) 
 */

  *a = Stereo_a;
  *f = Stereo_f;
  *Origin_Latitude = Stereo_Origin_Lat;
  *Central_Meridian = Stereo_Origin_Long;
  *False_Easting = Stereo_False_Easting;
  *False_Northing = Stereo_False_Northing;
  return;
} /* END OF Get_Stereographic_Parameters */

long Convert_Geodetic_To_Stereographic (double Latitude,
                                        double Longitude,
                                        double *Easting,
                                        double *Northing)

{  /* BEGIN Convert_Geodetic_To_Stereographic */

/*
 * The function Convert_Geodetic_To_Stereographic converts geodetic
 * coordinates (latitude and longitude) to Stereographic coordinates
 * (easting and northing), according to the current ellipsoid
 * and Stereographic projection parameters. If any errors occur, error
 * code(s) are returned by the function, otherwise STEREO_NO_ERROR is returned.
 *
 *    Latitude   :  Latitude, in radians                      (input)
 *    Longitude  :  Longitude, in radians                     (input)
 *    Easting    :  Easting (X), in meters                    (output)
 *    Northing   :  Northing (Y), in meters                   (output)
 */

  double g, k;
  double num = 0;
  double Ra_k = 0;
  double slat = sin(Latitude);
  double clat = cos(Latitude);
  double dlam;                        /* Longitude - Central Meridan */
  double cos_dlam;
  long   Error_Code = STEREO_NO_ERROR;

  if ((Latitude < -PI_OVER_2) || (Latitude > PI_OVER_2))
  {  /* Latitude out of range */
    Error_Code |= STEREO_LAT_ERROR;
  }
  if ((Longitude < -PI) || (Longitude > TWO_PI))
  {  /* Longitude out of range */
    Error_Code|= STEREO_LON_ERROR;
  }
  if (!Error_Code)
  {  /* no errors */


    dlam = Longitude - Stereo_Origin_Long;
    if (dlam > PI)
    {
      dlam -= TWO_PI;
    }
    if (dlam < -PI)
    {
      dlam += TWO_PI;
    }

    cos_dlam = cos(dlam);
    g = 1.0 + Sin_Stereo_Origin_Lat * slat + Cos_Stereo_Origin_Lat * clat * cos_dlam;
    if (fabs(g) <= 1.0e-10)
    {  /* Point is out of view.  Will return longitude out of range message
          since no point out of view is implemented.  */
      Error_Code |= STEREO_LON_ERROR;
    }
    else
    {
      if (Stereo_At_Pole)
      {
        if (fabs(fabs(Latitude) - PI_OVER_2) < 1.0e-10)
        {
          *Easting = Stereo_False_Easting;
          *Northing = Stereo_False_Northing;
        }
        else
        {
          if (Stereo_Origin_Lat > 0)
          {
            num = Two_Stereo_Ra * tan(PI_OVER_4 - Latitude / 2.0);
            *Easting = Stereo_False_Easting + num * sin(dlam);
            *Northing = Stereo_False_Northing + (-num * cos_dlam);
          }
          else
          {
            num = Two_Stereo_Ra * tan(PI_OVER_4 + Latitude / 2.0);
            *Easting = Stereo_False_Easting + num * sin(dlam);
            *Northing = Stereo_False_Northing + num * cos_dlam;
          }
        }
      }
      else
      {
        if (fabs(Stereo_Origin_Lat) <= 1.0e-10)
        {
          k = 2.0 / (1.0 + clat * cos_dlam);
          Ra_k = Stereo_Ra * k;
          *Northing = Stereo_False_Northing + Ra_k * slat;
        }
        else
        {
          k = 2.0 / g;
          Ra_k = Stereo_Ra * k;
          *Northing = Stereo_False_Northing + Ra_k * (Cos_Stereo_Origin_Lat * slat - Sin_Stereo_Origin_Lat * clat * cos_dlam);
        }
        *Easting = Stereo_False_Easting + Ra_k * clat * sin(dlam);
      }
    }
  }
  return (Error_Code);
} /* END OF Convert_Geodetic_To_Stereographic */
  
long Convert_Stereographic_To_Geodetic(double Easting,
                                       double Northing,
                                       double *Latitude,
                                       double *Longitude)
{ /* BEGIN Convert_Stereographic_To_Geodetic */
/*
 * The function Convert_Stereographic_To_Geodetic converts Stereographic projection
 * (easting and northing) coordinates to geodetic (latitude and longitude)
 * coordinates, according to the current ellipsoid and Stereographic projection
 * coordinates.  If any errors occur, the error code(s) are returned by the
 * function, otherwise STEREO_NO_ERROR is returned.
 *
 *    Easting           : Easting (X), in meters              (input)
 *    Northing          : Northing (Y), in meters             (input)
 *    Latitude          : Latitude (phi), in radians          (output)
 *    Longitude         : Longitude (lambda), in radians      (output)
 */

  double dx, dy;
  double rho, c;
  double sin_c, cos_c;
  double dy_sin_c;
  long Error_Code = STEREO_NO_ERROR;

  if ((Easting < (Stereo_False_Easting - Stereo_Delta_Easting))
      ||(Easting > (Stereo_False_Easting + Stereo_Delta_Easting)))
  { /* Easting out of range  */
    Error_Code |= STEREO_EASTING_ERROR;
  }
  if ((Northing < (Stereo_False_Northing - Stereo_Delta_Northing))
      || (Northing > (Stereo_False_Northing + Stereo_Delta_Northing)))
  { /* Northing out of range */
    Error_Code |= STEREO_NORTHING_ERROR;
  }
  if (!Error_Code)
  { /* no errors */

    dy = Northing - Stereo_False_Northing;
    dx = Easting - Stereo_False_Easting;
    rho = sqrt(dx * dx + dy * dy);
    if (fabs(rho) <= 1.0e-10)
    {
      *Latitude = Stereo_Origin_Lat;
      *Longitude = Stereo_Origin_Long;
    }
    else
    {
      c = 2.0 * atan(rho / (Two_Stereo_Ra));
      sin_c = sin(c);
      cos_c = cos(c);
      dy_sin_c = dy * sin_c;
      if (Stereo_At_Pole)
      {
        if (Stereo_Origin_Lat > 0)
          *Longitude = Stereo_Origin_Long + atan2(dx, -dy);
        else
          *Longitude = Stereo_Origin_Long + atan2(dx, dy);
      }
      else
        *Longitude = Stereo_Origin_Long + atan2(dx * sin_c, (rho * Cos_Stereo_Origin_Lat * cos_c - dy_sin_c * Sin_Stereo_Origin_Lat));
      *Latitude = asin(cos_c * Sin_Stereo_Origin_Lat + ((dy_sin_c * Cos_Stereo_Origin_Lat) / rho));
    }

    if (fabs(*Latitude) < 2.2e-8)  /* force lat to 0 to avoid -0 degrees */
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
  return (Error_Code);
} /* END OF Convert_Stereographic_To_Geodetic */
