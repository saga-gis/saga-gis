/*!
 *
 * \file gis_utils.cpp
 * \brief Various GIS-related functions. This version will build with GDAL version 2
 * \details TODO A more detailed description of these routines.
 * \author Andres Payo 
 * \author David Favis-Mortlock
 * \author Martin Husrt
 * \author Monica Palaseanu-Lovejoy
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
#include <iostream>
using std::cout;
using std::cerr;
using std::endl;
using std::ios;

#include <cmath>
#include <cfloat>

#if !defined(_SAGA_MSW) && !defined(_SAGA_LINUX)
#include <gdal_priv.h>
#include <ogrsf_frmts.h>
#endif // #if !defined(_SAGA_MSW) && !defined(_SAGA_LINUX)

#include "cliffmetrics.h"
#include "delineation.h"
#include "raster_grid.h"


/*
 Notes re. co-ordinate systems used

 1. In the raster CRS, cell[0][0] is at the top left (NW) corner of the grid. Raster grid co-oordinate [0][0] is actually the top left (NW) corner of this cell.

 2. We assume that the grid CRS and external CRS have parallel axes. If they have not, see http://www.gdal.org/classGDALDataset.html which says that:

   To convert between pixel/line (P,L) raster space, and projection coordinates (Xp,Yp) space
      Xp = padfTransform[0] + P*padfTransform[1] + L*padfTransform[2];
      Yp = padfTransform[3] + P*padfTransform[4] + L*padfTransform[5];

   In a north-up image, padfTransform[1] is the pixel width, and padfTransform[5] is the pixel height. The upper left corner of the upper left pixel is at position
      (padfTransform[0], padfTransform[3]).

 3. Usually, raster grid CRS values are integer, i.e. they refer to a point which is at the centroid of a cell. They may also be -ve or greater than m_nXGridMax-1 i.e. may refer to a point which lies outside any cell of the raster grid.

*/

/*==============================================================================================================================

 Given the integer X-axis ordinate of a cell in the raster-grid CRS, returns the external CRS X-axis ordinate of the cell's centroid

===============================================================================================================================*/
double CDelineation::dGridCentroidXToExtCRSX(int const nGridX) const
{
   return (m_dGeoTransform[0] + (nGridX * m_dGeoTransform[1]) + (m_dGeoTransform[1] / 2));
}


/*==============================================================================================================================

 Given the integer Y-axis ordinate of a cell in the raster-grid CRS, returns the external CRS Y-axis ordinate of the cell's centroid

===============================================================================================================================*/
double CDelineation::dGridCentroidYToExtCRSY(int const nGridY) const
{
   return (m_dGeoTransform[3] + (nGridY * m_dGeoTransform[5]) + (m_dGeoTransform[5] / 2));
}

/*==============================================================================================================================

 Transforms a pointer to a C2DIPoint in the raster-grid CRS (assumed to be the centroid of a cell) to the equivalent C2DPoint in the external CRS

===============================================================================================================================*/
C2DPoint CDelineation::PtGridCentroidToExt(C2DIPoint* const pPtiIn) const
{
    int
      nGridX = pPtiIn->nGetX(),
      nGridY = pPtiIn->nGetY();

   double
      dX = m_dGeoTransform[0] + (nGridX * m_dGeoTransform[1]) + (m_dGeoTransform[1] / 2),
      dY = m_dGeoTransform[3] + (nGridY * m_dGeoTransform[5]) + (m_dGeoTransform[5] / 2);

   return C2DPoint(dX, dY);
}


/*==============================================================================================================================

 Given a real-valued X-axis ordinate in the raster-grid CRS (i.e. not the centroid of a cell), returns the external CRS X-axis ordinate

===============================================================================================================================*/
double CDelineation::dGridXToExtCRSX(double const dGridX) const
{
   return (m_dGeoTransform[0] + (dGridX * m_dGeoTransform[1]));
}


