/*!
 *
 * \class CDelineation
 * \brief This class runs CliffMetrics simulations
 * \details TODO This is a more detailed description of the CDelineation class
 * \author Andres Payo, David Favis-Mortlock, Martin Husrt, Monica Palaseanu-Lovejoy
 * \date 2017
 * \copyright GNU General Public License
 *
 * \file delineation.h
 * \brief Contains CDelineation definitions
 *
 */

#ifndef DELINEATION_H
#define DELINEATION_H
/*===============================================================================================================================

 This file is part of CliffMetrics, the Coastal Modelling Environment.

 CliffMetrics is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

===============================================================================================================================*/
#include <fstream>
using std::ofstream;

#include <string>
using std::string;

#include <utility>
using std::pair;

#if !defined(_SAGA_MSW) && !defined(_SAGA_LINUX)
#include <gdal_priv.h>
#else // #if defined(_SAGA_MSW) || defined(_SAGA_LINUX)
#include <saga_api/saga_api.h>
#endif // #if defined(_SAGA_MSW) || defined(_SAGA_LINUX)

#include "line.h"
#include "i_line.h"


int const
   NRNG    = 2,
   SAVEMAX = 1000;

class CRasterGrid;               // Forward declarations
class CCoast;
class CProfile;
class CCoastPolygon;
class CCliff;

class CDelineation
{
private:
   bool
      m_bCliffTopSave,
      m_bCliffToeSave,
      m_bRasterCoastlineSave,
      m_bRasterNormalSave,
      m_bCoastSave,
      m_bNormalsSave,
      m_bInvalidNormalsSave,
      m_bCoastCurvatureSave,
      m_bOutputProfileData,
      m_bRandomCoastEdgeSearch,
      m_bGDALCanCreate,
      m_bGDALCanWriteFloat,
      m_bGDALCanWriteInt32,
      m_bScaleRasterOutput,
      m_bWorldFile;

   char** m_papszGDALRasterOptions;
   char** m_papszGDALVectorOptions;

   int
      m_nXGridMax,
      m_nYGridMax,
      m_nLayers,
      m_nCoastSmooth,
      m_nCoastSmoothWindow,
      m_nSavGolCoastPoly,
      m_nProfileSmoothWindow,
      m_nCoastNormalAvgSpacing,              // In cells
      m_nCoastCurvatureInterval,             // A length, measured in coastline points
      m_nCapeNormals,
      m_nGISSave,
      m_nUSave,
      m_nThisSave,
      m_nCoastMax,
      m_nCoastMin;
   
#if !defined(_SAGA_MSW) && !defined(_SAGA_LINUX)
   GDALDataType
      m_GDALWriteIntDataType,
      m_GDALWriteFloatDataType;
#endif //  // #if !defined(_SAGA_MSW) && !defined(_SAGA_LINUX)

   long
      m_lGDALMaxCanWrite,
      m_lGDALMinCanWrite;

   unsigned long
      m_ulTimestep,
      m_ulTotTimestep,
      m_ulRandSeed[NRNG],
      m_ulNumCells,
      m_ulThisTimestepNumSeaCells,
      m_ulThisTimestepNumCoastCells;
      
