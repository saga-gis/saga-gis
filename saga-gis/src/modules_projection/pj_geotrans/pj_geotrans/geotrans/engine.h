#ifndef ENGINE_H
  #define ENGINE_H
/***************************************************************************/
/* RSC IDENTIFIER: GEOTRANS ENGINE                 
 *
 * ABSTRACT
 *
 *    This component is the coordinate transformation engine for the GEOTRANS
 *    application.  It provides an external input interface that supports the
 *    GEOTRANS GUIs (Motif and Windows) and the GEOTRANS file processing
 *    component.
 *
 *    This component depends on the DT&CC modules:  DATUM, ELLIPSOID,
 *    GEOCENTRIC, GEOREF, MERCATOR, TRANSVERSE MERCATOR, UTM, MGRS, POLAR
 *    STEREOGRAPHIC, UPS, LAMBERT_1, LAMBERT, ALBERS, AZIMUTHAL EQUIDISTANT, BONNE, 
 *    BRITISH NATIONAL GRID,  CASSINI, CYLINDRICAL EQUAL AREA,ECKERT4, ECKERT6, 
 *    EQUIDISTANT CYLINDRICAL, GNOMONIC, LOCAL CARTESIAN, MILLER, MOLLWEIDE, 
 *    NEYS, NEW ZEALAND MAP GRID, OBLIQUE MERCATOR, ORTHOGRAPHIC, POLYCONIC, 
 *    SINUSOIDAL, STEREOGRAPHIC, TRANSVERSE CYLINDRICAL EQUAL AREA, and VAN DER GRINTEN.
 *
 *
 * ERROR HANDLING
 *
 *    This component checks for error codes returned by the DT&CC modules.
 *    If an error code is returned, it is combined with the current
 *    error code using the bitwise or.  This combining allows multiple error
 *    codes to be returned. The possible error codes are listed below.
 *
 *
 * REUSE NOTES
 *
 *    GEOTRANS ENGINE is intended for reuse by other applications that require 
 *    coordinate conversions between multiple coordinate systems and/or datum 
 *    transformations between multiple datums.
 *
 *
 * REFERENCES
 *
 *    Further information on GEOTRANS ENGINE can be found in the GEOTRANS ENGINE 
 *    Reuse Manual.
 *
 *    GEOTRANS ENGINE originated from :  U.S. Army Topographic Engineering Center
 *                                       Geospatial Information Division
 *                                       7701 Telegraph Road
 *                                       Alexandria, VA  22310-3864
 *
 * LICENSES
 *
 *    None apply to this component.
 *
 * RESTRICTIONS
 *
 *    GEOTRANS ENGINE has no restrictions.
 *
 * ENVIRONMENT
 *
 *    GEOTRANS ENGINE was tested and certified in the following environments:
 *
 *    1. Solaris 2.5 with GCC, version 2.8.1
 *    2. Windows 95 with MS Visual C++, version 6
 *
 * MODIFICATIONS
 *
 *    Date              Description
 *    ----              -----------
 *    04-22-97          Original Code
 *    09-30-99          Added support for 15 new projections
 *    05-30-00          Added support for 2 new projections
 *    06-30-00          Added support for 1 new projection
 *    09-30-00          Added support for 4 new projections
 *    03-24-05          Added support for Lambert Conformal Conic (1 parallel)
 */


/***************************************************************************/
/*
 *                              DEFINES
 */
/* Engine return status codes */
  #define ENGINE_INPUT_WARNING         0x00000001    /* Warning returned by 1st conversion */
  #define ENGINE_INPUT_ERROR           0x00000002    /* Error returned by 1st conversion */
  #define ENGINE_OUTPUT_WARNING        0x00000004    /* Warning returned by 2nd conversion */
  #define ENGINE_OUTPUT_ERROR          0x00000008    /* Error returned by 2nd conversion */
/* Initialization errors */
  #define ENGINE_NOT_INITIALIZED       0x00000010    /* Initialize_Engine has not been called */ 
  #define ENGINE_ELLIPSOID_ERROR       0x00000020    /* Error returned by Ellipsoid module */
  #define ENGINE_DATUM_ERROR           0x00000040    /* Error returned by Datum module */
  #define ENGINE_GEOID_ERROR           0x00000080    /* Error returned by Geoid module*/
/* Interface parameter errors (should never occur) */
  #define ENGINE_INVALID_TYPE          0x00000100    /* Invalid coordinate system type */
  #define ENGINE_INVALID_DIRECTION     0x00000200    /* Invalid direction (input or output) */
  #define ENGINE_INVALID_STATE         0x00000400    /* Invalid state (interactive or file) */
/* Lookup table errors */
  #define ENGINE_INVALID_INDEX_ERROR   0x00001000    /* Index is an invalid value */
  #define ENGINE_INVALID_CODE_ERROR    0x00002000    /* Code was not found in table */
  #define ENGINE_ELLIPSOID_OVERFLOW    0x00004000    /* Ellipsoid table overflow */
  #define ENGINE_DATUM_OVERFLOW        0x00008000    /* 3-parameter datum table overflow */
/* Datum definition errors */
  #define ENGINE_DATUM_SIGMA_ERROR     0x00010000    /* Standard error values must be positive
                                                        (or -1 if unknown) */
  #define ENGINE_DATUM_DOMAIN_ERROR    0x00020000    /* Invalid local datum domain of validity */
/* Ellipsoid errors */
  #define ENGINE_ELLIPSE_IN_USE_ERROR  0x00040000    /* Ellipsoid code is in use by a datum */

/* Datum, ellipsoid deletion error */   
  #define ENGINE_NOT_USERDEF_ERROR     0x00080000    /* Specified code is not user defined */

/* Input/Output conversion status codes */
  #define ENGINE_ORIGIN_LAT_ERROR      0x00000001    /* Invalid Origin Latitude */
  #define ENGINE_CENT_MER_ERROR        0x00000002    /* Invalid Central Meridian */
  #define ENGINE_EASTING_ERROR         0x00000004    /* Invalid Easting */
  #define ENGINE_NORTHING_ERROR        0x00000008    /* Invalid Northing */
  #define ENGINE_RADIUS_ERROR          0x00000010    /* Point too far from center of projection */
  #define ENGINE_HEMISPHERE_ERROR      0x00000020    /* Invalid Hemisphere */                                           
  #define ENGINE_SCALE_FACTOR_ERROR    0x00000040    /* Invalid Scale Factor */
  #define ENGINE_LON_WARNING           0x00000080    /* Longitude too far from Central Meridian */
  #define ENGINE_ORIGIN_LON_ERROR      0x00000100    /* Invalid Origin Longitude */
  #define ENGINE_DATUM_WARNING         0x00000200    /* Point outside datum validity rectangle */
/* Lambert/Albers error conditions */
  #define ENGINE_FIRST_STDP_ERROR      0x00000400    /* Invalid 1st Standard Parallel */
  #define ENGINE_SECOND_STDP_ERROR     0x00000800    /* Invalid 2nd Standard Parallel */
  #define ENGINE_FIRST_SECOND_ERROR    0x00001000    /* 1st & 2nd Standard Parallel cannot be same */
/* UTM/UPS/MGRS specific error conditions */
  #define ENGINE_ZONE_ERROR            0x00002000    /* Invalid UTM Zone */
  #define ENGINE_ZONE_OVERRIDE_ERROR   0x00004000    /* Invalid UTM zone Override */
/* MGRS, GEOREF, British National Grid error condition */
  #define ENGINE_STRING_ERROR          0x00008000    /* Invalid MGRS, GEOREF or BNG String */
/* GEOREF specific error conditions */
  #define ENGINE_STR_LON_MIN_ERROR     0x00010000    /* GEOREF string long. min. error */
  #define ENGINE_STR_LAT_MIN_ERROR     0x00020000    /* GEOREF string lat. min. error */
/* Equidistant Cylindrical/Lambert_1 error condition */   
  #define ENGINE_STDP_ERROR            0x00040000    /* Standard parallel error */
/* Azimuthal Equidistant error condition */
  #define ENGINE_PROJECTION_ERROR      0x00080000    /* Point projects into a circle of radius PI*R */
/* Oblique Mercator error conditions */
  #define ENGINE_LAT1_ERROR            0x00100000    /* Invalid 1st Latitude */
  #define ENGINE_LON1_ERROR            0x00200000    /* Invalid 1st Longitude */
  #define ENGINE_LAT2_ERROR            0x00400000    /* Invalid 2nd Latitude */
  #define ENGINE_LON2_ERROR            0x00800000    /* Invalid 2nd Longitude */
  #define ENGINE_LAT1_LAT2_ERROR       0x01000000    /* 1st & 2nd Latitudes cannot be equal */
/* British National Grid error condition */
  #define ENGINE_INVALID_AREA_ERROR    0x02000000    /* Coordinate outside BNG grid */
/* British National Grid, New Zealand Map Grid error condition */
  #define ENGINE_ELLIPSOID_CODE_ERROR  0x04000000    /* Invalid ellipsoid */

/* Common status codes */
  #define ENGINE_NO_ERROR              0x00000000    /* no error */ 
  #define ENGINE_LAT_ERROR             0x10000000    /* Invalid Latitude */
  #define ENGINE_LON_ERROR             0x20000000    /* Invalid Longitude */
  #define ENGINE_A_ERROR               0x40000000    /* Invalid Ellipsoid Semi-Major Axis */
  #define ENGINE_INV_F_ERROR           0x80000000    /* Invalid Inverse Flattening */

/* Symbolic constants */   
  #define NUMBER_COORD_SYS             33  /* Number of coordinate systems        */
  #define COORD_SYS_CODE_LENGTH         3  /* Length of coordinate system codes (including null) */
  #define COORD_SYS_NAME_LENGTH        50  /* Max length of coordinate system names (including null) */
  #define DATUM_CODE_LENGTH             7  /* Length of datum codes (including null) */
  #define DATUM_NAME_LENGTH            33  /* Max length of datum names (including null) */
  #define ELLIPSOID_CODE_LENGTH         3  /* Length of ellipsoid codes (including null) */
  #define ELLIPSOID_NAME_LENGTH        30  /* Max length of ellipsoid names (including null) */
  #define CONVERT_MSG_LENGTH         2048  /* Max length of coordinate conversion status message */
  #define RETURN_MSG_LENGTH           256  /* Max length of return code status message */

/* Constants used for determining conversion validity */   
  #define RED                           0
  #define YELLOW                        1
  #define GREEN                         2




/***************************************************************************/
/*
 *                          GLOBAL DECLARATIONS
 */

/* State Enumerations */
typedef enum Input_Output
{
  Input = 0,
  Output = 1
} Input_or_Output;

typedef enum File_Interactive
{
  File = 0,
  Interactive = 1
} File_or_Interactive;

/* Coordinate Type Enumeration */
typedef enum Coordinate_Types
{
  Geodetic,
  GEOREF,
  Geocentric,
  Local_Cartesian,    
  MGRS,
  UTM,
  UPS,
  Albers_Equal_Area_Conic,
  Azimuthal_Equidistant,
  BNG,
  Bonne,
  Cassini,
  Cylindrical_Equal_Area,
  Eckert4,
  Eckert6,
  Equidistant_Cylindrical,
  Gnomonic,
  Lambert_Conformal_Conic_1,
  Lambert_Conformal_Conic,
  Mercator,
  Miller_Cylindrical,
  Mollweide,
  Neys,
  NZMG,
  Oblique_Mercator,
  Orthographic,
  Polar_Stereo,
  Polyconic,
  Sinusoidal,
  Stereographic,
  Transverse_Cylindrical_Equal_Area,
  Transverse_Mercator,
  Van_der_Grinten
} Coordinate_Type;

/* Precision Enumeration */
typedef enum Precisions
{
  Degree,                /* 100000m */
  Ten_Minutes,           /* 10000m */
  Minute,                /* 1000m */
  Ten_Seconds,           /* 100m */
  Second,                /* 10m */
  Tenth_of_Second,       /* 1m */
  Hundredth_of_Second,   /* 0.1m */
  Thousandth_of_Second,  /* 0.01 */
  Ten_Thousandth_of_Second  /* 0.001m */
} Precision;

/* Heights */
typedef enum Height_Types
{
  No_Height,
  Ellipsoid_Height,
  Geoid_or_MSL_Height
} Height_Type;

/* Datums */
typedef enum Define_Datum_Types
{
  Three_Param_Datum_Type,
  Seven_Param_Datum_Type,
  WGS84_Datum_Type,
  WGS72_Datum_Type
} Define_Datum_Type; 

