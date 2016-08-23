/**********************************************************
 * Version $Id: loccart.c 911 2011-02-14 16:38:15Z reklov_w $
 *********************************************************/
/***************************************************************************/
/* RSC IDENTIFIER:  LOCAL CARTESIAN
 *
 * ABSTRACT
 *
 *    This component provides conversions between Geodetic coordinates (latitude,
 *    longitude in radians and height in meters) and Local Cartesian coordinates 
 *    (X, Y, Z).
 *
 * ERROR HANDLING
 *
 *    This component checks parameters for valid values.  If an invalid value
 *    is found, the error code is combined with the current error code using 
 *    the bitwise or.  This combining allows multiple error codes to be
 *    returned. The possible error codes are:
 *
 *      LOCCART_NO_ERROR            : No errors occurred in function
 *      LOCCART_LAT_ERROR           : Latitude out of valid range
 *                                      (-90 to 90 degrees)
 *      LOCCART_LON_ERROR           : Longitude out of valid range
 *                                      (-180 to 360 degrees)
 *      LOCCART_A_ERROR             : Semi-major axis less than or equal to zero
 *      LOCCART_INV_F_ERROR         : Inverse flattening outside of valid range
 *									                    (250 to 350)
 *      LOCCART_ORIGIN_LAT_ERROR    : Origin Latitude out of valid range
 *                                      (-90 to 90 degrees)
 *      LOCCART_ORIGIN_LON_ERROR    : Origin Longitude out of valid range
 *                                      (-180 to 360 degrees)
 *		  LOCCART_ORIENTATION_ERROR   : Orientation angle out of valid range
 *									                    (-360 to 360 degrees)
 *
 *
 * REUSE NOTES
 *
 *    LOCCART is intended for reuse by any application that performs
 *    coordinate conversions between geodetic coordinates or geocentric
 *    coordinates and local cartesian coordinates..
 *    
 *
 * REFERENCES
 *    
 *    Further information on GEOCENTRIC can be found in the Reuse Manual.
 *
 *    LOCCART originated from : U.S. Army Topographic Engineering Center
 *                              Geospatial Inforamtion Division
 *                              7701 Telegraph Road
 *                              Alexandria, VA  22310-3864
 *
 * LICENSES
 *
 *    None apply to this component.
 *
 * RESTRICTIONS
 *
 *    LOCCART has no restrictions.
 *
 * ENVIRONMENT
 *
 *    LOCCART was tested and certified in the following environments:
 *
 *    1. Solaris 2.5 with GCC version 2.8.1
 *    2. Windows 95 with MS Visual C++ version 6
 *
 * MODIFICATIONS
 *
 *    Date              Description
 *    ----              -----------
 *	  07-16-99			Original Code
 *
 */



/***************************************************************************/
/*
 *                               INCLUDES
 */

#include <math.h>
#include "loccart.h"
#include "geocent.h"

/*
 *    math.h    - Standard C math library
 *    geocent.h - Is needed to call the Convert_Geodetic_to_Geocentric and
 *                    Convert_Geocentric_to_Geodetic functions
 *    loccart.h - Is for prototype error checking.
 */

/***************************************************************************/
/*
 *                               DEFINES
 */

#define PI         3.14159265358979323e0  /* PI                            */
#define PI_OVER_2  ( PI / 2.0e0)                 
#define TWO_PI     (2.0 * PI)                    


/***************************************************************************/
/*
 *                               GLOBALS
 */

/* Ellipsoid Parameters, default to WGS 84 */
static double LocalCart_a = 6378137.0;         /* Semi-major axis of ellipsoid in meters */
static double LocalCart_f = 1 / 298.257223563; /* Flattening of ellipsoid */
static double es2 = 0.0066943799901413800;     /* Eccentricity (0.08181919084262188000) squared         */
static double u0 = 6378137.0;                  /* Geocentric origin coordinates in */
static double v0 = 0.0;                        /* terms of Local Cartesian origin  */
static double w0 = 0.0;                        /* parameters                       */


/* Local Cartesian Projection Parameters */
static double LocalCart_Origin_Lat = 0.0;      /* Latitude of origin in radians     */
static double LocalCart_Origin_Long = 0.0;     /* Longitude of origin in radians    */
static double LocalCart_Origin_Height = 0.0;   /* Height of origin in meters        */
static double LocalCart_Orientation = 0.0;     /* Orientation of Y axis in radians  */

