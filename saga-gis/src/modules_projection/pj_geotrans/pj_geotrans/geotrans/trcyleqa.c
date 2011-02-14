/**********************************************************
 * Version $Id$
 *********************************************************/
/***************************************************************************/
/* RSC IDENTIFIER: TRANSVERSE CYLINDRICAL EQUAL AREA
 *
 * ABSTRACT
 *
 *    This component provides conversions between Geodetic coordinates
 *    (latitude and longitude in radians) and Transverse Cylindrical Equal Area 
 *    projection coordinates (easting and northing in meters).
 *
 * ERROR HANDLING
 *
 *    This component checks parameters for valid values.  If an invalid value
 *    is found, the error code is combined with the current error code using 
 *    the bitwise or.  This combining allows multiple error codes to be
 *    returned. The possible error codes are:
 *
 *          TCEA_NO_ERROR           : No errors occurred in function
 *          TCEA_LAT_ERROR          : Latitude outside of valid range
 *                                      (-90 to 90 degrees)
 *          TCEA_LON_ERROR          : Longitude outside of valid range
 *                                      (-180 to 360 degrees)
 *          TCEA_EASTING_ERROR      : Easting outside of valid range
 *                                      (False_Easting +/- ~6,500,000 m,
 *                                       depending on ellipsoid parameters
 *                                       and Origin_Latitude)
 *          TCEA_NORTHING_ERROR     : Northing outside of valid range
 *                                      (False_Northing +/- ~20,000,000 m,
 *                                       depending on ellipsoid parameters
 *                                       and Origin_Latitude)
 *          TCEA_ORIGIN_LAT_ERROR   : Origin latitude outside of valid range
 *                                      (-90 to 90 degrees)
 *          TCEA_CENT_MER_ERROR     : Central meridian outside of valid range
 *                                      (-180 to 360 degrees)
 *          TCEA_A_ERROR            : Semi-major axis less than or equal to zero
 *          TCEA_INV_F_ERROR        : Inverse flattening outside of valid range
 *								  	                  (250 to 350)
 *			TCEA_SCALE_FACTOR_ERROR : Scale factor outside of valid
 *                                      range (0.3 to 3.0)
 *		    TCEA_LON_WARNING        : Distortion will result if longitude is more
 *                                     than 90 degrees from the Central Meridian
 *
 * REUSE NOTES
 *
 *    TRANSVERSE CYLINDRICAL EQUAL AREA is intended for reuse by any application that
 *    performs a Transverse Cylindrical Equal Area projection or its inverse.
 *    
 * REFERENCES
 *
 *    Further information on TRANSVERSE CYLINDRICAL EQUAL AREA can be found in the Reuse Manual.
 *
 *    TRANSVERSE CYLINDRICAL EQUAL AREA originated from :  
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
 *    TRANSVERSE CYLINDRICAL EQUAL AREA has no restrictions.
 *
 * ENVIRONMENT
 *
 *    TRANSVERSE CYLINDRICAL EQUAL AREA was tested and certified in the following environments:
 *
 *    1. Solaris 2.5 with GCC, version 2.8.1
 *    2. Windows 95 with MS Visual C++, version 6
 *
 * MODIFICATIONS
 *
 *    Date              Description
 *    ----              -----------
 *    7-15-99          Original Code
 *
 */


/***************************************************************************/
/*
 *                               INCLUDES
 */

#include <math.h>
#include "trcyleqa.h"

/*
 *    math.h     - Standard C math library
 *    trcyleqa.h - Is for prototype error checking
 */


/***************************************************************************/
/*
 *                               DEFINES
 */

#define PI         3.14159265358979323e0  /* PI                            */
#define PI_OVER_2  ( PI / 2.0)                 
#define TWO_PI     (2.0 * PI)
#define TCEA_Q(sinlat,x)                (One_MINUS_es2*(sinlat/(1.0-es2*sinlat*sinlat)-One_OVER_2es*log((1-x)/(1+x))))
#define TCEA_COEFF_TIMES_SIN(coeff,x,latit)         (coeff * sin(x*latit))
#define TCEA_M(c0lat,c1lat,c2lat,c3lat)             (Tcea_a * (c0lat - c1lat + c2lat - c3lat))
#define TCEA_L(Beta,c0lat,c1lat,c2lat)              (Beta + c0lat + c1lat + c2lat)
#define MIN_SCALE_FACTOR  0.3
#define MAX_SCALE_FACTOR  3.0

/***************************************************************************/
/*
 *                               GLOBALS
 */

