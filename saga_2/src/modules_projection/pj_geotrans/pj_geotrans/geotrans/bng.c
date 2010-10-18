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
 *                               INCLUDES
 */

#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "tranmerc.h"
#include "bng.h"

/*
 *    ctype.h     - Standard C character handling library
 *    math.h      - Standard C math library
 *    stdio.h     - Standard C input/output library
 *    string.h    - Standard C string handling library
 *    tranmerc.h  - Is used to convert transverse mercator coordinates
 *    bng.h       - Is for prototype error checking
 */


/***************************************************************************/
/*                               DEFINES 
 *
 */

#define PI              3.14159265358979323e0   /* PI     */
#define PI_OVER_2       (PI / 2.0e0)            /* PI over 2 */
#define TWO_PI          (2.0e0 * PI)            /* 2 * PI */
#define MAX_LAT         (61.5 * PI / 180.0)     /* 61.5 degrees */
#define MIN_LAT         (49.5 * PI / 180.0)     /* 49.5 degrees */
#define MAX_LON         (3.5 * PI / 180.0)      /* 3.5 degrees */
#define MIN_LON         (-10.0 * PI / 180.0)    /* -10 degrees */
#define BNG500GRID      "STNOHJ"                /* 500,000 unit square identifications */
#define BNG100GRID      "VWXYZQRSTULMNOPFGHJKABCDE" /* 100,000 unit square identifications */

/**************************************************************************/
/*                               GLOBAL DECLARATIONS
 *
 */

/* Ellipsoid Parameters, must be Airy  */
static const double BNG_a = 6377563.396;               /* Semi-major axis of ellipsoid, in meters */
static const double BNG_f = 1 / 299.324964600;         /* Flattening of ellipsoid */

/* BNG projection Parameters */
static const double BNG_Origin_Lat = (49.0 * PI / 180.0);  /* Latitude of origin, in radians */
static const double BNG_Origin_Long = (-2.0 * PI / 180.0); /* Longitude of origin, in radians */
static const double BNG_False_Northing = -100000.0;        /* False northing, in meters */
static const double BNG_False_Easting = 400000.0;          /* False easting, in meters */
static const double BNG_Scale_Factor = .9996012717;        /* Scale factor                      */

/* Maximum variance for easting and northing values for Airy. */
static double BNG_Max_Easting = 759961.0;
static double BNG_Max_Northing = 1257875.0;
static double BNG_Min_Easting = -133134.0;
static double BNG_Min_Northing = -14829.0;

static char BNG_Letters[3] = "SV";
static double BNG_Easting = 0.0;
static double BNG_Northing = 0.0;
static long string_Broken = 0;
static const char* Airy = "AA";
static char BNG_Ellipsoid_Code[3] = "AA";

/* These state variables are for optimization purposes. The only function
 * that should modify them is Set_BNG_Parameters.         */

/************************************************************************/
/*                              FUNCTIONS
 *
 */

long Find_Index (char letter, 
                 const char* letter_Array, 
                 long *index)

{ /* BEGIN Find_Index */  
/*
 * The function Find_Index searches for a given letter in an array. 
 * It returns the index of the letter in the array if the letter is found.
 * If the letter is not found, an error code is returned, otherwise
 * BNG_NO_ERROR is returned. 
 *
 *    letter              : Letter being searched for         
 *    letter_Array        : Array being searched         
 *    index               : Index of letter in array        
 */
  long i = 0;
  long not_Found = 1;
  long length = strlen(letter_Array);
  long Error_Code = BNG_NO_ERROR;

  while ((i < length) && (not_Found))
  {
    if (letter == letter_Array[i])
    {
      *index = i;
      not_Found = 0;
    }
    i++;
  }
  if (not_Found)
    Error_Code = BNG_STRING_ERROR;

  return Error_Code;
} /*END OF Find_Index */


long Round_BNG (double value)
/* Round value to nearest integer, using standard engineering rule */
{ /* Round_BNG */
  double ivalue;
  long ival;
  double fraction = modf (value, &ivalue);
  ival = (long)(ivalue);
  if ((fraction > 0.5) || ((fraction == 0.5) && (ival%2 == 1)))
    ival++;
  return (ival);
} /* Round_BNG */


long Make_BNG_String (char ltrnum[4],
                      long Easting, 
                      long Northing,
                      char* BNG,
                      long Precision)
                        
