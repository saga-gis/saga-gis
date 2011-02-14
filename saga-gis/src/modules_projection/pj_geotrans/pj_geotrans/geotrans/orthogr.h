#ifndef ORTHOGR_H
  #define ORTHOGR_H

/***************************************************************************/
/* RSC IDENTIFIER: ORTHOGRAPHIC
 *
 * ABSTRACT
 *
 *    This component provides conversions between Geodetic coordinates
 *    (latitude and longitude in radians) and Orthographic projection 
 *    coordinates (easting and northing in meters).  The Orthographic
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
 *          ORTH_NO_ERROR           : No errors occurred in function
 *          ORTH_LAT_ERROR          : Latitude outside of valid range
 *                                      (-90 to 90 degrees)
 *          ORTH_LON_ERROR          : Longitude outside of valid range
 *                                      (-180 to 360 degrees)
 *          ORTH_EASTING_ERROR      : Easting outside of valid range
 *                                      (False_Easting +/- ~6,500,000 m,
 *                                       depending on ellipsoid parameters)
 *          ORTH_NORTHING_ERROR     : Northing outside of valid range
 *                                      (False_Northing +/- ~6,500,000 m,
 *                                       depending on ellipsoid parameters)
 *          ORTH_RADIUS_ERROR       : Coordinates too far from pole,
 *                                      depending on ellipsoid and
 *                                      projection parameters
 *          ORTH_ORIGIN_LAT_ERROR   : Origin latitude outside of valid range
 *                                      (-90 to 90 degrees)
 *          ORTH_CENT_MER_ERROR     : Central meridian outside of valid range
 *                                      (-180 to 360 degrees)
 *          ORTH_A_ERROR            : Semi-major axis less than or equal to zero
 *          ORTH_INV_F_ERROR        : Inverse flattening outside of valid range
 *								  	                  (250 to 350)
 *
 * REUSE NOTES
 *
 *    ORTHOGRAPHIC is intended for reuse by any application that performs a
 *    Orthographic projection or its inverse.
 *
 * REFERENCES
 *
 *    Further information on ORTHOGRAPHIC can be found in the Reuse Manual.
 *
 *    ORTHOGRAPHIC originated from :  U.S. Army Topographic Engineering Center
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
 *    ORTHOGRAPHIC has no restrictions.
 *
 * ENVIRONMENT
 *
 *    ORTHOGRAPHIC was tested and certified in the following environments:
 *
 *    1. Solaris 2.5 with GCC, version 2.8.1
 *    2. Windows 95 with MS Visual C++, version 6
 *
 * MODIFICATIONS
 *
 *    Date              Description
 *    ----              -----------
 *    15-06-99          Original Code
 *
 *
 */


/***************************************************************************/
/*
 *                              DEFINES
 */

  #define ORTH_NO_ERROR           0x0000
  #define ORTH_LAT_ERROR          0x0001
  #define ORTH_LON_ERROR          0x0002
  #define ORTH_EASTING_ERROR      0x0004
  #define ORTH_NORTHING_ERROR     0x0008
  #define ORTH_ORIGIN_LAT_ERROR   0x0010
  #define ORTH_CENT_MER_ERROR     0x0020
  #define ORTH_A_ERROR            0x0040
  #define ORTH_INV_F_ERROR        0x0080
  #define ORTH_RADIUS_ERROR	    0x0100


/***************************************************************************/
/*
 *                              FUNCTION PROTOTYPES
 *                                for ORTHOGR.C
 */

/* ensure proper linkage to c++ programs */
  #ifdef __cplusplus
extern "C" {
  #endif

  long Set_Orthographic_Parameters(double a,
                                   double f,
                                   double Origin_Latitude,
                                   double Central_Meridian,
                                   double False_Easting,
                                   double False_Northing);

/*
 * The function Set_Orthographic_Parameters receives the ellipsoid parameters and
 * Orthographic projcetion parameters as inputs, and sets the corresponding state
 * variables.  If any errors occur, the error code(s) are returned by the function, 
 * otherwise ORTH_NO_ERROR is returned.
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


  void Get_Orthographic_Parameters(double *a,
                                   double *f,
                                   double *Origin_Latitude,
                                   double *Origin_Longitude,
                                   double *False_Easting,
                                   double *False_Northing);

/*
 * The function Get_Orthographic_Parameters returns the current ellipsoid
 * parameters and Orthographic projection parameters.
 *
 *    a                 : Semi-major axis of ellipsoid, in meters   (output)
 *    f                 : Flattening of ellipsoid                   (output)
 *    Origin_Latitude   : Latitude in radians at which the          (output)
 *                          point scale factor is 1.0
 *    Origin_Longitude  : Longitude in radians at the center of     (output)
 *                          the projection (central meridian)
 *    False_Easting     : A coordinate value in meters assigned to the
 *                          central meridian of the projection.     (output)
 *    False_Northing    : A coordinate value in meters assigned to the
 *                          origin latitude of the projection       (output) 
 */


  long Convert_Geodetic_To_Orthographic (double Latitude,
                                         double Longitude,
                                         double *Easting,
                                         double *Northing);
/*
 * The function Convert_Geodetic_To_Orthographic converts geodetic (latitude and
 * longitude) coordinates to Orthographic projection easting, and northing
 * coordinates, according to the current ellipsoid and Orthographic projection
 * parameters.  If any errors occur, the error code(s) are returned by the
 * function, otherwise ORTH_NO_ERROR is returned.
 *
 *    Latitude          : Latitude (phi) in radians           (input)
 *    Longitude         : Longitude (lambda) in radians       (input)
 *    Easting           : Easting (X) in meters               (output)
 *    Northing          : Northing (Y) in meters              (output)
 */


  long Convert_Orthographic_To_Geodetic(double Easting,
                                        double Northing,
                                        double *Latitude,
                                        double *Longitude);
/*
 * The function Convert_Orthographic_To_Geodetic converts Orthographic projection
 * easting and northing coordinates to geodetic (latitude and longitude)
 * coordinates, according to the current ellipsoid and Orthographic projection
 * coordinates.  If any errors occur, the error code(s) are returned by the
 * function, otherwise ORTH_NO_ERROR is returned.
 *
 *    Easting           : Easting (X) in meters                  (input)
 *    Northing          : Northing (Y) in meters                 (input)
 *    Latitude          : Latitude (phi) in radians              (output)
 *    Longitude         : Longitude (lambda) in radians          (output)
 */

  #ifdef __cplusplus
}
  #endif

#endif /* ORTHOGR_H */

