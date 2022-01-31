
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
// Free Software Foundation, either version 2 of the     //
// License, or (at your option) any later version.       //
//                                                       //
// SAGA is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU General Public        //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU General    //
// Public License along with this program; if not, see   //
// <http://www.gnu.org/licenses/>.                       //
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
	));

	Add_Reference("GDAL/OGR contributors", "2019",
		"GDAL/OGR Geospatial Data Abstraction software Library",
		"A translator library for raster and vector geospatial data formats. Open Source Geospatial Foundation.",
		SG_T("https://gdal.org"), SG_T("Link")
	);

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
			"%s (*.tif)|*.tif;*.tiff|%s|*.*",
			_TL("TIFF files"),
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
int CGDAL_Export_GeoTIFF::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("GRIDS") && pParameter->asGridList()->Get_Item_Count() > 0 )
	{
		CSG_String	Path(SG_File_Get_Path((*pParameters)["FILE"].asString()));

		pParameters->Set_Parameter("FILE", SG_File_Make_Path(Path, pParameter->asGridList()->Get_Item(0)->Get_Name(), "tif"));
	}

	return( CSG_Tool_Grid::On_Parameter_Changed(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGDAL_Export_GeoTIFF::On_Execute(void)
{
	CSG_Parameter_Grid_List	*pGrids	= Parameters("GRIDS")->asGridList();

	if( pGrids->Get_Grid_Count() < 1 )
	{
		return( false );
	}

	CSG_GDAL_DataSet DataSet; CSG_Projection Projection; Get_Projection(Projection);

	if( !DataSet.Open_Write(Parameters("FILE")->asString(), "GTiff", Parameters("OPTIONS")->asString(),
		SG_Get_Grid_Type(pGrids), pGrids->Get_Grid_Count(), Get_System(), Projection) )
	{
		return( false );
	}

	//-----------------------------------------------------
	for(int i=0; i<pGrids->Get_Grid_Count(); i++)
	{
		Process_Set_Text("%s %d", _TL("Band"), i + 1);

		DataSet.Write(i, pGrids->Get_Grid(i));

		if( pGrids->Get_Grid_Count() > 1 )
		{
			DataSet.Set_Description(i, pGrids->Get_Grid(i)->Get_Name());
		}
	}

	if( pGrids->Get_Grid_Count() == 1 )
	{
		DataSet.Set_Description(0, pGrids->Get_Grid(0)->Get_Description());
	}

	//-----------------------------------------------------
	return( DataSet.Close() );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