/*==============================================================================================================================

 Given a real-valued Y-axis ordinate in the raster-grid CRS (i.e. not the centroid of a cell), returns the external CRS Y-axis ordinate

===============================================================================================================================*/
double CDelineation::dGridYToExtCRSY(double const dGridY) const
{
   return (m_dGeoTransform[3] + (dGridY * m_dGeoTransform[5]));
}


/*==============================================================================================================================

 Transforms an X-axis ordinate in the external CRS to the equivalent X-axis ordinate in the raster-grid CRS (the result may not be integer, and may be outside the grid)

===============================================================================================================================*/
double CDelineation::dExtCRSXToGridX(double const dExtCRSX) const
{
   return ((dExtCRSX - m_dGeoTransform[0]) / m_dGeoTransform[1]);
}


/*==============================================================================================================================

 Transforms a Y-axis ordinate in the external CRS to the equivalent Y-axis ordinate in the raster-grid CRS (the result may not be integer, and may be outside the grid)

===============================================================================================================================*/
double CDelineation::dExtCRSYToGridY(double const dExtCRSY) const
{
   return ((dExtCRSY - m_dGeoTransform[3]) / m_dGeoTransform[5]);
}


/*==============================================================================================================================

 Transforms a pointer to a C2DPoint in the external CRS to the equivalent C2DIPoint in the raster-grid CRS (both values rounded)

===============================================================================================================================*/
C2DIPoint CDelineation::PtiExtCRSToGrid(C2DPoint* const pPtIn) const
{
   double
      dX = pPtIn->dGetX(),
      dY = pPtIn->dGetY();

   int
      nX = dRound((dX - m_dGeoTransform[0]) / m_dGeoTransform[1]),
      nY = dRound((dY - m_dGeoTransform[3]) / m_dGeoTransform[5]);

   return C2DIPoint(nX, nY);
}


/*==============================================================================================================================

 Returns the distance (in external CRS) between two points

===============================================================================================================================*/
double CDelineation::dGetDistanceBetween(C2DPoint* const Pt1, C2DPoint* const Pt2)
{
   double
      dXDist = Pt1->dGetX() - Pt2->dGetX(),
      dYDist = Pt1->dGetY() - Pt2->dGetY();

   return hypot(dXDist, dYDist);
}


/*==============================================================================================================================

 Returns the distance (in grid units) between two grid cell points

===============================================================================================================================*/
double CDelineation::dGetDistanceBetween(C2DIPoint* const Pti1, C2DIPoint* const Pti2)
{
   double
      dXDist = Pti1->nGetX() - Pti2->nGetX(),
      dYDist = Pti1->nGetY() - Pti2->nGetY();

   return hypot(dXDist, dYDist);
}


/*==============================================================================================================================

 Checks whether the supplied point (an x-y pair, in the grid CRS) is within the raster grid

===============================================================================================================================*/
bool CDelineation::bIsWithinGrid(int const nX, int const nY) const
{
   if ((nX < 0) || (nX >= m_nXGridMax) || (nY < 0) || (nY >= m_nYGridMax))
      return false;

   return true;
}


/*==============================================================================================================================

 Checks whether the supplied point (a reference to a C2DIPoint, in the grid CRS) is within the raster grid

===============================================================================================================================*/
bool CDelineation::bIsWithinGrid(C2DIPoint* const Pti) const
{
   int nX = Pti->nGetX();

   if ((nX < 0) || (nX >= m_nXGridMax))
      return false;

   int nY = Pti->nGetY();

   if ((nY < 0) || (nY >= m_nYGridMax))
      return false;

   return true;
}


/*==============================================================================================================================

 Constrains the supplied point (a reference to a C2DIPoint, in the grid CRS) to be within the raster grid

===============================================================================================================================*/
void CDelineation::KeepWithinGrid(C2DIPoint* Pti)
{
   int nX = Pti->nGetX();
   nX = tMax(nX, 0);
   nX = tMin(nX, m_nXGridMax-1);
   Pti->SetX(nX);

   int nY = Pti->nGetY();
   nY = tMax(nY, 0);
   nY = tMin(nY, m_nYGridMax-1);
   Pti->SetY(nY);
}