static double Sin_LocalCart_Origin_Lat = 0.0;  /* sin(LocalCart_Origin_Lat)         */
static double Cos_LocalCart_Origin_Lat = 1.0;  /* cos(LocalCart_Origin_Lat)         */
static double Sin_LocalCart_Origin_Lon = 0.0;  /* sin(LocalCart_Origin_Lon)         */
static double Cos_LocalCart_Origin_Lon = 1.0;  /* cos(LocalCart_Origin_Lon)         */
static double Sin_LocalCart_Orientation = 0.0; /* sin(LocalCart_Orientation)        */
static double Cos_LocalCart_Orientation = 1.0; /* cos(LocalCart_Orientation)        */

static double Sin_Lat_Sin_Orient = 0.0; /* sin(LocalCart_Origin_Lat) * sin(LocalCart_Orientation) */
static double Sin_Lat_Cos_Orient = 0.0; /* sin(LocalCart_Origin_Lat) * cos(LocalCart_Orientation) */
static double Cos_Lat_Cos_Orient = 1.0; /* cos(LocalCart_Origin_Lat) * cos(LocalCart_Orientation) */
static double Cos_Lat_Sin_Orient = 0.0; /* cos(LocalCart_Origin_Lat) * sin(LocalCart_Orientation) */
/*
 * These state variables are for optimization purposes.  The only function
 * that should modify them is Set_Local_Cartesian_Parameters.
 */


/***************************************************************************/
/*
 *                              FUNCTIONS
 */


long Set_Local_Cartesian_Parameters (double a, 
                                     double f,
                                     double Origin_Latitude,
                                     double Origin_Longitude,
                                     double Origin_Height,
                                     double Orientation)

{ /* BEGIN Set_Local_Cartesian_Parameters */
/*
 * The function Set_Local_Cartesian_Parameters receives the ellipsoid parameters
 * and local origin parameters as inputs and sets the corresponding state variables.
 *
 *    a                : Semi-major axis of ellipsoid, in meters           (input)
 *    f                : Flattening of ellipsoid					                 (input)
 *    Origin_Latitude  : Latitude of the local origin, in radians          (input)
 *    Origin_Longitude : Longitude of the local origin, in radians         (input)
 *    Origin_Height    : Ellipsoid height of the local origin, in meters   (input)
 *    Orientation      : Orientation angle of the local cartesian coordinate system,
 *                           in radians                                    (input)
 */

  double N0;
  double inv_f = 1 / f;
  double val;
  long Error_Code = LOCCART_NO_ERROR;

  if (a <= 0.0)
  { /* Semi-major axis must be greater than zero */
    Error_Code |= LOCCART_A_ERROR;
  }
  if ((inv_f < 250) || (inv_f > 350))
  { /* Inverse flattening must be between 250 and 350 */
    Error_Code |= LOCCART_INV_F_ERROR;
  }
  if ((Origin_Latitude < -PI_OVER_2) || (Origin_Latitude > PI_OVER_2))
  { /* origin latitude out of range */
    Error_Code |= LOCCART_ORIGIN_LAT_ERROR;
  }
  if ((Origin_Longitude < -PI) || (Origin_Longitude > TWO_PI))
  { /* origin longitude out of range */
    Error_Code |= LOCCART_ORIGIN_LON_ERROR;
  }
  if ((Orientation < -PI) || (Orientation > TWO_PI))
  { /* orientation angle out of range */
    Error_Code |= LOCCART_ORIENTATION_ERROR;
  }

  if (!Error_Code)
  { /* no errors */
    LocalCart_a = a;
    LocalCart_f = f;
    LocalCart_Origin_Lat = Origin_Latitude;
    if (Origin_Longitude > PI)
      Origin_Longitude -= TWO_PI;
    LocalCart_Origin_Long = Origin_Longitude;
    LocalCart_Origin_Height = Origin_Height;
    if (Orientation > PI)
      Orientation -= TWO_PI;
    LocalCart_Orientation = Orientation;
    es2 = 2 * LocalCart_f - LocalCart_f * LocalCart_f;

    Sin_LocalCart_Origin_Lat = sin(LocalCart_Origin_Lat);
    Cos_LocalCart_Origin_Lat = cos(LocalCart_Origin_Lat);
    Sin_LocalCart_Origin_Lon = sin(LocalCart_Origin_Long);
    Cos_LocalCart_Origin_Lon = cos(LocalCart_Origin_Long);
    Sin_LocalCart_Orientation = sin(LocalCart_Orientation);
    Cos_LocalCart_Orientation = cos(LocalCart_Orientation);

    Sin_Lat_Sin_Orient = Sin_LocalCart_Origin_Lat * Sin_LocalCart_Orientation;
    Sin_Lat_Cos_Orient = Sin_LocalCart_Origin_Lat * Cos_LocalCart_Orientation;
    Cos_Lat_Cos_Orient = Cos_LocalCart_Origin_Lat * Cos_LocalCart_Orientation;
    Cos_Lat_Sin_Orient = Cos_LocalCart_Origin_Lat * Sin_LocalCart_Orientation;

    N0 = LocalCart_a / sqrt(1 - es2 * Sin_LocalCart_Origin_Lat * Sin_LocalCart_Origin_Lat);
   
    val = (N0 + LocalCart_Origin_Height) * Cos_LocalCart_Origin_Lat;
    u0 = val * Cos_LocalCart_Origin_Lon;
    v0 = val * Sin_LocalCart_Origin_Lon;
    w0 = ((N0 * (1 - es2)) + LocalCart_Origin_Height) * Sin_LocalCart_Origin_Lat;

  } /* END OF if(!Error_Code) */
  return (Error_Code);
} /* END OF Set_Local_Cartesian_Parameters */


