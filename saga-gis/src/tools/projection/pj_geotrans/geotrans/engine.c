/**********************************************************
 * Version $Id: engine.c 911 2011-02-14 16:38:15Z reklov_w $
 *********************************************************/
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
 *    BRITISH NATIONAL GRID, CASSINI, CYLINDRICAL EQUAL AREA,ECKERT4, ECKERT6, 
 *    EQUIDISTANT CYLINDRICAL, GNOMONIC, LOCAL CARTESIAN, MILLER, MOLLWEIDE, 
 *    NEYS, NEW ZEALAND MAP GRID, OBLIQUE MERCATOR, ORTHOGRAPHIC, POLYCONIC, 
 *    SINUSOIDAL, STEREOGRAPHIC, TRANSVERSE CYLINDRICAL EQUAL AREA, and VAN DER GRINTEN.
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
 *                               INCLUDES
 */
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "datum.h"
#include "ellipse.h"
#include "geoid.h"
#include "geocent.h"
#include "loccart.h"
#include "georef.h"
#include "mgrs.h"
#include "utm.h"
#include "ups.h"
#include "albers.h"
#include "azeq.h"
#include "bng.h"
#include "bonne.h"
#include "cassini.h"
#include "cyleqa.h"
#include "eckert4.h"
#include "eckert6.h"
#include "eqdcyl.h"
#include "gnomonic.h"
#include "lambert.h"
#include "lambert_1.h"
#include "mercator.h"
#include "miller.h"
#include "mollweid.h"
#include "neys.h"
#include "nzmg.h"
#include "omerc.h"
#include "orthogr.h"
#include "polarst.h"
#include "polycon.h"
#include "sinusoid.h"
#include "stereogr.h"
#include "trcyleqa.h"
#include "tranmerc.h"
#include "grinten.h"
#include "engine.h"
/*
 *  ctype.h     - standard C character handling library
 *  math.h      - standard C math library
 *  stdio.h     - standard C input/output library
 *  stdlib.h    - standard C general utility library 
 *  string.h    - standard C string handling library
 *  datum.h     - access to datum parameters and datum transformation functions
 *  ellipse.h   - access to ellipsoid parameters
 *  geoid.h     - geoid-ellipsoid separation
 *  georef.h    - conversion between Geodetic and GEOREF coordinates
 *  geocent.h   - conversion between Geocentric and Geodetic coordinates
 *  loccart.h   - conversion between Geodetic and Local Cartesian coordinates
 *  mgrs.h      - conversion between Geodetic and MGRS coordinates
 *  utm.h       - conversion between Geodetic and UTM coordinates
 *  ups.h       - conversion between Geodetic and UPS coordinates
 *  albers.h    - conversion between Geodetic and Albers Equal Area Conic coordinates
 *  azeq.h      - conversion between Geodetic and Azimuthal Equidistant coordinates
 *  bng.h       - conversion between Geodetic and British National Grid coordinates
 *  bonne.h     - conversion between Geodetic and Bonne coordinates
 *  cassini.h   - conversion between Geodetic and Cassini coordinates
 *  cyleqa.h    - conversion between Geodetic and Cylindrical Equal Area coordinates
 *  eckert4.h   - conversion between Geodetic and Eckert4 coordinates
 *  eckert6.h   - conversion between Geodetic and Eckert6 coordinates
 *  eqdcyl.h    - conversion between Geodetic and Equidistant Cylindrical coordinates
 *  gnomonic.h  - conversion between Geodetic and Gnomonic coordinates
 *  lambert.h   - conversion between Geodetic and Lambert Conformal Conic coordinates
 *  lambert_1.h - conversion between Geodetic and Lambert Conformal Conic (1 parallel) coordinates
 *  mercator.h  - conversion between Geodetic and Mercator coordinates
 *  miller.h    - conversion between Geodetic and Miller Cylindrical coordinates
 *  mollweid.h  - conversion between Geodetic and Mollweide coordinates
 *  neys.h      - conversion between Geodetic and Ney's (Modified Lambert Conformal Conic) coordinates
 *  nzmg.h      - conversion between Geodetic and New Zealand Map Grid coordinates
 *  omerc.h     - conversion between Geodetic and Oblique Mercator coordinates
 *  orthogr.h   - conversion between Geodetic and Orthographic coordinates
 *  polarst.h   - conversion between Geodetic and Polar Stereographic coordinates
 *  polycon.h   - conversion between Geodetic and Polyconic coordinates
 *  sinusoid.h  - conversion between Geodetic and Sinusoidal coordinates
 *  stereogr.h  - conversion between Geodetic and Stereographic coordinates
 *  trcyleqa.h  - conversion between Geodetic and Transverse Cylindrical 
 *                  Equal Area coordinates
 *  tranmerc.h  - conversion between Geodetic and Transverse Mercator coordinates
 *  grinten.h   - conversion between Geodetic and Van Der Grinten coordinates
 *
 *  engine.h    - type definitions and function prototype error checking
 */


/****************************************************************************/
/*
 *                                DEFINES
 */

#define FALSE 0
#define TRUE  1
#define PI    3.14159265358979323e0  /* PI                                  */


/****************************************************************************/
/*
 *                                  TYPES
 */

/* Coordinate System Definition with Multiple Variants */
typedef union Parameters
{
  Geodetic_Parameters               Geodetic;
  Albers_Equal_Area_Conic_Parameters  Albers_Equal_Area_Conic; 
  Azimuthal_Equidistant_Parameters  Azimuthal_Equidistant;
  Bonne_Parameters                  Bonne;
  Cassini_Parameters                Cassini;
  Cylindrical_Equal_Area_Parameters   Cylindrical_Equal_Area;
  Equidistant_Cylindrical_Parameters  Equidistant_Cylindrical;
  Eckert4_Parameters                Eckert4;
  Eckert6_Parameters                Eckert6;
  Gnomonic_Parameters               Gnomonic;
  Lambert_Conformal_Conic_1_Parameters  Lambert_Conformal_Conic_1;
  Lambert_Conformal_Conic_Parameters  Lambert_Conformal_Conic;
  Local_Cartesian_Parameters        Local_Cartesian; 
  Mercator_Parameters               Mercator;
  Miller_Cylindrical_Parameters     Miller_Cylindrical;
  Mollweide_Parameters              Mollweide;
  Neys_Parameters                   Neys;
  Oblique_Mercator_Parameters       Oblique_Mercator;
  Orthographic_Parameters           Orthographic;
  Polar_Stereo_Parameters           Polar_Stereo;
  Polyconic_Parameters              Polyconic;         
  Sinusoidal_Parameters             Sinusoidal;
  Stereographic_Parameters          Stereographic;
  Transverse_Cylindrical_Equal_Area_Parameters  Transverse_Cylindrical_Equal_Area;
  Transverse_Mercator_Parameters    Transverse_Mercator;
  UTM_Parameters                    UTM;
  Van_der_Grinten_Parameters        Van_der_Grinten;
} Parameter_Tuple;

/* Coordinate Tuple Definition with Multiple Variants */
typedef union Coordinate_Tuples
{
  Geocentric_Tuple                  Geocentric;
  Geodetic_Tuple                    Geodetic;
  GEOREF_Tuple                      GEOREF;
  Albers_Equal_Area_Conic_Tuple     Albers_Equal_Area_Conic;  
  Azimuthal_Equidistant_Tuple       Azimuthal_Equidistant;
  BNG_Tuple                         BNG;
  Bonne_Tuple                       Bonne;
  Cassini_Tuple                     Cassini;
  Cylindrical_Equal_Area_Tuple      Cylindrical_Equal_Area;
  Equidistant_Cylindrical_Tuple     Equidistant_Cylindrical;
  Eckert4_Tuple                     Eckert4;
  Eckert6_Tuple                     Eckert6;
  Gnomonic_Tuple                    Gnomonic;
  Lambert_Conformal_Conic_1_Tuple   Lambert_Conformal_Conic_1;
  Lambert_Conformal_Conic_Tuple     Lambert_Conformal_Conic;
  Local_Cartesian_Tuple             Local_Cartesian;  
  Mercator_Tuple                    Mercator;
  Miller_Cylindrical_Tuple          Miller_Cylindrical;
  MGRS_Tuple                        MGRS;
  Mollweide_Tuple                   Mollweide;
  Neys_Tuple                        Neys;
  NZMG_Tuple                        NZMG;
  Oblique_Mercator_Tuple            Oblique_Mercator;
  Orthographic_Tuple                Orthographic;
  Polar_Stereo_Tuple                Polar_Stereo;
  Polyconic_Tuple                   Polyconic;
  Sinusoidal_Tuple                  Sinusoidal;
  Stereographic_Tuple               Stereographic;
  Transverse_Cylindrical_Equal_Area_Tuple   Transverse_Cylindrical_Equal_Area;
  Transverse_Mercator_Tuple         Transverse_Mercator;
  UPS_Tuple                         UPS;
  UTM_Tuple                         UTM;
  Van_der_Grinten_Tuple             Van_der_Grinten;

} Coordinate_Tuple;

typedef struct Coordinate_System_Table_Row
{
  char Name[COORD_SYS_NAME_LENGTH];
  char Code[COORD_SYS_CODE_LENGTH];
} Coordinate_System_Row;

/* Engine State Definition */
typedef struct coordinate_State_Row
{
  long                 datum_Index;  /* currently specified datum */
  long                 status;       /* current error status */
  Parameter_Tuple      parameters;   /* current coordinate system parameters */
  Coordinate_Tuple     coordinates;  /* current coordinates */
  Coordinate_Type      type;         /* current coordinate system type */
} Coordinate_State_Row;

/***************************************************************************/
/*
 *                              DECLARATIONS
 */

/* CS_State[x][y] is set up as follows:
   x = Number of interaction states (File, Interactive, etc.)
   y = Number of IO states (Input, Output, etc.) */
Coordinate_State_Row CS_State[2][2];

/* Local State Variables */
static Coordinate_System_Row Coordinate_System_Table[NUMBER_COORD_SYS];
Parameter_Tuple Default_Parameters[NUMBER_COORD_SYS];    /* Default Parameters */
Coordinate_Tuple Default_Coordinates[NUMBER_COORD_SYS];  /* Default Coordinates */
static long Engine_Initialized = FALSE;
long Number_of_Datums = 0;
Precision Engine_Precision = Tenth_of_Second; /* Default Precision Level */
double ce90 = -1.0;  /* 90% circular (horizontal) error */
double le90 = -1.0;  /* 90% linear (vertical) error */
double se90 = -1.0;  /* 90% spherical error */

/***************************************************************************/
/*
 *                       LOCAL FUNCTIONS
 */

int Valid_Datum_Index(const long Index)
/*
 *  The function Valid_Datum_Index returns TRUE if the specified index is a
 *  valid datum index and FALSE otherwise.
 *  Index      : Index of a particular datum                           (input)
 */
{ /* Valid_Datum_Index */
  return ( (Index > 0) && (Index <= Number_of_Datums) );
} /* Valid_Datum_Index */


int Valid_State(File_or_Interactive State)
/*
 *  The function Valid_State returns TRUE if the specified state is a
 *  valid state and FALSE otherwise.
 *  State      : Sepcified state                                       (input)
 */
{ /* Valid_State */
  return ( (State == File) || (State == Interactive) );
} /* Valid_State */


int Valid_Direction(const Input_or_Output Direction)
/*
 *  The function Valid_Direction returns TRUE if the specified direction is a
 *  valid direction and FALSE otherwise.
 *  Direction : Sepcified direction                                    (input)
 */
{ /* Valid_Direction */
  return ( (Direction == Input) || (Direction == Output) );
} /* Valid_Direction */


void Set_Defaults()
/*
 *  The function Set_Defaults sets the coordinate system parameters and coordinate
 *  values for all coordinate systems to default values.
 */
{ /* Set_Defaults */
  double a;
  double f;
  double origin_latitude;
  double latitude_1;
  double longitude_1;
  double latitude_2;
  double longitude_2;
  double std_parallel;
  double std_parallel_1;
  double std_parallel_2;
  double latitude_of_true_scale;
  double origin_longitude;
  double central_meridian;
  double longitude_down_from_pole;
  double false_easting;
  double false_northing;
  double scale_factor;
  double orientation;
  double origin_height;

  Default_Coordinates[Geocentric].Geocentric.x = 0.0;
  Default_Coordinates[Geocentric].Geocentric.y = 0.0;
  Default_Coordinates[Geocentric].Geocentric.z = 0.0;

  Default_Parameters[Geodetic].Geodetic.height_type = Ellipsoid_Height;

  Default_Coordinates[Geodetic].Geodetic.longitude = 0.0;
  Default_Coordinates[Geodetic].Geodetic.latitude = 0.0;
  Default_Coordinates[Geodetic].Geodetic.height = 0.0;

  strcpy(Default_Coordinates[GEOREF].GEOREF.string,"NGAA0000000000");

  Get_Albers_Parameters(&a, &f, &origin_latitude, &central_meridian, 
                        &std_parallel_1, &std_parallel_2, &false_easting, &false_northing);
  Default_Parameters[Albers_Equal_Area_Conic].Albers_Equal_Area_Conic.origin_latitude = origin_latitude;
  Default_Parameters[Albers_Equal_Area_Conic].Albers_Equal_Area_Conic.central_meridian = central_meridian;
  Default_Parameters[Albers_Equal_Area_Conic].Albers_Equal_Area_Conic.false_easting = false_easting;
  Default_Parameters[Albers_Equal_Area_Conic].Albers_Equal_Area_Conic.std_parallel_1 = std_parallel_1;
  Default_Parameters[Albers_Equal_Area_Conic].Albers_Equal_Area_Conic.std_parallel_2 = std_parallel_2;
  Default_Parameters[Albers_Equal_Area_Conic].Albers_Equal_Area_Conic.false_northing = false_northing;
  Default_Coordinates[Albers_Equal_Area_Conic].Albers_Equal_Area_Conic.easting = 0.0;
  Default_Coordinates[Albers_Equal_Area_Conic].Albers_Equal_Area_Conic.northing = 0.0;

  Get_Azimuthal_Equidistant_Parameters(&a, &f, &origin_latitude, &central_meridian, 
                                       &false_easting, &false_northing);
  Default_Parameters[Azimuthal_Equidistant].Azimuthal_Equidistant.origin_latitude = origin_latitude;
  Default_Parameters[Azimuthal_Equidistant].Azimuthal_Equidistant.central_meridian = central_meridian;
  Default_Parameters[Azimuthal_Equidistant].Azimuthal_Equidistant.false_easting = false_easting;
  Default_Parameters[Azimuthal_Equidistant].Azimuthal_Equidistant.false_northing = false_northing;
  Default_Coordinates[Azimuthal_Equidistant].Azimuthal_Equidistant.easting = 0.0;
  Default_Coordinates[Azimuthal_Equidistant].Azimuthal_Equidistant.northing = 0.0;

  strcpy(Default_Coordinates[BNG].BNG.string,"SV 0000000000");
  
  Get_Bonne_Parameters(&a, &f, &origin_latitude, &central_meridian,
                       &false_easting, &false_northing);
  Default_Parameters[Bonne].Bonne.origin_latitude = origin_latitude;
  Default_Parameters[Bonne].Bonne.central_meridian = central_meridian;
  Default_Parameters[Bonne].Bonne.false_easting = false_easting;
  Default_Parameters[Bonne].Bonne.false_northing = false_northing;
  Default_Coordinates[Bonne].Bonne.easting = 0.0;
  Default_Coordinates[Bonne].Bonne.northing = 0.0;

  Get_Cassini_Parameters(&a, &f, &origin_latitude, &central_meridian,
                         &false_easting, &false_northing);
  Default_Parameters[Cassini].Cassini.origin_latitude = origin_latitude;
  Default_Parameters[Cassini].Cassini.central_meridian = central_meridian;
  Default_Parameters[Cassini].Cassini.false_easting = false_easting;
  Default_Parameters[Cassini].Cassini.false_northing = false_northing;
  Default_Coordinates[Cassini].Cassini.easting = 0.0;
  Default_Coordinates[Cassini].Cassini.northing = 0.0;

  Get_Cyl_Eq_Area_Parameters(&a, &f, &origin_latitude, &central_meridian,
                             &false_easting, &false_northing);
  Default_Parameters[Cylindrical_Equal_Area].Cylindrical_Equal_Area.origin_latitude = origin_latitude;
  Default_Parameters[Cylindrical_Equal_Area].Cylindrical_Equal_Area.central_meridian = central_meridian;
  Default_Parameters[Cylindrical_Equal_Area].Cylindrical_Equal_Area.false_easting = false_easting;
  Default_Parameters[Cylindrical_Equal_Area].Cylindrical_Equal_Area.false_northing = false_northing;
  Default_Coordinates[Cylindrical_Equal_Area].Cylindrical_Equal_Area.easting = 0.0;
  Default_Coordinates[Cylindrical_Equal_Area].Cylindrical_Equal_Area.northing = 0.0;

  Get_Eckert4_Parameters(&a, &f, &central_meridian,
                         &false_easting, &false_northing);
  Default_Parameters[Eckert4].Eckert4.central_meridian = central_meridian;
  Default_Parameters[Eckert4].Eckert4.false_easting = false_easting;
  Default_Parameters[Eckert4].Eckert4.false_northing = false_northing;
  Default_Coordinates[Eckert4].Eckert4.easting = 0.0;
  Default_Coordinates[Eckert4].Eckert4.northing = 0.0;

  Get_Eckert6_Parameters(&a, &f, &central_meridian,
                         &false_easting, &false_northing);
  Default_Parameters[Eckert6].Eckert6.central_meridian = central_meridian;
  Default_Parameters[Eckert6].Eckert6.false_easting = false_easting;
  Default_Parameters[Eckert6].Eckert6.false_northing = false_northing;
  Default_Coordinates[Eckert6].Eckert6.easting = 0.0;
  Default_Coordinates[Eckert6].Eckert6.northing = 0.0;

  Get_Equidistant_Cyl_Parameters(&a, &f, &std_parallel, &central_meridian,
                                 &false_easting, &false_northing);
  Default_Parameters[Equidistant_Cylindrical].Equidistant_Cylindrical.std_parallel = std_parallel;
  Default_Parameters[Equidistant_Cylindrical].Equidistant_Cylindrical.central_meridian = central_meridian;
  Default_Parameters[Equidistant_Cylindrical].Equidistant_Cylindrical.false_easting = false_easting;
  Default_Parameters[Equidistant_Cylindrical].Equidistant_Cylindrical.false_northing = false_northing;
  Default_Coordinates[Equidistant_Cylindrical].Equidistant_Cylindrical.easting = 0.0;
  Default_Coordinates[Equidistant_Cylindrical].Equidistant_Cylindrical.northing = 0.0;

  Get_Gnomonic_Parameters(&a, &f, &origin_latitude, &central_meridian, 
                          &false_easting, &false_northing);
  Default_Parameters[Gnomonic].Gnomonic.origin_latitude = origin_latitude;
  Default_Parameters[Gnomonic].Gnomonic.central_meridian = central_meridian;
  Default_Parameters[Gnomonic].Gnomonic.false_easting = false_easting;
  Default_Parameters[Gnomonic].Gnomonic.false_northing = false_northing;
  Default_Coordinates[Gnomonic].Gnomonic.easting = 0.0;
  Default_Coordinates[Gnomonic].Gnomonic.northing = 0.0;

  Get_Lambert_1_Parameters(&a, &f, &origin_latitude, &central_meridian, 
                           &false_easting, &false_northing, &scale_factor);
  Default_Parameters[Lambert_Conformal_Conic_1].Lambert_Conformal_Conic_1.origin_latitude = origin_latitude;
  Default_Parameters[Lambert_Conformal_Conic_1].Lambert_Conformal_Conic_1.central_meridian = central_meridian;
  Default_Parameters[Lambert_Conformal_Conic_1].Lambert_Conformal_Conic_1.false_easting = false_easting;
  Default_Parameters[Lambert_Conformal_Conic_1].Lambert_Conformal_Conic_1.false_northing = false_northing;
  Default_Parameters[Lambert_Conformal_Conic_1].Lambert_Conformal_Conic_1.scale_factor = scale_factor;
  Default_Coordinates[Lambert_Conformal_Conic_1].Lambert_Conformal_Conic_1.easting = 0.0;
  Default_Coordinates[Lambert_Conformal_Conic_1].Lambert_Conformal_Conic_1.northing = 0.0;

  Get_Lambert_Parameters(&a, &f, &origin_latitude, &central_meridian, 
                         &std_parallel_1, &std_parallel_2, &false_easting, &false_northing);
  Default_Parameters[Lambert_Conformal_Conic].Lambert_Conformal_Conic.std_parallel_1 = std_parallel_1;
  Default_Parameters[Lambert_Conformal_Conic].Lambert_Conformal_Conic.std_parallel_2 = std_parallel_2;
  Default_Parameters[Lambert_Conformal_Conic].Lambert_Conformal_Conic.origin_latitude = origin_latitude;
  Default_Parameters[Lambert_Conformal_Conic].Lambert_Conformal_Conic.central_meridian = central_meridian;
  Default_Parameters[Lambert_Conformal_Conic].Lambert_Conformal_Conic.false_easting = false_easting;
  Default_Parameters[Lambert_Conformal_Conic].Lambert_Conformal_Conic.false_northing = false_northing;
  Default_Coordinates[Lambert_Conformal_Conic].Lambert_Conformal_Conic.easting = 0.0;
  Default_Coordinates[Lambert_Conformal_Conic].Lambert_Conformal_Conic.northing = 0.0;

  Get_Local_Cartesian_Parameters(&a, &f, &origin_latitude, &origin_longitude,
                                 &origin_height, &orientation);
  Default_Parameters[Local_Cartesian].Local_Cartesian.origin_latitude = origin_latitude;
  Default_Parameters[Local_Cartesian].Local_Cartesian.origin_longitude = origin_longitude;
  Default_Parameters[Local_Cartesian].Local_Cartesian.origin_height = origin_height;
  Default_Parameters[Local_Cartesian].Local_Cartesian.orientation = orientation;
  Default_Coordinates[Local_Cartesian].Local_Cartesian.x = 0.0;
  Default_Coordinates[Local_Cartesian].Local_Cartesian.y = 0.0;
  Default_Coordinates[Local_Cartesian].Local_Cartesian.z = 0.0;

  Get_Mercator_Parameters(&a, &f, &origin_latitude, &central_meridian,
                          &false_easting, &false_northing, &scale_factor);
  Default_Parameters[Mercator].Mercator.origin_latitude = origin_latitude;
  Default_Parameters[Mercator].Mercator.central_meridian = central_meridian;
  Default_Parameters[Mercator].Mercator.false_easting = false_easting;
  Default_Parameters[Mercator].Mercator.false_northing = false_northing;
  Default_Parameters[Mercator].Mercator.scale_factor = scale_factor;
  Default_Coordinates[Mercator].Mercator.easting = 0.0;
  Default_Coordinates[Mercator].Mercator.northing = 0.0;

  strcpy(Default_Coordinates[MGRS].MGRS.string,"31NEA0000000000");

  Get_Miller_Parameters(&a, &f, &central_meridian,
                        &false_easting, &false_northing);
  Default_Parameters[Miller_Cylindrical].Miller_Cylindrical.central_meridian = central_meridian;
  Default_Parameters[Miller_Cylindrical].Miller_Cylindrical.false_easting = false_easting;
  Default_Parameters[Miller_Cylindrical].Miller_Cylindrical.false_northing = false_northing;
  Default_Coordinates[Miller_Cylindrical].Miller_Cylindrical.easting = 0.0;
  Default_Coordinates[Miller_Cylindrical].Miller_Cylindrical.northing = 0.0;

  Get_Mollweide_Parameters(&a, &f, &central_meridian,
                           &false_easting, &false_northing);
  Default_Parameters[Mollweide].Mollweide.central_meridian = central_meridian;
  Default_Parameters[Mollweide].Mollweide.false_easting = false_easting;
  Default_Parameters[Mollweide].Mollweide.false_northing = false_northing;
  Default_Coordinates[Mollweide].Mollweide.easting = 0.0;
  Default_Coordinates[Mollweide].Mollweide.northing = 0.0;

  Get_Neys_Parameters(&a, &f, &origin_latitude, &central_meridian, 
                         &std_parallel_1, &false_easting, &false_northing);
  Default_Parameters[Neys].Neys.std_parallel_1 = std_parallel_1;
  Default_Parameters[Neys].Neys.origin_latitude = origin_latitude;
  Default_Parameters[Neys].Neys.central_meridian = central_meridian;
  Default_Parameters[Neys].Neys.false_easting = false_easting;
  Default_Parameters[Neys].Neys.false_northing = false_northing;
  Default_Coordinates[Neys].Neys.easting = 0.0;
  Default_Coordinates[Neys].Neys.northing = 0.0;
 
  Default_Coordinates[NZMG].NZMG.easting = 2510000.0;
  Default_Coordinates[NZMG].NZMG.northing = 6023150.0;

  Get_Oblique_Mercator_Parameters(&a, &f, &origin_latitude, &latitude_1, &longitude_1,
                                  &latitude_2, &longitude_2, &false_easting, &false_northing, &scale_factor);
  Default_Parameters[Oblique_Mercator].Oblique_Mercator.origin_latitude = origin_latitude;
  Default_Parameters[Oblique_Mercator].Oblique_Mercator.latitude_1 = latitude_1;
  Default_Parameters[Oblique_Mercator].Oblique_Mercator.longitude_1 = longitude_1;
  Default_Parameters[Oblique_Mercator].Oblique_Mercator.latitude_2 = latitude_2;
  Default_Parameters[Oblique_Mercator].Oblique_Mercator.longitude_2 = longitude_2;
  Default_Parameters[Oblique_Mercator].Oblique_Mercator.false_easting = false_easting;
  Default_Parameters[Oblique_Mercator].Oblique_Mercator.false_northing = false_northing;
  Default_Parameters[Oblique_Mercator].Oblique_Mercator.scale_factor = scale_factor;
  Default_Coordinates[Oblique_Mercator].Oblique_Mercator.easting = 0.0;
  Default_Coordinates[Oblique_Mercator].Oblique_Mercator.northing = 0.0;

  Get_Orthographic_Parameters(&a, &f, &origin_latitude, &central_meridian,
                              &false_easting, &false_northing);
  Default_Parameters[Orthographic].Orthographic.origin_latitude = origin_latitude;
  Default_Parameters[Orthographic].Orthographic.central_meridian = central_meridian;
  Default_Parameters[Orthographic].Orthographic.false_easting = false_easting;
  Default_Parameters[Orthographic].Orthographic.false_northing = false_northing;
  Default_Coordinates[Orthographic].Orthographic.easting = 0.0;
  Default_Coordinates[Orthographic].Orthographic.northing = 0.0;

  Get_Polar_Stereographic_Parameters(&a, &f, &latitude_of_true_scale, &longitude_down_from_pole,
                                     &false_easting, &false_northing);
  Default_Parameters[Polar_Stereo].Polar_Stereo.latitude_of_true_scale = latitude_of_true_scale;
  Default_Parameters[Polar_Stereo].Polar_Stereo.longitude_down_from_pole = longitude_down_from_pole;
  Default_Parameters[Polar_Stereo].Polar_Stereo.false_easting = false_easting;
  Default_Parameters[Polar_Stereo].Polar_Stereo.false_northing = false_northing;
  Default_Coordinates[Polar_Stereo].Polar_Stereo.northing = 0.0;
  Default_Coordinates[Polar_Stereo].Polar_Stereo.easting = 0.0;

  Get_Polyconic_Parameters(&a, &f, &origin_latitude, &central_meridian,
                           &false_easting, &false_northing);
  Default_Parameters[Polyconic].Polyconic.origin_latitude = origin_latitude;
  Default_Parameters[Polyconic].Polyconic.central_meridian = central_meridian;
  Default_Parameters[Polyconic].Polyconic.false_easting = false_easting;
  Default_Parameters[Polyconic].Polyconic.false_northing = false_northing;
  Default_Coordinates[Polyconic].Polyconic.easting = 0.0;
  Default_Coordinates[Polyconic].Polyconic.northing = 0.0;

  Get_Sinusoidal_Parameters(&a, &f, &central_meridian, &false_easting, &false_northing);
  Default_Parameters[Sinusoidal].Sinusoidal.central_meridian = central_meridian;
  Default_Parameters[Sinusoidal].Sinusoidal.false_easting = false_easting;
  Default_Parameters[Sinusoidal].Sinusoidal.false_northing = false_northing;
  Default_Coordinates[Sinusoidal].Sinusoidal.easting = 0.0;
  Default_Coordinates[Sinusoidal].Sinusoidal.northing = 0.0;

  Get_Stereographic_Parameters(&a, &f, &origin_latitude, &central_meridian,
                              &false_easting, &false_northing);
  Default_Parameters[Stereographic].Stereographic.origin_latitude = origin_latitude;
  Default_Parameters[Stereographic].Stereographic.central_meridian = central_meridian;
  Default_Parameters[Stereographic].Stereographic.false_easting = false_easting;
  Default_Parameters[Stereographic].Stereographic.false_northing = false_northing;
  Default_Coordinates[Stereographic].Stereographic.easting = 0.0;
  Default_Coordinates[Stereographic].Stereographic.northing = 0.0;

  Get_Trans_Cyl_Eq_Area_Parameters(&a, &f, &origin_latitude, &central_meridian,
                                   &false_easting, &false_northing, &scale_factor);
  Default_Parameters[Transverse_Cylindrical_Equal_Area].Transverse_Cylindrical_Equal_Area.origin_latitude = origin_latitude;
  Default_Parameters[Transverse_Cylindrical_Equal_Area].Transverse_Cylindrical_Equal_Area.central_meridian = central_meridian;
  Default_Parameters[Transverse_Cylindrical_Equal_Area].Transverse_Cylindrical_Equal_Area.false_easting = false_easting;
  Default_Parameters[Transverse_Cylindrical_Equal_Area].Transverse_Cylindrical_Equal_Area.false_northing = false_northing;
  Default_Parameters[Transverse_Cylindrical_Equal_Area].Transverse_Cylindrical_Equal_Area.scale_factor = 1.0;
  Default_Coordinates[Transverse_Cylindrical_Equal_Area].Transverse_Cylindrical_Equal_Area.easting = 0.0;
  Default_Coordinates[Transverse_Cylindrical_Equal_Area].Transverse_Cylindrical_Equal_Area.northing = 0.0;

  Get_Transverse_Mercator_Parameters(&a, &f, &origin_latitude, &central_meridian,
                                     &false_easting, &false_northing, &scale_factor);
  Default_Parameters[Transverse_Mercator].Transverse_Mercator.origin_latitude = origin_latitude;
  Default_Parameters[Transverse_Mercator].Transverse_Mercator.central_meridian = central_meridian;
  Default_Parameters[Transverse_Mercator].Transverse_Mercator.false_easting = false_easting;
  Default_Parameters[Transverse_Mercator].Transverse_Mercator.false_northing = false_northing;
  Default_Parameters[Transverse_Mercator].Transverse_Mercator.scale_factor = scale_factor;
  Default_Coordinates[Transverse_Mercator].Transverse_Mercator.easting = 0.0;
  Default_Coordinates[Transverse_Mercator].Transverse_Mercator.northing = 0.0;

  Default_Coordinates[UPS].UPS.easting = 2000000.0;
  Default_Coordinates[UPS].UPS.northing = 2000000.0;
  Default_Coordinates[UPS].UPS.hemisphere = 'N';

  Default_Parameters[UTM].UTM.override = 0;
  Default_Parameters[UTM].UTM.zone = 30;
  Default_Coordinates[UTM].UTM.zone = 31;
  Default_Coordinates[UTM].UTM.hemisphere = 'N';
  Default_Coordinates[UTM].UTM.easting = 500000.0;
  Default_Coordinates[UTM].UTM.northing = 0.0;

  Get_Van_der_Grinten_Parameters(&a, &f, &central_meridian, &false_easting, &false_northing);
  Default_Parameters[Van_der_Grinten].Van_der_Grinten.central_meridian = central_meridian;
  Default_Parameters[Van_der_Grinten].Van_der_Grinten.false_easting = false_easting;
  Default_Parameters[Van_der_Grinten].Van_der_Grinten.false_northing = false_northing;
  Default_Coordinates[Van_der_Grinten].Van_der_Grinten.easting = 0.0;
  Default_Coordinates[Van_der_Grinten].Van_der_Grinten.northing = 0.0;

} /* Set_Defaults */


void Initialize_Coordinate_System
( const Coordinate_Type    System,
  Parameter_Tuple   *Parameters,
  Coordinate_Tuple  *Coordinates )
/*
 *  The function Initialize_Coordinate_System set the coordinate system parameters and
 *  coordinate values for the specified coordinate system to the default values.
 *  System      : Coordinate system type                                (input)
 *  Parameters  : Parameters of specified coordinate system type        (output)
 *  Coordinates : Coordinates of specified coordinate system type       (output)
 */
{ /* Initialize_Coordinate_System */
  *Parameters = Default_Parameters[System];
  *Coordinates = Default_Coordinates[System];
} /* Initialize_Coordinate_System */


/***************************************************************************/
/*
 *                              FUNCTIONS
 */


