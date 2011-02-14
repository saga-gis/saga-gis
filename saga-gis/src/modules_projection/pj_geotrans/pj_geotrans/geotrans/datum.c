/**********************************************************
 * Version $Id$
 *********************************************************/
/***************************************************************************/
/* RSC IDENTIFIER: Datum
 *
 * ABSTRACT
 *
 *    This component provides datum shifts for a large collection of local
 *    datums, WGS72, and WGS84.  A particular datum can be accessed by using its 
 *    standard 5-letter code to find its index in the datum table.  The index 
 *    can then be used to retrieve the name, type, ellipsoid code, and datum 
 *    shift parameters, and to perform shifts to or from that datum.
 *    
 *    By sequentially retrieving all of the datum codes and/or names, a menu
 *    of the available datums can be constructed.  The index values resulting
 *    from selections from this menu can then be used to access the parameters
 *    of the selected datum, or to perform datum shifts involving that datum.
 *
 *    This component supports both 3-parameter local datums, for which only X,
 *    Y, and Z translations relative to WGS 84 have been defined, and 
 *    7-parameter local datums, for which X, Y, and Z rotations, and a scale 
 *    factor, are also defined.  It also includes entries for WGS 84 (with an
 *    index of 0), and WGS 72 (with an index of 1), but no shift parameter 
 *    values are defined for these.
 *
 *    This component provides datum shift functions for both geocentric and
 *    geodetic coordinates.  WGS84 is used as an intermediate state when
 *    shifting from one local datum to another.  When geodetic coordinates are
 *    given Molodensky's method is used, except near the poles where the 3-step
 *    step method is used instead.  Specific algorithms are used for shifting 
 *    between WGS72 and WGS84.
 *
 *    This component depends on two data files, named 3_param.dat and 
 *    7_param.dat, which contain the datum parameter values.  Copies of these
 *    files must be located in the directory specified by the value of the 
 *    environment variable "DATUM_DATA", if defined, or else in the current 
 *    directory whenever a program containing this component is executed. 
 *
 *    Additional datums can be added to these files, either manually or using 
 *    the Create_Datum function.  However, if a large number of datums are 
 *    added, the datum table array sizes in this component will have to be 
 *    increased.
 *
 *    This component depends on two other components: the Ellipsoid component
 *    for access to ellipsoid parameters; and the Geocentric component for 
 *    conversions between geodetic and geocentric coordinates.
 *
 * ERROR HANDLING
 *
 *    This component checks for input file errors and input parameter errors.
 *    If an invalid value is found, the error code is combined with the current
 *    error code using the bitwise or.  This combining allows multiple error
 *    codes to be returned. The possible error codes are:
 *
 *  DATUM_NO_ERROR                  : No errors occurred in function
 *  DATUM_NOT_INITIALIZED_ERROR     : Datum module has not been initialized
 *  DATUM_7PARAM_FILE_OPEN_ERROR    : 7 parameter file opening error
 *  DATUM_7PARAM_FILE_PARSING_ERROR : 7 parameter file structure error
 *  DATUM_7PARAM_OVERFLOW_ERROR     : 7 parameter table overflow
 *  DATUM_3PARAM_FILE_OPEN_ERROR    : 3 parameter file opening error
 *  DATUM_3PARAM_FILE_PARSING_ERROR : 3 parameter file structure error
 *  DATUM_3PARAM_OVERFLOW_ERROR     : 3 parameter table overflow
 *  DATUM_INVALID_INDEX_ERROR       : Index out of valid range (less than one
 *                                      or more than Number_of_Datums)
 *  DATUM_INVALID_SRC_INDEX_ERROR   : Source datum index invalid
 *  DATUM_INVALID_DEST_INDEX_ERROR  : Destination datum index invalid
 *  DATUM_INVALID_CODE_ERROR        : Datum code not found in table
 *  DATUM_LAT_ERROR                 : Latitude out of valid range (-90 to 90)
 *  DATUM_LON_ERROR                 : Longitude out of valid range (-180 to
 *                                    360)
 *  DATUM_SIGMA_ERROR               : Standard error values must be positive
 *                                    (or -1 if unknown)
 *  DATUM_DOMAIN_ERROR              : Domain of validity not well defined
 *  DATUM_ELLIPSE_ERROR             : Error in ellipsoid module
 *  DATUM_NOT_USERDEF_ERROR         : Datum code is not user defined - cannot 
 *                                    be deleted
 *
 *
 * REUSE NOTES
 *
 *    Datum is intended for reuse by any application that needs access to 
 *    datum shift parameters relative to WGS 84.
 *
 *    
 * REFERENCES
 *
 *    Further information on Datum can be found in the Reuse Manual.
 *
 *    Datum originated from :  U.S. Army Topographic Engineering Center (USATEC)
 *                             Geospatial Information Division (GID)
 *                             7701 Telegraph Road
 *                             Alexandria, VA  22310-3864
 *
 * LICENSES
 *
 *    None apply to this component.
 *
 * RESTRICTIONS
 *
 *    Datum has no restrictions.
 *
 * ENVIRONMENT
 *
 *    Datum was tested and certified in the following environments:
 *
 *    1. Solaris 2.5 with GCC 2.8.1
 *    2. MS Windows 95 with MS Visual C++ 6
 *
 * MODIFICATIONS
 *
 *    Date              Description
 *    ----              -----------
 *    03/30/97          Original Code
 *    05/28/99          Added user-definable datums (for JMTK)
 *                      Added datum domain of validity checking (for JMTK)
 *                      Added datum shift accuracy calculation (for JMTK) 
 */


/***************************************************************************/
/*
 *                               INCLUDES
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "ellipse.h"
#include "geocent.h"
#include "datum.h"
/* 
 *    stdio.h    - standard C input/output library
 *    stdlib.h   - standard C general utilities library 
 *    string.h   - standard C string handling library
 *    ctype.h    - standard C character handling library
 *    math.h     - standard C mathematics library
 *    ellipse.h  - used to get ellipsoid parameters
 *    geocent.h  - used to convert between geodetic and geocentric coordinates
 *    datum.h    - for prototype error ehecking and error codes
 */


/***************************************************************************/
/*
 *                               DEFINES
 */

#define SECONDS_PER_RADIAN     206264.8062471;   /* Seconds in a radian */
#define PI                          3.14159265358979323e0
#define PI_OVER_2  (PI / 2.0)                 
#define TWO_PI     (2.0 * PI)                 
#define MIN_LAT                  (-PI/2.0)
#define MAX_LAT                  (+PI/2.0)
#define MIN_LON                   -PI
#define MAX_LON                  (2.0 * PI)
#define DATUM_CODE_LENGTH           7
#define DATUM_NAME_LENGTH          33
#define ELLIPSOID_CODE_LENGTH       3
#define MAX_7PARAM                 25
#define MAX_3PARAM                250
#define MOLODENSKY_MAX            (89.75 * PI / 180.0) /* Polar limit */
#define FILENAME_LENGTH           128
#define FALSE 0
#define TRUE  1


/***************************************************************************/
/*
 *                          GLOBAL DECLARATIONS
 */

typedef struct Datum_Table_Row
{
  Datum_Type Type;
  char Code[DATUM_CODE_LENGTH];
  char Name[DATUM_NAME_LENGTH];
  char Ellipsoid_Code[ELLIPSOID_CODE_LENGTH];
  double Parameters[7];  /* interface for 3 and 7 Parameters */
  double Sigma_X;        /* standard error along X axis */
  double Sigma_Y;        /* standard error along Y axis */
  double Sigma_Z;        /* standard error along Z axis */
  double West_longitude; /* western boundary of validity rectangle */
  double East_longitude; /* eastern boundary of validity rectangle */
  double South_latitude; /* southern boundary of validity rectangle */
  double North_latitude; /* northern boundary of validity rectangle */
  long User_Defined;      /* Identifies a user defined datum */
} Datum_Row; /* defines a single entry in the datum table */

/* World Geodetic Systems */
static Datum_Row WGS84;
static Datum_Row WGS72;
const char *WGS84_Datum_Code = "WGE";
const char *WGS72_Datum_Code = "WGC";
static long Datum_WGS84_Index = 1; /* Index of WGS84 in datum table */
static long Datum_WGS72_Index = 2; /* Index of WGS72 in datum table */

static Datum_Row Datum_Table_3Param[MAX_3PARAM]; /* Array of 3Param datums */
static Datum_Row Datum_Table_7Param[MAX_7PARAM]; /* Array of 7Param datums */
static Datum_Row *Datum_Table[2 + MAX_3PARAM + MAX_7PARAM]; /* Datum pointer array, for sorting */
static long Datum_3Param_Count = 0;
static long Datum_7Param_Count = 0;
static long Number_of_Datums = 0; /* Count for datum table */

static long Datum_Initialized = 0; /* Indicates successful initialization */


/***************************************************************************/
/*
 *                              FUNCTIONS     
 */
/*Forward function declarations */
void Geodetic_Shift_WGS84_To_WGS72( const double WGS84_Lat,
                                    const double WGS84_Lon,
                                    const double WGS84_Hgt,
                                    double *WGS72_Lat,
                                    double *WGS72_Lon,
                                    double *WGS72_Hgt);

void Geodetic_Shift_WGS72_To_WGS84( const double WGS72_Lat,
                                    const double WGS72_Lon,
                                    const double WGS72_Hgt,
                                    double *WGS84_Lat,
                                    double *WGS84_Lon,
                                    double *WGS84_Hgt);

/* Index into 3 Param table function declaration */
long Datum_3Param_Index( const char *Code,
                         long *Index );

void Assign_Datum_Row(Datum_Row *destination, Datum_Row *source)
{ /* Begin Assign_Datum_Row */
  /*
   * The function Assign_Datum_Row copies the data from source into destination.
   *
   * destination      : Pointer to the destination datum container.   (output)
   * source           : Pointer to the source datum container.        (input)
   */
  long i = 0;

  destination->Type = source->Type;
  strcpy(destination->Code, source->Code);
  strcpy(destination->Name, source->Name);
  strcpy(destination->Ellipsoid_Code, source->Ellipsoid_Code);

  for (i = 0; i < 7; i++)
  {
    destination->Parameters[i] = source->Parameters[i];
  }

  destination->Sigma_X = source->Sigma_X;
  destination->Sigma_Y = source->Sigma_Y;
  destination->Sigma_Z = source->Sigma_Z;

  destination->User_Defined = source->User_Defined;

} /* End Assign_Datum_Row */


