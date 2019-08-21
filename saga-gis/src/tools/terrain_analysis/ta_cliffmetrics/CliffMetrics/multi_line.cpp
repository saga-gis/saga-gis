/*!
 *
 * \file multi_line.cpp
 * \brief CMultiLine routines
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
// #include <assert.h>

#include "cliffmetrics.h"
#include "multi_line.h"


//! Constructor, no parameters
CMultiLine::CMultiLine(void)
{
}

//! Destructor
CMultiLine::~CMultiLine(void)
{
}


//! Appends a new empty line segment
void CMultiLine::AppendLineSegment(void)
{
   m_prVVLineSegment.push_back(vector<pair<int, int> >());
}

//! Appends a line segment which is supplied as a parameter
void CMultiLine::AppendLineSegment(vector<pair<int, int> >* pprVIn)
{
   m_prVVLineSegment.push_back(*pprVIn);
}

//! Appends a line segment which then inherits from the preceding line segments
// void CMultiLine::AppendLineSegmentAndInherit(void)
// {
//    vector<pair<int, int> > prVNewLineSeg;
//    m_prVVLineSegment.push_back(prVNewLineSeg);
//
//    // Must inherit any profile numbers stored in earlier (i.e. coastward) line segments
//    int nSize = m_prVVLineSegment.size();
//    if (nSize > 1)
//    {
//       for (unsigned int n = 0; n < m_prVVLineSegment[nSize-2].size(); n++)
//       {
//          int
//             nPrevProfile = m_prVVLineSegment[nSize-2][n].first,
//             nPrevLineSeg = m_prVVLineSegment[nSize-2][n].second + 1;
//
//          m_prVVLineSegment[nSize-1].push_back(make_pair(nPrevProfile, nPrevLineSeg));
//       }
//    }
// }

//! Returns the CMultiLine object's number of line segments
int CMultiLine::nGetNumLineSegments(void) const
{
   return m_prVVLineSegment.size();
}

//! Cuts short the number of line segments
void CMultiLine::TruncateLineSegments(int const nSize)
{
   m_prVVLineSegment.resize(nSize);
}

//! Inserts a line segment, inheriting from preceding line segments
void CMultiLine::InsertLineSegment(int const nSegment)
{
//   assert(nSegment < m_prVVLineSegment.size());

   // The new vector of pairs is identical to the existing vector of pairs i.e. we inherit profile/line seg details from the previous line seg
   vector<pair<int, int> > prVPrev = m_prVVLineSegment[nSegment];

   // Store the profile numbers that are in this existing vector of pairs, these are the profiles that will be affected by this insertion
   vector<int> nVProfsAffected;
   for (unsigned int i = 0; i < prVPrev.size(); i++)
      nVProfsAffected.push_back(prVPrev[i].first);

   vector<vector<pair<int, int> > >::iterator it;
   it = m_prVVLineSegment.begin();

   m_prVVLineSegment.insert(it+nSegment+1, prVPrev);

   // Must now increment the profile's own line seg numbers, but only for those profile numbers which were affected by the insertion. Do this for the new line seg and every line seg after that
   for (unsigned int m = nSegment+1; m < m_prVVLineSegment.size(); m++)
   {
      for (unsigned int n = 0; n < m_prVVLineSegment[m].size(); n++)
      {
         for (unsigned int i = 0; i < nVProfsAffected.size(); i++)
         {
            if (m_prVVLineSegment[m][n].first == nVProfsAffected[i])
               m_prVVLineSegment[m][n].second++;
         }
      }
   }
}

//! Returns a vector of the line segments which succeed the specified line segment number
vector<vector<pair<int, int> > > CMultiLine::prVVGetAllLineSegAfter(int const nSegment)
{
   vector<vector<pair<int, int> > > prVTmp;
   for (unsigned int n = nSegment; n < m_prVVLineSegment.size(); n++)
      prVTmp.push_back(m_prVVLineSegment[n]);

   return prVTmp;
}

//! Removes a line segment
void CMultiLine::RemoveLineSegment(int const nSegment)
{
   m_prVVLineSegment.erase(m_prVVLineSegment.begin()+nSegment);
}


//! Appends a coincident profile pair to the CMultiLine object's final line segment
void CMultiLine::AppendCoincidentProfileToLineSegments(pair<int, int> const prIn)
{
   unsigned int nSize = m_prVVLineSegment.size();
   m_prVVLineSegment[nSize-1].push_back(prIn);
//   m_prVVLineSegment.back().push_back(prIn);
}

//! Adds a coincident profile to a pre-existing line segment of the CMultiLine object
void CMultiLine::AddCoincidentProfileToExistingLineSegment(int const nSegment, int const nProfile, int const nLineSeg)
{
//   assert(nSegment < m_prVVLineSegment.size());
   m_prVVLineSegment[nSegment].push_back(make_pair(nProfile, nLineSeg));
}

//! Returns a specified line segment
vector<pair<int, int> >* CMultiLine::pprVGetCoincidentProfilesForLineSegment(int const nSegment)
{
   // NOTE No check to see if nSegment < size()
   return &m_prVVLineSegment[nSegment];
}

//! Returns the number of concident profiles in a specified line segment
int CMultiLine::nGetNumCoincidentProfilesInLineSegment(int const nSegment)
{
   return m_prVVLineSegment[nSegment].size();
}

//! Returns true if the given profile number is amongst the coincident profiles of the CMultiLine object's final line segment
bool CMultiLine::bFindProfileInCoincidentProfilesOfLastLineSegment(int const nProfile)
{
   int
      nLineSegSize = m_prVVLineSegment.size(),
      nCoincidentSize = m_prVVLineSegment[nLineSegSize-1].size();

   for (int i = 0; i < nCoincidentSize; i++)
      if (m_prVVLineSegment[nLineSegSize-1][i].first == nProfile)
         return true;

   return false;
}

//! Returns true if the given profile number is one of the coincident profiles of the a specified line segment
// bool CMultiLine::bFindProfileInCoincidentProfilesOfLineSegment(int const nProfile, int const nSegment)
// {
//    // Note no check to see if nSegment < m_prVVLineSegment.size()
//    int nCoincidentSize = m_prVVLineSegment[nSegment].size();
//
//    for (int i = 0; i < nCoincidentSize; i++)
//       if (m_prVVLineSegment[nSegment][i].first == nProfile)
//          return true;
//
//    return false;
// }

//! Returns true if the given profile number is a coincident profile of any line segment of the CMultiLine object
bool CMultiLine::bFindProfileInCoincidentProfiles(int const nProfile)
{
   int nSegSize = m_prVVLineSegment.size();
   if (nSegSize == 0)
      return false;

   for (int i = nSegSize-1; i >= 0; i--)
   {
      for (unsigned int j = 0; j < m_prVVLineSegment[i].size(); j++)
      {
         if (m_prVVLineSegment[i][j].first == nProfile)
         return true;
      }
   }

   return false;
}

//! Finds the number of the most coastward line segment for which the two profiles are coincident, or -1 if they are not coincident. If they are conicidet, also finds the line segment of the other profile
void CMultiLine::GetMostCoastwardSharedLineSegment(int const nOtherProfile, int& nThisLineSegment, int& nOtherLineSegment)
{
   nThisLineSegment =
   nOtherLineSegment = -1;

   int nSegSize = m_prVVLineSegment.size();
   if (nSegSize == 0)
      return;

   for (int i = 0; i < nSegSize; i++)
   {
      for (unsigned int j = 0; j < m_prVVLineSegment[i].size(); j++)
      {
         if (m_prVVLineSegment[i][j].first == nOtherProfile)
         {
            nThisLineSegment = i;
            nOtherLineSegment = m_prVVLineSegment[i][j].second;

            return;
         }
      }
   }
}



//! Returns the profile number, given a line segment and the index of the co-incident profile for that line segment
int CMultiLine::nGetProf(int const nSegment, int const nCoinc) const
{
   return m_prVVLineSegment[nSegment][nCoinc].first;
}

//! Returns the profile's own line segment, given a line segment and the index of the co-incident profile for that line segment
int CMultiLine::nGetProfsLineSeg(int const nSegment, int const nCoinc) const
{
   return m_prVVLineSegment[nSegment][nCoinc].second;
}

//! Sets a profile's own line segment number, given a line segment and the index of the co-incident profile for that line segment
void CMultiLine::SetProfsLineSeg(int const nSegment, int const nCoinc, int const nLineSeg)
{
   // Note no check to see if nSegment < m_prVVLineSegment.size() or to see if nCoinc < m_prVVLineSegment[nSegment].size()
   m_prVVLineSegment[nSegment][nCoinc].second = nLineSeg;
}

//! Returns the number of the last line segment which includes the given profile number as a co-incident profile
int CMultiLine::nFindProfilesLastSeg(int const nProfile) const
{
   int nSeg = -1;
   for (int i = static_cast<int>(m_prVVLineSegment.size()-1); i >= 0; i--)
   {
      for (unsigned int j = 0; j < m_prVVLineSegment[i].size(); j++)
      {
         if (m_prVVLineSegment[i][j].first == nProfile)
            nSeg = i;
      }
   }

   return nSeg;
}
