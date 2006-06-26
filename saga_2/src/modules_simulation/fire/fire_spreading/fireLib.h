/*
 *******************************************************************************
 *
 *  fireLib.h
 *
 *  Description
 *      Library of BEHAVE (Andrews 1986) fire behavior algorithms
 *      encapsulated and optimized for fire behavior simulation.
 *
 *  Legalities
 *      Copyright (c) 1996 Collin D. Bevins.
 *      See the file "license.txt" for information on usage and
 *      redistribution of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 *  Description
 *      This header file describes the externally-visible facilities of
 *      the Fire Behavior Library C API.
 *
 *      This file really needs to be split into public and private portions.
 *
 *  History
 *      1996/09/04  Version 1.0.0 release.
 *      1999/03/05  Fixed NNFL07 live SAVR from 1500 to 1550.
 *
 *******************************************************************************
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#ifndef _FIRE_LIB
#define _FIRE_LIB 1

#define FIRELIB_VERSION "1.0"
#define FIRELIB_MAJOR_VERSION 1
#define FIRELIB_MINOR_VERSION 0
#define FIRELIB_PATCH_LEVEL   1

/*
 *------------------------------------------------------------------------------
 * Definitions that allow this header file to be used either with or
 * without ANSI C features like function prototypes.
 *------------------------------------------------------------------------------
 */

#undef _ANSI_ARGS_
#undef CONST
#if ((defined(__STDC__) || defined(SABER)) && !defined(NO_PROTOTYPE)) || defined(__cplusplus)
#   define _USING_PROTOTYPES_ 1
#   define _ANSI_ARGS_(x)       x
#   define CONST const
#   ifdef __cplusplus
#       define VARARGS(first) (first, ...)
#   else
#       define VARARGS(first) ()
#   endif
#else
#   define _ANSI_ARGS_(x)       ()
#   define CONST
#endif

#ifdef __cplusplus
#   define EXTERN extern "C"
#else
#   define EXTERN extern
#endif

/*
 *------------------------------------------------------------------------------
 * Macro to use instead of "void" for arguments that must have type "void *"
 * in ANSI C;  maps them to type "char *" in non-ANSI systems.
 *------------------------------------------------------------------------------
 */

#ifndef VOID
#   ifdef __STDC__
#       define VOID void
#   else
#       define VOID char
#   endif
#endif

/*
 *------------------------------------------------------------------------------
 *  Macro pseudo functions.
 *------------------------------------------------------------------------------
 */

#define Smidgen                 (0.000001)
#define DegreesToRadians(x)     ((x)*0.017453293)
#define RadiansToDegrees(x)     ((x)*57.29577951)
#define IsZero(x)               (fabs(x)<Smidgen)
#define Equal(x,y)              (fabs((x)-(y))<Smidgen)

/*
 *------------------------------------------------------------------------------
 * Firelib return status codes.
 *------------------------------------------------------------------------------
 */

#define  FIRE_STATUS_OK         (0)
#define  FIRE_STATUS_ERROR      (-1)
#define  FIRE_STATUS_EOF        (1)

/*
 *------------------------------------------------------------------------------
 *  Fuel moisture and mass weighting classes.
 *------------------------------------------------------------------------------
 */

#define  FIRE_LIFE_CATS     (2) /* Number of fuel particle life categories */
#define  FIRE_LIFE_DEAD     (0)
#define  FIRE_LIFE_LIVE     (1)

#define  FIRE_SIZE_CLASSES  (6) /* Number of mass weighting classes. */

#define  FIRE_MCLASSES      (6) /* Number of fuel moisture classes. */
#define  FIRE_MCLASS_1HR    (0)
#define  FIRE_MCLASS_10HR   (1)
#define  FIRE_MCLASS_100HR  (2)
#define  FIRE_MCLASS_1000HR (3)
#define  FIRE_MCLASS_HERB   (4)
#define  FIRE_MCLASS_WOOD   (5)