/*==============================================================================================================================

 Constrains the supplied point (an x-y pair, in the grid CRS) to be within the raster grid

===============================================================================================================================*/
void CDelineation::KeepWithinGrid(int& nX, int& nY)
{
   nX = tMax(nX, 0);
   nX = tMin(nX, m_nXGridMax-1);

   nY = tMax(nY, 0);
   nY = tMin(nY, m_nYGridMax-1);
}


/*==============================================================================================================================

 Constrains the supplied point (a reference to a C2DPoint, in the external CRS) to be within the raster grid

===============================================================================================================================*/
// C2DPoint* CDelineation::pPtExtCRSKeepWithinGrid(C2DPoint* pPt)
// {
//    double dGridX = dExtCRSXToGridX(pPt->dGetX());
//    dGridX = tMax(dGridX, 0.0);
//    dGridX = tMin(dGridX, static_cast<double>(m_nXGridMax-1));
//    pPt->SetX(dGridXToExtCRSX(dGridX));
//
//    double dGridY = dExtCRSYToGridY(pPt->dGetY());
//    dGridY = tMax(dGridY, 0.0);
//    dGridY = tMin(dGridY, static_cast<double>(m_nYGridMax-1));
//    pPt->SetY(dGridYToExtCRSY(dGridY));
//
//    return pPt;
// }


/*==============================================================================================================================

 Constrains the supplied angle to be within 0 and 360 degrees

===============================================================================================================================*/
double CDelineation::dKeepWithin360(double const dAngle)
{
   double dNewAngle = fmod(dAngle, 360);

   // Sort out -ve angles
   while (dNewAngle < 0)
      dNewAngle += 360;

   return dNewAngle;
}


/*==============================================================================================================================

 Returns a point (external CRS) which is the average of (i.e. is midway between) two other external CRS points

===============================================================================================================================*/
C2DPoint CDelineation::PtAverage(C2DPoint* const pPt1, C2DPoint* const pPt2)
{
   double
      dPt1X = pPt1->dGetX(),
      dPt1Y = pPt1->dGetY(),
      dPt2X = pPt2->dGetX(),
      dPt2Y = pPt2->dGetY(),
      dPtAvgX = (dPt1X + dPt2X) / 2,
      dPtAvgY = (dPt1Y + dPt2Y) / 2;

   return C2DPoint(dPtAvgX, dPtAvgY);
}


/*==============================================================================================================================

 Returns a point (external CRS) which is the average of a vector of external CRS points

===============================================================================================================================*/
C2DPoint CDelineation::PtAverage(vector<C2DPoint>* pVIn)
{
   int nSize = pVIn->size();
   if (nSize == 0)
      return C2DPoint(DBL_NODATA, DBL_NODATA);

   double
      dAvgX = 0,
      dAvgY = 0;

   for (int n = 0; n < nSize; n++)
   {
      dAvgX += pVIn->at(n).dGetX();
      dAvgY += pVIn->at(n).dGetY();
   }

   dAvgX /= nSize;
   dAvgY /= nSize;

   return C2DPoint(dAvgX, dAvgY);
}


/*==============================================================================================================================

 Returns a point (grid CRS) which is the rounded average of (i.e. is midway between) two other grid CRS points

===============================================================================================================================*/
C2DIPoint CDelineation::PtiAverage(C2DIPoint* const pPti1, C2DIPoint* const pPti2)
{
   double
      dPt1X = pPti1->nGetX(),
      dPt1Y = pPti1->nGetY(),
      dPt2X = pPti2->nGetX(),
      dPt2Y = pPti2->nGetY(),
      dPtAvgX = (dPt1X + dPt2X) / 2,
      dPtAvgY = (dPt1Y + dPt2Y) / 2;

   return C2DIPoint(static_cast<int>(dRound(dPtAvgX)), static_cast<int>(dRound(dPtAvgY)));
}


