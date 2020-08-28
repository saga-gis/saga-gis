/*!
 *
 * \file write_output.cpp
 * \brief Writes non-GIS output files for CliffMetrics
 * \details TODO A more detailed description of this routine.
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
#include <ctime>
#include <iostream>
using std::cout;
using std::cerr;
using std::endl;
using std::ios;

#include <iomanip>
using std::setiosflags;
using std::resetiosflags;
using std::setprecision;
using std::setw;

#include "cliffmetrics.h"
#include "delineation.h"


/*==============================================================================================================================

 Writes beginning-of-run information to Out and Log files

==============================================================================================================================*/
void CDelineation::WriteStartRunDetails(void)
{
   // Set default output format to fixed point
   OutStream << setiosflags(ios::fixed);

   OutStream << PROGNAME << " for " << PLATFORM << " " << strGetBuild() << " on " << strGetComputerName() << endl << endl;

   LogStream << PROGNAME << " for " << PLATFORM << " " << strGetBuild() << " on " << strGetComputerName() << endl << endl;

   // ----------------------------------------------- Run Information ----------------------------------------------------------
   OutStream << "RUN DETAILS" << endl;
   OutStream << " Name                                                      \t: " << m_strRunName << endl;
   OutStream << " Started on                                                \t: " << ctime(&m_tSysStartTime);   //  << endl;

   // Same info. for Log file
   LogStream << m_strRunName << " run started on " << ctime(&m_tSysStartTime) << endl;

   // Contine with Out file
   OutStream << " Initialization file                                       \t: "
#ifdef _WIN32
      << pstrChangeToForwardSlash(&m_strCLIFFIni) << endl;
#else
      << m_strCLIFFIni << endl;
#endif

   OutStream << " Input data read from                                      \t: "
#ifdef _WIN32
      << pstrChangeToForwardSlash(&m_strDataPathName) << endl;
#else
      << m_strDataPathName << endl;
#endif

   OutStream << " Random number seeds                                       \t: ";
   {
      for (int i = 0; i < NRNG; i++)
         OutStream << m_ulRandSeed[i] << '\t';
   }
   OutStream << endl;

   OutStream << "*First random numbers generated                            \t: " << ulGetRand0() << '\t' << ulGetRand1() << endl;
   OutStream << " Raster GIS output format                                  \t: " << m_strGDALRasterOutputDriverLongname << endl;
   OutStream << " Raster output values scaled (if needed)                   \t: " << (m_bScaleRasterOutput ? "Y": "N") << endl;
   OutStream << " Raster world files created (if needed)                    \t: " << (m_bWorldFile ? "Y": "N") << endl;
   OutStream << " Raster GIS files saved                                    \t: " << strListRasterFiles() << endl;

   OutStream << " Vector GIS output format                                  \t: " << m_strVectorGISOutFormat << endl;
   OutStream << " Vector GIS files saved                                    \t: " << strListVectorFiles() << endl;
   OutStream << " Output file (this file)                                   \t: "
#ifdef _WIN32
      << pstrChangeToForwardSlash(&m_strOutFile) << endl;
#else
      << m_strOutFile << endl;
#endif
   OutStream << " Log file                                                  \t: "
#ifdef _WIN32
      << pstrChangeToForwardSlash(&m_strLogFile) << endl;
#else
      << m_strLogFile << endl;
#endif

   OutStream << " Coastline vector smoothing algorithm                      \t: ";
   switch (m_nCoastSmooth)
   {
      case SMOOTH_NONE:
      {
         OutStream << "none";
         break;
      }

      case SMOOTH_RUNNING_MEAN:
      {
         OutStream << "running mean";
         break;
      }

      case SMOOTH_SAVITZKY_GOLAY:
      {
         OutStream << "Savitzky-Golay";
         break;
      }
   }
   OutStream << endl;
   OutStream << " Random edge for coastline search?                         \t: " << (m_bRandomCoastEdgeSearch ? "Y": "N") << endl;
   OutStream << endl;

   if (m_nCoastSmooth != SMOOTH_NONE)
   {
      OutStream << " Size of coastline vector smoothing window                 \t: " << m_nCoastSmoothWindow << endl;

      if (m_nCoastSmooth == SMOOTH_SAVITZKY_GOLAY)
         OutStream << " Savitzky-Golay coastline smoothing polynomial order       \t: " << m_nSavGolCoastPoly << endl;
   }

   // --------------------------------------------------- Raster GIS stuff -------------------------------------------------------
   OutStream << "Raster GIS Input Files" << endl;
   OutStream << " DTM file                                                  \t: "
#ifdef _WIN32
      << pstrChangeToForwardSlash(&m_strDTMFile) << endl;
#else
      << m_strDTMFile << endl;
#endif
   OutStream << " DTM driver code                                           \t: " << m_strGDALDTMDriverCode << endl;
   OutStream << " GDAL DTM driver description                               \t: " << m_strGDALDTMDriverDesc << endl;
   OutStream << " GDAL DTM projection                                       \t: " << m_strGDALDTMProjection << endl;
   OutStream << " GDAL DTM data type                                        \t: " << m_strGDALDTMDataType << endl;
   OutStream << " Grid size (X by Y)                                        \t: " << m_nXGridMax << " by " << m_nYGridMax << endl;
   OutStream << resetiosflags(ios::floatfield);
   OutStream << setiosflags(ios::fixed) << setprecision(1);
   OutStream << "*Coordinates of NW corner of grid (external CRS)           \t: " << m_dNorthWestXExtCRS << ", " << m_dNorthWestYExtCRS << endl;
   OutStream << "*Coordinates of SE corner of grid (external CRS)           \t: " << m_dSouthEastXExtCRS << ", " << m_dSouthEastYExtCRS << endl;
   OutStream << "*Cell size                                                 \t: " << m_dCellSide << " m" << endl;
   OutStream << "*Grid area                                                 \t: " << m_dExtCRSGridArea << " m^2" << endl;
   OutStream << setiosflags(ios::fixed) << setprecision(2);
   OutStream << "*Grid area                                                 \t: " << m_dExtCRSGridArea * 1e-6 << " km^2" << endl;
   OutStream << endl;


   // ---------------------------------------------------- Vector GIS stuff ------------------------------------------------------
   OutStream << "Vector GIS Input Files" << endl;
   if (m_strInitialCoastlineFile.empty())
      OutStream << " None" << endl;
   else
   {
      OutStream << " Initial Coastline file                                    \t: " << m_strInitialCoastlineFile << endl;
      OutStream << " OGR Initial Coastline file driver code                    \t: " << m_strOGRICDriverCode << endl;
     // OutStream << " OGR Initial Coastline file data type                      \t: " << m_strOGRICDataType << endl;
     // OutStream << " OGR Initial Coastline file data value                     \t: " << m_strOGRICDataValue << endl;
     // OutStream << " OGR Initial Coastline file geometry                       \t: " << m_strOGRICGeometry << endl;
      OutStream << endl;
   }
   OutStream << endl;

   // -------------------------------------------------------- Other data --------------------------------------------------------
   OutStream << "Other Input Data" << endl;

   OutStream << " Still water level used to extract shoreline               \t: " << resetiosflags(ios::floatfield) << setiosflags(ios::fixed) << setprecision(1) << m_dStillWaterLevel << " m" << endl;
   OutStream << " Length of coastline normals                               \t: " << m_dCoastNormalLength << " m" << endl;
   OutStream << " Vertical tolerance avoid false CliffTops/Toes             \t: " << resetiosflags(ios::floatfield) << setiosflags(ios::fixed) << setprecision(3) << m_dEleTolerance << " m" << endl;
   OutStream << endl;
   OutStream << endl << endl;

}



