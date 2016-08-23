/**********************************************************
 * Version $Id: lambert.h 911 2011-02-14 16:38:15Z reklov_w $
 *********************************************************/
#ifndef LAMBERT_H
  #define LAMBERT_H

/***************************************************************************/
/* RSC IDENTIFIER: LAMBERT
 *
 * ABSTRACT
 *
 *    This component provides conversions between Geodetic coordinates 
 *    (latitude and longitude in radians) and Lambert Conformal Conic
 *    projection coordinates (easting and northing in meters) defined 
 *    by two standard parallels.  When both standard parallel parameters
 *    are set to the same latitude value, the result is a Lambert 
 *    Conformal Conic projection with one standard parallel at the 
 *    specified latitude.
 *
 * ERROR HANDLING
 *
 *    This component checks parameters for valid values.  If an invalid value
 *    is found the error code is combined with the current error code using 
 *    the bitwise or.  This combining allows multiple error codes to be
 *    returned. The possible error codes are:
 *
 *       LAMBERT_NO_ERROR           : No errors occurred in function
 *       LAMBERT_LAT_ERROR          : Latitude outside of valid range
 *                                     (-90 to 90 degrees)
 *       LAMBERT_LON_ERROR          : Longitude outside of valid range
 *                                     (-180 to 360 degrees)
 *       LAMBERT_EASTING_ERROR      : Easting outside of valid range
 *                                     (depends on ellipsoid and projection
 *                                     parameters)
 *       LAMBERT_NORTHING_ERROR     : Northing outside of valid range
 *                                     (depends on ellipsoid and projection
 *                                     parameters)
 *       LAMBERT_FIRST_STDP_ERROR   : First standard parallel outside of valid
 *                                     range (-90 to 90 degrees)
 *       LAMBERT_SECOND_STDP_ERROR  : Second standard parallel outside of valid
 *                                     range (-90 to 90 degrees)
 *       LAMBERT_ORIGIN_LAT_ERROR   : Origin latitude outside of valid range
 *                                     (-90 to 90 degrees)
 *       LAMBERT_CENT_MER_ERROR     : Central meridian outside of valid range
 *                                     (-180 to 360 degrees)
 *       LAMBERT_A_ERROR            : Semi-major axis less than or equal to zero
 *       LAMBERT_INV_F_ERROR        : Inverse flattening outside of valid range
 *									   (250 to 350)
 *       LAMBERT_HEMISPHERE_ERROR   : Standard parallels cannot be opposite latitudes
 *       LAMBERT_FIRST_SECOND_ERROR : The 1st & 2nd standard parallels cannot
 *                                     both be 0
 *
 *
 * REUSE NOTES
 *
 *    LAMBERT is intended for reuse by any application that performs a Lambert
 *    Conformal Conic projection or its inverse.
 *    
 * REFERENCES
 *
 *    Further information on LAMBERT can be found in the Reuse Manual.
 *
 *    LAMBERT originated from:
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
 *    LAMBERT has no restrictions.
 *
 * ENVIRONMENT
 *
 *    LAMBERT was tested and certified in the following environments:
 *
 *    1. Solaris 2.5 with GCC, version 2.8.1
 *    2. Windows 98/2000 with MS Visual C++, version 6
 *
 * MODIFICATIONS
 *
 *    Date              Description
 *    ----              -----------
 *    10-02-97          Original Code
 *    08-15-99          Re-engineered Code
 *    03-05-05          Re-engineered Code
 *
 */


/***************************************************************************/
/*
 *                              DEFINES
 */

  #define LAMBERT_NO_ERROR           0x0000
  #define LAMBERT_LAT_ERROR          0x0001
  #define LAMBERT_LON_ERROR          0x0002
  #define LAMBERT_EASTING_ERROR      0x0004
  #define LAMBERT_NORTHING_ERROR     0x0008
  #define LAMBERT_FIRST_STDP_ERROR   0x0010
  #define LAMBERT_SECOND_STDP_ERROR  0x0020
  #define LAMBERT_ORIGIN_LAT_ERROR   0x0040
  #define LAMBERT_CENT_MER_ERROR     0x0080
  #define LAMBERT_A_ERROR            0x0100
  #define LAMBERT_INV_F_ERROR        0x0200
  #define LAMBERT_HEMISPHERE_ERROR   0x0400
  #define LAMBERT_FIRST_SECOND_ERROR 0x0800


