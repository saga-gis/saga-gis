/**********************************************************
 * Version $Id: eqdcyl.c 911 2011-02-14 16:38:15Z reklov_w $
 *********************************************************/
/***************************************************************************/
/* RSC IDENTIFIER: EQUIDISTANT CYLINDRICAL
 *
 * ABSTRACT
 *
 *    This component provides conversions between Geodetic coordinates
 *    (latitude and longitude in radians) and Equidistant Cylindrical projection coordinates
 *    (easting and northing in meters).  The Equidistant Cylindrical
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
 *          EQCY_NO_ERROR           : No errors occurred in function
 *          EQCY_LAT_ERROR          : Latitude outside of valid range
 *                                      (-90 to 90 degrees)
 *          EQCY_LON_ERROR          : Longitude outside of valid range
 *                                      (-180 to 360 degrees)
 *          EQCY_EASTING_ERROR      : Easting outside of valid range
 *                                      (False_Easting +/- ~20,000,000 m,
 *                                       depending on ellipsoid parameters
 *                                       and Standard Parallel)
 *          EQCY_NORTHING_ERROR     : Northing outside of valid range
 *                                      (False_Northing +/- 0 to ~10,000,000 m,
 *                                       depending on ellipsoid parameters
 *                                       and Standard Parallel)
 *          EQCY_STDP_ERROR         : Standard parallel outside of valid range
 *                                      (-90 to 90 degrees)
 *          EQCY_CENT_MER_ERROR     : Central meridian outside of valid range
 *                                      (-180 to 360 degrees)
 *          EQCY_A_ERROR            : Semi-major axis less than or equal to zero
 *          EQCY_INV_F_ERROR        : Inverse flattening outside of valid range
 *									                    (250 to 350)
 *
 * REUSE NOTES
 *
 *    EQUIDISTANT CYLINDRICAL is intended for reuse by any application that performs a
 *    Equidistant Cylindrical projection or its inverse.
 *
 * REFERENCES
 *
 *    Further information on EQUIDISTANT CYLINDRICAL can be found in the Reuse Manual.
 *
 *    EQUIDISTANT CYLINDRICAL originated from :  U.S. Army Topographic Engineering Center
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
 *    EQUIDISTANT CYLINDRICAL has no restrictions.
 *
 * ENVIRONMENT
 *
 *    EQUIDISTANT CYLINDRICAL was tested and certified in the following environments:
 *
 *    1. Solaris 2.5 with GCC 2.8.1
 *    2. MS Windows with MS Visual C++ 6
 *
 * MODIFICATIONS
 *
 *    Date              Description
 *    ----              -----------
 *    04/16/99          Original Code
 *
 *
 */


/***************************************************************************/
/*
 *                               INCLUDES
 */

#include <math.h>
#include "eqdcyl.h"

/*
 *    math.h   - Standard C math library
 *    eqdcyl.h - Is for prototype error checking
 */


/***************************************************************************/
/*
 *                               DEFINES
 */

#define PI         3.14159265358979323e0  /* PI                            */
#define PI_OVER_2  ( PI / 2.0) 
#define TWO_PI     (2.0 * PI)                  
#define ONE		   (1.0 * PI / 180)       /* 1 degree in radians           */

/***************************************************************************/
/*
 *                               GLOBALS
 */

/* Ellipsoid Parameters, default to WGS 84 */
static double Eqcy_a = 6378137.0;                  /* Semi-major axis of ellipsoid in meters */
static double Eqcy_f = 1 / 298.257223563;          /* Flattening of ellipsoid */
static double es2 = 0.0066943799901413800;         /* Eccentricity (0.08181919084262188000) squared         */
static double es4 = 4.4814723452405e-005;          /* es2 * es2	 */
static double es6 = 3.0000678794350e-007;          /* es4 * es2  */
static double Ra = 6371007.1810824;                /* Spherical Radius  */

/* Equidistant Cylindrical projection Parameters */
static double Eqcy_Std_Parallel = 0.0;             /* Latitude of standard parallel in radians     */
static double Cos_Eqcy_Std_Parallel = 1.0;         /* cos(Eqcy_Std_Parallel)  */
static double Eqcy_Origin_Long = 0.0;              /* Longitude of origin in radians    */
static double Eqcy_False_Easting = 0.0;
static double Eqcy_False_Northing = 0.0;
static double Eqcy_Delta_Northing = 10007555.0;
static double Eqcy_Max_Easting =  20015110.0;
static double Eqcy_Min_Easting =  -20015110.0;
static double Ra_Cos_Eqcy_Std_Parallel = 6371007.1810824; /* Ra * Cos_Eqcy_Std_Parallel */
/*
 * These state variables are for optimization purposes.  The only function
 * that should modify them is Set_Equidistant_Cyl_Parameters.
 */


