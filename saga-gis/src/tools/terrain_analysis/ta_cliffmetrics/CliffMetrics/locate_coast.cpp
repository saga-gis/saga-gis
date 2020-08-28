/*!
 *
 * \file locate_coast.cpp
 * \brief Finds the coastline on the raster grid
 * \details TODO A more detailed description of these routines.
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
// #include <assert.h>
#include <cfloat>

#include <iostream>
using std::cout;
using std::cerr;
using std::endl;
using std::ios;

#include <iomanip>
using std::setiosflags;

#include <stack>
using std::stack;

#include "cliffmetrics.h"
#include "i_line.h"
#include "line.h"
#include "delineation.h"
#include "raster_grid.h"
#include "coast.h"


/*===============================================================================================================================

 First find all connected sea areas, then locate the vector coastline(s), then put these onto the raster grid

===============================================================================================================================*/
int CDelineation::nLocateSeaAndCoasts(void)
{
   if (m_strInitialCoastlineFile.empty())
   {
      // Find all connected sea cells
      FindAllSeaCells();

      // Find every coastline on the raster grid, mark raster cells, then create the vector coastline
      int nRet = nTraceAllCoasts();
      if (nRet != RTN_OK)
	 return nRet;
   }
   else
   {
      // User has defined a Vector coastline, to be used instead
      int nCoast = m_VCoast.size()-1;
 
      // Next, set values for the coast's other attributes. First set the coast's handedness, and start and end edges
      m_VCoast[nCoast].SetSeaHandedness(m_nCoastSeaHandiness);
      
      // Mark these cells as coast cells
      for (int n = 0; n < m_VCoast[nCoast].nGetCoastlineSize(); n++)
      {
	  // The start point of the normal, must convert from the external CRS to grid CRS. If this is the first line segment of the profile, then the start point is the centroid of a coastline cell
      double
         dXn = dExtCRSXToGridX(m_VCoast[nCoast].pPtGetVectorCoastlinePoint(n)->dGetX()),
         dYn = dExtCRSYToGridY(m_VCoast[nCoast].pPtGetVectorCoastlinePoint(n)->dGetY());
      int
         nXn = static_cast<int>(dXn),
         nYn = static_cast<int>(dYn);
	 
	C2DIPoint Pti(nXn, nYn);
	 
	// Also store the locations of the corresponding unsmoothed points (in raster-grid CRS) in the coast's m_VCellsMarkedAsCoastline vector
         m_VCoast[nCoast].AppendCellMarkedAsCoastline(&Pti);
	
	 if (bIsWithinGrid(nXn, nYn))
	    m_pRasterGrid->pGetCell(nXn, nYn)->SetAsCoastline(true);
      }
      
      // To calculate the curvature of the vector coastline
      DoCoastCurvature(nCoast, m_nCoastSeaHandiness);
   }
   return RTN_OK;
}


/*===============================================================================================================================

 Finds and flags all sea areas which have at least one cell at a grid edge (i.e. does not flag 'inland' seas)

===============================================================================================================================*/
void CDelineation::FindAllSeaCells(void)
{
   // Go along all grid edges, starting from the approximate centre of each edge
   int
      nXMid = m_nXGridMax / 2,
      nYMid = m_nYGridMax / 2;

   // Start with the N edge
   for (int nX = nXMid; nX >= 0; nX--)
   {
      if ((m_pRasterGrid->pGetCell(nX, 0)->bIsInundated()) && (m_pRasterGrid->pGetCell(nX, 0)->dGetSeaDepth() == 0))
         // This edge cell is below SWL and sea depth is still zero
         FloodFillSea(nX, 0);
   }
   for (int nX = nXMid+1; nX < m_nXGridMax; nX++)
   {
      if ((m_pRasterGrid->pGetCell(nX, 0)->bIsInundated()) && (m_pRasterGrid->pGetCell(nX, 0)->dGetSeaDepth() == 0))
         // This edge cell is below SWL and sea depth is still zero
         FloodFillSea(nX, 0);
   }

   // Next the S edge
   for (int nX = nXMid; nX >= 0; nX--)
   {
      if ((m_pRasterGrid->pGetCell(nX, m_nYGridMax-1)->bIsInundated()) && (m_pRasterGrid->pGetCell(nX, m_nYGridMax-1)->dGetSeaDepth() == 0))
         // This edge cell is below SWL and sea depth is still zero
         FloodFillSea(nX, m_nYGridMax-1);
   }
   for (int nX = nXMid+1; nX < m_nXGridMax; nX++)
   {
      if ((m_pRasterGrid->pGetCell(nX, m_nYGridMax-1)->bIsInundated()) && (m_pRasterGrid->pGetCell(nX, m_nYGridMax-1)->dGetSeaDepth() == 0))
         // This edge cell is below SWL and sea depth is still zero
         FloodFillSea(nX, m_nYGridMax-1);
   }

   // Now the W edge
   for (int nY = nYMid; nY >= 0; nY--)
   {
      if ((m_pRasterGrid->pGetCell(0, nY)->bIsInundated()) && (m_pRasterGrid->pGetCell(0, nY)->dGetSeaDepth() == 0))
         // This edge cell is below SWL and sea depth is still zero
         FloodFillSea(0, nY);
   }
   for (int nY = nYMid+1; nY < m_nYGridMax; nY++)
   {
      if ((m_pRasterGrid->pGetCell(0, nY)->bIsInundated()) && (m_pRasterGrid->pGetCell(0, nY)->dGetSeaDepth() == 0))
         // This edge cell is below SWL and sea depth is still zero
         FloodFillSea(0, nY);
   }

   // Finally the E edge
   for (int nY = nYMid; nY >= 0; nY--)
   {
      if ((m_pRasterGrid->pGetCell(m_nXGridMax-1, nY)->bIsInundated()) && (m_pRasterGrid->pGetCell(m_nXGridMax-1, nY)->dGetSeaDepth() == 0))
         // This edge cell is below SWL and sea depth is still zero
         FloodFillSea(m_nXGridMax-1, nY);
   }
   for (int nY = nYMid+1; nY < m_nYGridMax; nY++)
   {
      if ((m_pRasterGrid->pGetCell(m_nXGridMax-1, nY)->bIsInundated()) && (m_pRasterGrid->pGetCell(m_nXGridMax-1, nY)->dGetSeaDepth() == 0))
         // This edge cell is below SWL and sea depth is still zero
         FloodFillSea(m_nXGridMax-1, nY);
   }
}


