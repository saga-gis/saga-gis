/**********************************************************
 * Version $Id: ellipse.c 911 2011-02-14 16:38:15Z reklov_w $
 *********************************************************/
/****************************************************************************/
/* RSC IDENTIFIER:  Ellipsoid
 *
 * ABSTRACT
 *
 *    The purpose of ELLIPSOID is to provide access to ellipsoid parameters 
 *    for a collection of common ellipsoids.  A particular ellipsoid can be 
 *    accessed by using its standard 2-letter code to find its index in the 
 *    ellipsoid table.  The index can then be used to retrieve the ellipsoid 
 *    name and parameters.
 *
 *    By sequentially retrieving all of the ellipsoid codes and/or names, a 
 *    menu of the available ellipsoids can be constructed.  The index values 
 *    resulting from selections from this menu can then be used to access the 
 *    parameters of the selected ellipsoid.
 *
 *    This component depends on a data file named "ellips.dat", which contains
 *    the ellipsoid parameter values.  A copy of this file must be located in 
 *    the directory specified by the environment variable "ELLIPSOID_DATA", if 
 *    defined, or else in the current directory, whenever a program containing 
 *    this component is executed.
 *
 *    Additional ellipsoids can be added to this file, either manually or using 
 *    the Create_Ellipsoid function.  However, if a large number of ellipsoids 
 *    are added, the ellipsoid table array size in this component will have to 
 *    be increased.
 *
 * ERROR HANDLING
 *
 *    This component checks parameters for valid values.  If an invalid value
 *    is found, the error code is combined with the current error code using 
 *    the bitwise or.  This combining allows multiple error codes to be
 *    returned. The possible error codes are:
 *
 *  ELLIPSE_NO_ERROR             : No errors occurred in function
 *  ELLIPSE_FILE_OPEN_ERROR      : Ellipsoid file opening error
 *  ELLIPSE_INITIALIZE_ERROR     : Ellipsoid table can not initialize
 *  ELLIPSE_TABLE_OVERFLOW_ERROR : Ellipsoid table overflow
 *  ELLIPSE_NOT_INITIALIZED_ERROR: Ellipsoid table not initialized properly
 *  ELLIPSE_INVALID_INDEX_ERROR  : Index is an invalid value
 *  ELLIPSE_INVALID_CODE_ERROR   : Code was not found in table
 *  ELLIPSE_A_ERROR              : Semi-major axis less than or equal to zero
 *  ELLIPSE_INV_F_ERROR          : Inverse flattening outside of valid range
 *	                                (250 to 350)
 *  ELLIPSE_IN_USE_ERROR         : User defined ellipsoid is in use by a user 
 *                                  defined datum
 *  ELLIPSE_NOT_USERDEF_ERROR    : Ellipsoid is not user defined - cannot be
 *                                  deleted
 *
 * REUSE NOTES
 *
 *    Ellipsoid is intended for reuse by any application that requires Earth
 *    approximating ellipsoids.
 *     
 * REFERENCES
 *
 *    Further information on Ellipsoid can be found in the Reuse Manual.
 *
 *    Ellipsoid originated from :  U.S. Army Topographic Engineering Center (USATEC)
 *                                 Geospatial Information Division (GID)
 *                                 7701 Telegraph Road
 *                                 Alexandria, VA  22310-3864
 *
 * LICENSES
 *
 *    None apply to this component.
 *
 * RESTRICTIONS
 *
 *    Ellipsoid has no restrictions.
 *
 * ENVIRONMENT
 *
 *    Ellipsoid was tested and certified in the following environments
 *
 *    1. Solaris 2.5
 *    2. Windows 95 
 *
 * MODIFICATIONS
 *
 *    Date              Description
 *    ----              -----------
 *    11-19-95          Original Code
 *    17-Jan-97         Moved local constants out of public interface
 *                      Improved efficiency in algorithms (GEOTRANS)
 *    24-May-99         Added user-defined ellipsoids (GEOTRANS for JMTK)
 *
 */


/***************************************************************************/
/*
 *                               INCLUDES
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "datum.h"
#include "ellipse.h"

/* 
 *    ctype.h    - standard C character handling library
 *    stdio.h    - standard C input/output library
 *    stdlib.h   - standard C general utilities library
 *    string.h   - standard C string handling library
 *    datum.h    - used to determine if user defined ellipsoid 
 *                  is in use by a user defined datum
 *    ellipse.h  - prototype error checking and error codes
 */


