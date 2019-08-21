/*!
 *
 * \class CMultiLine
 * \brief Class used to represent co-incident lines of profiles
 * \details TODO This is a more detailed description of the CMultiLine class.
 * \author David Favis-Mortlock
 * \author Andres Payo
 * \author Jim Hall
 * \date 2017
 * \copyright GNU General Public License
 *
 * \file multiline.h
 * \brief Contains CMultiLine definitions
 *
 */

#ifndef PROFLINE_H
#define PROFLINE_H
/*===============================================================================================================================

 This file is part of CliffMetrics, the Coastal Modelling Environment.

 CliffMetrics is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

===============================================================================================================================*/
#include <utility>
using std::pair;
using std::make_pair;

#include "line.h"


class CMultiLine : public CLine
{
private:
   vector<vector<pair<int, int> > > m_prVVLineSegment;         // A vector of line segments, each of which is a vector of pairs. The first of the pair is a co-incident profile number, the second is that profile's 'own' line segment number

public:
   CMultiLine(void);
   ~CMultiLine(void);

   void AppendLineSegment(void);
   void AppendLineSegment(vector<pair<int, int> >*);
//    void AppendLineSegmentAndInherit(void);
   int nGetNumLineSegments(void) const;
   void TruncateLineSegments(int const);
   void InsertLineSegment(int const);
   vector<vector<pair<int, int> > > prVVGetAllLineSegAfter(int const);
   void RemoveLineSegment(int const);

   void AppendCoincidentProfileToLineSegments(pair<int, int> const);
   void AddCoincidentProfileToExistingLineSegment(int const, int const, int const);
   vector<pair<int, int> >* pprVGetCoincidentProfilesForLineSegment(int const);
   int nGetNumCoincidentProfilesInLineSegment(int const);
   bool bFindProfileInCoincidentProfilesOfLastLineSegment(int const);
//    bool bFindProfileInCoincidentProfilesOfLineSegment(int const, int const);
   bool bFindProfileInCoincidentProfiles(int const);
   void GetMostCoastwardSharedLineSegment(int const, int&, int&);

   int nGetProf(int const, int const) const;
   int nGetProfsLineSeg(int const, int const) const;
   void SetProfsLineSeg(int const, int const, int const);

   int nFindProfilesLastSeg(int const) const;

};
#endif // PROFLINE_H

