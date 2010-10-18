/***************************************************************************/
/* RSC IDENTIFIER: ECKERT6
 *
 * ABSTRACT
 *
 *    This component provides conversions between Geodetic coordinates
 *    (latitude and longitude in radians) and Eckert VI projection coordinates
 *    (easting and northing in meters).  This projection employs a spherical
 *    Earth model.  The spherical radius used is the radius of the sphere
 *    having the same area as the ellipsoid.
 *
 * ERROR HANDLING
 *
 *    This component checks parameters for valid values.  If an invalid value
 *    is found, the error code is combined with the current error code using 
 *    the bitwise or.  This combining allows multiple error codes to be
 *    returned. The possible error codes are:
 *
 *          ECK6_NO_ERROR           : No errors occurred in function
 *          ECK6_LAT_ERROR          : Latitude outside of valid range
 *                                      (-90 to 90 degrees)
 *          ECK6_LON_ERROR          : Longitude outside of valid range
 *                                      (-180 to 360 degrees)
 *          ECK6_EASTING_ERROR      : Easting outside of valid range
 *                                      (False_Easting +/- ~18,000,000 m,
 *                                       depending on ellipsoid parameters)
 *          ECK6_NORTHING_ERROR     : Northing outside of valid range
 *                                      (False_Northing +/- 0 to ~8,000,000 m,
 *                                       depending on ellipsoid parameters)
 *          ECK6_CENT_MER_ERROR     : Central meridian outside of valid range
 *                                      (-180 to 360 degrees)
 *          ECK6_A_ERROR            : Semi-major axis less than or equal to zero
 *          ECK6_INV_F_ERROR        : Inverse flattening outside of valid range
 *									                    (250 to 350)
 *
 * REUSE NOTES
 *
 *    ECKERT6 is intended for reuse by any application that performs a
 *    Eckert VI projection or its inverse.
 *
 * REFERENCES
 *
 *    Further information on ECKERT6 can be found in the Reuse Manual.
 *
 *    ECKERT6 originated from :  U.S. Army Topographic Engineering Center
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
 *    ECKERT6 has no restrictions.
 *
 * ENVIRONMENT
 *
 *    ECKERT6 was tested and certified in the following environments:
 *
 *    1. Solaris 2.5 with GCC 2.8.1
 *    2. MS Windows 95 with MS Visual C++ 6
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
#include "eckert6.h"

/*
 *    math.h     - Is needed to call the math functions (sqrt, pow, exp, log,
 *                   sin, cos, tan, and atan).
 *    eckert6.h - Is for prototype error checking.
 */


/***************************************************************************/
/*
 *                               DEFINES
 */

#define PI         3.14159265358979323e0      /* PI                            */
#define PI_OVER_2  ( PI / 2.0)                
#define MAX_LAT    ( (PI * 90) / 180.0 )  /* 90 degrees in radians    */
#define TWO_PI     (2.0 * PI)                  

/***************************************************************************/
/*
 *                               GLOBALS
 */

const double one_PLUS_PI_OVER_2 = (1.0 + PI / 2.0);

/* Ellipsoid Parameters, default to WGS 84 */
static double Eck6_a = 6378137.0;                      /* Semi-major axis of ellipsoid in meters */
static double Eck6_f = 1 / 298.257223563;              /* Flattening of ellipsoid */
static double es2 = 0.0066943799901413800;             /* Eccentricity (0.08181919084262188000) squared         */
static double es4 = 4.4814723452405e-005;              /* es2 * es2	*/
static double es6 = 3.0000678794350e-007;              /* es4 * es2  */
static double Ra_Over_Sqrt_Two_Plus_PI = 2809695.5356062;      /* Ra(6371007.1810824)/Sqrt(2.0 + PI) */
static double Inv_Ra_Over_Sqrt_Two_Plus_PI = 3.5591044913137e-007; /* Sqrt(2.0 + PI)/Ra(6371007.1810824) */


