/*!
 *
 * \file smooth_line.cpp
 * \brief Smoothing routines for CLine objects
 * \details The Savitzky-Golay routines are modified from C originals by Jean-Pierre Moreau (jpmoreau@wanadoo.fr, http://jean-pierre.moreau.pagesperso-orange.fr/index.html), to whom we are much indebted
 * \author Andres Payo, David Favis-Mortlock, Martin Husrt, Monica Palaseanu-Lovejoy
 * \date 2020
 * \copyright GNU General Public License
 *
 */

/*==============================================================================================================================

 This file is part of CliffMetrics, the Coastal Modelling Environment.

 CliffMetrics is free software; you can redistribute it and/or modify it under the terms of the GNU General Public  License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

==============================================================================================================================*/
#include <cmath>
using std::abs;

#include <iostream>
using std::cout;
using std::cerr;
using std::endl;
using std::ios;

#include "cliffmetrics.h"
#include "delineation.h"


// Visible throughout this file. Note that arrays here are used from index 1
typedef double Matrix[SAVGOLPOLYMAX + 2][SAVGOLPOLYMAX + 2];

void LUDecomp(Matrix, int const, int const, int[], int*, int*);
void LULinearSolve(Matrix const, int const, int const[], double[]);


/*==============================================================================================================================

 Calculates the Savitzky-Golay smoothing coefficients for a given size of smoothing window

 This and associated Savitzky-Golay routines have been derived from C originals by Jean-Pierre Moreau (jpmoreau@wanadoo.fr, http://jean-pierre.moreau.pagesperso-orange.fr/index.html), to whom we are much indebted

==============================================================================================================================*/
void CDelineation::CalcSavitzkyGolayCoeffs(void)
{
   m_VnSavGolIndexCoast.resize(m_nCoastSmoothWindow+1, 0);

   // Note that m_nCoastSmoothWindow must be odd (have already checked this)
   int nHalfWindow = m_nCoastSmoothWindow / 2;

   // Calculate the shift index for this value of nHalfWindow
   int j = 3;
   for (int i = 2; i <= nHalfWindow+1; i++)
   {
      m_VnSavGolIndexCoast[i] = i - j;
      j += 2;
   }

   j = 2;
   for (int i = nHalfWindow+2; i <= m_nCoastSmoothWindow; i++)
   {
      m_VnSavGolIndexCoast[i] = i - j;
      j += 2;
   }

   // Now calculate the Savitzky-Golay filter coefficients
   m_VdSavGolFCCoast.resize(m_nCoastSmoothWindow+1, 0);

   CalcSavitzkyGolay(&(m_VdSavGolFCCoast.at(0)), m_nCoastSmoothWindow, nHalfWindow, nHalfWindow, 0, m_nSavGolCoastPoly);
}