/*===============================================================================================================================

 Flood-fills all sea cells starting from a given cell. The flood fill code used here is adapted from an example by Lode Vandevenne (http://lodev.org/cgtutor/floodfill.html#Scanline_Floodfill_Algorithm_With_Stack)

===============================================================================================================================*/
void CDelineation::FloodFillSea(int const nXStart, int const nYStart)
{
   // Create an empty stack
   stack<C2DIPoint> PtiStack;

   // Start at the given edge cell, push this onto the stack
   PtiStack.push(C2DIPoint(nXStart, nYStart));

   // Then do the flood fill: loop until there are no more cell co-ords on the stack
   while (! PtiStack.empty())
   {
      C2DIPoint Pti = PtiStack.top();
      PtiStack.pop();

      int
         nX = Pti.nGetX(),
         nY = Pti.nGetY();

      while ((nX >= 0) && (m_pRasterGrid->pGetCell(nX, nY)->bIsInundated()) && (m_pRasterGrid->pGetCell(nX, nY)->dGetSeaDepth() == 0))
         nX--;

      nX++;
      bool
         bSpanAbove = false,
         bSpanBelow = false;

      while ((nX < m_nXGridMax) && (m_pRasterGrid->pGetCell(nX, nY)->bIsInundated()) && (m_pRasterGrid->pGetCell(nX, nY)->dGetSeaDepth() == 0))
      {
         // Set the sea depth for this cell
         m_pRasterGrid->pGetCell(nX, nY)->SetSeaDepth();

//          double dDepth = m_pRasterGrid->pGetCell(nX, nY)->dGetSeaDepth();
//          assert(dDepth > 0);

         // Mark as sea
         m_pRasterGrid->pGetCell(nX, nY)->SetInContiguousSea();

        
         if ((! bSpanAbove) && (nY > 0) && (m_pRasterGrid->pGetCell(nX, nY-1)->bIsInundated()) && (m_pRasterGrid->pGetCell(nX, nY-1)->dGetSeaDepth() == 0))
         {
            PtiStack.push(C2DIPoint(nX, nY-1));
            bSpanAbove = true;
         }
         else if (bSpanAbove && (nY > 0) && ((! m_pRasterGrid->pGetCell(nX, nY-1)->bIsInundated()) || (m_pRasterGrid->pGetCell(nX, nY-1)->dGetSeaDepth() != 0)))
         {
            bSpanAbove = false;
         }

         if ((! bSpanBelow) && (nY < m_nYGridMax-1) && (m_pRasterGrid->pGetCell(nX, nY+1)->bIsInundated()) && (m_pRasterGrid->pGetCell(nX, nY+1)->dGetSeaDepth() == 0))
         {
            PtiStack.push(C2DIPoint(nX, nY+1));
            bSpanBelow = true;
         }
         else if (bSpanBelow && (nY < m_nYGridMax-1) && ((! m_pRasterGrid->pGetCell(nX, nY+1)->bIsInundated()) || (m_pRasterGrid->pGetCell(nX, nY+1)->dGetSeaDepth() != 0)))
         {
            bSpanBelow = false;
         }

         nX++;
      }
   }
}