long Initialize_Engine_File(const char *File_Ellipsoids, const char *File_7Parms, const char *File_3Parms, const char *File_Geoid)
/*
 *  The function Initialize_Engine sets the initial state of the engine in
 *  in preparation for coordinate conversion and/or datum transformation
 *  operations.
 */
{ /* Initialize_Engine */
  long error_code = ENGINE_NO_ERROR;
  int i, j;
  Engine_Initialized	= 0;

  if (Initialize_Ellipsoids_File(File_Ellipsoids))
    error_code |= ENGINE_ELLIPSOID_ERROR;
  if (Initialize_Datums_File(File_7Parms, File_3Parms))
    error_code |= ENGINE_DATUM_ERROR;
  if (Initialize_Geoid_File(File_Geoid))
    error_code |= ENGINE_GEOID_ERROR;

  /* Initialize Coordinate System Table */
  strcpy(Coordinate_System_Table[Geodetic].Name, "Geodetic");
  strcpy(Coordinate_System_Table[Geodetic].Code, "GD");
  strcpy(Coordinate_System_Table[GEOREF].Name, "GEOREF");
  strcpy(Coordinate_System_Table[GEOREF].Code, "GE");
  strcpy(Coordinate_System_Table[Geocentric].Name, "Geocentric");
  strcpy(Coordinate_System_Table[Geocentric].Code, "GC");
  strcpy(Coordinate_System_Table[Local_Cartesian].Name, "Local Cartesian");
  strcpy(Coordinate_System_Table[Local_Cartesian].Code, "LC");
  strcpy(Coordinate_System_Table[MGRS].Name, "Military Grid Reference System (MGRS)");
  strcpy(Coordinate_System_Table[MGRS].Code, "MG");
  strcpy(Coordinate_System_Table[UPS].Name, "Universal Polar Stereographic (UPS)");
  strcpy(Coordinate_System_Table[UPS].Code, "UP");
  strcpy(Coordinate_System_Table[UTM].Name, "Universal Transverse Mercator (UTM)");
  strcpy(Coordinate_System_Table[UTM].Code, "UT");
  strcpy(Coordinate_System_Table[Albers_Equal_Area_Conic].Name, "Albers Equal Area Conic");
  strcpy(Coordinate_System_Table[Albers_Equal_Area_Conic].Code, "AC");
  strcpy(Coordinate_System_Table[Azimuthal_Equidistant].Name, "Azimuthal Equidistant (S)");
  strcpy(Coordinate_System_Table[Azimuthal_Equidistant].Code, "AL");
  strcpy(Coordinate_System_Table[Bonne].Name, "Bonne");
  strcpy(Coordinate_System_Table[Bonne].Code, "BF");
  strcpy(Coordinate_System_Table[BNG].Name, "British National Grid (BNG)");
  strcpy(Coordinate_System_Table[BNG].Code, "BN");
  strcpy(Coordinate_System_Table[Cassini].Name, "Cassini");
  strcpy(Coordinate_System_Table[Cassini].Code, "CS");
  strcpy(Coordinate_System_Table[Cylindrical_Equal_Area].Name, "Cylindrical Equal Area");
  strcpy(Coordinate_System_Table[Cylindrical_Equal_Area].Code, "LI");
  strcpy(Coordinate_System_Table[Equidistant_Cylindrical].Name, "Equidistant Cylindrical (S)");
  strcpy(Coordinate_System_Table[Equidistant_Cylindrical].Code, "CP");
  strcpy(Coordinate_System_Table[Eckert4].Name, "Eckert IV (S)");
  strcpy(Coordinate_System_Table[Eckert4].Code, "EF");
  strcpy(Coordinate_System_Table[Eckert6].Name, "Eckert VI (S)");
  strcpy(Coordinate_System_Table[Eckert6].Code, "ED");
  strcpy(Coordinate_System_Table[Gnomonic].Name, "Gnomonic (S)");
  strcpy(Coordinate_System_Table[Gnomonic].Code, "GN");
  strcpy(Coordinate_System_Table[Lambert_Conformal_Conic_1].Name, "Lambert Conformal Conic (1 parallel)");
  strcpy(Coordinate_System_Table[Lambert_Conformal_Conic_1].Code, "L1");
  strcpy(Coordinate_System_Table[Lambert_Conformal_Conic].Name, "Lambert Conformal Conic (2 parallel)");
  strcpy(Coordinate_System_Table[Lambert_Conformal_Conic].Code, "LE");
  strcpy(Coordinate_System_Table[Mercator].Name, "Mercator");
  strcpy(Coordinate_System_Table[Mercator].Code, "MC");
  strcpy(Coordinate_System_Table[Miller_Cylindrical].Name, "Miller Cylindrical (S)");
  strcpy(Coordinate_System_Table[Miller_Cylindrical].Code, "MH");
  strcpy(Coordinate_System_Table[Mollweide].Name, "Mollweide (S)");
  strcpy(Coordinate_System_Table[Mollweide].Code, "MP");
  strcpy(Coordinate_System_Table[NZMG].Name, "New Zealand Map Grid (NZMG)");
  strcpy(Coordinate_System_Table[NZMG].Code, "NT");
  strcpy(Coordinate_System_Table[Neys].Name, "Ney's (Modified Lambert Conformal Conic)");
  strcpy(Coordinate_System_Table[Neys].Code, "NY");
  strcpy(Coordinate_System_Table[Oblique_Mercator].Name, "Oblique Mercator");
  strcpy(Coordinate_System_Table[Oblique_Mercator].Code, "OC");
  strcpy(Coordinate_System_Table[Orthographic].Name, "Orthographic (S)");
  strcpy(Coordinate_System_Table[Orthographic].Code, "OD");
  strcpy(Coordinate_System_Table[Polar_Stereo].Name, "Polar Stereographic");
  strcpy(Coordinate_System_Table[Polar_Stereo].Code, "PG");
  strcpy(Coordinate_System_Table[Polyconic].Name, "Polyconic");
  strcpy(Coordinate_System_Table[Polyconic].Code, "PH");
  strcpy(Coordinate_System_Table[Sinusoidal].Name, "Sinusoidal");
  strcpy(Coordinate_System_Table[Sinusoidal].Code, "SA");
  strcpy(Coordinate_System_Table[Stereographic].Name, "Stereographic (S)");
  strcpy(Coordinate_System_Table[Stereographic].Code, "SD");
  strcpy(Coordinate_System_Table[Transverse_Cylindrical_Equal_Area].Name, "Transverse Cylindrical Equal Area");
  strcpy(Coordinate_System_Table[Transverse_Cylindrical_Equal_Area].Code, "TX");
  strcpy(Coordinate_System_Table[Transverse_Mercator].Name, "Transverse Mercator");
  strcpy(Coordinate_System_Table[Transverse_Mercator].Code, "TC");
  strcpy(Coordinate_System_Table[Van_der_Grinten].Name, "Van der Grinten");
  strcpy(Coordinate_System_Table[Van_der_Grinten].Code, "VA");
  if (!error_code)
  {
    Engine_Initialized = TRUE;
    Set_Defaults();
    Datum_Count (&Number_of_Datums);
    Set_Coordinate_System(File,Input,Geodetic);
    Set_Coordinate_System(File,Output,Geodetic);
    Set_Coordinate_System(Interactive,Input,Geodetic);
    Set_Coordinate_System(Interactive,Output,Geodetic);
    for (i=0;i<2;i++)
      for (j=0;j<2;j++)
      {
        CS_State[i][j].datum_Index = 1;
        CS_State[i][j].status = ENGINE_NO_ERROR;
      }

    Engine_Precision = Tenth_of_Second;
  }
  return (error_code);
} /* Initialize_Engine */


long Initialize_Engine()
/*
 *  The function Initialize_Engine sets the initial state of the engine in
 *  in preparation for coordinate conversion and/or datum transformation
 *  operations.
 */
{ /* Initialize_Engine */
  long error_code = ENGINE_NO_ERROR;
  int i, j;
  if (Engine_Initialized)
    return (error_code);
  if (Initialize_Ellipsoids())
    error_code |= ENGINE_ELLIPSOID_ERROR;
  if (Initialize_Datums())
    error_code |= ENGINE_DATUM_ERROR;
  if (Initialize_Geoid())
    error_code |= ENGINE_GEOID_ERROR;
  /* Initialize Coordinate System Table */
  strcpy(Coordinate_System_Table[Geodetic].Name, "Geodetic");
  strcpy(Coordinate_System_Table[Geodetic].Code, "GD");
  strcpy(Coordinate_System_Table[GEOREF].Name, "GEOREF");
  strcpy(Coordinate_System_Table[GEOREF].Code, "GE");
  strcpy(Coordinate_System_Table[Geocentric].Name, "Geocentric");
  strcpy(Coordinate_System_Table[Geocentric].Code, "GC");
  strcpy(Coordinate_System_Table[Local_Cartesian].Name, "Local Cartesian");
  strcpy(Coordinate_System_Table[Local_Cartesian].Code, "LC");
  strcpy(Coordinate_System_Table[MGRS].Name, "Military Grid Reference System (MGRS)");
  strcpy(Coordinate_System_Table[MGRS].Code, "MG");
  strcpy(Coordinate_System_Table[UPS].Name, "Universal Polar Stereographic (UPS)");
  strcpy(Coordinate_System_Table[UPS].Code, "UP");
  strcpy(Coordinate_System_Table[UTM].Name, "Universal Transverse Mercator (UTM)");
  strcpy(Coordinate_System_Table[UTM].Code, "UT");
  strcpy(Coordinate_System_Table[Albers_Equal_Area_Conic].Name, "Albers Equal Area Conic");
  strcpy(Coordinate_System_Table[Albers_Equal_Area_Conic].Code, "AC");
  strcpy(Coordinate_System_Table[Azimuthal_Equidistant].Name, "Azimuthal Equidistant (S)");
  strcpy(Coordinate_System_Table[Azimuthal_Equidistant].Code, "AL");
  strcpy(Coordinate_System_Table[Bonne].Name, "Bonne");
  strcpy(Coordinate_System_Table[Bonne].Code, "BF");
  strcpy(Coordinate_System_Table[BNG].Name, "British National Grid (BNG)");
  strcpy(Coordinate_System_Table[BNG].Code, "BN");
  strcpy(Coordinate_System_Table[Cassini].Name, "Cassini");
  strcpy(Coordinate_System_Table[Cassini].Code, "CS");
  strcpy(Coordinate_System_Table[Cylindrical_Equal_Area].Name, "Cylindrical Equal Area");
  strcpy(Coordinate_System_Table[Cylindrical_Equal_Area].Code, "LI");
  strcpy(Coordinate_System_Table[Equidistant_Cylindrical].Name, "Equidistant Cylindrical (S)");
  strcpy(Coordinate_System_Table[Equidistant_Cylindrical].Code, "CP");
  strcpy(Coordinate_System_Table[Eckert4].Name, "Eckert IV (S)");
  strcpy(Coordinate_System_Table[Eckert4].Code, "EF");
  strcpy(Coordinate_System_Table[Eckert6].Name, "Eckert VI (S)");
  strcpy(Coordinate_System_Table[Eckert6].Code, "ED");
  strcpy(Coordinate_System_Table[Gnomonic].Name, "Gnomonic (S)");
  strcpy(Coordinate_System_Table[Gnomonic].Code, "GN");
  strcpy(Coordinate_System_Table[Lambert_Conformal_Conic_1].Name, "Lambert Conformal Conic (1 parallel)");
  strcpy(Coordinate_System_Table[Lambert_Conformal_Conic_1].Code, "L1");
  strcpy(Coordinate_System_Table[Lambert_Conformal_Conic].Name, "Lambert Conformal Conic (2 parallel)");
  strcpy(Coordinate_System_Table[Lambert_Conformal_Conic].Code, "LE");
  strcpy(Coordinate_System_Table[Mercator].Name, "Mercator");
  strcpy(Coordinate_System_Table[Mercator].Code, "MC");
  strcpy(Coordinate_System_Table[Miller_Cylindrical].Name, "Miller Cylindrical (S)");
  strcpy(Coordinate_System_Table[Miller_Cylindrical].Code, "MH");
  strcpy(Coordinate_System_Table[Mollweide].Name, "Mollweide (S)");
  strcpy(Coordinate_System_Table[Mollweide].Code, "MP");
  strcpy(Coordinate_System_Table[NZMG].Name, "New Zealand Map Grid (NZMG)");
  strcpy(Coordinate_System_Table[NZMG].Code, "NT");
  strcpy(Coordinate_System_Table[Neys].Name, "Ney's (Modified Lambert Conformal Conic)");
  strcpy(Coordinate_System_Table[Neys].Code, "NY");
  strcpy(Coordinate_System_Table[Oblique_Mercator].Name, "Oblique Mercator");
  strcpy(Coordinate_System_Table[Oblique_Mercator].Code, "OC");
  strcpy(Coordinate_System_Table[Orthographic].Name, "Orthographic (S)");
  strcpy(Coordinate_System_Table[Orthographic].Code, "OD");
  strcpy(Coordinate_System_Table[Polar_Stereo].Name, "Polar Stereographic");
  strcpy(Coordinate_System_Table[Polar_Stereo].Code, "PG");
  strcpy(Coordinate_System_Table[Polyconic].Name, "Polyconic");
  strcpy(Coordinate_System_Table[Polyconic].Code, "PH");
  strcpy(Coordinate_System_Table[Sinusoidal].Name, "Sinusoidal");
  strcpy(Coordinate_System_Table[Sinusoidal].Code, "SA");
  strcpy(Coordinate_System_Table[Stereographic].Name, "Stereographic (S)");
  strcpy(Coordinate_System_Table[Stereographic].Code, "SD");
  strcpy(Coordinate_System_Table[Transverse_Cylindrical_Equal_Area].Name, "Transverse Cylindrical Equal Area");
  strcpy(Coordinate_System_Table[Transverse_Cylindrical_Equal_Area].Code, "TX");
  strcpy(Coordinate_System_Table[Transverse_Mercator].Name, "Transverse Mercator");
  strcpy(Coordinate_System_Table[Transverse_Mercator].Code, "TC");
  strcpy(Coordinate_System_Table[Van_der_Grinten].Name, "Van der Grinten");
  strcpy(Coordinate_System_Table[Van_der_Grinten].Code, "VA");
  if (!error_code)
  {
    Engine_Initialized = TRUE;
    Set_Defaults();
    Datum_Count (&Number_of_Datums);
    Set_Coordinate_System(File,Input,Geodetic);
    Set_Coordinate_System(File,Output,Geodetic);
    Set_Coordinate_System(Interactive,Input,Geodetic);
    Set_Coordinate_System(Interactive,Output,Geodetic);
    for (i=0;i<2;i++)
      for (j=0;j<2;j++)
      {
        CS_State[i][j].datum_Index = 1;
        CS_State[i][j].status = ENGINE_NO_ERROR;
      }

    Engine_Precision = Tenth_of_Second;
  }
  return (error_code);
} /* Initialize_Engine */


int Valid_Conversion(Coordinate_Type input_system, Coordinate_Type output_system, long input_datum, long output_datum)
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
{
  char input_ellipsoid[ELLIPSOID_CODE_LENGTH];
  char output_ellipsoid[ELLIPSOID_CODE_LENGTH];
  double input_south_lat, input_north_lat, input_west_lon, input_east_lon;
  double output_south_lat, output_north_lat, output_west_lon, output_east_lon;

  Get_Datum_Ellipsoid_Code (input_datum, input_ellipsoid);
  Get_Datum_Ellipsoid_Code (output_datum, output_ellipsoid);

  // If British National Grid is chosen, ellipsoid should be Airy
  // If New Zealand Map Grid is chosen, ellipsoid should be International
  if((input_system == BNG && strcmp(input_ellipsoid, "AA") != 0) || 
     (output_system == BNG && strcmp(output_ellipsoid, "AA") != 0) ||
     (input_system == NZMG && strcmp(input_ellipsoid, "IN") != 0) ||
     (output_system == NZMG && strcmp(output_ellipsoid, "IN") != 0))
    return RED;

  Get_Datum_Valid_Rectangle(input_datum, &input_south_lat, &input_north_lat, &input_west_lon, &input_east_lon);
  Get_Datum_Valid_Rectangle(output_datum, &output_south_lat, &output_north_lat, &output_west_lon, &output_east_lon);
 
  // Bounding rectangles don't overlap
  if((input_west_lon >= output_east_lon) || 
     (input_east_lon <= output_west_lon) ||
     (input_south_lat >= output_north_lat) ||
     (input_north_lat <= output_south_lat))
    return YELLOW;
  else
    return GREEN;
}


long Get_Coordinate_System_Count ( long *Count )
/*
 *   Count    : The number of coordinate systems supported. (output) 
 *
 * The function Get_Coordinate_System_Count returns the number of coordinate
 * systems (including projections and grids) that are supported.
 */
{ /* Get_Coordinate_System_Count */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  *Count = NUMBER_COORD_SYS;
  return (error_code);
} /* Get_Coordinate_System_Count */


long Get_Coordinate_System_Index ( const char *Code,
                                   long *Index )
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
{ /* Get_Coordinate_System_Index */
  char temp_code[COORD_SYS_CODE_LENGTH];
  long error_code = ENGINE_NO_ERROR;
  long i = 0;        /* index for coordinate system table */
  long j = 0;
  *Index = 0;
  if (Engine_Initialized)
  {
    while (j < COORD_SYS_CODE_LENGTH)
    {
      temp_code[j] = (char)toupper(Code[j]);
      j++;
    }
    temp_code[COORD_SYS_CODE_LENGTH - 1] = 0;
    while ((i < NUMBER_COORD_SYS)
           && strcmp(temp_code, Coordinate_System_Table[i].Code))
    {
      i++;
    }
    if (strcmp(temp_code, Coordinate_System_Table[i].Code))
      error_code |= ENGINE_INVALID_CODE_ERROR;
    else
      *Index = i+1;
  }
  else
  {
    error_code |= ENGINE_NOT_INITIALIZED;
  }
  return (error_code);
} /* Get_Coordinate_System_Index */


long Get_Coordinate_System_Type ( const long Index,
                                  Coordinate_Type *System ) 
/*
 *    Index   : Index of a specific coordinate system            (input)
 *    System  : Type of the coordinate system referencd by index (output)
 *
 *  The Function Get_Coordinate_System_Type returns the type of the coordinate 
 *  system with the specified index.  If theh index is valid, ENGINE_NO_ERROR is 
 *  returned, otherwise ENGINE_INVALID_INDEX_ERROR is returned.
 */
{ /* Get_Coordinate_System_Type */
  long error_code = ENGINE_NO_ERROR;
  if (Engine_Initialized)
  {
    if ((Index < 1) || (Index > NUMBER_COORD_SYS))
      error_code |= ENGINE_INVALID_INDEX_ERROR;
    else
      *System = ((Coordinate_Type)(Index-1));
  }
  else
    error_code |= ENGINE_NOT_INITIALIZED;
  return (error_code);
} /* Get_Coordinate_System_Type */


long Get_Coordinate_System_Name ( const long Index,
                                  char *Name ) 
/*
 *    Index   : Index of a specific coordinate system            (input)
 *    Name    : Name of the coordinate system referencd by index (output)
 *
 *  The Function Get_Coordinate_System_Name returns the name of the coordinate 
 *  system with the specified index.  If theh index is valid, ENGINE_NO_ERROR is 
 *  returned, otherwise ENGINE_INVALID_INDEX_ERROR is returned.
 */
{ /* Get_Coordinate_System_Name */
  long error_code = ENGINE_NO_ERROR;
  strcpy(Name,"");
  if (Engine_Initialized)
  {
    if ((Index < 1) || (Index > NUMBER_COORD_SYS))
      error_code |= ENGINE_INVALID_INDEX_ERROR;
    else
      strcpy(Name, Coordinate_System_Table[Index-1].Name);
  }
  else
    error_code |= ENGINE_NOT_INITIALIZED;
  return (error_code);
} /* Get_Coordinate_System_Name */


long Get_Coordinate_System_Code ( const long Index,
                                  char *Code ) 
/*
 *    Index   : Index of a specific coordinate system            (input)
 *    Code    : 2-letter coordinate system code.                 (output)
 *
 *  The Function Get_Coordinate_System_Code returns the 2-letter code for the 
 *  coordinate system with the specified index.  If index is valid, ENGINE_NO_ERROR  
 *  is returned, otherwise ENGINE_INVALID_INDEX_ERROR is returned.
 */
{ /* Begin Get_Coordinate_System_Code */
  long error_code = ENGINE_NO_ERROR;
  strcpy(Code,"");
  if (Engine_Initialized)
  {
    if ((Index < 1) || (Index > NUMBER_COORD_SYS))
      error_code |= ENGINE_INVALID_INDEX_ERROR;
    else
      strcpy(Code, Coordinate_System_Table[Index-1].Code);
  }
  else
    error_code |= ENGINE_NOT_INITIALIZED;
  return (error_code);
} /* Get_Coordinate_System_Code */


long Set_Coordinate_System
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Coordinate_Type     System )
/*
 *  The function Set_Coordinate_System sets the coordinate system for the
 *  specified state to the specified coordinate system type.
 *  State      : Indicates whether the coordinate system is to be used for
 *               interactive or file processing                        (input)
 *  Direction  : Indicates whether the coordinate system is to be used for
 *               input or output                                       (input)
 *  System     : Identifies the coordinate system to be used           (input)
 */
{ /* Set_Coordinate_System */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    CS_State[State][Direction].type = System;
    CS_State[State][Direction].status = ENGINE_NO_ERROR;
    Initialize_Coordinate_System ( System,
                                   &(CS_State[State][Direction].parameters),
                                   &(CS_State[State][Direction].coordinates));
  }
  return (error_code);
} /* Set_Coordinate_System */


long Get_Coordinate_System
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Coordinate_Type           *System )
/*
 *  The function Get_Coordinate_System returns the current coordinate system
 *  type for the specified state.
 *  State      : Indicates whether the coordinate system is to be used for
 *               interactive or file processing                        (input)
 *  Direction  : Indicates whether the coordinate system is to be used for
 *               input or output                                       (input)
 *  System     : Identifies current coordinate system type             (output)
 */
{ /* Get_Coordinate_System */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    *System = CS_State[State][Direction].type;
  }
  return (error_code);
} /* Get_Coordinate_System */


long Get_Datum_Count ( long *Count )
/*
 *  The function Get_Datum_Count returns the number of Datums in the table
 *  if the table was initialized without error.
 *
 *  Count   : number of datums in the datum table                   (output)
 */
{ /* Get_Datum_Count */
  long error_code = ENGINE_NO_ERROR;
  long temp_error;

  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  else
  {
    temp_error = Datum_Count (Count);
    if (temp_error != DATUM_NO_ERROR)
      error_code |= ENGINE_DATUM_ERROR;
  }
  return (error_code);
} /* Get_Datum_Count */


long Get_Datum_Index ( const char *Code, 
                       long *Index )
/*
 *  The function Get_Datum_Index returns the index of the datum with the 
 *  specified code.
 *
 *  Code    : The datum code being searched for                     (input)
 *  Index   : The index of the datum in the table with the          (output)
 *              specified code
 */
{ /* Get_Datum_Index */
  long error_code = ENGINE_NO_ERROR;
  long temp_error;

  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  else
  {
    temp_error = Datum_Index (Code, Index);
    if (temp_error == DATUM_INVALID_CODE_ERROR)
      error_code |= ENGINE_INVALID_CODE_ERROR;
    else if (temp_error != DATUM_NO_ERROR)
      error_code |= ENGINE_DATUM_ERROR;
  }
  return (error_code);
} /* Get_Datum_Index */


long Get_Datum_Name ( const long Index,
                      char *Name )
/*
 *  The function Get_Datum_Name returns the name of the datum referenced by
 *  index.
 *
 *  Index   : The index of a given datum in the datum table         (input)
 *  Name    : The datum name of the datum referenced by index       (output)
 */
{ /* Get_Datum_Name */
  long error_code = ENGINE_NO_ERROR;
  long temp_error;

  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  else
  {
    temp_error = Datum_Name (Index, Name);
    if (temp_error == DATUM_INVALID_INDEX_ERROR)
      error_code |= ENGINE_INVALID_INDEX_ERROR;
    else if (temp_error != DATUM_NO_ERROR)
      error_code |= ENGINE_DATUM_ERROR;
  }
  return (error_code);
} /* Get_Datum_Name */


long Get_Datum_Code ( const long Index,
                      char *Code )
/*
 *  The function Get_Datum_Code returns the 5-letter code of the datum
 *  referenced by index.
 *
 *  Index   : The index of a given datum in the datum table         (input)
 *  Code    : The datum code of the datum referenced by index       (output)
 */
{ /* Get_Datum_Code */
  long error_code = ENGINE_NO_ERROR;
  long temp_error;

  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  else
  {
    temp_error = Datum_Code (Index, Code);
    if (temp_error == DATUM_INVALID_INDEX_ERROR)
      error_code |= ENGINE_INVALID_INDEX_ERROR;
    else if (temp_error != DATUM_NO_ERROR)
      error_code |= ENGINE_DATUM_ERROR;
  }
  return (error_code);
} /* Get_Datum_Code */


long Get_Datum_Ellipsoid_Code ( const long Index,
                                char *Code )
/*
 *  The function Get_Datum_Ellipsoid_Code returns the 2-letter ellipsoid code 
 *  for the ellipsoid associated with the datum referenced by index.
 *
 *  Index   : The index of a given datum in the datum table           (input)
 *  Code    : The ellisoid code for the ellipsoid associated with the (output)
 *               datum referenced by index 
 */
{ /* Get_Datum_Ellipsoid_Code */
  long error_code = ENGINE_NO_ERROR;
  long temp_error;

  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  else
  {
    temp_error = Datum_Ellipsoid_Code (Index, Code);
    if (temp_error == DATUM_INVALID_INDEX_ERROR)
      error_code |= ENGINE_INVALID_INDEX_ERROR;
    else if (temp_error != DATUM_NO_ERROR)
      error_code |= ENGINE_DATUM_ERROR;
  }
  return (error_code);
} /* Get_Datum_Ellipsoid_Code */


long Get_Datum_Type (const long Index,
                     Define_Datum_Type  *Type)
/*
 *  The function Get_Datum_Type returns the type of the datum referenced by
 *  index.
 *
 *  Index   : The index of a given datum in the datum table.        (input)
 *  Type    : The type of datum referenced by index.                (output)
 */
{ /* Get_Datum_Type */
  long error_code = ENGINE_NO_ERROR;
  long temp_error;

  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  else
  {
    temp_error = Retrieve_Datum_Type (Index, (Datum_Type *)Type);
    if (temp_error == DATUM_INVALID_INDEX_ERROR)
      error_code |= ENGINE_INVALID_INDEX_ERROR;
    else if (temp_error != DATUM_NO_ERROR)
      error_code |= ENGINE_DATUM_ERROR;
  }
  return (error_code);
} /* Get_Datum_Type */


long Get_Datum_Seven_Parameters (const long Index, 
                                 double *Delta_X,                             
                                 double *Delta_Y,
                                 double *Delta_Z,
                                 double *Rx, 
                                 double *Ry, 
                                 double *Rz, 
                                 double *Scale_Factor)
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
{ /* Get_Datum_Seven_Parameters */
  long error_code = ENGINE_NO_ERROR;
  long temp_error;

  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  else
  {
    temp_error = Datum_Seven_Parameters (Index, Delta_X, Delta_Y, Delta_Z, Rx, Ry, Rz, Scale_Factor);
    if (temp_error == DATUM_INVALID_INDEX_ERROR)
      error_code |= ENGINE_INVALID_INDEX_ERROR;
    else if (temp_error != DATUM_NO_ERROR)
      error_code |= ENGINE_DATUM_ERROR;
  }
  return (error_code);
} /* Get_Datum_Seven_Parameters */


long Get_Datum_Three_Parameters (const long Index, 
                                 double *Delta_X,
                                 double *Delta_Y,
                                 double *Delta_Z)
/*
 *   The function Get_Datum_Three_Parameters returns the three parameters
 *   for the datum referenced by index.
 *
 *    Index      : The index of a given datum in the datum table.  (input)
 *    Delta_X    : X translation in meters                         (output)
 *    Delta_Y    : Y translation in meters                         (output)
 *    Delta_Z    : Z translation in meters                         (output)
 */
{ /* Get_Datum_Three_Parameters */
  long error_code = ENGINE_NO_ERROR;
  long temp_error;

  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  else
  {
    temp_error = Datum_Three_Parameters (Index, Delta_X, Delta_Y, Delta_Z);
    if (temp_error == DATUM_INVALID_INDEX_ERROR)
      error_code |= ENGINE_INVALID_INDEX_ERROR;
    else if (temp_error != DATUM_NO_ERROR)
      error_code |= ENGINE_DATUM_ERROR;
  }
  return (error_code);
} /* Get_Datum_Three_Parameters */


long Get_Datum_Errors (const long Index, 
                       double *Sigma_X,
                       double *Sigma_Y,
                       double *Sigma_Z)
/*
 *   The function Get_Datum_Errors returns the standard errors in X,Y, & Z 
 *   for the datum referenced by index.
 *
 *    Index      : The index of a given datum in the datum table   (input)
 *    Sigma_X    : Standard error in X in meters                   (output)
 *    Sigma_Y    : Standard error in Y in meters                   (output)
 *    Sigma_Z    : Standard error in Z in meters                   (output)
 */
{ /* Get_Datum_Errors */
  long error_code = ENGINE_NO_ERROR;
  long temp_error;

  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  else
  {
    temp_error = Datum_Errors (Index, Sigma_X, Sigma_Y, Sigma_Z);
    if (temp_error == DATUM_INVALID_INDEX_ERROR)
      error_code |= ENGINE_INVALID_INDEX_ERROR;
    else if (temp_error != DATUM_NO_ERROR)
      error_code |= ENGINE_DATUM_ERROR;
  }
  return (error_code);
} /* Get_Datum_Errors */


long Get_Datum_Valid_Rectangle (const long Index,
                                double *South_latitude,
                                double *North_latitude,
                                double *West_longitude,
                                double *East_longitude)
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
{ /* Get_Datum_Valid_Rectangle */
  long error_code = ENGINE_NO_ERROR;
  long temp_error;

  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  else
  {
    temp_error = Datum_Valid_Rectangle (Index, South_latitude, North_latitude, West_longitude, East_longitude);
    if (temp_error == DATUM_INVALID_INDEX_ERROR)
      error_code |= ENGINE_INVALID_INDEX_ERROR;
    else if (temp_error != DATUM_NO_ERROR)
      error_code |= ENGINE_DATUM_ERROR;
  }
  return (error_code);
} /* Get_Datum_Valid_Rectangle */


long Check_Datum_User_Defined ( const long Index,
                                long *result )
/*
 *    Index    : Index of a given datum in the datum table (input)
 *    result   : Indicates whether specified datum is user defined (1)
 *               or not (0)                                (output)
 *
 *  The function Check_Datum_User_Defined checks whether or not the specified datum is 
 *  user defined. It returns 1 if the datum is user defined, and returns
 *  0 otherwise.  
 */
{ /* Check_Datum_User_Defined */
  long error_code = ENGINE_NO_ERROR;
  long temp_error;

  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  else
  {
    temp_error = Datum_User_Defined (Index, result);
    if (temp_error == DATUM_INVALID_INDEX_ERROR)
      error_code |= ENGINE_INVALID_INDEX_ERROR;
    else if (temp_error != DATUM_NO_ERROR)
      error_code |= ENGINE_DATUM_ERROR;
  }
  return (error_code);
} /* Check_Datum_User_Defined */


long Check_Valid_Datum(const long Index,
                       double latitude,
                       double longitude,
                       long *result)
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
{ /* Check_Valid_Datum */
  long error_code = ENGINE_NO_ERROR;
  long temp_error;

  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  else
  {
    temp_error = Valid_Datum (Index, latitude, longitude, result);
    if (temp_error == DATUM_INVALID_INDEX_ERROR)
      error_code |= ENGINE_INVALID_INDEX_ERROR;
    else if (temp_error != DATUM_NO_ERROR)
      error_code |= ENGINE_DATUM_ERROR;
  }
  return (error_code);
} /* Check_Valid_Datum */


long Set_Datum
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const long                Index )
/*
 *  The function Set_Datum sets the datum for the specified state to the
 *  datum corresponding to the specified index.
 *  State      : Indicates whether the datum is to be used for interactive
 *               or file processing                                    (input)
 *  Direction  : Indicates whether the datum is to be used for input or
 *               output                                                (input)
 *  Index      : Identifies the index of the datum to be used          (input)
 */
{ /* Set_Datum */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!Valid_Datum_Index(Index))
    error_code |= ENGINE_INVALID_INDEX_ERROR;
  if (!error_code)
  {
    CS_State[State][Direction].datum_Index = Index;
  }
  return (error_code);
} /* Set_Datum */


long Get_Datum
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  long                      *Index )
/*
 *  The function Get_Datum returns the index of the current datum for the
 *  specified state.
 *  State      : Indicates whether the datum is to be used for interactive
 *               or file processing                                    (input)
 *  Direction  : Indicates whether the datum is to be used for input or
 *               output                                                (input)
 *  Index      : Identifies the index of the current datum             (input)
 */
{ /* Get_Datum */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    *Index = CS_State[State][Direction].datum_Index;
  }
  return (error_code);
} /* Get_Datum */


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
                    double East_longitude )
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
{ /* Define_Datum */
  long error_code = ENGINE_NO_ERROR;
  long temp_error;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  else
  {
    temp_error = Create_Datum (Code, Name, Ellipsoid_Code,
                               Delta_X, Delta_Y, Delta_Z, Sigma_X, Sigma_Y, Sigma_Z,
                               South_latitude, North_latitude, West_longitude, East_longitude);
    if (temp_error & DATUM_INVALID_CODE_ERROR)
      error_code |= ENGINE_INVALID_CODE_ERROR;
    if (temp_error & DATUM_SIGMA_ERROR)
      error_code |= ENGINE_DATUM_SIGMA_ERROR;
    if (temp_error & DATUM_DOMAIN_ERROR)
      error_code |= ENGINE_DATUM_DOMAIN_ERROR;
    if (temp_error & DATUM_LAT_ERROR)
      error_code |= ENGINE_LAT_ERROR;
    if (temp_error & DATUM_LON_ERROR)
      error_code |= ENGINE_LON_ERROR;
    if (temp_error & DATUM_3PARAM_OVERFLOW_ERROR)
      error_code |= ENGINE_DATUM_OVERFLOW;
    if (temp_error & DATUM_ELLIPSE_ERROR)
      error_code |= ENGINE_ELLIPSOID_ERROR;
    if (temp_error & DATUM_3PARAM_FILE_OPEN_ERROR)
      error_code |= ENGINE_DATUM_ERROR;
    if (temp_error == DATUM_NO_ERROR)
      Datum_Count (&Number_of_Datums);
  }
  return (error_code);
} /* Define_Datum */


long Remove_Datum (const char *Code)
/*
 *   Code           : 5-letter new datum code.                      (input)
 *
 * The function Remove_Datum deletes a local (3-parameter) datum with the 
 * specified code.  If the datum table has not been initialized or a new 
 * version of the 3-param.dat file cannot be created, an error code is returned,  
 * otherwise ENGINE_NO_ERROR is returned.  Note that the indexes of all datums 
 * in the datum table may be changed by this function.
 */
{ /* Remove_Datum */
  long error_code = ENGINE_NO_ERROR;
  long temp_error;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  else
  {
    temp_error = Delete_Datum (Code);
    if (temp_error & DATUM_3PARAM_FILE_OPEN_ERROR)
      error_code |= ENGINE_DATUM_ERROR;
    if (temp_error & DATUM_NOT_USERDEF_ERROR)
      error_code |= ENGINE_NOT_USERDEF_ERROR;
    if (temp_error == DATUM_NO_ERROR)
      Datum_Count (&Number_of_Datums);
  }
  return (error_code);
} /* Remove_Datum */


