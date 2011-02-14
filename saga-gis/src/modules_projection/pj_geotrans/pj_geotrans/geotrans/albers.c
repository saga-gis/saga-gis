/***************************************************************************/
/* RSC IDENTIFIER: ALBERS
 *
 * ABSTRACT
 *
 *    This component provides conversions between Geodetic coordinates
 *    (latitude and longitude in radians) and Albers Equal Area Conic
 *    projection coordinates (easting and northing in meters) defined
 *    by two standard parallels.
 *
 * ERROR HANDLING
 *
 *    This component checks parameters for valid values.  If an invalid value
 *    is found the error code is combined with the current error code using
 *    the bitwise or.  This combining allows multiple error codes to be
 *    returned. The possible error codes are:
 *
 *       ALBERS_NO_ERROR           : No errors occurred in function
 *       ALBERS_LAT_ERROR          : Latitude outside of valid range
 *                                     (-90 to 90 degrees)
 *       ALBERS_LON_ERROR          : Longitude outside of valid range
 *                                     (-180 to 360 degrees)
 *       ALBERS_EASTING_ERROR      : Easting outside of valid range
 *                                     (depends on ellipsoid and projection
 *                                     parameters)
 *       ALBERS_NORTHING_ERROR     : Northing outside of valid range
 *                                     (depends on ellipsoid and projection
 *                                     parameters)
 *       ALBERS_FIRST_STDP_ERROR   : First standard parallel outside of valid
 *                                     range (-90 to 90 degrees)
 *       ALBERS_SECOND_STDP_ERROR  : Second standard parallel outside of valid
 *                                     range (-90 to 90 degrees)
 *       ALBERS_ORIGIN_LAT_ERROR   : Origin latitude outside of valid range
 *                                     (-90 to 90 degrees)
 *       ALBERS_CENT_MER_ERROR     : Central meridian outside of valid range
 *                                     (-180 to 360 degrees)
 *       ALBERS_A_ERROR            : Semi-major axis less than or equal to zero
 *       ALBERS_INV_F_ERROR        : Inverse flattening outside of valid range
 *									                 (250 to 350)
 *       ALBERS_HEMISPHERE_ERROR   : Standard parallels cannot be opposite
 *                                     latitudes
 *       ALBERS_FIRST_SECOND_ERROR : The 1st & 2nd standard parallels cannot
 *                                   both be 0
 *
 *
 * REUSE NOTES
 *
 *    ALBERS is intended for reuse by any application that performs an Albers
 *    Equal Area Conic projection or its inverse.
 *
 * REFERENCES
 *
 *    Further information on ALBERS can be found in the Reuse Manual.
 *
 *    ALBERS originated from:     U.S. Army Topographic Engineering Center
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
 *    ALBERS has no restrictions.
 *
 * ENVIRONMENT
 *
 *    ALBERS was tested and certified in the following environments:
 *
 *    1. Solaris 2.5 with GCC, version 2.8.1
 *    2. MSDOS with MS Visual C++, version 6
 *
 * MODIFICATIONS
 *
 *    Date              Description
 *    ----              -----------
 *    07-09-99          Original Code
 *    
 *
 */



/***************************************************************************/
/*
 *                               INCLUDES
 */

#include <math.h>
#include "albers.h"

/*
 *    math.h     - Standard C math library
 *    albers.h   - Is for prototype error checking
 */


/***************************************************************************/
/*
 *                               DEFINES
 */

#define PI         3.14159265358979323e0  /* PI                            */
#define PI_OVER_2  ( PI / 2.0)                 
#define TWO_PI     (2.0 * PI)                    
#define ES_SIN(sinlat)          (es * sinlat)
#define ONE_MINUS_SQR(x)        (1.0 - x * x)
#define ALBERS_M(clat,oneminussqressin)   (clat / sqrt(oneminussqressin))
#define ALBERS_Q(slat,oneminussqressin,essin) (one_MINUS_es2)*(slat / (oneminussqressin)-    \
											  (1 / (two_es)) *log((1 - essin) / (1 + essin)))

