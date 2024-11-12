/*!
 *
 * \file create_profiles.cpp
 * \brief Creates profiles which are approximately normal to the coastline, these will become inter-polygon boundaries
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
// #include <assert.h>
#include <cmath>

#include <iostream>
using std::cout;
using std::cerr;
using std::endl;
using std::ios;

#include <iomanip>
using std::setiosflags;
using std::setprecision;

#include <algorithm>

#include <utility>
using std::pair;
using std::make_pair;

#include "cliffmetrics.h"
#include "delineation.h"
#include "coast.h"


/*===============================================================================================================================

 Function used to sort coastline curvature values

===============================================================================================================================*/
bool bCurvaturePairCompare(const pair<int, double> &prLeft, const pair<int, double> &prRight)
{
   // Sort in descending order (i.e. most concave first)
   return prLeft.second > prRight.second;
}


/*===============================================================================================================================

 Create profiles normal to the coastline, modifies these if they intersect, then puts the profiles onto the raster grid

===============================================================================================================================*/
int CDelineation::nCreateAllProfilesAndCheckForIntersection(void)
{
   // Create all coastline-normal profiles, in coastline-concave-curvature sequence i.e. the first profiles are created 'around' the most concave bits of coast. An index is also created which allows profiles to be accessed in along-cost sequence
   int nRet = nCreateAllProfiles();
   if (nRet != RTN_OK)
      return nRet;

   // Check to see which coastline-normal profiles intersect. Then modify intersecting profiles so that the sections of each profile seaward of the point of intersection are 'shared' i.e. are multi-lines. This creates the boundaries of the triangular polygons
  // nRet = nModifyAllIntersectingProfiles();
  // if (nRet != RTN_OK)
  //    return nRet;

   // Put all valid coastline-normal profiles onto the raster grid: but if the profile is not long enough, or crosses a coastline or hits dry land, then mark the profile as invalid
   nRet = nPutAllProfilesOntoGrid();
   if (nRet != RTN_OK)
      return nRet;

   return RTN_OK;
}


/*===============================================================================================================================

 Create coastline-normal profiles for all coastlines: first at a limited number of 'cape' positions, then at locations of greatest concave curvature of the vector coastline. Finally, grid-edge profiles are created

===============================================================================================================================*/
int CDelineation::nCreateAllProfiles(void)
{
   // For each coastline, search for coastline points from which to start a normal profile
   for (unsigned int nCoast = 0; nCoast < m_VCoast.size(); nCoast++)
   {
      int
         nProfile = -1,
         nCoastSize = m_VCoast[nCoast].nGetCoastlineSize();

      // Create a vector of pairs: the first value of the pair is the coastline point, the second is the coastline's curvature at that point
      vector<pair<int, double> > prVCurvature;
      vector<double> dVCurvature;
      for (int nCoastPoint = 0; nCoastPoint < nCoastSize; nCoastPoint++)
      {
         double dCurvature = m_VCoast[nCoast].dGetCurvature(nCoastPoint);
         prVCurvature.push_back(make_pair(nCoastPoint, dCurvature));
         dVCurvature.push_back(dCurvature);
      }

      // And create a vector of the same length, to mark coastline points which have been searched
      vector<bool> bVPointSearched(nCoastSize, false);

      // Create Normals profiles at every coast points on the coastline 
      while (true)
      {
         // How many coastline points are still to be searched?
         int nStillToSearch = 0;
         for (int n = 0; n < nCoastSize; n++)
         {
            if (! bVPointSearched[n])
               nStillToSearch++;
         }
         // Are there any coastline points still to be searched?
         if (nStillToSearch == 0)
            {// Nope, we are done here
            break;
            }
            // Look along the vector of pairs starting at the most convex end
            for (int n = 0; n < nCoastSize; n++)
            {
                  // Curvature at this coastline point is more convex (i.e. less than) the convexity threshold, so this is a potential location for a 'cape' profile
                  int nCapePoint = prVCurvature[n].first;
                  if (! bVPointSearched[nCapePoint])
                  {
                     // We have not already searched this point, so try putting a 'cape' profile here
                     int nRet = nCreateProfile(nCoast, nCapePoint, nProfile);
                     bVPointSearched[nCapePoint] = true;

                    if (nRet != RTN_OK)
                        // This cape profile is no good (has hit coast, or hit dry land, etc.) so forget about it
                        continue;
                  }
               }
            }
        
 
      // Did we fail to create any normal profiles? If so, quit
      if (nProfile < 0)
      {
         string strErr = ERR + ": could not create profiles for coastline " + tToStr(nCoast) ;
         strErr += ". Check the initial SWL";
         strErr += "\n";
         cerr << strErr;
         LogStream << strErr;
         // return RTN_ERR_BADPROFILE;
      }

     // Create an index to the profiles in along-coast sequence
      m_VCoast[nCoast].CreateAlongCoastlineProfileIndex();

   }

   return RTN_OK;
}


/*==============================================================================================================================

 Creates a coastline-normal profile

===============================================================================================================================*/
int CDelineation::nCreateProfile(int const nCoast, int const nProfileStartPoint, int& nProfile)
{
   // OK, we have flagged the start point of this new coastline-normal profile, so create it. Make the start of the profile the centroid of the actual cell that is marked as coast (not the cell under the smoothed vector coast, they may well be different)
   int nCoastSize = m_VCoast[nCoast].nGetCoastlineSize();

   C2DPoint PtStart;                                     // PtStart has coordinates in external CRS
   PtStart.SetX(dGridCentroidXToExtCRSX(m_VCoast[nCoast].pPtiGetCellMarkedAsCoastline(nProfileStartPoint)->nGetX()));
   PtStart.SetY(dGridCentroidYToExtCRSY(m_VCoast[nCoast].pPtiGetCellMarkedAsCoastline(nProfileStartPoint)->nGetY()));

   C2DPoint PtSeaEnd;                                       // Also in external CRS
   C2DPoint PtLandEnd;                                      // Also in external CRS
   if (nGetCoastNormalEndPoint(nCoast, nProfileStartPoint, nCoastSize, &PtStart, m_dCoastNormalLength, &PtSeaEnd, &PtLandEnd) != RTN_OK)
      // Could not solve end-point equation, or profile end point is off-grid, so forget about this profile
      return RTN_ERR_OFFGRID_ENDPOINT;

   // No problems, so create the new profile
   m_VCoast[nCoast].AppendProfile(nProfileStartPoint, ++nProfile);

   // And create the profile's coastline-normal vector (start and end points are in external CRS)
   vector<C2DPoint> VNormal;
   VNormal.push_back(PtStart);
   VNormal.push_back(PtSeaEnd);     // to draw the profiles LANDWARDS!!!
   //VNormal.push_back(PtLandEnd);  // to draw the profiles SEAWARDS!!!! 

   CProfile* pProfile = m_VCoast[nCoast].pGetProfile(nProfile);
   pProfile->SetAllPointsInProfile(&VNormal);

   // Create the profile's CMultiLine then set nProfile as the only co-incident profile of the only line segment
   pProfile->AppendLineSegment();
   pProfile->AppendCoincidentProfileToLineSegments(make_pair(nProfile, 0));

//    LogStream << setiosflags(ios::fixed) << m_ulTimestep << ": profile " << nProfile << " created at point " << nProfileStartPoint << " from [" << PtStart.dGetX() << "][" << PtStart.dGetY() << "] to [" << PtEnd.dGetX() << "][" << PtEnd.dGetY() << "]" << endl;
   return RTN_OK;
}


/*==============================================================================================================================

 Finds the end point of a coastline-normal line, given the start point on the vector coastline. All coordinates are in the external CRS

===============================================================================================================================*/
int CDelineation::nGetCoastNormalEndPoint(int const nCoast, int const nStartCoastPoint, int const nCoastSize, C2DPoint* const pPtStart, double const dLineLength, C2DPoint* pPtSeaEnd, C2DPoint* pPtLandEnd)
{
   // If at beginning or end of coast, need special treatment for points before and points after
   int nCoastPointBefore = tMax(nStartCoastPoint-1, 0);
   int nCoastPointAfter = tMin(nStartCoastPoint+1, nCoastSize-1);

   // Get the y = a * x + b equation of the straight line linking the coastline points before and after 'this' coastline point
   C2DPoint PtBefore = *m_VCoast[nCoast].pPtGetVectorCoastlinePoint(nCoastPointBefore);           // PtBefore has coordinates in external CRS
   C2DPoint PtAfter = *m_VCoast[nCoast].pPtGetVectorCoastlinePoint(nCoastPointAfter);             // PtAfter has coordinates in external CRS

   // For this linking line, slope a = (y2 - y1) / (x2 - x1)
   double dYDiff = PtAfter.dGetY() - PtBefore.dGetY();
   double dXDiff = PtAfter.dGetX() - PtBefore.dGetX();

   double dXEnd1 = 0, dXEnd2 = 0, dYEnd1 = 0, dYEnd2 = 0;

   if (bFPIsEqual(dYDiff, 0, TOLERANCE))
   {
      // The linking line runs W-E or E-W, so a straight line at right angles to this runs N-S or S-N. Calculate the two possible end points for this coastline-normal profile
      dXEnd1 = dXEnd2 = pPtStart->dGetX();
      dYEnd1 = pPtStart->dGetY() + dLineLength;
      dYEnd2 = pPtStart->dGetY() - dLineLength;
   }
   else if (bFPIsEqual(dXDiff, 0, TOLERANCE))
   {
      // The linking line runs N-S or S-N, so a straight line at right angles to this runs W-E or E-W. Calculate the two possible end points for this coastline-normal profile
      dYEnd1 = dYEnd2 = pPtStart->dGetY();
      dXEnd1 = pPtStart->dGetX() + dLineLength;
      dXEnd2 = pPtStart->dGetX() - dLineLength;
   }
   else
   {
      // The linking line runs neither W-E nor N-S so we have to work a bit harder to find the end-point of the coastline-normal profile
      double dA = dYDiff / dXDiff;

      // Now calculate the equation of the straight line which is perpendicular to this linking line
      double dAPerp = -1 / dA;
      double dBPerp = pPtStart->dGetY() - (dAPerp * pPtStart->dGetX());

      // Calculate the end point of the profile: first do some substitution then rearrange as a quadratic equation i.e. in the form Ax^2 + Bx + C = 0 (see http://math.stackexchange.com/questions/228841/how-do-i-calculate-the-intersections-of-a-straight-line-and-a-circle)
      double dQuadA = 1 + (dAPerp * dAPerp);
      double dQuadB = 2 * ((dBPerp * dAPerp) - (dAPerp * pPtStart->dGetY()) - pPtStart->dGetX());
      double dQuadC = ((pPtStart->dGetX() * pPtStart->dGetX()) + (pPtStart->dGetY() * pPtStart->dGetY()) + (dBPerp * dBPerp) - (2 * pPtStart->dGetY() * dBPerp) - (dLineLength * dLineLength));

      // Solve for x and y using the quadratic formula x = (−B ± sqrt(B^2 − 4AC)) / 2A
      double dDiscriminant = (dQuadB * dQuadB) - (4 * dQuadA * dQuadC);
      if (dDiscriminant < 0)
      {
         LogStream << ERR << "timestep " << m_ulTimestep << ": discriminant < 0 when finding profile end point on coastline " << nCoast << ", from coastline point " << nStartCoastPoint << "), ignored" << endl;
         return RTN_ERR_BADENDPOINT;
      }

      dXEnd1 = (-dQuadB + sqrt(dDiscriminant)) / (2 * dQuadA);
      dYEnd1 = (dAPerp * dXEnd1) + dBPerp;
      dXEnd2 = (-dQuadB - sqrt(dDiscriminant)) / (2 * dQuadA);
      dYEnd2 = (dAPerp * dXEnd2) + dBPerp;
   }

   // We have two possible solutions, so decide which of the two endpoints to use then create the profile end-point (coordinates in external CRS)
   int nSeaHand = m_VCoast[nCoast].nGetSeaHandedness();            // Assumes handedness is either 0 or 1 (i.e. not -1)

   *pPtLandEnd = PtChooseLandEndPoint(nSeaHand, &PtBefore, &PtAfter, dXEnd1, dYEnd1, dXEnd2, dYEnd2);
   *pPtSeaEnd = PtChooseSeaEndPoint(nSeaHand, &PtBefore, &PtAfter, dXEnd1, dYEnd1, dXEnd2, dYEnd2);

// cout<< pPtSeaEnd->dGetX() << ", " << pPtSeaEnd->dGetY() << endl;
// cout<< pPtLandEnd->dGetX() << ", " << pPtLandEnd->dGetY() << endl;
// cout << endl;

   // Check that pPtSeaEnd is not off the grid. Note that ptSeaEnd is NOT (necessarily) a cell centroid
   if (! bIsWithinGrid(static_cast<int>(dRound(dExtCRSXToGridX(pPtSeaEnd->dGetX()))), static_cast<int>(dRound(dExtCRSYToGridY(pPtSeaEnd->dGetY())))))
   {
//      LogStream << WARN << "timestep " << m_ulTimestep << ": profile endpoint is outside grid for coastline " << nCoast << ",  from coastline point " << nStartCoastPoint << "), ignored" << endl;
      return RTN_ERR_OFFGRID_ENDPOINT;
   }

   // Check that pPtLandEnd is not off the grid. Note that ptLandEnd is NOT (necessarily) a cell centroid
   if (! bIsWithinGrid(static_cast<int>(dRound(dExtCRSXToGridX(pPtLandEnd->dGetX()))), static_cast<int>(dRound(dExtCRSYToGridY(pPtLandEnd->dGetY())))))
   {
//      LogStream << WARN << "timestep " << m_ulTimestep << ": profile endpoint is outside grid for coastline " << nCoast << ",  from coastline point " << nStartCoastPoint << "), ignored" << endl;
      return RTN_ERR_OFFGRID_ENDPOINT;
   }
   return RTN_OK;
}