/*
 *------------------------------------------------------------------------------
 *  FuelParticleData structure: fuel particle input and intermediate attributes.
 *------------------------------------------------------------------------------
 */

typedef struct fuelParticleDataStruct
{
    /* INPUT */
    double load;                /* fuel loading                     (lb/sqft) */
    double savr;                /* surface area-to-volume ratio        (1/ft) */
    double dens;                /* particle density                 (lb/cuft) */
    double heat;                /* heat of combustion                (BTU/lb) */
    double stot;                /* total silica content        (fraction odw) */
    double seff;                /* effective silica content    (fraction odw) */
    /* PARTICLE_DEPENDENT */
    double area;                /* surface area */
    double sigma;               /* exp(-138./sigma)                      (dl) */
    /* MODEL-DEPENDENT */
    double awtg;                /* surface area derived weighting factor (dl) */
    double gwtg;                /* size class area weighting factor */
    /* ENVIRONMENT-DEPENDENT */
    double mois;                /* particle moisture content       (fraction) */
    size_t live;                /* life category 0=dead, 1=live               */
    size_t type;                /* type category 0=dead, 1=herb, 2=live woody */
    size_t sizeClass;           /* fuel moisture size class                   */
} FuelParticleData, *FuelParticlePtr, *PartPtr;

#define FIRE_TYPE_DEAD   (1)
#define FIRE_TYPE_HERB   (2)
#define FIRE_TYPE_WOOD   (3)

/* FuelParticleData structure access macros. */

#define Fuel_Live(catalog,model,particle) \
                ((catalog)->modelPtr[(model)]->partPtr[(particle)]->live)

#define Fuel_Type(catalog,model,particle) \
                ((catalog)->modelPtr[(model)]->partPtr[(particle)]->type)

#define Fuel_SizeClass(catalog,model,particle) \
                ((catalog)->modelPtr[(model)]->partPtr[(particle)]->sizeClass)

#define Fuel_Load(catalog,model,particle) \
                ((catalog)->modelPtr[(model)]->partPtr[(particle)]->load)

#define Fuel_Savr(catalog,model,particle) \
                ((catalog)->modelPtr[(model)]->partPtr[(particle)]->savr)

#define Fuel_Heat(catalog,model,particle) \
                ((catalog)->modelPtr[(model)]->partPtr[(particle)]->heat)

#define Fuel_Density(catalog,model,particle) \
                ((catalog)->modelPtr[(model)]->partPtr[(particle)]->dens)

#define Fuel_SiTotal(catalog,model,particle) \
                ((catalog)->modelPtr[(model)]->partPtr[(particle)]->stot)

#define Fuel_SiEffective(catalog,model,particle) \
                ((catalog)->modelPtr[(model)]->partPtr[(particle)]->seff)

#define Fuel_SurfaceArea(catalog,model,particle) \
                ((catalog)->modelPtr[(model)]->partPtr[(particle)]->area)

#define Fuel_AreaWtg(catalog,model,particle) \
                ((catalog)->modelPtr[(model)]->partPtr[(particle)]->awtg)

#define Fuel_SizeAreaWtg(catalog,model,particle) \
                ((catalog)->modelPtr[(model)]->partPtr[(particle)]->gwtg)

#define Fuel_SigmaFactor(catalog,model,particle) \
                ((catalog)->modelPtr[(model)]->partPtr[(particle)]->sigma)

#define Fuel_Moisture(catalog,model,particle) \
                ((catalog)->modelPtr[(model)]->partPtr[(particle)]->mois)

/*
 *------------------------------------------------------------------------------
 *  FuelModelData structure: fuel model bed input attributes.
 *------------------------------------------------------------------------------
 */

