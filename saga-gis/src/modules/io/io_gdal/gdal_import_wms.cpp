/**********************************************************
 * Version $Id: gdal_import_wms.cpp 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

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
//                  gdal_import_wms.cpp                  //
//                                                       //
//            Copyright (C) 2016 O. Conrad               //
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
#include "gdal_import_wms.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGDAL_Import_WMS::CGDAL_Import_WMS(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name	(_TL("Import Open Street Map Image"));

	Set_Author	("O.Conrad (c) 2016");

	CSG_String	Description;

	Description	= _TW(
		"The \"Import OSM Image\" tool imports a map image from a Tile Mapping Service (TMS) using the "
		"\"Geospatial Data Abstraction Library\" (GDAL) by Frank Warmerdam. "
		"For more information have a look at the GDAL homepage:\n"
		"  <a target=\"_blank\" href=\"http://www.gdal.org/\">"
		"  http://www.gdal.org</a>\n"
	);

	Description	+= CSG_String::Format("\nGDAL %s:%s\n\n", _TL("Version"), SG_Get_GDAL_Drivers().Get_Version().c_str());

	Set_Description(Description);

	//-----------------------------------------------------
	pNode	= Parameters.Add_Grid(
		NULL	, "TARGET"		, _TL("Target System"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL
	)->Get_Parent();

	Parameters.Add_Grid(
		pNode	, "TARGET_MAP"	, _TL("Target Map"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid_Output(
		NULL	, "MAP"			, _TL("Map"),
		_TL("")
	);

	//-----------------------------------------------------
	pNode	= Parameters.Add_Choice(
		NULL	, "SERVER"		, _TL("Server"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|",
			_TL("Open Street Map"),
			_TL("MapQuest"),
			_TL("user defined")
		), 0
	);

	Parameters.Add_Value(
		pNode	, "BLOCKSIZE"	, _TL("Block Size"),
		_TL(""),
		PARAMETER_TYPE_Int, 256, 32, true
	);

	Parameters.Add_String(
		pNode	, "SERVER_USER"	, _TL("Server"),
		_TL(""),
		"tile.openstreetmap.org"
	);

	pNode	= Parameters.Add_Value(
		NULL	, "CACHE"		, _TL("Cache"),
		_TL("Enable local disk cache. Allows for offline operation."),
		PARAMETER_TYPE_Bool, false
	);

	Parameters.Add_FilePath(
		pNode	, "CACHE_DIR"	, _TL("Cache Directory"),
		_TL("If not specified the cache will be created in the current user's temporary directory."),
		NULL, NULL, false, true
	);

	Parameters.Add_Value(
		NULL	, "GRAYSCALE"	, _TL("Gray Scale Image"),
		_TL(""),
		PARAMETER_TYPE_Bool, false
	);

	//-----------------------------------------------------
	pNode	= Parameters.Add_Node(NULL, "TARGET_NODE", _TL("Target Grid"), _TL(""));

	Parameters.Add_Value(pNode, "XMIN", _TL("West"   ), _TL(""), PARAMETER_TYPE_Double, -20037508.34, -20037508.34, true, 20037508.34, true);
	Parameters.Add_Value(pNode, "YMIN", _TL("South"  ), _TL(""), PARAMETER_TYPE_Double, -20037508.34, -20037508.34, true, 20037508.34, true);
	Parameters.Add_Value(pNode, "XMAX", _TL("East"   ), _TL(""), PARAMETER_TYPE_Double,  20037508.34, -20037508.34, true, 20037508.34, true);
	Parameters.Add_Value(pNode, "YMAX", _TL("North"  ), _TL(""), PARAMETER_TYPE_Double,  20037508.34, -20037508.34, true, 20037508.34, true);
	Parameters.Add_Value(pNode, "NX"  , _TL("Columns"), _TL(""), PARAMETER_TYPE_Int, 600, 1, true);
	Parameters.Add_Value(pNode, "NY"  , _TL("Rows"   ), _TL(""), PARAMETER_TYPE_Int, 600, 1, true);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGDAL_Import_WMS::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	CSG_Parameter	*pXMin	= pParameters->Get_Parameter("XMIN");
	CSG_Parameter	*pYMin	= pParameters->Get_Parameter("YMIN");
	CSG_Parameter	*pXMax	= pParameters->Get_Parameter("XMAX");
	CSG_Parameter	*pYMax	= pParameters->Get_Parameter("YMAX");
	CSG_Parameter	*pNX	= pParameters->Get_Parameter("NX"  );
	CSG_Parameter	*pNY	= pParameters->Get_Parameter("NY"  );

	if( !SG_STR_CMP(pParameter->Get_Identifier(), "NX") )
	{
		double	d	= fabs(pXMax->asDouble() - pXMin->asDouble()) / pNX->asDouble();
		pNY  ->Set_Value(fabs(pYMax->asDouble() - pYMin->asDouble()) / d);
		pYMax->Set_Value(pYMin->asDouble() + d * pNY->asDouble());
	}

	if( !SG_STR_CMP(pParameter->Get_Identifier(), "NY") )
	{
		double	d	= fabs(pYMax->asDouble() - pYMin->asDouble()) / pNY->asDouble();
		pNX  ->Set_Value(fabs(pXMax->asDouble() - pXMin->asDouble()) / d);
		pXMax->Set_Value(pXMin->asDouble() + d * pNX->asDouble());
	}

	if( !SG_STR_CMP(pParameter->Get_Identifier(), "XMIN") )
	{
		double	d	= fabs(pYMax->asDouble() - pYMin->asDouble()) / pNY->asDouble();
		pNX  ->Set_Value(fabs(pXMax->asDouble() - pXMin->asDouble()) / d);
		pXMax->Set_Value(pXMin->asDouble() + d * pNX->asDouble());
	}

	if( !SG_STR_CMP(pParameter->Get_Identifier(), "YMIN") )
	{
		double	d	= fabs(pXMax->asDouble() - pXMin->asDouble()) / pNX->asDouble();
		pNY  ->Set_Value(fabs(pYMax->asDouble() - pYMin->asDouble()) / d);
		pYMax->Set_Value(pYMin->asDouble() + d * pNY->asDouble());
	}

	if( !SG_STR_CMP(pParameter->Get_Identifier(), "XMAX") )
	{
		double	d	= fabs(pYMax->asDouble() - pYMin->asDouble()) / pNY->asDouble();
		pNX  ->Set_Value(fabs(pXMax->asDouble() - pXMin->asDouble()) / d);
		pYMax->Set_Value(pYMax->asDouble() - d * pNY->asDouble());
	}

	if( !SG_STR_CMP(pParameter->Get_Identifier(), "YMAX") )
	{
		double	d	= fabs(pXMax->asDouble() - pXMin->asDouble()) / pNX->asDouble();
		pNY  ->Set_Value(fabs(pYMax->asDouble() - pYMin->asDouble()) / d);
		pXMax->Set_Value(pXMax->asDouble() - d * pNX->asDouble());
	}

	return( CSG_Module::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
int CGDAL_Import_WMS::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), "TARGET") )
	{
		pParameters->Set_Enabled("TARGET_MAP" , pParameter->asGrid() != NULL);
		pParameters->Set_Enabled("TARGET_NODE", pParameter->asGrid() == NULL);
	}

	if( !SG_STR_CMP(pParameter->Get_Identifier(), "SERVER") )
	{
		pParameters->Set_Enabled("SERVER_USER", pParameter->asInt() == 2);
	}

	if( !SG_STR_CMP(pParameter->Get_Identifier(), "CACHE") )
	{
		pParameters->Set_Enabled("CACHE_DIR", pParameter->asBool());
	}

	return( CSG_Module::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGDAL_Import_WMS::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Grid_System	System;

	if( !Get_System(System, Parameters("TARGET")->asGrid()) )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_Grid	*pBands[3];

	if( !Get_Bands(pBands, System) )
	{
		Error_Set(_TL("failed to retrieve map image data"));

		return( false );
	}

	//-----------------------------------------------------
	if( Parameters("TARGET")->asGrid() )
	{
		Get_Projected(pBands, Parameters("TARGET")->asGrid());
	}

	//-----------------------------------------------------
	return( Set_Image(pBands) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGDAL_Import_WMS::Get_System(CSG_Grid_System &System, CSG_Grid *pTarget)
{
	//-----------------------------------------------------
	if( !pTarget )
	{
		CSG_Rect	Extent(
			Parameters("XMIN")->asDouble(), Parameters("YMIN")->asDouble(),
			Parameters("XMAX")->asDouble(), Parameters("YMAX")->asDouble()
		);

		double	Cellsize	= Extent.Get_XRange() / Parameters("NX")->asDouble();

		return( System.Assign(Cellsize, Extent) );
	}

	//-----------------------------------------------------
	if( !pTarget->Get_Projection().is_Okay() )
	{
		return( false );
	}

	CSG_Shapes	rTarget(SHAPE_TYPE_Point), rSource;

	rTarget.Get_Projection()	= pTarget->Get_Projection();

	CSG_Rect	Extent	= pTarget->Get_Extent(true);

	rTarget.Add_Shape()->Add_Point(Extent.Get_XMin   (), Extent.Get_YMin   ());
	rTarget.Add_Shape()->Add_Point(Extent.Get_XMin   (), Extent.Get_YCenter());
	rTarget.Add_Shape()->Add_Point(Extent.Get_XMin   (), Extent.Get_YMax   ());
	rTarget.Add_Shape()->Add_Point(Extent.Get_XCenter(), Extent.Get_YMax   ());
	rTarget.Add_Shape()->Add_Point(Extent.Get_XMax   (), Extent.Get_YMax   ());
	rTarget.Add_Shape()->Add_Point(Extent.Get_XMax   (), Extent.Get_YCenter());
	rTarget.Add_Shape()->Add_Point(Extent.Get_XMax   (), Extent.Get_YMin   ());
	rTarget.Add_Shape()->Add_Point(Extent.Get_XCenter(), Extent.Get_YMin   ());

	//-----------------------------------------------------
	CSG_Module	*pModule	= SG_Get_Module_Library_Manager().Get_Module("pj_proj4", 2);	// Coordinate Transformation (Shapes);

	if(	!pModule )
	{
		return( false );
	}

	pModule->Settings_Push();

	if( SG_MODULE_PARAMETER_SET("CRS_PROJ4" , SG_T("+proj=merc +a=6378137 +b=6378137 +lat_ts=0.0 +k=1.0"))
	&&  SG_MODULE_PARAMETER_SET("SOURCE"    , &rTarget)
	&&  SG_MODULE_PARAMETER_SET("TARGET"    , &rSource)
	&&  pModule->Execute() )
	{
		double	Cellsize	= rSource.Get_Extent().Get_XRange() / pTarget->Get_NX() < rSource.Get_Extent().Get_YRange() / pTarget->Get_NY()
							? rSource.Get_Extent().Get_XRange() / pTarget->Get_NX() : rSource.Get_Extent().Get_YRange() / pTarget->Get_NY();

		System.Assign(Cellsize, rSource.Get_Extent());

		pModule->Settings_Pop();

		return( true );
	}

	pModule->Settings_Pop();

	return( false );
}

///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGDAL_Import_WMS::Get_Projected(CSG_Grid *pBands[3], CSG_Grid *pTarget)
{
	CSG_Module	*pModule	= SG_Get_Module_Library_Manager().Get_Module("pj_proj4", 3);	// Coordinate Transformation (Grid List);

	if(	!pModule )
	{
		return( false );
	}

	//-----------------------------------------------------
	pModule->Settings_Push();

	if( SG_MODULE_PARAMETER_SET("CRS_PROJ4" , pTarget->Get_Projection().Get_Proj4())
	&&  SG_MODULE_PARAMETER_SET("RESAMPLING", 3)
	&&  SG_MODULE_PARAMLIST_ADD("SOURCE"    , pBands[2])
	&&  SG_MODULE_PARAMLIST_ADD("SOURCE"    , pBands[1])
	&&  SG_MODULE_PARAMLIST_ADD("SOURCE"    , pBands[0])
	&&  pModule->Get_Parameters("TARGET")->Get_Parameter("DEFINITION")->Set_Value(1)
	&&  pModule->Get_Parameters("TARGET")->Get_Parameter("SYSTEM")->asGrid_System()->Assign(pTarget->Get_System())
	&&  pModule->Execute() )
	{
		CSG_Parameter_Grid_List	*pGrids	= pModule->Get_Parameters()->Get_Parameter("GRIDS")->asGridList();

		delete(pBands[0]);	pBands[0]	= pGrids->asGrid(0);
		delete(pBands[1]);	pBands[1]	= pGrids->asGrid(1);
		delete(pBands[2]);	pBands[2]	= pGrids->asGrid(2);

		pModule->Settings_Pop();

		return( true );
	}

	pModule->Settings_Pop();

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGDAL_Import_WMS::Set_Image(CSG_Grid *pBands[3])
{
	//-----------------------------------------------------
	CSG_Grid	*pMap	= Parameters("TARGET_MAP")->asGrid();

	if( !pMap )
	{
		pMap	= SG_Create_Grid();
	}

	if( !pMap->Get_System().is_Equal(pBands[0]->Get_System()) )
	{
		pMap->Create(pBands[0]->Get_System(), SG_DATATYPE_Int);
	}

	pMap->Set_Name(_TL("Open Street Map"));

	pMap->Get_Projection()	= pBands[0]->Get_Projection();

	//-----------------------------------------------------
	bool	bGrayscale	= Parameters("GRAYSCALE")->asBool();

	#pragma omp parallel for
	for(int y=0; y<pMap->Get_NY(); y++)	for(int x=0; x<pMap->Get_NX(); x++)
	{
		if( bGrayscale )
		{
			double	z	= (pBands[0]->asInt(x, y) + pBands[1]->asInt(x, y) + pBands[2]->asInt(x, y)) / 3.0;

			pMap->Set_Value(x, y, SG_GET_RGB(z, z, z));
		}
		else
		{
			pMap->Set_Value(x, y, SG_GET_RGB(pBands[0]->asInt(x, y), pBands[1]->asInt(x, y), pBands[2]->asInt(x, y)));
		}
	}

	delete(pBands[0]);
	delete(pBands[1]);
	delete(pBands[2]);

	Parameters("MAP")->Set_Value(pMap);

	DataObject_Add(pMap);
	DataObject_Set_Parameter(pMap, "COLORS_TYPE", 6);	// RGB Coded Values

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGDAL_Import_WMS::Get_Bands(CSG_Grid *pBands[3], const CSG_Grid_System &System)
{
	//-----------------------------------------------------
	CSG_String	Server;

	switch( Parameters("SERVER")->asInt() )
	{
	default:	Server	= "tile.openstreetmap.org"             ;	break;	// Open Street Map
	case  1:	Server	= "otile1.mqcdn.com/tiles/1.0.0/osm"   ;	break;	// MapQuest
	case  2:	Server	= Parameters("SERVER_USER")->asString();	break;	// user defined
	}

	//-----------------------------------------------------
	CSG_GDAL_DataSet	DataSet;

	if( DataSet.Open_Read(Get_Request(Server), System) == false || DataSet.Get_Count() != 3 )
	{
		return( false );
	}

	Message_Add("\n", false);
	Message_Add(CSG_String::Format("\n%s: %s", _TL("Driver" ), DataSet.Get_DriverID().c_str()), false);
	Message_Add(CSG_String::Format("\n%s: %d", _TL("Bands"  ), DataSet.Get_Count()           ), false);
	Message_Add(CSG_String::Format("\n%s: %d", _TL("Rows"   ), DataSet.Get_NX()              ), false);
	Message_Add(CSG_String::Format("\n%s: %d", _TL("Columns"), DataSet.Get_NY()              ), false);
	Message_Add("\n", false);

	//-----------------------------------------------------
	SG_UI_Progress_Lock(true);

	pBands[0]	= DataSet.Read(0);
	pBands[1]	= DataSet.Read(1);
	pBands[2]	= DataSet.Read(2);

	SG_UI_Progress_Lock(false);

	//-----------------------------------------------------
	if( !pBands[0] || !pBands[1] || !pBands[2] )
	{
		if( pBands[0] )	delete(pBands[0]);
		if( pBands[1] )	delete(pBands[1]);
		if( pBands[2] )	delete(pBands[2]);

		return( false );
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String CGDAL_Import_WMS::Get_Request(const CSG_String &Server)
{
	CSG_MetaData	XML, *pEntry;

	XML.Set_Name("GDAL_WMS");

	//-----------------------------------------------------
	pEntry	= XML.Add_Child("Service");	pEntry->Add_Property("name", "TMS");

	pEntry->Add_Child("ServerUrl"  , "http://" + Server + "/${z}/${x}/${y}.png");

	//-----------------------------------------------------
	pEntry	= XML.Add_Child("DataWindow");		// Define size and extents of the data. (required, except for TiledWMS and VirtualEarth)

	pEntry->Add_Child("UpperLeftX" , -20037508.34);		// X (longitude) coordinate of upper-left corner. (optional, defaults to -180.0, except for VirtualEarth)
	pEntry->Add_Child("UpperLeftY" ,  20037508.34);		// Y (latitude) coordinate of upper-left corner. (optional, defaults to 90.0, except for VirtualEarth)
	pEntry->Add_Child("LowerRightX",  20037508.34);		// X (longitude) coordinate of lower-right corner. (optional, defaults to 180.0, except for VirtualEarth)
	pEntry->Add_Child("LowerRightY", -20037508.34);		// Y (latitude) coordinate of lower-right corner. (optional, defaults to -90.0, except for VirtualEarth)
	pEntry->Add_Child("TileLevel"  ,           18);		// Tile level at highest resolution. (tiled image sources only, optional, defaults to 0)
	pEntry->Add_Child("TileCountX" ,            1);		// Can be used to define image size, SizeX = TileCountX * BlockSizeX * 2TileLevel. (tiled image sources only, optional, defaults to 0)
	pEntry->Add_Child("TileCountY" ,            1);		// Can be used to define image size, SizeY = TileCountY * BlockSizeY * 2TileLevel. (tiled image sources only, optional, defaults to 0)
	pEntry->Add_Child("YOrigin"    ,        "top");		// Can be used to define the position of the Y origin with respect to the tile grid. Possible values are 'top', 'bottom', and 'default', where the default behavior is mini-driver-specific. (TMS mini-driver only, optional, defaults to 'bottom' for TMS)

	//-----------------------------------------------------
	pEntry	= XML.Add_Child("Projection", "EPSG:3857");	// Image projection (optional, defaults to value reported by mini-driver or EPSG:4326)
	pEntry	= XML.Add_Child("BandsCount",           3);	// Number of bands/channels, 1 for grayscale data, 3 for RGB, 4 for RGBA. (optional, defaults to 3)

	int	Blocksize	= Parameters("BLOCKSIZE")->asInt();
	pEntry	= XML.Add_Child("BlockSizeX", Blocksize);	// Block size in pixels. (optional, defaults to 1024, except for VirtualEarth)
	pEntry	= XML.Add_Child("BlockSizeY", Blocksize);

	//-----------------------------------------------------
	if( Parameters("CACHE")->asBool() )
	{
		pEntry	= XML.Add_Child("Cache");

		CSG_String	Path	= Parameters("CACHE_DIR")->asString();

		if( !SG_Dir_Exists(Path) )
		{
			Path	= SG_Dir_Get_Temp();
		}

		pEntry->Add_Child("Path", SG_File_Make_Path(Path, SG_T("gdalwmscache")));
	}

	//-----------------------------------------------------
	return( XML.asText(2) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