/*==============================================================================================================================

 Choose which end point to use for the coastline-normal profile at the LAND side

===============================================================================================================================*/
C2DPoint CDelineation::PtChooseLandEndPoint(int const nHand, C2DPoint* const PtBefore, C2DPoint* const PtAfter, double const dXEnd1, double const dYEnd1, double const dXEnd2, double const dYEnd2)
{
   C2DPoint PtChosen;

   if (nHand == LEFT_HANDED) // sea is to the left, so land is to the right of the linking line
   {
      // The land is to the right of the linking line. So which way is the linking line oriented? First check the N-S component
      if (PtAfter->dGetY() > PtBefore->dGetY())
      {
         // We are going N to S and the land is to the right, so the normal endpoint is to the W
         if (dXEnd1 < dXEnd2)
         {
            PtChosen.SetX(dXEnd1);
            PtChosen.SetY(dYEnd1);
         }
         else
         {
            PtChosen.SetX(dXEnd2);
            PtChosen.SetY(dYEnd2);
         }
      }
     else if (PtAfter->dGetY() < PtBefore->dGetY())
      {
          // We are going S to N and the land is to the right, so the normal endpoint is to the E
         if (dXEnd1 > dXEnd2)
         {
            PtChosen.SetX(dXEnd1);
            PtChosen.SetY(dYEnd1);
         }
         else
         {
            PtChosen.SetX(dXEnd2);
            PtChosen.SetY(dYEnd2);
         }
      }
      else
      {
        // No N-S component i.e. the linking line is exactly W-E. So check the W-E component
         if (PtAfter->dGetX() > PtBefore->dGetX())
         {
            // We are going W to E and the land is to the right, so the normal endpoint is to the S (note that the origin of the grid is to the top left)
            if (dYEnd1 > dYEnd2)
            {
               PtChosen.SetX(dXEnd1);
               PtChosen.SetY(dYEnd1);
            }
            else
            {
               PtChosen.SetX(dXEnd2);
               PtChosen.SetY(dYEnd2);
            }
         }
         else     // Do not check for (PtAfter->dGetX() == PtBefore->dGetX()), since this would mean the two points are co-incident
         {
            // We are going E to W and the land is to the right, so the normal endpoint is to the N (note that the origin of the grid is to the top left)
            if (dYEnd1 < dYEnd2)
            {
               PtChosen.SetX(dXEnd1);
               PtChosen.SetY(dYEnd1);
            }
            else
            {
               PtChosen.SetX(dXEnd2);
               PtChosen.SetY(dYEnd2);
            }
         }
      }
   }
   else
   {
      // The sea is to the right (land is to the left) of the linking line. So which way is the linking line oriented? First check the N-S component
      if (PtAfter->dGetY() > PtBefore->dGetY())
      {
         // We are going N to S and the land is to the left, so the normal endpoint is to the E
         if (dXEnd1 > dXEnd2)
         {
            PtChosen.SetX(dXEnd1);
            PtChosen.SetY(dYEnd1);
         }
         else
         {
            PtChosen.SetX(dXEnd2);
            PtChosen.SetY(dYEnd2);
         }
      }
     else if (PtAfter->dGetY() < PtBefore->dGetY())
      {
          // We are going S to N and the land is to the left, so the normal endpoint is to the W
         if (dXEnd1 < dXEnd2)
         {
            PtChosen.SetX(dXEnd1);
            PtChosen.SetY(dYEnd1);
         }
         else
         {
            PtChosen.SetX(dXEnd2);
            PtChosen.SetY(dYEnd2);
         }
      }
      else
      {
         // No N-S component i.e. the linking line is exactly W-E. So check the W-E component
         if (PtAfter->dGetX() > PtBefore->dGetX())
         {
            // We are going W to E and the land is to the left, so the normal endpoint is to the N (note that the origin of the grid is to the top left)
            if (dYEnd1 < dYEnd2)
            {
               PtChosen.SetX(dXEnd1);
               PtChosen.SetY(dYEnd1);
            }
            else
            {
               PtChosen.SetX(dXEnd2);
               PtChosen.SetY(dYEnd2);
            }
         }
         else     // Do not check for (PtAfter->dGetX() == PtBefore->dGetX()), since this would mean the two points are co-incident
         {
            // We are going E to W and the land is to the left, so the normal endpoint is to the S (note that the origin of the grid is to the top left)
            if (dYEnd1 > dYEnd2)
            {
               PtChosen.SetX(dXEnd1);
               PtChosen.SetY(dYEnd1);
            }
            else
            {
               PtChosen.SetX(dXEnd2);
               PtChosen.SetY(dYEnd2);
            }
         }
      }
   }

   return PtChosen;
}

/*==============================================================================================================================

 Choose which end point to use for the coastline-normal profile at the SEA side

===============================================================================================================================*/
C2DPoint CDelineation::PtChooseSeaEndPoint(int const nHand, C2DPoint* const PtBefore, C2DPoint* const PtAfter, double const dXEnd1, double const dYEnd1, double const dXEnd2, double const dYEnd2)
{
   C2DPoint PtChosen;

   if (nHand == RIGHT_HANDED)
   {
      // The sea is to the right of the linking line. So which way is the linking line oriented? First check the N-S component
      if (PtAfter->dGetY() > PtBefore->dGetY())
      {
         // We are going N to S and the sea is to the right, so the normal endpoint is to the W
         if (dXEnd1 < dXEnd2)
         {
            PtChosen.SetX(dXEnd1);
            PtChosen.SetY(dYEnd1);
         }
         else
         {
            PtChosen.SetX(dXEnd2);
            PtChosen.SetY(dYEnd2);
         }
      }
      //else if (bFPIsEqual(PtAfter->dGetY(), PtBefore->dGetY(), TOLERANCE))
      else if (PtAfter->dGetY() < PtBefore->dGetY())
      {
          // We are going S to N and the sea is to the right, so the normal endpoint is to the E
         if (dXEnd1 > dXEnd2)
         {
            PtChosen.SetX(dXEnd1);
            PtChosen.SetY(dYEnd1);
         }
         else
         {
            PtChosen.SetX(dXEnd2);
            PtChosen.SetY(dYEnd2);
         }
      }
      else
      {
        // No N-S component i.e. the linking line is exactly W-E. So check the W-E component
         if (PtAfter->dGetX() > PtBefore->dGetX())
         {
            // We are going W to E and the sea is to the right, so the normal endpoint is to the S (note that the origin of the grid is to the top left)
            if (dYEnd1 > dYEnd2)
            {
               PtChosen.SetX(dXEnd1);
               PtChosen.SetY(dYEnd1);
            }
            else
            {
               PtChosen.SetX(dXEnd2);
               PtChosen.SetY(dYEnd2);
            }
         }
         else     // Do not check for (PtAfter->dGetX() == PtBefore->dGetX()), since this would mean the two points are co-incident
         {
            // We are going E to W and the sea is to the right, so the normal endpoint is to the N (note that the origin of the grid is to the top left)
            if (dYEnd1 < dYEnd2)
            {
               PtChosen.SetX(dXEnd1);
               PtChosen.SetY(dYEnd1);
            }
            else
            {
               PtChosen.SetX(dXEnd2);
               PtChosen.SetY(dYEnd2);
            }
         }
      }
   }
   else
   {
      // The sea is to the left of the linking line. So which way is the linking line oriented? First check the N-S component
      if (PtAfter->dGetY() > PtBefore->dGetY())
      {
         // We are going N to S and the sea is to the left, so the normal endpoint is to the E
         if (dXEnd1 > dXEnd2)
         {
            PtChosen.SetX(dXEnd1);
            PtChosen.SetY(dYEnd1);
         }
         else
         {
            PtChosen.SetX(dXEnd2);
            PtChosen.SetY(dYEnd2);
         }
      }
      //else if (bFPIsEqual(PtAfter->dGetY(), PtBefore->dGetY(), TOLERANCE))
      else if (PtAfter->dGetY() < PtBefore->dGetY())
      {
          // We are going S to N and the sea is to the left, so the normal endpoint is to the W
         if (dXEnd1 < dXEnd2)
         {
            PtChosen.SetX(dXEnd1);
            PtChosen.SetY(dYEnd1);
         }
         else
         {
            PtChosen.SetX(dXEnd2);
            PtChosen.SetY(dYEnd2);
         }
      }
      else
      {
         // No N-S component i.e. the linking line is exactly W-E. So check the W-E component
         if (PtAfter->dGetX() > PtBefore->dGetX())
         {
            // We are going W to E and the sea is to the left, so the normal endpoint is to the N (note that the origin of the grid is to the top left)
            if (dYEnd1 < dYEnd2)
            {
               PtChosen.SetX(dXEnd1);
               PtChosen.SetY(dYEnd1);
            }
            else
            {
               PtChosen.SetX(dXEnd2);
               PtChosen.SetY(dYEnd2);
            }
         }
         else     // Do not check for (PtAfter->dGetX() == PtBefore->dGetX()), since this would mean the two points are co-incident
         {
            // We are going E to W and the sea is to the left, so the normal endpoint is to the S (note that the origin of the grid is to the top left)
            if (dYEnd1 > dYEnd2)
            {
               PtChosen.SetX(dXEnd1);
               PtChosen.SetY(dYEnd1);
            }
            else
            {
               PtChosen.SetX(dXEnd2);
               PtChosen.SetY(dYEnd2);
            }
         }
      }
   }

   return PtChosen;
}