long Initialize_Datums_File(const char *File_7Parms, const char *File_3Parms)
{ /* Begin Initialize_Datums */
/*
 * The function Initialize_Datums creates the datum table from two external
 * files.  If an error occurs, the initialization stops and an error code is
 * returned.  This function must be called before any of the other functions
 * in this component.
 */
  long index = 0, i = 0;
  FILE *fp_7param = NULL;
  FILE *fp_3param = NULL;
  long error_code = DATUM_NO_ERROR;

  Datum_Initialized	= 0;

  //-------------------------------------------------------
  /*  Open the File READONLY, or Return Error Condition:                        */
  if( File_7Parms && File_7Parms[0] != '\0' && (fp_7param = fopen(File_7Parms, "r")) != NULL )
  {
    while ((!feof(fp_7param)) && (!error_code))
    {
      if (index < MAX_7PARAM)
      { /* build 7-parameter datum table entries */
        if (fscanf(fp_7param, "%s ", Datum_Table_7Param[index].Code) <= 0)
          error_code |= DATUM_7PARAM_FILE_PARSING_ERROR;
        if (fscanf(fp_7param, "\"%32[^\"]\"", Datum_Table_7Param[index].Name) <= 0)
          Datum_Table_7Param[index].Name[0] = '\0';
        if (fscanf(fp_7param, " %s %lf %lf %lf %lf %lf %lf %lf ", 
                   Datum_Table_7Param[index].Ellipsoid_Code,
                   &(Datum_Table_7Param[index].Parameters[0]),
                   &(Datum_Table_7Param[index].Parameters[1]),
                   &(Datum_Table_7Param[index].Parameters[2]),
                   &(Datum_Table_7Param[index].Parameters[3]),
                   &(Datum_Table_7Param[index].Parameters[4]),
                   &(Datum_Table_7Param[index].Parameters[5]),
                   &(Datum_Table_7Param[index].Parameters[6])) <= 0)
        {
          error_code |= DATUM_7PARAM_FILE_PARSING_ERROR;
        }
        else
        { /* convert from degrees to radians */
          Datum_Table_7Param[index].Type = Seven_Param_Datum;
          Datum_Table_7Param[index].Parameters[3] /= SECONDS_PER_RADIAN;
          Datum_Table_7Param[index].Parameters[4] /= SECONDS_PER_RADIAN;
          Datum_Table_7Param[index].Parameters[5] /= SECONDS_PER_RADIAN;
          Datum_Table_7Param[index].Sigma_X = 0.0;
          Datum_Table_7Param[index].Sigma_Y = 0.0;
          Datum_Table_7Param[index].Sigma_Z = 0.0;
          Datum_Table_7Param[index].South_latitude = -PI / 2.0;
          Datum_Table_7Param[index].North_latitude = +PI / 2.0;
          Datum_Table_7Param[index].West_longitude = -PI;
          Datum_Table_7Param[index].East_longitude = +PI;

        }
        index++;
      }
      else
      {
        error_code |= DATUM_7PARAM_OVERFLOW_ERROR;
      }
    }
    fclose(fp_7param);
    Datum_7Param_Count = index;
    index = 0;
  }
  else
  {
	strcpy(Datum_Table_7Param[index].Code, "EUR-7");
	strcpy(Datum_Table_7Param[index].Name, "EUROPEAN 1950, Mean (7 Param)");
//	Datum_Table_7Param[index].Name[0] = '\0';
	strcpy(Datum_Table_7Param[index].Ellipsoid_Code, "IN");
	Datum_Table_7Param[index].Parameters[0]		= -102;
	Datum_Table_7Param[index].Parameters[1]		= -102;
	Datum_Table_7Param[index].Parameters[2]		= -129;
	Datum_Table_7Param[index].Parameters[3]		=  0.413 / SECONDS_PER_RADIAN;
	Datum_Table_7Param[index].Parameters[4]		= -0.184 / SECONDS_PER_RADIAN;
	Datum_Table_7Param[index].Parameters[5]		=  0.385 / SECONDS_PER_RADIAN;
	Datum_Table_7Param[index].Parameters[6]		=  0.0000024664;
	Datum_Table_7Param[index].Type				= Seven_Param_Datum;
	Datum_Table_7Param[index].Sigma_X			= 0.0;
	Datum_Table_7Param[index].Sigma_Y			= 0.0;
	Datum_Table_7Param[index].Sigma_Z			= 0.0;
	Datum_Table_7Param[index].South_latitude	= -PI / 2.0;
	Datum_Table_7Param[index].North_latitude	= +PI / 2.0;
	Datum_Table_7Param[index].West_longitude	= -PI;
	Datum_Table_7Param[index].East_longitude	= +PI;
    index++;
	Datum_7Param_Count = index;
	index = 0;
  }

  //-------------------------------------------------------
  /*  Open the File READONLY, or Return Error Condition:                        */
  if( File_3Parms && File_3Parms[0] != '\0' && (fp_3param = fopen(File_3Parms, "r")) != NULL )
  {
    while ((!feof(fp_3param)) && (!error_code))
    {
      if (index < MAX_3PARAM)
      { /* build 3-parameter datum table entries */
        if (fscanf(fp_3param, "%s ", Datum_Table_3Param[index].Code) <= 0)
          error_code |= DATUM_3PARAM_FILE_PARSING_ERROR;
		else
		{
		  if (Datum_Table_3Param[index].Code[0] == '*')
		  {
			long i;
			Datum_Table_3Param[index].User_Defined = TRUE;
		    for (i = 0; i < DATUM_CODE_LENGTH; i++)
			  Datum_Table_3Param[index].Code[i] = Datum_Table_3Param[index].Code[i+1];
		  }
		  else
		    Datum_Table_3Param[index].User_Defined = FALSE;
		}
        if (fscanf(fp_3param, "\"%32[^\"]\"", Datum_Table_3Param[index].Name) <= 0)
          Datum_Table_3Param[index].Name[0] = '\0';
        if (fscanf(fp_3param, " %s %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf ",
                   Datum_Table_3Param[index].Ellipsoid_Code,
                   &(Datum_Table_3Param[index].Parameters[0]),
                   &(Datum_Table_3Param[index].Sigma_X),
                   &(Datum_Table_3Param[index].Parameters[1]),
                   &(Datum_Table_3Param[index].Sigma_Y),
                   &(Datum_Table_3Param[index].Parameters[2]),
                   &(Datum_Table_3Param[index].Sigma_Z),
                   &(Datum_Table_3Param[index].South_latitude),
                   &(Datum_Table_3Param[index].North_latitude),
                   &(Datum_Table_3Param[index].West_longitude),
                   &(Datum_Table_3Param[index].East_longitude)) <= 0)
        {
          error_code |= DATUM_3PARAM_FILE_PARSING_ERROR;
        }
        else
        {
          Datum_Table_3Param[index].Parameters[3] = 0.0;
          Datum_Table_3Param[index].Parameters[4] = 0.0;
          Datum_Table_3Param[index].Parameters[5] = 0.0;
          Datum_Table_3Param[index].Parameters[6] = 1.0;
          Datum_Table_3Param[index].South_latitude *= (PI / 180.0);
          Datum_Table_3Param[index].North_latitude *= (PI / 180.0);
          Datum_Table_3Param[index].West_longitude *= (PI / 180.0);
          Datum_Table_3Param[index].East_longitude *= (PI / 180.0);
          Datum_Table_3Param[index].Type = Three_Param_Datum;
        }
        index++;
      }
      else
      {
        error_code |= DATUM_3PARAM_OVERFLOW_ERROR;
      }
    }
    fclose(fp_3param);
    Datum_3Param_Count = index;
  }
  else
  {
	strcpy(Datum_Table_3Param[index].Code, "EUR-M");
	Datum_Table_3Param[index].User_Defined		= FALSE;
    strcpy(Datum_Table_3Param[index].Name, "EUROPEAN 1950, Mean (3 Param)");
//	Datum_Table_3Param[index].Name[0]			= '\0';
	strcpy(Datum_Table_3Param[index].Ellipsoid_Code, "IN");
	Datum_Table_3Param[index].Parameters[0]		= -87;
	Datum_Table_3Param[index].Sigma_X			=  3;
	Datum_Table_3Param[index].Parameters[1]		= -98;
	Datum_Table_3Param[index].Sigma_Y			=  8;
	Datum_Table_3Param[index].Parameters[2]		= -121;
	Datum_Table_3Param[index].Sigma_Z			=  5;
	Datum_Table_3Param[index].South_latitude	=  30 * (PI / 180.0);
	Datum_Table_3Param[index].North_latitude	=  80 * (PI / 180.0);
	Datum_Table_3Param[index].West_longitude	=   5 * (PI / 180.0);
	Datum_Table_3Param[index].East_longitude	=  33 * (PI / 180.0);
	Datum_Table_3Param[index].Parameters[3]		= 0.0;
	Datum_Table_3Param[index].Parameters[4]		= 0.0;
	Datum_Table_3Param[index].Parameters[5]		= 0.0;
	Datum_Table_3Param[index].Parameters[6]		= 1.0;
	Datum_Table_3Param[index].Type				= Three_Param_Datum;
	index++;
	Datum_3Param_Count = index;
  }

  //-------------------------------------------------------
  if (!error_code)
  {
    /* Initialize array of pointers to datums */
    if (!error_code)
    {
      /* set total number of datums available */
      Number_of_Datums = Datum_3Param_Count + Datum_7Param_Count + 2;

      /* build WGS84 and WGS72 datum table entries */
      WGS84.Type = WGS84_Datum;
      strcpy(WGS84.Name,"World Geodetic System 1984");
      strcpy(WGS84.Code,"WGE");
      strcpy(WGS84.Ellipsoid_Code,"WE");
      WGS72.Type = WGS72_Datum;
      strcpy(WGS72.Name,"World Geodetic System 1972");
      strcpy(WGS72.Code,"WGC");
      strcpy(WGS72.Ellipsoid_Code,"WD");
      for (i=0; i<6; i++)
      {
        WGS84.Parameters[i] = 0.0;
        WGS72.Parameters[i] = 0.0;
      }
      WGS84.Parameters[6] = 1.0;
      WGS72.Parameters[6] = 1.0;

      WGS84.Sigma_X = 0.0;
      WGS84.Sigma_Y = 0.0;
      WGS84.Sigma_Z = 0.0;
      WGS84.South_latitude = -PI / 2.0;
      WGS84.North_latitude = +PI / 2.0;
      WGS84.West_longitude = -PI;
      WGS84.East_longitude = +PI;

      WGS72.Sigma_X = 0.0;
      WGS72.Sigma_Y = 0.0;
      WGS72.Sigma_Z = 0.0;
      WGS72.South_latitude = -PI / 2.0;
      WGS72.North_latitude = +PI / 2.0;
      WGS72.West_longitude = -PI;
      WGS72.East_longitude = +PI;

      Datum_WGS84_Index = 1;
      Datum_Table[Datum_WGS84_Index - 1] = &WGS84;
      Datum_WGS72_Index = 2;
      Datum_Table[Datum_WGS72_Index - 1] = &WGS72;
      index = 2;
      for (i = 0; i < Datum_7Param_Count; i++)
      {
        Datum_Table[index++] = &(Datum_Table_7Param[i]);
      }
      for (i = 0; i < Datum_3Param_Count; i++)
      {
        Datum_Table[index++] = &(Datum_Table_3Param[i]);
      }

      if (error_code)
      {
        error_code |= DATUM_ELLIPSE_ERROR;
        Datum_Initialized = 0;
        Number_of_Datums = 0;
      }
      else
        Datum_Initialized = 1;      
    }
  }
  return (error_code);
} /* End Initialize_Datums_File */



long Initialize_Datums(void)
{ /* Begin Initialize_Datums */
/*
 * The function Initialize_Datums creates the datum table from two external
 * files.  If an error occurs, the initialization stops and an error code is
 * returned.  This function must be called before any of the other functions
 * in this component.
 */
  long index = 0, i = 0;
  char *PathName = NULL;
  char FileName7[FILENAME_LENGTH];
  FILE *fp_7param = NULL;
  FILE *fp_3param = NULL;
  char FileName3[FILENAME_LENGTH];
  long error_code = DATUM_NO_ERROR;

  if (Datum_Initialized)
  {
    return (error_code);
  }

  /*  Check the environment for a user provided path, else current directory;   */
  /*  Build a File Name, including specified or default path:                   */

  PathName = getenv( "DATUM_DATA" );
  if (PathName != NULL)
  {
    strcpy( FileName7, PathName );
    strcat( FileName7, "/" );
  }
  else
  {
    strcpy( FileName7, "./" );
  }
  strcat( FileName7, "7_param.dat" );

  /*  Open the File READONLY, or Return Error Condition:                        */

  if (( fp_7param = fopen( FileName7, "r" ) ) == NULL)
  {
    return ( DATUM_7PARAM_FILE_OPEN_ERROR);
  }

  if (PathName != NULL)
  {
    strcpy( FileName3, PathName );
    strcat( FileName3, "/" );
  }
  else
  {
    strcpy( FileName3, "./" );
  }
  strcat( FileName3, "3_param.dat" );

  /*  Open the File READONLY, or Return Error Condition:                        */

  if (( fp_3param = fopen( FileName3, "r" ) ) == NULL)
  {
    return ( DATUM_3PARAM_FILE_OPEN_ERROR);
  }

  if (!error_code)
  {
    while ((!feof(fp_7param)) && (!error_code))
    {
      if (index < MAX_7PARAM)
      { /* build 7-parameter datum table entries */
        if (fscanf(fp_7param, "%s ", Datum_Table_7Param[index].Code) <= 0)
          error_code |= DATUM_7PARAM_FILE_PARSING_ERROR;
        if (fscanf(fp_7param, "\"%32[^\"]\"", Datum_Table_7Param[index].Name) <= 0)
          Datum_Table_7Param[index].Name[0] = '\0';
        if (fscanf(fp_7param, " %s %lf %lf %lf %lf %lf %lf %lf ", 
                   Datum_Table_7Param[index].Ellipsoid_Code,
                   &(Datum_Table_7Param[index].Parameters[0]),
                   &(Datum_Table_7Param[index].Parameters[1]),
                   &(Datum_Table_7Param[index].Parameters[2]),
                   &(Datum_Table_7Param[index].Parameters[3]),
                   &(Datum_Table_7Param[index].Parameters[4]),
                   &(Datum_Table_7Param[index].Parameters[5]),
                   &(Datum_Table_7Param[index].Parameters[6])) <= 0)
        {
          error_code |= DATUM_7PARAM_FILE_PARSING_ERROR;
        }
        else
        { /* convert from degrees to radians */
          Datum_Table_7Param[index].Type = Seven_Param_Datum;
          Datum_Table_7Param[index].Parameters[3] /= SECONDS_PER_RADIAN;
          Datum_Table_7Param[index].Parameters[4] /= SECONDS_PER_RADIAN;
          Datum_Table_7Param[index].Parameters[5] /= SECONDS_PER_RADIAN;
          Datum_Table_7Param[index].Sigma_X = 0.0;
          Datum_Table_7Param[index].Sigma_Y = 0.0;
          Datum_Table_7Param[index].Sigma_Z = 0.0;
          Datum_Table_7Param[index].South_latitude = -PI / 2.0;
          Datum_Table_7Param[index].North_latitude = +PI / 2.0;
          Datum_Table_7Param[index].West_longitude = -PI;
          Datum_Table_7Param[index].East_longitude = +PI;

        }
        index++;
      }
      else
      {
        error_code |= DATUM_7PARAM_OVERFLOW_ERROR;
      }
    }
    fclose(fp_7param);
    Datum_7Param_Count = index;
    index = 0;

    while ((!feof(fp_3param)) && (!error_code))
    {
      if (index < MAX_3PARAM)
      { /* build 3-parameter datum table entries */
        if (fscanf(fp_3param, "%s ", Datum_Table_3Param[index].Code) <= 0)
          error_code |= DATUM_3PARAM_FILE_PARSING_ERROR;
		else
		{
		  if (Datum_Table_3Param[index].Code[0] == '*')
		  {
			long i;
			Datum_Table_3Param[index].User_Defined = TRUE;
		    for (i = 0; i < DATUM_CODE_LENGTH; i++)
			  Datum_Table_3Param[index].Code[i] = Datum_Table_3Param[index].Code[i+1];
		  }
		  else
		    Datum_Table_3Param[index].User_Defined = FALSE;
		}
        if (fscanf(fp_3param, "\"%32[^\"]\"", Datum_Table_3Param[index].Name) <= 0)
          Datum_Table_3Param[index].Name[0] = '\0';
        if (fscanf(fp_3param, " %s %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf ",
                   Datum_Table_3Param[index].Ellipsoid_Code,
                   &(Datum_Table_3Param[index].Parameters[0]),
                   &(Datum_Table_3Param[index].Sigma_X),
                   &(Datum_Table_3Param[index].Parameters[1]),
                   &(Datum_Table_3Param[index].Sigma_Y),
                   &(Datum_Table_3Param[index].Parameters[2]),
                   &(Datum_Table_3Param[index].Sigma_Z),
                   &(Datum_Table_3Param[index].South_latitude),
                   &(Datum_Table_3Param[index].North_latitude),
                   &(Datum_Table_3Param[index].West_longitude),
                   &(Datum_Table_3Param[index].East_longitude)) <= 0)
        {
          error_code |= DATUM_3PARAM_FILE_PARSING_ERROR;
        }
        else
        {
          Datum_Table_3Param[index].Parameters[3] = 0.0;
          Datum_Table_3Param[index].Parameters[4] = 0.0;
          Datum_Table_3Param[index].Parameters[5] = 0.0;
          Datum_Table_3Param[index].Parameters[6] = 1.0;
          Datum_Table_3Param[index].South_latitude *= (PI / 180.0);
          Datum_Table_3Param[index].North_latitude *= (PI / 180.0);
          Datum_Table_3Param[index].West_longitude *= (PI / 180.0);
          Datum_Table_3Param[index].East_longitude *= (PI / 180.0);
          Datum_Table_3Param[index].Type = Three_Param_Datum;
        }
        index++;
      }
      else
      {
        error_code |= DATUM_3PARAM_OVERFLOW_ERROR;
      }
    }
    fclose(fp_3param);
    Datum_3Param_Count = index;

    /* Initialize array of pointers to datums */
    if (!error_code)
    {
      /* set total number of datums available */
      Number_of_Datums = Datum_3Param_Count + Datum_7Param_Count + 2;

      /* build WGS84 and WGS72 datum table entries */
      WGS84.Type = WGS84_Datum;
      strcpy(WGS84.Name,"World Geodetic System 1984");
      strcpy(WGS84.Code,"WGE");
      strcpy(WGS84.Ellipsoid_Code,"WE");
      WGS72.Type = WGS72_Datum;
      strcpy(WGS72.Name,"World Geodetic System 1972");
      strcpy(WGS72.Code,"WGC");
      strcpy(WGS72.Ellipsoid_Code,"WD");
      for (i=0; i<6; i++)
      {
        WGS84.Parameters[i] = 0.0;
        WGS72.Parameters[i] = 0.0;
      }
      WGS84.Parameters[6] = 1.0;
      WGS72.Parameters[6] = 1.0;

      WGS84.Sigma_X = 0.0;
      WGS84.Sigma_Y = 0.0;
      WGS84.Sigma_Z = 0.0;
      WGS84.South_latitude = -PI / 2.0;
      WGS84.North_latitude = +PI / 2.0;
      WGS84.West_longitude = -PI;
      WGS84.East_longitude = +PI;

      WGS72.Sigma_X = 0.0;
      WGS72.Sigma_Y = 0.0;
      WGS72.Sigma_Z = 0.0;
      WGS72.South_latitude = -PI / 2.0;
      WGS72.North_latitude = +PI / 2.0;
      WGS72.West_longitude = -PI;
      WGS72.East_longitude = +PI;

      Datum_WGS84_Index = 1;
      Datum_Table[Datum_WGS84_Index - 1] = &WGS84;
      Datum_WGS72_Index = 2;
      Datum_Table[Datum_WGS72_Index - 1] = &WGS72;
      index = 2;
      for (i = 0; i < Datum_7Param_Count; i++)
      {
        Datum_Table[index++] = &(Datum_Table_7Param[i]);
      }
      for (i = 0; i < Datum_3Param_Count; i++)
      {
        Datum_Table[index++] = &(Datum_Table_3Param[i]);
      }

      if (error_code)
      {
        error_code |= DATUM_ELLIPSE_ERROR;
        Datum_Initialized = 0;
        Number_of_Datums = 0;
      }
      else
        Datum_Initialized = 1;      
    }
  }
  return (error_code);
} /* End Initialize_Datums */


