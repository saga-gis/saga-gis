#ifndef GRINTEN_H
  #define GRINTEN_H

/***************************************************************************/
/* RSC IDENTIFIER: VAN DER GRINTEN
 *
 * ABSTRACT
 *
 *    This component provides conversions between Geodetic coordinates
 *    (latitude and longitude in radians) and Van Der Grinten projection coordinates
 *    (easting and northing in meters).
 *
 * ERROR HANDLING
 *
 *    This component checks parameters for valid values.  If an invalid value
 *    is found, the error code is combined with the current error code using
 *    the bitwise or.  This combining allows multiple error codes to be
 *    returned. The possible error codes are:
 *
 *          GRIN_NO_ERROR           : No errors occurred in function
 *          GRIN_LAT_ERROR          : Latitude outside of valid range
 *                                      (-90 to 90 degrees)
 *          GRIN_LON_ERROR          : Longitude outside of valid range
 *                                      (-180 to 360 degrees)
 *          GRIN_EASTING_ERROR      : Easting outside of valid range
 *                                      (False_Easting +/- ~20,000,000 m,
 *                                       depending on ellipsoid parameters)
 *          GRIN_NORTHING_ERROR     : Northing outside of valid range
 *                                      (False_Northing +/- ~20,000,000 m,
 *                                       depending on ellipsoid parameters)
 *          GRIN_RADIUS_ERROR       : Coordinates too far from pole,
 *                                      depending on ellipsoid and
 *                                      projection parameters
 *          GRIN_CENT_MER_ERROR     : Central meridian outside of valid range
 *                                      (-180 to 360 degrees)
 *          GRIN_A_ERROR            : Semi-major axis less than or equal to zero
 *          GRIN_INV_F_ERROR        : Inverse flattening outside of valid range
 *									                    (250 to 350)
 *
 * REUSE NOTES
 *
 *    VAN DER GRINTEN is intended for reuse by any application that performs a
 *    Van Der Grinten projection or its inverse.
 *
 * REFERENCES
 *
 *    Further information on VAN DER GRINTEN can be found in the Reuse Manual.
 *
 *    VAN DER GRINTEN originated from :  U.S. Army Topographic Engineering Center
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
 *    VAN DER GRINTEN has no restrictions.
 *
 * ENVIRONMENT
 *
 *    VAN DER GRINTEN was tested and certified in the following environments:
 *
 *    1. Solaris 2.5 with GCC, version 2.8.1
 *    2. Windows 95 with MS Visual C++, version 6
 *
 * MODIFICATIONS
 *
 *    Date              Description
 *    ----              -----------
 *    10-02-97          Original Code
 *
 */


/***************************************************************************/
/*
 *                              DEFINES
 */

  #define GRIN_NO_ERROR           0x0000
  #define GRIN_LAT_ERROR          0x0001
  #define GRIN_LON_ERROR          0x0002
  #define GRIN_EASTING_ERROR      0x0004
  #define GRIN_NORTHING_ERROR     0x0008
  #define GRIN_CENT_MER_ERROR     0x0020
  #define GRIN_A_ERROR            0x0040
  #define GRIN_INV_F_ERROR        0x0080
  #define GRIN_RADIUS_ERROR		0x0100


/***************************************************************************/
/*
 *                              FUNCTION PROTOTYPES
 *                                for GRINTEN.C
 */

/* ensure proper linkage to c++ programs */
  #ifdef __cplusplus
extern "C" {
  #endif

  long Set_Van_der_Grinten_Parameters(double a,
                                      double f,
                                      double Central_Meridian,
                                      double False_Easting,
                                      double False_Northing);

/*
 * The function Set_Van_der_Grinten_Parameters receives the ellipsoid parameters and
 * Van Der Grinten projcetion parameters as inputs, and sets the corresponding state
 * variables.  If any errors occur, the error code(s) are returned by the function, otherwise
 * GRIN_NO_ERROR is returned.
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


  void Get_Van_der_Grinten_Parameters(double *a,
                                      double *f,
                                      double *Central_Meridian,
                                      double *False_Easting,
                                      double *False_Northing);

/*
 * The function Get_Van_der_Grinten_Parameters returns the current ellipsoid
 * parameters, and Van Der Grinten projection parameters.
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


  long Convert_Geodetic_To_Van_der_Grinten (double Latitude,
                                            double Longitude,
                                            double *Easting,
                                            double *Northing);
/*
 * The function Convert_Geodetic_To_Van_der_Grinten converts geodetic (latitude and
 * longitude) coordinates to Van Der Grinten projection easting, and northing
 * coordinates, according to the current ellipsoid and Van Der Grinten projection
 * parameters.  If any errors occur, the error code(s) are returned by the
 * function, otherwise GRIN_NO_ERROR is returned.
 *
 *    Latitude          : Latitude (phi) in radians           (input)
 *    Longitude         : Longitude (lambda) in radians       (input)
 *    Easting           : Easting (X) in meters               (output)
 *    Northing          : Northing (Y) in meters              (output)
 */


  long Convert_Van_der_Grinten_To_Geodetic(double Easting,
                                           double Northing,
                                           double *Latitude,
                                           double *Longitude);
/*
 * The function Convert_Van_der_Grinten_To_Geodetic converts Van Der Grinten projection
 * easting and northing coordinates to geodetic (latitude and longitude)
 * coordinates, according to the current ellipsoid and Van Der Grinten projection
 * coordinates.  If any errors occur, the error code(s) are returned by the
 * function, otherwise GRIN_NO_ERROR is returned.
 *
 *    Easting           : Easting (X) in meters                  (input)
 *    Northing          : Northing (Y) in meters                 (input)
 *    Latitude          : Latitude (phi) in radians              (output)
 *    Longitude         : Longitude (lambda) in radians          (output)
 */

  #ifdef __cplusplus
}
  #endif

#endif /* GRINTEN_H */

