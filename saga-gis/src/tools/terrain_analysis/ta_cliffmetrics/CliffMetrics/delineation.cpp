/*!
 *
 * \file delineation.cpp
 * \brief The start-of-simulation routine
 * \details TODO A more detailed description of this routine.
 * \author Andres Payo, David Favis-Mortlock, Martin Husrt, Monica Palaseanu-Lovejoy
 * \date 2020
 * \copyright GNU General Public License
 *
 */

/*==============================================================================================================================

 This file is part of CliffMetrics.

 CliffMetrics is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

==============================================================================================================================*/
#include <iostream>
using std::cout;
using std::cerr;
using std::endl;
using std::ios;

#include <cfloat>

#include "cliffmetrics.h"
#include "delineation.h"
#include "raster_grid.h"
#include "coast.h"


/*==============================================================================================================================

 The CDelineation constructor

==============================================================================================================================*/
CDelineation::CDelineation(void)
{
   // Initialization
   m_bNormalsSave                                  =
   m_bCoastSave                                    =
   m_bCliffTopSave                                 = 
   m_bCliffToeSave                                 = 
   m_bRasterCoastlineSave                          =
   m_bRasterNormalSave                             =
   m_bOutputProfileData                            = true;

   m_bInvalidNormalsSave                           = true;
   m_bCoastCurvatureSave                           = false;
   m_bGDALCanWriteInt32                            =
   m_bScaleRasterOutput                            =
   m_bWorldFile                                    = false;

   m_bGDALCanCreate                                = true;

   m_papszGDALRasterOptions                        =
   m_papszGDALVectorOptions                        = NULL;

   m_nCoastSmooth                                  =
   m_nCoastSmoothWindow                            =
   m_nSavGolCoastPoly                              =
   m_nProfileSmoothWindow                          =
   m_nCoastNormalAvgSpacing                        =
   m_nCoastCurvatureInterval                       =
   m_nCapeNormals                                  =
   m_nGISSave                                      =
   m_nUSave                                        =
   m_nThisSave                                     =
   m_nXGridMax                                     =
   m_nYGridMax                                     =
   m_nCoastMax                                     =
   m_nCoastMin                                     = 0;
   
#if !defined(_SAGA_MSW) && !defined(_SAGA_LINUX)
   m_GDALWriteIntDataType                          =
   m_GDALWriteFloatDataType                        = GDT_Unknown;
#endif // #if !defined(_SAGA_MSW) && !defined(_SAGA_LINUX)

   m_lGDALMaxCanWrite                              =
   m_lGDALMinCanWrite                              = 0;

   m_ulTimestep                                    =
   m_ulTotTimestep                                 =
   m_ulNumCells                                    =
   m_ulThisTimestepNumSeaCells                     =
   m_ulThisTimestepNumCoastCells                   = 0;

   for (int i = 0; i < NRNG; i++)
      m_ulRandSeed[i]  = 0;


   m_dEleTolerance                              = 1e-16;  // must be larger than zero, so it is initialized with a very small value but still larger than 0
   m_dNorthWestXExtCRS                          =
   m_dNorthWestYExtCRS                          =
   m_dSouthEastXExtCRS                          =
   m_dSouthEastYExtCRS                          =
   m_dExtCRSGridArea                            =
   m_dCellSide                                  =
   m_dCellDiagonal                              =
   m_dInvCellSide                               =
   m_dInvCellDiagonal                           =
   m_dCellArea                                  =
   m_dClkLast                                   =
   m_dCPUClock                                  =
   m_dStillWaterLevel                           =
   m_dCoastNormalAvgSpacing                     =
   m_dCoastNormalLength                         =
   m_dProfileMaxSlope                           =
   m_dSimpleSmoothWeight                        = 0;

   m_dMinSWL                                    = DBL_MAX;
   m_dMaxSWL                                    = DBL_MIN;

   for (int i = 0; i < 2; i++)
   {
      m_ulRState[i].s1                       =
      m_ulRState[i].s2                       =
      m_ulRState[i].s3                       = 0;
   }

   m_tSysStartTime                           =
   m_tSysEndTime                             = 0;

   m_pRasterGrid                             = NULL;
}

