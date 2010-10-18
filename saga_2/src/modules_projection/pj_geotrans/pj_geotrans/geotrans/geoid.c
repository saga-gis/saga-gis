/* RSC IDENTIFIER:  Geoid
 *
 * ABSTRACT
 *
 *    The purpose of GEOID is to support conversions between WGS84 ellipsoid
 *    heights and WGS84 geoid heights.
 *
 *
 * ERROR HANDLING
 *
 *    This component checks parameters for valid values.  If an invalid value
 *    is found, the error code is combined with the current error code using 
 *    the bitwise or.  This combining allows multiple error codes to be
 *    returned. The possible error codes are:
 *
 *  GEOID_NO_ERROR               : No errors occured in function
 *  GEOID_FILE_OPEN_ERROR        : Geoid file opening error
 *  GEOID_INITIALIZE_ERROR       : Geoid seoaration database can not initialize
 *  GEOID_LAT_ERROR              : Latitude out of valid range
 *                                 (-90 to 90 degrees)
 *  GEOID_LON_ERROR              : Longitude out of valid range
 *                                 (-180 to 360 degrees)
 *
 * REUSE NOTES
 *
 *    Geoid is intended for reuse by any application that requires conversion
 *    between WGS84 ellipsoid heights and WGS84 geoid heights.
 *     
 * REFERENCES
 *
 *    Further information on Geoid can be found in the Reuse Manual.
 *
 *    Geoid originated from :  U.S. Army Topographic Engineering Center
 *                             Geospatial Information Division
 *                             7701 Telegraph Road
 *                             Alexandria, VA  22310-3864
 *
 * LICENSES
 *
 *    None apply to this component.
 *
 * RESTRICTIONS
 *
 *    Geoid has no restrictions.
 *
 * ENVIRONMENT
 *
 *    Geoid was tested and certified in the following environments
 *
 *    1. Solaris 2.5 with GCC 2.8.1
 *    2. MS Windows 95 with MS Visual C++ 6
 *
 *
 * MODIFICATIONS
 *
 *    Date              Description
 *    ----              -----------
 *    24-May-99         Original Code
 *
 */

/*                                  INCLUDES                                  */

#include "geoid.h"
#include <stdio.h>   /* standard C input/output library */
#include <math.h>    /* standard C math library */
#include <stdlib.h>  /* standard C general utility library */
#include <string.h>  /* standard C string handling library */

/*                                  DEFINES                                   */

#define NumbGeoidCols 1441   /* 360 degrees of longitude at 15 minute spacing */
#define NumbGeoidRows  721   /* 180 degrees of latitude  at 15 minute spacing */
#define NumbHeaderItems 6    /* min, max lat, min, max long, lat, long spacing*/
#define ScaleFactor     4    /* 4 grid cells per degree at 15 minute spacing  */
#define NumbGeoidElevs NumbGeoidCols * NumbGeoidRows
#define PI              3.14159265358979323e0

FILE  *GeoidHeightFile;

float GeoidHeightBuffer[NumbGeoidElevs];
static long  Geoid_Initialized = 0;  /* indicates successful initialization */

/* Function used to read from the geoid height file */
float Read_Geoid_Height ( int *NumRead );

/*                                 FUNCTIONS                                  */


long Initialize_Geoid_File(const char *File_Geoid)
/*
 * The function Initialize_Geoid reads geoid separation data from a file in
 * the current directory and builds the geoid separation table from it.  
 * If the separation file can not be found or accessed, an error code of 
 * GEOID_FILE_OPEN_ERROR is returned, If the separation file is incomplete 
 * or improperly formatted, an error code of GEOID_INITIALIZE_ERROR is returned, 
 * otherwise GEOID_NO_ERROR is returned.
 */
{
  int   ItemsRead = 0;
  long  ElevationsRead = 0;
  long  ItemsDiscarded = 0;
  long  num = 0;
  char  *PathName = getenv( "GEOID_DATA" );

  Geoid_Initialized	= 0;

/*  Open the File READONLY, or Return Error Condition:                        */

  if( File_Geoid == NULL || File_Geoid[0] == '\0' || (GeoidHeightFile = fopen(File_Geoid, "rb")) == NULL )
  {
	memset(GeoidHeightBuffer, 0, NumbGeoidElevs * sizeof(float));
	Geoid_Initialized	= 1;
	return( GEOID_NO_ERROR );
  }

/*  Skip the Header Line:                                                     */

  while ( num < NumbHeaderItems )
  {
    if (feof( GeoidHeightFile )) break;
    if (ferror( GeoidHeightFile )) break;
    GeoidHeightBuffer[num] = Read_Geoid_Height( &ItemsRead );
    ItemsDiscarded += ItemsRead;
    num++;
  }

/*  Determine if header read properly, or NOT:                                */

  if (GeoidHeightBuffer[0] !=  -90.0 ||
      GeoidHeightBuffer[1] !=   90.0 ||
      GeoidHeightBuffer[2] !=    0.0 ||
      GeoidHeightBuffer[3] !=  360.0 ||
      GeoidHeightBuffer[4] !=  ( 1.0 / ScaleFactor ) ||
      GeoidHeightBuffer[5] !=  ( 1.0 / ScaleFactor ) ||
      ItemsDiscarded != NumbHeaderItems)
  {
    fclose(GeoidHeightFile);
    return ( GEOID_INITIALIZE_ERROR );
  }

/*  Extract elements from the file:                                           */

  num = 0;
  while ( num < NumbGeoidElevs )
  {
    if (feof( GeoidHeightFile )) break;
    if (ferror( GeoidHeightFile )) break;
    GeoidHeightBuffer[num] = Read_Geoid_Height ( &ItemsRead );
    ElevationsRead += ItemsRead;
    num++;
  }

/*  Determine if all elevations of file read properly, or NOT:                */

  if (ElevationsRead != NumbGeoidElevs)
  {
    fclose(GeoidHeightFile);
    return ( GEOID_INITIALIZE_ERROR );
  }

  fclose(GeoidHeightFile);
  Geoid_Initialized = 1;
  return ( GEOID_NO_ERROR );
}