{ /* Make_BNG_String */
/* Construct a BNG string from its component parts */

  double divisor;
  long east;
  long north;
  long i;
  long j;
  double unitInterval;
  long error_code = BNG_NO_ERROR;

  i = 0;
  for (j = 0; j < 3; j++)
    BNG[i++] = ltrnum[j];
  divisor = pow (10.0, (5 - Precision));
  unitInterval = pow (10.0, Precision);

  east = Round_BNG (Easting/divisor);
  if (east == unitInterval)
    east -= 1;
  if ((Precision == 0) && (east == 1))
    east = 0;
  i += sprintf (BNG + i, "%*.*ld", Precision, Precision, east);

  north = Round_BNG (Northing/divisor);
  if (north == unitInterval)
    north -= 1;
  if ((Precision == 0) && (north == 1))
    north = 0;
  i += sprintf (BNG + i, "%*.*ld",  Precision, Precision, north);

  return (error_code);
} /* Make_BNG_String */


long Check_Out_Of_Area(char BNG500,
                       char BNG100)

{ /* BEGIN Check_Out_Of_Area */
/*
 * The function Check_Out_Of_Area checks if the 500,000 and 
 * 100,000 unit square identifications are within the valid area. If
 * they are not, an error code is returned, otherwise BNG_NO_ERROR 
 * is returned.
 *
 *    BNG500        : 500,000 square unit identification         
 *    BNG100        : 100,000 square unit identification         
 */

  long Error_Code = BNG_NO_ERROR;

  switch (BNG500)
  {
    case 'S':
      switch (BNG100)
      {
        case 'A':
        case 'F':
        case 'L':
          Error_Code |= BNG_INVALID_AREA_ERROR;
          break;
        default:
          break;
      }
      break;
    case 'N':
      switch (BNG100)
      {
        case 'V':
          Error_Code |= BNG_INVALID_AREA_ERROR;
          break;
        default:
          break;
      }
      break;

    case 'H':
      if (BNG100 < 'L')
        Error_Code |= BNG_INVALID_AREA_ERROR;
      break;
    case 'T':
      switch (BNG100)
      {
        case 'D':
        case 'E':
        case 'J':
        case 'K':
        case 'O':
        case 'P':
        case 'T':
        case 'U':
        case 'X':
        case 'Y':
        case 'Z':
          Error_Code |= BNG_INVALID_AREA_ERROR;
          break;
        default:
          break;
      }
      break;
    case 'O':
      switch (BNG100)
      {
        case 'C':
        case 'D':
        case 'E':
        case 'J':
        case 'K':
        case 'O':
        case 'P':
        case 'T':
        case 'U':
        case 'Y':
        case 'Z':
          Error_Code |= BNG_INVALID_AREA_ERROR;
          break;
        default:
          break;
      }
      break;
    case 'J':
      switch (BNG100)
      {
        case 'L':
        case 'M':
        case 'Q':
        case 'R':
        case 'V':
        case 'W':
          Error_Code = BNG_NO_ERROR;
          break;
        default:
          Error_Code |= BNG_INVALID_AREA_ERROR;
          break;
      }
      break;
    default:
      Error_Code |= BNG_INVALID_AREA_ERROR;
      break;
  }
  return Error_Code;
} /* END OF Check_Out_Of_Area */


long Break_BNG_String (char* BNG,
                       char Letters[3],
                       double* Easting,
                       double* Northing,
                       long* Precision)
/* Break down a BNG string into its component parts */
{ /* Break_BNG_String */

  long i = 0;
  long j;
  long num_digits = 0;
  long num_letters;
  long temp_error = 0;
  long length = strlen(BNG);
  long error_code = BNG_NO_ERROR;

  string_Broken = 1;
  while (BNG[i] == ' ')
    i++;  /* skip any leading blanks */
  j = i;
  while (isalpha(BNG[i]))
    i++;
  num_letters = i - j;
  if (num_letters == 2)
  {
    /* get letters */
    Letters[0] = (char)toupper(BNG[j]);
    Letters[1] = (char)toupper(BNG[j+1]);
    Letters[2] = 0;
  }
  else
    error_code |= BNG_STRING_ERROR;
  if (!error_code)
  {
    error_code |= Check_Out_Of_Area(Letters[0], Letters[1]);
    while (BNG[i] == ' ')
      i++;
    j = i;
    if (BNG[length-1] == ' ')
      length --;
    while ((i < length) && (!temp_error))
    {
      if (isdigit(BNG[i]))
        i++;
      else
        temp_error = BNG_STRING_ERROR;
    }
    if (temp_error)
      error_code |= temp_error;
    if (!error_code)
    {
      num_digits = i - j;
      if ((num_digits <= 10) && (num_digits%2 == 0))
      {
        long n;
        char east_string[6];
        char north_string[6];
        long east;
        long north;
        double multiplier;

        /* get easting & northing */
        n = num_digits / 2;
        *Precision = n;
        if (n > 0)
        {
          strncpy (east_string, BNG+j, n);
          east_string[n] = 0;
          sscanf (east_string, "%ld", &east);
          strncpy (north_string, BNG+j+n, n);
          north_string[n] = 0;
          sscanf (north_string, "%ld", &north);
          multiplier = pow (10.0, 5 - n);
          *Easting = east * multiplier;
          *Northing = north * multiplier;
        }
        else
        {
          *Easting = 0.0;
          *Northing = 0.0;
        }
      }
      else
        error_code |= BNG_STRING_ERROR;
    }
  }
  return (error_code);
} /* Break_BNG_String */


