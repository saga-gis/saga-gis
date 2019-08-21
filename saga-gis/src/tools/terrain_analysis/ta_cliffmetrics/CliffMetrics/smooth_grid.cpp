/*!
 *
 * \file smooth_grid.cpp
 * \brief Smoothing routines for CLine
 * \details The Savitzky-Golay routines are modified from C originals by Jean-Pierre Moreau (jpmoreau@wanadoo.fr, http://jean-pierre.moreau.pagesperso-orange.fr/index.html), to whom we are much indebted
 * \author David Favis-Mortlock
 * \author Andres Payo
 * \author Jim Hall
 * \date 2017
 * \copyright GNU General Public License
 *
 */

/*==============================================================================================================================

 This file is part of CliffMetrics, the Coastal Modelling Environment.

 CliffMetrics is free software; you can redistribute it and/or modify it under the terms of the GNU General Public  License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

==============================================================================================================================*/
#include "cliffmetrics.h"
#include "delineation.h"
#include "raster_grid.h"

#include <iostream>
using std::endl;


/*==============================================================================================================================

 Smooths the raster grid (only those sediment layers that have changed this timestep, however)

==============================================================================================================================*/
// void CDelineation::SmoothGrid(void)
// {
//    bool bChanged = false;
//    for (int nLayer = 0; nLayer < m_nLayers; nLayer++)
//    {
//       if (m_bConsChangedThisTimestep[nLayer])
//       {
//          bChanged = true;
//
//          // Consolidated
//          SmoothLayer(nLayer, true);
//       }
//
//       if (m_bUnconsChangedThisTimestep[nLayer])
//       {
//          bChanged = true;
//
//          // Unconsolidated
//          SmoothLayer(nLayer, false);
//       }
//    }
//
//    if (bChanged)
//    {
//       for (int nX = 0; nX < m_nXGridMax; nX++)
//       {
//          for (int nY = 0; nY < m_nYGridMax; nY++)
//          {
//             // Re-calc all layer elevations
//             m_pRasterGrid->pGetCell(nX, nY)->CalcAllLayerElevs();
//
//             // And update the cell's sea depth
//             m_pRasterGrid->pGetCell(nX, nY)->SetSeaDepth();
//          }
//       }
//    }
// }


