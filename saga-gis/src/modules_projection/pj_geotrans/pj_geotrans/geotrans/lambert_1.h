#ifndef LAMBERT_1_H
  #define LAMBERT_1_H

/***************************************************************************/
/* RSC IDENTIFIER: LAMBERT_1
 *
 * ABSTRACT
 *
 *    This component provides conversions between Geodetic coordinates
 *    (latitude and longitude in radians) and Lambert Conformal Conic
 *    (1 parallel) projection coordinates (easting and northing in meters) defined
 *    by one standard parallel.  
 *
 * ERROR HANDLING
 *
 *    This component checks parameters for valid values.  If an invalid value
 *    is found the error code is combined with the current error code using
 *    the bitwise or.  This combining allows multiple error codes to be
 *    returned. The possible error codes are:
 *
 *       LAMBERT_1_NO_ERROR           : No errors occurred in function
 *       LAMBERT_1_LAT_ERROR          : Latitude outside of valid range
 *                                      (-90 to 90 degrees)
 *       LAMBERT_1_LON_ERROR          : Longitude outside of valid range
 *                                      (-180 to 360 degrees)
 *       LAMBERT_1_EASTING_ERROR      : Easting outside of valid range
 *                                      (depends on ellipsoid and projection
 *                                     parameters)
 *       LAMBERT_1_NORTHING_ERROR     : Northing outside of valid range
 *                                      (depends on ellipsoid and projection
 *                                     parameters)
 *       LAMBERT_1_ORIGIN_LAT_ERROR   : Origin latitude outside of valid
 *                                      range (-89 59 59.0 to 89 59 59.0 degrees)
 *       LAMBERT_1_CENT_MER_ERROR     : Central meridian outside of valid range
 *                                      (-180 to 360 degrees)
 *	     LAMBERT_1_SCALE_FACTOR_ERROR : Scale factor outside of valid
 *                                      range (0.3 to 3.0)
 *       LAMBERT_1_A_ERROR            : Semi-major axis less than or equal to zero
 *       LAMBERT_1_INV_F_ERROR        : Inverse flattening outside of valid range
 *									    (250 to 350)
 *
 *
 * REUSE NOTES
 *
 *    LAMBERT_1 is intended for reuse by any application that performs a Lambert
 *    Conformal Conic projection or its inverse.
 *    
 * REFERENCES
 *
 *    Further information on LAMBERT_1 can be found in the Reuse Manual.
 *
 *    LAMBERT_1 originated from:
 *                      U.S. Army Topographic Engineering Center
 *                      Geospatial Information Division
 *                      7701 Telegraph Road
 *                      Alexandria, VA  22310-3864
 *
 * LICENSES
 *
 *    None apply to this component.
 *
 * RESTRICTIONS
 *
 *    LAMBERT_1 has no restrictions.
 *
 * ENVIRONMENT
 *
 *    LAMBERT_1 was tested and certified in the following environments:
 *
 *    1. Solaris 2.5 with GCC, version 2.8.1
 *    2. Windows 98/2000 with MS Visual C++, version 6
 *
 * MODIFICATIONS
 *
 *    Date              Description
 *    ----              -----------
 *    03-05-05          Original Code
 *
 *
 */


/***************************************************************************/
/*
 *                              DEFINES
 */

  #define LAMBERT_1_NO_ERROR           0x0000
  #define LAMBERT_1_LAT_ERROR          0x0001
  #define LAMBERT_1_LON_ERROR          0x0002
  #define LAMBERT_1_EASTING_ERROR      0x0004
  #define LAMBERT_1_NORTHING_ERROR     0x0008
  #define LAMBERT_1_ORIGIN_LAT_ERROR   0x0010
  #define LAMBERT_1_CENT_MER_ERROR     0x0020
  #define LAMBERT_1_SCALE_FACTOR_ERROR 0x0040
  #define LAMBERT_1_A_ERROR            0x0080
  #define LAMBERT_1_INV_F_ERROR        0x0100


