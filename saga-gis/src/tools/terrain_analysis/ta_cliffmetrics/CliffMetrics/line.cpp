/*!
 *
 * \file line.cpp
 * \brief CLine routines
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
#include "line.h"


CLine::CLine(void)
{
}

CLine::~CLine(void)
{
}

double CLine::dGetXAt(int const n)
{
   return m_VPoints[n].dGetX();
}

double CLine::dGetYAt(int const n)
{
   return m_VPoints[n].dGetY();
}

void CLine::SetXAt(int const n, double const x)
{
   m_VPoints[n].SetX(x);
}

void CLine::SetYAt(int const n, double const y)
{
   m_VPoints[n].SetY(y);
}

// bool CLine::bIsPresent(C2DPoint* Pt)
// {
//    if (std::find(m_VPoints.begin(), m_VPoints.end(), *Pt) != m_VPoints.end())
//       return true;
//
//    return false;
// }

void CLine::Display(void)
{
   cout << endl;
   for (int n = 0; n < static_cast<int>(m_VPoints.size()); n++)
      cout << "[" << m_VPoints[n].dGetX() << "][" << m_VPoints[n].dGetY() << "], ";
   cout << endl;
   cout.flush();
}