/*==============================================================================================================================

 Smooths all three textures, either consolidated or unconsolidated, of a single layer of the raster grid

==============================================================================================================================*/
// void CDelineation::SmoothLayer(int const nLayer, bool const bCons)
// {
//    for (int nTexture = TEXTURE_FINE; nTexture <= TEXTURE_COARSE; nTexture++)
//    {
//       int nTot = 0;
//       double
//          dOrigAvg = 0,
//          dSmoothAvg = 0;
//
//       for (int nX = 0; nX < m_nXGridMax; nX++)
//       {
//          for (int nY = 0; nY < m_nYGridMax; nY++)
//          {
//             // Initialise
//             m_pRasterGrid->pGetCell(nX, nY)->SetTmpWork(0);
//
//             if (m_pRasterGrid->pGetCell(nX, nY)->bIsSeaIncBeach())
//             {
//                // This is a sea or beach cell, so calculate the smoothed layer thickness and get the original layer thickness
//                double dOrigThick;
//                double dSmoothThick = dSimpleAvgAdjCellThickness(nX, nY, nLayer, bCons, nTexture, dOrigThick);
//
//                if (dOrigThick > 0)
//                {
//                   nTot++;
//                   dOrigAvg += dOrigThick;
//                   dSmoothAvg += dSmoothThick;
//
//                   m_pRasterGrid->pGetCell(nX, nY)->SetTmpWork(dSmoothThick);
//                }
//             }
//          }
//       }
//
//       // Any sea cells with non-zero thickness?
//       if (nTot == 0)
//          // No, so move on to the next texture
//          continue;
//
//       // Calculate the pre-smoothing and post-smoothing average thicknesses
//       dOrigAvg /= nTot;
//       dSmoothAvg /= nTot;
//
//       // Are the two averages identical?
//       if (! bFPIsEqual(dOrigAvg, dSmoothAvg, TOLERANCE))
//       {
//          // They are not equal, so need to adjust all smoothed thicknesses to make them equal
//          double dDiff = dOrigAvg - dSmoothAvg;
//
//          for (int nX = 0; nX < m_nXGridMax; nX++)
//          {
//             for (int nY = 0; nY < m_nYGridMax; nY++)
//             {
//                if (m_pRasterGrid->pGetCell(nX, nY)->dGetTmpWork() > 0)
//                   m_pRasterGrid->pGetCell(nX, nY)->IncrTmpWork(dDiff);
//             }
//          }
//       }
//
//       // Set the layer thickness to be the smoothed value
//       int nNewTot = 0;
//       double dNewValAvg = 0;
//       for (int nX = 0; nX < m_nXGridMax; nX++)
//       {
//          for (int nY = 0; nY < m_nYGridMax; nY++)
//          {
//             double dNewVal = m_pRasterGrid->pGetCell(nX, nY)->dGetTmpWork();
//
//             if (dNewVal > 0)
//             {
//                nNewTot++;
//                dNewValAvg += dNewVal;
// //               LogStream << "[" << nX << "][" << nY << "] " << dNewVal << endl;
//
//                switch (nTexture)
//                {
//                case TEXTURE_FINE:
//                   if (bCons)
//                      m_pRasterGrid->pGetCell(nX, nY)->pGetLayer(nLayer)->pGetConsolidatedSediment()->SetFine(dNewVal);
//                   else
//                      m_pRasterGrid->pGetCell(nX, nY)->pGetLayer(nLayer)->pGetUnconsolidatedSediment()->SetFine(dNewVal);
//                   break;
//
//                case TEXTURE_SAND:
//                   if (bCons)
//                      m_pRasterGrid->pGetCell(nX, nY)->pGetLayer(nLayer)->pGetConsolidatedSediment()->SetSand(dNewVal);
//                   else
//                      m_pRasterGrid->pGetCell(nX, nY)->pGetLayer(nLayer)->pGetUnconsolidatedSediment()->SetSand(dNewVal);
//                   break;
//
//                case TEXTURE_COARSE:
//                   if (bCons)
//                      m_pRasterGrid->pGetCell(nX, nY)->pGetLayer(nLayer)->pGetConsolidatedSediment()->SetCoarse(dNewVal);
//                   else
//                      m_pRasterGrid->pGetCell(nX, nY)->pGetLayer(nLayer)->pGetUnconsolidatedSediment()->SetCoarse(dNewVal);
//                   break;
//                }
//             }
//          }
//       }
//
//       if (nNewTot > 0)
//          dNewValAvg /= nNewTot;
//
//       LogStream << endl << m_ulTimestep << ": smoothing layer " << nLayer << " " << (bCons ? "consolidated" : "unconsolidated") << " ";
//       switch (nTexture)
//       {
//       case TEXTURE_FINE:
//          LogStream << "fine";
//          break;
//
//       case TEXTURE_SAND:
//          LogStream << "sand";
//          break;
//
//       case TEXTURE_COARSE:
//          LogStream << "coarse";
//          break;
//       }
//       LogStream << " sediment, nTot = " << nTot << ", dOrigAvg = " << dOrigAvg << ", dSmoothAvg = " << dSmoothAvg << ", nNewTot = " << nNewTot << ", dNewValAvg = " << dNewValAvg << endl;
//    }
// }