/***************************************************************************/
/*
 *                             GLOBAL DATA STRUCTURES
 */

#define MAX_ELLIPSOIDS        32  /* Maximum number of ellipsoids in table */
#define ELLIPSOID_CODE_LENGTH  3  /* Length of ellipsoid code (including null) */
#define ELLIPSOID_NAME_LENGTH 30  /* Max length of ellipsoid name */
#define ELLIPSOID_BUF         90
#define FILENAME_LENGTH      128
#define FALSE 0
#define TRUE  1

const char *WGS84_Ellipsoid_Code = "WE";
const char *WGS72_Ellipsoid_Code = "WD";

typedef struct Ellipsoid_Table_Row
{
  char Name[ELLIPSOID_NAME_LENGTH];
  char Code[ELLIPSOID_CODE_LENGTH];
  double A;
  double B;
  double Recp_F;
  long User_Defined;  /* Identifies a user defined ellipsoid */
} Ellipsoid_Row;

static Ellipsoid_Row Ellipsoid_Table[MAX_ELLIPSOIDS];
static long WGS84_Index = 0;           /* Index of WGS84 in ellipsoid table */
static long WGS72_Index = 0;           /* Index of WGS72 in ellipsoid table */
static long Number_of_Ellipsoids = 0;  /* Number of ellipsoids in table */
static long Ellipsoid_Initialized = 0; /* Indicates successful initialization */

/***************************************************************************/
/*                              FUNCTIONS                                  */


void Assign_Ellipsoid_Row (Ellipsoid_Row *destination, 
                           const Ellipsoid_Row *source)
{ /* Begin Assign_Ellipsoid_Row */
/*
 *   destination  : The destination of the copy         (output)
 *   source       : The source for the copy             (input)
 *
 * The function Assign_Ellipsoid_Row copies ellipsoid data.
 */

  strcpy(destination->Name, source->Name);
  strcpy(destination->Code, source->Code);
  destination->A = source->A;
  destination->B = source->B;
  destination->Recp_F = source->Recp_F;
  destination->User_Defined = source->User_Defined;
} /* End Assign_Ellipsoid_Row */


long Initialize_Ellipsoids_File(const char *File_Ellipsoids) 
{ /* Begin Initialize_Ellipsoids */
/*
 * The function Initialize_Ellipsoids reads ellipsoid data from ellips.dat in
 * the current directory and builds the ellipsoid table from it.  If an 
 * error occurs, the error code is returned, otherwise ELLIPSE_NO_ERROR is 
 * returned.
 */

  FILE *fp = NULL;                    /* File pointer to file ellips.dat     */
  char buffer[ELLIPSOID_BUF];
  long index = 0;                     /* Array index                         */
  long error_code = ELLIPSE_NO_ERROR;

  Ellipsoid_Initialized	= 0;

  /*  Check the environment for a user provided path, else current directory;   */
  /*  Build a File Name, including specified or default path:                   */

  /*  Open the File READONLY, or Return Error Condition:                        */

  if( File_Ellipsoids && File_Ellipsoids[0] != '\0' && (fp = fopen(File_Ellipsoids, "r")) != NULL )
  {
	  /* read file */
	  while ((!feof(fp)) && (!error_code))
	  {
		if (index <= MAX_ELLIPSOIDS)
		{
		  if (fgets(buffer, ELLIPSOID_BUF, fp))
		  {
			sscanf(buffer, "%30c %s %lf %lf %lf",
				   Ellipsoid_Table[index].Name,
				   Ellipsoid_Table[index].Code,
				   &(Ellipsoid_Table[index].A),
				   &(Ellipsoid_Table[index].B),
				   &(Ellipsoid_Table[index].Recp_F));
			if (Ellipsoid_Table[index].Name[0] == '*')
			{
			  int i;
			  Ellipsoid_Table[index].User_Defined = TRUE;
			  for (i = 0; i < ELLIPSOID_NAME_LENGTH; i++)
				Ellipsoid_Table[index].Name[i] = Ellipsoid_Table[index].Name[i+1];
			}
			else
			  Ellipsoid_Table[index].User_Defined = FALSE;
			Ellipsoid_Table[index].Name[ELLIPSOID_NAME_LENGTH - 1] = '\0'; /* null terminate */
			index++;
		  }
		}
		else
		  error_code |= ELLIPSE_TABLE_OVERFLOW_ERROR;
	  } 
	  fclose(fp);
  }
  else
  {
	strcpy(Ellipsoid_Table[index].Name, "WGS 84");
	strcpy(Ellipsoid_Table[index].Code, "WE");
	Ellipsoid_Table[index].A		= 6378137.000;
	Ellipsoid_Table[index].B		= 6356752.3142;
	Ellipsoid_Table[index].Recp_F	= 298.257223563;
	Ellipsoid_Table[index].User_Defined = FALSE;
//	Ellipsoid_Table[index].Name[ELLIPSOID_NAME_LENGTH - 1] = '\0'; /* null terminate */
	index++;

	strcpy(Ellipsoid_Table[index].Name, "WGS 72");
	strcpy(Ellipsoid_Table[index].Code, "WD");
	Ellipsoid_Table[index].A		= 6378135.000;
	Ellipsoid_Table[index].B		= 6356750.5200;
	Ellipsoid_Table[index].Recp_F	= 298.260000000;
	Ellipsoid_Table[index].User_Defined = FALSE;
//	Ellipsoid_Table[index].Name[ELLIPSOID_NAME_LENGTH - 1] = '\0'; /* null terminate */
	index++;
  }

  Number_of_Ellipsoids = index;

  if (error_code)
  {
    Ellipsoid_Initialized = 0;
    Number_of_Ellipsoids = 0;
  }
  else
    Ellipsoid_Initialized = 1;

  /* Store WGS84 Index*/
  if (Ellipsoid_Index(WGS84_Ellipsoid_Code, &WGS84_Index))
    error_code |= ELLIPSE_INITIALIZE_ERROR;

  /* Store WGS72 Index*/
  if (Ellipsoid_Index(WGS72_Ellipsoid_Code, &WGS72_Index))
    error_code |= ELLIPSE_INITIALIZE_ERROR;

  return (error_code);
} /* End of Initialize_Ellipsoids */


