/**********************************************************
 * Version $Id: fireLib.c 911 2011-02-14 16:38:15Z reklov_w $
 *********************************************************/
/*
 *******************************************************************************
 *
 *  fireLib.c
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
 *  Naming Conventions
 *      All function names begin with "Fire_".
 *      All fuel model and behavior parameter access macros begin with "Fuel_".
 *      All fuel catalog parameter access macros begin with "FuelCat_".
 *
 *  Functions
 *      There are 8 functions to create and destroy fuel models and catalogs:
 *
 *          Fire_FuelCatalogCreate(name, maxModels)
 *              Creates a new fuel catalog capable of holding maxModels.
 *
 *          Fire_FuelCatalogCreateStandard(name, maxModels)
 *              Creates a new fuel catalog capable of holding maxModels,
 *              and fills models 0-13 with standard fire behavior models.
 *
 *          Fire_FuelModelCreate(catalog, model, name, desc, depth, mext,
 *                  adjust, maxParticles)
 *              Adds or replaces a fuel model in the catalog.  The model will
 *              accept up to maxParticles particles.
 *
 *          Fire_FuelModelExists(catalog, model)
 *              Returns 1 if model exists within the catalog.
 *
 *          Fire_FuelParticleAdd(catalog, model, live, load, savr, dens, heat,
 *                  stot, seff)
 *              Adds a fuel particle to a fuel model.
 *
 *          Fire_FlameLengthTable ( catalog, flameClasses, flameStep )
 *              Creates a flame length look-up table containing flameClasses
 *              number of classes, with each class spanning "flameStep"
 *              feet of flame length.  Creating a flame length table can
 *              significantly improve performance at the expense of user
 *              specified precision.
 *
 *          Fire_FuelModelDestroy(catalog, model)
 *              Destroys the model within the catalog.
 *
 *          Fire_FuelCatalogDestroy(catalog)
 *              Destroys the catalog and all models within it.
 *
 *      There are 5 functions to process data within fuel models:
 *
 *          Fire_FuelCombustion(catalog, model)
 *              Computes all the fuel-dependent model variables.
 *              Called only once for each fuel model.
 *              Called automatically by Fire_SpreadNoWindNoSlope().
 *
 *          Fire_SpreadNoWindNoSlope(catalog, model, moisture[6])
 *              Determines reaction intensity, heat per unit area, and the
 *              no-wind no-slope spread rate.
 *
 *          Fire_SpreadWindSlopeMax(catalog, model, windFpm, windDeg, slope,
 *                  aspectDeg)
 *              Determines maximum spread rate and azimuth of maximum spread
 *              based upon input parameters and results of the most recent
 *              call to Fire_SpreadNoWindNoSlope() for this model.
 *
 *          Fire_SpreadAtAzimuth(catalog, model, azimuth, doWhich)
 *              Determines the spread rate in the specified azimuth based
 *              upon the results of the most recent call to
 *              Fire_SpreadWindSlopeMax() for this model.  The "doWhich"
 *              parameter is the result of ORing the constants FIRE_BYRAMS,
 *              FIRE_FLAME, and FIRE_SCORCH to request computation of the
 *              associated fire variables.
 *
 *          Fire_FlameScorch(catalog, model, doWhich)
 *              Determines the flame length and/or scorch height based upon
 *              the most recent call to Fire_SpreadAtAzimuth().
 *
 *  History
 *      1996/09/04  Version 1.0.0 release.
 *      1999/03/05  Fixed NNFL07 live SAVR from 1500 to 1550.
 *
 *******************************************************************************
 */

#include "fireLib.h"

#ifndef M_PI
#define M_PI 3.14159
#endif

/*
 *******************************************************************************
 *
 *  Fire_FuelCombustion()
 *
 *  Description
 *      Calculates and stores all the fuel-dependent combustion variables.
 *
 *  Side Effects
 *      All combustion varaiables are reclaculated for the model.
 *      All behavior and environment variables are reset to zero.
 *
 *  Function Returns
 *      FIRE_STATUS_OK or FIRE_STATUS_ERROR.
 *      Return status and error text are stored in the Fire Catalog's buffers.
 *
 *******************************************************************************
 */