/* Geocentric Coordinate Tuple Definition */
typedef struct Geocentric_Tuple_Structure
{
  double  x;   /* meters */
  double  y;   /* meters */
  double  z;   /* meters */
} Geocentric_Tuple;

/* Geodetic Coordinate System Definition */
typedef struct Geodetic_Structure
{
  Height_Type  height_type;
} Geodetic_Parameters;

/* Geodetic Coordinate Tuple Definition */
typedef struct Geodetic_Tuple_Structure
{
  double  longitude;   /* radians */
  double  latitude;    /* radians */
  double  height;      /* meters */
} Geodetic_Tuple;

/* GEOREF Coordinate Tuple Definition */
typedef struct GEOREF_Tuple_Structure
{
  char    string[21];
} GEOREF_Tuple;

/* Albers Equal Area Conic Coordinate System Definition */
typedef struct Albers_Equal_Area_Conic_Structure
{
  double  origin_latitude;   /* radians */
  double  central_meridian;  /* radians */
  double  std_parallel_1;    /* radians */
  double  std_parallel_2;    /* radians */
  double  false_easting;     /* meters */
  double  false_northing;    /* meters */
} Albers_Equal_Area_Conic_Parameters;

/* Albers Equal Area Conic Coordinate Tuple Definition */
typedef struct Albers_Equal_Area_Conic_Tuple_Structure
{
  double  easting;     /* meters */
  double  northing;    /* meters */
} Albers_Equal_Area_Conic_Tuple;

/* Azimuthal Equidistant Coordinate System Definition */
typedef struct Azimuthal_Equidistant_Structure
{
  double  origin_latitude;   /* radians */
  double  central_meridian;  /* radians */
  double  false_easting;     /* meters */
  double  false_northing;    /* meters */
} Azimuthal_Equidistant_Parameters;

/* Azimuthal Equidistant Coordinate Tuple Definition */
typedef struct Azimuthal_Equidistant_Tuple_Structure
{
  double  easting;     /* meters */
  double  northing;    /* meters */
} Azimuthal_Equidistant_Tuple;

/* BNG Coordinate Tuple Definition */
typedef struct BNG_Tuple_Structure
{
  char string[21];
} BNG_Tuple;

/* Bonne Coordinate System Definition */
typedef struct Bonne_Structure
{
  double  origin_latitude;   /* radians */
  double  central_meridian;  /* radians */
  double  false_easting;     /* meters */
  double  false_northing;    /* meters */
} Bonne_Parameters;

/* Bonne Coordinate Tuple Definition */
typedef struct Bonne_Tuple_Structure
{
  double  easting;     /* meters */
  double  northing;    /* meters */
} Bonne_Tuple;

/* Cassini Coordinate System Definition */
typedef struct Cassini_Structure
{
  double  origin_latitude;   /* radians */
  double  central_meridian;  /* radians */
  double  false_easting;     /* meters */
  double  false_northing;    /* meters */
} Cassini_Parameters;

/* Cassini Coordinate Tuple Definition */
typedef struct Cassini_Tuple_Structure
{
  double  easting;     /* meters */
  double  northing;    /* meters */
} Cassini_Tuple;

/* Cylindrical Equal Area Coordinate System Definition */
typedef struct Cylindrical_Equal_Area_Structure
{
  double  origin_latitude;   /* radians */
  double  central_meridian;  /* radians */
  double  false_easting;     /* meters */
  double  false_northing;    /* meters */
} Cylindrical_Equal_Area_Parameters;

/* Cylindrical Equal Area Coordinate Tuple Definition */
typedef struct Cylindrical_Equal_Area_Tuple_Structure
{
  double  easting;     /* meters */
  double  northing;    /* meters */
} Cylindrical_Equal_Area_Tuple;

/* Eckert IV Coordinate System Definition */
typedef struct Eckert4_Structure
{
  double  central_meridian;  /* radians */
  double  false_easting;     /* meters */
  double  false_northing;    /* meters */
} Eckert4_Parameters;

/* Eckert IV Coordinate Tuple Definition */
typedef struct Eckert4_Tuple_Structure
{
  double  easting;     /* meters */
  double  northing;    /* meters */
} Eckert4_Tuple;

/* Eckert VI Coordinate System Definition */
typedef struct Eckert6_Structure
{
  double  central_meridian;  /* radians */
  double  false_easting;     /* meters */
  double  false_northing;    /* meters */
} Eckert6_Parameters;

/* Eckert VI Coordinate Tuple Definition */
typedef struct Eckert6_Tuple_Structure
{
  double  easting;     /* meters */
  double  northing;    /* meters */
} Eckert6_Tuple;

/* Equidistant Cylindrical Coordinate System Definition */
typedef struct Equidistant_Cylindrical_Structure
{
  double  std_parallel;      /* radians */
  double  central_meridian;  /* radians */
  double  false_easting;     /* meters */
  double  false_northing;    /* meters */
} Equidistant_Cylindrical_Parameters;

/* Equidistant Cylindrical Coordinate Tuple Definition */
typedef struct Equidistant_Cylindrical_Tuple_Structure
{
  double  easting;     /* meters */
  double  northing;    /* meters */
} Equidistant_Cylindrical_Tuple;

/* Gnomonic Coordinate System Definition */
typedef struct Gnomonic_Structure
{
  double  origin_latitude;   /* radians */
  double  central_meridian;  /* radians */
  double  false_easting;     /* meters */
  double  false_northing;    /* meters */
} Gnomonic_Parameters;

/* Gnomonic Coordinate Tuple Definition */
typedef struct Gnomonic_Tuple_Structure
{
  double  easting;     /* meters */
  double  northing;    /* meters */
} Gnomonic_Tuple;

/* Lambert Conformal Conic System (1 parallel) Definition */
typedef struct Lambert_Conformal_Conic_1_Structure
{
  double  origin_latitude;   /* radians */
  double  central_meridian;  /* radians */
  double  scale_factor;      /* unitless */
  double  false_easting;     /* meters */
  double  false_northing;    /* meters */
} Lambert_Conformal_Conic_1_Parameters;

/* Lambert Conformal Conic Coordinate (1 parallel) Tuple Definition */
typedef struct Lambert_Conformal_Conic_1_Tuple_Structure
{
  double  easting;     /* meters */
  double  northing;    /* meters */
} Lambert_Conformal_Conic_1_Tuple;

/* Lambert Conformal Conic System Definition */
typedef struct Lambert_Conformal_Conic_Structure
{
  double  origin_latitude;   /* radians */
  double  central_meridian;  /* radians */
  double  std_parallel_1;  /* radians */
  double  std_parallel_2;  /* radians */
  double  false_easting;     /* meters */
  double  false_northing;    /* meters */
} Lambert_Conformal_Conic_Parameters;

/* Lambert Conformal Conic Coordinate Tuple Definition */
typedef struct Lambert_Conformal_Conic_Tuple_Structure
{
  double  easting;     /* meters */
  double  northing;    /* meters */
} Lambert_Conformal_Conic_Tuple;

/* Local Cartesian Coordinate System Definition */
typedef struct Local_Cartesian_Structure
{
  double  origin_latitude;  /* radians */
  double  origin_longitude; /* radians */
  double  origin_height;    /* meters */
  double  orientation;      /* radians clockwise from north */
} Local_Cartesian_Parameters;

/* Local Cartesian Coordinate Tuple Definition */
typedef struct Local_Cartesian_Tuple_Structure
{
  double  x;  /* meters */
  double  y;  /* meters */
  double  z;  /* meters */
} Local_Cartesian_Tuple;

/* Mercator Coordinate System Definition */
typedef struct Mercator_Structure
{
  double  origin_latitude;   /* radians */
  double  central_meridian;  /* radians */
  double  scale_factor;      /* unitless */
  double  false_easting;     /* meters */
  double  false_northing;    /* meters */
} Mercator_Parameters;

/* Mercator Coordinate Tuple Definition */
typedef struct Mercator_Tuple_Structure
{
  double  easting;     /* meters */
  double  northing;    /* meters */
} Mercator_Tuple;

/* MGRS Coordinate Tuple Definition */
typedef struct MGRS_Tuple_Structure
{
  char    string[21];
} MGRS_Tuple;

/* Miller Cylindrical Coordinate System Definition */
typedef struct Miller_Cylindrical_Structure
{
  double  central_meridian;  /* radians */
  double  false_easting;     /* meters */
  double  false_northing;    /* meters */
} Miller_Cylindrical_Parameters;

/* Miller Cylindrical Coordinate Tuple Definition */
typedef struct Miller_Cylindrical_Tuple_Structure
{
  double  easting;     /* meters */
  double  northing;    /* meters */
} Miller_Cylindrical_Tuple;

/* Mollweide Coordinate System Definition */
typedef struct Mollweide_Structure
{
  double  central_meridian;  /* radians */
  double  false_easting;     /* meters */
  double  false_northing;    /* meters */
} Mollweide_Parameters;

/* Mollweide Coordinate Tuple Definition */
typedef struct Mollweide_Tuple_Structure
{
  double  easting;     /* meters */
  double  northing;    /* meters */
} Mollweide_Tuple;

/* Ney's (Modified Lambert Conformal Conic) System Definition */
typedef struct Neys_Structure
{
  double  origin_latitude;   /* radians */
  double  central_meridian;  /* radians */
  double  std_parallel_1;    /* radians */
  double  false_easting;     /* meters */
  double  false_northing;    /* meters */
} Neys_Parameters;

/* Ney's (Modified Lambert Conformal Conic) Coordinate Tuple Definition */
typedef struct Neys_Tuple_Structure
{
  double  easting;     /* meters */
  double  northing;    /* meters */
} Neys_Tuple;

/* New Zealand Map Grid Coordinate Tuple Definition */
typedef struct NZMG_Tuple_Structure
{
  double  easting;     /* meters */
  double  northing;    /* meters */
} NZMG_Tuple;

/* Oblique Mercator Coordinate System Definition */
typedef struct Oblique_Mercator_Structure
{
  double  origin_latitude;   /* radians */
  double  latitude_1;        /* radians */
  double  longitude_1;       /* radians */
  double  latitude_2;        /* radians */
  double  longitude_2;       /* radians */
  double  scale_factor;      /* unitless */
  double  false_easting;     /* meters */
  double  false_northing;    /* meters */
} Oblique_Mercator_Parameters;

/* Oblique Mercator Coordinate Tuple Definition */
typedef struct Oblique_Mercator_Tuple_Structure
{
  double  easting;     /* meters */
  double  northing;    /* meters */
} Oblique_Mercator_Tuple;

/* Orthographic Coordinate System Definition */
typedef struct Orthographic_Structure
{
  double  origin_latitude;   /* radians */
  double  central_meridian;  /* radians */
  double  false_easting;     /* meters */
  double  false_northing;    /* meters */
} Orthographic_Parameters;

/* Orthographic Coordinate Tuple Definition */
typedef struct Orthographic_Tuple_Structure
{
  double  easting;     /* meters */
  double  northing;    /* meters */
} Orthographic_Tuple;

/* Polar Stereographic Coordinate System Definition */
typedef struct Polar_Stereo_Structure
{
  double  latitude_of_true_scale;   /* radians */
  double  longitude_down_from_pole; /* radians */
  double  false_easting;     /* meters */
  double  false_northing;    /* meters */
} Polar_Stereo_Parameters;

/* Polar Stereographic Coordinate Tuple Definition */
typedef struct Polar_Stereo_Tuple_Structure
{
  double  easting;     /* meters */
  double  northing;    /* meters */
} Polar_Stereo_Tuple;

/* Polyconic Coordinate System Definition */
typedef struct Polyconic_Structure
{
  double  origin_latitude;   /* radians */
  double  central_meridian;  /* radians */
  double  false_easting;     /* meters */
  double  false_northing;    /* meters */
} Polyconic_Parameters;

/* Polyconic Coordinate Tuple Definition */
typedef struct Polyconic_Tuple_Structure
{
  double  easting;     /* meters */
  double  northing;    /* meters */
} Polyconic_Tuple;

/* Sinusoidal Coordinate System Definition */
typedef struct Sinusoidal_Structure
{
  double  central_meridian;  /* radians */
  double  false_easting;     /* meters */
  double  false_northing;    /* meters */
} Sinusoidal_Parameters;

/* Sinusoidal Coordinate Tuple Definition */
typedef struct Sinusoidal_Tuple_Structure
{
  double  easting;     /* meters */
  double  northing;    /* meters */
} Sinusoidal_Tuple;

