/*!
 *
 * \file gis_vector.cpp
 * \brief These functions use GDAL to read and write vector GIS files in several formats. This version will build with GDAL version 2
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

#if !defined(_SAGA_MSW) && !defined(SAGA_LINUX)
#include <gdal_priv.h>
#include <ogrsf_frmts.h>
#endif // #if !defined(_SAGA_MSW) && !defined(SAGA_LINUX)

#include "cliffmetrics.h"
#include "delineation.h"
#include "coast.h"

/*==============================================================================================================================

 Writes vector GIS files using OGR

===============================================================================================================================*/
#if !defined(_SAGA_MSW) && !defined(SAGA_LINUX)
bool CDelineation::bWriteVectorGIS(int const nDataItem, string const* strPlotTitle)
{
   // Begin constructing the file name for this save
   string strFilePathName(m_strOutPath);

   switch (nDataItem)
   {
      case (PLOT_COAST):
      {
         strFilePathName.append(VECTOR_COAST_NAME);
         break;
      }

      case (PLOT_NORMALS):
      {
         strFilePathName.append(VECTOR_NORMALS_NAME);
         break;
      }

      case (PLOT_INVALID_NORMALS):
      {
         strFilePathName.append(VECTOR_INVALID_NORMALS_NAME);
         break;
      }

      case (PLOT_COAST_CURVATURE):
      {
         strFilePathName.append(VECTOR_COAST_CURVATURE_NAME);
         break;
      }

      case (PLOT_CLIFF_TOP):
      {
         strFilePathName.append(VECTOR_CLIFF_TOP_NAME);
         break;
      }

      case (PLOT_CLIFF_TOE):
      {
         strFilePathName.append(VECTOR_CLIFF_TOE_NAME);
         break;
      }
      
      case (PLOT_COAST_POINT):
      {
         strFilePathName.append(VECTOR_COAST_POINT_NAME);
         break;
      }
   }

   // Append the 'RunName' to the filename
   strFilePathName.append("_");
   strFilePathName.append(m_strRunName);

   // Make a copy of the filename without any extension
   string strFilePathNameNoExt = strFilePathName;

   // If desired, append an extension
   if (! m_strOGRVectorOutputExtension.empty())
      strFilePathName.append(m_strOGRVectorOutputExtension);

   // Set up the vector driver
   GDALDriver* pGDALDriver = GetGDALDriverManager()->GetDriverByName(m_strVectorGISOutFormat.c_str());
   if (pGDALDriver == NULL)
   {
      cerr << ERR << "vector GIS output driver " << m_strVectorGISOutFormat << CPLGetLastErrorMsg() << endl;
      return false;
   }

   // Now create the dataset
   GDALDataset* pGDALDataSet = NULL;
   pGDALDataSet = pGDALDriver->Create(strFilePathName.c_str(), 0, 0, 0, GDT_Unknown, m_papszGDALVectorOptions);
   if (pGDALDataSet == NULL)
   {
      cerr << ERR << "cannot create " << m_strVectorGISOutFormat << " named " << strFilePathName << "\n" << CPLGetLastErrorMsg() << endl;
      return false;
   }

   // Create the output layer
   OGRLayer* pOGRLayer = NULL;
   OGRSpatialReference* pOGRSpatialRef = NULL;     // TODO add spatial reference
   OGRwkbGeometryType eGType = wkbUnknown;
   string strType = "unknown";

   pOGRLayer = pGDALDataSet->CreateLayer(strFilePathNameNoExt.c_str(), pOGRSpatialRef, eGType, m_papszGDALVectorOptions);
   if (pOGRLayer == NULL)
   {
      cerr << ERR << "cannot create '" << strType << "' layer in " << strFilePathName << "\n" << CPLGetLastErrorMsg() << endl;
      return false;
   }

   switch (nDataItem)
   {
      case (PLOT_COAST):
      {
         eGType = wkbLineString;
         strType = "line";

         // The layer has been created, so create an integer-numbered value (the number of the coast object) for the multi-line
         string strFieldValue1 = "Coast";
         OGRFieldDefn OGRField1(strFieldValue1.c_str(), OFTInteger);
         if (pOGRLayer->CreateField(&OGRField1) != OGRERR_NONE)
         {
            cerr << ERR << "cannot create " << strType << " attribute field 1 '" << strFieldValue1 << "' in " << strFilePathName << "\n" << CPLGetLastErrorMsg() << endl;
            return false;
         }

         // OK, now do features
         OGRLineString OGRls;

         for (int i = 0; i < static_cast<int>(m_VCoast.size()); i++)
         {
            // Create a feature object, one per coast
            OGRFeature *pOGRFeature = NULL;
            pOGRFeature = OGRFeature::CreateFeature(pOGRLayer->GetLayerDefn());

            // Set the feature's attribute (the coast number)
            pOGRFeature->SetField(strFieldValue1.c_str(), i);

            // Now attach a geometry to the feature object
            for (int j = 0; j < m_VCoast[i].pLGetCoastline()->nGetSize(); j++)
               //  In external CRS
               OGRls.addPoint(m_VCoast[i].pPtGetVectorCoastlinePoint(j)->dGetX(), m_VCoast[i].pPtGetVectorCoastlinePoint(j)->dGetY());

            pOGRFeature->SetGeometry(&OGRls);

            // Create the feature in the output layer
            if (pOGRLayer->CreateFeature(pOGRFeature) != OGRERR_NONE)
            {
               cerr << ERR << "cannot create  " << strType << " feature " << strPlotTitle << " for coast " << i << " in " << strFilePathName << "\n" << CPLGetLastErrorMsg() << endl;
               return false;
            }

            // Tidy up: empty the line string and get rid of the feature object
            OGRls.empty();
            OGRFeature::DestroyFeature(pOGRFeature);
         }

         break;
      }

      case (PLOT_NORMALS):
      case (PLOT_INVALID_NORMALS):
      {
         eGType = wkbLineString;
         strType = "line";

         // The layer has been created, so create an integer-numbered value (the number of the normal) associated with the line
         string strFieldValue1 = "Normal";
         OGRFieldDefn OGRField1(strFieldValue1.c_str(), OFTInteger);
         if (pOGRLayer->CreateField(&OGRField1) != OGRERR_NONE)
         {
            cerr << ERR << "cannot create " << strType << " attribute field 1 '" << strFieldValue1 << "' in " << strFilePathName << "\n" << CPLGetLastErrorMsg() << endl;
            return false;
         }

         // Also create other integer-numbered values for the category codes of the coastline-normalprofile
         string
            strFieldValue2 = "StartCoast",
            strFieldValue3 = "EndCoast",
            strFieldValue4 = "HitLand",
            strFieldValue5 = "HitCoast",
            strFieldValue6 = "HitNormal",
	    strFieldValue7 = "nCoast";
         OGRFieldDefn
            OGRField2(strFieldValue2.c_str(), OFTInteger),
            OGRField3(strFieldValue3.c_str(), OFTInteger),
            OGRField4(strFieldValue4.c_str(), OFTInteger),
            OGRField5(strFieldValue5.c_str(), OFTInteger),
            OGRField6(strFieldValue6.c_str(), OFTInteger),
            OGRField7(strFieldValue7.c_str(), OFTInteger);
         if (pOGRLayer->CreateField(&OGRField2) != OGRERR_NONE)
         {
            cerr << ERR << "cannot create " << strType << " attribute field 2 '" << strFieldValue2 << "' in " << strFilePathName << "\n" << CPLGetLastErrorMsg() << endl;
            return false;
         }
         if (pOGRLayer->CreateField(&OGRField3) != OGRERR_NONE)
         {
            cerr << ERR << "cannot create " << strType << " attribute field 3 '" << strFieldValue3 << "' in " << strFilePathName << "\n" << CPLGetLastErrorMsg() << endl;
            return false;
         }
         if (pOGRLayer->CreateField(&OGRField4) != OGRERR_NONE)
         {
            cerr << ERR << "cannot create " << strType << " attribute field 4 '" << strFieldValue4 << "' in " << strFilePathName << "\n" << CPLGetLastErrorMsg() << endl;
            return false;
         }
         if (pOGRLayer->CreateField(&OGRField5) != OGRERR_NONE)
         {
            cerr << ERR << "cannot create " << strType << " attribute field 5 '" << strFieldValue5 << "' in " << strFilePathName << "\n" << CPLGetLastErrorMsg() << endl;
            return false;
         }
         if (pOGRLayer->CreateField(&OGRField6) != OGRERR_NONE)
         {
            cerr << ERR << "cannot create " << strType << " attribute field 6 '" << strFieldValue6 << "' in " << strFilePathName << "\n" << CPLGetLastErrorMsg() << endl;
            return false;
         }
         if (pOGRLayer->CreateField(&OGRField7) != OGRERR_NONE)
         {
            cerr << ERR << "cannot create " << strType << " attribute field 7 '" << strFieldValue7 << "' in " << strFilePathName << "\n" << CPLGetLastErrorMsg() << endl;
            return false;
         }
         
         // OK, now create features
         OGRLineString OGRls;

         for (int i = 0; i < static_cast<int>(m_VCoast.size()); i++) // for each coast object
         {
            for (int j = 0; j < m_VCoast[i].nGetNumProfiles(); j++)
            {
               CProfile* pProfile = m_VCoast[i].pGetProfile(j);

               if (((nDataItem == PLOT_NORMALS) && (pProfile->bOKIncStartAndEndOfCoast())) || ((nDataItem == PLOT_INVALID_NORMALS) && (! pProfile->bOKIncStartAndEndOfCoast())))
               {
                  // Create a feature object, one per profile
                  OGRFeature *pOGRFeature = NULL;
                  pOGRFeature = OGRFeature::CreateFeature(pOGRLayer->GetLayerDefn());

                  // Set the feature's attributes
                  pOGRFeature->SetField(strFieldValue1.c_str(), j);
                  pOGRFeature->SetField(strFieldValue2.c_str(), 0);
                  pOGRFeature->SetField(strFieldValue3.c_str(), 0);
                  pOGRFeature->SetField(strFieldValue4.c_str(), 0);
                  pOGRFeature->SetField(strFieldValue5.c_str(), 0);
                  pOGRFeature->SetField(strFieldValue6.c_str(), 0);
                  pOGRFeature->SetField(strFieldValue7.c_str(), i);
                  if (pProfile->bStartOfCoast())
                     pOGRFeature->SetField(strFieldValue2.c_str(), 1);
                  if (pProfile->bEndOfCoast())
                     pOGRFeature->SetField(strFieldValue3.c_str(), 1);
                  if (pProfile->bHitLand())
                     pOGRFeature->SetField(strFieldValue4.c_str(), 1);
                  if (pProfile->bHitCoast())
                     pOGRFeature->SetField(strFieldValue5.c_str(), 1);
                  if (pProfile->bHitAnotherProfile())
                     pOGRFeature->SetField(strFieldValue6.c_str(), 1);

                  // Now attach a geometry to the feature object
                  for (int k = 0; k < pProfile->nGetProfileSize(); k++)
                     OGRls.addPoint(pProfile->pPtGetPointInProfile(k)->dGetX(), pProfile->pPtGetPointInProfile(k)->dGetY());

                  pOGRFeature->SetGeometry(&OGRls);
                  OGRls.empty();

                  // Create the feature in the output layer
                  if (pOGRLayer->CreateFeature(pOGRFeature) != OGRERR_NONE)
                  {
                     cerr << ERR << "cannot create  " << strType << " feature " << strPlotTitle << " for coast " << i << " and profile " << j << " in " << strFilePathName << "\n" << CPLGetLastErrorMsg() << endl;
                     return false;
                  }

                  // Tidy up: get rid of the feature object
                  OGRFeature::DestroyFeature(pOGRFeature);
               }
            }
         }

         break;
      }

      case (PLOT_COAST_CURVATURE):
      {
         eGType = wkbPoint;
         strType = "point";

         // The layer has been created, so create a real-numbered value associated with each point
         string strFieldValue1 = "Curve"; 
         
         OGRFieldDefn OGRField1(strFieldValue1.c_str(), OFTReal);
         if (pOGRLayer->CreateField(&OGRField1) != OGRERR_NONE)
         {
            cerr << ERR << "cannot create " << strType << " attribute field 1 '" << strFieldValue1 << "' in " << strFilePathName << "\n" << CPLGetLastErrorMsg() << endl;
            return false;
         }

         // OK, now create features
         OGRLineString OGRls;
         OGRMultiLineString OGRmls;
         OGRPoint OGRPt;

         for (int i = 0; i < static_cast<int>(m_VCoast.size()); i++)
         {
            for (int j = 0; j < m_VCoast[i].pLGetCoastline()->nGetSize(); j++)
            {
               // Create a feature object, one per coastline point
               OGRFeature *pOGRFeature = NULL;
               pOGRFeature = OGRFeature::CreateFeature(pOGRLayer->GetLayerDefn());

               // Set the feature's geometry (in external CRS)
               OGRPt.setX(m_VCoast[i].pPtGetVectorCoastlinePoint(j)->dGetX());
               OGRPt.setY(m_VCoast[i].pPtGetVectorCoastlinePoint(j)->dGetY());
               pOGRFeature->SetGeometry(&OGRPt);

               double dCurvature = m_VCoast[i].dGetCurvature(j);
               if (dCurvature == DBL_NODATA)
                     continue;

               // Set the feature's attribute
               pOGRFeature->SetField(strFieldValue1.c_str(), dCurvature);
          
               // Create the feature in the output layer
               if (pOGRLayer->CreateFeature(pOGRFeature) != OGRERR_NONE)
               {
                  cerr << ERR << "cannot create " << strType << " feature " << strPlotTitle << " for coast " << i << " point " << j << " in " << strFilePathName << "\n" << CPLGetLastErrorMsg() << endl;
                  return false;
               }

               // Get rid of the feature object
               OGRFeature::DestroyFeature(pOGRFeature);
            }
         }

         break;
      }
      case (PLOT_CLIFF_TOP):
      case (PLOT_CLIFF_TOE):
      case (PLOT_COAST_POINT):
      {
         eGType = wkbPoint;
         strType = "point";

         // The layer has been created, so create an integer-numbered value (the number of the coast) associated with the line and the profile
         string strFieldValue1 = "nCoast";
         string strFieldValue2 = "nProf";
	 string strFieldValue3 = "bisOK";
	 OGRFieldDefn OGRField1(strFieldValue1.c_str(), OFTInteger);
         OGRFieldDefn OGRField2(strFieldValue2.c_str(), OFTInteger);
	 OGRFieldDefn OGRField3(strFieldValue3.c_str(), OFTInteger);
         
         if (pOGRLayer->CreateField(&OGRField1) != OGRERR_NONE)
         {
            cerr << ERR << "cannot create " << strType << " attribute field 1 '" << strFieldValue1 << "' in " << strFilePathName << "\n" << CPLGetLastErrorMsg() << endl;
            return false;
         }
         if (pOGRLayer->CreateField(&OGRField2) != OGRERR_NONE)
         {
            cerr << ERR << "cannot create " << strType << " attribute field 2 '" << strFieldValue2 << "' in " << strFilePathName << "\n" << CPLGetLastErrorMsg() << endl;
            return false;
         }
         if (pOGRLayer->CreateField(&OGRField3) != OGRERR_NONE)
         {
            cerr << ERR << "cannot create " << strType << " attribute field 3 '" << strFieldValue3 << "' in " << strFilePathName << "\n" << CPLGetLastErrorMsg() << endl;
            return false;
         }
         // Also create other real-numbered values for the cliff top and toe elevation and chainage
         string  strFieldValue4;
	 string  strFieldValue5;
	 strFieldValue5 = "Chainage";
         if (nDataItem == PLOT_CLIFF_TOP)
            strFieldValue4 = "CliffTopEl";
         else if (nDataItem == PLOT_CLIFF_TOE)
            strFieldValue4 = "CliffToeEl";
	 else if (nDataItem == PLOT_COAST_POINT)
            strFieldValue4 = "	CoastEl";
	 
         OGRFieldDefn OGRField4(strFieldValue4.c_str(), OFTReal);
	 OGRFieldDefn OGRField5(strFieldValue5.c_str(), OFTReal);
            
         if (pOGRLayer->CreateField(&OGRField4) != OGRERR_NONE)
         {
            cerr << ERR << "cannot create " << strType << " attribute field 4 '" << strFieldValue4 << "' in " << strFilePathName << "\n" << CPLGetLastErrorMsg() << endl;
            return false;
         }
	 if (pOGRLayer->CreateField(&OGRField5) != OGRERR_NONE)
         {
            cerr << ERR << "cannot create " << strType << " attribute field 5 '" << strFieldValue5 << "' in " << strFilePathName << "\n" << CPLGetLastErrorMsg() << endl;
            return false;
         }
         
         // OK, now create features
         OGRLineString OGRls;
         OGRMultiLineString OGRmls;
         OGRPoint OGRPt;

         for (int i = 0; i < static_cast<int>(m_VCoast.size()); i++) // for each coast object
         {
            for (int j = 0; j < m_VCoast[i].nGetNumProfiles(); j++) // for each profile object
            {
               CProfile* pProfile = m_VCoast[i].pGetProfile(j);

               if (pProfile->bOKIncStartAndEndOfCoast()) 
               {
                  // Create a feature object, one per profile
                  OGRFeature *pOGRFeature = NULL;
                  pOGRFeature = OGRFeature::CreateFeature(pOGRLayer->GetLayerDefn());

                  // Set the feature's geometry (in external CRS)
                  int 
                    CliffPointIndex,
                    nX,
                    nY,
		    isQualityOK;
                  double
                    dX,
                    dY,
		    dChainage;

		  if (nDataItem == PLOT_CLIFF_TOP)  
                  {
                  CliffPointIndex = pProfile->nGetCliffTopPoint();
		  dChainage	  = pProfile->dGetCliffTopChainage();
                  nX = pProfile->pPtiVGetCellsInProfile()->at(CliffPointIndex).nGetX();
                  nY = pProfile->pPtiVGetCellsInProfile()->at(CliffPointIndex).nGetY();
                  dX = dGridCentroidXToExtCRSX(pProfile->pPtiVGetCellsInProfile()->at(CliffPointIndex).nGetX());
                  dY = dGridCentroidYToExtCRSY(pProfile->pPtiVGetCellsInProfile()->at(CliffPointIndex).nGetY());
		  if (pProfile->bGetCliffTopQualityFlag())
                     isQualityOK = 1;
		  else
		     isQualityOK = 0;
                  
                  }
                  else if (nDataItem == PLOT_CLIFF_TOE)  
                  {
                  CliffPointIndex = pProfile->nGetCliffToePoint();
		  dChainage	  = pProfile->dGetCliffToeChainage();
                  nX = pProfile->pPtiVGetCellsInProfile()->at(CliffPointIndex).nGetX();
                  nY = pProfile->pPtiVGetCellsInProfile()->at(CliffPointIndex).nGetY();
                  dX = dGridCentroidXToExtCRSX(pProfile->pPtiVGetCellsInProfile()->at(CliffPointIndex).nGetX());
                  dY = dGridCentroidYToExtCRSY(pProfile->pPtiVGetCellsInProfile()->at(CliffPointIndex).nGetY());
		  if (pProfile->bGetCliffTopQualityFlag())
                     isQualityOK = 1;
		  else
		     isQualityOK = 0;
                  }
                  
                  else if (nDataItem == PLOT_COAST_POINT)  
                  {
                  dChainage	  = 0;
                  nX = pProfile->pPtiVGetCellsInProfile()->at(0).nGetX();
                  nY = pProfile->pPtiVGetCellsInProfile()->at(0).nGetY();
                  dX = dGridCentroidXToExtCRSX(pProfile->pPtiVGetCellsInProfile()->at(0).nGetX());
                  dY = dGridCentroidYToExtCRSY(pProfile->pPtiVGetCellsInProfile()->at(0).nGetY());
		  isQualityOK = 1; // to do, sanity check for coastline points
		  
                  }

                  OGRPt.setX(dX);
                  OGRPt.setY(dY);
                  pOGRFeature->SetGeometry(&OGRPt);

                  // Get the elevation for both consolidated and unconsolidated sediment on this cell
                  double dVProfileZ = m_pRasterGrid->pGetCell(nX, nY)->dGetSedimentTopElev();

                  // Set the feature's attributes
                  pOGRFeature->SetField(strFieldValue1.c_str(), i);
                  pOGRFeature->SetField(strFieldValue2.c_str(), j);
		  pOGRFeature->SetField(strFieldValue3.c_str(), isQualityOK);
		  pOGRFeature->SetField(strFieldValue4.c_str(), dVProfileZ);
		  pOGRFeature->SetField(strFieldValue5.c_str(), dChainage);

                  // Create the feature in the output layer
                  if (pOGRLayer->CreateFeature(pOGRFeature) != OGRERR_NONE)
                  {
                     cerr << ERR << "cannot create  " << strType << " feature " << strPlotTitle << " for coast " << i << " and profile " << j << " in " << strFilePathName << "\n" << CPLGetLastErrorMsg() << endl;
                     return false;
                  }

                  // Tidy up: get rid of the feature object
                  OGRFeature::DestroyFeature(pOGRFeature);
               }
            }
         }

         break;
      }
   }

   // Get rid of the dataset object
   GDALClose(pGDALDataSet);

   return true;
}