/*==============================================================================================================================

 Calculates a simple weighted average of the elevations of this cell and up to eight adjacent cells

==============================================================================================================================*/
// double CDelineation::dSimpleAvgAdjCellThickness(int const nX, int const nY, int const nLayer, bool const bCons, int const nTexture, double& dOrigThick)
// {
//    switch (nTexture)
//    {
//    case TEXTURE_FINE:
//       if (bCons)
//          dOrigThick = m_pRasterGrid->pGetCell(nX, nY)->pGetLayer(nLayer)->pGetConsolidatedSediment()->dGetFine();
//       else
//          dOrigThick = m_pRasterGrid->pGetCell(nX, nY)->pGetLayer(nLayer)->pGetUnconsolidatedSediment()->dGetFine();
//       break;
//
//    case TEXTURE_SAND:
//       if (bCons)
//          dOrigThick = m_pRasterGrid->pGetCell(nX, nY)->pGetLayer(nLayer)->pGetConsolidatedSediment()->dGetSand();
//       else
//          dOrigThick = m_pRasterGrid->pGetCell(nX, nY)->pGetLayer(nLayer)->pGetUnconsolidatedSediment()->dGetSand();
//       break;
//
//    case TEXTURE_COARSE:
//       if (bCons)
//          dOrigThick = m_pRasterGrid->pGetCell(nX, nY)->pGetLayer(nLayer)->pGetConsolidatedSediment()->dGetCoarse();
//       else
//          dOrigThick = m_pRasterGrid->pGetCell(nX, nY)->pGetLayer(nLayer)->pGetUnconsolidatedSediment()->dGetCoarse();
//       break;
//    }
//
//    int
//       nXTmp,
//       nYTmp;
//    double
//       dAdjTot = dOrigThick,
//       dWeight,
//       dTotWeight = 1;
//
//    for (int nDir = 0; nDir < 8; nDir++)
//    {
//       // The origin of the grid is to the top left
//       switch (nDir)
//       {
//       case 0:
//          // West
//          nXTmp = nX-1;
//          nYTmp = nY;
//          dWeight = m_dInvCellSide * m_dSimpleSmoothWeight;
//          break;
//
//       case 1:
//          // North-West
//          nXTmp = nX-1;
//          nYTmp = nY-1;
//          dWeight = m_dInvCellDiagonal * m_dSimpleSmoothWeight;
//          break;
//
//       case 2:
//          // North
//          nXTmp = nX;
//          nYTmp = nY-1;
//          dWeight = m_dInvCellSide * m_dSimpleSmoothWeight;
//          break;
//
//       case 3:
//          // North-East
//          nXTmp = nX+1;
//          nYTmp = nY-1;
//          dWeight = m_dInvCellDiagonal * m_dSimpleSmoothWeight;
//          break;
//
//       case 4:
//          // East
//          nXTmp = nX+1;
//          nYTmp = nY;
//          dWeight = m_dInvCellSide * m_dSimpleSmoothWeight;
//          break;
//
//       case 5:
//          // South-East
//          nXTmp = nX+1;
//          nYTmp = nY+1;
//          dWeight = m_dInvCellDiagonal * m_dSimpleSmoothWeight;
//          break;
//
//       case 6:
//          // South
//          nXTmp = nX;
//          nYTmp = nY+1;
//          dWeight = m_dInvCellSide * m_dSimpleSmoothWeight;
//          break;
//
//       case 7:
//          // South-West
//          nXTmp = nX-1;
//          nYTmp = nY+1;
//          dWeight = m_dInvCellDiagonal * m_dSimpleSmoothWeight;
//          break;
//       }
//
//       if ((bIsWithinGrid(nXTmp, nYTmp)) && (m_pRasterGrid->pGetCell(nXTmp, nYTmp)->bIsSeaIncBeach()))
//       {
//          // This is a sea or beach cell
//          dTotWeight += dWeight;
//
//          switch (nTexture)
//          {
//          case TEXTURE_FINE:
//             if (bCons)
//                dAdjTot += (m_pRasterGrid->pGetCell(nXTmp, nYTmp)->pGetLayer(nLayer)->pGetConsolidatedSediment()->dGetFine() * dWeight);
//             else
//                dAdjTot += (m_pRasterGrid->pGetCell(nXTmp, nYTmp)->pGetLayer(nLayer)->pGetUnconsolidatedSediment()->dGetFine() * dWeight);
//             break;
//
//          case TEXTURE_SAND:
//             if (bCons)
//                dAdjTot += (m_pRasterGrid->pGetCell(nXTmp, nYTmp)->pGetLayer(nLayer)->pGetConsolidatedSediment()->dGetSand() * dWeight);
//             else
//                dAdjTot += (m_pRasterGrid->pGetCell(nXTmp, nYTmp)->pGetLayer(nLayer)->pGetUnconsolidatedSediment()->dGetSand() * dWeight);
//             break;
//
//          case TEXTURE_COARSE:
//             if (bCons)
//                dAdjTot += (m_pRasterGrid->pGetCell(nXTmp, nYTmp)->pGetLayer(nLayer)->pGetConsolidatedSediment()->dGetCoarse() * dWeight);
//             else
//                dAdjTot += (m_pRasterGrid->pGetCell(nXTmp, nYTmp)->pGetLayer(nLayer)->pGetUnconsolidatedSediment()->dGetCoarse() * dWeight);
//             break;
//          }
//       }
//    }
//
//    double dSimpleAvg = dAdjTot / dTotWeight;
//
//    return dSimpleAvg;
// }


