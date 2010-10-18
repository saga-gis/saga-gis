#ifndef GEOREF_H
#define GEOREF_H

/***************************************************************************/
/* RSC IDENTIFIER: GEOREF
 *
 * ABSTRACT
 *
 *    This component provides conversions from Geodetic coordinates (latitude
 *    and longitude in radians) to a GEOREF coordinate string.
 *
 * ERROR HANDLING
 *
 *    This component checks parameters for valid values.  If an invalid value
 *    is found, the error code is combined with the current error code using 
 *    the bitwise or.  This combining allows multiple error codes to be
 *    returned. The possible error codes are:
 *
 *          GEOREF_NO_ERROR          : No errors occurred in function
 *          GEOREF_LAT_ERROR         : Latitude outside of valid range 
 *                                      (-90 to 90 degrees)
 *          GEOREF_LON_ERROR         : Longitude outside of valid range
 *                                      (-180 to 360 degrees)
 *          GEOREF_STR_ERROR         : A GEOREF string error: string too long,
 *                                       string too short, or string length
 *                                       not even.
 *          GEOREF_STR_LAT_ERROR     : The latitude part of the GEOREF string
 *                                     (second or fourth character) is invalid.
 *          GEOREF_STR_LON_ERROR     : The longitude part of the GEOREF string
 *                                     (first or third character) is invalid.
 *          GEOREF_STR_LAT_MIN_ERROR : The latitude minute part of the GEOREF
 *                                      string is greater than 60.
 *          GEOREF_STR_LON_MIN_ERROR : The longitude minute part of the GEOREF
 *                                      string is greater than 60.
 *          GEOREF_PRECISION_ERROR   : The precision must be between 0 and 5 
 *                                      inclusive.
 *
 * REUSE NOTES
 *
 *    GEOREF is intended for reuse by any application that performs a 
 *    conversion between Geodetic and GEOREF coordinates.
 *    
 * REFERENCES
 *
 *    Further information on GEOREF can be found in the Reuse Manual.
 *
 *    GEOREF originated from :  U.S. Army Topographic Engineering Center
 *                              Geospatial Information Division
 *                              7701 Telegraph Road
 *                              Alexandria, VA  22310-3864
 *
 * LICENSES
 *
 *    None apply to this component.
 *
 * RESTRICTIONS
 *
 *    GEOREF has no restrictions.
 *
 * ENVIRONMENT
 *
 *    GEOREF was tested and certified in the following environments:
 *
 *    1. Solaris 2.5 with GCC version 2.8.1
 *    2. Windows 95 with MS Visual C++ version 6
 *
 * MODIFICATIONS
 *
 *    Date              Description
 *    ----              -----------
 *    20-02-97          Original Code
 */


/***************************************************************************/
/*
 *                               DEFINES
 */
#define GEOREF_NO_ERROR             0x0000
#define GEOREF_LAT_ERROR            0x0001
#define GEOREF_LON_ERROR            0x0002
#define GEOREF_STR_ERROR            0x0004
#define GEOREF_STR_LAT_ERROR        0x0008
#define GEOREF_STR_LON_ERROR        0x0010
#define GEOREF_STR_LAT_MIN_ERROR    0x0020
#define GEOREF_STR_LON_MIN_ERROR    0x0040
#define GEOREF_PRECISION_ERROR      0x0080


/***************************************************************************/
/*
 *                             FUNCTION PROTOTYPES
 *                                for georef.c
 */

/* ensure proper linkage to c++ programs */
#ifdef __cplusplus
extern "C" {
#endif


  long Convert_Geodetic_To_GEOREF (double Latitude,
                                   double Longitude,
                                   long Precision,
                                   char *Georef);
/*   
 *  This function converts Geodetic (latitude and longitude in radians)
 *  coordinates to a GEOREF coordinate string.  Precision specifies the
 *  number of digits in the GEOREF string for latitude and longitude:
 *                                  0 for nearest degree
 *                                  1 for nearest 10 minutes
 *                                  2 for nearest minute
 *                                  3 for nearest tenth of a minute
 *                                  4 for nearest hundredth of a minute
 *                                  5 for nearest thousandth of a minute
 *
 *    Latitude  : Latitude in radians                       (input)
 *    Longitude : Longitude in radians                      (input)
 *    Precision : level of precision specified by the user  (input)
 *    Georef    : GEOREF coordinate string                  (output)
 */


  long Convert_GEOREF_To_Geodetic (char *Georef,
                                   double *Latitude, 
                                   double *Longitude);
/*
 *  This function converts a GEOREF coordinate string to Geodetic (latitude
 *  and longitude in radians) coordinates.
 *
 *    Georef    : GEOREF coordinate string     (input)
 *    Latitude  : Latitude in radians          (output)
 *    Longitude : Longitude in radians         (output)
*/


#ifdef __cplusplus
}
#endif

#endif /* GEOREF_H */