/*==============================================================================================================================

 Returns a vector which is perpendicular to an existing vector

===============================================================================================================================*/
// vector<C2DPoint> CDelineation::VGetPerpendicular(C2DPoint* const PtStart, C2DPoint* const PtNext, double const dDesiredLength, int const nHandedness)
// {
//    // Returns a two-point vector which passes through PtStart with a scaled length
//    double dXLen = PtNext->dGetX() - PtStart->dGetX();
//    double dYLen = PtNext->dGetY() - PtStart->dGetY();
//
//    double dLength = hypot(dXLen, dYLen);
//    double dScaleFactor = dDesiredLength / dLength;
//
//    // The difference vector is (dXLen, dYLen), so the perpendicular difference vector is (-dYLen, dXLen) or (dYLen, -dXLen)
//    C2DPoint EndPt;
//    if (nHandedness == RIGHT_HANDED)
//    {
//       EndPt.SetX(PtStart->dGetX() + (dScaleFactor * dYLen));
//       EndPt.SetY(PtStart->dGetY() - (dScaleFactor * dXLen));
//    }
//    else
//    {
//       EndPt.SetX(PtStart->dGetX() - (dScaleFactor * dYLen));
//       EndPt.SetY(PtStart->dGetY() + (dScaleFactor * dXLen));
//    }
//
//    vector<C2DPoint> VNew;
//    VNew.push_back(*PtStart);
//    VNew.push_back(EndPt);
//    return VNew;
// }



/*==============================================================================================================================

 Returns a C2DPoint which is the 'other' point of a two-point vector passing through PtStart, and which is perpendicular to the two-point vector from PtStart to PtNext

===============================================================================================================================*/
C2DPoint CDelineation::PtGetPerpendicular(C2DPoint* const PtStart, C2DPoint* const PtNext, double const dDesiredLength, int const nHandedness)
{
   double dXLen = PtNext->dGetX() - PtStart->dGetX();
   double dYLen = PtNext->dGetY() - PtStart->dGetY();

   double dLength = hypot(dXLen, dYLen);
   double dScaleFactor = dDesiredLength / dLength;

   // The difference vector is (dXLen, dYLen), so the perpendicular difference vector is (-dYLen, dXLen) or (dYLen, -dXLen)
   C2DPoint EndPt;
   if (nHandedness == RIGHT_HANDED)
   {
      EndPt.SetX(PtStart->dGetX() + (dScaleFactor * dYLen));
      EndPt.SetY(PtStart->dGetY() - (dScaleFactor * dXLen));
   }
   else
   {
      EndPt.SetX(PtStart->dGetX() - (dScaleFactor * dYLen));
      EndPt.SetY(PtStart->dGetY() + (dScaleFactor * dXLen));
   }

   return EndPt;
}