long Create_Datum ( const char *Code,
                    const char *Name,
                    const char *Ellipsoid_Code,
                    double Delta_X,
                    double Delta_Y,
                    double Delta_Z,
                    double Sigma_X,
                    double Sigma_Y,
                    double Sigma_Z,
                    double South_latitude,
                    double North_latitude,
                    double West_longitude,
                    double East_longitude)
{ /* Begin Create_Datum */
/*
 *   Code           : 5-letter new datum code.                      (input)
 *   Name           : Name of the new datum                         (input)
 *   Ellipsoid_Code : 2-letter code for the associated ellipsoid    (input)
 *   Delta_X        : X translation to WGS84 in meters              (input)
 *   Delta_Y        : Y translation to WGS84 in meters              (input)
 *   Delta_Z        : Z translation to WGS84 in meters              (input)
 *   Sigma_X        : Standard error in X in meters                 (input)
 *   Sigma_Y        : Standard error in Y in meters                 (input)
 *   Sigma_Z        : Standard error in Z in meters                 (input)
 *   South_latitude : Southern edge of validity rectangle in radians(input)
 *   North_latitude : Northern edge of validity rectangle in radians(input)
 *   West_longitude : Western edge of validity rectangle in radians (input)
 *   East_longitude : Eastern edge of validity rectangle in radians (input)
 *
 * The function Create_Datum creates a new local (3-parameter) datum with the 
 * specified code, name, and axes.  If the datum table has not been initialized,
 * the specified code is already in use, or a new version of the 3-param.dat 
 * file cannot be created, an error code is returned, otherwise DATUM_NO_ERROR 
 * is returned.  Note that the indexes of all datums in the datum table may be 
 * changed by this function.
 */
  char datum_Code[DATUM_CODE_LENGTH];
  long error_code = DATUM_NO_ERROR;
  long index = 0;
  long ellipsoid_index = 0;
  long code_length = 0;
  char datum_name[DATUM_NAME_LENGTH+2];
  char *PathName = NULL;
  char FileName[FILENAME_LENGTH];
  FILE *fp_3param = NULL;

  if (!Datum_Initialized)
    error_code |= DATUM_NOT_INITIALIZED_ERROR;
  if (!(Datum_3Param_Count < MAX_3PARAM))
    error_code |= DATUM_3PARAM_OVERFLOW_ERROR;
  if (!(((Sigma_X > 0.0) || (Sigma_X == -1.0)) &&
        ((Sigma_Y > 0.0) || (Sigma_Y == -1.0)) &&
        ((Sigma_Z > 0.0) || (Sigma_Z == -1.0))))
    error_code |= DATUM_SIGMA_ERROR;
  if ((South_latitude < MIN_LAT) || (South_latitude > MAX_LAT))
    error_code |= DATUM_LAT_ERROR;
  if ((West_longitude < MIN_LON) || (West_longitude > MAX_LON))
    error_code |= DATUM_LON_ERROR;
  if ((North_latitude < MIN_LAT) || (North_latitude > MAX_LAT))
    error_code |= DATUM_LAT_ERROR;
  if ((East_longitude < MIN_LON) || (East_longitude > MAX_LON))
    error_code |= DATUM_LON_ERROR;
  if ((South_latitude >= North_latitude) || (West_longitude >= East_longitude))
    error_code |= DATUM_DOMAIN_ERROR;
  code_length = strlen(Code);
  if ((!Datum_Index(Code, &index)) || (code_length > (DATUM_CODE_LENGTH-1)))
    error_code |= DATUM_INVALID_CODE_ERROR;
  if (Ellipsoid_Index(Ellipsoid_Code, &ellipsoid_index))
    error_code |= DATUM_ELLIPSE_ERROR;
  if (!error_code)
  {
    long i;
    strcpy(datum_Code,Code);
    /* Convert code to upper case */
    for (i = 0; i < code_length; i++)
      datum_Code[i] = (char)toupper(datum_Code[i]);
    index = Datum_3Param_Count;
    strcpy(Datum_Table_3Param[index].Code, datum_Code);
    strcpy(Datum_Table_3Param[index].Name, Name);
    strcpy(Datum_Table_3Param[index].Ellipsoid_Code, Ellipsoid_Code);
    Datum_Table_3Param[index].Parameters[0] = Delta_X;
    Datum_Table_3Param[index].Parameters[1] = Delta_Y;
    Datum_Table_3Param[index].Parameters[2] = Delta_Z;
    Datum_Table_3Param[index].Parameters[3] = 0.0;
    Datum_Table_3Param[index].Parameters[4] = 0.0;
    Datum_Table_3Param[index].Parameters[5] = 0.0;
    Datum_Table_3Param[index].Parameters[6] = 1.0;
    Datum_Table_3Param[index].Sigma_X = Sigma_X;
    Datum_Table_3Param[index].Sigma_Y = Sigma_Y;
    Datum_Table_3Param[index].Sigma_Z = Sigma_Z;
    Datum_Table_3Param[index].South_latitude = South_latitude;
    Datum_Table_3Param[index].North_latitude = North_latitude;
    Datum_Table_3Param[index].West_longitude = West_longitude;
    Datum_Table_3Param[index].East_longitude = East_longitude;
    Datum_Table_3Param[index].Type = Three_Param_Datum;
    Datum_Table_3Param[index].User_Defined = TRUE;
    Datum_Table[Number_of_Datums] = &(Datum_Table_3Param[index]);
    Datum_3Param_Count++;
    Number_of_Datums++;

    /*output updated 3-parameter datum table*/
    PathName = getenv( "DATUM_DATA" );
    if (PathName != NULL)
    {
      strcpy( FileName, PathName );
      strcat( FileName, "/" );
    }
    else
    {
      strcpy( FileName, "./" );
    }
    strcat( FileName, "3_param.dat" );

    if ((fp_3param = fopen(FileName, "w")) == NULL)
    { /* fatal error */
      return DATUM_3PARAM_FILE_OPEN_ERROR;
    }

    /* write file */
    index = 0;
    while (index < Datum_3Param_Count)
    {
      strcpy( datum_name, "\"" );
      strcat( datum_name, Datum_Table_3Param[index].Name);
      strcat( datum_name, "\"" );
      if (Datum_Table_3Param[index].User_Defined)
        fprintf(fp_3param, "*%-6s %-33s%-2s %4.0f %4.0f %4.0f %4.0f %5.0f %4.0f %4.0f %4.0f %4.0f %4.0f \n",
                Datum_Table_3Param[index].Code,
                datum_name,
                Datum_Table_3Param[index].Ellipsoid_Code,
                Datum_Table_3Param[index].Parameters[0],
                Datum_Table_3Param[index].Sigma_X, 
                Datum_Table_3Param[index].Parameters[1],
                Datum_Table_3Param[index].Sigma_Y, 
                Datum_Table_3Param[index].Parameters[2],
                Datum_Table_3Param[index].Sigma_Z,
                (Datum_Table_3Param[index].South_latitude * 180.0 / PI),
                (Datum_Table_3Param[index].North_latitude * 180.0 / PI),
                (Datum_Table_3Param[index].West_longitude * 180.0 / PI),
                (Datum_Table_3Param[index].East_longitude * 180.0 / PI));
      else
        fprintf(fp_3param, "%-6s  %-33s%-2s %4.0f %4.0f %4.0f %4.0f %5.0f %4.0f %4.0f %4.0f %4.0f %4.0f \n",
                Datum_Table_3Param[index].Code,
                datum_name,
                Datum_Table_3Param[index].Ellipsoid_Code,
                Datum_Table_3Param[index].Parameters[0],
                Datum_Table_3Param[index].Sigma_X, 
                Datum_Table_3Param[index].Parameters[1],
                Datum_Table_3Param[index].Sigma_Y, 
                Datum_Table_3Param[index].Parameters[2],
                Datum_Table_3Param[index].Sigma_Z,
                (Datum_Table_3Param[index].South_latitude * 180.0 / PI),
                (Datum_Table_3Param[index].North_latitude * 180.0 / PI),
                (Datum_Table_3Param[index].West_longitude * 180.0 / PI),
                (Datum_Table_3Param[index].East_longitude * 180.0 / PI));

      index++;
    }
    fclose(fp_3param);
  }
  return (error_code);
} /* End Create_Datum */

long Delete_Datum (const char *Code)

