/*!
 *
 * \file calc_curvature.cpp
 * \brief Calculates curvature of 2D vectors
 * \details TODO A more detailed description of these routines.
 * \author David Favis-Mortlock
 * \author Andres Payo
 * \author Jim Hall
 * \date 2017
 * \copyright GNU General Public License
 *
 */

/*==============================================================================================================================

 This file is part of CliffMetrics, the Coastal Modelling Environment.

 CliffMetrics is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

==============================================================================================================================*/
#include <cmath>

#include "cliffmetrics.h"
#include "delineation.h"
#include "coast.h"


/*===============================================================================================================================

 Calculates curvature at each point on a coastline. The ends of the coastlinme are handled differently

===============================================================================================================================*/
void CDelineation::DoCoastCurvature(int const nCoast, int const nHandedness)
{
   int
      nCoastSize = m_VCoast[nCoast].nGetCoastlineSize(),
      nAvgInterval = 9;                // Arbitrary, used at the coastline ends
   double
      dAvgKappaBegin = 0,
      dAvgKappaEnd = 0;

   // Do the middle of the coastline (omit the beginning and end portions)
   for (int nThis = m_nCoastCurvatureInterval; nThis < (nCoastSize - m_nCoastCurvatureInterval); nThis++)
   {
      // Calculate the average position of the m_nCoastCurvatureInterval coastline points before this point, and after this point
      double
         dXAvgBefore = 0,
         dYAvgBefore = 0,
         dXAvgAfter = 0,
         dYAvgAfter = 0;

      for (int n = 0; n < m_nCoastCurvatureInterval; n++)
      {
         int
            nPointBefore = nThis - n,
            nPointAfter = nThis + n;

         dXAvgBefore += m_VCoast[nCoast].pPtGetVectorCoastlinePoint(nPointBefore)->dGetX();
         dYAvgBefore += m_VCoast[nCoast].pPtGetVectorCoastlinePoint(nPointBefore)->dGetY();
         dXAvgAfter += m_VCoast[nCoast].pPtGetVectorCoastlinePoint(nPointAfter)->dGetX();
         dYAvgAfter += m_VCoast[nCoast].pPtGetVectorCoastlinePoint(nPointAfter)->dGetY();
      }

      dXAvgBefore /= m_nCoastCurvatureInterval;
      dYAvgBefore /= m_nCoastCurvatureInterval;
      dXAvgAfter /= m_nCoastCurvatureInterval;
      dYAvgAfter /= m_nCoastCurvatureInterval;

      C2DPoint
         PtBefore(dXAvgBefore, dYAvgBefore),
         PtAfter(dXAvgAfter, dYAvgAfter);

      // Now get the signed curvature based on these average points
      double dKappa = dCalcCurvature(nHandedness, &PtBefore, m_VCoast[nCoast].pPtGetVectorCoastlinePoint(nThis), &PtAfter);

      // Set this curvature
      m_VCoast[nCoast].SetCurvature(nThis, dKappa);

      // Now find some 'representative' values (somewhat arbitrary) to use at the coastline ends
      if (nThis <= (m_nCoastCurvatureInterval + nAvgInterval))
         dAvgKappaBegin += dKappa;

      if (nThis > (nCoastSize - m_nCoastCurvatureInterval - nAvgInterval))
         dAvgKappaEnd += dKappa;
   }

   // Calculate these 'representative' average curvature values
   dAvgKappaBegin /= nAvgInterval;
   dAvgKappaEnd /= nAvgInterval;

   // And set them at the beginning of the coastline
   for (int nThis = m_nCoastCurvatureInterval-1; nThis >= 0; nThis--)
      m_VCoast[nCoast].SetCurvature(nThis, dAvgKappaBegin);

   // Do the same for the end of the coastline
   for (int nThis = (nCoastSize - m_nCoastCurvatureInterval); nThis < nCoastSize; nThis++)
      m_VCoast[nCoast].SetCurvature(nThis, dAvgKappaEnd);
}

/*===============================================================================================================================

 Calculates signed curvature from three points on a line. Returns +ve values for concave, -ve for convex, and zero if the points are co-linear. Curvature is multiplied by 1000 to give easier-to-read numbers

===============================================================================================================================*/
double CDelineation::dCalcCurvature(int const nHandedness, C2DPoint* const PtBefore, C2DPoint* const PtThis, C2DPoint* const PtAfter)
{
   double
      dXBefore = PtBefore->dGetX(),
      dYBefore = PtBefore->dGetY(),
      dXThis = PtThis->dGetX(),
      dYThis = PtThis->dGetY(),
      dXAfter = PtAfter->dGetX(),
      dYAfter = PtAfter->dGetY();

   // This part is Algorithm HK2003 from S. Hermann and R. Klette. Multigrid analysis of curvature estimators. In Proc. Image Vision Computing New Zealand, pages 108–112, Massey University, 2003.) from "A Comparative Study on 2D Curvature Estimators", Simon Hermann and Reinhard Klette
   double dBefore = hypot((dXThis - dXBefore), (dYThis - dYBefore));
   double dAfter = hypot((dXThis - dXAfter), (dYThis - dYAfter));

   double dThetaBefore = atan2(dXBefore - dXThis, dYBefore - dYThis);
   double dThetaAfter = atan2(dXThis - dXAfter, dYThis - dYAfter);

   double dThetaMean = (dThetaBefore + dThetaAfter) / 2;

   double dDeltaBefore = tAbs(dThetaBefore - dThetaMean);
   double dDeltaAfter = tAbs(dThetaAfter - dThetaMean);

   double dKappa = (dDeltaAfter / (2 * dAfter)) + (dDeltaBefore / (2 * dBefore));

   // Now calculate the cross product to determine whether the three points are convex or concave
   double dZ = dCrossProduct(dXBefore, dYBefore, dXThis, dYThis, dXAfter, dYAfter);

   int nShape = 0;
   if (dZ < 0)
      // Concave if right-handed
      nShape = 1;
   else if (dZ == 0)
      // Co-linear
      nShape = 0;
   else if (dZ > 0)
      // Convex if right-handed
      nShape = -1;

   // Reverse if left-handed
   if (nHandedness == LEFT_HANDED)
      nShape *= -1;

   return nShape * dKappa * 1000;
}

