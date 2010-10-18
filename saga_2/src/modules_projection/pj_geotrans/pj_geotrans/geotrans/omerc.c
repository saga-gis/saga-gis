/***************************************************************************/
/* RSC IDENTIFIER: OBLIQUE MERCATOR
 *
 * ABSTRACT
 *
 *    This component provides conversions between Geodetic coordinates
 *    (latitude and longitude in radians) and Oblique Mercator
 *    projection coordinates (easting and northing in meters).
 *
 * ERROR HANDLING
 *
 *    This component checks parameters for valid values.  If an invalid value
 *    is found the error code is combined with the current error code using
 *    the bitwise or.  This combining allows multiple error codes to be
 *    returned. The possible error codes are:
 *
 *       OMERC_NO_ERROR           : No errors occurred in function
 *       OMERC_LAT_ERROR          : Latitude outside of valid range
 *                                     (-90 to 90 degrees)
 *       OMERC_LON_ERROR          : Longitude outside of valid range
 *                                     (-180 to 360 degrees)
 *       OMERC_ORIGIN_LAT_ERROR   : Origin latitude outside of valid range
 *                                     (-89 to 89 degrees)
 *       OMERC_LAT1_ERROR         : First latitude outside of valid range
 *                                     (-89 to 89 degrees, excluding 0)
 *       OMERC_LAT2_ERROR         : First latitude outside of valid range
 *                                     (-89 to 89 degrees)
 *       OMERC_LON1_ERROR         : First longitude outside of valid range
 *                                     (-180 to 360 degrees)
 *       OMERC_LON2_ERROR         : Second longitude outside of valid range
 *                                     (-180 to 360 degrees)
 *       OMERC_LAT1_LAT2_ERROR    : First and second latitudes can not be equal
 *       OMERC_DIFF_HEMISPHERE_ERROR: First and second latitudes can not be
 *                                     in different hemispheres
 *       OMERC_EASTING_ERROR      : Easting outside of valid range
 *                                     (depends on ellipsoid and projection
 *                                     parameters)
 *       OMERC_NORTHING_ERROR     : Northing outside of valid range
 *                                     (depends on ellipsoid and projection
 *                                     parameters)
 *       OMERC_A_ERROR            : Semi-major axis less than or equal to zero
 *       OMERC_INV_F_ERROR        : Inverse flattening outside of valid range
 *                                     (250 to 350)
 *       OMERC_SCALE_FACTOR_ERROR : Scale factor outside of valid
 *                                     range (0.3 to 3.0)
 *       OMERC_LON_WARNING        : Distortion will result if longitude is 90 degrees or more
 *                                     from the Central Meridian
 *
 * REUSE NOTES
 *
 *    OBLIQUE MERCATOR is intended for reuse by any application that 
 *    performs an Oblique Mercator projection or its inverse.
 *
 * REFERENCES
 *
 *    Further information on OBLIQUE MERCATOR can be found in the Reuse Manual.
 *
 *    OBLIQUE MERCATOR originated from:     U.S. Army Topographic Engineering Center
 *                                          Geospatial Information Division
 *                                          7701 Telegraph Road
 *                                          Alexandria, VA  22310-3864
 *
 * LICENSES
 *
 *    None apply to this component.
 *
 * RESTRICTIONS
 *
 *    OBLIQUE MERCATOR has no restrictions.
 *
 * ENVIRONMENT
 *
 *    OBLIQUE MERCATOR was tested and certified in the following environments:
 *
 *    1. Solaris 2.5 with GCC, version 2.8.1
 *    2. MSDOS with MS Visual C++, version 6
 *
 * MODIFICATIONS
 *
 *    Date              Description
 *    ----              -----------
 *    06-07-00          Original Code
 *    
 *
 */

/***************************************************************************/
/*
 *                               INCLUDES
 */

#include <stdio.h>
#include <math.h>
#include "omerc.h"

/*
 *    math.h     - Standard C math library
 *    omerc.h   - Is for prototype error checking
 */


/***************************************************************************/
/*
 *                               DEFINES
 */

#define PI         3.14159265358979323e0  /* PI                            */
#define PI_OVER_2  ( PI / 2.0)                 
#define PI_OVER_4  ( PI / 4.0)                 
#define TWO_PI     ( 2.0 * PI)                 
#define MIN_SCALE_FACTOR  0.3
#define MAX_SCALE_FACTOR  3.0
#define OMERC_t(lat, e_sinlat, e_over_2)  (tan(PI_OVER_4 - lat / 2.0)) /         \
                      (pow((1 - e_sinlat) / (1 + e_sinlat), e_over_2))

