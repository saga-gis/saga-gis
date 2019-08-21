/*!
 * \class CCoastPolygon
 * \brief Class which is used for coast polygon objects
 * \details TODO This is a more detailed description of the CCoast class.
 * \author David Favis-Mortlock
 * \author Andres Payo
 * \author Jim Hall
 * \date 2017
 * \copyright GNU General Public License
 *
 * \file coastpolygon.h
 * \brief Contains CCoastPolygon definitions
 *
 */

#ifndef COASTPOLYGON_H
#define COASTPOLYGON_H
/*===============================================================================================================================

 This file is part of CliffMetrics, the Coastal Modelling Environment.

 CliffMetrics is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

===============================================================================================================================*/
#include "2d_shape.h"

class CCoastPolygon : public C2DShape
{
private:
   bool
//       m_bIsPointedSeaward,                // Does the polygon meet at a point at its seaward end? (i.e. is it roughly triangular?)
      m_bDownCoastThisTimestep;

   int
      m_nGlobalID,                        // The simulation-global number of this polygon
      m_nCoastID,                         // This-coast-only number of this polygon
      m_nCoastNode,                       // The point on this polygon's coastline segment with maximum concave curvature, roughly at the middle of the coastline segment
      m_nNormalProfileUpCoast,            // The normal profile which bounds the polygon in the up-coast direction
      m_nNormalProfileDownCoast,          // Ditto for the down-coast direction
      m_nProfileUpCoastNumPointsUsed,     // The number of points from the up-coast normal which are part of this polygon (less than the normal's full length if the polygon is triangular)
      m_nProfileDownCoastNumPointsUsed,   // Ditto for the down-coast normal
      m_nNumCells,                        // The number of cells in the polygon
      m_nPiPSearchStartPoint;             // The number of the vector point from which we start the point-in-polygon search

   double
      m_dAvgUnconsD50,                    // The average d50 of unconsolidated sediment on this polygon
//       m_dSeawaterVolume,                  // The volume (m3) of seawater within the polygon
      m_dDeltaPotentialTotalSediment,     // Potential change (i.e. ignoring supply-limitation) in total sediment (depth in m, all size classes) this timestep (-ve values for erosion, +ve values for deposition)
      m_dDeltaEstimatedUnconsFine,        // Estimated actual change (i.e. considering supply-limitation) in fine-sized sediment (depth in m) this timestep (-ve values for erosion, +ve values for deposition)
      m_dDeltaEstimatedUnconsSand,        // Estimated actual change (i.e. considering supply-limitation) in sand-sized sediment (depth in m) this timestep (-ve values for erosion, +ve values for deposition)
      m_dDeltaEstimatedUnconsCoarse,      // Estimatedactual change (i.e. considering supply-limitation) in coarse-sized sediment (depth in m) this timestep (-ve values for erosion, +ve values for deposition)
      m_dDeltaActualUnconsFine,           // Actual change (i.e. considering supply-limitation) in fine-sized sediment (depth in m) this timestep (-ve values for erosion, +ve values for deposition)
      m_dDeltaActualUnconsSand,           // Actual change (i.e. considering supply-limitation) in sand-sized sediment (depth in m) this timestep (-ve values for erosion, +ve values for deposition)
      m_dDeltaActualUnconsCoarse;         // Actual change (i.e. considering supply-limitation) in coarse-sized sediment (depth in m) this timestep (-ve values for erosion, +ve values for deposition)

   C2DIPoint
      m_PtiNode,                          // Co-ords of the coast node cell (raster-grid CRS)
      m_PtiAntinode;                      // Co-ords of the cell (raster-grid CRS) which is at other (seaward) end of the polygon

   vector<int>
      m_nVUpCoastAdjacentPolygon,
      m_nVDownCoastAdjacentPolygon;

   vector<double>
      m_dVUpCoastAdjacentPolygonBoundaryShare,
      m_dVDownCoastAdjacentPolygonBoundaryShare;

public:
   CCoastPolygon(int const, int const, int const, int const, int const, vector<C2DPoint>* const, int const, int const, C2DIPoint* const, C2DIPoint* const, int const);
   ~CCoastPolygon(void);

   void SetDownCoastThisTimestep(bool const);
   bool bDownCoastThisTimestep(void) const;

   int nGetGlobalID(void) const;

   int nGetCoastID(void) const;

//    void SetCoastNode(int const);
   int nGetNodeCoastPoint(void) const;
   C2DIPoint* pPtiGetNode(void);
   C2DIPoint* pPtiGetAntinode(void);

//    void SetNotPointed(void);
//    bool bIsPointed(void) const;

   void SetNumCells(int const);
//    int nGetNumCells(void) const;

   int nGetUpCoastProfile(void) const;
   int nGetDownCoastProfile(void) const;

//    void SetBoundary(vector<C2DPoint>* const);
//    vector<C2DPoint>* pPtVGetBoundary(void);
   C2DPoint* pPtGetBoundaryPoint(int const);
   int nGetBoundarySize(void) const;

   int nGetUpCoastProfileNumPointsUsed(void) const;
   int nGetDownCoastProfileNumPointsUsed(void) const;

//    void SetSeawaterVolume(const double);
//    double dGetSeawaterVolume(void) const;

   void AddDeltaPotentialTotalSediment(double const);
   double dGetDeltaPotentialErosion(void) const;

   void SetDeltaEstimatedUnconsFine(double const);
   double dGetDeltaEstimatedUnconsFine(void) const;
   void SetDeltaEstimatedUnconsSand(double const);
   double dGetDeltaEstimatedUnconsSand(void) const;
   void SetDeltaEstimatedUnconsCoarse(double const);
   double dGetDeltaEstimatedUnconsCoarse(void) const;

   void AddDeltaActualUnconsFine(double const);
//    void SetDeltaActualUnconsFine(double const);
   double dGetDeltaActualUnconsFine(void) const;
   void AddDeltaActualUnconsSand(double const);
   double dGetDeltaActualUnconsSand(void) const;
//    void SetDeltaActualUnconsSand(double const);
   void AddDeltaActualUnconsCoarse(double const);
   double dGetDeltaActualUnconsCoarse(void) const;
//    void SetDeltaActualUnconsCoarse(double const);
   double dGetDeltaActualTotalSediment(void) const;

   void SetUpCoastAdjacentPolygons(vector<int>* const);
   int nGetUpCoastAdjacentPolygon(int const) const;
   int nGetNumUpCoastAdjacentPolygons(void) const;

   void SetDownCoastAdjacentPolygons(vector<int>* const);
   int nGetDownCoastAdjacentPolygon(int const) const;
   int nGetNumDownCoastAdjacentPolygons(void) const;

   void SetUpCoastAdjacentPolygonBoundaryShares(vector<double>* const);
   double dGetUpCoastAdjacentPolygonBoundaryShare(int const) const;

   void SetDownCoastAdjacentPolygonBoundaryShares(vector<double>* const);
   double dGetDownCoastAdjacentPolygonBoundaryShare(int const) const;

   int nGetPiPSearchStartPoint(void) const;

   void SetAvgUnconsD50(double const);
   double dGetAvgUnconsD50(void) const;

   void Display(void);
};
#endif //COASTPOLYGON_H

