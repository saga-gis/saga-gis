/*!
 *
 * \file coast.cpp
 * \brief CCoast routines
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
//#include <assert.h>

#include "cliffmetrics.h"
#include "coast.h"
#include "line.h"
#include "i_line.h"


CCoast::CCoast(void)
:  m_nSeaHandedness(NULL_HANDED),
   m_nStartEdge(INT_NODATA),
   m_nEndEdge(INT_NODATA)
{
}

CCoast::~CCoast(void)
{
   for (unsigned int i = 0; i < m_pVLandforms.size(); i++)
      delete m_pVLandforms[i];

   for (unsigned int i = 0; i < m_pVPolygon.size(); i++)
      delete m_pVPolygon[i];
}


void CCoast::SetSeaHandedness(int const nNewHandedness)
{
   m_nSeaHandedness = nNewHandedness;
}

int CCoast::nGetSeaHandedness(void) const
{
   return m_nSeaHandedness;
}


void CCoast::SetStartEdge(int const nEdge)
{
   m_nStartEdge = nEdge;
}

int CCoast::nGetStartEdge(void) const
{
   return m_nStartEdge;
}


void CCoast::SetEndEdge(int const nEdge)
{
   m_nEndEdge = nEdge;
}

int CCoast::nGetEndEdge(void) const
{
   return m_nEndEdge;
}


void CCoast::AppendToCoastline(double const dX, double const dY)
{
   // Appends a coastline point (in external CRS), also appends dummy values for curvature, breaking wave height, wave angle, and flux orientation
   m_LCoastline.Append(dX, dY);

   m_nVProfileNumber.push_back(INT_NODATA);
   m_nVPolygonNode.push_back(INT_NODATA);

   m_dVCurvature.push_back(DBL_NODATA);
   m_dVBreakingWaveHeight.push_back(DBL_NODATA);
   m_dVBreakingWaveAngle.push_back(DBL_NODATA);
   m_dVDepthOfBreaking.push_back(DBL_NODATA);
   m_dVFluxOrientation.push_back(DBL_NODATA);
   m_dVWaveEnergy.push_back(DBL_NODATA);
   m_nVBreakingDistance.push_back(INT_NODATA);
}

CLine* CCoast::pLGetCoastline(void)
{
   return &m_LCoastline;
}

C2DPoint* CCoast::pPtGetVectorCoastlinePoint(int const n)
{
   // Point is in external CRS NOTE no check to see that n is < m_LCoastline.Size()
   return &m_LCoastline[n];
}

int CCoast::nGetCoastlineSize(void) const
{
   return m_LCoastline.nGetSize();
}

// void CCoast::DisplayCoastline(void)
// {
//    m_LCoastline.Display();
// }

void CCoast::AppendCellMarkedAsCoastline(C2DIPoint* Pti)
{
   m_VCellsMarkedAsCoastline.push_back(*Pti);
}

void CCoast::AppendCellMarkedAsCoastline(int const nX, int const nY)
{
   m_VCellsMarkedAsCoastline.push_back(C2DIPoint(nX, nY));
}

// void CCoast::SetCellsMarkedAsCoastline(vector<C2DIPoint>* VNewPoints)
// {
//    m_VCellsMarkedAsCoastline = *VNewPoints;
// }

C2DIPoint* CCoast::pPtiGetCellMarkedAsCoastline(int const n)
{
   // NOTE No check to see if n < size()
   return &m_VCellsMarkedAsCoastline[n];
}

// int CCoast::nGetNCellsMarkedAsCoastline(void) const
// {
//    return m_VCellsMarkedAsCoastline.size();
// }

// double CCoast::dGetCoastlineSegmentLength(int const m, int const n)
// {
//    // NOTE no check to see that m is < m_LCoastline.Size(), same for n
//    if (m == n)
//       return 0;
//
//    return hypot(m_LCoastline[n].dGetX() - m_LCoastline[m].dGetX(), m_LCoastline[n].dGetY() - m_LCoastline[m].dGetY());
// }

// double CCoast::dGetCoastlineLengthSoFar(int const n)
// {
//    // NOTE no check to see that n is < m_LCoastline.Size()
//    double dLen = 0;
//    for (int m = 0; m < n; m++)
//       dLen += dGetCoastlineSegmentLength(m, m+1);
//    return dLen;
// }


double CCoast::dGetCurvature(int const nCoastPoint) const
{
   // NOTE no sanity check for nCoastPoint < m_dVCurvature.Size()
   return m_dVCurvature[nCoastPoint];
}

void CCoast::SetCurvature(int const nCoastPoint, double const dCurvature)
{
   // NOTE no check to see if nCoastPoint < m_dVCurvature.size()
   m_dVCurvature[nCoastPoint] = dCurvature;
}


CProfile* CCoast::pGetProfile(int const nProfile)
{
   // NOTE No safety check that nProfile < m_VProfile.size()
   return &m_VProfile[nProfile];
}

void CCoast::AppendProfile(int const nCoastPoint, int const nProfile)
{
   CProfile Profile(nCoastPoint);
   m_VProfile.push_back(Profile);

   m_nVProfileNumber[nCoastPoint] = nProfile;
}

// void CCoast::ReplaceProfile(int const nProfile, vector<C2DPoint>* const pPtVProfileNew)
// {
//    // NOTE No safety check that nProfile < m_VProfile.size()
//    m_VProfile[nProfile].SetAllPointsInProfile(pPtVProfileNew);
// }

int CCoast::nGetNumProfiles(void) const
{
   return m_VProfile.size();
}

bool CCoast::bIsNormalProfileStartPoint(int const nCoastPoint) const
{
   // NOTE no sanity check for nCoastPoint < m_nVProfileNumber.Size()
   if (m_nVProfileNumber[nCoastPoint] != INT_NODATA)
      return true;

   return false;
}

int CCoast::nGetProfileNumber(int const nCoastPoint) const
{
   // Returns INT_NODATA if no profile at this point
   return m_nVProfileNumber[nCoastPoint];
}


void CCoast::CreateAlongCoastlineProfileIndex(void)
{
   // Creates an index containing the numbers of the coastline-normal profiles in along-coast sequence
   for (int nCoastPoint = 0; nCoastPoint < m_LCoastline.nGetSize(); nCoastPoint++)
      if (m_nVProfileNumber[nCoastPoint] != INT_NODATA)
         m_nVProfileCoastIndex.push_back(m_nVProfileNumber[nCoastPoint]);
}

int CCoast::nGetProfileAtAlongCoastlinePosition(int const n) const
{
   // Returns the number of the coastline-normal profile which is at position n in the along-coast sequence
   return m_nVProfileCoastIndex[n];
}

// int CCoast::nGetAlongCoastlineIndexOfProfile(int const nProfile)
// {
//    // Returns the along-coastline position of a coastline-normal profile
//    for (unsigned int n = 0; n < m_nVProfileCoastIndex.size(); n++)
//       if (m_nVProfileCoastIndex[n] == nProfile)
//          return n;
//    return -1;
// }


void CCoast::SetBreakingWaveHeight(int const nCoastPoint, double const dHeight)
{
   // NOTE no check to see if nCoastPoint < m_dVBreakingWaveHeight.size()
   m_dVBreakingWaveHeight[nCoastPoint] = dHeight;
}

double CCoast::dGetBreakingWaveHeight(int const nCoastPoint) const
{
   // NOTE no check to see if nCoastPoint < m_dVBreakingWaveHeight.size()
   return m_dVBreakingWaveHeight[nCoastPoint];
}

void CCoast::SetBreakingWaveOrientation(int const nCoastPoint, double const dOrientation)
{
   // NOTE no check to see if nCoastPoint < m_dVBreakingWaveAngle.size()
   m_dVBreakingWaveAngle[nCoastPoint] = dOrientation;
}

double CCoast::dGetBreakingWaveOrientation(int const nCoastPoint) const
{
   // NOTE no check to see if nCoastPoint < m_dVBreakingWaveAngle.size()
   return m_dVBreakingWaveAngle[nCoastPoint];
}

void CCoast::SetDepthOfBreaking(int const nCoastPoint, double const dDepth)
{
   // NOTE no check to see if nCoastPoint < m_dVDepthOfBreaking.size()
   m_dVDepthOfBreaking[nCoastPoint] = dDepth;
}

double CCoast::dGetDepthOfBreaking(int const nCoastPoint) const
{
   // NOTE no check to see if nCoastPoint < m_dVDepthOfBreaking.size()
   return m_dVDepthOfBreaking[nCoastPoint];
}

void CCoast::SetBreakingDistance(int const nCoastPoint, int const nDist)
{
   // NOTE no check to see if nCoastPoint < m_nVBreakingDistance.size()
   m_nVBreakingDistance[nCoastPoint] = nDist;
}

int CCoast::nGetBreakingDistance(int const nCoastPoint) const
{
   // NOTE no check to see if nCoastPoint < m_nVBreakingDistance.size()
   return m_nVBreakingDistance[nCoastPoint];
}

void CCoast::SetFluxOrientation(int const nCoastPoint, double const dOrientation)
{
   // NOTE no check to see if nCoastPoint < m_dVFluxOrientation.size()
   m_dVFluxOrientation[nCoastPoint] = dOrientation;
}

double CCoast::dGetFluxOrientation(int const nCoastPoint) const
{
   // NOTE no check to see if nCoastPoint < m_dVFluxOrientation.size()
   return m_dVFluxOrientation[nCoastPoint];
}

void CCoast::SetWaveEnergy(int const nCoastPoint, double const dEnergy)
{
   // NOTE no check to see if nCoastPoint < m_dVWaveEnergy.size()
//    assert(bIsFinite(dEnergy));
   m_dVWaveEnergy[nCoastPoint] = dEnergy;
}

double CCoast::dGetWaveEnergy(int const nCoastPoint) const
{
   // NOTE no check to see if nCoastPoint < m_dVWaveEnergy.size()
//    assert(bIsFinite(m_dVWaveEnergy[nCoastPoint]));

   return m_dVWaveEnergy[nCoastPoint];
}


void CCoast::AppendCoastLandform(CCoastLandform* pCoastLandform)
{
   m_pVLandforms.push_back(pCoastLandform);
}

CCoastLandform* CCoast::pGetCoastLandform(int const nCoastPoint)
{
   // NOTE no check to see if nCoastPoint < m_VCellsMarkedAsCoastline.size()
   return m_pVLandforms[nCoastPoint];
}


void CCoast::SetPolygonNode(int const nPoint, int const nNode)
{
   // NOTE no check to see if nPoint < m_nVPolygonNode.size()
   m_nVPolygonNode[nPoint] = nNode;
}

int CCoast::nGetPolygonNode(int const nPoint) const
{
   // NOTE no check to see if nPoint < m_nVPolygonNode.size()
   return m_nVPolygonNode[nPoint];
}

void CCoast::CreatePolygon(int const nGlobalID, int const nCoastID, int const nCoastPoint, C2DIPoint* const PtiNode, C2DIPoint* const PtiAntiNode, int const nProfileUpCoast, int const nProfileDownCoast, vector<C2DPoint>* const pVIn, int const nPointsUpCoastProfile, int const nPointsDownCoastProfile, int const nPiPStartPoint)
{
   CCoastPolygon* pPolygon = new CCoastPolygon(nGlobalID, nCoastID, nCoastPoint, nProfileUpCoast, nProfileDownCoast, pVIn, nPointsUpCoastProfile, nPointsDownCoastProfile, PtiNode, PtiAntiNode, nPiPStartPoint);

   m_pVPolygon.push_back(pPolygon);
}

int CCoast::nGetNumPolygons(void) const
{
   return m_pVPolygon.size();
}

CCoastPolygon* CCoast::pGetPolygon(int const nPoly) const
{
   // NOTE no check to see if nPoint < m_nVPolygonNode.size()
   return m_pVPolygon[nPoly];
}


void CCoast::AppendPolygonLength(const double dLength)
{
   m_dVPolygonLength.push_back(dLength);
}

double CCoast::dGetPolygonLength(int const nIndex) const
{
   // NOTE no check to see if nIndex < m_dVPolygonLength.size()
   return m_dVPolygonLength[nIndex];
}