/* Stereographic Coordinate System Definition */
typedef struct Stereographic_Structure
{
  double  origin_latitude;   /* radians */
  double  central_meridian;  /* radians */
  double  false_easting;     /* meters */
  double  false_northing;    /* meters */
} Stereographic_Parameters;

/* Stereographic Coordinate Tuple Definition */
typedef struct Stereographic_Tuple_Structure
{
  double  easting;     /* meters */
  double  northing;    /* meters */
} Stereographic_Tuple;

/* Transverse Cylindrical Equal Area Coordinate System Definition */
typedef struct Transverse_Cylindrical_Equal_Area_Structure
{
  double  origin_latitude;   /* radians */
  double  central_meridian;  /* radians */
  double  scale_factor;      /* unitless */
  double  false_easting;     /* meters */
  double  false_northing;    /* meters */
} Transverse_Cylindrical_Equal_Area_Parameters;

/* Transverse Cylindrical Equal Area Coordinate Tuple Definition */
typedef struct Transverse_Cylindrical_Equal_Area_Tuple_Structure
{
  double  easting;     /* meters */
  double  northing;    /* meters */
} Transverse_Cylindrical_Equal_Area_Tuple;

/* Transverse Mercator Coordinate System Definition */
typedef struct Transverse_Mercator_Structure
{
  double  origin_latitude;   /* radians */
  double  central_meridian;  /* radians */
  double  scale_factor;      /* unitless */
  double  false_easting;     /* meters */
  double  false_northing;    /* meters */
} Transverse_Mercator_Parameters;

/* Transverse Mercator Coordinate Tuple Definition */
typedef struct Transverse_Mercator_Tuple_Structure
{
  double  easting;     /* meters */
  double  northing;    /* meters */
} Transverse_Mercator_Tuple;

/* UPS Coordinate Tuple Definition */
typedef struct UPS_Tuple_Structure
{
  double  easting;     /* meters */
  double  northing;    /* meters */
  char    hemisphere;  /* "N" or "S" */
} UPS_Tuple;

/* UTM Coordinate System Definition */
typedef struct UTM_Structure
{
  long     zone;     /* 1..60 */
  long     override; /* 0 or 1 */
} UTM_Parameters;

/* UTM Coordinate Tuple Definition */
typedef struct UTM_Tuple_Structure
{
  double  easting;     /* meters */
  double  northing;    /* meters */
  long    zone;        /* 1..60 */
  char    hemisphere;  /* "N" or "S" */
}  UTM_Tuple;

/* Van Der Grinten Coordinate System Definition */
typedef struct Van_der_Grinten_Structure
{
  double  central_meridian;  /* radians */
  double  false_easting;     /* meters */
  double  false_northing;    /* meters */
} Van_der_Grinten_Parameters;

/* Van Der Grinten Coordinate Tuple Definition */
typedef struct Van_der_Grinten_Tuple_Structure
{
  double  easting;     /* meters */
  double  northing;    /* meters */
} Van_der_Grinten_Tuple;


/***************************************************************************/
/*
 *                              FUNCTION PROTOTYPES
 *                                for ENGINE.C
 */

/* ensure proper linkage to c++ programs */
  #ifdef __cplusplus
extern "C" {
  #endif



long Initialize_Engine_File(const char *File_Ellipsoids, const char *File_7Parms, const char *File_3Parms, const char *File_Geoid);
long Initialize_Engine();
/*
 *  The function Initialize_Engine sets the initial state of the engine in
 *  in preparation for coordinate conversion and/or datum transformation
 *  operations.
 */


int Valid_Conversion(Coordinate_Type input_system, Coordinate_Type output_system, long input_datum, long output_datum);
/*
 *  The function Valid_Conversion checks the validity of the given conversion based on 
 *  the input/output coordinate systems and datums. The function returns RED if the datums and
 *  coordinate systems are incompatible and the conversion will produce an error, 
 *  YELLOW if the bounding rectangles of the input and output datums do not overlap
 *  and the conversion will produce a warning, GREEN if the conversion is not expected
 *  to produce an error or warning due to the input/output coordinate systems and datums.
 *
 *  input_system    : Input coordinate system                    (input)
 *  output_system   : Output coordinate system                   (input)
 *  input_datum     : Input datum                                (input)
 *  output_datum    : Output datum                               (input)
 */


long Get_Coordinate_System_Count ( long *Count );
/*
 *   Count    : The number of coordinate systems supported. (output) 
 *
 * The function Get_Coordinate_System_Count returns the number of coordinate
 * systems (including projections and grids) that are supported.
 */


long Get_Coordinate_System_Index ( const char *Code,
                                   long *Index );
/*
 *    Code     : 2-letter coordinate system code.              (input)
 *    Index    : Index of the coordinate system with the specified code
 *                                                             (output)
 *
 *  The function Get_Coordinate_System_Index returns the index of the coordinate
 *  system with the specified code.  If the specified Code is found, 
 *  ENGINE_NO_ERROR is returned, otherwise ENGINE_INVALID_CODE_ERROR is 
 *  returned.
 */


long Get_Coordinate_System_Type ( const long Index,
                                  Coordinate_Type *System ); 
/*
 *    Index   : Index of a specific coordinate system            (input)
 *    System  : Type of the coordinate system referencd by index (output)
 *
 *  The Function Get_Coordinate_System_Type returns the type of the coordinate 
 *  system with the specified index.  If theh index is valid, ENGINE_NO_ERROR is 
 *  returned, otherwise ENGINE_INVALID_INDEX_ERROR is returned.
 */


long Get_Coordinate_System_Name ( const long Index, 
                                  char *Name );
/*
 *    Index   : Index of a specific coordinate system            (input)
 *    Name    : Name of the coordinate system referencd by index (output)
 *
 *  The Function Get_Coordinate_System_Name returns the name of the coordinate 
 *  system with the specified index.  If the index is valid, ENGINE_NO_ERROR is 
 *  returned, otherwise ENGINE_INVALID_INDEX_ERROR is returned.
 */


long Get_Coordinate_System_Code ( const long Index,
                                  char *Code );
/*
 *    Index   : Index of a specific coordinate system            (input)
 *    Code    : 2-letter coordinate system code.                 (output)
 *
 *  The Function Get_Coordinate_System_Code returns the 2-letter code for the 
 *  coordinate system with the specified index.  If index is valid, ENGINE_NO_ERROR  
 *  is returned, otherwise ENGINE_INVALID_INDEX_ERROR is returned.
 */


long Set_Coordinate_System
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Coordinate_Type     System );
/*
 *  The function Set_Coordinate_System sets the coordinate system for the 
 *  specified state to the specified coordinate system type.
 *  State      : Indicates whether the coordinate system is to be used for 
 *               interactive or file processing                        (input)
 *  Direction  : Indicates whether the coordinate system is to be used for 
 *               input or output                                       (input)
 *  System     : Identifies the coordinate system to be used           (input)
 */


long Get_Coordinate_System
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Coordinate_Type           *System );
/*
 *  The function Get_Coordinate_System returns the current coordinate system  
 *  type for the specified state.
 *  State      : Indicates whether the coordinate system is to be used for 
 *               interactive or file processing                        (input)
 *  Direction  : Indicates whether the coordinate system is to be used for 
 *               input or output                                       (input)
 *  System     : Identifies current coordinate system type             (output)
 */

long Get_Datum_Count ( long *Count );
/*
 *  The function Get_Datum_Count returns the number of Datums in the table
 *  if the table was initialized without error.
 *
 *  Count   : number of datums in the datum table                   (output)
 */


long Get_Datum_Index ( const char *Code, 
                       long *Index );
/*
 *  The function Get_Datum_Index returns the index of the datum with the 
 *  specified code.
 *
 *  Code    : The datum code being searched for                     (input)
 *  Index   : The index of the datum in the table with the          (output)
 *              specified code
 */


long Get_Datum_Name ( const long Index,
                      char *Name );
/*
 *  The function Get_Datum_Name returns the name of the datum referenced by
 *  index.
 *
 *  Index   : The index of a given datum in the datum table         (input)
 *  Name    : The datum name of the datum referenced by index       (output)
 */


long Get_Datum_Code ( const long Index,
                      char *Code );
/*
 *  The function Get_Datum_Code returns the 5-letter code of the datum
 *  referenced by index.
 *
 *  Index   : The index of a given datum in the datum table         (input)
 *  Code    : The datum code of the datum referenced by index       (output)
 */


long Get_Datum_Ellipsoid_Code ( const long Index,
                                char *Code );
/*
 *  The function Get_Datum_Ellipsoid_Code returns the 2-letter ellipsoid code 
 *  for the ellipsoid associated with the datum referenced by index.
 *
 *  Index   : The index of a given datum in the datum table           (input)
 *  Code    : The ellisoid code for the ellipsoid associated with the (output)
 *               datum referenced by index 
 */


long Get_Datum_Type (const long Index,
                     Define_Datum_Type *Type);
/*
 *  The function Get_Datum_Type returns the type of the datum referenced by
 *  index.
 *
 *  Index   : The index of a given datum in the datum table.        (input)
 *  Type    : The type of datum referenced by index.                (output)
 */


long Get_Datum_Seven_Parameters (const long Index, 
                                 double *Delta_X,                             
                                 double *Delta_Y,
                                 double *Delta_Z,
                                 double *Rx, 
                                 double *Ry, 
                                 double *Rz, 
                                 double *Scale_Factor);
/*
 *   The function Get_Datum_Seven_Parameters returns the seven parameters 
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


long Get_Datum_Three_Parameters (const long Index, 
                                 double *Delta_X,
                                 double *Delta_Y,
                                 double *Delta_Z);
/*
 *   The function Get_Datum_Three_Parameters returns the three parameters
 *   for the datum referenced by index.
 *
 *    Index      : The index of a given datum in the datum table.  (input)
 *    Delta_X    : X translation in meters                         (output)
 *    Delta_Y    : Y translation in meters                         (output)
 *    Delta_Z    : Z translation in meters                         (output)
 */


long Get_Datum_Errors (const long Index, 
                       double *Sigma_X,
                       double *Sigma_Y,
                       double *Sigma_Z);
/*
 *   The function Get_Datum_Errors returns the standard errors in X,Y, & Z 
 *   for the datum referenced by index.
 *
 *    Index      : The index of a given datum in the datum table   (input)
 *    Sigma_X    : Standard error in X in meters                   (output)
 *    Sigma_Y    : Standard error in Y in meters                   (output)
 *    Sigma_Z    : Standard error in Z in meters                   (output)
 */


long Get_Datum_Valid_Rectangle (const long Index,
                                double *South_latitude,
                                double *North_latitude,
                                double *West_longitude,
                                double *East_longitude);
/*
 *   The function Get_Datum_Valid_Rectangle returns the edges of the validity 
 *   rectangle for the datum referenced by index.
 *
 *   Index          : The index of a given datum in the datum table   (input)
 *   South_latitude : Southern edge of validity rectangle in radians  (output)
 *   North_latitude : Northern edge of validity rectangle in radians  (output)
 *   West_longitude : Western edge of validity rectangle in radians   (output)
 *   East_longitude : Eastern edge of validity rectangle in radians   (output)
 */


long Check_Datum_User_Defined ( const long Index,
                                long *result );
/*
 *    Index    : Index of a given datum in the datum table (input)
 *    result   : Indicates whether specified datum is user defined (1)
 *               or not (0)                                (output)
 *
 *  The function Check_Datum_User_Defined checks whether or not the specified datum is 
 *  user defined. It returns 1 if the datum is user defined, and returns
 *  0 otherwise.  
 */


long Check_Valid_Datum(const long Index,
                       double latitude,
                       double longitude,
                       long *result);
/*
 *  The function Check_Valid_Datum checks whether or not the specified location is within the 
 *  validity rectangle for the specified datum.  It returns zero if the specified
 *  location is NOT within the validity rectangle, and returns 1 otherwise.
 *
 *   Index     : The index of a given datum in the datum table      (input)
 *   latitude  : Latitude of the location to be checked in radians  (input)
 *   longitude : Longitude of the location to be checked in radians (input)
 *   result    : Indicates whether location is inside (1) or outside (0)
 *               of the validity rectangle of the specified datum   (output)
 */


