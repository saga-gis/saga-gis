/**********************************************************
 * Version $Id$
 *********************************************************/
#ifndef NEYS_H
  #define NEYS_H

/***************************************************************************/
/* RSC IDENTIFIER: NEYS
 *
 * ABSTRACT
 *
 *    This component provides conversions between Geodetic coordinates
 *    (latitude and longitude in radians) and Ney's (Modified Lambert 
 *    Conformal Conic) projection coordinates (easting and northing in meters).
 *
 * ERROR HANDLING
 *
 *    This component checks parameters for valid values.  If an invalid value
 *    is found the error code is combined with the current error code using
 *    the bitwise or.  This combining allows multiple error codes to be
 *    returned. The possible error codes are:
 *
 *       NEYS_NO_ERROR           : No errors occurred in function
 *       NEYS_LAT_ERROR          : Latitude outside of valid range
 *                                     (-90 to 90 degrees)
 *       NEYS_LON_ERROR          : Longitude outside of valid range
 *                                     (-180 to 360 degrees)
 *       NEYS_EASTING_ERROR      : Easting outside of valid range
 *                                     (depends on ellipsoid and projection
 *                                     parameters)
 *       NEYS_NORTHING_ERROR     : Northing outside of valid range
 *                                     (depends on ellipsoid and projection
 *                                     parameters)
 *       NEYS_FIRST_STDP_ERROR   : First standard parallel outside of valid
 *                                     range (±71 or ±74 degrees)
 *       NEYS_ORIGIN_LAT_ERROR   : Origin latitude outside of valid range
 *                                     (-89 59 59.0 to 89 59 59.0 degrees)
 *       NEYS_CENT_MER_ERROR     : Central meridian outside of valid range
 *                                     (-180 to 360 degrees)
 *       NEYS_A_ERROR            : Semi-major axis less than or equal to zero
 *       NEYS_INV_F_ERROR        : Inverse flattening outside of valid range
 *									                   (250 to 350)
 *
 *
 * REUSE NOTES
 *
 *    NEYS is intended for reuse by any application that performs a Ney's (Modified
 *    Lambert Conformal Conic) projection or its inverse.
 *    
 * REFERENCES
 *
 *    Further information on NEYS can be found in the Reuse Manual.
 *
 *    NEYS originated from:
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
 *    NEYS has no restrictions.
 *
 * ENVIRONMENT
 *
 *    NEYS was tested and certified in the following environments:
 *
 *    1. Solaris 2.5 with GCC, version 2.8.1
 *    2. Windows 95 with MS Visual C++, version 6
 *
 * MODIFICATIONS
 *
 *    Date              Description
 *    ----              -----------
 *    8-4-00            Original Code
 *
 *
 *
 */

/***************************************************************************/
/*
 *                              DEFINES
 */

  #define NEYS_NO_ERROR           0x0000
  #define NEYS_LAT_ERROR          0x0001
  #define NEYS_LON_ERROR          0x0002
  #define NEYS_EASTING_ERROR      0x0004
  #define NEYS_NORTHING_ERROR     0x0008
  #define NEYS_FIRST_STDP_ERROR   0x0010
  #define NEYS_ORIGIN_LAT_ERROR   0x0020
  #define NEYS_CENT_MER_ERROR     0x0040
  #define NEYS_A_ERROR            0x0080
  #define NEYS_INV_F_ERROR        0x0100


/***************************************************************************/
/*
 *                              FUNCTION PROTOTYPES
 *                                for NEYS.C
 */

/* ensure proper linkage to c++ programs */
  #ifdef __cplusplus
extern "C" {
  #endif


  long Set_Neys_Parameters(double a,
                           double f,
                           double Origin_Latitude,
                           double Central_Meridian,
                           double Std_Parallel_1,
                           double False_Easting,
                           double False_Northing);
/*
 * The function Set_Neys_Parameters receives the ellipsoid parameters and
 * Ney's (Modified Lambert Conformal Conic) projection parameters as inputs, and sets the
 * corresponding state variables.  If any errors occur, the error code(s)
 * are returned by the function, otherwise NEYS_NO_ERROR is returned.
 *
 *   a                   : Semi-major axis of ellipsoid, in meters   (input)
 *   f                   : Flattening of ellipsoid						       (input)
 *   Origin_Latitude     : Latitude of origin, in radians            (input)
 *   Central_Meridian    : Longitude of origin, in radians           (input)
 *   Std_Parallel_1      : First standard parallel, in radians       (input)
 *   False_Easting       : False easting, in meters                  (input)
 *   False_Northing      : False northing, in meters                 (input)
 */


  void Get_Neys_Parameters(double *a,
                           double *f,
                           double *Origin_Latitude,
                           double *Central_Meridian,
                           double *Std_Parallel_1,
                           double *False_Easting,
                           double *False_Northing);
/*                         
 * The function Get_Neys_Parameters returns the current ellipsoid
 * parameters and Ney's (Modified Lambert Conformal Conic) projection parameters.
 *
 *   a                   : Semi-major axis of ellipsoid, in meters   (output)
 *   f                   : Flattening of ellipsoid					         (output)
 *   Origin_Latitude     : Latitude of origin, in radians            (output)
 *   Central_Meridian    : Longitude of origin, in radians           (output)
 *   Std_Parallel_1      : First standard parallel, in radians       (output)
 *   False_Easting       : False easting, in meters                  (output)
 *   False_Northing      : False northing, in meters                 (output)
 */


  long Convert_Geodetic_To_Neys(double Latitude,
                                double Longitude,
                                double *Easting,
                                double *Northing);
/*
 * The function Convert_Geodetic_To_Neys converts Geodetic (latitude and
 * longitude) coordinates to Ney's (Modified Lambert Conformal Conic) projection 
 * (easting and northing) coordinates, according to the current ellipsoid and
 * Ney's (Modified Lambert Conformal Conic) projection parameters.  If any errors 
 * occur, the error code(s) are returned by the function, otherwise NEYS_NO_ERROR is
 * returned.
 *
 *    Latitude         : Latitude, in radians                         (input)
 *    Longitude        : Longitude, in radians                        (input)
 *    Easting          : Easting (X), in meters                       (output)
 *    Northing         : Northing (Y), in meters                      (output)
 */


  long Convert_Neys_To_Geodetic(double Easting,
                                double Northing,
                                double *Latitude,
                                double *Longitude);
/*
 * The function Convert_Neys_To_Geodetic converts Ney's (Modified Lambert Conformal
 * Conic) projection (easting and northing) coordinates to Geodetic (latitude) 
 * and longitude) coordinates, according to the current ellipsoid and Ney's 
 * (Modified Lambert Conformal Conic) projection parameters.  If any errors occur,
 * the error code(s) are returned by the function, otherwise NEYS_NO_ERROR
 * is returned.
 *
 *    Easting          : Easting (X), in meters                       (input)
 *    Northing         : Northing (Y), in meters                      (input)
 *    Latitude         : Latitude, in radians                         (output)
 *    Longitude        : Longitude, in radians                        (output)
 */


  #ifdef __cplusplus
}
  #endif

#endif /* NEYS_H */
