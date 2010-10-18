#ifndef AZEQ_H
  #define AZEQ_H

/***************************************************************************/
/* RSC IDENTIFIER: AZIMUTHAL EQUIDISTANT
 *
 * ABSTRACT
 *
 *    This component provides conversions between Geodetic coordinates
 *    (latitude and longitude in radians) and Azimuthal Equidistant
 *    projection coordinates (easting and northing in meters).  This projection 
 *    employs a spherical Earth model.  The spherical radius used is the radius of 
 *    the sphere having the same area as the ellipsoid.
 *
 * ERROR HANDLING
 *
 *    This component checks parameters for valid values.  If an invalid value
 *    is found the error code is combined with the current error code using
 *    the bitwise or.  This combining allows multiple error codes to be
 *    returned. The possible error codes are:
 *
 *       AZEQ_NO_ERROR           : No errors occurred in function
 *       AZEQ_LAT_ERROR          : Latitude outside of valid range
 *                                     (-90 to 90 degrees)
 *       AZEQ_LON_ERROR          : Longitude outside of valid range
 *                                     (-180 to 360 degrees)
 *       AZEQ_EASTING_ERROR      : Easting outside of valid range
 *                                     (depends on ellipsoid and projection
 *                                     parameters)
 *       AZEQ_NORTHING_ERROR     : Northing outside of valid range
 *                                     (depends on ellipsoid and projection
 *                                     parameters)
 *       AZEQ_ORIGIN_LAT_ERROR   : Origin latitude outside of valid range
 *                                     (-90 to 90 degrees)
 *       AZEQ_CENT_MER_ERROR     : Central meridian outside of valid range
 *                                     (-180 to 360 degrees)
 *       AZEQ_A_ERROR            : Semi-major axis less than or equal to zero
 *       AZEQ_INV_F_ERROR        : Inverse flattening outside of valid range
 *									                   (250 to 350)
 *       AZEQ_PROJECTION_ERROR   : Point is plotted as a circle of radius PI * Ra
 *
 *
 * REUSE NOTES
 *
 *    AZIMUTHAL EQUIDISTANT is intended for reuse by any application that 
 *    performs an Azimuthal Equidistant projection or its inverse.
 *
 * REFERENCES
 *
 *    Further information on AZIMUTHAL EQUIDISTANT can be found in the Reuse Manual.
 *
 *    AZIMUTHAL EQUIDISTANT originated from:     U.S. Army Topographic Engineering Center
 *                                               Geospatial Information Division
 *                                               7701 Telegraph Road
 *                                               Alexandria, VA  22310-3864
 *
 * LICENSES
 *
 *    None apply to this component.
 *
 * RESTRICTIONS
 *
 *    AZIMUTHAL EQUIDISTANT has no restrictions.
 *
 * ENVIRONMENT
 *
 *    AZIMUTHAL EQUIDISTANT was tested and certified in the following environments:
 *
 *    1. Solaris 2.5 with GCC, version 2.8.1
 *    2. MSDOS with MS Visual C++, version 6
 *
 * MODIFICATIONS
 *
 *    Date              Description
 *    ----              -----------
 *    05-19-00          Original Code
 *    
 *
 */

/***************************************************************************/
/*
 *                              DEFINES
 */

  #define AZEQ_NO_ERROR           0x0000
  #define AZEQ_LAT_ERROR          0x0001
  #define AZEQ_LON_ERROR          0x0002
  #define AZEQ_EASTING_ERROR      0x0004
  #define AZEQ_NORTHING_ERROR     0x0008
  #define AZEQ_ORIGIN_LAT_ERROR   0x0010
  #define AZEQ_CENT_MER_ERROR     0x0020
  #define AZEQ_A_ERROR            0x0040
  #define AZEQ_INV_F_ERROR        0x0080
  #define AZEQ_PROJECTION_ERROR   0x0100



/***************************************************************************/
/*
 *                              FUNCTION PROTOTYPES
 *                                for AZEQ.C
 */

/* ensure proper linkage to c++ programs */
  #ifdef __cplusplus
extern "C" {
  #endif

  long Set_Azimuthal_Equidistant_Parameters(double a,
                                            double f,
                                            double Origin_Latitude,
                                            double Central_Meridian,
                                            double False_Easting,
                                            double False_Northing);
/*
 * The function Set_Azimuthal_Equidistant_Parameters receives the ellipsoid 
 * parameters and projection parameters as inputs, and sets the corresponding state
 * variables.  If any errors occur, the error code(s) are returned by the function, 
 * otherwise AZEQ_NO_ERROR is returned.
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

  void Get_Azimuthal_Equidistant_Parameters(double *a,
                                            double *f,
                                            double *Origin_Latitude,
                                            double *Central_Meridian,
                                            double *False_Easting,
                                            double *False_Northing);
/*
 * The function Get_Azimuthal_Equidistant_Parameters returns the current ellipsoid
 * parameters and Azimuthal Equidistant projection parameters.
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


  long Convert_Geodetic_To_Azimuthal_Equidistant (double Latitude,
                                                  double Longitude,
                                                  double *Easting,
                                                  double *Northing);
/*
 * The function Convert_Geodetic_To_Azimuthal_Equidistant converts geodetic (latitude and
 * longitude) coordinates to Azimuthal Equidistant projection (easting and northing)
 * coordinates, according to the current ellipsoid and Azimuthal Equidistant projection
 * parameters.  If any errors occur, the error code(s) are returned by the
 * function, otherwise AZEQ_NO_ERROR is returned.
 *
 *    Latitude          : Latitude (phi) in radians           (input)
 *    Longitude         : Longitude (lambda) in radians       (input)
 *    Easting           : Easting (X) in meters               (output)
 *    Northing          : Northing (Y) in meters              (output)
 */

  long Convert_Azimuthal_Equidistant_To_Geodetic(double Easting,
                                                 double Northing,
                                                 double *Latitude,
                                                 double *Longitude);
/*
 * The function Convert_Azimuthal_Equidistant_To_Geodetic converts Azimuthal Equidistant 
 * projection (easting and northing) coordinates to geodetic (latitude and longitude)
 * coordinates, according to the current ellipsoid and Azimuthal Equidistant projection
 * coordinates.  If any errors occur, the error code(s) are returned by the
 * function, otherwise AZEQ_NO_ERROR is returned.
 *
 *    Easting           : Easting (X) in meters                  (input)
 *    Northing          : Northing (Y) in meters                 (input)
 *    Latitude          : Latitude (phi) in radians              (output)
 *    Longitude         : Longitude (lambda) in radians          (output)
 */

  #ifdef __cplusplus
}
  #endif

#endif /* AZEQ_H */