long Get_Ellipsoid_Count ( long *Count )
/*
 *   Count    : The number of ellipsoids in the ellipsoid table. (output)
 *
 * The function Get_Ellipsoid_Count returns the number of ellipsoids in the
 * ellipsoid table.  If the ellipsoid table has been initialized without error,
 * ENGINE_NO_ERROR is returned, otherwise ENGINE_NOT_INITIALIZED_ERROR
 * is returned.
 */
{ /* Get_Ellipsoid_Count */
  long error_code = ENGINE_NO_ERROR;
  long temp_error;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  else
  {
    temp_error = Ellipsoid_Count (Count);
    if (temp_error != ELLIPSE_NO_ERROR)
      error_code |= ENGINE_ELLIPSOID_ERROR;
  }
  return (error_code);
} /* Get_Ellipsoid_Count */


long Get_Ellipsoid_Index ( const char *Code,
                           long *Index )
/*
 *    Code     : 2-letter ellipsoid code.                      (input)
 *    Index    : Index of the ellipsoid in the ellipsoid table with the 
 *                  specified code                             (output)
 *
 *  The function Get_Ellipsoid_Index returns the index of the ellipsoid in 
 *  the ellipsoid table with the specified code.  If ellipsoid_Code is found, 
 *  ENGINE_NO_ERROR is returned, otherwise ENGINE_INVALID_CODE_ERROR is 
 *  returned.
 */
{ /* Get_Ellipsoid_Index */
  long error_code = ENGINE_NO_ERROR;
  long temp_error;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  else
  {
    temp_error = Ellipsoid_Index (Code, Index);
    if (temp_error == ELLIPSE_INVALID_CODE_ERROR)
      error_code |= ENGINE_INVALID_CODE_ERROR;
    else if (temp_error != ELLIPSE_NO_ERROR)
      error_code |= ENGINE_ELLIPSOID_ERROR;
  }
  return (error_code);
} /* Get_Ellipsoid_Index */


long Get_Ellipsoid_Name ( const long Index, 
                          char *Name )
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
{ /* Get_Ellipsoid_Name */
  long error_code = ENGINE_NO_ERROR;
  long temp_error;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  else
  {
    temp_error = Ellipsoid_Name (Index, Name);
    if (temp_error == ELLIPSE_INVALID_INDEX_ERROR)
      error_code |= ENGINE_INVALID_INDEX_ERROR;
    else if (temp_error != ELLIPSE_NO_ERROR)
      error_code |= ENGINE_ELLIPSOID_ERROR;
  }
  return (error_code);
} /* Get_Ellipsoid_Name */


long Get_Ellipsoid_Parameters ( const long Index,
                                double *a,
                                double *f )
/*
 *    Index    : Index of a given ellipsoid in the ellipsoid table (input)
 *    a        : Semi-major axis, in meters, of ellipsoid          (output)
 *    f        : Flattening of ellipsoid.                          (output)
 *
 *  The function Get_Ellipsoid_Parameters returns the semi-major axis 
 *  and flattening for the ellipsoid with the specified index.  
 */
{ /* Get_Ellipsoid_Parameters */
  long error_code = ENGINE_NO_ERROR;
  long temp_error;

  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  else
  {
    temp_error = Ellipsoid_Parameters (Index, a, f);
    if (temp_error == ELLIPSE_INVALID_INDEX_ERROR)
      error_code |= ENGINE_INVALID_INDEX_ERROR;
    else if (temp_error != ELLIPSE_NO_ERROR)
      error_code |= ENGINE_ELLIPSOID_ERROR;
  }
  return (error_code);
} /* Get_Ellipsoid_Parameters */


long Get_Ellipsoid_Code ( const long Index,
                          char *Code )
/*
 *    Index    : Index of a given ellipsoid in the ellipsoid table (input)
 *    Code     : 2-letter ellipsoid code.                          (output)
 *
 *  The Function Get_Ellipsoid_Code returns the 2-letter code for the 
 *  ellipsoid in the ellipsoid table with the specified index.  If index is 
 *  valid, ENGINE_NO_ERROR is returned, otherwise ENGINE_INVALID_INDEX_ERROR 
 *  is returned.
 */
{ /* Get_Ellipsoid_Code */
  long error_code = ENGINE_NO_ERROR;
  long temp_error;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  else
  {
    temp_error = Ellipsoid_Code (Index, Code);
    if (temp_error == ELLIPSE_INVALID_INDEX_ERROR)
      error_code |= ENGINE_INVALID_INDEX_ERROR;
    else if (temp_error != ELLIPSE_NO_ERROR)
      error_code |= ENGINE_ELLIPSOID_ERROR;
  }
  return (error_code);
} /* Get_Ellipsoid_Code */


long Get_Ellipsoid_Eccentricity2 ( const long Index,
                                   double *e2 )
/*
 *    Index    : Index of a given ellipsoid in the ellipsoid table (input)
 *    e2       : Square of eccentricity of ellipsoid               (output)
 *
 *  The function Get_Ellipsoid_Eccentricity2 returns the square of the 
 *  eccentricity for the ellipsoid with the specified index.  
 */
{ /* Get_Ellipsoid_Eccentricity2 */
  long error_code = ENGINE_NO_ERROR;
  long temp_error;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  else
  {
    temp_error = Ellipsoid_Eccentricity2 (Index, e2);
    if (temp_error == ELLIPSE_INVALID_INDEX_ERROR)
      error_code |= ENGINE_INVALID_INDEX_ERROR;
    else if (temp_error != ELLIPSE_NO_ERROR)
      error_code |= ENGINE_ELLIPSOID_ERROR;
  }
  return (error_code);
} /* Get_Ellipsoid_Eccentricity2 */


long Check_Ellipsoid_User_Defined ( const long Index,
                                    long *result )
/*
 *    Index    : Index of a given ellipsoid in the ellipsoid table (input)
 *    result   : Indicates whether specified ellipsoid is user defined (1)
 *               or not (0)                                        (output)
 *
 *  The function Check_Ellipsoid_User_Defined returns 1 if the ellipsoid is user 
 *  defined.  Otherwise, 0 is returned. 
 */
{ /* Check_Ellipsoid_User_Defined */
  long error_code = ENGINE_NO_ERROR;
  long temp_error;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  else
  {
    temp_error = Ellipsoid_User_Defined (Index, result);
    if (temp_error == ELLIPSE_INVALID_INDEX_ERROR)
      error_code |= ENGINE_INVALID_INDEX_ERROR;
    else if (temp_error != ELLIPSE_NO_ERROR)
      error_code |= ENGINE_ELLIPSOID_ERROR;
  }
  return (error_code);
} /* Check_Ellipsoid_User_Defined */


long Define_Ellipsoid (const char* Code,
                       const char* Name,
                       double a,
                       double f)
/*
 *   Code     : 2-letter ellipsoid code.                      (input)
 *   Name     : Name of the new ellipsoid                     (input)
 *   a        : Semi-major axis, in meters, of new ellipsoid  (input)
 *   f        : Flattening of new ellipsoid.                  (input)
 *
 * The function Define_Ellipsoid creates a new ellipsoid with the specified
 * code, name, and axes.  If the ellipsoid table has not been initialized,
 * the specified code is already in use, or a new version of the ellips.dat 
 * file cannot be created, an error code is returned, otherwise ENGINE_NO_ERROR 
 * is returned.  Note that the indexes of all ellipsoids in the ellipsoid
 * table may be changed by this function.
 */
{ /* Define_Ellipsoid */
  long error_code = ENGINE_NO_ERROR;
  long temp_error;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  else
  {
    temp_error = Create_Ellipsoid (Code, Name, a, f);
    if (temp_error & ELLIPSE_TABLE_OVERFLOW_ERROR)
      error_code |= ENGINE_ELLIPSOID_OVERFLOW;
    if (temp_error & ELLIPSE_INVALID_CODE_ERROR)
      error_code |= ENGINE_INVALID_CODE_ERROR;
    if (temp_error & ELLIPSE_A_ERROR)
      error_code |= ENGINE_A_ERROR;
    if (temp_error & ELLIPSE_INV_F_ERROR)
      error_code |= ENGINE_INV_F_ERROR;
    if (temp_error & ELLIPSE_FILE_OPEN_ERROR)
      error_code |= ENGINE_ELLIPSOID_ERROR;
  }
  return (error_code);
} /* Define_Ellipsoid */


long Remove_Ellipsoid (const char* Code)
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
{ /* Remove_Ellipsoid */
  long error_code = ENGINE_NO_ERROR;
  long temp_error;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  else
  {
    temp_error = Delete_Ellipsoid (Code);
    if (temp_error & ELLIPSE_IN_USE_ERROR)
      error_code |= ENGINE_ELLIPSE_IN_USE_ERROR;
    if (temp_error & ELLIPSE_NOT_USERDEF_ERROR)
      error_code |= ENGINE_NOT_USERDEF_ERROR;
    if (temp_error & ELLIPSE_FILE_OPEN_ERROR)
      error_code |= ENGINE_ELLIPSOID_ERROR;
  }
  return (error_code);
} /* Remove_Ellipsoid */


void Set_Precision(Precision Precis)
/*
 *  The function Set_Precision sets the output precision to the specified level.
 *  Precis     : Indicates the desired level of precision              (input)
 */
{ /* Set_Precision */
  Engine_Precision = Precis;
} /* Set_Precision */


void Get_Precision(Precision *Precis)
/*
 *  The function Get_Precision returns the current level of precision.
 *  Precis     : Indicates the current level of precision              (output)
 */
{ /* Get_Precision */
  *Precis = Engine_Precision;
} /* Get_Precision */


long Set_Geocentric_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Geocentric_Tuple    coordinates)
/*
 *  The function Set_Geocentric_Coordinates sets the Geocentric coordinates
 *  for the specified state..
 *  State            : Indicates whether the coordinates are to be set for
 *                     interactive or file processing                  (input)
 *  Direction        : Indicates whether the coordinates are to be set for
 *                     input or output                                 (input)
 *  coordinates      : Geocentric coordinate values to be set          (input)
 */
{ /* Set_Geocentric_Coordinates */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Geocentric)
      error_code |= ENGINE_INVALID_TYPE;
    else
      CS_State[State][Direction].coordinates.Geocentric = coordinates;
  }
  return ( error_code );
} /* Set_Geocentric_Coordinates */


long Get_Geocentric_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Geocentric_Tuple          *coordinates)
/*
 *  The function Get_Geocentric_Coordinates returns the Geocentric coordinates
 *  for the specified state..
 *  State            : Indicates whether the coordinates are to be returned for
 *                     interactive or file processing                  (input)
 *  Direction        : Indicates whether the coordinates are to be returned for
 *                     input or output                                 (input)
 *  coordinates      : Geocentric coordinate values to be returned     (output)
 */
{ /* Get_Geocentric_Coordinates */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Geocentric)
      error_code |= ENGINE_INVALID_TYPE;
    else
      *coordinates = CS_State[State][Direction].coordinates.Geocentric;
  }
  return ( error_code );
} /* Get_Geocentric_Coordinates */


long Set_Geodetic_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Geodetic_Parameters parameters)
/*
 *  The function Set_Geodetic_Params sets the Geodetic coordinate system  
 *  parameters for the specified state. 
 *  State        : Indicates whether the parameters are to be set for  
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the parameters are to be set for  
 *                 input or output                                     (input)
 *  parameters   : Geodetic parameters to be set                       (input)
 */
{ /* Set_Geodetic_Parameters */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Geodetic)
      error_code |= ENGINE_INVALID_TYPE;
    else
      CS_State[State][Direction].parameters.Geodetic = parameters;
  }
  return (error_code);
} /* END Set_Geodetic_Parameters */


long Get_Geodetic_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Geodetic_Parameters       *parameters)
/*
 *  The function Get_Geodetic_Params returns the Geodetic coordinate system
 *  parameters for the specified state. 
 *  State        : Indicates whether the parameters are to be returned for
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the parameters are to be returned for  
 *                 input or output                                     (input)
 *  parameters   : Geodetic parameters to be returned                  (output)
 */
{ /* Get_Geodetic_Parameters */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Geodetic)
      error_code |= ENGINE_INVALID_TYPE;
    else
      *parameters = CS_State[State][Direction].parameters.Geodetic;
  }
  return ( error_code );
} /* END Get_Geodetic_Parameters */


long Set_Geodetic_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Geodetic_Tuple      coordinates)
/*
 *  The function Set_Geodetic_Coordinates sets the Geodetic coordinates
 *  for the specified state..
 *  State          : Indicates whether the parameters are to be set for
 *                   interactive or file processing                    (input)
 *  Direction      : Indicates whether the parameters are to be set for
 *                   input or output                                   (input)
 *  coordinates    : Geodetic coordinate values to be set              (input)
 */
{ /* Set_Geodetic_Coordinates */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Geodetic)
      error_code |= ENGINE_INVALID_TYPE;
    else
      CS_State[State][Direction].coordinates.Geodetic = coordinates;
  }
  return ( error_code );
} /* Set_Geodetic_Coordinates */


long Get_Geodetic_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Geodetic_Tuple            *coordinates)
/*
 *  The function Get_Geodetic_Coordinates returns the Geodetic coordinates
 *  for the specified state..
 *  State          : Indicates whether the coordinates are to be returned for
 *                   interactive or file processing                    (input)
 *  Direction      : Indicates whether the coordinates are to be returned for
 *                   input or output                                   (input)
 *  coordinates    : Geodetic coordinate values to be returned         (output)
 */
{ /* Get_Geodetic_Coordinates */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Geodetic)
      error_code |= ENGINE_INVALID_TYPE;
    else
      *coordinates = CS_State[State][Direction].coordinates.Geodetic;
  }
  return ( error_code );
} /* Get_Geodetic_Coordinates */


long Set_GEOREF_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const GEOREF_Tuple        coordinates)
/*
 *  The function Set_GEOREF_Coordinates sets the GEOREF coordinates
 *  for the specified state..
 *  State        : Indicates whether the coordinates are to be set for
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the coordinates are to be set for
 *                 input or output                                     (input)
 *  coordinates  : GEOREF coordinate string value to be set            (input)
 */
{ /* Set_GEOREF_Coordinates */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != GEOREF)
      error_code |= ENGINE_INVALID_TYPE;
    else
      CS_State[State][Direction].coordinates.GEOREF = coordinates;
  }
  return ( error_code );
} /* Set_GEOREF_Coordinates */


long Get_GEOREF_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  GEOREF_Tuple              *coordinates)
/*
 *  The function Get_GEOREF_Coordinates returns the GEOREF coordinates
 *  for the specified state.
 *  State        : Indicates whether the coordinates are to be returned for
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the coordinates are to be returned for
 *                 input or output                                     (input)
 *  coordinates  : GEOREF coordinate string value to be returned       (output)
 */
{ /* Get_GEOREF_Coordinates */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != GEOREF)
      error_code |= ENGINE_INVALID_TYPE;
    else
      *coordinates = CS_State[State][Direction].coordinates.GEOREF;
  }
  return ( error_code );
} /* Get_GEOREF_Coordinates */


long Set_Albers_Equal_Area_Conic_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Albers_Equal_Area_Conic_Parameters parameters)
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
{ 
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Albers_Equal_Area_Conic)
      error_code |= ENGINE_INVALID_TYPE;
    else
      CS_State[State][Direction].parameters.Albers_Equal_Area_Conic = parameters;
  }
  return (error_code);
} /* Set_Albers_Equal_Area_Conic_Params */     


long Get_Albers_Equal_Area_Conic_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Albers_Equal_Area_Conic_Parameters *parameters)
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
{ /* Get_Albers_Equal_Area_Conic_Params */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Albers_Equal_Area_Conic)
      error_code |= ENGINE_INVALID_TYPE;
    else
      *parameters = CS_State[State][Direction].parameters.Albers_Equal_Area_Conic;
  }
  return ( error_code );
} /* Get_Albers_Equal_Area_Conic_Params */


long Set_Albers_Equal_Area_Conic_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Albers_Equal_Area_Conic_Tuple coordinates)
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
{ /* Set_Albers_Equal_Area_Conic_Coordinates */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Albers_Equal_Area_Conic)
      error_code |= ENGINE_INVALID_TYPE;
    else
      CS_State[State][Direction].coordinates.Albers_Equal_Area_Conic = coordinates;
  }
  return ( error_code );
} /* Set_Albers_Equal_Area_Conic_Coordinates */


long Get_Albers_Equal_Area_Conic_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Albers_Equal_Area_Conic_Tuple *coordinates)
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
{ /* Get_Albers_Equal_Area_Conic_Coordinates */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Albers_Equal_Area_Conic)
      error_code |= ENGINE_INVALID_TYPE;
    else
      *coordinates = CS_State[State][Direction].coordinates.Albers_Equal_Area_Conic;
  }
  return ( error_code );
} /* Get_Albers_Equal_Area_Conic_Coordinates */


long Set_Azimuthal_Equidistant_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Azimuthal_Equidistant_Parameters    parameters)
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
{ /* Set_Azimuthal_Equidistant_Params */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Azimuthal_Equidistant)
      error_code |= ENGINE_INVALID_TYPE;
    else
      CS_State[State][Direction].parameters.Azimuthal_Equidistant = parameters;
  }
  return (error_code);
} /* Set_Azimuthal_Equidistant_Params */


long Get_Azimuthal_Equidistant_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Azimuthal_Equidistant_Parameters          *parameters)
/*
 *  The function Get_Azimuthal_Equidistant_Params returns the Azimuthal_Equidistant 
 *  projection parameters for the specified state. 
 *  State        : Indicates whether the parameters are to be returned for
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the parameters are to be returned for  
 *                 input or output                                     (input)
 *  parameters   : Azimuthal Equidistant projection parameters to be returned
 *                                                                     (output)
 */
{ /* Get_Azimuthal_Equidistant_Params */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Azimuthal_Equidistant)
      error_code |= ENGINE_INVALID_TYPE;
    else
      *parameters = CS_State[State][Direction].parameters.Azimuthal_Equidistant;
  }
  return ( error_code );
} /* Get_Azimuthal_Equidistant_Params */


long Set_Azimuthal_Equidistant_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Azimuthal_Equidistant_Tuple         coordinates)
/*
 *  The function Set_Azimuthal_Equidistant_Coordinates sets the Azimuthal_Equidistant 
 *  projection coordinates for the specified state.
 *  State        : Indicates whether the coordinates are to be set for
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the coordinates are to be set for
 *                 input or output                                     (input)
 *  coordinates  : Azimuthal Equidistant projection coordinates to be set 
 *                                                                     (input)
 */
{ /* Set_Azimuthal_Equidistant_Coordinates */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Azimuthal_Equidistant)
      error_code |= ENGINE_INVALID_TYPE;
    else
      CS_State[State][Direction].coordinates.Azimuthal_Equidistant = coordinates;
  }
  return ( error_code );
} /* Set_Azimuthal_Equidistant_Coordinates */


long Get_Azimuthal_Equidistant_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Azimuthal_Equidistant_Tuple               *coordinates)
/*
 *  The function Get_Azimuthal_Equidistant_Coordinates returns the Azimuthal_Equidistant  
 *  projection coordinates for the specified state.
 *  State        : Indicates whether the coordinates are to be returned for
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the coordinates are to be returned for
 *                 input or output                                     (input)
 *  coordinates  : Azimuthal Equidistant projection coordinates to be returned 
 *                                                                     (output)
 */
{ /* Get_Azimuthal_Equidistant_Coordinates */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Azimuthal_Equidistant)
      error_code |= ENGINE_INVALID_TYPE;
    else
      *coordinates = CS_State[State][Direction].coordinates.Azimuthal_Equidistant;
  }
  return ( error_code );
} /* Get_Azimuthal_Equidistant_Coordinates */


long Set_BNG_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const BNG_Tuple           coordinates)
/*
 *  The function Set_BNG_Coordinates sets the British National Grid coordinate
 *  string for the specified state..
 *  State        : Indicates whether the coordinates are to be set for
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the coordinates are to be set for
 *                 input or output                                     (input)
 *  coordinates  : BNG coordinate string value to be set               (input)
 */
{ /* Set_BNG_Coordinates */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != BNG)
      error_code |= ENGINE_INVALID_TYPE;
    else
      CS_State[State][Direction].coordinates.BNG = coordinates;
  }
  return ( error_code );
} /* Set_BNG_Coordinates */


long Get_BNG_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  BNG_Tuple                 *coordinates)
/*
 *  The function Get_BNG_Coordinates returns the British National Grid coordinate
 *  string for the specified state.
 *  State        : Indicates whether the coordinates are to be returned for
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the coordinates are to be returned for
 *                 input or output                                     (input)
 *  coordinates  : BNG coordinate string value to be returned          (output)
 */
{ /* Get_BNG_Coordinates */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != BNG)
      error_code |= ENGINE_INVALID_TYPE;
    else
      *coordinates = CS_State[State][Direction].coordinates.BNG;
  }
  return ( error_code );
} /* Get_BNG_Coordinates */


long Set_Bonne_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Bonne_Parameters    parameters)
/*
 *  The function Set_Bonne_Params sets the Bonne projection parameters 
 *  for the specified state.
 *  State        : Indicates whether the parameters are to be set for
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the parameters are to be set for
 *                 input or output                                     (input)
 *  parameters   : Bonne projection parameters to be set               (input)
 */
{ /* Set_Bonne_Params */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Bonne)
      error_code |= ENGINE_INVALID_TYPE;
    else
      CS_State[State][Direction].parameters.Bonne = parameters;
  }
  return (error_code);
} /* Set_Bonne_Params */


long Get_Bonne_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Bonne_Parameters          *parameters)
/*
 *  The function Get_Bonne_Params returns the Bonne projection parameters 
 *  for the specified state. 
 *  State        : Indicates whether the parameters are to be returned for
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the parameters are to be returned for  
 *                 input or output                                     (input)
 *  parameters   : Bonne projection parameters to be returned          (output)
 */
{ /* Get_Bonne_Params */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Bonne)
      error_code |= ENGINE_INVALID_TYPE;
    else
      *parameters = CS_State[State][Direction].parameters.Bonne;
  }
  return ( error_code );
} /* Get_Bonne_Params */


long Set_Bonne_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Bonne_Tuple         coordinates)
/*
 *  The function Set_Bonne_Coordinates sets the Bonne projection 
 *  coordinates for the specified state.
 *  State        : Indicates whether the coordinates are to be set for
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the coordinates are to be set for
 *                 input or output                                     (input)
 *  coordinates  : Bonne projection coordinates to be set              (input)
 */
{ /* Set_Bonne_Coordinates */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Bonne)
      error_code |= ENGINE_INVALID_TYPE;
    else
      CS_State[State][Direction].coordinates.Bonne = coordinates;
  }
  return ( error_code );
} /* Set_Bonne_Coordinates */


long Get_Bonne_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Bonne_Tuple               *coordinates)
/*
 *  The function Get_Bonne_Coordinates returns the Bonne projection
 *  coordinates for the specified state. 
 *  State          : Indicates whether the coordinates are to be returned for  
 *                   interactive or file processing                    (input)
 *  Direction      : Indicates whether the coordinates are to be returned for  
 *                   input or output                                   (input)
 *  coordinates    : Bonne projection coordinates to be returned       (output)
 */
{ /* Get_Bonne_Coordinates */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Bonne)
      error_code |= ENGINE_INVALID_TYPE;
    else
      *coordinates = CS_State[State][Direction].coordinates.Bonne;
  }
  return ( error_code );
} /* Get_Bonne_Coordinates */


long Set_Cassini_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Cassini_Parameters  parameters)
/*
 *  The function Set_Cassini_Params sets the Cassini projection
 *  parameters for the specified state. 
 *  State        : Indicates whether the parameters are to be set for  
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the parameters are to be set for  
 *                 input or output                                     (input)
 *  parameters   : Cassini projection parameters to be set             (input)
 */
{ /* Set_Cassini_Params */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Cassini)
      error_code |= ENGINE_INVALID_TYPE;
    else
      CS_State[State][Direction].parameters.Cassini = parameters;
  }
  return (error_code);
} /* Set_Cassini_Params */


long Get_Cassini_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Cassini_Parameters        *parameters)
/*
 *  The function Get_Cassini_Params returns the Cassini projection  
 *  parameters for the specified state. 
 *  State        : Indicates whether the parameters are to be returned for
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the parameters are to be returned for  
 *                 input or output                                     (input)
 *  parameters   : Cassini projection parameters to be returned        (output)
 */
{ /* Get_Cassini_Params */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Cassini)
      error_code |= ENGINE_INVALID_TYPE;
    else
      *parameters = CS_State[State][Direction].parameters.Cassini;
  }
  return ( error_code );
} /* Get_Cassini_Params */


long Set_Cassini_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Cassini_Tuple       coordinates)
/*
 *  The function Set_Cassini_Coordinates sets the Cassini projection
 *  coordinates for the specified state. 
 *  State        : Indicates whether the coordinates are to be set for  
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the coordinates are to be set for  
 *                 input or output                                     (input)
 *  coordinates  : Cassini projection coordinates to be set            (input)
 */
{ /* Set_Cassini_Coordinates */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Cassini)
      error_code |= ENGINE_INVALID_TYPE;
    else
      CS_State[State][Direction].coordinates.Cassini = coordinates;
  }
  return ( error_code );
} /* Set_Cassini_Coordinates */


long Get_Cassini_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Cassini_Tuple             *coordinates)
/*
 *  The function Get_Cassini_Coordinates returns the Cassini projection
 *  coordinates for the specified state. 
 *  State          : Indicates whether the coordinates are to be returned for  
 *                   interactive or file processing                    (input)
 *  Direction      : Indicates whether the coordinates are to be returned for  
 *                   input or output                                   (input)
 *  coordinates    : Cassini projection coordinates to be returned     (output)
 */
{ /* Get_Cassini_Coordinates */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Cassini)
      error_code |= ENGINE_INVALID_TYPE;
    else
      *coordinates = CS_State[State][Direction].coordinates.Cassini;
  }
  return ( error_code );
} /* Get_Cassini_Coordinates */


long Set_Cylindrical_Equal_Area_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Cylindrical_Equal_Area_Parameters parameters)
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
{ /* Set_Cylindrical_Equal_Area_Params */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Cylindrical_Equal_Area)
      error_code |= ENGINE_INVALID_TYPE;
    else
      CS_State[State][Direction].parameters.Cylindrical_Equal_Area = parameters;
  }
  return (error_code);
} /* Set_Cylindrical_Equal_Area_Params */


long Get_Cylindrical_Equal_Area_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Cylindrical_Equal_Area_Parameters *parameters)
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
{ /* Get_Cylindrical_Equal_Area_Params */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Cylindrical_Equal_Area)
      error_code |= ENGINE_INVALID_TYPE;
    else
      *parameters = CS_State[State][Direction].parameters.Cylindrical_Equal_Area;
  }
  return ( error_code );
} /* Get_Cylindrical_Equal_Area_Params */


long Set_Cylindrical_Equal_Area_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Cylindrical_Equal_Area_Tuple coordinates)
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
{ /* Set_Cylindrical_Equal_Area_Coordinates */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Cylindrical_Equal_Area)
      error_code |= ENGINE_INVALID_TYPE;
    else
      CS_State[State][Direction].coordinates.Cylindrical_Equal_Area = coordinates;
  }
  return ( error_code );
} /* Set_Cylindrical_Equal_Area_Coordinates */


long Get_Cylindrical_Equal_Area_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Cylindrical_Equal_Area_Tuple *coordinates)
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
{ /* Get_Cylindrical_Equal_Area_Coordinates */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Cylindrical_Equal_Area)
      error_code |= ENGINE_INVALID_TYPE;
    else
      *coordinates = CS_State[State][Direction].coordinates.Cylindrical_Equal_Area;
  }
  return ( error_code );
} /* Get_Cylindrical_Equal_Area_Coordinates */


long Set_Eckert4_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Eckert4_Parameters  parameters)
/*
 *  The function Set_Eckert4_Params sets the Eckert IV projection 
 *  parameters for the specified state.
 *  State        : Indicates whether the parameters are to be set for
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the parameters are to be set for
 *                 input or output                                     (input)
 *  parameters   : Eckert IV projection parameters to be set           (input)
 */
{ /* Set_Eckert4_Params */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Eckert4)
      error_code |= ENGINE_INVALID_TYPE;
    else
      CS_State[State][Direction].parameters.Eckert4 = parameters;
  }
  return (error_code);
} /* Set_Eckert4_Params */


long Get_Eckert4_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Eckert4_Parameters        *parameters)
/*
 *  The function Get_Eckert4_Params returns the Eckert IV projection 
 *  parameters for the specified state.
 *  State        : Indicates whether the parameters are to be returned for
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the parameters are to be returned for
 *                 input or output                                     (input)
 *  parameters   : Eckert IV projection parameters to be returned      (output)
 */
{ /* Get_Eckert4_Params */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Eckert4)
      error_code |= ENGINE_INVALID_TYPE;
    else
      *parameters = CS_State[State][Direction].parameters.Eckert4;
  }
  return ( error_code );
} /* Get_Eckert4_Params */


long Set_Eckert4_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Eckert4_Tuple       coordinates)
/*
 *  The function Set_Eckert4_Coordinates sets the Eckert IV projection 
 *  coordinates for the specified state.
 *  State        : Indicates whether the coordinates are to be set for
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the coordinates are to be set for
 *                 input or output                                     (input)
 *  coordinates  : Eckert IV projection coordinates to be set          (input)
 */
{ /* Set_Eckert4_Coordinates */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Eckert4)
      error_code |= ENGINE_INVALID_TYPE;
    else
      CS_State[State][Direction].coordinates.Eckert4 = coordinates;
  }
  return ( error_code );
} /* Set_Eckert4_Coordinates */


long Get_Eckert4_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Eckert4_Tuple             *coordinates)
/*
 *  The function Get_Eckert4_Coordinates returns the Eckert IV projection 
 *  coordinates for the specified state.
 *  State          : Indicates whether the coordinates are to be returned for
 *                   interactive or file processing                    (input)
 *  Direction      : Indicates whether the coordinates are to be returned for
 *                   input or output                                   (input)
 *  coordinates    : Eckert IV projection coordinates to be returned   (output)
 */
{ /* Get_Eckert4_Coordinates */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Eckert4)
      error_code |= ENGINE_INVALID_TYPE;
    else
      *coordinates = CS_State[State][Direction].coordinates.Eckert4;
  }
  return ( error_code );
} /* Get_Eckert4_Coordinates */


long Set_Eckert6_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Eckert6_Parameters  parameters)
/*
 *  The function Set_Eckert6_Params sets the Eckert VI projection parameters for the specified state.
 *  State        : Indicates whether the parameters are to be set for
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the parameters are to be set for
 *                 input or output                                     (input)
 *  parameters   : Eckert VI projection parameters to be set           (input)
 */
{ /* Set_Eckert6_Params */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Eckert6)
      error_code |= ENGINE_INVALID_TYPE;
    else
      CS_State[State][Direction].parameters.Eckert6 = parameters;
  }
  return (error_code);
} /* Set_Eckert6_Params */


long Get_Eckert6_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Eckert6_Parameters        *parameters)
/*
 *  The function Get_Eckert6_Params returns the Eckert VI projection 
 *  parameters for the specified state.
 *  State        : Indicates whether the parameters are to be returned for
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the parameters are to be returned for
 *                 input or output                                     (input)
 *  parameters   : Eckert VI projection parameters to be returned      (output)
 */
{ /* Get_Eckert6_Params */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Eckert6)
      error_code |= ENGINE_INVALID_TYPE;
    else
      *parameters = CS_State[State][Direction].parameters.Eckert6;
  }
  return ( error_code );
} /* Get_Eckert6_Params */


long Set_Eckert6_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Eckert6_Tuple       coordinates)
/*
 *  The function Set_Eckert6_Coordinates sets the Eckert VI projection 
 *  coordinates for the specified state.
 *  State        : Indicates whether the coordinates are to be set for
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the coordinates are to be set for
 *                 input or output                                     (input)
 *  coordinates  : Eckert VI projection coordinates to be set          (input)
 */
{ /* Set_Eckert6_Coordinates */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Eckert6)
      error_code |= ENGINE_INVALID_TYPE;
    else
      CS_State[State][Direction].coordinates.Eckert6 = coordinates;
  }
  return ( error_code );
} /* Set_Eckert6_Coordinates */


long Get_Eckert6_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Eckert6_Tuple             *coordinates)
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
{ /* Get_Eckert6_Coordinates */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Eckert6)
      error_code |= ENGINE_INVALID_TYPE;
    else
      *coordinates = CS_State[State][Direction].coordinates.Eckert6;
  }
  return ( error_code );
} /* Get_Eckert6_Coordinates */


long Set_Equidistant_Cylindrical_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Equidistant_Cylindrical_Parameters parameters)
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
{ /* Set_Equidistant_Cylindrical_Params */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Equidistant_Cylindrical)
      error_code |= ENGINE_INVALID_TYPE;
    else
      CS_State[State][Direction].parameters.Equidistant_Cylindrical = parameters;
  }
  return (error_code);
} /* Set_Equidistant_Cylindrical_Params */


long Get_Equidistant_Cylindrical_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Equidistant_Cylindrical_Parameters *parameters)
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
{ /* Get_Equidistant_Cylindrical_Params */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Equidistant_Cylindrical)
      error_code |= ENGINE_INVALID_TYPE;
    else
      *parameters = CS_State[State][Direction].parameters.Equidistant_Cylindrical;
  }
  return ( error_code );
} /* Get_Equidistant_Cylindrical_Params */


long Set_Equidistant_Cylindrical_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Equidistant_Cylindrical_Tuple coordinates)
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
{ /* Set_Equidistant_Cylindrical_Coordinates */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Equidistant_Cylindrical)
      error_code |= ENGINE_INVALID_TYPE;
    else
      CS_State[State][Direction].coordinates.Equidistant_Cylindrical = coordinates;
  }
  return ( error_code );
} /* Set_Equidistant_Cylindrical_Coordinates */


long Get_Equidistant_Cylindrical_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Equidistant_Cylindrical_Tuple *coordinates)
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
{ /* Get_Equidistant_Cylindrical_Coordinates */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Equidistant_Cylindrical)
      error_code |= ENGINE_INVALID_TYPE;
    else
      *coordinates = CS_State[State][Direction].coordinates.Equidistant_Cylindrical;
  }
  return ( error_code );
} /* Get_Equidistant_Cylindrical_Coordinates */