/*==============================================================================================================================

 Does smoothing of a CLine coastline vector using a Savitzky-Golay filter

==============================================================================================================================*/
CLine CDelineation::LSmoothCoastSavitzkyGolay(CLine* pLineIn, int const nStartEdge, int const nEndEdge) const
{
   // Note that m_nCoastSmoothWindow must be odd (have already checked this)
   int nHalfWindow = m_nCoastSmoothWindow / 2;

   // Make a copy of the unsmoothed CLine (must be blank)
   int nSize = pLineIn->nGetSize();
   CLine LTemp;
   LTemp.Resize(nSize);

   // Apply the Savitzky-Golay smoothing filter
   for (int i = 0; i < nSize; i++)
   {
      if (i < nHalfWindow)
      {
         // For the first few values of LTemp, just apply a running mean with a variable-sized window
         int nTmpWindow = 0;
         double dWindowTotX = 0, dWindowTotY = 0;
         for (int j = -nHalfWindow; j < m_nCoastSmoothWindow - nHalfWindow; j++)
         {
            int k = i+j;

            if ((k > 0) && (k < nSize))
            {
               dWindowTotX += pLineIn->dGetXAt(k);
               dWindowTotY += pLineIn->dGetYAt(k);
               nTmpWindow++;
            }
         }

         switch (nStartEdge)
         {
            case ORIENTATION_NORTH:
            case ORIENTATION_SOUTH:
               // Don't apply the filter in the Y direction
               LTemp.SetXAt(i, dWindowTotX / static_cast<double>(nTmpWindow));
               LTemp.SetYAt(i, pLineIn->dGetYAt(i));
               break;

            case ORIENTATION_EAST:
            case ORIENTATION_WEST:
               // Don't apply the filter in the X direction
               LTemp.SetXAt(i, pLineIn->dGetXAt(i));
               LTemp.SetYAt(i, dWindowTotY / static_cast<double>(nTmpWindow));
               break;
         }
      }
      else if (i >= (nSize - nHalfWindow))
      {
         // For the last few values of PtVTemp, just apply a running mean with a variable-sized window
         int nTmpWindow = 0;
         double dWindowTotX = 0, dWindowTotY = 0;
         for (int j = -nHalfWindow; j < m_nCoastSmoothWindow - nHalfWindow; j++)
         {
            int k = i+j;

            if ((k > 0) && (k < nSize))
            {
               dWindowTotX += pLineIn->dGetXAt(k);
               dWindowTotY += pLineIn->dGetYAt(k);
               nTmpWindow++;
            }
         }

         switch (nEndEdge)
         {
            case ORIENTATION_NORTH:
            case ORIENTATION_SOUTH:
               // Don't apply the filter in the Y direction
               LTemp.SetXAt(i, dWindowTotX / static_cast<double>(nTmpWindow));
               LTemp.SetYAt(i, pLineIn->dGetYAt(i));
               break;

            case ORIENTATION_EAST:
            case ORIENTATION_WEST:
               // Don't apply the filter in the X direction
               LTemp.SetXAt(i, pLineIn->dGetXAt(i));
               LTemp.SetYAt(i, dWindowTotY / static_cast<double>(nTmpWindow));
               break;
         }
      }
      else
      {
         // For all other PtVTemp values, calc Savitzky-Golay weighted values for both X and Y
         for (int j = 0; j < m_nCoastSmoothWindow; j++)
         {
            int k = i + m_VnSavGolIndexCoast[j+1];
            if ((k >= 0) && (k < nSize))        // Skip points that do not exist, note starts from 1
            {
               double dX = LTemp.dGetXAt(i);
               dX += m_VdSavGolFCCoast[j+1] * pLineIn->dGetXAt(k);
               LTemp.SetXAt(i, dX);

               double dY = LTemp.dGetYAt(i);
               dY += m_VdSavGolFCCoast[j+1] * pLineIn->dGetYAt(k);
               LTemp.SetYAt(i, dY);
            }
         }
      }
   }

   // Return the smoothed CLine
   return LTemp;
}


/*==============================================================================================================================

 Does running-mean smoothing of a CLine coastline vector

==============================================================================================================================*/
CLine CDelineation::LSmoothCoastRunningMean(CLine* pLineIn, int const nStartEdge, int const nEndEdge) const
{
   // Note that m_nCoastSmoothWindow must be odd (have already checked this)
   int nHalfWindow = m_nCoastSmoothWindow / 2;

   // Make a copy of the unsmoothed CLine
   int nSize = pLineIn->nGetSize();
   CLine LTemp;
   LTemp = *pLineIn;

   // Apply the running mean smoothing filter, with a variable window size at both ends of the line
   for (int i = 0; i < nSize; i++)
   {
      bool bNearStartEdge = false, bNearEndEdge = false;
      int nTmpWindow = 0;
      double dWindowTotX = 0, dWindowTotY = 0;
      for (int j = -nHalfWindow; j < m_nCoastSmoothWindow - nHalfWindow; j++)
      {
         // For points at both ends of the coastline, use a smaller window
         int k = i+j;

         if (k < 0)
         {
            bNearStartEdge = true;
            continue;
         }

         if (k >= nSize)
         {
            bNearEndEdge = true;
            continue;
         }

         dWindowTotX += pLineIn->dGetXAt(k);
         dWindowTotY += pLineIn->dGetYAt(k);
         nTmpWindow++;
      }

      if (bNearStartEdge)
      {
         // We are near the start edge
         switch (nStartEdge)
         {
            case ORIENTATION_NORTH:
            case ORIENTATION_SOUTH:
               // Don't apply the filter in the y direction
               LTemp.SetXAt(i, dWindowTotX / static_cast<double>(nTmpWindow));
               break;

            case ORIENTATION_EAST:
            case ORIENTATION_WEST:
               // Don't apply the filter in the x direction
               LTemp.SetYAt(i, dWindowTotY / static_cast<double>(nTmpWindow));
               break;
         }
      }
      else if (bNearEndEdge)
      {
         // We are near the end edge
         switch (nEndEdge)
         {
            case ORIENTATION_NORTH:
            case ORIENTATION_SOUTH:
               // Don't apply the filter in the y direction
               LTemp.SetXAt(i, dWindowTotX / static_cast<double>(nTmpWindow));
               break;

            case ORIENTATION_EAST:
            case ORIENTATION_WEST:
               // Don't apply the filter in the x direction
               LTemp.SetYAt(i, dWindowTotY / static_cast<double>(nTmpWindow));
               break;
         }
      }
      else
      {
         // Not near any edge, apply both x and y filters
         LTemp.SetXAt(i, dWindowTotX / static_cast<double>(nTmpWindow));
         LTemp.SetYAt(i, dWindowTotY / static_cast<double>(nTmpWindow));
      }
   }

   // Return the smoothed CLine
   return LTemp;
}