{ /* Begin Delete_Datum */
/*
 *   Code           : 5-letter datum code.                      (input)
 *
 * The function Delete_Datum deletes a local (3-parameter) datum with the 
 * specified code.  If the datum table has not been initialized or a new 
 * version of the 3-param.dat file cannot be created, an error code is returned,  
 * otherwise DATUM_NO_ERROR is returned.  Note that the indexes of all datums 
 * in the datum table may be changed by this function.
 */

  char *PathName = NULL;
  char FileName[FILENAME_LENGTH];
  char datum_name[DATUM_NAME_LENGTH+2];
  FILE *fp_3param = NULL;
  long index = 0;
  long error_code = DATUM_NO_ERROR;

  if (!Datum_Initialized)
    error_code |= DATUM_NOT_INITIALIZED_ERROR;
  else
  {
    if (!Datum_3Param_Index(Code, &index))
    {
      if (!Datum_Table_3Param[index-1].User_Defined)
        error_code |= DATUM_NOT_USERDEF_ERROR;
    }
    else
      error_code |= DATUM_NOT_USERDEF_ERROR;
  }
  if (!error_code)
  {
    long i = 0;
    long count = 0;

    for (i = index-1; i < Datum_3Param_Count-1; i++)
      Datum_Table_3Param[i] = Datum_Table_3Param[i+1];

    if (Datum_3Param_Count != MAX_3PARAM)
      Datum_Table_3Param[i] = Datum_Table_3Param[i+1];
    else
    {
      Datum_Table_3Param[i].Type = (Datum_Type)' ';
      strcpy(Datum_Table_3Param[i].Code, "");
      strcpy(Datum_Table_3Param[i].Name, "");
      strcpy(Datum_Table_3Param[i].Ellipsoid_Code, "");
      Datum_Table_3Param[i].Parameters[0] = 0;
      Datum_Table_3Param[i].Parameters[1] = 0;
      Datum_Table_3Param[i].Parameters[2] = 0;
      Datum_Table_3Param[i].Parameters[3] = 0;
      Datum_Table_3Param[i].Parameters[4] = 0;
      Datum_Table_3Param[i].Parameters[5] = 0;
      Datum_Table_3Param[i].Parameters[6] = 0;
      Datum_Table_3Param[i].Sigma_X = 0;
      Datum_Table_3Param[i].Sigma_Y = 0;
      Datum_Table_3Param[i].Sigma_Z = 0;
      Datum_Table_3Param[i].West_longitude = 0;
      Datum_Table_3Param[i].East_longitude = 0;
      Datum_Table_3Param[i].South_latitude = 0;
      Datum_Table_3Param[i].North_latitude = 0;
      Datum_Table_3Param[i].User_Defined = ' ';
    }
    Datum_3Param_Count--;
    Number_of_Datums--;

    for (i = 0; i < Number_of_Datums; i++)
    {
      if (Datum_Table[i]->Type == Three_Param_Datum)
      {
        Datum_Table[i] = &(Datum_Table_3Param[count]);
        count++;
      }
    }
    Datum_Table[Number_of_Datums] = 0x00000000;

    PathName = getenv( "DATUM_DATA" );
    if (PathName != NULL)
    {
      strcpy( FileName, PathName );
      strcat( FileName, "/" );
    }
    else
    {
      strcpy( FileName, "./" );
    }
    strcat( FileName, "3_param.dat" );

    if ((fp_3param = fopen(FileName, "w")) == NULL)
    { /* fatal error */
      return DATUM_3PARAM_FILE_OPEN_ERROR;
    }

    /* write file */
    index = 0;
    while (index < Datum_3Param_Count)
    {
      strcpy( datum_name, "\"" );
      strcat( datum_name, Datum_Table_3Param[index].Name);
      strcat( datum_name, "\"" );
      if (Datum_Table_3Param[index].User_Defined)
        fprintf(fp_3param, "*%-6s %-33s%-2s %4.0f %4.0f %4.0f %4.0f %5.0f %4.0f %4.0f %4.0f %4.0f %4.0f \n",
                Datum_Table_3Param[index].Code,
                datum_name,
                Datum_Table_3Param[index].Ellipsoid_Code,
                Datum_Table_3Param[index].Parameters[0],
                Datum_Table_3Param[index].Sigma_X, 
                Datum_Table_3Param[index].Parameters[1],
                Datum_Table_3Param[index].Sigma_Y, 
                Datum_Table_3Param[index].Parameters[2],
                Datum_Table_3Param[index].Sigma_Z,
                (Datum_Table_3Param[index].South_latitude * 180.0 / PI),
                (Datum_Table_3Param[index].North_latitude * 180.0 / PI),
                (Datum_Table_3Param[index].West_longitude * 180.0 / PI),
                (Datum_Table_3Param[index].East_longitude * 180.0 / PI));
      else
        fprintf(fp_3param, "%-6s  %-33s%-2s %4.0f %4.0f %4.0f %4.0f %5.0f %4.0f %4.0f %4.0f %4.0f %4.0f \n",
                Datum_Table_3Param[index].Code,
                datum_name,
                Datum_Table_3Param[index].Ellipsoid_Code,
                Datum_Table_3Param[index].Parameters[0],
                Datum_Table_3Param[index].Sigma_X, 
                Datum_Table_3Param[index].Parameters[1],
                Datum_Table_3Param[index].Sigma_Y, 
                Datum_Table_3Param[index].Parameters[2],
                Datum_Table_3Param[index].Sigma_Z,
                (Datum_Table_3Param[index].South_latitude * 180.0 / PI),
                (Datum_Table_3Param[index].North_latitude * 180.0 / PI),
                (Datum_Table_3Param[index].West_longitude * 180.0 / PI),
                (Datum_Table_3Param[index].East_longitude * 180.0 / PI));
      index++;
    }
    fclose(fp_3param);
  }
  return (error_code);
} /* End Delete_Datum */


long Datum_Uses_Ellipsoid (const char *Code)

{ /* Begin Datum_Uses_Ellipsoid */
  /*
  *  The function Datum_Uses_Ellipsoid returns 1 if the ellipsoid is in use by a 
  *  user defined datum.  Otherwise, 0 is returned.  
  *
  *  Code               : The ellipsoid code being searched for.    (input)
  */

  char temp_code[DATUM_CODE_LENGTH];
  long length;
  long pos = 0;
  long i = 0;
  long ellipsoid_in_use = FALSE;

  if (Datum_Initialized)
  {
    length = strlen(Code);
    if (length <= (ELLIPSOID_CODE_LENGTH-1))
    {
      strcpy(temp_code,Code);

      /* Convert to upper case */
      for (i=0;i<length;i++)
        temp_code[i] = (char)toupper(temp_code[i]);

      /* Strip blank spaces */
      while (pos < length)
      {
        if (isspace(temp_code[pos]))
        {
          for (i=pos;i<=length;i++)
            temp_code[i] = temp_code[i+1];
          length -= 1;
        }
        else
          pos += 1;
      }
      /* Search for code */
      i = 0;
      while ((i < Number_of_Datums) && (!ellipsoid_in_use))
      {
        if (strcmp(temp_code, Datum_Table[i]->Ellipsoid_Code) == 0)
          ellipsoid_in_use = TRUE;
        i++;
      }
    }
  }
  return (ellipsoid_in_use);
} /* End Datum_Uses_Ellipsoid */


long Datum_Count (long *Count)

{ /* Begin Datum_Count */
  /*
   *  The function Datum_Count returns the number of Datums in the table
   *  if the table was initialized without error.
   *
   *  Count                : number of datums in the datum table     (output)
   */

  long error_code = DATUM_NO_ERROR;

  if (Datum_Initialized)
    *Count = Number_of_Datums;
  else
  {
    *Count = 0;
    error_code |= DATUM_NOT_INITIALIZED_ERROR;
  }
  return (error_code);
} /* End of Datum_Count */


long Datum_3Param_Index( const char *Code,
                         long *Index )

{ /* Begin Datum_3Param_Index */
  /*
   *  The function Datum_3Param_Index returns the index of the datum with the 
   *  specified code.
   *
   *  Code    : The datum code being searched for.                    (input)
   *  Index   : The index of the datum in the table with the          (output)
   *              specified code.
   */
  char temp_code[DATUM_CODE_LENGTH];
  long error_code = DATUM_NO_ERROR;
  long length;
  long pos = 0;
  long i = 0;

  *Index = 0;
  if (Datum_Initialized)
  {
    length = strlen(Code);
    if (length > (DATUM_CODE_LENGTH-1))
      error_code |= DATUM_INVALID_CODE_ERROR;
    else
    {
      strcpy(temp_code,Code);

      /* Convert to upper case */
      for (i=0;i<length;i++)
        temp_code[i] = (char)toupper(temp_code[i]);

      /* Strip blank spaces */
      while (pos < length)
      {
        if (isspace(temp_code[pos]))
        {
          for (i=pos;i<=length;i++)
            temp_code[i] = temp_code[i+1];
          length -= 1;
        }
        else
          pos += 1;
      }
      /* Search for code */
      i = 0;
      while (i < Datum_3Param_Count && strcmp(temp_code, Datum_Table_3Param[i].Code))
      {
        i++;
      }
      if (i == Datum_3Param_Count || strcmp(temp_code, Datum_Table_3Param[i].Code))
        error_code |= DATUM_INVALID_CODE_ERROR;
      else
        *Index = i+1;
    }
  }
  else
  {
    error_code |= DATUM_NOT_INITIALIZED_ERROR;
  }
  return (error_code);
} /* End Datum_3Param_Index */


long Datum_Index( const char *Code,
                  long *Index )

{ /* Begin Datum_Index */
  /*
   *  The function Datum_Index returns the index of the datum with the 
   *  specified code.
   *
   *  Code    : The datum code being searched for.                    (input)
   *  Index   : The index of the datum in the table with the          (output)
   *              specified code.
   */
  char temp_code[DATUM_CODE_LENGTH];
  long error_code = DATUM_NO_ERROR;
  long length;
  long pos = 0;
  long i = 0;

  *Index = 0;
  if (Datum_Initialized)
  {
    length = strlen(Code);
    if (length > (DATUM_CODE_LENGTH-1))
      error_code |= DATUM_INVALID_CODE_ERROR;
    else
    {
      strcpy(temp_code,Code);

      /* Convert to upper case */
      for (i=0;i<length;i++)
        temp_code[i] = (char)toupper(temp_code[i]);

      /* Strip blank spaces */
      while (pos < length)
      {
        if (isspace(temp_code[pos]))
        {
          for (i=pos;i<=length;i++)
            temp_code[i] = temp_code[i+1];
          length -= 1;
        }
        else
          pos += 1;
      }
      /* Search for code */
      i = 0;
      while (i < Number_of_Datums && strcmp(temp_code, Datum_Table[i]->Code))
      {
        i++;
      }
      if (i == Number_of_Datums || strcmp(temp_code, Datum_Table[i]->Code))
        error_code |= DATUM_INVALID_CODE_ERROR;
      else
        *Index = i+1;
    }
  }
  else
  {
    error_code |= DATUM_NOT_INITIALIZED_ERROR;
  }
  return (error_code);
} /* End Datum_Index */


long Datum_Code (const long Index,
                 char *Code)
{ /* Begin Datum_Code */
  /*
   *  The function Datum_Code returns the 5-letter code of the datum
   *  referenced by index.
   *
   *  Index   : The index of a given datum in the datum table.        (input)
   *  Code    : The datum Code of the datum referenced by Index.      (output)
   */

  long error_code = DATUM_NO_ERROR;

  if (Datum_Initialized)
  {
    if (Index > 0 && Index <= Number_of_Datums)
      strcpy(Code, Datum_Table[Index-1]->Code);
    else
      error_code |= DATUM_INVALID_INDEX_ERROR;
  }
  else
  {
    error_code |= DATUM_NOT_INITIALIZED_ERROR;
  }
  return (error_code);
} /* End Datum_Code */


long Datum_Name (const long Index,
                 char *Name)
{ /* Begin Datum_Name */
  /*
   *  The function Datum_Name returns the name of the datum referenced by
   *  index.
   *
   *  Index   : The index of a given datum in the datum table.        (input)
   *  Name    : The datum Name of the datum referenced by Index.      (output)
   */

  long error_code = DATUM_NO_ERROR;

  if (Datum_Initialized)
  {
    if ((Index > 0) && (Index <= Number_of_Datums))
      strcpy(Name, Datum_Table[Index-1]->Name);
    else
      error_code |= DATUM_INVALID_INDEX_ERROR;
  }
  else
  {
    error_code |= DATUM_NOT_INITIALIZED_ERROR;
  }
  return (error_code);
} /* End Datum_Name */


long Datum_Ellipsoid_Code (const long Index,
                           char *Code)
{ /* Begin Datum_Ellipsoid_Code */
  /*
   *  The function Datum_Ellipsoid_Code returns the 2-letter ellipsoid code 
   *  for the ellipsoid associated with the datum referenced by index.
   *
   *  Index   : The index of a given datum in the datum table.          (input)
   *  Code    : The ellipsoid code for the ellipsoid associated with    (output)
   *               the datum referenced by index.
   */

  long error_code = DATUM_NO_ERROR;

  if (Datum_Initialized)
  {
    if ((Index < 1) || (Index > Number_of_Datums))
      error_code |= DATUM_INVALID_INDEX_ERROR;
    else
      strcpy(Code, Datum_Table[Index-1]->Ellipsoid_Code);
  }
  else
  {
    error_code |= DATUM_NOT_INITIALIZED_ERROR;
  }
  return (error_code);
} /* End Datum_Ellipsoid_Code */


long Retrieve_Datum_Type (const long Index,
                          Datum_Type *Type)
{ /* Begin Retrieve_Datum_Type */
  /*
   *  The function Retrieve_Datum_Type returns the type of the datum referenced by
   *  index.
   *
   *  Index   : The index of a given datum in the datum table.        (input)
   *  Type    : The type of datum referenced by index.                (output)
   */

  long error_code = DATUM_NO_ERROR;

  if (Datum_Initialized)
  {
    if ((Index < 1) || (Index > Number_of_Datums))
      error_code |= DATUM_INVALID_INDEX_ERROR;
    else
      *Type = Datum_Table[Index-1]->Type;
  }
  else
  {
    error_code |= DATUM_NOT_INITIALIZED_ERROR;
  }
  return (error_code);
} /* End Retrieve_Datum_Type */


