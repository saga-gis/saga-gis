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
 *                               INCLUDES
 */

#include <string.h>
#include <math.h>
#include "nzmg.h"

/*
 *    string.h    - Standard C string handling library
 *    math.h      - Standard C math library
 *    nzmg.h      - Is for prototype error checking
 */


/***************************************************************************/
/*                               DEFINES 
 *
 */

#define PI              3.14159265358979323e0   /* PI     */
#define PI_OVER_2       (PI / 2.0)                 
#define TWO_PI          (2.0 * PI)                 
#define MAX_LAT         (-33.5 * PI / 180.0)    /* -33.5 degrees */
#define MIN_LAT         (-48.5 * PI / 180.0)    /* -48.5 degrees */
#define MAX_LON         (180.0 * PI / 180.0)    /* 180 degrees */
#define MIN_LON         (165.5 * PI / 180.0)    /* 165.5 degrees */

/**************************************************************************/
/*                               GLOBAL DECLARATIONS
 *
 */

/* Ellipsoid Parameters, must be International  */
static const double NZMG_a = 6378388.0;         /* Semi-major axis of ellipsoid, in meters */
static char NZMG_Ellipsoid_Code[3] = "IN";

/* NZMG projection Parameters */
static const double NZMG_Origin_Lat = (-41.0);               /* Latitude of origin, in radians */
static const double NZMG_Origin_Long = (173.0 * PI / 180.0); /* Longitude of origin, in radians */
static const double NZMG_False_Northing = 6023150.0;         /* False northing, in meters */
static const double NZMG_False_Easting = 2510000.0;          /* False easting, in meters */

/* Maximum variance for easting and northing values for International. */
static double NZMG_Max_Easting = 3170000.0;
static double NZMG_Max_Northing = 6900000.0;
static double NZMG_Min_Easting = 1810000.0;
static double NZMG_Min_Northing = 5160000.0;

static const char* International = "IN";

typedef struct ComplexNumber
{
  double real;
  double imag;
} Complex;

static double A[] = { 0.6399175073, -0.1358797613, 0.063294409,
                      -0.02526853, 0.0117879, -0.0055161,
                      0.0026906, -0.001333, 0.00067, -0.00034 };

static Complex B[] = { { 0.7557853228, 0.0 },
                       { 0.249204646, 0.003371507 },
                       { -0.001541739, 0.041058560 },
                       { -0.10162907, 0.01727609 },
                       { -0.26623489, -0.36249218 },
                       { -0.6870983, -1.1651967 } };

static Complex C[] = { { 1.3231270439, 0.0 },
                       { -0.577245789, -0.007809598 },
                       { 0.508307513, -0.112208952 },
                       { -0.15094762, 0.18200602 },
                       { 1.01418179, 1.64497696 },
                       { 1.9660549, 2.5127645 } };

static double D[] = { 1.5627014243, 0.5185406398, -0.03333098,
                      -0.1052906, -0.0368594, 0.007317,
                      0.01220, 0.00394, -0.0013 };   
                    
/* These state variables are for optimization purposes. The only function
 * that should modify them is Set_NZMG_Parameters.         */

/************************************************************************/
/*                              FUNCTIONS
 *
 */

/* Add two complex numbers */
Complex Add(Complex z1, Complex z2)
{
  Complex z;

  z.real = z1.real + z2.real;
  z.imag = z1.imag + z2.imag;

  return z;
}


/* Multiply two complex numbers */
Complex Multiply(Complex z1, Complex z2)
{
  Complex z;

  z.real = z1.real * z2.real - z1.imag * z2.imag;
  z.imag = z1.imag * z2.real + z1.real * z2.imag;

  return z;
}


/* Divide two complex numbers */
Complex Divide(Complex z1, Complex z2)
{
  Complex z;
  double denom;

  denom = z2.real * z2.real + z2.imag * z2.imag;
  z.real = (z1.real * z2.real + z1.imag * z2.imag) / denom;
  z.imag = (z1.imag * z2.real - z1.real * z2.imag) / denom;

  return z;
}


long Set_NZMG_Parameters(char *Ellipsoid_Code)

{ /* BEGIN Set_NZMG_Parameters */
/*
 * The function Set_NZMG_Parameters receives the ellipsoid code and sets
 * the corresponding state variables. If any errors occur, the error code(s)
 * are returned by the function, otherwise NZMG_NO_ERROR is returned.
 *
 *   Ellipsoid_Code : 2-letter code for ellipsoid           (input)
 */

  long Error_Code = NZMG_NO_ERROR;

  if (strcmp(Ellipsoid_Code, International) != 0)
  { /* Ellipsoid must be International */
    Error_Code |= NZMG_ELLIPSOID_ERROR;
  }

  if (!Error_Code)
  { /* no errors */
    strcpy(NZMG_Ellipsoid_Code, Ellipsoid_Code);
  }
  return (Error_Code);
}  /* END of Set_NZMG_Parameters  */


void Get_NZMG_Parameters(char *Ellipsoid_Code)
                        
{ /* BEGIN Get_NZMG_Parameters */
/*                         
 * The function Get_NZMG_Parameters returns the current ellipsoid
 * code.
 *
 *   Ellipsoid_Code : 2-letter code for ellipsoid          (output)
 */

  strcpy(Ellipsoid_Code, NZMG_Ellipsoid_Code);

  return;
} /* END OF Get_NZMG_Parameters */