/*==============================================================================================================================

 Checks all coastline-normal profiles for intersection, and modifies those that intersect

===============================================================================================================================*/
int CDelineation::nModifyAllIntersectingProfiles(void)
{
   // Do once for every coastline object
   int nCoastLines = m_VCoast.size();
   for (int nCoast = 0; nCoast < nCoastLines; nCoast++)
   {
      int nNumProfiles = m_VCoast[nCoast].nGetNumProfiles();

      // Go along the coast, looking at profiles which are increasingly distant from the first profile
      int nMaxDist = nNumProfiles / 2;       // Arbitrary
      for (int nDist = 1; nDist < nMaxDist; nDist++)
      {
         // Do once for every profile
         for (int nFirst = 0; nFirst < nNumProfiles; nFirst++)
         {
            // In coastline curvature sequence
            int nFirstProfile = m_VCoast[nCoast].nGetProfileAtAlongCoastlinePosition(nFirst);
            if (nFirstProfile < 0)
               // Not found
               return RTN_ERR_BAD_INDEX;

//            LogStream << m_ulTimestep << ": nFirst = " << nFirst << " nFirstProfile = " << nFirstProfile << endl;

            // Don't modify the start- or end-of coastline normals
            CProfile* pFirstProfile = m_VCoast[nCoast].pGetProfile(nFirstProfile);
            if ((pFirstProfile->bStartOfCoast()) || (pFirstProfile->bEndOfCoast()))
               continue;

            // Do this in alternate directions: first down-coast (in the direction of increasing coast point numbers) then up-coast
            for (int nDirection = DIRECTION_DOWNCOAST; nDirection <= DIRECTION_UPCOAST; nDirection++)
            {
               int nSecond;
               if (nDirection == DIRECTION_DOWNCOAST)
                  nSecond = nFirst + nDist;
               else
                  nSecond = nFirst - nDist;

               if ((nSecond < 0) || (nSecond > nNumProfiles-1))
                  // Make sure that we don't go beyond the ends of the along-coast index
                  continue;

               int nSecondProfile = m_VCoast[nCoast].nGetProfileAtAlongCoastlinePosition(nSecond);

//               LogStream << m_ulTimestep << ": " << (nDirection == DIRECTION_DOWNCOAST ? "down" : "up") << "-coast search, nFirst = " << nFirst << " nSecond = " << nSecond << " (profiles " << nFirstProfile << " and " << nSecondProfile << ")" << endl;

               // Only check these profiles for intersection if both are problem-free
               CProfile* pSecondProfile = m_VCoast[nCoast].pGetProfile(nSecondProfile);
               if (! (pFirstProfile->bProfileOK()) || (! pSecondProfile->bProfileOK()))
                  continue;

               // Only check these two profiles for intersection if they are are not co-incident in the final line segment of both profiles (i.e. the profiles have not already intersected)
               if ((pFirstProfile->bFindProfileInCoincidentProfilesOfLastLineSegment(nSecondProfile)) || (pSecondProfile->bFindProfileInCoincidentProfilesOfLastLineSegment(nFirstProfile)))
                  continue;

               // OK go for it
               int
                  nProf1LineSeg = 0,
                  nProf2LineSeg = 0;
               double
                  dIntersectX = 0,
                  dIntersectY = 0,
                  dAvgEndX = 0,
                  dAvgEndY = 0;

               if (bCheckForIntersection(pFirstProfile, pSecondProfile, nProf1LineSeg, nProf2LineSeg, dIntersectX, dIntersectY, dAvgEndX, dAvgEndY))
               {
                  // The profiles intersect. Is the point of intersection already present in the first profile (i.e. because there has already been an intersection at this point between the first profile and some other profile)?
                  int nPoint = -1;
                  if (pFirstProfile->bIsPointInProfile(dIntersectX, dIntersectY, nPoint))
                  {
//                      LogStream << m_ulTimestep << ": ^^^^ profiles " << nFirstProfile << " and " << nSecondProfile << " intersect, but point {" << dIntersectX << ", " << dIntersectY << "} is already present in profile " << nFirstProfile << " as point " << nPoint << endl;

                     // Truncate the second profile and merge it with the first profile
                     TruncateOneProfileRetainOtherProfile(nCoast, nSecondProfile, nFirstProfile, dIntersectX, dIntersectY, nProf2LineSeg, nProf1LineSeg, true);
                  }

                  // Is the point of intersection already present in the second profile?
                  else if (pSecondProfile->bIsPointInProfile(dIntersectX, dIntersectY, nPoint))
                  {
//                      LogStream << m_ulTimestep << ": ^^^^ profiles " << nFirstProfile << " and " << nSecondProfile << " intersect, but point {" << dIntersectX << ", " << dIntersectY << "} is already present in profile " << nSecondProfile << " as point " << nPoint << endl;

                     // Truncate the first profile and merge it with the second profile
                     TruncateOneProfileRetainOtherProfile(nCoast, nFirstProfile, nSecondProfile, dIntersectX, dIntersectY, nProf1LineSeg, nProf2LineSeg, true);
                  }

                  else
                  {
                     // The point of intersection is not already present in either profile, so get the number of line segments of each profile
                     int
                        nFirstProfileLineSegments = pFirstProfile->nGetNumLineSegments(),
                        nSecondProfileLineSegments = pSecondProfile->nGetNumLineSegments();

   //                   assert(nProf1LineSeg < nFirstProfileLineSegments);
   //                   assert(nProf2LineSeg < nSecondProfileLineSegments);

                     //  Next check whether the point of intersection is on the final line segment of both profiles
                     if ((nProf1LineSeg == (nFirstProfileLineSegments-1)) && (nProf2LineSeg == (nSecondProfileLineSegments-1)))
                     {
                        // Yes, the point of intersection is on the final line segment of both profiles, so merge the profiles seaward of the point of intersection
                        MergeProfilesAtFinalLineSegments(nCoast, nFirstProfile, nSecondProfile, nFirstProfileLineSegments, nSecondProfileLineSegments, dIntersectX, dIntersectY, dAvgEndX, dAvgEndY);

   //                     LogStream << m_ulTimestep << ": " << ((nDirection == DIRECTION_DOWNCOAST) ? "down" : "up") << "-coast search, end-segment intersection between profiles {" << nFirstProfile << "} and {" << nSecondProfile << "} at [" << dIntersectX << ", " << dIntersectY << "] in line segment [" << nProf1LineSeg << "] of " << nFirstProfileLineSegments << ", and line segment [" << nProf2LineSeg << "] of " << nSecondProfileLineSegments << ", respectively" << endl;
                     }
                     else
                     {
                        // The profiles intersect, but the point of intersection is not on the final line segment of both profiles. One of the profiles will be truncated, the other profile will be retained
   //                     LogStream << m_ulTimestep << ": " << ((nDirection == DIRECTION_DOWNCOAST) ? "down" : "up") << "-coast search, intersection (NOT both end segments) between profiles {" << nFirstProfile << "} and {" << nSecondProfile << "} at [" << dIntersectX << ", " << dIntersectY << "] in line segment [" << nProf1LineSeg << "] of " << nFirstProfileLineSegments << ", and line segment [" << nProf2LineSeg << "] of " << nSecondProfileLineSegments << ", respectively" << endl;

                        // Decide which profile to truncate, and which to retain
                        if (nFirstProfileLineSegments > nSecondProfileLineSegments)
                           // Truncate the second profile, since it has a smaller number of line segments
                           TruncateOneProfileRetainOtherProfile(nCoast, nSecondProfile, nFirstProfile, dIntersectX, dIntersectY, nProf2LineSeg, nProf1LineSeg, false);

                        else if (nFirstProfileLineSegments < nSecondProfileLineSegments)
                           // Truncate the first profile, since it has a smaller number of line segments
                           TruncateOneProfileRetainOtherProfile(nCoast, nFirstProfile, nSecondProfile, dIntersectX, dIntersectY, nProf1LineSeg, nProf2LineSeg, false);

                        else
                        {
                           // Both profiles have the same number of line segments, so choose randomnly
                           if (dGetRand0d1() >= 0.5)
                              TruncateOneProfileRetainOtherProfile(nCoast, nFirstProfile, nSecondProfile, dIntersectX, dIntersectY, nProf1LineSeg, nProf2LineSeg, false);
                           else
                              TruncateOneProfileRetainOtherProfile(nCoast, nSecondProfile, nFirstProfile, dIntersectX, dIntersectY, nProf2LineSeg, nProf1LineSeg, false);
                        }
                     }
                  }

//                   int
//                      nProfile1NumSegments = pFirstProfile->nGetNumLineSegments(),
//                      nProfile2NumSegments = pSecondProfile->nGetNumLineSegments(),
//                      nProfile1Size = pFirstProfile->nGetProfileSize(),
//                      nProfile2Size = pSecondProfile->nGetProfileSize();

//                   assert(pFirstProfile->nGetNumLineSegments() > 0);
//                   assert(pSecondProfile->nGetNumLineSegments() > 0);
//                   assert(nProfile1Size == nProfile1NumSegments+1);
//                   assert(nProfile2Size == nProfile2NumSegments+1);
               }
            }
         }
      }
   }

   return RTN_OK;
}