long Datum_Seven_Parameters (const long Index, 
                             double *Delta_X,                             
                             double *Delta_Y,
                             double *Delta_Z,
                             double *Rx, 
                             double *Ry, 
                             double *Rz, 
                             double *Scale_Factor)

{ /* Begin Datum_Seven_Parameters */
  /*
   *   The function Datum_Seven_Parameters returns the seven parameters 
   *   for the datum referenced by index.
   *
   *    Index      : The index of a given datum in the datum table.  (input)
   *    Delta_X    : X translation in meters                         (output)
   *    Delta_Y    : Y translation in meters                         (output)
   *    Delta_Z    : Z translation in meters                         (output)
   *    Rx         : X rotation in radians                           (output)
   *    Rx         : Y rotation in radians                           (output)
   *    Ry         : Z rotation in radians                           (output)
   *    Scale_Factor : Scale factor                                  (output)
   */

  long error_code = DATUM_NO_ERROR;

  if (Datum_Initialized)
  {
    if (Index > 0 && Index <= Number_of_Datums)
    {
      *Delta_X = Datum_Table[Index-1]->Parameters[0];
      *Delta_Y = Datum_Table[Index-1]->Parameters[1];
      *Delta_Z = Datum_Table[Index-1]->Parameters[2];
      *Rx = Datum_Table[Index-1]->Parameters[3];
      *Ry = Datum_Table[Index-1]->Parameters[4];
      *Rz = Datum_Table[Index-1]->Parameters[5];
      *Scale_Factor = Datum_Table[Index-1]->Parameters[6];
    }
    else
    {
      error_code |= DATUM_INVALID_INDEX_ERROR;
    }
  }
  else
  {
    error_code |= DATUM_NOT_INITIALIZED_ERROR;
  }
  return (error_code);
} /* End Datum_Seven_Parameters */


long Datum_Three_Parameters (const long Index, 
                             double *Delta_X,
                             double *Delta_Y,
                             double *Delta_Z)
{ /* Begin Datum_Three_Parameters */
  /*
   *   The function Datum_Three_Parameters returns the three parameters
   *   for the datum referenced by index.
   *
   *    Index      : The index of a given datum in the datum table.  (input)
   *    Delta_X    : X translation in meters                         (output)
   *    Delta_Y    : Y translation in meters                         (output)
   *    Delta_Z    : Z translation in meters                         (output)
   */

  long error_code = DATUM_NO_ERROR;

  if (Datum_Initialized)
  {
    if (Index > 0 && Index <= Number_of_Datums)
    {
      *Delta_X = Datum_Table[Index-1]->Parameters[0];
      *Delta_Y = Datum_Table[Index-1]->Parameters[1];
      *Delta_Z = Datum_Table[Index-1]->Parameters[2];
    }
    else
    {
      error_code |= DATUM_INVALID_INDEX_ERROR;
    }
  }
  else
  {
    error_code |= DATUM_NOT_INITIALIZED_ERROR;
  }
  return (error_code);
} /* End Datum_Three_Parameters */

long Datum_Errors (const long Index, 
                   double *Sigma_X,
                   double *Sigma_Y,
                   double *Sigma_Z)
{ /* Begin Datum_Errors */
/*
 *   The function Datum_Errors returns the standard errors in X,Y, & Z 
 *   for the datum referenced by index.
 *
 *    Index      : The index of a given datum in the datum table   (input)
 *    Sigma_X    : Standard error in X in meters                   (output)
 *    Sigma_Y    : Standard error in Y in meters                   (output)
 *    Sigma_Z    : Standard error in Z in meters                   (output)
 */

  long error_code = DATUM_NO_ERROR;

  if (Datum_Initialized)
  {
    if (Index > 0 && Index <= Number_of_Datums)
    {
      *Sigma_X = Datum_Table[Index-1]->Sigma_X;
      *Sigma_Y = Datum_Table[Index-1]->Sigma_Y;
      *Sigma_Z = Datum_Table[Index-1]->Sigma_Z;
    }
    else
    {
      error_code |= DATUM_INVALID_INDEX_ERROR;
    }
  }
  else
  {
    error_code |= DATUM_NOT_INITIALIZED_ERROR;
  }
  return (error_code);
} /* End Datum_Errors */


long Datum_Valid_Rectangle ( const long Index,
                             double *South_latitude,
                             double *North_latitude,
                             double *West_longitude,
                             double *East_longitude)
{ /* Begin Datum_Valid_Rectangle */
  /*
   *   The function Datum_Valid_Rectangle returns the edges of the validity 
   *   rectangle for the datum referenced by index.
   *
   *   Index          : The index of a given datum in the datum table   (input)
   *   South_latitude : Southern edge of validity rectangle in radians  (output)
   *   North_latitude : Northern edge of validity rectangle in radians  (output)
   *   West_longitude : Western edge of validity rectangle in radians   (output)
   *   East_longitude : Eastern edge of validity rectangle in radians   (output)
   */
  long error_code = DATUM_NO_ERROR;

  if (Datum_Initialized)
  {
    if (Index > 0 && Index <= Number_of_Datums)
    {
      *South_latitude = Datum_Table[Index-1]->South_latitude;
      *North_latitude = Datum_Table[Index-1]->North_latitude;
      *West_longitude = Datum_Table[Index-1]->West_longitude;
      *East_longitude = Datum_Table[Index-1]->East_longitude;
    }
    else
    {
      error_code |= DATUM_INVALID_INDEX_ERROR;
    }
  }
  else
  {
    error_code |= DATUM_NOT_INITIALIZED_ERROR;
  }
  return (error_code);
} /* End Datum_Valid_Rectangle */


long Datum_User_Defined ( const long Index,
						              long *result )

{ /* Begin Datum_User_Defined */
/*
 *    Index    : Index of a given datum in the datum table (input)
 *    result   : Indicates whether specified datum is user defined (1)
 *               or not (0)                                (output)
 *
 *  The function Datum_User_Defined checks whether or not the specified datum is 
 *  user defined. It returns 1 if the datum is user defined, and returns
 *  0 otherwise. If index is valid DATUM_NO_ERROR is returned, otherwise
 *  DATUM_INVALID_INDEX_ERROR is returned.
 */

  long error_code = DATUM_NO_ERROR;
  
  *result = FALSE;

  if (Datum_Initialized)
  {
    if ((Index < 1) || (Index > Number_of_Datums))
      error_code |= DATUM_INVALID_INDEX_ERROR;
    else
	{
      if (Datum_Table[Index-1]->User_Defined)
	    *result = TRUE;
	}
  }
  else
    error_code |= DATUM_NOT_INITIALIZED_ERROR;
  return (error_code);
} /* End Datum_User_Defined */


long Valid_Datum(const long Index,
                 double latitude,
                 double longitude,
                 long *result)
{ /* Begin Valid_Datum */
  /*
   *  This function checks whether or not the specified location is within the 
   *  validity rectangle for the specified datum.  It returns zero if the specified
   *  location is NOT within the validity rectangle, and returns 1 otherwise.
   *
   *   Index     : The index of a given datum in the datum table      (input)
   *   latitude  : Latitude of the location to be checked in radians  (input)
   *   longitude : Longitude of the location to be checked in radians (input)
   *   result    : Indicates whether location is inside (1) or outside (0)
   *               of the validity rectangle of the specified datum   (output)
   */
  long error_code = DATUM_NO_ERROR;
  *result = 0;
  if (Datum_Initialized)
  {
    if ((Index < 1) || (Index > Number_of_Datums))
      error_code |= DATUM_INVALID_INDEX_ERROR;
    if ((latitude < MIN_LAT) || (latitude > MAX_LAT))
      error_code |= DATUM_LAT_ERROR;
    if ((longitude < MIN_LON) || (longitude > MAX_LON))
      error_code |= DATUM_LON_ERROR;
    if (!error_code)
    {
      if ((Datum_Table[Index-1]->South_latitude <= latitude) &&
          (latitude <= Datum_Table[Index-1]->North_latitude) &&
          (Datum_Table[Index-1]->West_longitude <= longitude) &&
          (longitude <= Datum_Table[Index-1]->East_longitude))
      {
        *result = 1;
      }
      else
      {
        *result = 0;
      }
    }
  }
  else
  {
    error_code |= DATUM_NOT_INITIALIZED_ERROR;
  }
  return (error_code);
} /* End Valid_Datum */


void Geocentric_Shift_WGS72_To_WGS84(const double X,
                                     const double Y,
                                     const double Z,
                                     double *X_WGS84,
                                     double *Y_WGS84,
                                     double *Z_WGS84)

{ /* Begin Geocentric_Shift_WGS72_To_WGS84 */
  /*
   *  This function shifts a geocentric coordinate (X, Y, Z in meters) relative
   *  to WGS72 to a geocentric coordinate (X, Y, Z in meters) relative to WGS84.
   *
   *  X       : X coordinate relative to WGS72            (input)
   *  Y       : Y coordinate relative to WGS72            (input)
   *  Z       : Z coordinate relative to WGS72            (input)
   *  X_WGS84 : X coordinate relative to WGS84            (output)
   *  Y_WGS84 : Y coordinate relative to WGS84            (output)
   *  Z_WGS84 : Z coordinate relative to WGS84            (output)
   */
  double Lat_72; /* Latitude relative to WGS72                   */
  double Lon_72; /* Longitude relative to WGS72                  */
  double Hgt_72; /* Height relative to WGS72                     */
  double Lat_84; /* Latitude relative to WGS84                   */
  double Lon_84; /* Longitude relative to WGS84                  */
  double Hgt_84; /* Heightt relative to WGS84                    */
  double a_72;   /* Semi-major axis in meters of WGS72 ellipsoid */
  double f_72;   /* Flattening of WGS72 ellipsoid                */
  double a_84;   /* Semi-major axis in meters of WGS84 ellipsoid */
  double f_84;   /* Flattening of WGS84 ellipsoid                */

  /* Set WGS72 ellipsoid params */
  WGS72_Parameters(&a_72, &f_72);
  Set_Geocentric_Parameters(a_72, f_72);
  Convert_Geocentric_To_Geodetic(X, Y, Z, &Lat_72, &Lon_72, &Hgt_72);
  Geodetic_Shift_WGS72_To_WGS84(Lat_72, Lon_72, Hgt_72, &Lat_84, &Lon_84,
                                &Hgt_84);
  /* Set WGS84 ellipsoid params */
  WGS84_Parameters(&a_84, &f_84);
  Set_Geocentric_Parameters(a_84, f_84);
  Convert_Geodetic_To_Geocentric(Lat_84, Lon_84, Hgt_84, X_WGS84, Y_WGS84,
                                 Z_WGS84);
} /* End Geocentric_Shift_WGS72_To_WGS84 */


void Geocentric_Shift_WGS84_To_WGS72(const double X_WGS84,
                                     const double Y_WGS84,
                                     const double Z_WGS84,
                                     double *X,
                                     double *Y,
                                     double *Z)

{ /* Begin Geocentric_Shift_WGS84_To_WGS72 */
  /*
   *  This function shifts a geocentric coordinate (X, Y, Z in meters) relative
   *  to WGS84 to a geocentric coordinate (X, Y, Z in meters) relative to WGS72.
   *
   *  X_WGS84 : X coordinate relative to WGS84            (input)
   *  Y_WGS84 : Y coordinate relative to WGS84            (input)
   *  Z_WGS84 : Z coordinate relative to WGS84            (input)
   *  X       : X coordinate relative to WGS72            (output)
   *  Y       : Y coordinate relative to WGS72            (output)
   *  Z       : Z coordinate relative to WGS72            (output)
   */
  double Lat_72; /* Latitude relative to WGS72                   */
  double Lon_72; /* Longitude relative to WGS72                  */
  double Hgt_72; /* Height relative to WGS72                     */
  double Lat_84; /* Latitude relative to WGS84                   */
  double Lon_84; /* Longitude relative to WGS84                  */
  double Hgt_84; /* Heightt relative to WGS84                    */
  double a_72;   /* Semi-major axis in meters of WGS72 ellipsoid */
  double f_72;   /* Flattening of WGS72 ellipsoid      */
  double a_84;   /* Semi-major axis in meters of WGS84 ellipsoid */
  double f_84;   /* Flattening of WGS84 ellipsoid      */
 
  /* Set WGS84 ellipsoid params */
  WGS84_Parameters(&a_84, &f_84);
  Set_Geocentric_Parameters(a_84, f_84);
  Convert_Geocentric_To_Geodetic(X_WGS84, Y_WGS84, Z_WGS84, &Lat_84, &Lon_84, &Hgt_84);
  Geodetic_Shift_WGS84_To_WGS72(Lat_84, Lon_84, Hgt_84, &Lat_72, &Lon_72,
                                &Hgt_72);
  /* Set WGS72 ellipsoid params */
  WGS72_Parameters(&a_72, &f_72);
  Set_Geocentric_Parameters(a_72, f_72);
  Convert_Geodetic_To_Geocentric(Lat_72, Lon_72, Hgt_72, X, Y, Z);
} /* End Geocentric_Shift_WGS84_To_WGS72 */


long Geocentric_Shift_To_WGS84(const long Index,
                               const double X,
                               const double Y,
                               const double Z,
                               double *X_WGS84,
                               double *Y_WGS84,
                               double *Z_WGS84)