/***************************************************************************/
/*
 *                               GLOBALS
 */

/* Ellipsoid Parameters, default to WGS 84 */
static double Albers_a = 6378137.0;                   /* Semi-major axis of ellipsoid in meters */
static double Albers_f = 1 / 298.257223563;           /* Flattening of ellipsoid */
static double es = 0.08181919084262188000;            /* Eccentricity of ellipsoid */
static double es2 = 0.0066943799901413800;            /* Eccentricity squared         */
static double C =1.4896626908850;                     /* constant c   */
static double rho0 = 6388749.3391064;                 /* height above ellipsoid		*/
static double n = .70443998701755;                    /* ratio between meridians		*/
static double Albers_a_OVER_n = 9054194.9882824;      /* Albers_a / n */
static double one_MINUS_es2 = .99330562000986;        /* 1 - es2 */
static double two_es = .16363838168524;               /* 2 * es */

/* Albers Projection Parameters */
static double Albers_Origin_Lat = (45 * PI / 180);    /* Latitude of origin in radians     */
static double Albers_Origin_Long = 0.0;               /* Longitude of origin in radians    */
static double Albers_Std_Parallel_1 = (40 * PI / 180);
static double Albers_Std_Parallel_2 = (50 * PI / 180);
static double Albers_False_Easting = 0.0;
static double Albers_False_Northing = 0.0;

static double Albers_Delta_Northing = 40000000;
static double Albers_Delta_Easting =  40000000;
/*
 * These state variables are for optimization purposes.  The only function
 * that should modify them is Set_Albers_Parameters.
 */


/***************************************************************************/
/*
 *                              FUNCTIONS
 */
long Set_Albers_Parameters(double a,
                           double f,
                           double Origin_Latitude,
                           double Central_Meridian,
                           double Std_Parallel_1,
                           double Std_Parallel_2,
                           double False_Easting,
                           double False_Northing)