/***************************************************************************/
/*
 *                               GLOBALS
 */
/* Ellipsoid Parameters, default to WGS 84 */
static double OMerc_a = 6378137.0;                   /* Semi-major axis of ellipsoid, in meters */
static double OMerc_f = 1 / 298.257223563;           /* Flattening of ellipsoid */
static double es = 0.08181919084262188000;
static double es_OVER_2 = .040909595421311;
static double OMerc_A = 6383471.9177251;
static double OMerc_B = 1.0008420825413;
static double OMerc_E = 1.0028158089754;
static double OMerc_gamma = .41705894983580;
static double OMerc_azimuth = .60940407333533;       /* Azimuth of central line as it crosses origin lat */
static double OMerc_Origin_Long = -.46732023406900;  /* Longitude at center of projection */
static double cos_gamma = .91428423352628;
static double sin_gamma = .40507325303611;
static double sin_azimuth = .57237890829911;  
static double cos_azimuth = .81998925927985;
static double A_over_B = 6378101.0302010;
static double B_over_A = 1.5678647849335e-7;
static double OMerc_u = 5632885.2272051;             /* Coordinates for center point (uc , vc), vc = 0 */
                                                     /* at center lat and lon */
/* Oblique Mercator projection Parameters */
static double OMerc_Origin_Lat = ((45.0 * PI) / 180.0);/* Latitude of projection center, in radians */
static double OMerc_Lat_1 = ((40.0 * PI) / 180.0);     /* Latitude of first point lying on central line */
static double OMerc_Lon_1 = ((-5.0 * PI) / 180.0);     /* Longitude of first point lying on central line */
static double OMerc_Lat_2 = ((50.0 * PI) / 180.0);     /* Latitude of second point lying on central line */
static double OMerc_Lon_2 = ((5.0 * PI) / 180.0);      /* Longitude of second point lying on central line */
static double OMerc_Scale_Factor = 1.0;                /* Scale factor at projection center */
static double OMerc_False_Northing = 0.0;              /* False northing, in meters, at projection center */
static double OMerc_False_Easting = 0.0;               /* False easting, in meters, at projection center */

static double OMerc_Delta_Northing = 40000000.0;
static double OMerc_Delta_Easting =  40000000.0;
/*
 * These state variables are for optimization purposes.  The only function
 * that should modify them is Set_Oblique_Mercator_Parameters.
 */
/***************************************************************************/
/*
 *                              FUNCTIONS     
 */

