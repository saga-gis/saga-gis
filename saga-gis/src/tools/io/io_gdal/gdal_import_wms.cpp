
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
//                Institute of Geography                 //
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
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
	Set_Name	(_TL("Import TMS Image"));

	Set_Author	("O.Conrad (c) 2016");

	CSG_String	Description;

	Description	= _TW(
		"The \"Import TMS Image\" tool imports a map image from a Tile Mapping Service (TMS) using the "
		"\"Geospatial Data Abstraction Library\" (GDAL) by Frank Warmerdam. "
	);

	Description	+= CSG_String::Format("\nGDAL %s:%s\n\n", _TL("Version"), SG_Get_GDAL_Drivers().Get_Version().c_str());

	Set_Description(Description);

	Add_Reference("GDAL/OGR contributors", "2019",
		"GDAL/OGR Geospatial Data Abstraction software Library",
		"A translator library for raster and vector geospatial data formats. Open Source Geospatial Foundation.",
		SG_T("https://gdal.org"), SG_T("Link")
	);

	//-----------------------------------------------------
	Parameters.Add_Grid("",
		"TARGET"	, _TL("Target System"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL
	)->Get_Parent();

	Parameters.Add_Grid("TARGET",
		"TARGET_MAP"	, _TL("Target Map"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	//-----------------------------------------------------
	Parameters.Add_Choice("",
		"SERVER"	, _TL("Server"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s|%s|%s|%s|%s|%s|%s",
			SG_T("Open Street Map"),
			SG_T("Google Map"),
			SG_T("Google Satellite"),
			SG_T("Google Hybrid"),
			SG_T("Google Terrain"),
			SG_T("Google Terrain, Streets and Water"),
			SG_T("ArcGIS MapServer Tiles"),
			SG_T("TopPlusOpen"),
			SG_T("EMODnet Bathymetry WMTS service"),
			SG_T("user defined")
		), 0
	);

	Parameters.Add_String("SERVER",
		"SERVER_USER", _TL("Server"),
		_TL(""),
		"tile.openstreetmap.org/${z}/${x}/${y}.png"
	);

	Parameters.Add_Int("SERVER",
		"SERVER_EPSG", _TL("EPSG"),
		_TL(""),
		3857
	);

	Parameters.Add_Int("SERVER",
		"BLOCKSIZE"	, _TL("Block Size"),
		_TL(""),
		256, 32, true
	);

	Parameters.Add_Bool("",
		"CACHE"		, _TL("Cache"),
		_TL("Enable local disk cache. Allows for offline operation."),
		false
	);

	Parameters.Add_FilePath("CACHE",
		"CACHE_DIR"	, _TL("Cache Directory"),
		_TL("If not specified the cache will be created in the current user's temporary directory."),
		NULL, NULL, false, true
	);

	Parameters.Add_Bool("",
		"GRAYSCALE"	, _TL("Gray Scale Image"),
		_TL(""),
		false
	);

	//-----------------------------------------------------
	Parameters.Add_Node("", "TARGET_NODE", _TL("Target Grid"), _TL(""));

	Parameters.Add_Double("TARGET_NODE", "XMIN", _TL("West"   ), _TL(""), -20037508.34, -20037508.34, true, 20037508.34, true);
	Parameters.Add_Double("TARGET_NODE", "YMIN", _TL("South"  ), _TL(""), -20037508.34, -20037508.34, true, 20037508.34, true);
	Parameters.Add_Double("TARGET_NODE", "XMAX", _TL("East"   ), _TL(""),  20037508.34, -20037508.34, true, 20037508.34, true);
	Parameters.Add_Double("TARGET_NODE", "YMAX", _TL("North"  ), _TL(""),  20037508.34, -20037508.34, true, 20037508.34, true);
	Parameters.Add_Int   ("TARGET_NODE", "NX"  , _TL("Columns"), _TL(""),  600, 1, true);
	Parameters.Add_Int   ("TARGET_NODE", "NY"  , _TL("Rows"   ), _TL(""),  600, 1, true);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGDAL_Import_WMS::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	#define pXMin (*pParameters)("XMIN")
	#define pYMin (*pParameters)("YMIN")
	#define pXMax (*pParameters)("XMAX")
	#define pYMax (*pParameters)("YMAX")
	#define pNX   (*pParameters)("NX"  )
	#define pNY   (*pParameters)("NY"  )

	if( pParameter->Cmp_Identifier("NX") )
	{
		double d = fabs(pXMax->asDouble() - pXMin->asDouble()) / pNX->asDouble();
		pNY  ->Set_Value(fabs(pYMax->asDouble() - pYMin->asDouble()) / d);
		pYMax->Set_Value(pYMin->asDouble() + d * pNY->asDouble());
	}

	if( pParameter->Cmp_Identifier("NY") )
	{
		double d = fabs(pYMax->asDouble() - pYMin->asDouble()) / pNY->asDouble();
		pNX  ->Set_Value(fabs(pXMax->asDouble() - pXMin->asDouble()) / d);
		pXMax->Set_Value(pXMin->asDouble() + d * pNX->asDouble());
	}

	if( pParameter->Cmp_Identifier("XMIN") )
	{
		double d = fabs(pYMax->asDouble() - pYMin->asDouble()) / pNY->asDouble();
		pNX  ->Set_Value(fabs(pXMax->asDouble() - pXMin->asDouble()) / d);
		pXMax->Set_Value(pXMin->asDouble() + d * pNX->asDouble());
	}

	if( pParameter->Cmp_Identifier("YMIN") )
	{
		double d = fabs(pXMax->asDouble() - pXMin->asDouble()) / pNX->asDouble();
		pNY  ->Set_Value(fabs(pYMax->asDouble() - pYMin->asDouble()) / d);
		pYMax->Set_Value(pYMin->asDouble() + d * pNY->asDouble());
	}

	if( pParameter->Cmp_Identifier("XMAX") )
	{
		double d = fabs(pYMax->asDouble() - pYMin->asDouble()) / pNY->asDouble();
		pNX  ->Set_Value(fabs(pXMax->asDouble() - pXMin->asDouble()) / d);
		pYMax->Set_Value(pYMin->asDouble() + d * pNY->asDouble());
	}

	if( pParameter->Cmp_Identifier("YMAX") )
	{
		double d = fabs(pXMax->asDouble() - pXMin->asDouble()) / pNX->asDouble();
		pNY  ->Set_Value(fabs(pYMax->asDouble() - pYMin->asDouble()) / d);
		pXMax->Set_Value(pXMin->asDouble() + d * pNX->asDouble());
	}

	return( CSG_Tool::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
int CGDAL_Import_WMS::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("TARGET") )
	{
		pParameters->Set_Enabled("TARGET_MAP" , pParameter->asPointer() != NULL);
		pParameters->Set_Enabled("TARGET_NODE", pParameter->asPointer() == NULL);
	}

	if( pParameter->Cmp_Identifier("SERVER") )
	{
		bool bUser = pParameter->asInt() >= pParameter->asChoice()->Get_Count() - 1;

		pParameters->Set_Enabled("SERVER_USER", bUser);
		pParameters->Set_Enabled("SERVER_EPSG", bUser);
	}

	if( pParameter->Cmp_Identifier("CACHE") )
	{
		pParameters->Set_Enabled("CACHE_DIR", pParameter->asBool());
	}

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGDAL_Import_WMS::On_Execute(void)
{
	CSG_Grid_System	System; CSG_Projection Projection;

	if( !Get_WMS_System(System, Projection) )
	{
		if( !SG_UI_Msg_is_Locked() )
		{
			Error_Set(_TL("failed to project target extent to server SRS"));
		}

		return( false );
	}

	//-----------------------------------------------------
	CSG_Grid *pBands[3];

	if( !Get_WMS_Bands(pBands, System, Projection) )
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
bool CGDAL_Import_WMS::Get_WMS_System(CSG_Grid_System &System, CSG_Projection &Projection)
{
	Projection.Create(Parameters("SERVER")->asInt() >= Parameters("SERVER")->asChoice()->Get_Count()
		? Parameters("SERVER_EPSG")->asInt() : 3857
	); // predefines default to EPSG:3857 => Web Mercator

	if( Projection.is_Okay() == false )
	{
		Projection.Set_GCS_WGS84();
	}

	//-----------------------------------------------------
	CSG_Grid *pTarget = Parameters("TARGET")->asGrid();

	if( !pTarget )
	{
		CSG_Rect Extent(
			Parameters("XMIN")->asDouble(), Parameters("YMIN")->asDouble(),
			Parameters("XMAX")->asDouble(), Parameters("YMAX")->asDouble()
		);

		double Cellsize = Extent.Get_XRange() / Parameters("NX")->asDouble();

		return( System.Assign(Cellsize, Extent) );
	}

	//-----------------------------------------------------
	if( !pTarget->Get_Projection().is_Okay() )
	{
		return( false );
	}

	CSG_Shapes Points(SHAPE_TYPE_Point);

	Points.Get_Projection() = pTarget->Get_Projection();

	CSG_Rect Extent = pTarget->Get_Extent(true);

	Points.Add_Shape()->Add_Point(Extent.Get_XMin   (), Extent.Get_YMin   ());
	Points.Add_Shape()->Add_Point(Extent.Get_XMin   (), Extent.Get_YCenter());
	Points.Add_Shape()->Add_Point(Extent.Get_XMin   (), Extent.Get_YMax   ());
	Points.Add_Shape()->Add_Point(Extent.Get_XCenter(), Extent.Get_YMax   ());
	Points.Add_Shape()->Add_Point(Extent.Get_XMax   (), Extent.Get_YMax   ());
	Points.Add_Shape()->Add_Point(Extent.Get_XMax   (), Extent.Get_YCenter());
	Points.Add_Shape()->Add_Point(Extent.Get_XMax   (), Extent.Get_YMin   ());
	Points.Add_Shape()->Add_Point(Extent.Get_XCenter(), Extent.Get_YMin   ());

	//-----------------------------------------------------
	CSG_Tool *pTool = SG_Get_Tool_Library_Manager().Create_Tool("pj_proj4", 2); // Coordinate Transformation (Shapes)

	if( pTool )
	{
		SG_UI_ProgressAndMsg_Lock(true);

		pTool->Set_Manager(NULL); pTool->Set_Callback(false);

		if( pTool->Set_Parameter("SOURCE"    , &Points)
		&&  pTool->Set_Parameter("CRS_STRING", Projection.Get_WKT2())
		&&  pTool->Set_Parameter("COPY"      , false)
		&&  pTool->Set_Parameter("PARALLEL"  , true)
		&&  pTool->Execute() )
		{
			SG_Get_Tool_Library_Manager().Delete_Tool(pTool);
			SG_UI_ProgressAndMsg_Lock(false);

			Extent = Points.Get_Extent();

			double Cellsize = Extent.Get_XRange() / pTarget->Get_NX() < Extent.Get_YRange() / pTarget->Get_NY()
							? Extent.Get_XRange() / pTarget->Get_NX() : Extent.Get_YRange() / pTarget->Get_NY();

			System.Create(Cellsize, Extent);

			return( true );
		}

		SG_Get_Tool_Library_Manager().Delete_Tool(pTool);
		SG_UI_ProgressAndMsg_Lock(false);
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String CGDAL_Import_WMS::Get_WMS_Request(const CSG_Projection &Projection)
{
	CSG_String Server;

	switch( Parameters("SERVER")->asInt() )
	{
	case  0: Server = "tile.openstreetmap.org/${z}/${x}/${y}.png"                                                     ;	break; // Open Street Map
	case  1: Server = "mt.google.com/vt/lyrs=m&x=${x}&y=${y}&z=${z}"                                                  ;	break; // Google Map
	case  2: Server = "mt.google.com/vt/lyrs=s&x=${x}&y=${y}&z=${z}"                                                  ;	break; // Google Satellite
	case  3: Server = "mt.google.com/vt/lyrs=y&x=${x}&y=${y}&z=${z}"                                                  ;	break; // Google Hybrid
	case  4: Server = "mt.google.com/vt/lyrs=t&x=${x}&y=${y}&z=${z}"                                                  ;	break; // Google Terrain
	case  5: Server = "mt.google.com/vt/lyrs=p&x=${x}&y=${y}&z=${z}"                                                  ;	break; // Google Terrain, Streets and Water
	case  6: Server = "services.arcgisonline.com/ArcGIS/rest/services/World_Street_Map/MapServer/tile/${z}/${y}/${x}" ;	break; // ArcGIS MapServer Tiles
	case  7: Server = "sgx.geodatenzentrum.de/wmts_topplus_open/tile/1.0.0/web/default/WEBMERCATOR/${z}/${y}/${x}.png";	break; // TopPlusOpen
	case  8: Server = "tiles.emodnet-bathymetry.eu/2020/baselayer/web_mercator/${z}/${x}/${y}.png"                    ; break; // EMODnet
	default: Server = Parameters("SERVER_USER")->asString();                                                          ;	break; // user defined
//	case  x: Server = "s3.amazonaws.com/com.modestmaps.bluemarble/${z}-r${y}-c${x}.jpg"                               ;	break; // Blue Marble
	}

	//-----------------------------------------------------
	CSG_Rect r;

	if( Projection.is_Geographic() )
	{
		r.xMin = -180.; r.xMax = 180.;
		r.yMin =  -90.; r.yMax =  90.;
	}
	else if( Projection.Get_Code() == 3857 ) // Web Mercator ?
	{
		r.xMin = -20037508.34; r.xMax = 20037508.34;
		r.yMin = -20037508.34; r.yMax = 20037508.34;
	}
	else
	{
		SG_Get_Projected(CSG_Projection::Get_GCS_WGS84(), Projection, r);
	}

	//-----------------------------------------------------
	CSG_MetaData XML, *pEntry;

	XML.Set_Name("GDAL_WMS");

	//-----------------------------------------------------
	pEntry = XML.Add_Child("Service");	pEntry->Add_Property("name", "TMS");

	pEntry->Add_Child("ServerUrl"  , "https://" + Server);

	//-----------------------------------------------------
	pEntry = XML.Add_Child("DataWindow");             // Define size and extents of the data. (required, except for TiledWMS and VirtualEarth)

	pEntry->Add_Child("UpperLeftX" ,         r.xMin); // X (longitude) coordinate of upper-left corner. (optional, defaults to -180.0, except for VirtualEarth)
	pEntry->Add_Child("UpperLeftY" ,         r.yMax); // Y (latitude) coordinate of upper-left corner. (optional, defaults to 90.0, except for VirtualEarth)
	pEntry->Add_Child("LowerRightX",         r.xMax); // X (longitude) coordinate of lower-right corner. (optional, defaults to 180.0, except for VirtualEarth)
	pEntry->Add_Child("LowerRightY",         r.yMin); // Y (latitude) coordinate of lower-right corner. (optional, defaults to -90.0, except for VirtualEarth)

	pEntry->Add_Child("TileLevel"  ,             18); // Tile level at highest resolution. (tiled image sources only, optional, defaults to 0)
	pEntry->Add_Child("TileCountX" ,              1); // Can be used to define image size, SizeX = TileCountX * BlockSizeX * 2TileLevel. (tiled image sources only, optional, defaults to 0)
	pEntry->Add_Child("TileCountY" ,              1); // Can be used to define image size, SizeY = TileCountY * BlockSizeY * 2TileLevel. (tiled image sources only, optional, defaults to 0)
	pEntry->Add_Child("YOrigin"    ,          "top"); // Can be used to define the position of the Y origin with respect to the tile grid. Possible values are 'top', 'bottom', and 'default', where the default behavior is mini-driver-specific. (TMS mini-driver only, optional, defaults to 'bottom' for TMS)

	//-----------------------------------------------------
	CSG_String CRS(CSG_String::Format("EPSG:%d", Projection.Get_Code()));

	pEntry = XML.Add_Child("Projection",       CRS);  // Image projection (optional, defaults to value reported by mini-driver or EPSG:4326)

	//-----------------------------------------------------
	pEntry = XML.Add_Child("BandsCount",         3);  // Number of bands/channels, 1 for grayscale data, 3 for RGB, 4 for RGBA. (optional, defaults to 3)

	int Blocksize = Parameters("BLOCKSIZE")->asInt();
	pEntry = XML.Add_Child("BlockSizeX", Blocksize);  // Block size in pixels. (optional, defaults to 1024, except for VirtualEarth)
	pEntry = XML.Add_Child("BlockSizeY", Blocksize);

	//-----------------------------------------------------
	if( Parameters("CACHE")->asBool() )
	{
		pEntry = XML.Add_Child("Cache");

		CSG_String Path(Parameters("CACHE_DIR")->asString());

		if( !SG_Dir_Exists(Path) )
		{
			Path = SG_Dir_Get_Temp();
		}

		pEntry->Add_Child("Path", SG_File_Make_Path(Path, "gdalwmscache"));
	}

	//-----------------------------------------------------
	return( XML.asText(2) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGDAL_Import_WMS::Get_WMS_Bands(CSG_Grid *pBands[3], const CSG_Grid_System &System, const CSG_Projection &Projection)
{
	CSG_GDAL_DataSet DataSet;

	if( DataSet.Open_Read(Get_WMS_Request(Projection), System) == false || DataSet.Get_Count() != 3 )
	{
		return( false );
	}

	Message_Add("\n", false);
	Message_Fmt("\n%s: %s", _TL("Driver" ), DataSet.Get_DriverID().c_str());
	Message_Fmt("\n%s: %d", _TL("Bands"  ), DataSet.Get_Count()           );
	Message_Fmt("\n%s: %d", _TL("Rows"   ), DataSet.Get_NX()              );
	Message_Fmt("\n%s: %d", _TL("Columns"), DataSet.Get_NY()              );
	Message_Add("\n", false);

	//-----------------------------------------------------
	SG_UI_Progress_Lock(true);

	pBands[0] = DataSet.Read(0);
	pBands[1] = DataSet.Read(1);
	pBands[2] = DataSet.Read(2);

	SG_UI_Progress_Lock(false);

	//-----------------------------------------------------
	if( !pBands[0] || !pBands[1] || !pBands[2] )
	{
		if( pBands[0] ) delete(pBands[0]);
		if( pBands[1] ) delete(pBands[1]);
		if( pBands[2] ) delete(pBands[2]);

		return( false );
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGDAL_Import_WMS::Get_Projected(CSG_Grid *pBands[3], CSG_Grid *pTarget)
{
	CSG_Tool *pTool = SG_Get_Tool_Library_Manager().Create_Tool("pj_proj4", 3); // Coordinate Transformation (Grid List);

	if(	pTool )
	{
		SG_UI_ProgressAndMsg_Lock(true);

		pTool->Set_Manager(NULL); pTool->Set_Callback(false);

		if( pTool->Set_Parameter("CRS_STRING"       , pTarget->Get_Projection().Get_WKT())
		&&  pTool->Set_Parameter("SOURCE"           , pBands[0])
		&&  pTool->Set_Parameter("SOURCE"           , pBands[1])
		&&  pTool->Set_Parameter("SOURCE"           , pBands[2])
	//	&&  pTool->Set_Parameter("DATA_TYPE"        , 10) // "Preserve" => is already default!
		&&  pTool->Set_Parameter("RESAMPLING"       ,  3)
		&&  pTool->Set_Parameter("TARGET_DEFINITION",  1)
		&&  pTool->Set_Parameter("TARGET_SYSTEM"    , (void *)&pTarget->Get_System())
		&&  pTool->Execute() )
		{
			CSG_Parameter_Grid_List	*pGrids	= pTool->Get_Parameters()->Get_Parameter("GRIDS")->asGridList();

			delete(pBands[0]); pBands[0] = pGrids->Get_Grid(0);
			delete(pBands[1]); pBands[1] = pGrids->Get_Grid(1);
			delete(pBands[2]); pBands[2] = pGrids->Get_Grid(2);

			SG_Get_Tool_Library_Manager().Delete_Tool(pTool);

			SG_UI_ProgressAndMsg_Lock(false);

			return( true );
		}

		SG_Get_Tool_Library_Manager().Delete_Tool(pTool);

		SG_UI_ProgressAndMsg_Lock(false);
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGDAL_Import_WMS::Set_Image(CSG_Grid *pBands[3])
{
	CSG_Grid *pMap = Parameters("TARGET_MAP")->asGrid();

	if( !pMap )
	{
		pMap = SG_Create_Grid(pBands[0]->Get_System(), SG_DATATYPE_Int);
	}
	else if( pMap->Get_System() != pBands[0]->Get_System() )
	{
		pMap->Create(pBands[0]->Get_System(), SG_DATATYPE_Int);
	}

	pMap->Set_Name(Parameters("SERVER")->asString());

	pMap->Get_Projection() = pBands[0]->Get_Projection();

	//-----------------------------------------------------
	bool bGrayscale = Parameters("GRAYSCALE")->asBool();

	#pragma omp parallel for
	for(sLong i=0; i<pMap->Get_NCells(); i++)
	{
		if( bGrayscale )
		{
			double z = (pBands[0]->asInt(i) + pBands[1]->asInt(i) + pBands[2]->asInt(i)) / 3.;

			pMap->Set_Value(i, SG_GET_RGB(z, z, z));
		}
		else
		{
			pMap->Set_Value(i, SG_GET_RGB(pBands[0]->asInt(i), pBands[1]->asInt(i), pBands[2]->asInt(i)));
		}
	}

	delete(pBands[0]);
	delete(pBands[1]);
	delete(pBands[2]);

	if( Get_Manager() == &SG_Get_Data_Manager() && this == SG_Get_Tool_Library_Manager().Get_Tool("io_gdal", 9) )
	{
		DataObject_Add(pMap); DataObject_Set_Parameter(pMap, "COLORS_TYPE", 5); // Color Classification Type: RGB Coded Values
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