long Set_Datum
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const long                Index );
/*
 *  The function Set_Datum sets the datum for the specified state to the 
 *  datum corresponding to the specified index.
 *  State      : Indicates whether the datum is to be used for interactive 
 *               or file processing                                    (input)
 *  Direction  : Indicates whether the datum is to be used for input or 
 *               output                                                (input)
 *  Index      : Identifies the index of the datum to be used          (input)
 */


long Get_Datum
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  long                      *Index );
/*
 *  The function Get_Datum returns the index of the current datum for the 
 *  specified state. 
 *  State      : Indicates whether the datum is to be used for interactive 
 *               or file processing                                    (input)
 *  Direction  : Indicates whether the datum is to be used for input or 
 *               output                                                (input)
 *  Index      : Identifies the index of the current datum             (input)
 */

long Define_Datum ( const char *Code,
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
                    double East_longitude);
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
 * The function Define_Datum creates a new local (3-parameter) datum with the 
 * specified code, name, shift values, and standard error values.  If the 
 * datum table has not been initialized, the specified code is already in use, 
 * or a new version of the 3-param.dat file cannot be created, an error code 
 * is returned, otherwise ENGINE_NO_ERROR is returned.  Note that the indexes 
 * of all datums in the datum table may be changed by this function.
 */


long Remove_Datum (const char *Code);
/*
 *   Code           : 5-letter new datum code.                      (input)
 *
 * The function Remove_Datum deletes a user defined local (3-parameter) datum  
 * with the specified code.  If the datum table has not been initialized, or 
 * a new version of the 3-param.dat file cannot be created, an error code is
 * returned, otherwise ENGINE_NO_ERROR is returned.  Note that the indexes 
 * of all datums in the datum table may be changed by this function.
 */


long Get_Ellipsoid_Count ( long *Count );
/*
 *   Count    : The number of ellipsoids in the ellipsoid table. (output)
 *
 * The function Get_Ellipsoid_Count returns the number of ellipsoids in the
 * ellipsoid table.  If the ellipsoid table has been initialized without error,
 * ENGINE_NO_ERROR is returned, otherwise ENGINE_NOT_INITIALIZED_ERROR
 * is returned.
 */


long Get_Ellipsoid_Index ( const char *Code,
                           long *Index );
/*
 *    Code     : 2-letter ellipsoid code.                      (input)
 *    Index    : Index of the ellipsoid in the ellipsoid table with the 
 *                  specified code                             (output)
 *
 *  The function Get_Ellipsoid_Index returns the index of the ellipsoid in 
 *  the ellipsoid table with the specified code.  If ellipsoid code is found, 
 *  ENGINE_NO_ERROR is returned, otherwise ENGINE_INVALID_CODE_ERROR is 
 *  returned.
 */


long Get_Ellipsoid_Name ( const long Index, 
                          char *Name );
/*
 *    Index   : Index of a given ellipsoid.in the ellipsoid table with the
 *                 specified index                             (input)
 *    Name    : Name of the ellipsoid referencd by index       (output)
 *
 *  The Function Get_Ellipsoid_Name returns the name of the ellipsoid in 
 *  the ellipsoid table with the specified index.  If index is valid, 
 *  ENGINE_NO_ERROR is returned, otherwise ENGINE_INVALID_INDEX_ERROR is
 *  returned.
 */


long Get_Ellipsoid_Parameters ( const long Index,
                                double *a,
                                double *f );
/*
 *    Index    : Index of a given ellipsoid in the ellipsoid table (input)
 *    a        : Semi-major axis, in meters, of ellipsoid          (output)
 *    f        : Flattening of ellipsoid.                          (output)
 *
 *  The function Get_Ellipsoid_Parameters returns the semi-major axis 
 *  and flattening for the ellipsoid with the specified index.  
 */


long Get_Ellipsoid_Code ( const long Index,
                          char *Code );
/*
 *    Index    : Index of a given ellipsoid in the ellipsoid table (input)
 *    Code     : 2-letter ellipsoid code.                          (output)
 *
 *  The Function Get_Ellipsoid_Code returns the 2-letter code for the 
 *  ellipsoid in the ellipsoid table with the specified index.  If index is 
 *  valid, ENGINE_NO_ERROR is returned, otherwise ENGINE_INVALID_INDEX_ERROR 
 *  is returned.
 */


long Get_Ellipsoid_Eccentricity2 ( const long Index,
                                   double *e2 );
/*
 *    Index    : Index of a given ellipsoid in the ellipsoid table (input)
 *    e2       : Square of eccentricity of ellipsoid               (output)
 *
 *  The function Get_Ellipsoid_Eccentricity2 returns the square of the 
 *  eccentricity for the ellipsoid with the specified index.  
 */


long Check_Ellipsoid_User_Defined ( const long Index,
                                    long *result );
/*
 *    Index    : Index of a given ellipsoid in the ellipsoid table (input)
 *    result   : Indicates whether specified ellipsoid is user defined (1)
 *               or not (0)                                        (output)
 *
 *  The function Check_Ellipsoid_User_Defined returns 1 if the ellipsoid is user 
 *  defined.  Otherwise, 0 is returned. 
 */


long Define_Ellipsoid (const char* Code,
                       const char* Name,
                       double a,
                       double f);
/*
 *   Code     : 2-letter ellipsoid code.                      (input)
 *   Name     : Name of the new ellipsoid                     (input)
 *   a        : Semi-major axis, in meters, of new ellipsoid  (input)
 *   f        : Flattening of new ellipsoid.                  (input)
 *
 * The function Define_Ellipsoid creates a new ellipsoid with the specified
 * code, name, and parameters.  If the ellipsoid table has not been initialized,
 * the specified code is already in use, or a new version of the ellips.dat 
 * file cannot be created, an error code is returned, otherwise ENGINE_NO_ERROR 
 * is returned.  Note that the indexes of all ellipsoids in the ellipsoid
 * table may be changed by this function.
 */


long Remove_Ellipsoid (const char* Code);
/*
 *   Code     : 2-letter ellipsoid code.                      (input)
 *
 * The function Remove_Ellipsoid deletes a user defined ellipsoid with 
 * the specified Code.  If the ellipsoid table has not been created,
 * the specified code is in use by a user defined datum, or a new version   
 * of the ellips.dat file cannot be created, an error code is returned, 
 * otherwise ENGINE_NO_ERROR is returned.  Note that the indexes of all  
 * ellipsoids in the ellipsoid table may be changed by this function.
 */


void Set_Precision(Precision Precis);
/*
 *  The function Set_Precision sets the output precision to the specified level. 
 *  Precis     : Indicates the desired level of precision              (input)
 */


void Get_Precision(Precision *Precis);
/*
 *  The function Get_Precision returns the current level of precision. 
 *  Precis     : Indicates the current level of precision              (output)
 */


long Set_Geocentric_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Geocentric_Tuple    coordinates);
/*
 *  The function Set_Geocentric_Coordinates sets the Geocentric coordinates
 *  for the specified state.. 
 *  State            : Indicates whether the coordinates are to be set for  
 *                     interactive or file processing                  (input)
 *  Direction        : Indicates whether the coordinates are to be set for  
 *                     input or output                                 (input)
 *  coordinates      : Geocentric coordinate values to be set          (input)
 */


long Get_Geocentric_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Geocentric_Tuple          *coordinates);
/*
 *  The function Get_Geocentric_Coordinates returns the Geocentric coordinates
 *  for the specified state.. 
 *  State            : Indicates whether the coordinates are to be returned for 
 *                     interactive or file processing                  (input)
 *  Direction        : Indicates whether the coordinates are to be returned for  
 *                     input or output                                 (input)
 *  coordinates      : Geocentric coordinate values to be returned     (output)
 */


long Set_Geodetic_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Geodetic_Parameters parameters);
/*
 *  The function Set_Geodetic_Params sets the Geodetic coordinate system  
 *  parameters for the specified state. 
 *  State        : Indicates whether the parameters are to be set for  
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the parameters are to be set for  
 *                 input or output                                     (input)
 *  parameters   : Geodetic parameters to be set                       (input)
 */


long Get_Geodetic_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Geodetic_Parameters       *parameters);
/*
 *  The function Get_Geodetic_Params returns the Geodetic coordinate system
 *  parameters for the specified state. 
 *  State        : Indicates whether the parameters are to be returned for
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the parameters are to be returned for  
 *                 input or output                                     (input)
 *  parameters   : Geodetic parameters to be returned                  (output)
 */


long Set_Geodetic_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Geodetic_Tuple      coordinates);
/*
 *  The function Set_Geodetic_Coordinates sets the Geodetic coordinates
 *  for the specified state.. 
 *  State          : Indicates whether the parameters are to be set for  
 *                   interactive or file processing                    (input)
 *  Direction      : Indicates whether the parameters are to be set for  
 *                   input or output                                   (input)
 *  coordinates    : Geodetic coordinate values to be set              (input)
 */


long Get_Geodetic_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Geodetic_Tuple            *coordinates);
/*
 *  The function Get_Geodetic_Coordinates returns the Geodetic coordinates
 *  for the specified state.. 
 *  State          : Indicates whether the coordinates are to be returned for  
 *                   interactive or file processing                    (input)
 *  Direction      : Indicates whether the coordinates are to be returned for  
 *                   input or output                                   (input)
 *  coordinates    : Geodetic coordinate values to be returned         (output)
 */


long Set_GEOREF_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const GEOREF_Tuple coordinates);
/*
 *  The function Set_GEOREF_Coordinates sets the GEOREF coordinates
 *  for the specified state.. 
 *  State        : Indicates whether the coordinates are to be set for  
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the coordinates are to be set for  
 *                 input or output                                     (input)
 *  coordinates  : GEOREF coordinate string value to be set            (input)
 */


long Get_GEOREF_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  GEOREF_Tuple              *coordinates);
/*
 *  The function Get_GEOREF_Coordinates returns the GEOREF coordinates
 *  for the specified state. 
 *  State        : Indicates whether the coordinates are to be returned for  
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the coordinates are to be returned for 
 *                 input or output                                     (input)
 *  coordinates  : GEOREF coordinate string value to be returned       (output)
 */


long Set_Albers_Equal_Area_Conic_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Albers_Equal_Area_Conic_Parameters parameters);
/*
 *  The function Set_Albers_Equal_Area_Conic_Params sets the Albers Equal
 *  Area Conic projection parameters for the specified state.
 *  State        : Indicates whether the parameters are to be set for
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the parameters are to be set for
 *                 input or output                                     (input)
 *  parameters   : Albers Equal Area Conic projection parameters to be set 
 *                                                                     (input)
 */


long Get_Albers_Equal_Area_Conic_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Albers_Equal_Area_Conic_Parameters *parameters);
/*
 *  The function Get_Albers_Equal_Area_Conic_Params returns the Albers Equal
 *  Area Conic projection parameters for the specified state.
 *  State        : Indicates whether the parameters are to be returned for
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the parameters are to be returned for
 *                 input or output                                     (input)
 *  parameters   : Albers Equal Area Conic projection parameters to be returned
 *                                                                     (output)
 */


long Set_Albers_Equal_Area_Conic_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Albers_Equal_Area_Conic_Tuple coordinates);
/*
 *  The function Set_Albers_Equal_Area_Conic_Coordinates sets the Albers Equal
 *  Area Conic projection coordinates for the specified state.
 *  State        : Indicates whether the coordinates are to be set for
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the coordinates are to be set for
 *                 input or output                                     (input)
 *  coordinates  : Albers Equal Area Conic projection coordinates to be set 
 *                                                                     (input)
 */


long Get_Albers_Equal_Area_Conic_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Albers_Equal_Area_Conic_Tuple *coordinates);
/*
 *  The function Get_Albers_Equal_Area_Conic_Coordinates returns the Albers Equal
 *  Area Conic projection coordinates for the specified state.
 *  State          : Indicates whether the coordinates are to be returned for
 *                   interactive or file processing                    (input)
 *  Direction      : Indicates whether the coordinates are to be returned for
 *                   input or output                                   (input)
 *  coordinates    : Albers Equal Area Conic projection coordinates to be returned
 *                                                                     (output)
 */


long Set_Azimuthal_Equidistant_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Azimuthal_Equidistant_Parameters parameters);
/*
 *  The function Set_Azimuthal_Equidistant_Params sets the Azimuthal Equidistant
 *  projection parameters for the specified state.
 *  State        : Indicates whether the parameters are to be set for
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the parameters are to be set for
 *                 input or output                                     (input)
 *  parameters   : Azimuthal Equidistant projection parameters to be set 
 *                                                                     (input)
 */


