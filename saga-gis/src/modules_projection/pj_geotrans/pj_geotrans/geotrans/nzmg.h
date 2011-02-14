#ifndef NZMG_H
#define NZMG_H

/***************************************************************************/
/* RSC IDENTIFIER: NEW ZEALAND MAP GRID
 *
 * ABSTRACT
 *
 *    This component provides conversions between Geodetic coordinates 
 *    (latitude and longitude) and New Zealand Map Grid coordinates
 *    (easting and northing).
 *
 * ERROR HANDLING
 *
 *    This component checks parameters for valid values.  If an invalid value
 *    is found the error code is combined with the current error code using 
 *    the bitwise or.  This combining allows multiple error codes to be
 *    returned. The possible error codes are:
 *
 *       NZMG_NO_ERROR              : No errors occurred in function
 *       NZMG_LAT_ERROR             : Latitude outside of valid range
 *                                      (-33.5 to -48.5 degrees)
 *       NZMG_LON_ERROR             : Longitude outside of valid range
 *                                      (165.5 to 180.0 degrees)
 *       NZMG_EASTING_ERROR         : Easting outside of valid range
 *                                      (depending on ellipsoid and
 *                                       projection parameters)
 *       NZMG_NORTHING_ERROR        : Northing outside of valid range
 *                                      (depending on ellipsoid and
 *                                       projection parameters)
 *       NZMG_ELLIPSOID_ERROR       : Invalid ellipsoid - must be International
 *
 * REUSE NOTES
 *
 *    NEW ZEALAND MAP GRID is intended for reuse by any application that 
 *    performs a New Zealand Map Grid projection or its inverse.
 *    
 * REFERENCES
 *
 *    Further information on NEW ZEALAND MAP GRID can be found in the 
 *    Reuse Manual.
 *
 *    NEW ZEALAND MAP GRID originated from :  
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
 *    NEW ZEALAND MAP GRID has no restrictions.
 *
 * ENVIRONMENT
 *
 *    NEW ZEALAND MAP GRID was tested and certified in the following 
 *    environments:
 *
 *    1. Solaris 2.5 with GCC, version 2.8.1
 *    2. Windows 95 with MS Visual C++, version 6
 *
 * MODIFICATIONS
 *
 *    Date              Description
 *    ----              -----------
 *    09-14-00          Original Code
 *
 *
 */

/***************************************************************************/
/*
 *                              DEFINES
 */

#define NZMG_NO_ERROR           0x0000
#define NZMG_LAT_ERROR          0x0001
#define NZMG_LON_ERROR          0x0002
#define NZMG_EASTING_ERROR      0x0004
#define NZMG_NORTHING_ERROR     0x0008
#define NZMG_ELLIPSOID_ERROR    0x0010


/***************************************************************************/
/*
 *                              FUNCTION PROTOTYPES
 *                                for NZMG.C
 */

/* ensure proper linkage to c++ programs */
#ifdef __cplusplus
extern "C" {
#endif

long Set_NZMG_Parameters(char *Ellipsoid_Code);

/*
 * The function Set_NZMG_Parameters receives the ellipsoid code and sets
 * the corresponding state variables. If any errors occur, the error code(s)
 * are returned by the function, otherwise NZMG_NO_ERROR is returned.
 *
 *   Ellipsoid_Code : 2-letter code for ellipsoid           (input)
 */


void Get_NZMG_Parameters(char *Ellipsoid_Code);
                        
/*                         
 * The function Get_NZMG_Parameters returns the current ellipsoid
 * code.
 *
 *   Ellipsoid_Code : 2-letter code for ellipsoid          (output)
 */


long Convert_Geodetic_To_NZMG (double Latitude,
                               double Longitude,
                               double *Easting,
                               double *Northing);

/*
 * The function Convert_Geodetic_To_NZMG converts geodetic (latitude and
 * longitude) coordinates to New Zealand Map Grid projection (easting and northing)
 * coordinates, according to the current ellipsoid and New Zealand Map Grid 
 * projection parameters.  If any errors occur, the error code(s) are returned 
 * by the function, otherwise NZMG_NO_ERROR is returned.
 *
 *    Latitude          : Latitude (phi), in radians           (input)
 *    Longitude         : Longitude (lambda), in radians       (input)
 *    Easting           : Easting (X), in meters               (output)
 *    Northing          : Northing (Y), in meters              (output)
 */


long Convert_NZMG_To_Geodetic(double Easting,
                              double Northing,
                              double *Latitude,
                              double *Longitude);

/*
 * The function Convert_NZMG_To_Geodetic converts New Zealand Map Grid projection
 * (easting and northing) coordinates to geodetic (latitude and longitude)
 * coordinates, according to the current ellipsoid and New Zealand Map Grid projection
 * coordinates.  If any errors occur, the error code(s) are returned by the
 * function, otherwise NZMG_NO_ERROR is returned.
 *
 *    Easting           : Easting (X), in meters                  (input)
 *    Northing          : Northing (Y), in meters                 (input)
 *    Latitude          : Latitude (phi), in radians              (output)
 *    Longitude         : Longitude (lambda), in radians          (output)
 */


#ifdef __cplusplus
}
#endif

#endif /* NZMG_H */