   double
      m_dDurationUnitsMult,
      m_dEleTolerance,
      m_dNorthWestXExtCRS,
      m_dNorthWestYExtCRS,
      m_dSouthEastXExtCRS,
      m_dSouthEastYExtCRS,
      m_dExtCRSGridArea,
      m_dCellSide,                     // Length of cell side (in external CRS units)
      m_dCellArea,                     // Area of cell  (in external CRS units)
      m_dCellDiagonal,                 // Length of cell's diagonal (in external CRS units)
      m_dInvCellSide,                  // Inverse of m_dCellSide
      m_dInvCellDiagonal,              // Inverse of m_dCellDiagonal
      m_dSimDuration,                  // Duration of simulation, in hours
      m_dUSaveTime[SAVEMAX],
      m_dClkLast,                      // Last value returned by clock()
      m_dCPUClock,                     // Total elapsed CPU time
      m_dGeoTransform[6],
      m_dSeaWaterDensity,
      m_dOrigSWL,
      m_dFinalSWL,
      m_dDeltaSWLPerTimestep,
      m_dStillWaterLevel,
      m_dMinSWL,
      m_dMaxSWL,
      m_dBreakingWaveHeight,
      m_dWavePeriod,
      m_dC_0,                          // Deep water wave speed (m/s)
      m_dL_0,                          // Deep water wave length (m)
      m_dWaveDepthRatioForWaveCalcs,
      m_dDeepWaterWaveHeight,
      m_dDeepWaterWaveOrientation,
      m_dR,
      m_dD50Fine,
      m_dD50Sand,
      m_dD50Coarse,
      m_dBeachSedimentDensity,
      m_dBeachSedimentPorosity,
      m_dFineErodibility,
      m_dSandErodibility,
      m_dCoarseErodibility,
      m_dFineErodibilityNormalized,
      m_dSandErodibilityNormalized,
      m_dCoarseErodibilityNormalized,
      m_dKLS,
      m_dKamphuis,
      m_dG,
      m_dInmersedToBulkVolumetric,
      m_dDepthOfClosure,
      m_dCoastNormalAvgSpacing,        // In m
      m_dCoastNormalLength,
      m_dThisTimestepTotSeaDepth,
      m_dThisTimestepPotentialPlatformErosion,
      m_dThisTimestepActualFinePlatformErosion,
      m_dThisTimestepActualSandPlatformErosion,
      m_dThisTimestepActualCoarsePlatformErosion,
      m_dThisTimestepPotentialBeachErosion,
      m_dThisTimestepActualFineBeachErosion,
      m_dThisTimestepActualSandBeachErosion,
      m_dThisTimestepActualCoarseBeachErosion,
      m_dThisTimestepSandBeachDeposition,
      m_dThisTimestepCoarseBeachDeposition,
      m_dThisTimestepFineSedimentToSuspension,
      m_dThisTimestepPotentialSedLostBeachErosion,
      m_dThisTimestepActualFineSedLostBeachErosion,
      m_dThisTimestepActualSandSedLostBeachErosion,
      m_dThisTimestepActualCoarseSedLostBeachErosion,
      m_dThisTimestepEstimatedActualFineBeachErosion,
      m_dThisTimestepEstimatedActualSandBeachErosion,
      m_dThisTimestepEstimatedActualCoarseBeachErosion,
      m_dThisTimestepCliffTalusFineErosion,
      m_dThisTimestepCliffTalusSandErosion,
      m_dThisTimestepCliffTalusCoarseErosion,
      m_dThisTimestepSandSedLostCliffCollapse,
      m_dThisTimestepCoarseSedLostCliffCollapse,
      m_dThisTimestepMassBalanceErosionError,
      m_dThisTimestepMassBalanceDepositionError,
      m_dDepthOverDBMax,                                    // Used in erosion potential look-up function
      m_dTotPotErosionOnProfiles,
      m_dTotPotErosionBetweenProfiles,
      m_dProfileMaxSlope,
      m_dSimpleSmoothWeight,
      m_dBeachSmoothingVertTolerance,
      m_dCliffErodibility,
      m_dNotchOverhangAtCollapse,
      m_dNotchBaseBelowSWL,
      m_dCliffDepositionA,
      m_dCliffDepositionPlanviewLength,
      m_dCliffDepositionHeightFrac,
      m_dThisTimestepCliffCollapseFine,
      m_dThisTimestepCliffCollapseSand,
      m_dThisTimestepCliffCollapseCoarse,
      m_dThisTimestepCliffTalusSandDeposition,
      m_dThisTimestepCliffTalusCoarseDeposition,
      m_dCoastNormalRandSpaceFact,
      m_dDeanProfileStartAboveSWL;

   // These grand totals are all long doubles, the aim is to minimize rounding errors when many very small numbers are added to a single much larger number, see e.g. http://www.ddj.com/cpp/184403224
   long double
      m_ldGTotPotentialPlatformErosion,
      m_ldGTotFineActualPlatformErosion,
      m_ldGTotSandActualPlatformErosion,
      m_ldGTotCoarseActualPlatformErosion,
      m_ldGTotPotentialSedLostBeachErosion,
      m_ldGTotActualFineSedLostBeachErosion,
      m_ldGTotActualSandSedLostBeachErosion,
      m_ldGTotActualCoarseSedLostBeachErosion,
      m_ldGTotSandSedLostCliffCollapse,
      m_ldGTotCoarseSedLostCliffCollapse,
      m_ldGTotCliffCollapseFine,
      m_ldGTotCliffCollapseSand,
      m_ldGTotCliffCollapseCoarse,
      m_ldGTotCliffTalusSandDeposition,
      m_ldGTotCliffTalusCoarseDeposition,
      m_ldGTotCliffTalusFineErosion,
      m_ldGTotCliffTalusSandErosion,
      m_ldGTotCliffTalusCoarseErosion,
      m_ldGTotPotentialBeachErosion,
      m_ldGTotActualFineBeachErosion,
      m_ldGTotActualSandBeachErosion,
      m_ldGTotActualCoarseBeachErosion,
      m_ldGTotSandBeachDeposition,
      m_ldGTotCoarseBeachDeposition,
      m_ldGTotSuspendedSediment,
      m_ldGTotMassBalanceErosionError,
      m_ldGTotMassBalanceDepositionError;