/* Ellipsoid Parameters, default to WGS 84 */
static double Tcea_a = 6378137.0;                /* Semi-major axis of ellipsoid in meters */
static double Tcea_f = 1 / 298.257223563;        /* Flattening of ellipsoid */
static double es2 = 0.0066943799901413800;       /* Eccentricity (0.08181919084262188000) squared         */
static double es4 = 4.4814723452405e-005;        /* es2 * es2 */
static double es6 = 3.0000678794350e-007;        /* es4 * es2 */
static double es = .081819190842622;             /* sqrt(es2) */
static double M0 = 0.0; 
static double qp = 1.9955310875028;
static double One_MINUS_es2 = .99330562000986;   /* 1.0 - es2 */
static double One_OVER_2es = 6.1110357466348;    /* 1.0 / (2.0 * es) */
static double a0 = .0022392088624809;     /* es2 / 3.0 + 31.0 * es4 / 180.0 + 517.0 * es6 / 5040.0 */
static double a1 = 2.8830839728915e-006;         /*  23.0 * es4 / 360.0 + 251.0 * es6 / 3780.0 */
static double a2 = 5.0331826636906e-009;         /* 761.0 * es6 / 45360.0 */
static double b0 = .0025188265843907;            /* 3.0 * e1 / 2.0 - 27.0 * e3 / 32.0 */
static double b1 = 3.7009490356205e-006;         /* 21.0 * e2 / 16.0 - 55.0 * e4 / 32.0 */
static double b2 = 7.4478137675038e-009;         /* 151.0 * e3 / 96.0 */
static double b3 = 1.7035993238596e-011;         /* 1097.0 * e4 / 512.0 */
static double c0 = .99832429845280;              /* 1.0 - es2 / 4.0 - 3.0 * es4 / 64.0 - 5.0 * es6 / 256.0 */
static double c1 = .0025146070605187;            /* 3.0 * es2 / 8.0 + 3.0 * es4 / 32.0 + 45.0 * es6 / 1024.0 */
static double c2 = 2.6390465943377e-006;         /* 15.0 * es4 / 256.0 + 45.0 * es6 / 1024.0 */
static double c3 = 3.4180460865959e-009;         /* 35.0 * es6 / 3072.0 */

/* Transverse Cylindrical Equal Area projection Parameters */
static double Tcea_Origin_Lat = 0.0;             /* Latitude of origin in radians     */
static double Tcea_Origin_Long = 0.0;            /* Longitude of origin in radians    */
static double Tcea_False_Northing = 0.0;         /* False northing in meters          */
static double Tcea_False_Easting = 0.0;          /* False easting in meters           */
static double Tcea_Scale_Factor = 1.0;           /* Scale factor                      */

/* Maximum variance for easting and northing values for WGS 84.
 */
static double Tcea_Min_Easting = -6398628.0;
static double Tcea_Max_Easting = 6398628.0;
static double Tcea_Min_Northing = -20003931.0;
static double Tcea_Max_Northing = 20003931.0;
/*
 * These state variables are for optimization purposes.  The only function
 * that should modify them is Set_Trans_Cyl_Eq_Area_Parameters.
 */


/***************************************************************************/
/*
 *                              FUNCTIONS
 */


