/**********************************************************
 * Version $Id: gdal_export_geotiff.cpp 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
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
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
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
	Set_Name		(_TL("Export GeoTIFF"));

	Set_Author		("O.Conrad (c) 2007");

	Set_Description	(_TW(
		"The \"GDAL GeoTIFF Export\" tool exports one or more grids to a Geocoded Tagged Image File Format using the "
		"\"Geospatial Data Abstraction Library\" (GDAL) by Frank Warmerdam. "
		"For more information have a look at the GDAL homepage:\n"
		"  <a target=\"_blank\" href=\"http://www.gdal.org/\">"
		"  http://www.gdal.org</a>\n"
	));

	//-----------------------------------------------------
	Parameters.Add_Grid_List("",
		"GRIDS"	, _TL("Grid(s)"),
		_TL("The SAGA grids to be exported."),
		PARAMETER_INPUT
	);

	Parameters.Add_FilePath("",
		"FILE"	, _TL("File"),
		_TL("The GeoTIFF File to be created."),
		CSG_String::Format(
			"%s|*.tif;*.tiff|%s|*.*",
			_TL("TIFF files (*.tif)"),
			_TL("All Files")
		), NULL, true
	);
	
	Parameters.Add_String("",
		"OPTIONS", _TL("Creation Options"),
		_TL("A space separated list of key-value pairs (K=V)."),
		_TL("")
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGDAL_Export_GeoTIFF::On_Execute(void)
{
	CSG_String				File_Name, Options;
	CSG_Projection			Projection;
	CSG_Parameter_Grid_List	*pGrids;
	CSG_GDAL_DataSet		DataSet;

	//-----------------------------------------------------
	pGrids		= Parameters("GRIDS"  )->asGridList();
	File_Name	= Parameters("FILE"   )->asString();
	Options		= Parameters("OPTIONS")->asString();
	Get_Projection(Projection);

	//-----------------------------------------------------
	if( !DataSet.Open_Write(File_Name, "GTiff", Options, SG_Get_Grid_Type(pGrids), pGrids->Get_Count(), *Get_System(), Projection) )
	{
		return( false );
	}

	//-----------------------------------------------------
	for(int i=0; i<pGrids->Get_Count(); i++)
	{
		Process_Set_Text(CSG_String::Format("%s %d", _TL("Band"), i + 1));

		DataSet.Write(i, pGrids->asGrid(i));
	}

	if( !DataSet.Close() )
	{
		return( false );
	}
	
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