/*==============================================================================================================================

 Checks whether the selected raster GDAL driver supports file creation, 32-bit doubles, etc.

===============================================================================================================================*/
bool CDelineation::bCheckRasterGISOutputFormat(void)
{
#if !defined(_SAGA_MSW) && !defined(_SAGA_LINUX)
   // Register all available GDAL raster and vector drivers (GDAL 2)
   GDALAllRegister();

   // If the user hasn't specified a GIS output format, assume that we will use the same GIS format as the input basement DEM
   if (m_strRasterGISOutFormat.empty())
      m_strRasterGISOutFormat = m_strGDALBasementDEMDriverCode;

   // Load the raster GDAL driver
   GDALDriver* pDriver = GetGDALDriverManager()->GetDriverByName(m_strRasterGISOutFormat.c_str());
   if (NULL == pDriver)
   {
      // Can't load raster GDAL driver. Incorrectly specified?
      cerr << ERR << "Unknown raster GIS output format '" << m_strRasterGISOutFormat << "'." << endl;
      return false;
   }

   // Get the metadata for this raster driver
   char** papszMetadata = pDriver->GetMetadata();

//    for (int i = 0; papszMetadata[i] != NULL; i++)
//       cout << papszMetadata[i] << endl;
//    cout << endl;

   // For GDAL2, need to test if this is a raster driver
   if (! CSLFetchBoolean(papszMetadata, GDAL_DCAP_RASTER, FALSE))
   {
      // This is not a raster driver
      cerr << ERR << "GDAL driver '" << m_strRasterGISOutFormat << "' is not a raster driver. Choose another format." << endl;
      return false;
   }

   // This driver is OK, so store its longname and the default file extension
   m_strGDALRasterOutputDriverLongname  = CSLFetchNameValue(papszMetadata, "DMD_LONGNAME");
   m_strGDALRasterOutputDriverExtension = CSLFetchNameValue(papszMetadata, "DMD_EXTENSION");

   // Set up any defaults for raster files that are created using this driver
   SetRasterFileCreationDefaults();

   // Now do various tests of the driver's capabilities
   if (! CSLFetchBoolean(papszMetadata, GDAL_DCAP_CREATE, FALSE))
   {
      // This raster driver does not support the Create() method, does it support CreateCopy()?
      if (! CSLFetchBoolean(papszMetadata, GDAL_DCAP_CREATECOPY, FALSE))
      {
         cerr << ERR << "Cannot write using raster GDAL driver '" << m_strRasterGISOutFormat << " since neither Create() or CreateCopy() are supported'. Choose another GDAL raster format." << endl;
         return false;
      }

      // Can't use Create() but can use CreateCopy()
      m_bGDALCanCreate = false;
   }

   // Next, test to see what data types the driver can write and from this, work out the largest int and float we can write
   if (strstr(CSLFetchNameValue(papszMetadata, "DMD_CREATIONDATATYPES"), "Float"))
   {
      m_bGDALCanWriteFloat = true;
      m_GDALWriteFloatDataType = GDT_Float32;
   }

   if (strstr(CSLFetchNameValue(papszMetadata, "DMD_CREATIONDATATYPES"), "UInt32"))
   {
      m_bGDALCanWriteInt32 = true;

      m_GDALWriteIntDataType = GDT_UInt32;
      m_lGDALMaxCanWrite = UINT32_MAX;
      m_lGDALMinCanWrite = 0;

      if (! m_bGDALCanWriteFloat)
         m_GDALWriteFloatDataType = GDT_UInt32;

      return true;
   }

   if (strstr(CSLFetchNameValue(papszMetadata, "DMD_CREATIONDATATYPES"), "Int32"))
   {
      m_bGDALCanWriteInt32 = true;

      m_GDALWriteIntDataType = GDT_Int32;
      m_lGDALMaxCanWrite = INT32_MAX;
      m_lGDALMinCanWrite = INT32_MIN;

      if (! m_bGDALCanWriteFloat)
         m_GDALWriteFloatDataType = GDT_Int32;

      return true;
   }

   if (strstr(CSLFetchNameValue(papszMetadata, "DMD_CREATIONDATATYPES"), "UInt16"))
   {
      m_bGDALCanWriteInt32 = false;

      m_GDALWriteIntDataType = GDT_UInt16;
      m_lGDALMaxCanWrite = UINT16_MAX;
      m_lGDALMinCanWrite = 0;

      if (! m_bGDALCanWriteFloat)
         m_GDALWriteFloatDataType = GDT_UInt16;

      return true;
   }

   if (strstr(CSLFetchNameValue(papszMetadata, "DMD_CREATIONDATATYPES"), "Int16"))
   {
      m_bGDALCanWriteInt32 = false;

      m_GDALWriteIntDataType = GDT_Int16;
      m_lGDALMaxCanWrite = INT16_MAX;
      m_lGDALMinCanWrite = INT16_MIN;

      if (! m_bGDALCanWriteFloat)
         m_GDALWriteFloatDataType = GDT_Int16;

      return true;
   }

   if (strstr(CSLFetchNameValue(papszMetadata, "DMD_CREATIONDATATYPES"), "Byte"))
   {
      m_bGDALCanWriteInt32 = false;

      m_GDALWriteIntDataType = GDT_Byte;
      m_lGDALMaxCanWrite = UINT8_MAX;
      m_lGDALMinCanWrite = 0;

      if (! m_bGDALCanWriteFloat)
         m_GDALWriteFloatDataType = GDT_Byte;

      return true;
   }

   // This driver does not even support byte output
   cerr << ERR << "Cannot write using raster GDAL driver '" << m_strRasterGISOutFormat << ", not even byte output is supported'. Choose another GIS raster format." << endl;
   return false;
#else // #if defined(_SAGA_MSW) || defined(_SAGA_LINUX)
   return true;
#endif // #if defined(_SAGA_MSW) || defined(_SAGA_LINUX)
}