/***************************************************************************/
/*
 *                              FUNCTION PROTOTYPES
 *                                for LAMBERT.C
 */

/* ensure proper linkage to c++ programs */
  #ifdef __cplusplus
extern "C" {
  #endif



  long Set_Lambert_Parameters(double a,
                              double f,
                              double Origin_Latitude,
                              double Central_Meridian,
                              double Std_Parallel_1,
                              double Std_Parallel_2,
                              double False_Easting,
                              double False_Northing);

/*
 * The function Set_Lambert_Parameters receives the ellipsoid parameters and
 * Lambert Conformal Conic projection parameters as inputs, and sets the
 * corresponding state variables.  If any errors occur, the error code(s)
 * are returned by the function, otherwise LAMBERT_NO_ERROR is returned.
 *
 *   a                   : Semi-major axis of ellipsoid, in meters   (input)
 *   f                   : Flattening of ellipsoid				  	 (input)
 *   Origin_Latitude     : Latitude of origin in radians             (input)
 *   Central_Meridian    : Longitude of origin in radians            (input)
 *   Std_Parallel_1      : First standard parallel                   (input)
 *   Std_Parallel_2      : Second standard parallel                  (input)
 *   False_Easting       : False easting in meters                   (input)
 *   False_Northing      : False northing in meters                  (input)
 *
 *   Note that when the two standard parallel parameters are both set to the 
 *   same latitude value, the result is a Lambert Conformal Conic projection 
 *   with one standard parallel at the specified latitude.
 */



  void Get_Lambert_Parameters(double *a,
                              double *f,
                              double *Origin_Latitude,
                              double *Central_Meridian,
                              double *Std_Parallel_1,
                              double *Std_Parallel_2,
                              double *False_Easting,
                              double *False_Northing);

/*                         
 * The function Get_Lambert_Parameters returns the current ellipsoid
 * parameters and Lambert Conformal Conic projection parameters.
 *
 *   a                   : Semi-major axis of ellipsoid, in meters   (output)
 *   f                   : Flattening of ellipsoid					 (output)
 *   Origin_Latitude     : Latitude of origin, in radians            (output)
 *   Central_Meridian    : Longitude of origin, in radians           (output)
 *   Std_Parallel_1      : First standard parallel                   (output)
 *   Std_Parallel_2      : Second standard parallel                  (output)
 *   False_Easting       : False easting, in meters                  (output)
 *   False_Northing      : False northing, in meters                 (output) 
 */



  long Convert_Geodetic_To_Lambert (double Latitude,
                                    double Longitude,
                                    double *Easting,
                                    double *Northing);

/*
 * The function Convert_Geodetic_To_Lambert converts Geodetic (latitude and
 * longitude) coordinates to Lambert Conformal Conic projection (easting
 * and northing) coordinates, according to the current ellipsoid and
 * Lambert Conformal Conic projection parameters.  If any errors occur, the
 * error code(s) are returned by the function, otherwise LAMBERT_NO_ERROR is
 * returned.
 *
 *    Latitude         : Latitude in radians                         (input)
 *    Longitude        : Longitude in radians                        (input)
 *    Easting          : Easting (X) in meters                       (output)
 *    Northing         : Northing (Y) in meters                      (output)
 */


  long Convert_Lambert_To_Geodetic (double Easting,
                                    double Northing,
                                    double *Latitude,
                                    double *Longitude);

/*
 * The function Convert_Lambert_To_Geodetic converts Lambert Conformal
 * Conic projection (easting and northing) coordinates to Geodetic
 * (latitude and longitude) coordinates, according to the current ellipsoid
 * and Lambert Conformal Conic projection parameters.  If any errors occur,
 * the error code(s) are returned by the function, otherwise LAMBERT_NO_ERROR
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

#endif /* LAMBERT_H */