int
Fire_FuelCombustion (FuelCatalogPtr catalog, size_t model )
    //FuelCatalogPtr catalog;     /* FuelCatalogData instance pointer           */
    //size_t         model;       /* fuel model id number         [0-maxModels] */
{
    size_t particle, size, life;

    double sizeClassAreaWtg[FIRE_LIFE_CATS][FIRE_SIZE_CLASSES];
    double lifeLoad[FIRE_LIFE_CATS];
    double lifeArea[FIRE_LIFE_CATS];
    double lifeSavr[FIRE_LIFE_CATS];
    double lifeHeat[FIRE_LIFE_CATS];
    double lifeSeff[FIRE_LIFE_CATS];
    double lifeEtaS[FIRE_LIFE_CATS];

    double totalArea;
    double fineLive;
    double beta;
    double betaOpt;
    double sigma;
    double ratio;
    double aa;
    double sigma15;
    double gammaMax;
    double gamma;
    double c;
    double e;

    /* Validate catalog and fuel model existence. */
    assert(catalog!= NULL && FuelCat_MagicCookie(catalog)==FIRE_CATALOG_MAGIC);
    if ( ! Fire_FuelModelExists(catalog,model) )
    {
        sprintf(FuelCat_Error(catalog),
            "Fire_FuelCombustion(): el modelo de combustible %d no existe en el catálogo de combuestibles \"%s\".",
            model, FuelCat_Name(catalog));
        return (FuelCat_Status(catalog) = FIRE_STATUS_ERROR);
    }

    /* Initialize the model's fuel particle dependent variables. */
    for ( particle=0; particle<Fuel_Particles(catalog,model); particle++ )
    {
        Fuel_AreaWtg(catalog,model,particle)     = 0.;
        Fuel_SizeAreaWtg(catalog,model,particle) = 0.;
        Fuel_Moisture(catalog,model,particle)    = 0.;
    }

    /* Initialize the model's fuel combustion variables. */
    /* The following are calculated by this function. */
    Fuel_FineDead(catalog,model)        = 0.0;
    Fuel_LiveMextFactor(catalog,model)  = 0.0;
    Fuel_BulkDensity(catalog,model)     = 0.0;
    Fuel_ResidenceTime(catalog,model)   = 0.0;
    Fuel_PropFlux(catalog,model)        = 0.0;
    Fuel_SlopeK(catalog,model)          = 0.0;
    Fuel_WindB(catalog,model)           = 0.0;
    Fuel_WindE(catalog,model)           = 0.0;
    Fuel_WindK(catalog,model)           = 0.0;

    for (life=0; life<FIRE_LIFE_CATS; life++)
    {
        Fuel_LifeAreaWtg(catalog,model,life) = 0.;
        Fuel_LifeRxFactor(catalog,model,life) = 0.;
        lifeLoad[life] = 0.;
        lifeArea[life] = 0.;
        lifeSavr[life] = 0.;
        lifeHeat[life] = 0.;
        lifeEtaS[life] = 0.;
        lifeSeff[life] = 0.;
        for ( size=0; size<FIRE_SIZE_CLASSES; size++ )
            sizeClassAreaWtg[life][size] = 0.;
    }

    /* Initialize the model's fire behavior variables. */
    /* These are calculated by Fire_SpreadNoWindNoSlope(). */
    Fuel_Spread0(catalog,model)         = 0.;
    Fuel_RxIntensity(catalog,model)     = 0.;
    Fuel_HeatPerUnitArea(catalog,model) = 0.;

    /* Initialize the model's fire behavior variables. */
    /* These are calculated by Fire_SpreadWindSlopeMax(). */
    Fuel_SpreadMax(catalog,model)       = 0.;
    Fuel_AzimuthMax(catalog,model)      = 0.;
    Fuel_EffectiveWind(catalog,model)   = 0.;
    Fuel_PhiSlope(catalog,model)        = 0.;
    Fuel_PhiWind(catalog,model)         = 0.;
    Fuel_PhiEffWind(catalog,model)      = 0.;
    Fuel_LwRatio(catalog,model)         = 1.;
    Fuel_Eccentricity(catalog,model)    = 0.;
    Fuel_WindLimit(catalog,model)       = 0;

    /* Initialize the model's fire behavior variables. */
    /* These are calculated by Fire_SpreadAtAzimuth(). */
    Fuel_SpreadAny(catalog,model)       = 0.;
    Fuel_AzimuthAny(catalog,model)      = 0.;
    Fuel_ByramsIntensity(catalog,model) = 0.;
    Fuel_FlameLength(catalog,model)     = 0.;
    Fuel_ScorchHeight(catalog,model)    = 0.;

    /* Initialize the model's environmental variables. */
    Fuel_WindSpeed(catalog,model) = 0.;
    Fuel_WindDir(catalog,model)   = 0.;
    Fuel_Slope(catalog,model)     = 0.;
    Fuel_Aspect(catalog,model)    = 0.;
    for ( size=0; size<FIRE_MCLASSES; size++ )
        Fuel_EnvMoisture(catalog,model,size) = 0.;

    /* Initialize the model's combustion flag. */
    Fuel_CombustionFlag(catalog,model) = 1;

    /* If the model has no particles, we're all done. */
    if ( Fuel_Particles(catalog,model) <= 0 )
        return (FuelCat_Status(catalog) = FIRE_STATUS_OK);

    /* Initialize local fuel bed and combustion variables. */
    beta = betaOpt = sigma = ratio = aa = sigma15 = 0.;
    gamma = gammaMax = c = e = fineLive = totalArea = 0.;

    /* Accumulate surface areas by life category for the entire fuel bed. */
    for ( particle=0; particle<Fuel_Particles(catalog,model); particle++ )
    {
        life = Fuel_Live(catalog,model,particle);
        lifeArea[life] += Fuel_SurfaceArea(catalog,model,particle);
        totalArea      += Fuel_SurfaceArea(catalog,model,particle);
    }

    /* If no surface area, we're all done. */
    if ( totalArea <= Smidgen )
        return (FuelCat_Status(catalog) = FIRE_STATUS_OK);

    /* Surface area wtg factor for each particle within its life category */
    /* and within its size class category (used to weight loading). */
    for ( particle=0; particle<Fuel_Particles(catalog,model); particle++ )
    {
        life = Fuel_Live(catalog,model,particle);
        if ( lifeArea[life] > Smidgen )
        {
            Fuel_AreaWtg(catalog,model,particle) =
                Fuel_SurfaceArea(catalog,model,particle) / lifeArea[life];

            size = Fuel_SizeClass(catalog,model,particle);
            sizeClassAreaWtg[life][size] +=
                Fuel_AreaWtg(catalog,model,particle);
        }
    }

    /* Assign size class surface area weights to each particle. */
    for ( particle=0; particle<Fuel_Particles(catalog,model); particle++ )
    {
        life = Fuel_Live(catalog,model,particle);
        size = Fuel_SizeClass(catalog,model,particle);
        Fuel_SizeAreaWtg(catalog,model,particle) = sizeClassAreaWtg[life][size];
    }

    /* Derive life category surface area weighting factors. */
    for ( life=0; life<FIRE_LIFE_CATS; life++ )
        Fuel_LifeAreaWtg(catalog,model,life) = lifeArea[life] / totalArea;

    /* Accumulate life category weighted load, heat, savr, and seff. */
    for ( particle=0; particle<Fuel_Particles(catalog,model); particle++ )
    {
        life = Fuel_Live(catalog,model,particle);

        lifeLoad[life] += Fuel_SizeAreaWtg(catalog,model,particle)
                        * Fuel_Load(catalog,model,particle)
                        * (1. - Fuel_SiTotal(catalog,model,particle));

        lifeSavr[life] += Fuel_AreaWtg(catalog,model,particle)
                        * Fuel_Savr(catalog,model,particle);

        lifeHeat[life] += Fuel_AreaWtg(catalog,model,particle)
                        * Fuel_Heat(catalog,model,particle);

        lifeSeff[life] += Fuel_AreaWtg(catalog,model,particle)
                        * Fuel_SiEffective(catalog,model,particle);

        Fuel_BulkDensity(catalog,model) += Fuel_Load(catalog,model,particle);

        if ( Fuel_Density(catalog,model,particle) > Smidgen )
            beta += Fuel_Load(catalog,model,particle)
                  / Fuel_Density(catalog,model,particle);
    }

    /* Accumulate life category contribution to reaction intensity. */
    for ( life=0; life<FIRE_LIFE_CATS; life++ )
    {
        sigma += Fuel_LifeAreaWtg(catalog,model,life) * lifeSavr[life];

        lifeEtaS[life] = 1.;
        if (lifeSeff[life] > 0.)
        {
            if ( (lifeEtaS[life] = 0.174 / pow(lifeSeff[life], 0.19)) > 1.0 )
                lifeEtaS[life] = 1.0;
        }

        Fuel_LifeRxFactor(catalog,model,life) =
            lifeLoad[life] * lifeHeat[life] * lifeEtaS[life];
    }

    /* Fuel model residence time */
    Fuel_ResidenceTime(catalog,model) = 384. / sigma;

    /* Fuel model bulk density */
    if ( Fuel_Depth(catalog,model) > Smidgen )
    {
        Fuel_BulkDensity(catalog,model) /= Fuel_Depth(catalog,model);
        beta /= Fuel_Depth(catalog,model);
    }

    /* Propagating flux depends upon sigma and beta only. */
    Fuel_PropFlux(catalog,model) =
        exp((0.792 + 0.681*sqrt(sigma)) * (beta+0.1)) / (192.+0.2595*sigma);

    /* Gamma */
    betaOpt   = 3.348 / (pow(sigma, 0.8189));
    ratio     = beta / betaOpt;
    aa        = 133. / (pow(sigma, 0.7913));
    sigma15   = pow(sigma, 1.5);
    gammaMax  = sigma15 / (495. + 0.0594*sigma15);
    gamma     = gammaMax * pow(ratio, aa) * exp(aa * (1.-ratio));

    /* Factor gamma into life category reaction intensity contribution. */
    for ( life=0; life<FIRE_LIFE_CATS; life++ )
        Fuel_LifeRxFactor(catalog,model,life) *= gamma;

    /* Slope and wind intermediates constants for the fuel model. */
    Fuel_SlopeK(catalog,model) = 5.275 * pow(beta, -0.3);
    Fuel_WindB(catalog,model)  = 0.02526 * pow(sigma, 0.54);

    c = 7.47 * exp(-0.133 * pow(sigma, 0.55));
    e = 0.715 * exp(-0.000359 * sigma);
    Fuel_WindK(catalog,model) = c * pow(ratio, -e);
    Fuel_WindE(catalog,model) = pow(ratio, e) / c;

    /* If no live fuel, we're done. */
    if ( lifeLoad[FIRE_LIFE_LIVE] < Smidgen )
        return (FuelCat_Status(catalog) = FIRE_STATUS_OK);

    /*  Fine dead fuel and fine live fuel factors. */
    for ( particle=0; particle<Fuel_Particles(catalog,model); particle++ )
    {
        if ( Fuel_Live(catalog,model,particle) )
            fineLive
                  += Fuel_Load(catalog,model,particle)
                   * exp(-500. / Fuel_Savr(catalog,model,particle));
        else
            Fuel_FineDead(catalog,model)
                  += Fuel_Load(catalog,model,particle)
                   * Fuel_SigmaFactor(catalog,model,particle);
    }

    /* Live fuel extinction moisture factor. */
    if ( fineLive > Smidgen )
        Fuel_LiveMextFactor(catalog,model)
            = 2.9 * Fuel_FineDead(catalog,model) / fineLive;

    /* That's all, folks!. */
    return (FuelCat_Status(catalog) = FIRE_STATUS_OK);
}

/*
 *******************************************************************************
 *
 *  Fire_SpreadNoWindNoSlope()
 *
 *  Description
 *      Calculates the fire reaction intensity and no-wind, no-slope spread
 *      rate given the fuel model, combustion, and moisture regime inputs.
 *
 *  Side Effects
 *      Updates the following fire behavior variables:
 *          Fuel_RxIntensity(catalog,model).
 *          Fuel_HeatPerUnitArea(catalog,model).
 *          Fuel_Spread0(catalog,model).
 *          Fuel_SpreadMax(catalog,model)  = Fuel_Spread0(catalog,model)
 *          Fuel_SpreadAny(catalog,model)  = Fuel_Spread0(catalog,model)
 *          Fuel_AzimuthAny(catalog,model) = 0.
 *          Fuel_AzimuthMax(catalog,model) = 0.
 *
 *  Function Returns
 *      FIRE_STATUS_OK or FIRE_STATUS_ERROR.
 *      Return status and error text are stored in the Fire Catalog's buffers.
 *
 *******************************************************************************
 */

