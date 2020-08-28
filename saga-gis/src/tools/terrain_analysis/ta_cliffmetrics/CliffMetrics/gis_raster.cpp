/*!
 *
 * \file gis_raster.cpp
 * \brief These functions use GDAL to read and write raster GIS files in several formats. This version will build with GDAL version 2
 * \details TODO A more detailed description of these routines.
 * \author David Favis-Mortlock
 * \author Andres Payo
 * \author Jim Hall
 * \date 2016
 * \copyright GNU General Public License
 *
 */

/*===============================================================================================================================

 This file is part of CliffMetrics, the Coastal Modelling Environment.

 CliffMetrics is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

===============================================================================================================================*/
//#include <assert.h>

#include <iostream>
using std::cout;
using std::cerr;
using std::endl;
using std::ios;

#include <sstream>
using std::stringstream;

#if !defined(_SAGA_MSW) && !defined(_SAGA_LINUX)
#include <gdal_priv.h>
#endif  // #if !defined(_SAGA_MSW) && !defined(_SAGA_LINUX)

#include "cliffmetrics.h"
#include "delineation.h"
#include "raster_grid.h"


/*==============================================================================================================================

 Reads a raster DTM elevation data to the Cell array

===============================================================================================================================*/
#if !defined(_SAGA_MSW) && !defined(_SAGA_LINUX)
int CDelineation::nReadDTMData(void)
{
   // Use GDAL to create a dataset object, which then opens the DTM file
   GDALDataset* pGDALDataset = NULL;
   pGDALDataset = (GDALDataset *) GDALOpen(m_strDTMFile.c_str(), GA_ReadOnly);
   if (NULL == pGDALDataset)
   {
      // Can't open file (note will already have sent GDAL error message to stdout)
      cerr << ERR << "cannot open " << m_strDTMFile << " for input: " << CPLGetLastErrorMsg() << endl;
      return RTN_ERR_DEMFILE;
   }

   // Opened OK, so get GDAL basement DEM dataset information
   m_strGDALDTMDriverCode = pGDALDataset->GetDriver()->GetDescription();
   m_strGDALDTMDriverDesc = pGDALDataset->GetDriver()->GetMetadataItem(GDAL_DMD_LONGNAME);
   m_strGDALDTMProjection = pGDALDataset->GetProjectionRef();

   // If we have reference units, then check that they are in meters (note US spelling)
   if (! m_strGDALDTMProjection.empty())
   {
      string strTmp = strToLower(&m_strGDALDTMProjection);
      if (strTmp.find("meter") == string::npos)
      {
         // error: x-y values must be in metres
         cerr << ERR << "GIS file x-y values (" << m_strGDALDTMProjection << ") in " << m_strDTMFile << " must be in metres" << endl;
         return RTN_ERR_DEMFILE;
      }
   }

   // Now get dataset size, and do some rudimentary checks
   m_nXGridMax = pGDALDataset->GetRasterXSize();
   if (m_nXGridMax == 0)
   {
      // Error: silly number of columns specified
      cerr << ERR << "invalid number of columns (" << m_nXGridMax << ") in " << m_strDTMFile << endl;
      return RTN_ERR_DEMFILE;
   }

   m_nYGridMax = pGDALDataset->GetRasterYSize();
   if (m_nYGridMax == 0)
   {
      // Error: silly number of rows specified
      cerr << ERR << "invalid number of rows (" << m_nYGridMax << ") in " << m_strDTMFile << endl;
      return RTN_ERR_DEMFILE;
   }

   // Get geotransformation info (see http://www.gdal.org/classGDALDataset.html)
   if (CE_Failure == pGDALDataset->GetGeoTransform(m_dGeoTransform))
   {
      // Can't get geotransformation (note will already have sent GDAL error message to stdout)
      cerr << ERR << CPLGetLastErrorMsg() << " in " << m_strDTMFile << endl;
      return RTN_ERR_DEMFILE;
   }

   // Get the X and Y cell sizes, in external CRS units. Note that while the cell is supposed to be square, it may not be exactly so due to oddities with some GIS calculations
   double dCellSideX = tAbs(m_dGeoTransform[1]);
   double dCellSideY = tAbs(m_dGeoTransform[5]);

   // Check that the cell is more or less square
   if (! bFPIsEqual(dCellSideX, dCellSideY, 1e-2))
   {
      // Error: cell is not square enough
      cerr << ERR << "cell is not square in " << m_strDTMFile << ", is " << dCellSideX << " x " << dCellSideY << endl;
      return (RTN_ERR_RASTER_FILE_READ);
   }

   // Calculate the average length of cell side, the cell's diagonal, and the area of a cell (in external CRS units)
   m_dCellSide = (dCellSideX + dCellSideY) / 2.0;
   m_dCellArea = m_dCellSide * m_dCellSide;
   m_dCellDiagonal = hypot(m_dCellSide, m_dCellSide);

   // And calculate the inverse values
   m_dInvCellSide = 1 / m_dCellSide;
   m_dInvCellDiagonal = 1 / m_dCellDiagonal;

   // Save some values in external CRS
   m_dNorthWestXExtCRS = m_dGeoTransform[0] - (m_dGeoTransform[1] / 2);
   m_dNorthWestYExtCRS = m_dGeoTransform[3] - (m_dGeoTransform[5] / 2);
   m_dSouthEastXExtCRS = m_dGeoTransform[0] + (m_nXGridMax * m_dGeoTransform[1]) + (m_dGeoTransform[1] / 2);
   m_dSouthEastYExtCRS = m_dGeoTransform[3] + (m_nYGridMax * m_dGeoTransform[5]) + (m_dGeoTransform[5] / 2);

   // And calc the grid area in external CRS units
   m_dExtCRSGridArea = tAbs(m_dNorthWestXExtCRS - m_dSouthEastXExtCRS) * tAbs(m_dNorthWestYExtCRS * m_dSouthEastYExtCRS);

   // Now get GDAL raster band information
   GDALRasterBand* pGDALBand = NULL;
   int nBlockXSize = 0, nBlockYSize = 0;
   pGDALBand = pGDALDataset->GetRasterBand(1);
   pGDALBand->GetBlockSize(&nBlockXSize, &nBlockYSize);
   m_strGDALDTMDataType = GDALGetDataTypeName(pGDALBand->GetRasterDataType());

   // If we have value units, then check them
   char szUnits[10] = "";

   strcpy(szUnits, pGDALBand->GetUnitType());

   if ((*szUnits != '\0') && strcmp(szUnits, "m"))
   {
      // Error: value units must be m
      cerr << ERR << "DTM vertical units are (" << szUnits << " ) in " << m_strDTMFile << ", should be 'm'" << endl;
      return RTN_ERR_DEMFILE;
   }

   // Next allocate memory for two 2D arrays of raster cell objects: tell the user what is happening
   AnnounceAllocateMemory();
   int nRet = m_pRasterGrid->nCreateGrid();
   if (nRet != RTN_OK)
      return nRet;

   // Allocate memory for a 1D floating-point array, to hold the scan line for GDAL
   float* pfScanline = new float[m_nXGridMax];
   if (NULL == pfScanline)
   {
      // Error, can't allocate memory
      cerr << ERR << "cannot allocate memory for " << m_nXGridMax << " x 1D array" << endl;
      return (RTN_ERR_MEMALLOC);
   }

   // Now read in the data
   for (int j = 0; j < m_nYGridMax; j++)
   {
      // Read scanline
      if (CE_Failure == pGDALBand->RasterIO(GF_Read, 0, j, m_nXGridMax, 1, pfScanline, m_nXGridMax, 1, GDT_Float32, 0, 0))
      {
         // Error while reading scanline
         cerr << ERR << CPLGetLastErrorMsg() << " in " << m_strDTMFile << endl;
         return RTN_ERR_DEMFILE;
      }

      // All OK, so read scanline into cell elevations
      for (int i = 0; i < m_nXGridMax; i++)
         m_pRasterGrid->pGetCell(i, j)->SetBasementElev(pfScanline[i]);
   }

   // Finished, so get rid of dataset object
   GDALClose(pGDALDataset);

   // Get rid of memory allocated to this array
   delete[] pfScanline;

   return RTN_OK;
}

