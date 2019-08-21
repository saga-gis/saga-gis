/*!
 *
 * \file utils_global.cpp
 * \brief Globally-available utility routines
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

 CliffMetrics is free software; you can redistribute it and/or modify it under the terms of the GNU General Public  License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

==============================================================================================================================*/
#include <string.h>                 // for strcpy(), strlen() etc.
#include <string>
#include <cmath>
#include <cfloat>

#include <sstream>
using std::stringstream;
using std::ostringstream;

#include <iomanip>
using std::setw;

#include <algorithm>
using std::transform;

#include <numeric>
using std::accumulate;
using std::inner_product;

#include "cliffmetrics.h"


/*==============================================================================================================================

 Correctly rounds doubles

==============================================================================================================================*/
double dRound(double const d)
{
   // Rounds positive or negative doubles correctly
   return ((d < 0.0) ? ceil(d - 0.5) : floor(d + 0.5));
}


/*==============================================================================================================================

 For comparison of two floating-point numbers, with a specified accuracy

==============================================================================================================================*/
bool bFPIsEqual(double const d1, double const d2, double const dEpsilon)
{
   // Since the accuracy of floating-point numbers varies with their magnitude, must compare them by using an accuracy threshold which is relative to the magnitude of the two numbers being compared. This is a blend of an example from Knuth's 'The Art of Computer Programming. Volume 1. Fundamental Algorithms' and a posting dated 18 Nov 93 by rmartin@rcmcon.com (Robert Martin), archived in cpp_tips
   if ((0 == d1) && (tAbs(d2) < dEpsilon))
      return true;
   else if ((0 == d2) && (tAbs(d1) < dEpsilon))
      return true;
   else
      return ((tAbs(d1 - d2) < (dEpsilon * tAbs(d1))) ? true : false);
}


// bool bIsWhole(double d)
// {
//    // From http://answers.yahoo.com/question/index?qid=20110320132617AAMdb7u
//    return (static_cast<int>(d) == d);
// }


/*==============================================================================================================================

 Calculates the vector cross product of three points

==============================================================================================================================*/
double dCrossProduct(double const dX1, double const dY1, double const dX2, double const dY2, double const dX3, double const dY3)
{
   // Based on code at http://debian.fmi.uni-sofia.bg/~sergei/cgsr/docs/clockwise.htm
   return (dX2 - dX1) * (dY3 - dY2) - ((dY2 - dY1) * (dX3 - dX2));
}


/*==============================================================================================================================

 Checks a double to see if it is NaN. From http://www.johndcook.com/blog/IEEE_exceptions_in_cpp/

==============================================================================================================================*/
bool bIsNumber(double const dX)
{
   // This looks like it should always be true, but it is false if dX is a NaN
   return (dX == dX);
}


/*==============================================================================================================================

 Checks a double to see if it is finite. From http://www.johndcook.com/blog/IEEE_exceptions_in_cpp/

==============================================================================================================================*/
// bool bIsFinite(double const dX)
// {
//    return (dX <= DBL_MAX && dX >= -DBL_MAX);
// }


/*==============================================================================================================================

 Calculates the mean of a pointer to a vector of doubles

==============================================================================================================================*/
double dGetMean(vector<double>* const pV)
{
   double dSum = std::accumulate(pV->begin(), pV->end(), 0.0);
   double dMean = dSum / pV->size();
   return dMean;
}


/*==============================================================================================================================

 Calculates the standard deviation of a pointer to a vector of doubles. From http://stackoverflow.com/questions/7616511/calculate-mean-and-standard-deviation-from-a-vector-of-samples-in-c-using-boos

==============================================================================================================================*/
double dGetStdDev(vector<double>* const pV)
{
   double dSum = std::accumulate(pV->begin(), pV->end(), 0.0);
   double dMean = dSum / pV->size();

   double dSqSum = std::inner_product(pV->begin(), pV->end(), pV->begin(), 0.0);
   double dStdDev = std::sqrt(dSqSum / pV->size() - dMean * dMean);

   return dStdDev;
}