long Set_Trans_Cyl_Eq_Area_Parameters(double a,
                                      double f,
                                      double Origin_Latitude,
                                      double Central_Meridian,
                                      double False_Easting,
                                      double False_Northing,
                                      double Scale_Factor)
{ /* BEGIN Set_Trans_Cyl_Eq_Area_Parameters */
/*
 * The function Set_Trans_Cyl_Eq_Area_Parameters receives the ellipsoid parameters and
 * Transverse Cylindrical Equal Area projection parameters as inputs, and sets the 
 * corresponding state variables.  If any errors occur, the error code(s) are returned 
 * by the function, otherwise TCEA_NO_ERROR is returned.
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
 *    Scale_Factor      : Multiplier which reduces distances in the
 *                          projection to the actual distance on the
 *                          ellipsoid                               (input)
 */

  double sin_lat_90 = sin(PI_OVER_2);
  double x, j, three_es4;
  double Sqrt_One_MINUS_es2;
  double e1, e2, e3, e4;
  double lat, sin2lat, sin4lat, sin6lat;
  double temp, temp_northing;
  double inv_f = 1 / f;
  long Error_Code = TCEA_NO_ERROR;

  if (a <= 0.0)
  { /* Semi-major axis must be greater than zero */
    Error_Code |= TCEA_A_ERROR;
  }
  if ((inv_f < 250) || (inv_f > 350))
  { /* Inverse flattening must be between 250 and 350 */
    Error_Code |= TCEA_INV_F_ERROR;
  }
  if ((Origin_Latitude < -PI_OVER_2) || (Origin_Latitude > PI_OVER_2))
  { /* origin latitude out of range */
    Error_Code |= TCEA_ORIGIN_LAT_ERROR;
  }
  if ((Central_Meridian < -PI) || (Central_Meridian > TWO_PI))
  { /* origin longitude out of range */
    Error_Code |= TCEA_CENT_MER_ERROR;
  }
  if ((Scale_Factor < MIN_SCALE_FACTOR) || (Scale_Factor > MAX_SCALE_FACTOR))
  {
    Error_Code |= TCEA_SCALE_FACTOR_ERROR;
  }

  if (!Error_Code)
  { /* no errors */
    Tcea_a = a;
    Tcea_f = f;
    Tcea_Origin_Lat = Origin_Latitude;
    if (Central_Meridian > PI)
      Central_Meridian -= TWO_PI;
    Tcea_Origin_Long = Central_Meridian;
    Tcea_False_Northing = False_Northing;
    Tcea_False_Easting = False_Easting;
    Tcea_Scale_Factor = Scale_Factor;

    es2 = 2 * Tcea_f - Tcea_f * Tcea_f;
    es = sqrt(es2);
    One_MINUS_es2 = 1.0 - es2;
    Sqrt_One_MINUS_es2 = sqrt(One_MINUS_es2);
    One_OVER_2es = 1.0 / (2.0 * es);
    es4 = es2 * es2;
    es6 = es4 * es2;
    x = es * sin_lat_90;
    qp = TCEA_Q(sin_lat_90,x);

    a0 = es2 / 3.0 + 31.0 * es4 / 180.0 + 517.0 * es6 / 5040.0;
    a1 = 23.0 * es4 / 360.0 + 251.0 * es6 / 3780.0;
    a2 = 761.0 * es6 / 45360.0;

    e1 = (1.0 - Sqrt_One_MINUS_es2) / (1.0 + Sqrt_One_MINUS_es2);
    e2 = e1 * e1;
    e3 = e2 * e1;
    e4 = e3 * e1;
    b0 = 3.0 * e1 / 2.0 - 27.0 * e3 / 32.0;
    b1 = 21.0 * e2 / 16.0 - 55.0 * e4 / 32.0;
    b2 = 151.0 * e3 / 96.0;
    b3 = 1097.0 * e4 / 512.0;

    j = 45.0 * es6 / 1024.0;
    three_es4 = 3.0 * es4;
    c0 = 1.0 - es2 / 4.0 - three_es4 / 64.0 - 5.0 * es6 / 256.0;
    c1 = 3.0 * es2 / 8.0 + three_es4 / 32.0 + j;
    c2 = 15.0 * es4 / 256.0 + j;
    c3 = 35.0 * es6 / 3072.0;
    lat = c0 * Tcea_Origin_Lat;
    sin2lat = TCEA_COEFF_TIMES_SIN(c1, 2.0, Tcea_Origin_Lat);
    sin4lat = TCEA_COEFF_TIMES_SIN(c2, 4.0, Tcea_Origin_Lat);
    sin6lat = TCEA_COEFF_TIMES_SIN(c3, 6.0, Tcea_Origin_Lat);
    M0 = TCEA_M(lat, sin2lat, sin4lat, sin6lat);
    Convert_Geodetic_To_Trans_Cyl_Eq_Area(PI_OVER_2, PI, &temp, &temp_northing);
    if (temp_northing > 0)
    {
      Tcea_Min_Northing = temp_northing - 20003931.458986;
      Tcea_Max_Northing = temp_northing;
    }
    else if (temp_northing < 0)
    {
      Tcea_Max_Northing = temp_northing + 20003931.458986;
      Tcea_Min_Northing = temp_northing;
    }
  } /* END OF if(!Error_Code) */
  return (Error_Code);
} /* END OF Set_Trans_Cyl_Eq_Area_Parameters */

void Get_Trans_Cyl_Eq_Area_Parameters(double *a,
                                      double *f,
                                      double *Origin_Latitude,
                                      double *Central_Meridian,
                                      double *False_Easting,
                                      double *False_Northing,
                                      double *Scale_Factor)
{ /* BEGIN Get_Trans_Cyl_Eq_Area_Parameters */
/*
 * The function Get_Trans_Cyl_Eq_Area_Parameters returns the current ellipsoid
 * parameters, Transverse Cylindrical Equal Area projection parameters, and scale factor.
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
 *    Scale_Factor      : Multiplier which reduces distances in the
 *                          projection to the actual distance on the
 *                          ellipsoid                               (output)
 */

  *a = Tcea_a;
  *f = Tcea_f;
  *Origin_Latitude = Tcea_Origin_Lat;
  *Central_Meridian = Tcea_Origin_Long;
  *False_Easting = Tcea_False_Easting;
  *False_Northing = Tcea_False_Northing;
  *Scale_Factor = Tcea_Scale_Factor;
  return;
} /* END OF Get_Trans_Cyl_Eq_Area_Parameters */


