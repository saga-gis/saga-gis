#ifndef ECKERT6_H
  #define ECKERT6_H

/***************************************************************************/
/* RSC IDENTIFIER: ECKERT6
 *
 * ABSTRACT
 *
 *    This component provides conversions between Geodetic coordinates
 *    (latitude and longitude in radians) and Eckert VI projection coordinates
 *    (easting and northing in meters).
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
 *									                    (250 to 350s)
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
 *                              DEFINES
 */

  #define ECK6_NO_ERROR           0x0000
  #define ECK6_LAT_ERROR          0x0001
  #define ECK6_LON_ERROR          0x0002
  #define ECK6_EASTING_ERROR      0x0004
  #define ECK6_NORTHING_ERROR     0x0008
  #define ECK6_CENT_MER_ERROR     0x0020
  #define ECK6_A_ERROR            0x0040
  #define ECK6_INV_F_ERROR        0x0080


/***************************************************************************/
/*
 *                              FUNCTION PROTOTYPES
 *                                for ECKERT6.C
 */

/* ensure proper linkage to c++ programs */
  #ifdef __cplusplus
extern "C" {
  #endif

  long Set_Eckert6_Parameters(double a,
                              double f,
                              double Central_Meridian,
                              double False_Easting,
                              double False_Northing);
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


  void Get_Eckert6_Parameters(double *a,
                              double *f,
                              double *Central_Meridian,
                              double *False_Easting,
                              double *False_Northing);
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


  long Convert_Geodetic_To_Eckert6 (double Latitude,
                                    double Longitude,
                                    double *Easting,
                                    double *Northing);
/*
 * The function Convert_Geodetic_To_Eckert6 converts geodetic (latitude and
 * longitude) coordinates to Eckert VI projection easting, and northing
 * coordinates, according to the current ellipsoid and Eckert VI projection
 * parameters.  If any errors occur, the error code(s) are returned by the
 * function, otherwise ECK6_NO_ERROR is returned.
 *
 *    Latitude          : Latitude (phi) in radians           (input)
 *    Longitude         : Longitude (lambda) in radians       (input)
 *    Easting           : Easting (X) in meters               (output)
 *    Northing          : Northing (Y) in meters              (output)
 */


  long Convert_Eckert6_To_Geodetic(double Easting,
                                   double Northing,
                                   double *Latitude,
                                   double *Longitude);
/*
 * The function Convert_Eckert6_To_Geodetic converts Eckert VI projection
 * easting and northing coordinates to geodetic (latitude and longitude)
 * coordinates, according to the current ellipsoid and Eckert VI projection
 * coordinates.  If any errors occur, the error code(s) are returned by the
 * function, otherwise ECK6_NO_ERROR is returned.
 *
 *    Easting           : Easting (X) in meters                  (input)
 *    Northing          : Northing (Y) in meters                 (input)
 *    Latitude          : Latitude (phi) in radians              (output)
 *    Longitude         : Longitude (lambda) in radians          (output)
 */

  #ifdef __cplusplus
}
  #endif

#endif /* ECKERT6_H */