/*==============================================================================================================================

 Checks all line segments of a pair of coastline-normal profiles for intersection. If the lines intersect, returns true with numbers of the line segments at which intersection occurs in nProfile1LineSegment and nProfile1LineSegment, the intersection point in dXIntersect and dYIntersect, and the 'average' seaward endpoint of the two intersecting profiles at dXAvgEnd and dYAvgEnd

===============================================================================================================================*/
bool CDelineation::bCheckForIntersection(CProfile* const pVProfile1, CProfile* const pVProfile2, int& nProfile1LineSegment, int& nProfile2LineSegment, double& dXIntersect, double& dYIntersect, double& dXAvgEnd, double& dYAvgEnd)
{
   // For both profiles, look at all line segments
   int
      nProfile1NumSegments = pVProfile1->nGetNumLineSegments(),
      nProfile2NumSegments = pVProfile2->nGetNumLineSegments();
//       nProfile1Size = pVProfile1->nGetProfileSize(),
//       nProfile2Size = pVProfile2->nGetProfileSize();

//    assert(nProfile1Size == nProfile1NumSegments+1);
//    assert(nProfile2Size == nProfile2NumSegments+1);

   for (int i = 0; i < nProfile1NumSegments; i++)
   {
      for (int j = 0; j < nProfile2NumSegments; j++)
      {
         // In external coordinates
         double
            dX1 = pVProfile1->pPtVGetPoints()->at(i).dGetX(),
            dY1 = pVProfile1->pPtVGetPoints()->at(i).dGetY(),
            dX2 = pVProfile1->pPtVGetPoints()->at(i+1).dGetX(),
            dY2 = pVProfile1->pPtVGetPoints()->at(i+1).dGetY();

         double
            dX3 = pVProfile2->pPtVGetPoints()->at(j).dGetX(),
            dY3 = pVProfile2->pPtVGetPoints()->at(j).dGetY(),
            dX4 = pVProfile2->pPtVGetPoints()->at(j+1).dGetX(),
            dY4 = pVProfile2->pPtVGetPoints()->at(j+1).dGetY();

         // Uses Cramer's Rule to solve the equations. Modified from code at http://stackoverflow.com/questions/563198/how-do-you-detect-where-two-line-segments-intersect (in turn based on Andre LeMothe's "Tricks of the Windows Game Programming Gurus")
         double
            dDiffX1 = dX2 - dX1,
            dDiffY1 = dY2 - dY1,
            dDiffX2 = dX4 - dX3,
            dDiffY2 = dY4 - dY3;

         double
            dS = -999,
            dT = -999,
            dTmp = 0;

         dTmp = -dDiffX2 * dDiffY1 + dDiffX1 * dDiffY2;
         if (dTmp != 0)
            dS = (-dDiffY1 * (dX1 - dX3) + dDiffX1 * (dY1 - dY3)) / dTmp;

         dTmp = -dDiffX2 * dDiffY1 + dDiffX1 * dDiffY2;
         if (dTmp != 0)
            dT = (dDiffX2 * (dY1 - dY3) - dDiffY2 * (dX1 - dX3)) / dTmp;

         if (dS >= 0 && dS <= 1 && dT >= 0 && dT <= 1)
         {
            // Collision detected, calculate intersection co-ords
            dXIntersect = dX1 + (dT * dDiffX1);
            dYIntersect = dY1 + (dT * dDiffY1);

            // And calc the average end-point co-ords
            dXAvgEnd = (dX2 + dX4) / 2;
            dYAvgEnd = (dY2 + dY4) / 2;

            // Get the line segments at which intersection occurred
            nProfile1LineSegment = i;
            nProfile2LineSegment = j;

      //      LogStream << "\t" << "INTERSECTION dX2 = " << dX2 << " dX4 = " << dX4 << " dY2 = " << dY2 << " dY4 = " << dY4 << endl;
            return true;
         }
      }
   }

    // No collision
    return false;
}


/*==============================================================================================================================

 Puts the coastline-normal profiles onto the raster grid, i.e. rasterizes multi-line vector objects onto the raster grid. Note that this doesn't work if the vector has already been interpolated to fit on the grid i.e. if distances between vector points are just one cell apart

===============================================================================================================================*/
int CDelineation::nPutAllProfilesOntoGrid(void)
{
   int nValidProfiles = 0;

   // Do once for every coastline
   for (int nCoast = 0; nCoast < static_cast<int>(m_VCoast.size()); nCoast++)
   {
      // How many profiles on this coast?
      int nProfiles = m_VCoast[nCoast].nGetNumProfiles();
      if (nProfiles == 0)
      {
         // This can happen if the coastline is very short, so just give a warning and carry on with the next coastline
         LogStream << WARN << m_ulTimestep << ": coastline " << nCoast << " has no profiles" << endl;
         continue;
      }

      // Now do this loop for every profile in the list
      for (int nProfile = 0; nProfile < nProfiles; nProfile++)
      {
         CProfile* const pProfile = m_VCoast[nCoast].pGetProfile(nProfile);

         // If this profile has a problem, then forget about it. Still do start- and end-of-coast profiles however
         if (! pProfile->bOKIncStartAndEndOfCoast())
            continue;

         int nPoints = pProfile->nGetProfileSize();
         if (nPoints < 2)
         {
            // Need at least two points in the profile, so this profile is invalid: mark it
            pProfile->SetTooShort(true);
            continue;
         }

         // OK, go for it: set up temporary vectors to hold the x-y coords (in grid CRS) of the cells which we will mark
         vector<C2DIPoint> VCellsToMark;
         vector<bool> bVShared;
         bool
            bTooShort          = false,
            bTruncated         = false,
            bHitCoast          = false,
            bHitAnotherProfile = false;

         RasterizeProfile(nCoast, nProfile, &VCellsToMark, &bVShared, bTooShort, bTruncated, bHitCoast, bHitAnotherProfile);

         //if ((! bTruncated) && (! bTooShort) && (! bHitCoast)  && (! bHitAnotherProfile))
	 //if ( (! bTooShort) && (! bHitAnotherProfile))
	 if ( (! bTooShort))
         {
            // This profile is fine
            nValidProfiles++;

            for (unsigned int k = 0; k < VCellsToMark.size(); k++)
            {
               // So mark each cell in the raster grid
               m_pRasterGrid->pGetCell(VCellsToMark[k].nGetX(), VCellsToMark[k].nGetY())->SetNormalProfile(nProfile);

               // Store the raster-grid coordinates in the profile object
               pProfile->AppendCellInProfile(VCellsToMark[k].nGetX(), VCellsToMark[k].nGetY());

               // And also store the external coordinates in the profile object
               pProfile->AppendCellInProfileExtCRS(dGridCentroidXToExtCRSX(VCellsToMark[k].nGetX()), dGridCentroidYToExtCRSY(VCellsToMark[k].nGetY()));

               // Mark the shared (i.e. multi-line) parts of the profile (if any)
//                if (bVShared[k])
//                {
//                   pProfile->AppendPointShared(true);
// //                     LogStream << m_ulTimestep << ": profile " << j << " point " << k << " marked as shared" << endl;
//                }
//                else
//                {
//                   pProfile->AppendPointShared(false);
// //                     LogStream << m_ulTimestep << ": profile " << nProfile << " point " << k << " marked as NOT shared" << endl;
//                }
            }
         }
      }
   }

   if (nValidProfiles == 0)
   {
      // Problem! No valid profiles. However, carry on
      cerr << WARN << m_ulTimestep << ": no valid profiles" << endl;
   }

   return RTN_OK;
}