{ /* BEGIN Set_Albers_Parameters */
/*
 * The function Set_Albers_Parameters receives the ellipsoid parameters and
 * projection parameters as inputs, and sets the corresponding state
 * variables.  If any errors occur, the error code(s) are returned by the function, 
 * otherwise ALBERS_NO_ERROR is returned.
 *
 *    a                 : Semi-major axis of ellipsoid, in meters   (input)
 *    f                 : Flattening of ellipsoid                   (input)
 *    Origin_Latitude   : Latitude in radians at which the          (input)
 *                          point scale factor is 1.0
 *    Central_Meridian  : Longitude in radians at the center of     (input)
 *                          the projection
 *    Std_Parallel_1    : First standard parallel                   (input)
 *    Std_Parallel_2    : Second standard parallel                  (input)
 *    False_Easting     : A coordinate value in meters assigned to the
 *                          central meridian of the projection.     (input)
 *    False_Northing    : A coordinate value in meters assigned to the
 *                          origin latitude of the projection       (input)
 */

  double sin_lat, sin_lat_1, cos_lat;
  double m1, m2, SQRm1;
  double q0, q1, q2;
  double es_sin, one_MINUS_SQRes_sin;
  double nq0;
  double inv_f = 1 / f;
  long Error_Code = ALBERS_NO_ERROR;

  if (a <= 0.0)
  { /* Semi-major axis must be greater than zero */
    Error_Code |= ALBERS_A_ERROR;
  }
  if ((inv_f < 250) || (inv_f > 350))
  { /* Inverse flattening must be between 250 and 350 */
    Error_Code |= ALBERS_INV_F_ERROR;
  }
  if ((Origin_Latitude < -PI_OVER_2) || (Origin_Latitude > PI_OVER_2))
  { /* origin latitude out of range */
    Error_Code |= ALBERS_ORIGIN_LAT_ERROR;
  }
  if ((Central_Meridian < -PI) || (Central_Meridian > TWO_PI))
  { /* origin longitude out of range */
    Error_Code |= ALBERS_CENT_MER_ERROR;
  }
  if ((Std_Parallel_1 < -PI_OVER_2) || (Std_Parallel_1 > PI_OVER_2))
  { /* First Standard Parallel out of range */
    Error_Code |= ALBERS_FIRST_STDP_ERROR;
  }
  if ((Std_Parallel_2 < -PI_OVER_2) || (Std_Parallel_2 > PI_OVER_2))
  { /* Second Standard Parallel out of range */
    Error_Code |= ALBERS_SECOND_STDP_ERROR;
  }
  if ((Std_Parallel_1 == 0.0) && (Std_Parallel_2 == 0.0))
  { /* First & Second Standard Parallels equal 0 */
    Error_Code |= ALBERS_FIRST_SECOND_ERROR;
  }
  if (Std_Parallel_1 == -Std_Parallel_2)
  { /* Parallels are opposite latitudes */
    Error_Code |= ALBERS_HEMISPHERE_ERROR;
  }

  if (!Error_Code)
  { /* no errors */
    Albers_a = a;
    Albers_f = f;
    Albers_Origin_Lat = Origin_Latitude;
    Albers_Std_Parallel_1 = Std_Parallel_1;
    Albers_Std_Parallel_2 = Std_Parallel_2;
    if (Central_Meridian > PI)
      Central_Meridian -= TWO_PI;
    Albers_Origin_Long = Central_Meridian;
    Albers_False_Easting = False_Easting;
    Albers_False_Northing = False_Northing;

    es2 = 2 * Albers_f - Albers_f * Albers_f;
    es = sqrt(es2);
    one_MINUS_es2 = 1 - es2;
    two_es = 2 * es;

    sin_lat = sin(Albers_Origin_Lat);
    es_sin = ES_SIN(sin_lat);
    one_MINUS_SQRes_sin = ONE_MINUS_SQR(es_sin);
    q0 = ALBERS_Q(sin_lat, one_MINUS_SQRes_sin, es_sin);

    sin_lat_1 = sin(Albers_Std_Parallel_1);
    cos_lat = cos(Albers_Std_Parallel_1);
    es_sin = ES_SIN(sin_lat_1);
    one_MINUS_SQRes_sin = ONE_MINUS_SQR(es_sin);
    m1 = ALBERS_M(cos_lat, one_MINUS_SQRes_sin);
    q1 = ALBERS_Q(sin_lat_1, one_MINUS_SQRes_sin, es_sin);

    SQRm1 = m1 * m1;
    if (fabs(Albers_Std_Parallel_1 - Albers_Std_Parallel_2) > 1.0e-10)
    {
      sin_lat = sin(Albers_Std_Parallel_2);
      cos_lat = cos(Albers_Std_Parallel_2);
      es_sin = ES_SIN(sin_lat);
      one_MINUS_SQRes_sin = ONE_MINUS_SQR(es_sin);
      m2 = ALBERS_M(cos_lat, one_MINUS_SQRes_sin);
      q2 = ALBERS_Q(sin_lat, one_MINUS_SQRes_sin, es_sin);
      n = (SQRm1 - m2 * m2) / (q2 - q1);
    }
    else
      n = sin_lat_1;

    C = SQRm1 + n * q1;
    Albers_a_OVER_n = Albers_a / n;
    nq0 = n * q0;
    if (C < nq0)
      rho0 = 0;
    else
      rho0 = Albers_a_OVER_n * sqrt(C - nq0);


  } /* END OF if(!Error_Code) */
  return (Error_Code);
} /* END OF Set_Albers_Parameters */


void Get_Albers_Parameters(double *a,
                           double *f,
                           double *Origin_Latitude,
                           double *Central_Meridian,
                           double *Std_Parallel_1,
                           double *Std_Parallel_2,
                           double *False_Easting,
                           double *False_Northing)