long Set_BNG_Parameters(char *Ellipsoid_Code)

{ /* BEGIN Set_BNG_Parameters */
/*
 * The function Set_BNG_Parameters receives the ellipsoid code and sets
 * the corresponding state variables. If any errors occur, the error code(s)
 * are returned by the function, otherwise BNG_NO_ERROR is returned.
 *
 *   Ellipsoid_Code : 2-letter code for ellipsoid           (input)
 */

  long Error_Code = BNG_NO_ERROR;

  if (strcmp(Ellipsoid_Code, Airy) != 0)
  { /* Ellipsoid must be Airy */
    Error_Code |= BNG_ELLIPSOID_ERROR;
  }
    
  if (!Error_Code)
  { /* no errors */
    strcpy(BNG_Ellipsoid_Code, Ellipsoid_Code);
    string_Broken = 0;

    Set_Transverse_Mercator_Parameters(BNG_a, BNG_f, BNG_Origin_Lat,
                             BNG_Origin_Long, BNG_False_Easting, 
                             BNG_False_Northing, BNG_Scale_Factor);
  }
  return (Error_Code);
}  /* END of Set_BNG_Parameters  */


void Get_BNG_Parameters(char *Ellipsoid_Code)
                        
{ /* BEGIN Get_BNG_Parameters */
/*                         
 * The function Get_BNG_Parameters returns the current ellipsoid
 * code.
 *
 *   Ellipsoid_Code : 2-letter code for ellipsoid          (output)
 */

  strcpy(Ellipsoid_Code, BNG_Ellipsoid_Code);

  return;
} /* END OF Get_BNG_Parameters */


long Convert_Geodetic_To_BNG (double Latitude,
                              double Longitude,
                              long Precision,
                              char* BNG)

{ /* BEGIN Convert_Geodetic_To_BNG */
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

  double TMEasting, TMNorthing;
  long Error_Code = BNG_NO_ERROR;

  if ((Latitude < MIN_LAT) || (Latitude > MAX_LAT))
  {  /* Latitude out of range */
    Error_Code|= BNG_LAT_ERROR;
  }
  if ((Longitude < MIN_LON) || (Longitude > MAX_LON))
  {  /* Longitude out of range */
    Error_Code|= BNG_LON_ERROR;
  }

  if (!Error_Code)
  { /* no errors */

    Error_Code |= Convert_Geodetic_To_Transverse_Mercator(Latitude, Longitude, &TMEasting, &TMNorthing);
    
    if ((TMEasting < 0.0) && (TMEasting > -2.0))
      TMEasting = 0.0;
    if ((TMNorthing < 0.0) && (TMNorthing > -2.0))
      TMNorthing = 0.0;

    if ((TMEasting < BNG_Min_Easting) || (TMEasting > BNG_Max_Easting))
      Error_Code |= BNG_INVALID_AREA_ERROR;
    if ((TMNorthing < BNG_Min_Northing) || (TMNorthing > BNG_Max_Northing))
      Error_Code |= BNG_INVALID_AREA_ERROR;
    
    if (!Error_Code)
      Error_Code |= Convert_Transverse_Mercator_To_BNG(TMEasting, TMNorthing, Precision, BNG);
  }
  return Error_Code;
} /* END OF Convert_Geodetic_To_BNG */


long Convert_BNG_To_Geodetic (char *BNG, 
                              double *Latitude, 
                              double *Longitude)