void Get_Local_Cartesian_Parameters (double *a, 
                                     double *f,
                                     double *Origin_Latitude,
                                     double *Origin_Longitude,
                                     double *Origin_Height,
                                     double *Orientation)

{ /* BEGIN Get_Local_Cartesian_Parameters */
/*
 * The function Get_Local_Cartesian_Parameters returns the ellipsoid parameters
 * and local origin parameters.
 *
 *    a                : Semi-major axis of ellipsoid, in meters           (output)
 *    f                : Flattening of ellipsoid					                 (output)
 *    Origin_Latitude  : Latitude of the local origin, in radians          (output)
 *    Origin_Longitude : Longitude of the local origin, in radians         (output)
 *    Origin_Height    : Ellipsoid height of the local origin, in meters   (output)
 *    Orientation      : Orientation angle of the local cartesian coordinate system,
 *                           in radians                                    (output)
 */

  *a = LocalCart_a;
  *f = LocalCart_f;
  *Origin_Latitude = LocalCart_Origin_Lat;
  *Origin_Longitude = LocalCart_Origin_Long;
  *Origin_Height = LocalCart_Origin_Height;
  *Orientation = LocalCart_Orientation;

} /* END OF Get_Local_Cartesian_Parameters */


void Convert_Geocentric_To_Local_Cartesian (double u, 
                                            double v,
                                            double w,
                                            double *X, 
                                            double *Y,
                                            double *Z)
{ /* BEGIN Convert_Geocentric_To_Local_Cartesian
/*
 * The function Convert_Geocentric_To_Local_Cartesian converts geocentric
 * coordinates according to the current ellipsoid and local origin parameters.
 *
 *    u         : Geocentric latitude, in meters                       (input)
 *    v         : Geocentric longitude, in meters                      (input)
 *    w         : Geocentric height, in meters                         (input)
 *    X         : Calculated local cartesian X coordinate, in meters   (output)
 *    Y         : Calculated local cartesian Y coordinate, in meters   (output)
 *    Z         : Calculated local cartesian Z coordinate, in meters   (output)
 *
 */

  double u_MINUS_u0, v_MINUS_v0, w_MINUS_w0;

  u_MINUS_u0 = u - u0;
  v_MINUS_v0 = v - v0;
  w_MINUS_w0 = w - w0;

  if (LocalCart_Orientation == 0.0)
  {
    double cos_lon_u_MINUS_u0 = Cos_LocalCart_Origin_Lon * u_MINUS_u0;
    double sin_lon_v_MINUS_v0 = Sin_LocalCart_Origin_Lon * v_MINUS_v0;

    *X = -Sin_LocalCart_Origin_Lon * u_MINUS_u0 + Cos_LocalCart_Origin_Lon * v_MINUS_v0;
    *Y = -Sin_LocalCart_Origin_Lat * cos_lon_u_MINUS_u0 + -Sin_LocalCart_Origin_Lat * sin_lon_v_MINUS_v0 + Cos_LocalCart_Origin_Lat * w_MINUS_w0;
    *Z = Cos_LocalCart_Origin_Lat * cos_lon_u_MINUS_u0 + Cos_LocalCart_Origin_Lat * sin_lon_v_MINUS_v0 + Sin_LocalCart_Origin_Lat * w_MINUS_w0;
  }
  else
  {
    double cos_lat_w_MINUS_w0 = Cos_LocalCart_Origin_Lat * w_MINUS_w0;

    *X = (-Cos_LocalCart_Orientation * Sin_LocalCart_Origin_Lon + Sin_Lat_Sin_Orient * Cos_LocalCart_Origin_Lon) * u_MINUS_u0 +
         (Cos_LocalCart_Orientation * Cos_LocalCart_Origin_Lon + Sin_Lat_Sin_Orient * Sin_LocalCart_Origin_Lon) * v_MINUS_v0 +
         (-Sin_LocalCart_Orientation * cos_lat_w_MINUS_w0);

    *Y = (-Sin_LocalCart_Orientation * Sin_LocalCart_Origin_Lon - Sin_Lat_Cos_Orient * Cos_LocalCart_Origin_Lon) * u_MINUS_u0 +
         (Sin_LocalCart_Orientation * Cos_LocalCart_Origin_Lon - Sin_Lat_Cos_Orient * Sin_LocalCart_Origin_Lon) * v_MINUS_v0 +
         (Cos_LocalCart_Orientation * cos_lat_w_MINUS_w0);

    *Z = (Cos_LocalCart_Origin_Lat * Cos_LocalCart_Origin_Lon) * u_MINUS_u0 +
         (Cos_LocalCart_Origin_Lat * Sin_LocalCart_Origin_Lon) * v_MINUS_v0 +
         Sin_LocalCart_Origin_Lat * w_MINUS_w0;
  }

} /* END OF Convert_Geocentric_To_Local_Cartesian */