/*==============================================================================================================================

Does running-mean smoothing of the slope of a coastline-normal profile

==============================================================================================================================*/
vector<double> CDelineation::dVSmoothProfileSlope(vector<double>* pdVSlope)
{
   // Note that m_nProfileSmoothWindow must be odd (have already checked this)
   int const nHalfWindow = m_nProfileSmoothWindow / 2;

   // Make a copy of the unsmoothed profile slope vector
   int const nSize = pdVSlope->size();
   vector<double> dVSmoothed = *pdVSlope;

   // Apply the running mean smoothing filter, with a variable window size at both ends of the line
   for (int i = 0; i < nSize; i++)
   {
      int nTmpWindow = 0;
      double dWindowTot = 0;
      for (int j = -nHalfWindow; j < m_nCoastSmoothWindow - nHalfWindow; j++)
      {
         // For points at both ends of the profile, use a smaller window
         int const k = i+j;

         if ((k < 0) || (k >= nSize))
            continue;

         dWindowTot += pdVSlope->at(k);
         nTmpWindow++;
      }

      dVSmoothed[i] = dWindowTot / static_cast<double>(nTmpWindow);

      //  If necessary, constrain the slope as in SCAPE
      if (dVSmoothed[i] >= 0)
         dVSmoothed[i] = tMin(dVSmoothed[i], m_dProfileMaxSlope);
      else
         dVSmoothed[i] = tMax(dVSmoothed[i], -m_dProfileMaxSlope);
   }

   // Return the smoothed vector
   return dVSmoothed;
}


/*==============================================================================================================================

 CalcSavitzkyGolay uses LULinearSolve and LUDecomp. It returns dFilterCoeffsArray[nWindowSize], a set of Savitzky-Golay filter coefficients in wrap-around order. nLeft is the number of leftward data points used, nRight is the number of rightward data points, making the total number of data points used = nLeft + nRight + 1. nDerivOrder is the order of the derivative desired (e.g. nDerivOrder = 0 for smoothed function), nSmoothPolyOrder is the order of the smoothing polynomial also equal to the highest conserved moment; usual values are nSmoothPolyOrder = 2 or nSmoothPolyOrder = 4

==============================================================================================================================*/
void CDelineation::CalcSavitzkyGolay(double dFilterCoeffsArray[], int const nWindowSize, int const nLeft, int const nRight, int const nDerivOrder, int const nSmoothPolyOrder)
{
   // Some sanity checks
   if ((nWindowSize < nLeft + nRight + 1) || (nLeft < 0) || (nRight < 0) || (nDerivOrder > nSmoothPolyOrder)  || (nSmoothPolyOrder  > static_cast<int>(SAVGOLPOLYMAX)) || (nLeft + nRight < nSmoothPolyOrder))
   {
      cerr << ERR << "Error in arguments to CalcSavitzkyGolay" << endl;
      return;
   }

   // Initialise arrays (including index 0 for tidiness)
   int nIndexArray[SAVGOLPOLYMAX+2];
   Matrix dMatrix;
   double dSolutionArray[SAVGOLPOLYMAX+2];
   for (int i = 0; i <= SAVGOLPOLYMAX+1; i++)
   {
      for (int j = 0; j <= SAVGOLPOLYMAX+1; j++)
         dMatrix[i][j] = 0;
      dSolutionArray[i] = 0;
      nIndexArray[i] = 0;
   }

   for (int ipj = 0; ipj <= 2 * nSmoothPolyOrder ; ipj++)
   {
      // Set up the equations for the desired least squares fit
      double dSum = 0;
      if (ipj == 0)
         dSum = 1;
      for (int k = 1; k <= nRight; k++)
         dSum += pow(k, ipj);
      for (int k = 1; k <= nLeft; k++)
         dSum += pow(-k, ipj);
      int mm = tMin(ipj, 2 * nSmoothPolyOrder  - ipj);
      int imj = -mm;
      do
      {
         dMatrix[1 + (ipj + imj) / 2][1 + (ipj - imj) / 2] = dSum;
         imj += 2;
      }
      while (imj <= mm);
   }

   // Solve them using LU decomposition
   int nDCode = 0, nICode = 0;
   LUDecomp(dMatrix, nSmoothPolyOrder+1, SAVGOLPOLYMAX+1, nIndexArray, &nDCode, &nICode);

   // Right-hand side vector is unit vector, depending on which derivative we want
   dSolutionArray[nDerivOrder+1] = 1;

   // Backsubstitute, giving one row of the inverse matrix
   LULinearSolve(dMatrix, nSmoothPolyOrder +1, nIndexArray, dSolutionArray);

   // Zero the output array (it may be bigger than the number of coefficients). Again include index 0 for tidiness
//   for (int n = 0; n < SAVGOLPOLYMAX+1; n++)
//   for (int n = 0; n <= nWindowSize; n++)
//      dFilterCoeffsArray[n] = 0;

   for (int n = -nLeft; n <= nRight; n++)
   {
      // Each Savitzky-Golay coefficient is the dot product of powers of an integer with the inverse matrix row
      double dSum = dSolutionArray[1];
      double dFac = 1;
      for (int m = 1; m <= nSmoothPolyOrder ; m++)
      {
         dFac *= n;
         dSum += dSolutionArray[m+1] * dFac;
      }

      // Store in wrap-around order
      int nInd = ((nWindowSize - n) % nWindowSize) + 1;
      dFilterCoeffsArray[nInd] = dSum;
   }
}