   string
      m_strDTMFile,                              // Digital Terrain Model raster file name
      m_strCLIFFDir,
      m_strCLIFFIni,
      m_strMailAddress,
      m_strDataPathName,
      m_strRasterGISOutFormat,
      m_strVectorGISOutFormat,
      m_strInitialBasementDEMFile,
      m_strInitialLandformFile,
      m_strInitialInterventionFile,
      m_strInitialSuspSedimentFile,
      m_strInitialCoastlineFile,
      m_strShapeFunctionFile,
//       m_strTideDataFile,
      m_strLogFile,
      m_strOutPath,
      m_strOutFile,
      m_strPalFile,
      m_strGDALDTMDriverCode,           	// DTM (raster)
      m_strGDALDTMDriverDesc,
      m_strGDALDTMProjection,
      m_strGDALDTMDataType,
      m_strGDALBasementDEMDriverCode,           // Basement DEM (raster)
      m_strGDALBasementDEMDriverDesc,
      m_strGDALBasementDEMProjection,
      m_strGDALBasementDEMDataType,
      m_strGDALLDriverCode,                     // Initial Landform Class (raster)
      m_strGDALLDriverDesc,
      m_strGDALLProjection,
      m_strGDALLDataType,
      m_strGDALIDriverCode,                     // Initial Intervention Class (raster)
      m_strGDALIDriverDesc,
      m_strGDALIProjection,
      m_strGDALIDataType,
      m_strGDALIWDriverCode,                    // Initial Water Depth (raster)
      m_strGDALIWDriverDesc,
      m_strGDALIWProjection,
      m_strGDALIWDataType,
      m_strGDALISSDriverCode,                   // Initial Suspended Sediment (raster)
      m_strGDALISSDriverDesc,
      m_strGDALISSProjection,
      m_strGDALISSDataType,
      m_strOGRICDriverCode,                     // Initial Coastline (vector)
      m_strOGRICGeometry,
      m_strOGRICDataType,
      m_strOGRICDataValue,
      m_strGDALRasterOutputDriverLongname,
      m_strGDALRasterOutputDriverExtension,
      m_strOGRVectorOutputExtension,
      m_strRunName,
      m_strDurationUnits;

   struct RandState
   {
      unsigned long s1, s2, s3;
   } m_ulRState[NRNG];

   time_t
      m_tSysStartTime,
      m_tSysEndTime;

   ofstream
      OutStream;
  
   vector<int>
      m_VnSavGolIndexCoast;            // Savitzky-Golay shift index for the coastline vector(s)

   vector<double>
      m_VdSavGolFCCoast,               // Savitzky-Golay filter coefficients for the coastline vector(s)
      m_VdSavGolFCProfile;             // Savitzky-Golay filter coefficients for the profile vectors


   // The raster grid object
   CRasterGrid* m_pRasterGrid;

   // The coastline objects
   vector<CCoast> m_VCoast;

   // Pointers to coast polygon objects
   vector<CCoastPolygon*> m_pVCoastPolygon;

private:
   // Input and output routines
   static int nHandleCommandLineParams(int, char* []);
   bool bReadIni(void);
   bool bReadRunData(void);
   bool bOpenLogFile(void);
   void WriteStartRunDetails(void);
   int nWriteEndRunDetails(void);
   int nSaveProfile(int const, int const, int const, vector<double>* const, vector<double>* const,vector<C2DIPoint>* const, vector<double>* const);
   bool bWriteProfileData(int const, int const, int const, vector<double>* const, vector<double>* const, vector<C2DIPoint>* const, vector<double>* const);

