
#ifndef EQDCYL_H
  #define EQDCYL_H

/***************************************************************************/
/* RSC IDENTIFIER: EQUIDISTANT CYLINDRICAL
 *
 * ABSTRACT
 *
 *    This component provides conversions between Geodetic coordinates
 *    (latitude and longitude in radians) and Equidistant Cylindrical projection coordinates
 *    (easting and northing in meters).
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
 *    EQDCYL was tested and certified in the following environments:
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
 */


/***************************************************************************/
/*
 *                              DEFINES
 */

  #define EQCY_NO_ERROR           0x0000
  #define EQCY_LAT_ERROR          0x0001
  #define EQCY_LON_ERROR          0x0002
  #define EQCY_EASTING_ERROR      0x0004
  #define EQCY_NORTHING_ERROR     0x0008
  #define EQCY_STDP_ERROR         0x0010
  #define EQCY_CENT_MER_ERROR     0x0020
  #define EQCY_A_ERROR            0x0040
  #define EQCY_INV_F_ERROR        0x0080


/***************************************************************************/
/*
 *                              FUNCTION PROTOTYPES
 *                                for EQDCYL.C
 */

/* ensure proper linkage to c++ programs */
  #ifdef __cplusplus
extern "C" {
  #endif

  long Set_Equidistant_Cyl_Parameters(double a,
                                      double f,
                                      double Std_Parallel,
                                      double Central_Meridian,
                                      double False_Easting,
                                      double False_Northing);
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


  void Get_Equidistant_Cyl_Parameters(double *a,
                                      double *f,
                                      double *Std_Parallel,
                                      double *Central_Meridian,
                                      double *False_Easting,
                                      double *False_Northing);
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


  long Convert_Geodetic_To_Equidistant_Cyl (double Latitude,
                                            double Longitude,
                                            double *Easting,
                                            double *Northing);
/*
 * The function Convert_Geodetic_To_Equidistant_Cyl converts geodetic (latitude and
 * longitude) coordinates to Equidistant Cylindrical projection easting, and northing
 * coordinates, according to the current ellipsoid and Equidistant Cylindrical projection
 * parameters.  If any errors occur, the error code(s) are returned by the
 * function, otherwise EQCY_NO_ERROR is returned.
 *
 *    Latitude          : Latitude (phi) in radians           (input)
 *    Longitude         : Longitude (lambda) in radians       (input)
 *    Easting           : Easting (X) in meters               (output)
 *    Northing          : Northing (Y) in meters              (output)
 */


  long Convert_Equidistant_Cyl_To_Geodetic(double Easting,
                                           double Northing,
                                           double *Latitude,
                                           double *Longitude);
/*
 * The function Convert_Equidistant_Cyl_To_Geodetic converts Equidistant Cylindrical projection
 * easting and northing coordinates to geodetic (latitude and longitude)
 * coordinates, according to the current ellipsoid and Equidistant Cylindrical projection
 * coordinates.  If any errors occur, the error code(s) are returned by the
 * function, otherwise EQCY_NO_ERROR is returned.
 *
 *    Easting           : Easting (X) in meters                  (input)
 *    Northing          : Northing (Y) in meters                 (input)
 *    Latitude          : Latitude (phi) in radians              (output)
 *    Longitude         : Longitude (lambda) in radians          (output)
 */

  #ifdef __cplusplus
}
  #endif

#endif /* EQDCYL_H */