{ /* Begin Geocentric_Shift_To_WGS84 */
  /*
   *  This function shifts a geocentric coordinate (X, Y, Z in meters) relative
   *  to the datum referenced by index to a geocentric coordinate (X, Y, Z in
   *  meters) relative to WGS84.
   *
   *  Index   : Index of source datum                         (input)
   *  X       : X coordinate relative to the source datum     (input)
   *  Y       : Y coordinate relative to the source datum     (input)
   *  Z       : Z coordinate relative to the source datum     (input)
   *  X_WGS84 : X coordinate relative to WGS84                (output)
   *  Y_WGS84 : Y coordinate relative to WGS84                (output)
   *  Z_WGS84 : Z coordinate relative to WGS84                (output)
   */
  Datum_Row *local;
  long error_code = DATUM_NO_ERROR;

  if (Datum_Initialized)
  {
    if ((Index < 1) || (Index > Number_of_Datums))
      error_code |= DATUM_INVALID_SRC_INDEX_ERROR;
    if (!error_code)
    {
      local = Datum_Table[Index-1];
      switch (local->Type)
      {
      case WGS72_Datum:
        {
          Geocentric_Shift_WGS72_To_WGS84(X, Y, Z, X_WGS84, Y_WGS84, Z_WGS84);
          break;
        }
      case WGS84_Datum:
        {          
          *X_WGS84 = X;
          *Y_WGS84 = Y;
          *Z_WGS84 = Z;
          break;
        }
      case Seven_Param_Datum:
        {
          *X_WGS84 = X + local->Parameters[0] + local->Parameters[5] * Y
                     - local->Parameters[4] * Z + local->Parameters[6] * X;
          *Y_WGS84 = Y + local->Parameters[1] - local->Parameters[5] * X
                     + local->Parameters[3] * Z + local->Parameters[6] * Y;
          *Z_WGS84 = Z + local->Parameters[2] + local->Parameters[4] * X
                     - local->Parameters[3] * Y + local->Parameters[6] * Z;
          break;
        }
      case Three_Param_Datum:
        {
          *X_WGS84 = X + local->Parameters[0];
          *Y_WGS84 = Y + local->Parameters[1];
          *Z_WGS84 = Z + local->Parameters[2];
          break;
        }
      } /* End switch */
    }
  }
  else
  {
    error_code |= DATUM_NOT_INITIALIZED_ERROR;
  }
  return (error_code);
} /* End Geocentric_Shift_To_WGS84 */


long Geocentric_Shift_From_WGS84(const double X_WGS84,
                                 const double Y_WGS84,
                                 const double Z_WGS84,
                                 const long Index,
                                 double *X,
                                 double *Y,
                                 double *Z)

{ /* Begin Geocentric_Shift_From_WGS84 */
  /*
   *  This function shifts a geocentric coordinate (X, Y, Z in meters) relative
   *  to WGS84 to a geocentric coordinate (X, Y, Z in meters) relative to the
   *  local datum referenced by index.
   *
   *  X_WGS84 : X coordinate relative to WGS84                      (input)
   *  Y_WGS84 : Y coordinate relative to WGS84                      (input)
   *  Z_WGS84 : Z coordinate relative to WGS84                      (input)
   *  Index   : Index of destination datum                          (input)
   *  X       : X coordinate relative to the destination datum      (output)
   *  Y       : Y coordinate relative to the destination datum      (output)
   *  Z       : Z coordinate relative to the destination datum      (output)
   */
  Datum_Row *local;
  long error_code = DATUM_NO_ERROR;

  if (Datum_Initialized)
  {
    if ((Index < 1) || (Index > Number_of_Datums))
      error_code |= DATUM_INVALID_DEST_INDEX_ERROR;
    if (!error_code)
    {
      local = Datum_Table[Index-1];
      switch (local->Type)
      {
      case WGS72_Datum:
        {
          Geocentric_Shift_WGS84_To_WGS72(X_WGS84, Y_WGS84, Z_WGS84, X, Y, Z);
          break;
        }
      case WGS84_Datum:
        {
          *X = X_WGS84;
          *Y = Y_WGS84;
          *Z = Z_WGS84;    
          break;
        }
      case Seven_Param_Datum:
        {
          *X = X_WGS84 - local->Parameters[0] - local->Parameters[5] * Y_WGS84
               + local->Parameters[4] * Z_WGS84 - local->Parameters[6] * X_WGS84;
          *Y = Y_WGS84 - local->Parameters[1] + local->Parameters[5] * X_WGS84
               - local->Parameters[3] * Z_WGS84 - local->Parameters[6] * Y_WGS84;
          *Z = Z_WGS84 - local->Parameters[2] - local->Parameters[4] * X_WGS84
               + local->Parameters[3] * Y_WGS84 - local->Parameters[6] * Z_WGS84;
          break;
        }
      case Three_Param_Datum:
        {
          *X = X_WGS84 - local->Parameters[0];
          *Y = Y_WGS84 - local->Parameters[1];
          *Z = Z_WGS84 - local->Parameters[2];
          break;
        }
      } /* End switch */
    }
  }
  else
  {
    error_code |= DATUM_NOT_INITIALIZED_ERROR;
  }
  return (error_code);
} /* End Geocentric_Shift_From_WGS84 */


long Geocentric_Datum_Shift (const long Index_in,
                             const double X_in,
                             const double Y_in,
                             const double Z_in,
                             const long Index_out,
                             double *X_out,
                             double *Y_out,
                             double *Z_out)

{ /* Begin Geocentric_Datum_Shift */
  /*
   *  This function shifts a geocentric coordinate (X, Y, Z in meters) relative
   *  to the source datum to geocentric coordinate (X, Y, Z in meters) relative
   *  to the destination datum.
   *
   *  Index_in  : Index of source datum                      (input)
   *  X_in      : X coordinate relative to source datum      (input)
   *  Y_in      : Y coordinate relative to source datum      (input)
   *  Z_in      : Z coordinate relative to source datum      (input)
   *  Index_out : Index of destination datum                 (input)
   *  X_out     : X coordinate relative to destination datum (output)
   *  Y_out     : Y coordinate relative to destination datum (output)
   *  Z_out     : Z coordinate relative to destination datum (output)
   */
  long error_code = DATUM_NO_ERROR;
  double X_WGS84;
  double Y_WGS84;
  double Z_WGS84;

  if (Datum_Initialized)
  {
    if ((Index_in < 1) || (Index_in > Number_of_Datums))
      error_code |= DATUM_INVALID_SRC_INDEX_ERROR;
    if ((Index_out < 1) || (Index_out > Number_of_Datums))
      error_code |= DATUM_INVALID_DEST_INDEX_ERROR;
    if (!error_code)
    {
      if (Index_in == Index_out)
      {
        *X_out = X_in;
        *Y_out = Y_in;
        *Z_out = Z_in;  
      }
      else
      {
        Geocentric_Shift_To_WGS84(Index_in, X_in, Y_in, Z_in, &X_WGS84,
                                  &Y_WGS84,&Z_WGS84);
        Geocentric_Shift_From_WGS84(X_WGS84, Y_WGS84, Z_WGS84, Index_out,
                                    X_out, Y_out, Z_out);      
      }
    }
  }
  else
  {
    error_code |= DATUM_NOT_INITIALIZED_ERROR;
  }
  return (error_code);
} /* End Geocentric_Datum_Shift */


void Geodetic_Shift_WGS72_To_WGS84( const double WGS72_Lat,
                                    const double WGS72_Lon,
                                    const double WGS72_Hgt,
                                    double *WGS84_Lat,
                                    double *WGS84_Lon,
                                    double *WGS84_Hgt)

{ /* Begin Geodetic_Shift_WGS72_To_WGS84 */
  /*
   *  This function shifts a geodetic coordinate (latitude, longitude in radians
   *  and height in meters) relative to WGS72 to a geodetic coordinate 
   *  (latitude, longitude in radians and height in meters) relative to WGS84.
   *
   *  WGS72_Lat : Latitude in radians relative to WGS72     (input)
   *  WGS72_Lon : Longitude in radians relative to WGS72    (input)
   *  WGS72_Hgt : Height in meters relative to WGS72        (input)
   *  WGS84_Lat : Latitude in radians relative to WGS84     (output)
   *  WGS84_Lon : Longitude in radians relative to WGS84    (output)
   *  WGS84_Hgt : Height in meters  relative to WGS84       (output)
   */
  double Delta_Lat;
  double Delta_Lon;
  double Delta_Hgt;
  double WGS84_a;       /* Semi-major axis of WGS84 ellipsoid               */
  double WGS84_f;       /* Flattening of WGS84 ellipsoid                    */
  double WGS72_a;       /* Semi-major axis of WGS72 ellipsoid               */
  double WGS72_f;       /* Flattening of WGS72 ellipsoid                    */
  double da;            /* WGS84_a - WGS72_a                                */
  double df;            /* WGS84_f - WGS72_f                                */
  double Q;
  double sin_Lat;
  double sin2_Lat;

  WGS84_Parameters( &WGS84_a, &WGS84_f );  
  WGS72_Parameters( &WGS72_a, &WGS72_f );  
  da = WGS84_a - WGS72_a;
  df = WGS84_f - WGS72_f;
  Q = PI /  648000;
  sin_Lat = sin(WGS72_Lat);
  sin2_Lat = sin_Lat * sin_Lat;

  Delta_Lat = (4.5 * cos(WGS72_Lat)) / (WGS72_a*Q) + (df * sin(2*WGS72_Lat)) / Q;
  Delta_Lat /= SECONDS_PER_RADIAN;
  Delta_Lon = 0.554 / SECONDS_PER_RADIAN;
  Delta_Hgt = 4.5 * sin_Lat + WGS72_a * df * sin2_Lat - da + 1.4;

  *WGS84_Lat = WGS72_Lat + Delta_Lat;
  *WGS84_Lon = WGS72_Lon + Delta_Lon;
  *WGS84_Hgt = WGS72_Hgt + Delta_Hgt;

	if (*WGS84_Lat > PI_OVER_2)  
		*WGS84_Lat = PI_OVER_2 - (*WGS84_Lat - PI_OVER_2);
	else if (*WGS84_Lat < -PI_OVER_2)
		*WGS84_Lat = -PI_OVER_2  - (*WGS84_Lat + PI_OVER_2);

	if (*WGS84_Lon > PI)
		*WGS84_Lon -= TWO_PI;
	if (*WGS84_Lon < -PI)
		*WGS84_Lon += TWO_PI;
} /* End Geodetic_Shift_WGS72_To_WGS84 */


void Geodetic_Shift_WGS84_To_WGS72( const double WGS84_Lat,
                                    const double WGS84_Lon,
                                    const double WGS84_Hgt,
                                    double *WGS72_Lat,
                                    double *WGS72_Lon,
                                    double *WGS72_Hgt)

{ /* Begin Geodetic_Shift_WGS84_To_WGS72 */
  /*
   *  This function shifts a geodetic coordinate (latitude, longitude in radians
   *  and height in meters) relative to WGS84 to a geodetic coordinate 
   *  (latitude, longitude in radians and height in meters) relative to WGS72.
   *
   *  WGS84_Lat : Latitude in radians relative to WGS84     (input)
   *  WGS84_Lon : Longitude in radians relative to WGS84    (input)
   *  WGS84_Hgt : Height in meters  relative to WGS84       (input)
   *  WGS72_Lat : Latitude in radians relative to WGS72     (output)
   *  WGS72_Lon : Longitude in radians relative to WGS72    (output)
   *  WGS72_Hgt : Height in meters relative to WGS72        (output)
   */
  double Delta_Lat;
  double Delta_Lon;
  double Delta_Hgt;
  double WGS84_a;       /* Semi-major axis of WGS84 ellipsoid               */
  double WGS84_f;       /* Flattening of WGS84 ellipsoid                    */
  double WGS72_a;       /* Semi-major axis of WGS72 ellipsoid               */
  double WGS72_f;       /* Flattening of WGS72 ellipsoid                    */
  double da;            /* WGS72_a - WGS84_a                                */
  double df;            /* WGS72_f - WGS84_f                                */
  double Q;
  double sin_Lat;
  double sin2_Lat;

  WGS84_Parameters( &WGS84_a, &WGS84_f );  
  WGS72_Parameters( &WGS72_a, &WGS72_f );  
  da = WGS72_a - WGS84_a;
  df = WGS72_f - WGS84_f;
  Q = PI / 648000;
  sin_Lat = sin(WGS84_Lat);
  sin2_Lat = sin_Lat * sin_Lat;

  Delta_Lat = (-4.5 * cos(WGS84_Lat)) / (WGS84_a*Q)
              + (df * sin(2*WGS84_Lat)) / Q;
  Delta_Lat /= SECONDS_PER_RADIAN;
  Delta_Lon = -0.554 / SECONDS_PER_RADIAN;
  Delta_Hgt = -4.5 * sin_Lat + WGS84_a * df * sin2_Lat - da - 1.4;

  *WGS72_Lat = WGS84_Lat + Delta_Lat;
  *WGS72_Lon = WGS84_Lon + Delta_Lon;
  *WGS72_Hgt = WGS84_Hgt + Delta_Hgt;

  if (*WGS72_Lat > PI_OVER_2)  
		*WGS72_Lat = PI_OVER_2 - (*WGS72_Lat - PI_OVER_2);
	else if (*WGS72_Lat < -PI_OVER_2)
		*WGS72_Lat = -PI_OVER_2  - (*WGS72_Lat + PI_OVER_2);

	if (*WGS72_Lon > PI)
		*WGS72_Lon -= TWO_PI;
	if (*WGS72_Lon < -PI)
		*WGS72_Lon += TWO_PI;
} /* End Geodetic_Shift_WGS84_To_WGS72 */