/*==============================================================================================================================

 Given a pointer to a coastline-normal profile, returns an output vector of cells which are 'under' every line segment of the profile. If there is a problem with the profile (e.g. a rasterized cell is dry land or coast, or the profile has to be truncated) then we pass this back as an error code

===============================================================================================================================*/
void CDelineation::RasterizeProfile(int const nCoast, int const nProfile, vector<C2DIPoint>* pVIPointsOut, vector<bool>* pbVShared, bool& bTooShort, bool& bTruncated, bool& bHitCoast, bool& bHitAnotherProfile)
{
   CProfile* const pProfile = m_VCoast[nCoast].pGetProfile(nProfile);

   pVIPointsOut->clear();
   int
      nSeg = 0,
      nSegments = pProfile->nGetNumLineSegments(),
      nProfiles = m_VCoast[nCoast].nGetNumProfiles();    // TODO this is a bodge, needed if we hit a profile which belongs to a different coast object

   for (nSeg = 0; nSeg < nSegments; nSeg++)
   {
      // Do once for every line segment
      vector<C2DPoint> PtVSegment;
      PtVSegment.push_back(*pProfile->pPtGetPointInProfile(nSeg));
      PtVSegment.push_back(*pProfile->pPtGetPointInProfile(nSeg+1));     // This is OK

      bool bShared = false;
      if (pProfile->nGetNumCoincidentProfilesInLineSegment(nSeg) > 1)
         bShared = true;

      // The start point of the normal, must convert from the external CRS to grid CRS. If this is the first line segment of the profile, then the start point is the centroid of a coastline cell
      double
         dXStart = dExtCRSXToGridX(PtVSegment[0].dGetX()),
         dYStart = dExtCRSYToGridY(PtVSegment[0].dGetY());

      // The end point of the normal, again convert from the external CRS to grid CRS. Note too that it could be off the grid
      double
         dXEnd = dExtCRSXToGridX(PtVSegment[1].dGetX()),
         dYEnd = dExtCRSYToGridY(PtVSegment[1].dGetY());

      // Interpolate between cells by a simple DDA line algorithm, see http://en.wikipedia.org/wiki/Digital_differential_analyzer_(graphics_algorithm) Note that Bresenham's algorithm gave occasional gaps
      double
         dXInc = dXEnd - dXStart,
         dYInc = dYEnd - dYStart,
         dLength = tMax(tAbs(dXInc), tAbs(dYInc));

      dXInc /= dLength;
      dYInc /= dLength;

      double
         dX = dXStart,
         dY = dYStart;

      // Process each interpolated point
      for (int m = 0; m <= static_cast<int>(dRound(dLength)); m++)
      {
         int
            nX = static_cast<int>(dX),
            nY = static_cast<int>(dY);

         // Do some checking of this interpolated point, but only if this is not a grid-edge profile (these profiles are always valid)
         if ((! pProfile->bStartOfCoast()) && (! pProfile->bEndOfCoast()))
         {
            // Is the interpolated point within the raster grid?
            if (! bIsWithinGrid(nX, nY))
            {
               // It isn't, so mark the too-short profile and quit
               bTruncated = true;
               pProfile->SetTruncated(true);

               LogStream << m_ulTimestep << ": profile " << nProfile << " TRUNCATED at [" << nX << "][" << nY << "] = {" << dGridCentroidXToExtCRSX(nX) << ", " << dGridCentroidYToExtCRSY(nY) << "}" << endl;

               break;
            }

            // If this is the first line segment of the profile, then once we are clear of the coastline (say, when m > 1), check if this profile hits land at this interpolated point
            // NOTE Get problems here since if the coastline vector has been heavily smoothed, this can result is 'false positives' profiles marked as invalid which are not actually invalid, because the profile hits land when m = 0 or m = 1. This results in some cells being flagged as profile cells which are actually inland
            if ((nSeg == 0) && (m > 1))
            {
               // Two diagonal(ish) raster lines can cross each other without any intersection, so must also test an adjacent cell for intersection (does not matter which adjacent cell)
               if (m_pRasterGrid->pGetCell(nX, nY)->bIsCoastline() || (bIsWithinGrid(nX, nY+1) && m_pRasterGrid->pGetCell(nX, nY+1)->bIsCoastline()))
               {
                  // We've hit a coastline so set a switch and mark the profile and truncate 
		  bHitCoast = true;
                  pProfile->SetHitCoast(true);
		  
		  LogStream << m_ulTimestep << ": profile " << nProfile << " HIT COAST at [" << nX << "][" << nY << "] = {" << dGridCentroidXToExtCRSX(nX) << ", " << dGridCentroidYToExtCRSY(nY) << "}" << endl;
		  
		  bTruncated = true;
		  pProfile->SetTruncated(true);
		  break;
               }

//                if (! m_pRasterGrid->pGetCell(nX, nY)->bIsInContiguousSea())
//                {
//                   // We've hit dry land so set a switch and mark the profile, however keep going
//                   bHitLand = true;
//                   pProfile->SetHitLand(true);
//
//                   LogStream << m_ulTimestep << ": profile " << nProfile << " HIT LAND at [" << nX << "][" << nY << "] = {" << dGridCentroidXToExtCRSX(nX) << ", " << dGridCentroidYToExtCRSY(nY) << "}, elevation = " << m_pRasterGrid->pGetCell(nX, nY)->dGetSedimentTopElev() << ", SWL = " << m_dStillWaterLevel << endl;
//
//                   LogStream << "On [" << nX << "][" << nY << "] = {" << dGridCentroidXToExtCRSX(nX) << ", " << dGridCentroidYToExtCRSY(nY) << "}, sea depth = " << m_pRasterGrid->pGetCell(nX, nY)->dGetSeaDepth() << ", bIsInContiguousSea = " << (m_pRasterGrid->pGetCell(nX, nY)->bIsInContiguousSea() ? "true" : "false") << ", landform = " << (m_pRasterGrid->pGetCell(nX, nY)->bIsInContiguousSea() ? "sea" : "not sea") << endl;
//
//                   LogStream << "On [" << nX << "][" << nY << "] = {" << dGridCentroidXToExtCRSX(nX) << ", " << dGridCentroidYToExtCRSY(nY) << "}, elevation of consolidated sediment = " << m_pRasterGrid->pGetCell(nX, nY)->dGetConsSedTopForLayer(m_pRasterGrid->pGetCell(nX, nY)->nGetTopNonZeroLayer()) << ", total cliff collapse = " << m_pRasterGrid->pGetCell(nX, nY)->dGetTotCliffCollapse() << ", total beach deposition = " << m_pRasterGrid->pGetCell(nX, nY)->dGetTotBeachDeposition() << endl;
//                }
            }

            // Check to see if we hit another profile which is not a coincident normal to this normal
            static int nLastProfileChecked = -1;
            if ((nProfile != nLastProfileChecked) && m_pRasterGrid->pGetCell(nX, nY)->bIsNormalProfile())
            {
               // For the first time for this profile, we've hit a raster cell which is already marked as 'under' a normal profile. Get the number of the profile which marked this cell
               int nHitProfile = m_pRasterGrid->pGetCell(nX, nY)->nGetNormalProfile();

               // TODO Bodge in case we hit a profile which belongs to a different coast
               if (nHitProfile > nProfiles-1)
               {
                  bHitAnotherProfile = true;
                  pProfile->SetHitAnotherProfile(true);

                  LogStream << m_ulTimestep << ": profile " << nProfile << " hit another profile A (" << nHitProfile << ") at [" << nX << "][" << nY << "] = {" << dGridCentroidXToExtCRSX(nX) << ", " << dGridCentroidYToExtCRSY(nY) << "}" << endl;
               }
               else
               {
		  // Only set the flag if this isn't a coincident normal to one or other of the profiles
                  if ((! pProfile->bFindProfileInCoincidentProfiles(nHitProfile)) && (! m_VCoast[nCoast].pGetProfile(nHitProfile)->bFindProfileInCoincidentProfiles(nProfile)))
                  {
                     bHitAnotherProfile = true;
                     pProfile->SetHitAnotherProfile(true);

                     LogStream << m_ulTimestep << ": profile " << nProfile << " hit another profile B (" << nHitProfile << ") at [" << nX << "][" << nY << "] = {" << dGridCentroidXToExtCRSX(nX) << ", " << dGridCentroidYToExtCRSY(nY) << "}" << endl;
                  }

                  nLastProfileChecked = nProfile;
               }
            }
         }

         // Append this point to the output vector
         pVIPointsOut->push_back(C2DIPoint(nX, nY));         // Is in raster-grid coordinates
         pbVShared->push_back(bShared);

         // And increment for next time
         dX += dXInc;
         dY += dYInc;
      }

      if (bTruncated)
         break;
   }

   if (bTruncated)
   {
      if (nSeg < (nSegments-1))
         // We are truncating the profile, so remove any line segments after this one
         pProfile->TruncateLineSegments(nSeg);

      // Shorten the vector input
      int
         nOutSize = pVIPointsOut->size(),
         nLastX = pVIPointsOut->at(nOutSize-1).nGetX(),
         nLastY = pVIPointsOut->at(nOutSize-1).nGetY();

      pProfile->pPtGetPointInProfile(nSeg+1)->SetX(dGridCentroidXToExtCRSX(nLastX));
      pProfile->pPtGetPointInProfile(nSeg+1)->SetY(dGridCentroidYToExtCRSY(nLastY));

   }

   if (pVIPointsOut->size() < 3)
   {
      // For shore platform coastline-normal profiles, we cannot have very short normal profiles with less than 3 cells, since we cannot calculate along-profile slope properly for such short profiles
      bTooShort = true;
      pProfile->SetTooShort(true);

      LogStream << m_ulTimestep << ": profile " << nProfile << " is TOO SHORT" << endl;
   }
}


/*==============================================================================================================================

 Merges two profiles which intersect at their final (most seaward) line segments, seaward of their point of intersection

===============================================================================================================================*/
void CDelineation::MergeProfilesAtFinalLineSegments(int const nCoast, int const nFirstProfile, int const nSecondProfile, int const nFirstProfileLineSegments, int const nSecondProfileLineSegments, double const dIntersectX, double const dIntersectY, double const dAvgEndX, double const dAvgEndY)
{
   // The point of intersection is on the final (most seaward) line segment of both profiles. Put together a vector of coincident profile numbers (with no duplicates) for both profiles
   int nCombinedLastSeg = 0;
   vector<pair<int, int> >
      prVCombinedProfilesCoincidentProfilesLastSeg;
   CProfile* pFirstProfile = m_VCoast[nCoast].pGetProfile(nFirstProfile);
   CProfile* pSecondProfile = m_VCoast[nCoast].pGetProfile(nSecondProfile);

   for (unsigned int n = 0; n < pFirstProfile->pprVGetCoincidentProfilesForLineSegment(nFirstProfileLineSegments-1)->size(); n++)
   {
      pair<int, int> prTmp;
      prTmp.first = pFirstProfile->pprVGetCoincidentProfilesForLineSegment(nFirstProfileLineSegments-1)->at(n).first;
      prTmp.second = pFirstProfile->pprVGetCoincidentProfilesForLineSegment(nFirstProfileLineSegments-1)->at(n).second;

      bool bFound = false;
      for (unsigned int m = 0; m < prVCombinedProfilesCoincidentProfilesLastSeg.size(); m++)
      {
         if (prVCombinedProfilesCoincidentProfilesLastSeg[m].first == prTmp.first)
         {
            bFound = true;
            break;
         }
      }

      if (! bFound)
      {
         prVCombinedProfilesCoincidentProfilesLastSeg.push_back(prTmp);
         nCombinedLastSeg++;
      }
   }

   for (unsigned int n = 0; n < pSecondProfile->pprVGetCoincidentProfilesForLineSegment(nSecondProfileLineSegments-1)->size(); n++)
   {
      pair<int, int> prTmp;
      prTmp.first = pSecondProfile->pprVGetCoincidentProfilesForLineSegment(nSecondProfileLineSegments-1)->at(n).first;
      prTmp.second = pSecondProfile->pprVGetCoincidentProfilesForLineSegment(nSecondProfileLineSegments-1)->at(n).second;

      bool bFound = false;
      for (unsigned int m = 0; m < prVCombinedProfilesCoincidentProfilesLastSeg.size(); m++)
      {
         if (prVCombinedProfilesCoincidentProfilesLastSeg[m].first == prTmp.first)
         {
            bFound = true;
            break;
         }
      }

      if (! bFound)
      {
         prVCombinedProfilesCoincidentProfilesLastSeg.push_back(prTmp);
         nCombinedLastSeg++;
      }
   }

   // Increment the number of each line segment
   for (int m = 0; m < nCombinedLastSeg; m++)
      prVCombinedProfilesCoincidentProfilesLastSeg[m].second++;

   vector<pair<int, int> >
      prVFirstProfileCoincidentProfilesLastSeg = *pFirstProfile->pprVGetCoincidentProfilesForLineSegment(nFirstProfileLineSegments-1),
      prVSecondProfileCoincidentProfilesLastSeg = *pSecondProfile->pprVGetCoincidentProfilesForLineSegment(nSecondProfileLineSegments-1);
   int
      nNumFirstProfileCoincidentProfilesLastSeg = prVFirstProfileCoincidentProfilesLastSeg.size(),
      nNumSecondProfileCoincidentProfilesLastSeg = prVSecondProfileCoincidentProfilesLastSeg.size();

//   LogStream << m_ulTimestep << ": END-SEGMENT INTERSECTION between profiles {" << nFirstProfile << "} and {" << nSecondProfile << "} at line segment " << nFirstProfileLineSegments-1 << "/" << nFirstProfileLineSegments-1 << ", and line segment " << nSecondProfileLineSegments-1 << "/" << nSecondProfileLineSegments-1 << ", respectively. Both truncated at [" << dIntersectX << ", " << dIntersectY << "] then profiles {" << nFirstProfile << "} and {" << nSecondProfile << "} extended to [" << dAvgEndX << ", " << dAvgEndY << "]" << endl;

   // Truncate the first profile, and all co-incident profiles, at the point of intersection
   for (int n = 0; n < nNumFirstProfileCoincidentProfilesLastSeg; n++)
   {
      int nThisProfile = prVFirstProfileCoincidentProfilesLastSeg[n].first;
      CProfile* pThisProfile = m_VCoast[nCoast].pGetProfile(nThisProfile);
      int nProfileLength = pThisProfile->nGetProfileSize();

      // NOTE: this is the final line segment of the first 'main' profile. We are assuming that it is also the final line segment of all co-incident profiles. This is fine, altho' each profile may well have a different number of line segments landwards i.e. the number of the line segment may be different for each co-incident profile
      pThisProfile->SetPointInProfile(nProfileLength-1, dIntersectX, dIntersectY);
   }

   // Truncate the second profile, and all co-incident profiles, at the point of intersection
   for (int n = 0; n < nNumSecondProfileCoincidentProfilesLastSeg; n++)
   {
      int nThisProfile = prVSecondProfileCoincidentProfilesLastSeg[n].first;
      CProfile* pThisProfile = m_VCoast[nCoast].pGetProfile(nThisProfile);
      int nProfileLength = pThisProfile->nGetProfileSize();

      // NOTE: this is the final line segment of the second 'main' profile. We are assuming that it is also the final line segment of all co-incident profiles. This is fine, altho' each profile may well have a different number of line segments landwards i.e. the number of the line segment may be different for each co-incident profile
      pThisProfile->SetPointInProfile(nProfileLength-1, dIntersectX, dIntersectY);
   }

   // Append a new straight line segment to the existing line segment(s) of the first profile, and to all co-incident profiles
   for (int nThisLineSeg = 0; nThisLineSeg < nNumFirstProfileCoincidentProfilesLastSeg; nThisLineSeg++)
   {
      int nThisProfile = prVFirstProfileCoincidentProfilesLastSeg[nThisLineSeg].first;
      CProfile* pThisProfile = m_VCoast[nCoast].pGetProfile(nThisProfile);

      // Update this profile
      pThisProfile->AppendPointInProfile(dAvgEndX, dAvgEndY);

      // Append details of the combined profiles
      pThisProfile->AppendLineSegment();
      for (int m = 0; m < nCombinedLastSeg; m++)
         pThisProfile->AppendCoincidentProfileToLineSegments(prVCombinedProfilesCoincidentProfilesLastSeg[m]);
   }

   // Append a new straight line segment to the existing line segment(s) of the second profile, and to all co-incident profiles
   for (int nThisLineSeg = 0; nThisLineSeg < nNumSecondProfileCoincidentProfilesLastSeg; nThisLineSeg++)
   {
      int nThisProfile = prVSecondProfileCoincidentProfilesLastSeg[nThisLineSeg].first;
      CProfile* pThisProfile = m_VCoast[nCoast].pGetProfile(nThisProfile);

      // Update this profile
      pThisProfile->AppendPointInProfile(dAvgEndX, dAvgEndY);

      // Append details of the combined profiles
      pThisProfile->AppendLineSegment();
      for (int m = 0; m < nCombinedLastSeg; m++)
         pThisProfile->AppendCoincidentProfileToLineSegments(prVCombinedProfilesCoincidentProfilesLastSeg[m]);
   }


   // START: FOR CHECKING PURPOSES ****************************************************************
//    int nFirstProfileLineSeg= pFirstProfile->nGetNumLineSegments();
//    int nSecondProfileLineSeg = pSecondProfile->nGetNumLineSegments();
//
//    LogStream << "\tProfile {" << nFirstProfile << "} now has " << nFirstProfileLineSeg << " line segments" << endl;
//    for (int m = 0; m < nFirstProfileLineSeg; m++)
//    {
//       vector<pair<int, int> > prVCoincidentProfiles = *pFirstProfile->pprVGetCoincidentProfilesForLineSegment(m);
//       LogStream << "\tCo-incident profiles and line segments for line segment " << m << " of profile {" << nFirstProfile << "} are {";
//       for (unsigned int nn = 0; nn < prVCoincidentProfiles.size(); nn++)
//          LogStream << " " << prVCoincidentProfiles[nn].first << "[" << prVCoincidentProfiles[nn].second << "] ";
//       LogStream << " }" << endl;
//    }
//    LogStream << "\tProfile {" << nSecondProfile << "} now has " << nSecondProfileLineSeg << " line segments" << endl;
//    for (int m = 0; m < nSecondProfileLineSeg; m++)
//    {
//       vector<pair<int, int> > prVCoincidentProfiles = *pSecondProfile->pprVGetCoincidentProfilesForLineSegment(m);
//       LogStream << "\tCo-incident profiles and line segments for line segment " << m << " of profile {" << nSecondProfile << "} are {";
//       for (unsigned int nn = 0; nn < prVCoincidentProfiles.size(); nn++)
//          LogStream << " " << prVCoincidentProfiles[nn].first << "[" << prVCoincidentProfiles[nn].second << "] ";
//       LogStream << " }" << endl;
//    }
   // END: FOR CHECKING PURPOSES ******************************************************************
}


