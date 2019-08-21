/*!
 *
 * \file 2d_shape.cpp
 * \brief C2DShape routines
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
#include "2d_shape.h"


C2DShape::C2DShape(void)
{
}

C2DShape::~C2DShape(void)
{
}

C2DPoint& C2DShape::operator[] (int const n)
{
   // NOTE No safety check
   return m_VPoints[n];
}

void C2DShape::Clear(void)
{
   m_VPoints.clear();
}

void C2DShape::Resize(int const nSize)
{
   m_VPoints.resize(nSize);
}

// void C2DShape::InsertAtFront(double const dX, double const dY)
// {
//    m_VPoints.insert(m_VPoints.begin(), C2DPoint(dX, dY));
// }

void C2DShape::Append(const C2DPoint* PtNew)
{
   m_VPoints.push_back(*PtNew);
}

void C2DShape::Append(double const dX, double const dY)
{
   m_VPoints.push_back(C2DPoint(dX, dY));
}

int C2DShape::nGetSize(void) const
{
   return m_VPoints.size();
}

// void C2DShape::SetPoints(const vector<C2DPoint>* VNewPoints)
// {
//    m_VPoints = *VNewPoints;
// }

// int C2DShape::nLookUp(C2DPoint* Pt)
// {
//    vector<C2DPoint>::iterator it = std::find(m_VPoints.begin(), m_VPoints.end(), *Pt);
//    if (it != m_VPoints.end())
//       return it - m_VPoints.begin();
//    else
//       return -1;
// }

// double C2DShape::dGetLength(void) const
// {
//    int nSize = m_VPoints.size();
//
//    if (nSize < 2)
//       return -1;
//
//    double dLength = 0;
//    for (int n = 1; n < nSize; n++)
//    {
//       double dXlen = m_VPoints[n].dGetX() - m_VPoints[n-1].dGetX();
//       double dYlen = m_VPoints[n].dGetY() - m_VPoints[n-1].dGetY();
//
//       dLength += hypot(dXlen, dYlen);
//    }
//
//    return dLength;
// }


vector<C2DPoint>* C2DShape::pPtVGetPoints(void)
{
   return &m_VPoints;
}


//! Computes the centroid of this 2D polygon (which may be outside, if this is a concave the polygon)
//! From http://stackoverflow.com/questions/2792443/finding-the-centroid-of-a-polygon
C2DPoint C2DShape::PtGetCentroid(void)
{
   int nVertexCount = m_VPoints.size();
   double
      dSignedArea = 0,
      dCentroidX = 0,
      dCentroidY = 0;

   // For all vertices
   for (int i = 0; i < nVertexCount; ++i)
   {
      double
         dXThis = m_VPoints[i].dGetX(),
         dYThis = m_VPoints[i].dGetY(),
         dXNext = m_VPoints[(i+1) % nVertexCount].dGetX(),
         dYNext = m_VPoints[(i+1) % nVertexCount].dGetY();

      double dA = (dXThis * dYNext) - (dXNext * dYThis);
      dSignedArea += dA;

      dCentroidX += (dXThis + dXNext) * dA;
      dCentroidY += (dYThis + dYNext) * dA;
   }

   dSignedArea *= 0.5;
   dCentroidX /= (6 * dSignedArea);
   dCentroidY /= (6 * dSignedArea);

   return (C2DPoint(dCentroidX, dCentroidY));
}