/*===============================================================================================================================

 Locates coastline start points on the edges of the raster grid, and trace the vector coastline(s) from these start points. The vector coastlines may then be smoothed

===============================================================================================================================*/
int CDelineation::nTraceAllCoasts(void)
{
   // Go along all grid edges, starting from the approximate centre of each edge
   int
      nXMid = m_nXGridMax / 2,
      nYMid = m_nYGridMax / 2;

   // Start with the N edge
   for (int nX = nXMid; nX > 0; nX--)
   {
      // Get "Is it sea?" information for 'this' and 'next' cells
      bool
         bThisCellIsSea = m_pRasterGrid->pGetCell(nX, 0)->bIsInContiguousSea(),
         bNextCellIsSea = m_pRasterGrid->pGetCell(nX-1, 0)->bIsInContiguousSea();

      // Are we at a coast?
      if ((! bThisCellIsSea) && bNextCellIsSea)
      {
         // 'This' cell is just inland, has it already been flagged as a coast cell?
         if (! m_pRasterGrid->pGetCell(nX, 0)->bIsCoastline())
         {
            // It has not, so trace a coastline from 'this' cell
            int nRet = nTraceCoastLine(ORIENTATION_SOUTH, RIGHT_HANDED, nX, 0);
            if (nRet != RTN_OK)
               return nRet;
         }
      }
      else if (bThisCellIsSea && (! bNextCellIsSea))
      {
         // The 'next' cell is just inland, has it already been flagged as a coast cell?
         if (! m_pRasterGrid->pGetCell(nX-1, 0)->bIsCoastline())
         {
            // It has not, so trace a coastline from the 'next' cell
            int nRet = nTraceCoastLine(ORIENTATION_SOUTH, LEFT_HANDED, nX-1, 0);
            if (nRet != RTN_OK)
               return nRet;
         }
      }
   }

   for (int nX = nXMid; nX < m_nXGridMax-1; nX++)
   {
      // Get "Is it sea?" information for 'this' and 'next' cells
      bool
         bThisCellIsSea = m_pRasterGrid->pGetCell(nX, 0)->bIsInContiguousSea(),
         bNextCellIsSea = m_pRasterGrid->pGetCell(nX+1, 0)->bIsInContiguousSea();

      // Are we at a coast?
      if ((! bThisCellIsSea) && bNextCellIsSea)
      {
         // 'This' cell is just inland, has it already been flagged as a coast cell?
         if (! m_pRasterGrid->pGetCell(nX, 0)->bIsCoastline())
         {
            // It has not, so trace a coastline from 'this' cell
            int nRet = nTraceCoastLine(ORIENTATION_SOUTH, LEFT_HANDED, nX, 0);
            if (nRet != RTN_OK)
               return nRet;
         }
      }
      else if (bThisCellIsSea && (! bNextCellIsSea))
      {
         // The 'next' cell is just inland, has it already been flagged as a coast cell?
         if (! m_pRasterGrid->pGetCell(nX+1, 0)->bIsCoastline())
         {
            // It has not, so trace a coastline from the 'next' cell
            int nRet = nTraceCoastLine(ORIENTATION_SOUTH, RIGHT_HANDED, nX+1, 0);
            if (nRet != RTN_OK)
               return nRet;
         }
      }
   }

   // Next do the S edge
   for (int nX = nXMid; nX > 0; nX--)
   {
      // Get "Is it sea?" information for 'this' and 'next' cells
      bool
         bThisCellIsSea = m_pRasterGrid->pGetCell(nX, m_nYGridMax-1)->bIsInContiguousSea(),
         bNextCellIsSea = m_pRasterGrid->pGetCell(nX-1, m_nYGridMax-1)->bIsInContiguousSea();

      // Are we at a coast?
      if ((! bThisCellIsSea) && bNextCellIsSea)
      {
         // 'This' cell is just inland, has it already been flagged as a coast cell?
         if (! m_pRasterGrid->pGetCell(nX, m_nYGridMax-1)->bIsCoastline())
         {
            // It has not, so trace a coastline from 'this' cell
            int nRet = nTraceCoastLine(ORIENTATION_NORTH, LEFT_HANDED, nX, m_nYGridMax-1);
            if (nRet != RTN_OK)
               return nRet;
         }
      }
      else if (bThisCellIsSea && (! bNextCellIsSea))
      {
         // The 'next' cell is just inland, has it already been flagged as a coast cell?
         if (! m_pRasterGrid->pGetCell(nX-1, m_nYGridMax-1)->bIsCoastline())
         {
            // It has not, so trace a coastline from the 'next' cell
            int nRet = nTraceCoastLine(ORIENTATION_NORTH, RIGHT_HANDED, nX-1, m_nYGridMax-1);
            if (nRet != RTN_OK)
               return nRet;
         }
      }
   }

   for (int nX = nXMid; nX < m_nXGridMax-1; nX++)
   {
      // Get "Is it sea?" information for 'this' and 'next' cells
      bool
         bThisCellIsSea = m_pRasterGrid->pGetCell(nX, m_nYGridMax-1)->bIsInContiguousSea(),
         bNextCellIsSea = m_pRasterGrid->pGetCell(nX+1, m_nYGridMax-1)->bIsInContiguousSea();

      // Are we at a coast?
      if ((! bThisCellIsSea) && bNextCellIsSea)
      {
         // 'This' cell is just inland, has it already been flagged as a coast cell?
         if (! m_pRasterGrid->pGetCell(nX, m_nYGridMax-1)->bIsCoastline())
         {
            // It has not, so trace a coastline from 'this' cell
            int nRet = nTraceCoastLine(ORIENTATION_NORTH, RIGHT_HANDED, nX, m_nYGridMax-1);
            if (nRet != RTN_OK)
               return nRet;
         }
      }
      else if (bThisCellIsSea && (! bNextCellIsSea))
      {
         // The 'next' cell is just inland, has it already been flagged as a coast cell?
         if (! m_pRasterGrid->pGetCell(nX+1, m_nYGridMax-1)->bIsCoastline())
         {
            // It has not, so trace a coastline from the 'next' cell
            int nRet = nTraceCoastLine(ORIENTATION_NORTH, LEFT_HANDED, nX+1, m_nYGridMax-1);
            if (nRet != RTN_OK)
               return nRet;
         }
      }
   }

   // Now the W edge
   for (int nY = nYMid; nY > 0; nY--)
   {
      // Get "Is it sea?" information for 'this' and 'next' cells
      bool
         bThisCellIsSea = m_pRasterGrid->pGetCell(0, nY)->bIsInContiguousSea(),
         bNextCellIsSea = m_pRasterGrid->pGetCell(0, nY-1)->bIsInContiguousSea();

      // Are we at a coast?
      if ((! bThisCellIsSea) && bNextCellIsSea)
      {
         // 'This' cell is just inland, has it already been flagged as a coast cell?
         if (! m_pRasterGrid->pGetCell(0, nY)->bIsCoastline())
         {
            // It has not, so trace a coastline from 'this' cell
            int nRet = nTraceCoastLine(ORIENTATION_EAST, LEFT_HANDED, 0, nY);
            if (nRet != RTN_OK)
               return nRet;
         }
      }
      else if (bThisCellIsSea && (! bNextCellIsSea))
      {
         // The 'next' cell is just inland, has it already been flagged as a coast cell?
         if (! m_pRasterGrid->pGetCell(0, nY-1)->bIsCoastline())
         {
            // It has not, so trace a coastline from the 'next' cell
            int nRet = nTraceCoastLine(ORIENTATION_EAST, RIGHT_HANDED, 0, nY-1);
            if (nRet != RTN_OK)
               return nRet;
         }
      }
   }

   for (int nY = nYMid; nY < m_nYGridMax-1; nY++)
   {
      // Get "Is it sea?" information for 'this' and 'next' cells
      bool
         bThisCellIsSea = m_pRasterGrid->pGetCell(0, nY)->bIsInContiguousSea(),
         bNextCellIsSea = m_pRasterGrid->pGetCell(0, nY+1)->bIsInContiguousSea();

      // Are we at a coast?
      if ((! bThisCellIsSea) && bNextCellIsSea)
      {
         // 'This' cell is just inland, has it already been flagged as a coast cell?
         if (! m_pRasterGrid->pGetCell(0, nY)->bIsCoastline())
         {
            // It has not, so trace a coastline from 'this' cell
            int nRet = nTraceCoastLine(ORIENTATION_EAST, RIGHT_HANDED, 0, nY);
            if (nRet != RTN_OK)
               return nRet;
         }
      }
      else if (bThisCellIsSea && (! bNextCellIsSea))
      {
         // The 'next' cell is just inland, has it already been flagged as a coast cell?
         if (! m_pRasterGrid->pGetCell(0, nY+1)->bIsCoastline())
         {
            // It has not, so trace a coastline from the 'next' cell
            int nRet = nTraceCoastLine(ORIENTATION_EAST, LEFT_HANDED, 0, nY+1);
            if (nRet != RTN_OK)
               return nRet;
         }
      }
   }

   // Finally the E edge
   for (int nY = nYMid; nY > 0; nY--)
   {
      // Get "Is it sea?" information for 'this' and 'next' cells
      bool
         bThisCellIsSea = m_pRasterGrid->pGetCell(m_nXGridMax-1, nY)->bIsInContiguousSea(),
         bNextCellIsSea = m_pRasterGrid->pGetCell(m_nXGridMax-1, nY-1)->bIsInContiguousSea();

      // Are we at a coast?
      if ((! bThisCellIsSea) && bNextCellIsSea)
      {
         // 'This' cell is just inland, has it already been flagged as a coast cell?
         if (! m_pRasterGrid->pGetCell(m_nXGridMax-1, nY)->bIsCoastline())
         {
            // It has not, so trace a coastline from 'this' cell
            int nRet = nTraceCoastLine(ORIENTATION_WEST, RIGHT_HANDED, m_nXGridMax-1, nY);
            if (nRet != RTN_OK)
               return nRet;
         }
      }
      else if (bThisCellIsSea && (! bNextCellIsSea))
      {
         // The 'next' cell is just inland, has it already been flagged as a coast cell?
         if (! m_pRasterGrid->pGetCell(m_nXGridMax-1, nY-1)->bIsCoastline())
         {
            // It has not, so trace a coastline from the 'next' cell
            int nRet = nTraceCoastLine(ORIENTATION_WEST, LEFT_HANDED, m_nXGridMax-1, nY-1);
            if (nRet != RTN_OK)
               return nRet;
         }
      }
   }

   for (int nY = nYMid; nY < m_nYGridMax-1; nY++)
   {
      // Get "Is it sea?" information for 'this' and 'next' cells
      bool
         bThisCellIsSea = m_pRasterGrid->pGetCell(m_nXGridMax-1, nY)->bIsInContiguousSea(),
         bNextCellIsSea = m_pRasterGrid->pGetCell(m_nXGridMax-1, nY+1)->bIsInContiguousSea();

      // Are we at a coast?
      if ((! bThisCellIsSea) && bNextCellIsSea)
      {
         // 'This' cell is just inland, has it already been flagged as a coast cell?
         if (! m_pRasterGrid->pGetCell(m_nXGridMax-1, nY)->bIsCoastline())
         {
            // It has not, so trace a coastline from 'this' cell
            int nRet = nTraceCoastLine(ORIENTATION_WEST, LEFT_HANDED, m_nXGridMax-1, nY);
            if (nRet != RTN_OK)
               return nRet;
         }
      }
      else if (bThisCellIsSea && (! bNextCellIsSea))
      {
         // The 'next' cell is just inland, has it already been flagged as a coast cell?
         if (! m_pRasterGrid->pGetCell(m_nXGridMax-1, nY+1)->bIsCoastline())
         {
            // It has not, so trace a coastline from the 'next' cell
            int nRet = nTraceCoastLine(ORIENTATION_WEST, RIGHT_HANDED, m_nXGridMax-1, nY+1);
            if (nRet != RTN_OK)
               return nRet;
         }
      }
   }

   return RTN_OK;
}