/***************************************************************************/
/*
 *                              FUNCTIONS
 */


long Set_Equidistant_Cyl_Parameters(double a,
                                    double f,
                                    double Std_Parallel,
                                    double Central_Meridian,
                                    double False_Easting,
                                    double False_Northing)
{ /* Begin Set_Equidistant_Cyl_Parameters */
/*
 * The function Set_Equidistant_Cyl_Parameters receives the ellipsoid parameters and
 * projection parameters as inputs, and sets the corresponding state
 * variables.  It also calculates the spherical radius of the sphere having 
 * the same area as the ellipsoid.  If any errors occur, the error code(s) 
 * are returned by the function, otherwise EQCY_NO_ERROR is returned.
 *
 *    a                 : Semi-major axis of ellipsoid, in meters   (input)
 *    f                 : Flattening of ellipsoid						        (input)
 *    Std_Parallel      : Latitude in radians at which the          (input)
 *                          point scale factor is 1.0
 *    Central_Meridian  : Longitude in radians at the center of     (input)
 *                          the projection
 *    False_Easting     : A coordinate value in meters assigned to the
 *                          central meridian of the projection.     (input)
 *    False_Northing    : A coordinate value in meters assigned to the
 *                          standard parallel of the projection     (input)
 */

  double temp;
  double inv_f = 1 / f;
  long Error_Code = EQCY_NO_ERROR;

  if (a <= 0.0)
  { /* Semi-major axis must be greater than zero */
    Error_Code |= EQCY_A_ERROR;
  }
  if ((inv_f < 250) || (inv_f > 350))
  { /* Inverse flattening must be between 250 and 350 */
    Error_Code |= EQCY_INV_F_ERROR;
  }
  if ((Std_Parallel < -PI_OVER_2) || (Std_Parallel > PI_OVER_2))
  { /* standard parallel out of range */
    Error_Code |= EQCY_STDP_ERROR;
  }
  if ((Central_Meridian < -PI) || (Central_Meridian > TWO_PI))
  { /* origin longitude out of range */
    Error_Code |= EQCY_CENT_MER_ERROR;
  }
  if (!Error_Code)
  { /* no errors */
    Eqcy_a = a;
    Eqcy_f = f;
    es2 = 2 * Eqcy_f - Eqcy_f * Eqcy_f;
    es4 = es2 * es2;
    es6 = es4 * es2;
    /* spherical radius */
    Ra = Eqcy_a * (1.0 - es2 / 6.0 - 17.0 * es4 / 360.0 - 67.0 * es6 /3024.0);
    Eqcy_Std_Parallel = Std_Parallel;
    Cos_Eqcy_Std_Parallel = cos(Eqcy_Std_Parallel);
    Ra_Cos_Eqcy_Std_Parallel = Ra * Cos_Eqcy_Std_Parallel;
    if (Central_Meridian > PI)
      Central_Meridian -= TWO_PI;
    Eqcy_Origin_Long = Central_Meridian;
    Eqcy_False_Easting = False_Easting;
    Eqcy_False_Northing = False_Northing;
    if (Eqcy_Origin_Long > 0)
    {
      Convert_Geodetic_To_Equidistant_Cyl(PI_OVER_2, Eqcy_Origin_Long - PI - ONE, &Eqcy_Max_Easting, &temp);
      Convert_Geodetic_To_Equidistant_Cyl(PI_OVER_2, Eqcy_Origin_Long - PI, &Eqcy_Min_Easting, &temp);
    }
    else if (Eqcy_Origin_Long < 0)
    {
      Convert_Geodetic_To_Equidistant_Cyl(PI_OVER_2, Eqcy_Origin_Long + PI, &Eqcy_Max_Easting, &temp);
      Convert_Geodetic_To_Equidistant_Cyl(PI_OVER_2, Eqcy_Origin_Long + PI + ONE, &Eqcy_Min_Easting, &temp);
    }
    else
    {
      Convert_Geodetic_To_Equidistant_Cyl(PI_OVER_2, PI, &Eqcy_Max_Easting, &temp);
      Eqcy_Min_Easting = -Eqcy_Max_Easting;
    }
  } /* End if(!Error_Code) */
  return (Error_Code);
} /* End Set_Equidistant_Cyl_Parameters */