void Molodensky_Shift( const double a,
                       const double da,
                       const double f,
                       const double df,
                       const double dx,
                       const double dy,
                       const double dz,
                       const double Lat_in,
                       const double Lon_in,
                       const double Hgt_in,
                       double *Lat_out,
                       double *Lon_out,
                       double *Hgt_out)

{ /* Begin Molodensky_Shift */
  /*
   *  This function shifts geodetic coordinates using the Molodensky method.
   *
   *    a         : Semi-major axis of source ellipsoid in meters  (input)
   *    da        : Destination a minus source a                   (input)
   *    f         : Flattening of source ellipsoid                 (input)
   *    df        : Destination f minus source f                   (input)
   *    dx        : X coordinate shift in meters                   (input)
   *    dy        : Y coordinate shift in meters                   (input)
   *    dz        : Z coordinate shift in meters                   (input)
   *    Lat_in    : Latitude in radians.                           (input)
   *    Lon_in    : Longitude in radians.                          (input)
   *    Hgt_in    : Height in meters.                              (input)
   *    Lat_out   : Calculated latitude in radians.                (output)
   *    Lon_out   : Calculated longitude in radians.               (output)
   *    Hgt_out   : Calculated height in meters.                   (output)
   */
  double tLon_in;   /* temp longitude                                   */
  double e2;        /* Intermediate calculations for dp, dl               */
  double ep2;       /* Intermediate calculations for dp, dl               */
  double sin_Lat;   /* sin(Latitude_1)                                    */
  double sin2_Lat;  /* (sin(Latitude_1))^2                                */
  double sin_Lon;   /* sin(Longitude_1)                                   */
  double cos_Lat;   /* cos(Latitude_1)                                    */
  double cos_Lon;   /* cos(Longitude_1)                                   */
  double w2;        /* Intermediate calculations for dp, dl               */
  double w;         /* Intermediate calculations for dp, dl               */
  double w3;        /* Intermediate calculations for dp, dl               */
  double m;         /* Intermediate calculations for dp, dl               */
  double n;         /* Intermediate calculations for dp, dl               */
  double dp;        /* Delta phi                                          */
  double dp1;       /* Delta phi calculations                             */
  double dp2;       /* Delta phi calculations                             */
  double dp3;       /* Delta phi calculations                             */
  double dl;        /* Delta lambda                                       */
  double dh;        /* Delta height                                       */
  double dh1;       /* Delta height calculations                          */
  double dh2;       /* Delta height calculations                          */

  if (Lon_in > PI)
    tLon_in = Lon_in - (2*PI);
  else
    tLon_in = Lon_in;
  e2 = 2 * f - f * f;
  ep2 = e2 / (1 - e2);
  sin_Lat = sin(Lat_in);
  cos_Lat = cos(Lat_in);
  sin_Lon = sin(tLon_in);
  cos_Lon = cos(tLon_in);
  sin2_Lat = sin_Lat * sin_Lat;
  w2 = 1.0 - e2 * sin2_Lat;
  w = sqrt(w2);
  w3 = w * w2;
  m = (a * (1.0 - e2)) / w3;
  n = a / w;
  dp1 = cos_Lat * dz - sin_Lat * cos_Lon * dx - sin_Lat * sin_Lon * dy;
  dp2 = ((e2 * sin_Lat * cos_Lat) / w) * da;
  dp3 = sin_Lat * cos_Lat * (2.0 * n + ep2 * m * sin2_Lat) * (1.0 - f) * df;
  dp = (dp1 + dp2 + dp3) / (m + Hgt_in);
  dl = (-sin_Lon * dx + cos_Lon * dy) / ((n + Hgt_in) * cos_Lat);
  dh1 = (cos_Lat * cos_Lon * dx) + (cos_Lat * sin_Lon * dy) + (sin_Lat * dz);
  dh2 = -(w * da) + ((a * (1 - f)) / w) * sin2_Lat * df;
  dh = dh1 + dh2;
  *Lat_out = Lat_in + dp;
  *Lon_out = Lon_in + dl;
  *Hgt_out = Hgt_in + dh;
  if (*Lon_out > (PI * 2))
    *Lon_out -= 2*PI;
  if (*Lon_out < (- PI))
    *Lon_out += 2*PI;
} /* End Molodensky_Shift */


long Geodetic_Shift_To_WGS84( const long Index,
                              const double Lat_in,
                              const double Lon_in,
                              const double Hgt_in,
                              double *WGS84_Lat,
                              double *WGS84_Lon,
                              double *WGS84_Hgt)

{ /* Begin Geodetic_Shift_To_WGS84 */
  /*
   *  This function shifts geodetic coordinates relative to a given source datum
   *  to geodetic coordinates relative to WGS84.
   *
   *    Index     : Index of source datum                         (input)
   *    Lat_in    : Latitude in radians relative to source datum  (input)
   *    Lon_in    : Longitude in radians relative to source datum (input)
   *    Hgt_in    : Height in meters relative to source datum     (input)
   *    WGS84_Lat : Latitude in radians relative to WGS84         (output)
   *    WGS84_Lon : Longitude in radians relative to WGS84        (output)
   *    WGS84_Hgt : Height in meters relative to WGS84            (output)
   */
  double WGS84_a;   /* Semi-major axis of WGS84 ellipsoid in meters */
  double WGS84_f;   /* Flattening of WGS84 ellisoid                 */
  double a;         /* Semi-major axis of ellipsoid in meters       */
  double da;        /* Difference in semi-major axes                */
  double f;         /* Flattening of ellipsoid                      */
  double df;        /* Difference in flattening                     */
  double dx;
  double dy;
  double dz;
  long E_Index;
  long error_code = DATUM_NO_ERROR;
  Datum_Row *local;

  if (Datum_Initialized)
  {
    if ((Index < 1) || (Index > Number_of_Datums))
      error_code |= DATUM_INVALID_SRC_INDEX_ERROR;
    if ((Lat_in < (-90*PI/180)) || (Lat_in > (90*PI/180)))
      error_code |= DATUM_LAT_ERROR;
    if ((Lon_in < (-PI)) || (Lon_in > (2*PI)))
      error_code |= DATUM_LON_ERROR;
    if (!error_code)
    {
      local = Datum_Table[Index-1];
      switch (local->Type)
      {
      case WGS72_Datum:
        { /* Special case for WGS72 */
          Geodetic_Shift_WGS72_To_WGS84(Lat_in, Lon_in, Hgt_in, WGS84_Lat,
                                        WGS84_Lon, WGS84_Hgt);
          break;
        }
      case WGS84_Datum:
        {        /* Just copy */
          *WGS84_Lat = Lat_in;
          *WGS84_Lon = Lon_in;
          *WGS84_Hgt = Hgt_in;
          break;
        }
      case Seven_Param_Datum:
      case Three_Param_Datum:
        {
          if (Ellipsoid_Index(local->Ellipsoid_Code, &E_Index))
            error_code |= DATUM_ELLIPSE_ERROR;
          if (Ellipsoid_Parameters(E_Index, &a, &f))
            error_code |= DATUM_ELLIPSE_ERROR;
          if (!error_code)
          {
            if ((local->Type == Seven_Param_Datum) ||
                (Lat_in < (-MOLODENSKY_MAX)) || 
                (Lat_in > MOLODENSKY_MAX))
            { /* Use 3-step method */
              double local_X;
              double local_Y;
              double local_Z;
              double WGS84_X;
              double WGS84_Y;
              double WGS84_Z;
              Set_Geocentric_Parameters(a,f);
              Convert_Geodetic_To_Geocentric(Lat_in, Lon_in, Hgt_in,
                                             &local_X, &local_Y, &local_Z);
              Geocentric_Shift_To_WGS84(Index,
                                        local_X, local_Y, local_Z,
                                        &WGS84_X, &WGS84_Y, &WGS84_Z);
              WGS84_Parameters(&WGS84_a, &WGS84_f);
              Set_Geocentric_Parameters(WGS84_a, WGS84_f);
              Convert_Geocentric_To_Geodetic(WGS84_X, WGS84_Y, WGS84_Z,
                                             WGS84_Lat, WGS84_Lon, WGS84_Hgt);
            }
            else
            { /* Use Molodensky's method */
              WGS84_Parameters(&WGS84_a, &WGS84_f);
              da = WGS84_a - a;
              df = WGS84_f - f;
              dx = local->Parameters[0];
              dy = local->Parameters[1];
              dz = local->Parameters[2];
              Molodensky_Shift(a, da, f, df, dx, dy, dz, Lat_in, Lon_in, 
                               Hgt_in, WGS84_Lat, WGS84_Lon, WGS84_Hgt);
            }
          }
          break;
        }
      } /* End switch */
    }
  } /* End if (Datum_Initialized) */
  return (error_code);
} /* End Geodetic_Shift_To_WGS84 */


long Geodetic_Shift_From_WGS84( const double WGS84_Lat,
                                const double WGS84_Lon,
                                const double WGS84_Hgt,
                                const long Index,
                                double *Lat_out,
                                double *Lon_out,
                                double *Hgt_out)

{ /* Begin Geodetic_Shift_From_WGS84 */
  /*
   *  This function shifts geodetic coordinates relative to WGS84 
   *  to geodetic coordinates relative to a given local datum.
   *
   *    WGS84_Lat : Latitude in radians relative to WGS84              (input)
   *    WGS84_Lon : Longitude in radians relative to WGS84             (input)
   *    WGS84_Hgt : Height in meters  relative to WGS84                (input)
   *    Index     : Index of destination datum                         (input)
   *    Lat_out   : Latitude in radians relative to destination datum  (output)
   *    Lon_out   : Longitude in radians relative to destination datum (output)
   *    Hgt_out   : Height in meters relative to destination datum     (output)
   *
   */
  double WGS84_a;   /* Semi-major axis of WGS84 ellipsoid in meters */
  double WGS84_f;   /* Flattening of WGS84 ellisoid                 */
  double a;         /* Semi-major axis of ellipsoid in meters       */
  double da;        /* Difference in semi-major axes                */
  double f;         /* Flattening of ellipsoid                      */
  double df;        /* Difference in flattening                     */
  double dx;
  double dy;
  double dz;
  long E_Index;
  long error_code = DATUM_NO_ERROR;
  Datum_Row *local;

  if (Datum_Initialized)
  {
    if ((Index < 1) || (Index > Number_of_Datums))
      error_code |= DATUM_INVALID_DEST_INDEX_ERROR;
    if ((WGS84_Lat < (-90*PI/180)) || (WGS84_Lat > (90*PI/180)))
      error_code |= DATUM_LAT_ERROR;
    if ((WGS84_Lon < (-PI)) || (WGS84_Lon > (2*PI)))
      error_code |= DATUM_LON_ERROR;
    if (!error_code)
    {
      local = Datum_Table[Index-1];
      switch (local->Type)
      {
      case WGS72_Datum:
        {
          Geodetic_Shift_WGS84_To_WGS72( WGS84_Lat, WGS84_Lon, WGS84_Hgt,
                                         Lat_out, Lon_out, Hgt_out);
          break;
        }
      case WGS84_Datum:
        {          
          *Lat_out = WGS84_Lat;
          *Lon_out = WGS84_Lon;
          *Hgt_out = WGS84_Hgt;
          break;
        }
      case Seven_Param_Datum:
      case Three_Param_Datum:
        {
          if (Ellipsoid_Index(local->Ellipsoid_Code, &E_Index))
            error_code |= DATUM_ELLIPSE_ERROR;
          if (Ellipsoid_Parameters(E_Index, &a, &f))
            error_code |= DATUM_ELLIPSE_ERROR;
          if (!error_code)
          {
            if ((local->Type == Seven_Param_Datum) ||
                (WGS84_Lat < (-MOLODENSKY_MAX)) || 
                (WGS84_Lat > MOLODENSKY_MAX))
            { /* Use 3-step method */
              double local_X;
              double local_Y;
              double local_Z;
              double WGS84_X;
              double WGS84_Y;
              double WGS84_Z;
              WGS84_Parameters(&WGS84_a, &WGS84_f);
              Set_Geocentric_Parameters(WGS84_a, WGS84_f);
              Convert_Geodetic_To_Geocentric(WGS84_Lat, WGS84_Lon, WGS84_Hgt,
                                             &WGS84_X, &WGS84_Y, &WGS84_Z);
              Geocentric_Shift_From_WGS84(WGS84_X, WGS84_Y, WGS84_Z,
                                          Index, &local_X, &local_Y, &local_Z);
              Set_Geocentric_Parameters(a, f);
              Convert_Geocentric_To_Geodetic(local_X, local_Y, local_Z,
                                             Lat_out, Lon_out, Hgt_out);
            }
            else
            { /* Use Molodensky's method */
              WGS84_Parameters(&WGS84_a, &WGS84_f);
              da = a - WGS84_a;
              df = f - WGS84_f;
              dx = -(local->Parameters[0]);
              dy = -(local->Parameters[1]);
              dz = -(local->Parameters[2]);
              Molodensky_Shift(WGS84_a, da, WGS84_f, df, dx, dy, dz, 
                               WGS84_Lat, WGS84_Lon, WGS84_Hgt, Lat_out, Lon_out, Hgt_out);
            }
          }
          break;
        }
      } /* End switch */
    }
  } /* End if (Datum_Initialized) */
  return (error_code);
} /* End Geodetic_Shift_From_WGS84 */