#else // #if defined(_SAGA_MSW) || defined(SAGA_LINUX)
bool CDelineation::bWriteVectorGIS(int const nDataItem, CSG_Shapes *pShapes)
{
	if( !pShapes )
	{
		return( true );
	}

	CSG_String	Name;

	switch (nDataItem)
	{
	case (PLOT_COAST)          : Name = VECTOR_COAST_NAME          .c_str(); break;
	case (PLOT_NORMALS)        : Name = VECTOR_NORMALS_NAME        .c_str(); break;
	case (PLOT_INVALID_NORMALS): Name = VECTOR_INVALID_NORMALS_NAME.c_str(); break;
	case (PLOT_COAST_CURVATURE): Name = VECTOR_COAST_CURVATURE_NAME.c_str(); break;
	case (PLOT_CLIFF_TOP)      : Name = VECTOR_CLIFF_TOP_NAME      .c_str(); break;
	case (PLOT_CLIFF_TOE)      : Name = VECTOR_CLIFF_TOE_NAME      .c_str(); break;
	case (PLOT_COAST_POINT)    : Name = VECTOR_COAST_POINT_NAME    .c_str(); break;
	}

	Name	+= m_strRunName.c_str();

	switch (nDataItem)
	{
		//-----------------------------------------------------
	case (PLOT_COAST):
	{
		pShapes->Create(SHAPE_TYPE_Line, Name);

		pShapes->Add_Field("Coast", SG_DATATYPE_Int);	// The layer has been created, so create an integer-numbered value (the number of the coast object) for the multi-line

		for (int i = 0; i < static_cast<int>(m_VCoast.size()); i++)	// OK, now do features
		{
			CSG_Shape	*pShape	= pShapes->Add_Shape();	// Create a feature object, one per coast

			pShape->Set_Value("Coast", i);	// Set the feature's attribute (the coast number)

			for (int j = 0; j < m_VCoast[i].pLGetCoastline()->nGetSize(); j++)	// Now attach a geometry to the feature object
			{
				pShape->Add_Point(	//  In external CRS
					m_VCoast[i].pPtGetVectorCoastlinePoint(j)->dGetX(),
					m_VCoast[i].pPtGetVectorCoastlinePoint(j)->dGetY()
				);
			}
		}
	}	break;

	//-----------------------------------------------------
	case (PLOT_NORMALS):
	case (PLOT_INVALID_NORMALS):
	{
		pShapes->Create(SHAPE_TYPE_Line, Name);

		pShapes->Add_Field("Normal"    , SG_DATATYPE_Int);	// The layer has been created, so create an integer-numbered value (the number of the normal) associated with the line
		pShapes->Add_Field("StartCoast", SG_DATATYPE_Int);	// Also create other integer-numbered values for the category codes of the coastline-normalprofile
		pShapes->Add_Field("EndCoast"  , SG_DATATYPE_Int);
		pShapes->Add_Field("HitLand"   , SG_DATATYPE_Int);
		pShapes->Add_Field("HitCoast"  , SG_DATATYPE_Int);
		pShapes->Add_Field("HitNormal" , SG_DATATYPE_Int);
		pShapes->Add_Field("nCoast"    , SG_DATATYPE_Int);

		for (int i = 0; i < static_cast<int>(m_VCoast.size()); i++) // for each coast object
		{
			for (int j = 0; j < m_VCoast[i].nGetNumProfiles(); j++)
			{
				CProfile* pProfile = m_VCoast[i].pGetProfile(j);

				if (((nDataItem == PLOT_NORMALS) && (pProfile->bOKIncStartAndEndOfCoast())) || ((nDataItem == PLOT_INVALID_NORMALS) && (! pProfile->bOKIncStartAndEndOfCoast())))
				{
					CSG_Shape	*pShape	= pShapes->Add_Shape();	// Create a feature object, one per profile

					pShape->Set_Value(0, j);	// Set the feature's attributes
					pShape->Set_Value(1, pProfile->bStartOfCoast     () ? 1 : 0);
					pShape->Set_Value(2, pProfile->bEndOfCoast       () ? 1 : 0);
					pShape->Set_Value(3, pProfile->bHitLand          () ? 1 : 0);
					pShape->Set_Value(4, pProfile->bHitCoast         () ? 1 : 0);
					pShape->Set_Value(5, pProfile->bHitAnotherProfile() ? 1 : 0);
					pShape->Set_Value(6, i);

					for (int k = 0; k < pProfile->nGetProfileSize(); k++)	// Now attach a geometry to the feature object
					{
						pShape->Add_Point(
							pProfile->pPtGetPointInProfile(k)->dGetX(),
							pProfile->pPtGetPointInProfile(k)->dGetY()
						);
					}
				}
			}
		}
	}	break;

	//-----------------------------------------------------
	case (PLOT_COAST_CURVATURE):
	{
		pShapes->Create(SHAPE_TYPE_Point, Name);

		pShapes->Add_Field("Curve", SG_DATATYPE_Double);	// The layer has been created, so create an integer-numbered value (the number of the normal) associated with the line

		for (int i = 0; i < static_cast<int>(m_VCoast.size()); i++)	// OK, now create features
		{
			for (int j = 0; j < m_VCoast[i].pLGetCoastline()->nGetSize(); j++)
			{
				CSG_Shape	*pShape	= pShapes->Add_Shape();	// Create a feature object, one per coastline point

				pShape->Add_Point(	// Set the feature's geometry (in external CRS)
					m_VCoast[i].pPtGetVectorCoastlinePoint(j)->dGetX(),
					m_VCoast[i].pPtGetVectorCoastlinePoint(j)->dGetY()
				);

				double dCurvature = m_VCoast[i].dGetCurvature(j);
				if (dCurvature == DBL_NODATA)
				{
					pShape->Set_NoData(0);
				}
				else
				{
					pShape->Set_Value(0, dCurvature);
				}
			}
		}
	}	break;

	//-----------------------------------------------------
	case (PLOT_CLIFF_TOP):
	case (PLOT_CLIFF_TOE):
	case (PLOT_COAST_POINT):
	{
		pShapes->Create(SHAPE_TYPE_Point, Name);

		pShapes->Add_Field("nCoast"  , SG_DATATYPE_Int   );	// The layer has been created, so create an integer-numbered value (the number of the coast) associated with the line and the profile
		pShapes->Add_Field("nProf"   , SG_DATATYPE_Int   );
		pShapes->Add_Field("bisOK"   , SG_DATATYPE_Int   );
		pShapes->Add_Field("CoastEl" , SG_DATATYPE_Double);	// Also create other real-numbered values for the cliff top and toe elevation and chainage
		pShapes->Add_Field("Chainage", SG_DATATYPE_Double);

		if(nDataItem == PLOT_CLIFF_TOP)
			pShapes->Set_Field_Name(3, SG_T("CliffTopEl"));
		else if (nDataItem == PLOT_CLIFF_TOE)
			pShapes->Set_Field_Name(4, SG_T("CliffToeEl"));

		for (int i = 0; i < static_cast<int>(m_VCoast.size()); i++) // for each coast object
		{
			for (int j = 0; j < m_VCoast[i].nGetNumProfiles(); j++) // for each profile object
			{
				CProfile* pProfile = m_VCoast[i].pGetProfile(j);

				if (pProfile->bOKIncStartAndEndOfCoast()) 
				{
					CSG_Shape	*pShape	= pShapes->Add_Shape();	// Create a feature object, one per profile

					int	CliffPointIndex	= 0;

					if (nDataItem == PLOT_CLIFF_TOP)  
					{
						CliffPointIndex	= pProfile->nGetCliffTopPoint();

						pShape->Set_Value(2, pProfile->bGetCliffTopQualityFlag() ? 1 : 0);
						pShape->Set_Value(4, pProfile->dGetCliffTopChainage());
					}
					else if (nDataItem == PLOT_CLIFF_TOE)  
					{
						CliffPointIndex	= pProfile->nGetCliffToePoint();

						pShape->Set_Value(2, pProfile->bGetCliffToeQualityFlag() ? 1 : 0);
						pShape->Set_Value(4, pProfile->dGetCliffToeChainage());
					}                  
					else if (nDataItem == PLOT_COAST_POINT)  
					{
						pShape->Set_Value(2, 1);	// to do, sanity check for coastline points
						pShape->Set_Value(4, 0);
					}

					int	nX	= pProfile->pPtiVGetCellsInProfile()->at(CliffPointIndex).nGetX();
					int	nY	= pProfile->pPtiVGetCellsInProfile()->at(CliffPointIndex).nGetY();

					pShape->Add_Point(
						dGridCentroidXToExtCRSX(nX),
						dGridCentroidYToExtCRSY(nY)
					);

					// Set the feature's attributes
					pShape->Set_Value(0, i);
					pShape->Set_Value(1, j);
					pShape->Set_Value(3, m_pRasterGrid->pGetCell(nX, nY)->dGetSedimentTopElev());	// Get the elevation for both consolidated and unconsolidated sediment on this cell
				}
			}
		}
	}	break;
	}

	return( true );
}

#endif // #if defined(_SAGA_MSW) || defined(SAGA_LINUX)