long Initialize_Ellipsoids () 
{ /* Begin Initialize_Ellipsoids */
/*
 * The function Initialize_Ellipsoids reads ellipsoid data from ellips.dat in
 * the current directory and builds the ellipsoid table from it.  If an 
 * error occurs, the error code is returned, otherwise ELLIPSE_NO_ERROR is 
 * returned.
 */

  char *PathName = NULL;
  char FileName[FILENAME_LENGTH];
  FILE *fp = NULL;                    /* File pointer to file ellips.dat     */
  char buffer[ELLIPSOID_BUF];
  long index = 0;                     /* Array index                         */
  long error_code = ELLIPSE_NO_ERROR;

  if (Ellipsoid_Initialized)
  {
    return error_code;
  }

  /*  Check the environment for a user provided path, else current directory;   */
  /*  Build a File Name, including specified or default path:                   */

  PathName = getenv( "ELLIPSOID_DATA" );
  if (PathName != NULL)
  {
    strcpy( FileName, PathName );
    strcat( FileName, "/" );
  }
  else
  {
    strcpy( FileName, "./" );
  }
  strcat( FileName, "ellips.dat" );

  /*  Open the File READONLY, or Return Error Condition:                        */

  if (( fp = fopen( FileName, "r" ) ) == NULL)
  {
    return ( ELLIPSE_FILE_OPEN_ERROR);
  }

  /* read file */
  while ((!feof(fp)) && (!error_code))
  {
    if (index <= MAX_ELLIPSOIDS)
    {
      if (fgets(buffer, ELLIPSOID_BUF, fp))
      {
        sscanf(buffer, "%30c %s %lf %lf %lf",
               Ellipsoid_Table[index].Name,
               Ellipsoid_Table[index].Code,
               &(Ellipsoid_Table[index].A),
               &(Ellipsoid_Table[index].B),
               &(Ellipsoid_Table[index].Recp_F));
        if (Ellipsoid_Table[index].Name[0] == '*')
        {
          int i;
          Ellipsoid_Table[index].User_Defined = TRUE;
          for (i = 0; i < ELLIPSOID_NAME_LENGTH; i++)
            Ellipsoid_Table[index].Name[i] = Ellipsoid_Table[index].Name[i+1];
        }
        else
          Ellipsoid_Table[index].User_Defined = FALSE;
        Ellipsoid_Table[index].Name[ELLIPSOID_NAME_LENGTH - 1] = '\0'; /* null terminate */
        index++;
      }
    }
    else
      error_code |= ELLIPSE_TABLE_OVERFLOW_ERROR;
  } 
  fclose(fp);
  Number_of_Ellipsoids = index;

  if (error_code)
  {
    Ellipsoid_Initialized = 0;
    Number_of_Ellipsoids = 0;
  }
  else
    Ellipsoid_Initialized = 1;

  /* Store WGS84 Index*/
  if (Ellipsoid_Index(WGS84_Ellipsoid_Code, &WGS84_Index))
    error_code |= ELLIPSE_INITIALIZE_ERROR;

  /* Store WGS72 Index*/
  if (Ellipsoid_Index(WGS72_Ellipsoid_Code, &WGS72_Index))
    error_code |= ELLIPSE_INITIALIZE_ERROR;

  return (error_code);
} /* End of Initialize_Ellipsoids */


