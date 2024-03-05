
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
	Set_Name		(_TL("Import SPOT 1-5 Scene"));

	Set_Author		("O.Conrad (c) 2024");

	Set_Description	(_TW(
		"Imports a SPOT (Satellite Pour l'Observation de la Terre) scene. "
		"Currently this is only a simple import tool for SPOT level 1A data. "
	));

	Add_Reference("https://regards.cnes.fr/user/swh/modules/60",
		SG_T("Spot World Heritage at CNES")
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

	Parameters.Add_Double("PROJECTION", "OFFSET_X", CSG_String::Format("%s X", _TL("Offset")), _TL(""));
	Parameters.Add_Double("PROJECTION", "OFFSET_Y", CSG_String::Format("%s Y", _TL("Offset")), _TL(""));

	//-----------------------------------------------------
	//Parameters.Add_Choice("",
	//	"CALIBRATION", _TL("Radiometric Calibration"),
	//	_TL(""),
	//	CSG_String::Format("%s|%s|%s",
	//		_TL("digital numbers"),
	//		_TL("radiance"),
	//		_TL("reflectance")
	//	), 0
	//);

	//Parameters.Add_Choice("CALIBRATION",
	//	"DATA_TYPE"  , _TL("Output Data Type"),
	//	_TL(""),
	//	CSG_String::Format("%s|%s",
	//		_TL("integers with scaling"),
	//		_TL("floating point numbers")
	//	), 0
	//);
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
	CSG_String File;

	if( !Metadata.Get_Content("Data_Access.Data_File.DATA_FILE_PATH", File) )
	{
		File = "IMAGERY.TIF";
	}

	int Level = Metadata["Data_Processing.PROCESSING_LEVEL"].Cmp_Content("1A", true) ? 1 : 2;

	CSG_Grids Bands, *pBands;

	if( Level == 1 )
	{
		pBands = &Bands;
	}
	else
	{
		pBands = SG_Create_Grids();
	}

	if( !pBands->Load(SG_File_Make_Path(SG_File_Get_Path(Metafile), File)) )
	{
		delete(pBands);

		Error_Fmt("%s [%s]", _TL("failed to load imagery"), File.c_str());

		return( false );
	}

	//-----------------------------------------------------
	Parameters("BANDS")->asGridList()->Del_Items();

	if( Level == 1 )
	{
		if( !Georeference(Metadata, Bands) )
		{
			return( false );
		}

		pBands = Parameters("BANDS")->asGridList()->Get_Item(0)->asGrids();
	}
	else
	{
		Parameters("BANDS")->asGridList()->Add_Item(pBands);
	}

	//-----------------------------------------------------
	Metadata.Del_Child("Dataset_Frame");
	Metadata.Del_Child("Raster_CS");
	Metadata.Del_Child("Geoposition");
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

	return( true );

	//-----------------------------------------------------
	//int  Calibration = Parameters("CALIBRATION")->asInt ();

	//double SunHeight = -1;

	//if( Info_Scene("SUN_ELEVATION") )
	//{
	//	SunHeight = Info_Scene["SUN_ELEVATION"].Get_Content().asDouble();
	//}
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
	int nRows, xField, yField, EPSG; double Cellsize; CSG_Rect Extent;

	if( !Metadata["Raster_Dimensions"].Get_Content("NROWS", nRows) )
	{
		return( false );
	}

	if( Parameters("PROJECTION")->asInt() == 1 )
	{
		int  UTM_Zone  = Parameters("UTM_ZONE" )->asInt();
		bool UTM_South = Parameters("UTM_SOUTH")->asBool();
		Set_Reference_UTM(Frame, UTM_Zone, UTM_South);

		xField = 4; yField = 5; EPSG = (UTM_South ? 32700 : 32600) + UTM_Zone;

		if( Parameters("OFFSET_X")->asDouble() || Parameters("OFFSET_Y")->asDouble() )
		{
			for(sLong i=0; i<Frame.Get_Count(); i++)
			{
				Frame.Get_Shape(i)->Add_Value(xField, Parameters("OFFSET_X")->asDouble());
				Frame.Get_Shape(i)->Add_Value(yField, Parameters("OFFSET_Y")->asDouble());
			}
		}

		Cellsize = nRows == 6000 ? 10. : 20.;

		Extent.Create(
			Cellsize * floor(Frame.Get_Minimum(xField) / Cellsize), Cellsize * ceil(Frame.Get_Minimum(yField) / Cellsize),
			Cellsize * floor(Frame.Get_Maximum(xField) / Cellsize), Cellsize * ceil(Frame.Get_Maximum(yField) / Cellsize)
		);
	}
	else
	{
		xField = 2; yField = 3; EPSG = 4326;

		if( Parameters("OFFSET_X")->asDouble() || Parameters("OFFSET_Y")->asDouble() )
		{
			for(sLong i=0; i<Frame.Get_Count(); i++)
			{
				Frame.Get_Shape(i)->Add_Value(xField, Parameters("OFFSET_X")->asDouble());
				Frame.Get_Shape(i)->Add_Value(yField, Parameters("OFFSET_Y")->asDouble());
			}
		}

		Cellsize = SG_Get_Distance(
			Frame[0].asDouble(xField), Frame[0].asDouble(yField),
			Frame[1].asDouble(xField), Frame[1].asDouble(yField)
		) / nRows;

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
	&&  pTool->Set_Parameter("CRS_CODE"        , EPSG)
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
	int EPSG = (UTM_South ? 32700 : 32600) + UTM_Zone;

	CSG_Tool *pTool = SG_Get_Tool_Library_Manager().Create_Tool("pj_proj4", 31); // Coordinate Conversion (Table)

	if( pTool && pTool->Set_Manager(NULL)
	&&  pTool->Set_Parameter("TABLE"   , &Frame)
	&&  pTool->Set_Parameter("SOURCE_X", "FRAME_LON")
	&&  pTool->Set_Parameter("SOURCE_Y", "FRAME_LAT")
	&&  pTool->Set_Parameter("TARGET_X", "FRAME_X"  )
	&&  pTool->Set_Parameter("TARGET_Y", "FRAME_Y"  )
	&&  pTool->Set_Parameter("SOURCE_CRS.CRS_METHOD",    1) // EPSG CODE
	&&  pTool->Set_Parameter("SOURCE_CRS.CRS_EPSG"  , 4236) // GCS/WGS84
	&&  pTool->Set_Parameter("TARGET_CRS.CRS_METHOD",    1) // EPSG CODE
	&&  pTool->Set_Parameter("TARGET_CRS.CRS_EPSG"  , EPSG) // UTM/WGS84
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
