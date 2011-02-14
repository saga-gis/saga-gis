#ifndef OMERC_H
  #define OMERC_H

/***************************************************************************/
/* RSC IDENTIFIER: OBLIQUE MERCATOR
 *
 * ABSTRACT
 *
 *    This component provides conversions between Geodetic coordinates
 *    (latitude and longitude in radians) and Oblique Mercator
 *    projection coordinates (easting and northing in meters).
 *
 * ERROR HANDLING
 *
 *    This component checks parameters for valid values.  If an invalid value
 *    is found the error code is combined with the current error code using
 *    the bitwise or.  This combining allows multiple error codes to be
 *    returned. The possible error codes are:
 *
 *       OMERC_NO_ERROR           : No errors occurred in function
 *       OMERC_LAT_ERROR          : Latitude outside of valid range
 *                                     (-90 to 90 degrees)
 *       OMERC_LON_ERROR          : Longitude outside of valid range
 *                                     (-180 to 360 degrees)
 *       OMERC_ORIGIN_LAT_ERROR   : Origin latitude outside of valid range
 *                                     (-89 to 89 degrees)
 *       OMERC_LAT1_ERROR         : First latitude outside of valid range
 *                                     (-89 to 89 degrees, excluding 0)
 *       OMERC_LAT2_ERROR         : First latitude outside of valid range
 *                                     (-89 to 89 degrees)
 *       OMERC_LON1_ERROR         : First longitude outside of valid range
 *                                     (-180 to 360 degrees)
 *       OMERC_LON2_ERROR         : Second longitude outside of valid range
 *                                     (-180 to 360 degrees)
 *       OMERC_OLAT_POLE_ERROR    : Origin latitude can not be at a pole
 *       OMERC_LAT1_POLE_ERROR    : First latitude can not be at a pole
 *       OMERC_LAT2_POLE_ERROR    : Second latitude can not be at the south pole
 *       OMERC_LAT1_EQUATOR_ERROR : First latitude can not be 0
 *       OMERC_LAT1_LAT2_ERROR    : First and second latitudes can not be equal
 *       OMERC_DIFF_HEMISPHERE_ERROR: First and second latitudes can not be
 *                                     in different hemispheres
 *       OMERC_EASTING_ERROR      : Easting outside of valid range
 *                                     (depends on ellipsoid and projection
 *                                     parameters)
 *       OMERC_NORTHING_ERROR     : Northing outside of valid range
 *                                     (depends on ellipsoid and projection
 *                                     parameters)
 *       OMERC_A_ERROR            : Semi-major axis less than or equal to zero
 *       OMERC_INV_F_ERROR        : Inverse flattening outside of valid range
 *                                     (250 to 350)
 *       OMERC_SCALE_FACTOR_ERROR : Scale factor outside of valid
 *                                     range (0.3 to 3.0)
 *       OMERC_LON_WARNING        : Distortion will result if longitude is 90 degrees or more
 *                                     from the Central Meridian
 *
 * REUSE NOTES
 *
 *    OBLIQUE MERCATOR is intended for reuse by any application that 
 *    performs an Oblique Mercator projection or its inverse.
 *
 * REFERENCES
 *
 *    Further information on OBLIQUE MERCATOR can be found in the Reuse Manual.
 *
 *    OBLIQUE MERCATOR originated from:     U.S. Army Topographic Engineering Center
 *                                          Geospatial Information Division
 *                                          7701 Telegraph Road
 *                                          Alexandria, VA  22310-3864
 *
 * LICENSES
 *
 *    None apply to this component.
 *
 * RESTRICTIONS
 *
 *    OBLIQUE MERCATOR has no restrictions.
 *
 * ENVIRONMENT
 *
 *    OBLIQUE MERCATOR was tested and certified in the following environments:
 *
 *    1. Solaris 2.5 with GCC, version 2.8.1
 *    2. MSDOS with MS Visual C++, version 6
 *
 * MODIFICATIONS
 *
 *    Date              Description
 *    ----              -----------
 *    05-30-00          Original Code
 *    
 *
 */

/***************************************************************************/
/*
 *                              DEFINES
 */

#define OMERC_NO_ERROR                0x0000
#define OMERC_LAT_ERROR               0x0001
#define OMERC_LON_ERROR               0x0002
#define OMERC_ORIGIN_LAT_ERROR        0x0004
#define OMERC_LAT1_ERROR              0x0008
#define OMERC_LAT2_ERROR              0x0010
#define OMERC_LON1_ERROR              0x0020
#define OMERC_LON2_ERROR              0x0040
#define OMERC_LAT1_LAT2_ERROR         0x0080
#define OMERC_DIFF_HEMISPHERE_ERROR   0x0100
#define OMERC_EASTING_ERROR           0x0200
#define OMERC_NORTHING_ERROR          0x0400
#define OMERC_A_ERROR                 0x0800
#define OMERC_INV_F_ERROR             0x1000
#define OMERC_SCALE_FACTOR_ERROR      0x2000
#define OMERC_LON_WARNING             0x4000
 