/*==============================================================================================================================

 Given an N x N matrix A, this routine replaces it by the LU decomposition of a rowwise permutation of itself. A and N are input. nIndexArray is an output vector which records the row permutation effected by the partial pivoting; D is output as -1 or 1, depending on whether the number of row interchanges was even or odd, respectively. This routine is used in combination with LULinearSolve to solve linear equations or to invert a matrix. Returns with nICode = 1 if matrix is singular

==============================================================================================================================*/
void LUDecomp(Matrix A, int const N, int const np, int nIndexArray[], int* nDCode, int* nICode)
{
   if (N >= np)
   {
      cerr << ERR << "in LUDecomp" << endl;
      return;
   }

   double TINY = 1e-12;
   double AMAX, DUM, SUM;
   double* VV = new double[np];
   int   I, IMAX = 0, J, K;

   *nDCode = 1;
   *nICode = 0;

   for (I = 1; I <= N; I++)
   {
      AMAX = 0.0;
      for (J = 1; J <= N; J++)
         if (tAbs(A[I][J]) > AMAX)
            AMAX = tAbs(A[I][J]);

      if (AMAX < TINY)
      {
         *nICode = 1;
         delete[] VV;
         return;
      }

      VV[I] = 1.0 / AMAX;
   }

   for (J = 1; J <= N; J++)
   {
      for (I = 1; I < J; I++)
      {
         SUM = A[I][J];
         for (K = 1; K < I; K++)
            SUM -= A[I][K] * A[K][J];
         A[I][J] = SUM;
      }

      AMAX = 0.0;
      for (I = J; I <= N; I++)
      {
         SUM = A[I][J];
         for (K = 1; K < J; K++)
            SUM -= A[I][K] * A[K][J];

         A[I][J] = SUM;
         DUM = VV[I] * tAbs(SUM);
         if (DUM >= AMAX)
         {
            IMAX = I;
            AMAX = DUM;
         }
      }

      if (J != IMAX)
      {
         for (K = 1; K <= N; K++)
         {
            DUM = A[IMAX][K];
            A[IMAX][K] = A[J][K];
            A[J][K] = DUM;
         }

         *nDCode = -(*nDCode);
         VV[IMAX] = VV[J];
      }

      nIndexArray[J] = IMAX;
      if (tAbs(A[J][J]) < TINY)
         A[J][J] = TINY;

      if (J != N)
      {
         DUM = 1.0 / A[J][J];
         for (I = J + 1; I <= N; I++)
            A[I][J] *= DUM;
      }
   }

   delete[] VV;
}


/*==============================================================================================================================

 Solves the set of N linear equations A . X = B.  Here A is input, not as the matrix A but rather as its LU decomposition, determined by the routine LUDecomp. nIndexArray is input as the permutation vector returned by LUDecomp. B is input as the right-hand side vector B, and returns with the solution vector X. A, N and nIndexArray are not modified by this routine and can be used for successive calls with different right-hand sides. This routine is also efficient for plain matrix inversion

==============================================================================================================================*/
void LULinearSolve(Matrix const A, int const N, int const nIndexArray[], double B[])
{
   int II = 0;
   double SUM = 0;

   for (int I = 1; I <= N; I++)
   {
      int LL = nIndexArray[I];
      SUM = B[LL];
      B[LL] = B[I];
      if (II != 0)
         for (int J = II; J < I; J++)
            SUM -= A[I][J] * B[J];
      else if (SUM != 0.0)
         II = I;
      B[I] = SUM;
   }

   for (int I = N; I > 0; I--)
   {
      SUM = B[I];
      if (I < N)
         for (int J = I+1; J <= N; J++)
            SUM -= A[I][J] * B[J];
      B[I] = SUM / A[I][I];
   }
}