long Set_Gnomonic_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Gnomonic_Parameters    parameters)
/*
 *  The function Set_Gnomonic_Params sets the Gnomonic
 *  projection parameters for the specified state.
 *  State        : Indicates whether the parameters are to be set for
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the parameters are to be set for
 *                 input or output                                     (input)
 *  parameters   : Gnomonic projection parameters to be set            (input)
 */
{ /* Set_Gnomonic_Params */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Gnomonic)
      error_code |= ENGINE_INVALID_TYPE;
    else
      CS_State[State][Direction].parameters.Gnomonic = parameters;
  }
  return (error_code);
} /* Set_Gnomonic_Params */


long Get_Gnomonic_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Gnomonic_Parameters          *parameters)
/*
 *  The function Get_Gnomonic_Params returns the Gnomonic 
 *  projection parameters for the specified state. 
 *  State        : Indicates whether the parameters are to be returned for
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the parameters are to be returned for  
 *                 input or output                                     (input)
 *  parameters   : Gnomonic projection parameters to be returned       (output)
 */
{ /* Get_Gnomonic_Params */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Gnomonic)
      error_code |= ENGINE_INVALID_TYPE;
    else
      *parameters = CS_State[State][Direction].parameters.Gnomonic;
  }
  return ( error_code );
} /* Get_Gnomonic_Params */


long Set_Gnomonic_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Gnomonic_Tuple         coordinates)
/*
 *  The function Set_Gnomonic_Coordinates sets the Gnomonic 
 *  projection coordinates for the specified state.
 *  State        : Indicates whether the coordinates are to be set for
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the coordinates are to be set for
 *                 input or output                                     (input)
 *  coordinates  : Gnomonic projection coordinates to be set           (input)
 */
{ /* Set_Gnomonic_Coordinates */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Gnomonic)
      error_code |= ENGINE_INVALID_TYPE;
    else
      CS_State[State][Direction].coordinates.Gnomonic = coordinates;
  }
  return ( error_code );
} /* Set_Gnomonic_Coordinates */


long Get_Gnomonic_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Gnomonic_Tuple               *coordinates)
/*
 *  The function Set_Gnomonic_Coordinates sets the Gnomonic  
 *  projection coordinates for the specified state.
 *  State        : Indicates whether the coordinates are to be set for
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the coordinates are to be set for
 *                 input or output                                     (input)
 *  coordinates  : Gnomonic projection coordinates to be set           (input)
 */
{ /* Get_Gnomonic_Coordinates */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Gnomonic)
      error_code |= ENGINE_INVALID_TYPE;
    else
      *coordinates = CS_State[State][Direction].coordinates.Gnomonic;
  }
  return ( error_code );
} /* Get_Gnomonic_Coordinates */


long Set_Lambert_Conformal_Conic_1_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Lambert_Conformal_Conic_1_Parameters parameters)
/*
 *  The function Set_Lambert_Conformal_Conic_1_Params sets the Lambert 
 *  Conformal Conic (1 parallel) projection parameters for the specified state. 
 *  State        : Indicates whether the parameters are to be set for  
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the parameters are to be set for  
 *                 input or output                                     (input)
 *  parameters   : Lambert Conformal Conic (1 parallel projection parameters to be set
 *                                                                     (input)
 */
{ /* Set_Lambert_Conformal_Conic_1_Params */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Lambert_Conformal_Conic_1)
      error_code |= ENGINE_INVALID_TYPE;
    else
      CS_State[State][Direction].parameters.Lambert_Conformal_Conic_1 = parameters;
  }
  return (error_code);
} /* Set_Lambert_Conformal_Conic_1_Params */


long Get_Lambert_Conformal_Conic_1_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Lambert_Conformal_Conic_1_Parameters *parameters)
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
{ /* Get_Lambert_Conformal_Conic_1_Params */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Lambert_Conformal_Conic_1)
      error_code |= ENGINE_INVALID_TYPE;
    else
      *parameters = CS_State[State][Direction].parameters.Lambert_Conformal_Conic_1;
  }
  return ( error_code );
} /* Get_Lambert_Conformal_Conic_1_Params */


long Set_Lambert_Conformal_Conic_1_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Lambert_Conformal_Conic_1_Tuple coordinates)
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
{ /* Set_Lambert_Conformal_Conic_1_Coordinates */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Lambert_Conformal_Conic_1)
      error_code |= ENGINE_INVALID_TYPE;
    else
      CS_State[State][Direction].coordinates.Lambert_Conformal_Conic_1 = coordinates;
  }
  return ( error_code );
} /* Set_Lambert_Conformal_Conic_1_Coordinates */


long Get_Lambert_Conformal_Conic_1_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Lambert_Conformal_Conic_1_Tuple *coordinates)
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
{ /* Get_Lambert_Conformal_Conic_1_Coordinates */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Lambert_Conformal_Conic_1)
      error_code |= ENGINE_INVALID_TYPE;
    else
      *coordinates = CS_State[State][Direction].coordinates.Lambert_Conformal_Conic_1;
  }
  return ( error_code );
} /* Get_Lambert_Conformal_Conic_1_Coordinates */


long Set_Lambert_Conformal_Conic_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Lambert_Conformal_Conic_Parameters parameters)
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
{ /* Set_Lambert_Conformal_Conic_Params */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Lambert_Conformal_Conic)
      error_code |= ENGINE_INVALID_TYPE;
    else
      CS_State[State][Direction].parameters.Lambert_Conformal_Conic = parameters;
  }
  return (error_code);
} /* Set_Lambert_Conformal_Conic_Params */


long Get_Lambert_Conformal_Conic_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Lambert_Conformal_Conic_Parameters *parameters)
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
{ /* Get_Lambert_Conformal_Conic_Params */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Lambert_Conformal_Conic)
      error_code |= ENGINE_INVALID_TYPE;
    else
      *parameters = CS_State[State][Direction].parameters.Lambert_Conformal_Conic;
  }
  return ( error_code );
} /* Get_Lambert_Conformal_Conic_Params */


long Set_Lambert_Conformal_Conic_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Lambert_Conformal_Conic_Tuple coordinates)
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
{ /* Set_Lambert_Conformal_Conic_Coordinates */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Lambert_Conformal_Conic)
      error_code |= ENGINE_INVALID_TYPE;
    else
      CS_State[State][Direction].coordinates.Lambert_Conformal_Conic = coordinates;
  }
  return ( error_code );
} /* Set_Lambert_Conformal_Conic_Coordinates */


long Get_Lambert_Conformal_Conic_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Lambert_Conformal_Conic_Tuple *coordinates)
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
{ /* Get_Lambert_Conformal_Conic_Coordinates */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Lambert_Conformal_Conic)
      error_code |= ENGINE_INVALID_TYPE;
    else
      *coordinates = CS_State[State][Direction].coordinates.Lambert_Conformal_Conic;
  }
  return ( error_code );
} /* Get_Lambert_Conformal_Conic_Coordinates */


long Set_Local_Cartesian_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Local_Cartesian_Parameters parameters)
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
{ /* Set_Local_Cartesian_Params */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Local_Cartesian)
      error_code |= ENGINE_INVALID_TYPE;
    else
      CS_State[State][Direction].parameters.Local_Cartesian = parameters;
  }
  return (error_code);
} /* Set_Local_Cartesian_Params */


long Get_Local_Cartesian_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Local_Cartesian_Parameters *parameters)
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
{ /* Get_Local_Cartesian_Params */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Local_Cartesian)
      error_code |= ENGINE_INVALID_TYPE;
    else
      *parameters = CS_State[State][Direction].parameters.Local_Cartesian;
  }
  return ( error_code );
} /* Get_Local_Cartesian_Params */


long Set_Local_Cartesian_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Local_Cartesian_Tuple coordinates)
/*
 *  The function Set_Local_Cartesian_Coordinates sets the Local Cartesian 
 *  coordinates for the specified state.
 *  State        : Indicates whether the coordinates are to be set for
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the coordinates are to be set for
 *                 input or output                                     (input)
 *  coordinates  : Local Cartesian coordinates to be set               (input)
 */
{ /* Set_Local_Cartesian_Coordinates */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Local_Cartesian)
      error_code |= ENGINE_INVALID_TYPE;
    else
      CS_State[State][Direction].coordinates.Local_Cartesian = coordinates;
  }
  return ( error_code );
} /* Set_Local_Cartesian_Coordinates */


long Get_Local_Cartesian_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Local_Cartesian_Tuple     *coordinates)
/*
 *  The function Get_Local_Cartesian_Coordinates returns the Local Cartesian projection 
 *  coordinates for the specified state.
 *  State          : Indicates whether the coordinates are to be returned for
 *                   interactive or file processing                    (input)
 *  Direction      : Indicates whether the coordinates are to be returned for
 *                   input or output                                   (input)
 *  coordinates    : Local Cartesian coordinates to be returned        (output)
 */
{ /* Get_Local_Cartesian_Coordinates */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Local_Cartesian)
      error_code |= ENGINE_INVALID_TYPE;
    else
      *coordinates = CS_State[State][Direction].coordinates.Local_Cartesian;
  }
  return ( error_code );
} /* Get_Local_Cartesian_Coordinates */


long Set_Mercator_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Mercator_Parameters parameters)
/*
 *  The function Set_Mercator_Params sets the Mercator projection  
 *  parameters for the specified state. 
 *  State        : Indicates whether the parameters are to be set for  
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the parameters are to be set for  
 *                 input or output                                     (input)
 *  parameters   : Mercator projection parameters to be set            (input)
 */
{ /* Set_Mercator_Params */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Mercator)
      error_code |= ENGINE_INVALID_TYPE;
    else
      CS_State[State][Direction].parameters.Mercator = parameters;
  }
  return (error_code);
} /* Set_Mercator_Params */


long Get_Mercator_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Mercator_Parameters       *parameters)
/*
 *  The function Get_Mercator_Params returns the Mercator projection
 *  parameters for the specified state. 
 *  State        : Indicates whether the parameters are to be returned for
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the parameters are to be returned for  
 *                 input or output                                     (input)
 *  parameters   : Mercator projection parameters to be returned       (output)
 */
{ /* Get_Mercator_Params */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Mercator)
      error_code |= ENGINE_INVALID_TYPE;
    else
      *parameters = CS_State[State][Direction].parameters.Mercator;
  }
  return ( error_code );
} /* Get_Mercator_Params */


long Set_Mercator_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Mercator_Tuple      coordinates)
/*
 *  The function Set_Mercator_Coordinates sets the Mercator projection 
 *  coordinates for the specified state. 
 *  State        : Indicates whether the coordinates are to be set for  
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the coordinates are to be set for  
 *                 input or output                                     (input)
 *  coordinates  : Mercator projection coordinates to be set           (input)
 */
{ /* Set_Mercator_Coordinates */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Mercator)
      error_code |= ENGINE_INVALID_TYPE;
    else
      CS_State[State][Direction].coordinates.Mercator = coordinates;
  }
  return ( error_code );
} /* Set_Mercator_Coordinates */


long Get_Mercator_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Mercator_Tuple            *coordinates)
/*
 *  The function Get_Mercator_Coordinates returns the Mercator projection
 *  coordinates for the specified state. 
 *  State          : Indicates whether the coordinates are to be returned for  
 *                   interactive or file processing                    (input)
 *  Direction      : Indicates whether the coordinates are to be returned for  
 *                   input or output                                   (input)
 *  coordinates    : Mercator projection coordinates to be returned    (output)
 */
{ /* Get_Mercator_Coordinates */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Mercator)
      error_code |= ENGINE_INVALID_TYPE;
    else
      *coordinates = CS_State[State][Direction].coordinates.Mercator;
  }
  return ( error_code );
} /* Get_Mercator_Coordinates */


long Set_MGRS_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const MGRS_Tuple          coordinates)
/*
 *  The function Set_MGRS_Coordinates sets the MGRS projection coordinates
 *  for the specified state. 
 *  State        : Indicates whether the coordinates are to be set for  
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the coordinates are to be set for  
 *                 input or output                                     (input)
 *  coordinates  : MGRS projection coordinates to be set               (input)
 */
{ /* Set_MGRS_Coordinates */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != MGRS)
      error_code |= ENGINE_INVALID_TYPE;
    else
      CS_State[State][Direction].coordinates.MGRS = coordinates;
  }
  return ( error_code );
} /* Set_MGRS_Coordinates */


long Get_MGRS_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  MGRS_Tuple                *coordinates)
/*
 *  The function Get_MGRS_Coordinates returns the MGRS projection coordinates 
 *  for the specified state. 
 *  State          : Indicates whether the coordinates are to be returned for  
 *                   interactive or file processing                    (input)
 *  Direction      : Indicates whether the coordinates are to be returned for  
 *                   input or output                                   (input)
 *  coordinates    : MGRS projection coordinates to be returned        (output)
 */
{ /* Get_MGRS_Coordinates */
  long error_code = ENGINE_NO_ERROR;

  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != MGRS)
      error_code |= ENGINE_INVALID_TYPE;
    else
      *coordinates = CS_State[State][Direction].coordinates.MGRS;
  }
  return ( error_code );
} /* Get_MGRS_Coordinates */


long Set_Miller_Cylindrical_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Miller_Cylindrical_Parameters parameters)
/*
 *  The function Set_Miller_Cylindrical_Params sets the Miller Cylindrical   
 *  projection parameters for the specified state. 
 *  State        : Indicates whether the parameters are to be set for  
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the parameters are to be set for  
 *                 input or output                                     (input)
 *  parameters   : Miller Cylindrical projection parameters to be set  (input)
 */
{ /* Set_Miller_Cylindrical_Params */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Miller_Cylindrical)
      error_code |= ENGINE_INVALID_TYPE;
    else
      CS_State[State][Direction].parameters.Miller_Cylindrical = parameters;
  }
  return (error_code);
} /* Set_Miller_Cylindrical_Params */


long Get_Miller_Cylindrical_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Miller_Cylindrical_Parameters *parameters)
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
{ /* Get_Miller_Cylindrical_Params */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Miller_Cylindrical)
      error_code |= ENGINE_INVALID_TYPE;
    else
      *parameters = CS_State[State][Direction].parameters.Miller_Cylindrical;
  }
  return ( error_code );
} /* Get_Miller_Cylindrical_Params */


long Set_Miller_Cylindrical_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Miller_Cylindrical_Tuple coordinates)
/*
 *  The function Set_Miller_Cylindrical_Coordinates sets the Miller Cylindrical  
 *  projection coordinates for the specified state. 
 *  State        : Indicates whether the coordinates are to be set for  
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the coordinates are to be set for  
 *                 input or output                                     (input)
 *  coordinates  : Miller Cylindrical projection coordinates to be set (input)
 */
{ /* Set_Miller_Cylindrical_Coordinates */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Miller_Cylindrical)
      error_code |= ENGINE_INVALID_TYPE;
    else
      CS_State[State][Direction].coordinates.Miller_Cylindrical = coordinates;
  }
  return ( error_code );
} /* Set_Miller_Cylindrical_Coordinates */


long Get_Miller_Cylindrical_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Miller_Cylindrical_Tuple  *coordinates)
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
{ /* Get_Miller_Cylindrical_Coordinates */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Miller_Cylindrical)
      error_code |= ENGINE_INVALID_TYPE;
    else
      *coordinates = CS_State[State][Direction].coordinates.Miller_Cylindrical;
  }
  return ( error_code );
} /* Get_Miller_Cylindrical_Coordinates */


long Set_Mollweide_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Mollweide_Parameters parameters)
/*
 *  The function Set_Mollweide_Params sets the Mollweide
 *  projection parameters for the specified state. 
 *  State        : Indicates whether the parameters are to be set for  
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the parameters are to be set for  
 *                 input or output                                     (input)
 *  parameters   : Mollweide projection parameters to be set           (input)
 */
{ /* Set_Mollweide_Params */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Mollweide)
      error_code |= ENGINE_INVALID_TYPE;
    else
      CS_State[State][Direction].parameters.Mollweide = parameters;
  }
  return (error_code);
} /* Set_Mollweide_Params */


long Get_Mollweide_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Mollweide_Parameters      *parameters)
/*
 *  The function Get_Mollweide_Params returns the Mollweide projection  
 *  parameters for the specified state. 
 *  State        : Indicates whether the parameters are to be returned for
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the parameters are to be returned for  
 *                 input or output                                     (input)
 *  parameters    : Mollweide projection parameters to be returned     (output)
 */
{ /* Get_Mollweide_Params */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Mollweide)
      error_code |= ENGINE_INVALID_TYPE;
    else
      *parameters = CS_State[State][Direction].parameters.Mollweide;
  }
  return ( error_code );
} /* Get_Mollweide_Params */


long Set_Mollweide_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Mollweide_Tuple     coordinates)
/*
 *  The function Set_Mollweide_Coordinates sets the Mollweide projection 
 *  coordinates for the specified state. 
 *  State        : Indicates whether the coordinates are to be set for  
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the coordinates are to be set for  
 *                 input or output                                     (input)
 *  coordinates  : Mollweide projection coordinates to be set          (input)
 */
{ /* Set_Mollweide_Coordinates */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Mollweide)
      error_code |= ENGINE_INVALID_TYPE;
    else
      CS_State[State][Direction].coordinates.Mollweide = coordinates;
  }
  return ( error_code );
} /* Set_Mollweide_Coordinates */


long Get_Mollweide_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Mollweide_Tuple           *coordinates)
/*
 *  The function Get_Mollweide_Coordinates returns the Mollweide 
 *  projection coordinates for the specified state. 
 *  State          : Indicates whether the coordinates are to be returned for  
 *                   interactive or file processing                    (input)
 *  Direction      : Indicates whether the coordinates are to be returned for  
 *                   input or output                                   (input)
 *  coordinates    : Mollweide projection coordinates to be returned   (output)
 */
{ /* Get_Mollweide_Coordinates */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Mollweide)
      error_code |= ENGINE_INVALID_TYPE;
    else
      *coordinates = CS_State[State][Direction].coordinates.Mollweide;
  }
  return ( error_code );
} /* Get_Mollweide_Coordinates */


long Set_Neys_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Neys_Parameters parameters)
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
{ /* Set_Neys_Params */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Neys)
      error_code |= ENGINE_INVALID_TYPE;
    else
      CS_State[State][Direction].parameters.Neys = parameters;
  }
  return (error_code);
} /* Set_Neys_Params */


long Get_Neys_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Neys_Parameters *parameters)
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
{ /* Get_Neys_Params */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Neys)
      error_code |= ENGINE_INVALID_TYPE;
    else
      *parameters = CS_State[State][Direction].parameters.Neys;
  }
  return ( error_code );
} /* Get_Neys_Params */


long Set_Neys_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Neys_Tuple coordinates)
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
{ /* Set_Neys_Coordinates */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Neys)
      error_code |= ENGINE_INVALID_TYPE;
    else
      CS_State[State][Direction].coordinates.Neys = coordinates;
  }
  return ( error_code );
} /* Set_Neys_Coordinates */


long Get_Neys_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Neys_Tuple *coordinates)
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
{ /* Get_Neys_Coordinates */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Neys)
      error_code |= ENGINE_INVALID_TYPE;
    else
      *coordinates = CS_State[State][Direction].coordinates.Neys;
  }
  return ( error_code );
} /* Get_Neys_Coordinates */


long Set_NZMG_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const NZMG_Tuple coordinates)
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
{ /* Set_NZMG_Coordinates */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != NZMG)
      error_code |= ENGINE_INVALID_TYPE;
    else
      CS_State[State][Direction].coordinates.NZMG = coordinates;
  }
  return ( error_code );
} /* Set_NZMG_Coordinates */


long Get_NZMG_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  NZMG_Tuple *coordinates)
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
{ /* Get_NZMG_Coordinates */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != NZMG)
      error_code |= ENGINE_INVALID_TYPE;
    else
      *coordinates = CS_State[State][Direction].coordinates.NZMG;
  }
  return ( error_code );
}/* Get_NZMG_Coordinates */


long Set_Oblique_Mercator_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Oblique_Mercator_Parameters parameters)
/*
 *  The function Set_Oblique_Mercator_Params sets the Oblique Mercator projection  
 *  parameters for the specified state. 
 *  State        : Indicates whether the parameters are to be set for  
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the parameters are to be set for  
 *                 input or output                                     (input)
 *  parameters   : Oblique Mercator projection parameters to be set    (input)
 */
{ /* Set_Oblique_Mercator_Params */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Oblique_Mercator)
      error_code |= ENGINE_INVALID_TYPE;
    else
      CS_State[State][Direction].parameters.Oblique_Mercator = parameters;
  }
  return (error_code);
} /* Set_Oblique_Mercator_Params */


long Get_Oblique_Mercator_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Oblique_Mercator_Parameters      *parameters)
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
{ /* Get_Oblique_Mercator_Params */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Oblique_Mercator)
      error_code |= ENGINE_INVALID_TYPE;
    else
      *parameters = CS_State[State][Direction].parameters.Oblique_Mercator;
  }
  return ( error_code );
} /* Get_Oblique_Mercator_Params */


long Set_Oblique_Mercator_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Oblique_Mercator_Tuple     coordinates)
/*
 *  The function Set_Mercator_Coordinates sets the Oblique Mercator projection 
 *  coordinates for the specified state. 
 *  State        : Indicates whether the coordinates are to be set for  
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the coordinates are to be set for  
 *                 input or output                                     (input)
 *  coordinates  : Oblique Mercator projection coordinates to be set   (input)
 */
{ /* Set_Oblique_Mercator_Coordinates */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Oblique_Mercator)
      error_code |= ENGINE_INVALID_TYPE;
    else
      CS_State[State][Direction].coordinates.Oblique_Mercator = coordinates;
  }
  return ( error_code );
} /* Set_Oblique_Mercator_Coordinates */


long Get_Oblique_Mercator_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Oblique_Mercator_Tuple           *coordinates)
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
{ /* Get_Oblique_Mercator_Coordinates */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Oblique_Mercator)
      error_code |= ENGINE_INVALID_TYPE;
    else
      *coordinates = CS_State[State][Direction].coordinates.Oblique_Mercator;
  }
  return ( error_code );
} /* Get_Oblique_Mercator_Coordinates */


long Set_Orthographic_Params
( const File_or_Interactive State,
  const Input_or_Output Direction,
  const Orthographic_Parameters parameters)
/*
 *  The function Set_Orthographic_Params sets the Orthographic projection 
 *  parameters for the specified state. 
 *  State        : Indicates whether the parameters are to be set for  
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the parameters are to be set for  
 *                 input or output                                     (input)
 *  parameters   : Orthographic projection parameters to be set        (input)
 */
{ /* Set_Orthographic_Params */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Orthographic)
      error_code |= ENGINE_INVALID_TYPE;
    else
      CS_State[State][Direction].parameters.Orthographic = parameters;
  }
  return (error_code);
} /* Set_Orthographic_Params */


long Get_Orthographic_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Orthographic_Parameters  *parameters)
/*
 *  The function Get_Orthographic_Params returns the Orthographic projection 
 *  parameters for the specified state. 
 *  State        : Indicates whether the parameters are to be returned for
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the parameters are to be returned for  
 *                 input or output                                     (input)
 *  parameters   : Orthographic projection parameters to be returned   (output)
 */
{  /* Get_Orthographic_Params */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Orthographic)
      error_code |= ENGINE_INVALID_TYPE;
    else
      *parameters = CS_State[State][Direction].parameters.Orthographic;
  }
  return ( error_code );
} /* Get_Orthographic_Params */


long Set_Orthographic_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Orthographic_Tuple  coordinates)
/*
 *  The function Set_Orthographic_Coordinates sets the Orthographic projection
 *  coordinates for the specified state. 
 *  State        : Indicates whether the coordinates are to be set for  
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the coordinates are to be set for  
 *                 input or output                                     (input)
 *  coordinates  : Orthographic projection coordinates to be set       (input)
 */
{   /* Set_Orthographic_Coordinates */
  long error_code = ENGINE_NO_ERROR;

  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Orthographic)
      error_code |= ENGINE_INVALID_TYPE;
    else
      CS_State[State][Direction].coordinates.Orthographic = coordinates;
  }
  return ( error_code );
} /* Set_Orthographic_Coordinates */


long Get_Orthographic_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Orthographic_Tuple        *coordinates)
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
{ /* Get_Orthographic_Coordinates */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Orthographic)
      error_code |= ENGINE_INVALID_TYPE;
    else
      *coordinates = CS_State[State][Direction].coordinates.Orthographic;
  }
  return ( error_code );
} /* Get_Orthographic_Coordinates */


long Set_Polar_Stereo_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Polar_Stereo_Parameters parameters)
/*
 *  The function Set_Polar_Stereo_Params sets the Polar Stereographic
 *  projection parameters for the specified state.
 *  State        : Indicates whether the parameters are to be set for
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the parameters are to be set for
 *                 input or output                                     (input)
 *  parameters   : Polar Stereographic projection parameters to be set (input)
 */
{ /* Set_Polar_Stereo_Params */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Polar_Stereo)
      error_code |= ENGINE_INVALID_TYPE;
    else
      CS_State[State][Direction].parameters.Polar_Stereo = parameters;
  }
  return (error_code);
} /* Set_Polar_Stereo_Params */


long Get_Polar_Stereo_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Polar_Stereo_Parameters   *parameters)
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
{ /* Get_Polar_Stereo_Params */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Polar_Stereo)
      error_code |= ENGINE_INVALID_TYPE;
    else
      *parameters = CS_State[State][Direction].parameters.Polar_Stereo;
  }
  return ( error_code );
} /* Get_Polar_Params */


long Set_Polar_Stereo_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Polar_Stereo_Tuple  coordinates)
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
{ /* Set_Polar_Stereo_Coordinates */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Polar_Stereo)
      error_code |= ENGINE_INVALID_TYPE;
    else
      CS_State[State][Direction].coordinates.Polar_Stereo = coordinates;
  }
  return ( error_code );
} /* Set_Polar_Stereo_Coordinates */


long Get_Polar_Stereo_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Polar_Stereo_Tuple        *coordinates)
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
{ /* Get_Polar_Stereo_Coordinates */
  long error_code = ENGINE_NO_ERROR;

  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Polar_Stereo)
      error_code |= ENGINE_INVALID_TYPE;
    else
      *coordinates = CS_State[State][Direction].coordinates.Polar_Stereo;
  }
  return ( error_code );
} /* Get_Polar_Stereo_Coordinates */


long Set_Polyconic_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Polyconic_Parameters parameters)
/*
 *  The function Set_Polyconic_Params sets the Polyconic projection  
 *  parameters for the specified state. 
 *  State        : Indicates whether the parameters are to be set for  
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the parameters are to be set for  
 *                 input or output                                     (input)
 *  parameters   : Polyconic projection parameters to be set           (input)
 */
{ /* Set_Polyconic_Params */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Polyconic)
      error_code |= ENGINE_INVALID_TYPE;
    else
      CS_State[State][Direction].parameters.Polyconic = parameters;
  }
  return (error_code);
} /* Set_Polyconic_Params */


long Get_Polyconic_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Polyconic_Parameters      *parameters)
/*
 *  The function Get_Polyconic_Params returns the Polyconic projection  
 *  parameters for the specified state. 
 *  State        : Indicates whether the parameters are to be returned for
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the parameters are to be returned for  
 *                 input or output                                     (input)
 *  parameters   : Polyconic projection parameters to be returned      (output)
 */
{ /* Get_Polyconic_Params */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Polyconic)
      error_code |= ENGINE_INVALID_TYPE;
    else
      *parameters = CS_State[State][Direction].parameters.Polyconic;
  }
  return ( error_code );
} /* Get_Polyconic_Params */


long Set_Polyconic_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Polyconic_Tuple     coordinates)
/*
 *  The function Set_Polyconic_Coordinates sets the Polyconic projection
 *  coordinates for the specified state. 
 *  State        : Indicates whether the coordinates are to be set for  
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the coordinates are to be set for  
 *                 input or output                                     (input)
 *  coordinates  : Polyconic projection coordinates to be set          (input)
 */
{ /* Set_Polyconic_Coordinates */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Polyconic)
      error_code |= ENGINE_INVALID_TYPE;
    else
      CS_State[State][Direction].coordinates.Polyconic = coordinates;
  }
  return ( error_code );
} /* Set_Polyconic_Coordinates */


long Get_Polyconic_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Polyconic_Tuple           *coordinates)
/*
 *  The function Get_Polyconic_Coordinates returns the Polyconic projection
 *  coordinates for the specified state. 
 *  State          : Indicates whether the coordinates are to be returned for  
 *                   interactive or file processing                    (input)
 *  Direction      : Indicates whether the coordinates are to be returned for  
 *                   input or output                                   (input)
 *  coordinates    : Polyconic projection coordinates to be returned   (output)
 */
{ /* Get_Polyconic_Coordinates */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Polyconic)
      error_code |= ENGINE_INVALID_TYPE;
    else
      *coordinates = CS_State[State][Direction].coordinates.Polyconic;
  }
  return ( error_code );
} /* Get_Polyconic_Coordinates */


long Set_Sinusoidal_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Sinusoidal_Parameters parameters)
/*
 *  The function Set_Sinusoidal_Params sets the Sinusoidal projection  
 *  parameters for the specified state. 
 *  State        : Indicates whether the parameters are to be set for  
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the parameters are to be set for  
 *                 input or output                                     (input)
 *  parameters   : Sinusoidal projection parameters to be set          (input)
 */
{ /* Set_Sinusoidal_Params */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Sinusoidal)
      error_code |= ENGINE_INVALID_TYPE;
    else
      CS_State[State][Direction].parameters.Sinusoidal = parameters;
  }
  return (error_code);
} /* Set_Sinusoidal_Params */


long Get_Sinusoidal_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Sinusoidal_Parameters     *parameters)
/*
 *  The function Get_Sinusoidal_Params returns the Sinusoidal projection  
 *  parameters for the specified state. 
 *  State        : Indicates whether the parameters are to be returned for
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the parameters are to be returned for  
 *                 input or output                                     (input)
 *  parameters   : Sinusoidal projection parameters to be returned     (output)
 */
{ /* Get_Sinusoidal_Params */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Sinusoidal)
      error_code |= ENGINE_INVALID_TYPE;
    else
      *parameters = CS_State[State][Direction].parameters.Sinusoidal;
  }
  return ( error_code );
} /* Get_Sinusoidal_Params */


long Set_Sinusoidal_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Sinusoidal_Tuple coordinates)
/*
 *  The function Set_Sinusoidal_Coordinates sets the Sinusoidal projection
 *  coordinates for the specified state. 
 *  State        : Indicates whether the coordinates are to be set for  
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the coordinates are to be set for  
 *                 input or output                                     (input)
 *  coordinates  : Sinusoidal projection coordinates to be set         (input)
 */
{ /* Set_Sinusoidal_Coordinates */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Sinusoidal)
      error_code |= ENGINE_INVALID_TYPE;
    else
      CS_State[State][Direction].coordinates.Sinusoidal = coordinates;
  }
  return ( error_code );
} /* Set_Sinusoidal_Coordinates */


long Get_Sinusoidal_Coordinates
( const File_or_Interactive State,
  const Input_or_Output Direction,
  Sinusoidal_Tuple *coordinates)
/*
 *  The function Get_Sinusoidal_Coordinates returns the Sinusoidal projection
 *  coordinates for the specified state. 
 *  State          : Indicates whether the coordinates are to be returned for  
 *                   interactive or file processing                       (input)
 *  Direction      : Indicates whether the coordinates are to be returned for  
 *                   input or output                                      (input)
 *  coordinates    : Sinusoidal projection coordinates to be returned     (output)
 */
{ /* Get_Sinusoidal_Coordinates */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Sinusoidal)
      error_code |= ENGINE_INVALID_TYPE;
    else
      *coordinates = CS_State[State][Direction].coordinates.Sinusoidal;
  }
  return ( error_code );
} /* Get_Sinusoidal_Coordinates */


long Set_Stereographic_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Stereographic_Parameters parameters)
/*
 *  The function Set_Stereographic_Params sets the Stereographic projection  
 *  parameters for the specified state. 
 *  State        : Indicates whether the parameters are to be set for  
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the parameters are to be set for  
 *                 input or output                                     (input)
 *  parameters   : Stereographic projection parameters to be set           (input)
 */
{ /* Set_Stereographic_Params */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Stereographic)
      error_code |= ENGINE_INVALID_TYPE;
    else
      CS_State[State][Direction].parameters.Stereographic = parameters;
  }
  return (error_code);
} /* Set_Stereographic_Params */


long Get_Stereographic_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Stereographic_Parameters      *parameters)
/*
 *  The function Get_Stereographic_Params returns the Stereographic projection  
 *  parameters for the specified state. 
 *  State        : Indicates whether the parameters are to be returned for
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the parameters are to be returned for  
 *                 input or output                                     (input)
 *  parameters   : Stereographic projection parameters to be returned      (output)
 */
{ /* Get_Stereographic_Params */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Stereographic)
      error_code |= ENGINE_INVALID_TYPE;
    else
      *parameters = CS_State[State][Direction].parameters.Stereographic;
  }
  return ( error_code );
} /* Get_Stereographic_Params */


long Set_Stereographic_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Stereographic_Tuple     coordinates)
/*
 *  The function Set_Stereographic_Coordinates sets the Stereographic projection
 *  coordinates for the specified state. 
 *  State        : Indicates whether the coordinates are to be set for  
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the coordinates are to be set for  
 *                 input or output                                     (input)
 *  coordinates  : Stereographic projection coordinates to be set          (input)
 */
{ /* Set_Stereographic_Coordinates */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Stereographic)
      error_code |= ENGINE_INVALID_TYPE;
    else
      CS_State[State][Direction].coordinates.Stereographic = coordinates;
  }
  return ( error_code );
} /* Set_Stereographic_Coordinates */


long Get_Stereographic_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Stereographic_Tuple           *coordinates)
/*
 *  The function Get_Stereographic_Coordinates returns the Stereographic projection
 *  coordinates for the specified state. 
 *  State          : Indicates whether the coordinates are to be returned for  
 *                   interactive or file processing                    (input)
 *  Direction      : Indicates whether the coordinates are to be returned for  
 *                   input or output                                   (input)
 *  coordinates    : Stereographic projection coordinates to be returned   (output)
 */
{ /* Get_Stereographic_Coordinates */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Stereographic)
      error_code |= ENGINE_INVALID_TYPE;
    else
      *coordinates = CS_State[State][Direction].coordinates.Stereographic;
  }
  return ( error_code );
} /* Get_Stereographic_Coordinates */


