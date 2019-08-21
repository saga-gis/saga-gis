/*!
 *
 * \class CRasterGrid
 * \brief Class used to represent the raster grid of cell objects
 * \details TODO This is a more detailed description of the CRasterGrid class.
 * \author David Favis-Mortlock
 * \author Andres Payo
 * \author Jim Hall
 * \date 2017
 * \copyright GNU General Public License
 *
 * \file rastergrid.h
 * \brief Contains CRasterGrid definitions
 *
 */

#ifndef RASTERGRID_H
#define RASTERGRID_H
/*===============================================================================================================================

 This file is part of CliffMetrics, the Coastal Modelling Environment.

 CliffMetrics is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

===============================================================================================================================*/
#include "cliffmetrics.h"
#include "cell.h"


class CCell;         // Forward declaration
class CDelineation;   // Ditto

class CRasterGrid
{
private:
   double
      m_dD50Fine,
      m_dD50Sand,
      m_dD50Coarse;

   CDelineation* m_pSim;

   vector< vector<CCell> > m_Cell;

public:

   explicit CRasterGrid(CDelineation*);
   ~CRasterGrid(void);

   CDelineation* pGetSim(void);
   CCell* pGetCell(int const, int const);
   int nCreateGrid(void);
};
#endif // RASTERGRID_H
