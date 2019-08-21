/*!
 *
 * \file profile.cpp
 * \brief CProfile routines
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
#include <cmath>
#include <vector>
#include <algorithm>

#include "cliffmetrics.h"
#include "profile.h"


CProfile::CProfile(int const nCoastPoint)
:  m_bStartOfCoast(false),
   m_bEndOfCoast(false),
   m_bHitLand(false),
   m_bHitCoast(false),
   m_bTooShort(false),
   m_bTruncated(false),
   m_bHitAnotherProfile(false),
   m_nNumCoastPoint(nCoastPoint)
{
}

CProfile::~CProfile(void)
{
}


int CProfile::nGetNumCoastPoint(void) const
{
   return m_nNumCoastPoint;
}


void CProfile::SetStartOfCoast(bool const bFlag)
{
   m_bStartOfCoast = bFlag;
}

bool CProfile::bStartOfCoast(void) const
{
   return m_bStartOfCoast;
}

void CProfile::SetEndOfCoast(bool const bFlag)
{
   m_bEndOfCoast = bFlag;
}

bool CProfile::bEndOfCoast(void) const
{
   return m_bEndOfCoast;
}

void CProfile::SetHitLand(bool const bFlag)
{
   m_bHitLand = bFlag;
}

bool CProfile::bHitLand(void) const
{
   return m_bHitLand;
}

void CProfile::SetHitCoast(bool const bFlag)
{
   m_bHitCoast = bFlag;
}

bool CProfile::bHitCoast(void) const
{
   return m_bHitCoast;
}

void CProfile::SetTooShort(bool const bFlag)
{
   m_bTooShort = bFlag;
}

bool CProfile::bTooShort(void) const
{
   return m_bTooShort;
}

void CProfile::SetTruncated(bool const bFlag)
{
   m_bTruncated = bFlag;
}

bool CProfile::bTruncated(void) const
{
   return m_bTruncated;
}

void CProfile::SetHitAnotherProfile(bool const bFlag)
{
   m_bHitAnotherProfile = bFlag;
}

bool CProfile::bHitAnotherProfile(void) const
{
   return m_bHitAnotherProfile;
}

bool CProfile::bProfileOK(void) const
{
   // All profiles without problems, but not start- or end-of-coast profiles
   if ((! m_bStartOfCoast) &&
       (! m_bEndOfCoast) &&
       (! m_bHitLand)    &&
       //(! m_bHitCoast)   &&
       (! m_bTooShort)   &&
       //(! m_bTruncated)  &&
       (! m_bHitAnotherProfile))
      return true;

   return false;
}	

bool CProfile::bOKIncStartAndEndOfCoast(void) const
{
   // All profiles without problems, including start- and end-of-coast profiles
   if ((! m_bHitLand)    &&
       //(! m_bHitCoast)   &&
       (! m_bTooShort)) //  &&
       //(! m_bTruncated)  &&
       //(! m_bHitAnotherProfile))
      return true;

   return false;
}




void CProfile::SetAllPointsInProfile(vector<C2DPoint>* VNewPoints)
{
   m_VPoints = *VNewPoints;
}

void CProfile::SetPointInProfile(int const nPoint, double const dNewX, double const dNewY)
{
   // NOTE No check to see if nPoint < m_VPoints,size()
   m_VPoints[nPoint] = C2DPoint(dNewX, dNewY);
}

void CProfile::AppendPointInProfile(double const dNewX, double const dNewY)
{
   m_VPoints.push_back(C2DPoint(dNewX, dNewY));
}

void CProfile::AppendPointInProfile(C2DPoint* const pPt)
{
   m_VPoints.push_back(*pPt);
}

bool CProfile::bInsertIntersection(double const dX, double const dY, int const nSeg)
{
   // Safety check
   if (nSeg >= nGetNumLineSegments())
      return false;

   vector<C2DPoint>::iterator it;
   it = m_VPoints.begin();

   // Do the insertion
   m_VPoints.insert(it+nSeg+1, C2DPoint(dX, dY));

   // Now insert a line segment in the associated multi-line, this will inherit the profile/line seg details from the preceding line segment
   CMultiLine::InsertLineSegment(nSeg);

   return true;
}

void CProfile::TruncateProfile(int const nSize)
{
   m_VPoints.resize(nSize);
}

// void CProfile::TruncateAndSetPointInProfile(int const nPoint, double const dNewX, double const dNewY)
// {
//    m_VPoints.resize(nPoint+1);
//    m_VPoints[nPoint] = C2DPoint(dNewX, dNewY);
// }


// void CProfile::ShowProfile(void) const
// {
//    for (unsigned int n = 0; n < m_VPoints.size(); n++)
//    {
//       cout << n << " [" << m_VPoints[n].dGetX() << "][" << m_VPoints[n].dGetY() << "]" << endl;
//    }
// }

int CProfile::nGetProfileSize(void) const
{
   return m_VPoints.size();
}

int CProfile::nGetCliffTopPoint(void) const
{
   return m_nCliffTopPoint;
}
double CProfile::dGetCliffTopChainage(void) const
{
   return m_nCliffTopChainage;
}
int CProfile::nGetCliffToePoint(void) const
{
   return m_nCliffToePoint;
}
double CProfile::dGetCliffToeChainage(void) const
{
   return m_nCliffToeChainage;
}

void CProfile::SetCliffTopPoint(const int n)
{
   m_nCliffTopPoint = n;
}
void CProfile::SetCliffTopChainage(const double x)
{
   m_nCliffTopChainage = x;
}
void CProfile::SetCliffToePoint(const int n)
{
   m_nCliffToePoint = n;
}
void CProfile::SetCliffToeChainage(const double x)
{
   m_nCliffToeChainage = x;
}
void CProfile::SetCliffTopQualityFlag(const bool isOK)
{
   m_bCliffTopQualityFlag = isOK;
}
void CProfile::SetCliffToeQualityFlag(const bool isOK)
{
   m_bCliffToeQualityFlag = isOK;
}
bool CProfile::bGetCliffTopQualityFlag(void) const
{
   return m_bCliffTopQualityFlag;
}

bool CProfile::bGetCliffToeQualityFlag(void) const
{
   return m_bCliffToeQualityFlag;
}

C2DPoint* CProfile::pPtGetPointInProfile(int const n)
{
   return &m_VPoints[n];
}

vector<C2DPoint> CProfile::PtVGetThisPointAndAllAfter(int const nStart)
{
   return vector<C2DPoint> (m_VPoints.begin() + nStart, m_VPoints.end());
}

void CProfile::RemoveLineSegment(int const nPoint)
{
   m_VPoints.erase(m_VPoints.begin()+nPoint);
   CMultiLine::RemoveLineSegment(nPoint);
}

bool CProfile::bIsPointInProfile(double const dX, double const dY)
{
   C2DPoint Pt(dX, dY);
   vector<C2DPoint>::iterator it = std::find(m_VPoints.begin(), m_VPoints.end(), &Pt);
   if (it != m_VPoints.end())
      return true;
   else
      return false;
}

bool CProfile::bIsPointInProfile(double const dX, double const dY, int& nPoint)
{
   C2DPoint Pt(dX, dY);
   vector<C2DPoint>::iterator it = std::find(m_VPoints.begin(), m_VPoints.end(), &Pt);
   if (it != m_VPoints.end())
   {
      // Found, so return true and set nPoint to be the index of the point which was found
      nPoint = it - m_VPoints.begin();
      return true;
   }
   else
      return false;
}

// int CProfile::nFindInsertionLineSeg(double const dInsertX, double const dInsertY)
// {
//    for (unsigned int n = 0; n < m_VPoints.size()-1; n++)
//    {
//       double
//          dThisX = m_VPoints[n].dGetX(),
//          dThisY = m_VPoints[n].dGetY(),
//          dNextX = m_VPoints[n+1].dGetX(),
//          dNextY = m_VPoints[n+1].dGetY();
//
//       bool
//          bBetweenX = false,
//          bBetweenY = false;
//
//       if (dNextX >= dThisX)
//       {
//          // Ascending
//          if ((dInsertX >= dThisX) && (dInsertX <= dNextX))
//             bBetweenX = true;
//       }
//       else
//       {
//          // Descending
//          if ((dInsertX >= dNextX) && (dInsertX <= dThisX))
//             bBetweenX = true;
//       }
//
//       if (dNextY >= dThisY)
//       {
//          // Ascending
//          if ((dInsertY >= dThisY) && (dInsertY <= dNextY))
//             bBetweenY = true;
//       }
//       else
//       {
//          // Descending
//          if ((dInsertY >= dNextY) && (dInsertY <= dThisY))
//             bBetweenY = true;
//       }
//
//       if (bBetweenX && bBetweenY)
//          return n;
//    }
//
//    return -1;
// }


// void CProfile::AppendPointShared(bool const bShared)
// {
//    m_bVShared.push_back(bShared);
// }

// bool CProfile::bPointShared(int const n) const
// {
//    // NOTE No check to see if n < size()
//    return m_bVShared[n];
// }


// void CProfile::SetCoastPolyToLeft(int const n, int const nPoly)
// {
//    // NOTE No check to see if n < size()
//    m_nVCoastPolyToLeft[n] = nPoly;
// }

// int CProfile::nGetCoastPolyToleft(int const n)
// {
//    // NOTE No check to see if n < size()
//    return m_nVCoastPolyToLeft[n];
// }


// void CProfile::SetCoastPolyToRight(int const n, int const nPoly)
// {
//    // NOTE No check to see if n < size()
//    m_nVCoastPolyToRight[n] = nPoly;
// }

// int CProfile::nGetCoastPolyToRight(int const n)
// {
//    // NOTE No check to see if n < size()
//    return m_nVCoastPolyToRight[n];
// }


void CProfile::AppendCellInProfile(C2DIPoint* Pti)
{
   // In grid CRS
   m_VCellInProfile.push_back(*Pti);
}

void CProfile::AppendCellInProfile(int const nX, int const nY)
{
   // In grid CRS
   m_VCellInProfile.push_back(C2DIPoint(nX, nY));
}

// void CProfile::SetCellsInProfile(vector<C2DIPoint>* VNewPoints)
// {
//    // In grid CRS
//    m_VCellInProfile = *VNewPoints;
// }

vector<C2DIPoint>* CProfile::pPtiVGetCellsInProfile(void)
{
   // In grid CRS
   return &m_VCellInProfile;
}

C2DIPoint* CProfile::pPtiGetCellInProfile(int const n)
{
   // In grid CRS
   // NOTE No check to see if n < size()
   return &m_VCellInProfile[n];
}

int CProfile::nGetNumCellsInProfile(void) const
{
   // In grid CRS
   return m_VCellInProfile.size();
}


// vector<C2DPoint>* CProfile::PtVGetCellsInProfileExtCRS(void)
// {
//    // In external CRS
//    return &m_VCellInProfileExtCRS;
// }

void CProfile::AppendCellInProfileExtCRS(double const dX, double const dY)
{
   // In external CRS
   m_VCellInProfileExtCRS.push_back(C2DPoint(dX, dY));
}

// int CProfile::nGetCellGivenDepth(CRasterGrid* const pGrid, double const dDepth, C2DIPoint* pPtiFound)
// {
//    int nIndex = INT_NODATA;      // If not found, i.e. if every profile cell has sea depth greater than dDepth
//    pPtiFound = &m_VCellInProfile[0];
//
//    for (int n = m_VCellInProfile.size()-1; n >= 0; n--)
//    {
//       int
//          nX = m_VCellInProfile[n].nGetX(),
//          nY = m_VCellInProfile[n].nGetY();
//
//       double dThisDepth = pGrid->pGetCell(nX, nY)->dGetSeaDepth();
//       if (dThisDepth <= dDepth)
//       {
//          nIndex = n;                            // Grid CRS units
//          pPtiFound = &m_VCellInProfile[n];      // Grid CRS
//          break;
//       }
//    }
//
//    return nIndex;
// }

int CProfile::nGetCellGivenDepth(CRasterGrid* const pGrid, double const dDepth)
{
   int nIndex = INT_NODATA;      // If not found, i.e. if every profile cell has sea depth greater than dDepth

   for (int n = m_VCellInProfile.size()-1; n >= 0; n--)
   {
      int
         nX = m_VCellInProfile[n].nGetX(),
         nY = m_VCellInProfile[n].nGetY();

      double dThisDepth = pGrid->pGetCell(nX, nY)->dGetSeaDepth();
      if (dThisDepth <= dDepth)
      {
         nIndex = n;                            // Grid CRS units
         break;
      }
   }

   return nIndex;
}
