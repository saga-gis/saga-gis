#ifndef LOCCART_H
  #define LOCCART_H

/***************************************************************************/
/* RSC IDENTIFIER:  LOCAL CARTESIAN
 *
 * ABSTRACT
 *
 *    This component provides conversions between Geodetic coordinates (latitude,
 *    longitude in radians and height in meters) or Geocentric coordinates
 *    (u, v, w) in meters and Local Cartesian coordinates (X, Y, Z).

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
 *		LOCCART_ORIENTATION_ERROR   : Orientation angle out of valid range
 *									    (-360 to 360 degrees)
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
 *
 *
 */


/***************************************************************************/
/*
 *                              DEFINES
 */
  #define LOCCART_NO_ERROR            0x0000
  #define LOCCART_LAT_ERROR           0x0001
  #define LOCCART_LON_ERROR           0x0002
  #define LOCCART_A_ERROR             0x0004
  #define LOCCART_INV_F_ERROR         0x0008
  #define LOCCART_ORIGIN_LAT_ERROR    0x0010
  #define LOCCART_ORIGIN_LON_ERROR    0x0020
  #define LOCCART_ORIENTATION_ERROR   0x0040


/***************************************************************************/
/*
 *                              FUNCTION PROTOTYPES
 *								  for LOCCART.C
 */

/* ensure proper linkage to c++ programs */
  #ifdef __cplusplus
extern "C" {
  #endif


  long Set_Local_Cartesian_Parameters (double a, 
                                       double f,
                                       double Origin_Latitude,
                                       double Origin_Longitude,
                                       double Origin_Height,
                                       double Orientation);
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


  void Get_Local_Cartesian_Parameters (double *a, 
                                       double *f,
                                       double *Origin_Latitude,
                                       double *Origin_Longitude,
                                       double *Origin_Height,
                                       double *Orientation);
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

  void Convert_Geocentric_To_Local_Cartesian (double u, 
                                              double v,
                                              double w,
                                              double *X, 
                                              double *Y,
                                              double *Z);
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

  long Convert_Geodetic_To_Local_Cartesian (double Latitude,
                                            double Longitude,
                                            double Height,
                                            double *X,
                                            double *Y,
                                            double *Z);
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

  void Convert_Local_Cartesian_To_Geocentric (double X,
                                              double Y,
                                              double Z,
                                              double *u,
                                              double *v,
                                              double *w);
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

  void Convert_Local_Cartesian_To_Geodetic (double X,
                                            double Y, 
                                            double Z,
                                            double *Latitude,
                                            double *Longitude,
                                            double *Height);
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


  #ifdef __cplusplus
}
  #endif

#endif /* LOCCART_H */