typedef struct fuelModelDataStruct
{
    /* Input variables. */
    size_t modelId;             /* fuel model number                          */
    size_t combustion;          /* 0 if combustion not yet calculated         */
    size_t maxParticles;        /* maximum number of FuelParticles            */
    size_t particles;           /* current number of FuelParticles            */
    PartPtr *partPtr;           /* array of pointers to Fuel Particles        */
    char  *name;                /* fuel model short name                      */
    char  *desc;                /* fuel model description text                */
    char  *reserved1;           /* used for alignment                         */
    double depth;               /* fuel bed depth                        (ft) */
    double mext;                /* dead fuel extinction moisture   (fraction) */
    double adjust;              /* spread rate adjustment factor         (dl) */
    /* Combustion intermediates. */
    double awtg[2];             /* dead & live fuel area weighting factors    */
    double rxFactor[2];         /* dead and live fuel rx factors              */
    double fineDead;            /* fine dead fuel ratio                       */
    double liveFactor;          /* live fuel moisture extinction factor       */
    double rhob;                /* fuel bed bulk density                      */
    double taur;                /* residence time                       (min) */
    double propFlux;            /* propagating flux ratio                     */
    double slopeK;              /* slope parameter 'k'                        */
    double windB;               /* wind parameter 'b'                         */
    double windE;               /* wind parameter (ratio**e/c)                */
    double windK;               /* wind parameter (c * ratio**-e)             */
    /* Current environment. */
    double moisture[FIRE_MCLASSES]; /* array of fuel moistures (fraction odw) */
    double windFpm;             /* wind speed                        (ft/min) */
    double windDeg;             /* wind vector         (degrees from upslope) */
    double slope;               /* slope                         (rise/reach) */
    double aspect;              /* aspect (downslope) azimuth  (compass degs) */
    /* Updated by Fire_SpreadNoWindNoSlope() */
    double rxInt;               /* reaction intensity          (BTU/sqft/min) */
    double spread0;             /* no-wind, no-slope spread rate     (ft/min) */
    double hpua;                /* heat per unit area              (BTU/sqft) */
    /* Updated by Fire_SpreadWindSlopeMax() */
    double spreadMax;           /* spread in direction of max spread (ft/min) */
    double azimuthMax;          /* direction of maximum spread      (degrees) */
    double effWind;             /* effective windspeed                        */
    double lwRatio;             /* length-to-width ratio for eff windspeed    */
    double eccentricity;        /* eccentricity of ellipse for eff windspeed  */
    double phiW;                /* wind factor                                */
    double phiS;                /* slope factor                               */
    double phiEw;               /* combined wind-slope factor                 */
    size_t wLimit;              /* wind limit 0=not reached, 1=reached        */
    size_t reserved2;           /* used for alignment                         */
    /* Updated by Fire_SpreadAtAzimuth() */
    double spreadAny;           /* spread rate at arbitrary azimuth  (ft/min) */
    double azimuthAny;          /* direction of arbitrary spread    (degrees) */
    double byrams;              /* fireline intensity              (BTU/ft/s) */
    double flame;               /* flame length                          (ft) */
    double scorch;              /* scorch height                         (ft) */
} FuelModelData, *FuelModelPtr;

/* Fuel model input variable macros. */
#define Fuel_Model(catalog,model) \
                    ((catalog)->modelPtr[(model)]->modelId)

#define Fuel_Name(catalog,model) \
                    ((catalog)->modelPtr[(model)]->name)

#define Fuel_Desc(catalog,model) \
                    ((catalog)->modelPtr[(model)]->desc)

#define Fuel_Depth(catalog,model) \
                    ((catalog)->modelPtr[(model)]->depth)

#define Fuel_Mext(catalog,model) \
                    ((catalog)->modelPtr[(model)]->mext)

#define Fuel_SpreadAdjustment(catalog,model) \
                    ((catalog)->modelPtr[(model)]->adjust)

#define Fuel_CombustionFlag(catalog,model) \
                    ((catalog)->modelPtr[(model)]->combustion)

#define Fuel_MaxParticles(catalog,model) \
                    ((catalog)->modelPtr[(model)]->maxParticles)