#else // #if defined(_SAGA_MSW) || defined(_SAGA_LINUX)
int CDelineation::nReadDTMData(CSG_Grid *pDEM)
{
	m_dGeoTransform[0] = pDEM->Get_XMin(true); m_dGeoTransform[1] =  pDEM->Get_Cellsize(); m_dGeoTransform[2] = 0.;
	m_dGeoTransform[3] = pDEM->Get_YMax(true); m_dGeoTransform[5] = -pDEM->Get_Cellsize(); m_dGeoTransform[4] = 0.;

	// Now get dataset size, and do some rudimentary checks
	m_nXGridMax = pDEM->Get_NX();	//	m_nXGridMax = pGDALDataset->GetRasterXSize();
	m_nYGridMax = pDEM->Get_NY();	//	m_nYGridMax = pGDALDataset->GetRasterYSize();

	// Get the X and Y cell sizes, in external CRS units. Note that while the cell is supposed to be square, it may not be exactly so due to oddities with some GIS calculations
	m_dCellSide = pDEM->Get_Cellsize();
	m_dCellArea = pDEM->Get_Cellarea();
	m_dCellDiagonal = hypot(m_dCellSide, m_dCellSide);

	// And calculate the inverse values
	m_dInvCellSide = 1 / m_dCellSide;
	m_dInvCellDiagonal = 1 / m_dCellDiagonal;

	// Save some values in external CRS
	m_dNorthWestXExtCRS = pDEM->Get_XMin(true);	// m_dGeoTransform[0] - (m_dGeoTransform[1] / 2);
	m_dNorthWestYExtCRS = pDEM->Get_YMax(true);	// m_dGeoTransform[3] - (m_dGeoTransform[5] / 2);
	m_dSouthEastXExtCRS = pDEM->Get_XMax(true);	// m_dGeoTransform[0] + (m_nXGridMax * m_dGeoTransform[1]) + (m_dGeoTransform[1] / 2);
	m_dSouthEastYExtCRS = pDEM->Get_YMin(true);	// m_dGeoTransform[3] + (m_nYGridMax * m_dGeoTransform[5]) + (m_dGeoTransform[5] / 2);

	// And calc the grid area in external CRS units
	m_dExtCRSGridArea = tAbs(m_dNorthWestXExtCRS - m_dSouthEastXExtCRS) * tAbs(m_dNorthWestYExtCRS - m_dSouthEastYExtCRS);

	// Next allocate memory for two 2D arrays of raster cell objects: tell the user what is happening
	int nRet = m_pRasterGrid->nCreateGrid();
	if (nRet != RTN_OK)
		return nRet;

	// Now read in the data
	for(int y=0, yy=m_nYGridMax-1; y<m_nYGridMax; y++, yy--)
	{
		for(int x=0; x<m_nXGridMax; x++)
		{
			m_pRasterGrid->pGetCell(x, y)->SetBasementElev(pDEM->asDouble(x, yy));
		}
	}

	return RTN_OK;
}