/*==============================================================================================================================

 Trims whitespace from the left side of a string, does not change the original string

==============================================================================================================================*/
string strTrimLeft(string const* strIn)
{
   // Trim leading spaces
   size_t nStartpos = strIn->find_first_not_of(" \t");
   if (nStartpos == string::npos)
      return *strIn;
   else
      return strIn->substr(nStartpos);
}

/*==============================================================================================================================

 Trims whitespace from the right side of a string, does not change the original string

==============================================================================================================================*/
string strTrimRight(string const* strIn)
{
   // Trim trailing spaces
   size_t nEndpos = strIn->find_last_not_of(" \t");
   if (nEndpos == string::npos)
      return *strIn;
   else
      return strIn->substr(0, nEndpos+1);
}

/*==============================================================================================================================

 Trims whitespace from both sides of a string, does not change the original string

==============================================================================================================================*/
string strTrim(string* strIn)
{
   string strTmp = *strIn;

   // Trim trailing spaces
   size_t nPos = strTmp.find_last_not_of(" \t");

   if (nPos != string::npos)
      strTmp = strTmp.substr(0, nPos+1);

   // Trim leading spaces
   nPos = strTmp.find_first_not_of(" \t");

   if (nPos != string::npos)
      strTmp = strTmp.substr(nPos);

   return strTmp;
}

/*==============================================================================================================================

 Returns the lower case version of an string, leaving the original unchanged

==============================================================================================================================*/
string strToLower(string* strIn)
{
   string strOut = *strIn;
   std::transform(strIn->begin(), strIn->end(), strOut.begin(), ::tolower);
   return strOut;
}

/*==============================================================================================================================

 Returns the upper case version of an string, leaving the original unchanged

==============================================================================================================================*/
// string strToUpper(string* strIn)
// {
//    string strOut = *strIn;
//    std::transform(strIn->begin(), strIn->end(), strOut.begin(), ::toupper);
//    return strOut;
// }

/*==============================================================================================================================

 Returns a string with a substring removed

==============================================================================================================================*/
string strRemoveSubstr(string* strIn, string const* strSub)
{
   size_t nPos = strIn->find(*strSub);

   // If not found, return the string unchanged
   if (nPos != string::npos)
      strIn->replace(nPos, strSub->size(), "");

   return *strIn;
}

/*==============================================================================================================================

 These two functions are from http://stackoverflow.com/questions/236129/split-a-string-in-c They implement (approximately) Python's split() function. This first version puts the results into a pre-constructed string vector. It ignores empty items

==============================================================================================================================*/
vector<string>* strSplit(string const* s, char const delim, vector<string>* elems)
{
   stringstream ss(*s);
   string item;
   while (getline(ss, item, delim))
   {
      if (! item.empty())
         elems->push_back(item);
   }
   return elems;
}

/*==============================================================================================================================

 This second version returns a new string vector (it calls the first version)

==============================================================================================================================*/
vector<string> strSplit(string const* s, char const delim)
{
   vector<string> elems;
   strSplit(s, delim, &elems);
   return elems;
}

/*==============================================================================================================================

 Changes all forward slashes in the input string to backslashes, leaving the original unchanged

==============================================================================================================================*/
string pstrChangeToBackslash(string const* strIn)
{
   string strOut(*strIn);
   std::replace(strOut.begin(), strOut.end(), '/', '\\');
   return strOut;
}

/*==============================================================================================================================

 Swaps all backslashes in the input string to forward slashes, leaving the original unchanged

==============================================================================================================================*/
string pstrChangeToForwardSlash(string const* strIn)
{
   string strOut(*strIn);
   std::replace(strOut.begin(), strOut.end(), '\\', '/');
   return strOut;
}