{ /* BEGIN Get_Albers_Parameters */
/*
 * The function Get_Albers_Parameters returns the current ellipsoid
 * parameters, and Albers projection parameters.
 *
 *    a                 : Semi-major axis of ellipsoid, in meters   (output)
 *    f                 : Flattening of ellipsoid										(output)
 *    Origin_Latitude   : Latitude in radians at which the          (output)
 *                          point scale factor is 1.0
 *    Origin_Longitude  : Longitude in radians at the center of     (output)
 *                          the projection
 *    Std_Parallel_1    : First standard parallel                   (output)
 *    Std_Parallel_2    : Second standard parallel                  (output)
 *    False_Easting     : A coordinate value in meters assigned to the
 *                          central meridian of the projection.     (output)
 *    False_Northing    : A coordinate value in meters assigned to the
 *                          origin latitude of the projection       (output)
 */

  *a = Albers_a;
  *f = Albers_f;
  *Origin_Latitude = Albers_Origin_Lat;
  *Std_Parallel_1 = Albers_Std_Parallel_1;
  *Std_Parallel_2 = Albers_Std_Parallel_2;
  *Central_Meridian = Albers_Origin_Long;
  *False_Easting = Albers_False_Easting;
  *False_Northing = Albers_False_Northing;
  return;
} /* END OF Get_Albers_Parameters */


long Convert_Geodetic_To_Albers (double Latitude,
                                 double Longitude,
                                 double *Easting,
                                 double *Northing)

{ /* BEGIN Convert_Geodetic_To_Albers */
/*
 * The function Convert_Geodetic_To_Albers converts geodetic (latitude and
 * longitude) coordinates to Albers projection (easting and northing)
 * coordinates, according to the current ellipsoid and Albers projection
 * parameters.  If any errors occur, the error code(s) are returned by the
 * function, otherwise ALBERS_NO_ERROR is returned.
 *
 *    Latitude          : Latitude (phi) in radians           (input)
 *    Longitude         : Longitude (lambda) in radians       (input)
 *    Easting           : Easting (X) in meters               (output)
 *    Northing          : Northing (Y) in meters              (output)
 */

  double dlam;                      /* Longitude - Central Meridan */
  double sin_lat, cos_lat;
  double es_sin, one_MINUS_SQRes_sin;
  double q;
  double rho;
  double theta;
  double nq;
  long Error_Code = ALBERS_NO_ERROR;

  if ((Latitude < -PI_OVER_2) || (Latitude > PI_OVER_2))
  {  /* Latitude out of range */
    Error_Code |= ALBERS_LAT_ERROR;
  }
  if ((Longitude < -PI) || (Longitude > TWO_PI))
  {  /* Longitude out of range */
    Error_Code|= ALBERS_LON_ERROR;
  }

  if (!Error_Code)
  { /* no errors */

    dlam = Longitude - Albers_Origin_Long;
    if (dlam > PI)
    {
      dlam -= TWO_PI;
    }
    if (dlam < -PI)
    {
      dlam += TWO_PI;
    }
    sin_lat = sin(Latitude);
    cos_lat = cos(Latitude);
    es_sin = ES_SIN(sin_lat);
    one_MINUS_SQRes_sin = ONE_MINUS_SQR(es_sin);
    q = ALBERS_Q(sin_lat, one_MINUS_SQRes_sin, es_sin);
    nq = n * q;
    if (C < nq)
      rho = 0;
    else
      rho = Albers_a_OVER_n * sqrt(C - nq);


    theta = n * dlam;
    *Easting = rho * sin(theta) + Albers_False_Easting;
    *Northing = rho0 - rho * cos(theta) + Albers_False_Northing;
  }
  return (Error_Code);
} /* END OF Convert_Geodetic_To_Albers */


