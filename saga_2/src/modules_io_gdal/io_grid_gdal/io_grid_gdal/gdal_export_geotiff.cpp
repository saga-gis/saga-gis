
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library                     //
//                                                       //
//                     io_grid_gdal                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//               gdal_export_geotiff.cpp                 //
//                                                       //
//            Copyright (C) 2007 O. Conrad               //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'. SAGA is free software; you   //
// can redistribute it and/or modify it under the terms  //
// of the GNU General Public License as published by the //
// Free Software Foundation; version 2 of the License.   //
//                                                       //
// SAGA is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU General Public        //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU General    //
// Public License along with this program; if not,       //
// write to the Free Software Foundation, Inc.,          //
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
// USA.                                                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    e-mail:     oconrad@saga-gis.de                    //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Bundesstr. 55                          //
//                D-20146 Hamburg                        //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "gdal_export_geotiff.h"

#include <cpl_string.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGDAL_Export_GeoTIFF::CGDAL_Export_GeoTIFF(void)
{
	Set_Name		(_TL("Export Raster to GeoTIFF via GDAL"));

	Set_Author		(_TL("Copyrights (c) 2007 by O.Conrad"));

	Set_Description	(_TW(
		"The \"GDAL GeoTIFF Export\" module exports one or more grids to a Geocoded Tagged Image File Format using the "
		"\"Geospatial Data Abstraction Library\" (GDAL) by Frank Warmerdam. "
		"For more information have a look at the GDAL homepage:\n"
		"  <a target=\"_blank\" href=\"http://www.gdal.org/\">"
		"  http://www.gdal.org</a>\n"
	));

	//-----------------------------------------------------
	Parameters.Add_Grid_List(
		NULL, "GRIDS"	, _TL("Grid(s)"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_FilePath(
		NULL, "FILE"	, _TL("File"),
		_TL(""),

		CSG_String::Format(
			SG_T("%s|*.tif;*.tiff|%s|*.*"),
			_TL("TIFF files (*.tif)"),
			_TL("All Files")
		), NULL, true
	);
}

//---------------------------------------------------------
CGDAL_Export_GeoTIFF::~CGDAL_Export_GeoTIFF(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGDAL_Export_GeoTIFF::On_Execute(void)
{
	char					**pOptions	= NULL;
	int						x, y, n;
	double					*zLine;
	CSG_String				File_Name;
	CSG_Parameter_Grid_List	*pGrids;
	CSG_Grid				*pGrid;
	GDALDataType			gdal_Type;
	GDALDriver				*pDriver;
	GDALDataset				*pDataset;
	GDALRasterBand			*pBand;

	//-----------------------------------------------------
	pGrids		= Parameters("GRIDS")	->asGridList();
	File_Name	= Parameters("FILE")	->asString();

	//-----------------------------------------------------
	gdal_Type	= g_GDAL_Driver.Get_GDAL_Type(pGrids);

	//-----------------------------------------------------
	if( (pDriver = g_GDAL_Driver.Get_Driver("GTiff")) == NULL )
	{
		Message_Add(_TL("GeoTIFF driver not found."));
	}
	else if( CSLFetchBoolean(pDriver->GetMetadata(), GDAL_DCAP_CREATE, false) == false )
	{
		Message_Add(_TL("Driver does not support file creation."));
	}
	else if( (pDataset = pDriver->Create(File_Name.b_str(), Get_NX(), Get_NY(), pGrids->Get_Count(), gdal_Type, pOptions)) == NULL )
	{
		Message_Add(_TL("Could not create dataset."));
	}
	else
	{
		double	Transform[6]	= {	Get_XMin(), Get_Cellsize(), 0.0, Get_System()->Get_YMax(), 0.0, -Get_Cellsize()	};

		pDataset->SetGeoTransform(Transform);
		zLine	= (double *)SG_Malloc(Get_NX() * sizeof(double));

		for(n=0; n<pGrids->Get_Count(); n++)
		{
			Process_Set_Text(CSG_String::Format(SG_T("%s %d"), _TL("Band"), n + 1));

			pGrid	= pGrids->asGrid(n);
			pBand	= pDataset->GetRasterBand(n + 1);

			for(y=0; y<Get_NY() && Set_Progress(y, Get_NY()); y++)
			{
				for(x=0; x<Get_NX(); x++)
				{
					zLine[x]	= pGrid->asDouble(x, Get_NY() - 1 - y);
				}

				pBand->RasterIO(GF_Write, 0, y, Get_NX(), 1, zLine, Get_NX(), 1, GDT_Float64, 0, 0);
			}
		}

		//-------------------------------------------------
		SG_Free(zLine);

		GDALClose(pDataset);

		return( true );
	}

	//-----------------------------------------------------
	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