#endif // #if defined(_SAGA_MSW) || defined(_SAGA_LINUX)




/*==============================================================================================================================

 Writes floating point GIS raster files using GDAL, using data from the RasterGrid array

===============================================================================================================================*/
#if !defined(_SAGA_MSW) && !defined(_SAGA_LINUX)
bool CDelineation::bWriteRasterGISFloat(int const nDataItem, string const* strPlotTitle)
{
   // Begin constructing the file name for this save
   string strFilePathName(m_strOutPath);

   switch (nDataItem)
   {
      case (PLOT_SEDIMENT_TOP_ELEV):
      {
         strFilePathName.append(RASTER_SEDIMENT_TOP_NAME);
         break;
      }
   }

   // Finally, maybe append the extension
   if (! m_strGDALRasterOutputDriverExtension.empty())
   {
      strFilePathName.append(".");
      strFilePathName.append(m_strGDALRasterOutputDriverExtension);
   }

   GDALDriver* pDriver;
   GDALDataset* pDataSet;
   if (m_bGDALCanCreate)
   {
      // The user-requested raster driver supports the Create() method
      pDriver = GetGDALDriverManager()->GetDriverByName(m_strRasterGISOutFormat.c_str());
      pDataSet = pDriver->Create(strFilePathName.c_str(), m_nXGridMax, m_nYGridMax, 1, m_GDALWriteFloatDataType, m_papszGDALRasterOptions);
      if (NULL == pDataSet)
      {
         // Error, couldn't create file
         cerr << ERR << "cannot create " << m_strRasterGISOutFormat << " file named " << strFilePathName << "\n" << CPLGetLastErrorMsg() << endl;
         return false;
      }
   }
   else
   {
      // The user-requested raster driver does not support the Create() method, so we must first create a memory-file dataset
      pDriver = GetGDALDriverManager()->GetDriverByName("MEM");
      pDataSet = pDriver->Create("", m_nXGridMax, m_nYGridMax, 1, m_GDALWriteFloatDataType, NULL);
      if (NULL == pDataSet)
      {
         // Couldn't create in-memory file dataset
         cerr << ERR << "cannot create in-memory file for " << m_strRasterGISOutFormat << " file named " << strFilePathName << "\n" << CPLGetLastErrorMsg() << endl;
         return false;
      }
   }

   // Set projection info for output dataset (will be same as was read in from DEM)
   CPLPushErrorHandler(CPLQuietErrorHandler);                        // needed to get next line to fail silently, if it fails
   pDataSet->SetProjection(m_strGDALBasementDEMProjection.c_str());       // will fail for some formats
   CPLPopErrorHandler();

   // Set geotransformation info for output dataset (will be same as was read in from DEM)
   if (CE_Failure == pDataSet->SetGeoTransform(m_dGeoTransform))
      LogStream << WARN << "cannot write geotransformation information to " << m_strRasterGISOutFormat << " file named " << strFilePathName << "\n" << CPLGetLastErrorMsg() << endl;

   // Allocate memory for a 1D array, to hold the floating point raster band data for GDAL
   float* pfRaster;
   pfRaster = new float[m_nXGridMax * m_nYGridMax];
   if (NULL == pfRaster)
   {
      // Error, can't allocate memory
      cerr << ERR << "cannot allocate memory for " << m_nXGridMax * m_nYGridMax << " x 1D floating-point array for " << m_strRasterGISOutFormat << " file named " << strFilePathName << endl;
      return (RTN_ERR_MEMALLOC);
   }

   bool bScaleOutput = false;
   double
      dRangeScale = 0,
      dDataMin = 0;

   if (! m_bGDALCanWriteFloat)
   {
      double dDataMax = 0;

      // The output file format cannot handle floating-point numbers, so we may need to scale the output
      GetRasterOutputMinMax(nDataItem, dDataMin, dDataMax);

      double
         dDataRange = dDataMax - dDataMin,
         dWriteRange = m_lGDALMaxCanWrite - m_lGDALMinCanWrite;

      if (dDataRange > 0)
         dRangeScale = dWriteRange / dDataRange;

      // If we are attempting to write values which are outside this format's allowable range, and the user has set the option, then scale the output
      if (((dDataMin < m_lGDALMinCanWrite) || (dDataMax > m_lGDALMaxCanWrite)) && m_bScaleRasterOutput)
         bScaleOutput = true;
   }

   // Fill the array
   int n = 0;
   double dTmp = 0;
   for (int nY = 0; nY < m_nYGridMax; nY++)
   {
      for (int nX = 0; nX < m_nXGridMax; nX++)
      {
         switch (nDataItem)
         {
            case (PLOT_SEDIMENT_TOP_ELEV):
            {
               dTmp = m_pRasterGrid->pGetCell(nX, nY)->dGetSedimentTopElev();
               break;
            }

          }

         // If necessary, scale this value
         if (bScaleOutput)
         {
            if (dTmp == DBL_NODATA)
               dTmp = 0;         // TODO Improve this
            else
               dTmp = dRound(m_lGDALMinCanWrite + (dRangeScale * (dTmp - dDataMin)));
         }

         // Write this value to the array
         pfRaster[n++] = dTmp;
      }
   }

   // Create a single raster band
   GDALRasterBand* pBand = pDataSet->GetRasterBand(1);

   // Set value units for this band
   char szUnits[10] = "";
   switch (nDataItem)
   {
      case (PLOT_SEDIMENT_TOP_ELEV):
      {
         strcpy(szUnits, "m");
         break;
      }
   }

   CPLPushErrorHandler(CPLQuietErrorHandler);                  // Needed to get next line to fail silently, if it fails
   pBand->SetUnitType(szUnits);                                // Not supported for some GIS formats
   CPLPopErrorHandler();

   // Tell the output dataset about NODATA (missing values)
   CPLPushErrorHandler(CPLQuietErrorHandler);                  // Needed to get next line to fail silently, if it fails
   pBand->SetNoDataValue(DBL_NODATA);                          // Will fail for some formats
   CPLPopErrorHandler();

   // Construct the description
   string strDesc(*strPlotTitle);


   // Set the GDAL description
   pBand->SetDescription(strDesc.c_str());

   // Now write the data
   if (CE_Failure == pBand->RasterIO(GF_Write, 0, 0, m_nXGridMax, m_nYGridMax, pfRaster, m_nXGridMax, m_nYGridMax, GDT_Float32, 0, 0))
   {
      // Write error, better error message
      cerr << ERR << "cannot write data for " << m_strRasterGISOutFormat << " file named " << strFilePathName << "\n" << CPLGetLastErrorMsg() << endl;
      delete[] pfRaster;
      return false;
   }

   // Calculate statistics for this band
   double dMin, dMax, dMean, dStdDev;
   CPLPushErrorHandler(CPLQuietErrorHandler);        // needed to get next line to fail silently, if it fails
   pBand->ComputeStatistics(false, &dMin, &dMax, &dMean, &dStdDev, NULL, NULL);
   CPLPopErrorHandler();

   // And then write the statistics
   CPLPushErrorHandler(CPLQuietErrorHandler);        // needed to get next line to fail silently, if it fails
   pBand->SetStatistics(dMin, dMax, dMean, dStdDev);
   CPLPopErrorHandler();

   if (! m_bGDALCanCreate)
   {
      // Since the user-selected raster driver cannot use the Create() method, we have been writing to a dataset created by the in-memory driver. So now we need to use CreateCopy() to copy this in-memory dataset to a file in the user-specified raster driver format
      GDALDriver* pOutDriver = GetGDALDriverManager()->GetDriverByName(m_strRasterGISOutFormat.c_str());
      GDALDataset* pOutDataSet = pOutDriver->CreateCopy(strFilePathName.c_str(), pDataSet, FALSE, m_papszGDALRasterOptions, NULL, NULL);
      if (NULL == pOutDataSet)
      {
         // Couldn't create file
         cerr << ERR << "cannot create " << m_strRasterGISOutFormat << " file named " << strFilePathName << "\n" << CPLGetLastErrorMsg() << endl;
         return false;
      }

      // Get rid of this user-selected dataset object
      GDALClose(pOutDataSet);
   }

   // Get rid of dataset object
   GDALClose(pDataSet);

   // Also get rid of memory allocated to this array
   delete[] pfRaster;

   return true;
}