long Set_Oblique_Mercator_Parameters(double a,
                                     double f,
                                     double Origin_Latitude,
                                     double Latitude_1,
                                     double Longitude_1,
                                     double Latitude_2,
                                     double Longitude_2,
                                     double False_Easting,
                                     double False_Northing,
                                     double Scale_Factor)
{ /* BEGIN Set_Oblique_Mercator_Parameters */
/*
 * The function Set_Oblique_Mercator_Parameters receives the ellipsoid parameters and
 * projection parameters as inputs, and sets the corresponding state
 * variables.  If any errors occur, the error code(s) are returned by the function, 
 * otherwise OMERC_NO_ERROR is returned.
 *
 *    a                 : Semi-major axis of ellipsoid, in meters  (input)
 *    f                 : Flattening of ellipsoid                  (input)
 *    Origin_Latitude   : Latitude, in radians, at which the       (input)
 *                          point scale factor is 1.0
 *    Latitude_1        : Latitude, in radians, of first point lying on
 *                          central line                           (input)
 *    Longitude_1       : Longitude, in radians, of first point lying on
 *                          central line                           (input)
 *    Latitude_2        : Latitude, in radians, of second point lying on
 *                          central line                           (input)
 *    Longitude_2       : Longitude, in radians, of second point lying on
 *                          central line                           (input)
 *    False_Easting     : A coordinate value, in meters, assigned to the
 *                          central meridian of the projection     (input)
 *    False_Northing    : A coordinate value, in meters, assigned to the
 *                          origin latitude of the projection      (input)
 *    Scale_Factor      : Multiplier which reduces distances in the
 *                          projection to the actual distance on the
 *                          ellipsoid                              (input)
 */

  double inv_f = 1 / f;
  double es2, one_MINUS_es2;
  double cos_olat, cos_olat2;
  double sin_olat, sin_olat2, es2_sin_olat2;
  double t0, t1, t2;
  double D, D2, D2_MINUS_1, sqrt_D2_MINUS_1;
  double H, L, LH;
  double E2;
  double F, G, J, P;
  double dlon;
  long Error_Code = OMERC_NO_ERROR;

  if (a <= 0.0)
  { /* Semi-major axis must be greater than zero */
    Error_Code |= OMERC_A_ERROR;
  }
  if ((inv_f < 250) || (inv_f > 350))
  { /* Inverse flattening must be between 250 and 350 */
    Error_Code |= OMERC_INV_F_ERROR;
  }
  if ((Origin_Latitude <= -PI_OVER_2) || (Origin_Latitude >= PI_OVER_2))
  { /* origin latitude out of range -  can not be at a pole */
    Error_Code |= OMERC_ORIGIN_LAT_ERROR;
  }
  if ((Latitude_1 <= -PI_OVER_2) || (Latitude_1 >= PI_OVER_2))
  { /* first latitude out of range -  can not be at a pole */
    Error_Code |= OMERC_LAT1_ERROR;
  }
  if ((Latitude_2 <= -PI_OVER_2) || (Latitude_2 >= PI_OVER_2))
  { /* second latitude out of range -  can not be at a pole */
    Error_Code |= OMERC_LAT2_ERROR;
  }
  if (Latitude_1 == 0.0)
  { /* first latitude can not be at the equator */
    Error_Code |= OMERC_LAT1_ERROR;
  }
  if (Latitude_1 == Latitude_2)
  { /* first and second latitudes can not be equal */
    Error_Code |= OMERC_LAT1_LAT2_ERROR;
  }
  if (((Latitude_1 < 0.0) && (Latitude_2 > 0.0)) ||
      ((Latitude_1 > 0.0) && (Latitude_2 < 0.0)))
  { /*first and second points can not be in different hemispheres */
    Error_Code |= OMERC_DIFF_HEMISPHERE_ERROR;
  }
  if ((Longitude_1 < -PI) || (Longitude_1 > TWO_PI))
  { /* first longitude out of range */
    Error_Code |= OMERC_LON1_ERROR;
  }
  if ((Longitude_2 < -PI) || (Longitude_2 > TWO_PI))
  { /* first longitude out of range */
    Error_Code |= OMERC_LON2_ERROR;
  }
  if ((Scale_Factor < MIN_SCALE_FACTOR) || (Scale_Factor > MAX_SCALE_FACTOR))
  { /* scale factor out of range */
    Error_Code |= OMERC_SCALE_FACTOR_ERROR;
  }
  if (!Error_Code)
  { /* no errors */

    OMerc_a = a;
    OMerc_f = f;
    OMerc_Origin_Lat = Origin_Latitude;
    OMerc_Lat_1 = Latitude_1;
    OMerc_Lat_2 = Latitude_2;
    OMerc_Lon_1 = Longitude_1;
    OMerc_Lon_2 = Longitude_2;
    OMerc_Scale_Factor = Scale_Factor;
    OMerc_False_Northing = False_Northing;
    OMerc_False_Easting = False_Easting;

    es2 = 2 * OMerc_f - OMerc_f * OMerc_f;
    es = sqrt(es2);
    one_MINUS_es2 = 1 - es2;
    es_OVER_2 = es / 2.0;

    cos_olat = cos(OMerc_Origin_Lat);
    cos_olat2 = cos_olat * cos_olat;
    sin_olat = sin(OMerc_Origin_Lat);
    sin_olat2 = sin_olat * sin_olat;
    es2_sin_olat2 = es2 * sin_olat2;

    OMerc_B = sqrt(1 + (es2 * cos_olat2 * cos_olat2) / one_MINUS_es2);
    OMerc_A = (OMerc_a * OMerc_B * OMerc_Scale_Factor * sqrt(one_MINUS_es2)) / (1.0 - es2_sin_olat2);  
    A_over_B = OMerc_A / OMerc_B;
    B_over_A = OMerc_B / OMerc_A;

    t0 = OMERC_t(OMerc_Origin_Lat, es * sin_olat, es_OVER_2);
    t1 = OMERC_t(OMerc_Lat_1, es * sin(OMerc_Lat_1), es_OVER_2);  
    t2 = OMERC_t(OMerc_Lat_2, es * sin(OMerc_Lat_2), es_OVER_2);  

    D = (OMerc_B * sqrt(one_MINUS_es2)) / (cos_olat * sqrt(1.0 - es2_sin_olat2)); 
    D2 = D * D;
    if (D2 < 1.0)
      D2 = 1.0;
    D2_MINUS_1 = D2 - 1.0;
    sqrt_D2_MINUS_1 = sqrt(D2_MINUS_1);
    if (D2_MINUS_1 > 1.0e-10)
    {
      if (OMerc_Origin_Lat >= 0.0)
        OMerc_E = (D + sqrt_D2_MINUS_1) * pow(t0, OMerc_B);
      else
        OMerc_E = (D - sqrt_D2_MINUS_1) * pow(t0, OMerc_B);
    }
    else
      OMerc_E = D * pow(t0, OMerc_B);
    H = pow(t1, OMerc_B);
    L = pow(t2, OMerc_B);
    F = OMerc_E / H;
    G = (F - 1.0 / F) / 2.0;
    E2 = OMerc_E * OMerc_E;
    LH = L * H;
    J = (E2 - LH) / (E2 + LH);
    P = (L - H) / (L + H);

    dlon = OMerc_Lon_1 - OMerc_Lon_2;
    if (dlon < -PI )
      OMerc_Lon_2 -= TWO_PI;
    if (dlon > PI)
      OMerc_Lon_2 += TWO_PI;
    dlon = OMerc_Lon_1 - OMerc_Lon_2;
    OMerc_Origin_Long = (OMerc_Lon_1 + OMerc_Lon_2) / 2.0 - (atan(J * tan(OMerc_B * dlon / 2.0) / P)) / OMerc_B;

    dlon = OMerc_Lon_1 - OMerc_Origin_Long;
    if (dlon < -PI )
      OMerc_Origin_Long -= TWO_PI;
    if (dlon > PI)
      OMerc_Origin_Long += TWO_PI;
   
    dlon = OMerc_Lon_1 - OMerc_Origin_Long;
    OMerc_gamma = atan(sin(OMerc_B * dlon) / G);
    cos_gamma = cos(OMerc_gamma);
    sin_gamma = sin(OMerc_gamma);
  
    OMerc_azimuth = asin(D * sin_gamma);
    cos_azimuth = cos(OMerc_azimuth);
    sin_azimuth = sin(OMerc_azimuth);

   if (OMerc_Origin_Lat >= 0)
      OMerc_u =  A_over_B * atan(sqrt_D2_MINUS_1/cos_azimuth);
   else
      OMerc_u = -A_over_B * atan(sqrt_D2_MINUS_1/cos_azimuth);

  } /* End if(!Error_Code) */
  return (Error_Code);
} /* End Set_Oblique_Mercator_Parameters */

