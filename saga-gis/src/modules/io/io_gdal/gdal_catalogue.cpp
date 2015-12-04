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
	Set_Name	(_TL("Create Raster Catalogue from Files"));

	Set_Author	("O.Conrad (c) 2015");

	CSG_String	Description;

	Description	= _TW(
		"Create a raster catalogue from selected files. Output is a polygon layer "
		"that shows the extent for each valid raster file. Each extent is attributed with "
		"the original file path and raster system properties.\n"
		"\n"
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

		Process_Set_Text(CSG_String::Format("\n%s: %s", _TL("analyzing"), Name.c_str()));

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
CGDAL_Catalogues::CGDAL_Catalogues(void)
{
	//-----------------------------------------------------
	Set_Name	(_TL("Create Raster Catalogues from Directory"));

	Set_Author	("O.Conrad (c) 2015");

	CSG_String	Description;

	Description	= _TW(
		"Creates raster catalogues from all raster files found in the selected directory. "
		"Catalogues are splitted according to the coordinate reference system used by the "
		"different raster files. Output are polygon layers that show the extent for each "
		"raster file in it. Each extent is attributed with the original file path and "
		"raster system properties.\n"
		"\n"
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
	Parameters.Add_Shapes_List(
		NULL	, "CATALOGUES"		, _TL("Raster Catalogues"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Shapes(
		NULL	, "CATALOGUE_GCS"	, _TL("Raster Catalogue"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Shapes(
		NULL	, "CATALOGUE_UKN"	, _TL("Raster Catalogue (unknown CRS)"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Polygon
	);

	Parameters.Add_FilePath(
		NULL	, "DIRECTORY"		, _TL("Directory"),
		_TL(""),
		NULL, NULL, false, true
	);

	Parameters.Add_String(
		NULL	, "EXTENSIONS"		, _TL("Extensions"),
		_TL(""),
		"sgrd; tif"
	);

	Parameters.Add_Choice(
		NULL	, "OUTPUT"		, _TL("Output"),
		_TL(""),
		CSG_String::Format("%s|%s|",
			_TL("one catalogue for each coordinate system"),
			_TL("one catalogue using geographic coordinates")
		), 1
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGDAL_Catalogues::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), "OUTPUT") )
	{
		pParameters->Set_Enabled("CATALOGUES"   , pParameter->asInt() == 0);
		pParameters->Set_Enabled("CATALOGUE_GCS", pParameter->asInt() == 1);
		pParameters->Set_Enabled("CATALOGUE_UKN", pParameter->asInt() == 1);
	}

	return( CSG_Module::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGDAL_Catalogues::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_String	Directory	= Parameters("DIRECTORY")->asString();

	if( !SG_Dir_Exists(Directory) )
	{
		return( false );
	}

	//-----------------------------------------------------
	m_pCatalogues	= Parameters("CATALOGUES")->asShapesList();
	m_pCatalogues	->Del_Items();

	//-----------------------------------------------------
	CSG_String_Tokenizer	Extensions(Parameters("EXTENSIONS")->asString(), ";");

	while( Extensions.Has_More_Tokens() )
	{
		CSG_String	Extension	= Extensions.Get_Next_Token();	Extension.Trim(true);	Extension.Trim(false);

		if( Extension.CmpNoCase("sgrd") == 0 )	// gdal ignores saga's grid header file, use grid data file instead!
		{
			Extension	= "sdat";
		}

		if( !Extension.is_Empty() )
		{
			m_Extensions	+= Extension;
		}
	}

	if( m_Extensions.Get_Count() == 0 )
	{
		m_Extensions	+= "";
	}

	//-----------------------------------------------------
	int	n	= Add_Directory(Directory);

	m_Extensions.Clear();

	if( n <= 0 )
	{
		Message_Add(CSG_String::Format("\n%s\n", _TL("No raster files have been found in directory.")));

		return( false );
	}

	Message_Add(CSG_String::Format("\n%s: %d\n", _TL("Number of raster files found in directory"), n), false);

	//-----------------------------------------------------
	if( Parameters("OUTPUT")->asInt() == 1 )	// one catalogue using geographic coordinates
	{
		Get_Catalogue(CSG_Projection("+proj=longlat +datum=WGS84", SG_PROJ_FMT_Proj4),
			Parameters("CATALOGUE_GCS")->asShapes(), _TL("Raster Catalogue")
		);

		Get_Catalogue(CSG_Projection(),
			Parameters("CATALOGUE_UKN")->asShapes(), _TL("Raster Catalogue (unknown CRS)")
		);

		for(int i=m_pCatalogues->Get_Count()-1; i>=0 && Process_Get_Okay(true); i--)
		{
			CSG_Shapes	*pCatalogue	= m_pCatalogues->asShapes(i);

			Add_To_Geographic(pCatalogue);

			delete(pCatalogue);

			m_pCatalogues->Del_Item(i);
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Shapes * CGDAL_Catalogues::Get_Catalogue(const CSG_Projection &Projection, CSG_Shapes *pCatalogue, const CSG_String &Name)
{
	if( pCatalogue )
	{
		pCatalogue->Create(SHAPE_TYPE_Polygon, Name);

		pCatalogue->Add_Field("ID"      , SG_DATATYPE_Int   );
		pCatalogue->Add_Field("NAME"    , SG_DATATYPE_String);
		pCatalogue->Add_Field("FILE"    , SG_DATATYPE_String);
		pCatalogue->Add_Field("CRS"     , SG_DATATYPE_String);
		pCatalogue->Add_Field("PROJ4"   , SG_DATATYPE_String);
		pCatalogue->Add_Field("BANDS"   , SG_DATATYPE_Int   );
		pCatalogue->Add_Field("CELLSIZE", SG_DATATYPE_Double);
		pCatalogue->Add_Field("ROWS"    , SG_DATATYPE_Int   );
		pCatalogue->Add_Field("COLUMNS" , SG_DATATYPE_Int   );

		pCatalogue->Get_Projection().Create(Projection);
	}

	return( pCatalogue );
}

//---------------------------------------------------------
CSG_Shapes * CGDAL_Catalogues::Get_Catalogue(const CSG_Projection &Projection)
{
	for(int i=0; i<m_pCatalogues->Get_Count(); i++)
	{
		if( Projection.is_Equal(m_pCatalogues->asShapes(i)->Get_Projection()) )
		{
			return( m_pCatalogues->asShapes(i) );
		}
	}

	//-----------------------------------------------------
	CSG_Shapes	*pCatalogue	= Get_Catalogue(Projection, SG_Create_Shapes(), CSG_String::Format("%s No.%d", _TL("Raster Catalogue"), 1 + m_pCatalogues->Get_Count()));

	m_pCatalogues->Add_Item(pCatalogue);

	return( pCatalogue );
}

//---------------------------------------------------------
int CGDAL_Catalogues::Add_Directory(const CSG_String &Directory)
{
	int	n	= 0;

	CSG_Strings	List;

	//-----------------------------------------------------
	for(int iExtension=0; iExtension<m_Extensions.Get_Count(); iExtension++)
	{
		if( SG_Dir_List_Files(List, Directory, m_Extensions[iExtension]) )
		{
			for(int i=0; i<List.Get_Count() && Process_Get_Okay(true); i++)
			{
				n	+= Add_File(List[i]);
			}
		}
	}

	//-----------------------------------------------------
	if( SG_Dir_List_Subdirectories(List, Directory) )
	{
		for(int i=0; i<List.Get_Count() && Process_Get_Okay(true); i++)
		{
			n	+= Add_Directory(List[i]);
		}
	}

	return( n );
}

//---------------------------------------------------------
int CGDAL_Catalogues::Add_File(const CSG_String &File)
{
	CSG_String	Name	= SG_File_Get_Name(File, true);

	Process_Set_Text(CSG_String::Format("\n%s: %s", _TL("analyzing"), Name.c_str()));

	//-----------------------------------------------------
	CSG_GDAL_DataSet	DataSet;

	if( DataSet.Open_Read(File) == false )
	{
		return( 0 );
	}

	//-----------------------------------------------------
	CSG_Projection	Projection(DataSet.Get_Projection(), SG_PROJ_FMT_WKT);

	CSG_Shapes	*pCatalogue	= Get_Catalogue(Projection);

	if( !pCatalogue )
	{
		return( 0 );
	}

	//-----------------------------------------------------
	CSG_Grid_System	System	= DataSet.Get_System();

	CSG_Shape	*pEntry	= pCatalogue->Add_Shape();

	CSG_String	Filename	= DataSet.Get_File_Name();

	if( SG_File_Cmp_Extension(Filename, SG_T("sdat")) )
	{
		SG_File_Set_Extension(Filename, "sgrd");
	}

	pEntry->Set_Value(0, pCatalogue->Get_Count ());
	pEntry->Set_Value(1, Name                    );
	pEntry->Set_Value(2, Filename                );
	pEntry->Set_Value(3, DataSet.Get_Projection());
	pEntry->Set_Value(4, Projection.Get_Proj4  ());
	pEntry->Set_Value(5, DataSet.Get_Count     ());
	pEntry->Set_Value(6, System.Get_Cellsize   ());
	pEntry->Set_Value(7, System.Get_NX         ());
	pEntry->Set_Value(8, System.Get_NY         ());

	pEntry->Add_Point(System.Get_XMin(), System.Get_YMin());
	pEntry->Add_Point(System.Get_XMin(), System.Get_YMax());
	pEntry->Add_Point(System.Get_XMax(), System.Get_YMax());
	pEntry->Add_Point(System.Get_XMax(), System.Get_YMin());
	pEntry->Add_Point(System.Get_XMin(), System.Get_YMin());

	return( 1 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGDAL_Catalogues::Add_To_Geographic(CSG_Shapes *pCatalogue)
{
	CSG_Shapes	Catalogue, *pTarget	= Parameters("CATALOGUE_UKN")->asShapes();

	if( pCatalogue->Get_Projection().is_Okay() )
	{
		bool	bResult;

		SG_RUN_MODULE(bResult, "pj_proj4", 2,
				SG_MODULE_PARAMETER_SET("SOURCE"   , pCatalogue)
			&&	SG_MODULE_PARAMETER_SET("TARGET"   , &Catalogue)
			&&	SG_MODULE_PARAMETER_SET("CRS_PROJ4", Parameters("CATALOGUE_GCS")->asShapes()->Get_Projection().Get_Proj4())
		);

		if( bResult )
		{
			pCatalogue	= &Catalogue;

			pTarget	= Parameters("CATALOGUE_GCS")->asShapes();
		}
	}

	//-----------------------------------------------------
	if( pTarget )
	{
		for(int i=0; i<pCatalogue->Get_Count(); i++)
		{
			pTarget->Add_Shape(pCatalogue->Get_Shape(i));
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