/*==============================================================================================================================

 Save a coastline-normal profile

==============================================================================================================================*/
int CDelineation::nSaveProfile(int const nProfile, int const nCoast, int const nProfSize, vector<double>* const pdVDistXY, vector<double>* const pdVZ, vector<C2DIPoint>* const pPtVGridProfile, vector<double>* const pdetrendedZ)
{
   // TODO make this more efficient, also give warnings if no profiles will be output
   if (! bWriteProfileData(nCoast, nProfile, nProfSize, pdVDistXY, pdVZ, pPtVGridProfile, pdetrendedZ))
   return RTN_ERR_PROFILEWRITE;
  
   return RTN_OK;
}


/*==============================================================================================================================

 Writes values for a single profile, for checking purposes

==============================================================================================================================*/
#if !defined(_SAGA_MSW) && !defined(_SAGA_LINUX)
bool CDelineation::bWriteProfileData(int const nCoast, int const nProfile, int const nProfSize, vector<double>* const pdVDistXY, vector<double>* const pdVZ, vector<C2DIPoint>* const pPtVGridProfile, vector<double>* const pdetrendedZ)
{
   string strFName = m_strOutPath;
   strFName.append("coast_");
   strFName.append(NumberToString(nCoast));
   strFName.append("_profile_");
   char szNumTmp1[7] = "";
   pszLongToSz(nProfile, szNumTmp1, 7);          // Pad with zeros
   strFName.append(pszTrimLeft(szNumTmp1));
   strFName.append("_");
   strFName.append(m_strRunName);
   strFName.append(".csv");

   ofstream OutProfStream;
   OutProfStream.open(strFName.c_str(), ios::out | ios::trunc);
   if (! OutProfStream)
   {
      // Error, cannot open file
      cerr << ERR << "cannot open " << strFName << " for output" << endl;
      return false;
   }

   OutProfStream << "\"Dist\", \"X\", \"Y\", \"Z \", \"detrendZ \", \"For profile " << nProfile << " from coastline " << nCoast << "\"" << endl;
   for (int i = 0; i < nProfSize; i++)
   {
      double dX = dGridCentroidXToExtCRSX(pPtVGridProfile->at(i).nGetX());
      double dY = dGridCentroidYToExtCRSY(pPtVGridProfile->at(i).nGetY());

      OutProfStream << pdVDistXY->at(i) << ", " << dX << ", " << dY << ", " << pdVZ->at(i) << ", " << pdetrendedZ->at(i) << endl;
   }

   OutProfStream.close();

   return true;
}
#else // #if defined(_SAGA_MSW) || defined(_SAGA_LINUX)
bool CDelineation::bWriteProfileData(int const nCoast, int const nProfile, int const nProfSize, vector<double>* const pdVDistXY, vector<double>* const pdVZ, vector<C2DIPoint>* const pPtVGridProfile, vector<double>* const pdetrendedZ)
{
	if( m_strOutPath.size() && SG_Dir_Exists(m_strOutPath.c_str()) )
	{
		string strFName = m_strOutPath;
		strFName.append("coast_");
		strFName.append(NumberToString(nCoast));
		strFName.append("_profile_");
		char szNumTmp1[7] = "";
		pszLongToSz(nProfile, szNumTmp1, 7);          // Pad with zeros
		strFName.append(pszTrimLeft(szNumTmp1));
		strFName.append("_");
		strFName.append(m_strRunName);
		strFName.append(".csv");

		ofstream OutProfStream;
		OutProfStream.open(strFName.c_str(), ios::out | ios::trunc);
		if (! OutProfStream)
		{
			// Error, cannot open file
			cerr << ERR << "cannot open " << strFName << " for output" << endl;
			return false;
		}

		OutProfStream << "\"Dist\", \"X\", \"Y\", \"Z \", \"detrendZ \", \"For profile " << nProfile << " from coastline " << nCoast << "\"" << endl;
		for (int i = 0; i < nProfSize; i++)
		{
			double dX = dGridCentroidXToExtCRSX(pPtVGridProfile->at(i).nGetX());
			double dY = dGridCentroidYToExtCRSY(pPtVGridProfile->at(i).nGetY());

			OutProfStream << pdVDistXY->at(i) << ", " << dX << ", " << dY << ", " << pdVZ->at(i) << ", " << pdetrendedZ->at(i) << endl;
		}

		OutProfStream.close();
	}

	//-----------------------------------------------------
	CSG_Table	&Profiles	= *(*m_pParameters)("PROFILES")->asTable();

	if( !Profiles.Get_Count() )
	{
		Profiles.Destroy();

		Profiles.Add_Field("COASTLINE", SG_DATATYPE_Int   );
		Profiles.Add_Field("PROFILE"  , SG_DATATYPE_Int   );
		Profiles.Add_Field("DISTANCE" , SG_DATATYPE_Double);
		Profiles.Add_Field("X"        , SG_DATATYPE_Double);
		Profiles.Add_Field("Y"        , SG_DATATYPE_Double);
		Profiles.Add_Field("Z"        , SG_DATATYPE_Double);
		Profiles.Add_Field("Z_DETREND", SG_DATATYPE_Double);
	}

	for(int i=0; i<nProfSize; i++)
	{
		CSG_Table_Record	&Record	= *Profiles.Add_Record();

		Record[0]	= nCoast;
		Record[1]	= nProfile;
		Record[2]	= pdVDistXY  ->at(i);
		Record[3]	= dGridCentroidXToExtCRSX(pPtVGridProfile->at(i).nGetX());
		Record[4]	= dGridCentroidYToExtCRSY(pPtVGridProfile->at(i).nGetY());
		Record[5]	= pdVZ       ->at(i);
		Record[6]	= pdetrendedZ->at(i);
	}

	//-----------------------------------------------------
	return( true );
}

