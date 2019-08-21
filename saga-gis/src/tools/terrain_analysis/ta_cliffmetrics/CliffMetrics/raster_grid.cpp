/*!
 *
 * \file raster_grid.cpp
 * \brief CRasterGrid routines
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
#include "cliffmetrics.h"
#include "raster_grid.h"


CRasterGrid* CCell::m_pGrid = NULL;          // Initialise m_pGrid, the static member of CCell


CRasterGrid::CRasterGrid(CDelineation* pSimIn)
:  m_pSim(pSimIn)
{
}


CRasterGrid::~CRasterGrid(void)
{
}


CDelineation* CRasterGrid::pGetSim(void)
{
   return m_pSim;
}


CCell* CRasterGrid::pGetCell(int const nX, int const nY)
{
   return &m_Cell[nX][nY];
}


int CRasterGrid::nCreateGrid(void)
{
   // Create the 2D vector CCell array
   int
      nXMax = m_pSim->nGetGridXMax(),
      nYMax = m_pSim->nGetGridYMax();

   // TODO Check if we don't have enough memory, if so return RTN_ERR_MEMALLOC
   m_Cell.resize(nXMax);
   for (int nX = 0; nX < nXMax; nX++)
      m_Cell[nX].resize(nYMax);

   // Initialize the CCell shared pointer to the CRasterGrid object
   CCell::m_pGrid = this;

   return RTN_OK;
}

