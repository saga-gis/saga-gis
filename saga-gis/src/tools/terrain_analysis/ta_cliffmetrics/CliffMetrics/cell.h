/*!
 * \class CCell
 * \brief Class for the cell objects which comprise the raster grid
 * \details TODO This is a more detailed description of the CCell class.
 * \author David Favis-Mortlock
 * \author Andres Payo
 * \author Jim Hall
 * \date 2017
 * \copyright GNU General Public License
 *
 * \file cell.h
 * \brief Contains CCell definitions
 *
 */

#ifndef CELL_H
#define CELL_H
/*===============================================================================================================================

 This file is part of CliffMetrics, the Coastal Modelling Environment.

 CliffMetrics is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

===============================================================================================================================*/
#include "cliffmetrics.h"
#include "cell_landform.h"
#include "raster_grid.h"


class CCell
{
private:
   bool
      m_bInContiguousSea,                    // Is a sea cell, contiguous with other sea cells
      m_bInActiveZone,
      m_bCoastline,
      m_bEstimated;

   int
      m_nIntervention,                       // Intervention code, see CLIFF for codes
      m_nPolygonID,
      m_nCoastlineNormal;

   double
      m_dLocalConsSlope,                     // As used in erosion calcs (really just for display purposes)
      m_dBasementElevation,                  // Elevation of basement surface (m)
      m_dSeaDepth,                           // Depth of still water (m), is zero if not inundated
      m_dTotSeaDepth,                        // Total depth of still water (m) since beginning of simulation (used to calc average)
      m_dWaveHeight,                         // Wave height
      m_dTotWaveHeight,                      // Total wave height (m) (used to calc average)
      m_dWaveOrientation,                    // Wave orientation
      m_dTotWaveOrientation,                 // Total wave orientation  (used to calc average)
      m_dBeachProtectionFactor,              // Only meaningful if in zone of platform erosion. 0 is fully protected, 1 = no protection
      m_dSuspendedSediment,                  // Suspended sediment as depth equivalent (m)
      m_dTotSuspendedSediment,               // Total depth of suspended sediment (m) since simulation start (used to calc average)
      m_dPotentialPlatformErosion,           // Depth of sediment on the shore platform that could be eroded this timestep, if no supply-limitation
      m_dTotPotentialPlatformErosion,        // Total depth of sediment eroded from the shore platform, if no supply-limitation
      m_dActualPlatformErosion,              // Depth of sediment actually eroded from the shore platform this timestep
      m_dTotActualPlatformErosion,           // Total depth of sediment actually eroded from the shore platform
      m_dCliffCollapse,                      // Depth of sediment removed via cliff collapse this timestep
      m_dTotCliffCollapse,                   // Total depth of sediment removed via cliff collapse
      m_dCliffCollapseDeposition,            // Depth of sediment deposited as a result of cliff collapse this timestep
      m_dTotCliffCollapseDeposition,         // Total depth of sediment deposited as a result of cliff collapse
      m_dPotentialBeachErosion,              // Depth of unconsolidated beach sediment that could be eroded this timestep, if no supply-limitation
      m_dTotPotentialBeachErosion,           // Total depth of unconsolidated beach sediment eroded, if no supply-limitation
      m_dActualBeachErosion,                 // Depth of unconsolidated beach sediment actually eroded this timestep
      m_dTotActualBeachErosion,              // Total depth of unconsolidated beach sediment actually eroded
      m_dBeachDeposition,                    // Depth of unconsolidated beach sediment deposited this timestep
      m_dTotBeachDeposition,                 // Total depth of unconsolidated beach sediment deposited
      m_dUnconsD50;                          // d50 of unconsolidated sediment on top layer with unconsolidated sediment depth > 0
//       m_dTmpWork;                            // Used in smoothing calcs

   // This cell's landform data
   CCellLandform m_Landform;

