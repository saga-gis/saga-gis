/**********************************************************
 * Version $Id: grinten.c 911 2011-02-14 16:38:15Z reklov_w $
 *********************************************************/
/***************************************************************************/
/* RSC IDENTIFIER: VAN DER GRINTEN
 *
 * ABSTRACT
 *
 *    This component provides conversions between Geodetic coordinates
 *    (latitude and longitude in radians) and Van Der Grinten projection
 *    coordinates (easting and northing in meters).  The Van Der Grinten
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
 *          GRIN_NO_ERROR           : No errors occurred in function
 *          GRIN_LAT_ERROR          : Latitude outside of valid range
 *                                      (-90 to 90 degrees)
 *          GRIN_LON_ERROR          : Longitude outside of valid range
 *                                      (-180 to 360 degrees)
 *          GRIN_EASTING_ERROR      : Easting outside of valid range
 *                                      (False_Easting +/- ~20,000,000 m,
 *                                       depending on ellipsoid parameters)
 *          GRIN_NORTHING_ERROR     : Northing outside of valid range
 *                                      (False_Northing +/- ~20,000,000 m,
 *                                       depending on ellipsoid parameters)
 *          GRIN_RADIUS_ERROR       : Coordinates too far from pole,
 *                                      depending on ellipsoid and
 *                                      projection parameters
 *          GRIN_CENT_MER_ERROR     : Central meridian outside of valid range
 *                                      (-180 to 360 degrees)
 *          GRIN_A_ERROR            : Semi-major axis less than or equal to zero
 *          GRIN_INV_F_ERROR        : Inverse flattening outside of valid range
 *									                    (250 to 350)
 *
 * REUSE NOTES
 *
 *    VAN DER GRINTEN is intended for reuse by any application that performs a
 *    Van Der Grinten projection or its inverse.
 *
 * REFERENCES
 *
 *    Further information on VAN DER GRINTEN can be found in the Reuse Manual.
 *
 *    VAN DER GRINTEN originated from :  U.S. Army Topographic Engineering Center
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
 *    VAN DER GRINTEN has no restrictions.
 *
 * ENVIRONMENT
 *
 *    VAN DER GRINTEN was tested and certified in the following environments:
 *
 *    1. Solaris 2.5 with GCC, version 2.8.1
 *    2. Windows 95 with MS Visual C++, version 6
 *
 * MODIFICATIONS
 *
 *    Date              Description
 *    ----              -----------
 *    10-02-97          Original Code
 *
 */


/***************************************************************************/
/*
 *                               INCLUDES
 */

#include <math.h>
#include "grinten.h"

/*
 *    math.h    - Standard C math library
 *    grinten.h - Is for prototype error checking
 */


/***************************************************************************/
/*
 *                               DEFINES
 */

#define PI         3.14159265358979323e0  /* PI                            */
#define PI_OVER_2   ( PI / 2.0)                 
#define MAX_LAT     ( 90 * (PI / 180.0) )  /* 90 degrees in radians   */
#define TWO_PI      (2.0 * PI)                  
#define FLOAT_EQ(x,v,epsilon)   (((v - epsilon) < x) && (x < (v + epsilon)))


/***************************************************************************/
/*
 *                               GLOBALS
 */

const double TWO_OVER_PI = (2.0 / PI);
const double PI_OVER_3 = (PI / 3.0);
const double ONE_THIRD  = (1.0 / 3.0);

/* Ellipsoid Parameters, default to WGS 84 */
static double Grin_a = 6378137.0;                      /* Semi-major axis of ellipsoid in meters */
static double Grin_f = 1 / 298.257223563;              /* Flattening of ellipsoid */
static double es2 = 0.0066943799901413800;             /* Eccentricity (0.08181919084262188000) squared         */
static double es4 = 4.4814723452405e-005;              /* es2 * es2 */
static double es6 = 3.0000678794350e-007;              /* es4 * es2 */
static double Ra = 6371007.1810824;                    /* Spherical Radius */
static double PI_Ra = 20015109.356056;                  

