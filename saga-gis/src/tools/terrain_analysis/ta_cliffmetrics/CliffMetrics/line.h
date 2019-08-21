/*!
 *
 * \class CLine
 * \brief Class used to represent 2D vector line objects
 * \details TODO This is a more detailed description of the CLine class.
 * \author David Favis-Mortlock
 * \author Andres Payo
 * \author Jim Hall
 * \date 2017
 * \copyright GNU General Public License
 *
 * \file line.h
 * \brief Contains CLine definitions
 *
 */

#ifndef LINE_H
#define LINE_H
/*===============================================================================================================================

 This file is part of CliffMetrics, the Coastal Modelling Environment.

 CliffMetrics is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

===============================================================================================================================*/
#include "2d_shape.h"


class CLine : public C2DShape
{
public:
   CLine(void);
   ~CLine(void);

   double dGetXAt(int const);
   double dGetYAt(int const);
   void SetXAt(int const, double const);
   void SetYAt(int const, double const);
//    bool bIsPresent(C2DPoint*);

   void Display(void);
};
#endif // LINE_H