/*==============================================================================================================================

 The CDelineation destructor

==============================================================================================================================*/
CDelineation::~CDelineation(void)
{
   // Close output files if open
   if (LogStream && LogStream.is_open())
      LogStream.close();

   if (OutStream && OutStream.is_open())
      OutStream.close();

   if (m_pRasterGrid)
      delete m_pRasterGrid;
}

double CDelineation::dGetSWL(void) const
{
   return m_dStillWaterLevel;
}

int CDelineation::nGetGridXMax(void) const
{
   return m_nXGridMax;
}

int CDelineation::nGetGridYMax(void) const
{
   return m_nYGridMax;
}


/*==============================================================================================================================

 The nDoSimulation member function of CDelineation sets up and runs the simulation

==============================================================================================================================*/
#if !defined(_SAGA_MSW) && !defined(_SAGA_LINUX)
int CDelineation::nDoDelineation(int nArg, char* pcArgv[])
{
#ifdef RANDCHECK
   CheckRand();
   return RTN_OK;
#endif

   // ================================================== initialization section ================================================
   // Hello, World!
   AnnounceStart();

   // Start the clock ticking
   StartClock();

   // Find out the folder in which the CliffMetrics executable sits, in order to open the .ini file (they are assumed to be in the same folder)
   if (! bFindExeDir(pcArgv[0]))
      return (RTN_ERR_CLIFFDIR);

   // Deal with command-line parameters
   int nRet = nHandleCommandLineParams(nArg, pcArgv);
   if (nRet != RTN_OK)
      return (nRet);

   // OK, we are off, tell the user about the licence
   AnnounceLicence();

   // Read the .ini file and get the name of the run-data file, and path for output etc.
   if (! bReadIni())
      return (RTN_ERR_INI);

   // We have the name of the run-data input file, so read it
   if (! bReadRunData())
      return (RTN_ERR_RUNDATA);

   // Check raster GIS output format
   if (! bCheckRasterGISOutputFormat())
      return (RTN_ERR_RASTER_GIS_OUT_FORMAT);

   // Check vector GIS output format
   if (! bCheckVectorGISOutputFormat())
      return (RTN_ERR_VECTOR_GIS_OUT_FORMAT);

   // Open log file
   if (! bOpenLogFile())
      return (RTN_ERR_LOGFILE);

   // Initialize the random number generators
   InitRand0(m_ulRandSeed[0]);
   InitRand1(m_ulRandSeed[1]);

   // If we are doing Savitzky-Golay smoothing of the vector coastline(s), calculate the filter coefficients
   if (m_nCoastSmooth == SMOOTH_SAVITZKY_GOLAY)
      CalcSavitzkyGolayCoeffs();

   // Create the raster grid object
   m_pRasterGrid = new CRasterGrid(this);

   // Read in the DTM (NOTE MUST HAVE THIS FILE) and create the raster grid, then read in the DTM data to the array
   AnnounceReadDTM();
   nRet = nReadDTMData();
   if (nRet != RTN_OK)
      return nRet;

   // If we are using the default cell spacing, then now that we know the size of the raster cells, we can set the size of profile spacing in m
   if (m_dCoastNormalAvgSpacing == 0)
      m_dCoastNormalAvgSpacing = MIN_PROFILE_SPACING * m_dCellSide;
   else
   {
      // The user specified a profile spacing, is this too small?
      m_nCoastNormalAvgSpacing = m_dCoastNormalAvgSpacing / m_dCellSide;

     //if (m_nCoastNormalAvgSpacing < m_dCellSide)
     //{
     //    cerr << ERR << "polygon creation works poorly if profile spacing is less than " << m_dCellSide << " x the size of raster cells" << endl;
         //return RTN_ERR_PROFILESPACING;
     //}
   }

   // May wish to read in the shoreline vector file instead of calculating it from the raster
   if (! m_strInitialCoastlineFile.empty())
   {
      AnnounceReadUserCoastLine();

      // Create a new coastline object
      CCoast CoastTmp;
      m_VCoast.push_back(CoastTmp);

      // Read in the points of user defined coastline
      nRet = nReadVectorCoastlineData();
      if (nRet != RTN_OK)
         return (nRet);
   }


   // Open OUT file
   OutStream.open(m_strOutFile.c_str(), ios::out | ios::trunc);
   if (! OutStream)
   {
      // Error, cannot open Out file
      cerr << ERR << "cannot open " << m_strOutFile << " for output" << endl;
      return (RTN_ERR_OUTFILE);
   }

   // Write beginning-of-run information to Out and Log files
   WriteStartRunDetails();

   // Start initializing
   AnnounceInitializing();

   // Misc initialization calcs
   m_ulNumCells = m_nXGridMax * m_nYGridMax;
   m_nCoastMax = COASTMAX * tMax(m_nXGridMax, m_nYGridMax);                                        // Arbitrary but probably OK
   m_nCoastMin = COASTMIN * m_dCoastNormalAvgSpacing / m_dCellSide;                                // Ditto
   m_nCoastCurvatureInterval = tMax(dRound(m_dCoastNormalAvgSpacing / (m_dCellSide * 2)), 2.0);    // Ditto

   
   // ===================================================== The main loop ======================================================
   // Tell the user what is happening
   AnnounceIsRunning();

      // Do per-timestep intialization: set up the grid cells ready for this timestep, also initialize per-timestep totals
      nRet = nInitGridAndCalcStillWaterLevel();
      if (nRet != RTN_OK)
         return nRet;

      // Next find out which cells are inundated and locate the coastline(s)
      nRet = nLocateSeaAndCoasts();
      if (nRet != RTN_OK)
         return nRet;

      // Create the coastline-normal profiles
      nRet = nCreateAllProfilesAndCheckForIntersection();
      if (nRet != RTN_OK)
         return nRet;

      // Locate the cliff top/toe and save profiles
      nRet = nLocateCliffTop();
         if (nRet != RTN_OK)
            return nRet;
	
     // Now save results, first the raster and vector GIS files
        // Save the values from the RasterGrid array into raster GIS files
        if (! bSaveAllRasterGISFiles())
           return (RTN_ERR_RASTER_FILE_WRITE);

        // Save the vector GIS files
        if (! bSaveAllVectorGISFiles())
           return (RTN_ERR_VECTOR_FILE_WRITE);

     // ================================================ End of main loop ======================================================

  // =================================================== post-loop tidying =====================================================
    // Tell the user what is happening
   AnnounceSimEnd();

   // Write end-of-run information to Out, Log and time-series files
   //nRet = nWriteEndRunDetails();
   if (nRet != RTN_OK)
      return (nRet);

   return RTN_OK;
} // end DoDelineation