long Set_Transverse_Cylindrical_Equal_Area_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Transverse_Cylindrical_Equal_Area_Parameters parameters)
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
{ /* Set_Transverse_Cylindrical_Equal_Area_Params */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Transverse_Cylindrical_Equal_Area)
      error_code |= ENGINE_INVALID_TYPE;
    else
      CS_State[State][Direction].parameters.Transverse_Cylindrical_Equal_Area = parameters;
  }
  return (error_code);
} /* Set_Transverse_Cylindrical_Equal_Area_Params */


long Get_Transverse_Cylindrical_Equal_Area_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Transverse_Cylindrical_Equal_Area_Parameters *parameters)
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
{ /* Get_Transverse_Cylindrical_Equal_Area_Params */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Transverse_Cylindrical_Equal_Area)
      error_code |= ENGINE_INVALID_TYPE;
    else
      *parameters = CS_State[State][Direction].parameters.Transverse_Cylindrical_Equal_Area;
  }
  return ( error_code );
} /* Get_Transverse_Cylindrical_Equal_Area_Params */


long Set_Transverse_Cylindrical_Equal_Area_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Transverse_Cylindrical_Equal_Area_Tuple coordinates)
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
{ /* Set_Transverse_Cylindrical_Equal_Area_Coordinates */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Transverse_Cylindrical_Equal_Area)
      error_code |= ENGINE_INVALID_TYPE;
    else
      CS_State[State][Direction].coordinates.Transverse_Cylindrical_Equal_Area = coordinates;
  }
  return ( error_code );
} /* Set_Transverse_Cylindrical_Equal_Area_Coordinates */


long Get_Transverse_Cylindrical_Equal_Area_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Transverse_Cylindrical_Equal_Area_Tuple *coordinates)
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
{ /* Get_Transverse_Cylindrical_Equal_Area_Coordinates */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Transverse_Cylindrical_Equal_Area)
      error_code |= ENGINE_INVALID_TYPE;
    else
      *coordinates = CS_State[State][Direction].coordinates.Transverse_Cylindrical_Equal_Area;
  }
  return ( error_code );
} /* Get_Transverse_Cylindrical_Equal_Area_Coordinates */


long Set_Transverse_Mercator_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Transverse_Mercator_Parameters parameters)
/*
 *  The function Set_Transverse_Mercator_Params sets the Transverse   
 *  Mercator projection parameters for the specified state. 
 *  State        : Indicates whether the parameters are to be set for  
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the parameters are to be set for  
 *                 input or output                                     (input)
 *  parameters   : Transverse Mercator projection parameters to be set (input)
 */
{ /* Set_Transverse_Mercator_Params */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Transverse_Mercator)
      error_code |= ENGINE_INVALID_TYPE;
    else
      CS_State[State][Direction].parameters.Transverse_Mercator = parameters;
  }
  return (error_code);
} /* Set_Transverse_Mercator_Params */


long Get_Transverse_Mercator_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Transverse_Mercator_Parameters *parameters)
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
{ /* Get_Transverse_Mercator_Params */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Transverse_Mercator)
      error_code |= ENGINE_INVALID_TYPE;
    else
      *parameters = CS_State[State][Direction].parameters.Transverse_Mercator;
  }
  return ( error_code );
} /* Get_Transverse_Mercator_Params */


long Set_Transverse_Mercator_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Transverse_Mercator_Tuple coordinates)
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
{ /* Set_Transverse_Mercator_Coordinates */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Transverse_Mercator)
      error_code |= ENGINE_INVALID_TYPE;
    else
      CS_State[State][Direction].coordinates.Transverse_Mercator = coordinates;
  }
  return ( error_code );
} /* Set_Transverse_Mercator_Coordinates */


long Get_Transverse_Mercator_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Transverse_Mercator_Tuple *coordinates)
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
{ /* Get_Transverse_Mercator_Coordinates */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Transverse_Mercator)
      error_code |= ENGINE_INVALID_TYPE;
    else
      *coordinates = CS_State[State][Direction].coordinates.Transverse_Mercator;
  }
  return ( error_code );
} /* Get_Transverse_Mercator_Coordinates */


long Set_UPS_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const UPS_Tuple           coordinates)
/*
 *  The function Set_UPS_Coordinates sets the UPS projection coordinates
 *  for the specified state. 
 *  State        : Indicates whether the coordinates are to be set for  
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the coordinates are to be set for  
 *                 input or output                                     (input)
 *  coordinates  : UPS projection coordinates to be set                (input)
 */
{ /* Set_UPS_Coordinates */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != UPS)
      error_code |= ENGINE_INVALID_TYPE;
    else
      CS_State[State][Direction].coordinates.UPS = coordinates;
  }
  return ( error_code );
} /* Set_UPS_Coordinates */


long Get_UPS_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  UPS_Tuple                 *coordinates)
/*
 *  The function Get_UPS_Coordinates returns the UPS projection coordinates 
 *  for the specified state. 
 *  State          : Indicates whether the coordinates are to be returned for  
 *                   interactive or file processing                    (input)
 *  Direction      : Indicates whether the coordinates are to be returned for  
 *                   input or output                                   (input)
 *  coordinates    : UPS projection coordinates to be returned         (output)
 */
{ /* Get_UPS_Coordinates */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != UPS)
      error_code |= ENGINE_INVALID_TYPE;
    else
      *coordinates = CS_State[State][Direction].coordinates.UPS;
  }
  return ( error_code );
} /* Get_UPS_Coordinates */


long Set_UTM_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const UTM_Parameters      parameters)
/*
 *  The function Set_UTM_Params sets the UTM projection parameters   
 *  for the specified state. 
 *  State        : Indicates whether the parameters are to be set for  
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the parameters are to be set for  
 *                 input or output                                     (input)
 *  parameters   : UTM projection parameters to be set                 (input)
 */
{ /* Set_UTM_Params */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != UTM)
      error_code |= ENGINE_INVALID_TYPE;
    else
      CS_State[State][Direction].parameters.UTM = parameters;
  }
  return ( error_code );
} /* Set_UTM_Params */


long Get_UTM_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  UTM_Parameters            *parameters)
/*
 *  The function Get_UTM_Params returns the UTM projection parameters 
 *  for the specified state. 
 *  State        : Indicates whether the parameters are to be returned for
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the parameters are to be returned for  
 *                 input or output                                     (input)
 *  parameters   : UTM projection parameters to be returned            (output)
 */
{ /* Get_UTM_Params */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != UTM)
      error_code |= ENGINE_INVALID_TYPE;
    else
      *parameters = CS_State[State][Direction].parameters.UTM;
  }
  return ( error_code );
} /* Get_UTM_Params */


long Set_UTM_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const UTM_Tuple           coordinates)
/*
 *  The function Set_UTM_Coordinates sets the UTM projection coordinates  
 *  for the specified state. 
 *  State        : Indicates whether the coordinates are to be set for  
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the coordinates are to be set for  
 *                 input or output                                     (input)
 *  coordinates  : UTM projection coordinates to be set                (input)
 */
{ /* Set_UTM_Coordinates */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != UTM)
      error_code |= ENGINE_INVALID_TYPE;
    else
      CS_State[State][Direction].coordinates.UTM = coordinates;
  }
  return ( error_code );
} /* Set_UTM_Coordinates */


long Get_UTM_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  UTM_Tuple                 *coordinates)
/*
 *  The function Get_UTM_Coordinates returns the UTM projection coordinates
 *  for the specified state. 
 *  State          : Indicates whether the coordinates are to be returned for  
 *                   interactive or file processing                    (input)
 *  Direction      : Indicates whether the coordinates are to be returned for  
 *                   input or output                                   (input)
 *  coordinates    : UTM projection coordinates to be returned         (output)
 */
{ /* Get_UTM_Coordinates */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != UTM)
      error_code |= ENGINE_INVALID_TYPE;
    else
      *coordinates = CS_State[State][Direction].coordinates.UTM;
  }
  return ( error_code );
} /* Get_UTM_Coordinates */


long Set_Van_der_Grinten_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Van_der_Grinten_Parameters parameters)
/*
 *  The function Set_Van_der_Grinten_Params sets the Van der Grinten   
 *  projection parameters for the specified state. 
 *  State        : Indicates whether the parameters are to be set for  
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the parameters are to be set for  
 *                 input or output                                     (input)
 *  parameters   : Van der Grinten projection parameters to be set     (input)
 */
{ /* Set_Van_der_Grinten_Params */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Van_der_Grinten)
      error_code |= ENGINE_INVALID_TYPE;
    else
      CS_State[State][Direction].parameters.Van_der_Grinten = parameters;
  }
  return (error_code);
} /* Set_Van_der_Grinten_Params */


long Get_Van_der_Grinten_Params
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  Van_der_Grinten_Parameters *parameters)
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
{ /* Get_Van_der_Grinten_Params */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Van_der_Grinten)
      error_code |= ENGINE_INVALID_TYPE;
    else
      *parameters = CS_State[State][Direction].parameters.Van_der_Grinten;
  }
  return ( error_code );
} /* Get_Van_der_Grinten_Params */


long Set_Van_der_Grinten_Coordinates
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  const Van_der_Grinten_Tuple coordinates)
/*
 *  The function Set_Van_der_Grinten_Coordinates sets the Van der Grinten 
 *  projection coordinates for the specified state. 
 *  State        : Indicates whether the coordinates are to be set for  
 *                 interactive or file processing                      (input)
 *  Direction    : Indicates whether the coordinates are to be set for  
 *                 input or output                                     (input)
 *  coordinates  : Van der Grinten projection coordinates to be set    (input)
 */
{ /* Set_Van_der_Grinten_Coordinates */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Van_der_Grinten)
      error_code |= ENGINE_INVALID_TYPE;
    else
      CS_State[State][Direction].coordinates.Van_der_Grinten = coordinates;
  }
  return ( error_code );
} /* Set_Van_der_Grinten_Coordinates */


long Get_Van_der_Grinten_Coordinates
( const File_or_Interactive State,
  const Input_or_Output Direction,
  Van_der_Grinten_Tuple *coordinates)
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
{ /* Get_Van_der_Grinten_Coordinates */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (CS_State[State][Direction].type != Van_der_Grinten)
      error_code |= ENGINE_INVALID_TYPE;
    else
      *coordinates = CS_State[State][Direction].coordinates.Van_der_Grinten;
  }
  return ( error_code );
} /* Get_Van_der_Grinten_Coordinates */