int
Fire_SpreadNoWindNoSlope ( FuelCatalogPtr catalog, size_t model, double moisture[FIRE_MCLASSES] )
   // FuelCatalogPtr catalog;     /* FuelCatalogData instance pointer           */
   // size_t  model;              /* fuel model number            [0-maxModels] */
   // double  moisture[FIRE_MCLASSES]; /* array of fuel moistures   (fractions) */
{
    size_t mclass, particle, life, nlive;
    double wfmd;
    double rbQig;
    double fdmois;
    double qig;
    double ratio;
    double lifeMoisture[FIRE_LIFE_CATS];
    double lifeEtaM[FIRE_LIFE_CATS];
    double lifeMext[FIRE_LIFE_CATS];

    static size_t TimeLagClass[FIRE_SIZE_CLASSES] = {0, 0, 1, 1, 2, 2};

    /* Validate the catalog and fuel model. */
    assert(catalog!= NULL && FuelCat_MagicCookie(catalog)==FIRE_CATALOG_MAGIC);
    if ( ! Fire_FuelModelExists(catalog,model) )
    {
        sprintf(FuelCat_Error(catalog),
            "Fire_SpreadNoWindNoSlope(): el modelo de combustible %d no existe en el catálogo de combustibles \"%s\".",
            model, FuelCat_Name(catalog));
        return (FuelCat_Status(catalog) = FIRE_STATUS_ERROR);
    }

    /*  Check if we must recalculate combustion intermediates. */
    if ( ! Fuel_CombustionFlag(catalog,model) )
    {
        Fire_FuelCombustion(catalog,model);
    }

    /* Otherwise check if the moisture environment has changed. */
    else
    {
        for ( mclass=0; mclass<FIRE_MCLASSES; mclass++ )
            if ( ! Equal(moisture[mclass],Fuel_EnvMoisture(catalog,model,mclass)) )
                break;

        /* If no change in fuel moisture, no more computation is needed. */
        if ( mclass == FIRE_MCLASSES )
            return (FuelCat_Status(catalog) = FIRE_STATUS_OK);
    }

    /* Store the new moistures in the fuel's environmental moisture array. */
    for ( mclass=0; mclass<FIRE_MCLASSES; mclass++ )
        Fuel_EnvMoisture(catalog,model,mclass) = moisture[mclass];

    /* Initialize the model's fire behavior variables. */
    /* These are calculated by this function. */
    Fuel_Spread0(catalog,model)         = 0.;
    Fuel_RxIntensity(catalog,model)     = 0.;
    Fuel_HeatPerUnitArea(catalog,model) = 0.;

    /* Initialize the model's fire behavior variables. */
    /* These are calculated by Fire_SpreadWindSlopeMax(). */
    Fuel_SpreadMax(catalog,model)       = 0.;
    Fuel_AzimuthMax(catalog,model)      = 0.;

    /* Initialize the model's fire behavior variables. */
    /* These are calculated by Fire_SpreadAtAzimuth(). */
    Fuel_SpreadAny(catalog,model)       = 0.;
    Fuel_AzimuthAny(catalog,model)      = 0.;
    Fuel_ByramsIntensity(catalog,model) = 0.;
    Fuel_FlameLength(catalog,model)     = 0.;
    Fuel_ScorchHeight(catalog,model)    = 0.;

    /* If no fuel particles, return. */
    if (Fuel_Particles(catalog,model) <= 0)
        return (FuelCat_Status(catalog) = FIRE_STATUS_OK);

    /* Initialize local variables. */
    wfmd = fdmois = rbQig = 0.;
    for ( life=0; life<FIRE_LIFE_CATS; life++ )
    {
        lifeMoisture[life] = 0.;
        lifeEtaM[life] = 0.;
        lifeMext[life] = 0.;
    }

    /* Assign particle moistures based upon their size class. */
    nlive = 0;
    for ( particle=0; particle<Fuel_Particles(catalog,model); particle++ )
    {
        /* if this is a dead fuel, accumulate its wtd fuel moisture. */
        if ( Fuel_Live(catalog,model,particle) == FIRE_LIFE_DEAD )
        {
            mclass = TimeLagClass[Fuel_SizeClass(catalog,model,particle)];
            wfmd += moisture[mclass]
                  * Fuel_SigmaFactor(catalog,model,particle)
                  * Fuel_Load(catalog,model,particle);
        }
        else
        {
            nlive++;
            mclass = (Fuel_Type(catalog,model,particle) == FIRE_TYPE_HERB) ?
                FIRE_MCLASS_HERB : FIRE_MCLASS_WOOD;
        }

        /* Assign this particle the fuel moisture for its size class. */
        Fuel_Moisture(catalog,model,particle) = moisture[mclass];
    }

    /* Compute live fuel extinction moisture. */
    if ( nlive )
    {
        fdmois = ( Fuel_FineDead(catalog,model) > Smidgen ) ?
            (wfmd / Fuel_FineDead(catalog,model)) : 0.;

        lifeMext[FIRE_LIFE_LIVE]
            = ((Fuel_LiveMextFactor(catalog,model)
            * (1.0 - fdmois/Fuel_Mext(catalog,model))) - 0.226);

        if ( lifeMext[FIRE_LIFE_LIVE] < Fuel_Mext(catalog,model) )
            lifeMext[FIRE_LIFE_LIVE] = Fuel_Mext(catalog,model);
    }

    /* Dead fuel extinction moisture is a fuel model input. */
    lifeMext[FIRE_LIFE_DEAD] = Fuel_Mext(catalog,model);

    /* Compute category weighted moisture and accumulate the rbQig. */
    for (particle=0; particle<Fuel_Particles(catalog,model); particle++)
    {
        qig = 250. + 1116. * Fuel_Moisture(catalog,model,particle);

        life = Fuel_Live(catalog,model,particle);

        lifeMoisture[life] += Fuel_AreaWtg(catalog,model,particle)
                            * Fuel_Moisture(catalog,model,particle);

        rbQig += qig
               * Fuel_AreaWtg(catalog,model,particle)
               * Fuel_LifeAreaWtg(catalog,model,life)
               * Fuel_SigmaFactor(catalog,model,particle);
    }

    /* Complete the rbQig calculation. */
    rbQig *= Fuel_BulkDensity(catalog,model);

    /*  Compute moisture damping coeff by life class, and combine with the */
    /*  life class's rx factor to get the total reaction intensity. */
    for (life=0; life<FIRE_LIFE_CATS; life++)
    {
        ratio = 0.;
        if ( lifeMext[life] > Smidgen )
        {
            ratio = lifeMoisture[life] / lifeMext[life];
            lifeEtaM[life] =
                1.0 - 2.59*ratio + 5.11*ratio*ratio - 3.52*ratio*ratio*ratio;
        }

        /* If category moisture exceeds category extinction moisture, */
        /* the damping coefficient is zero. */
        if ( lifeMoisture[life] >= lifeMext[life] )
            lifeEtaM[life] = 0.;

        /* Accumulate total reaction intensity. */
        Fuel_RxIntensity(catalog,model)
            += Fuel_LifeRxFactor(catalog,model,life)
             * lifeEtaM[life];
    }

    /* Calculate heat per unit area from rx intensity and residence time. */
    Fuel_HeatPerUnitArea(catalog,model)
        = Fuel_RxIntensity(catalog,model)
        * Fuel_ResidenceTime(catalog,model);

    /* Calculate no-wind, no-slope spread rate. */
    Fuel_Spread0(catalog,model)
        = (rbQig > Smidgen)
        ? Fuel_RxIntensity(catalog,model) * Fuel_PropFlux(catalog,model) / rbQig
        : 0.;

    /* Re-initialize spread in maximum and any azimuth to no wind-no slope. */
    Fuel_SpreadMax(catalog,model) = Fuel_Spread0(catalog,model);
    Fuel_SpreadAny(catalog,model) = Fuel_Spread0(catalog,model);
    Fuel_AzimuthMax(catalog,model) = Fuel_AzimuthAny(catalog,model) = 0.;

    return (FuelCat_Status(catalog) = FIRE_STATUS_OK);
}

/*
 *******************************************************************************
 *
 *  Fire_SpreadWindSlopeMax()
 *
 *  Description
 *      Calculates maximum fire spread rate and direction under the given
 *      wind-slope conditions.  Results depend only upon:
 *      - no wind-slope spread rate
 *      - wind speed and direction
 *      - aspect and slope
 *
 *  Side Effects
 *      Updates the following variables:
 *          Fuel_Slope()                      = slope;
 *          Fuel_PhiSlope().
 *          Fuel_Wind()                       = windFpm;
 *          Fuel_PhiWind().
 *          Fuel_Aspect(catalog,model)        = aspect;
 *          Fuel_WindDir(catalog,model)       = windDeg;
 *          Fuel_PhiEffWind(catalog,model)    = phiEw;
 *          Fuel_EffectiveWind(catalog,model) = effectiveWind;
 *          Fuel_WindLimit(catalog,model)     = windLimit;
 *          Fuel_SpreadMax(catalog,model)     = spreadMax;
 *          Fuel_AzimuthMax(catalog,model)    = azimuthMax;
 *          Fuel_LwRatio(catalog,model)       = lwRatio;
 *          Fuel_Eccentricity(catalog,model)  = eccentricity;
 *
 *      Resets Fire_SpreadAtAzimuth() variables:
 *          Fuel_SpreadAny(catalog,model)       = spreadMax;
 *          Fuel_AzimuthAny(catalog,model)      = azimuthMax;
 *          Fuel_ByramsIntensity(catalog,model) = 0.;
 *          Fuel_FlameLength(catalog,model)     = 0.;
 *          Fuel_ScorchHeight(catalog,model)    = 0.;
 *
 *      Previous Fire_SpreadAtAzimiuth() results become obsolete for this model.
 *
 *  Function Returns
 *      FIRE_STATUS_OK or FIRE_STATUS_ERROR.
 *      Return status and error text are stored in the Fire Catalog's buffers.
 *
 *******************************************************************************
 */