#define Fuel_Particles(catalog,model) \
                    ((catalog)->modelPtr[(model)]->particles)

#define Fuel_ParticleArray(catalog,model) \
                    ((catalog)->modelPtr[(model)]->partPtr)

#define Fuel_ParticlePtr(catalog,model,particle) \
                    ((catalog)->modelPtr[(model)]->partPtr[(particle)])

/* Fuel model combustion intermediates macros. */
#define Fuel_LifeAreaWtg(catalog,model,life) \
                    ((catalog)->modelPtr[(model)]->awtg[(life)])

#define Fuel_LifeRxFactor(catalog,model,life) \
                    ((catalog)->modelPtr[(model)]->rxFactor[(life)])

#define Fuel_FineDead(catalog,model) \
                    ((catalog)->modelPtr[(model)]->fineDead)

#define Fuel_LiveMextFactor(catalog,model) \
                    ((catalog)->modelPtr[(model)]->liveFactor)

#define Fuel_BulkDensity(catalog,model) \
                    ((catalog)->modelPtr[(model)]->rhob)

#define Fuel_ResidenceTime(catalog,model) \
                    ((catalog)->modelPtr[(model)]->taur)

#define Fuel_PropFlux(catalog,model) \
                    ((catalog)->modelPtr[(model)]->propFlux)

#define Fuel_SlopeK(catalog,model) \
                    ((catalog)->modelPtr[(model)]->slopeK)

#define Fuel_WindB(catalog,model) \
                    ((catalog)->modelPtr[(model)]->windB)

#define Fuel_WindE(catalog,model) \
                    ((catalog)->modelPtr[(model)]->windE)

#define Fuel_WindK(catalog,model) \
                    ((catalog)->modelPtr[(model)]->windK)

/* Fuel model fire behavior variable macros. */
#define Fuel_RxIntensity(catalog,model) \
                    ((catalog)->modelPtr[(model)]->rxInt)

#define Fuel_Spread0(catalog,model) \
                    ((catalog)->modelPtr[(model)]->spread0)

#define Fuel_HeatPerUnitArea(catalog,model) \
                    ((catalog)->modelPtr[(model)]->hpua)

#define Fuel_SpreadMax(catalog,model) \
                    ((catalog)->modelPtr[(model)]->spreadMax)

#define Fuel_AzimuthMax(catalog,model) \
                    ((catalog)->modelPtr[(model)]->azimuthMax)

#define Fuel_SpreadAny(catalog,model) \
                    ((catalog)->modelPtr[(model)]->spreadAny)

#define Fuel_AzimuthAny(catalog,model) \
                    ((catalog)->modelPtr[(model)]->azimuthAny)

#define Fuel_EffectiveWind(catalog,model) \
                    ((catalog)->modelPtr[(model)]->effWind)

#define Fuel_LwRatio(catalog,model) \
                    ((catalog)->modelPtr[(model)]->lwRatio)

#define Fuel_Eccentricity(catalog,model) \
                    ((catalog)->modelPtr[(model)]->eccentricity)

#define Fuel_PhiWind(catalog,model) \
                    ((catalog)->modelPtr[(model)]->phiW)

#define Fuel_PhiSlope(catalog,model) \
                    ((catalog)->modelPtr[(model)]->phiS)

#define Fuel_PhiEffWind(catalog,model) \
                    ((catalog)->modelPtr[(model)]->phiEw)

#define Fuel_WindLimit(catalog,model) \
                    ((catalog)->modelPtr[(model)]->wLimit)

#define Fuel_ByramsIntensity(catalog,model) \
                    ((catalog)->modelPtr[(model)]->byrams)

#define Fuel_FlameLength(catalog,model) \
                    ((catalog)->modelPtr[(model)]->flame)

#define Fuel_ScorchHeight(catalog,model) \
                    ((catalog)->modelPtr[(model)]->scorch)