/*==============================================================================================================================

 Truncates one intersecting profile at the point of intersection, and retains the other profile

===============================================================================================================================*/
void CDelineation::TruncateOneProfileRetainOtherProfile(int const nCoast, int const nProfileToTruncate, int const nProfileToRetain, double const dIntersectX, double const dIntersectY, int const nProfileToTruncateIntersectLineSeg, int const nProfileToRetainIntersectLineSeg, bool const bAlreadyPresent)
{
   // Insert the intersection point into the main retain-profile if it is not already in the profile, and do the same for all co-incident profiles of the main retain-profile. Also add details of the to-truncate profile (and all its coincident profiles) to every line segment of the main to-retain profile which is seaward of the point of intersection
   int nRet = nInsertPointIntoProfilesIfNeededThenUpdate(nCoast, nProfileToRetain, dIntersectX, dIntersectY, nProfileToRetainIntersectLineSeg, nProfileToTruncate, nProfileToTruncateIntersectLineSeg, bAlreadyPresent);
   if (nRet != RTN_OK)
   {
      LogStream << m_ulTimestep << ": error in nInsertPointIntoProfilesIfNeededThenUpdate()" << endl;
      return;
   }

   // Get all profile points of the main retain-profile seawards from the intersection point, and do the same for the corresponding line segments (including coincident profiles). This also includes details of the main to-truncate profile (and all its coincident profiles)
   CProfile* pProfileToRetain = m_VCoast[nCoast].pGetProfile(nProfileToRetain);
   vector<C2DPoint> PtVProfileLastPart;
   vector<vector<pair<int, int> > > prVLineSegLastPart;
   if (bAlreadyPresent)
   {
      PtVProfileLastPart = pProfileToRetain->PtVGetThisPointAndAllAfter(nProfileToRetainIntersectLineSeg);
      prVLineSegLastPart = pProfileToRetain->prVVGetAllLineSegAfter(nProfileToRetainIntersectLineSeg);
   }
   else
   {
      PtVProfileLastPart = pProfileToRetain->PtVGetThisPointAndAllAfter(nProfileToRetainIntersectLineSeg+1);
      prVLineSegLastPart = pProfileToRetain->prVVGetAllLineSegAfter(nProfileToRetainIntersectLineSeg+1);
   }

//    assert(PtVProfileLastPart.size() > 1);
//    assert(prVLineSegLastPart.size() > 0);

   // Truncate the truncate-profile at the point of intersection, and do the same for all its co-incident profiles. Then append the profile points of the main to-retain profile seaward from the intersection point, and do the same for the corresponding line segments (including coincident profiles)
   TruncateProfileAndAppendNew(nCoast, nProfileToTruncate, nProfileToTruncateIntersectLineSeg, &PtVProfileLastPart, &prVLineSegLastPart);

//    assert(m_VCoast[nCoast].pGetProfile(nProfileToTruncate)->nGetProfileSize() > 1);
//    assert(pProfileToRetain->nGetNumLineSegments() > 0);
//    assert(m_VCoast[nCoast].pGetProfile(nProfileToTruncate)->nGetNumLineSegments() > 0);
}


