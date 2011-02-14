#ifndef POLYCON_H
  #define POLYCON_H

/***************************************************************************/
/* RSC IDENTIFIER: POLYCONIC
 *
 * ABSTRACT
 *
 *    This component provides conversions between Geodetic coordinates 
 *    (latitude and longitude in radians) and Polyconic projection coordinates
 *    (easting and northing in meters).
 *
 * ERROR HANDLING
 *
 *    This component checks parameters for valid values.  If an invalid value
 *    is found, the error code is combined with the current error code using 
 *    the bitwise or.  This combining allows multiple error codes to be
 *    returned. The possible error codes are:
 *
 *          POLY_NO_ERROR           : No errors occurred in function
 *          POLY_LAT_ERROR          : Latitude outside of valid range
 *                                      (-90 to 90 degrees)
 *          POLY_LON_ERROR          : Longitude outside of valid range
 *                                      (-180 to 360 degrees)
 *          POLY_EASTING_ERROR      : Easting outside of valid range
 *                                      (False_Easting +/- ~20,500,000 m,
 *                                       depending on ellipsoid parameters
 *                                       and Origin_Latitude)
 *          POLY_NORTHING_ERROR     : Northing outside of valid range
 *                                      (False_Northing +/- ~15,500,000 m,
 *                                       depending on ellipsoid parameters
 *                                       and Origin_Latitude)
 *          POLY_ORIGIN_LAT_ERROR   : Origin latitude outside of valid range
 *                                      (-90 to 90 degrees)
 *          POLY_CENT_MER_ERROR     : Central meridian outside of valid range
 *                                      (-180 to 360 degrees)
 *          POLY_A_ERROR            : Semi-major axis less than or equal to zero
 *          POLY_INV_F_ERROR        : Inverse flattening outside of valid range
 *								  	                  (250 to 350)
 *		    POLY_LON_WARNING        : Distortion will result if longitude is more
 *                                     than 90 degrees from the Central Meridian
 *
 * REUSE NOTES
 *
 *    POLYCONIC is intended for reuse by any application that performs a
 *    Polyconic projection or its inverse.
 *    
 * REFERENCES
 *
 *    Further information on POLYCONIC can be found in the Reuse Manual.
 *
 *    POLYCONIC originated from :  U.S. Army Topographic Engineering Center
 *                                 Geospatial Information Division
 *                                 7701 Telegraph Road
 *                                 Alexandria, VA  22310-3864
 *
 * LICENSES
 *
 *    None apply to this component.
 *
 * RESTRICTIONS
 *
 *    POLYCONIC has no restrictions.
 *
 * ENVIRONMENT
 *
 *    POLYCONIC was tested and certified in the following environments:
 *
 *    1. Solaris 2.5 with GCC, version 2.8.1
 *    2. Windows 95 with MS Visual C++, version 6
 *
 * MODIFICATIONS
 *
 *    Date              Description
 *    ----              -----------
 *    10-06-99          Original Code
 *
 */


/***************************************************************************/
/*
 *                              DEFINES
 */

  #define POLY_NO_ERROR           0x0000
  #define POLY_LAT_ERROR          0x0001
  #define POLY_LON_ERROR          0x0002
  #define POLY_EASTING_ERROR      0x0004
  #define POLY_NORTHING_ERROR     0x0008
  #define POLY_ORIGIN_LAT_ERROR   0x0010
  #define POLY_CENT_MER_ERROR     0x0020
  #define POLY_A_ERROR            0x0040
  #define POLY_INV_F_ERROR        0x0080
  #define POLY_LON_WARNING        0x0100


/***************************************************************************/
/*
 *                              FUNCTION PROTOTYPES
 *                                for POLYCON.C
 */

/* ensure proper linkage to c++ programs */
  #ifdef __cplusplus
extern "C" {
  #endif

  long Set_Polyconic_Parameters(double a,
                                double f,
                                double Origin_Latitude,
                                double Central_Meridian,
                                double False_Easting,
                                double False_Northing);

/*
 * The function Set_Polyconic_Parameters receives the ellipsoid parameters and
 * Polyconic projcetion parameters as inputs, and sets the corresponding state
 * variables.If any errors
 * occur, the error code(s) are returned by the function, otherwise 
 * POLY_NO_ERROR is returned.
 *
 *    a                 : Semi-major axis of ellipsoid, in meters   (input)
 *    f                 : Flattening of ellipsoid                   (input)
 *    Origin_Latitude   : Latitude in radians at which the          (input)
 *                          point scale factor is 1.0
 *    Central_Meridian  : Longitude in radians at the center of     (input)
 *                          the projection
 *    False_Easting     : A coordinate value in meters assigned to the
 *                          central meridian of the projection.     (input)
 *    False_Northing    : A coordinate value in meters assigned to the
 *                          origin latitude of the projection       (input)
 */


  void Get_Polyconic_Parameters(double *a,
                                double *f,
                                double *Origin_Latitude,
                                double *Central_Meridian,
                                double *False_Easting,
                                double *False_Northing);

/*
 * The function Get_Polyconic_Parameters returns the current ellipsoid
 * parameters, and Polyconic projection parameters.
 *
 *    a                 : Semi-major axis of ellipsoid, in meters   (output)
 *    f                 : Flattening of ellipsoid                   (output)
 *    Origin_Latitude   : Latitude in radians at which the          (output)
 *                          point scale factor is 1.0
 *    Central_Meridian  : Longitude in radians at the center of     (output)
 *                          the projection
 *    False_Easting     : A coordinate value in meters assigned to the
 *                          central meridian of the projection.     (output)
 *    False_Northing    : A coordinate value in meters assigned to the
 *                          origin latitude of the projection       (output)
 */


  long Convert_Geodetic_To_Polyconic (double Latitude,
                                      double Longitude,
                                      double *Easting,
                                      double *Northing); 
/*
 * The function Convert_Geodetic_To_Polyconic converts geodetic (latitude and
 * longitude) coordinates to Polyconic projection easting, and northing
 * coordinates, according to the current ellipsoid and Polyconic projection
 * parameters.  If any errors occur, the error code(s) are returned by the
 * function, otherwise POLY_NO_ERROR is returned.
 *
 *    Latitude          : Latitude (phi) in radians           (input)
 *    Longitude         : Longitude (lambda) in radians       (input)
 *    Easting           : Easting (X) in meters               (output)
 *    Northing          : Northing (Y) in meters              (output)
 */


  long Convert_Polyconic_To_Geodetic(double Easting,
                                     double Northing,
                                     double *Latitude,
                                     double *Longitude);
/*
 * The function Convert_Polyconic_To_Geodetic converts Polyconic projection
 * easting and northing coordinates to geodetic (latitude and longitude)
 * coordinates, according to the current ellipsoid and Polyconic projection
 * coordinates.  If any errors occur, the error code(s) are returned by the
 * function, otherwise POLY_NO_ERROR is returned.
 *
 *    Easting           : Easting (X) in meters                  (input)
 *    Northing          : Northing (Y) in meters                 (input)
 *    Latitude          : Latitude (phi) in radians              (output)
 *    Longitude         : Longitude (lambda) in radians          (output)
 */

  #ifdef __cplusplus
}
  #endif

#endif /* POLYCON_H */

