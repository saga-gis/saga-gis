/**********************************************************
 * Version $Id: gdal_catalogue.cpp 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                       io_gdal                         //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  gdal_catalogue.cpp                   //
//                                                       //
//                 Copyright (C) 2015 by                 //
//                      Olaf Conrad                      //
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
//                Institute of Geography                 //
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "gdal_catalogue.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGDAL_Catalogue::CGDAL_Catalogue(void)
{
	//-----------------------------------------------------
	Set_Name	(_TL("Create Raster Catalogue"));

	Set_Author	("O.Conrad (c) 2015");

	CSG_String	Description;

	Description	= _TW(
		"The \"GDAL Raster Import\" module imports grid data from various file formats using the "
		"\"Geospatial Data Abstraction Library\" (GDAL) by Frank Warmerdam. "
		"For more information have a look at the GDAL homepage:\n"
		"  <a target=\"_blank\" href=\"http://www.gdal.org/\">"
		"  http://www.gdal.org</a>\n"
	);

	Description	+= CSG_String::Format("\nGDAL %s:%s\n\n", _TL("Version"), SG_Get_GDAL_Drivers().Get_Version().c_str());

	Description	+= _TW(
		"Following raster formats are currently supported:\n"
		"<table border=\"1\"><tr><th>ID</th><th>Name</th></tr>\n"
	);

	for(int i=0; i<SG_Get_GDAL_Drivers().Get_Count(); i++)
    {
		Description	+= CSG_String::Format("<tr><td>%s</td><td>%s</td></tr>\n",
			SG_Get_GDAL_Drivers().Get_Description(i).c_str(),
			SG_Get_GDAL_Drivers().Get_Name       (i).c_str()
		);
    }

	Description	+= "</table>";

	Set_Description(Description);

	//-----------------------------------------------------
	Parameters.Add_Shapes(
		NULL	, "CATALOGUE"	, _TL("Raster Catalogue"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_FilePath(
		NULL	, "FILES"		, _TL("Files"),
		_TL(""),
		NULL, NULL, false, false, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGDAL_Catalogue::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	return( CSG_Module::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGDAL_Catalogue::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Strings	Files;

	if( !Parameters("FILES")->asFilePath()->Get_FilePaths(Files) )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_Shapes	*pCatalogue	= Parameters("CATALOGUE")->asShapes();

	pCatalogue->Create(SHAPE_TYPE_Polygon, _TL("Raster Catalogue"));

	pCatalogue->Add_Field("ID"      , SG_DATATYPE_Int   );
	pCatalogue->Add_Field("NAME"    , SG_DATATYPE_String);
	pCatalogue->Add_Field("FILE"    , SG_DATATYPE_String);
	pCatalogue->Add_Field("CRS"     , SG_DATATYPE_String);
	pCatalogue->Add_Field("BANDS"   , SG_DATATYPE_Int   );
	pCatalogue->Add_Field("CELLSIZE", SG_DATATYPE_Double);
	pCatalogue->Add_Field("ROWS"    , SG_DATATYPE_Int   );
	pCatalogue->Add_Field("COLUMNS" , SG_DATATYPE_Int   );

	//-----------------------------------------------------
	for(int i=0; i<Files.Get_Count() && Set_Progress(i, Files.Get_Count()); i++)
	{
		CSG_String	Name	= SG_File_Get_Name(Files[i], true);

		Process_Set_Text(CSG_String::Format("\n%s: %s", _TL("loading"), Name.c_str()));

		CSG_GDAL_DataSet	DataSet;

		if( DataSet.Open_Read(Files[i]) == false )
		{
			Message_Add(_TL("failed: could not find a suitable import driver"));
		}
		else
		{
			CSG_Grid_System	System	= DataSet.Get_System();

			CSG_Shape	*pEntry	= pCatalogue->Add_Shape();

			pEntry->Set_Value(0, pCatalogue->Get_Count ());
			pEntry->Set_Value(1, Name                    );
			pEntry->Set_Value(2, DataSet.Get_File_Name ());
			pEntry->Set_Value(3, DataSet.Get_Projection());
			pEntry->Set_Value(4, DataSet.Get_Count     ());
			pEntry->Set_Value(5, System.Get_Cellsize   ());
			pEntry->Set_Value(6, System.Get_NX         ());
			pEntry->Set_Value(7, System.Get_NY         ());

			pEntry->Add_Point(System.Get_XMin(), System.Get_YMin());
			pEntry->Add_Point(System.Get_XMin(), System.Get_YMax());
			pEntry->Add_Point(System.Get_XMax(), System.Get_YMax());
			pEntry->Add_Point(System.Get_XMax(), System.Get_YMin());
			pEntry->Add_Point(System.Get_XMin(), System.Get_YMin());

			if( !pCatalogue->Get_Projection().is_Okay() )
			{
				pCatalogue->Get_Projection().Create(DataSet.Get_Projection(), SG_PROJ_FMT_WKT);
			}
		}
	}

	//-----------------------------------------------------
	return( pCatalogue->Get_Count() > 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