long Convert
( const File_or_Interactive State)
/*
 *  The function Convert converts the current input coordinates in the coordinate
 *  system defined by the current input coordinate system parameters and input datum,
 *  into output coordinates in the coordinate system defined by the output coordinate
 *  system parameters and output datum.
 *  State      : Indicates whether the interactive or file processing state data
 *               is to be used                                             (input)
 */
{ /* Convert */
  Coordinate_State_Row *input;
  Coordinate_State_Row *output;
  Geodetic_Tuple Converted_Geodetic = {0.0, 0.0, 0.0};
  Geodetic_Tuple WGS84_Geodetic = {0.0, 0.0, 0.0};
  Geodetic_Tuple Shifted_Geodetic = {0.0, 0.0, 0.0};
  double input_a;
  double input_f;
  double output_a;
  double output_f;
  char Input_Ellipsoid_Code[3];
  long Input_Ellipsoid_Index;
  char Output_Ellipsoid_Code[3];
  long Output_Ellipsoid_Index;
  long error_code = ENGINE_NO_ERROR;
  long temp_error = ENGINE_NO_ERROR;
  long input_valid = 0;
  long output_valid = 0;
  long special = 0;

  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    input = &(CS_State[State][Input]);
    output = &(CS_State[State][Output]);
    input->status = ENGINE_NO_ERROR;
    output->status = ENGINE_NO_ERROR;
    Datum_Ellipsoid_Code(input->datum_Index, Input_Ellipsoid_Code);
    Datum_Ellipsoid_Code(output->datum_Index, Output_Ellipsoid_Code);
    Ellipsoid_Index(Input_Ellipsoid_Code, &Input_Ellipsoid_Index);
    Ellipsoid_Index(Output_Ellipsoid_Code, &Output_Ellipsoid_Index);
    Ellipsoid_Parameters(Input_Ellipsoid_Index,&input_a,&input_f);
    Ellipsoid_Parameters(Output_Ellipsoid_Index,&output_a,&output_f);
    /********************************************************/
    /* Check for special cases when there is no datum shift */
    /********************************************************/
    if (input->datum_Index == output->datum_Index)
    {
      if ((input->type == Geocentric) && (output->type == Local_Cartesian))
      {
        Local_Cartesian_Parameters param = output->parameters.Local_Cartesian;
        temp_error = Set_Local_Cartesian_Parameters(output_a,output_f,
                                                    param.origin_latitude,
                                                    param.origin_longitude,
                                                    param.origin_height,
                                                    param.orientation);
        if (temp_error)
        {
          if (temp_error & LOCCART_A_ERROR)
            output->status |= ENGINE_A_ERROR;
          if (temp_error & LOCCART_INV_F_ERROR)
            output->status |= ENGINE_INV_F_ERROR;
          if (temp_error & LOCCART_LAT_ERROR)
            output->status |= ENGINE_LAT_ERROR;
          if (temp_error & LOCCART_LON_ERROR)
            output->status |= ENGINE_LON_ERROR;
          if (temp_error & LOCCART_ORIGIN_LAT_ERROR)
            output->status |= ENGINE_ORIGIN_LAT_ERROR;
          if (temp_error & LOCCART_ORIGIN_LON_ERROR)
            output->status |= ENGINE_ORIGIN_LON_ERROR;
        }
        else
        {
          Convert_Geocentric_To_Local_Cartesian(input->coordinates.Geocentric.x,
                                                input->coordinates.Geocentric.y,
                                                input->coordinates.Geocentric.z,
                                                &(output->coordinates.Local_Cartesian.x),
                                                &(output->coordinates.Local_Cartesian.y),
                                                &(output->coordinates.Local_Cartesian.z));
        }
        special = 1;
      }
      else if ((input->type == Local_Cartesian) && (output->type == Geocentric))
      {
        Local_Cartesian_Parameters param = input->parameters.Local_Cartesian;
        temp_error = Set_Local_Cartesian_Parameters(input_a,input_f,
                                                    param.origin_latitude,
                                                    param.origin_longitude,
                                                    param.origin_height,
                                                    param.orientation);
        if (temp_error)
        {
          if (temp_error & LOCCART_A_ERROR)
            input->status |= ENGINE_A_ERROR;
          if (temp_error & LOCCART_INV_F_ERROR)
            input->status |= ENGINE_INV_F_ERROR;
          if (temp_error & LOCCART_LAT_ERROR)
            input->status |= ENGINE_LAT_ERROR;
          if (temp_error & LOCCART_LON_ERROR)
            input->status |= ENGINE_LON_ERROR;
          if (temp_error & LOCCART_ORIGIN_LAT_ERROR)
            input->status |= ENGINE_ORIGIN_LAT_ERROR;
          if (temp_error & LOCCART_ORIGIN_LON_ERROR)
            input->status |= ENGINE_ORIGIN_LON_ERROR;
        }
        else
        {
          Convert_Local_Cartesian_To_Geocentric(input->coordinates.Local_Cartesian.x,
                                                input->coordinates.Local_Cartesian.y,
                                                input->coordinates.Local_Cartesian.z,
                                                &(output->coordinates.Geocentric.x),
                                                &(output->coordinates.Geocentric.y),
                                                &(output->coordinates.Geocentric.z));
        }
        special = 1;
      }
      else if ((input->type == MGRS) && 
               (output->type == UTM) && (output->parameters.UTM.override == 0))
      {
        temp_error = Set_MGRS_Parameters(input_a,input_f,
                                         Input_Ellipsoid_Code);
        if (temp_error)
        {
          if (temp_error & MGRS_A_ERROR)
            input->status |= ENGINE_A_ERROR;
          if (temp_error & MGRS_INV_F_ERROR)
            input->status |= ENGINE_INV_F_ERROR;
        }
        else
        {
          temp_error = Convert_MGRS_To_UTM(input->coordinates.MGRS.string,
                                           &(output->coordinates.UTM.zone),
                                           &(output->coordinates.UTM.hemisphere),
                                           &(output->coordinates.UTM.easting),
                                           &(output->coordinates.UTM.northing));
          if (temp_error & MGRS_STRING_ERROR)
            input->status = ENGINE_STRING_ERROR;
        }
        special = 1;
      }
      else if ((input->type == MGRS) && (output->type == UPS))
      {
        temp_error = Set_MGRS_Parameters(input_a,input_f,
                                         Input_Ellipsoid_Code);
        if (temp_error)
        {
          if (temp_error & MGRS_A_ERROR)
            input->status |= ENGINE_A_ERROR;
          if (temp_error & MGRS_INV_F_ERROR)
            input->status |= ENGINE_INV_F_ERROR;
        }
        else
        {
          temp_error = Convert_MGRS_To_UPS(input->coordinates.MGRS.string,
                                           &(output->coordinates.UPS.hemisphere),
                                           &(output->coordinates.UPS.easting),
                                           &(output->coordinates.UPS.northing));
          if (temp_error & MGRS_STRING_ERROR)
            input->status = ENGINE_STRING_ERROR;
        }
        special = 1;
      }
      else if ((input->type == UTM) && (output->type == MGRS))
      {
        temp_error = ENGINE_NO_ERROR;
        temp_error = Set_MGRS_Parameters(output_a,output_f,
                                         Output_Ellipsoid_Code);
        if (temp_error)
        {
          if (temp_error & MGRS_A_ERROR)
            output->status |= ENGINE_A_ERROR;
          if (temp_error & MGRS_INV_F_ERROR)
            output->status |= ENGINE_INV_F_ERROR;
        }
        else
        {
          UTM_Tuple coord = input->coordinates.UTM;
          Precision temp_precision = Engine_Precision;
          if (temp_precision < 0)
            temp_precision = (Precision)0;
          if (temp_precision > 5)
            temp_precision = (Precision)5;
          temp_error = Convert_UTM_To_MGRS(coord.zone,
                                           coord.hemisphere,
                                           coord.easting,
                                           coord.northing,
                                           temp_precision,
                                           output->coordinates.MGRS.string);
          if (temp_error & MGRS_ZONE_ERROR)
            input->status |= ENGINE_ZONE_ERROR;
          if (temp_error & MGRS_HEMISPHERE_ERROR)
            input->status |= ENGINE_HEMISPHERE_ERROR;
          if (temp_error & MGRS_EASTING_ERROR)
            input->status |= ENGINE_EASTING_ERROR;
          if (temp_error & MGRS_NORTHING_ERROR)
            input->status |= ENGINE_NORTHING_ERROR;
        }
        special = 1;
      }
      else if ((input->type == UPS) && (output->type == MGRS))
      {
        temp_error = ENGINE_NO_ERROR;
        temp_error = Set_MGRS_Parameters(output_a,output_f,
                                         Output_Ellipsoid_Code);
        if (temp_error)
        {
          if (temp_error & MGRS_A_ERROR)
            output->status |= ENGINE_A_ERROR;
          if (temp_error & MGRS_INV_F_ERROR)
            output->status |= ENGINE_INV_F_ERROR;
        }
        else
        {
          UPS_Tuple coord = input->coordinates.UPS;
          Precision temp_precision = Engine_Precision;
          if (temp_precision < 0)
            temp_precision = (Precision)0;
          if (temp_precision > 5)
            temp_precision = (Precision)5;
          temp_error = Convert_UPS_To_MGRS(coord.hemisphere,
                                           coord.easting,coord.northing,
                                           temp_precision,
                                           output->coordinates.MGRS.string);
          if (temp_error & MGRS_HEMISPHERE_ERROR)
            input->status |= ENGINE_HEMISPHERE_ERROR;
          if (temp_error & MGRS_EASTING_ERROR)
            input->status |= ENGINE_EASTING_ERROR;
          if (temp_error & MGRS_NORTHING_ERROR)
            input->status |= ENGINE_NORTHING_ERROR;
        }
        special = 1;
      }
      else if ((input->type == Transverse_Mercator) && (output->type == BNG))
      {
        Transverse_Mercator_Parameters param = input->parameters.Transverse_Mercator;
        if ((param.central_meridian == -2.0 * PI / 180) &&
            (param.origin_latitude == 49.0 * PI / 180) &&
            (param.scale_factor == .9996012717) &&
            (param.false_easting == 400000.0) && 
            (param.false_northing == -100000.0))
        {
          temp_error = Set_BNG_Parameters(Output_Ellipsoid_Code);
                                         
          if (temp_error)
          {
            if (temp_error & BNG_ELLIPSOID_ERROR)
              output->status |= ENGINE_ELLIPSOID_CODE_ERROR;
          }
          else
          {
            Transverse_Mercator_Tuple coord = input->coordinates.Transverse_Mercator;
            Precision temp_precision = Engine_Precision;
            if (temp_precision < 0)
              temp_precision = (Precision)0;
            if (temp_precision > 5)
              temp_precision = (Precision)5;
            temp_error = Convert_Transverse_Mercator_To_BNG(coord.easting,
                                             coord.northing,
                                             temp_precision,
                                             output->coordinates.BNG.string);
            if (temp_error & BNG_EASTING_ERROR)
              output->status |= ENGINE_EASTING_ERROR;
            if (temp_error & BNG_NORTHING_ERROR)
              output->status |= ENGINE_NORTHING_ERROR;
            if (temp_error & BNG_INVALID_AREA_ERROR)
              output->status |= ENGINE_INVALID_AREA_ERROR;
          }
          special = 1;
        }
        else
          special = 0;
      }
      else if ((input->type == BNG) && (output->type == Transverse_Mercator))
      {
        Transverse_Mercator_Parameters param = output->parameters.Transverse_Mercator;
        if ((param.central_meridian == -2.0 * PI / 180) &&
            (param.origin_latitude == 49.0 * PI / 180) &&
            (param.scale_factor == .9996012717) &&
            (param.false_easting == 400000.0) && 
            (param.false_northing == -100000.0))
        {
          temp_error = Set_BNG_Parameters(Input_Ellipsoid_Code);
                                        
          if (temp_error)
          {
            if (temp_error & BNG_ELLIPSOID_ERROR)
              input->status |= ENGINE_ELLIPSOID_CODE_ERROR;
          }
          else
          {
            Precision temp_precision = Engine_Precision;
            if (temp_precision < 0)
              temp_precision = (Precision)0;
            if (temp_precision > 5)
              temp_precision = (Precision)5;
            temp_error = Convert_BNG_To_Transverse_Mercator(input->coordinates.BNG.string,
                                             &(output->coordinates.Transverse_Mercator.easting),
                                             &(output->coordinates.Transverse_Mercator.northing));
            if (temp_error & BNG_INVALID_AREA_ERROR)
              input->status |= ENGINE_INVALID_AREA_ERROR;
            if (temp_error & BNG_STRING_ERROR)
              input->status |= ENGINE_STRING_ERROR;
          }
          special = 1;
        }
        else
          special = 0;
      }
    }
    if (special)
    {  /* Special case error reporting */
      if (input->status)
        error_code = ENGINE_INPUT_ERROR;
      if (output->status)
        error_code = ENGINE_OUTPUT_ERROR;
    }
    else
    {
      /**********************************************************/
      /* First coordinate conversion stage, convert to Geodetic */
      /**********************************************************/
      switch (input->type)
      {
      case Geocentric:
        {
          temp_error = ENGINE_NO_ERROR;
          temp_error = Set_Geocentric_Parameters(input_a,input_f);
          if (temp_error)
          {
            if (temp_error & GEOCENT_A_ERROR)
              input->status |= ENGINE_A_ERROR;
            if (temp_error & GEOCENT_INV_F_ERROR)
              input->status |= ENGINE_INV_F_ERROR;
          }
          else
          {
            Convert_Geocentric_To_Geodetic(input->coordinates.Geocentric.x,
                                           input->coordinates.Geocentric.y,
                                           input->coordinates.Geocentric.z,
                                           &(Converted_Geodetic.latitude),
                                           &(Converted_Geodetic.longitude),
                                           &(Converted_Geodetic.height));
          }
          break;
        }
      case Geodetic:
        {
          Converted_Geodetic.latitude = input->coordinates.Geodetic.latitude;
          Converted_Geodetic.longitude = input->coordinates.Geodetic.longitude;
          Converted_Geodetic.height = input->coordinates.Geodetic.height;
          if (input->parameters.Geodetic.height_type == No_Height)
            Converted_Geodetic.height = 0.0;
          break;
        }
      case GEOREF:
        {
          temp_error = Convert_GEOREF_To_Geodetic(input->coordinates.GEOREF.string,
                                                  &(Converted_Geodetic.latitude),
                                                  &(Converted_Geodetic.longitude));
          if (temp_error & GEOREF_STR_ERROR)
            input->status |= ENGINE_STRING_ERROR;
          if (temp_error & GEOREF_STR_LON_ERROR)
            input->status |= ENGINE_STRING_ERROR;
          if (temp_error & GEOREF_STR_LAT_ERROR)
            input->status |= ENGINE_STRING_ERROR;
          if (temp_error & GEOREF_STR_LON_MIN_ERROR)
            input->status |= ENGINE_STR_LON_MIN_ERROR;
          if (temp_error & GEOREF_STR_LAT_MIN_ERROR)
            input->status |= ENGINE_STR_LAT_MIN_ERROR;
          break;
        }
      case Albers_Equal_Area_Conic:
        {
          Albers_Equal_Area_Conic_Parameters param = input->parameters.Albers_Equal_Area_Conic;
          temp_error = Set_Albers_Parameters(input_a,input_f,
                                             param.origin_latitude,
                                             param.central_meridian,
                                             param.std_parallel_1,
                                             param.std_parallel_2,
                                             param.false_easting,
                                             param.false_northing);
          if (temp_error)
          {
            if (temp_error & ALBERS_A_ERROR)
              input->status |= ENGINE_A_ERROR;
            if (temp_error & ALBERS_INV_F_ERROR)
              input->status |= ENGINE_INV_F_ERROR;
            if (temp_error & ALBERS_ORIGIN_LAT_ERROR)
              input->status |= ENGINE_ORIGIN_LAT_ERROR;
            if (temp_error & ALBERS_CENT_MER_ERROR)
              input->status |= ENGINE_CENT_MER_ERROR;
            if (temp_error & ALBERS_FIRST_STDP_ERROR)
              input->status |= ENGINE_FIRST_STDP_ERROR;
            if (temp_error & ALBERS_SECOND_STDP_ERROR)
              input->status |= ENGINE_SECOND_STDP_ERROR;
            if (temp_error & ALBERS_FIRST_SECOND_ERROR)
              input->status |= ENGINE_FIRST_SECOND_ERROR;
            if (temp_error & ALBERS_HEMISPHERE_ERROR)
              input->status |= ENGINE_HEMISPHERE_ERROR;

          }
          else
          {
            Albers_Equal_Area_Conic_Tuple coord = input->coordinates.Albers_Equal_Area_Conic;
            temp_error = Convert_Albers_To_Geodetic(coord.easting,coord.northing,
                                                    &(Converted_Geodetic.latitude),
                                                    &(Converted_Geodetic.longitude));
            if (temp_error & ALBERS_EASTING_ERROR)
              input->status |= ENGINE_EASTING_ERROR;
            if (temp_error & ALBERS_NORTHING_ERROR)
              input->status |= ENGINE_NORTHING_ERROR;
          }
          break;
        }
      case Azimuthal_Equidistant:
        {
          Azimuthal_Equidistant_Parameters param = input->parameters.Azimuthal_Equidistant;
          temp_error = Set_Azimuthal_Equidistant_Parameters(input_a,input_f,
                                                            param.origin_latitude,
                                                            param.central_meridian,
                                                            param.false_easting,
                                                            param.false_northing);
          if (temp_error)
          {
            if (temp_error & AZEQ_A_ERROR)
              input->status |= ENGINE_A_ERROR;
            if (temp_error & AZEQ_INV_F_ERROR)
              input->status |= ENGINE_INV_F_ERROR;
            if (temp_error & AZEQ_ORIGIN_LAT_ERROR)
              input->status |= ENGINE_ORIGIN_LAT_ERROR;
            if (temp_error & AZEQ_CENT_MER_ERROR)
              input->status |= ENGINE_CENT_MER_ERROR;
          }
          else
          {
            Azimuthal_Equidistant_Tuple coord = input->coordinates.Azimuthal_Equidistant;
            temp_error = Convert_Azimuthal_Equidistant_To_Geodetic(coord.easting,coord.northing,
                                                                   &(Converted_Geodetic.latitude),
                                                                   &(Converted_Geodetic.longitude));
            if (temp_error & AZEQ_EASTING_ERROR)
              input->status |= ENGINE_EASTING_ERROR;
            if (temp_error & AZEQ_NORTHING_ERROR)
              input->status |= ENGINE_NORTHING_ERROR;
          }
          break;
        }
      case BNG:
        {
          temp_error = Set_BNG_Parameters(Input_Ellipsoid_Code);
          if (temp_error)
          {
            if (temp_error & BNG_ELLIPSOID_ERROR)
              input->status |= ENGINE_ELLIPSOID_CODE_ERROR;
          }
          else
          {
            temp_error = Convert_BNG_To_Geodetic(input->coordinates.BNG.string,
                                                  &(Converted_Geodetic.latitude),
                                                  &(Converted_Geodetic.longitude));
            if (temp_error & BNG_STRING_ERROR)
              input->status = ENGINE_STRING_ERROR;
            if (temp_error & BNG_INVALID_AREA_ERROR)
              input->status = ENGINE_INVALID_AREA_ERROR;
            if (temp_error & BNG_EASTING_ERROR)
              input->status |= ENGINE_EASTING_ERROR;
            if (temp_error & BNG_NORTHING_ERROR)
              input->status |= ENGINE_NORTHING_ERROR;
            if (temp_error & BNG_LAT_ERROR)
              input->status |= ENGINE_LAT_ERROR;
            if (temp_error & BNG_LON_ERROR)
              input->status |= ENGINE_LON_ERROR;
          }
          break;
        }
      case Bonne:
        {
          Bonne_Parameters param = input->parameters.Bonne;
          temp_error = Set_Bonne_Parameters(input_a,input_f,
                                            param.origin_latitude,
                                            param.central_meridian,
                                            param.false_easting,
                                            param.false_northing);
          if (temp_error)
          {
            if (temp_error & BONN_A_ERROR)
              input->status |= ENGINE_A_ERROR;
            if (temp_error & BONN_INV_F_ERROR)
              input->status |= ENGINE_INV_F_ERROR;
            if (temp_error & BONN_ORIGIN_LAT_ERROR)
              input->status |= ENGINE_ORIGIN_LAT_ERROR;
            if (temp_error & BONN_CENT_MER_ERROR)
              input->status |= ENGINE_CENT_MER_ERROR;
          }
          else
          {
            Bonne_Tuple coord = input->coordinates.Bonne;
            temp_error = Convert_Bonne_To_Geodetic(coord.easting,coord.northing,
                                                   &(Converted_Geodetic.latitude),
                                                   &(Converted_Geodetic.longitude));
            if (temp_error & BONN_EASTING_ERROR)
              input->status |= ENGINE_EASTING_ERROR;
            if (temp_error & BONN_NORTHING_ERROR)
              input->status |= ENGINE_NORTHING_ERROR;
          }
          break;
        }
      case Cassini:
        {
          Cassini_Parameters param = input->parameters.Cassini;
          temp_error = Set_Cassini_Parameters(input_a,input_f,
                                              param.origin_latitude,
                                              param.central_meridian,
                                              param.false_easting,
                                              param.false_northing);
          if (temp_error)
          {
            if (temp_error & CASS_A_ERROR)
              input->status |= ENGINE_A_ERROR;
            if (temp_error & CASS_INV_F_ERROR)
              input->status |= ENGINE_INV_F_ERROR;
            if (temp_error & CASS_ORIGIN_LAT_ERROR)
              input->status |= ENGINE_ORIGIN_LAT_ERROR;
            if (temp_error & CASS_CENT_MER_ERROR)
              input->status |= ENGINE_CENT_MER_ERROR;
          }
          else
          {
            Cassini_Tuple coord = input->coordinates.Cassini;
            temp_error = Convert_Cassini_To_Geodetic(coord.easting,coord.northing,
                                                     &(Converted_Geodetic.latitude),
                                                     &(Converted_Geodetic.longitude));
            if (temp_error & CASS_EASTING_ERROR)
              input->status |= ENGINE_EASTING_ERROR;
            if (temp_error & CASS_NORTHING_ERROR)
              input->status |= ENGINE_NORTHING_ERROR;
            if (temp_error & CASS_LON_WARNING)
              output->status |= ENGINE_LON_WARNING;
          }
          break;
        }
      case Cylindrical_Equal_Area:
        {
          Cylindrical_Equal_Area_Parameters param = input->parameters.Cylindrical_Equal_Area;
          temp_error = Set_Cyl_Eq_Area_Parameters(input_a,input_f,
                                                  param.origin_latitude,
                                                  param.central_meridian,
                                                  param.false_easting,
                                                  param.false_northing);
          if (temp_error)
          {
            if (temp_error & CYEQ_A_ERROR)
              input->status |= ENGINE_A_ERROR;
            if (temp_error & CYEQ_INV_F_ERROR)
              input->status |= ENGINE_INV_F_ERROR;
            if (temp_error & CYEQ_ORIGIN_LAT_ERROR)
              input->status |= ENGINE_ORIGIN_LAT_ERROR;
            if (temp_error & CYEQ_CENT_MER_ERROR)
              input->status |= ENGINE_CENT_MER_ERROR;
          }
          else
          {
            Cylindrical_Equal_Area_Tuple coord = input->coordinates.Cylindrical_Equal_Area;
            temp_error = Convert_Cyl_Eq_Area_To_Geodetic(coord.easting,coord.northing,
                                                         &(Converted_Geodetic.latitude),
                                                         &(Converted_Geodetic.longitude));
            if (temp_error & CYEQ_EASTING_ERROR)
              input->status |= ENGINE_EASTING_ERROR;
            if (temp_error & CYEQ_NORTHING_ERROR)
              input->status |= ENGINE_NORTHING_ERROR;
          }
          break;
        }
      case Eckert4:
        {
          Eckert4_Parameters param = input->parameters.Eckert4;
          temp_error = Set_Eckert4_Parameters(input_a,input_f,
                                              param.central_meridian,
                                              param.false_easting,
                                              param.false_northing);
          if (temp_error)
          {
            if (temp_error & ECK4_A_ERROR)
              input->status |= ENGINE_A_ERROR;
            if (temp_error & ECK4_INV_F_ERROR)
              input->status |= ENGINE_INV_F_ERROR;
            if (temp_error & ECK4_CENT_MER_ERROR)
              input->status |= ENGINE_CENT_MER_ERROR;
          }
          else
          {
            Eckert4_Tuple coord = input->coordinates.Eckert4;
            temp_error = Convert_Eckert4_To_Geodetic(coord.easting,coord.northing,
                                                     &(Converted_Geodetic.latitude),
                                                     &(Converted_Geodetic.longitude));
            if (temp_error & ECK4_EASTING_ERROR)
              input->status |= ENGINE_EASTING_ERROR;
            if (temp_error & ECK4_NORTHING_ERROR)
              input->status |= ENGINE_NORTHING_ERROR;
          }
          break;
        }
      case Eckert6:
        {
          Eckert6_Parameters param = input->parameters.Eckert6;
          temp_error = Set_Eckert6_Parameters(input_a,input_f,
                                              param.central_meridian,
                                              param.false_easting,
                                              param.false_northing);
          if (temp_error)
          {
            if (temp_error & ECK6_A_ERROR)
              input->status |= ENGINE_A_ERROR;
            if (temp_error & ECK6_INV_F_ERROR)
              input->status |= ENGINE_INV_F_ERROR;
            if (temp_error & ECK6_CENT_MER_ERROR)
              input->status |= ENGINE_CENT_MER_ERROR;
          }
          else
          {
            Eckert6_Tuple coord = input->coordinates.Eckert6;
            temp_error = Convert_Eckert6_To_Geodetic(coord.easting,coord.northing,
                                                     &(Converted_Geodetic.latitude),
                                                     &(Converted_Geodetic.longitude));
            if (temp_error & ECK6_EASTING_ERROR)
              input->status |= ENGINE_EASTING_ERROR;
            if (temp_error & ECK6_NORTHING_ERROR)
              input->status |= ENGINE_NORTHING_ERROR;
          }
          break;
        }
      case Equidistant_Cylindrical:
        {
          Equidistant_Cylindrical_Parameters param = input->parameters.Equidistant_Cylindrical;
          temp_error = Set_Equidistant_Cyl_Parameters(input_a,input_f,
                                                      param.std_parallel,
                                                      param.central_meridian,
                                                      param.false_easting,
                                                      param.false_northing);
          if (temp_error)
          {
            if (temp_error & EQCY_A_ERROR)
              input->status |= ENGINE_A_ERROR;
            if (temp_error & EQCY_INV_F_ERROR)
              input->status |= ENGINE_INV_F_ERROR;
            if (temp_error & EQCY_STDP_ERROR)
              input->status |= ENGINE_STDP_ERROR;
            if (temp_error & EQCY_CENT_MER_ERROR)
              input->status |= ENGINE_CENT_MER_ERROR;
          }
          else
          {
            Equidistant_Cylindrical_Tuple coord = input->coordinates.Equidistant_Cylindrical;
            temp_error = Convert_Equidistant_Cyl_To_Geodetic(coord.easting,coord.northing,
                                                             &(Converted_Geodetic.latitude),
                                                             &(Converted_Geodetic.longitude));
            if (temp_error & EQCY_EASTING_ERROR)
              input->status |= ENGINE_EASTING_ERROR;
            if (temp_error & EQCY_NORTHING_ERROR)
              input->status |= ENGINE_NORTHING_ERROR;
          }
          break;
        }
      case Gnomonic:
        {
          Gnomonic_Parameters param = input->parameters.Gnomonic;
          temp_error = Set_Gnomonic_Parameters(input_a,input_f,
                                               param.origin_latitude,
                                               param.central_meridian,
                                               param.false_easting,
                                               param.false_northing);
          if (temp_error)
          {
            if (temp_error & GNOM_A_ERROR)
              input->status |= ENGINE_A_ERROR;
            if (temp_error & GNOM_INV_F_ERROR)
              input->status |= ENGINE_INV_F_ERROR;
            if (temp_error & GNOM_ORIGIN_LAT_ERROR)
              input->status |= ENGINE_ORIGIN_LAT_ERROR;
            if (temp_error & GNOM_CENT_MER_ERROR)
              input->status |= ENGINE_CENT_MER_ERROR;
          }
          else
          {
            Gnomonic_Tuple coord = input->coordinates.Gnomonic;
            temp_error = Convert_Gnomonic_To_Geodetic(coord.easting,coord.northing,
                                                      &(Converted_Geodetic.latitude),
                                                      &(Converted_Geodetic.longitude));
            if (temp_error & GNOM_EASTING_ERROR)
              input->status |= ENGINE_EASTING_ERROR;
            if (temp_error & GNOM_NORTHING_ERROR)
              input->status |= ENGINE_NORTHING_ERROR;
          }
          break;
        }
      case Lambert_Conformal_Conic_1:
        {
          Lambert_Conformal_Conic_1_Parameters param = input->parameters.Lambert_Conformal_Conic_1;
          temp_error = Set_Lambert_1_Parameters(input_a,input_f,
                                                param.origin_latitude,
                                                param.central_meridian,
                                                param.false_easting,
                                                param.false_northing,
                                                param.scale_factor);
          if (temp_error)
          {
            if (temp_error & LAMBERT_1_A_ERROR)
              input->status |= ENGINE_A_ERROR;
            if (temp_error & LAMBERT_1_INV_F_ERROR)
              input->status |= ENGINE_INV_F_ERROR;
            if (temp_error & LAMBERT_1_ORIGIN_LAT_ERROR)
              input->status |= ENGINE_ORIGIN_LAT_ERROR;
            if (temp_error & LAMBERT_1_CENT_MER_ERROR)
              input->status |= ENGINE_CENT_MER_ERROR;
            if (temp_error & LAMBERT_1_SCALE_FACTOR_ERROR)
              input->status |= ENGINE_SCALE_FACTOR_ERROR;
          }
          else
          {
            Lambert_Conformal_Conic_1_Tuple coord = input->coordinates.Lambert_Conformal_Conic_1;
            temp_error = Convert_Lambert_1_To_Geodetic(coord.easting,coord.northing,
                                                       &(Converted_Geodetic.latitude),
                                                       &(Converted_Geodetic.longitude));
            if (temp_error & LAMBERT_1_EASTING_ERROR)
              input->status |= ENGINE_EASTING_ERROR;
            if (temp_error & LAMBERT_1_NORTHING_ERROR)
              input->status |= ENGINE_NORTHING_ERROR;
          }
          break;
        }
      case Lambert_Conformal_Conic:
        {
          Lambert_Conformal_Conic_Parameters param = input->parameters.Lambert_Conformal_Conic;
          temp_error = Set_Lambert_Parameters(input_a,input_f,
                                              param.origin_latitude,
                                              param.central_meridian,
                                              param.std_parallel_1,
                                              param.std_parallel_2,
                                              param.false_easting,
                                              param.false_northing);
          if (temp_error)
          {
            if (temp_error & LAMBERT_A_ERROR)
              input->status |= ENGINE_A_ERROR;
            if (temp_error & LAMBERT_INV_F_ERROR)
              input->status |= ENGINE_INV_F_ERROR;
            if (temp_error & LAMBERT_SECOND_STDP_ERROR)
              input->status |= ENGINE_SECOND_STDP_ERROR;
            if (temp_error & LAMBERT_FIRST_STDP_ERROR)
              input->status |= ENGINE_FIRST_STDP_ERROR;
            if (temp_error & LAMBERT_FIRST_SECOND_ERROR)
              input->status |= ENGINE_FIRST_SECOND_ERROR;
            if (temp_error & LAMBERT_HEMISPHERE_ERROR)
              input->status |= ENGINE_HEMISPHERE_ERROR;
            if (temp_error & LAMBERT_CENT_MER_ERROR)
              input->status |= ENGINE_CENT_MER_ERROR;
            if (temp_error & LAMBERT_ORIGIN_LAT_ERROR)
              input->status |= ENGINE_ORIGIN_LAT_ERROR;
          }
          else
          {
            Lambert_Conformal_Conic_Tuple coord = input->coordinates.Lambert_Conformal_Conic;
            temp_error = Convert_Lambert_To_Geodetic(coord.easting,coord.northing,
                                                     &(Converted_Geodetic.latitude),
                                                     &(Converted_Geodetic.longitude));
            if (temp_error & LAMBERT_EASTING_ERROR)
              input->status |= ENGINE_EASTING_ERROR;
            if (temp_error & LAMBERT_NORTHING_ERROR)
              input->status |= ENGINE_NORTHING_ERROR;
          }
          break;
        }
      case Local_Cartesian:
        {
          Local_Cartesian_Parameters param = input->parameters.Local_Cartesian;
          temp_error = Set_Local_Cartesian_Parameters(input_a,input_f,
                                                      param.origin_latitude,
                                                      param.origin_longitude,
                                                      param.origin_height,
                                                      param.orientation);
          if (temp_error)
          {
            if (temp_error & LOCCART_A_ERROR)
              input->status |= ENGINE_A_ERROR;
            if (temp_error & LOCCART_INV_F_ERROR)
              input->status |= ENGINE_INV_F_ERROR;
            if (temp_error & LOCCART_LAT_ERROR)
              input->status |= ENGINE_LAT_ERROR;
            if (temp_error & LOCCART_LON_ERROR)
              input->status |= ENGINE_LON_ERROR;
            if (temp_error & LOCCART_ORIGIN_LAT_ERROR)
              input->status |= ENGINE_ORIGIN_LAT_ERROR;
            if (temp_error & LOCCART_ORIGIN_LON_ERROR)
              input->status |= ENGINE_ORIGIN_LON_ERROR;
          }
          else
          {
            Local_Cartesian_Tuple coord = input->coordinates.Local_Cartesian;
            Convert_Local_Cartesian_To_Geodetic(coord.x,coord.y,coord.z,
                                                &(Converted_Geodetic.latitude),
                                                &(Converted_Geodetic.longitude),
                                                &(Converted_Geodetic.height));
          }
          break;
        }
      case Mercator:
        {
          Mercator_Parameters param = input->parameters.Mercator;
          temp_error = Set_Mercator_Parameters( input_a,input_f,
                                                param.origin_latitude,
                                                param.central_meridian,
                                                param.false_easting,
                                                param.false_northing,
                                                &(input->parameters.Mercator.scale_factor));
          if (temp_error)
          {
            if (temp_error & MERC_A_ERROR)
              input->status |= ENGINE_A_ERROR;
            if (temp_error & MERC_INV_F_ERROR)
              input->status |= ENGINE_INV_F_ERROR;
            if (temp_error & MERC_ORIGIN_LAT_ERROR)
              input->status |= ENGINE_ORIGIN_LAT_ERROR;
            if (temp_error & MERC_CENT_MER_ERROR)
              input->status |= ENGINE_CENT_MER_ERROR;
          }
          else
          {
            Mercator_Tuple coord = input->coordinates.Mercator;
            temp_error = Convert_Mercator_To_Geodetic(coord.easting,coord.northing,
                                                      &(Converted_Geodetic.latitude),
                                                      &(Converted_Geodetic.longitude));
            if (temp_error & MERC_EASTING_ERROR)
              input->status |= ENGINE_EASTING_ERROR;
            if (temp_error & MERC_NORTHING_ERROR)
              input->status |= ENGINE_NORTHING_ERROR;
          }
          break;
        }
      case MGRS:
        {
          temp_error = Set_MGRS_Parameters(input_a,input_f,
                                           Input_Ellipsoid_Code);
          if (temp_error)
          {
            if (temp_error & MGRS_A_ERROR)
              input->status |= ENGINE_A_ERROR;
            if (temp_error & MGRS_INV_F_ERROR)
              input->status |= ENGINE_INV_F_ERROR;
          }
          else
          {
            temp_error = Convert_MGRS_To_Geodetic(input->coordinates.MGRS.string,
                                                  &(Converted_Geodetic.latitude),
                                                  &(Converted_Geodetic.longitude));
            if (temp_error & MGRS_STRING_ERROR)
              input->status = ENGINE_STRING_ERROR;

          }
          break;
        }
      case Miller_Cylindrical:
        {
          Miller_Cylindrical_Parameters param = input->parameters.Miller_Cylindrical;
          temp_error = Set_Miller_Parameters(input_a,input_f,
                                             param.central_meridian,
                                             param.false_easting,
                                             param.false_northing);
          if (temp_error)
          {
            if (temp_error & MILL_A_ERROR)
              input->status |= ENGINE_A_ERROR;
            if (temp_error & MILL_INV_F_ERROR)
              input->status |= ENGINE_INV_F_ERROR;
            if (temp_error & MILL_CENT_MER_ERROR)
              input->status |= ENGINE_CENT_MER_ERROR;
          }
          else
          {
            Miller_Cylindrical_Tuple coord = input->coordinates.Miller_Cylindrical;
            temp_error = Convert_Miller_To_Geodetic(coord.easting,coord.northing,
                                                    &(Converted_Geodetic.latitude),
                                                    &(Converted_Geodetic.longitude));
            if (temp_error & MILL_EASTING_ERROR)
              input->status |= ENGINE_EASTING_ERROR;
            if (temp_error & MILL_NORTHING_ERROR)
              input->status |= ENGINE_NORTHING_ERROR;
          }
          break;
        }
      case Mollweide:
        {
          Mollweide_Parameters param = input->parameters.Mollweide;
          temp_error = Set_Mollweide_Parameters(input_a,input_f,
                                                param.central_meridian,
                                                param.false_easting,
                                                param.false_northing);
          if (temp_error)
          {
            if (temp_error & MOLL_A_ERROR)
              input->status |= ENGINE_A_ERROR;
            if (temp_error & MOLL_INV_F_ERROR)
              input->status |= ENGINE_INV_F_ERROR;
            if (temp_error & MOLL_CENT_MER_ERROR)
              input->status |= ENGINE_CENT_MER_ERROR;
          }
          else
          {
            Mollweide_Tuple coord = input->coordinates.Mollweide;
            temp_error = Convert_Mollweide_To_Geodetic(coord.easting,coord.northing,
                                                       &(Converted_Geodetic.latitude),
                                                       &(Converted_Geodetic.longitude));
            if (temp_error & MOLL_EASTING_ERROR)
              input->status |= ENGINE_EASTING_ERROR;
            if (temp_error & MOLL_NORTHING_ERROR)
              input->status |= ENGINE_NORTHING_ERROR;
          }
          break;
        } 
      case Neys:
        {
          Neys_Parameters param = input->parameters.Neys;
          temp_error = Set_Neys_Parameters(input_a,input_f,
                                           param.origin_latitude,
                                           param.central_meridian,
                                           param.std_parallel_1,
                                           param.false_easting,
                                           param.false_northing);
          if (temp_error)
          {
            if (temp_error & NEYS_A_ERROR)
              input->status |= ENGINE_A_ERROR;
            if (temp_error & NEYS_INV_F_ERROR)
              input->status |= ENGINE_INV_F_ERROR;
            if (temp_error & NEYS_FIRST_STDP_ERROR)
              input->status |= ENGINE_FIRST_STDP_ERROR;
            if (temp_error & NEYS_CENT_MER_ERROR)
              input->status |= ENGINE_CENT_MER_ERROR;
            if (temp_error & NEYS_ORIGIN_LAT_ERROR)
              input->status |= ENGINE_ORIGIN_LAT_ERROR;
          }
          else
          {
            Neys_Tuple coord = input->coordinates.Neys;
            temp_error = Convert_Neys_To_Geodetic(coord.easting,coord.northing,
                                                  &(Converted_Geodetic.latitude),
                                                  &(Converted_Geodetic.longitude));
            if (temp_error & NEYS_EASTING_ERROR)
              input->status |= ENGINE_EASTING_ERROR;
            if (temp_error & NEYS_NORTHING_ERROR)
              input->status |= ENGINE_NORTHING_ERROR;
          }
          break;
        }
      case NZMG:
        {
          temp_error = Set_NZMG_Parameters(Input_Ellipsoid_Code);
          if (temp_error)
          {
            if (temp_error & NZMG_ELLIPSOID_ERROR)
              input->status |= ENGINE_ELLIPSOID_CODE_ERROR;
          }
          else
          {
            NZMG_Tuple coord = input->coordinates.NZMG;
            temp_error = Convert_NZMG_To_Geodetic(coord.easting,coord.northing,
                                                  &(Converted_Geodetic.latitude),
                                                  &(Converted_Geodetic.longitude));
            if (temp_error & NZMG_EASTING_ERROR)
              input->status |= ENGINE_EASTING_ERROR;
            if (temp_error & NZMG_NORTHING_ERROR)
              input->status |= ENGINE_NORTHING_ERROR;
          }
          break;
        }
      case Oblique_Mercator:
        {
          Oblique_Mercator_Parameters param = input->parameters.Oblique_Mercator;
          temp_error = Set_Oblique_Mercator_Parameters(input_a,input_f,
                                                       param.origin_latitude,
                                                       param.latitude_1,
                                                       param.longitude_1,
                                                       param.latitude_2,
                                                       param.longitude_2,
                                                       param.false_easting,
                                                       param.false_northing,
                                                       param.scale_factor);
          if (temp_error)
          {
            if (temp_error & OMERC_A_ERROR)
              input->status |= ENGINE_A_ERROR;
            if (temp_error & OMERC_INV_F_ERROR)
              input->status |= ENGINE_INV_F_ERROR;
            if (temp_error & OMERC_ORIGIN_LAT_ERROR)
              input->status |= ENGINE_ORIGIN_LAT_ERROR;
            if (temp_error & OMERC_LAT1_ERROR)
              input->status |= ENGINE_LAT1_ERROR;
            if (temp_error & OMERC_LON1_ERROR)
              input->status |= ENGINE_LON1_ERROR;
            if (temp_error & OMERC_LAT2_ERROR)
              input->status |= ENGINE_LAT2_ERROR;
            if (temp_error & OMERC_LON2_ERROR)
              input->status |= ENGINE_LON2_ERROR;
            if (temp_error & OMERC_LAT1_LAT2_ERROR)
              input->status |= ENGINE_LAT1_LAT2_ERROR;
            if (temp_error & OMERC_DIFF_HEMISPHERE_ERROR)
              input->status |= ENGINE_HEMISPHERE_ERROR;
            if (temp_error & OMERC_SCALE_FACTOR_ERROR)
              input->status |= ENGINE_SCALE_FACTOR_ERROR;
          }
          else
          {
            Oblique_Mercator_Tuple coord = input->coordinates.Oblique_Mercator;
            temp_error = Convert_Oblique_Mercator_To_Geodetic(coord.easting,coord.northing,
                                                            &(Converted_Geodetic.latitude),
                                                            &(Converted_Geodetic.longitude));
            if (temp_error & OMERC_EASTING_ERROR)
              input->status |= ENGINE_EASTING_ERROR;
            if (temp_error & OMERC_NORTHING_ERROR)
              input->status |= ENGINE_NORTHING_ERROR;
            if (temp_error & OMERC_LON_WARNING)
              output->status |= ENGINE_LON_WARNING;
          }
          break;
        }
      case Orthographic:
        {
          Orthographic_Parameters param = input->parameters.Orthographic;
          temp_error = Set_Orthographic_Parameters(input_a,input_f,
                                                   param.origin_latitude,
                                                   param.central_meridian,
                                                   param.false_easting,
                                                   param.false_northing);
          if (temp_error)
          {
            if (temp_error & ORTH_A_ERROR)
              input->status |= ENGINE_A_ERROR;
            if (temp_error & ORTH_INV_F_ERROR)
              input->status |= ENGINE_INV_F_ERROR;
            if (temp_error & ORTH_ORIGIN_LAT_ERROR)
              input->status |= ENGINE_ORIGIN_LAT_ERROR;
            if (temp_error & ORTH_CENT_MER_ERROR)
              input->status |= ENGINE_CENT_MER_ERROR;
          }
          else
          {
            Orthographic_Tuple coord = input->coordinates.Orthographic;
            temp_error = Convert_Orthographic_To_Geodetic(coord.easting,coord.northing,
                                                          &(Converted_Geodetic.latitude),
                                                          &(Converted_Geodetic.longitude));
            if (temp_error & ORTH_EASTING_ERROR)
              input->status |= ENGINE_EASTING_ERROR;
            if (temp_error & ORTH_NORTHING_ERROR)
              input->status |= ENGINE_NORTHING_ERROR;
            if (temp_error & ORTH_RADIUS_ERROR)
              input->status |= ENGINE_RADIUS_ERROR;
          }
          break;
        }
      case Polar_Stereo:
        {
          Polar_Stereo_Parameters param = input->parameters.Polar_Stereo;
          temp_error = Set_Polar_Stereographic_Parameters( input_a,input_f,
                                                           param.latitude_of_true_scale,
                                                           param.longitude_down_from_pole,
                                                           param.false_easting,
                                                           param.false_northing);
          if (temp_error)
          {
            if (temp_error & POLAR_A_ERROR)
              input->status |= ENGINE_A_ERROR;
            if (temp_error & POLAR_INV_F_ERROR)
              input->status |= ENGINE_INV_F_ERROR;
            if (temp_error & POLAR_ORIGIN_LAT_ERROR)
              input->status |= ENGINE_ORIGIN_LAT_ERROR;
          }
          else
          {
            Polar_Stereo_Tuple coord = input->coordinates.Polar_Stereo;
            temp_error = Convert_Polar_Stereographic_To_Geodetic(coord.easting,coord.northing,
                                                                 &(Converted_Geodetic.latitude),
                                                                 &(Converted_Geodetic.longitude));
            if (temp_error & POLAR_EASTING_ERROR)
              input->status |= ENGINE_EASTING_ERROR;
            if (temp_error & POLAR_NORTHING_ERROR)
              input->status |= ENGINE_NORTHING_ERROR;
            if (temp_error & POLAR_RADIUS_ERROR)
              input->status |= ENGINE_RADIUS_ERROR;

          } 
          break;
        }
      case Polyconic:
        {
          Polyconic_Parameters param = input->parameters.Polyconic;
          temp_error = Set_Polyconic_Parameters(input_a,input_f,
                                                param.origin_latitude,
                                                param.central_meridian,
                                                param.false_easting,
                                                param.false_northing);
          if (temp_error)
          {
            if (temp_error & POLY_A_ERROR)
              input->status |= ENGINE_A_ERROR;
            if (temp_error & POLY_INV_F_ERROR)
              input->status |= ENGINE_INV_F_ERROR;
            if (temp_error & POLY_ORIGIN_LAT_ERROR)
              input->status |= ENGINE_ORIGIN_LAT_ERROR;
            if (temp_error & POLY_CENT_MER_ERROR)
              input->status |= ENGINE_CENT_MER_ERROR;
          }
          else
          {
            Polyconic_Tuple coord = input->coordinates.Polyconic;
            temp_error = Convert_Polyconic_To_Geodetic(coord.easting,coord.northing,
                                                       &(Converted_Geodetic.latitude),
                                                       &(Converted_Geodetic.longitude));
            if (temp_error & POLY_EASTING_ERROR)
              input->status |= ENGINE_EASTING_ERROR;
            if (temp_error & POLY_NORTHING_ERROR)
              input->status |= ENGINE_NORTHING_ERROR;
          }
          break;
        }
      case Sinusoidal:
        {
          Sinusoidal_Parameters param = input->parameters.Sinusoidal;
          temp_error = Set_Sinusoidal_Parameters(input_a,input_f,
                                                 param.central_meridian,
                                                 param.false_easting,
                                                 param.false_northing);
          if (temp_error)
          {
            if (temp_error & SINU_A_ERROR)
              input->status |= ENGINE_A_ERROR;
            if (temp_error & SINU_INV_F_ERROR)
              input->status |= ENGINE_INV_F_ERROR;
            if (temp_error & SINU_CENT_MER_ERROR)
              input->status |= ENGINE_CENT_MER_ERROR;
          }
          else
          {
            Sinusoidal_Tuple coord = input->coordinates.Sinusoidal;
            temp_error = Convert_Sinusoidal_To_Geodetic(coord.easting,coord.northing,
                                                        &(Converted_Geodetic.latitude),
                                                        &(Converted_Geodetic.longitude));
            if (temp_error & SINU_EASTING_ERROR)
              input->status |= ENGINE_EASTING_ERROR;
            if (temp_error & SINU_NORTHING_ERROR)
              input->status |= ENGINE_NORTHING_ERROR;
          }
          break;
        }
      case Stereographic:
        {
          Stereographic_Parameters param = input->parameters.Stereographic;
          temp_error = Set_Stereographic_Parameters(input_a,input_f,
                                                param.origin_latitude,
                                                param.central_meridian,
                                                param.false_easting,
                                                param.false_northing);
          if (temp_error)
          {
            if (temp_error & STEREO_A_ERROR)
              input->status |= ENGINE_A_ERROR;
            if (temp_error & STEREO_INV_F_ERROR)
              input->status |= ENGINE_INV_F_ERROR;
            if (temp_error & STEREO_ORIGIN_LAT_ERROR)
              input->status |= ENGINE_ORIGIN_LAT_ERROR;
            if (temp_error & STEREO_CENT_MER_ERROR)
              input->status |= ENGINE_CENT_MER_ERROR;
          }
          else
          {
            Stereographic_Tuple coord = input->coordinates.Stereographic;
            temp_error = Convert_Stereographic_To_Geodetic(coord.easting,coord.northing,
                                                       &(Converted_Geodetic.latitude),
                                                       &(Converted_Geodetic.longitude));
            if (temp_error & STEREO_EASTING_ERROR)
              input->status |= ENGINE_EASTING_ERROR;
            if (temp_error & STEREO_NORTHING_ERROR)
              input->status |= ENGINE_NORTHING_ERROR;
          }
          break;
        }
      case Transverse_Cylindrical_Equal_Area:
        {
          Transverse_Cylindrical_Equal_Area_Parameters param = input->parameters.Transverse_Cylindrical_Equal_Area;
          temp_error = Set_Trans_Cyl_Eq_Area_Parameters(input_a,input_f,
                                                        param.origin_latitude,
                                                        param.central_meridian,
                                                        param.false_easting,
                                                        param.false_northing,
                                                        param.scale_factor);
          if (temp_error)
          {
            if (temp_error & TCEA_A_ERROR)
              input->status |= ENGINE_A_ERROR;
            if (temp_error & TCEA_INV_F_ERROR)
              input->status |= ENGINE_INV_F_ERROR;
            if (temp_error & TCEA_ORIGIN_LAT_ERROR)
              input->status |= ENGINE_ORIGIN_LAT_ERROR;
            if (temp_error & TCEA_CENT_MER_ERROR)
              input->status |= ENGINE_CENT_MER_ERROR;
            if (temp_error & TCEA_SCALE_FACTOR_ERROR)
              input->status |= ENGINE_SCALE_FACTOR_ERROR;
          }
          else
          {
            Transverse_Cylindrical_Equal_Area_Tuple coord = input->coordinates.Transverse_Cylindrical_Equal_Area;
            temp_error = Convert_Trans_Cyl_Eq_Area_To_Geodetic(coord.easting,coord.northing,
                                                               &(Converted_Geodetic.latitude),
                                                               &(Converted_Geodetic.longitude));
            if (temp_error & TCEA_EASTING_ERROR)
              input->status |= ENGINE_EASTING_ERROR;
            if (temp_error & TCEA_NORTHING_ERROR)
              input->status |= ENGINE_NORTHING_ERROR;
          }
          break;
        }
      case Transverse_Mercator:
        {
          Transverse_Mercator_Parameters param = input->parameters.Transverse_Mercator;
          temp_error = Set_Transverse_Mercator_Parameters(
                                                         input_a,input_f,
                                                         param.origin_latitude,
                                                         param.central_meridian,
                                                         param.false_easting,
                                                         param.false_northing,
                                                         param.scale_factor);
          if (temp_error)
          {
            if (temp_error & TRANMERC_A_ERROR)
              input->status |= ENGINE_A_ERROR;
            if (temp_error & TRANMERC_INV_F_ERROR)
              input->status |= ENGINE_INV_F_ERROR;
            if (temp_error & TRANMERC_ORIGIN_LAT_ERROR)
              input->status |= ENGINE_ORIGIN_LAT_ERROR;
            if (temp_error & TRANMERC_CENT_MER_ERROR)
              input->status |= ENGINE_CENT_MER_ERROR;
            if (temp_error & TRANMERC_SCALE_FACTOR_ERROR)
              input->status |= ENGINE_SCALE_FACTOR_ERROR;
          }
          else
          {
            Transverse_Mercator_Tuple coord = input->coordinates.Transverse_Mercator;
            temp_error = Convert_Transverse_Mercator_To_Geodetic(coord.easting,coord.northing,
                                                                 &(Converted_Geodetic.latitude),
                                                                 &(Converted_Geodetic.longitude));
            if (temp_error & TRANMERC_EASTING_ERROR)
              input->status |= ENGINE_EASTING_ERROR;
            if (temp_error & TRANMERC_NORTHING_ERROR)
              input->status |= ENGINE_NORTHING_ERROR;
            if (temp_error & TRANMERC_LON_WARNING)
              output->status |= ENGINE_LON_WARNING;
          }
          break;
        }
      case UPS:
        {  
          temp_error = Set_UPS_Parameters(input_a,input_f);
          if (temp_error)
          {
            if (temp_error & UPS_A_ERROR)
              input->status |= ENGINE_A_ERROR;
            if (temp_error & UPS_INV_F_ERROR)
              input->status |= ENGINE_INV_F_ERROR;
          }
          else
          {
            UPS_Tuple coord = input->coordinates.UPS;
            temp_error = Convert_UPS_To_Geodetic(coord.hemisphere,
                                                 coord.easting,coord.northing,
                                                 &(Converted_Geodetic.latitude),
                                                 &(Converted_Geodetic.longitude));
            if (temp_error & UPS_HEMISPHERE_ERROR)
              input->status |= ENGINE_HEMISPHERE_ERROR;
            if (temp_error & UPS_EASTING_ERROR)
              input->status |= ENGINE_EASTING_ERROR;
            if (temp_error & UPS_NORTHING_ERROR)
              input->status |= ENGINE_NORTHING_ERROR;
            if (temp_error & UPS_LAT_ERROR)
              input->status |= ENGINE_RADIUS_ERROR;
          }
          break;                               
        }
      case UTM:
        {
          temp_error = Set_UTM_Parameters(input_a,input_f,0);
          if (temp_error)
          {
            if (temp_error & UTM_A_ERROR)
              input->status |= ENGINE_A_ERROR;
            if (temp_error & UTM_INV_F_ERROR)
              input->status |= ENGINE_INV_F_ERROR;
            if (temp_error & UTM_ZONE_OVERRIDE_ERROR)
              input->status |= ENGINE_ZONE_OVERRIDE_ERROR;
          }
          else
          {
            UTM_Tuple coord = input->coordinates.UTM;
            temp_error = Convert_UTM_To_Geodetic(coord.zone,coord.hemisphere,
                                                 coord.easting,coord.northing,
                                                 &(Converted_Geodetic.latitude),
                                                 &(Converted_Geodetic.longitude));
            if (temp_error & UTM_ZONE_ERROR)
              input->status |= ENGINE_ZONE_ERROR;
            if (temp_error & UTM_HEMISPHERE_ERROR)
              input->status |= ENGINE_HEMISPHERE_ERROR;
            if (temp_error & UTM_EASTING_ERROR)
              input->status |= ENGINE_EASTING_ERROR;
            if (temp_error & UTM_NORTHING_ERROR)
              input->status |= ENGINE_NORTHING_ERROR;
          }
          break;
        }
      case Van_der_Grinten:
        {
          Van_der_Grinten_Parameters param = input->parameters.Van_der_Grinten;
          temp_error = Set_Van_der_Grinten_Parameters(input_a,input_f,
                                                      param.central_meridian,
                                                      param.false_easting,
                                                      param.false_northing);
          if (temp_error)
          {
            if (temp_error & GRIN_A_ERROR)
              input->status |= ENGINE_A_ERROR;
            if (temp_error & GRIN_INV_F_ERROR)
              input->status |= ENGINE_INV_F_ERROR;
            if (temp_error & GRIN_CENT_MER_ERROR)
              input->status |= ENGINE_CENT_MER_ERROR;
          }
          else
          {
            Van_der_Grinten_Tuple coord = input->coordinates.Van_der_Grinten;
            temp_error = Convert_Van_der_Grinten_To_Geodetic(coord.easting,coord.northing,
                                                             &(Converted_Geodetic.latitude),
                                                             &(Converted_Geodetic.longitude));
            if (temp_error & GRIN_EASTING_ERROR)
              input->status |= ENGINE_EASTING_ERROR;
            if (temp_error & GRIN_NORTHING_ERROR)
              input->status |= ENGINE_NORTHING_ERROR;
            if (temp_error & GRIN_RADIUS_ERROR)
              input->status |= ENGINE_RADIUS_ERROR;
          }
          break;
        }   
      } /* switch (input->type) */
      if ((input->status) && 
          (input->status != ENGINE_DATUM_WARNING) &&
          (input->status != ENGINE_LON_WARNING))
        error_code |= ENGINE_INPUT_ERROR;
      if (input->status == ENGINE_LON_WARNING)
        error_code |= ENGINE_INPUT_WARNING;
      /******************************/
      /* Datum Transformation Stage */
      /******************************/
      if (!(input->status))
      { /* Datum transformation */
        long WGS84_datum_index;
        Height_Type input_height_type;
        Height_Type output_height_type;
        if (input->type == Geodetic)
          input_height_type = input->parameters.Geodetic.height_type;
        else if ((input->type == Geocentric) || (input->type == Local_Cartesian))
          input_height_type = Ellipsoid_Height;
        else
          input_height_type = No_Height;
        if (output->type == Geodetic)
          output_height_type = output->parameters.Geodetic.height_type;
        else if ((output->type == Geocentric) || (output->type == Local_Cartesian))
          output_height_type = Ellipsoid_Height;
        else
          output_height_type = No_Height;
        Datum_Index ("WGE", &WGS84_datum_index);
        if ((input->datum_Index == output->datum_Index)
            && ((input_height_type == output_height_type)
                || (input_height_type == No_Height)
                || (output_height_type == No_Height)))
        { /* Copy coordinate tuple */
          WGS84_Geodetic = Converted_Geodetic;
          Shifted_Geodetic = Converted_Geodetic;
          if ((input_height_type == No_Height) || (output_height_type == No_Height))
            Shifted_Geodetic.height = 0.0;
        }
        else
        { /* Shift to WGS84, apply geoid correction, shift to output datum */
          if (input->datum_Index != WGS84_datum_index)
          {
            Geodetic_Shift_To_WGS84(input->datum_Index,
                                    Converted_Geodetic.latitude,
                                    Converted_Geodetic.longitude,
                                    Converted_Geodetic.height,
                                    &(WGS84_Geodetic.latitude),
                                    &(WGS84_Geodetic.longitude),
                                    &(WGS84_Geodetic.height));
            if (input_height_type == Geoid_or_MSL_Height)
              WGS84_Geodetic.height = Converted_Geodetic.height;
            else if (input_height_type == No_Height)
              WGS84_Geodetic.height = 0.0;
            /* check input datum validity */
            temp_error = Valid_Datum (input->datum_Index, 
                                      WGS84_Geodetic.latitude, WGS84_Geodetic.longitude, &input_valid);
            if (!input_valid)
            {
              input->status |= ENGINE_DATUM_WARNING;
              error_code |= ENGINE_INPUT_WARNING;
            }
          }
          else
          { /* Copy coordinate tuple */
            WGS84_Geodetic = Converted_Geodetic;
            if (input_height_type == No_Height)
              WGS84_Geodetic.height = 0.0;
          }

          if ((input_height_type == Geoid_or_MSL_Height) &&
              (output_height_type == Ellipsoid_Height))
          {
            double Corrected_Height;
            Convert_Geoid_To_Ellipsoid_Height ( WGS84_Geodetic.latitude,
                                                WGS84_Geodetic.longitude,
                                                WGS84_Geodetic.height,
                                                &(Corrected_Height));
            WGS84_Geodetic.height = Corrected_Height;
          }
          if ((input_height_type == Ellipsoid_Height) &&
              (output_height_type == Geoid_or_MSL_Height))
          {
            double Corrected_Height;
            Convert_Ellipsoid_To_Geoid_Height ( WGS84_Geodetic.latitude,
                                                WGS84_Geodetic.longitude,
                                                WGS84_Geodetic.height,
                                                &(Corrected_Height));
            WGS84_Geodetic.height = Corrected_Height;
          }
          if (output->datum_Index != WGS84_datum_index)
          {
            Geodetic_Shift_From_WGS84(WGS84_Geodetic.latitude,
                                      WGS84_Geodetic.longitude,
                                      WGS84_Geodetic.height,
                                      output->datum_Index,
                                      &(Shifted_Geodetic.latitude),
                                      &(Shifted_Geodetic.longitude),
                                      &(Shifted_Geodetic.height));
            if (output_height_type == Geoid_or_MSL_Height)
              Shifted_Geodetic.height = WGS84_Geodetic.height;
            else if (output_height_type == No_Height)
              Shifted_Geodetic.height = 0.0;
            /* check output datum validity */
            temp_error = Valid_Datum (output->datum_Index, 
                                      WGS84_Geodetic.latitude, WGS84_Geodetic.longitude, &output_valid);
            if (!output_valid)
            {
              output->status |= ENGINE_DATUM_WARNING;
              error_code |= ENGINE_OUTPUT_WARNING;
            }
          }
          else
          { /* Copy coordinate tuple */
            Shifted_Geodetic = WGS84_Geodetic;
            if (output_height_type == No_Height)
              Shifted_Geodetic.height = 0.0;
          }
        }
      }
      /* calculate conversion errors */
      if ((input->status) || (output->status))
      {
        ce90 = -1.0;
        le90 = -1.0;
        se90 = -1.0;
      }
      else
        temp_error = Datum_Shift_Error (input->datum_Index, output->datum_Index, 
                                        WGS84_Geodetic.latitude, WGS84_Geodetic.longitude, &ce90, &le90, &se90);
      /*************************************************************/
      /* Second coordinate conversion stage, convert from Geodetic */
      /*************************************************************/
      if (error_code != ENGINE_INPUT_ERROR)
      {
        switch (output->type)
        {
        case Geocentric:
          {
            temp_error = Set_Geocentric_Parameters(output_a,output_f);
            if (temp_error)
            {
              if (temp_error & GEOCENT_A_ERROR)
                output->status |= ENGINE_A_ERROR;
              if (temp_error & GEOCENT_INV_F_ERROR)
                output->status |= ENGINE_INV_F_ERROR;
            }
            else
            {
              temp_error = Convert_Geodetic_To_Geocentric(Shifted_Geodetic.latitude,
                                                          Shifted_Geodetic.longitude,
                                                          Shifted_Geodetic.height,
                                                          &(output->coordinates.Geocentric.x),
                                                          &(output->coordinates.Geocentric.y),
                                                          &(output->coordinates.Geocentric.z));
              if (temp_error & GEOCENT_LAT_ERROR)
                output->status |= ENGINE_LAT_ERROR;
              if (temp_error & GEOCENT_LON_ERROR)
                output->status |= ENGINE_LON_ERROR;
            }
            break;
          }
        case Geodetic:
          {
            output->coordinates.Geodetic.latitude = Shifted_Geodetic.latitude;
            output->coordinates.Geodetic.longitude = Shifted_Geodetic.longitude;
            output->coordinates.Geodetic.height = Shifted_Geodetic.height;
            if (output->parameters.Geodetic.height_type == No_Height)
              output->coordinates.Geodetic.height = 0.0;
            break;
          }
        case GEOREF:
          {
            Precision temp_precision = Engine_Precision;
            if (temp_precision < 0)
              temp_precision = (Precision)0;
            if (temp_precision > 5)
              temp_precision = (Precision)5;
            temp_error = Convert_Geodetic_To_GEOREF(Shifted_Geodetic.latitude,
                                                    Shifted_Geodetic.longitude,
                                                    temp_precision,
                                                    output->coordinates.GEOREF.string);
            if (temp_error & GEOREF_LAT_ERROR)
              output->status |= ENGINE_LAT_ERROR;
            if (temp_error & GEOREF_LON_ERROR)
              output->status |= ENGINE_LON_ERROR;
            break;
          }
        case Albers_Equal_Area_Conic:
          {
            Albers_Equal_Area_Conic_Parameters params = output->parameters.Albers_Equal_Area_Conic;
            temp_error = Set_Albers_Parameters( output_a,output_f,
                                                params.origin_latitude,
                                                params.central_meridian,
                                                params.std_parallel_1,
                                                params.std_parallel_2,
                                                params.false_easting,
                                                params.false_northing);
            if (temp_error)
            {
              if (temp_error & ALBERS_A_ERROR)
                output->status |= ENGINE_A_ERROR;
              if (temp_error & ALBERS_INV_F_ERROR)
                output->status |= ENGINE_INV_F_ERROR;
              if (temp_error & ALBERS_ORIGIN_LAT_ERROR)
                output->status |= ENGINE_ORIGIN_LAT_ERROR;
              if (temp_error & ALBERS_CENT_MER_ERROR)
                output->status |= ENGINE_CENT_MER_ERROR;
              if (temp_error & ALBERS_CENT_MER_ERROR)
                output->status |= ENGINE_CENT_MER_ERROR;
              if (temp_error & ALBERS_FIRST_STDP_ERROR)
                output->status |= ENGINE_FIRST_STDP_ERROR;
              if (temp_error & ALBERS_SECOND_STDP_ERROR)
                output->status |= ENGINE_SECOND_STDP_ERROR;
              if (temp_error & ALBERS_FIRST_SECOND_ERROR)
                output->status |= ENGINE_FIRST_SECOND_ERROR;
              if (temp_error & ALBERS_HEMISPHERE_ERROR)
                output->status |= ENGINE_HEMISPHERE_ERROR;
            }
            else
            {
              Albers_Equal_Area_Conic_Tuple coord;
              temp_error = Convert_Geodetic_To_Albers(Shifted_Geodetic.latitude,
                                                      Shifted_Geodetic.longitude,
                                                      &(coord.easting),
                                                      &(coord.northing));
              output->coordinates.Albers_Equal_Area_Conic = coord;
              if (temp_error & ALBERS_LAT_ERROR)
                output->status |= ENGINE_LAT_ERROR;
              if (temp_error & ALBERS_LON_ERROR)
                output->status |= ENGINE_LON_ERROR;
            }
            break;
          }
        case Azimuthal_Equidistant:
          {
            Azimuthal_Equidistant_Parameters param = output->parameters.Azimuthal_Equidistant;
            temp_error = Set_Azimuthal_Equidistant_Parameters( output_a,output_f,
                                                               param.origin_latitude,
                                                               param.central_meridian,
                                                               param.false_easting,
                                                               param.false_northing);
            if (temp_error)
            {
              if (temp_error & AZEQ_A_ERROR)
                output->status |= ENGINE_A_ERROR;
              if (temp_error & AZEQ_INV_F_ERROR)
                output->status |= ENGINE_INV_F_ERROR;
              if (temp_error & AZEQ_ORIGIN_LAT_ERROR)
                output->status |= ENGINE_ORIGIN_LAT_ERROR;
              if (temp_error & AZEQ_CENT_MER_ERROR)
                output->status |= ENGINE_CENT_MER_ERROR;
            }
            else
            {
              Azimuthal_Equidistant_Tuple coord;
              temp_error = Convert_Geodetic_To_Azimuthal_Equidistant(Shifted_Geodetic.latitude,
                                                                     Shifted_Geodetic.longitude,
                                                                     &(coord.easting),
                                                                     &(coord.northing));
              output->coordinates.Azimuthal_Equidistant = coord;
              if (temp_error & AZEQ_LAT_ERROR)
                output->status |= ENGINE_LAT_ERROR;
              if (temp_error & AZEQ_LON_ERROR)
                output->status |= ENGINE_LON_ERROR;
              if (temp_error & AZEQ_PROJECTION_ERROR)
                output->status |= ENGINE_PROJECTION_ERROR;
            }
            break;
          }
        case BNG:
          {
            temp_error = Set_BNG_Parameters(Output_Ellipsoid_Code);
                                             
            if (temp_error)
            {
              if (temp_error & BNG_ELLIPSOID_ERROR)
                output->status |= ENGINE_ELLIPSOID_CODE_ERROR;
            }
            else
            {
              Precision temp_precision = Engine_Precision;
              if (temp_precision < 0)
                temp_precision = (Precision)0;
              if (temp_precision > 5)
                temp_precision = (Precision)5;
              temp_error = Convert_Geodetic_To_BNG(Shifted_Geodetic.latitude,
                                       Shifted_Geodetic.longitude,
                                       temp_precision,
                                       output->coordinates.BNG.string);
              
              if (temp_error & BNG_INVALID_AREA_ERROR)
                output->status |= ENGINE_INVALID_AREA_ERROR;
              if (temp_error & BNG_EASTING_ERROR)
                output->status |= ENGINE_EASTING_ERROR;
              if (temp_error & BNG_NORTHING_ERROR)
                output->status |= ENGINE_NORTHING_ERROR;
              if (temp_error & BNG_LAT_ERROR)
                output->status |= ENGINE_LAT_ERROR;
              if (temp_error & BNG_LON_ERROR)
                output->status |= ENGINE_LON_ERROR;
            }
            break;
          }
        case Bonne:
          {
            Bonne_Parameters param = output->parameters.Bonne;
            temp_error = Set_Bonne_Parameters(output_a,output_f,
                                              param.origin_latitude,
                                              param.central_meridian,
                                              param.false_easting,
                                              param.false_northing);
            if (temp_error)
            {
              if (temp_error & BONN_A_ERROR)
                output->status |= ENGINE_A_ERROR;
              if (temp_error & BONN_INV_F_ERROR)
                output->status |= ENGINE_INV_F_ERROR;
              if (temp_error & BONN_ORIGIN_LAT_ERROR)
                output->status |= ENGINE_ORIGIN_LAT_ERROR;
              if (temp_error & BONN_CENT_MER_ERROR)
                output->status |= ENGINE_CENT_MER_ERROR;
            }
            else
            {
              Bonne_Tuple coord;
              temp_error = Convert_Geodetic_To_Bonne(Shifted_Geodetic.latitude,
                                                     Shifted_Geodetic.longitude,
                                                     &(coord.easting),
                                                     &(coord.northing));
              output->coordinates.Bonne = coord;
              if (temp_error & BONN_LAT_ERROR)
                output->status |= ENGINE_LAT_ERROR;
              if (temp_error & BONN_LON_ERROR)
                output->status |= ENGINE_LON_ERROR;
            }
            break;
          }
        case Cassini:
          {
            Cassini_Parameters param = output->parameters.Cassini;
            temp_error = Set_Cassini_Parameters(output_a,output_f,
                                                param.origin_latitude,
                                                param.central_meridian,
                                                param.false_easting,
                                                param.false_northing);
            if (temp_error)
            {
              if (temp_error & CASS_A_ERROR)
                output->status |= ENGINE_A_ERROR;
              if (temp_error & CASS_INV_F_ERROR)
                output->status |= ENGINE_INV_F_ERROR;
              if (temp_error & CASS_ORIGIN_LAT_ERROR)
                output->status |= ENGINE_ORIGIN_LAT_ERROR;
              if (temp_error & CASS_CENT_MER_ERROR)
                output->status |= ENGINE_CENT_MER_ERROR;
            }
            else
            {
              Cassini_Tuple coord;
              temp_error = Convert_Geodetic_To_Cassini(Shifted_Geodetic.latitude,
                                                       Shifted_Geodetic.longitude,
                                                       &(coord.easting),
                                                       &(coord.northing));
              output->coordinates.Cassini = coord;
              if (temp_error & CASS_LAT_ERROR)
                output->status |= ENGINE_LAT_ERROR;
              if (temp_error & CASS_LON_ERROR)
                output->status |= ENGINE_LON_ERROR;
              if (temp_error & CASS_LON_WARNING)
                output->status |= ENGINE_LON_WARNING;
            }
            break;
          }
        case Cylindrical_Equal_Area:
          {
            Cylindrical_Equal_Area_Parameters param = output->parameters.Cylindrical_Equal_Area;
            temp_error = Set_Cyl_Eq_Area_Parameters( output_a,output_f,
                                                     param.origin_latitude,
                                                     param.central_meridian,
                                                     param.false_easting,
                                                     param.false_northing);
            if (temp_error)
            {
              if (temp_error & CYEQ_A_ERROR)
                output->status |= ENGINE_A_ERROR;
              if (temp_error & CYEQ_INV_F_ERROR)
                output->status |= ENGINE_INV_F_ERROR;
              if (temp_error & CYEQ_ORIGIN_LAT_ERROR)
                output->status |= ENGINE_ORIGIN_LAT_ERROR;
              if (temp_error & CYEQ_CENT_MER_ERROR)
                output->status |= ENGINE_CENT_MER_ERROR;
            }
            else
            {
              Cylindrical_Equal_Area_Tuple coord;
              temp_error = Convert_Geodetic_To_Cyl_Eq_Area(Shifted_Geodetic.latitude,
                                                           Shifted_Geodetic.longitude,
                                                           &(coord.easting),
                                                           &(coord.northing));
              output->coordinates.Cylindrical_Equal_Area = coord;
              if (temp_error & CYEQ_LAT_ERROR)
                output->status |= ENGINE_LAT_ERROR;
              if (temp_error & CYEQ_LON_ERROR)
                output->status |= ENGINE_LON_ERROR;
            }
            break;
          }
        case Eckert4:
          {
            Eckert4_Parameters param = output->parameters.Eckert4;
            temp_error = Set_Eckert4_Parameters(output_a,output_f,
                                                param.central_meridian,
                                                param.false_easting,
                                                param.false_northing);
            if (temp_error)
            {
              if (temp_error & ECK4_A_ERROR)
                output->status |= ENGINE_A_ERROR;
              if (temp_error & ECK4_INV_F_ERROR)
                output->status |= ENGINE_INV_F_ERROR;
              if (temp_error & ECK4_CENT_MER_ERROR)
                output->status |= ENGINE_CENT_MER_ERROR;
            }
            else
            {
              Eckert4_Tuple coord;
              temp_error = Convert_Geodetic_To_Eckert4(Shifted_Geodetic.latitude,
                                                       Shifted_Geodetic.longitude,
                                                       &(coord.easting),
                                                       &(coord.northing));
              output->coordinates.Eckert4 = coord;
              if (temp_error & ECK4_LAT_ERROR)
                output->status |= ENGINE_LAT_ERROR;
              if (temp_error & ECK4_LON_ERROR)
                output->status |= ENGINE_LON_ERROR;
            }
            break;
          }
        case Eckert6:
          {
            Eckert6_Parameters param = output->parameters.Eckert6;
            temp_error = Set_Eckert6_Parameters(output_a,output_f,
                                                param.central_meridian,
                                                param.false_easting,
                                                param.false_northing);
            if (temp_error)
            {
              if (temp_error & ECK6_A_ERROR)
                output->status |= ENGINE_A_ERROR;
              if (temp_error & ECK6_INV_F_ERROR)
                output->status |= ENGINE_INV_F_ERROR;
              if (temp_error & ECK6_CENT_MER_ERROR)
                output->status |= ENGINE_CENT_MER_ERROR;
            }
            else
            {
              Eckert6_Tuple coord;
              temp_error = Convert_Geodetic_To_Eckert6(Shifted_Geodetic.latitude,
                                                       Shifted_Geodetic.longitude,
                                                       &(coord.easting),
                                                       &(coord.northing));
              output->coordinates.Eckert6 = coord;
              if (temp_error & ECK6_LAT_ERROR)
                output->status |= ENGINE_LAT_ERROR;
              if (temp_error & ECK6_LON_ERROR)
                output->status |= ENGINE_LON_ERROR;
            }
            break;
          }
        case Equidistant_Cylindrical:
          {
            Equidistant_Cylindrical_Parameters param = output->parameters.Equidistant_Cylindrical;
            temp_error = Set_Equidistant_Cyl_Parameters(output_a,output_f,
                                                        param.std_parallel,
                                                        param.central_meridian,
                                                        param.false_easting,
                                                        param.false_northing);
            if (temp_error)
            {
              if (temp_error & EQCY_A_ERROR)
                output->status |= ENGINE_A_ERROR;
              if (temp_error & EQCY_INV_F_ERROR)
                output->status |= ENGINE_INV_F_ERROR;
              if (temp_error & EQCY_STDP_ERROR)
                output->status |= ENGINE_STDP_ERROR;
              if (temp_error & EQCY_CENT_MER_ERROR)
                output->status |= ENGINE_CENT_MER_ERROR;
            }
            else
            {
              Equidistant_Cylindrical_Tuple coord;
              temp_error = Convert_Geodetic_To_Equidistant_Cyl(Shifted_Geodetic.latitude,
                                                               Shifted_Geodetic.longitude,
                                                               &(coord.easting),
                                                               &(coord.northing));
              output->coordinates.Equidistant_Cylindrical = coord;
              if (temp_error & EQCY_LAT_ERROR)
                output->status |= ENGINE_LAT_ERROR;
              if (temp_error & EQCY_LON_ERROR)
                output->status |= ENGINE_LON_ERROR;
            }
            break;
          }
        case Gnomonic:
          {
            Gnomonic_Parameters param = output->parameters.Gnomonic;
            temp_error = Set_Gnomonic_Parameters( output_a,output_f,
                                                  param.origin_latitude,
                                                  param.central_meridian,
                                                  param.false_easting,
                                                  param.false_northing);
            if (temp_error)
            {
              if (temp_error & GNOM_A_ERROR)
                output->status |= ENGINE_A_ERROR;
              if (temp_error & GNOM_INV_F_ERROR)
                output->status |= ENGINE_INV_F_ERROR;
              if (temp_error & GNOM_ORIGIN_LAT_ERROR)
                output->status |= ENGINE_ORIGIN_LAT_ERROR;
              if (temp_error & GNOM_CENT_MER_ERROR)
                output->status |= ENGINE_CENT_MER_ERROR;
            }
            else
            {
              Gnomonic_Tuple coord;
              temp_error = Convert_Geodetic_To_Gnomonic(Shifted_Geodetic.latitude,
                                                        Shifted_Geodetic.longitude,
                                                        &(coord.easting),
                                                        &(coord.northing));
              output->coordinates.Gnomonic = coord;
              if (temp_error & GNOM_LAT_ERROR)
                output->status |= ENGINE_LAT_ERROR;
              if (temp_error & GNOM_LON_ERROR)
                output->status |= ENGINE_LON_ERROR;
            }
            break;
          }
        case Lambert_Conformal_Conic_1:
          {
            Lambert_Conformal_Conic_1_Parameters param = output->parameters.Lambert_Conformal_Conic_1;
            temp_error = Set_Lambert_1_Parameters(output_a,output_f,
                                                  param.origin_latitude,
                                                  param.central_meridian,
                                                  param.false_easting,
                                                  param.false_northing,
                                                  param.scale_factor);
            if (temp_error)
            {
              if (temp_error & LAMBERT_1_A_ERROR)
                output->status |= ENGINE_A_ERROR;
              if (temp_error & LAMBERT_1_INV_F_ERROR)
                output->status |= ENGINE_INV_F_ERROR;
              if (temp_error & LAMBERT_1_ORIGIN_LAT_ERROR)
                output->status |= ENGINE_ORIGIN_LAT_ERROR;
              if (temp_error & LAMBERT_1_CENT_MER_ERROR)
                output->status |= ENGINE_CENT_MER_ERROR;
              if (temp_error & LAMBERT_1_SCALE_FACTOR_ERROR)
                output->status |= ENGINE_SCALE_FACTOR_ERROR;
            }
            else
            {
              Lambert_Conformal_Conic_1_Tuple coord;
              temp_error = Convert_Geodetic_To_Lambert_1(Shifted_Geodetic.latitude,
                                                         Shifted_Geodetic.longitude,
                                                         &(coord.easting),
                                                         &(coord.northing));
              output->coordinates.Lambert_Conformal_Conic_1 = coord;
              if (temp_error & LAMBERT_1_LAT_ERROR)
                output->status |= ENGINE_LAT_ERROR;
              if (temp_error & LAMBERT_1_LON_ERROR)
                output->status |= ENGINE_LON_ERROR;
            }
            break;
          }
        case Lambert_Conformal_Conic:
          {
            Lambert_Conformal_Conic_Parameters param = output->parameters.Lambert_Conformal_Conic;
            temp_error = Set_Lambert_Parameters(output_a,output_f,
                                                param.origin_latitude,
                                                param.central_meridian,
                                                param.std_parallel_1,
                                                param.std_parallel_2,
                                                param.false_easting,
                                                param.false_northing);
            if (temp_error)
            {
              if (temp_error & LAMBERT_A_ERROR)
                output->status |= ENGINE_A_ERROR;
              if (temp_error & LAMBERT_INV_F_ERROR)
                output->status |= ENGINE_INV_F_ERROR;
              if (temp_error & LAMBERT_SECOND_STDP_ERROR)
                output->status |= ENGINE_SECOND_STDP_ERROR;
              if (temp_error & LAMBERT_FIRST_STDP_ERROR)
                output->status |= ENGINE_FIRST_STDP_ERROR;
              if (temp_error & LAMBERT_FIRST_SECOND_ERROR)
                output->status |= ENGINE_FIRST_SECOND_ERROR;
              if (temp_error & LAMBERT_HEMISPHERE_ERROR)
                output->status |= ENGINE_HEMISPHERE_ERROR;
              if (temp_error & LAMBERT_ORIGIN_LAT_ERROR)
                output->status |= ENGINE_ORIGIN_LAT_ERROR;
              if (temp_error & LAMBERT_CENT_MER_ERROR)
                output->status |= ENGINE_CENT_MER_ERROR;
            }
            else
            {
              Lambert_Conformal_Conic_Tuple coord;
              temp_error = Convert_Geodetic_To_Lambert(Shifted_Geodetic.latitude,
                                                       Shifted_Geodetic.longitude,
                                                       &(coord.easting),
                                                       &(coord.northing));
              output->coordinates.Lambert_Conformal_Conic = coord;
              if (temp_error & LAMBERT_LAT_ERROR)
                output->status |= ENGINE_LAT_ERROR;
              if (temp_error & LAMBERT_LON_ERROR)
                output->status |= ENGINE_LON_ERROR;
            }
            break;
          }
        case Local_Cartesian:
          {
            Local_Cartesian_Parameters param = output->parameters.Local_Cartesian;
            temp_error = Set_Local_Cartesian_Parameters(output_a,output_f,
                                                        param.origin_latitude,
                                                        param.origin_longitude,
                                                        param.origin_height,
                                                        param.orientation);
            if (temp_error)
            {
              if (temp_error & LOCCART_A_ERROR)
                output->status |= ENGINE_A_ERROR;
              if (temp_error & LOCCART_INV_F_ERROR)
                output->status |= ENGINE_INV_F_ERROR;
              if (temp_error & LOCCART_ORIGIN_LAT_ERROR)
                output->status |= ENGINE_ORIGIN_LAT_ERROR;
              if (temp_error & LOCCART_ORIGIN_LON_ERROR)
                output->status |= ENGINE_ORIGIN_LON_ERROR;
            }
            else
            {
              Local_Cartesian_Tuple coord;
              temp_error = Convert_Geodetic_To_Local_Cartesian(Shifted_Geodetic.latitude,
                                                               Shifted_Geodetic.longitude,
                                                               Shifted_Geodetic.height,
                                                               &(coord.x),
                                                               &(coord.y),
                                                               &(coord.z));
              output->coordinates.Local_Cartesian = coord;
              if (temp_error & LOCCART_LAT_ERROR)
                output->status |= ENGINE_LAT_ERROR;
              if (temp_error & LOCCART_LON_ERROR)
                output->status |= ENGINE_LON_ERROR;
              if (temp_error & LOCCART_ORIGIN_LAT_ERROR)
                output->status |= ENGINE_ORIGIN_LAT_ERROR;
            }
            break;
          }  
        case Mercator:
          {
            Mercator_Parameters param = output->parameters.Mercator;
            temp_error = Set_Mercator_Parameters( output_a,output_f,
                                                  param.origin_latitude,
                                                  param.central_meridian,
                                                  param.false_easting,
                                                  param.false_northing,
                                                  &(output->parameters.Mercator.scale_factor));
            if (temp_error)
            {
              if (temp_error & MERC_A_ERROR)
                output->status |= ENGINE_A_ERROR;
              if (temp_error & MERC_INV_F_ERROR)
                output->status |= ENGINE_INV_F_ERROR;
              if (temp_error & MERC_ORIGIN_LAT_ERROR)
                output->status |= ENGINE_ORIGIN_LAT_ERROR;
              if (temp_error & MERC_CENT_MER_ERROR)
                output->status |= ENGINE_CENT_MER_ERROR;
            }
            else
            {
              Mercator_Tuple coord;
              temp_error = Convert_Geodetic_To_Mercator(Shifted_Geodetic.latitude,
                                                        Shifted_Geodetic.longitude,
                                                        &(coord.easting),
                                                        &(coord.northing));
              output->coordinates.Mercator = coord;
              if (temp_error & MERC_LAT_ERROR)
                output->status |= ENGINE_LAT_ERROR;
              if (temp_error & MERC_LON_ERROR)
                output->status |= ENGINE_LON_ERROR;
            }
            break;
          }
        case MGRS:
          {
            temp_error = Set_MGRS_Parameters(output_a,output_f,
                                            Output_Ellipsoid_Code);
                                             
            if (temp_error)
            {
              if (temp_error & MGRS_A_ERROR)
                output->status |= ENGINE_A_ERROR;
              if (temp_error & MGRS_INV_F_ERROR)
                output->status |= ENGINE_INV_F_ERROR;
            }
            else
            {
              Precision temp_precision = Engine_Precision;
              if (temp_precision < 0)
                temp_precision = (Precision)0;
              if (temp_precision > 5)
                temp_precision = (Precision)5;
              temp_error = Convert_Geodetic_To_MGRS(Shifted_Geodetic.latitude,
                                                    Shifted_Geodetic.longitude,
                                                    temp_precision,
                                                    output->coordinates.MGRS.string);

              if (temp_error & MGRS_LAT_ERROR)
                output->status |= ENGINE_LAT_ERROR;
              if (temp_error & MGRS_LON_ERROR)
                output->status |= ENGINE_LON_ERROR;
            }
            break;
          }
        case Miller_Cylindrical:
          {
            Miller_Cylindrical_Parameters param = output->parameters.Miller_Cylindrical;
            temp_error = Set_Miller_Parameters(output_a,output_f,
                                               param.central_meridian,
                                               param.false_easting,
                                               param.false_northing);
            if (temp_error)
            {
              if (temp_error & MILL_A_ERROR)
                output->status |= ENGINE_A_ERROR;
              if (temp_error & MILL_INV_F_ERROR)
                output->status |= ENGINE_INV_F_ERROR;
              if (temp_error & MILL_CENT_MER_ERROR)
                output->status |= ENGINE_CENT_MER_ERROR;
            }
            else
            {
              Miller_Cylindrical_Tuple coord;
              temp_error = Convert_Geodetic_To_Miller(Shifted_Geodetic.latitude,
                                                      Shifted_Geodetic.longitude,
                                                      &(coord.easting),
                                                      &(coord.northing));
              output->coordinates.Miller_Cylindrical = coord;
              if (temp_error & MILL_LAT_ERROR)
                output->status = ENGINE_LAT_ERROR;
              if (temp_error & MILL_LON_ERROR)
                output->status = ENGINE_LON_ERROR;
            }
            break;
          }
        case Mollweide:
          {
            Mollweide_Parameters param = output->parameters.Mollweide;
            temp_error = Set_Mollweide_Parameters( output_a,output_f,
                                                   param.central_meridian,
                                                   param.false_easting,
                                                   param.false_northing);
            if (temp_error)
            {
              if (temp_error & MOLL_A_ERROR)
                output->status |= ENGINE_A_ERROR;
              if (temp_error & MOLL_INV_F_ERROR)
                output->status |= ENGINE_INV_F_ERROR;
              if (temp_error & MOLL_CENT_MER_ERROR)
                output->status |= ENGINE_CENT_MER_ERROR;
            }
            else
            {
              Mollweide_Tuple coord;
              temp_error = Convert_Geodetic_To_Mollweide(Shifted_Geodetic.latitude,
                                                         Shifted_Geodetic.longitude,
                                                         &(coord.easting),
                                                         &(coord.northing));
              output->coordinates.Mollweide = coord;
              if (temp_error & MOLL_LAT_ERROR)
                output->status |= ENGINE_LAT_ERROR;
              if (temp_error & MOLL_LON_ERROR)
                output->status |= ENGINE_LON_ERROR;
            }
            break;
          }
        case Neys:
          {
            Neys_Parameters param = output->parameters.Neys;
            temp_error = Set_Neys_Parameters(output_a,output_f,
                                             param.origin_latitude,
                                             param.central_meridian,
                                             param.std_parallel_1,
                                             param.false_easting,
                                             param.false_northing);
            if (temp_error)
            {
              if (temp_error & NEYS_A_ERROR)
                output->status |= ENGINE_A_ERROR;
              if (temp_error & NEYS_INV_F_ERROR)
                output->status |= ENGINE_INV_F_ERROR;
              if (temp_error & NEYS_FIRST_STDP_ERROR)
                output->status |= ENGINE_FIRST_STDP_ERROR;
              if (temp_error & NEYS_ORIGIN_LAT_ERROR)
                output->status |= ENGINE_ORIGIN_LAT_ERROR;
              if (temp_error & NEYS_CENT_MER_ERROR)
                output->status |= ENGINE_CENT_MER_ERROR;
            }
            else
            {
              Neys_Tuple coord;
              temp_error = Convert_Geodetic_To_Neys(Shifted_Geodetic.latitude,
                                                    Shifted_Geodetic.longitude,
                                                    &(coord.easting),
                                                    &(coord.northing));
              output->coordinates.Neys = coord;
              if (temp_error & NEYS_LAT_ERROR)
                output->status |= ENGINE_LAT_ERROR;
              if (temp_error & NEYS_LON_ERROR)
                output->status |= ENGINE_LON_ERROR;
            }
            break;
          }
        case NZMG:
          {
            temp_error = Set_NZMG_Parameters(Output_Ellipsoid_Code);
                                             
            if (temp_error)
            {
              if (temp_error & NZMG_ELLIPSOID_ERROR)
                output->status |= ENGINE_ELLIPSOID_CODE_ERROR;
            }
            else
            {
              NZMG_Tuple coord;
              temp_error = Convert_Geodetic_To_NZMG(Shifted_Geodetic.latitude,
                                                    Shifted_Geodetic.longitude,
                                                    &(coord.easting),
                                                    &(coord.northing));
              output->coordinates.NZMG = coord;
              if (temp_error & NZMG_LAT_ERROR)
                output->status |= ENGINE_LAT_ERROR;
              if (temp_error & NZMG_LON_ERROR)
                output->status |= ENGINE_LON_ERROR;
            }
            break;
          }
        case Oblique_Mercator:
          {
            Oblique_Mercator_Parameters param = output->parameters.Oblique_Mercator;
            temp_error = Set_Oblique_Mercator_Parameters(output_a,output_f,
                                                         param.origin_latitude,
                                                         param.latitude_1,
                                                         param.longitude_1,
                                                         param.latitude_2,
                                                         param.longitude_2,
                                                         param.false_easting,
                                                         param.false_northing,
                                                         param.scale_factor);
            if (temp_error)
            {
              if (temp_error & OMERC_A_ERROR)
                output->status |= ENGINE_A_ERROR;
              if (temp_error & OMERC_INV_F_ERROR)
                output->status |= ENGINE_INV_F_ERROR;
              if (temp_error & OMERC_ORIGIN_LAT_ERROR)
                output->status |= ENGINE_ORIGIN_LAT_ERROR;
              if (temp_error & OMERC_LAT1_ERROR)
                output->status |= ENGINE_LAT1_ERROR;
              if (temp_error & OMERC_LON1_ERROR)
                output->status |= ENGINE_LON1_ERROR;
              if (temp_error & OMERC_LAT2_ERROR)
                output->status |= ENGINE_LAT2_ERROR;
              if (temp_error & OMERC_LON2_ERROR)
                output->status |= ENGINE_LON2_ERROR;
              if (temp_error & OMERC_LAT1_LAT2_ERROR)
                output->status |= ENGINE_LAT1_LAT2_ERROR;
              if (temp_error & OMERC_DIFF_HEMISPHERE_ERROR)
                output->status |= ENGINE_HEMISPHERE_ERROR;
              if (temp_error & OMERC_SCALE_FACTOR_ERROR)
                output->status |= ENGINE_SCALE_FACTOR_ERROR;
            }
            else
            {
              Oblique_Mercator_Tuple coord;
              temp_error = Convert_Geodetic_To_Oblique_Mercator(Shifted_Geodetic.latitude,
                                                                Shifted_Geodetic.longitude,
                                                                &(coord.easting),
                                                                &(coord.northing));
              output->coordinates.Oblique_Mercator = coord;
              if (temp_error & OMERC_LAT_ERROR)
                output->status |= ENGINE_LAT_ERROR;
              if (temp_error & OMERC_LON_ERROR)
                output->status |= ENGINE_LON_ERROR;
              if (temp_error & OMERC_LON_WARNING)
                output->status |= ENGINE_LON_WARNING;
            }
            break;
          }
        case Orthographic:
          {
            Orthographic_Parameters param = output->parameters.Orthographic;
            temp_error = Set_Orthographic_Parameters( output_a,output_f,
                                                      param.origin_latitude,
                                                      param.central_meridian,
                                                      param.false_easting,
                                                      param.false_northing);
            if (temp_error)
            {
              if (temp_error & ORTH_A_ERROR)
                output->status |= ENGINE_A_ERROR;
              if (temp_error & ORTH_INV_F_ERROR)
                output->status |= ENGINE_INV_F_ERROR;
              if (temp_error & ORTH_ORIGIN_LAT_ERROR)
                output->status |= ENGINE_ORIGIN_LAT_ERROR;
              if (temp_error & ORTH_CENT_MER_ERROR)
                output->status |= ENGINE_CENT_MER_ERROR;
            }
            else
            {
              Orthographic_Tuple coord;
              temp_error = Convert_Geodetic_To_Orthographic(Shifted_Geodetic.latitude,
                                                            Shifted_Geodetic.longitude,
                                                            &(coord.easting),
                                                            &(coord.northing));
              output->coordinates.Orthographic = coord;
              if (temp_error & ORTH_LAT_ERROR)
                output->status |= ENGINE_LAT_ERROR;
              if (temp_error & ORTH_LON_ERROR)
                output->status |= ENGINE_LON_ERROR;
              if (temp_error & ORTH_RADIUS_ERROR)
                output->status |= ENGINE_RADIUS_ERROR;
            }
            break;
          }
        case Polar_Stereo:
          {
            Polar_Stereo_Parameters param = output->parameters.Polar_Stereo;
            Polar_Stereo_Tuple *coord;
            coord = &(output->coordinates.Polar_Stereo);
            temp_error = Set_Polar_Stereographic_Parameters( output_a,output_f,
                                                             param.latitude_of_true_scale,
                                                             param.longitude_down_from_pole,
                                                             param.false_easting,
                                                             param.false_northing);
            if (temp_error)
            {
              if (temp_error & POLAR_A_ERROR)
                output->status |= ENGINE_A_ERROR;
              if (temp_error & POLAR_INV_F_ERROR)
                output->status |= ENGINE_INV_F_ERROR;
              if (temp_error & POLAR_ORIGIN_LAT_ERROR)
                output->status |= ENGINE_ORIGIN_LAT_ERROR;
            }
            else
            {
              Polar_Stereo_Tuple coord;
              temp_error = Convert_Geodetic_To_Polar_Stereographic(Shifted_Geodetic.latitude,
                                                                   Shifted_Geodetic.longitude,
                                                                   &(coord.easting),
                                                                   &(coord.northing));
              output->coordinates.Polar_Stereo = coord;
              if (temp_error & POLAR_LAT_ERROR)
                output->status |= ENGINE_LAT_ERROR;
              if (temp_error & POLAR_LON_ERROR)
                output->status |= ENGINE_LON_ERROR;
              if (temp_error & POLAR_RADIUS_ERROR)
                output->status |= ENGINE_RADIUS_ERROR;
            }
            break;
          }
        case Polyconic:
          {
            Polyconic_Parameters param = output->parameters.Polyconic;
            temp_error = Set_Polyconic_Parameters( output_a,output_f,
                                                   param.origin_latitude,
                                                   param.central_meridian,
                                                   param.false_easting,
                                                   param.false_northing);
            if (temp_error)
            {
              if (temp_error & POLY_A_ERROR)
                output->status |= ENGINE_A_ERROR;
              if (temp_error & POLY_INV_F_ERROR)
                output->status |= ENGINE_INV_F_ERROR;
              if (temp_error & POLY_ORIGIN_LAT_ERROR)
                output->status |= ENGINE_ORIGIN_LAT_ERROR;
              if (temp_error & POLY_CENT_MER_ERROR)
                output->status |= ENGINE_CENT_MER_ERROR;
            }
            else
            {
              Polyconic_Tuple coord;
              temp_error = Convert_Geodetic_To_Polyconic(Shifted_Geodetic.latitude,
                                                         Shifted_Geodetic.longitude,
                                                         &(coord.easting),
                                                         &(coord.northing));
              output->coordinates.Polyconic = coord;
              if (temp_error & POLY_LAT_ERROR)
                output->status |= ENGINE_LAT_ERROR;
              if (temp_error & POLY_LON_ERROR)
                output->status |= ENGINE_LON_ERROR;
              if (temp_error & POLY_LON_WARNING)
                output->status |= ENGINE_LON_WARNING;
            }
            break;
          }
        case Sinusoidal:
          {
            Sinusoidal_Parameters param = output->parameters.Sinusoidal;
            Sinusoidal_Tuple *coord;
            coord = &(output->coordinates.Sinusoidal);
            temp_error = Set_Sinusoidal_Parameters( output_a,output_f,
                                                    param.central_meridian,
                                                    param.false_easting,
                                                    param.false_northing);
            if (temp_error)
            {
              if (temp_error & SINU_A_ERROR)
                output->status |= ENGINE_A_ERROR;
              if (temp_error & SINU_INV_F_ERROR)
                output->status |= ENGINE_INV_F_ERROR;
              if (temp_error & SINU_CENT_MER_ERROR)
                output->status |= ENGINE_CENT_MER_ERROR;
            }
            else
            {
              Sinusoidal_Tuple coord;
              temp_error = Convert_Geodetic_To_Sinusoidal(Shifted_Geodetic.latitude,
                                                          Shifted_Geodetic.longitude,
                                                          &(coord.easting),
                                                          &(coord.northing));
              output->coordinates.Sinusoidal = coord;
              if (temp_error & SINU_LAT_ERROR)
                output->status |= ENGINE_LAT_ERROR;
              if (temp_error & SINU_LON_ERROR)
                output->status |= ENGINE_LON_ERROR;
            }
            break;
          }
        case Stereographic:
          {
            Stereographic_Parameters param = output->parameters.Stereographic;
            temp_error = Set_Stereographic_Parameters( output_a,output_f,
                                                   param.origin_latitude,
                                                   param.central_meridian,
                                                   param.false_easting,
                                                   param.false_northing);
            if (temp_error)
            {
              if (temp_error & STEREO_A_ERROR)
                output->status |= ENGINE_A_ERROR;
              if (temp_error & STEREO_INV_F_ERROR)
                output->status |= ENGINE_INV_F_ERROR;
              if (temp_error & STEREO_ORIGIN_LAT_ERROR)
                output->status |= ENGINE_ORIGIN_LAT_ERROR;
              if (temp_error & STEREO_CENT_MER_ERROR)
                output->status |= ENGINE_CENT_MER_ERROR;
            }
            else
            {
              Stereographic_Tuple coord;
              temp_error = Convert_Geodetic_To_Stereographic(Shifted_Geodetic.latitude,
                                                         Shifted_Geodetic.longitude,
                                                         &(coord.easting),
                                                         &(coord.northing));
              output->coordinates.Stereographic = coord;
              if (temp_error & STEREO_LAT_ERROR)
                output->status |= ENGINE_LAT_ERROR;
              if (temp_error & STEREO_LON_ERROR)
                output->status |= ENGINE_LON_ERROR;
            }
            break;
          }
        case Transverse_Cylindrical_Equal_Area:
          {
            Transverse_Cylindrical_Equal_Area_Parameters param = output->parameters.Transverse_Cylindrical_Equal_Area;
            temp_error = Set_Trans_Cyl_Eq_Area_Parameters( output_a,output_f,
                                                           param.origin_latitude,
                                                           param.central_meridian,
                                                           param.false_easting,
                                                           param.false_northing,
                                                           param.scale_factor);
            if (temp_error)
            {
              if (temp_error & TCEA_A_ERROR)
                output->status |= ENGINE_A_ERROR;
              if (temp_error & TCEA_INV_F_ERROR)
                output->status |= ENGINE_INV_F_ERROR;
              if (temp_error & TCEA_ORIGIN_LAT_ERROR)
                output->status |= ENGINE_ORIGIN_LAT_ERROR;
              if (temp_error & TCEA_CENT_MER_ERROR)
                output->status |= ENGINE_CENT_MER_ERROR;
              if (temp_error & TCEA_SCALE_FACTOR_ERROR)
                output->status |= ENGINE_SCALE_FACTOR_ERROR;
            }
            else
            {
              Transverse_Cylindrical_Equal_Area_Tuple coord;
              temp_error = Convert_Geodetic_To_Trans_Cyl_Eq_Area(Shifted_Geodetic.latitude,
                                                                 Shifted_Geodetic.longitude,
                                                                 &(coord.easting),
                                                                 &(coord.northing));
              output->coordinates.Transverse_Cylindrical_Equal_Area = coord;
              if (temp_error & TCEA_LAT_ERROR)
                output->status |= ENGINE_LAT_ERROR;
              if (temp_error & TCEA_LON_ERROR)
                output->status |= ENGINE_LON_ERROR;
              if (temp_error & TCEA_LON_WARNING)
                output->status |= ENGINE_LON_WARNING;

            }
            break;
          }
        case Transverse_Mercator:
          {
            Transverse_Mercator_Parameters param = output->parameters.Transverse_Mercator;
            temp_error = Set_Transverse_Mercator_Parameters( output_a,output_f,
                                                             param.origin_latitude,
                                                             param.central_meridian,
                                                             param.false_easting,
                                                             param.false_northing,
                                                             param.scale_factor);
            if (temp_error)
            {
              if (temp_error & TRANMERC_A_ERROR)
                output->status |= ENGINE_A_ERROR;
              if (temp_error & TRANMERC_INV_F_ERROR)
                output->status |= ENGINE_INV_F_ERROR;
              if (temp_error & TRANMERC_ORIGIN_LAT_ERROR)
                output->status |= ENGINE_ORIGIN_LAT_ERROR;
              if (temp_error & TRANMERC_CENT_MER_ERROR)
                output->status |= ENGINE_CENT_MER_ERROR;
              if (temp_error & TRANMERC_SCALE_FACTOR_ERROR)
                output->status |= ENGINE_SCALE_FACTOR_ERROR;
            }
            else
            {
              Transverse_Mercator_Tuple coord;
              temp_error = Convert_Geodetic_To_Transverse_Mercator(Shifted_Geodetic.latitude,
                                                                   Shifted_Geodetic.longitude,
                                                                   &(coord.easting),
                                                                   &(coord.northing));
              output->coordinates.Transverse_Mercator = coord;
              if (temp_error & TRANMERC_LAT_ERROR)
                output->status |= ENGINE_LAT_ERROR;
              if (temp_error & TRANMERC_LON_ERROR)
                output->status |= ENGINE_LON_ERROR;
              if (temp_error & TRANMERC_LON_WARNING)
                output->status |= ENGINE_LON_WARNING;
            }
            break;
          }
        case UPS:
          {
            temp_error = Set_UPS_Parameters(output_a,output_f);
            if (temp_error)
            {
              if (temp_error & UPS_A_ERROR)
                output->status |= ENGINE_A_ERROR;
              if (temp_error & UPS_INV_F_ERROR)
                output->status |= ENGINE_INV_F_ERROR;
            }
            else
            {
              UPS_Tuple coord;
              temp_error = Convert_Geodetic_To_UPS(Shifted_Geodetic.latitude,
                                                   Shifted_Geodetic.longitude,
                                                   &(coord.hemisphere),
                                                   &(coord.easting),
                                                   &(coord.northing));
              output->coordinates.UPS = coord;
              if (temp_error & UPS_LAT_ERROR)
                output->status = ENGINE_LAT_ERROR;
              if (temp_error & UPS_LON_ERROR)
                output->status = ENGINE_LON_ERROR;
              break;
            }
          }
        case UTM:
          {
            UTM_Parameters param = output->parameters.UTM;
            if (param.override && !param.zone)
              param.zone = output->coordinates.UTM.zone;
            else
            {
              if (!param.override)
                param.zone = 0;
            }
            temp_error = Set_UTM_Parameters(output_a,output_f, 
                                            param.zone);
            if (temp_error)
            {
              if (temp_error & UTM_A_ERROR)
                output->status |= ENGINE_A_ERROR;
              if (temp_error & UTM_INV_F_ERROR)
                output->status |= ENGINE_INV_F_ERROR;
              if (temp_error & UTM_ZONE_OVERRIDE_ERROR)
                output->status |= ENGINE_ZONE_OVERRIDE_ERROR;
            }
            else
            {
              UTM_Tuple coord;
              temp_error = Convert_Geodetic_To_UTM(Shifted_Geodetic.latitude,
                                                   Shifted_Geodetic.longitude,
                                                   &(coord.zone),
                                                   &(coord.hemisphere),
                                                   &(coord.easting),
                                                   &(coord.northing));
              output->coordinates.UTM = coord;
              if (temp_error & UTM_LAT_ERROR)
                output->status |= ENGINE_LAT_ERROR;
              if (temp_error & UTM_LON_ERROR)
                output->status |= ENGINE_LON_ERROR;
              if (temp_error & UTM_ZONE_OVERRIDE_ERROR)
                output->status |= ENGINE_ZONE_ERROR;
              if (temp_error & UTM_EASTING_ERROR)
                output->status |= ENGINE_EASTING_ERROR;
              if (temp_error & UTM_NORTHING_ERROR)
                output->status |= ENGINE_NORTHING_ERROR;
            }
            break;
          }
        case Van_der_Grinten:
          {
            Van_der_Grinten_Parameters param = output->parameters.Van_der_Grinten;
            temp_error = Set_Van_der_Grinten_Parameters( output_a,output_f,
                                                         param.central_meridian,
                                                         param.false_easting,
                                                         param.false_northing);
            if (temp_error)
            {
              if (temp_error & GRIN_A_ERROR)
                output->status |= ENGINE_A_ERROR;
              if (temp_error & GRIN_INV_F_ERROR)
                output->status |= ENGINE_INV_F_ERROR;
              if (temp_error & GRIN_CENT_MER_ERROR)
                output->status |= ENGINE_CENT_MER_ERROR;
            }
            else
            {
              Van_der_Grinten_Tuple coord;
              temp_error = Convert_Geodetic_To_Van_der_Grinten(Shifted_Geodetic.latitude,
                                                               Shifted_Geodetic.longitude,
                                                               &(coord.easting),
                                                               &(coord.northing));
              output->coordinates.Van_der_Grinten = coord;
              if (temp_error & GRIN_LAT_ERROR)
                output->status |= ENGINE_LAT_ERROR;
              if (temp_error & GRIN_LON_ERROR)
                output->status |= ENGINE_LON_ERROR;
              if (temp_error & GRIN_RADIUS_ERROR)
                output->status |= ENGINE_RADIUS_ERROR;
            }
            break;
          }
        }           /* switch (output->type) */
        if ((output->status) && 
            (output->status != ENGINE_DATUM_WARNING) &&
            (output->status != ENGINE_LON_WARNING))
          error_code |= ENGINE_OUTPUT_ERROR;
        if (output->status == ENGINE_LON_WARNING)
          error_code |= ENGINE_OUTPUT_WARNING;
        if ((input->status) || (output->status))
        { /* if a distortion warning occurs, standard errors are unknown */
          ce90 = -1.0;
          le90 = -1.0;
          se90 = -1.0;
        }
      }
    } /* if (!special) */
  } /* if (!error_code) */
  return (error_code);
} /* Convert */