long Convert_Geodetic_To_Trans_Cyl_Eq_Area (double Latitude,
                                            double Longitude,
                                            double *Easting,
                                            double *Northing)
{ /* BEGIN Convert_Geodetic_To_Trans_Cyl_Eq_Area */
/*
 * The function Convert_Geodetic_To_Trans_Cyl_Eq_Area converts geodetic (latitude and
 * longitude) coordinates to Transverse Cylindrical Equal Area projection (easting and
 * northing) coordinates, according to the current ellipsoid and Transverse Cylindrical 
 * Equal Area projection parameters.  If any errors occur, the error code(s) are returned 
 * by the function, otherwise TCEA_NO_ERROR is returned.
 *
 *    Latitude          : Latitude (phi) in radians           (input)
 *    Longitude         : Longitude (lambda) in radians       (input)
 *    Easting           : Easting (X) in meters               (output)
 *    Northing          : Northing (Y) in meters              (output)
 */

  double x;
  double dlam;                      /* Longitude - Central Meridan */
  double sin_lat = sin(Latitude);
  double qq, qq_OVER_qp;
  double beta, betac;
  double sin2betac, sin4betac, sin6betac;
  double PHIc;
  double phi, sin2phi, sin4phi, sin6phi;
  double sinPHIc;
  double Mc;
  long Error_Code = TCEA_NO_ERROR;

  if ((Latitude < -PI_OVER_2) || (Latitude > PI_OVER_2))
  { /* Latitude out of range */
    Error_Code |= TCEA_LAT_ERROR;
  }
  if ((Longitude < -PI) || (Longitude > TWO_PI))
  { /* Longitude out of range */
    Error_Code |= TCEA_LON_ERROR;
  }
  if (!Error_Code)
  { /* no errors */
    dlam = Longitude - Tcea_Origin_Long;
    if (fabs(dlam) >= (PI / 2))
    { /* Distortion will result if Longitude is more than 90 degrees from the Central Meridian */
      Error_Code |= TCEA_LON_WARNING;
    }

    if (dlam > PI)
    {
      dlam -= TWO_PI;
    }
    if (dlam < -PI)
    {
      dlam += TWO_PI;
    }
    if (Latitude == PI_OVER_2)
    {
      qq = qp;
      qq_OVER_qp = 1.0;
    }
    else
    {
      x = es * sin_lat;
      qq = TCEA_Q(sin_lat, x);
      qq_OVER_qp = qq / qp;
    }


    if (qq_OVER_qp > 1.0)
      qq_OVER_qp = 1.0;
    else if (qq_OVER_qp < -1.0)
      qq_OVER_qp = -1.0;

    beta = asin(qq_OVER_qp);
    betac = atan(tan(beta) / cos(dlam));

    if ((fabs(betac) - PI_OVER_2) > 1.0e-8)
      PHIc = betac;
    else
    {
      sin2betac = TCEA_COEFF_TIMES_SIN(a0, 2.0, betac);
      sin4betac = TCEA_COEFF_TIMES_SIN(a1, 4.0, betac);
      sin6betac = TCEA_COEFF_TIMES_SIN(a2, 6.0, betac);
      PHIc = TCEA_L(betac, sin2betac, sin4betac, sin6betac);
    }

    sinPHIc = sin(PHIc);
    *Easting = Tcea_a * cos(beta) * cos(PHIc) * sin(dlam) /
               (Tcea_Scale_Factor * cos(betac) * sqrt(1.0 - es2 *
                                                      sinPHIc * sinPHIc)) + Tcea_False_Easting;

    phi = c0 * PHIc;
    sin2phi = TCEA_COEFF_TIMES_SIN(c1, 2.0, PHIc);
    sin4phi = TCEA_COEFF_TIMES_SIN(c2, 4.0, PHIc);
    sin6phi = TCEA_COEFF_TIMES_SIN(c3, 6.0, PHIc);
    Mc = TCEA_M(phi, sin2phi, sin4phi, sin6phi);

    *Northing = Tcea_Scale_Factor * (Mc - M0) + Tcea_False_Northing;
  }
  return (Error_Code);
} /* END OF Convert_Geodetic_To_Trans_Cyl_Eq_Area */