/*==============================================================================================================================

 Public domain routines from the Snippets library, dated 05-Jul-1997

==============================================================================================================================*/
extern "C"
{
   void* MoveStr(char* pszDest, char* const pszSource)
   {
      // Adapted from a safe string-moving macro by Keiichi Nakasato: like strcpy, except guaranteed to work with overlapping strings
      return memmove(pszDest, pszSource, strlen(pszSource) + 1);
   }

   // Portable, public domain replacements for strupr() & strlwr() by Bob Stout
//    char* pszToUpper(char* pszStr)
//    {
//       if (pszStr)
//       {
//          for (char* s = pszStr; *s; ++s)
//             *s = toupper(*s);
//       }
//       return pszStr;
//    }

//    char* pszToLower(char* pszStr)
//    {
//       if (pszStr)
//       {
//          for (char* s = pszStr; *s; ++s)
//             *s = tolower(*s);
//       }
//       return pszStr;
//    }

   // Removes leading whitespace from a string. Originally published as part of the MicroFirm Function Library. Copyright 1986, S.E. Margison. Copyright 1989, Robert B. Stout. The user is granted a free limited license to use this source file to create royalty-free programs, subject to the terms of the license restrictions specified in the LICENSE.MFL file
   char* pszTrimLeft(char* pszStr)
   {
      if (pszStr)
      {
         char* obuf;
         for (obuf = pszStr; *obuf && isspace(*obuf); ++obuf)
            ;
         if (pszStr != obuf)
            MoveStr(pszStr, obuf);
      }
      return pszStr;
   }

   // Removes trailing whitespace from a string. Originally published as part of the MicroFirm Function Library. Copyright 1986, S.E. Margison. Copyright 1989, Robert B. Stout. The user is granted a free limited license to use this source file to create royalty-free programs, subject to the terms of the license restrictions specified in the LICENSE.MFL file
//    char* pszTrimRight(char* pszStr)
//    {
//       int i;
//       if (pszStr && 0 != (i = strlen(pszStr)))
//       {
//          while (--i >= 0)
//          {
//             if (! isspace(pszStr[i]))
//                break;
//          }
//          pszStr[++i] = '\0';
//       }
//       return pszStr;
//    }

   // pszLongToSz.c -- an improved, safer, ltoa(). On call:
   //    num       =   number to convert
   //    string    =   buffer for output
   //    max_chars =   maximum size of buffer
   //    base      =   number base for conversion.
   //
   // Return value:
   //    if illegal base
   //       NULL
   //    beginning of converted number.
   //
   // Note: if number is too large in magnitude to fit in the buffer, the MOST significant digits will be truncated. If the number is negative, a leading '-' will be placed in the buffer even if this causes the most significant digit to be truncated. The number is right justified in the buffer and the location of the first character in the number is returned so:
   // * If you want right justification, use the original string. (NOTE: modified by DFM to pad with zeros rather than blanks)
   // * If you want left justification, use the returned string.
   // If the number doesn't fill the buffer, leading characters will be filled with spaces.
   //
   // Public domain by Jerry Coffin
   //
   // NOTE: this did not work correctly, and has been partly rewritten by DFM. It appears that the original 'for' loop construction worked differently when compiled by a C compiler, as compared to a C++ compiler
   char* pszLongToSz(long num, char* pszStr, int max_chars, int base)
   {
      int nsign = 0;       // number of digits occupied by the sign

      if (base < 2 || base > 36)
         return (NULL);

      if (num < 0)
      {
         nsign = 1;
         num = -num;
      }

      pszStr[--max_chars] = '\0';

      while (max_chars >= nsign && num != 0)
      {
         max_chars--;

         char rem = (char)(num % base);
         if (rem <= 9)
            pszStr[max_chars] = rem + '0';
         else
            pszStr[max_chars] = rem - 10 + 'A';

         num /= base;
      }

      if (nsign)
         pszStr[--max_chars] = '-';

      if (max_chars > 0)
         memset(pszStr, '0', max_chars);

      return pszStr + max_chars;
   }

   /*===========================================================================================================================

    Similar routine of my own

   ===========================================================================================================================*/
//    char* pszRemoveSubstr(char* pszStr, char* substr, char* subpos)
//    {
//       if (pszStr)
//       {
//          char* p;
//          char* s;
//          int l = strlen(substr);
//
//          for (p = subpos, s = subpos+l; *s; ++p,++s)
//             *p = *s;
//
//          *p = '\0';
//       }
//
//       return pszStr;
//    }

}

/*==============================================================================================================================

 Convert int into a string from stack overflow

==============================================================================================================================*/
string NumberToString(int Number)
{
   ostringstream oOStrStream;
   oOStrStream <<  Number;
   return oOStrStream.str();
}