/* Eckert6 projection Parameters */
static double Eck6_Origin_Long = 0.0;                         /* Longitude of origin in radians    */
static double Eck6_False_Easting = 0.0;
static double Eck6_False_Northing = 0.0;
static double Eck6_Delta_Northing = 8826919.0;
static double Eck6_Max_Easting =  17653838.0;
static double Eck6_Min_Easting =  -17653838.0;
/*
 * These state variables are for optimization purposes.  The only function
 * that should modify them is Set_Eckert6_Parameters.
 */


/***************************************************************************/
/*
 *                              FUNCTIONS
 */


long Set_Eckert6_Parameters(double a,
                            double f,
                            double Central_Meridian,
                            double False_Easting,
                            double False_Northing)
{ /* Begin Set_Eckert6_Parameters */
/*
 * The function Set_Eckert6_Parameters receives the ellipsoid parameters and
 * Eckert VI projection parameters as inputs, and sets the corresponding state
 * variables.  If any errors occur, the error code(s) are returned by the 
 * function, otherwise ECK6_NO_ERROR is returned.
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

  double Ra;                      /* Spherical radius */
  double inv_f = 1 / f;
  long Error_Code = ECK6_NO_ERROR;

  if (a <= 0.0)
  { /* Semi-major axis must be greater than zero */
    Error_Code |= ECK6_A_ERROR;
  }
  if ((inv_f < 250) || (inv_f > 350))
  { /* Inverse flattening must be between 250 and 350 */
    Error_Code |= ECK6_INV_F_ERROR;
  }
  if ((Central_Meridian < -PI) || (Central_Meridian > TWO_PI))
  { /* origin longitude out of range */
    Error_Code |= ECK6_CENT_MER_ERROR;
  }
  if (!Error_Code)
  { /* no errors */
    Eck6_a = a;
    Eck6_f = f;
    es2 = 2 * Eck6_f - Eck6_f * Eck6_f;
    es4 = es2 * es2;
    es6 = es4 * es2;
    /* spherical radius */
    Ra = Eck6_a * (1.0 - es2 / 6.0 - 17.0 * es4 / 360.0 - 67.0 * es6 /3024.0);
    Ra_Over_Sqrt_Two_Plus_PI = Ra / (sqrt(2.0 + PI));
    Inv_Ra_Over_Sqrt_Two_Plus_PI = 1 / Ra_Over_Sqrt_Two_Plus_PI;
    if (Central_Meridian > PI)
      Central_Meridian -= TWO_PI;
    Eck6_Origin_Long = Central_Meridian;
    Eck6_False_Easting = False_Easting;
    Eck6_False_Northing = False_Northing;
    if (Eck6_Origin_Long > 0)
    {
      Eck6_Max_Easting = 17555761.0;
      Eck6_Min_Easting = -17653839.0;
    }
    else if (Eck6_Origin_Long < 0)
    {
      Eck6_Max_Easting = 17653838.0;
      Eck6_Min_Easting = -17555761.0;
    }
    else
    {
      Eck6_Max_Easting = 17653838.0;
      Eck6_Min_Easting = -17653838.0;
    }

  } /* End if(!Error_Code) */
  return (Error_Code);
} /* End Set_Eckert6_Parameters */