long Convert_Trans_Cyl_Eq_Area_To_Geodetic(double Easting,
                                           double Northing,
                                           double *Latitude,
                                           double *Longitude)
{ /* BEGIN Convert_Trans_Cyl_Eq_Area_To_Geodetic */
/*
 * The function Convert_Trans_Cyl_Eq_Area_To_Geodetic converts Transverse
 * Cylindrical Equal Area projection (easting and northing) coordinates
 * to geodetic (latitude and longitude) coordinates, according to the
 * current ellipsoid and Transverse Cylindrical Equal Area projection
 * coordinates.  If any errors occur, the error code(s) are returned by the
 * function, otherwise TCEA_NO_ERROR is returned.
 *
 *    Easting           : Easting (X) in meters                  (input)
 *    Northing          : Northing (Y) in meters                 (input)
 *    Latitude          : Latitude (phi) in radians              (output)
 *    Longitude         : Longitude (lambda) in radians          (output)
 */

  double x;
  double dx;     /* Delta easting - Difference in easting (easting-FE)      */
  double dy;     /* Delta northing - Difference in northing (northing-FN)   */
  double Mc;
  double MUc;
  double sin2mu, sin4mu, sin6mu, sin8mu;
  double PHIc;
  double Qc;
  double sin_lat;
  double beta, betac, beta_prime;
  double sin2beta, sin4beta, sin6beta;
  double cosbetac;
  double Qc_OVER_qp;
  double temp;

  long Error_Code = TCEA_NO_ERROR;

  if ((Easting < (Tcea_False_Easting + Tcea_Min_Easting))
      || (Easting > (Tcea_False_Easting + Tcea_Max_Easting)))
  { /* Easting out of range */
    Error_Code |= TCEA_EASTING_ERROR;
  }
  if ((Northing < (Tcea_False_Northing + Tcea_Min_Northing))
      || (Northing > (Tcea_False_Northing + Tcea_Max_Northing)))
  { /* Northing out of range */
    Error_Code |= TCEA_NORTHING_ERROR;
  }
  if (!Error_Code)
  { /* no errors */
    dy = Northing - Tcea_False_Northing;
    dx = Easting - Tcea_False_Easting;
    Mc = M0 + dy / Tcea_Scale_Factor;
    MUc = Mc / (Tcea_a * c0);

    sin2mu = TCEA_COEFF_TIMES_SIN(b0, 2.0, MUc);
    sin4mu = TCEA_COEFF_TIMES_SIN(b1, 4.0, MUc);
    sin6mu = TCEA_COEFF_TIMES_SIN(b2, 6.0, MUc);
    sin8mu = TCEA_COEFF_TIMES_SIN(b3, 8.0, MUc);
    PHIc = MUc + sin2mu + sin4mu + sin6mu + sin8mu;

    sin_lat = sin(PHIc);
    x = es * sin_lat;
    Qc = TCEA_Q(sin_lat, x);
    Qc_OVER_qp = Qc / qp;

    if (Qc_OVER_qp < -1.0)
      Qc_OVER_qp = -1.0;
    else if (Qc_OVER_qp > 1.0)
      Qc_OVER_qp = 1.0;

    betac = asin(Qc_OVER_qp);
    cosbetac = cos(betac);
    temp = Tcea_Scale_Factor * dx * cosbetac * sqrt(1.0 -
                                                    es2 * sin_lat * sin_lat) / (Tcea_a * cos(PHIc));
    if (temp > 1.0)
      temp = 1.0;
    else if (temp < -1.0)
      temp = -1.0;
    beta_prime = -asin(temp);
    beta = asin(cos(beta_prime) * sin(betac));

    sin2beta = TCEA_COEFF_TIMES_SIN(a0, 2.0, beta);
    sin4beta = TCEA_COEFF_TIMES_SIN(a1, 4.0, beta);
    sin6beta = TCEA_COEFF_TIMES_SIN(a2, 6.0, beta);
    *Latitude = TCEA_L(beta, sin2beta, sin4beta, sin6beta);

    *Longitude = Tcea_Origin_Long - atan(tan(beta_prime) / cosbetac);

    if (*Latitude > PI_OVER_2)  /* force distorted values to 90, -90 degrees */
      *Latitude = PI_OVER_2;
    else if (*Latitude < -PI_OVER_2)
      *Latitude = -PI_OVER_2;

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
} /* END OF Convert_Trans_Cyl_Eq_Area_To_Geodetic */
