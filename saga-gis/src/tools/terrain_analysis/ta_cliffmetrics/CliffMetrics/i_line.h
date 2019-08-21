/*!
 *
 * \class CILine
 * \brief Class used to represent 2D vector integer line objects
 * \details TODO This is a more detailed description of the CCLine class.
 * \author David Favis-Mortlock
 * \author Andres Payo
 * \author Jim Hall
 * \date 2017
 * \copyright GNU General Public License
 *
 * \file iline.h
 * \brief Contains CILine definitions
 *
 */

#ifndef ILINE_H
#define ILINE_H
/*===============================================================================================================================

 This file is part of CliffMetrics, the Coastal Modelling Environment.

 CliffMetrics is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

===============================================================================================================================*/
#include "2di_shape.h"


class CILine : public C2DIShape
{
public:
   CILine(void);
   ~CILine(void);
//    int nGetXAt(int const);
//    int nGetYAt(int const);
   void SetXAt(int const, int const);
   void SetYAt(int const, int const);
//    bool bIsPresent(C2DIPoint*);
//    bool bIsPresent(int const, int const);
   void Display(void);

};
#endif // ILINE_H

