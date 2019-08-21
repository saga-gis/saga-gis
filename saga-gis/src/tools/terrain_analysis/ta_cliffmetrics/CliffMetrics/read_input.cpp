/*!
 *
 * \file read_input.cpp
 * \brief Reads non-GIS input files
 * \details TODO A more detailed description of these routines.
 * \author Andres Payo 
 * \author David Favis-Mortlock
 * \author Martin Husrt
 * \author Monica Palaseanu-Lovejoy
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
#include <stdlib.h>                 // for atof()
#include <fstream>
using std::ifstream;

#include <iostream>
using std::cout;
using std::cerr;
using std::endl;
using std::ios;

#include "cliffmetrics.h"
#include "delineation.h"


/*==============================================================================================================================

 The bReadIni member function reads the initialization file

==============================================================================================================================*/
bool CDelineation::bReadIni(void)
{
   m_strCLIFFIni = m_strCLIFFDir;
   m_strCLIFFIni.append(CLIFF_INI);

   // The .ini file is assumed to be in the CliffMetrics executable's directory
   string strFilePathName(m_strCLIFFIni);

   // Tell the user what is happening
   cout << READFILELOC << strFilePathName << endl;

   // Create an ifstream object
   ifstream InStream;

   // Try to open .ini file for input
   InStream.open(strFilePathName.c_str(), ios::in);

   // Did it open OK?
   if (! InStream.is_open())
   {
      // Error: cannot open .ini file for input
      cerr << ERR << "cannot open " << strFilePathName << " for input" << endl;
      return false;
   }

   char szRec[BUFSIZE] = "";
   int i = 0;
   string strRec, strErr;

   while (InStream.getline(szRec, BUFSIZE, '\n'))
   {
      strRec = szRec;

      // Trim off leading and trailing whitespace
      strRec = strTrimLeft(&strRec);
      strRec = strTrimRight(&strRec);

      // If it is a blank line or a comment then ignore it
      if ((! strRec.empty()) && (strRec[0] != QUOTE1) && (strRec[0] != QUOTE2))
      {
         // It isn't so increment counter
         i++;

         // Find the colon: note that lines MUST have a colon separating data from leading description portion
         size_t nPos = strRec.find(':');
         if (nPos == string::npos)
         {
            // Error: badly formatted line (no colon)
            cerr << ERR << "badly formatted line (no ':') in " << strFilePathName << endl << szRec << endl;
            return false;
         }

         if (nPos == strRec.size()-1)
         {
            // Error: badly formatted line (colon with nothing following)
            cerr << ERR << "badly formatted line (nothing following ':') in " << strFilePathName << endl << szRec << endl;
            return false;
         }

         // Strip off leading portion (the bit up to and including the colon)
         string strRH = strRec.substr(nPos+1);

         // Remove leading whitespace
         strRH = strTrimLeft(&strRH);

         // Look for a trailing comment, if found then terminate string at that point and trim off any trailing whitespace
         nPos = strRH.rfind(QUOTE1);
         if (nPos != string::npos)
            strRH = strRH.substr(0, nPos+1);

         nPos = strRH.rfind(QUOTE2);
         if (nPos != string::npos)
            strRH = strRH.substr(0, nPos+1);

         // Remove trailing whitespace
         strRH = strTrimRight(&strRH);

         switch (i)
         {
         case 1:
            // The main input run-data filename
            if (strRH.empty())
               strErr = "path and name of main datafile";
            else
            {
               // First check that we don't already have an input run-data filename, e.g. one entered on the command-line
               if (m_strDataPathName.empty())
               {
                  // We don't: so first check for leading slash, or leading Unix home dir symbol, or occurrence of a drive letter
                  if ((strRH[0] == PATH_SEPARATOR) || (strRH[0] == '~') || (strRH[1] == ':'))
                     // It has an absolute path, so use it 'as is'
                     m_strDataPathName = strRH;
                  else
                  {
                     // It has a relative path, so prepend the CliffMetrics dir
                     m_strDataPathName = m_strCLIFFDir;
                     m_strDataPathName.append(strRH);
                  }
               }
            }
            break;

         case 2:
            // Path for CliffMetrics output
            if (strRH.empty())
               strErr = "path for CliffMetrics output";
            else
            {
               // Check for trailing slash on CliffMetrics output directory name (is vital)
               if (strRH[strRH.size()-1] != PATH_SEPARATOR)
                  strRH.append(&PATH_SEPARATOR);

               // Now check for leading slash, or leading Unix home dir symbol, or occurrence of a drive letter
               if ((strRH[0] == PATH_SEPARATOR) || (strRH[0] == '~') || (strRH[1] == ':'))
                  // It is an absolute path, so use it 'as is'
                  m_strOutPath = strRH;
               else
               {
                  // It is a relative path, so prepend the CliffMetrics dir
                  m_strOutPath = m_strCLIFFDir;
                  m_strOutPath.append(strRH);
               }
            }
            break;

         case 3:
            // Email address, only useful if running under Linux/Unix
            if (! strRH.empty())
            {
               // Something was entered, do rudimentary check for valid email address
               if (strRH.find('@') == string::npos)
                  strErr = "email address for messages";
               else
                  m_strMailAddress = strRH;
            }
            break;
         }

         // Did an error occur?
         if (! strErr.empty())
         {
            // Error in input to initialisation file
            cerr << ERR << "reading " << strErr << " in " << strFilePathName << endl << "'" << strRec << "'" << endl;
            InStream.close();

            return false;
         }
      }
   }

   InStream.close();
   return true;
}