/***************************************************************************/
/*
 *                              FUNCTION PROTOTYPES
 *                                for LAMBERT_1.C
 */

/* ensure proper linkage to c++ programs */
  #ifdef __cplusplus
extern "C" {
  #endif


  long Set_Lambert_1_Parameters(double a,
                                double f,
                                double Origin_Latitude,
                                double Central_Meridian,
                                double False_Easting,
                                double False_Northing,
                                double Scale_Factor);

/*
 * The function Set_Lambert_1_Parameters receives the ellipsoid parameters and
 * Lambert Conformal Conic (1 parallel) projection parameters as inputs, and sets the
 * corresponding state variables.  If any errors occur, the error code(s)
 * are returned by the function, otherwise LAMBERT_1_NO_ERROR is returned.
 *
 *   a                   : Semi-major axis of ellipsoid, in meters   (input)
 *   f                   : Flattening of ellipsoid					 (input)
 *   Origin_Latitude     : Latitude of origin, in radians            (input)
 *   Central_Meridian    : Longitude of origin, in radians           (input)
 *   False_Easting       : False easting, in meters                  (input)
 *   False_Northing      : False northing, in meters                 (input)
 *   Scale_Factor        : Projection scale factor                   (input) 
 *
 */


  void Get_Lambert_1_Parameters(double *a,
                                double *f,
                                double *Origin_Latitude,
                                double *Central_Meridian,
                                double *False_Easting,
                                double *False_Northing,
                                double *Scale_Factor);

/*                         
 * The function Get_Lambert_1_Parameters returns the current ellipsoid
 * parameters and Lambert Conformal Conic (1 parallel) projection parameters.
 *
 *   a                   : Semi-major axis of ellipsoid, in meters   (output)
 *   f                   : Flattening of ellipsoid					 (output)
 *   Origin_Latitude     : Latitude of origin, in radians            (output)
 *   Central_Meridian    : Longitude of origin, in radians           (output)
 *   False_Easting       : False easting, in meters                  (output)
 *   False_Northing      : False northing, in meters                 (output)
 *   Scale_Factor        : Projection scale factor                   (output) 
 */


  long Convert_Geodetic_To_Lambert_1 (double Latitude,
                                      double Longitude,
                                      double *Easting,
                                      double *Northing);

/*
 * The function Convert_Geodetic_To_Lambert_1 converts Geodetic (latitude and
 * longitude) coordinates to Lambert Conformal Conic (1 parallel) projection (easting
 * and northing) coordinates, according to the current ellipsoid and
 * Lambert Conformal Conic (1 parallel) projection parameters.  If any errors occur, the
 * error code(s) are returned by the function, otherwise LAMBERT_1_NO_ERROR is
 * returned.
 *
 *    Latitude         : Latitude in radians                         (input)
 *    Longitude        : Longitude in radians                        (input)
 *    Easting          : Easting (X) in meters                       (output)
 *    Northing         : Northing (Y) in meters                      (output)
 */


  long Convert_Lambert_1_To_Geodetic (double Easting,
                                      double Northing,
                                      double *Latitude,
                                      double *Longitude);

/*
 * The function Convert_Lambert_1_To_Geodetic converts Lambert Conformal
 * Conic (1 parallel) projection (easting and northing) coordinates to Geodetic
 * (latitude and longitude) coordinates, according to the current ellipsoid
 * and Lambert Conformal Conic (1 parallel) projection parameters.  If any errors occur,
 * the error code(s) are returned by the function, otherwise LAMBERT_1_NO_ERROR
 * is returned.
 *
 *    Easting          : Easting (X) in meters                       (input)
 *    Northing         : Northing (Y) in meters                      (input)
 *    Latitude         : Latitude in radians                         (output)
 *    Longitude        : Longitude in radians                        (output)
 */


  #ifdef __cplusplus
}
  #endif

#endif /* LAMBERT_1_H */