#else // #if defined(_SAGA_MSW) || defined(_SAGA_LINUX)
bool CDelineation::bWriteRasterGISFloat(int const nDataItem, CSG_Grid *pGrid)
{
	if( !pGrid || pGrid->Get_NX() != m_nXGridMax || pGrid->Get_NY() != m_nYGridMax )
	{
		return( false );
	}

	// Fill the array
	for(int y=0, yy=m_nYGridMax-1; y<m_nYGridMax; y++, yy--)
	{
		for(int x=0; x<m_nXGridMax; x++)
		{
			switch( nDataItem )
			{
			case (PLOT_SEDIMENT_TOP_ELEV):
				pGrid->Set_Value(x, yy, m_pRasterGrid->pGetCell(x, y)->dGetSedimentTopElev());
				break;
			}
		}
	}

	return( true );
}

#endif // #if defined(_SAGA_MSW) || defined(_SAGA_LINUX)



/*==============================================================================================================================

 Writes integer GIS raster files using GDAL, using data from the RasterGrid array

===============================================================================================================================*/
#if !defined(_SAGA_MSW) && !defined(_SAGA_LINUX)
bool CDelineation::bWriteRasterGISInt(int const nDataItem, string const* strPlotTitle, double const dElev)
{
   // Begin constructing the file name for this save
   string strFilePathName(m_strOutPath);
   stringstream ststrTmp;

   switch (nDataItem)
   {

      case (PLOT_RASTER_COAST):
      {
         strFilePathName.append(RASTER_COAST_NAME);
         break;
      }

      case (PLOT_RASTER_NORMAL):
      {
         strFilePathName.append(RASTER_COAST_NORMAL_NAME);
         break;
      }

   }

   // Append the 'save number' to the filename
  /* strFilePathName.append("_");
   if (m_nGISSave > 99)
   {
      // For save numbers of three or more digits, don't prepend zeros (note 10 digits is max)
      char szNumTmp[10] = "";
      strFilePathName.append(pszTrimLeft(pszLongToSz(m_nGISSave, szNumTmp, 10)));
   }
   else
   {
      // Prepend zeros to the save number
      char szNumTmp[3] = "";
      pszLongToSz(m_nGISSave, szNumTmp, 3);
      strFilePathName.append(pszTrimLeft(szNumTmp));
   }*/

   // Finally, maybe append the extension
   if (! m_strGDALRasterOutputDriverExtension.empty())
   {
      strFilePathName.append(".");
      strFilePathName.append(m_strGDALRasterOutputDriverExtension);
   }

   GDALDriver* pDriver;
   GDALDataset* pDataSet;
   if (m_bGDALCanCreate)
   {
      // The user-requested raster driver supports the Create() method
      pDriver = GetGDALDriverManager()->GetDriverByName(m_strRasterGISOutFormat.c_str());
      pDataSet = pDriver->Create(strFilePathName.c_str(), m_nXGridMax, m_nYGridMax, 1, m_GDALWriteIntDataType, m_papszGDALRasterOptions);
      if (NULL == pDataSet)
      {
         // Couldn't create file
         cerr << ERR << "cannot create " << m_strRasterGISOutFormat << " file named " << strFilePathName << "\n" << CPLGetLastErrorMsg() << endl;
         return false;
      }
   }
   else
   {
      // The user-requested raster driver does not support the Create() method, so we must first create a memory-file dataset
      pDriver = GetGDALDriverManager()->GetDriverByName("MEM");
      pDataSet = pDriver->Create("", m_nXGridMax, m_nYGridMax, 1, m_GDALWriteIntDataType, NULL);
      if (NULL == pDataSet)
      {
         // Couldn't create in-memory file dataset
         cerr << ERR << "cannot create in-memory file for " << m_strRasterGISOutFormat << " file named " << strFilePathName << "\n" << CPLGetLastErrorMsg() << endl;
         return false;
      }
   }

   // Set projection info for output dataset (will be same as was read in from DEM)
   CPLPushErrorHandler(CPLQuietErrorHandler);                              // Needed to get next line to fail silently, if it fails
   pDataSet->SetProjection(m_strGDALBasementDEMProjection.c_str());     // Will fail for some formats
   CPLPopErrorHandler();

   // Set geotransformation info for output dataset (will be same as was read in from DEM)
   if (CE_Failure == pDataSet->SetGeoTransform(m_dGeoTransform))
      LogStream << WARN << "cannot write geotransformation information to " << m_strRasterGISOutFormat << " file named " << strFilePathName << "\n" << CPLGetLastErrorMsg() << endl;

   // Allocate memory for a 1D array, to hold the integer raster band data for GDAL
   int* pnRaster;
   pnRaster = new int[m_nXGridMax * m_nYGridMax];
   if (NULL == pnRaster)
   {
      // Error, can't allocate memory
      cerr << ERR << "cannot allocate memory for " << m_nXGridMax * m_nYGridMax << " x 1D integer array for " << m_strRasterGISOutFormat << " file named " << strFilePathName << endl;
      return (RTN_ERR_MEMALLOC);
   }

   bool bScaleOutput = false;
   double
      dRangeScale = 0,
      dDataMin = 0;

   if (! m_bGDALCanWriteInt32)
   {
      double dDataMax = 0;

      // The output file format cannot handle 32-bit integers, so we may have to scale the output
      GetRasterOutputMinMax(nDataItem, dDataMin, dDataMax);

      double
         dDataRange = dDataMax - dDataMin,
         dWriteRange = m_lGDALMaxCanWrite - m_lGDALMinCanWrite;

      if (dDataRange > 0)
         dRangeScale = dWriteRange / dDataRange;

      // If we are attempting to write values which are outside this format's allowable range, and the user has set the option, then scale the output
      if (((dDataMin < m_lGDALMinCanWrite) || (dDataMax > m_lGDALMaxCanWrite)) && m_bScaleRasterOutput)
         bScaleOutput = true;
   }

   // Fill the array
   int nTmp  = 0, n = 0;
   for (int nY = 0; nY < m_nYGridMax; nY++)
   {
      for (int nX = 0; nX < m_nXGridMax; nX++)
      {
         switch (nDataItem)
         {
            case (PLOT_RASTER_COAST):
            {
               nTmp = (m_pRasterGrid->pGetCell(nX, nY)->bIsCoastline() ? 1 : 0);
               break;
            }

            case (PLOT_RASTER_NORMAL):
            {
               nTmp = (m_pRasterGrid->pGetCell(nX, nY)->bIsNormalProfile() ? 1 : 0);
               break;
            }

         }

         // If necessary, scale this value
         if (bScaleOutput)
            nTmp = dRound(m_lGDALMinCanWrite + (dRangeScale * (nTmp - dDataMin)));

         // Write it to the array
         pnRaster[n++] = nTmp;
      }
   }

   // Create a single raster band
   GDALRasterBand* pBand = pDataSet->GetRasterBand(1);

   // Set value units for this band
   string strUnits;
   switch (nDataItem)
   {
      case (PLOT_RASTER_COAST):
      case (PLOT_RASTER_NORMAL):
      {
         strUnits = "none";
      }
   }

   CPLPushErrorHandler(CPLQuietErrorHandler);                  // Needed to get next line to fail silently, if it fails
   pBand->SetUnitType(strUnits.c_str());                       // Not supported for some GIS formats
   CPLPopErrorHandler();

   // Tell the output dataset about NODATA (missing values)
   CPLPushErrorHandler(CPLQuietErrorHandler);                  // Needed to get next line to fail silently, if it fails
   pBand->SetNoDataValue(INT_NODATA);                          // Will fail for some formats
   CPLPopErrorHandler();

   // Construct the description
   string strDesc(*strPlotTitle);
   if (nDataItem == PLOT_SLICE)
   {
      ststrTmp.clear();
      ststrTmp << dElev << "m, ";
      strDesc.append(ststrTmp.str());
   }
   
   // Set the GDAL description
   pBand->SetDescription(strDesc.c_str());

   // Set raster category names
   char** papszCategoryNames = NULL;
   switch (nDataItem)
   {
     case (PLOT_RASTER_COAST):
      {
         papszCategoryNames = CSLAddString(papszCategoryNames, "Not coastline");
         papszCategoryNames = CSLAddString(papszCategoryNames, "Coastline");
         break;
      }

      case (PLOT_RASTER_NORMAL):
      {
         papszCategoryNames = CSLAddString(papszCategoryNames, "Not coastline-normal profile");
         papszCategoryNames = CSLAddString(papszCategoryNames, "Coastline-normal profile");
         break;
      }
   }

   CPLPushErrorHandler(CPLQuietErrorHandler);        // Needed to get next line to fail silently, if it fails
   pBand->SetCategoryNames(papszCategoryNames);      // Not supported for some GIS formats
   CPLPopErrorHandler();

   // Now write the data
   if (CE_Failure == pBand->RasterIO(GF_Write, 0, 0, m_nXGridMax, m_nYGridMax, pnRaster, m_nXGridMax, m_nYGridMax, GDT_Int32, 0, 0))
   {
      // Write error
      cerr << ERR << "cannot write data for " << m_strRasterGISOutFormat << " file named " << strFilePathName << "\n" << CPLGetLastErrorMsg() << endl;
      delete[] pnRaster;
      return false;
   }

   // Calculate statistics for this band
   double dMin, dMax, dMean, dStdDev;
   CPLPushErrorHandler(CPLQuietErrorHandler);        // Needed to get next line to fail silently, if it fails
   pBand->ComputeStatistics(false, &dMin, &dMax, &dMean, &dStdDev, NULL, NULL);
   CPLPopErrorHandler();

   // And then write the statistics
   CPLPushErrorHandler(CPLQuietErrorHandler);        // Needed to get next line to fail silently, if it fails
   pBand->SetStatistics(dMin, dMax, dMean, dStdDev);
   CPLPopErrorHandler();

   if (! m_bGDALCanCreate)
   {
      // Since the user-selected raster driver cannot use the Create() method, we have been writing to a dataset created by the in-memory driver. So now we need to use CreateCopy() to copy this in-memory dataset to a file in the user-specified raster driver format
      GDALDriver* pOutDriver = GetGDALDriverManager()->GetDriverByName(m_strRasterGISOutFormat.c_str());
      GDALDataset* pOutDataSet = pOutDriver->CreateCopy(strFilePathName.c_str(), pDataSet, FALSE, m_papszGDALRasterOptions, NULL, NULL);
      if (NULL == pOutDataSet)
      {
         // Couldn't create file
         cerr << ERR << "cannot create " << m_strRasterGISOutFormat << " file named " << strFilePathName << "\n" << CPLGetLastErrorMsg() << endl;
         return false;
      }

      // Get rid of this user-selected dataset object
      GDALClose(pOutDataSet);
   }

   // Get rid of dataset object
   GDALClose(pDataSet);

   // Get rid of memory allocated to this array
   delete[] pnRaster;

   return true;
}