   // Initialize these as empty vectors
   //vector<CCellLayer> m_VLayer;              // Number of layers NOT including the basement. Layer 0 is the lowest
   vector<double> m_VdHorizonElev;           // Number of layer-top elevations (including that of the basement, which is m_VdHorizonElev[0]). This has size 1 greater than size of m_VLayer

public:
   static CRasterGrid* m_pGrid;

   CCell();
   ~CCell(void);

   void SetInContiguousSea(void);
   bool bIsInContiguousSea(void) const;

   void SetActualBeachErosionEstimated(void);
   bool bGetActualBeachErosionEstimated(void) const;

   void SetInActiveZone(bool const);
   bool bInActiveZone(void) const;
   bool bPotentialPlatformErosion(void) const;
//    bool bActualPlatformErosion(void) const;
   void SetAsCoastline(bool const);
   bool bIsCoastline(void) const;

   void SetNormalProfile(int const);
   int nGetNormalProfile(void) const;
   bool bIsNormalProfile(void) const;

   void SetPolygonID(int const);
   int nGetPolygonID(void) const;

   void SetIntervention(int const);
   int nGetIntervention(void) const;

   CCellLandform* pGetLandform(void);

   void SetLocalConsSlope(double const);
   double dGetLocalConsSlope(void) const;

   void SetBasementElev(double const);
   double dGetBasementElev(void) const;

   double dGetVolEquivSedTopElev(void) const;
   double dGetSedimentTopElev(void) const;
   double dGetTop(void) const;

   bool bIsInundated(void) const;
   bool bIsSeaIncBeach(void) const;
//    double dGetConsThickness(void) const;
//    double dGetUnconsThickness(void) const;
   void SetSeaDepth(void);
   double dGetSeaDepth(void) const;
   void InitCell(void);
   double dGetTotSeaDepth(void) const;

   void SetWaveHeight(double const);
   double dGetWaveHeight(void) const;
   double dGetTotWaveHeight(void) const;
   void SetWaveOrientation(double const);
   double dGetWaveOrientation(void) const;
   double dGetTotWaveOrientation(void) const;

   void SetBeachProtectionFactor(double const);
   double dGetBeachProtectionFactor(void) const;

   void SetSuspendedSediment(double const);
   double dGetSuspendedSediment(void) const;
   double dGetTotSuspendedSediment(void) const;

   int nGetTopNonZeroLayer(void) const;
   int nGetTopLayer(void) const;

   double dGetConsSedTopForLayer(int const) const;
 
   void AppendLayers(int const);
   void CalcAllLayerElevs(void);
   int nGetLayerAtElev(double const) const;
   double dCalcLayerElev(const int);

   void SetPotentialPlatformErosion(double const);
   double dGetPotentialPlatformErosion(void) const;
   double dGetTotPotentialPlatformErosion(void) const;

   void SetActualPlatformErosion(double const);
   double dGetActualPlatformErosion(void) const;
   double dGetTotActualPlatformErosion(void) const;

   void IncrCliffCollapse(double const);
   double dGetCliffCollapse(void) const;
   double dGetTotCliffCollapse(void) const;

   void IncrCliffCollapseDeposition(double const);
   double dGetCliffCollapseDeposition(void) const;
   double dGetTotCliffCollapseDeposition(void) const;

   void SetPotentialBeachErosion(double const);
   double dGetPotentialBeachErosion(void) const;
   double dGetTotPotentialBeachErosion(void) const;
   void SetActualBeachErosion(double const);
   double dGetActualBeachErosion(void) const;
   double dGetTotActualBeachErosion(void) const;
//    bool bActualBeachErosionThisTimestep(void) const;

   void IncrBeachDeposition(double const);
   double dGetBeachDeposition(void) const;
   double dGetTotBeachDeposition(void) const;
//    bool bBeachDepositionThisTimestep(void) const;

   bool bBeachErosionOrDepositionThisTimestep(void) const;

   void SetTmpWork(double const);
   void IncrTmpWork(double const);
   double dGetTmpWork(void) const;

   double dGetUnconsD50(void) const;
};
#endif // CELL_H
