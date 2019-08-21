/*!
 *
 * \file coast_polygon.cpp
 * \brief CCoastPolygon routines
 * \details TODO A more detailed description of these routines.
 * \author David Favis-Mortlock
 * \author Andres Payo
 * \author Jim Hall
 * \date 2017
 * \copyright GNU General Public License
 *
 */

/*===============================================================================================================================

 This file is part of CliffMetrics, the Coastal Modelling Environment.

 CliffMetrics is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

===============================================================================================================================*/
// #include <assert.h>

#include "cliffmetrics.h"
#include "coast_polygon.h"


//! Constructor with 8 parameters
CCoastPolygon::CCoastPolygon(int const nGlobalID, int const nCoastID, int const nNode, int const nProfileUpCoast, int const nProfileDownCoast, vector<C2DPoint>* const pVIn, int const nLastPointUpCoast, const int nLastPointDownCoast, C2DIPoint* const PtiNode, C2DIPoint* const PtiAntinode, int const nPiPStartPoint)
:
//    m_bIsPointedSeaward(true),
   m_bDownCoastThisTimestep(false),
   m_nGlobalID(nGlobalID),
   m_nCoastID(nCoastID),
   m_nCoastNode(nNode),
   m_nNormalProfileUpCoast(nProfileUpCoast),
   m_nNormalProfileDownCoast(nProfileDownCoast),
   m_nProfileUpCoastNumPointsUsed(nLastPointUpCoast),
   m_nProfileDownCoastNumPointsUsed(nLastPointDownCoast),
   m_nNumCells(0),
   m_nPiPSearchStartPoint(nPiPStartPoint),
//    m_dSeawaterVolume(0),
   m_dAvgUnconsD50(0),
   m_dDeltaPotentialTotalSediment(0),
   m_dDeltaEstimatedUnconsFine(0),
   m_dDeltaEstimatedUnconsSand(0),
   m_dDeltaEstimatedUnconsCoarse(0),
   m_dDeltaActualUnconsFine(0),
   m_dDeltaActualUnconsSand(0),
   m_dDeltaActualUnconsCoarse(0),
   m_PtiNode(*PtiNode),
   m_PtiAntinode(*PtiAntinode)
{
   m_VPoints = *pVIn;
}

CCoastPolygon::~CCoastPolygon(void)
{
}


// void CCoastPolygon::SetNotPointed(void)
// {
//    m_bIsPointedSeaward = false;
// }
//
// bool CCoastPolygon::bIsPointed(void) const
// {
//    return m_bIsPointedSeaward;
// }


void CCoastPolygon::SetDownCoastThisTimestep(bool const bFlag)
{
   m_bDownCoastThisTimestep = bFlag;
}

bool CCoastPolygon::bDownCoastThisTimestep(void) const
{
   return m_bDownCoastThisTimestep;
}


int CCoastPolygon::nGetGlobalID(void) const
{
   return m_nGlobalID;
}

int CCoastPolygon::nGetCoastID(void) const
{
   return m_nCoastID;
}


// void CCoastPolygon::SetCoastNode(int const nNode)
// {
//    m_nCoastNode = nNode;
// }

int CCoastPolygon::nGetNodeCoastPoint(void) const
{
   return m_nCoastNode;
}

C2DIPoint* CCoastPolygon::pPtiGetNode(void)
{
   return &m_PtiNode;

}

C2DIPoint* CCoastPolygon::pPtiGetAntinode(void)
{
   return &m_PtiAntinode;
}


void CCoastPolygon::SetNumCells(int const nCells)
{
   m_nNumCells = nCells;
}


// int CCoastPolygon::nGetNumCells(void) const
// {
//    return m_nNumCells;
// }


int CCoastPolygon::nGetUpCoastProfile(void) const
{
   return m_nNormalProfileUpCoast;
}

int CCoastPolygon::nGetDownCoastProfile(void) const
{
   return m_nNormalProfileDownCoast;
}


// void CCoastPolygon::SetBoundary(vector<C2DPoint>* const pVIn)
// {
//    m_VPoints = *pVIn;
// }

// vector<C2DPoint>* CCoastPolygon::pPtVGetBoundary(void)
// {
//    return &m_VPoints;
// }

C2DPoint* CCoastPolygon::pPtGetBoundaryPoint(int const nPoint)
{
   // NOTE no check to see if nPoint < m_VPoints.size()
   return &m_VPoints[nPoint];
}