int
Fire_SpreadWindSlopeMax ( FuelCatalogPtr catalog, size_t model, double windFpm, double windDeg, double slope, double aspect )
   // FuelCatalogPtr catalog;     /* FuelCatalogData instance pointer           */
   // size_t  model;              /* fuel model number            [0-maxModels] */
   // double  windFpm;            /* wind speed                        (ft/min) */
   // double  windDeg;            /* wind bearing vector      (compass degrees) */
   // double  slope;              /* slope                         (rise/reach) */
   // double  aspect;             /* aspect (downslope)   azimuth (compass deg) */
{
    double upslope, azimuthMax, phiEw;
    double splitDeg, splitRad;
    double slpRate, wndRate, rv, spreadMax;
    double x, y, al, a;
    double maxWind, effectiveWind, lwRatio, eccentricity;
    size_t doEffectiveWind, checkWindLimit, windLimit;

    /* Validate the catalog and fuel model. */
    assert(catalog!= NULL && FuelCat_MagicCookie(catalog)==FIRE_CATALOG_MAGIC);
    if ( ! Fire_FuelModelExists(catalog,model) )
    {
        sprintf(FuelCat_Error(catalog),
            "Fire_SpreadMax(): el modelo de combustible %d no existe en el catálogo de combustibles \"%s\".",
            model, FuelCat_Name(catalog));
        return (FuelCat_Status(catalog) = FIRE_STATUS_ERROR);
    }

    /* Recalculate slope factors ONLY if different from previous model input. */
    if ( ! Equal(Fuel_Slope(catalog,model),slope) )
    {
        Fuel_PhiSlope(catalog,model) =
            Fuel_SlopeK(catalog,model) * slope * slope;
        Fuel_Slope(catalog,model) = slope;
    }

    /* Recalculate wind factors ONLY if different from previous model input. */
    if ( ! Equal(Fuel_WindSpeed(catalog,model),windFpm) )
    {
        Fuel_PhiWind(catalog,model) = (windFpm < Smidgen) ? 0. :
            Fuel_WindK(catalog,model) * pow(windFpm, Fuel_WindB(catalog,model));
        Fuel_WindSpeed(catalog,model) = windFpm;
    }

    /* Combine wind and slope factors. */
    phiEw = Fuel_PhiSlope(catalog,model) + Fuel_PhiWind(catalog,model);
    windLimit = 0;
    lwRatio = 1.;
    eccentricity = 0.;
    upslope = (aspect>=180.) ? aspect-180. : aspect+180.;

    /* Situation 1: no fire spread or reaction intensity. */
    if ( Fuel_Spread0(catalog,model) < Smidgen )
    {
        spreadMax = 0.;
        azimuthMax = 0;
        /* There IS an effective wind even if there is no fire. */
        doEffectiveWind = 1;
        /* But since BEHAVE doesn't calculate effective wind when no spread. */
        /* we wont either. */
        effectiveWind = 0.;
        doEffectiveWind = 0;
        checkWindLimit = 0;
    }

    /* Situation 2: no wind and no wind */
    else if ( phiEw < Smidgen )
    {
        phiEw = 0.;
        effectiveWind = 0.;
        doEffectiveWind = 0;
        spreadMax = Fuel_Spread0(catalog,model);
        azimuthMax = 0;
        checkWindLimit = 0;
    }

    /* Situation 3: wind with no slope. */
    else if ( slope < Smidgen )
    {
        effectiveWind = windFpm;
        doEffectiveWind = 0;
        spreadMax = Fuel_Spread0(catalog,model) * (1. + phiEw);
        azimuthMax = windDeg;
        checkWindLimit = 1;
    }

    /* Situation 4: slope with no wind. */
    else if ( windFpm < Smidgen )
    {
        doEffectiveWind = 1;
        spreadMax = Fuel_Spread0(catalog,model) * (1. + phiEw);
        azimuthMax = upslope;
        checkWindLimit = 1;
    }

    /* Situation 5: wind blows upslope. */
    else if ( Equal(upslope,windDeg) )
    {
        doEffectiveWind = 1;
        spreadMax = Fuel_Spread0(catalog,model) * (1. + phiEw);
        azimuthMax = upslope;
        checkWindLimit = 1;
    }

    /* Situation 6: wind blows cross slope. */
    else
    {
        /* Recalculate spread rate in the optimal direction. */
        splitDeg = (upslope<=windDeg) ? windDeg-upslope : 360.-upslope+windDeg;
        splitRad = DegreesToRadians(splitDeg);
        slpRate  = Fuel_Spread0(catalog,model) * Fuel_PhiSlope(catalog,model);
        wndRate  = Fuel_Spread0(catalog,model) * Fuel_PhiWind(catalog,model);
        x        = slpRate + wndRate * cos(splitRad);
        y        = wndRate * sin(splitRad);
        rv       = sqrt(x*x + y*y);
        spreadMax= Fuel_Spread0(catalog,model) + rv;

        /* Recalculate phiEw in the optimal direction. */
        phiEw    = spreadMax / Fuel_Spread0(catalog,model) - 1.0;
        doEffectiveWind = (phiEw > Smidgen) ? 1 : 0;
        checkWindLimit = 1;

        /* Recalculate direction of maximum spread in azimuth degrees. */
        al = asin(fabs(y) / rv);
        if ( x >= 0. )
            a = (y >= 0.) ? al          : M_PI + M_PI - al;
        else
            a = (y >= 0.) ? (M_PI - al) : (M_PI + al);

        splitDeg = RadiansToDegrees(a);
        if ( (azimuthMax = upslope + splitDeg) > 360. )
            azimuthMax -= 360.;
    }

    /* Recalculate effective wind speed based upon phiEw. */
    if ( doEffectiveWind )
        effectiveWind = pow( (phiEw * Fuel_WindE(catalog,model)),
                             (1. / Fuel_WindB(catalog,model)) );

    /* If effective wind exceeds maximum wind, scale back spread & phiEw. */
    if ( checkWindLimit )
    {
        maxWind = 0.9 * Fuel_RxIntensity(catalog,model);
        if ( effectiveWind > maxWind )
        {
            phiEw = (maxWind < Smidgen) ? 0. :
                Fuel_WindK(catalog,model) * pow(maxWind, Fuel_WindB(catalog,model));

            spreadMax = Fuel_Spread0(catalog,model) * (1. + phiEw);
            effectiveWind = maxWind;
            windLimit = 1;
        }
    }

    /* Determine fire ellipse parameters from the effective wind speed. */
    /* = 1. + 0.25 * (Fuel_EffectiveWind(catalog,model) / 88.0); */
    if ( effectiveWind > Smidgen )
    {
        lwRatio = 1. + 0.002840909 * effectiveWind;
        eccentricity = sqrt(lwRatio * lwRatio - 1.0) / lwRatio;
    }

    /* Store the results. */
    Fuel_Aspect(catalog,model)       = aspect;
    Fuel_WindDir(catalog,model)      = windDeg;
    Fuel_PhiEffWind(catalog,model)   = phiEw;
    Fuel_EffectiveWind(catalog,model)= effectiveWind;
    Fuel_WindLimit(catalog,model)    = windLimit;
    Fuel_SpreadMax(catalog,model)    = Fuel_SpreadAny(catalog,model)  = spreadMax;
    Fuel_AzimuthMax(catalog,model)   = Fuel_AzimuthAny(catalog,model) = azimuthMax;
    Fuel_LwRatio(catalog,model)      = lwRatio;
    Fuel_Eccentricity(catalog,model) = eccentricity;

    /* Initialize behavior variables updated by Fire_SpreadAtAzimuth(). */
    Fuel_ByramsIntensity(catalog,model) = 0.;
    Fuel_FlameLength(catalog,model)     = 0.;
    Fuel_ScorchHeight(catalog,model)    = 0.;

    return (FuelCat_Status(catalog) = FIRE_STATUS_OK);
}

/*
 *******************************************************************************
 *
 *  Fire_SpreadAtAzimuth()
 *
 *  Description
 *      Calculates fire spread rate in a specified direction and optionally
 *      calculates the fireline intensity, flame length, and scorch height
 *      along the fire spread vector.
 *
 *  Side Effects
 *      The following variables are updated:
 *          Fuel_SpreadAny(catalog,model)
 *          Fuel_AzimuthAny(catalog,model) == azimuth;
 *          Fuel_ByramsIntensity(catalog,model) is updated if FIRE_BYRAMS.
 *          Fuel_FlameLength(catalog,model) is updated if FIRE_FLAME.
 *          Fuel_ScorchHeight(catalog,model) is updated if FIRE_SCORCH.
 *  Notes
 *      The calculations depend upon the most recent calls to
 *      Fire_SpreadNoWindNoSlope() and Fire_SpreadWindSlopeMax() for this model.
 *
 *      The input azimuth is the degrees clockwise from north.
 *
 *  Function Returns
 *      FIRE_STATUS_OK or FIRE_STATUS_ERROR.
 *      Return status and error text are stored in the Fire Catalog's buffers.
 *
 *******************************************************************************
 */