long Get_Conversion_Errors
( const File_or_Interactive State,
  double *CE90,
  double *LE90,
  double *SE90)
/*
 *  The function Get_Conversion_Errors returns the estimated errors in the location
 *  the most recently converted location
 *  State  : Indicates whether the datum is to be used for interactive 
 *           or file processing                                    (input)
 *  CE90   : 90% circular (horizontal) error, in meters            (output)
 *  LE90   : 90% linear (vertical) error, in meters                (output)
 *  SE90   : 90% spherical error, in meters                        (output)
 */
{ /* Get_Conversion_Errors */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    *CE90 = ce90;
    *LE90 = le90;
    *SE90 = se90;
  }
  return (error_code);
} /* Get_Conversion_Errors */


long Set_Conversion_Errors
( const File_or_Interactive State,
  double CE90,
  double LE90,
  double SE90)
/*
 *  The function Set_Conversion_Errors sets the estimated errors 
 *  State  : Indicates whether the datum is to be used for interactive 
 *           or file processing                                    (input)
 *  CE90   : 90% circular (horizontal) error, in meters            (input)
 *  LE90   : 90% linear (vertical) error, in meters                (input)
 *  SE90   : 90% spherical error, in meters                        (input)
 */
{ /* Set_Conversion_Errors */
  long error_code = ENGINE_NO_ERROR;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    ce90 = CE90;
    le90 = LE90;
    se90 = SE90;
  }
  return (error_code);
} /* Set_Conversion_Errors */