long Initialize_Geoid( )
/*
 * The function Initialize_Geoid reads geoid separation data from a file in
 * the current directory and builds the geoid separation table from it.  
 * If the separation file can not be found or accessed, an error code of 
 * GEOID_FILE_OPEN_ERROR is returned, If the separation file is incomplete 
 * or improperly formatted, an error code of GEOID_INITIALIZE_ERROR is returned, 
 * otherwise GEOID_NO_ERROR is returned.
 */
{
  int   ItemsRead = 0;
  long  ElevationsRead = 0;
  long  ItemsDiscarded = 0;
  long  num = 0;
  char  FileName[128];
  char  *PathName = getenv( "GEOID_DATA" );

  if (Geoid_Initialized)
  {
    return (GEOID_NO_ERROR);
  }

/*  Check the environment for a user provided path, else current directory;   */
/*  Build a File Name, including specified or default path:                   */

  if (PathName != NULL)
  {
    strcpy( FileName, PathName );
    strcat( FileName, "/" );
  }
  else
  {
    strcpy( FileName, "./" );
  }
  strcat( FileName, "egm96.grd" );

/*  Open the File READONLY, or Return Error Condition:                        */

  if ( ( GeoidHeightFile = fopen( FileName, "rb" ) ) == NULL)
  {
    return ( GEOID_FILE_OPEN_ERROR);
  }

/*  Skip the Header Line:                                                     */

  while ( num < NumbHeaderItems )
  {
    if (feof( GeoidHeightFile )) break;
    if (ferror( GeoidHeightFile )) break;
    GeoidHeightBuffer[num] = Read_Geoid_Height( &ItemsRead );
    ItemsDiscarded += ItemsRead;
    num++;
  }

/*  Determine if header read properly, or NOT:                                */

  if (GeoidHeightBuffer[0] !=  -90.0 ||
      GeoidHeightBuffer[1] !=   90.0 ||
      GeoidHeightBuffer[2] !=    0.0 ||
      GeoidHeightBuffer[3] !=  360.0 ||
      GeoidHeightBuffer[4] !=  ( 1.0 / ScaleFactor ) ||
      GeoidHeightBuffer[5] !=  ( 1.0 / ScaleFactor ) ||
      ItemsDiscarded != NumbHeaderItems)
  {
    fclose(GeoidHeightFile);
    return ( GEOID_INITIALIZE_ERROR );
  }

/*  Extract elements from the file:                                           */

  num = 0;
  while ( num < NumbGeoidElevs )
  {
    if (feof( GeoidHeightFile )) break;
    if (ferror( GeoidHeightFile )) break;
    GeoidHeightBuffer[num] = Read_Geoid_Height ( &ItemsRead );
    ElevationsRead += ItemsRead;
    num++;
  }

/*  Determine if all elevations of file read properly, or NOT:                */

  if (ElevationsRead != NumbGeoidElevs)
  {
    fclose(GeoidHeightFile);
    return ( GEOID_INITIALIZE_ERROR );
  }

  fclose(GeoidHeightFile);
  Geoid_Initialized = 1;
  return ( GEOID_NO_ERROR );
}


float Read_Geoid_Height ( int *NumRead )
/*
 * The private function Read_Geoid_Height returns the geoid height 
 * read from the geoid file. 4 bytes are read from the file and,
 * if necessary, the bytes are swapped.
 *
 *    NumRead             : Number of heights read from file         (output)
 *
 */
{
 	float result;
	char* swap = (char*)&result;
	char temp;
    *NumRead = fread( swap, 4, 1, GeoidHeightFile );
#ifdef LITTLE_ENDIAN
	temp = swap[0];
	swap[0] = swap[3];
	swap[3] = temp;
	temp = swap[1];
	swap[1] = swap[2];
	swap[2] = temp;
#endif
	return result;
}


long Get_Geoid_Height ( double Latitude,
                        double Longitude,
                        double *DeltaHeight )