void Get_Equidistant_Cyl_Parameters(double *a,
                                    double *f,
                                    double *Std_Parallel,
                                    double *Central_Meridian,
                                    double *False_Easting,
                                    double *False_Northing)
{ /* Begin Get_Equidistant_Cyl_Parameters */
/*
 * The function Get_Equidistant_Cyl_Parameters returns the current ellipsoid
 * parameters and Equidistant Cylindrical projection parameters.
 *
 *    a                 : Semi-major axis of ellipsoid, in meters   (output)
 *    f                 : Flattening of ellipsoid						        (output)
 *    Std_Parallel      : Latitude in radians at which the          (output)
 *                          point scale factor is 1.0
 *    Central_Meridian  : Longitude in radians at the center of     (output)
 *                          the projection
 *    False_Easting     : A coordinate value in meters assigned to the
 *                          central meridian of the projection.     (output)
 *    False_Northing    : A coordinate value in meters assigned to the
 *                          standard parallel of the projection     (output)
 */

  *a = Eqcy_a;
  *f = Eqcy_f;
  *Std_Parallel = Eqcy_Std_Parallel;
  *Central_Meridian = Eqcy_Origin_Long;
  *False_Easting = Eqcy_False_Easting;
  *False_Northing = Eqcy_False_Northing;
  return;
} /* End Get_Equidistant_Cyl_Parameters */


long Convert_Geodetic_To_Equidistant_Cyl (double Latitude,
                                          double Longitude,
                                          double *Easting,
                                          double *Northing)

{ /* Begin Convert_Geodetic_To_Equidistant_Cyl */
/*
 * The function Convert_Geodetic_To_Equidistant_Cyl converts geodetic (latitude and
 * longitude) coordinates to Equidistant Cylindrical projection (easting and northing)
 * coordinates, according to the current ellipsoid, spherical radiius
 * and Equidistant Cylindrical projection parameters.
 * If any errors occur, the error code(s) are returned by the
 * function, otherwise EQCY_NO_ERROR is returned.
 *
 *    Latitude          : Latitude (phi) in radians           (input)
 *    Longitude         : Longitude (lambda) in radians       (input)
 *    Easting           : Easting (X) in meters               (output)
 *    Northing          : Northing (Y) in meters              (output)
 */

  double dlam;     /* Longitude - Central Meridan */
  long Error_Code = EQCY_NO_ERROR;

  if ((Latitude < -PI_OVER_2) || (Latitude > PI_OVER_2))
  {  /* Latitude out of range */
    Error_Code |= EQCY_LAT_ERROR;
  }
  if ((Longitude < -PI) || (Longitude > TWO_PI))
  {  /* Longitude out of range */
    Error_Code|= EQCY_LON_ERROR;
  }

  if (!Error_Code)
  { /* no errors */
    dlam = Longitude - Eqcy_Origin_Long;
    if (dlam > PI)
    {
      dlam -= TWO_PI;
    }
    if (dlam < -PI)
    {
      dlam += TWO_PI;
    }

    *Easting = Ra_Cos_Eqcy_Std_Parallel * dlam + Eqcy_False_Easting;
    *Northing = Ra * Latitude + Eqcy_False_Northing;

  }
  return (Error_Code);

} /* End Convert_Geodetic_To_Equidistant_Cyl */


long Convert_Equidistant_Cyl_To_Geodetic(double Easting,
                                         double Northing,
                                         double *Latitude,
                                         double *Longitude)
{ /* Begin Convert_Equidistant_Cyl_To_Geodetic */
/*
 * The function Convert_Equidistant_Cyl_To_Geodetic converts Equidistant Cylindrical projection
 * (easting and northing) coordinates to geodetic (latitude and longitude)
 * coordinates, according to the current ellipsoid, spherical radius
 * and Equidistant Cylindrical projection coordinates.
 * If any errors occur, the error code(s) are returned by the
 * function, otherwise EQCY_NO_ERROR is returned.
 *
 *    Easting           : Easting (X) in meters                  (input)
 *    Northing          : Northing (Y) in meters                 (input)
 *    Latitude          : Latitude (phi) in radians              (output)
 *    Longitude         : Longitude (lambda) in radians          (output)
 */

  double dx, dy;
  long Error_Code = EQCY_NO_ERROR;

  if ((Easting < (Eqcy_False_Easting + Eqcy_Min_Easting))
      || (Easting > (Eqcy_False_Easting + Eqcy_Max_Easting)))
  { /* Easting out of range */
    Error_Code |= EQCY_EASTING_ERROR;
  }
  if ((Northing < (Eqcy_False_Northing - Eqcy_Delta_Northing))
      || (Northing > (Eqcy_False_Northing + Eqcy_Delta_Northing)))
  { /* Northing out of range */
    Error_Code |= EQCY_NORTHING_ERROR;
  }

  if (!Error_Code)
  {
    dy = Northing - Eqcy_False_Northing;
    dx = Easting - Eqcy_False_Easting;
    *Latitude = dy / Ra;

    if (Ra_Cos_Eqcy_Std_Parallel == 0)
      *Longitude = 0;
    else
      *Longitude = Eqcy_Origin_Long + dx / Ra_Cos_Eqcy_Std_Parallel;

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

} /* End Convert_Equidistant_Cyl_To_Geodetic */