long Geodetic_Datum_Shift ( const long Index_in,
                            const double Lat_in,
                            const double Lon_in,
                            const double Hgt_in,
                            const long Index_out,
                            double *Lat_out,
                            double *Lon_out,
                            double *Hgt_out)

{ /* Begin Geodetic_Datum_Shift */
  /*
   *  This function shifts geodetic coordinates (latitude, longitude in radians
   *  and height in meters) relative to the source datum to geodetic coordinates
   *  (latitude, longitude in radians and height in meters) relative to the
   *  destination datum.
   *
   *  Index_in  : Index of source datum                               (input)
   *  Lat_in    : Latitude in radians relative to source datum        (input)
   *  Lon_in    : Longitude in radians relative to source datum       (input)
   *  Hgt_in    : Height in meters relative to source datum           (input)
   *  Index_out : Index of destination datum                          (input)
   *  Lat_out   : Latitude in radians relative to destination datum   (output)
   *  Lon_out   : Longitude in radians relative to destination datum  (output)
   *  Hgt_out   : Height in meters relative to destination datum      (output)
   */
  long error_code = DATUM_NO_ERROR;
  double WGS84_Lat; /* Latitude in radians relative to WGS84   */
  double WGS84_Lon; /* Longitude in radians relative to WGS84  */
  double WGS84_Hgt; /* Height in meters relative to WGS84      */
  Datum_Row *In_Datum;
  Datum_Row *Out_Datum;
  long E_Index;
  double a;
  double f;
  double X1;
  double X2;
  double Y1;
  double Y2;
  double Z1;
  double Z2;

  if (Datum_Initialized)
  {
    if ((Index_in < 1) || (Index_in > Number_of_Datums))
      error_code |= DATUM_INVALID_SRC_INDEX_ERROR;
    if ((Index_out < 1) || (Index_out > Number_of_Datums))
      error_code |= DATUM_INVALID_DEST_INDEX_ERROR;
    if ((Lat_in < (-90*PI/180)) || (Lat_in > (90*PI/180)))
      error_code |= DATUM_LAT_ERROR;
    if ((Lon_in < (-PI)) || (Lon_in > (2*PI)))
      error_code |= DATUM_LON_ERROR;
    if (!error_code)
    {
      In_Datum = Datum_Table[Index_in-1];
      Out_Datum = Datum_Table[Index_out-1];
      if (Index_in == Index_out)
      { /* Just copy */
        *Lat_out = Lat_in;
        *Lon_out = Lon_in;
        *Hgt_out = Hgt_in;  
      }
      else if (In_Datum->Type == Seven_Param_Datum)
      {
        if (Ellipsoid_Index(In_Datum->Ellipsoid_Code, &E_Index))
          error_code |= DATUM_ELLIPSE_ERROR;
        if (Ellipsoid_Parameters(E_Index, &a, &f))
          error_code |= DATUM_ELLIPSE_ERROR;
        Set_Geocentric_Parameters(a, f);
        Convert_Geodetic_To_Geocentric(Lat_in, Lon_in, Hgt_in, &X1, &Y1, &Z1);
        if (Out_Datum->Type == Seven_Param_Datum)
        { /* Use 3-step method for both stages */
          Geocentric_Datum_Shift(Index_in, X1, Y1, Z1, Index_out, &X2, &Y2, &Z2);
          if (Ellipsoid_Index(Out_Datum->Ellipsoid_Code, &E_Index))
            error_code |= DATUM_ELLIPSE_ERROR;
          if (Ellipsoid_Parameters(E_Index, &a, &f))
            error_code |= DATUM_ELLIPSE_ERROR;
          Set_Geocentric_Parameters(a,f);
          Convert_Geocentric_To_Geodetic(X2, Y2, Z2, Lat_out, Lon_out, Hgt_out);
        }
        else
        { /* Use 3-step method for 1st stage, Molodensky if possible for 2nd stage */
          Geocentric_Shift_To_WGS84(Index_in, X1, Y1, Z1, &X2, &Y2, &Z2);
          WGS84_Parameters(&a, &f);
          Set_Geocentric_Parameters(a, f);
          Convert_Geocentric_To_Geodetic(X2, Y2, Z2, &WGS84_Lat, &WGS84_Lon, &WGS84_Hgt);
          Geodetic_Shift_From_WGS84(WGS84_Lat, WGS84_Lon, WGS84_Hgt, Index_out,
                                    Lat_out, Lon_out, Hgt_out);

        }
      }
      else if (Out_Datum->Type == Seven_Param_Datum)
      { /* Use Molodensky if possible for 1st stage, 3-step method for 2nd stage */
        Geodetic_Shift_To_WGS84(Index_in, Lat_in, Lon_in,
                                Hgt_in, &WGS84_Lat, &WGS84_Lon, &WGS84_Hgt);
        WGS84_Parameters( &a, &f);
        Set_Geocentric_Parameters(a, f);
        Convert_Geodetic_To_Geocentric(WGS84_Lat, WGS84_Lon, WGS84_Hgt, &X1, &Y1, &Z1);
        Geocentric_Shift_From_WGS84(X1, Y1, Z1, Index_out, &X2, &Y2, &Z2);
        if (Ellipsoid_Index(Out_Datum->Ellipsoid_Code, &E_Index))
          error_code |= DATUM_ELLIPSE_ERROR;
        if (Ellipsoid_Parameters(E_Index, &a, &f))
          error_code |= DATUM_ELLIPSE_ERROR;
        Set_Geocentric_Parameters(a,f);
        Convert_Geocentric_To_Geodetic(X2, Y2, Z2, Lat_out, Lon_out, Hgt_out);
      }
      else
      { /* Use Molodensky if possible for both stages */
        error_code |= Geodetic_Shift_To_WGS84(Index_in, Lat_in, Lon_in,
                                              Hgt_in, &WGS84_Lat, &WGS84_Lon, &WGS84_Hgt);
        if (!error_code)
          error_code |= Geodetic_Shift_From_WGS84(WGS84_Lat, WGS84_Lon,
                                                  WGS84_Hgt, Index_out, Lat_out, Lon_out, Hgt_out);
      }
    }
  }
  else
  {
    error_code |= DATUM_NOT_INITIALIZED_ERROR;
  }
  return (error_code);
} /* End Geodetic_Datum_Shift */


long Datum_Shift_Error (const long Index_in,
                        const long Index_out,
                        double latitude,
                        double longitude,
                        double *ce90,
                        double *le90,
                        double *se90)
/*
 *  This function returns the 90% horizontal (circular), vertical (linear), and 
 *  spherical errors for a shift from the specified source datum to the 
 *  specified destination datum at the specified location.
 *
 *  Index_in  : Index of source datum                                      (input)
 *  Index_out : Index of destination datum                                 (input)
 *  latitude  : Latitude of point being converted in radians               (input)
 *  longitude : Longitude of point being converted in radians              (input)
 *  ce90      : Combined 90% circular horizontal error in meters           (update)
 *  le90      : Combined 90% linear vertical error in meters               (update)
 *  se90      : Combined 90% spherical error in meters                     (update)
 */
{ /* Begin Datum_Shift_Error */
  long error_code = DATUM_NO_ERROR;
  Datum_Row *In_Datum;
  Datum_Row *Out_Datum;
  double sinlat = sin(latitude);
  double coslat = cos(latitude);
  double sinlon = sin(longitude);
  double coslon = cos(longitude);
  double sigma_delta_lat;
  double sigma_delta_lon;
  double sigma_delta_height;
  double sx, sy, sz;
  double ce90_in = -1.0;
  double le90_in = -1.0;
  double se90_in = -1.0;
  double ce90_out = -1.0;
  double le90_out = -1.0;
  double se90_out = -1.0;

  if (Datum_Initialized)
  {
    if ((Index_in < 1) || (Index_in > Number_of_Datums))
      error_code |= DATUM_INVALID_SRC_INDEX_ERROR;
    if ((Index_out < 1) || (Index_out > Number_of_Datums))
      error_code |= DATUM_INVALID_DEST_INDEX_ERROR;
    if ((latitude < (-90*PI/180)) || (latitude > (90*PI/180)))
      error_code |= DATUM_LAT_ERROR;
    if ((longitude < (-PI)) || (longitude > (2*PI)))
      error_code |= DATUM_LON_ERROR;
    if (!error_code)
    {
      In_Datum = Datum_Table[Index_in-1];
      Out_Datum = Datum_Table[Index_out-1];

      if (Index_in == Index_out)
      { /* Just copy */
        *ce90 = *ce90;
        *le90 = *le90;
        *se90 = *se90;
      }
      else
      {
        /* calculate input datum errors */
        switch (In_Datum->Type)
        {
        case WGS84_Datum:
        case WGS72_Datum:
        case Seven_Param_Datum:
          {
            ce90_in = 0.0;
            le90_in = 0.0;
            se90_in = 0.0;
            break;
          }
        case Three_Param_Datum:
          {
            if ((In_Datum->Sigma_X < 0)
                ||(In_Datum->Sigma_Y < 0)
                ||(In_Datum->Sigma_Z < 0))
            {
              ce90_in = -1.0;
              le90_in = -1.0;
              se90_in = -1.0;
            }
            else
            {
              sx = (In_Datum->Sigma_X * sinlat * coslon);
              sy = (In_Datum->Sigma_Y * sinlat * sinlon);
              sz = (In_Datum->Sigma_Z * coslat);
              sigma_delta_lat = sqrt((sx * sx) + (sy * sy) + (sz * sz));
              sx = (In_Datum->Sigma_X * sinlon);
              sy = (In_Datum->Sigma_Y * coslon);
              sigma_delta_lon = sqrt((sx * sx) + (sy * sy));
              sx = (In_Datum->Sigma_X * coslat * coslon);
              sy = (In_Datum->Sigma_Y * coslat * sinlon);
              sz = (In_Datum->Sigma_Z * sinlat);
              sigma_delta_height = sqrt((sx * sx) + (sy * sy) + (sz * sz));
              ce90_in = 2.146 * (sigma_delta_lat + sigma_delta_lon) / 2.0;
              le90_in = 1.6449 * sigma_delta_height;
              se90_in = 2.5003 * (In_Datum->Sigma_X + In_Datum->Sigma_Y + In_Datum->Sigma_Z) / 3.0;
            }
            break;
          }
        } /* End switch */
        /* calculate output datum errors */
        switch (Out_Datum->Type)
        {
        case WGS84_Datum:
        case WGS72_Datum:
        case Seven_Param_Datum:
          {
            ce90_out = 0.0;
            le90_out = 0.0;
            se90_out = 0.0;
            break;
          }
        case Three_Param_Datum:
          {
            if ((Out_Datum->Sigma_X < 0)
                ||(Out_Datum->Sigma_Y < 0)
                ||(Out_Datum->Sigma_Z < 0))
            {
              ce90_out = -1.0;
              le90_out = -1.0;
              se90_out = -1.0;
            }
            else
            {
              sx = (Out_Datum->Sigma_X * sinlat * coslon);
              sy = (Out_Datum->Sigma_Y * sinlat * sinlon);
              sz = (Out_Datum->Sigma_Z * coslat);
              sigma_delta_lat = sqrt((sx * sx) + (sy * sy) + (sz * sz));
              sx = (Out_Datum->Sigma_X * sinlon);
              sy = (Out_Datum->Sigma_Y * coslon);
              sigma_delta_lon = sqrt((sx * sx) + (sy * sy));
              sx = (Out_Datum->Sigma_X * coslat * coslon);
              sy = (Out_Datum->Sigma_Y * coslat * sinlon);
              sz = (Out_Datum->Sigma_Z * sinlat);
              sigma_delta_height = sqrt((sx * sx) + (sy * sy) + (sz * sz));
              ce90_out = 2.146 * (sigma_delta_lat + sigma_delta_lon) / 2.0;
              le90_out = 1.6449 * sigma_delta_height;
              se90_out = 2.5003 * (Out_Datum->Sigma_X + Out_Datum->Sigma_Y + Out_Datum->Sigma_Z) / 3.0;
            }
            break;
          }
        } /* End switch */
        /* combine errors */
        if ((*ce90 < 0.0) || (ce90_in < 0.0) || (ce90_out < 0.0))
        {
          *ce90 = -1.0;
          *le90 = -1.0;
          *se90 = -1.0;
        }
        else
        {
          *ce90 = sqrt((*ce90 * *ce90) + (ce90_in * ce90_in) + (ce90_out * ce90_out));
          if (*ce90 < 1.0)
          {
            *ce90 = 1.0;
          }
          if ((*le90 < 0.0) || (le90_in < 0.0) || (le90_out < 0.0))
          {
            *le90 = -1.0;
            *se90 = -1.0;
          }
          else
          {
            *le90 = sqrt((*le90 * *le90) + (le90_in * le90_in) + (le90_out * le90_out));
            if (*le90 < 1.0)
            {
              *le90 = 1.0;
            }
            if ((*se90 < 0.0) || (se90_in < 0.0) || (se90_out < 0.0))
              *se90 = -1.0;
            else
            {
              *se90 = sqrt((*se90 * *se90) + (se90_in * se90_in) + (se90_out * se90_out));
              if (*se90 < 1.0)
              {
                *se90 = 1.0;
              }
            }
          }
        }
      }/* End else */
    }
  }
  else
  {
    error_code |= DATUM_NOT_INITIALIZED_ERROR;
  }
  return (error_code);
} /* End Datum_Shift_Error */