long Get_Azimuthal_Equidistant_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Azimuthal_Equidistant_Parameters *parameters);
/*
 *  The function Get_Azimuthal_Equidistant_Params returns the Azimuthal Equidistant 
 *  projection parameters for the specified state. 
 *  State        : Indicates whether the parameters are to be returned for
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the parameters are to be returned for  
 *                 input or output                                     (input)
 *  parameters   : Azimuthal Equidistant projection parameters to be returned
 *                                                                     (output)
 */


long Set_Azimuthal_Equidistant_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Azimuthal_Equidistant_Tuple coordinates);
/*
 *  The function Set_Azimuthal_Equidistant_Coordinates sets the Azimuthal Equidistant 
 *  projection coordinates for the specified state.
 *  State        : Indicates whether the coordinates are to be set for
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the coordinates are to be set for
 *                 input or output                                     (input)
 *  coordinates  : Azimuthal Equidistant projection coordinates to be set 
 *                                                                     (input)
 */


long Get_Azimuthal_Equidistant_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Azimuthal_Equidistant_Tuple               *coordinates);
/*
 *  The function Get_Azimuthal_Equidistant_Coordinates returns the Azimuthal Equidistant  
 *  projection coordinates for the specified state.
 *  State        : Indicates whether the coordinates are to be returned for
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the coordinates are to be returned for
 *                 input or output                                     (input)
 *  coordinates  : Azimuthal Equidistant projection coordinates to be returned 
 *                                                                     (output)
 */


long Set_BNG_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const BNG_Tuple           coordinates);
/*
 *  The function Set_BNG_Coordinates sets the BNG coordinates
 *  for the specified state.. 
 *  State        : Indicates whether the coordinates are to be set for  
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the coordinates are to be set for  
 *                 input or output                                     (input)
 *  coordinates  : BNG coordinate string value to be set               (input)
 */


long Get_BNG_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  BNG_Tuple                 *coordinates);
/*
 *  The function Get_BNG_Coordinates returns the BNG coordinates
 *  for the specified state. 
 *  State        : Indicates whether the coordinates are to be returned for  
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the coordinates are to be returned for 
 *                 input or output                                     (input)
 *  coordinates  : BNG coordinate string value to be returned          (output)
 */


long Set_Bonne_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Bonne_Parameters    parameters);
/*
 *  The function Set_Bonne_Params sets the Bonne projection parameters 
 *  for the specified state.
 *  State        : Indicates whether the parameters are to be set for
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the parameters are to be set for
 *                 input or output                                     (input)
 *  parameters   : Bonne projection parameters to be set               (input)
 */


long Get_Bonne_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Bonne_Parameters          *parameters);
/*
 *  The function Get_Bonne_Params returns the Bonne projection parameters 
 *  for the specified state. 
 *  State        : Indicates whether the parameters are to be returned for
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the parameters are to be returned for  
 *                 input or output                                     (input)
 *  parameters   : Bonne projection parameters to be returned          (output)
 */


long Set_Bonne_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Bonne_Tuple         coordinates);
/*
 *  The function Set_Bonne_Coordinates sets the Bonne projection 
 *  coordinates for the specified state.
 *  State        : Indicates whether the coordinates are to be set for
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the coordinates are to be set for
 *                 input or output                                     (input)
 *  coordinates  : Bonne projection coordinates to be set              (input)
 */


long Get_Bonne_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Bonne_Tuple               *coordinates);
/*
 *  The function Get_Bonne_Coordinates returns the Bonne projection
 *  coordinates for the specified state. 
 *  State          : Indicates whether the coordinates are to be returned for  
 *                   interactive or file processing                    (input)
 *  Direction      : Indicates whether the coordinates are to be returned for  
 *                   input or output                                   (input)
 *  coordinates    : Bonne projection coordinates to be returned       (output)
 */


long Set_Cassini_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Cassini_Parameters  parameters);
/*
 *  The function Set_Cassini_Params sets the Cassini projection
 *  parameters for the specified state. 
 *  State        : Indicates whether the parameters are to be set for  
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the parameters are to be set for  
 *                 input or output                                     (input)
 *  parameters   : Cassini projection parameters to be set             (input)
 */


long Get_Cassini_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Cassini_Parameters        *parameters);
/*
 *  The function Get_Cassini_Params returns the Cassini projection  
 *  parameters for the specified state. 
 *  State        : Indicates whether the parameters are to be returned for
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the parameters are to be returned for  
 *                 input or output                                     (input)
 *  parameters   : Cassini projection parameters to be returned        (output)
 */


long Set_Cassini_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Cassini_Tuple       coordinates);
/*
 *  The function Set_Cassini_Coordinates sets the Cassini projection
 *  coordinates for the specified state. 
 *  State        : Indicates whether the coordinates are to be set for  
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the coordinates are to be set for  
 *                 input or output                                     (input)
 *  coordinates  : Cassini projection coordinates to be set            (input)
 */


long Get_Cassini_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Cassini_Tuple             *coordinates);
/*
 *  The function Get_Cassini_Coordinates returns the Cassini projection
 *  coordinates for the specified state. 
 *  State          : Indicates whether the coordinates are to be returned for  
 *                   interactive or file processing                    (input)
 *  Direction      : Indicates whether the coordinates are to be returned for  
 *                   input or output                                   (input)
 *  coordinates    : Cassini projection coordinates to be returned     (output)
 */


long Set_Cylindrical_Equal_Area_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Cylindrical_Equal_Area_Parameters parameters);
/*
 *  The function Set_Cylindrical_Equal_Area_Params sets the Cylindrical 
 *  Equal Area projection parameters for the specified state.
 *  State        : Indicates whether the parameters are to be set for
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the parameters are to be set for
 *                 input or output                                     (input)
 *  parameters   : Cylindrical Equal Area projection parameters to be set
 *                                                                     (input)
 */


long Get_Cylindrical_Equal_Area_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
    Cylindrical_Equal_Area_Parameters *parameters);
/*
 *  The function Get_Cylindrical_Equal_Area_Params returns the Cylindrical  
 *  Equal Area projection parameters for the specified state.
 *  State        : Indicates whether the parameters are to be returned for
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the parameters are to be returned for
 *                 input or output                                     (input)
 *  parameters   : Cylindrical Equal Area projection parameters to be returned
 *                                                                     (output)
 */


long Set_Cylindrical_Equal_Area_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Cylindrical_Equal_Area_Tuple coordinates);
/*
 *  The function Set_Cylindrical_Equal_Area_Coordinates sets the Cylindrical 
 *  Equal Area projection coordinates for the specified state.
 *  State        : Indicates whether the coordinates are to be set for
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the coordinates are to be set for
 *                 input or output                                     (input)
 *  coordinates  : Cylindrical Equal Area projection coordinates to be set
 *                                                                     (input)
 */


long Get_Cylindrical_Equal_Area_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Cylindrical_Equal_Area_Tuple *coordinates);
/*
 *  The function Get_Cylindrical_Equal_Area_Coordinates returns the Cylindrical 
 *  Equal Area projection coordinates for the specified state.
 *  State          : Indicates whether the coordinates are to be returned for
 *                   interactive or file processing                    (input)
 *  Direction      : Indicates whether the coordinates are to be returned for
 *                   input or output                                   (input)
 *  coordinates    : Cylindrical Equal Area projection coordinates to be returned
 *                                                                     (output)
 */


long Set_Eckert4_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Eckert4_Parameters  parameters);
/*
 *  The function Set_Eckert4_Params sets the Eckert IV projection 
 *  parameters for the specified state.
 *  State        : Indicates whether the parameters are to be set for
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the parameters are to be set for
 *                 input or output                                     (input)
 *  parameters   : Eckert IV projection parameters to be set           (input)
 */


long Get_Eckert4_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Eckert4_Parameters        *parameters);
/*
 *  The function Get_Eckert4_Params returns the Eckert IV projection 
 *  parameters for the specified state.
 *  State        : Indicates whether the parameters are to be returned for
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the parameters are to be returned for
 *                 input or output                                     (input)
 *  parameters   : Eckert IV projection parameters to be returned      (output)
 */


long Set_Eckert4_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Eckert4_Tuple       coordinates);
/*
 *  The function Set_Eckert4_Coordinates sets the Eckert IV projection 
 *  coordinates for the specified state.
 *  State        : Indicates whether the coordinates are to be set for
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the coordinates are to be set for
 *                 input or output                                     (input)
 *  coordinates  : Eckert IV projection coordinates to be set          (input)
 */


long Get_Eckert4_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Eckert4_Tuple             *coordinates);
/*
 *  The function Get_Eckert4_Coordinates returns the Eckert IV projection 
 *  coordinates for the specified state.
 *  State          : Indicates whether the coordinates are to be returned for
 *                   interactive or file processing                    (input)
 *  Direction      : Indicates whether the coordinates are to be returned for
 *                   input or output                                   (input)
 *  coordinates    : Eckert IV projection coordinates to be returned   (output)
 */


long Set_Eckert6_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Eckert6_Parameters  parameters);
/*
 *  The function Set_Eckert6_Params sets the Eckert VI projection parameters for the specified state.
 *  State        : Indicates whether the parameters are to be set for
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the parameters are to be set for
 *                 input or output                                     (input)
 *  parameters   : Eckert VI projection parameters to be set           (input)
 */


long Get_Eckert6_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Eckert6_Parameters        *parameters);
/*
 *  The function Get_Eckert6_Params returns the Eckert VI projection 
 *  parameters for the specified state.
 *  State        : Indicates whether the parameters are to be returned for
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the parameters are to be returned for
 *                 input or output                                     (input)
 *  parameters   : Eckert VI projection parameters to be returned      (output)
 */


long Set_Eckert6_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Eckert6_Tuple       coordinates);
/*
 *  The function Set_Eckert6_Coordinates sets the Eckert VI projection 
 *  coordinates for the specified state.
 *  State        : Indicates whether the coordinates are to be set for
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the coordinates are to be set for
 *                 input or output                                     (input)
 *  coordinates  : Eckert VI projection coordinates to be set          (input)
 */


long Get_Eckert6_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Eckert6_Tuple             *coordinates);
/*
 *  The function Get_Eckert6_Coordinates returns the Eckert VI projection 
 *  coordinates for the
 *  specified state.
 *  State          : Indicates whether the coordinates are to be returned for
 *                   interactive or file processing                    (input)
 *  Direction      : Indicates whether the coordinates are to be returned for
 *                   input or output                                   (input)
 *  coordinates    : Eckert VI projection coordinates to be returned   (output)
 */


long Set_Equidistant_Cylindrical_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Equidistant_Cylindrical_Parameters parameters);
/*
 *  The function Set_Equidistant_Cylindrical_Params sets the Equidistant 
 *  Cylindrical projection parameters for the specified state. 
 *  State        : Indicates whether the parameters are to be set for  
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the parameters are to be set for  
 *                 input or output                                     (input)
 *  parameters   : Equidistant Cylindrical projection parameters to be set
 *                                                                     (input)
 */


long Get_Equidistant_Cylindrical_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Equidistant_Cylindrical_Parameters *parameters);
/*
 *  The function Get_Equidistant_Cylindrical_Params returns the Equidistant 
 *  Cylindrical projection parameters for the specified state. 
 *  State        : Indicates whether the parameters are to be returned for
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the parameters are to be returned for  
 *                 input or output                                     (input)
 *  parameters   : Equidistant Cylindrical projection parameters to be returned
 *                                                                     (output)
 */


long Set_Equidistant_Cylindrical_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Equidistant_Cylindrical_Tuple coordinates);
/*
 *  The function Set_Equidistant_Cylindrical_Coordinates sets the Equidistant 
 *  Cylindrical projection coordinates for the specified state. 
 *  State        : Indicates whether the coordinates are to be set for  
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the coordinates are to be set for  
 *                 input or output                                     (input)
 *  coordinates  : Equidistant Cylindrical projection coordinates to be set
 *                                                                     (input)
 */


long Get_Equidistant_Cylindrical_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Equidistant_Cylindrical_Tuple *coordinates);
/*
 *  The function Get_Equidistant_Cylindrical_Coordinates returns the Equidistant 
 *  Cylindrical projection coordinates for the specified state. 
 *  State          : Indicates whether the coordinates are to be returned for  
 *                   interactive or file processing                    (input)
 *  Direction      : Indicates whether the coordinates are to be returned for  
 *                   input or output                                   (input)
 *  coordinates    : Equidistant Cylindrical projection coordinates to be returned 
 *                                                                     (output)
 */