int CCoastPolygon::nGetBoundarySize(void) const
{
   return m_VPoints.size();
}


int CCoastPolygon::nGetUpCoastProfileNumPointsUsed(void) const
{
   return m_nProfileUpCoastNumPointsUsed;
}

int CCoastPolygon::nGetDownCoastProfileNumPointsUsed(void) const
{
   return m_nProfileDownCoastNumPointsUsed;
}


// void CCoastPolygon::SetSeawaterVolume(const double dDepth)
// {
//    m_dSeawaterVolume = dDepth;
// }

// double CCoastPolygon::dGetSeawaterVolume(void) const
// {
//    return m_dSeawaterVolume;
// }

//! Adds a change in potential erosion or deposition to this timestep's total change in depth of unconsolidated sediment (all size classes) on this polygon (-ve values for erosion, +ve values for deposition)
void CCoastPolygon::AddDeltaPotentialTotalSediment(double const dDepth)
{
   m_dDeltaPotentialTotalSediment += dDepth;
}

//! Returns this timestep's total change in depth of unconsolidated sediment (all size classes) on this polygon (-ve values for erosion, +ve values for deposition)
double CCoastPolygon::dGetDeltaPotentialErosion(void) const
{
   return m_dDeltaPotentialTotalSediment;
}

//! Sets a value for this timestep's estimated total change in depth of fine unconsolidated sediment on this polygon (-ve values for erosion, +ve values for deposition)
void CCoastPolygon::SetDeltaEstimatedUnconsFine(double const dDepth)
{
   m_dDeltaEstimatedUnconsFine = dDepth;
}

//! Returns this timestep's estimate of total change in depth of fine unconsolidated sediment on this polygon (-ve values for erosion, +ve values for deposition)
double CCoastPolygon::dGetDeltaEstimatedUnconsFine(void) const
{
   return m_dDeltaEstimatedUnconsFine;
}

//! Sets a value for this timestep's estimated total change in depth of sand-sized unconsolidated sediment on this polygon (-ve values for erosion, +ve values for deposition)
void CCoastPolygon::SetDeltaEstimatedUnconsSand(double const dDepth)
{
   m_dDeltaEstimatedUnconsSand = dDepth;
}

//! Returns this timestep's estimate of total change in depth of sand-sized unconsolidated sediment on this polygon (-ve values for erosion, +ve values for deposition)
double CCoastPolygon::dGetDeltaEstimatedUnconsSand(void) const
{
   return m_dDeltaEstimatedUnconsSand;
}

//! Sets a value for this timestep's estimate of total change in depth of coarse unconsolidated sediment on this polygon (-ve values for erosion, +ve values for deposition)
void CCoastPolygon::SetDeltaEstimatedUnconsCoarse(double const dDepth)
{
   m_dDeltaEstimatedUnconsCoarse = dDepth;
}

//! Returns this timestep's estimate of total change in depth of coarse unconsolidated sediment on this polygon (-ve values for erosion, +ve values for deposition)
double CCoastPolygon::dGetDeltaEstimatedUnconsCoarse(void) const
{
   return m_dDeltaEstimatedUnconsCoarse;
}

//! Adds a change in erosion or deposition to this timestep's total actual change in depth of unconsolidated fine sediment on this polygon (-ve values for erosion, +ve values for deposition)
void CCoastPolygon::AddDeltaActualUnconsFine(double const dDepth)
{
   m_dDeltaActualUnconsFine += dDepth;
}

// void CCoastPolygon::SetDeltaActualUnconsFine(double const dDepth)
// {
//    m_dDeltaActualUnconsFine = dDepth;
// }

//! Returns this timestep's actual total change in depth of fine unconsolidated sediment on this polygon (-ve values for erosion, +ve values for deposition)
double CCoastPolygon::dGetDeltaActualUnconsFine(void) const
{
   return m_dDeltaActualUnconsFine;
}

//! Adds a change in erosion or deposition to this timestep's total actual change in depth of unconsolidated sand-sized sediment on this polygon (-ve values for erosion, +ve values for deposition)
void CCoastPolygon::AddDeltaActualUnconsSand(double const dDepth)
{
   m_dDeltaActualUnconsSand += dDepth;
}

// void CCoastPolygon::SetDeltaActualUnconsSand(double const dDepth)
// {
//    m_dDeltaActualUnconsSand = dDepth;
// }

