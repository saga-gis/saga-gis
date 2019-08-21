/*!
 *
 * \class C2DIShape
 * \brief Abstract class, used as a base class for integer 2D objects (line, area, etc.)
 * \details TODO This is a more detailed description of the C2IDShape abstract class.
 * \author David Favis-Mortlock
 * \author Andres Payo
 * \author Jim Hall
 * \date 2017
 * \copyright GNU General Public License
 *
 * \file 2dishape.h
 * \brief Contains C2DShape definitions
 *
 */

#ifndef C2DISHAPE_H
#define C2DISHAPE_H
/*===============================================================================================================================

 This file is part of CliffMetrics, the Coastal Modelling Environment.

 CliffMetrics is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

===============================================================================================================================*/
#include <vector>
using std::vector;

#include "2di_point.h"


class C2DIShape
{
protected:
    vector<C2DIPoint> m_VPoints;

public:
   C2DIShape(void);
   virtual ~C2DIShape(void);
   C2DIPoint& operator[] (int const);
   void Clear(void);
   void Resize(const int);
//    void InsertAtFront(int const, int const);
   void Append(const C2DIPoint*);
   void Append(int const, int const);
   int nGetSize(void) const;
//    void SetPoints(const vector<C2DIPoint>*);
//    int nLookUp(C2DIPoint*);
   virtual void Display() = 0;
};
#endif // C2DISHAPE_H