/*==============================================================================================================================

 Reads the run details input file and does some initialization

==============================================================================================================================*/
bool CDelineation::bReadRunData(void)
{
   // Create an ifstream object
   ifstream InStream;

   // Try to open run details file for input
   InStream.open(m_strDataPathName.c_str(), ios::in);

   // Did it open OK?
   if (! InStream.is_open())
   {
      // Error: cannot open run details file for input
      cerr << ERR << "cannot open " << m_strDataPathName << " for input" << endl;
      return false;
   }

   char szRec[BUFSIZE] = "";
   int
      i = 0;
      size_t nPos = 0;
   string strRec, strErr;

   while (InStream.getline(szRec, BUFSIZE, '\n'))
   {
      strRec = szRec;

      // Trim off leading and trailing whitespace
      strRec = strTrimLeft(&strRec);
      strRec = strTrimRight(&strRec);

      // If it is a blank line or a comment then ignore it
      if ((! strRec.empty()) && (strRec[0] != QUOTE1) && (strRec[0] != QUOTE2))
      {
         // It isn't so increment counter
         i++;

         // Find the colon: note that lines MUST have a colon separating data from leading description portion
         nPos = strRec.find(':');
         if (nPos == string::npos)
         {
            // Error: badly formatted line (no colon)
            cerr << ERR << "badly formatted line (no ':') in " << m_strDataPathName << endl << szRec << endl;
            return false;
         }

         // Strip off leading portion (the bit up to and including the colon)
         string strRH = strRec.substr(nPos+1);

         // Remove leading whitespace after the colon
         strRH = strTrimLeft(&strRH);

         // Look for trailing comments, if found then terminate string at that point and trim off any trailing whitespace
         bool bFound = true;
         while (bFound)
         {
            bFound = false;

            nPos = strRH.rfind(QUOTE1);
            if (nPos != string::npos)
            {
               strRH = strRH.substr(0, nPos);
               bFound = true;
            }

            nPos = strRH.rfind(QUOTE2);
            if (nPos != string::npos)
            {
               strRH = strRH.substr(0, nPos);
               bFound = true;
            }

            // Trim trailing spaces
            strRH = strTrimRight(&strRH);
         }

#ifdef _WIN32
            // For Windows, make sure has backslashes, not Unix-style slashes
            strRH = pstrChangeToBackslash(&strRH);
#endif
        
        string strTmp;

         switch (i)
         {
         // ---------------------------------------------- Run Information -----------------------------------------------------
         case 1:
            // Text output file names, don't change case
            if (strRH.empty())
               strErr = "output file names";
            else
            {
               m_strRunName = strRH;

               m_strOutFile = m_strOutPath;
               m_strOutFile.append(strRH);
               m_strOutFile.append(OUTEXT);

               m_strLogFile = m_strOutPath;
               m_strLogFile.append(strRH);
               m_strLogFile.append(LOGEXT);
            }
            break;

         case 2:
            //  DTM file (can't be blank)
            if (! strRH.empty())
            {
#ifdef _WIN32
               // For Windows, make sure has backslashes, not Unix-style slashes
               strRH = pstrChangeToBackslash(&strRH);
#endif
               // Now check for leading slash, or leading Unix home dir symbol, or occurrence of a drive letter
               if ((strRH[0] == PATH_SEPARATOR) || (strRH[0] == '~') || (strRH[1] == ':'))
                  // It has an absolute path, so use it 'as is'
 		    m_strDTMFile = strRH;
               else
               {
                  // It has a relative path, so prepend the CliffMetrics dir
 	            m_strDTMFile = m_strCLIFFDir;
                    m_strDTMFile.append(strRH);
               }
            }
            break;

         case 3:
            //  Still water level (m) used to extract the shoreline
            m_dStillWaterLevel = atof(strRH.c_str());
            break;

         case 4:
            // Vector coastline smoothing algorithm: 0 = none, 1 = running mean, 2 = Savitsky-Golay
            m_nCoastSmooth = atoi(strRH.c_str());
            if ((m_nCoastSmooth < SMOOTH_NONE) || (m_nCoastSmooth > SMOOTH_SAVITZKY_GOLAY))
               strErr = "coastline vector smoothing algorithm";
            break;

         case 5:
            // Size of coastline smoothing window: must be odd
            m_nCoastSmoothWindow = atoi(strRH.c_str());
            if ((m_nCoastSmoothWindow <= 0) || !(m_nCoastSmoothWindow % 2))
               strErr = "size of coastline vector smoothing window (must be > 0 and odd)";
            break;

         case 6:
            // Order of coastline profile smoothing polynomial for Savitsky-Golay: usually 2 or 4, max is 6
            m_nSavGolCoastPoly = atoi(strRH.c_str());
            if ((m_nSavGolCoastPoly <= 0) || (m_nSavGolCoastPoly > 6))
               strErr = "value of Savitsky-Golay polynomial for coastline smoothing (must be <= 6)";
            break;

         case 7:
            // Optional shoreline shape file (can be blank)
            if (! strRH.empty())
            {
#ifdef _WIN32
               // For Windows, make sure has backslashes, not Unix-style slashes
               strRH = pstrChangeToBackslash(&strRH);
#endif
               // Now check for leading slash, or leading Unix home dir symbol, or occurrence of a drive letter
               if ((strRH[0] == PATH_SEPARATOR) || (strRH[0] == '~') || (strRH[1] == ':'))
               {
                  // It has an absolute path, so use it 'as is'
                  m_strInitialLandformFile = strRH;
               }
               else
               {
                  // It has a relative path, so prepend the CliffMetrics dir
                  m_strInitialLandformFile = m_strCLIFFDir;
                  m_strInitialLandformFile.append(strRH);
               }
            }
            break;

         case 8:
            // Raster GIS output format (note must retain original case). Blank means use same format as input DEM file (if possible)
            m_strRasterGISOutFormat = strTrimLeft(&strRH);
            break;

         case 9:
            // If needed, also output GIS raster world file?
            strRH = strToLower(&strRH);

            m_bWorldFile = false;
            if (strRH.find("y") != string::npos)
               m_bWorldFile = true;
            break;

         case 10:
            // If needed, scale GIS raster output values?
            strRH = strToLower(&strRH);

            m_bScaleRasterOutput = false;
            if (strRH.find("y") != string::npos)
               m_bScaleRasterOutput = true;
            break;

        case 11:
            // Vector GIS output format (note must retain original case)
            m_strVectorGISOutFormat = strRH;

            if (strRH.empty())
               strErr = "vector GIS output format";
            break;
	    
         case 12:
            // Random edge for coastline search?
            strRH = strToLower(&strRH);

            m_bRandomCoastEdgeSearch = false;
            if (strRH.find("y") != string::npos)
               m_bRandomCoastEdgeSearch = true;
            break;
         
         case 13:
            // Random number seed(s)
            m_ulRandSeed[0] = atol(strRH.c_str());
            if (0 == m_ulRandSeed[0])
            {
               strErr = "random number seed";
               break;
            }
             // TODO rewrite this, similar to reading raster slice elevations
            // OK, so find out whether we're dealing with a single seed or more than one: check for a space
            nPos = strRH.find(SPACE);
            if (nPos != string::npos)
            {
               // There's a space, so we must have more than one number
               int n = 0;
               do
               {
                  // Trim off the part before the first space then remove leading whitespace
                  strRH = strRH.substr(nPos, strRH.size()-nPos);
                  strRH = strTrimLeft(&strRH);

                  // Put the number into the array
                  m_ulRandSeed[++n] = atol(strRH.c_str());

                  // Now look for another space
                  nPos = strRH.find(SPACE);
               }
               while ((n < NRNG) && (nPos != string::npos));
            }
            else
            {
               // Only one seed specified, so make all seeds the same
               for (int n = 1; n < NRNG; n++)
                  m_ulRandSeed[n] = m_ulRandSeed[n-1];
            }
            break;

         case 14:
            // Length of coastline normals (m)
            m_dCoastNormalLength = atof(strRH.c_str());
            if (m_dCoastNormalLength <= 0)
               strErr = "length of coastline normals must be greater than zero";
            break;

         case 15:
            // Vertical tolerance avoid false CliffTops/Toes
             m_dEleTolerance = atof(strRH.c_str());
            if (m_dEleTolerance <= 0)
               strErr = "vertical elevation tolerance must be greater than zero";
            break;
	 }
	 
         // Did an error occur?
         if (! strErr.empty())
         {
            // Error in input to run details file
            cerr << endl << ERR << strErr << ".\nPlease edit " << m_strDataPathName << " and change this line:" << endl << "'" << szRec << "'" << endl << endl;
            InStream.close();
            return false;
         }
      }
   }
   // Close file
   InStream.close();

   return true;
}

