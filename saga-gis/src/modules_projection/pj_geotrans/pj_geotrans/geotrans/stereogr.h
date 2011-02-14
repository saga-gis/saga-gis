#ifndef STEREOGR_H
  #define STEREOGR_H
/***************************************************************************/
/* RSC IDENTIFIER: STEREOGRAPHIC 
 *
 *
 * ABSTRACT
 *
 *    This component provides conversions between geodetic (latitude and
 *    longitude) coordinates and Stereographic (easting and northing) 
 *    coordinates.
 *
 * ERROR HANDLING
 *
 *    This component checks parameters for valid values.  If an invalid 
 *    value is found the error code is combined with the current error code 
 *    using the bitwise or.  This combining allows multiple error codes to 
 *    be returned. The possible error codes are:
 *
 *          STEREO_NO_ERROR           : No errors occurred in function
 *          STEREO_LAT_ERROR          : Latitude outside of valid range
 *                                      (-90 to 90 degrees)
 *          STEREO_LON_ERROR          : Longitude outside of valid range
 *                                      (-180 to 360 degrees) 
 *          STEREO_ORIGIN_LAT_ERROR   : Origin latitude outside of valid range
 *                                      (-90 to 90 degrees)
 *          STEREO_CENT_MER_ERROR     : Central meridian outside of valid range
 *                                      (-180 to 360 degrees)
 *          STEREO_EASTING_ERROR      : Easting outside of valid range,
 *                                      (False_Easting +/- ~1,460,090,226 m,
 *                                       depending on ellipsoid and projection
 *                                       parameters)
 *          STEREO_NORTHING_ERROR     : Northing outside of valid range,
 *                                      (False_Northing +/- ~1,460,090,226 m,
 *                                       depending on ellipsoid and projection
 *                                       parameters)
 *          STEREO_A_ERROR            : Semi-major axis less than or equal to zero
 *          STEREO_INV_F_ERROR        : Inverse flattening outside of valid range
 *								  	                  (250 to 350)
 *
 *
 * REUSE NOTES
 *
 *    STEREOGRAPHIC is intended for reuse by any application that  
 *    performs a Stereographic projection.
 *
 *
 * REFERENCES
 *
 *    Further information on STEREOGRAPHIC can be found in the
 *    Reuse Manual.
 *
 *
 *    STEREOGRAPHIC originated from :
 *                                U.S. Army Topographic Engineering Center
 *                                Geospatial Information Division
 *                                7701 Telegraph Road
 *                                Alexandria, VA  22310-3864
 *
 *
 * LICENSES
 *
 *    None apply to this component.
 *
 *
 * RESTRICTIONS
 *
 *    STEREOGRAPHIC has no restrictions.
 *
 *
 * ENVIRONMENT
 *
 *    STEREOGRAPHIC was tested and certified in the following
 *    environments:
 *
 *    1. Solaris 2.5 with GCC, version 2.8.1
 *    2. Window 95 with MS Visual C++, version 6
 *
 *
 * MODIFICATIONS
 *
 *    Date              Description
 *    ----              -----------
 *    7/26/00           Original Code
 *
 */

/**********************************************************************/
/*
 *                        DEFINES
 */

  #define STEREO_NO_ERROR                0x0000
  #define STEREO_LAT_ERROR               0x0001
  #define STEREO_LON_ERROR               0x0002
  #define STEREO_ORIGIN_LAT_ERROR        0x0004
  #define STEREO_CENT_MER_ERROR          0x0008
  #define STEREO_EASTING_ERROR			     0x0010
  #define STEREO_NORTHING_ERROR		       0x0020
  #define STEREO_A_ERROR                 0x0040
  #define STEREO_INV_F_ERROR             0x0080

/**********************************************************************/
/*
 *                        FUNCTION PROTOTYPES
 */

/* ensure proper linkage to c++ programs */
  #ifdef __cplusplus
extern "C" {
  #endif

long Set_Stereographic_Parameters (double a,
                                   double f,
                                   double Origin_Latitude,
                                   double Central_Meridian,
                                   double False_Easting,
                                   double False_Northing);
/*  
 *  The function Set_Stereographic_Parameters receives the ellipsoid
 *  parameters and Stereograpic projection parameters as inputs, and
 *  sets the corresponding state variables.  If any errors occur, error
 *  code(s) are returned by the function, otherwise STEREO_NO_ERROR is returned.
 *
 *  a                : Semi-major axis of ellipsoid, in meters         (input)
 *  f                : Flattening of ellipsoid					               (input)
 *  Origin_Latitude  : Latitude, in radians, at the center of          (input)
 *                       the projection
 *  Central_Meridian : Longitude, in radians, at the center of         (input)
 *                       the projection
 *  False_Easting    : Easting (X) at center of projection, in meters  (input)
 *  False_Northing   : Northing (Y) at center of projection, in meters (input)
 */

void Get_Stereographic_Parameters(double *a,
                                  double *f,
                                  double *Origin_Latitude,
                                  double *Central_Meridian,
                                  double *False_Easting,
                                  double *False_Northing);
/*
 * The function Get_Stereographic_Parameters returns the current ellipsoid
 * parameters and Stereographic projection parameters.
 *
 *    a                 : Semi-major axis of ellipsoid, in meters   (output)
 *    f                 : Flattening of ellipsoid						        (output)
 *    Origin_Latitude   : Latitude, in radians, at the center of    (output)
 *                          the projection
 *    Central_Meridian  : Longitude, in radians, at the center of   (output)
 *                          the projection
 *    False_Easting     : A coordinate value, in meters, assigned to the
 *                          central meridian of the projection.     (output)
 *    False_Northing    : A coordinate value, in meters, assigned to the
 *                          origin latitude of the projection       (output) 
 */

long Convert_Geodetic_To_Stereographic (double Latitude,
                                        double Longitude,
                                        double *Easting,
                                        double *Northing);
/*
 * The function Convert_Geodetic_To_Stereographic converts geodetic
 * coordinates (latitude and longitude) to Stereographic coordinates
 * (easting and northing), according to the current ellipsoid
 * and Stereographic projection parameters. If any errors occur, error
 * code(s) are returned by the function, otherwise STEREO_NO_ERROR is returned.
 *
 *    Latitude   :  Latitude, in radians                      (input)
 *    Longitude  :  Longitude, in radians                     (input)
 *    Easting    :  Easting (X), in meters                    (output)
 *    Northing   :  Northing (Y), in meters                   (output)
 */

long Convert_Stereographic_To_Geodetic(double Easting,
                                       double Northing,
                                       double *Latitude,
                                       double *Longitude);
/*
 * The function Convert_Stereographic_To_Geodetic converts Stereographic projection
 * (easting and northing) coordinates to geodetic (latitude and longitude)
 * coordinates, according to the current ellipsoid and Stereographic projection
 * coordinates.  If any errors occur, the error code(s) are returned by the
 * function, otherwise STEREO_NO_ERROR is returned.
 *
 *    Easting           : Easting (X), in meters              (input)
 *    Northing          : Northing (Y), in meters             (input)
 *    Latitude          : Latitude (phi), in radians          (output)
 *    Longitude         : Longitude (lambda), in radians      (output)
 */

  #ifdef __cplusplus
}
  #endif

#endif  /* STEREOGR_H  */