long Convert_Geodetic_To_Local_Cartesian (double Latitude,
                                          double Longitude,
                                          double Height,
                                          double *X,
                                          double *Y,
                                          double *Z)

{ /* BEGIN Convert_Geodetic_TO_Local_Cartesian 
/*
 * The function Convert_Geodetic_To_Local_Cartesian converts geodetic coordinates
 * (latitude, longitude, and height) to local cartesian coordinates (X, Y, Z),
 * according to the current ellipsoid and local origin parameters.
 *
 *    Latitude  : Geodetic latitude, in radians                        (input)
 *    Longitude : Geodetic longitude, in radians                       (input)
 *    Height    : Geodetic height, in meters                           (input)
 *    X         : Calculated local cartesian X coordinate, in meters   (output)
 *    Y         : Calculated local cartesian Y coordinate, in meters   (output)
 *    Z         : Calculated local cartesian Z coordinate, in meters   (output)
 *
 */

  double U, V, W;
  long Error_Code = LOCCART_NO_ERROR;

  if ((Latitude < -PI_OVER_2) || (Latitude > PI_OVER_2))
  { /* geodetic latitude out of range */
    Error_Code |= LOCCART_LAT_ERROR;
  }
  if ((Longitude < -PI) || (Longitude > TWO_PI))
  { /* geodetic longitude out of range */
    Error_Code |= LOCCART_LON_ERROR;
  }

  if (!Error_Code)
  {

    Set_Geocentric_Parameters(LocalCart_a, LocalCart_f);
    Convert_Geodetic_To_Geocentric(Latitude, Longitude, Height, &U, &V, &W);

    Convert_Geocentric_To_Local_Cartesian(U, V, W, X, Y, Z);

  } /* END OF if(!Error_Code) */
  return (Error_Code);
} /* END OF Convert_Geodetic_To_Local_Cartesian */