/*==============================================================================================================================

 Checks whether the selected vector OGR driver supports file creation etc.

===============================================================================================================================*/
bool CDelineation::bCheckVectorGISOutputFormat(void)
{
#if !defined(_SAGA_MSW) && !defined(_SAGA_LINUX)
   // Load the vector GDAL driver (NOTE this assumes that GDALAllRegister() has already been called)
   GDALDriver* pDriver = GetGDALDriverManager()->GetDriverByName(m_strVectorGISOutFormat.c_str());
   if (NULL == pDriver)
   {
      // Can't load vector GDAL driver. Incorrectly specified?
      cerr << ERR << "Unknown vector GIS output format '" << m_strVectorGISOutFormat << "'." << endl;
      return false;
   }

   // Get the metadata for this vector driver
   char** papszMetadata = pDriver->GetMetadata();

   // For GDAL2, need to test if this is a vector driver
   if (! CSLFetchBoolean(papszMetadata, GDAL_DCAP_VECTOR, FALSE))
   {
      // This is not a vector driver
      cerr << ERR << "GDAL driver '" << m_strVectorGISOutFormat << "' is not a vector driver. Choose another format." << endl;
      return false;
   }

   if (! CSLFetchBoolean(papszMetadata, GDAL_DCAP_CREATE, FALSE))
   {
      // Driver does not support create() method
      cerr << ERR << "Cannot write vector GIS files using GDAL driver '" << m_strRasterGISOutFormat << "'. Choose another format." << endl;
      return false;
   }

   // Driver is OK, now set some options for individual drivers
   if (m_strVectorGISOutFormat == "ESRI Shapefile")
   {
      // Set this, so that just a single dataset-with-one-layer shapefile is created, rather than a directory
      // (see http://www.gdal.org/ogr/drv_shapefile.html)
      m_strOGRVectorOutputExtension = ".shp";

   }
   // TODO Others
#endif // #if !defined(_SAGA_MSW) && !defined(_SAGA_LINUX)

   return true;
}


/*==============================================================================================================================

 The bSaveAllRasterGISFiles member function saves the raster GIS files using values from the RasterGrid array

==============================================================================================================================*/
bool CDelineation::bSaveAllRasterGISFiles(void)
{
#if !defined(_SAGA_MSW) && !defined(_SAGA_LINUX)
   // Increment file number
   // m_nGISSave++;

   // Set for next save
   //  m_nThisSave = tMin(++m_nThisSave, m_nUSave);

   // These are always written
	if (! bWriteRasterGISFloat(PLOT_SEDIMENT_TOP_ELEV, &PLOT_SEDIMENT_TOP_ELEV_TITLE))
		return false;

	if (m_bRasterCoastlineSave)
	{
		if (! bWriteRasterGISInt(PLOT_RASTER_COAST, &PLOT_RASTER_COAST_TITLE))
			return false;
	}

	if (m_bRasterNormalSave)
	{
		if (! bWriteRasterGISInt(PLOT_RASTER_NORMAL, &PLOT_RASTER_NORMAL_TITLE))
			return false;
	}

   // These are optional
   //if (! bWriteRasterGISInt(PLOT_LANDFORM, &PLOT_LANDFORM_TITLE))
   //     return false;*/
   
    return true;
#else // #if defined(_SAGA_MSW) || defined(_SAGA_LINUX)
	CSG_Grid	*pGrid;

	if( (pGrid = (*m_pParameters)("SEDIMENT_TOP" )->asGrid()) )
	{
		if( !bWriteRasterGISFloat(PLOT_SEDIMENT_TOP_ELEV, pGrid) )
		{
			return( false );
		}
	}

	if( (pGrid = (*m_pParameters)("RASTER_COAST" )->asGrid()) )
	{
		if( !bWriteRasterGISInt  (PLOT_RASTER_COAST     , pGrid) )
		{
			return( false );
		}
	}

	if( (pGrid = (*m_pParameters)("RASTER_NORMAL")->asGrid()) )
	{
		if( !bWriteRasterGISInt  (PLOT_RASTER_NORMAL    , pGrid) )
		{
			return( false );
		}
	}

	//if( (pGrid = (*m_pParameters)("PLOT_LANDFORM")->asGrid()) )
	//{
	//	if( !bWriteRasterGISFloat(PLOT_LANDFORM         , pGrid) )
	//	{
	//		return( false );
	//	}
	//}

	return( true );
#endif // #if defined(_SAGA_MSW) || defined(_SAGA_LINUX)
}