long Set_Gnomonic_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Gnomonic_Parameters parameters);
/*
 *  The function Set_Gnomonic_Params sets the Gnomonic
 *  projection parameters for the specified state.
 *  State        : Indicates whether the parameters are to be set for
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the parameters are to be set for
 *                 input or output                                     (input)
 *  parameters   : Gnomonic projection parameters to be set            (input)
 */


long Get_Gnomonic_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Gnomonic_Parameters       *parameters);
/*
 *  The function Get_Gnomonic_Params returns the Gnomonic 
 *  projection parameters for the specified state. 
 *  State        : Indicates whether the parameters are to be returned for
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the parameters are to be returned for  
 *                 input or output                                     (input)
 *  parameters   : Gnomonic projection parameters to be returned       (output)
 */


long Set_Gnomonic_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
    const Gnomonic_Tuple      coordinates);
/*
 *  The function Set_Gnomonic_Coordinates sets the Gnomonic 
 *  projection coordinates for the specified state.
 *  State        : Indicates whether the coordinates are to be set for
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the coordinates are to be set for
 *                 input or output                                     (input)
 *  coordinates  : Gnomonic projection coordinates to be set           (input)
 */


long Get_Gnomonic_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Gnomonic_Tuple            *coordinates);
/*
 *  The function Set_Gnomonic_Coordinates sets the Gnomonic  
 *  projection coordinates for the specified state.
 *  State        : Indicates whether the coordinates are to be set for
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the coordinates are to be set for
 *                 input or output                                     (input)
 *  coordinates  : Gnomonic projection coordinates to be set           (input)
 */


long Set_Lambert_Conformal_Conic_1_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Lambert_Conformal_Conic_1_Parameters parameters);
/*
 *  The function Set_Lambert_Conformal_Conic_1_Params sets the Lambert 
 *  Conformal Conic (1 parallel) projection parameters for the specified state. 
 *  State        : Indicates whether the parameters are to be set for  
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the parameters are to be set for  
 *                 input or output                                     (input)
 *  parameters   : Lambert Conformal Conic (1 parallel) projection parameters to be set
 *                                                                     (input)
 */


long Get_Lambert_Conformal_Conic_1_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Lambert_Conformal_Conic_1_Parameters *parameters);
/*
 *  The function Get_Lambert_Conformal_Conic_1_Params returns the Lambert  
 *  Conformal Conic (1 parallel) projection parameters for the specified state. 
 *  State        : Indicates whether the parameters are to be returned for
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the parameters are to be returned for  
 *                 input or output                                     (input)
 *  parameters   : Lambert Conformal Conic (1 parallel) projection parameters to be returned 
 *                                                                     (output)
 */


long Set_Lambert_Conformal_Conic_1_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Lambert_Conformal_Conic_1_Tuple coordinates);
/*
 *  The function Set_Lambert_Conformal_Conic_1_Coordinates sets the Lambert Conformal  
 *  Conic (1 parallel) projection coordinates for the specified state. 
 *  State        : Indicates whether the coordinates are to be set for  
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the coordinates are to be set for  
 *                 input or output                                     (input)
 *  coordinates  : Lambert Conformal Conic (1 parallel) projection coordinates to be set
 *                                                                     (input)
 */


long Get_Lambert_Conformal_Conic_1_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Lambert_Conformal_Conic_1_Tuple *coordinates);
/*
 *  The function Get_Lambert_Conformal_Conic_1_Coordinates returns the Lambert
 *  Conformal Conic (1 parallel) projection coordinates for the specified state. 
 *  State          : Indicates whether the coordinates are to be returned for  
 *                   interactive or file processing                    (input)
 *  Direction      : Indicates whether the coordinates are to be returned for  
 *                   input or output                                   (input)
 *  coordinates    : Lambert Conformal Conic (1 parallel) projection coordinates to be returned
 *                                                                     (output)
 */

long Set_Lambert_Conformal_Conic_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Lambert_Conformal_Conic_Parameters parameters);
/*
 *  The function Set_Lambert_Conformal_Conic_Params sets the Lambert 
 *  Conformal Conic projection parameters for the specified state. 
 *  State        : Indicates whether the parameters are to be set for  
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the parameters are to be set for  
 *                 input or output                                     (input)
 *  parameters   : Lambert Conformal Conic projection parameters to be set
 *                                                                     (input)
 */


long Get_Lambert_Conformal_Conic_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Lambert_Conformal_Conic_Parameters *parameters);
/*
 *  The function Get_Lambert_Conformal_Conic_Params returns the Lambert  
 *  Conformal Conic projection parameters for the specified state. 
 *  State        : Indicates whether the parameters are to be returned for
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the parameters are to be returned for  
 *                 input or output                                     (input)
 *  parameters   : Lambert Conformal Conic projection parameters to be returned 
 *                                                                     (output)
 */


long Set_Lambert_Conformal_Conic_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Lambert_Conformal_Conic_Tuple coordinates);
/*
 *  The function Set_Lambert_Conformal_Conic_Coordinates sets the Lambert Conformal  
 *  Conic projection coordinates for the specified state. 
 *  State        : Indicates whether the coordinates are to be set for  
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the coordinates are to be set for  
 *                 input or output                                     (input)
 *  coordinates  : Lambert Conformal Conic projection coordinates to be set
 *                                                                     (input)
 */


long Get_Lambert_Conformal_Conic_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Lambert_Conformal_Conic_Tuple *coordinates);
/*
 *  The function Get_Lambert_Conformal_Conic_Coordinates returns the Lambert
 *  Conformal Conic projection coordinates for the specified state. 
 *  State          : Indicates whether the coordinates are to be returned for  
 *                   interactive or file processing                    (input)
 *  Direction      : Indicates whether the coordinates are to be returned for  
 *                   input or output                                   (input)
 *  coordinates    : Lambert Conformal Conic projection coordinates to be returned
 *                                                                     (output)
 */


long Set_Local_Cartesian_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Local_Cartesian_Parameters parameters);
/*
 *  The function Set_Local_Cartesian_Params sets the Local Cartesian 
 *  coordinate system parameters for the specified state.
 *  State        : Indicates whether the parameters are to be set for
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the parameters are to be set for
 *                 input or output                                     (input)
 *  parameters   : Local Cartesian coordinate system parameters to be set
 *                                                                     (input)
 */


long Get_Local_Cartesian_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
    Local_Cartesian_Parameters *parameters);
/*
 *  The function Get_Local_Cartesian_Params returns the Local Cartesian 
 *  coordinate system parameters for the specified state.
 *  State        : Indicates whether the parameters are to be returned for
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the parameters are to be returned for
 *                 input or output                                     (input)
 *  parameters   : Local Cartesian coordinate system parameters to be returned
 *                                                                     (output)
 */


long Set_Local_Cartesian_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
    const Local_Cartesian_Tuple coordinates);
/*
 *  The function Set_Local_Cartesian_Coordinates sets the Local Cartesian 
 *  coordinates for the specified state.
 *  State        : Indicates whether the coordinates are to be set for
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the coordinates are to be set for
 *                 input or output                                     (input)
 *  coordinates  : Local Cartesian coordinates to be set               (input)
 */


long Get_Local_Cartesian_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Local_Cartesian_Tuple     *coordinates);
/*
 *  The function Get_Local_Cartesian_Coordinates returns the Local Cartesian projection 
 *  coordinates for the specified state.
 *  State          : Indicates whether the coordinates are to be returned for
 *                   interactive or file processing                    (input)
 *  Direction      : Indicates whether the coordinates are to be returned for
 *                   input or output                                   (input)
 *  coordinates    : Local Cartesian coordinates to be returned        (output)
 */


long Set_Mercator_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Mercator_Parameters parameters);
/*
 *  The function Set_Mercator_Params sets the Mercator projection  
 *  parameters for the specified state. 
 *  State        : Indicates whether the parameters are to be set for  
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the parameters are to be set for  
 *                 input or output                                     (input)
 *  parameters   : Mercator projection parameters to be set            (input)
 */


long Get_Mercator_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Mercator_Parameters       *parameters);
/*
 *  The function Get_Mercator_Params returns the Mercator projection
 *  parameters for the specified state. 
 *  State        : Indicates whether the parameters are to be returned for
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the parameters are to be returned for  
 *                 input or output                                     (input)
 *  parameters   : Mercator projection parameters to be returned       (output)
 */


long Set_Mercator_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Mercator_Tuple      coordinates);
/*
 *  The function Set_Mercator_Coordinates sets the Mercator projection 
 *  coordinates for the specified state. 
 *  State        : Indicates whether the coordinates are to be set for  
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the coordinates are to be set for  
 *                 input or output                                     (input)
 *  coordinates  : Mercator projection coordinates to be set           (input)
 */


long Get_Mercator_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Mercator_Tuple            *coordinates);
/*
 *  The function Get_Mercator_Coordinates returns the Mercator projection
 *  coordinates for the specified state. 
 *  State          : Indicates whether the coordinates are to be returned for  
 *                   interactive or file processing                    (input)
 *  Direction      : Indicates whether the coordinates are to be returned for  
 *                   input or output                                   (input)
 *  coordinates    : Mercator projection coordinates to be returned    (output)
 */


long Set_MGRS_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const MGRS_Tuple          coordinates);
/*
 *  The function Set_MGRS_Coordinates sets the MGRS projection coordinates
 *  for the specified state. 
 *  State        : Indicates whether the coordinates are to be set for  
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the coordinates are to be set for  
 *                 input or output                                     (input)
 *  coordinates  : MGRS projection coordinates to be set               (input)
 */


long Get_MGRS_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  MGRS_Tuple                *coordinates);
/*
 *  The function Get_MGRS_Coordinates returns the MGRS projection coordinates 
 *  for the specified state. 
 *  State          : Indicates whether the coordinates are to be returned for  
 *                   interactive or file processing                    (input)
 *  Direction      : Indicates whether the coordinates are to be returned for  
 *                   input or output                                   (input)
 *  coordinates    : MGRS projection coordinates to be returned        (output)
 */


long Set_Miller_Cylindrical_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Miller_Cylindrical_Parameters parameters);
/*
 *  The function Set_Miller_Cylindrical_Params sets the Miller Cylindrical   
 *  projection parameters for the specified state. 
 *  State        : Indicates whether the parameters are to be set for  
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the parameters are to be set for  
 *                 input or output                                     (input)
 *  parameters   : Miller Cylindrical projection parameters to be set  (input)
 */


long Get_Miller_Cylindrical_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Miller_Cylindrical_Parameters *parameters);
/*
 *  The function Get_Miller_Cylindrical_Params returns the Miller Cylindrical   
 *  projection parameters for the specified state. 
 *  State        : Indicates whether the parameters are to be returned for
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the parameters are to be returned for  
 *                 input or output                                     (input)
 *  parameters   : Miller Cylindrical projection parameters to be returned
 *                                                                     (output)
 */


long Set_Miller_Cylindrical_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Miller_Cylindrical_Tuple coordinates);
/*
 *  The function Set_Miller_Cylindrical_Coordinates sets the Miller Cylindrical  
 *  projection coordinates for the specified state. 
 *  State        : Indicates whether the coordinates are to be set for  
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the coordinates are to be set for  
 *                 input or output                                     (input)
 *  coordinates  : Miller Cylindrical projection coordinates to be set (input)
 */


long Get_Miller_Cylindrical_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Miller_Cylindrical_Tuple  *coordinates);
/*
 *  The function Get_Miller_Cylindrical_Coordinates returns the Miller Cylindrical 
 *  projection coordinates for the specified state. 
 *  State          : Indicates whether the coordinates are to be returned for  
 *                   interactive or file processing                    (input)
 *  Direction      : Indicates whether the coordinates are to be returned for  
 *                   input or output                                   (input)
 *  coordinates    : Miller Cylindrical projection coordinates to be returned
 *                                                                     (output)
 */


long Set_Mollweide_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Mollweide_Parameters parameters);
/*
 *  The function Set_Mollweide_Params sets the Mollweide
 *  projection parameters for the specified state. 
 *  State        : Indicates whether the parameters are to be set for  
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the parameters are to be set for  
 *                 input or output                                     (input)
 *  parameters   : Mollweide projection parameters to be set           (input)
 */


long Get_Mollweide_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Mollweide_Parameters      *parameters);
/*
 *  The function Get_Mollweide_Params returns the Mollweide projection  
 *  parameters for the specified state. 
 *  State        : Indicates whether the parameters are to be returned for
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the parameters are to be returned for  
 *                 input or output                                     (input)
 *  parameters    : Mollweide projection parameters to be returned     (output)
 */