void Convert_Local_Cartesian_To_Geocentric (double X,
                                            double Y,
                                            double Z,
                                            double *u,
                                            double *v,
                                            double *w)

{ /* BEGIN Convert_Local_Cartesian_To_Geocentric */
/*
 * The function Convert_Local_Cartesian_To_Geocentric converts local cartesian
 * coordinates (x, y, z) to geocentric coordinates (X, Y, Z) according to the 
 * current ellipsoid and local origin parameters.
 *
 *    X         : Local cartesian X coordinate, in meters    (input)
 *    Y         : Local cartesian Y coordinate, in meters    (input)
 *    Z         : Local cartesian Z coordinate, in meters    (input)
 *    u         : Calculated u value, in meters              (output)
 *    v         : Calculated v value, in meters              (output)
 *    w         : Calculated w value, in meters              (output)
 */


  if (LocalCart_Orientation == 0.0)
  {
    double sin_lat_y = Sin_LocalCart_Origin_Lat * Y;
    double cos_lat_z = Cos_LocalCart_Origin_Lat * Z;

    *u = -Sin_LocalCart_Origin_Lon * X - sin_lat_y * Cos_LocalCart_Origin_Lon + cos_lat_z * Cos_LocalCart_Origin_Lon + u0;
    *v = Cos_LocalCart_Origin_Lon * X -  sin_lat_y * Sin_LocalCart_Origin_Lon + cos_lat_z * Sin_LocalCart_Origin_Lon + v0;
    *w = Cos_LocalCart_Origin_Lat * Y + Sin_LocalCart_Origin_Lat * Z + w0;
  }
  else
  {
    double rotated_x, rotated_y;
    double rotated_y_sin_lat, z_cos_lat;

    rotated_x = Cos_LocalCart_Orientation * X + Sin_LocalCart_Orientation * Y;
    rotated_y = -Sin_LocalCart_Orientation * X + Cos_LocalCart_Orientation * Y;

    rotated_y_sin_lat = rotated_y * Sin_LocalCart_Origin_Lat;
    z_cos_lat = Z * Cos_LocalCart_Origin_Lat;

    *u = -Sin_LocalCart_Origin_Lon * rotated_x - Cos_LocalCart_Origin_Lon * rotated_y_sin_lat + Cos_LocalCart_Origin_Lon * z_cos_lat + u0;
    *v = Cos_LocalCart_Origin_Lon * rotated_x -  Sin_LocalCart_Origin_Lon * rotated_y_sin_lat + Sin_LocalCart_Origin_Lon * z_cos_lat + v0;
    *w = Cos_LocalCart_Origin_Lat * rotated_y + Sin_LocalCart_Origin_Lat * Z + w0;
  }
} /* END OF Convert_Local_Cartesian_To_Geocentric */

void Convert_Local_Cartesian_To_Geodetic (double X,
                                          double Y, 
                                          double Z,
                                          double *Latitude,
                                          double *Longitude,
                                          double *Height)

{ /* BEGIN Convert_Local_Cartesian_To_Geodetic */
/*
 * The function Convert_Local_Cartesian_To_Geodetic converts local cartesian
 * coordinates (X, Y, Z) to geodetic coordinates (latitude, longitude, 
 * and height), according to the current ellipsoid and local origin parameters.
 *
 *    X         : Local cartesian X coordinate, in meters    (input)
 *    Y         : Local cartesian Y coordinate, in meters    (input)
 *    Z         : Local cartesian Z coordinate, in meters    (input)
 *    Latitude  : Calculated latitude value, in radians      (output)
 *    Longitude : Calculated longitude value, in radians     (output)
 *    Height    : Calculated height value, in meters         (output)
 */

  double U, V, W;

  Convert_Local_Cartesian_To_Geocentric(X, Y, Z, &U, &V, &W);

  Set_Geocentric_Parameters(LocalCart_a, LocalCart_f);
  Convert_Geocentric_To_Geodetic(U, V, W, Latitude, Longitude, Height);

  if (*Longitude > PI)
    *Longitude -= TWO_PI;
  if (*Longitude < -PI)
    *Longitude += TWO_PI;

} /* END OF Convert_Local_Cartesian_To_Geodetic */