void Get_Eckert6_Parameters(double *a,
                            double *f,
                            double *Central_Meridian,
                            double *False_Easting,
                            double *False_Northing)
{ /* Begin Get_Eckert6_Parameters */
/*
 * The function Get_Eckert6_Parameters returns the current ellipsoid
 * parameters and Eckert VI projection parameters.
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

  *a = Eck6_a;
  *f = Eck6_f;
  *Central_Meridian = Eck6_Origin_Long;
  *False_Easting = Eck6_False_Easting;
  *False_Northing = Eck6_False_Northing;
  return;
} /* End Get_Eckert6_Parameters */


long Convert_Geodetic_To_Eckert6 (double Latitude,
                                  double Longitude,
                                  double *Easting,
                                  double *Northing)

{ /* Begin Convert_Geodetic_To_Eckert6 */
/*
 * The function Convert_Geodetic_To_Eckert6 converts geodetic (latitude and
 * longitude) coordinates to Eckert VI projection (easting and northing)
 * coordinates, according to the current ellipsoid and Eckert VI projection
 * parameters.  If any errors occur, the error code(s) are returned by the
 * function, otherwise ECK6_NO_ERROR is returned.
 *
 *    Latitude          : Latitude (phi) in radians           (input)
 *    Longitude         : Longitude (lambda) in radians       (input)
 *    Easting           : Easting (X) in meters               (output)
 *    Northing          : Northing (Y) in meters              (output)
 */

  double slat = sin(Latitude);
  double dlam;     /* Longitude - Central Meridan */
  double theta = Latitude;
  double delta_theta = 1.0;
  double dt_tolerance = 4.85e-10;        /* approximately 1/1000th of
                                             an arc second or 1/10th meter */
  long Error_Code = ECK6_NO_ERROR;

  if ((Latitude < -PI_OVER_2) || (Latitude > PI_OVER_2))
  {  /* Latitude out of range */
    Error_Code |= ECK6_LAT_ERROR;
  }
  if ((Longitude < -PI) || (Longitude > TWO_PI))
  {  /* Longitude out of range */
    Error_Code|= ECK6_LON_ERROR;
  }

  if (!Error_Code)
  { /* no errors */

    dlam = Longitude - Eck6_Origin_Long;
    if (dlam > PI)
    {
      dlam -= TWO_PI;
    }
    if (dlam < -PI)
    {
      dlam += TWO_PI;
    }
    while (fabs(delta_theta) > dt_tolerance)
    {
      delta_theta = -(theta + sin(theta) - one_PLUS_PI_OVER_2 *
                      slat) / (1.0 + cos(theta));
      theta += delta_theta;
    }
    *Easting = Ra_Over_Sqrt_Two_Plus_PI * dlam * (1.0 + cos(theta)) +
               Eck6_False_Easting;
    *Northing = 2.0 * Ra_Over_Sqrt_Two_Plus_PI  * theta + Eck6_False_Northing;

  }
  return (Error_Code);

} /* End Convert_Geodetic_To_Eckert6 */


long Convert_Eckert6_To_Geodetic(double Easting,
                                 double Northing,
                                 double *Latitude,
                                 double *Longitude)
{ /* Begin Convert_Eckert6_To_Geodetic */
/*
 * The function Convert_Eckert6_To_Geodetic converts Eckert VI projection
 * (easting and northing) coordinates to geodetic (latitude and longitude)
 * coordinates, according to the current ellipsoid and Eckert VI projection
 * coordinates.  If any errors occur, the error code(s) are returned by the
 * function, otherwise ECK6_NO_ERROR is returned.
 *
 *    Easting           : Easting (X) in meters                  (input)
 *    Northing          : Northing (Y) in meters                 (input)
 *    Latitude          : Latitude (phi) in radians              (output)
 *    Longitude         : Longitude (lambda) in radians          (output)
 */

  double dx, dy;
  double theta;
  double i;

  long Error_Code = ECK6_NO_ERROR;

  if ((Easting < (Eck6_False_Easting + Eck6_Min_Easting))
      || (Easting > (Eck6_False_Easting + Eck6_Max_Easting)))
  { /* Easting out of range  */
    Error_Code |= ECK6_EASTING_ERROR;
  }
  if ((Northing < (Eck6_False_Northing - Eck6_Delta_Northing))
      || (Northing > (Eck6_False_Northing + Eck6_Delta_Northing)))
  { /* Northing out of range */
    Error_Code |= ECK6_NORTHING_ERROR;
  }

  if (!Error_Code)
  {
    dy = Northing - Eck6_False_Northing;
    dx = Easting - Eck6_False_Easting;
    theta = Inv_Ra_Over_Sqrt_Two_Plus_PI * dy / 2.0;
    i = (theta + sin(theta)) / one_PLUS_PI_OVER_2;
    if (i > 1.0)
      *Latitude = MAX_LAT;
    else if (i < -1.0)
      *Latitude = -MAX_LAT;
    else
      *Latitude = asin(i);
    *Longitude = Eck6_Origin_Long + Inv_Ra_Over_Sqrt_Two_Plus_PI * dx / (1 + cos(theta));

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

} /* End Convert_Eckert6_To_Geodetic */