long Create_Ellipsoid (const char* Code,
                       const char* Name,
                       double a,
                       double f)
{ /* Begin Create_Ellipsoid */
/*
 *   Code     : 2-letter ellipsoid code.                      (input)
 *   Name     : Name of the new ellipsoid                     (input)
 *   a        : Semi-major axis, in meters, of new ellipsoid  (input)
 *   f        : Flattening of new ellipsoid.                  (input)
 *
 * The function Create_Ellipsoid creates a new ellipsoid with the specified
 * Code, name, and axes.  If the ellipsoid table has not been initialized,
 * the specified code is already in use, or a new version of the ellips.dat 
 * file cannot be created, an error code is returned, otherwise ELLIPSE_NO_ERROR 
 * is returned.  Note that the indexes of all ellipsoids in the ellipsoid
 * table may be changed by this function.
 */

  long error_code = ELLIPSE_NO_ERROR;
  long index = 0;
  long code_length = 0;
  char *PathName = NULL;
  char FileName[FILENAME_LENGTH];
  char ellipsoid_code[ELLIPSOID_CODE_LENGTH];
  FILE *fp = NULL;                    /* File pointer to file ellips.dat     */
  double inv_f = 1 / f;

  if (!Ellipsoid_Initialized)
    error_code |= ELLIPSE_NOT_INITIALIZED_ERROR;
  else if (!(Number_of_Ellipsoids < MAX_ELLIPSOIDS))
    error_code |= ELLIPSE_TABLE_OVERFLOW_ERROR;
  else
  {
    code_length = strlen(Code);
    if ((!Ellipsoid_Index(Code, &index)) || (code_length > (ELLIPSOID_CODE_LENGTH-1)))
      error_code |= ELLIPSE_INVALID_CODE_ERROR;
    if (a <= 0.0)
      error_code |= ELLIPSE_A_ERROR;
    if ((inv_f < 250) || (inv_f > 350))
    { /* Inverse flattening must be between 250 and 350 */
      error_code |= ELLIPSE_INV_F_ERROR;
    }
    if (!error_code)
    {
      long i;
      strcpy(ellipsoid_code,Code);
      /* Convert code to upper case */
      for (i = 0; i < code_length; i++)
        ellipsoid_code[i] = (char)toupper(ellipsoid_code[i]);
      index = Number_of_Ellipsoids;
      strcpy(Ellipsoid_Table[index].Name, Name);
      strcpy(Ellipsoid_Table[index].Code, ellipsoid_code);
      Ellipsoid_Table[index].A = a;
      Ellipsoid_Table[index].B = a * (1 - f);
      Ellipsoid_Table[index].Recp_F = inv_f;
      Ellipsoid_Table[index].User_Defined = TRUE;
      Number_of_Ellipsoids++;
      /*output updated ellipsoid table*/
      PathName = getenv( "ELLIPSOID_DATA" );
      if (PathName != NULL)
      {
        strcpy( FileName, PathName );
        strcat( FileName, "/" );
      }
      else
      {
        strcpy( FileName, "./" );
      }
      strcat( FileName, "ellips.dat" );

      if ((fp = fopen(FileName, "w")) == NULL)
      { /* fatal error */
        return ELLIPSE_FILE_OPEN_ERROR;
      }
      /* write file */
      index = 0;
      while (index < Number_of_Ellipsoids)
      {
        if (Ellipsoid_Table[index].User_Defined)
          fprintf(fp, "*%-29s %-2s %11.3f %12.4f %13.9f \n",
                  Ellipsoid_Table[index].Name,
                  Ellipsoid_Table[index].Code,
                  Ellipsoid_Table[index].A,
                  Ellipsoid_Table[index].B,
                  Ellipsoid_Table[index].Recp_F);
        else
          fprintf(fp, "%-29s  %-2s %11.3f %12.4f %13.9f \n",
                  Ellipsoid_Table[index].Name,
                  Ellipsoid_Table[index].Code,
                  Ellipsoid_Table[index].A,
                  Ellipsoid_Table[index].B,
                  Ellipsoid_Table[index].Recp_F);
        index++;
      }
      fclose(fp);
      /* Store WGS84 */
      Ellipsoid_Index(WGS84_Ellipsoid_Code, &WGS84_Index);
      /* Store WGS72 */
      Ellipsoid_Index(WGS72_Ellipsoid_Code, &WGS72_Index);
    }
  }
  return (error_code);
} /* End Create_Ellipsoid */


