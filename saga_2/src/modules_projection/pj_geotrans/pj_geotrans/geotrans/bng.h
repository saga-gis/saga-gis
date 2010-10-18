#ifndef BNG_H
#define BNG_H

/***************************************************************************/
/* RSC IDENTIFIER: BRITISH NATIONAL GRID
 *
 * ABSTRACT
 *
 *    This component provides conversions between Geodetic coordinates 
 *    (latitude and longitude) and British National Grid coordinates.
 *
 * ERROR HANDLING
 *
 *    This component checks parameters for valid values.  If an invalid value
 *    is found the error code is combined with the current error code using 
 *    the bitwise or.  This combining allows multiple error codes to be
 *    returned. The possible error codes are:
 *
 *       BNG_NO_ERROR               : No errors occurred in function
 *       BNG_LAT_ERROR              : Latitude outside of valid range
 *                                      (49.5 to 61.5 degrees)
 *       BNG_LON_ERROR              : Longitude outside of valid range
 *                                      (-10.0 to 3.5 degrees)
 *       BNG_EASTING_ERROR          : Easting outside of valid range
 *                                      (depending on ellipsoid and
 *                                       projection parameters)
 *       BNG_NORTHING_ERROR         : Northing outside of valid range
 *                                      (depending on ellipsoid and
 *                                       projection parameters)
 *       BNG_STRING_ERROR           : A BNG string error: string too long,
 *                                      too short, or badly formed
 *       BNG_INVALID_AREA_ERROR     : Coordinate is outside of valid area
 *       BNG_ELLIPSOID_ERROR        : Invalid ellipsoid - must be Airy
 *
 * REUSE NOTES
 *
 *    BRITISH NATIONAL GRID is intended for reuse by any application that 
 *    performs a British National Grid projection or its inverse.
 *    
 * REFERENCES
 *
 *    Further information on BRITISH NATIONAL GRID can be found in the 
 *    Reuse Manual.
 *
 *    BRITISH NATIONAL GRID originated from :  
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
 *    BRITISH NATIONAL GRID has no restrictions.
 *
 * ENVIRONMENT
 *
 *    BRITISH NATIONAL GRID was tested and certified in the following 
 *    environments:
 *
 *    1. Solaris 2.5 with GCC, version 2.8.1
 *    2. Windows 95 with MS Visual C++, version 6
 *
 * MODIFICATIONS
 *
 *    Date              Description
 *    ----              -----------
 *    09-06-00          Original Code
 *
 *
 */

/***************************************************************************/
/*
 *                              DEFINES
 */

#define BNG_NO_ERROR           0x0000
#define BNG_LAT_ERROR          0x0001
#define BNG_LON_ERROR          0x0002
#define BNG_EASTING_ERROR      0x0004
#define BNG_NORTHING_ERROR     0x0008
#define BNG_INVALID_AREA_ERROR 0x0010
#define BNG_STRING_ERROR       0x0020
#define BNG_ELLIPSOID_ERROR    0x0040


/***************************************************************************/
/*
 *                              FUNCTION PROTOTYPES
 *                                for BNG.C
 */

/* ensure proper linkage to c++ programs */
#ifdef __cplusplus
extern "C" {
#endif


long Set_BNG_Parameters(char *Ellipsoid_Code);
                        
/*
 * The function Set_BNG_Parameters receives the ellipsoid code and sets
 * the corresponding state variables. If any errors occur, the error code(s)
 * are returned by the function, otherwise BNG_NO_ERROR is returned.
 *
 *   Ellipsoid_Code : 2-letter code for ellipsoid           (input)
 */


void Get_BNG_Parameters(char *Ellipsoid_Code);

/*                         
 * The function Get_BNG_Parameters returns the current ellipsoid
 * code.
 *
 *   Ellipsoid_Code : 2-letter code for ellipsoid           (output)
 */


long Convert_Geodetic_To_BNG (double Latitude,
                              double Longitude,
                              long Precision,
                              char* BNG);

/*
 * The function Convert_Geodetic_To_BNG converts geodetic (latitude and
 * longitude) coordinates to a BNG coordinate string, according to the 
 * current ellipsoid parameters.  If any errors occur, the error code(s)  
 * are returned by the function, otherwise BNG_NO_ERROR is returned.
 * 
 *    Latitude   : Latitude, in radians                    (input)
 *    Longitude  : Longitude, in radians                   (input)
 *    Precision  : Precision level of BNG string           (input)
 *    BNG        : British National Grid coordinate string (output)
 *  
 */


long Convert_BNG_To_Geodetic (char *BNG, 
                              double *Latitude, 
                              double *Longitude);

/*
 * The function Convert_BNG_To_Geodetic converts a BNG coordinate string 
 * to geodetic (latitude and longitude) coordinates, according to the current
 * ellipsoid parameters. If any errors occur, the error code(s) are returned 
 * by the function, otherwise BNG_NO_ERROR is returned. 
 * 
 *    BNG        : British National Grid coordinate string (input)
 *    Latitude   : Latitude, in radians                    (output)
 *    Longitude  : Longitude, in radians                   (output)
 *  
 */


long Convert_Transverse_Mercator_To_BNG(double Easting,
                                        double Northing,            
                                        long Precision,
                                        char *BNG);

/*
 * The function Convert_Transverse_Mercator_To_BNG converts Transverse Mercator
 * (easting and northing) coordinates to a BNG coordinate string, according
 * to the current ellipsoid parameters.  If any errors occur, the error code(s)
 * are returned by the function, otherwise BNG_NO_ERROR is returned.
 *
 *    Easting    : Easting (X), in meters                  (input)
 *    Northing   : Northing (Y), in meters                 (input)
 *    Precision  : Precision level of BNG string           (input)
 *    BNG        : British National Grid coordinate string (output)
 */


long Convert_BNG_To_Transverse_Mercator(char *BNG,
                                        double *Easting,
                                        double *Northing);

/*
 * The function Convert_BNG_To_Transverse_Mercator converts a BNG coordinate string
 * to Transverse Mercator projection (easting and northing) coordinates 
 * according to the current ellipsoid parameters.  If any errors occur, 
 * the error code(s) are returned by the function, otherwise BNG_NO_ERROR 
 * is returned.
 *
 *    BNG        : British National Grid coordinate string (input)
 *    Easting    : Easting (X), in meters                  (output)
 *    Northing   : Northing (Y), in meters                 (output)
 */


#ifdef __cplusplus
}
#endif

#endif /* BNG_H */