long Convert_Albers_To_Geodetic(double Easting,
                                double Northing,
                                double *Latitude,
                                double *Longitude)
{ /* BEGIN Convert_Albers_To_Geodetic */
/*
 * The function Convert_Albers_To_Geodetic converts Albers projection
 * (easting and northing) coordinates to geodetic (latitude and longitude)
 * coordinates, according to the current ellipsoid and Albers projection
 * coordinates.  If any errors occur, the error code(s) are returned by the
 * function, otherwise ALBERS_NO_ERROR is returned.
 *
 *    Easting           : Easting (X) in meters                  (input)
 *    Northing          : Northing (Y) in meters                 (input)
 *    Latitude          : Latitude (phi) in radians              (output)
 *    Longitude         : Longitude (lambda) in radians          (output)
 */
  double dy, dx;
  double rho0_MINUS_dy;
  double q, qconst, q_OVER_2;
  double rho, rho_n;
  double PHI, Delta_PHI = 1.0;
  double sin_phi;
  double es_sin, one_MINUS_SQRes_sin;
  double theta = 0.0;
  int count = 30;
  double tolerance = 4.85e-10;        /* approximately 1/1000th of
                              an arc second or 1/10th meter */
  long Error_Code = ALBERS_NO_ERROR; 

  if ((Easting < (Albers_False_Easting - Albers_Delta_Easting)) 
      || (Easting > Albers_False_Easting + Albers_Delta_Easting))
  { /* Easting out of range  */
    Error_Code |= ALBERS_EASTING_ERROR;
  }
  if ((Northing < (Albers_False_Northing - Albers_Delta_Northing)) 
      || (Northing > Albers_False_Northing + Albers_Delta_Northing))
  { /* Northing out of range */
    Error_Code |= ALBERS_NORTHING_ERROR;
  }

  if (!Error_Code)
  {
    dy = Northing - Albers_False_Northing;
    dx = Easting - Albers_False_Easting;
    rho0_MINUS_dy = rho0 - dy;
    rho = sqrt(dx * dx + rho0_MINUS_dy * rho0_MINUS_dy);

    if (n < 0)
    {
      rho *= -1.0;
      dy *= -1.0;
      dx *= -1.0;
      rho0_MINUS_dy *= -1.0;
    }

    if (rho != 0.0)
      theta = atan2(dx, rho0_MINUS_dy);
    rho_n = rho * n;
    q = (C - (rho_n * rho_n) / (Albers_a * Albers_a)) / n;
    qconst = 1 - ((one_MINUS_es2) / (two_es)) * log((1.0 - es) / (1.0 + es));
    if (fabs(fabs(qconst) - fabs(q)) > 1.0e-6)
    {
      q_OVER_2 = q / 2.0;
      if (q_OVER_2 > 1.0)
        *Latitude = PI_OVER_2;
      else if (q_OVER_2 < -1.0)
        *Latitude = -PI_OVER_2;
      else
      {
        PHI = asin(q_OVER_2);
        if (es < 1.0e-10)
          *Latitude = PHI;
        else
        {
          while ((fabs(Delta_PHI) > tolerance) && count)
          {
            sin_phi = sin(PHI);
            es_sin = ES_SIN(sin_phi);
            one_MINUS_SQRes_sin = ONE_MINUS_SQR(es_sin);
            Delta_PHI = (one_MINUS_SQRes_sin * one_MINUS_SQRes_sin) / (2.0 * cos(PHI)) *
                        (q / (one_MINUS_es2) - sin_phi / one_MINUS_SQRes_sin +
                         (log((1.0 - es_sin) / (1.0 + es_sin)) / (two_es)));
            PHI += Delta_PHI;
            count --;
          }

          if(!count)
            return Error_Code |= ALBERS_NORTHING_ERROR;

          *Latitude = PHI;
        }

        if (*Latitude > PI_OVER_2)  /* force distorted values to 90, -90 degrees */
          *Latitude = PI_OVER_2;
        else if (*Latitude < -PI_OVER_2)
          *Latitude = -PI_OVER_2;

      }
    }
    else
    {
      if (q >= 0.0)
        *Latitude = PI_OVER_2;
      else
        *Latitude = -PI_OVER_2;
    }
    *Longitude = Albers_Origin_Long + theta / n;

    if (*Longitude > PI)
      *Longitude -= TWO_PI;
    if (*Longitude < -PI)
      *Longitude += TWO_PI;

    if (*Longitude > PI) /* force distorted values to 180, -180 degrees */
      *Longitude = PI;
    else if (*Longitude < -PI)
      *Longitude = -PI;

  }
  return (Error_Code);
} /* END OF Convert_Albers_To_Geodetic */