long Set_Mollweide_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Mollweide_Tuple     coordinates);
/*
 *  The function Set_Mollweide_Coordinates sets the Mollweide projection 
 *  coordinates for the specified state. 
 *  State        : Indicates whether the coordinates are to be set for  
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the coordinates are to be set for  
 *                 input or output                                     (input)
 *  coordinates  : Mollweide projection coordinates to be set          (input)
 */


long Get_Mollweide_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Mollweide_Tuple           *coordinates);
/*
 *  The function Get_Mollweide_Coordinates returns the Mollweide 
 *  projection coordinates for the specified state. 
 *  State          : Indicates whether the coordinates are to be returned for  
 *                   interactive or file processing                    (input)
 *  Direction      : Indicates whether the coordinates are to be returned for  
 *                   input or output                                   (input)
 *  coordinates    : Mollweide projection coordinates to be returned   (output)
 */


long Set_Neys_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Neys_Parameters parameters);
/*
 *  The function Set_Neys_Params sets the Ney's (Modified Lambert 
 *  Conformal Conic) projection parameters for the specified state. 
 *  State        : Indicates whether the parameters are to be set for  
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the parameters are to be set for  
 *                 input or output                                     (input)
 *  parameters   : Ney's projection parameters to be set
 *                                                                     (input)
 */


long Get_Neys_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Neys_Parameters *parameters);
/*
 *  The function Get_Neys_Params returns the Ney's (Modified Lambert  
 *  Conformal Conic) projection parameters for the specified state. 
 *  State        : Indicates whether the parameters are to be returned for
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the parameters are to be returned for  
 *                 input or output                                     (input)
 *  parameters   : Ney's projection parameters to be returned 
 *                                                                     (output)
 */


long Set_Neys_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Neys_Tuple coordinates);
/*
 *  The function Set_Neys_Coordinates sets the Ney's (Modified Lambert Conformal  
 *  Conic) projection coordinates for the specified state. 
 *  State        : Indicates whether the coordinates are to be set for  
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the coordinates are to be set for  
 *                 input or output                                     (input)
 *  coordinates  : Ney's projection coordinates to be set
 *                                                                     (input)
 */


long Get_Neys_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Neys_Tuple *coordinates);
/*
 *  The function Get_Neys_Coordinates returns the Ney's (Modified Lambert
 *  Conformal Conic) projection coordinates for the specified state. 
 *  State          : Indicates whether the coordinates are to be returned for  
 *                   interactive or file processing                    (input)
 *  Direction      : Indicates whether the coordinates are to be returned for  
 *                   input or output                                   (input)
 *  coordinates    : Ney's projection coordinates to be returned
 *                                                                     (output)
 */


long Set_NZMG_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const NZMG_Tuple coordinates);
/*
 *  The function Set_NZMG_Coordinates sets the New Zealand Map Grid  
 *  projection coordinates for the specified state. 
 *  State        : Indicates whether the coordinates are to be set for  
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the coordinates are to be set for  
 *                 input or output                                     (input)
 *  coordinates  : New Zealand Map Grid projection coordinates to be set
 *                                                                     (input)
 */


long Get_NZMG_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  NZMG_Tuple *coordinates);
/*
 *  The function Get_NZMG_Coordinates returns the New Zealand Map Grid
 *  projection coordinates for the specified state. 
 *  State          : Indicates whether the coordinates are to be returned for  
 *                   interactive or file processing                    (input)
 *  Direction      : Indicates whether the coordinates are to be returned for  
 *                   input or output                                   (input)
 *  coordinates    : New Zealand Map Grid projection coordinates to be returned
 *                                                                     (output)
 */


long Set_Oblique_Mercator_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Oblique_Mercator_Parameters parameters);
/*
 *  The function Set_Oblique_Mercator_Params sets the Oblique Mercator projection  
 *  parameters for the specified state. 
 *  State        : Indicates whether the parameters are to be set for  
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the parameters are to be set for  
 *                 input or output                                     (input)
 *  parameters   : Oblique Mercator projection parameters to be set    (input)
 */


long Get_Oblique_Mercator_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Oblique_Mercator_Parameters       *parameters);
/*
 *  The function Get_Oblique_Mercator_Params returns the Oblique Mercator projection
 *  parameters for the specified state. 
 *  State        : Indicates whether the parameters are to be returned for
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the parameters are to be returned for  
 *                 input or output                                     (input)
 *  parameters   : Oblique Mercator projection parameters to be returned 
 *                                                                     (output)
 */


long Set_Oblique_Mercator_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Oblique_Mercator_Tuple      coordinates);
/*
 *  The function Set_Mercator_Coordinates sets the Oblique Mercator projection 
 *  coordinates for the specified state. 
 *  State        : Indicates whether the coordinates are to be set for  
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the coordinates are to be set for  
 *                 input or output                                     (input)
 *  coordinates  : Oblique Mercator projection coordinates to be set   (input)
 */


long Get_Oblique_Mercator_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Oblique_Mercator_Tuple            *coordinates);
/*
 *  The function Get_Oblique_Mercator_Coordinates returns the Oblique Mercator projection
 *  coordinates for the specified state. 
 *  State          : Indicates whether the coordinates are to be returned for  
 *                   interactive or file processing                    (input)
 *  Direction      : Indicates whether the coordinates are to be returned for  
 *                   input or output                                   (input)
 *  coordinates    : Oblique Mercator projection coordinates to be returned  
 *                                                                     (output)
 */


long Set_Orthographic_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Orthographic_Parameters parameters);
/*
 *  The function Set_Orthographic_Params sets the Orthographic projection 
 *  parameters for the specified state. 
 *  State        : Indicates whether the parameters are to be set for  
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the parameters are to be set for  
 *                 input or output                                     (input)
 *  parameters   : Orthographic projection parameters to be set        (input)
 */


long Get_Orthographic_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Orthographic_Parameters   *parameters);
/*
 *  The function Get_Orthographic_Params returns the Orthographic projection 
 *  parameters for the specified state. 
 *  State        : Indicates whether the parameters are to be returned for
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the parameters are to be returned for  
 *                 input or output                                     (input)
 *  parameters   : Orthographic projection parameters to be returned   (output)
 */


long Set_Orthographic_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Orthographic_Tuple  coordinates);
/*
 *  The function Set_Orthographic_Coordinates sets the Orthographic projection
 *  coordinates for the specified state. 
 *  State        : Indicates whether the coordinates are to be set for  
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the coordinates are to be set for  
 *                 input or output                                     (input)
 *  coordinates  : Orthographic projection coordinates to be set       (input)
 */


long Get_Orthographic_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
    Orthographic_Tuple        *coordinates);
/*
 *  The function Get_Orthographic_Coordinates returns the Orthographic projection
 *  coordinates for the specified state. 
 *  State          : Indicates whether the coordinates are to be returned for  
 *                   interactive or file processing                    (input)
 *  Direction      : Indicates whether the coordinates are to be returned for  
 *                   input or output                                   (input)
 *  coordinates    : Orthographic projection coordinates to be returned
 *                                                                     (output)
 */


long Set_Polar_Stereo_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Polar_Stereo_Parameters parameters);
/*
 *  The function Set_Polar_Stereo_Params sets the Polar Stereographic
 *  projection parameters for the specified state.
 *  State        : Indicates whether the parameters are to be set for
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the parameters are to be set for
 *                 input or output                                     (input)
 *  parameters   : Polar Stereographic projection parameters to be set (input)
 */


long Get_Polar_Stereo_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Polar_Stereo_Parameters   *parameters);
/*
 *  The function Get_Polar_Stereo_Params returns the Polar Stereographic
 *  projection parameters for the specified state.
 *  State        : Indicates whether the parameters are to be returned for
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the parameters are to be returned for
 *                 input or output                                     (input)
 *  parameters   : Polar Stereographic projection parameters to be returned
 *                                                                     (output)
 */


long Set_Polar_Stereo_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Polar_Stereo_Tuple  coordinates);
/*
 *  The function Set_Polar_Stereo_Coordinates sets the Polar Stereographic
 *  projection coordinates for the specified state. 
 *  State        : Indicates whether the coordinates are to be set for  
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the coordinates are to be set for  
 *                 input or output                                     (input)
 *  coordinates  : Polar Stereographic projection coordinates to be set
 *                                                                     (input)
 */


long Get_Polar_Stereo_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Polar_Stereo_Tuple        *coordinates);
/*
 *  The function Get_Polar_Stereo_Coordinates returns the Polar Stereographic 
 *  projection coordinates for the specified state. 
 *  State          : Indicates whether the coordinates are to be returned for  
 *                   interactive or file processing                    (input)
 *  Direction      : Indicates whether the coordinates are to be returned for  
 *                   input or output                                   (input)
 *  coordinates    : Polar Stereographic projection coordinates to be returned
 *                                                                     (output)
 */


long Set_Polyconic_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Polyconic_Parameters parameters);
/*
 *  The function Set_Polyconic_Params sets the Polyconic projection  
 *  parameters for the specified state. 
 *  State        : Indicates whether the parameters are to be set for  
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the parameters are to be set for  
 *                 input or output                                     (input)
 *  parameters   : Polyconic projection parameters to be set           (input)
 */


long Get_Polyconic_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Polyconic_Parameters      *parameters);
/*
 *  The function Get_Polyconic_Params returns the Polyconic projection  
 *  parameters for the specified state. 
 *  State        : Indicates whether the parameters are to be returned for
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the parameters are to be returned for  
 *                 input or output                                     (input)
 *  parameters   : Polyconic projection parameters to be returned      (output)
 */


long Set_Polyconic_Coordinates
(const File_or_Interactive State,
 const Input_or_Output     Direction,
 const Polyconic_Tuple     coordinates);
/*
 *  The function Set_Polyconic_Coordinates sets the Polyconic projection
 *  coordinates for the specified state. 
 *  State        : Indicates whether the coordinates are to be set for  
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the coordinates are to be set for  
 *                 input or output                                     (input)
 *  coordinates  : Polyconic projection coordinates to be set          (input)
 */


long Get_Polyconic_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Polyconic_Tuple           *coordinates);
/*
 *  The function Get_Polyconic_Coordinates returns the Polyconic projection
 *  coordinates for the specified state. 
 *  State          : Indicates whether the coordinates are to be returned for  
 *                   interactive or file processing                    (input)
 *  Direction      : Indicates whether the coordinates are to be returned for  
 *                   input or output                                   (input)
 *  coordinates    : Polyconic projection coordinates to be returned   (output)
 */


long Set_Sinusoidal_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Sinusoidal_Parameters parameters);
/*
 *  The function Set_Sinusoidal_Params sets the Sinusoidal projection  
 *  parameters for the specified state. 
 *  State        : Indicates whether the parameters are to be set for  
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the parameters are to be set for  
 *                 input or output                                     (input)
 *  parameters   : Sinusoidal projection parameters to be set          (input)
 */


long Get_Sinusoidal_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Sinusoidal_Parameters     *parameters);
/*
 *  The function Get_Sinusoidal_Params returns the Sinusoidal projection  
 *  parameters for the specified state. 
 *  State        : Indicates whether the parameters are to be returned for
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the parameters are to be returned for  
 *                 input or output                                     (input)
 *  parameters   : Sinusoidal projection parameters to be returned     (output)
 */


long Set_Sinusoidal_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Sinusoidal_Tuple    coordinates);
/*
 *  The function Set_Sinusoidal_Coordinates sets the Sinusoidal projection
 *  coordinates for the specified state. 
 *  State        : Indicates whether the coordinates are to be set for  
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the coordinates are to be set for  
 *                 input or output                                     (input)
 *  coordinates  : Sinusoidal projection coordinates to be set         (input)
 */


long Get_Sinusoidal_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Sinusoidal_Tuple          *coordinates);
/*
 *  The function Get_Sinusoidal_Coordinates returns the Sinusoidal projection
 *  coordinates for the specified state. 
 *  State          : Indicates whether the coordinates are to be returned for  
 *                   interactive or file processing                       (input)
 *  Direction      : Indicates whether the coordinates are to be returned for  
 *                   input or output                                      (input)
 *  coordinates    : Sinusoidal projection coordinates to be returned     (output)
 */


long Set_Stereographic_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Stereographic_Parameters parameters);
/*
 *  The function Set_Stereographic_Params sets the Stereographic projection 
 *  parameters for the specified state. 
 *  State        : Indicates whether the parameters are to be set for  
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the parameters are to be set for  
 *                 input or output                                     (input)
 *  parameters   : Stereographic projection parameters to be set        (input)
 */


