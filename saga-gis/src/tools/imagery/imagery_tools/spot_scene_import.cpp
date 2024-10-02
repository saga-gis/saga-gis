
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                     image_tools                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 spot_scene_import.cpp                 //
//                                                       //
//                 Olaf Conrad (C) 2024                  //
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
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "spot_scene_import.h"


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSPOT_Scene_Import::CSPOT_Scene_Import(void)
{
	Set_Name		(_TL("Import SPOT Scene"));

	Set_Author		("O.Conrad (c) 2024");

	Set_Description	(_TW(
		"Imports a SPOT (Satellite Pour l'Observation de la Terre) scene. "
		"Currently this is just a simple import support tool for SPOT level 1A data. "
	));

	Add_Reference("https://regards.cnes.fr/user/swh/modules/60",
		SG_T("Spot World Heritage at CNES")
	);

	Add_Reference("https://earth.esa.int/eogateway/catalog/spot1-5-esa-archive",
		SG_T("SPOT 1-5 ESA archive")
	);

	//-----------------------------------------------------
	Parameters.Add_FilePath("",
		"METAFILE"   , _TL("Metadata File"),
		_TL(""),
		CSG_String::Format("%s|*.dim|%s|*.*",
			_TL("SPOT DIMAP Files"),
			_TL("All Files")
		)
	);

	Parameters.Add_Grid_List("",
		"BANDS"      , _TL("Spectral Bands"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	//-----------------------------------------------------
	Parameters.Add_Choice("",
		"PROJECTION" , _TL("Coordinate System"),
		_TL(""),
		CSG_String::Format("%s|%s",
			_TL("Geographic Coordinates"),
			_TL("Universal Transverse Mercator")
		), 1
	);

	Parameters.Add_Choice("PROJECTION",
		"RESAMPLING" , _TL("Resampling"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s",
			_TL("Nearest Neighbour"),
			_TL("Bilinear Interpolation"),
			_TL("Bicubic Spline Interpolation"),
			_TL("B-Spline Interpolation")
		), 3
	);

	Parameters.Add_Int("PROJECTION",
		"UTM_ZONE"   , _TL("Zone"),
		_TL(""),
		32, 1, true, 60, true
	);

	Parameters.Add_Bool("PROJECTION",
		"UTM_SOUTH"  , _TL("South"),
		_TL(""),
		false
	);

	Parameters.Add_Node("PROJECTION", "SHIFT", _TL("Adjustment"), _TL(""));
	Parameters.Add_Double("SHIFT", "SHIFT_X", CSG_String::Format("%s.x", _TL("Shift")), _TL(""));
	Parameters.Add_Double("SHIFT", "SHIFT_Y", CSG_String::Format("%s.y", _TL("Shift")), _TL(""));
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CSPOT_Scene_Import::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("METAFILE") )
	{
		CSG_MetaData Metadata; CSG_Shapes Frame;

		if( Load_Metadata(Metadata, pParameter->asString(), false)
		&&  Metadata["Data_Processing.PROCESSING_LEVEL"].Cmp_Content("1A", true)
		&&  Get_Reference_Frame(Metadata, Frame) )
		{
			pParameters->Set_Enabled("PROJECTION", true);

			pParameters->Set_Parameter("UTM_ZONE" , 1 + (int)((180. + Frame.Get_Mean(2)) / 6.));
			pParameters->Set_Parameter("UTM_SOUTH", Frame.Get_Mean(3) < 0.);
		}
		else
		{
			pParameters->Set_Enabled("PROJECTION", false);
		}
	}

	if( pParameter->Cmp_Identifier("PROJECTION") )
	{
		pParameters->Set_Enabled("UTM_ZONE" , pParameter->asInt() == 1);
		pParameters->Set_Enabled("UTM_SOUTH", pParameter->asInt() == 1);
	}

	if( pParameter->Cmp_Identifier("CALIBRATION") )
	{
		pParameters->Set_Enabled("DATA_TYPE", pParameter->asInt() != 0);
	}

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSPOT_Scene_Import::Load_Metadata(CSG_MetaData &Metadata, const CSG_String &File, bool bVerbose)
{
	if( !Metadata.Load(File) || !Metadata.Cmp_Name("Dimap_Document") )
	{
		return( false );
	}

	#define CHECK_TAG_EXISTS(tag) if( !Metadata(tag) ) { if( bVerbose ) { Error_Fmt("%s: \'%s\'", _TL("missing tag"), tag); } return( false ); }

	CHECK_TAG_EXISTS("Data_Processing.PROCESSING_LEVEL");
	CHECK_TAG_EXISTS("Data_Access.Data_File.DATA_FILE_PATH");
	CHECK_TAG_EXISTS("Dataset_Frame");
	CHECK_TAG_EXISTS("Raster_Dimensions");
	CHECK_TAG_EXISTS("Raster_Dimensions.NCOLS" );
	CHECK_TAG_EXISTS("Raster_Dimensions.NROWS" );
	CHECK_TAG_EXISTS("Raster_Dimensions.NBANDS");

	return( true );
}

//---------------------------------------------------------
CSG_String CSPOT_Scene_Import::Get_File_Path(const CSG_MetaData &Metadata, const CSG_String &Root)
{
	CSG_String File;

	if( !Metadata("Data_Access.Data_File.DATA_FILE_PATH")
	||  !Metadata["Data_Access.Data_File.DATA_FILE_PATH"].Get_Property("href", File) || File.is_Empty() )
	{
		File = "IMAGERY.TIF";
	}

	if( !SG_File_Exists(SG_File_Make_Path(Root, File)) )
	{
		File.Make_Lower();

		if( !SG_File_Exists(SG_File_Make_Path(Root, File)) )
		{
			File.Make_Upper();
		}
	}

	return( SG_File_Make_Path(Root, File) );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSPOT_Scene_Import::On_Execute(void)
{
	CSG_MetaData Metadata; CSG_String Metafile(Parameters("METAFILE")->asString());

	if( !Load_Metadata(Metadata, Metafile) )
	{
		Error_Fmt("%s [%s]", _TL("failed to load metadata"), Metafile.c_str());

		return( false );
	}

	//-----------------------------------------------------
	CSG_String File = Get_File_Path(Metadata, SG_File_Get_Path(Metafile));

	if( !SG_File_Exists(File) )
	{
		Error_Fmt("%s [%s]", _TL("failed to locate imagery file"), File.c_str());

		return( false );
	}

	int Mission = 0; Metadata.Get_Content("Dataset_Sources.Source_Information.Scene_Source.MISSION_INDEX", Mission);

	int Level = Metadata["Data_Processing.PROCESSING_LEVEL"].Cmp_Content("1A", true) ? 1 : 2;

	//-----------------------------------------------------
	CSG_Grids Bands, *pBands; pBands = Level == 1 ? &Bands : SG_Create_Grids();

	if( !pBands->Load(File) )
	{
		if( pBands != &Bands )
		{
			delete(pBands);
		}

		Error_Fmt("%s [%s]", _TL("failed to load imagery file"), File.c_str());

		return( false );
	}

	//-----------------------------------------------------
	Parameters("BANDS")->asGridList()->Del_Items();

	if( Level != 1 )
	{
		Parameters("BANDS")->asGridList()->Add_Item(pBands);
	}
	else
	{
		if( !Georeference(Metadata, Bands) )
		{
			return( false );
		}

		pBands = Parameters("BANDS")->asGridList()->Get_Item(0)->asGrids();
	}

	//-----------------------------------------------------
	Metadata.Del_Child("Dataset_Frame");
	Metadata.Del_Child("Raster_CS");
	Metadata.Del_Child("Geoposition");
	Metadata.Del_Child("Image_Display");
	Metadata.Del_Child("Data_Strip.Ephemeris.Points");
	Metadata.Del_Child("Data_Strip.Models");
	Metadata.Del_Child("Data_Strip.Satellite_Attitudes");
	Metadata.Del_Child("Data_Strip.Sensor_Configuration");
	Metadata.Del_Child("Data_Strip.Sensor_Calibration.Calibration");
	Metadata.Del_Child("Data_Strip.Sensor_Calibration.Spectral_Sensitivities");

	pBands->Get_MetaData().Add_Child(Metadata)->Set_Name("SPOT");

	pBands->Fmt_Name("SPOT-%s %s",
		Metadata.Get_Content("Dataset_Sources.Source_Information.Scene_Source.MISSION_INDEX"),
		Metadata.Get_Content("Dataset_Sources.Source_Information.Scene_Source.IMAGING_DATE" )
	);

	//-----------------------------------------------------
	pBands->Add_Attribute("INDEX"   , SG_DATATYPE_Short , 0);
	pBands->Add_Attribute("NAME"    , SG_DATATYPE_String, 1);
	pBands->Add_Attribute("WAVE"    , SG_DATATYPE_Double, 2);
	pBands->Add_Attribute("WAVE_MIN", SG_DATATYPE_Double, 3);
	pBands->Add_Attribute("WAVE_MAX", SG_DATATYPE_Double, 4);

	for(int i=0; i<pBands->Get_NZ(); i++)
	{
		Set_Band_Info(pBands, i, Mission);
	}

	pBands->Set_Z_Attribute(2); pBands->Set_Z_Name_Field(1); pBands->Del_Attribute(5);

	//-----------------------------------------------------
	if( pBands->Get_NZ() > 1 )
	{
		DataObject_Add(pBands);

		DataObject_Set_Parameter(pBands, "BAND_R", 0); // green
		DataObject_Set_Parameter(pBands, "BAND_G", 1); // red
		DataObject_Set_Parameter(pBands, "BAND_B", 2); // nir
	}

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSPOT_Scene_Import::Set_Band_Info(CSG_Grids *pBands, int Band, int Mission)
{
	pBands->Set_Attribute(Band, 0, 1 + Band);

	if( !Mission )
	{
		pBands->Set_Attribute(Band, 1, CSG_String::Format("Band %i", 1 + Band));
		pBands->Set_Attribute(Band, 2, 1 + Band);
		pBands->Set_Attribute(Band, 3, 1 + Band);
		pBands->Set_Attribute(Band, 4, 1 + Band);

		return( false );
	}

	static double Waves[4][5][2] = // [Sensor][Index][Range]
	{
		{ { 0.50, 0.73 }, { 0.50, 0.59 }, { 0.61, 0.68 }, { 0.78, 0.89 }, { -1.0, -1.0 } }, // SPOT 1/2/3
		{ { 0.61, 0.68 }, { 0.50, 0.59 }, { 0.61, 0.68 }, { 0.78, 0.89 }, { 1.58, 1.75 } }, // SPOT 4
		{ { 0.48, 0.71 }, { 0.50, 0.59 }, { 0.61, 0.68 }, { 0.78, 0.89 }, { 1.58, 1.75 } }, // SPOT 5
		{ { 0.45, 0.74 }, { 0.45, 0.52 }, { 0.53, 0.59 }, { 0.62, 0.69 }, { 0.78, 0.89 } }  // SPOT 6/7
	};

	static CSG_String Names[6] = { "Panchromatic", "Blue", "Green", "Red", "NIR", "SWIR"};

	int Sensor = Mission < 4 ? 0 : Mission < 5 ? 1 : Mission < 6 ? 2 : 3;

	double *Wave = Waves[Sensor][pBands->Get_NZ() == 1 ? 0 : 1 + Band];

	pBands->Set_Attribute(Band, 1, Names[pBands->Get_NZ() == 1 ? 0 : Mission < 6 ? 2 + Band : 1 + Band]);
	pBands->Set_Attribute(Band, 2, (Wave[0] + Wave[1]) / 2.);
	pBands->Set_Attribute(Band, 3,  Wave[0]);
	pBands->Set_Attribute(Band, 4,  Wave[1]);

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSPOT_Scene_Import::Georeference(const CSG_MetaData &Metadata, CSG_Grids &Bands)
{
	CSG_Shapes Frame;

	if( !Get_Reference_Frame(Metadata, Frame) )
	{
		Error_Fmt("%s", _TL("failed to detect frame parameters"));

		return( false );
	}

	//-----------------------------------------------------
	int nRows, xField, yField; double Cellsize; CSG_Rect Extent; CSG_Projection Projection;

	if( !Metadata["Raster_Dimensions"].Get_Content("NROWS", nRows) )
	{
		return( false );
	}

	if( Parameters("PROJECTION")->asInt() == 1 )
	{
		int  UTM_Zone  = Parameters("UTM_ZONE" )->asInt();
		bool UTM_South = Parameters("UTM_SOUTH")->asBool();
		Set_Reference_UTM(Frame, UTM_Zone, UTM_South);

		xField = 4; yField = 5; Projection.Set_UTM_WGS84(UTM_Zone, UTM_South);

		if( Parameters("SHIFT_X")->asDouble() || Parameters("SHIFT_Y")->asDouble() )
		{
			for(sLong i=0; i<Frame.Get_Count(); i++)
			{
				Frame.Get_Shape(i)->Add_Value(xField, Parameters("SHIFT_X")->asDouble());
				Frame.Get_Shape(i)->Add_Value(yField, Parameters("SHIFT_Y")->asDouble());
			}
		}

		int Mission = 0; Metadata.Get_Content("Dataset_Sources.Source_Information.Scene_Source.MISSION_INDEX", Mission);
		int nBands  = 0; Metadata.Get_Content("Raster_Dimensions.NBANDS"                                     , nBands );

		if( Mission && nBands )
		{
			Cellsize = nBands == 1
				? (Mission < 5 ? 10. : Mission < 6 ?  5. : 1.5)  // panchromatic
				: (Mission < 5 ? 20. : Mission < 6 ? 10. : 6. ); // spectral
		}
		else
		{
			Cellsize = SG_Get_Distance(
				Frame[0].asDouble(xField), Frame[0].asDouble(yField), // lower left image pixel
				Frame[3].asDouble(xField), Frame[3].asDouble(yField)  // upper left image pixel
			) / (1. + nRows);

			Cellsize = floor(0.5 + Cellsize); // round to meter
		}

		Extent.Create(
			Cellsize * floor(Frame.Get_Minimum(xField) / Cellsize), Cellsize * ceil(Frame.Get_Minimum(yField) / Cellsize),
			Cellsize * floor(Frame.Get_Maximum(xField) / Cellsize), Cellsize * ceil(Frame.Get_Maximum(yField) / Cellsize)
		);
	}
	else
	{
		xField = 2; yField = 3; Projection.Set_GCS_WGS84();

		if( Parameters("SHIFT_X")->asDouble() || Parameters("SHIFT_Y")->asDouble() )
		{
			for(sLong i=0; i<Frame.Get_Count(); i++)
			{
				Frame.Get_Shape(i)->Add_Value(xField, Parameters("SHIFT_X")->asDouble());
				Frame.Get_Shape(i)->Add_Value(yField, Parameters("SHIFT_Y")->asDouble());
			}
		}

		Cellsize = SG_Get_Distance(
			Frame[0].asDouble(xField), Frame[0].asDouble(yField),
			Frame[3].asDouble(xField), Frame[3].asDouble(yField)
		) / (1. + nRows);

		Extent.Create(
			Frame.Get_Minimum(xField), Frame.Get_Minimum(yField),
			Frame.Get_Maximum(xField), Frame.Get_Maximum(yField)
		);
	}

	//-----------------------------------------------------
	CSG_Tool *pTool = SG_Get_Tool_Library_Manager().Create_Tool("pj_georeference", 10); // Rectify Grid List

	if( pTool && pTool->Set_Manager(NULL)
	&&  pTool->Set_Parameter("GRIDS"           , &Bands)
	&&  pTool->Set_Parameter("REF_SOURCE"      , &Frame)
	&&  pTool->Set_Parameter("XFIELD"          , xField)
	&&  pTool->Set_Parameter("YFIELD"          , yField)
	&&  pTool->Set_Parameter("METHOD"          , 0) // automatic
	&&  pTool->Set_Parameter("CRS_WKT"         , Projection.Get_WKT2())
	&&  pTool->Set_Parameter("CRS_PROJ"        , Projection.Get_PROJ())
	&&  pTool->Set_Parameter("TARGET_USER_SIZE", Cellsize)
	&&  pTool->Set_Parameter("TARGET_USER_XMIN", Extent.xMin)
	&&  pTool->Set_Parameter("TARGET_USER_XMAX", Extent.xMax)
	&&  pTool->Set_Parameter("TARGET_USER_YMIN", Extent.yMin)
	&&  pTool->Set_Parameter("TARGET_USER_YMAX", Extent.yMax)
	&&  pTool->Execute() )
	{
		CSG_Parameter_Grid_List *pBands = pTool->Get_Parameter("TARGET_GRIDS")->asGridList();

		for(int i=0; i<pBands->Get_Item_Count(); i++)
		{
			Parameters("BANDS")->asGridList()->Add_Item(pBands->Get_Item(i));
		}

		SG_Get_Tool_Library_Manager().Delete_Tool(pTool);

		return( true );
	}

	//-----------------------------------------------------
	SG_Get_Tool_Library_Manager().Delete_Tool(pTool);

	Error_Fmt("%s", _TL("failed to project imagery"));

	return( false );
}

//---------------------------------------------------------
bool CSPOT_Scene_Import::Get_Reference_Frame(const CSG_MetaData &Metadata, CSG_Shapes &Frame)
{
	int nRows;

	if( !Metadata["Raster_Dimensions"].Get_Content("NROWS", nRows) )
	{
		return( false );
	}

	Frame.Create(SHAPE_TYPE_Point);
	Frame.Add_Field("FRAME_COL", SG_DATATYPE_Double);
	Frame.Add_Field("FRAME_ROW", SG_DATATYPE_Double);
	Frame.Add_Field("FRAME_LON", SG_DATATYPE_Double);
	Frame.Add_Field("FRAME_LAT", SG_DATATYPE_Double);
	Frame.Add_Field("FRAME_X"  , SG_DATATYPE_Double);
	Frame.Add_Field("FRAME_Y"  , SG_DATATYPE_Double);

	for(int i=0; i<Metadata["Dataset_Frame"].Get_Children_Count(); i++)
	{
		CSG_MetaData &Entry = *Metadata["Dataset_Frame"].Get_Child(i); double v[4];

		if( Entry.Cmp_Name("Vertex")
		&&  Entry.Get_Content("FRAME_COL", v[0])
		&&  Entry.Get_Content("FRAME_ROW", v[1])
		&&  Entry.Get_Content("FRAME_LON", v[2])
		&&  Entry.Get_Content("FRAME_LAT", v[3]) )
		{
			CSG_Shape &Point = *Frame.Add_Shape(); v[1] = nRows - v[1];

			Point.Set_Point(v[0], v[1]);

			Point.Set_Value(0, v[0]);
			Point.Set_Value(1, v[1]);
			Point.Set_Value(2, v[2]);
			Point.Set_Value(3, v[3]);
		}
	}

	if( Frame.Get_Count() < 4 )
	{
		return( false );
	}

	return( true );
}

//---------------------------------------------------------
bool CSPOT_Scene_Import::Set_Reference_UTM(CSG_Shapes &Frame, int UTM_Zone, bool UTM_South)
{
	CSG_Projection GCS; GCS.Set_GCS_WGS84();
	CSG_Projection UTM; UTM.Set_UTM_WGS84(UTM_Zone, UTM_South);

	CSG_Tool *pTool = SG_Get_Tool_Library_Manager().Create_Tool("pj_proj4", 31); // Coordinate Conversion (Table)

	if( pTool && pTool->Set_Manager(NULL)
	&&  pTool->Set_Parameter("TABLE"   , &Frame)
	&&  pTool->Set_Parameter("SOURCE_X", "FRAME_LON")
	&&  pTool->Set_Parameter("SOURCE_Y", "FRAME_LAT")
	&&  pTool->Set_Parameter("TARGET_X", "FRAME_X"  )
	&&  pTool->Set_Parameter("TARGET_Y", "FRAME_Y"  )
	&&  pTool->Set_Parameter("SOURCE_CRS.CRS_METHOD", 0) // Definition String
	&&  pTool->Set_Parameter("SOURCE_CRS.CRS_STRING", GCS.Get_WKT())
	&&  pTool->Set_Parameter("TARGET_CRS.CRS_METHOD", 0) // Definition String
	&&  pTool->Set_Parameter("TARGET_CRS.CRS_STRING", UTM.Get_WKT())
	&&  pTool->Execute() )
	{
		Frame.Get_Projection().Set_UTM_WGS84(UTM_Zone, UTM_South);

		SG_Get_Tool_Library_Manager().Delete_Tool(pTool);

		return( true );
	}

	SG_Get_Tool_Library_Manager().Delete_Tool(pTool);

	return( false );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
