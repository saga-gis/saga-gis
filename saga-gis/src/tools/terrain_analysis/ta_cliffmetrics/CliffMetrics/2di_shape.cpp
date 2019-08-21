/*!
 *
 * \file 2di_shape.cpp
 * \brief C2DIShape routines
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
#include <vector>

#include "2di_shape.h"

//! Constructor, no parameters
C2DIShape::C2DIShape(void)
{
}

//! Destructor
C2DIShape::~C2DIShape(void)
{
}

C2DIPoint& C2DIShape::operator[] (int const n)
{
   // NOTE No safety check
   return m_VPoints[n];
}

void C2DIShape::Clear(void)
{
   m_VPoints.clear();
}

void C2DIShape::Resize(const int nSize)
{
   m_VPoints.resize(nSize);
}

// void C2DIShape::InsertAtFront(int const nX, int const nY)
// {
//    m_VPoints.insert(m_VPoints.begin(), C2DIPoint(nX, nY));
// }

void C2DIShape::Append(const C2DIPoint* PtiNew)
{
   m_VPoints.push_back(*PtiNew);
}

void C2DIShape::Append(int const nX, int const nY)
{
   m_VPoints.push_back(C2DIPoint(nX, nY));
}

int C2DIShape::nGetSize(void) const
{
   return m_VPoints.size();
}

// void C2DIShape::SetPoints(const vector<C2DIPoint>* VNewPoints)
// {
//    m_VPoints = *VNewPoints;
// }

// int C2DIShape::nLookUp(C2DIPoint* Pti)
// {
//    vector<C2DIPoint>::iterator it = std::find(m_VPoints.begin(), m_VPoints.end(), *Pti);
//    if (it != m_VPoints.end())
//       return it - m_VPoints.begin();
//    else
//       return -1;
// }