long Get_Stereographic_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Stereographic_Parameters   *parameters);
/*
 *  The function Get_Stereographic_Params returns the Stereographic projection 
 *  parameters for the specified state. 
 *  State        : Indicates whether the parameters are to be returned for
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the parameters are to be returned for  
 *                 input or output                                     (input)
 *  parameters   : Stereographic projection parameters to be returned   (output)
 */


long Set_Stereographic_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Stereographic_Tuple  coordinates);
/*
 *  The function Set_Stereographic_Coordinates sets the Stereographic projection
 *  coordinates for the specified state. 
 *  State        : Indicates whether the coordinates are to be set for  
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the coordinates are to be set for  
 *                 input or output                                     (input)
 *  coordinates  : Stereographic projection coordinates to be set       (input)
 */


long Get_Stereographic_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
    Stereographic_Tuple        *coordinates);
/*
 *  The function Get_Stereographic_Coordinates returns the Stereographic projection
 *  coordinates for the specified state. 
 *  State          : Indicates whether the coordinates are to be returned for  
 *                   interactive or file processing                    (input)
 *  Direction      : Indicates whether the coordinates are to be returned for  
 *                   input or output                                   (input)
 *  coordinates    : Stereographic projection coordinates to be returned
 *                                                                     (output)
 */


long Set_Transverse_Cylindrical_Equal_Area_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Transverse_Cylindrical_Equal_Area_Parameters parameters);
/*
 *  The function Set_Transverse_Cylindrical_Equal_Area_Params sets the 
 *  Transverse Cylindrical Equal Area projection parameters for the specified state. 
 *  State        : Indicates whether the parameters are to be set for  
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the parameters are to be set for  
 *                 input or output                                     (input)
 *  parameters   : Transverse Cylindrical Equal Area projection parameters 
 *                 to be set                                           (input)
 */


long Get_Transverse_Cylindrical_Equal_Area_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Transverse_Cylindrical_Equal_Area_Parameters *parameters);
/*
 *  The function Get_Transverse_Cylindrical_Equal_Area_Params returns the 
 *  Transverse Cylindrical Equal Area projection parameters for the specified state. 
 *  State        : Indicates whether the parameters are to be returned for
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the parameters are to be returned for  
 *                 input or output                                     (input)
 *  parameters   : Transverse Cylindrical Equal Area projection parameters 
 *                 to be returned                                      (output)
 */


long Set_Transverse_Cylindrical_Equal_Area_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Transverse_Cylindrical_Equal_Area_Tuple coordinates);
/*
 *  The function Set_Transverse_Cylindrical_Equal_Area_Coordinates sets the 
 *  Transverse Cylindrical Equal Area projection coordinates for the specified state. 
 *  State        : Indicates whether the coordinates are to be set for  
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the coordinates are to be set for  
 *                 input or output                                     (input)
 *  coordinates  : Transverse Cylindrical Equal Area projection coordinates 
 *                 to be set                                           (input)
 */


long Get_Transverse_Cylindrical_Equal_Area_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Transverse_Cylindrical_Equal_Area_Tuple *coordinates);
/*
 *  The function Get_Transverse_Cylindrical_Equal_Area_Coordinates returns the 
 *  Transverse Cylindrical Equal Area projection coordinates for the specified state. 
 *  State          : Indicates whether the coordinates are to be returned for  
 *                   interactive or file processing                    (input)
 *  Direction      : Indicates whether the coordinates are to be returned for  
 *                   input or output                                   (input)
 *  coordinates    : Transverse Cylindrical Equal Area projection coordinates 
 *                   to be returned                                    (output)
 */


long Set_Transverse_Mercator_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Transverse_Mercator_Parameters parameters);
/*
 *  The function Set_Transverse_Mercator_Params sets the Transverse   
 *  Mercator projection parameters for the specified state. 
 *  State        : Indicates whether the parameters are to be set for  
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the parameters are to be set for  
 *                 input or output                                     (input)
 *  parameters   : Transverse Mercator projection parameters to be set (input)
 */


long Get_Transverse_Mercator_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Transverse_Mercator_Parameters *parameters);
/*
 *  The function Get_Transverse_Mercator_Params returns the Transverse 
 *  Mercator projection parameters for the specified state. 
 *  State        : Indicates whether the parameters are to be returned for
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the parameters are to be returned for  
 *                 input or output                                     (input)
 *  parameters   : Transverse Mercator projection parameters to be returned
 *                                                                     (output)
 */


long Set_Transverse_Mercator_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Transverse_Mercator_Tuple coordinates);
/*
 *  The function Set_Transverse_Mercator_Coordinates sets the Transverse  
 *  Mercator projection coordinates for the specified state. 
 *  State        : Indicates whether the coordinates are to be set for  
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the coordinates are to be set for  
 *                 input or output                                     (input)
 *  coordinates  : Transverse Mercator projection coordinates to be set
 *                                                                     (input)
 */


long Get_Transverse_Mercator_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Transverse_Mercator_Tuple *coordinates);
/*
 *  The function Get_Transverse_Mercator_Coordinates returns the Transverse
 *  Mercator projection coordinates for the specified state. 
 *  State          : Indicates whether the coordinates are to be returned for  
 *                   interactive or file processing                    (input)
 *  Direction      : Indicates whether the coordinates are to be returned for  
 *                   input or output                                   (input)
 *  coordinates    : Transverse Mercator projection coordinates to be returned 
 *                                                                     (output)
 */


long Set_UPS_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const UPS_Tuple           coordinates);
/*
 *  The function Set_UPS_Coordinates sets the UPS projection coordinates
 *  for the specified state. 
 *  State        : Indicates whether the coordinates are to be set for  
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the coordinates are to be set for  
 *                 input or output                                     (input)
 *  coordinates  : UPS projection coordinates to be set                (input)
 */


long Get_UPS_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  UPS_Tuple                 *coordinates);
/*
 *  The function Get_UPS_Coordinates returns the UPS projection coordinates 
 *  for the specified state. 
 *  State          : Indicates whether the coordinates are to be returned for  
 *                   interactive or file processing                    (input)
 *  Direction      : Indicates whether the coordinates are to be returned for  
 *                   input or output                                   (input)
 *  coordinates    : UPS projection coordinates to be returned         (output)
 */


long Set_UTM_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const UTM_Parameters      parameters);
/*
 *  The function Set_UTM_Params sets the UTM projection parameters   
 *  for the specified state. 
 *  State        : Indicates whether the parameters are to be set for  
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the parameters are to be set for  
 *                 input or output                                     (input)
 *  parameters   : UTM projection parameters to be set                 (input)
 */


long Get_UTM_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  UTM_Parameters            *parameters);
/*
 *  The function Get_UTM_Params returns the UTM projection parameters 
 *  for the specified state. 
 *  State        : Indicates whether the parameters are to be returned for
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the parameters are to be returned for  
 *                 input or output                                     (input)
 *  parameters   : UTM projection parameters to be returned            (output)
 */


long Set_UTM_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const UTM_Tuple           coordinates); 
/*
 *  The function Set_UTM_Coordinates sets the UTM projection coordinates  
 *  for the specified state. 
 *  State        : Indicates whether the coordinates are to be set for  
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the coordinates are to be set for  
 *                 input or output                                     (input)
 *  coordinates  : UTM projection coordinates to be set                (input)
 */


long Get_UTM_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  UTM_Tuple                 *coordinates);
/*
 *  The function Get_UTM_Coordinates returns the UTM projection coordinates
 *  for the specified state. 
 *  State          : Indicates whether the coordinates are to be returned for  
 *                   interactive or file processing                    (input)
 *  Direction      : Indicates whether the coordinates are to be returned for  
 *                   input or output                                   (input)
 *  coordinates    : UTM projection coordinates to be returned         (output)
 */


long Set_Van_der_Grinten_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Van_der_Grinten_Parameters parameters);
/*
 *  The function Set_Van_der_Grinten_Params sets the Van der Grinten   
 *  projection parameters for the specified state. 
 *  State        : Indicates whether the parameters are to be set for  
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the parameters are to be set for  
 *                 input or output                                     (input)
 *  parameters   : Van der Grinten projection parameters to be set     (input)
 */


long Get_Van_der_Grinten_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Van_der_Grinten_Parameters *parameters);
/*
 *  The function Get_Van_der_Grinten_Params returns the Van der Grinten 
 *  projection parameters for the specified state. 
 *  State        : Indicates whether the parameters are to be returned for
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the parameters are to be returned for  
 *                 input or output                                     (input)
 *  parameters   : Van der Grinten projection parameters to be returned
 *                                                                     (input)
 */


long Set_Van_der_Grinten_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Van_der_Grinten_Tuple coordinates);
/*
 *  The function Set_Van_der_Grinten_Coordinates sets the Van der Grinten 
 *  projection coordinates for the specified state. 
 *  State        : Indicates whether the coordinates are to be set for  
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the coordinates are to be set for  
 *                 input or output                                     (input)
 *  coordinates  : Van der Grinten projection coordinates to be set    (input)
 */


long Get_Van_der_Grinten_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Van_der_Grinten_Tuple     *coordinates);
/*
 *  The function Get_Van_der_Grinten_Coordinates returns the Van der Grinten 
 *  projection coordinates for the specified state. 
 *  State          : Indicates whether the coordinates are to be returned for  
 *                   interactive or file processing                    (input)
 *  Direction      : Indicates whether the coordinates are to be returned for  
 *                   input or output                                   (input)
 *  coordinates    : Van der Grinten projection coordinates to be returned
 *                                                                     (output)
 */


long Convert
( const File_or_Interactive State);
/*
 *  The function Convert converts the current input coordinates in the coordinate
 *  system defined by the current input coordinate system parameters and input datum,
 *  into output coordinates in the coordinate system defined by the output coordinate
 *  system parameters and output datum.
 *  State      : Indicates whether the interactive or file processing state data
 *               is to be used                                             (input)
 */


long Get_Conversion_Errors
( const File_or_Interactive State,
  double *CE90,
  double *LE90,
  double *SE90);
/*
 *  The function Get_Conversion_Errors returns the estimated errors in the location
 *  the most recently converted location
 *  State  : Indicates whether the conversion error information is for interactive 
 *           or file processing                                    (input)
 *  CE90   : 90% circular (horizontal) error, in meters            (output)
 *  LE90   : 90% linear (vertical) error, in meters                (output)
 *  SE90   : 90% spherical error, in meters                        (output)
 */

long Set_Conversion_Errors
( const File_or_Interactive State,
  double CE90,
  double LE90,
  double SE90);
/*
 *  The function Set_Conversion_Errors sets the estimated errors 
 *  State  : Indicates whether the datum is to be used for interactive 
 *           or file processing                                    (input)
 *  CE90   : 90% circular (horizontal) error, in meters            (input)
 *  LE90   : 90% linear (vertical) error, in meters                (input)
 *  SE90   : 90% spherical error, in meters                        (input)
 */

 
long Get_Conversion_Status
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  long                      *Conversion_Status);
/*
 *  The function Get_Conversion_Status returns the current status for the specified state.. 
 *  State      : Indicates whether the status returned is for interactive 
 *               or file processing                                    (input)
 *  Direction  : Indicates whether the status returned is for input or output
 *                                                                     (input)
 *  Conversion_Status : The current status for the specified state     (output)
 */


long Get_Conversion_Status_String
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  char                      *Separator,
  char                      *String);
/*
 *  The function Get_Conversion_Status_String returns a string describing the 
 *  current status for the specified state. 
 *  State      : Indicates whether the status string returned is for interactive 
 *               or file processing                                    (input)
 *  Direction  : Indicates whether the status string returned is for input or 
 *               output                                                (input)
 *  Separator  : String to be used to separate individual status messages
 *                                                                     (input)
 *  String     : String describing the current status for the specified state     
 *                                                                     (output)
 */


void Get_Return_Code_String
( long  Error_Code,
  char  *Separator,
  char  *String);
/*
 *  The function Get_Return_Code_String returns a string describing the specified 
 *  engine return code.
 *  Error_Code : Status code returned by engine function               (input) 
 *  Separator  : String to be used to separate individual status messages
 *                                                                     (input)
 *  String     : String describing the current status for the specified state     
 *                                                                     (output)
 */


/* ensure proper linkage to c++ programs */
  #ifdef __cplusplus
}
  #endif

#endif /* ENGINE_H */