/*
 * The private function Get_Geoid_Height returns the height of the 
 * WGS84 geiod above or below the WGS84 ellipsoid, 
 * at the specified geodetic coordinates,
 * using a grid of height adjustments from the EGM96 gravity model.
 *
 *    Latitude            : Geodetic latitude in radians           (input)
 *    Longitude           : Geodetic longitude in radians          (input)
 *    DeltaHeight         : Height Adjustment, in meters.          (output)
 *
 */
{
  long    Index;
  double DeltaX, DeltaY;
  double ElevationSE, ElevationSW, ElevationNE, ElevationNW;
  double LatitudeDD, LongitudeDD;
  double OffsetX, OffsetY;
  double PostX, PostY;
  double UpperY, LowerY;
  long Error_Code = 0;

  if (!Geoid_Initialized)
  {
    return (GEOID_NOT_INITIALIZED_ERROR);
  }
  if ((Latitude < -PI/2.0) || (Latitude > PI/2.0))
  { /* Latitude out of range */
    Error_Code |= GEOID_LAT_ERROR;
  }
  if ((Longitude < -PI) || (Longitude > (2*PI)))
  { /* Longitude out of range */
    Error_Code |= GEOID_LON_ERROR;
  }

  if (!Error_Code)
  { /* no errors */
    LatitudeDD  = Latitude  * 180.0 / PI;
    LongitudeDD = Longitude * 180.0 / PI;

    /*  Compute X and Y Offsets into Geoid Height Array:                          */

    if (LongitudeDD < 0.0)
    {
      OffsetX = ( LongitudeDD + 360.0 ) * ScaleFactor;
    }
    else
    {
      OffsetX = LongitudeDD * ScaleFactor;
    }
    OffsetY = ( 90.0 - LatitudeDD ) * ScaleFactor;

    /*  Find Four Nearest Geoid Height Cells for specified Latitude, Longitude;   */
    /*  Assumes that (0,0) of Geoid Height Array is at Northwest corner:          */

    PostX = floor( OffsetX );
    if ((PostX + 1) == NumbGeoidCols)
      PostX--;
    PostY = floor( OffsetY );
    if ((PostY + 1) == NumbGeoidRows)
      PostY--;

    Index = (long)(PostY * NumbGeoidCols + PostX);
    ElevationNW = GeoidHeightBuffer[ Index ];
    ElevationNE = GeoidHeightBuffer[ Index+ 1 ];

    Index = (long)((PostY + 1) * NumbGeoidCols + PostX);
    ElevationSW = GeoidHeightBuffer[ Index ];
    ElevationSE = GeoidHeightBuffer[ Index + 1 ];

    /*  Perform Bi-Linear Interpolation to compute Height above Ellipsoid:        */

    DeltaX = OffsetX - PostX;
    DeltaY = OffsetY - PostY;

    UpperY = ElevationNW + DeltaX * ( ElevationNE - ElevationNW );
    LowerY = ElevationSW + DeltaX * ( ElevationSE - ElevationSW );

    *DeltaHeight = LowerY + DeltaY * ( UpperY - LowerY );
  }
  return Error_Code;
}


long Convert_Ellipsoid_To_Geoid_Height ( double Latitude,
                                         double Longitude,
                                         double Ellipsoid_Height,
                                         double *Geoid_Height )
/*
 * The function Convert_Ellipsoid_To_Geoid_Height converts the specified WGS84
 * ellipsoid height at the specified geodetic coordinates to the equivalent
 * geoid height, using the EGM96 gravity model.
 *
 *    Latitude            : Geodetic latitude in radians           (input)
 *    Longitude           : Geodetic longitude in radians          (input)
 *    Ellipsoid_Height    : Ellipsoid height, in meters            (input)
 *    Geoid_Height        : Geoid height, in meters.               (output)
 *
 */
{
  double  DeltaHeight;
  long Error_Code;
  Error_Code = Get_Geoid_Height ( Latitude, Longitude, &DeltaHeight );
  *Geoid_Height = Ellipsoid_Height - DeltaHeight;
  return ( Error_Code );
}


long Convert_Geoid_To_Ellipsoid_Height ( double Latitude,
                                         double Longitude,
                                         double Geoid_Height,
                                         double *Ellipsoid_Height )
/*
 * The function Convert_Geoid_To_Ellipsoid_Height converts the specified WGS84
 * geoid height at the specified geodetic coordinates to the equivalent
 * ellipsoid height, using the EGM96 gravity model.
 *
 *    Latitude            : Geodetic latitude in radians           (input)
 *    Longitude           : Geodetic longitude in radians          (input)
 *    Ellipsoid_Height    : Ellipsoid height, in meters            (input)
 *    Geoid_Height        : Geoid height, in meters.               (output)
 *
 */
{
  double  DeltaHeight;
  long Error_Code;
  Error_Code = Get_Geoid_Height ( Latitude, Longitude, &DeltaHeight );
  *Ellipsoid_Height = Geoid_Height + DeltaHeight;
  return ( Error_Code );
}
