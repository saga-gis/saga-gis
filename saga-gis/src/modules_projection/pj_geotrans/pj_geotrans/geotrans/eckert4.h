#ifndef ECKERT4_H
  #define ECKERT4_H

/***************************************************************************/
/* RSC IDENTIFIER: ECKERT4
 *
 * ABSTRACT
 *
 *    This component provides conversions between Geodetic coordinates
 *    (latitude and longitude in radians) and Eckert4 projection coordinates
 *    (easting and northing in meters).
 *
 * ERROR HANDLING
 *
 *    This component checks parameters for valid values.  If an invalid value
 *    is found, the error code is combined with the current error code using
 *    the bitwise or.  This combining allows multiple error codes to be
 *    returned. The possible error codes are:
 *
 *          ECK4_NO_ERROR           : No errors occurred in function
 *          ECK4_LAT_ERROR          : Latitude outside of valid range
 *                                      (-90 to 90 degrees)
 *          ECK4_LON_ERROR          : Longitude outside of valid range
 *                                      (-180 to 360 degrees)
 *          ECK4_EASTING_ERROR      : Easting outside of valid range
 *                                      (False_Easting +/- ~17,000,000 m,
 *										 depending on ellipsoid parameters)
 *          ECK4_NORTHING_ERROR     : Northing outside of valid range
 *                                      (False_Northing +/- 0 to 8,000,000 m,
 *										 depending on ellipsoid parameters)
 *          ECK4_CENT_MER_ERROR     : Central Meridian outside of valid range
 *                                      (-180 to 360 degrees)
 *          ECK4_A_ERROR            : Semi-major axis less than or equal to zero
 *          ECK4_INV_F_ERROR        : Inverse flattening outside of valid range
 *									                    (250 to 350)
 *
 * REUSE NOTES
 *
 *    ECKERT4 is intended for reuse by any application that performs a
 *    Eckert IV projection or its inverse.
 *
 * REFERENCES
 *
 *    Further information on ECKERT4 can be found in the Reuse Manual.
 *
 *    ECKERT4 originated from :  U.S. Army Topographic Engineering Center
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
 *    ECKERT4 has no restrictions.
 *
 * ENVIRONMENT
 *
 *    ECKERT4 was tested and certified in the following environments:
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

  #define ECK4_NO_ERROR           0x0000
  #define ECK4_LAT_ERROR          0x0001
  #define ECK4_LON_ERROR          0x0002
  #define ECK4_EASTING_ERROR      0x0004
  #define ECK4_NORTHING_ERROR     0x0008
  #define ECK4_CENT_MER_ERROR     0x0020
  #define ECK4_A_ERROR            0x0040
  #define ECK4_INV_F_ERROR        0x0080


/***************************************************************************/
/*
 *                              FUNCTION PROTOTYPES
 *                                for ECKERT4.C
 */

/* ensure proper linkage to c++ programs */
  #ifdef __cplusplus
extern "C" {
  #endif

  long Set_Eckert4_Parameters(double a,
                              double f,                           
                              double Central_Meridian,
                              double False_Easting,
                              double False_Northing);
/*
 * The function Set_Eckert4_Parameters receives the ellipsoid parameters and
 * Eckert IV projection parameters as inputs, and sets the corresponding state
 * variables.  If any errors occur, the error code(s) are returned by the 
 * function, otherwise ECK4_NO_ERROR is returned.
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


  void Get_Eckert4_Parameters(double *a,
                              double *f,                            
                              double *Central_Meridian,
                              double *False_Easting,
                              double *False_Northing);
/*
 * The function Get_Eckert4_Parameters returns the current ellipsoid
 * parameters and EckertIV projection parameters.
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


  long Convert_Geodetic_To_Eckert4 (double Latitude,
                                    double Longitude,
                                    double *Easting,
                                    double *Northing);
/*
 * The function Convert_Geodetic_To_Eckert4 converts geodetic (latitude and
 * longitude) coordinates to Eckert IV projection easting, and northing
 * coordinates, according to the current ellipsoid and Eckert IV projection
 * parameters.  If any errors occur, the error code(s) are returned by the
 * function, otherwise ECK4_NO_ERROR is returned.
 *
 *    Latitude          : Latitude (phi) in radians           (input)
 *    Longitude         : Longitude (lambda) in radians       (input)
 *    Easting           : Easting (X) in meters               (output)
 *    Northing          : Northing (Y) in meters              (output)
 */


  long Convert_Eckert4_To_Geodetic(double Easting,
                                   double Northing,
                                   double *Latitude,
                                   double *Longitude);
/*
 * The function Convert_Eckert4_To_Geodetic converts Eckert IV projection
 * easting and northing coordinates to geodetic (latitude and longitude)
 * coordinates, according to the current ellipsoid and Eckert IV projection
 * coordinates.  If any errors occur, the error code(s) are returned by the
 * function, otherwise ECK4_NO_ERROR is returned.
 *
 *    Easting           : Easting (X) in meters                  (input)
 *    Northing          : Northing (Y) in meters                 (input)
 *    Latitude          : Latitude (phi) in radians              (output)
 *    Longitude         : Longitude (lambda) in radians          (output)
 */

  #ifdef __cplusplus
}
  #endif

#endif /* ECKERT4_H */