/***************************************************************************/
/*
 *                              FUNCTION PROTOTYPES
 *                                for OMERC.C
 */

/* ensure proper linkage to c++ programs */
  #ifdef __cplusplus
extern "C" {
  #endif

  long Set_Oblique_Mercator_Parameters(double a,
                                       double f,
                                       double Origin_Latitude,
                                       double Latitude_1,
                                       double Longitude_1,
                                       double Latitude_2,
                                       double Longitude_2,
                                       double False_Easting,
                                       double False_Northing,
                                       double Scale_Factor);
/*
 * The function Set_Oblique_Mercator_Parameters receives the ellipsoid parameters and
 * projection parameters as inputs, and sets the corresponding state
 * variables.  If any errors occur, the error code(s) are returned by the function, 
 * otherwise OMERC_NO_ERROR is returned.
 *
 *    a                 : Semi-major axis of ellipsoid, in meters   (input)
 *    f                 : Flattening of ellipsoid                   (input)
 *    Origin_Latitude   : Latitude, in radians, at which the        (input)
 *                          point scale factor is 1.0
 *    Latitude_1        : Latitude, in radians, of first point lying on
 *                           central line                           (input)
 *    Longitude_1       : Longitude, in radians, of first point lying on
 *                           central line                           (input)
 *    Latitude_2        : Latitude, in radians, of second point lying on
 *                           central line                           (input)
 *    Longitude_2       : Longitude, in radians, of second point lying on
 *                           central line                           (input)
 *    False_Easting     : A coordinate value, in meters, assigned to the
 *                          central meridian of the projection      (input)
 *    False_Northing    : A coordinate value, in meters, assigned to the
 *                          origin latitude of the projection       (input)
 *    Scale_Factor      : Multiplier which reduces distances in the
 *                          projection to the actual distance on the
 *                          ellipsoid                               (input)
 */


  void Get_Oblique_Mercator_Parameters(double *a,
                                       double *f,
                                       double *Origin_Latitude,
                                       double *Latitude_1,
                                       double *Longitude_1,
                                       double *Latitude_2,
                                       double *Longitude_2,
                                       double *False_Easting,
                                       double *False_Northing,
                                       double *Scale_Factor);
/*
 * The function Get_Oblique_Mercator_Parameters returns the current ellipsoid
 * parameters and Oblique Mercator projection parameters.
 *
 *    a                 : Semi-major axis of ellipsoid, in meters  (output)
 *    f                 : Flattening of ellipsoid                  (output)
 *    Origin_Latitude   : Latitude, in radians, at which the       (output)
 *                          point scale factor is 1.0
 *    Latitude_1        : Latitude, in radians, of first point lying on
 *                          central line                           (output)
 *    Longitude_1       : Longitude, in radians, of first point lying on
 *                          central line                           (output)
 *    Latitude_2        : Latitude, in radians, of second point lying on
 *                          central line                           (output)
 *    Longitude_2       : Longitude, in radians, of second point lying on
 *                          central line                           (output)
 *    False_Easting     : A coordinate value, in meters, assigned to the
 *                          central meridian of the projection     (output)
 *    False_Northing    : A coordinate value, in meters, assigned to the
 *                          origin latitude of the projection      (output)
 *    Scale_Factor      : Multiplier which reduces distances in the
 *                          projection to the actual distance on the
 *                          ellipsoid                              (output)
 */


  long Convert_Geodetic_To_Oblique_Mercator(double Latitude,
                                            double Longitude,
                                            double *Easting,
                                            double *Northing);
/*
 * The function Convert_Geodetic_To_Oblique_Mercator converts geodetic (latitude and
 * longitude) coordinates to Oblique Mercator projection (easting and
 * northing) coordinates, according to the current ellipsoid and Oblique Mercator 
 * projection parameters.  If any errors occur, the error code(s) are returned 
 * by the function, otherwise OMERC_NO_ERROR is returned.
 *
 *    Latitude          : Latitude (phi), in radians           (input)
 *    Longitude         : Longitude (lambda), in radians       (input)
 *    Easting           : Easting (X), in meters               (output)
 *    Northing          : Northing (Y), in meters              (output)
 */


  long Convert_Oblique_Mercator_To_Geodetic(double Easting,
                                            double Northing,
                                            double *Latitude,
                                            double *Longitude);
/*
 * The function Convert_Oblique_Mercator_To_Geodetic converts Oblique Mercator projection
 * (easting and northing) coordinates to geodetic (latitude and longitude)
 * coordinates, according to the current ellipsoid and Oblique Mercator projection
 * coordinates.  If any errors occur, the error code(s) are returned by the
 * function, otherwise OMERC_NO_ERROR is returned.
 *
 *    Easting           : Easting (X), in meters                  (input)
 *    Northing          : Northing (Y), in meters                 (input)
 *    Latitude          : Latitude (phi), in radians              (output)
 *    Longitude         : Longitude (lambda), in radians          (output)
 */

  #ifdef __cplusplus
}
  #endif

#endif