int
Fire_SpreadAtAzimuth ( FuelCatalogPtr catalog, size_t model, double azimuth, size_t which )
   // FuelCatalogPtr catalog;     /* FuelCatalogData instance pointer           */
   // size_t  model;              /* fuel model number            [0-maxModels] */
   // double  azimuth;            /* fire spread azimuth     (deg from upslope) */
   // size_t  which;      /* FIRE_NONE | FIRE_BYRAMS | FIRE_FLAME | FIRE_SCORCH */
{
    double dir;
    double radians;
    double byrams;
    double mph;
    size_t lo, hi, mid, n;

    /* Validate catalog and the fuel model. */
    assert(catalog!= NULL && FuelCat_MagicCookie(catalog)==FIRE_CATALOG_MAGIC);
    if ( ! Fire_FuelModelExists(catalog,model) )
    {
        sprintf(FuelCat_Error(catalog),
            "Fire_SpreadAtAzimuth(): el modelo de combustible %d no existe en el catálogo de combustibles \"%s\".",
            model, FuelCat_Name(catalog));
        return (FuelCat_Status(catalog) = FIRE_STATUS_ERROR);
    }

    /* Situation 1: no fire or reaction intensity, so no Byrams or flame. */
    if ( Fuel_SpreadMax(catalog,model) < Smidgen )
        return (FuelCat_Status(catalog) = FIRE_STATUS_OK);

    /* Situation 2: phiEw is zero OR azimuth is in the max spread direction */
    if ( Fuel_PhiEffWind(catalog,model) < Smidgen
      || Equal(Fuel_AzimuthMax(catalog,model),azimuth) )
    {
        Fuel_SpreadAny(catalog,model) = Fuel_SpreadMax(catalog,model);
    }

    /* Situation 3: wind and/or slope and azimuth not in max spread direction */
    else
    {
        /* Angle between maximum spread azimuth and requested azimuth. */
        if ( (dir = fabs(Fuel_AzimuthMax(catalog,model) - azimuth)) > 180. )
            dir = 360. - dir;
        radians = DegreesToRadians(dir);

        /* Calculate the fire spread rate in this azimuth. */
        Fuel_SpreadAny(catalog,model)
            = Fuel_SpreadMax(catalog,model)
            * (1. - Fuel_Eccentricity(catalog,model))
            / (1. - Fuel_Eccentricity(catalog,model) * cos(radians));
    }
    Fuel_AzimuthAny(catalog,model) = azimuth;

    /* Additional fire behavior outputs. */
    if ( which )
    {
        /* Must compute Byram's if any of the three are requested. */
        byrams = Fuel_ResidenceTime(catalog,model)
               * Fuel_SpreadAny(catalog,model)
               * Fuel_RxIntensity(catalog,model)
               / 60.;

        /* Byrams intensity is requested. */
        if ( which & FIRE_BYRAMS )
            Fuel_ByramsIntensity(catalog,model) = byrams;

        /* Flame length is requested. */
        if ( (which & FIRE_FLAME) )
        {
            if ( byrams < Smidgen )
            {
                Fuel_FlameLength(catalog,model) = 0.;
            }
            else
            {
                /* Use lookup table if it exists & includes this intensity. */
                if ( (n = FuelCat_FlameClasses(catalog)) > 0
                  && FuelCat_FlameArray(catalog)[n-1] > byrams )
                {
                    hi = n-1;
                    lo = 0;
                    do {
                        mid = lo + (hi-lo)/2;
                        if ( FuelCat_FlameArray(catalog)[mid] > byrams )
                            hi = mid;
                        else
                            lo = mid + 1;
                    } while (lo != hi);
                    Fuel_FlameLength(catalog,model) =
                        FuelCat_FlameStep(catalog) * (lo+1);
                }
                /* otherwise compute flame length from scratch. */
                else
                {
                    Fuel_FlameLength(catalog,model) = 0.45 * pow(byrams, 0.46);
                }
            }
        }

        /* Scorch height is requested. */
        if ( (which & FIRE_SCORCH) )
        {
            if ( byrams < Smidgen )
            {
                Fuel_ScorchHeight(catalog,model) = 0.;
            }
            else
            {
                mph = Fuel_WindSpeed(catalog,model) / 88.;
                Fuel_ScorchHeight(catalog,model) =
                    pow(byrams, 1.166667) / sqrt(byrams + (mph * mph * mph));
            /*  Fuel_ScorchHeight(catalog,model) *= (63. / (140. - temp_f) ); */
            }
        }
    }

    return (FuelCat_Status(catalog) = FIRE_STATUS_OK);
}

/*
 *******************************************************************************
 *
 *  Fire_FlameScorch()
 *
 *  Description
 *      Calculates the flame length and/or scorch height for the current
 *      Byram's intensity and azimuth (as determined by the most recent
 *      call to Fire_SpreadAtAzimuth()).
 *      Uses the Flame Length Table if it exists.
 *      Offers a method of getting flame length if Fire_SpreadAtAzimuth()
 *      is not calculating it.
 *
 *  Side Effects
 *      The following variables are updated:
 *          Fuel_FlameLength(catalog,model) is updated.
 *          Fuel_ScorchHeight(catalog,model)
 *  Notes
 *      The calculations depend upon the most recent calls to
 *      Fire_SpreadNoWindNoSlope(), Fire_SpreadWindSlopeMax(), and
 *      Fire_SpreadAtAzimuth() for this model.
 *
 *  Function Returns
 *      FIRE_STATUS_OK or FIRE_STATUS_ERROR.
 *      Return status and error text are stored in the Fire Catalog's buffers.
 *
 *******************************************************************************
 */

int
Fire_FlameScorch ( FuelCatalogPtr catalog, size_t model, size_t which )
   // FuelCatalogPtr catalog;     /* FuelCatalogData instance pointer           */
   // size_t  model;              /* fuel model number            [0-maxModels] */
   // size_t  which;      /* FIRE_NONE | FIRE_BYRAMS | FIRE_FLAME | FIRE_SCORCH */
{
    double byrams;
    double mph;
    size_t lo, hi, mid, n;

    /* Validate catalog and the fuel model. */
    assert(catalog!= NULL && FuelCat_MagicCookie(catalog)==FIRE_CATALOG_MAGIC);
    if ( ! Fire_FuelModelExists(catalog,model) )
    {
        sprintf(FuelCat_Error(catalog),
            "Fire_FlameScorch(): el modelo de combustible %d no existe en el catálogo de combustibles \"%s\".",
            model, FuelCat_Name(catalog));
        return (FuelCat_Status(catalog) = FIRE_STATUS_ERROR);
    }

    byrams = Fuel_ResidenceTime(catalog,model)
           * Fuel_SpreadAny(catalog,model)
           * Fuel_RxIntensity(catalog,model)
           / 60.;

    /* Flame length is requested. */
    if ( (which & FIRE_FLAME) )
    {
        if ( byrams < Smidgen )
        {
            Fuel_FlameLength(catalog,model) = 0.;
        }
        else
        {
            /* Use lookup table if it exists & includes this intensity. */
            if ( (n = FuelCat_FlameClasses(catalog)) > 0
              && FuelCat_FlameArray(catalog)[n-1] > byrams )
            {
                hi = n-1;
                lo = 0;
                do {
                    mid = lo + (hi-lo)/2;
                    if ( FuelCat_FlameArray(catalog)[mid] > byrams )
                        hi = mid;
                    else
                        lo = mid + 1;
                } while (lo != hi);
                Fuel_FlameLength(catalog,model) =
                    FuelCat_FlameStep(catalog) * (lo+1);
            }
            /* otherwise compute flame length from scratch. */
            else
            {
                Fuel_FlameLength(catalog,model) = 0.45 * pow(byrams, 0.46);
            }
        }
    }

    /* Scorch height is requested. */
    if ( (which & FIRE_SCORCH) )
    {
        if ( byrams < Smidgen )
        {
            Fuel_ScorchHeight(catalog,model) = 0.;
        }
        else
        {
            mph = Fuel_WindSpeed(catalog,model) / 88.;
            Fuel_ScorchHeight(catalog,model) =
                pow(byrams, 1.166667) / sqrt(byrams + (mph * mph * mph));
        /*  Fuel_ScorchHeight(catalog,model) *= (63. / (140. - temp_f) ); */
        }
    }

    return (FuelCat_Status(catalog) = FIRE_STATUS_OK);
}

/*
 *******************************************************************************
 *
 *  Fire_FlameLengthTable()
 *
 *  Description
 *      Creates a flame length lookup table containing "flameClasses" classes
 *      with each class spanning "flameStep" feet.
 *
 *  Discussion
 *      Since flame length is strictly an output variable (e.g., it is never
 *      used as the basis for subsequent computations), we can usually afford
 *      to round it to some precision that makes sense to fire managers.
 *      Usually this will be in 1 foot or perhaps 6 inch increments.  The call
 *
 *
 *      creates a flame length table for flame lengths of 1 through 500 feet.
 *
 *      Fire_SpreadAtAzimuth() uses the flame table (if one is defined for the
 *      catalog) to avoid using the costly pow() function for highly iterative
 *      flame length calculations, saving a considerable amount of processing
 *      time.  Fire_SpreadAtAzimuth() will still use the pow() function to
 *      compute flame length if (1) a flame length table is not defined,
 *      (2) the fireline intensity exceeds the upper limit of the currently
 *      defined flame length table, or (3) the flame length table becomes
 *      undefined by a Fire_FlameLengthTable(catalog, 0, 0.) call.
 *
 *
 *  Examples
 *      Fire_FlameLengthTable(catalog, 200, 1.0);
 *          Creates a table for flame lengths of 1 through 200 feet in 1-foot
 *          intervals.  Any previously defined flame length table for this
 *          fuel catalog is destroyed.
 *
 *      Fire_FlameLengthTable(catalog, 500, 0.5);
 *          Creates a table for flame lengths of 0.5 through 250 feet in 6-inch
 *          intervals.  ANy previously defined flame length table for this
 *          fuel catalog is destroyed.
 *
 *      Fire_FlameLengthTable(catalog, 0, 0.);
 *          Destroys any existing flame length table for this catalog, and
 *          forces actual flame length computation using pow() function.
 *
 *  Side Effects
 *      If a flame length table currently exists, it is destroyed, and the
 *      FuelCat_FlameArray(), FuelCat_FlameClasses(), and
 *      FuelCat_FlameStep() are set to NULL, 0, and 0.0, respectively.
 *
 *      If fireClasses > 0, allocates a flame length table and fills it with
 *      the fireline intensity associated with the upper limit of each flame
 *      length class.  The FuelCat_FlameArray(), FuelCat_FlameClasses(), and
 *      FuelCat_FlameStep() are then updated.
 *
 *  Function Returns
 *      FIRE_STATUS_OK or FIRE_STATUS_ERROR.
 *      Return status and error text are stored in the Fire Catalog's buffers.
 *
 *******************************************************************************
 */

