#ifndef ALBERS_H
  #define ALBERS_H

/***************************************************************************/
/* RSC IDENTIFIER: ALBERS
 *
 * ABSTRACT
 *
 *    This component provides conversions between Geodetic coordinates 
 *    (latitude and longitude in radians) and Albers Equal Area Conic
 *    projection coordinates (easting and northing in meters) defined 
 *    by two standard parallels.
 *
 * ERROR HANDLING
 *
 *    This component checks parameters for valid values.  If an invalid value
 *    is found the error code is combined with the current error code using 
 *    the bitwise or.  This combining allows multiple error codes to be
 *    returned. The possible error codes are:
 *
 *       ALBERS_NO_ERROR           : No errors occurred in function
 *       ALBERS_LAT_ERROR          : Latitude outside of valid range
 *                                     (-90 to 90 degrees)
 *       ALBERS_LON_ERROR          : Longitude outside of valid range
 *                                     (-180 to 360 degrees)
 *       ALBERS_EASTING_ERROR      : Easting outside of valid range
 *                                     (depends on ellipsoid and projection
 *                                     parameters)
 *       ALBERS_NORTHING_ERROR     : Northing outside of valid range
 *                                     (depends on ellipsoid and projection
 *                                     parameters)
 *       ALBERS_FIRST_STDP_ERROR   : First standard parallel outside of valid
 *                                     range (-90 to 90 degrees)
 *       ALBERS_SECOND_STDP_ERROR  : Second standard parallel outside of valid
 *                                     range (-90 to 90 degrees)
 *       ALBERS_ORIGIN_LAT_ERROR   : Origin latitude outside of valid range
 *                                     (-90 to 90 degrees)
 *       ALBERS_CENT_MER_ERROR     : Central meridian outside of valid range
 *                                     (-180 to 360 degrees)
 *       ALBERS_A_ERROR            : Semi-major axis less than or equal to zero
 *       ALBERS_INV_F_ERROR        : Inverse flattening outside of valid range
 *									                   (250 to 350)
 *       ALBERS_HEMISPHERE_ERROR   : Standard parallels cannot be opposite
 *                                     latitudes
 *       ALBERS_FIRST_SECOND_ERROR : The 1st & 2nd standard parallels cannot
 *                                   both be 0
 *
 *
 * REUSE NOTES
 *
 *    ALBERS is intended for reuse by any application that performs an Albers
 *    Equal Area Conic projection or its inverse.
 *    
 * REFERENCES
 *
 *    Further information on ALBERS can be found in the Reuse Manual.
 *
 *    ALBERS originated from:     U.S. Army Topographic Engineering Center
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
 *    ALBERS has no restrictions.
 *
 * ENVIRONMENT
 *
 *    ALBERS was tested and certified in the following environments:
 *
 *    1. Solaris 2.5 with GCC, version 2.8.1
 *    2. MSDOS with MS Visual C++, version 6
 *
 * MODIFICATIONS
 *
 *    Date              Description
 *    ----              -----------
 *    07-09-99          Original Code
 *
 */



/***************************************************************************/
/*
 *                              DEFINES
 */

  #define ALBERS_NO_ERROR           0x0000
  #define ALBERS_LAT_ERROR          0x0001
  #define ALBERS_LON_ERROR          0x0002
  #define ALBERS_EASTING_ERROR      0x0004
  #define ALBERS_NORTHING_ERROR     0x0008
  #define ALBERS_ORIGIN_LAT_ERROR   0x0010
  #define ALBERS_CENT_MER_ERROR     0x0020
  #define ALBERS_A_ERROR            0x0040
  #define ALBERS_INV_F_ERROR        0x0080
  #define ALBERS_FIRST_STDP_ERROR   0x0100
  #define ALBERS_SECOND_STDP_ERROR  0x0200
  #define ALBERS_FIRST_SECOND_ERROR 0x0400
  #define ALBERS_HEMISPHERE_ERROR   0x0800

