/*!
 *
 * \class CProfile
 * \brief Class used to represent coast profile objects
 * \details TODO This is a more detailed description of the CProfile class.
 * \author David Favis-Mortlock
 * \author Andres Payo
 * \author Jim Hall
 * \date 2017
 * \copyright GNU General Public License
 *
 * \file profile.h
 * \brief Contains CProfile definitions
 *
 */

#ifndef PROFILE_H
#define PROFILE_H
/*===============================================================================================================================

 This file is part of CliffMetrics, the Coastal Modelling Environment.

 CliffMetrics is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

===============================================================================================================================*/
#include "cliffmetrics.h"
#include "2d_point.h"
#include "2di_point.h"
#include "multi_line.h"
#include "raster_grid.h"


class CProfile : public CMultiLine
{
private:
   bool
      m_bStartOfCoast,
      m_bEndOfCoast,
      m_bHitLand,
      m_bHitCoast,
      m_bTooShort,
      m_bTruncated,
      m_bHitAnotherProfile,
      m_bCliffTopQualityFlag,
      m_bCliffToeQualityFlag;
   int
      m_nNumCoastPoint,          // The coastline point at which this profile hits the coast (not necessarily coincident with the profile start cell)
      m_nCliffTopPoint,          // The profile point at which the cliff top is located
      m_nCliffToePoint;          // The profile point at which the cliff toe is located
   double
      m_nCliffTopChainage,
      m_nCliffToeChainage;

   vector<C2DIPoint>
      m_VCellInProfile;         // In grid CRS, the integer coords of the cells 'under' this profile. NOTE Point zero is the same as 'cell marked as coastline' in coast object

   // The following have the same length as m_VCellInProfile
   vector<C2DPoint>
      m_VCellInProfileExtCRS;   // In external CRS, the coords of cells 'under' this profile
//    vector<bool>
//       m_bVShared;                // Is this profile point part of a multi-line?
   vector<int>
      m_nVCoastPolyToLeft,       // The ID of the CoastPolygon to the left (looking seaward)
      m_nVCoastPolyToRight;      // The ID of the CoastPolygon to the right (looking seaward)

public:
   explicit CProfile(int const);
   ~CProfile(void);

   int nGetNumCoastPoint(void) const;

   void SetCliffTopPoint(int const);
   int nGetCliffTopPoint(void) const;
   double dGetCliffTopChainage(void) const;
   void SetCliffTopChainage(double const);
   
   void SetCliffToePoint(int const);
   int nGetCliffToePoint(void) const;
   double dGetCliffToeChainage(void) const;
   void SetCliffToeChainage(double const);
   
   void SetCliffTopQualityFlag(bool const);
   bool bGetCliffTopQualityFlag(void) const;
   void SetCliffToeQualityFlag(bool const);
   bool bGetCliffToeQualityFlag(void) const;
   
   void SetStartOfCoast(bool const);
   bool bStartOfCoast(void) const;

   void SetEndOfCoast(bool const);
   bool bEndOfCoast(void) const;

   void SetHitLand(bool const);
   bool bHitLand(void) const;

   void SetHitCoast(bool const);
   bool bHitCoast(void) const;

   void SetTooShort(bool const);
   bool bTooShort(void) const;

   void SetTruncated(bool const);
   bool bTruncated(void) const;

   void SetHitAnotherProfile(bool const);
   bool bHitAnotherProfile(void) const;

   bool bProfileOK(void) const;
   bool bOKIncStartAndEndOfCoast(void) const;
   bool bOKIncStartOfCoast(void) const;

   void SetAllPointsInProfile(vector<C2DPoint>*);
   void SetPointInProfile(int const, double const, double const);
   void AppendPointInProfile(double const, double const);
   void AppendPointInProfile(C2DPoint* const);
   void TruncateProfile(int const);
//    void TruncateAndSetPointInProfile(int const, double const, double const);
   bool bInsertIntersection(double const, double const, int const);
//    void ShowProfile(void) const;
   int nGetProfileSize(void) const;
   C2DPoint* pPtGetPointInProfile(int const);
   vector<C2DPoint> PtVGetThisPointAndAllAfter(int const);
   void RemoveLineSegment(int const);
   bool bIsPointInProfile(double const, double const);
   bool bIsPointInProfile(double const, double const, int&);
//    int nFindInsertionLineSeg(double const, double const);

//    void AppendPointShared(bool const);
//    bool bPointShared(int const) const;

//    void SetCoastPolyToLeft(int const, int const);
//    int nGetCoastPolyToleft(int const);

//    void SetCoastPolyToRight(int const, int const);
//    int nGetCoastPolyToRight(int const);

   void AppendCellInProfile(C2DIPoint*);
   void AppendCellInProfile(int const, int const);
//    void SetCellsInProfile(vector<C2DIPoint>*);
   vector<C2DIPoint>* pPtiVGetCellsInProfile(void);
   C2DIPoint* pPtiGetCellInProfile(int const);
   int nGetNumCellsInProfile(void) const;

   void AppendCellInProfileExtCRS(double const, double const);
//    vector<C2DPoint>* PtVGetCellsInProfileExtCRS(void);

//    int nGetCellGivenDepth(CRasterGrid* const, double const, C2DIPoint*);
   int nGetCellGivenDepth(CRasterGrid* const, double const);
};
#endif //PROFILE_H