#endif // #if defined(_SAGA_MSW) || defined(_SAGA_LINUX)



/*==============================================================================================================================

 Writes end-of-run information to Out, Log and time-series files

==============================================================================================================================*/
int CDelineation::nWriteEndRunDetails(void)
{
   // Final write to time series CSV files
   //if (! bWriteTSFiles())
   //   return (RTN_ERR_TIMESERIES_FILE_WRITE);

   // Save the values from the RasterGrid array into raster GIS files
   if (! bSaveAllRasterGISFiles())
      return (RTN_ERR_RASTER_FILE_WRITE);

   // Save the vector GIS files
   if (! bSaveAllVectorGISFiles())
      return (RTN_ERR_VECTOR_FILE_WRITE);

   OutStream << " GIS" << m_nGISSave << endl;

   // Print out run totals etc.
   OutStream << PERITERHEAD1 << endl;
   OutStream << PERITERHEAD2 << endl;
   OutStream << PERITERHEAD3 << endl;
   OutStream << PERITERHEAD4 << endl;
   OutStream << PERITERHEAD5 << endl;

   OutStream << setiosflags(ios::fixed) << setprecision(2);
   OutStream << endl << endl;
 
   // Calculate statistics re. memory usage etc.
   CalcProcessStats();
   OutStream << endl << "END OF RUN" << endl;
   LogStream << endl << "END OF RUN" << endl;

   // Need to flush these here (if we don't, the buffer may not get written)
   LogStream.flush();
   OutStream.flush();

   return RTN_OK;
}