long Convert_Geodetic_To_NZMG (double Latitude,
                               double Longitude,
                               double *Easting,
                               double *Northing)

{ /* BEGIN Convert_Geodetic_To_NZMG */
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

  Complex Zeta, z;
  int n;
  double dphi;
  double du, dlam;
  long Error_Code = NZMG_NO_ERROR;

  if ((Latitude < MIN_LAT) || (Latitude > MAX_LAT))
  {  /* Latitude out of range */
    Error_Code|= NZMG_LAT_ERROR;
  }
  if ((Longitude < MIN_LON) || (Longitude > MAX_LON))
  {  /* Longitude out of range */
    Error_Code|= NZMG_LON_ERROR;
  }

  if (!Error_Code)
  { /* no errors */
    dphi = (Latitude * (180.0 / PI) - NZMG_Origin_Lat) * 3600.0 * 1.0e-5;
    du = A[9];
    for (n = 8; n >= 0; n--)
      du = du * dphi + A[n];
    du *= dphi;

    dlam = Longitude - NZMG_Origin_Long;

    Zeta.real = du;
    Zeta.imag = dlam;

    z.real = B[5].real;
    z.imag = B[5].imag;
    for (n = 4; n >= 0; n--)
    {
      z = Multiply(z, Zeta);
      z = Add(B[n], z);
    }
    z = Multiply(z, Zeta);

    *Easting = (z.imag * NZMG_a) + NZMG_False_Easting;
    *Northing = (z.real * NZMG_a) + NZMG_False_Northing;

    if ((*Easting < NZMG_Min_Easting) || (*Easting > NZMG_Max_Easting)) 
      Error_Code |= NZMG_EASTING_ERROR;
    if ((*Northing < NZMG_Min_Northing) || (*Northing > NZMG_Max_Northing))
      Error_Code |= NZMG_NORTHING_ERROR;
  }
  return (Error_Code);
} /* END OF Convert_Geodetic_To_NZMG */


long Convert_NZMG_To_Geodetic(double Easting,
                              double Northing,
                              double *Latitude,
                              double *Longitude)

{ /* Begin Convert_NZMG_To_Geodetic */
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

  int i, n;
  Complex coeff;
  Complex z, Zeta, Zeta_Numer, Zeta_Denom, Zeta_sqr;
  double dphi;
  long Error_Code = NZMG_NO_ERROR; 

  if ((Easting < NZMG_Min_Easting) || (Easting > NZMG_Max_Easting)) 
  { /* Easting out of range  */
    Error_Code |= NZMG_EASTING_ERROR;
  }
  if ((Northing < NZMG_Min_Northing) || (Northing > NZMG_Max_Northing))
  { /* Northing out of range */
    Error_Code |= NZMG_NORTHING_ERROR;
  }

  if (!Error_Code)
  { /* no errors */
    z.real = (Northing - NZMG_False_Northing) / NZMG_a;
    z.imag = (Easting - NZMG_False_Easting) / NZMG_a;

    Zeta.real = C[5].real;
    Zeta.imag = C[5].imag;
    for (n = 4; n >= 0; n--)
    {
      Zeta = Multiply(Zeta, z);
      Zeta = Add(C[n], Zeta);
    }
    Zeta = Multiply(Zeta, z);

    for (i = 0; i < 2; i++)
    {
      Zeta_Numer.real = 5.0 * B[5].real;
      Zeta_Numer.imag = 5.0 * B[5].imag;
      Zeta_Denom.real = 6.0 * B[5].real;
      Zeta_Denom.imag = 6.0 * B[5].imag;
      for (n = 4; n >= 1; n--)
      {
        Zeta_Numer = Multiply(Zeta_Numer, Zeta);
        coeff.real = n * B[n].real;
        coeff.imag = n * B[n].imag;
        Zeta_Numer = Add(coeff, Zeta_Numer); 

        Zeta_Denom = Multiply(Zeta_Denom, Zeta);
        coeff.real = (n+1) * B[n].real;
        coeff.imag = (n+1) * B[n].imag;
        Zeta_Denom = Add(coeff, Zeta_Denom);
      }
      Zeta_sqr = Multiply(Zeta, Zeta);

      Zeta_Numer = Multiply(Zeta_Numer, Zeta_sqr);
      Zeta_Numer = Add(z, Zeta_Numer);

      Zeta_Denom = Multiply(Zeta_Denom, Zeta);
      Zeta_Denom = Add(B[0], Zeta_Denom);

      Zeta = Divide(Zeta_Numer, Zeta_Denom);  
    }
    dphi = D[8];
    for (n = 7; n >= 0; n--)
      dphi = dphi * Zeta.real + D[n];
    dphi *= Zeta.real;
      
    *Latitude = NZMG_Origin_Lat + (dphi * 1.0e5 / 3600.0);
    *Latitude *= PI / 180.0;
    *Longitude = NZMG_Origin_Long + Zeta.imag;

    if ((*Longitude > PI) && (*Longitude - PI < 1.0e-6))
      *Longitude = PI;

    if ((*Latitude < MIN_LAT) || (*Latitude > MAX_LAT))
      Error_Code|= NZMG_LAT_ERROR;
    if ((*Longitude < MIN_LON) || (*Longitude > MAX_LON))
      Error_Code|= NZMG_LON_ERROR;
  }
  return (Error_Code);
} /* END OF Convert_NZMG_To_Geodetic */
