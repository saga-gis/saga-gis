#ifndef TRCYLEQA_H
  #define TRCYLEQA_H

/***************************************************************************/
/* RSC IDENTIFIER: TRANSVERSE CYLINDRICAL EQUAL AREA
 *
 * ABSTRACT
 *
 *    This component provides conversions between Geodetic coordinates
 *    (latitude and longitude in radians) and Transverse Cylindrical Equal Area 
 *    projection coordinates (easting and northing in meters).
 *
 * ERROR HANDLING
 *
 *    This component checks parameters for valid values.  If an invalid value
 *    is found, the error code is combined with the current error code using 
 *    the bitwise or.  This combining allows multiple error codes to be
 *    returned. The possible error codes are:
 *
 *          TCEA_NO_ERROR           : No errors occurred in function
 *          TCEA_LAT_ERROR          : Latitude outside of valid range
 *                                      (-90 to 90 degrees)
 *          TCEA_LON_ERROR          : Longitude outside of valid range
 *                                      (-180 to 360 degrees)
 *          TCEA_EASTING_ERROR      : Easting outside of valid range
 *                                      (False_Easting +/- ~6,500,000 m,
 *                                       depending on ellipsoid parameters
 *                                       and Origin_Latitude)
 *          TCEA_NORTHING_ERROR     : Northing outside of valid range
 *                                      (False_Northing +/- ~20,000,000 m,
 *                                       depending on ellipsoid parameters
 *                                       and Origin_Latitude)
 *          TCEA_ORIGIN_LAT_ERROR   : Origin latitude outside of valid range
 *                                      (-90 to 90 degrees)
 *          TCEA_CENT_MER_ERROR     : Central meridian outside of valid range
 *                                      (-180 to 360 degrees)
 *          TCEA_A_ERROR            : Semi-major axis less than or equal to zero
 *          TCEA_INV_F_ERROR        : Inverse flattening outside of valid range
 *								  	                  (250 to 350)
 *			TCEA_SCALE_FACTOR_ERROR : Scale factor outside of valid
 *                                      range (0.3 to 3.0)
 *		    TCEA_LON_WARNING        : Distortion will result if longitude is more
 *                                     than 90 degrees from the Central Meridian
 *
 * REUSE NOTES
 *
 *    TRANSVERSE CYLINDRICAL EQUAL AREA is intended for reuse by any application that
 *    performs a Transverse Cylindrical Equal Area projection or its inverse.
 *    
 * REFERENCES
 *
 *    Further information on TRANSVERSE CYLINDRICAL EQUAL AREA can be found in the Reuse Manual.
 *
 *    TRANSVERSE CYLINDRICAL EQUAL AREA originated from :  
 *                                U.S. Army Topographic Engineering Center
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
 *    TRANSVERSE CYLINDRICAL EQUAL AREA has no restrictions.
 *
 * ENVIRONMENT
 *
 *    TRANSVERSE CYLINDRICAL EQUAL AREA was tested and certified in the following environments:
 *
 *    1. Solaris 2.5 with GCC, version 2.8.1
 *    2. Windows 95 with MS Visual C++, version 6
 *
 * MODIFICATIONS
 *
 *    Date              Description
 *    ----              -----------
 *    7-15-99          Original Code
 *
 */


/***************************************************************************/
/*
 *                              DEFINES
 */

  #define TCEA_NO_ERROR           0x0000
  #define TCEA_LAT_ERROR          0x0001
  #define TCEA_LON_ERROR          0x0002
  #define TCEA_EASTING_ERROR      0x0004
  #define TCEA_NORTHING_ERROR     0x0008
  #define TCEA_ORIGIN_LAT_ERROR   0x0010
  #define TCEA_CENT_MER_ERROR     0x0020
  #define TCEA_A_ERROR            0x0040
  #define TCEA_INV_F_ERROR        0x0080
  #define TCEA_SCALE_FACTOR_ERROR 0x0100
  #define TCEA_LON_WARNING        0x0200

/***************************************************************************/
/*
 *                              FUNCTION PROTOTYPES
 *                                for TRCYLEQA.C
 */

/* ensure proper linkage to c++ programs */
  #ifdef __cplusplus
extern "C" {
  #endif

  long Set_Trans_Cyl_Eq_Area_Parameters(double a,
                                        double f,
                                        double Origin_Latitude,
                                        double Central_Meridian,
                                        double False_Easting,
                                        double False_Northing,
                                        double Scale_Factor);
/*
 * The function Set_Trans_Cyl_Eq_Area_Parameters receives the ellipsoid parameters and
 * Transverse Cylindrical Equal Area projcetion parameters as inputs, and sets the corresponding
 * state variables.  If any errors occur, the error code(s) are returned by the function, 
 * otherwise TCEA_NO_ERROR is returned.
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
 *    Scale_Factor      : Multiplier which reduces distances in the 
 *                          projection to the actual distance on the
 *                          ellipsoid                               (input)
 */


  void Get_Trans_Cyl_Eq_Area_Parameters(double *a,
                                        double *f,
                                        double *Origin_Latitude,
                                        double *Central_Meridian,
                                        double *False_Easting,
                                        double *False_Northing,
                                        double *Scale_Factor);
/*
 * The function Get_Trans_Cyl_Eq_Area_Parameters returns the current ellipsoid
 * parameters, Transverse Cylindrical Equal Area projection parameters, and scale factor.
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
 *    Scale_Factor      : Multiplier which reduces distances in the
 *                          projection to the actual distance on the
 *                          ellipsoid                               (output)
 */


  long Convert_Geodetic_To_Trans_Cyl_Eq_Area (double Latitude,
                                              double Longitude,
                                              double *Easting,
                                              double *Northing); 
/*
 * The function Convert_Geodetic_To_Trans_Cyl_Eq_Area converts geodetic (latitude and
 * longitude) coordinates to Transverse Cylindrical Equal Area projection easting, and northing
 * coordinates, according to the current ellipsoid and Transverse Cylindrical Equal Area projection
 * parameters.  If any errors occur, the error code(s) are returned by the
 * function, otherwise TCEA_NO_ERROR is returned.
 *
 *    Latitude          : Latitude (phi) in radians           (input)
 *    Longitude         : Longitude (lambda) in radians       (input)
 *    Easting           : Easting (X) in meters               (output)
 *    Northing          : Northing (Y) in meters              (output)
 */


  long Convert_Trans_Cyl_Eq_Area_To_Geodetic(double Easting,
                                             double Northing,
                                             double *Latitude,
                                             double *Longitude);
/*
 * The function Convert_Trans_Cyl_Eq_Area_To_Geodetic converts Transverse Cylindrical Equal Area 
 * projection easting and northing coordinates to geodetic (latitude and longitude) coordinates.
 * coordinates, according to the current ellipsoid and Transverse Cylindrical Equal Area projection
 * If any errors occur, the error code(s) are returned by the function, otherwise TCEA_NO_ERROR 
 * is returned.
 *
 *    Easting           : Easting (X) in meters                  (input)
 *    Northing          : Northing (Y) in meters                 (input)
 *    Latitude          : Latitude (phi) in radians              (output)
 *    Longitude         : Longitude (lambda) in radians          (output)
 */

  #ifdef __cplusplus
}
  #endif

#endif /* TRCYLEQA_H */

