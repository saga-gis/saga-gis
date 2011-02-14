#ifndef GNOM_H
  #define GNOM_H

/***************************************************************************/
/* RSC IDENTIFIER: GNOMONIC
 *
 * ABSTRACT
 *
 *    This component provides conversions between Geodetic coordinates
 *    (latitude and longitude in radians) and Gnomonic
 *    projection coordinates (easting and northing in meters).  This projection 
 *    employs a spherical Earth model.  The spherical radius used is the radius 
 *    of the sphere having the same area as the ellipsoid.
 *
 * ERROR HANDLING
 *
 *    This component checks parameters for valid values.  If an invalid value
 *    is found the error code is combined with the current error code using
 *    the bitwise or.  This combining allows multiple error codes to be
 *    returned. The possible error codes are:
 *
 *       GNOM_NO_ERROR           : No errors occurred in function
 *       GNOM_LAT_ERROR          : Latitude outside of valid range
 *                                     (-90 to 90 degrees)
 *       GNOM_LON_ERROR          : Longitude outside of valid range
 *                                     (-180 to 360 degrees)
 *       GNOM_EASTING_ERROR      : Easting outside of valid range
 *                                     (depends on ellipsoid and projection
 *                                     parameters)
 *       GNOM_NORTHING_ERROR     : Northing outside of valid range
 *                                     (depends on ellipsoid and projection
 *                                     parameters)
 *       GNOM_ORIGIN_LAT_ERROR   : Origin latitude outside of valid range
 *                                     (-90 to 90 degrees)
 *       GNOM_CENT_MER_ERROR     : Central meridian outside of valid range
 *                                     (-180 to 360 degrees)
 *       GNOM_A_ERROR            : Semi-major axis less than or equal to zero
 *       GNOM_INV_F_ERROR        : Inverse flattening outside of valid range
 *									                   (250 to 350)
 *
 *
 * REUSE NOTES
 *
 *    GNOMONIC is intended for reuse by any application that 
 *    performs a Gnomonic projection or its inverse.
 *
 * REFERENCES
 *
 *    Further information on GNOMONIC can be found in the Reuse Manual.
 *
 *    GNOMONIC originated from:     U.S. Army Topographic Engineering Center
 *                                  Geospatial Information Division
 *                                  7701 Telegraph Road
 *                                  Alexandria, VA  22310-3864
 *
 * LICENSES
 *
 *    None apply to this component.
 *
 * RESTRICTIONS
 *
 *    GNOMONIC has no restrictions.
 *
 * ENVIRONMENT
 *
 *    GNOMONIC was tested and certified in the following environments:
 *
 *    1. Solaris 2.5 with GCC, version 2.8.1
 *    2. MSDOS with MS Visual C++, version 6
 *
 * MODIFICATIONS
 *
 *    Date              Description
 *    ----              -----------
 *    05-22-00          Original Code
 *    
 *
 */

/***************************************************************************/
/*
 *                              DEFINES
 */

  #define GNOM_NO_ERROR           0x0000
  #define GNOM_LAT_ERROR          0x0001
  #define GNOM_LON_ERROR          0x0002
  #define GNOM_EASTING_ERROR      0x0004
  #define GNOM_NORTHING_ERROR     0x0008
  #define GNOM_ORIGIN_LAT_ERROR   0x0010
  #define GNOM_CENT_MER_ERROR     0x0020
  #define GNOM_A_ERROR            0x0040
  #define GNOM_INV_F_ERROR        0x0080


/***************************************************************************/
/*
 *                              FUNCTION PROTOTYPES
 *                                for GNOM.C
 */

/* ensure proper linkage to c++ programs */
  #ifdef __cplusplus
extern "C" {
  #endif


  long Set_Gnomonic_Parameters(double a,
                               double f,
                               double Origin_Latitude,
                               double Central_Meridian,
                               double False_Easting,
                               double False_Northing);
/*
 * The function Set_Gnomonic_Parameters receives the ellipsoid parameters and
 * projection parameters as inputs, and sets the corresponding state
 * variables.  If any errors occur, the error code(s) are returned by the function, 
 * otherwise GNOM_NO_ERROR is returned.
 *
 *    a                 : Semi-major axis of ellipsoid, in meters   (input)
 *    f                 : Flattening of ellipsoid						        (input)
 *    Origin_Latitude   : Latitude in radians at which the          (input)
 *                          point scale factor is 1.0
 *    Central_Meridian  : Longitude in radians at the center of     (input)
 *                          the projection
 *    False_Easting     : A coordinate value in meters assigned to the
 *                          central meridian of the projection.     (input)
 *    False_Northing    : A coordinate value in meters assigned to the
 *                          origin latitude of the projection       (input)
 */

  void Get_Gnomonic_Parameters(double *a,
                               double *f,
                               double *Origin_Latitude,
                               double *Central_Meridian,
                               double *False_Easting,
                               double *False_Northing);
/*
 * The function Get_Gnomonic_Parameters returns the current ellipsoid
 * parameters and Gnomonic projection parameters.
 *
 *    a                 : Semi-major axis of ellipsoid, in meters   (output)
 *    f                 : Flattening of ellipsoid						        (output)
 *    Origin_Latitude   : Latitude in radians at which the          (output)
 *                          point scale factor is 1.0
 *    Central_Meridian  : Longitude in radians at the center of     (output)
 *                          the projection
 *    False_Easting     : A coordinate value in meters assigned to the
 *                          central meridian of the projection.     (output)
 *    False_Northing    : A coordinate value in meters assigned to the
 *                          origin latitude of the projection       (output)
 */


  long Convert_Geodetic_To_Gnomonic (double Latitude,
                                     double Longitude,
                                     double *Easting,
                                     double *Northing);
/*
 * The function Convert_Geodetic_To_Gnomonic converts geodetic (latitude and
 * longitude) coordinates to Gnomonic projection (easting and northing)
 * coordinates, according to the current ellipsoid and Gnomonic projection
 * parameters.  If any errors occur, the error code(s) are returned by the
 * function, otherwise GNOM_NO_ERROR is returned.
 *
 *    Latitude          : Latitude (phi) in radians           (input)
 *    Longitude         : Longitude (lambda) in radians       (input)
 *    Easting           : Easting (X) in meters               (output)
 *    Northing          : Northing (Y) in meters              (output)
 */


  long Convert_Gnomonic_To_Geodetic(double Easting,
                                    double Northing,
                                    double *Latitude,
                                    double *Longitude);
/*
 * The function Convert_Gnomonic_To_Geodetic converts Gnomonic projection
 * (easting and northing) coordinates to geodetic (latitude and longitude)
 * coordinates, according to the current ellipsoid and Gnomonic projection
 * coordinates.  If any errors occur, the error code(s) are returned by the
 * function, otherwise GNOM_NO_ERROR is returned.
 *
 *    Easting           : Easting (X) in meters                  (input)
 *    Northing          : Northing (Y) in meters                 (input)
 *    Latitude          : Latitude (phi) in radians              (output)
 *    Longitude         : Longitude (lambda) in radians          (output)
 */

  #ifdef __cplusplus
}
  #endif

#endif /* GNOM_H */