/* Fuel model environment variable macros. */
#define Fuel_EnvMoisture(catalog,model,mclass) \
                    ((catalog)->modelPtr[(model)]->moisture[(mclass)])

#define Fuel_WindSpeed(catalog,model) \
                    ((catalog)->modelPtr[(model)]->windFpm)

#define Fuel_WindDir(catalog,model) \
                    ((catalog)->modelPtr[(model)]->windDeg)

#define Fuel_Slope(catalog,model) \
                    ((catalog)->modelPtr[(model)]->slope)

#define Fuel_Aspect(catalog,model) \
                    ((catalog)->modelPtr[(model)]->aspect)

/*
 *------------------------------------------------------------------------------
 *  FuelCatData structure; provides a complete fuel catalog.
 *------------------------------------------------------------------------------
 */

#define FIRE_CATALOG_MAGIC      (19520904L)
#define FIRE_ERROR_BUFFER_SIZE  (1024)

typedef struct fuelCatalogStruct
{
    long      magicCookie;      /* magic cookie for sanity checking           */
    int       status;           /* return status of most recent call          */
    size_t    maxModels;        /* maximum number of models in this catalog   */
    size_t    flameClasses;     /* size of the flame length array             */
    char         *name;         /* name for this catalog instance             */
    char         *error;        /* error message buffer                       */
    FuelModelPtr *modelPtr;     /* array of ModelPtr[maxModels+1]             */
    double       *flamePtr;     /* flame length lookup array                  */
    double        flameStep;    /* size of each flame length table class (ft) */
} FuelCatalogData, *FuelCatalogPtr;

#define FuelCat_MagicCookie(catalog)    (catalog->magicCookie)
#define FuelCat_MaxModels(catalog)      (catalog->maxModels)
#define FuelCat_Status(catalog)         (catalog->status)
#define FuelCat_FlameClasses(catalog)   (catalog->flameClasses)
#define FuelCat_FlameStep(catalog)      (catalog->flameStep)
#define FuelCat_FlameArray(catalog)     (catalog->flamePtr)
#define FuelCat_Name(catalog)           (catalog->name)
#define FuelCat_Error(catalog)          (catalog->error)
#define FuelCat_ModelArray(catalog)     (catalog->modelPtr)
#define FuelCat_ModelPtr(catalog,model) (catalog->modelPtr[model])

/*
 *------------------------------------------------------------------------------
 *  Function prototypes for fire behavior computations.
 *------------------------------------------------------------------------------
 */

#define FIRE_NONE       (0)
#define FIRE_BYRAMS     (1)
#define FIRE_FLAME      (2)
#define FIRE_SCORCH     (4)

EXTERN int Fire_FlameScorch _ANSI_ARGS_((
    FuelCatalogPtr catalog,     /* FuelCatalogData instance pointer           */
    size_t  model,              /* fuel model number            [0-maxModels] */
    size_t  doWhich     /* FIRE_NONE | FIRE_BYRAMS | FIRE_FLAME | FIRE_SCORCH */
    )) ;

EXTERN int Fire_FuelCombustion _ANSI_ARGS_((
    FuelCatalogPtr catalog,     /* FuelCatalogData instance pointer           */
    size_t  model               /* fuel model number            [0-maxModels] */
    )) ;

EXTERN int Fire_SpreadNoWindNoSlope _ANSI_ARGS_((
    FuelCatalogPtr catalog,     /* FuelCatalogData instance pointer           */
    size_t  model,              /* fuel model number            [0-maxModels] */
    double  moisture[FIRE_MCLASSES]  /* array of fuel moistures   (fractions) */
    )) ;

EXTERN int Fire_SpreadWindSlopeMax _ANSI_ARGS_((
    FuelCatalogPtr catalog,     /* FuelCatalogData instance pointer           */
    size_t  model,              /* fuel model number            [0-maxModels] */
    double  windFpm,            /* wind speed                        (ft/min) */
    double  windDeg,            /* wind bearing vector         (compass degs) */
    double  slope,              /* slope                         (rise/reach) */
    double  aspect              /* aspect (downslope) azimuth  (compass degs) */
    )) ;