   // GIS input and output stuff
#if !defined(_SAGA_MSW) && !defined(_SAGA_LINUX)
   int nReadDTMData(void);
   bool bWriteRasterGISFloat(int const, string const*);
   bool bWriteRasterGISInt(int const, string const*, double const = 0);
   bool bWriteVectorGIS(int const, string const*);
#else // #if defined(_SAGA_MSW) || defined(_SAGA_LINUX)
   int	nReadDTMData(CSG_Grid *pGrid);
   bool	bWriteRasterGISFloat(int const, CSG_Grid *pGrid);
   bool	bWriteRasterGISInt  (int const, CSG_Grid *pGrid, double const = 0);
   bool	bWriteVectorGIS     (int const, CSG_Shapes *pShapes);
#endif // #if defined(_SAGA_MSW) || defined(_SAGA_LINUX)
   void GetRasterOutputMinMax(int const, double&, double&);
   void SetRasterFileCreationDefaults(void);

   // Top-level simulation routines
   int nInitGridAndCalcStillWaterLevel(void);
   int nLocateSeaAndCoasts(void);
   int nLocateCliffTop(void);

   // Lower-level simulation routines
   void FindAllSeaCells(void);
   void FloodFillSea(int const, int const);
   int nTraceCoastLine(int const, int const, int const, int const);
   int nTraceAllCoasts(void);
   void DoCoastCurvature(int const, int const);
   int nCreateAllProfilesAndCheckForIntersection(void);
   int nCreateAllProfiles(void);
   int nCreateProfile(int const, int const, int&);
   int nPutAllProfilesOntoGrid(void);
   int nModifyAllIntersectingProfiles(void);
   static bool bCheckForIntersection(CProfile* const, CProfile* const, int&, int&, double&, double&, double&, double&);
   void MergeProfilesAtFinalLineSegments(int const, int const, int const, int const, int const, double const, double const, double const, double const);
   void TruncateOneProfileRetainOtherProfile(int const, int const, int const, double const, double const, int const, int const, bool const);
   int nInsertPointIntoProfilesIfNeededThenUpdate(int const, int const, double const, double const, int const, int const, int const, bool const);
   void TruncateProfileAndAppendNew(int const, int const, int const, vector<C2DPoint>* const, vector<vector<pair<int, int> > >* const);
   void RasterizeProfile(int const, int const, vector<C2DIPoint>*, vector<bool>*, bool&, bool&, bool&, bool&);
   static C2DPoint PtChooseLandEndPoint(int const, C2DPoint* const, C2DPoint* const, double const, double const, double const, double const);
   static C2DPoint PtChooseSeaEndPoint(int const, C2DPoint* const, C2DPoint* const, double const, double const, double const, double const);
   int nGetCoastNormalEndPoint(int const, int const, int const, C2DPoint* const, double const, C2DPoint*, C2DPoint*);
   int nLandformToGrid(int const, int const);
   static double dCalcCurvature(int const, C2DPoint* const, C2DPoint* const, C2DPoint* const);
   void SmoothGrid(void);
   void SmoothLayer(int const, bool const);


   // GIS utility routines
   bool bCheckRasterGISOutputFormat(void);
   bool bCheckVectorGISOutputFormat(void);
   bool bSaveAllRasterGISFiles(void);
   bool bSaveAllVectorGISFiles(void);
   bool bIsWithinGrid(int const, int const) const;
   bool bIsWithinGrid(C2DIPoint* const) const;
   double dGridCentroidXToExtCRSX(int const) const;
   double dGridCentroidYToExtCRSY(int const) const;
   double dGridXToExtCRSX(double const) const;
   double dGridYToExtCRSY(double const) const;
   double dExtCRSXToGridCentroidX(double const) const;
   double dExtCRSYToGridCentroidY(double const) const;
   double dExtCRSXToGridX(double const) const;
   double dExtCRSYToGridY(double const) const;
   int nExtCRSXToGridX(double const) const;
   int nExtCRSYToGridY(double const) const;
   C2DIPoint PtiExtCRSToGrid(C2DPoint* const) const;
   C2DPoint PtGridCentroidToExt(C2DIPoint* const) const;
   static double dGetDistanceBetween(C2DPoint* const, C2DPoint* const);
   static double dGetDistanceBetween(C2DIPoint* const, C2DIPoint* const);
   void KeepWithinGrid(int&, int&);
   void KeepWithinGrid(C2DIPoint*);
//    C2DPoint* pPtExtCRSKeepWithinGrid(C2DPoint*);
   static double dKeepWithin360(double const);
//    vector<C2DPoint> VGetPerpendicular(C2DPoint* const, C2DPoint* const, double const, int const);
   static C2DPoint PtGetPerpendicular(C2DPoint* const, C2DPoint* const, double const, int const);
   static C2DPoint PtAverage(C2DPoint* const, C2DPoint* const);
   static C2DPoint PtAverage(vector<C2DPoint>*);
   static C2DIPoint PtiAverage(C2DIPoint* const, C2DIPoint* const);