long Get_Conversion_Status
( const File_or_Interactive State,
  const Input_or_Output Direction,
  long  *Conversion_Status )
/*
 *  The function Get_Conversion_Status returns the current status for the specified state..
 *  State      : Indicates whether the datum is to be used for interactive
 *               or file processing                                    (input)
 *  Direction  : Indicates whether the datum is to be used for input or
 *               output                                                (input)
 *  Conversion_Status : The current status for the specified state     (output)
 */
{ /* Get_Conversion_Status */
  long error_code = ENGINE_NO_ERROR;

  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    *Conversion_Status = CS_State[State][Direction].status;
  }
  return (error_code);
} /* Get_Conversion_Status */



long Get_Conversion_Status_String
( const File_or_Interactive State,
  const Input_or_Output     Direction,
  char  *Separator,
  char  *String)
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
{ /* Get_Conversion_Status_String */
  long error_code = ENGINE_NO_ERROR;
  char   *in_out;
  long Error_Code;
  Coordinate_Type System;
  if (!Engine_Initialized)
    error_code |= ENGINE_NOT_INITIALIZED;
  if (!Valid_Direction(Direction))
    error_code |= ENGINE_INVALID_DIRECTION;
  if (!Valid_State(State))
    error_code |= ENGINE_INVALID_STATE;
  if (!error_code)
  {
    if (Direction == Input)
      in_out = "Input";
    else
      in_out = "Output";
    Get_Coordinate_System(State, Direction, &System);
    Get_Conversion_Status(State, Direction, &Error_Code);
    switch (System)
    {
    case Geocentric:
      {
        sprintf(String,"%s%s%s%s",in_out," Geocentric Coordinates:",Separator,Separator);
        break;
      }
    case Geodetic:
      {
        sprintf(String,"%s%s%s%s",in_out," Geodetic Coordinates:",Separator,Separator);
        break;
      }
    case GEOREF:
      {
        sprintf(String,"%s%s%s%s",in_out," GEOREF Coordinates:",Separator,Separator);
        break;
      }
    case Albers_Equal_Area_Conic:
      {
        sprintf(String,"%s%s%s%s",in_out," Albers Equal Area Conic Projection:",Separator,Separator);
        break;
      }
    case Azimuthal_Equidistant:
      {
        sprintf(String,"%s%s%s%s",in_out," Azimuthal Equidistant Projection:",Separator,Separator);
        break;
      }
    case BNG:
      {
        sprintf(String,"%s%s%s%s",in_out," British National Grid Coordinates:",Separator,Separator);
        break;
      }
    case Bonne:
      {
        sprintf(String,"%s%s%s%s",in_out," Bonne Projection:",Separator,Separator);
        break;
      }
    case Cassini:
      {
        sprintf(String,"%s%s%s%s",in_out," Cassini Projection:",Separator,Separator);
        break;
      }
    case Cylindrical_Equal_Area:
      {
        sprintf(String,"%s%s%s%s",in_out," Cylindrical Equal Area Projection:",Separator,Separator);
        break;
      }
    case Eckert4:
      {
        sprintf(String,"%s%s%s%s",in_out," Eckert IV Projection:",Separator,Separator);
        break;
      }
    case Eckert6:
      {
        sprintf(String,"%s%s%s%s",in_out," Eckert VI Projection:",Separator,Separator);
        break;
      }
    case Equidistant_Cylindrical:
      {
        sprintf(String,"%s%s%s%s",in_out," Equidistant Cylindrical Projection:",Separator,Separator);
        break;
      }
    case Gnomonic:
      {
        sprintf(String,"%s%s%s%s",in_out," Gnomonic Projection:",Separator,Separator);
        break;
      }
    case Lambert_Conformal_Conic_1:
      {
        sprintf(String,"%s%s%s%s",in_out," Lambert Conformal Conic (1 parallel) Projection:",Separator,Separator);
        break;
      }
    case Lambert_Conformal_Conic:
      {
        sprintf(String,"%s%s%s%s",in_out," Lambert Conformal Conic (2 parallel) Projection:",Separator,Separator);
        break;
      }
    case Local_Cartesian:
      {
        sprintf(String,"%s%s%s%s",in_out," Local Cartesian Coordinates:",Separator,Separator);
        break;
      }
    case Mercator:
      {
        sprintf(String,"%s%s%s%s",in_out," Mercator Projection:",Separator,Separator);
        break;
      }
    case MGRS:
      {
        sprintf(String,"%s%s%s%s",in_out," MGRS Coordinates:",Separator,Separator);
        break;
      }
    case Miller_Cylindrical:
      {
        sprintf(String,"%s%s%s%s",in_out," Miller Cylindrical Projection:",Separator,Separator);
        break;
      }
    case Mollweide:
      {
        sprintf(String,"%s%s%s%s",in_out," Mollweide Projection:",Separator,Separator);
        break;
      }
    case Neys:
      {
        sprintf(String,"%s%s%s%s",in_out," Ney's Projection:",Separator,Separator);
        break;
      }
    case NZMG:
      {
        sprintf(String,"%s%s%s%s",in_out," New Zealand Map Grid Projection:",Separator,Separator);
        break;
      }
    case Oblique_Mercator:
      {
        sprintf(String,"%s%s%s%s",in_out," Oblique Mercator Projection:",Separator,Separator);
        break;
      }
    case Orthographic:
      {
        sprintf(String,"%s%s%s%s",in_out," Orthographic Projection:",Separator,Separator);
        break;
      }
    case Polar_Stereo:
      {
        sprintf(String,"%s%s%s%s",in_out," Polar Stereographic Projection:",Separator,Separator);
        break;
      }
    case Polyconic:
      {
        sprintf(String,"%s%s%s%s",in_out," Polyconic Projection:",Separator,Separator);
        break;
      }
    case Sinusoidal:
      {
        sprintf(String,"%s%s%s%s",in_out," Sinusoidal Projection:",Separator,Separator);
        break;
      }
    case Stereographic:
      {
        sprintf(String,"%s%s%s%s",in_out," Stereographic Projection:",Separator,Separator);
        break;
      }
    case Transverse_Cylindrical_Equal_Area:
      {
        sprintf(String,"%s%s%s%s",in_out," Transverse Cylindrical Equal Area Projection:",Separator,Separator);
        break;
      }
    case Transverse_Mercator:
      {
        sprintf(String,"%s%s%s%s",in_out," Transverse Mercator Projection:",Separator,Separator);
        break;
      }
    case UPS:
      {
        sprintf(String,"%s%s%s%s",in_out," UPS Coordinates:",Separator,Separator);
        break;
      }
    case UTM:
      {
        sprintf(String,"%s%s%s%s",in_out," UTM Coordinates:",Separator,Separator);
        break;
      }
    case Van_der_Grinten:
      {
        sprintf(String,"%s%s%s%s",in_out," Van der Grinten Projection:",Separator,Separator);
        break;
      }
    } /* switch */
    if (Error_Code & ENGINE_DATUM_WARNING)
    {
      strcat(String,"Location is not within valid domain for current datum");
      strcat(String,Separator);
    }
    if (Error_Code & ENGINE_LON_WARNING)
    {
      strcat(String,"Longitude is too far from Central Meridian, distortion may be significant");
      strcat(String,Separator);
    }
    if (Error_Code & ENGINE_A_ERROR)
    {
      strcat(String,"Ellipsoid semi-major axis must be greater than zero");
      strcat(String,Separator);
    }
    if (Error_Code & ENGINE_INV_F_ERROR)
    {
      strcat(String,"Inverse flattening must be between 250 and 350");
      strcat(String,Separator);
    }
    if (Error_Code & ENGINE_ORIGIN_LAT_ERROR)
    {
      strcat(String,"Origin Latitude (or Latitude of True Scale) out of range");
      strcat(String,Separator);
    }
    if (Error_Code & ENGINE_ORIGIN_LON_ERROR)
    {
      strcat(String,"Origin Longitude (or Longitude Down from Pole) out of range");
      strcat(String,Separator);
    }
    if (Error_Code & ENGINE_CENT_MER_ERROR)
    {
      strcat(String,"Central Meridian out of range");
      strcat(String,Separator);
    }
    if (Error_Code & ENGINE_RADIUS_ERROR)
    {
      strcat(String,"Easting/Northing too far from center of projection");
      strcat(String,Separator);
    }
    if (Error_Code & ENGINE_PROJECTION_ERROR)
    {
      strcat(String,"Point projects into a circle");
      strcat(String,Separator);
    }
    if (Error_Code & ENGINE_HEMISPHERE_ERROR)
    {
      switch(System)
      {
        case Albers_Equal_Area_Conic:
        case Lambert_Conformal_Conic:
        {
          strcat(String,"Standard Parallels cannot be equal and opposite latitudes");
          break;
        }
        case Oblique_Mercator:
        {
          strcat(String,"Point 1 and Point 2 cannot be in different hemispheres");
          break;
        }
        case MGRS:
        case UPS:
        case UTM:
        {
          strcat(String,"Invalid Hemisphere");
          break;
        }
      }
      strcat(String,Separator);
    }
    if (Error_Code & ENGINE_SCALE_FACTOR_ERROR)
    {
      strcat(String,"Scale Factor out of range");
      strcat(String,Separator);
    }
    if (Error_Code & ENGINE_LAT_ERROR)
    {
      strcat(String,"Latitude out of range");
      strcat(String,Separator);
    }
    if (Error_Code & ENGINE_LAT1_ERROR)
    {
      strcat(String,"Latitude 1 out of range");
      strcat(String,Separator);
    }
    if (Error_Code & ENGINE_LAT2_ERROR)
    {
      strcat(String,"Latitude 2 out of range");
      strcat(String,Separator);
    }
    if (Error_Code & ENGINE_LAT1_LAT2_ERROR)
    {
      strcat(String,"Latitude 1 and Latitude 2 cannot be equal");
      strcat(String,Separator);
    }
    if (Error_Code & ENGINE_LON_ERROR)
    {
      strcat(String,"Longitude out of range");
      strcat(String,Separator);
    }
    if (Error_Code & ENGINE_LON1_ERROR)
    {
      strcat(String,"Longitude 1 out of range");
      strcat(String,Separator);
    }
    if (Error_Code & ENGINE_LON2_ERROR)
    {
      strcat(String,"Longitude 2 out of range");
      strcat(String,Separator);
    }
    if (Error_Code & ENGINE_EASTING_ERROR)
    {
      strcat(String,"Easting/X out of range");
      strcat(String,Separator);
    }
    if (Error_Code & ENGINE_NORTHING_ERROR)
    {
      strcat(String,"Northing/Y out of range");
      strcat(String,Separator);
    }
    if (Error_Code & ENGINE_STDP_ERROR)
    {
      strcat(String,"Invalid Standard Parallel");
      strcat(String,Separator);
    }
    if (Error_Code & ENGINE_FIRST_STDP_ERROR)
    {
      strcat(String,"Invalid 1st Standard Parallel");
      strcat(String,Separator);
    }
    if (Error_Code & ENGINE_SECOND_STDP_ERROR)
    {
      strcat(String,"Invalid 2nd Standard Parallel");
      strcat(String,Separator);
    }
    if (Error_Code & ENGINE_FIRST_SECOND_ERROR)
    {
      strcat(String,"1st & 2nd Standard Parallels cannot both be zero");
      strcat(String,Separator);
    }
    if (Error_Code & ENGINE_ZONE_ERROR)
    {
      strcat(String,"Invalid Zone");
      strcat(String,Separator);
    }
    if (Error_Code & ENGINE_ZONE_OVERRIDE_ERROR)
    {
      strcat(String,"Invalid Zone Override");
      strcat(String,Separator);
    }
    if (Error_Code & ENGINE_STRING_ERROR)
    {
      switch (System)
      {
        case MGRS:
        {
          strcat(String,"Invalid MGRS String");
          break;
        }
        case GEOREF:
        {
          strcat(String,"Invalid GEOREF String");
          break;
        }
        case BNG:
        {
          strcat(String,"Invalid British National Grid String");
          break;
        }
      }
      strcat(String,Separator);
    }
    if (Error_Code & ENGINE_STR_LON_MIN_ERROR)
    {
      strcat(String,"The longitude minute part of the GEOREF string is greater than 60");
      strcat(String,Separator);
    }
    if (Error_Code & ENGINE_STR_LAT_MIN_ERROR)
    {
      strcat(String,"The latitude minute part of the GEOREF string is greater than 60");
      strcat(String,Separator);
    }    
    if (Error_Code & ENGINE_ELLIPSOID_CODE_ERROR)
    {
      switch (System)
      {
        case BNG:
        {
          strcat(String,"Ellipsoid must be Airy");
          break;
        }
        case NZMG:
        {
          strcat(String,"Ellipsoid must be International");
          break;
        }
      }
      strcat(String,Separator);
    }    
    if (Error_Code & ENGINE_INVALID_AREA_ERROR)
    {
      strcat(String,"Coordinates are outside valid area");
      strcat(String,Separator);
    }    
  }
  return (error_code);
} /* Get_Conversion_Status_String */



void Get_Return_Code_String
( long  Error_Code,
  char  *Separator,
  char  *String)
/*
 *  The function Get_Return_Code_String returns a string describing the specified 
 *  engine return code.
 *  Error_Code : Status code returned by engine function               (input) 
 *  Separator  : String to be used to separate individual status messages
 *                                                                     (input)
 *  String     : String describing the current status for the specified state     
 *                                                                     (output)
 */
{ /* Get_Return_Code_String */
  String[0] = 0;
  if (Error_Code & ENGINE_INPUT_WARNING)
  {
    strcat(String,"Warning returned by 1st conversion");
    strcat(String,Separator);
  }
  if (Error_Code & ENGINE_OUTPUT_WARNING)
  {
    strcat(String,"Warning returned by 2st conversion");
    strcat(String,Separator);
  }
  if (Error_Code & ENGINE_INPUT_ERROR)
  {
    strcat(String,"Error returned by 1st conversion");
    strcat(String,Separator);
  }
  if (Error_Code & ENGINE_OUTPUT_ERROR)
  {
    strcat(String,"Error returned by 2nd conversion");
    strcat(String,Separator);
  }
  if (Error_Code & ENGINE_INVALID_TYPE)
  {
    strcat(String,"Invalid coordinate system type");
    strcat(String,Separator);
  }
  if (Error_Code & ENGINE_INVALID_DIRECTION)
  {
    strcat(String,"Invalid direction (input or output)");
    strcat(String,Separator);
  }
  if (Error_Code & ENGINE_INVALID_STATE)
  {
    strcat(String,"Invalid state (interactive or file)");
    strcat(String,Separator);
  }
  if (Error_Code & ENGINE_INVALID_INDEX_ERROR)
  {
    strcat(String,"Index value outside of valid range");
    strcat(String,Separator);
  }
  if (Error_Code & ENGINE_INVALID_CODE_ERROR)
  {
    strcat(String,"Specified code already in use");
    strcat(String,Separator);
  }
  if (Error_Code & ENGINE_NOT_USERDEF_ERROR)
  {
    strcat(String,"Specified code not user defined");
    strcat(String,Separator);
  }
  if (Error_Code & ENGINE_ELLIPSE_IN_USE_ERROR)
  {
    strcat(String,"Ellipsoid is in use by a datum");
    strcat(String,Separator);
  }
  if (Error_Code & ENGINE_ELLIPSOID_OVERFLOW)
  {
    strcat(String,"Ellipsoid table is full");
    strcat(String,Separator);
  }
  if (Error_Code & ENGINE_DATUM_OVERFLOW)
  {
    strcat(String,"3-parameter datum table is full");
    strcat(String,Separator);
  }
  if (Error_Code & ENGINE_DATUM_SIGMA_ERROR)
  {
    strcat(String,"Standard error values must be positive, or -1 if unknown");
    strcat(String,Separator);
  }
  if (Error_Code & ENGINE_DATUM_DOMAIN_ERROR)
  {
    strcat(String,"Invalid local datum domain of validity");
    strcat(String,Separator);
  }
  if (Error_Code & ENGINE_LAT_ERROR)
  {
    strcat(String,"Latitude out of range");
    strcat(String,Separator);
  }
  if (Error_Code & ENGINE_LON_ERROR)
  {
    strcat(String,"Longitude out of range");
    strcat(String,Separator);
  }
  if (Error_Code & ENGINE_A_ERROR)
  {
    strcat(String,"Ellipsoid semi-major axis must be greater than zero");
    strcat(String,Separator);
  }
  if (Error_Code & ENGINE_INV_F_ERROR)
  {
    strcat(String,"Inverse flattening must be between 250 and 350");
    strcat(String,Separator);
  }
  if (Error_Code & ENGINE_DATUM_ERROR)
  {
    strcat(String,"Error returned by Datum module");
    strcat(String,Separator);
  }
  if (Error_Code & ENGINE_ELLIPSOID_ERROR)
  {
    strcat(String,"Error returned by Ellipsoid module");
    strcat(String,Separator);
  }
  if (Error_Code & ENGINE_GEOID_ERROR)
  {
    strcat(String,"Error returned by Geoid module");
    strcat(String,Separator);
  }
} /* Get_Return_Code_String */