EXTERN int Fire_SpreadAtAzimuth _ANSI_ARGS_((
    FuelCatalogPtr catalog,     /* FuelCatalogData instance pointer           */
    size_t  model,              /* fuel model number            [0-maxModels] */
    double  azimuth,            /* fire spread azimuth     (deg from upslope) */
    size_t  doWhich     /* FIRE_NONE | FIRE_BYRAMS | FIRE_FLAME | FIRE_SCORCH */
    )) ;

/*
 *------------------------------------------------------------------------------
 *  Function prototypes for creating and destroying fuel catalogs, fuel models,
 *  fuel particles, and flame length tables.
 *------------------------------------------------------------------------------
 */

EXTERN int Fire_FlameLengthTable _ANSI_ARGS_((
    FuelCatalogPtr catalog,     /* FuelCatalogData instance pointer           */
    size_t  flameClasses,       /* number of flame length classes             */
    double  flameStep           /* flame length step value per class          */
    )) ;

EXTERN FuelCatalogPtr Fire_FuelCatalogCreate _ANSI_ARGS_((
    char  *name,                /* FuelCatalogData instance name              */
    size_t maxModels            /* maximum modelId allowed in this catalog    */
    )) ;

EXTERN FuelCatalogPtr Fire_FuelCatalogCreateStandard _ANSI_ARGS_((
    char  *name,                /* FuelCatalogData instance name              */
    size_t maxModels            /* maximum modelId allowed in this catalog    */
    )) ;

EXTERN int Fire_FuelCatalogDestroy _ANSI_ARGS_((
    FuelCatalogPtr catalog      /* FuelCatalogData instance pointer           */
    )) ;

EXTERN int Fire_FuelModelCreate _ANSI_ARGS_((
    FuelCatalogPtr catalog,     /* FuelCatalogData instance                   */
    size_t  model,              /* fuel model number            [0-maxModels] */
    char   *name,               /* short name                                 */
    char   *desc,               /* longer description                         */
    double  depth,              /* bed depth                             (ft) */
    double  mext,               /* moisture of extinction                (dl) */
    double  adjust,             /* spread adjustment factor              (dl) */
    size_t  maxParticles        /* maximum number of fuel model particles     */
    )) ;

EXTERN int Fire_FuelModelDestroy _ANSI_ARGS_((
    FuelCatalogPtr catalog,     /* FuelCatalogData instance pointer           */
    size_t         model        /* fuel model id number         [0-maxModels] */
    )) ;

EXTERN int Fire_FuelModelExists _ANSI_ARGS_((
    FuelCatalogPtr catalog,     /* FuelCatalogData instance pointer           */
    size_t         model        /* fuel model id number         [0-maxModels] */
    )) ;

EXTERN int Fire_FuelParticleAdd _ANSI_ARGS_((
    FuelCatalogPtr catalog,     /* FuelCatalogData instance pointer           */
    size_t  model,              /* fuel model id number         [0-maxModels] */
    size_t  type,               /* FIRE_TYPE_DEAD, _TYPE_HERB, or _TYPE_WOOD  */
    double  load,               /* fuel load                        (lbs/ft2) */
    double  savr,               /* surface-area-to-volume ratio     (ft2/ft3) */
    double  dens,               /* density                          (lbs/ft3) */
    double  heat,               /* heat of combustion               (btus/lb) */
    double  stot,               /* total silica content               (lb/lb) */
    double  seff                /* effective silica content           (lb/lb) */
    )) ;

#ifdef NEED_STRDUP
char *strdup ( const char *str ) ;
#endif

#endif

/*
 *******************************************************************************
 * End of fireLib.h
 *******************************************************************************
 */