long Delete_Ellipsoid (const char* Code) 
{/* Begin Delete_Ellipsoid */
/*
 *   Code     : 2-letter ellipsoid code.                      (input)
 *
 * The function Delete_Ellipsoid deletes a user defined ellipsoid with 
 * the specified Code.  If the ellipsoid table has not been created,
 * the specified code is in use by a user defined datum, or a new version   
 * of the ellips.dat file cannot be created, an error code is returned, 
 * otherwise ELLIPSE_NO_ERROR is returned.  Note that the indexes of all  
 * ellipsoids in the ellipsoid table may be changed by this function.
 */

  long error_code = ELLIPSE_NO_ERROR;
  long index = 0;
  char *PathName = NULL;
  char FileName[FILENAME_LENGTH];
  FILE *fp = NULL;                    /* File pointer to file ellips.dat     */

  if (!Ellipsoid_Initialized)
    error_code |= ELLIPSE_NOT_INITIALIZED_ERROR;
  else
  {
    if (!Ellipsoid_Index(Code, &index))
    {
      if (Ellipsoid_Table[index-1].User_Defined)
      {
        if (Datum_Uses_Ellipsoid(Code))
          error_code |= ELLIPSE_IN_USE_ERROR;
      }
      else
        error_code |= ELLIPSE_NOT_USERDEF_ERROR;
    }
    else
      error_code |= ELLIPSE_NOT_USERDEF_ERROR;
  }
  if (!error_code)
  {
    long i = 0;
    for (i = index-1; i < Number_of_Ellipsoids-1; i++)
      Ellipsoid_Table[i] = Ellipsoid_Table[i+1];

    if (Number_of_Ellipsoids != MAX_ELLIPSOIDS)
      Ellipsoid_Table[i] = Ellipsoid_Table[i+1];
    else
    {
      strcpy(Ellipsoid_Table[i].Name, "");
      strcpy(Ellipsoid_Table[i].Code, "");
      Ellipsoid_Table[i].A = 0;
      Ellipsoid_Table[i].B = 0;
      Ellipsoid_Table[i].Recp_F = 0;  
      Ellipsoid_Table[i].User_Defined = ' ';
    }
    Number_of_Ellipsoids--;
    /*output updated ellipsoid table*/
    PathName = getenv( "ELLIPSOID_DATA" );
    if (PathName != NULL)
    {
      strcpy( FileName, PathName );
      strcat( FileName, "/" );
    }
    else
    {
      strcpy( FileName, "./" );
    }
    strcat( FileName, "ellips.dat" );
    if ((fp = fopen(FileName, "w")) == NULL)
    { /* fatal error */
      return ELLIPSE_FILE_OPEN_ERROR;
    }
    /* write file */
    index = 0;
    while (index < Number_of_Ellipsoids)
    {
      if (Ellipsoid_Table[index].User_Defined)
        fprintf(fp, "*%-29s %-2s %11.3f %12.4f %13.9f \n",
                Ellipsoid_Table[index].Name,
                Ellipsoid_Table[index].Code,
                Ellipsoid_Table[index].A,
                Ellipsoid_Table[index].B,
                Ellipsoid_Table[index].Recp_F);
      else
        fprintf(fp, "%-29s  %-2s %11.3f %12.4f %13.9f \n",
                Ellipsoid_Table[index].Name,
                Ellipsoid_Table[index].Code,
                Ellipsoid_Table[index].A,
                Ellipsoid_Table[index].B,
                Ellipsoid_Table[index].Recp_F);
      index++;
    }
    fclose(fp);
    /* Store WGS84 */
    Ellipsoid_Index(WGS84_Ellipsoid_Code, &WGS84_Index);
    /* Store WGS72 */
    Ellipsoid_Index(WGS72_Ellipsoid_Code, &WGS72_Index);
  }
  return (error_code);
}/* End Delete_Ellipsoid */