/***************************************************************************/
/*
 *                              FUNCTION PROTOTYPES
 *                                for ALBERS.C
 */

/* ensure proper linkage to c++ programs */


  #ifdef __cplusplus
extern "C" {
  #endif



  long Set_Albers_Parameters(double a,
                             double f,
                             double Origin_Latitude,
                             double Central_Meridian,
                             double Std_Parallel_1,
                             double Std_Parallel_2,
                             double False_Easting,
                             double False_Northing);

/* The function Set_Albers_Parameters receives the ellipsoid parameters and
 * projection parameters as inputs, and sets the corresponding state
 * variables.  If any errors occur, the error code(s) are returned by the function, 
 * otherwise ALBERS_NO_ERROR is returned.
 *
 *    a                 : Semi-major axis of ellipsoid, in meters   (input)
 *    f                 : Flattening of ellipsoid						        (input)
 *    Origin_Latitude   : Latitude in radians at which the          (input)
 *                          point scale factor is 1.0
 *    Central_Meridian  : Longitude in radians at the center of     (input)
 *                          the projection (central meridian)
 *    Std_Parallel_1    : First standard parallel                   (input)
 *    Std_Parallel_2    : Second standard parallel                  (input)
 *    False_Easting     : A coordinate value in meters assigned to the
 *                          central meridian of the projection.     (input)
 *    False_Northing    : A coordinate value in meters assigned to the
 *                          origin latitude of the projection       (input)
 */


  void Get_Albers_Parameters(double *a,
                             double *f,
                             double *Origin_Latitude,
                             double *Central_Meridian,
                             double *Std_Parallel_1,
                             double *Std_Parallel_2,
                             double *False_Easting,
                             double *False_Northing);

/* The function Get_Albers_Parameters returns the current ellipsoid
 * parameters, and Albers projection parameters.
 *
 *    a                 : Semi-major axis of ellipsoid, in meters   (output)
 *    f                 : Flattening of ellipsoid						        (output)
 *    Origin_Latitude   : Latitude in radians at which the          (output)
 *                          point scale factor is 1.0
 *    Central_Meridian  : Longitude in radians at the center of     (output)
 *                          the projection
 *    Std_Parallel_1    : First standard parallel                   (output)
 *    Std_Parallel_2    : Second standard parallel                  (output)
 *    False_Easting     : A coordinate value in meters assigned to the
 *                          central meridian of the projection.     (output)
 *    False_Northing    : A coordinate value in meters assigned to the
 *                          origin latitude of the projection       (output)
 */


  long Convert_Geodetic_To_Albers (double Latitude,
                                   double Longitude,
                                   double *Easting,
                                   double *Northing);

/* The function Convert_Geodetic_To_Albers converts geodetic (latitude and
 * longitude) coordinates to Albers projection (easting and northing)
 * coordinates, according to the current ellipsoid and Albers projection
 * parameters.  If any errors occur, the error code(s) are returned by the
 * function, otherwise ALBERS_NO_ERROR is returned.
 *
 *    Latitude          : Latitude (phi) in radians           (input)
 *    Longitude         : Longitude (lambda) in radians       (input)
 *    Easting           : Easting (X) in meters               (output)
 *    Northing          : Northing (Y) in meters              (output)
 */


  long Convert_Albers_To_Geodetic(double Easting,
                                  double Northing,
                                  double *Latitude,
                                  double *Longitude);

/* The function Convert_Albers_To_Geodetic converts Albers projection
 * (easting and northing) coordinates to geodetic (latitude and longitude)
 * coordinates, according to the current ellipsoid and Albers projection
 * coordinates.  If any errors occur, the error code(s) are returned by the
 * function, otherwise ALBERS_NO_ERROR is returned.
 *
 *    Easting           : Easting (X) in meters                  (input)
 *    Northing          : Northing (Y) in meters                 (input)
 *    Latitude          : Latitude (phi) in radians              (output)
 *    Longitude         : Longitude (lambda) in radians          (output)
 */


  #ifdef __cplusplus
}
  #endif

#endif