/*==============================================================================================================================

 The bSaveAllvectorGISFiles member function saves the vector GIS files

==============================================================================================================================*/
bool CDelineation::bSaveAllVectorGISFiles(void)
{
#if !defined(_SAGA_MSW) && !defined(_SAGA_LINUX)
   // Always written
   if (! bWriteVectorGIS(PLOT_COAST, &PLOT_COAST_TITLE))
      return false;

   if (! bWriteVectorGIS(PLOT_NORMALS, &PLOT_NORMALS_TITLE))
      return false;
   
   if (! bWriteVectorGIS(PLOT_CLIFF_TOP, &PLOT_CLIFF_TOP_TITLE))
      return false;
   
   if (! bWriteVectorGIS(PLOT_CLIFF_TOE, &PLOT_CLIFF_TOE_TITLE))
      return false;

   if (! bWriteVectorGIS(PLOT_COAST_POINT, &PLOT_COAST_POINT_TITLE))
      return false;

   // These are optional (see delineation constructor)
   if (m_bInvalidNormalsSave)
   {
      if (! bWriteVectorGIS(PLOT_INVALID_NORMALS, &PLOT_INVALID_NORMALS_TITLE))
         return false;
   }

   if (m_bCoastCurvatureSave)
   {
      if (! bWriteVectorGIS(PLOT_COAST_CURVATURE, &PLOT_COAST_CURVATURE_TITLE))
         return false;
   }

   return true;
#else // #if defined(_SAGA_MSW) || defined(_SAGA_LINUX)
	if( !bWriteVectorGIS(PLOT_COAST      , (*m_pParameters)("COAST"      )->asShapes()) )
		return false;

	if( !bWriteVectorGIS(PLOT_NORMALS    , (*m_pParameters)("NORMALS"    )->asShapes()) )
		return false;

	if( !bWriteVectorGIS(PLOT_CLIFF_TOP  , (*m_pParameters)("CLIFF_TOP"  )->asShapes()) )
		return false;

	if( !bWriteVectorGIS(PLOT_CLIFF_TOE  , (*m_pParameters)("CLIFF_TOE"  )->asShapes()) )
		return false;

	if( !bWriteVectorGIS(PLOT_COAST_POINT, (*m_pParameters)("COAST_POINT")->asShapes()) )
		return false;

	// These are optional (see delineation constructor)
	if( (*m_pParameters)("INVALID_NORMALS")->asShapes() )
	{
		if( !bWriteVectorGIS(PLOT_INVALID_NORMALS, (*m_pParameters)("INVALID_NORMALS")->asShapes()) )
			return false;
	}

	if( (*m_pParameters)("COAST_CURVATURE")->asShapes() )
	{
		if( !bWriteVectorGIS(PLOT_COAST_CURVATURE, (*m_pParameters)("COAST_CURVATURE")->asShapes()) )
			return false;
	}

	return( true );
#endif // #if !defined(_SAGA_MSW) && !defined(_SAGA_LINUX)
}