long Ellipsoid_Count ( long *Count )
{ /* Begin Ellipsoid_Count */
/*
 *   Count    : The number of ellipsoids in the ellipsoid table. (output)
 *
 * The function Ellipsoid_Count returns the number of ellipsoids in the
 * ellipsoid table.  If the ellipsoid table has been initialized without error,
 * ELLIPSE_NO_ERROR is returned, otherwise ELLIPSE_NOT_INITIALIZED_ERROR
 * is returned.
 */
  long error_code = ELLIPSE_NO_ERROR;
  if (!Ellipsoid_Initialized)
    error_code |= ELLIPSE_NOT_INITIALIZED_ERROR;
  *Count = Number_of_Ellipsoids;
  return (error_code);
} /* End Ellipsoid_Count */


long Ellipsoid_Index ( const char *Code,
                       long *Index )
{ /* Begin Ellipsoid_Index */
/*
 *    Code     : 2-letter ellipsoid code.                      (input)
 *    Index    : Index of the ellipsoid in the ellipsoid table with the 
 *                  specified code                             (output)
 *
 *  The function Ellipsoid_Index returns the index of the ellipsoid in 
 *  the ellipsoid table with the specified code.  If ellipsoid code is found, 
 *  ELLIPSE_NO_ERROR is returned, otherwise ELLIPSE_INVALID_CODE_ERROR is 
 *  returned.
 */
  char temp_code[3];
  long error_code = ELLIPSE_NO_ERROR;
  long i = 0;                   /* index for ellipsoid table */
  long j = 0;
  *Index = 0;
  if (Ellipsoid_Initialized)
  {
    while (j < ELLIPSOID_CODE_LENGTH)
    {
      temp_code[j] = (char)toupper(Code[j]);
      j++;
    }
    temp_code[ELLIPSOID_CODE_LENGTH - 1] = 0;
    while ((i < Number_of_Ellipsoids)
           && strcmp(temp_code, Ellipsoid_Table[i].Code))
    {
      i++;
    }
    if (strcmp(temp_code, Ellipsoid_Table[i].Code))
      error_code |= ELLIPSE_INVALID_CODE_ERROR;
    else
      *Index = i+1;
  }
  else
  {
    error_code |= ELLIPSE_NOT_INITIALIZED_ERROR;
  }
  return (error_code);
} /* End Ellipsoid_Index */


long Ellipsoid_Name ( const long Index,
                      char *Name ) 
{ /* Begin Ellipsoid_Name */
/*
 *    Index   : Index of a given ellipsoid.in the ellipsoid table with the
 *                 specified index                             (input)
 *    Name    : Name of the ellipsoid referenced by index      (output)
 *
 *  The Function Ellipsoid_Name returns the name of the ellipsoid in 
 *  the ellipsoid table with the specified index.  If index is valid, 
 *  ELLIPSE_NO_ERROR is returned, otherwise ELLIPSE_INVALID_INDEX_ERROR is
 *  returned.
 */

  long error_code = ELLIPSE_NO_ERROR;

  strcpy(Name,"");
  if (Ellipsoid_Initialized)
  {
    if ((Index < 1) || (Index > Number_of_Ellipsoids))
      error_code |= ELLIPSE_INVALID_INDEX_ERROR;
    else
      strcpy(Name, Ellipsoid_Table[Index-1].Name);
  }
  else
    error_code |= ELLIPSE_NOT_INITIALIZED_ERROR;
  return (error_code);
} /* End Ellipsoid_Name */