#else // #if defined(_SAGA_MSW) || defined(_SAGA_LINUX)
int CDelineation::nDoDelineation(CSG_Parameters *pParameters)
{
	m_pParameters	= pParameters;	CSG_Parameters	&Parameters	= *pParameters;

	// ================================================== initialization section ================================================
	AnnounceStart();
	StartClock();
	AnnounceLicence();

	// Read the .ini file and get the name of the run-data file, and path for output etc.
	m_strDataPathName;	// The main input run-data filename
	m_strMailAddress;	// Email address, only useful if running under Linux/Unix

						// We have the name of the run-data input file, so read it   if (! bReadRunData())
	CSG_String	OutPath	= Parameters["OutPath"].asString();

	m_strOutPath	= OutPath.b_str();		// Path for CliffMetrics output

	if( !SG_Dir_Exists(OutPath) )
	{
		OutPath	= SG_Dir_Get_Temp();
	}

	m_strOutFile	= SG_File_Make_Path(OutPath, "cliffmetrics", "txt").b_str();	// Text output file names, don't change case
	m_strLogFile	= SG_File_Make_Path(OutPath, "cliffmetrics", "log").b_str();	// Text output file names, don't change case

	SG_UI_Msg_Add_Execution("\noutput file: ", false); SG_UI_Msg_Add_Execution(m_strOutFile.c_str(), false);
	SG_UI_Msg_Add_Execution("\nlog file: "   , false); SG_UI_Msg_Add_Execution(m_strLogFile.c_str(), false);

	m_dStillWaterLevel			= Parameters["StillWaterLevel"  ].asDouble();	//  Still water level (m) used to extract the shoreline
	m_nCoastSmooth				= Parameters["CoastSmooth"      ].asInt();	// Vector coastline smoothing algorithm: 0 = none, 1 = running mean, 2 = Savitsky-Golay
	m_nCoastSmoothWindow		= Parameters["CoastSmoothWindow"].asInt() * 2 + 1;	// Size of coastline smoothing window: must be odd
	m_nSavGolCoastPoly			= Parameters["SavGolCoastPoly"  ].asInt();	// Order of coastline profile smoothing polynomial for Savitsky-Golay: usually 2 or 4, max is 6

//	m_strInitialLandformFile;	// Optional shoreline shape file (can be blank)
	m_strRasterGISOutFormat;	// Raster GIS output format (note must retain original case). Blank means use same format as input DEM file (if possible)
	m_bWorldFile	= true;	// If needed, also output GIS raster world file?

	m_bScaleRasterOutput		= Parameters["ScaleRasterOutput"    ].asBool();	// If needed, scale GIS raster output values?
	m_strVectorGISOutFormat;	// Vector GIS output format (note must retain original case)
	m_bRandomCoastEdgeSearch	= Parameters["RandomCoastEdgeSearch"].asBool();	// Random edge for coastline search?
	m_dCoastNormalLength		= Parameters["CoastNormalLength"    ].asDouble();	// Length of coastline normals (m)
	m_dEleTolerance				= Parameters["EleTolerance"         ].asDouble();	// Vertical tolerance avoid false CliffTops/Toes

	// Initialize the random number generators
	//	m_ulRandSeed[0]	= Parameters["RandSeed"].asInt();	// Random number seed(s) MUST BE > 0!!!
	// Only one seed specified, so make all seeds the same
	//	for(int n=1; n < NRNG; n++)	m_ulRandSeed[n] = m_ulRandSeed[n-1];
	m_ulRandSeed[0]	= 280761;	// Random number seed(s) MUST BE > 0!!!
	m_ulRandSeed[1]	= 280761;	// Random number seed(s) MUST BE > 0!!!
	InitRand0(m_ulRandSeed[0]);
	InitRand1(m_ulRandSeed[1]);

	// reset output tables
	Parameters["PROFILES"].asTable()->Destroy();

	// Open log file
	if (! bOpenLogFile())
		return (RTN_ERR_LOGFILE);

	// If we are doing Savitzky-Golay smoothing of the vector coastline(s), calculate the filter coefficients
	if (m_nCoastSmooth == SMOOTH_SAVITZKY_GOLAY)
		CalcSavitzkyGolayCoeffs();

	// Create the raster grid object
	m_pRasterGrid = new CRasterGrid(this);

	// Read in the DTM (NOTE MUST HAVE THIS FILE) and create the raster grid, then read in the DTM data to the array
	int	nRet = nReadDTMData(Parameters["DEM"].asGrid());
	if (nRet != RTN_OK)
		return nRet;

	// If we are using the default cell spacing, then now that we know the size of the raster cells, we can set the size of profile spacing in m
	if (m_dCoastNormalAvgSpacing == 0)
		m_dCoastNormalAvgSpacing = MIN_PROFILE_SPACING * m_dCellSide;
	else
	{
		// The user specified a profile spacing, is this too small?
		m_nCoastNormalAvgSpacing = m_dCoastNormalAvgSpacing / m_dCellSide;

		//if (m_nCoastNormalAvgSpacing < m_dCellSide)
		//{
		//    cerr << ERR << "polygon creation works poorly if profile spacing is less than " << m_dCellSide << " x the size of raster cells" << endl;
		//return RTN_ERR_PROFILESPACING;
		//}
	}

	m_nCoastSeaHandiness	= Parameters["CoastSeaHandiness"].asInt();

	// May wish to read in the shoreline vector file instead of calculating it from the raster
	if( Parameters["COAST_INITIAL"].asShapes() )
	{
		AnnounceReadUserCoastLine();

		// Create a new coastline object
		CCoast CoastTmp;
		m_VCoast.push_back(CoastTmp);

		// Read in the points of user defined coastline
		nRet = nReadVectorCoastlineData(Parameters["COAST_INITIAL"].asShapes());
		if (nRet != RTN_OK)
			return (nRet);
	}

	// Open OUT file
	OutStream.open(m_strOutFile.c_str(), ios::out | ios::trunc);
	if (! OutStream)
	{
		// Error, cannot open Out file
		cerr << ERR << "cannot open " << m_strOutFile << " for output" << endl;
		return (RTN_ERR_OUTFILE);
	}

	// Write beginning-of-run information to Out and Log files
	WriteStartRunDetails();

	// Start initializing
	AnnounceInitializing();

	// Misc initialization calcs
	m_ulNumCells = m_nXGridMax * m_nYGridMax;
	m_nCoastMax = COASTMAX * tMax(m_nXGridMax, m_nYGridMax);                                        // Arbitrary but probably OK
	m_nCoastMin = COASTMIN * m_dCoastNormalAvgSpacing / m_dCellSide;                                // Ditto
	m_nCoastCurvatureInterval = tMax(dRound(m_dCoastNormalAvgSpacing / (m_dCellSide * 2)), 2.0);    // Ditto


	// ===================================================== The main loop ======================================================
	// Tell the user what is happening
	AnnounceIsRunning();

	// Do per-timestep intialization: set up the grid cells ready for this timestep, also initialize per-timestep totals
	nRet = nInitGridAndCalcStillWaterLevel();
	if (nRet != RTN_OK)
		return nRet;

	// Next find out which cells are inundated and locate the coastline(s)
	nRet = nLocateSeaAndCoasts();
	if (nRet != RTN_OK)
		return nRet;

	// Create the coastline-normal profiles
	nRet = nCreateAllProfilesAndCheckForIntersection();
	if (nRet != RTN_OK)
		return nRet;

	// Locate the cliff top/toe and save profiles
	nRet = nLocateCliffTop();
	if (nRet != RTN_OK)
		return nRet;

	// Now save results, first the raster and vector GIS files
	// Save the values from the RasterGrid array into raster GIS files
	if (! bSaveAllRasterGISFiles())
		return (RTN_ERR_RASTER_FILE_WRITE);

	// Save the vector GIS files
	if (! bSaveAllVectorGISFiles())
		return (RTN_ERR_VECTOR_FILE_WRITE);

	// ================================================ End of main loop ======================================================

	// =================================================== post-loop tidying =====================================================
	// Tell the user what is happening
	AnnounceSimEnd();

	// Write end-of-run information to Out, Log and time-series files
	//nRet = nWriteEndRunDetails();
	if (nRet != RTN_OK)
		return (nRet);

	return RTN_OK;
} // end DoDelineation
#endif // #if defined(_SAGA_MSW) || defined(_SAGA_LINUX)
