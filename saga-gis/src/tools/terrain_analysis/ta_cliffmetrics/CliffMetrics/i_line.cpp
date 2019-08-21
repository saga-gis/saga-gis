/*!
 *
 * \file i_line.cpp
 * \brief CILine routines
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
#include <iostream>
using std::cout;
using std::cerr;
using std::endl;
using std::ios;

#include "cliffmetrics.h"
#include "i_line.h"


CILine::CILine(void)
{
}

CILine::~CILine(void)
{
}

// int CILine::nGetXAt(int const n)
// {
//    return m_VPoints[n].nGetX();
// }
//
// int CILine::nGetYAt(int const n)
// {
//    return m_VPoints[n].nGetY();
// }

void CILine::SetXAt(int const n, int const nX)
{
   m_VPoints[n].SetX(nX);
}

void CILine::SetYAt(int const n, int const nY)
{
   m_VPoints[n].SetY(nY);
}

// bool CILine::bIsPresent(C2DIPoint* Pt)
// {
//    if (find(m_VPoints.begin(), m_VPoints.end(), *Pt) != m_VPoints.end())
//       return true;
//
//    return false;
// }

// bool CILine::bIsPresent(int const nX, int const nY)
// {
//    for (int n = 0; n < static_cast<int>(m_VPoints.size()); n++)
//    {
//       if ((nX == m_VPoints[n].nGetX()) && (nY == m_VPoints[n].nGetY()))
//          return true;
//    }
//    return false;
// }

void CILine::Display(void)
{
   cout << endl;
   for (int n = 0; n < static_cast<int>(m_VPoints.size()); n++)
      cout << "[" << m_VPoints[n].nGetX() << "][" << m_VPoints[n].nGetY() << "], ";
   cout << endl;
   cout.flush();
}