long Ellipsoid_Code ( const long Index,
                      char *Code ) 
{ /* Begin Ellipsoid_Code */
/*
 *    Index    : Index of a given ellipsoid in the ellipsoid table (input)
 *    Code     : 2-letter ellipsoid code.                          (output)
 *
 *  The Function Ellipsoid_Code returns the 2-letter code for the 
 *  ellipsoid in the ellipsoid table with the specified index.  If index is 
 *  valid, ELLIPSE_NO_ERROR is returned, otherwise ELLIPSE_INVALID_INDEX_ERROR 
 *  is returned.
 */

  long error_code = ELLIPSE_NO_ERROR;

  strcpy(Code,"");
  if (Ellipsoid_Initialized)
  {
    if ((Index < 1) || (Index > Number_of_Ellipsoids))
      error_code |= ELLIPSE_INVALID_INDEX_ERROR;
    else
      strcpy(Code, Ellipsoid_Table[Index-1].Code);
  }
  else
    error_code |= ELLIPSE_NOT_INITIALIZED_ERROR;
  return (error_code);
} /* End Ellipsoid_Code */


long Ellipsoid_Parameters ( const long Index,
                            double *a,
                            double *f )
{ /* Begin Ellipsoid_Parameters */
/*
 *    Index    : Index of a given ellipsoid in the ellipsoid table (input)
 *    a        : Semi-major axis, in meters, of ellipsoid          (output)
 *    f        : Flattening of ellipsoid.                          (output)
 *
 *  The function Ellipsoid_Parameters returns the semi-major axis and flattening
 *  for the ellipsoid with the specified index.  If index is valid,
 *  ELLIPSE_NO_ERROR is returned, otherwise ELLIPSE_INVALID_INDEX_ERROR is 
 *  returned.
 */

  long error_code = ELLIPSE_NO_ERROR;

  *a = 0;
  *f = 0;
  if (Ellipsoid_Initialized)
  {
    if ((Index < 1) || (Index > Number_of_Ellipsoids))
    {
      error_code |= ELLIPSE_INVALID_INDEX_ERROR;
    }
    else
    {
      *a = Ellipsoid_Table[Index-1].A;
      *f = 1 / Ellipsoid_Table[Index-1].Recp_F;
    }
  }
  else
    error_code |= ELLIPSE_NOT_INITIALIZED_ERROR;
  return (error_code);
} /* End Ellipsoid_Parameters */


long Ellipsoid_Eccentricity2 ( const long Index,
                               double *e2 )
{ /* Begin Ellipsoid_Eccentricity2 */
/*
 *    Index    : Index of a given ellipsoid in the ellipsoid table (input)
 *    e2       : Square of eccentricity of ellipsoid               (output)
 *
 *  The function Ellipsoid_Eccentricity2 returns the square of the 
 *  eccentricity for the ellipsoid with the specified index.  If index is 
 *  valid, ELLIPSE_NO_ERROR is returned, otherwise ELLIPSE_INVALID_INDEX_ERROR 
 *  is returned.
 */
  double f;
  long error_code = ELLIPSE_NO_ERROR;

  *e2 = 0;
  if (Ellipsoid_Initialized)
  {
    if ((Index < 1) || (Index > Number_of_Ellipsoids))
    {
      error_code |= ELLIPSE_INVALID_INDEX_ERROR;
    }
    else
    {
      f = 1 / Ellipsoid_Table[Index-1].Recp_F;
      *e2 = 2 * f - f * f;
    }
  }
  else
    error_code |= ELLIPSE_NOT_INITIALIZED_ERROR;
  return (error_code);
} /* End Ellipsoid_Eccentricity2 */


long Ellipsoid_User_Defined ( const long Index,
							                long *result )

{ /* Begin Ellipsoid_User_Defined */
/*
 *    Index    : Index of a given ellipsoid in the ellipsoid table (input)
 *    result   : Indicates whether specified ellipsoid is user defined (1)
 *               or not (0)                                        (output)
 *
 *  The function Ellipsoid_User_Defined returns 1 if the ellipsoid is user 
 *  defined.  Otherwise, 0 is returned.  If index is valid ELLIPSE_NO_ERROR is
 *  returned, otherwise ELLIPSE_INVALID_INDEX_ERROR is returned.
 */

  long error_code = ELLIPSE_NO_ERROR;

  *result = FALSE;

  if (Ellipsoid_Initialized)
  {
    if ((Index < 1) || (Index > Number_of_Ellipsoids))
      error_code |= ELLIPSE_INVALID_INDEX_ERROR;
    else
    {
      if (Ellipsoid_Table[Index-1].User_Defined)
        *result = TRUE;
    }
  }
  else
    error_code |= ELLIPSE_NOT_INITIALIZED_ERROR;
  return (error_code);
} /* End Ellipsoid_User_Defined */


