
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library                     //
//                                                       //
//                       io_gdal                         //
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
	Set_Name		(_TL("GDAL: Export Raster to GeoTIFF"));

	Set_Author		(SG_T("O.Conrad (c) 2007"));

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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGDAL_Export_GeoTIFF::On_Execute(void)
{
	CSG_String				File_Name;
	CSG_Projection			Projection;
	CSG_Parameter_Grid_List	*pGrids;
	CSG_GDAL_DataSet		DataSet;

	//-----------------------------------------------------
	pGrids		= Parameters("GRIDS")	->asGridList();
	File_Name	= Parameters("FILE")	->asString();

	Get_Projection(Projection);

	//-----------------------------------------------------
	if( !DataSet.Open_Write(File_Name, SG_T("GTiff"), SG_Get_Grid_Type(pGrids), pGrids->Get_Count(), *Get_System(), Projection) )
	{
		return( false );
	}

	//-----------------------------------------------------
	for(int i=0; i<pGrids->Get_Count(); i++)
	{
		Process_Set_Text(CSG_String::Format(SG_T("%s %d"), _TL("Band"), i + 1));

		DataSet.Write(i, pGrids->asGrid(i));
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