/*===============================================================================================================================

 Inserts an intersection point into the profile that is to be retained, if that point is not already present in the profile, then does the same for all co-incident profiles. Finally adds the numbers of the to-truncate profile (and all its coincident profiles) to the seaward line segments of the to-retain profile and all its coincident profiles

===============================================================================================================================*/
int CDelineation::nInsertPointIntoProfilesIfNeededThenUpdate(int const nCoast, int const nMainProfile, double const dIntersectX, double const dIntersectY, int const nMainProfileIntersectLineSeg, int const nProfileToTruncate, int const nProfileToTruncateIntersectLineSeg, bool const bAlreadyPresent)
{
   // START: FOR CHECKING PURPOSES ****************************************************************
   // Get the index numbers of all coincident profiles for the 'main' to-retain profile for the line segment in which intersection occurred
//    vector<pair<int, int> > prVRetainCoincidentProfilesCHECK1 = *m_VCoast[nCoast].pGetProfile(nMainProfile)->pprVGetCoincidentProfilesForLineSegment(nMainProfileIntersectLineSeg);
//    int nNumRetainCoincidentCHECK1 = prVRetainCoincidentProfilesCHECK1.size();
//    for (int nn = 0; nn < nNumRetainCoincidentCHECK1; nn++)
//    {
//       int nThisProfile = prVRetainCoincidentProfilesCHECK1[nn].first;
//       LogStream << "\tBEFORE nInsertPointIntoProfilesIfNeededThenUpdate(): " << (nThisProfile == nMainProfile ? "MAIN" : "COINCIDENT") << " to-retain profile {" << nThisProfile << "} has " << m_VCoast[nCoast].pGetProfile(nThisProfile)->nGetProfileSize() << " points ";
//       for (int nn = 0; nn < m_VCoast[nCoast].pGetProfile(nThisProfile)->nGetProfileSize(); nn++)
//          LogStream << "[" << m_VCoast[nCoast].pGetProfile(nThisProfile)->pPtGetPointInProfile(nn)->dGetX() << ", " << m_VCoast[nCoast].pGetProfile(nThisProfile)->pPtGetPointInProfile(nn)->dGetY() << "] ";
//       LogStream << "), and " << m_VCoast[nCoast].pGetProfile(nThisProfile)->nGetNumLineSegments() << " line segments, co-incident profiles and their line segments are ";
//       for (int mm = 0; mm < m_VCoast[nCoast].pGetProfile(nThisProfile)->nGetNumLineSegments(); mm++)
//       {
//          vector<pair<int, int> > prVTmp = *m_VCoast[nCoast].pGetProfile(nThisProfile)->pprVGetCoincidentProfilesForLineSegment(mm);
//          LogStream << "{ ";
//          for (int nn = 0; nn < m_VCoast[nCoast].pGetProfile(nThisProfile)->nGetNumCoincidentProfilesInLineSegment(mm); nn++)
//             LogStream << prVTmp[nn].first << "[" << prVTmp[nn].second << "] ";
//          LogStream << "} ";
//       }
//       LogStream << endl;
//
//       for (int nPoint = 0; nPoint < m_VCoast[nCoast].pGetProfile(nThisProfile)->nGetProfileSize()-1; nPoint++)
//       {
//          C2DPoint
//             Pt1 = *m_VCoast[nCoast].pGetProfile(nThisProfile)->pPtGetPointInProfile(nPoint),
//             Pt2 = *m_VCoast[nCoast].pGetProfile(nThisProfile)->pPtGetPointInProfile(nPoint+1);
//
//          if (Pt1 == Pt2)
//             LogStream << m_ulTimestep << ": IDENTICAL POINTS before changes, in profile {" << nThisProfile << "} points = " << nPoint << " and " << nPoint+1 << endl;
//       }
//    }
//    // Get the index numbers of all coincident profiles for the 'main' to-truncate profile for the line segment in which intersection occurred
//    vector<pair<int, int> > prVTruncateCoincidentProfilesCHECK1 = *m_VCoast[nCoast].pGetProfile(nProfileToTruncate)->pprVGetCoincidentProfilesForLineSegment(nProfileToTruncateIntersectLineSeg);
//    int nNumTruncateCoincidentCHECK1 = prVTruncateCoincidentProfilesCHECK1.size();
//    for (int nn = 0; nn < nNumTruncateCoincidentCHECK1; nn++)
//    {
//       int nThisProfile = prVTruncateCoincidentProfilesCHECK1[nn].first;
//       LogStream << "\tBEFORE nInsertPointIntoProfilesIfNeededThenUpdate(): " << (nThisProfile == nProfileToTruncate ? "MAIN" : "COINCIDENT") << " to-truncate profile {" << nThisProfile << "} has " << m_VCoast[nCoast].pGetProfile(nThisProfile)->nGetProfileSize() << " points ";
//       for (int nn = 0; nn < m_VCoast[nCoast].pGetProfile(nThisProfile)->nGetProfileSize(); nn++)
//          LogStream << "[" << m_VCoast[nCoast].pGetProfile(nThisProfile)->pPtGetPointInProfile(nn)->dGetX() << ", " << m_VCoast[nCoast].pGetProfile(nThisProfile)->pPtGetPointInProfile(nn)->dGetY() << "] ";
//       LogStream << "), and " << m_VCoast[nCoast].pGetProfile(nThisProfile)->nGetNumLineSegments() << " line segments, co-incident profiles and their line segments are ";
//       for (int mm = 0; mm < m_VCoast[nCoast].pGetProfile(nThisProfile)->nGetNumLineSegments(); mm++)
//       {
//          vector<pair<int, int> > prVTmp = *m_VCoast[nCoast].pGetProfile(nThisProfile)->pprVGetCoincidentProfilesForLineSegment(mm);
//          LogStream << "{ ";
//          for (int nn = 0; nn < m_VCoast[nCoast].pGetProfile(nThisProfile)->nGetNumCoincidentProfilesInLineSegment(mm); nn++)
//             LogStream << prVTmp[nn].first << "[" << prVTmp[nn].second << "] ";
//          LogStream << "} ";
//       }
//       LogStream << endl;
//
//       for (int nPoint = 0; nPoint < m_VCoast[nCoast].pGetProfile(nThisProfile)->nGetProfileSize()-1; nPoint++)
//       {
//          C2DPoint
//             Pt1 = *m_VCoast[nCoast].pGetProfile(nThisProfile)->pPtGetPointInProfile(nPoint),
//             Pt2 = *m_VCoast[nCoast].pGetProfile(nThisProfile)->pPtGetPointInProfile(nPoint+1);
//
//          if (Pt1 == Pt2)
//             LogStream << m_ulTimestep << ": IDENTICAL POINTS before changes, in profile {" << nThisProfile << "} points = " << nPoint << " and " << nPoint+1 << endl;
//       }
//    }
   // END: FOR CHECKING PURPOSES ******************************************************************

   // Get the index numbers of all coincident profiles for the 'main' to-retain profile for the line segment in which intersection occurs
   vector<pair<int, int> > prVCoincidentProfiles = *m_VCoast[nCoast].pGetProfile(nMainProfile)->pprVGetCoincidentProfilesForLineSegment(nMainProfileIntersectLineSeg);
   int nNumCoincident = prVCoincidentProfiles.size();
   vector<int> nLineSegAfterIntersect(nNumCoincident, -1);           // The line segment after the point of intersection, for each co-incident profile

   // Do this for the main profile and all profiles which are co-incident for this line segment
   for (int nn = 0; nn < nNumCoincident; nn++)
   {
      int
         nThisProfile = prVCoincidentProfiles[nn].first,             // The number of this profile
         nThisLineSeg = prVCoincidentProfiles[nn].second;            // The line segment of this profile
      CProfile* pThisProfile = m_VCoast[nCoast].pGetProfile(nThisProfile);

      // Is the intersection point already present in the to-retain profile?
      if (! bAlreadyPresent)
      {
         // It is not already present, so insert it and also update the associated multi-line
         if (! pThisProfile->bInsertIntersection(dIntersectX, dIntersectY, nThisLineSeg))
         {
            // Error
            LogStream << WARN << m_ulTimestep << ": cannot insert a line segment after the final line segment (" << nThisLineSeg << ") for " << (nThisProfile == nMainProfile ? "main" : "co-incident") << " profile (" << nThisProfile << "), abandoning" << endl;

            return RTN_ERR_CANNOT_INSERT_POINT;
         }

//          LogStream << "\tIntersection point NOT already in " << (nThisProfile == nMainProfile ? "main" : "co-incident") << " profile {" << nThisProfile << "}, inserted it as point " << nThisLineSeg+1 << endl;
      }

      // Get the line segment after intersection
      nLineSegAfterIntersect[nn] = nThisLineSeg+1;
   }

//    for (int nn = 0; nn < nNumCoincident; nn++)
//       LogStream << "\tFor profile {" << prVCoincidentProfiles[nn].first << "} line segment [" << nLineSegAfterIntersect[nn] << "] is immediately after the intersection point" << endl;

   // Get the coincident profiles for the to-truncate profile, at the line segment where intersection occurs
   vector<pair<int, int> > prVToTruncateCoincidentProfiles = *m_VCoast[nCoast].pGetProfile(nProfileToTruncate)->pprVGetCoincidentProfilesForLineSegment(nProfileToTruncateIntersectLineSeg);
   int nNumToTruncateCoincident = prVToTruncateCoincidentProfiles.size();

   // Now add the number of the to-truncate profile, and all its coincident profiles, to all line segments which are seaward of the point of intersection. Do this for the main profile and all profiles which are co-incident for this line segment
   for (int nn = 0; nn < nNumCoincident; nn++)
   {
      int nThisProfile = prVCoincidentProfiles[nn].first;             // The number of this profile
      CProfile* pThisProfile = m_VCoast[nCoast].pGetProfile(nThisProfile);

      // Get the number of line segments for this to-retain profile (will have just increased, if we just inserted a point)
      int nNumLineSegs = pThisProfile->nGetNumLineSegments();

      // Do for all line segments seaward of the point of intersection
      for (int nLineSeg = nLineSegAfterIntersect[nn], nIncr = 0; nLineSeg < nNumLineSegs; nLineSeg++, nIncr++)
      {
//         LogStream << "\tnThisProfile = " << nThisProfile << " nThisLineSeg = " << nThisLineSeg << " nLineSeg = " << nLineSeg << " nNumLineSegs = " << nNumLineSegs << endl;

         // Have no idea how this can happen, but check for the moment
//          if (nThisProfile == nProfileToTruncate)
//          {
//             LogStream << "\t*** ERROR nThisProfile = " << nThisProfile << " nProfileToTruncate = " << nProfileToTruncate << ", ignoring" << endl;
//             continue;
//          }

         // Add the number of the to-truncate profile, and all its coincident profiles, to this line segment
         for (int m = 0; m < nNumToTruncateCoincident; m++)
         {
            int
               nProfileToAdd = prVToTruncateCoincidentProfiles[m].first,
               nProfileToAddLineSeg = prVToTruncateCoincidentProfiles[m].second;

//            LogStream << "\tAdding " << (nProfileToAdd == nProfileToTruncate ? "main" : "co-incident") << " truncate-profile number {" << nProfileToAdd << "}, line segment [" << nProfileToAddLineSeg + nIncr << "] to line segment " << nLineSeg << " of " << (nThisProfile == nMainProfile ? "main" : "co-incident") << " to-retain profile {" << nThisProfile << "}" << endl;

            pThisProfile->AddCoincidentProfileToExistingLineSegment(nLineSeg, nProfileToAdd, nProfileToAddLineSeg + nIncr);
         }
      }
   }


   // START: FOR CHECKING PURPOSES ****************************************************************
   // Get the index numbers of all coincident profiles for the 'main' profile for the line segment in which intersection occurred
//    vector<pair<int, int> > prVCoincidentProfilesCHECK2 = *m_VCoast[nCoast].pGetProfile(nMainProfile)->pprVGetCoincidentProfilesForLineSegment(nMainProfileIntersectLineSeg);
//    int nNumCoincidentCHECK2 = prVCoincidentProfilesCHECK2.size();
//    for (int nn = 0; nn < nNumCoincidentCHECK2; nn++)
//    {
//       int nThisProfile = prVCoincidentProfilesCHECK2[nn].first;
//       LogStream << "\tAFTER nInsertPointIntoProfilesIfNeededThenUpdate(): " << (nThisProfile == nMainProfile ? "MAIN" : "COINCIDENT") << " to-retain profile {" << nThisProfile << "} has " << m_VCoast[nCoast].pGetProfile(nThisProfile)->nGetProfileSize() << " points ";
//       for (int nn = 0; nn < m_VCoast[nCoast].pGetProfile(nThisProfile)->nGetProfileSize(); nn++)
//          LogStream << "[" << m_VCoast[nCoast].pGetProfile(nThisProfile)->pPtGetPointInProfile(nn)->dGetX() << ", " << m_VCoast[nCoast].pGetProfile(nThisProfile)->pPtGetPointInProfile(nn)->dGetY() << "] ";
//       LogStream << "), and " << m_VCoast[nCoast].pGetProfile(nThisProfile)->nGetNumLineSegments() << " line segments, co-incident profiles and their line segments are ";
//       for (int nLineSeg = 0; nLineSeg < m_VCoast[nCoast].pGetProfile(nThisProfile)->nGetNumLineSegments(); nLineSeg++)
//       {
//          vector<pair<int, int> > prVTmp = *m_VCoast[nCoast].pGetProfile(nThisProfile)->pprVGetCoincidentProfilesForLineSegment(nLineSeg);
//          LogStream << "{ ";
//          for (int nn = 0; nn < m_VCoast[nCoast].pGetProfile(nThisProfile)->nGetNumCoincidentProfilesInLineSegment(nLineSeg); nn++)
//             LogStream << prVTmp[nn].first << "[" << prVTmp[nn].second << "] ";
//          LogStream << "} ";
//       }
//       LogStream << endl;
//
//       for (int nPoint = 0; nPoint < m_VCoast[nCoast].pGetProfile(nThisProfile)->nGetProfileSize()-1; nPoint++)
//       {
//          C2DPoint
//             Pt1 = *m_VCoast[nCoast].pGetProfile(nThisProfile)->pPtGetPointInProfile(nPoint),
//             Pt2 = *m_VCoast[nCoast].pGetProfile(nThisProfile)->pPtGetPointInProfile(nPoint+1);
//
//          if (Pt1 == Pt2)
//             LogStream << m_ulTimestep << ": IDENTICAL POINTS before changes, in profile {" << nThisProfile << "} points = " << nPoint << " and " << nPoint+1 << endl;
//       }
//    }
   // END: FOR CHECKING PURPOSES ******************************************************************

   return RTN_OK;
}