long WGS84_Parameters ( double *a,
                        double *f )
{ /* Begin WGS84_Parameters */
/*
 *    a      : Semi-major axis, in meters, of ellipsoid       (output)
 *    f      : Flattening of ellipsoid                        (output)
 *
 *  The function WGS84_Parameters returns the semi-major axis and the
 *  flattening for the WGS84 ellipsoid.  If the ellipsoid table was 
 *  initialized successfully, ELLIPSE_NO_ERROR is returned, otherwise 
 *  ELLIPSE_NOT_INITIALIZED_ERROR is returned.
 */

  long error_code = ELLIPSE_NO_ERROR;

  *a = 0;
  *f = 0;
  if (Ellipsoid_Initialized)
  {
    *a = Ellipsoid_Table[WGS84_Index-1].A;
    *f = 1 / Ellipsoid_Table[WGS84_Index-1].Recp_F;
  }
  else
    error_code |= ELLIPSE_NOT_INITIALIZED_ERROR;
  return (error_code);
} /* End WGS84_Parameters */


long WGS84_Eccentricity2 ( double *e2 )
{ /* Begin WGS84_Eccentricity2 */
/*
 *    e2    : Square of eccentricity of WGS84 ellipsoid      (output)
 *
 *  The function WGS84_Eccentricity2 returns the square of the 
 *  eccentricity for the WGS84 ellipsoid.  If the ellipsoid table was 
 *  initialized successfully, ELLIPSE_NO_ERROR is returned, otherwise 
 *  ELLIPSE_NOT_INITIALIZED_ERROR is returned.
 */

  long error_code = ELLIPSE_NO_ERROR;
  double f;

  *e2 = 0;
  if (Ellipsoid_Initialized)
  {
    f = 1 / Ellipsoid_Table[WGS84_Index-1].Recp_F;
    *e2 = 2 * f - f * f;
  }
  else
    error_code |= ELLIPSE_NOT_INITIALIZED_ERROR;
  return (error_code);
} /* End WGS84_Eccentricity2 */


long WGS72_Parameters( double *a,
                       double *f )
{ /* Begin WGS72_Parameters */
/*
 *    a    : Semi-major axis, in meters, of ellipsoid        (output)
 *    f    : Flattening of ellipsoid                         (output)
 *
 *  The function WGS72_Parameters returns the semi-major axis and the 
 *  flattening for the WGS72 ellipsoid.  If the ellipsoid table was 
 *  initialized successfully, ELLIPSE_NO_ERROR is returned, otherwise 
 *  ELLIPSE_NOT_INITIALIZED_ERROR is returned.
 */

  long error_code = ELLIPSE_NO_ERROR;

  *a = 0;
  *f = 0;
  if (Ellipsoid_Initialized)
  {
    *a = Ellipsoid_Table[WGS72_Index-1].A;
    *f = 1 / Ellipsoid_Table[WGS72_Index-1].Recp_F;
  }
  else
    error_code |= ELLIPSE_NOT_INITIALIZED_ERROR;
  return (error_code);
} /* End WGS72_Parameters */


long WGS72_Eccentricity2 ( double *e2 )
{ /* Begin WGS72_Eccentricity2 */
/*
 *    e2     : Square of eccentricity of WGS84 ellipsoid     (output)
 *
 *  The function WGS72_Eccentricity2 returns the square of the 
 *  eccentricity for the WGS72 ellipsoid.  If the ellipsoid table was 
 *  initialized successfully, ELLIPSE_NO_ERROR is returned, otherwise 
 *  ELLIPSE_NOT_INITIALIZED_ERROR is returned.
 */

  long error_code = ELLIPSE_NO_ERROR;
  double f;

  *e2 = 0;
  if (Ellipsoid_Initialized)
  {
    f = 1 / Ellipsoid_Table[WGS72_Index-1].Recp_F;
    *e2 = 2 * f - f * f;
  }
  else
    error_code |= ELLIPSE_NOT_INITIALIZED_ERROR;
  return (error_code);
} /* End WGS72_Eccentricity2 */