int
Fire_FlameLengthTable ( FuelCatalogPtr catalog, size_t flameClasses, double flameStep )
   // FuelCatalogPtr catalog;     /* FuelCatalogData instance pointer           */
   // size_t  flameClasses;       /* number of flame length classes             */
   // double  flameStep;          /* flame length step value per class          */
{
    double power, flame;
    size_t i;

    /* Validate the catalog. */
    assert(catalog!= NULL && FuelCat_MagicCookie(catalog)==FIRE_CATALOG_MAGIC);

    /* If a flame table already exists, destroy it. */
    if ( FuelCat_FlameArray(catalog) )
    {
        free(FuelCat_FlameArray(catalog));
        FuelCat_FlameArray(catalog)   = NULL;
        FuelCat_FlameClasses(catalog) = 0;
        FuelCat_FlameStep(catalog)    = 0.0;
    }

    /* If flameClasses is zero, simply return. */
    if ( flameClasses == 0 )
        return (FuelCat_Status(catalog) = FIRE_STATUS_OK);

    /* Otherwise create a new flame table. */
    if ( (FuelCat_FlameArray(catalog) = (double *)
        calloc(flameClasses, sizeof(double))) == NULL )
    {
        sprintf(FuelCat_Error(catalog),
            "Fire_FlameLengthTable(): imposible asignar tabla de longitud de llama con %d clases de %f pies.",
            flameClasses, flameStep);
        return (FuelCat_Status(catalog) = FIRE_STATUS_ERROR);
    }

    /* Fill the array. */
    power = 1. / .46;
    for ( i=0; i<flameClasses; i++ )
    {
        flame = flameStep * (i+1);
        FuelCat_FlameArray(catalog)[i] = pow((flame / .45), power);
    }
    FuelCat_FlameClasses(catalog) = flameClasses;
    FuelCat_FlameStep(catalog)    = flameStep;

    return (FuelCat_Status(catalog) = FIRE_STATUS_OK);
}

/*
 *******************************************************************************
 *
 *  Fire_FuelCatalogCreate()
 *
 *  Description
 *      Creates a new fuel model catalog capable of holding fuel models with
 *      id's in the range [0..maxModel].
 *      The catalog is filled by subsequent calls to Fire_FuelModelCreate().
 *
 *  Side Effects
 *      Allocates a new FuelCatalogData structure.
 *      Allocates an error text buffer for the catalog.
 *      Allocates a name for the catalog.
 *      Allocates an array of pointers to FuelData structures (the FuelData
 *      structures themselves are allocated by Fire_FuelModelCreate() and
 *      their pointers are stored here).
 *
 *  Notes
 *      The FuelCatalog contains a dynamically-allocated array of pointers
 *      to FuelData blocks.  These pointers are initially NULL and are
 *      subsequently assigned by Fire_FuelModelCreate().  The array provides
 *      the programmer with a means of directly accessing fuel models via
 *      their model number, which is handy when simulating fire growth.
 *
 *  Function Returns
 *      While most FireLib functions return a status code, this one returns
 *      a pointer to the new FuelCatalogData on success or NULL if unable
 *      to allocate any of the dynamic structures.
 *
 *******************************************************************************
 */

FuelCatalogPtr
Fire_FuelCatalogCreate ( char *name, size_t maxModels )
   // char  *name;                /* FuelCatalogData instance name */
   // size_t maxModels;           /* maximum modelId allowed in this catalog */
{
    FuelCatalogPtr catalog;
    static char *blank = {""};

    /* Catch a NULL name. */
    if ( name == NULL )
        name = blank;

    /* Allocate the FireCatalogData structure. */
    if ( (catalog = (FuelCatalogPtr) malloc(sizeof(FuelCatalogData))) == NULL )
    {
        fprintf(stderr,
            "Fire_FuelCatalogCreate(): imposible asignar el objeto \"%s\" del catálogo de combustibles.\n",
            name);
        return (NULL);
    }

    /* Assign the magic cookie right away. */
    FuelCat_MagicCookie(catalog) = FIRE_CATALOG_MAGIC;

    /* Allocate and store the catalog instance name. */
    if ( (FuelCat_Name(catalog) = strdup(name)) == NULL )
    {
        fprintf(stderr,
            "Fire_FuelCatalogCreate(): imposible duplicar el nombre \"%s\" del catálogo de combustibles.\n",
            name);
        free(catalog);
        return (NULL);
    }

    /* Allocate the FireCatalogData error message buffer. */
    if ( (FuelCat_Error(catalog) =
        (char *) calloc(FIRE_ERROR_BUFFER_SIZE, sizeof(char))) == NULL )
    {
        fprintf(stderr,
            "Fire_FuelCatalogCreate(): imposible asignar el bufer de error  \"%s\" del catálogo de combustibles.\n",
            name);
        free(FuelCat_Name(catalog));
        free(catalog);
        return (NULL);
    }
    FuelCat_Status(catalog) = FIRE_STATUS_ERROR;

    /* Allocate a FuelModelPtr array to handle models [0..maxModels]. */
    maxModels++;
    FuelCat_MaxModels(catalog) = maxModels;
    if ( (FuelCat_ModelArray(catalog) = (FuelModelPtr *)
        calloc(FuelCat_MaxModels(catalog), sizeof(FuelModelPtr))) == NULL )
    {
        fprintf(stderr,
            "Fire_FuelCatalogCreate(): imposible asignar \"%s\" con %d modelos de combustible del catálogo de combustibles.\n",
            name, maxModels);
        free(FuelCat_Error(catalog));
        free(FuelCat_Name(catalog));
        free(catalog);
        return (NULL);
    }

    /* Initialize variables and return ptr to this instance. */
    FuelCat_FlameArray(catalog)   = NULL;
    FuelCat_FlameClasses(catalog) = 0;
    FuelCat_FlameStep(catalog)    = 0.0;
    FuelCat_Status(catalog)       = FIRE_STATUS_OK;
    return (catalog);
}

/*
 *******************************************************************************
 *
 *  Fire_FuelCatalogCreateStandard()
 *
 *  Description
 *      Creates a new fuel model catalog capable of holding fuel models with
 *      id's in the range [0..maxModel].
 *      The catalog is then filled with the 13 standard fire behavior fuel
 *      models.  Other models may be added by subsequent calls to
 *      Fire_FuelModelCreate().
 *
 *  Side Effects
 *      Allocates a new FuelCatalogData structure.
 *      Fills the catalog with standard fuels models 0-13.
 *
 *  Function Returns
 *      While most FireLib functions return a status code, this one returns
 *      a pointer to the new FuelCatalogData on success, or NULL if unable
 *      to allocate any of the dynamic structures.
 *
 *******************************************************************************
 */

FuelCatalogPtr
Fire_FuelCatalogCreateStandard (char *name, size_t maxModels )
   // char  *name;                /* FuelCatalogData instance name */
   // size_t maxModels;           /* maximum modelId allowed in this catalog */
{
    FuelCatalogPtr catalog;
    double stot, seff, heat, dens, adjust;
    size_t m, p;

    /* Fuel model definitions. */
    typedef struct {
        char *name; double depth; double mext; size_t maxParticles; char *desc;
    } StandardModels;

    StandardModels M[14] = {
        {"NoFuel", 0.1, 0.01, 0, "No Combustible Fuel" },
        {"NFFL01", 1.0, 0.12, 1, "Short Grass (1 ft)" },
        {"NFFL02", 1.0, 0.15, 4, "Timber (grass & understory)" },
        {"NFFL03", 2.5, 0.25, 1, "Tall Grass (2.5 ft)" },
        {"NFFL04", 6.0, 0.20, 4, "Chaparral (6 ft)" },
        {"NFFL05", 2.0, 0.20, 3, "Brush (2 ft)" },
        {"NFFL06", 2.5, 0.25, 3, "Dormant Brush & Hardwood Slash" },
        {"NFFL07", 2.5, 0.40, 4, "Southern Rough" },
        {"NFFL08", 0.2, 0.30, 3, "Closed Timber Litter" },
        {"NFFL09", 0.2, 0.25, 3, "Hardwood Litter" },
        {"NFFL10", 1.0, 0.25, 4, "Timber (litter & understory)" },
        {"NFFL11", 1.0, 0.15, 3, "Light Logging Slash" },
        {"NFFL12", 2.3, 0.20, 3, "Medium Logging Slash" },
        {"NFFL13", 3.0, 0.25, 3, "Heavy Logging Slash" }
    };

    /* Fuel particle definitions. */
    typedef struct {
        size_t model; size_t type; double load; double savr;
    } StandardParticle;

    static StandardParticle P[39] = {
        { 1, FIRE_TYPE_DEAD, 0.0340, 3500.},
        { 2, FIRE_TYPE_DEAD, 0.0920, 3000.},
        { 2, FIRE_TYPE_DEAD, 0.0460, 109.},
        { 2, FIRE_TYPE_DEAD, 0.0230, 30.},
        { 2, FIRE_TYPE_HERB, 0.0230, 1500.},
        { 3, FIRE_TYPE_DEAD, 0.1380, 1500.},
        { 4, FIRE_TYPE_DEAD, 0.2300, 2000.},
        { 4, FIRE_TYPE_DEAD, 0.1840, 109.},
        { 4, FIRE_TYPE_DEAD, 0.0920, 30.},
        { 4, FIRE_TYPE_WOOD, 0.2300, 1500.},
        { 5, FIRE_TYPE_DEAD, 0.0460, 2000.},
        { 5, FIRE_TYPE_DEAD, 0.0230, 109.},
        { 5, FIRE_TYPE_WOOD, 0.0920, 1500.},
        { 6, FIRE_TYPE_DEAD, 0.0690, 1750.},
        { 6, FIRE_TYPE_DEAD, 0.1150, 109.},
        { 6, FIRE_TYPE_DEAD, 0.0920, 30.},
        { 7, FIRE_TYPE_DEAD, 0.0520, 1750.},
        { 7, FIRE_TYPE_DEAD, 0.0860, 109.},
        { 7, FIRE_TYPE_DEAD, 0.0690, 30.},
        { 7, FIRE_TYPE_WOOD, 0.0170, 1550.},
        { 8, FIRE_TYPE_DEAD, 0.0690, 2000.},
        { 8, FIRE_TYPE_DEAD, 0.0460, 109.},
        { 8, FIRE_TYPE_DEAD, 0.1150, 30.},
        { 9, FIRE_TYPE_DEAD, 0.1340, 2500.},
        { 9, FIRE_TYPE_DEAD, 0.0190, 109.},
        { 9, FIRE_TYPE_DEAD, 0.0070, 30.},
        {10, FIRE_TYPE_DEAD, 0.1380, 2000.},
        {10, FIRE_TYPE_DEAD, 0.0920, 109.},
        {10, FIRE_TYPE_DEAD, 0.2300, 30.},
        {10, FIRE_TYPE_WOOD, 0.0920, 1500.},
        {11, FIRE_TYPE_DEAD, 0.0690, 1500.},
        {11, FIRE_TYPE_DEAD, 0.2070, 109.},
        {11, FIRE_TYPE_DEAD, 0.2530, 30.},
        {12, FIRE_TYPE_DEAD, 0.1840, 1500.},
        {12, FIRE_TYPE_DEAD, 0.6440, 109.},
        {12, FIRE_TYPE_DEAD, 0.7590, 30.},
        {13, FIRE_TYPE_DEAD, 0.3220, 1500.},
        {13, FIRE_TYPE_DEAD, 1.0580, 109.},
        {13, FIRE_TYPE_DEAD, 1.2880, 30.},
    };

    /* First, create the catalog. */
    if ( maxModels < 13 )
        maxModels = 13;
    if ( (catalog = Fire_FuelCatalogCreate(name, maxModels)) == NULL )
        return (NULL);

    /* Second, create all 14 models. */
    adjust = 1.0;
    for ( m=0; m<14; m++ )
    {
        if ( Fire_FuelModelCreate(catalog, m, M[m].name, M[m].desc, M[m].depth,
            M[m].mext, adjust, M[m].maxParticles) != FIRE_STATUS_OK )
        {
            fprintf(stderr, "%s\n", FuelCat_Error(catalog));
            Fire_FuelCatalogDestroy(catalog);
            return (NULL);
        }
    }

    /* Finally, add all the fuel particles. */
    stot   = 0.0555;
    seff   = 0.0100;
    heat   = 8000.0;
    dens   = 32.0;
    for ( p=0; p<39; p++ )
    {
        if ( Fire_FuelParticleAdd(catalog, P[p].model, P[p].type, P[p].load,
            P[p].savr, dens, heat, stot, seff) != FIRE_STATUS_OK )
        {
            fprintf(stderr, "%s\n", FuelCat_Error(catalog));
            Fire_FuelCatalogDestroy(catalog);
            return (NULL);
        }
    }

    return (catalog);
}

