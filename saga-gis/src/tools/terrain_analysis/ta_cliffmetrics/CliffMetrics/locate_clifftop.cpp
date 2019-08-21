/*!
 *
 * \file locate_clifftop.cpp
 * \brief For each profile along the coast, locate the cliff top (and toe)
 * \details TODO A more detailed description of these routines.
 * \author Andres Payo 
 * \author David Favis-Mortlock
 * \author Martin Husrt
 * \author Monica Palaseanu-Lovejoy
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
#include <cmath>

#include <iostream>
using std::cout;
using std::cerr;
using std::endl;
using std::ios;

#include <iomanip>
using std::setiosflags;

#include "cliffmetrics.h"
#include "i_line.h"
#include "line.h"
#include "delineation.h"
#include "raster_grid.h"
#include "coast.h"
#include "hermite_cubic.h"

/*===============================================================================================================================

 Locate cliff top on coastline-normal profiles

===============================================================================================================================*/
int CDelineation::nLocateCliffTop(void)
{

   // Do this for each coast
   for (int nCoast = 0; nCoast < static_cast<int>(m_VCoast.size()); nCoast++)
   {
      int const nNumProfiles = m_VCoast[nCoast].nGetNumProfiles();

      // Do it for every coastline-normal profile. Can do this in the original, curvature-related, sequence of profiles
      for (int nProfile = 0; nProfile < nNumProfiles; nProfile++)
      {
	 // Get the number of the coastline-normal profile
	 CProfile* const pProfile = m_VCoast[nCoast].pGetProfile(nProfile);
	 
	 // Get the length of the profile (in cells) and the index of the coast point at which this profile starts
         int const  nProfSize = pProfile->nGetNumCellsInProfile();
	 if (nProfSize < 3) pProfile->SetTooShort(true);
	 
	 // Only work on this profile if it is problem-free 
        if (pProfile->bOKIncStartAndEndOfCoast())
	{  
	// cout << "nProfSize, m_bTooShort: " << nProfSize << ", " << pProfile->bTooShort() << endl;

	 // Calculate the length of the profile in external CRS units
         int const nSegments = pProfile->nGetProfileSize()-1;
         double dProfileLenXY = 0;
         for (int nSeg = 0; nSeg < nSegments; nSeg++)
         {
           // Do once for every line segment
           double const
           dSegStartX = pProfile->pPtGetPointInProfile(nSeg)->dGetX(),
           dSegStartY = pProfile->pPtGetPointInProfile(nSeg)->dGetY(),
           dSegEndX = pProfile->pPtGetPointInProfile(nSeg+1)->dGetX(),     // Is OK
           dSegEndY = pProfile->pPtGetPointInProfile(nSeg+1)->dGetY();

           double const dSegLen = hypot(dSegStartX - dSegEndX, dSegStartY - dSegEndY);
           dProfileLenXY += dSegLen;
	 }
	 // Next calculate the average distance between profile points, again in external CRS units. Assume that the sample points are equally spaced along the profile (not quite true)
	 double const dSpacingXY = dProfileLenXY / (nProfSize-1);
	 
	 // Set up vectors for the coastline-normal profile elevations. The length of this vector line is given by the number of cells 'under' the profile. Thus each point on the vector relates to a single cell in the grid. This assumes that all points on the profile vector are equally spaced (not quite true, depends on the orientation of the line segments which comprise the profile)
	 // The elevation of each of these profile points is the elevation of the centroid of the cell that is 'under' the point. However we cannot always be confident that this is the 'true' elevation of the point on the vector since (unless the profile runs planview N-S or W-E) the vector does not always run exactly through the centroid of the cell
	 vector<double>
	  dVProfileZ(nProfSize, 0),                 // Elevation of DTM for cells 'under' the profile
	  dVProfileDistXY(nProfSize,  0);           // Along-profile distance measured from the coast, in external CRS units
	  
	 for (int i = 0; i < nProfSize; i++)
	 {
	    int const
	    nX = pProfile->pPtiVGetCellsInProfile()->at(i).nGetX(),
	    nY = pProfile->pPtiVGetCellsInProfile()->at(i).nGetY();

	    // Get the elevation for both consolidated and unconsolidated sediment on this cell
	    dVProfileZ[i] = m_pRasterGrid->pGetCell(nX, nY)->dGetSedimentTopElev();

	    // And store the X-Y plane distance from the start of the profile
	    dVProfileDistXY[i] = i * dSpacingXY;
	 }
	
	 // Now we have all information needed to locate the cliff top (and cliff toe) so do it
	 double dElevationChangeperUnitdistance = (dVProfileZ.back()-dVProfileZ.front())/(dVProfileDistXY.back());
	 int 
	    nCliffTopIndex = nProfSize-1,
	    nCliffToeIndex = 0;
	
	 vector<double> dDetrendedProfileElev(nProfSize,  0); 

	 // first detrend the profile elevation and  find the CliffTop location
	 double dDetrendedProfileMaxElev = 0;
	 for (int i = 0; i < nProfSize; i++)
	 {
	    dDetrendedProfileElev[i] = (dVProfileZ[i] - dElevationChangeperUnitdistance*dVProfileDistXY[i]) - dVProfileZ[0];
	    if (dDetrendedProfileElev[i] >= dDetrendedProfileMaxElev && dDetrendedProfileElev[i]>= m_dEleTolerance)
	    {
	       dDetrendedProfileMaxElev = dDetrendedProfileElev[i];
	       nCliffTopIndex = i;
	    }
	 }
	 // Next find the CliffToe location. CliffToe can not be more landwards than CliffTop 
	 double  dDetrendedProfileMinElev = 0;
	 for (int i = 0; i < nProfSize; i++)
	 {
	    if ((dDetrendedProfileElev[i] <= dDetrendedProfileMinElev)&& (dDetrendedProfileElev[i] <= -m_dEleTolerance) && (i < nCliffTopIndex))
	    {
	       dDetrendedProfileMinElev = dDetrendedProfileElev[i];
	       nCliffToeIndex = i;
	    }
	 }  
	 
	 // Sanity check of calculated cliff top and cliff toe
	 pProfile->SetCliffTopQualityFlag(true);
	 pProfile->SetCliffToeQualityFlag(true);

	 if (dVProfileZ[nCliffToeIndex] >= dVProfileZ[nCliffTopIndex])
	 {
	    // Save the profile index at which the Cliff Top and Toe has been located
	    pProfile->SetCliffTopQualityFlag(false);
	    pProfile->SetCliffToeQualityFlag(false);
	 }
	 
	 // Save the profile index at which the Cliff Top and Toe has been located
	 pProfile->SetCliffTopPoint(nCliffTopIndex);
	 pProfile->SetCliffToePoint(nCliffToeIndex);
	 
	 // Save the Chainage at which the Cliff Top and Toe has been located
	 pProfile->SetCliffTopChainage(dVProfileDistXY[nCliffTopIndex]);
	 pProfile->SetCliffToeChainage(dVProfileDistXY[nCliffToeIndex]);

	 // If desired, save this coastline-normal profile data for checking purposes
         if (m_bOutputProfileData)
         {
         int nRet = nSaveProfile(nProfile, nCoast, nProfSize, &dVProfileDistXY, &dVProfileZ, pProfile->pPtiVGetCellsInProfile(), &dDetrendedProfileElev);
         if (nRet != RTN_OK)
           return nRet;
         }
	} // end of if profile has no problem
      } // enf for each normal profile

   } // end for each coast

   return RTN_OK;
}
