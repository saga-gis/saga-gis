/*!
 *
 * \file 2d_point.cpp
 * \brief Class used to represent 2D point objects with floating-point coordinates
 * \details The C2DPoint class is used to represent 2D points where the x and y coordinates are floating-point values, e.g. points for which the x and y coordinates are in the external CRS (coordinate reference system)
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
#include "2d_point.h"


C2DPoint::C2DPoint(void)
:  dX(0),
   dY(0)
{
}

C2DPoint::C2DPoint(double const dNewX, double const dNewY)
:  dX(dNewX),
   dY(dNewY)
{
}


double C2DPoint::dGetX(void) const
{
   return dX;
}

double C2DPoint::dGetY(void) const
{
   return dY;
}

void C2DPoint::SetX(double const dNewX)
{
   dX = dNewX;
}

void C2DPoint::SetY(double const dNewY)
{
   dY = dNewY;
}

// void C2DPoint::SetXY(double const dNewX, double const dNewY)
// {
//    dX = dNewX;
//    dY = dNewY;
// }

// void C2DPoint::SetXY(C2DPoint const* Pt)
// {
//    dX = Pt->dGetX();
//    dY = Pt->dGetY();
// }

//! Sets one C2DPoint object equal to another
void C2DPoint::operator= (C2DPoint* pPt)
{
   dX = pPt->dGetX();
   dY = pPt->dGetY();
}

// //! Compares two C2DPoint objects for equality
// bool C2DPoint::operator== (C2DPoint* pPt) const
// {
//    if ((pPt->dGetX() == dX) && (pPt->dGetY() == dY))
//       return true;
//
//    return false;
// }

//! Compares two C2DPoint objects for equality
bool C2DPoint::operator== (C2DPoint* pPt) const
{
   if ((pPt->dGetX() == dX) && (pPt->dGetY() == dY))
      return true;

   return false;
}

//! Compares two C2DPoint objects for inequality
bool C2DPoint::operator!= (C2DPoint* pPt) const
{
   if ((pPt->dGetX() == dX) && (pPt->dGetY() == dY))
      return false;

   return true;
}