void Get_Oblique_Mercator_Parameters(double *a,
                                     double *f,
                                     double *Origin_Latitude,
                                     double *Latitude_1,
                                     double *Longitude_1,
                                     double *Latitude_2,
                                     double *Longitude_2,
                                     double *False_Easting,
                                     double *False_Northing,
                                     double *Scale_Factor)
{ /* Begin Get_Oblique_Mercator_Parameters */
/*
 * The function Get_Oblique_Mercator_Parameters returns the current ellipsoid
 * parameters and Oblique Mercator projection parameters.
 *
 *    a                 : Semi-major axis of ellipsoid, in meters  (output)
 *    f                 : Flattening of ellipsoid                  (output)
 *    Origin_Latitude   : Latitude, in radians, at which the       (output)
 *                          point scale factor is 1.0
 *    Latitude_1        : Latitude, in radians, of first point lying on
 *                          central line                           (output)
 *    Longitude_1       : Longitude, in radians, of first point lying on
 *                          central line                           (output)
 *    Latitude_2        : Latitude, in radians, of second point lying on
 *                          central line                           (output)
 *    Longitude_2       : Longitude, in radians, of second point lying on
 *                          central line                           (output)
 *    False_Easting     : A coordinate value, in meters, assigned to the
 *                          central meridian of the projection     (output)
 *    False_Northing    : A coordinate value, in meters, assigned to the
 *                          origin latitude of the projection      (output)
 *    Scale_Factor      : Multiplier which reduces distances in the
 *                          projection to the actual distance on the
 *                          ellipsoid                              (output)
 */

  *a = OMerc_a;
  *f = OMerc_f;
  *Origin_Latitude = OMerc_Origin_Lat;
  *Latitude_1 = OMerc_Lat_1;
  *Longitude_1 = OMerc_Lon_1;
  *Latitude_2 = OMerc_Lat_2;
  *Longitude_2 = OMerc_Lon_2;
  *Scale_Factor = OMerc_Scale_Factor;
  *False_Easting = OMerc_False_Easting;
  *False_Northing = OMerc_False_Northing;
  return;
} /* End Get_Azimuthal_Equidistant_Parameters */