/*==============================================================================================================================

 Finds the max and min values in order to scale raster output if we cannot write floats or 32-bit integers

==============================================================================================================================*/
void CDelineation::GetRasterOutputMinMax(int const nDataItem, double& dMin, double& dMax)
{
   // If this is a binary mask layer, we already know the max and min values
   if ((nDataItem == PLOT_POTENTIAL_PLATFORM_EROSION_MASK) ||
       (nDataItem == PLOT_RASTER_COAST) ||
       (nDataItem == PLOT_RASTER_NORMAL))
       
   {
      dMin = 0;
      dMax = 1;

      return;
   }

   // Not a binary mask layer, so we must find the max and min values
   dMin = DBL_MAX;
   dMax = DBL_MIN;

   double dTmp;
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

         if (dTmp != DBL_NODATA)
         {
            if (dTmp > dMax)
               dMax = dTmp;

            if (dTmp < dMin)
               dMin = dTmp;
         }
      }
   }
}


/*==============================================================================================================================

 Sets per-driver defaults for raster files created using GDAL

===============================================================================================================================*/
void CDelineation::SetRasterFileCreationDefaults(void)
{
#if !defined(_SAGA_MSW) && !defined(_SAGA_LINUX)
   string
      strDriver = strToLower(&m_strRasterGISOutFormat),
      strComment =  "Created by " + PROGNAME + " for " + PLATFORM + " " + strGetBuild() + " running on " + strGetComputerName();

   // TODO Do these for all commonly-used file types
   if (strDriver == "aaigrid")
   {

   }

   else if (strDriver == "bmp")
   {

   }

   else if (strDriver == "gtiff")
   {
      if (m_bWorldFile)
         m_papszGDALRasterOptions = CSLSetNameValue(m_papszGDALRasterOptions, "TFW", "YES");

//       if (m_bCompressGTIFF)
//       {
//          m_papszGDALRasterOptions = CSLSetNameValue(m_papszGDALRasterOptions, "NUM_THREADS", "ALL_CPUS");
//          m_papszGDALRasterOptions = CSLSetNameValue(m_papszGDALRasterOptions, "COMPRESS", "LZW");
//       }
   }

   else if (strDriver == "hfa")
   {
      m_papszGDALRasterOptions = CSLSetNameValue(m_papszGDALRasterOptions, "NBITS", "4");
   }

   else if (strDriver == "jpeg")
   {
      m_papszGDALRasterOptions = CSLSetNameValue(m_papszGDALRasterOptions, "COMMENT", strComment.c_str());
      m_papszGDALRasterOptions = CSLSetNameValue(m_papszGDALRasterOptions, "QUALITY", "95");
   }

   else if (strDriver == "png")
   {
      if (m_bWorldFile)
         m_papszGDALRasterOptions = CSLSetNameValue(m_papszGDALRasterOptions, "WORLDFILE", "YES");

//       m_papszGDALRasterOptions = CSLSetNameValue(m_papszGDALRasterOptions, "TITLE", "This is the title");
//       m_papszGDALRasterOptions = CSLSetNameValue(m_papszGDALRasterOptions, "DESCRIPTION", "This is a description");
//       m_papszGDALRasterOptions = CSLSetNameValue(m_papszGDALRasterOptions, "COPYRIGHT", "This is some copyright statement");
      m_papszGDALRasterOptions = CSLSetNameValue(m_papszGDALRasterOptions, "COMMENT", strComment.c_str());
      m_papszGDALRasterOptions = CSLSetNameValue(m_papszGDALRasterOptions, "NBITS", "4");
   }

   else if (strDriver == "rst")
   {

   }

//    for (int i = 0; m_papszGDALRasterOptions[i] != NULL; i++)
//       cout << m_papszGDALRasterOptions[i] << endl;
//    cout << endl;
#endif // #if !defined(_SAGA_MSW) && !defined(_SAGA_LINUX)
}