/*==============================================================================================================================

 Traces the coastline (which is defined to be just above still water level) on the grid using the 'wall follower' rule for maze traversal (http://en.wikipedia.org/wiki/Maze_solving_algorithm#Wall_follower)

===============================================================================================================================*/
int CDelineation::nTraceCoastLine(int const nStartSearchDirection, int const nHandedness, int const nStartX, int const nStartY)
{
   bool
      bAtCoast = false,
      bHasLeftStartEdge = false,
      bSearchedTooLong = false;

   int
      nX = nStartX,
      nY = nStartY,
      nSearchDirection = nStartSearchDirection,
      nRoundTheLoop = 0;

   CILine LTempGridCRS;                      // Temporary coastline as a line of integer points (in grid coordinates)

   // Start at this grid-edge point and trace the rest of the coastline using the 'wall follower' rule for maze traversal, keeping next to cells flagged as sea
   do
   {
      // Safety device
      if (++nRoundTheLoop >= ROUND_LOOP_MAX)
      {
         bSearchedTooLong = true;
         break;
      }

      // Have we left the start edge?
      if (! bHasLeftStartEdge)
      {
         if (((nStartX == 0) && (nX > 0)) ||
             ((nStartX == (m_nXGridMax-1)) && (nX < m_nXGridMax-1)) ||
             ((nStartY == 0) && (nY > 0)) ||
             ((nStartY == (m_nYGridMax-1)) && (nY < m_nYGridMax-1)))
            bHasLeftStartEdge = true;
      }

      // Leave the loop if the vector coastline has left the start edge, then we find a coast cell which is at an edge (note that this edge could be the same edge from which this coastline started)
      if (bHasLeftStartEdge && bAtCoast)
      {
         if ((nX <= 0) || (nX >= m_nXGridMax-1) || (nY <= 0) || (nY >= m_nYGridMax-1))
            break;
      }

      // A sanity check: has the coastline become too long?
      if (LTempGridCRS.nGetSize() > m_nCoastMax)
      {
         // We have a problem, the vector coastline is unreasonably big
         LogStream << ERR << "timestep " << m_ulTimestep << ": size of temporary coastline from [" << nStartX << "][" << nStartY << "] is " << LTempGridCRS.nGetSize() << " which  exceeds maximum (" << m_nCoastMax << ")" << endl;
         return RTN_ERR_FINDCOAST;
      }

      // OK now sort out the next timestep of the search
      bAtCoast = false;
//       KeepWithinGrid(nX, nY);       // Safety check, is rarely needed but keep it anyway

      int
         nXSeaward = 0,
         nYSeaward = 0,
         nSeawardNewDirection = 0,
         nXStraightOn = 0,
         nYStraightOn = 0,
         nXAntiSeaward = 0,
         nYAntiSeaward = 0,
         nAntiSeawardNewDirection = 0,
         nXGoBack = 0,
         nYGoBack = 0,
         nGoBackNewDirection = 0;

      C2DIPoint Pti(nX, nY);

      // Set up the variables
      switch (nHandedness)
      {
         case RIGHT_HANDED:
            // The sea is to the right-hand side of the coast as we traverse it. We are just inland, so we need to keep heading right to find the sea
            switch (nSearchDirection)
            {
               case ORIENTATION_NORTH:
                  // The sea is towards the RHS (E) of the coast, so first try to go right (to the E)
                  nXSeaward = nX+1;
                  nYSeaward = nY;
                  nSeawardNewDirection = ORIENTATION_EAST;

                  // If can't do this, try to go straight on (to the N)
                  nXStraightOn = nX;
                  nYStraightOn = nY-1;

                  // If can't do either of these, try to go anti-seaward i.e. towards the LHS (W)
                  nXAntiSeaward = nX-1;
                  nYAntiSeaward = nY;
                  nAntiSeawardNewDirection = ORIENTATION_WEST;

                  // As a last resort, go back (to the S)
                  nXGoBack = nX;
                  nYGoBack = nY+1;
                  nGoBackNewDirection = ORIENTATION_SOUTH;

                  break;

               case ORIENTATION_EAST:
                  // The sea is towards the RHS (S) of the coast, so first try to go right (to the S)
                  nXSeaward = nX;
                  nYSeaward = nY+1;
                  nSeawardNewDirection = ORIENTATION_SOUTH;

                  // If can't do this, try to go straight on (to the E)
                  nXStraightOn = nX+1;
                  nYStraightOn = nY;

                  // If can't do either of these, try to go anti-seaward i.e. towards the LHS (N)
                  nXAntiSeaward = nX;
                  nYAntiSeaward = nY-1;
                  nAntiSeawardNewDirection = ORIENTATION_NORTH;

                  // As a last resort, go back (to the W)
                  nXGoBack = nX-1;
                  nYGoBack = nY;
                  nGoBackNewDirection = ORIENTATION_WEST;

                  break;

               case ORIENTATION_SOUTH:
                  // The sea is towards the RHS (W) of the coast, so first try to go right (to the W)
                  nXSeaward = nX-1;
                  nYSeaward = nY;
                  nSeawardNewDirection = ORIENTATION_WEST;

                  // If can't do this, try to go straight on (to the S)
                  nXStraightOn = nX;
                  nYStraightOn = nY+1;

                  // If can't do either of these, try to go anti-seaward i.e. towards the LHS (E)
                  nXAntiSeaward = nX+1;
                  nYAntiSeaward = nY;
                  nAntiSeawardNewDirection = ORIENTATION_EAST;

                  // As a last resort, go back (to the N)
                  nXGoBack = nX;
                  nYGoBack = nY-1;
                  nGoBackNewDirection = ORIENTATION_NORTH;

                  break;

               case ORIENTATION_WEST :
                  // The sea is towards the RHS (N) of the coast, so first try to go right (to the N)
                  nXSeaward = nX;
                  nYSeaward = nY-1;
                  nSeawardNewDirection = ORIENTATION_NORTH;

                  // If can't do this, try to go straight on (to the W)
                  nXStraightOn = nX-1;
                  nYStraightOn = nY;

                  // If can't do either of these, try to go anti-seaward i.e. towards the LHS (S)
                  nXAntiSeaward = nX;
                  nYAntiSeaward = nY+1;
                  nAntiSeawardNewDirection = ORIENTATION_SOUTH;

                  // As a last resort, go back (to the E)
                  nXGoBack = nX+1;
                  nYGoBack = nY;
                  nGoBackNewDirection = ORIENTATION_EAST;

                  break;
            }
            break;

         case LEFT_HANDED:
            // The sea is to the left-hand side of the coast as we traverse it. We are just inland, so we need to keep heading left to find the sea
            switch (nSearchDirection)
            {
               case ORIENTATION_NORTH:
                  // The sea is towards the LHS (W) of the coast, so first try to go left (to the W)
                  nXSeaward = nX-1;
                  nYSeaward = nY;
                  nSeawardNewDirection = ORIENTATION_WEST;

                  // If can't do this, try to go straight on (to the N)
                  nXStraightOn = nX;
                  nYStraightOn = nY-1;

                  // If can't do either of these, try to go anti-seaward i.e. towards the RHS (E)
                  nXAntiSeaward = nX+1;
                  nYAntiSeaward = nY;
                  nAntiSeawardNewDirection = ORIENTATION_EAST;

                  // As a last resort, go back (to the S)
                  nXGoBack = nX;
                  nYGoBack = nY+1;
                  nGoBackNewDirection = ORIENTATION_SOUTH;

                  break;

               case ORIENTATION_EAST :
                  // The sea is towards the LHS (N) of the coast, so first try to go left (to the N)
                  nXSeaward = nX;
                  nYSeaward = nY-1;
                  nSeawardNewDirection = ORIENTATION_NORTH;

                  // If can't do this, try to go straight on (to the E)
                  nXStraightOn = nX+1;
                  nYStraightOn = nY;

                  // If can't do either of these, try to go anti-seaward i.e. towards the RHS (S)
                  nXAntiSeaward = nX;
                  nYAntiSeaward = nY+1;
                  nAntiSeawardNewDirection = ORIENTATION_SOUTH;

                  // As a last resort, go back (to the W)
                  nXGoBack = nX-1;
                  nYGoBack = nY;
                  nGoBackNewDirection = ORIENTATION_WEST;

                  break;

               case ORIENTATION_SOUTH:
                  // The sea is towards the LHS (E) of the coast, so first try to go left (to the E)
                  nXSeaward = nX+1;
                  nYSeaward = nY;
                  nSeawardNewDirection = ORIENTATION_EAST;

                  // If can't do this, try to go straight on (to the S)
                  nXStraightOn = nX;
                  nYStraightOn = nY+1;

                  // If can't do either of these, try to go anti-seaward i.e. towards the RHS (W)
                  nXAntiSeaward = nX-1;
                  nYAntiSeaward = nY;
                  nAntiSeawardNewDirection = ORIENTATION_WEST;

                  // As a last resort, go back (to the N)
                  nXGoBack = nX;
                  nYGoBack = nY-1;
                  nGoBackNewDirection = ORIENTATION_NORTH;

                  break;

               case ORIENTATION_WEST :
                  // The sea is towards the LHS (S) of the coast, so first try to go left (to the S)
                  nXSeaward = nX;
                  nYSeaward = nY+1;
                  nSeawardNewDirection = ORIENTATION_SOUTH;

                  // If can't do this, try to go straight on (to the W)
                  nXStraightOn = nX-1;
                  nYStraightOn = nY;

                  // If can't do either of these, try to go anti-seaward i.e. towards the RHS (N)
                  nXAntiSeaward = nX;
                  nYAntiSeaward = nY-1;
                  nAntiSeawardNewDirection = ORIENTATION_NORTH;

                  // As a last resort, go back (to the E)
                  nXGoBack = nX+1;
                  nYGoBack = nY;
                  nGoBackNewDirection = ORIENTATION_EAST;

                  break;
            }
            break;
      }

      // Now do the actual search for this timestep: first try going in the direction of the sea. Is this seaward cell still within the grid?
      if (bIsWithinGrid(nXSeaward, nYSeaward))
      {
         // It is, so check if the cell in the seaward direction is a sea cell
         if (m_pRasterGrid->pGetCell(nXSeaward, nYSeaward)->bIsInContiguousSea())
         {
            // There is sea in this seaward direction, so we are on the coast. Mark the current cell as a coast cell, if not already done so
            bAtCoast = true;
            if (! m_pRasterGrid->pGetCell(nX, nY)->bIsCoastline())
            {
               // Not already a coast cell, so mark the current cell as coast and add it to the vector object
               m_pRasterGrid->pGetCell(nX, nY)->SetAsCoastline(true);
               LTempGridCRS.Append(&Pti);
            }
         }
         else
         {
            // The seaward cell is not a sea cell, so we will move to it next time
            nX = nXSeaward;
            nY = nYSeaward;

            // And set a new search direction, to keep turning seaward
            nSearchDirection = nSeawardNewDirection;
            continue;
         }
      }

      // OK, we couldn't move seaward (but we may have marked the current cell as coast) so next try to move straight on. Is this straight-ahead cell still within the grid?
      if (bIsWithinGrid(nXStraightOn, nYStraightOn))
      {
         // It is, so check if there is sea immediately in front
         if (m_pRasterGrid->pGetCell(nXStraightOn, nYStraightOn)->bIsInContiguousSea())
         {
            // Sea is in front, so we are on the coast. Mark the current cell as a coast cell, if not already done so
            bAtCoast = true;
            if (! m_pRasterGrid->pGetCell(nX, nY)->bIsCoastline())
            {
               // Not already checked, so mark the current cell as coast and add it to the vector object
               m_pRasterGrid->pGetCell(nX, nY)->SetAsCoastline(true);
               LTempGridCRS.Append(&Pti);
            }
         }
         else
         {
            // The straight-ahead cell is not a sea cell, so we will move to it next time
            nX = nXStraightOn;
            nY = nYStraightOn;

            // The search direction remains unchanged
            continue;
         }
      }

      // Couldn't move either seaward or straight on (but we may have marked the current cell as coast) so next try to move in the anti-seaward direction. Is this anti-seaward cell still within the grid?
      if (bIsWithinGrid(nXAntiSeaward, nYAntiSeaward))
      {
         // It is, so check if there is sea in this anti-seaward cell
         if (m_pRasterGrid->pGetCell(nXAntiSeaward, nYAntiSeaward)->bIsInContiguousSea())
         {
            // There is sea on the anti-seaward side, so we are on the coast. Mark the current cell as a coast cell, if not already done so
            bAtCoast = true;
            if (! m_pRasterGrid->pGetCell(nX, nY)->bIsCoastline())
            {
               // Not already checked, so mark this point as coast and add it to the vector object
               m_pRasterGrid->pGetCell(nX, nY)->SetAsCoastline(true);
               LTempGridCRS.Append(&Pti);
            }
         }
         else
         {
            // The anti-seaward cell is not a sea cell, so we will move to it next time
            nX = nXAntiSeaward;
            nY = nYAntiSeaward;

            // And set a new search direction, to keep turning seaward
            nSearchDirection = nAntiSeawardNewDirection;
            continue;
         }
      }

      // Could not move to the seaward side, move straight ahead, or move to the anti-seaward side, so we must be in a single-cell dead end! As a last resort, turn round and move back to where we just came from
      nX = nXGoBack;
      nY = nYGoBack;

      // And change the search direction
      nSearchDirection = nGoBackNewDirection;
   }
   while (true);

   if (bSearchedTooLong)
   {
      // Could not find the other end of the coastline
      LogStream << WARN << m_ulTimestep << ": abandoned tracing coastline after " << nRoundTheLoop << " iterations" << endl;

      return RTN_OK;
   }

//      cout << m_ulTimestep << ": coastline = " << nThisCoast << " nRoundTheLoop = " << nRoundTheLoop << endl;

   // OK, we have finished tracing this coastline on the grid, so check to see if the coastline is too short
   int nCoastSize = LTempGridCRS.nGetSize();
   if (nCoastSize < m_nCoastMin)
   {
      // The vector coastline is unreasonably small, so abandon it
//       LogStream << m_ulTimestep << ": ignoring temporary coastline from [" << nStartX << "][" << nStartY << "] = {" << dGridCentroidXToExtCRSX(nStartX) << ", " << dGridCentroidYToExtCRSY(nStartY) << "} to [" << LTempGridCRS[nCoastSize-1].nGetX() << "][" << LTempGridCRS[nCoastSize-1].nGetY() << "] = {" << dGridCentroidXToExtCRSX(LTempGridCRS[nCoastSize-1].nGetX()) << ", " << dGridCentroidYToExtCRSY(LTempGridCRS[nCoastSize-1].nGetY()) << "} since size (" << nCoastSize << ") is less than minimum (" << m_nCoastMin << ")" << endl;

      // Unmark these cells
      for (int n = 0; n < nCoastSize; n++)
         m_pRasterGrid->pGetCell(LTempGridCRS[n].nGetX(), LTempGridCRS[n].nGetY())->SetAsCoastline(false);

      return RTN_OK;
   }

   int nEndX = nX;
   int nEndY = nY;
   if (! ((LTempGridCRS[nCoastSize-1].nGetX() == nEndX) && (LTempGridCRS[nCoastSize-1].nGetY()) == nEndY))
   {
      // Not already at end of LTempGridCRS, so add it and mark the cell as coastline
      LTempGridCRS.Append(nEndX, nEndY);
      nCoastSize++;

      m_pRasterGrid->pGetCell(nEndX, nEndY)->SetAsCoastline(true);
   }

   // Need to specify start edge and end edge for smoothing routines
   int
      nStartEdge,
      nEndEdge;

   if (nStartX == 0)
      nStartEdge = ORIENTATION_WEST;
   else if (nStartX == m_nXGridMax-1)
      nStartEdge = ORIENTATION_EAST;
   else if (nStartY == 0)
      nStartEdge = ORIENTATION_NORTH;
   else if (nStartY == m_nYGridMax-1)
      nStartEdge = ORIENTATION_SOUTH;

   if (nEndX == 0)
      nEndEdge = ORIENTATION_WEST;
   else if (nEndX == m_nXGridMax-1)
      nEndEdge = ORIENTATION_EAST;
   else if (nEndY == 0)
      nEndEdge = ORIENTATION_NORTH;
   else if (nEndY == m_nYGridMax-1)
      nEndEdge = ORIENTATION_SOUTH;

   // Next, convert the grid coordinates in LTempGridCRS (integer values stored as doubles) to external CRS coordinates (which will probably be non-integer, again stored as doubles). This is done now, so that smoothing is more effective
   CLine LTempExtCRS;
   for (int j = 0; j < nCoastSize; j++)
      LTempExtCRS.Append(dGridCentroidXToExtCRSX(LTempGridCRS[j].nGetX()), dGridCentroidYToExtCRSY(LTempGridCRS[j].nGetY()));

   // Now do some smoothing of the vector output, if desired
   if (m_nCoastSmooth == SMOOTH_RUNNING_MEAN)
      LTempExtCRS = LSmoothCoastRunningMean(&LTempExtCRS, nStartEdge, nEndEdge);
   else if (m_nCoastSmooth == SMOOTH_SAVITZKY_GOLAY)
      LTempExtCRS = LSmoothCoastSavitzkyGolay(&LTempExtCRS, nStartEdge, nEndEdge);

   // Create a new coastline object and append to it the vector of coastline objects
   CCoast CoastTmp;
   m_VCoast.push_back(CoastTmp);
   int nCoast = m_VCoast.size()-1;

   C2DPoint PtLast(DBL_MIN, DBL_MIN);
   for (int j = 0; j < nCoastSize; j++)
   {
      // Store the smoothed points (in external CRS) in the coast's m_LCoastline object, also append dummy values to the other attribute vectors
      if (PtLast != &LTempExtCRS[j])        // Avoid duplicate points
      {
         m_VCoast[nCoast].AppendToCoastline(LTempExtCRS[j].dGetX(), LTempExtCRS[j].dGetY());

         // Also store the locations of the corresponding unsmoothed points (in raster-grid CRS) in the coast's m_VCellsMarkedAsCoastline vector
         m_VCoast[nCoast].AppendCellMarkedAsCoastline(&LTempGridCRS[j]);
      }

      PtLast = LTempExtCRS[j];
   }

   // Next, set values for the coast's other attributes. First set the coast's handedness, and start and end edges
   m_VCoast[nCoast].SetSeaHandedness(nHandedness);
   m_VCoast[nCoast].SetStartEdge(nStartEdge);
   m_VCoast[nCoast].SetEndEdge(nEndEdge);


   LogStream << "=====================================================================================================" << endl;
   LogStream << "At timestep " << m_ulTimestep << ": coastline " << nCoast << " created, from [" << nStartX << "][" << nStartY << "] to [" << nEndX << "][" << nEndY << "] = {" << dGridCentroidXToExtCRSX(nStartX) << ", " << dGridCentroidYToExtCRSY(nStartY) << "} to {" << dGridCentroidXToExtCRSX(nEndX) << ", " << dGridCentroidYToExtCRSY(nEndY) << "} with " << nCoastSize << " points, handedness = " << (nHandedness == LEFT_HANDED ? "left" : "right") << "." << endl;
   LogStream << "Smoothed coastline " << nCoast << " runs from {" << LTempExtCRS[0].dGetX() << ", " << LTempExtCRS[0].dGetY() << "} to {" << LTempExtCRS[nCoastSize-1].dGetX() << ", " << LTempExtCRS[nCoastSize-1].dGetY() << "}" << endl;
//       LogStream << "-----------------" << endl;
//       for (unsigned int kk = 0; kk < nCoastSize; kk++)
//          LogStream << "[" << LTempGridCRS[kk].nGetX() << "][" << LTempGridCRS[kk].nGetY() << "]" << endl;
//       LogStream << "-----------------" << endl;
   LogStream << endl;
   LogStream.flush();

   // Next calculate the curvature of the vector coastline
   DoCoastCurvature(nCoast, nHandedness);


   return RTN_OK;
}