long Convert_Geodetic_To_Oblique_Mercator(double Latitude,
                                          double Longitude,
                                          double *Easting,
                                          double *Northing)
{ /* BEGIN Convert_Geodetic_To_Oblique_Mercator */
/*
 * The function Convert_Geodetic_To_Oblique_Mercator converts geodetic (latitude and
 * longitude) coordinates to Oblique Mercator projection (easting and
 * northing) coordinates, according to the current ellipsoid and Oblique Mercator 
 * projection parameters.  If any errors occur, the error code(s) are returned 
 * by the function, otherwise OMERC_NO_ERROR is returned.
 *
 *    Latitude          : Latitude (phi), in radians           (input)
 *    Longitude         : Longitude (lambda), in radians       (input)
 *    Easting           : Easting (X), in meters               (output)
 *    Northing          : Northing (Y), in meters              (output)
 */

  double dlam, B_dlam, cos_B_dlam;
  double t, S, T, V, U;
  double Q, Q_inv;
  /* Coordinate axes defined with respect to the azimuth of the center line */
  /* Natural origin*/
  double v = 0;
  double u = 0;
  long Error_Code = OMERC_NO_ERROR;

  if ((Latitude < -PI_OVER_2) || (Latitude > PI_OVER_2))
  { /* Latitude out of range */
    Error_Code |= OMERC_LAT_ERROR;
  }
  if ((Longitude < -PI) || (Longitude > TWO_PI))
  { /* Longitude out of range */
    Error_Code |= OMERC_LON_ERROR;
  }
  if (!Error_Code)
  { /* no errors */
    dlam = Longitude - OMerc_Origin_Long;

    if (fabs(dlam) >= PI_OVER_2)
    { /* Distortion will result if Longitude is 90 degrees or more from the Central Meridian */
      Error_Code |= OMERC_LON_WARNING;
    }

    if (dlam > PI)
    {
      dlam -= TWO_PI;
    }
    if (dlam < -PI)
    {
      dlam += TWO_PI;
    }

    if (fabs(fabs(Latitude) - PI_OVER_2) > 1.0e-10)
    {
      t = OMERC_t(Latitude, es * sin(Latitude), es_OVER_2);  
      Q = OMerc_E / pow(t, OMerc_B);
      Q_inv = 1.0 / Q;
      S = (Q - Q_inv) / 2.0;
      T = (Q + Q_inv) / 2.0;
      B_dlam = OMerc_B * dlam;
      V = sin(B_dlam);
      U = ((-1.0 * V * cos_gamma) + (S * sin_gamma)) / T;
      if (fabs(fabs(U) - 1.0) < 1.0e-10)
      { /* Point projects into infinity */
        Error_Code |= OMERC_LON_ERROR;
      }
      else
      {
        v = A_over_B * log((1.0 - U) / (1.0 + U)) / 2.0;
        cos_B_dlam = cos(B_dlam);
        if (fabs(cos_B_dlam) < 1.0e-10)
          u = OMerc_A * B_dlam;
        else
          u = A_over_B * atan(((S * cos_gamma) + (V * sin_gamma)) / cos_B_dlam);
      }
    }
    else
    {
      if (Latitude > 0.0)
        v = A_over_B * log(tan(PI_OVER_4 - (OMerc_gamma / 2.0)));
      else
        v = A_over_B * log(tan(PI_OVER_4 + (OMerc_gamma / 2.0)));
      u = A_over_B * Latitude;
    }


    u = u - OMerc_u;

    *Easting = OMerc_False_Easting + v * cos_azimuth + u * sin_azimuth;
    *Northing = OMerc_False_Northing + u * cos_azimuth - v * sin_azimuth;

  }
  return (Error_Code);
} /* End Convert_Geodetic_To_Oblique_Mercator */


