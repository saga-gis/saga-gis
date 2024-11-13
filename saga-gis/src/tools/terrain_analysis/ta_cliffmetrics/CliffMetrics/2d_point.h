/*!
 *
 * \class C2DPoint
 * \brief Class used to represent 2D point objects with floating-point coordinates
 * \details The C2DPoint class is used to represent 2D points where the x and y coordinates are floating-point values, e.g. points for which the x and y coordinates are in the external CRS (coordinate reference system)
 * \author David Favis-Mortlock
 * \author Andres Payo
 * \author Jim Hall
 * \date 2017
 * \copyright GNU General Public License
 *
 * \file 2dpoint.h
 * \brief Contains C2DPoint definitions
 *
 */

#ifndef C2DPOINT_H
#define C2DPOINT_H
/*===============================================================================================================================

 This file is part of CliffMetrics, the Coastal Modelling Environment.

 CliffMetrics is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

===============================================================================================================================*/
class C2DPoint
{
private:
   double dX, dY;

public:
   C2DPoint(void);
   C2DPoint(double const, double const);
   double dGetX(void) const;
   double dGetY(void) const;
   void SetX(double const);
   void SetY(double const);
//    void SetXY(double const, double const);
//    void SetXY(C2DPoint const*);
   void operator= (C2DPoint*);
//    bool operator== (C2DPoint*) const;
   bool operator== (C2DPoint*) const;
   bool operator!= (C2DPoint*) const;
};
#endif // C2DPOINT_H