   // Utility routines
   static void AnnounceStart(void);
   void AnnounceLicence(void);
   void AnnounceReadDTM(void) const;
   void AnnounceReadBasementDEM(void) const;
//    static void AnnounceReadVectorFiles(void);
   void AnnounceReadLGIS(void) const;
   void AnnounceReadIGIS(void) const;
   static void AnnounceInitializing(void);
   static void AnnounceAllocateMemory(void);
   static void AnnounceIsRunning(void);
   static void AnnounceSimEnd(void);
   void StartClock(void);
   bool bFindExeDir(char* pcArg);
   static int nDoTimeUnits(string const*);
   int nDoSimulationTimeMultiplier(string const*);
   static double dGetTimeMultiplier(string const*);
   void UpdateGrandTotals(void);
   static string strGetBuild(void);
   static string strGetComputerName(void);
   void DoCPUClockReset(void);
   void CalcTime(double const);
   static string strDispTime(double const, bool const, bool const);
   static string strDispSimTime(double const);
   void AnnounceProgress(void);
   static string strGetErrorText(int const);
   string strListRasterFiles(void) const;
   string strListVectorFiles(void) const;
   string strListTSFiles(void) const;
   void CalcProcessStats(void);
   void CalcSavitzkyGolayCoeffs(void);
   CLine LSmoothCoastSavitzkyGolay(CLine*, int const, int const) const;
   CLine LSmoothCoastRunningMean(CLine*, int const, int const) const;
   vector<double> dVSmoothProfileSlope(vector<double>*);
//    vector<double> dVCalcProfileSlope(vector<C2DPoint>*, vector<double>*);
   vector<double> dVSmoothProfileSavitzkyGolay(vector<double>*, vector<double>*);
   vector<double> dVSmoothProfileRunningMean(vector<double>*);
   void CalcSavitzkyGolay(double[], int const, int const, int const, int const, int const);

   // Random number stuff
   static unsigned long ulGetTausworthe(unsigned long const, unsigned long const, unsigned long const, unsigned long const, unsigned long const);
   void InitRand0(unsigned long const);
   void InitRand1(unsigned long const);
   unsigned long ulGetRand0(void);
   unsigned long ulGetRand1(void);
   static unsigned long ulGetLCG(unsigned long const);            // Used by all generators
   double dGetRand0d1(void);
//    int nGetRand0To(int const);
   int nGetRand1To(int const);
//    double dGetRand0GaussPos(double const, double const);
   double dGetRand0Gaussian(void);
//    double dGetCGaussianPDF(double const);
   void Rand1Shuffle(int*, int);
#ifdef RANDCHECK
   void CheckRand(void) const;
#endif

public:
   ofstream LogStream;

   CDelineation(void);
   ~CDelineation(void);

   //! Returns this timestep's still water level
   double dGetSWL(void) const;

   //! Returns the vertical tolerance for beach cells to be included in smoothing
   double dGetBeachSmoothingVertTolerance(void) const;


   //! Returns the size of the grid in the X direction
   int nGetGridXMax(void) const;

   //! Returns the size of the grid in the Y direction
   int nGetGridYMax(void) const;

   //! Runs the simulation
#if !defined(_SAGA_MSW) && !defined(_SAGA_LINUX)
   int nDoDelineation(int, char*[]);
#else // #if defined(_SAGA_MSW) || defined(_SAGA_LINUX)
   CSG_Parameters	*m_pParameters;

   int	nDoDelineation(CSG_Parameters *pParameters);
#endif // #if defined(_SAGA_MSW) || defined(_SAGA_LINUX)

   //! Carries out end-of-simulation tidying (error messages etc.)
   void DoDelineationEnd(int const);
};
#endif // DELINEATION_H