/*===============================================================================================================================

 Truncate a profile at the point of intersection, and do the same for all its co-incident profiles

===============================================================================================================================*/
void CDelineation::TruncateProfileAndAppendNew(int const nCoast, int const nMainProfile, int const nMainProfileIntersectLineSeg, vector<C2DPoint>* const pPtVProfileLastPart, vector<vector<pair<int, int> > >* const pprVLineSegLastPart)
{
   // START: FOR CHECKING PURPOSES ****************************************************************
   // Get the index numbers of all coincident profiles for the 'main' profile for the line segment in which intersection occurred
//    vector<pair<int, int> > prVCoincidentProfilesCHECK1 = *m_VCoast[nCoast].pGetProfile(nMainProfile)->pprVGetCoincidentProfilesForLineSegment(nMainProfileIntersectLineSeg);
//    int nNumCoincidentCHECK1 = prVCoincidentProfilesCHECK1.size();
//
//    LogStream << "\tTruncating profile {" << nMainProfile << "}, intersection is at [" << dIntersectX << ", " << dIntersectY << "] in line segment " << nMainProfileIntersectLineSeg << endl;
//    for (int nn = 0; nn < nNumCoincidentCHECK1; nn++)
//    {
//       int nThisProfile = prVCoincidentProfilesCHECK1[nn].first;
//       LogStream << "\tBEFORE TruncateProfileAndAppendNew(): " << (nThisProfile == nMainProfile ? "MAIN" : "COINCIDENT") << " to-truncate profile {" << nThisProfile << "} has " << m_VCoast[nCoast].pGetProfile(nThisProfile)->nGetProfileSize() << " points (";
//       for (int nn = 0; nn < m_VCoast[nCoast].pGetProfile(nThisProfile)->nGetProfileSize(); nn++)
//          LogStream << "[" << m_VCoast[nCoast].pGetProfile(nThisProfile)->pPtGetPointInProfile(nn)->dGetX() << ", " << m_VCoast[nCoast].pGetProfile(nThisProfile)->pPtGetPointInProfile(nn)->dGetY() << "] ";
//       LogStream << "), and " << m_VCoast[nCoast].pGetProfile(nThisProfile)->nGetNumLineSegments() << " line segments, co-incident profiles are ";
//       for (int nLineSeg = 0; nLineSeg < m_VCoast[nCoast].pGetProfile(nThisProfile)->nGetNumLineSegments(); nLineSeg++)
//       {
//          vector<pair<int, int> > prVTmp = *m_VCoast[nCoast].pGetProfile(nThisProfile)->pprVGetCoincidentProfilesForLineSegment(nLineSeg);
//          LogStream << "{ ";
//          for (int nn = 0; nn < m_VCoast[nCoast].pGetProfile(nThisProfile)->nGetNumCoincidentProfilesInLineSegment(nLineSeg); nn++)
//             LogStream << prVTmp[nn].first << "[" << prVTmp[nn].second << "] ";
//          LogStream << "} ";
//       }
//       LogStream << endl;
//
//       for (int nPoint = 0; nPoint < m_VCoast[nCoast].pGetProfile(nThisProfile)->nGetProfileSize()-1; nPoint++)
//       {
//          C2DPoint
//             Pt1 = *m_VCoast[nCoast].pGetProfile(nThisProfile)->pPtGetPointInProfile(nPoint),
//             Pt2 = *m_VCoast[nCoast].pGetProfile(nThisProfile)->pPtGetPointInProfile(nPoint+1);
//
//          if (Pt1 == Pt2)
//             LogStream << m_ulTimestep << ": IDENTICAL POINTS before changes, in profile {" << nThisProfile << "} points = " << nPoint << " and " << nPoint+1 << endl;
//       }
//    }
//    LogStream << "\tPart-profile to append is ";
//    for (unsigned int mm = 0; mm < pPtVProfileLastPart->size(); mm++)
//       LogStream << "[" << pPtVProfileLastPart->at(mm).dGetX() << ", " << pPtVProfileLastPart->at(mm).dGetY() << "] ";
//    LogStream << endl;
//    LogStream << "\tPart line-segment to append is ";
//    for (unsigned int mm = 0; mm < pprVLineSegLastPart->size(); mm++)
//    {
//       vector<pair<int, int> > prVTmp = pprVLineSegLastPart->at(mm);
//       LogStream << "{ ";
//       for (unsigned int nn = 0; nn < prVTmp.size(); nn++)
//          LogStream << prVTmp[nn].first << "[" << prVTmp[nn].second << "] ";
//       LogStream << "} ";
//    }
//    LogStream << endl;
   // END: FOR CHECKING PURPOSES ******************************************************************


   // Get the index numbers of all coincident profiles for the 'main' profile for the line segment in which intersection occurs
   vector<pair<int, int> > prVCoincidentProfiles = *m_VCoast[nCoast].pGetProfile(nMainProfile)->pprVGetCoincidentProfilesForLineSegment(nMainProfileIntersectLineSeg);
   int nNumCoincident = prVCoincidentProfiles.size();

   for (int nn = 0; nn < nNumCoincident; nn++)
   {
      // Do this for the main to-truncate profile, and do the same for all its co-incident profiles
      int
         nThisProfile = prVCoincidentProfiles[nn].first,
         nThisProfileLineSeg = prVCoincidentProfiles[nn].second;
      CProfile* pThisProfile = m_VCoast[nCoast].pGetProfile(nThisProfile);

//       if (nThisProfile == nMainProfile)
//          assert(nThisProfileLineSeg == nMainProfileIntersectLineSeg);

      // Truncate the profile
//      LogStream << "\tTruncating " << (nThisProfile == nMainProfile ? "MAIN" : "COINCIDENT") << " to-truncate profile {" << nThisProfile << "} at line segment " << nThisProfileLineSeg+1 << endl;
      pThisProfile->TruncateProfile(nThisProfileLineSeg+1);

      // Reduce the number of line segments for this profile
      pThisProfile->TruncateLineSegments(nThisProfileLineSeg+1);

      // Append the profile points from the last part of the retain-profile
      for (unsigned int mm = 0; mm < pPtVProfileLastPart->size(); mm++)
      {
         C2DPoint Pt = pPtVProfileLastPart->at(mm);
         pThisProfile->AppendPointInProfile(&Pt);
      }

      // Append the line segments, and their co-incident profile numbers, from the last part of the retain-profile
      for (unsigned int mm = 0; mm < pprVLineSegLastPart->size(); mm++)
      {
         vector<pair<int, int> > prVTmp = pprVLineSegLastPart->at(mm);

         pThisProfile->AppendLineSegment(&prVTmp);
      }

      // Fix the line seg numbers for this profile
      vector<int>
         nVProf,
         nVProfsLineSeg;
      for (int nSeg = 0; nSeg < pThisProfile->nGetNumLineSegments(); nSeg++)
      {
         for (int nCoinc = 0; nCoinc < pThisProfile->nGetNumCoincidentProfilesInLineSegment(nSeg); nCoinc++)
         {
            int
               nProf = pThisProfile->nGetProf(nSeg, nCoinc),
               nProfsLineSeg = pThisProfile->nGetProfsLineSeg(nSeg, nCoinc);

            vector<int>::iterator it = std::find(nVProf.begin(), nVProf.end(), nProf);
            if (it == nVProf.end())
            {
               // Not found
               nVProf.push_back(nProf);
               nVProfsLineSeg.push_back(nProfsLineSeg);
            }
            else
            {
               // Found
               int nPos = it - nVProf.begin();
               int nNewProfsLineSeg = nVProfsLineSeg[nPos];
               nNewProfsLineSeg++;

               nVProfsLineSeg[nPos] = nNewProfsLineSeg;
               pThisProfile->SetProfsLineSeg(nSeg, nCoinc, nNewProfsLineSeg);
            }
         }
      }

//       assert(pThisProfile->nGetProfileSize() > 1);
   }


   // START: FOR CHECKING PURPOSES ****************************************************************
   // Get the index numbers of all coincident profiles for the 'main' to-truncate profile for the line segment in which intersection occurred
//    vector<pair<int, int> > prVToTruncateCoincidentProfilesCHECK2 = *m_VCoast[nCoast].pGetProfile(nMainProfile)->pprVGetCoincidentProfilesForLineSegment(nMainProfileIntersectLineSeg);
//    int nNumToTruncateCoincidentCHECK2 = prVToTruncateCoincidentProfilesCHECK2.size();
//    for (int nn = 0; nn < nNumToTruncateCoincidentCHECK2; nn++)
//    {
//       int nThisProfile = prVToTruncateCoincidentProfilesCHECK2[nn].first;
//       LogStream << "\tAFTER TruncateProfileAndAppendNew(): " << (nThisProfile == nMainProfile ? "MAIN" : "COINCIDENT") << " to-truncate profile {" << nThisProfile << "} has " << m_VCoast[nCoast].pGetProfile(nThisProfile)->nGetProfileSize() << " points (";
//       for (int nn = 0; nn < m_VCoast[nCoast].pGetProfile(nThisProfile)->nGetProfileSize(); nn++)
//          LogStream << "[" << m_VCoast[nCoast].pGetProfile(nThisProfile)->pPtGetPointInProfile(nn)->dGetX() << ", " << m_VCoast[nCoast].pGetProfile(nThisProfile)->pPtGetPointInProfile(nn)->dGetY() << "] ";
//       LogStream << "), and " << m_VCoast[nCoast].pGetProfile(nThisProfile)->nGetNumLineSegments() << " line segments, co-incident profiles are ";
//       for (int mm = 0; mm < m_VCoast[nCoast].pGetProfile(nThisProfile)->nGetNumLineSegments(); mm++)
//       {
//          vector<pair<int, int> > prVTmp = *m_VCoast[nCoast].pGetProfile(nThisProfile)->pprVGetCoincidentProfilesForLineSegment(mm);
//          LogStream << "{ ";
//          for (int nn = 0; nn < m_VCoast[nCoast].pGetProfile(nThisProfile)->nGetNumCoincidentProfilesInLineSegment(mm); nn++)
//             LogStream << prVTmp[nn].first << "[" << prVTmp[nn].second << "] ";
//          LogStream << "} ";
//       }
//       LogStream << endl;
//
//       for (int nPoint = 0; nPoint < m_VCoast[nCoast].pGetProfile(nThisProfile)->nGetProfileSize()-1; nPoint++)
//       {
//          C2DPoint
//             Pt1 = *m_VCoast[nCoast].pGetProfile(nThisProfile)->pPtGetPointInProfile(nPoint),
//             Pt2 = *m_VCoast[nCoast].pGetProfile(nThisProfile)->pPtGetPointInProfile(nPoint+1);
//
//          if (Pt1 == Pt2)
//             LogStream << m_ulTimestep << ": IDENTICAL POINTS before changes, in profile {" << nThisProfile << "} points = " << nPoint << " and " << nPoint+1 << endl;
//       }
//    }
   // END: FOR CHECKING PURPOSES ******************************************************************
}