/*
 *******************************************************************************
 *
 *  Fire_FuelCatalogDestroy()
 *
 *  Description
 *      Destroys the fuel catalog and all its associated models and particles.
 *
 *  Side Effects
 *      Destroys all FuelData instances belonging to the catalog.
 *      Frees the array of pointers to FuelData structures.
 *      Frees the catalog name.
 *      Frees the catalog error text buffer.
 *      Frees the FuelCatalog instance.
 *
 *  Function Returns
 *      FIRE_STATUS_OK or FIRE_STATUS_ERROR.
 *      Return status and error text are stored in the Fire Catalog's buffers.
 *
 *******************************************************************************
 */

int
Fire_FuelCatalogDestroy ( FuelCatalogPtr catalog )
   // FuelCatalogPtr catalog;     /* FuelCatalogData instance to destroy. */
{
    size_t model;

    /* Validate the catalog. */
    assert(catalog!=NULL && FuelCat_MagicCookie(catalog)==FIRE_CATALOG_MAGIC);

    /* First destroy all the fuel models in this catalog. */
    /* The free the catalog's array of FuelData pointers. */
    if ( FuelCat_ModelArray(catalog) )
    {
        for ( model=0; model <= FuelCat_MaxModels(catalog); model++ )
        {
            if ( FuelCat_ModelPtr(catalog,model) )
                Fire_FuelModelDestroy(catalog, model);
        }
        free(FuelCat_ModelArray(catalog));
        FuelCat_ModelArray(catalog) = NULL;
    }

    /* Next destroy the flame length table. */
    if ( FuelCat_FlameArray(catalog) )
    {
        free(FuelCat_FlameArray(catalog));
        FuelCat_FlameArray(catalog)   = NULL;
        FuelCat_FlameClasses(catalog) = 0;
        FuelCat_FlameStep(catalog)    = 0.0;
    }

    /* Then free the name and error buffer for this FuelCatalogData instance. */
    if ( FuelCat_Error(catalog) )
    {
        free(FuelCat_Error(catalog));
        FuelCat_Error(catalog) = NULL;
    }

    if ( FuelCat_Name(catalog) )
    {
        free(FuelCat_Name(catalog));
        FuelCat_Name(catalog) = NULL;
    }

    /* Finally,free the FuelCatalogData instance and return. */
    free(catalog);

    return (FuelCat_Status(catalog) = FIRE_STATUS_OK);
}

/*
 *******************************************************************************
 *
 *  Fire_FuelModelCreate()
 *
 *  Description
 *      Creates a new fuel model able to hold maxParticles fuel particles.
 *      Fuel particles are subsequently added by Fire_FuelParticleAdd().
 *
 *  Side Effects
 *      Any existing fuel model with modelId in the Fuel Catalog is destroyed.
 *      Allocates the fuel model's FuelData block.
 *      Allocates the fuel model's name string.
 *      Allocates the fuel model's description string.
 *      Allocates the fuel model's fuel particle pointer array of maxParticles
 *      (the FuelParticleData blocks are actually allocated within
 *      Fire_FuelparticleAdd() and thier pointers stored in this array).
 *      The fuel model's address is stored in the fuel catalog's pointer array.
 *
 *  Function Returns
 *      FIRE_STATUS_OK or FIRE_STATUS_ERROR.
 *      Return status and error text are stored in the Fire Catalog's buffers.
 *
 *******************************************************************************
 */

int
Fire_FuelModelCreate (FuelCatalogPtr catalog, size_t model, char *name, char *desc, double depth, double mext, double adjust, size_t maxParticles)
   // FuelCatalogPtr catalog;     /* FuelCatalogData instance */
   // size_t  model;              /* fuel model number            [0-maxModels] */
   // char   *name;               /* short name */
   // char   *desc;               /* longer description */
   // double  depth;              /* bed depth                             (ft) */
   // double  mext;               /* moisture of extinction                (dl) */
   // double  adjust;             /* spread adjustment factor              (dl) */
   // size_t  maxParticles;       /* maximum number of fuel model particles     */
{
    static char *blank = {""};
    size_t particle;

    /* Validate the catalog. */
    assert(catalog!= NULL && FuelCat_MagicCookie(catalog)==FIRE_CATALOG_MAGIC);

    /* Make sure model id is within range. */
    if ( model > FuelCat_MaxModels(catalog) )
    {
        sprintf(FuelCat_Error(catalog),
            "Fire_FuelModelCreate(): fuel model \"%s\" number %d exceeds fuel catalog \"%s\" range [0..%d].",
            name, model, FuelCat_Name(catalog), FuelCat_MaxModels(catalog));
        return (FuelCat_Status(catalog) = FIRE_STATUS_ERROR);
    }

    /* Validate depth and mext. */
    if ( depth < Smidgen )
    {
        sprintf(FuelCat_Error(catalog),
            "Fire_FuelModelCreate(): el modelo de combustible \"%s\" número %d de ancho %5.4f es demasiado pequeño.",
            name, model, depth);
        return (FuelCat_Status(catalog) = FIRE_STATUS_ERROR);
    }

    if ( mext < Smidgen )
    {
        sprintf(FuelCat_Error(catalog),
            "Fire_FuelModelCreate(): el modelo de combustible \"%s\" número %d de humedad de extinción %5.4f es demasiado pequeño.",
            name, model, mext);
        return (FuelCat_Status(catalog) = FIRE_STATUS_ERROR);
    }

    /* If this model already exists, delete it. */
    if ( FuelCat_ModelPtr(catalog,model) )
        Fire_FuelModelDestroy(catalog, model);

    /* Allocate the model's FuelData structure. */
    if ( maxParticles < 1 )
        maxParticles = 1;
    if ( (FuelCat_ModelPtr(catalog,model) =
                (FuelModelPtr) calloc(1, sizeof(FuelModelData))) == NULL
      || (Fuel_ParticleArray(catalog,model) =
                (PartPtr *) calloc(maxParticles, sizeof(PartPtr))) == NULL )
    {
        Fire_FuelModelDestroy(catalog, model);
        sprintf(FuelCat_Error(catalog),
            "Fire_FuelModelCreate(): imposible asignar el modelos de combustible \"%s\" número %d para el catálogo de combustibles \"%s\".",
            name, model, FuelCat_Name(catalog));
        return (FuelCat_Status(catalog) = FIRE_STATUS_ERROR);
    }

    /* Catch NULL names and descriptions. */
    if ( name == NULL )
        name = blank;
    if ( desc == NULL )
        desc = blank;

    /* Store remaining attributes. */
    Fuel_Model(catalog,model)            = model;
    Fuel_Depth(catalog,model)            = depth;
    Fuel_Mext(catalog,model)             = mext;
    Fuel_SpreadAdjustment(catalog,model) = adjust;
    Fuel_Name(catalog,model)             = strdup(name);
    Fuel_Desc(catalog,model)             = strdup(desc);
    Fuel_CombustionFlag(catalog,model)   = 0;
    Fuel_MaxParticles(catalog,model)     = maxParticles;
    Fuel_Particles(catalog,model)        = 0;
    for ( particle=0; particle<Fuel_MaxParticles(catalog,model); particle++ )
        Fuel_ParticlePtr(catalog,model,particle) = NULL;

    return (FuelCat_Status(catalog) = FIRE_STATUS_OK);
}