#else // #if defined(_SAGA_MSW) || defined(_SAGA_LINUX)
bool CDelineation::bWriteRasterGISInt(int const nDataItem, class CSG_Grid *pGrid, double const dElev)
{
	if( !pGrid || pGrid->Get_NX() != m_nXGridMax || pGrid->Get_NY() != m_nYGridMax )
	{
		return( false );
	}

	// Fill the array
	for(int y=0, yy=m_nYGridMax-1; y<m_nYGridMax; y++, yy--)
	{
		for(int x=0; x<m_nXGridMax; x++)
		{
			switch( nDataItem )
			{
			case (PLOT_RASTER_COAST):
				pGrid->Set_Value(x, yy, m_pRasterGrid->pGetCell(x, y)->bIsCoastline    () ? 1 : 0);
				break;

			case (PLOT_RASTER_NORMAL):
				pGrid->Set_Value(x, yy, m_pRasterGrid->pGetCell(x, y)->bIsNormalProfile() ? 1 : 0);
				break;
			}
		}
	}

	if( 0 )	// Set no-data value to zero
	{
		switch( nDataItem )
		{
		case (PLOT_RASTER_COAST):
		case (PLOT_RASTER_NORMAL):
			pGrid->Set_NoData_Value(0);
			break;
		}
	}
	else	// Set raster category names
	{
		CSG_Parameters	P;

		if( SG_UI_DataObject_Params_Get(pGrid, &P) && P("LUT") && P("LUT")->asTable() )
		{
			CSG_Table	&LUT = *P("LUT")->asTable(); LUT.Set_Record_Count(2);

			LUT[0][0] = SG_COLOR_GREY_LIGHT; LUT[0][2] = SG_T(""); LUT[0][3] = 0.;  LUT[0][4] = 0.;
			LUT[1][0] = SG_COLOR_RED       ; LUT[1][2] = SG_T(""); LUT[1][3] = 0.;  LUT[1][4] = 1.;

			switch( nDataItem )
			{
			case (PLOT_RASTER_COAST):
				LUT[0][1] = SG_T("Not coastline");
				LUT[1][1] = SG_T(    "Coastline");
				break;

			case (PLOT_RASTER_NORMAL):
				LUT[0][1] = SG_T("Not coastline-normal profile");
				LUT[1][1] = SG_T(    "Coastline-normal profile");
				break;
			}

			P.Set_Parameter("COLORS_TYPE", 1);	// Color Classification Type: Lookup Table

			SG_UI_DataObject_Params_Set(pGrid, &P);
		}
	}

	return( true );
}

#endif // #if defined(_SAGA_MSW) || defined(_SAGA_LINUX)