long Convert_Oblique_Mercator_To_Geodetic(double Easting,
                                          double Northing,
                                          double *Latitude,
                                          double *Longitude)
{ /* Begin Convert_Oblique_Mercator_To_Geodetic */
/*
 * The function Convert_Oblique_Mercator_To_Geodetic converts Oblique Mercator projection
 * (easting and northing) coordinates to geodetic (latitude and longitude)
 * coordinates, according to the current ellipsoid and Oblique Mercator projection
 * coordinates.  If any errors occur, the error code(s) are returned by the
 * function, otherwise OMERC_NO_ERROR is returned.
 *
 *    Easting           : Easting (X), in meters                  (input)
 *    Northing          : Northing (Y), in meters                 (input)
 *    Latitude          : Latitude (phi), in radians              (output)
 *    Longitude         : Longitude (lambda), in radians          (output)
 */

  double dx, dy;
  /* Coordinate axes defined with respect to the azimuth of the center line */
  /* Natural origin*/
  double u, v;
  double Q_prime, Q_prime_inv;
  double S_prime, T_prime, V_prime, U_prime;
  double t;
  double es_sin;
  double u_B_over_A;
  double phi;
  double temp_phi = 0.0;
  long Error_Code = OMERC_NO_ERROR; 

  if ((Easting < (OMerc_False_Easting - OMerc_Delta_Easting)) 
      || (Easting > (OMerc_False_Easting + OMerc_Delta_Easting)))
  { /* Easting out of range  */
    Error_Code |= OMERC_EASTING_ERROR;
  }
  if ((Northing < (OMerc_False_Northing - OMerc_Delta_Northing)) 
      || (Northing > (OMerc_False_Northing + OMerc_Delta_Northing)))
  { /* Northing out of range */
    Error_Code |= OMERC_NORTHING_ERROR;
  }

  if (!Error_Code)
  {
    dy = Northing - OMerc_False_Northing;
    dx = Easting - OMerc_False_Easting;
    v = dx * cos_azimuth - dy * sin_azimuth;
    u = dy * cos_azimuth + dx * sin_azimuth;
    u = u + OMerc_u;
    Q_prime = exp(-1.0 * (v * B_over_A ));
    Q_prime_inv = 1.0 / Q_prime;
    S_prime = (Q_prime - Q_prime_inv) / 2.0;
    T_prime = (Q_prime + Q_prime_inv) / 2.0;
    u_B_over_A = u * B_over_A;
    V_prime = sin(u_B_over_A);
    U_prime = (V_prime * cos_gamma + S_prime * sin_gamma) / T_prime;
    if (fabs(fabs(U_prime) - 1.0) < 1.0e-10)
    {
      if (U_prime > 0)
        *Latitude = PI_OVER_2;
      else
        *Latitude = -PI_OVER_2;
      *Longitude = OMerc_Origin_Long;
    }
    else
    {
      t = pow(OMerc_E / sqrt((1.0 + U_prime) / (1.0 - U_prime)), 1.0 / OMerc_B);
      phi = PI_OVER_2 - 2.0 * atan(t);
      while (fabs(phi - temp_phi) > 1.0e-10)
      {
        temp_phi = phi;
        es_sin = es * sin(phi);
        phi = PI_OVER_2 - 2.0 * atan(t * pow((1.0 - es_sin) / (1.0 + es_sin), es_OVER_2));
      }
      *Latitude = phi;
      *Longitude = OMerc_Origin_Long - atan2((S_prime * cos_gamma - V_prime * sin_gamma), cos(u_B_over_A)) / OMerc_B;
    }

    if (fabs(*Latitude) < 2.0e-7)  /* force lat to 0 to avoid -0 degrees */
      *Latitude = 0.0;
    if (*Latitude > PI_OVER_2)  /* force distorted values to 90, -90 degrees */
      *Latitude = PI_OVER_2;
    else if (*Latitude < -PI_OVER_2)
      *Latitude = -PI_OVER_2;

    if (*Longitude > PI)
      *Longitude -= TWO_PI;
    if (*Longitude < -PI)
      *Longitude += TWO_PI;

    if (fabs(*Longitude) < 2.0e-7)  /* force lon to 0 to avoid -0 degrees */
      *Longitude = 0.0;
    if (*Longitude > PI)  /* force distorted values to 180, -180 degrees */
      *Longitude = PI;
    else if (*Longitude < -PI)
      *Longitude = -PI;

    if (fabs(*Longitude - OMerc_Origin_Long) >= PI_OVER_2)
    { /* Distortion will result if Longitude is 90 degrees or more from the Central Meridian */
      Error_Code |= OMERC_LON_WARNING;
    }

  }
  return (Error_Code);
} /* End Convert_Oblique_Mercator_To_Geodetic */