//! Returns this timestep's actual total change in depth of sand-sized unconsolidated sediment on this polygon (-ve values for erosion, +ve values for deposition)
double CCoastPolygon::dGetDeltaActualUnconsSand(void) const
{
   return m_dDeltaActualUnconsSand;
}

//! Adds a change in erosion or deposition to this timestep's total actual change in depth of unconsolidated coarse sediment on this polygon (-ve values for erosion, +ve values for deposition)
void CCoastPolygon::AddDeltaActualUnconsCoarse(double const dDepth)
{
   m_dDeltaActualUnconsCoarse += dDepth;
}

// void CCoastPolygon::SetDeltaActualUnconsCoarse(double const dDepth)
// {
//    m_dDeltaActualUnconsCoarse = dDepth;
// }

//! Returns this timestep's actual total change in depth of coarse unconsolidated sediment on this polygon (-ve values for erosion, +ve values for deposition)
double CCoastPolygon::dGetDeltaActualUnconsCoarse(void) const
{
   return m_dDeltaActualUnconsCoarse;
}

//! Returns this timestep's actual total change in depth of unconsolidated sediment (all size classes) on this polygon (-ve values for erosion, +ve values for deposition)
double CCoastPolygon::dGetDeltaActualTotalSediment(void) const
{
   return m_dDeltaActualUnconsFine + m_dDeltaActualUnconsSand + m_dDeltaActualUnconsCoarse;
}


void CCoastPolygon::SetUpCoastAdjacentPolygons(vector<int>* const pnVPolygons)
{
   m_nVUpCoastAdjacentPolygon = *pnVPolygons;
}

int CCoastPolygon::nGetUpCoastAdjacentPolygon(int const nIndex) const
{
//    assert(nIndex < m_nVUpCoastAdjacentPolygon.size());
   return m_nVUpCoastAdjacentPolygon[nIndex];
}

int CCoastPolygon::nGetNumUpCoastAdjacentPolygons(void) const
{
   return m_nVUpCoastAdjacentPolygon.size();
}


void CCoastPolygon::SetDownCoastAdjacentPolygons(vector<int>* const pnVPolygons)
{
   m_nVDownCoastAdjacentPolygon = *pnVPolygons;
}

int CCoastPolygon::nGetDownCoastAdjacentPolygon(int const nIndex) const
{
//    assert(nIndex < m_nVDownCoastAdjacentPolygon.size());
   return m_nVDownCoastAdjacentPolygon[nIndex];
}

int CCoastPolygon::nGetNumDownCoastAdjacentPolygons(void) const
{
   return m_nVDownCoastAdjacentPolygon.size();
}


void CCoastPolygon::SetUpCoastAdjacentPolygonBoundaryShares(vector<double>* const pdVShares)
{
   m_dVUpCoastAdjacentPolygonBoundaryShare = *pdVShares;
}

double CCoastPolygon::dGetUpCoastAdjacentPolygonBoundaryShare(int const nIndex) const
{
   // NOTE no check to see if nIndex < m_dVUpCoastAdjacentPolygonBoundaryShare.size()
   return m_dVUpCoastAdjacentPolygonBoundaryShare[nIndex];
}


void CCoastPolygon::SetDownCoastAdjacentPolygonBoundaryShares(vector<double>* const pdVShares)
{
   m_dVDownCoastAdjacentPolygonBoundaryShare = *pdVShares;
}

double CCoastPolygon::dGetDownCoastAdjacentPolygonBoundaryShare(int const nIndex) const
{
   // NOTE no check to see if nIndex < m_dVDownCoastAdjacentPolygonBoundaryShare.size()
   return m_dVDownCoastAdjacentPolygonBoundaryShare[nIndex];
}


int CCoastPolygon::nGetPiPSearchStartPoint(void) const
{
   return m_nPiPSearchStartPoint;
}


void CCoastPolygon::SetAvgUnconsD50(double const dD50)
{
   m_dAvgUnconsD50 = dD50;
}

double CCoastPolygon::dGetAvgUnconsD50(void) const
{
   return m_dAvgUnconsD50;
}


void CCoastPolygon::Display(void)
{
//    cout << endl;
//    for (int n = 0; n < static_cast<int>(m_VPoints.size()); n++)
//       cout << "[" << m_VPoints[n].dGetX() << "][" << m_VPoints[n].dGetY() << "], ";
//    cout << endl;
//    cout.flush();
}
