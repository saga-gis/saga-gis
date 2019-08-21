/*!
 * \class CCoast
 * \brief Class which is used for coast objects
 * \details TODO This is a more detailed description of the CCoast class.
 * \author David Favis-Mortlock
 * \author Andres Payo
 * \author Jim Hall
 * \date 2017
 * \copyright GNU General Public License
 *
 * \file coast.h
 * \brief Contains CCoast definitions
 *
 */

#ifndef COAST_H
#define COAST_H
/*===============================================================================================================================

 This file is part of CliffMetrics, the Coastal Modelling Environment.

 CliffMetrics is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

===============================================================================================================================*/
#include "cliffmetrics.h"
#include "profile.h"
#include "cell.h"
#include "coast_landform.h"
#include "coast_polygon.h"


class CProfile;
class CCoastLandform;
class CCoastPolygon;

class CCoast
{
private:
   int
      m_nSeaHandedness,          // Handedness of the direction of the sea from the coastline, travelling down-coast (i.e. in direction of increasing coast point indices)
      m_nStartEdge,
      m_nEndEdge;

   CLine
      m_LCoastline;              // Smoothed line of points (external CRS) giving the plan view of the vector coast

   // All these are the same length as m_LCoastline (which may be different each timestep)
   vector<int>
      m_nVProfileNumber,         // At each point on m_LCoastline: INT_NODATA if no profile there, otherwise the profile number
      m_nVBreakingDistance,      // Distance of breaking (in cells), at each point on m_LCoastline
      m_nVPolygonNode;           // At every point on m_LCoastline: INT_NODATA if no nodepoint there, otherwise the node (point of greatest concave curvature) number for a coast polygon
   vector<double>
      m_dVCurvature,             // Curvature at each point on m_LCoastline
      m_dVBreakingWaveHeight,    // Breaking wave height at each point on m_LCoastline
      m_dVBreakingWaveAngle,     // Breaking wave azimuth at each point on m_LCoastline
      m_dVDepthOfBreaking,       // Depth of breaking at each point on m_LCoastline
      m_dVFluxOrientation,       // As in the COVE model, is the angle (measured from azimuth) of alongshore energy/sediment movement; a +ve flux is in direction of increasing indices along coast. At each point on m_LCoastline
      m_dVWaveEnergy;            // Wave energy at each point on m_LCoastline
   vector<C2DIPoint>
      m_VCellsMarkedAsCoastline; // Unsmoothed integer x-y co-ords (grid CRS) of the cell marked as coastline at each point on the vector coastline. Note that this is the same as point zero in profile coords
   vector<CCoastLandform*>
      m_pVLandforms;             // Pointer to a coastal landform object, at each point on m_LCoastline

   // These do not have the same length as m_LCoastline
   vector<CProfile>
      m_VProfile;                // Coast profile objects, in the sequence in which they were created (concave coastline curvature)
   vector<int>
      m_nVProfileCoastIndex;     // Indices of coast profiles sorted into along-coastline sequence, size = number of profiles
   vector<CCoastPolygon*>
      m_pVPolygon;               // Coast polygons, size = number of polygons
   vector<double>
      m_dVPolygonLength;         // Lengths of coast polygons, size = number of polygons

public:
   CCoast(void);
   ~CCoast(void);

   void SetSeaHandedness(int const);
   int nGetSeaHandedness(void) const;

   void SetStartEdge(int const);
   int nGetStartEdge(void) const;

   void SetEndEdge(int const);
   int nGetEndEdge(void) const;

   void AppendToCoastline(double const, double const);
   CLine* pLGetCoastline(void);
   C2DPoint* pPtGetVectorCoastlinePoint(int const);
   int nGetCoastlineSize(void) const;
//    double dGetCoastlineSegmentLength(int const, int const);
//    double dGetCoastlineLengthSoFar(int const);
//    void DisplayCoastline(void);
   void AppendCellMarkedAsCoastline(C2DIPoint*);
   void AppendCellMarkedAsCoastline(int const, int const);
//    void SetCellsMarkedAsCoastline(vector<C2DIPoint>*);
   C2DIPoint* pPtiGetCellMarkedAsCoastline(int const);
//    int nGetNCellsMarkedAsCoastline(void) const;

   double dGetCurvature(int const) const;
   void SetCurvature(int const, double const);

   CProfile* pGetProfile(int const);
   void AppendProfile(int const, int const);
//    void ReplaceProfile(int const, vector<C2DPoint>* const);
   int nGetNumProfiles(void) const;
   bool bIsNormalProfileStartPoint(int const) const;
   int nGetProfileNumber(int const) const;

   void CreateAlongCoastlineProfileIndex(void);
   int nGetProfileAtAlongCoastlinePosition(int const) const;
//    int nGetAlongCoastlineIndexOfProfile(int const);

   void SetBreakingWaveHeight(int const, double const);
   double dGetBreakingWaveHeight(int const) const;
   void SetBreakingWaveOrientation(int const, double const);
   double dGetBreakingWaveOrientation(int const) const;
   void SetDepthOfBreaking(int const, double const);
   double dGetDepthOfBreaking(int const) const;
   void SetBreakingDistance(int const, int const);
   int nGetBreakingDistance(int const) const;
   void SetFluxOrientation(int const, double const);
   double dGetFluxOrientation(int const) const;
   void SetWaveEnergy(int const, double const);
   double dGetWaveEnergy(int const) const;

   void AppendCoastLandform(CCoastLandform*);
   CCoastLandform* pGetCoastLandform(int const);

   void SetPolygonNode(int const, int const);
   int nGetPolygonNode(int const) const;
   void CreatePolygon(int const, int const, int const, C2DIPoint* const, C2DIPoint* const, int const, int const, vector<C2DPoint>* const, int const, int const, int const);
   int nGetNumPolygons(void) const;
   CCoastPolygon* pGetPolygon(int const) const;

   void AppendPolygonLength(const double);
   double dGetPolygonLength(int const) const;
};
#endif //COAST_H

