#ifndef CYLEQA_H
  #define CYLEQA_H

/***************************************************************************/
/* RSC IDENTIFIER: CYLINDRICAL EQUAL AREA
 *
 * ABSTRACT
 *
 *    This component provides conversions between Geodetic coordinates 
 *    (latitude and longitude in radians) and Cylindrical Equal Area projection
 *    coordinates (easting and northing in meters).
 *
 * ERROR HANDLING
 *
 *    This component checks parameters for valid values.  If an invalid value
 *    is found, the error code is combined with the current error code using 
 *    the bitwise or.  This combining allows multiple error codes to be
 *    returned. The possible error codes are:
 *
 *          CYEQ_NO_ERROR           : No errors occurred in function
 *          CYEQ_LAT_ERROR          : Latitude outside of valid range
 *                                      (-90 to 90 degrees)
 *          CYEQ_LON_ERROR          : Longitude outside of valid range
 *                                      (-180 to 360 degrees)
 *          CYEQ_EASTING_ERROR      : Easting outside of valid range
 *                                      (False_Easting +/- ~20,000,000 m,
 *                                       depending on ellipsoid parameters
 *                                       and Origin_Latitude)
 *          CYEQ_NORTHING_ERROR     : Northing outside of valid range
 *                                      (False_Northing +/- ~6,000,000 m,
 *                                       depending on ellipsoid parameters
 *                                       and Origin_Latitude)
 *          CYEQ_ORIGIN_LAT_ERROR   : Origin latitude outside of valid range
 *                                      (-90 to 90 degrees)
 *          CYEQ_CENT_MER_ERROR     : Central meridian outside of valid range
 *                                      (-180 to 360 degrees)
 *          CYEQ_A_ERROR            : Semi-major axis less than or equal to zero
 *          CYEQ_INV_F_ERROR        : Inverse flattening outside of valid range
 *									                    (250 to 350)
 *
 * REUSE NOTES
 *
 *    CYLINDRICAL EQUAL AREA is intended for reuse by any application that performs a
 *    Cylindrical Equal Area projection or its inverse.
 *    
 * REFERENCES
 *
 *    Further information on CYLINDRICAL EQUAL AREA can be found in the Reuse Manual.
 *
 *    CYLINDRICAL EQUAL AREA originated from :  
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
 *    CYLINDRICAL EQUAL AREA has no restrictions.
 *
 * ENVIRONMENT
 *
 *    CYLINDRICAL EQUAL AREA was tested and certified in the following environments:
 *
 *    1. Solaris 2.5 with GCC 2.8.1
 *    2. MS Windows 95 with MS Visual C++ 6
 *
 * MODIFICATIONS
 *
 *    Date              Description
 *    ----              -----------
 *    4/16/99          Original Code
 *
 */


/***************************************************************************/
/*
 *                              DEFINES
 */

  #define CYEQ_NO_ERROR           0x0000
  #define CYEQ_LAT_ERROR          0x0001
  #define CYEQ_LON_ERROR          0x0002
  #define CYEQ_EASTING_ERROR      0x0004
  #define CYEQ_NORTHING_ERROR     0x0008
  #define CYEQ_ORIGIN_LAT_ERROR   0x0010
  #define CYEQ_CENT_MER_ERROR     0x0020
  #define CYEQ_A_ERROR            0x0040
  #define CYEQ_INV_F_ERROR        0x0080


/***************************************************************************/
/*
 *                              FUNCTION PROTOTYPES
 *                                for CYLEQA.C
 */

/* ensure proper linkage to c++ programs */
  #ifdef __cplusplus
extern "C" {
  #endif

  long Set_Cyl_Eq_Area_Parameters(double a,
                                  double f,
                                  double Origin_Latitude,
                                  double Central_Meridian,
                                  double False_Easting,
                                  double False_Northing);
/*
 * The function Set_Cyl_Eq_Area_Parameters receives the ellipsoid parameters and
 * Cylindrical Equal Area projcetion parameters as inputs, and sets the corresponding 
 * state variables.  If any errors occur, the error code(s) are returned by the 
 * function, otherwise CYEQ_NO_ERROR is returned.
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


  void Get_Cyl_Eq_Area_Parameters(double *a,
                                  double *f,
                                  double *Origin_Latitude,
                                  double *Central_Meridian,
                                  double *False_Easting,
                                  double *False_Northing);
/*
 * The function Get_Cyl_Eq_Area_Parameters returns the current ellipsoid
 * parameters and Cylindrical Equal Area projection parameters.
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


  long Convert_Geodetic_To_Cyl_Eq_Area (double Latitude,
                                        double Longitude,
                                        double *Easting,
                                        double *Northing); 
/*
 * The function Convert_Geodetic_To_Cyl_Eq_Area converts geodetic (latitude and
 * longitude) coordinates to Cylindrical Equal Area projection easting, and northing
 * coordinates, according to the current ellipsoid and Cylindrical Equal Area projection
 * parameters.  If any errors occur, the error code(s) are returned by the
 * function, otherwise CYEQ_NO_ERROR is returned.
 *
 *    Latitude          : Latitude (phi) in radians           (input)
 *    Longitude         : Longitude (lambda) in radians       (input)
 *    Easting           : Easting (X) in meters               (output)
 *    Northing          : Northing (Y) in meters              (output)
 */


  long Convert_Cyl_Eq_Area_To_Geodetic(double Easting,
                                       double Northing,
                                       double *Latitude,
                                       double *Longitude);
/*
 * The function Convert_Cyl_Eq_Area_To_Geodetic converts Cylindrical Equal Area projection
 * easting and northing coordinates to geodetic (latitude and longitude)
 * coordinates, according to the current ellipsoid and Cylindrical Equal Area projection
 * coordinates.  If any errors occur, the error code(s) are returned by the
 * function, otherwise CYEQ_NO_ERROR is returned.
 *
 *    Easting           : Easting (X) in meters                  (input)
 *    Northing          : Northing (Y) in meters                 (input)
 *    Latitude          : Latitude (phi) in radians              (output)
 *    Longitude         : Longitude (lambda) in radians          (output)
 */

  #ifdef __cplusplus
}
  #endif

#endif /* CYLEQA_H */