/* Van Der Grinten projection Parameters */
static double Grin_Origin_Long = 0.0;                  /* Longitude of origin in radians    */
static double Grin_False_Easting = 0.0;
static double Grin_False_Northing = 0.0;
/*
 * These state variables are for optimization purposes.  The only function
 * that should modify them is Set_Van_der_Grinten_Parameters.
 */


/***************************************************************************/
/*
 *                              FUNCTIONS
 */


long Set_Van_der_Grinten_Parameters(double a,
                                    double f,
                                    double Central_Meridian,
                                    double False_Easting,
                                    double False_Northing)

{ /* BEGIN Set_Van_der_Grinten_Parameters */
/*
 * The function Set_Van_der_Grinten_Parameters receives the ellipsoid parameters and
 * projection parameters as inputs, and sets the corresponding state
 * variables.  If any errors occur, the error code(s) are returned by the function, 
 * otherwise Grin_NO_ERROR is returned.
 *
 *    a                 : Semi-major axis of ellipsoid, in meters   (input)
 *    f                 : Flattening of ellipsoid							      (input)
 *    Central_Meridian  : Longitude in radians at the center of     (input)
 *                          the projection
 *    False_Easting     : A coordinate value in meters assigned to the
 *                          central meridian of the projection.     (input)
 *    False_Northing    : A coordinate value in meters assigned to the
 *                          origin latitude of the projection       (input)
 */

  double inv_f = 1 / f;
  long Error_Code = GRIN_NO_ERROR;

  if (a <= 0.0)
  { /* Semi-major axis must be greater than zero */
    Error_Code |= GRIN_A_ERROR;
  }
  if ((inv_f < 250) || (inv_f > 350))
  { /* Inverse flattening must be between 250 and 350 */
    Error_Code |= GRIN_INV_F_ERROR;
  }
  if ((Central_Meridian < -PI) || (Central_Meridian > TWO_PI))
  { /* origin longitude out of range */
    Error_Code |= GRIN_CENT_MER_ERROR;
  }
  if (!Error_Code)
  { /* no errors */
    Grin_a = a;
    Grin_f = f;
    es2 = 2 * Grin_f - Grin_f * Grin_f;
    es4 = es2 * es2;
    es6 = es4 * es2;
    /* spherical radius */
    Ra = Grin_a * (1.0 - es2 / 6.0 - 17.0 * es4 / 360.0 - 67.0 * es6 /3024.0);
    PI_Ra = PI * Ra;
    if (Central_Meridian > PI)
      Central_Meridian -= TWO_PI;
    Grin_Origin_Long = Central_Meridian;
    Grin_False_Easting = False_Easting;
    Grin_False_Northing = False_Northing;

  } /* END OF if(!Error_Code) */
  return (Error_Code);
} /* END OF Set_Van_der_Grinten_Parameters */


void Get_Van_der_Grinten_Parameters(double *a,
                                    double *f,
                                    double *Central_Meridian,
                                    double *False_Easting,
                                    double *False_Northing)

