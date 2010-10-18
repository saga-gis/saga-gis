#ifndef MOLLWEID_H
  #define MOLLWEID_H

/***************************************************************************/
/* RSC IDENTIFIER: MOLLWEIDE
 *
 * ABSTRACT
 *
 *    This component provides conversions between Geodetic coordinates
 *    (latitude and longitude in radians) and Mollweide projection coordinates
 *    (easting and northing in meters).
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
 *                              DEFINES
 */

  #define MOLL_NO_ERROR           0x0000
  #define MOLL_LAT_ERROR          0x0001
  #define MOLL_LON_ERROR          0x0002
  #define MOLL_EASTING_ERROR      0x0004
  #define MOLL_NORTHING_ERROR     0x0008
  #define MOLL_CENT_MER_ERROR     0x0020
  #define MOLL_A_ERROR            0x0040
  #define MOLL_INV_F_ERROR        0x0080


/***************************************************************************/
/*
 *                              FUNCTION PROTOTYPES
 *                                for MOLLWEID.C
 */

/* ensure proper linkage to c++ programs */
  #ifdef __cplusplus
extern "C" {
  #endif

  long Set_Mollweide_Parameters(double a,
                                double f,
                                double Central_Meridian,
                                double False_Easting,
                                double False_Northing);
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


  void Get_Mollweide_Parameters(double *a,
                                double *f,
                                double *Central_Meridian,
                                double *False_Easting,
                                double *False_Northing);
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


  long Convert_Geodetic_To_Mollweide (double Latitude,
                                      double Longitude,
                                      double *Easting,
                                      double *Northing);
/*
 * The function Convert_Geodetic_To_Mollweide converts geodetic (latitude and
 * longitude) coordinates to Mollweide projection easting, and northing
 * coordinates, according to the current ellipsoid and Mollweide projection
 * parameters.  If any errors occur, the error code(s) are returned by the
 * function, otherwise MOLL_NO_ERROR is returned.
 *
 *    Latitude          : Latitude (phi) in radians           (input)
 *    Longitude         : Longitude (lambda) in radians       (input)
 *    Easting           : Easting (X) in meters               (output)
 *    Northing          : Northing (Y) in meters              (output)
 */


  long Convert_Mollweide_To_Geodetic(double Easting,
                                     double Northing,
                                     double *Latitude,
                                     double *Longitude);
/*
 * The function Convert_Mollweide_To_Geodetic converts Mollweide projection
 * easting and northing coordinates to geodetic (latitude and longitude)
 * coordinates, according to the current ellipsoid and Mollweide projection
 * coordinates.  If any errors occur, the error code(s) are returned by the
 * function, otherwise MOLL_NO_ERROR is returned.
 *
 *    Easting           : Easting (X) in meters                  (input)
 *    Northing          : Northing (Y) in meters                 (input)
 *    Latitude          : Latitude (phi) in radians              (output)
 *    Longitude         : Longitude (lambda) in radians          (output)
 */

  #ifdef __cplusplus
}
  #endif

#endif /* MOLLWEID_H */