/*
 *******************************************************************************
 *
 *  Fire_FuelModelDestroy()
 *
 *  Description
 *      Deletes the specified fuel model.
 *      Note: this is one of only 3 functions that use the modelId instead
 *      of a FuelData pointer to identify the model.
 *
 *  Side Effects
 *      Free's all fuel particles added to the fuel model.
 *      Free's the fuel particle pointer array.
 *      Free's the fuel model's name.
 *      Free's the fuel model's description.
 *      Free's the fuel model's FuelData block.
 *      Sets the Fuel Catalog's pointer for this fuel model to NULL.
 *
 *  Function Returns
 *      FIRE_STATUS_OK or FIRE_STATUS_ERROR.
 *      Return status and error text are stored in the Fire Catalog's buffers.
 *
 *******************************************************************************
 */

int
Fire_FuelModelDestroy ( FuelCatalogPtr catalog, size_t model )
   // FuelCatalogPtr catalog;     /* FuelCatalogData instance pointer           */
   // size_t         model;       /* fuel model id number         [0-maxModels] */
{
    size_t particle;

    /* Validate the catalog. */
    assert(catalog!= NULL && FuelCat_MagicCookie(catalog)==FIRE_CATALOG_MAGIC);

    /* Make sure model id is within range and exists. */
    if ( ! Fire_FuelModelExists(catalog,model) )
    {
        sprintf(FuelCat_Error(catalog),
            "Fire_FuelModelDestroy(): el modelo de combustible %d no existe en el catálogo de combustibles \"%s\".",
            model, FuelCat_Name(catalog));
        return (FuelCat_Status(catalog) = FIRE_STATUS_ERROR);
    }

    /* Free all the fuel model particles and their pointer array. */
    if ( Fuel_ParticleArray(catalog,model) )
    {
        for (particle=0; particle<Fuel_MaxParticles(catalog,model); particle++)
        {
            if ( Fuel_ParticlePtr(catalog,model,particle) )
            {
                free(Fuel_ParticlePtr(catalog,model,particle));
                Fuel_ParticlePtr(catalog,model,particle) = NULL;
            }
        }
        free(Fuel_ParticleArray(catalog,model));
        Fuel_ParticleArray(catalog,model) = NULL;
    }

    /* Free the fuel model name and description. */
    if ( Fuel_Name(catalog,model) )
    {
        free(Fuel_Name(catalog,model));
        Fuel_Name(catalog,model) = NULL;
    }

    if ( Fuel_Desc(catalog,model) )
    {
        free(Fuel_Desc(catalog,model));
        Fuel_Desc(catalog,model) = NULL;
    }

    /* Now free the FuelData instance and reset its catalog entry. */
    free(FuelCat_ModelPtr(catalog,model));
    FuelCat_ModelPtr(catalog,model) = NULL;

    return (FuelCat_Status(catalog) = FIRE_STATUS_OK);
}

/*
 *******************************************************************************
 *
 *  Fire_FuelModelExists()
 *
 *  Description
 *      Performs a sanity check to make sure the catalog pointer is valid
 *      and the fuel model number is within range and exists.
 *
 *  Side Effects
 *      None.
 *
 *  Function Returns
 *      1 if "model" exists, 0 if it is undefined.
 *
 *******************************************************************************
 */

int
Fire_FuelModelExists ( FuelCatalogPtr catalog, size_t model )
   // FuelCatalogPtr catalog;     /* FuelCatalogData instance pointer           */
   // size_t         model;       /* fuel model id number         [0-maxModels] */
{
    /* Validate the model number. */
    if ( model > FuelCat_MaxModels(catalog)
      || ! FuelCat_ModelPtr(catalog,model) )
        return (int) 0;

    return (int) 1;
}

/*
 *******************************************************************************
 *
 *  Fire_FuelParticleAdd()
 *
 *  Description
 *      Adds a fuel particle to the specified fuel model.
 *
 *  Side Effects
 *      A FuelParticleData is allocated and appended to the model's array.
 *      The fuel model's particle counter is incremented.
 *      The fuel model's combustion flag set to 0.
 *
 *  Function Returns
 *      FIRE_STATUS_OK or FIRE_STATUS_ERROR.
 *      Return status and error text are stored in the Fire Catalog's buffers.
 *
 *******************************************************************************
 */

int
Fire_FuelParticleAdd ( FuelCatalogPtr catalog, size_t model, size_t type, double load, double savr, double dens, double heat, double stot, double seff )
   // FuelCatalogPtr catalog;     /* FuelCatalogData instance pointer           */
   // size_t  model;              /* fuel model id number         [0-maxModels] */
   // size_t  type;               /* FIRE_TYPE_DEAD, _TYPE_HERB, or _TYPE_WOOD  */
   // double  load;               /* fuel load                        (lbs/ft2) */
   // double  savr;               /* surface-area-to-volume ratio     (ft2/ft3) */
   // double  dens;               /* density                          (lbs/ft3) */
   // double  heat;               /* heat of combustion               (btus/lb) */
   // double  stot;               /* total silica content               (lb/lb) */
   // double  seff;               /* effective silica content           (lb/lb) */
{
    static double Size_boundary[FIRE_SIZE_CLASSES] =
        {1200., 192., 96., 48., 16., 0.};
    size_t particle, size;

    /* Validate the catalog. */
    assert(catalog!= NULL && FuelCat_MagicCookie(catalog)==FIRE_CATALOG_MAGIC);

    /* Validate the fuel model. */
    if ( ! Fire_FuelModelExists(catalog,model) )
    {
        sprintf(FuelCat_Error(catalog),
            "Fire_FuelParticleAdd(): el modelo de combustible %d no existe en el catálogo de combustibles \"%s\".",
            model, FuelCat_Name(catalog));
        return (FuelCat_Status(catalog) = FIRE_STATUS_ERROR);
    }

    /* Validate the "type" parameter. */
    if ( type != FIRE_TYPE_DEAD
      && type != FIRE_TYPE_HERB
      && type != FIRE_TYPE_WOOD )
    {
        sprintf(FuelCat_Error(catalog),
            "Fire_FuelParticleAdd(): el modelo de combustible %d de tipo de valor (arg #3) no es FIRE_TYPE_DEAD, FIRE_TYPE_HERB, o FIRE_TYPE_WOOD.",
            model);
        return (FuelCat_Status(catalog) = FIRE_STATUS_ERROR);
    }

    /* Allocate a new FuelParticle */
    particle = Fuel_Particles(catalog,model);
    if ( (Fuel_ParticlePtr(catalog,model,particle) =
        (PartPtr) calloc(1, sizeof(FuelParticleData))) == NULL )
    {
        sprintf(FuelCat_Error(catalog),
            "Fire_FuelParticleAdd(): imposible asignar la partícula de combustible al modelo de combustible \"%s\" número %d en el catálogo de combustibles \"%s\".",
            Fuel_Name(catalog,model), model, FuelCat_Name(catalog));
        return (FuelCat_Status(catalog) = FIRE_STATUS_ERROR);
    }

    /* Store the input particle attributes. */
    Fuel_Type(catalog,model,particle)       = type;
    Fuel_Load(catalog,model,particle)       = load;
    Fuel_Savr(catalog,model,particle)       = savr;
    Fuel_Density(catalog,model,particle)    = dens;
    Fuel_Heat(catalog,model,particle)       = heat;
    Fuel_SiTotal(catalog,model,particle)    = stot;
    Fuel_SiEffective(catalog,model,particle)= seff;

    /* Fuel life category. */
    Fuel_Live(catalog,model,particle) =
        (type==FIRE_TYPE_DEAD) ? FIRE_LIFE_DEAD : FIRE_LIFE_LIVE;

    /* Fuel particle surface area. */
    Fuel_SurfaceArea(catalog,model,particle) =
        (dens > Smidgen) ? load * savr / dens : 0.;

    /* Particle SAVR exponent factor. */
    Fuel_SigmaFactor(catalog,model,particle) =
        (savr > Smidgen) ? exp(-138. / savr) : 0.;

    /* Particle size class. */
    for ( size=0; savr < Size_boundary[size]; size++ )
        /* NOTHING */ ;
    Fuel_SizeClass(catalog,model,particle) = size;

    /* Initialize particle attributes that are bed & environ dependent. */
    Fuel_AreaWtg(catalog,model,particle)     = 0.;
    Fuel_SizeAreaWtg(catalog,model,particle) = 0.;
    Fuel_Moisture(catalog,model,particle)    = 0.;

    /* Increment the fuel model's particle counter and reset it flag. */
    Fuel_Particles(catalog,model)++;
    Fuel_CombustionFlag(catalog,model) = 0;

    return (FuelCat_Status(catalog) = FIRE_STATUS_OK);
}

/*
 *******************************************************************************
 * End of fireLib.c
 *******************************************************************************
 */