{ /* BEGIN Get_Van_der_Grinten_Parameters */
/*
 * The function Get_Van_der_Grinten_Parameters returns the current ellipsoid
 * parameters, and Van Der Grinten projection parameters.
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

  *a = Grin_a;
  *f = Grin_f;
  *Central_Meridian = Grin_Origin_Long;
  *False_Easting = Grin_False_Easting;
  *False_Northing = Grin_False_Northing;
  return;
} /* END OF Get_Van_der_Grinten_Parameters */


long Convert_Geodetic_To_Van_der_Grinten (double Latitude,
                                          double Longitude,
                                          double *Easting,
                                          double *Northing)

{ /* BEGIN Convert_Geodetic_To_Van_der_Grinten */
/*
 * The function Convert_Geodetic_To_Van_der_Grinten converts geodetic (latitude and
 * longitude) coordinates to Van Der Grinten projection (easting and northing)
 * coordinates, according to the current ellipsoid and Van Der Grinten projection
 * parameters.  If any errors occur, the error code(s) are returned by the
 * function, otherwise GRIN_NO_ERROR is returned.
 *
 *    Latitude          : Latitude (phi) in radians           (input)
 *    Longitude         : Longitude (lambda) in radians       (input)
 *    Easting           : Easting (X) in meters               (output)
 *    Northing          : Northing (Y) in meters              (output)
 */

  double dlam;                      /* Longitude - Central Meridan */
  double aa, aasqr;
  double gg;
  double pp, ppsqr;
  double gg_MINUS_ppsqr, ppsqr_PLUS_aasqr;
  double in_theta;
  double theta;
  double sin_theta, cos_theta;
  double qq;
  long   Error_Code = GRIN_NO_ERROR;

  if ((Latitude < -PI_OVER_2) || (Latitude > PI_OVER_2))
  {  /* Latitude out of range */
    Error_Code |= GRIN_LAT_ERROR;
  }
  if ((Longitude < -PI) || (Longitude > TWO_PI))
  {  /* Longitude out of range */
    Error_Code|= GRIN_LON_ERROR;
  }

  if (!Error_Code)
  { /* no errors */

    dlam = Longitude - Grin_Origin_Long;
    if (dlam > PI)
    {
      dlam -= TWO_PI;
    }
    if (dlam < -PI)
    {
      dlam += TWO_PI;
    }

    if (Latitude == 0.0)
    {
      *Easting = Ra * dlam + Grin_False_Easting;
      *Northing = 0.0;
    }
    else if (dlam == 0.0 || FLOAT_EQ(Latitude,MAX_LAT,.00001)  || FLOAT_EQ(Latitude,-MAX_LAT,.00001))
    {
      in_theta = fabs(TWO_OVER_PI * Latitude);

      if (in_theta > 1.0)
        in_theta = 1.0;
      else if (in_theta < -1.0)
        in_theta = -1.0;

      theta = asin(in_theta);
      *Easting = 0.0;
      *Northing = PI_Ra * tan(theta / 2) + Grin_False_Northing;
      if (Latitude < 0.0)
        *Northing *= -1.0;
    }
    else
    {
      aa = 0.5 * fabs(PI / dlam - dlam / PI);
      in_theta = fabs(TWO_OVER_PI * Latitude);

      if (in_theta > 1.0)
        in_theta = 1.0;
      else if (in_theta < -1.0)
        in_theta = -1.0;

      theta = asin(in_theta);
      sin_theta = sin(theta);
      cos_theta = cos(theta);
      gg = cos_theta / (sin_theta + cos_theta - 1);
      pp = gg * (2 / sin_theta - 1);
      aasqr = aa * aa;
      ppsqr = pp * pp;
      gg_MINUS_ppsqr = gg - ppsqr;
      ppsqr_PLUS_aasqr = ppsqr + aasqr;
      qq = aasqr + gg;
      *Easting = PI_Ra * (aa * (gg_MINUS_ppsqr) +
                          sqrt(aasqr * (gg_MINUS_ppsqr) * (gg_MINUS_ppsqr) -
                               (ppsqr_PLUS_aasqr) * (gg * gg - ppsqr))) / 
                 (ppsqr_PLUS_aasqr) + Grin_False_Easting;
      if (dlam < 0.0)
        *Easting *= -1.0;
      *Northing = PI_Ra * (pp * qq - aa * sqrt ((aasqr + 1) * (ppsqr_PLUS_aasqr) - qq * qq)) /
                  (ppsqr_PLUS_aasqr) + Grin_False_Northing;
      if (Latitude < 0.0)
        *Northing *= -1.0;
    }
  }
  return (Error_Code);

} /* END OF Convert_Geodetic_To_Van_der_Grinten */


long Convert_Van_der_Grinten_To_Geodetic(double Easting,
                                         double Northing,
                                         double *Latitude,
                                         double *Longitude)
{ /* BEGIN Convert_Van_der_Grinten_To_Geodetic */
/*
 * The function Convert_Van_der_Grinten_To_Geodetic converts Grinten projection
 * (easting and northing) coordinates to geodetic (latitude and longitude)
 * coordinates, according to the current ellipsoid and Grinten projection
 * coordinates.  If any errors occur, the error code(s) are returned by the
 * function, otherwise GRIN_NO_ERROR is returned.
 *
 *    Easting           : Easting (X) in meters                  (input)
 *    Northing          : Northing (Y) in meters                 (input)
 *    Latitude          : Latitude (phi) in radians              (output)
 *    Longitude         : Longitude (lambda) in radians          (output)
 */

  double dx, dy;
  double xx, xxsqr;
  double yy, yysqr, two_yysqr;
  double xxsqr_PLUS_yysqr;
  double c1;
  double c2;
  double c3, c3sqr;
  double c2_OVER_3c3;
  double dd;
  double a1;
  double m1;
  double i;
  double theta1;
  double temp;
  const double epsilon = 1.0e-2;

  long Error_Code = GRIN_NO_ERROR;

  if ((Easting > (Grin_False_Easting + PI_Ra + epsilon)) ||
      (Easting < (Grin_False_Easting - PI_Ra - epsilon)))
  { /* Easting out of range */
    Error_Code |= GRIN_EASTING_ERROR;
  }
  if ((Northing > (Grin_False_Northing + PI_Ra + epsilon)) ||
      (Northing < (Grin_False_Northing - PI_Ra - epsilon)))
  { /* Northing out of range */
    Error_Code |= GRIN_NORTHING_ERROR;
  }
  if (!Error_Code)
  {
    temp = sqrt(Easting * Easting + Northing * Northing);     

    if ((temp > (Grin_False_Easting + PI_Ra + epsilon)) || 
        (temp > (Grin_False_Northing + PI_Ra + epsilon)) ||
        (temp < (Grin_False_Easting - PI_Ra - epsilon)) || 
        (temp < (Grin_False_Northing - PI_Ra - epsilon)))
    { /* Point is outside of projection area */
      Error_Code |= GRIN_RADIUS_ERROR;
    }
  }

  if (!Error_Code)
  {
    dy = Northing - Grin_False_Northing;
    dx = Easting - Grin_False_Easting;
    xx = dx / PI_Ra;
    yy = dy / PI_Ra;
    xxsqr = xx * xx;
    yysqr = yy * yy;
    xxsqr_PLUS_yysqr = xxsqr + yysqr;
    two_yysqr = 2 * yysqr;

    if (Northing == 0.0)
      *Latitude = 0.0;

    else
    {
      c1 = - fabs(yy) * (1 + xxsqr_PLUS_yysqr);
      c2 = c1 - two_yysqr + xxsqr;
      c3 = - 2 * c1 + 1 + two_yysqr + (xxsqr_PLUS_yysqr) * (xxsqr_PLUS_yysqr);
      c2_OVER_3c3 = c2 / (3.0 * c3);
      c3sqr = c3 * c3;
      dd = yysqr / c3 + ((2 * c2 * c2 * c2) / (c3sqr * c3) - (9 * c1 * c2) / (c3sqr)) / 27;
      a1 = (c1 - c2 * c2_OVER_3c3) /c3;
      m1 = 2 * sqrt(-ONE_THIRD * a1);
      i = 3 * dd/ (a1 * m1);
      if ((i > 1.0)||(i < -1.0))
        *Latitude = MAX_LAT;
      else
      {
        theta1 = ONE_THIRD * acos(3 * dd / (a1 * m1));
        *Latitude = PI * (-m1 * cos(theta1 + PI_OVER_3) - c2_OVER_3c3);
      }
    }
    if (Northing < 0.0)
      *Latitude *= -1.0;

    if (xx == 0.0)
      *Longitude = Grin_Origin_Long;
    else
    {
      *Longitude = PI * (xxsqr_PLUS_yysqr - 1 + 
                         sqrt(1 + (2 * xxsqr - two_yysqr) + (xxsqr_PLUS_yysqr) * (xxsqr_PLUS_yysqr))) / 
                   (2 * xx) + Grin_Origin_Long;
    }
    if (*Latitude > PI_OVER_2)  /* force distorted values to 90, -90 degrees */
      *Latitude = PI_OVER_2;
    else if (*Latitude < -PI_OVER_2)
      *Latitude = -PI_OVER_2;

    if (*Longitude > PI)
      *Longitude -= TWO_PI;
    if (*Longitude < -PI)
      *Longitude += TWO_PI;

    if (*Longitude > PI) /* force distorted values to 180, -180 degrees */
      *Longitude = PI;
    else if (*Longitude < -PI)
      *Longitude = -PI;

  }
  return (Error_Code);

} /* END OF Convert_Van_der_Grinten_To_Geodetic */

