/*!
 *
 * \file init_grid.cpp
 * \brief Initialises the raster grid and calculates sea depth on each cell
 * \details TODO A more detailed description of this routine.
 * \author Andres Payo, David Favis-Mortlock, Martin Husrt, Monica Palaseanu-Lovejoy
 * \date 2020
 * \copyright GNU General Public License
 *
 */

/*==============================================================================================================================

 This file is part of CliffMetrics, the Coastal Modelling Environment.

 CliffMetrics is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

==============================================================================================================================*/
//#include <assert.h>
#include <iostream>
using std::endl;

#include "cliffmetrics.h"
#include "line.h"
#include "cell.h"
#include "coast.h"
#include "delineation.h"
#include "raster_grid.h"


/*===============================================================================================================================

  At the beginning of each timestep: initialize the raster grid; clear all coastlines, profiles, and polygons; and initialize some per-timestep accounting variables

===============================================================================================================================*/
int CDelineation::nInitGridAndCalcStillWaterLevel(void)
{
   // Clear all vector coastlines, profiles, and polygons
   if (m_strInitialCoastlineFile.empty())
      m_VCoast.clear();
   
   m_pVCoastPolygon.clear();

   // Do some per-timestep initialization
   m_ulThisTimestepNumSeaCells                      =
   m_ulThisTimestepNumCoastCells                    = 0;


   // And go through all cells in the RasterGrid array
   for (int nX = 0; nX < m_nXGridMax; nX++)
   {
      for (int nY = 0; nY < m_nYGridMax; nY++)
      {
         // Initialize values for this cell
         m_pRasterGrid->pGetCell(nX, nY)->InitCell();

            // Calculate the elevation of all this cell's layers. During the rest of the simulation, each cell's elevation is re-calculated just after any change occurs on that cell
            m_pRasterGrid->pGetCell(nX, nY)->CalcAllLayerElevs();
      }
   }

   return RTN_OK;
}