{ /* Convert_BNG_To_Geodetic */
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

  double TMEasting, TMNorthing;
  long in_Precision;
  long temp_Error = 0;
  long Error_Code = BNG_NO_ERROR;
  
  Error_Code |= Break_BNG_String(BNG, BNG_Letters, &BNG_Easting, &BNG_Northing, &in_Precision);
  
  if(!Error_Code)
  {
    Error_Code |= Convert_BNG_To_Transverse_Mercator(BNG, &TMEasting, &TMNorthing);

    if ((TMEasting < BNG_Min_Easting) || (TMEasting > BNG_Max_Easting))
      Error_Code |= BNG_INVALID_AREA_ERROR;
    if ((TMNorthing < BNG_Min_Northing) || (TMNorthing > BNG_Max_Northing))
      Error_Code |= BNG_INVALID_AREA_ERROR;

    if(!Error_Code)
    {
      temp_Error |= Convert_Transverse_Mercator_To_Geodetic(TMEasting, TMNorthing, Latitude, Longitude);
      
      if (temp_Error & TRANMERC_EASTING_ERROR)
        Error_Code |= BNG_EASTING_ERROR;
      if (temp_Error & TRANMERC_NORTHING_ERROR)
        Error_Code |= BNG_NORTHING_ERROR;

      if ((*Latitude < MIN_LAT) || (*Latitude > MAX_LAT))
        Error_Code|= BNG_INVALID_AREA_ERROR;
      if ((*Longitude < MIN_LON) || (*Longitude > MAX_LON))
        Error_Code|= BNG_INVALID_AREA_ERROR;
    }
  }
  return Error_Code;
} /* END OF Convert_BNG_To_Geodetic */


long Convert_Transverse_Mercator_To_BNG(double Easting,
                                        double Northing,            
                                        long Precision,
                                        char *BNG)

{/* BEGIN Convert_Transverse_Mercator_To_BNG */
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

  char letters[4];
  long x, y;
  long index;
  long temp_Easting, temp_Northing;
  long Error_Code = BNG_NO_ERROR;

  if ((Easting < BNG_Min_Easting) || (Easting > BNG_Max_Easting))
  { /* Easting out of range  */
    Error_Code |= BNG_EASTING_ERROR;
  }
  if ((Northing < BNG_Min_Northing) || (Northing > BNG_Max_Northing))
  { /* Northing out of range */
    Error_Code |= BNG_NORTHING_ERROR;
  }

  if (!Error_Code)
  { /* no errors */
    temp_Easting = Round_BNG(Easting);
    temp_Northing = Round_BNG(Northing);

    temp_Easting += 1000000; 
    temp_Northing += 500000;

    x = temp_Easting / 500000;
    y = temp_Northing / 500000;

    index = y * 5 + x;
    if ((index >= 0) && (index < 25))
    {
      letters[0] = BNG100GRID[index];
      temp_Easting %= 500000;
      temp_Northing %= 500000;
      x = temp_Easting / 100000;
      y = temp_Northing / 100000;
      index = y * 5 + x;
      if ((index >= 0) && (index < 25))
      {
        letters[1] = BNG100GRID[index];
        Error_Code |= Check_Out_Of_Area(letters[0], letters[1]);
        if (!Error_Code)
        {
          letters[2] = ' ';
          letters[3] = 0;
          temp_Easting %= 100000;
          temp_Northing %= 100000;
          Make_BNG_String(letters, temp_Easting, temp_Northing, BNG, Precision);
        }
      }
      else
      {
        long five_y = 5 * y;
        if ((x >= (25 - five_y)) || (x < -five_y))
          Error_Code |= BNG_EASTING_ERROR;
        if ((five_y >= (25 - x)) || (five_y < -x))
          Error_Code |= BNG_NORTHING_ERROR;
      }
    }
    else
    {
      long five_y = 5 * y;
      if ((x >= (25 - five_y)) || (x < -five_y))
        Error_Code |= BNG_EASTING_ERROR;
      if ((five_y >= (25 - x)) || (five_y < -x))
        Error_Code |= BNG_NORTHING_ERROR;
    }
  }
  return Error_Code;
} /* END OF Convert_Transverse_Mercator_To_BNG */


long Convert_BNG_To_Transverse_Mercator(char *BNG,
                                        double *Easting,
                                        double *Northing)

{ /* Convert_BNG_To_Transverse_Mercator */
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

  long northing_Offset, easting_Offset;
  long i = 0;
  long j = 0;
  long in_Precision;
  long Error_Code = BNG_NO_ERROR;

  if (!string_Broken)
    Error_Code |= Break_BNG_String(BNG, BNG_Letters, &BNG_Easting, &BNG_Northing, &in_Precision);
  if (!Error_Code)
  {
    Error_Code |= Find_Index(BNG_Letters[0], BNG500GRID, &i);
    if (!Error_Code)
    {
      northing_Offset = 500000 * (i / 2);
      easting_Offset = 500000 * (i % 2);

      Error_Code |= Find_Index(BNG_Letters[1], BNG100GRID, &j);
      if (!Error_Code)
      {
        northing_Offset += 100000 * (j / 5);
        easting_Offset += 100000 * (j % 5);

        *Easting = BNG_Easting + easting_Offset;
        *Northing = BNG_Northing + northing_Offset;
      }
    }
  }
  return Error_Code;
} /* END OF Convert_BNG_To_Transverse_Mercator */



