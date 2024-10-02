
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
//               landsat_scene_import.cpp                //
//                                                       //
//                 Olaf Conrad (C) 2017                  //
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
#include "landsat_scene_import.h"
#include "saga_api/api_core.h"
#include <bitset>
#include <vector>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CLandsat_Scene_Import::CLandsat_Scene_Import(void)
{
	Set_Name		(_TL("Import Landsat Scene"));

	Set_Author		("O.Conrad (c) 2017");

	Set_Description	(_TW(
		"Import Landsat scenes including metadata from Landsat metadata files. "
		"Band data have to be stored in the same folder as the chosen metadata file "
		"in uncompressed GeoTIFF format. "
	));

	Add_Reference("https://landsat.usgs.gov/level-1-landsat-data-products-metadata",
		SG_T("Level-1 Landsat Data Products Metadata")
	);

	Add_Reference("https://landsat.usgs.gov/landsat-8-l8-data-users-handbook",
		SG_T("Landsat 8 (L8) Data Users Handbook")
	);

	Add_Reference("https://landsat.gsfc.nasa.gov/landsat-7-science-data-users-handbook",
		SG_T("Landsat 7 Science Data Users Handbook")
	);

	//-----------------------------------------------------
	Parameters.Add_FilePath("",
		"METAFILE"		, _TL("Metadata File"),
		_TL(""),
		CSG_String::Format("%s|*.met;*.txt;*.xml;*.json|%s (*.met)|*.met|%s (*.txt)|*.txt|%s (*.xml)|*.xml|%s (*.json)|*.json|%s|*.*",
			_TL("Recognized Files"),
			_TL("Landsat Metadata Files"),
			_TL("Text Files"),
			_TL("Extended Markup Language Files"),
			_TL("JavaScript Object Notation Files"),
			_TL("All Files")
		)
	);

	Parameters.Add_Grid_List("", "BANDS_SPECTRAL" , _TL("Spectral Bands" ), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid_List("", "BANDS_THERMAL"  , _TL("Thermal Bands"  ), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid_List("", "BANDS_AUXILIARY", _TL("Auxiliary Bands"), _TL(""), PARAMETER_OUTPUT);

	Parameters.Add_Table("",
		"BAND_INFO"		, _TL("Band Info"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Bool("",
		"MULTI2GRIDS"	, _TL("Spectral Bands as Grid Collection"),
		_TL(""),
		true
	);

	Parameters.Add_Bool("",
		"SKIP_PAN"		, _TL("Skip Panchromatic Band"),
		_TL(""),
		true
	);

	Parameters.Add_Bool("",
		"SKIP_AEROSOL"	, _TL("Skip Aerosol Band"),
		_TL(""),
		true
	);

	Parameters.Add_Bool("",
		"SKIP_CIRRUS"	, _TL("Skip Cirrus Band"),
		_TL(""),
		true
	);

	Parameters.Add_Choice("",
		"CALIBRATION"	, _TL("Radiometric Calibration"),
		_TL(""),
		CSG_String::Format("%s|%s|%s",
			_TL("digital numbers"),
			_TL("radiance"),
			_TL("reflectance")
		), 0
	);

	Parameters.Add_Choice("CALIBRATION",
		"DATA_TYPE"		, _TL("Output Data Type"),
		_TL(""),
		CSG_String::Format("%s|%s",
			_TL("integers with scaling"),
			_TL("floating point numbers")
		), 0
	);

	Parameters.Add_Choice("CALIBRATION",
		"TEMP_UNIT"		, _TL("Temperature Unit"),
		_TL(""),
		CSG_String::Format("%s|%s",
			_TL("Kelvin"),
			_TL("Celsius")
		), 0
	);

	Parameters.Add_Choice("",
		"PROJECTION"	, _TL("Coordinate System"),
		_TL("If using the extent option in combination with \'Different UTM Zone\' or \'Geographic Coordinates\' extent is expected to be defined with UTM North coordinates."),
		CSG_String::Format("%s|%s|%s|%s",
			_TL("UTM North"), // original
			_TL("UTM South"),
			_TL("Different UTM Zone"),
			_TL("Geographic Coordinates")
		), 0
	);

	Parameters.Add_Choice("PROJECTION",
		"RESAMPLING"	, _TL("Resampling"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s",
			_TL("Nearest Neighbour"),
			_TL("Bilinear Interpolation"),
			_TL("Bicubic Spline Interpolation"),
			_TL("B-Spline Interpolation")
		), 3
	);

	Parameters.Add_Int("PROJECTION",
		"UTM_ZONE"		, _TL("Zone"),
		_TL(""),
		32, 1, true, 60, true
	);

	Parameters.Add_Bool("PROJECTION",
		"UTM_SOUTH"		, _TL("South"),
		_TL(""),
		false
	);

	//-----------------------------------------------------
	Parameters.Add_Choice("",
		"EXTENT"		, _TL("Extent"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s",
			_TL("original"),
			_TL("user defined"),
			_TL("grid system"),
			_TL("shapes extent")
		), 0
	);

	Parameters.Add_Double("EXTENT", "EXTENT_XMIN", _TL("Left"  ), _TL(""));
	Parameters.Add_Double("EXTENT", "EXTENT_XMAX", _TL("Right" ), _TL(""));
	Parameters.Add_Double("EXTENT", "EXTENT_YMIN", _TL("Bottom"), _TL(""));
	Parameters.Add_Double("EXTENT", "EXTENT_YMAX", _TL("Top"   ), _TL(""));

	Parameters.Add_Grid_System("EXTENT",
		"EXTENT_GRID"	, _TL("Grid System"),
		_TL("")
	);

	Parameters.Add_Shapes("EXTENT",
		"EXTENT_SHAPES"	, _TL("Shapes Extent"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Double("EXTENT",
		"EXTENT_BUFFER"	, _TL("Buffer"),
		_TL(""),
		0., 0., true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CLandsat_Scene_Import::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("METAFILE") )
	{
		CSG_MetaData Metadata, Info_Scene; CSG_Table Info_Bands; CSG_Strings File_Bands;

		if( Load_Metadata(Metadata, pParameter->asString()) && Get_Info(Metadata, File_Bands, Info_Bands, Info_Scene) )
		{
			int Sensor = Get_Info_Sensor(Metadata);

			pParameters->Set_Enabled("SKIP_PAN"    , Sensor >= SENSOR_ETM);
			pParameters->Set_Enabled("SKIP_AEROSOL", Sensor >= SENSOR_OLI);
			pParameters->Set_Enabled("SKIP_CIRRUS" , Sensor >= SENSOR_OLI);

			const CSG_Table_Record &Info_Band = Info_Bands[0];

			bool bRadiance =  (Info_Band.asString("RADIANCE_ADD") && Info_Band.asString("RADIANCE_MUL"))
			               || (Info_Band.asString("L_MIN") && Info_Band.asString("QCAL_MIN")
			               &&  Info_Band.asString("L_MAX") && Info_Band.asString("QCAL_MAX"));

			bool bReflectance = Info_Band.asString("REFLECTANCE_ADD") && Info_Band.asString("REFLECTANCE_MUL");

			pParameters->Set_Enabled("CALIBRATION", bRadiance || bReflectance);

			if( bRadiance || bReflectance )
			{
				CSG_String Choices(_TL("digital number"));

				if( bRadiance    ) Choices += CSG_String("|") + _TL("radiance"   );
				if( bReflectance ) Choices += CSG_String("|") + _TL("reflectance");

				(*pParameters)("CALIBRATION")->asChoice()->Set_Items(Choices);

				if( bReflectance ) (*pParameters)("CALIBRATION")->Set_Value((*pParameters)("CALIBRATION")->asChoice()->Get_Count() - 1);
				On_Parameters_Enable( pParameters, (*pParameters)("CALIBRATION") );
			}
		}
		else
		{
			pParameters->Set_Enabled("SKIP_PAN"    , false);
			pParameters->Set_Enabled("SKIP_AEROSOL", false);
			pParameters->Set_Enabled("SKIP_CIRRUS" , false);
			pParameters->Set_Enabled("CALIBRATION" , false);
		}
	}

	if( pParameter->Cmp_Identifier("CALIBRATION") )
	{
		pParameters->Set_Enabled("DATA_TYPE"    , pParameter->asInt() != 0);
		pParameters->Set_Enabled("TEMP_UNIT"    , pParameter->asInt() == 2);
	}

	if( pParameter->Cmp_Identifier("PROJECTION") )
	{
		pParameters->Set_Enabled("RESAMPLING"   , pParameter->asInt() == 2 || pParameter->asInt() == 3);
		pParameters->Set_Enabled("UTM_ZONE"     , pParameter->asInt() == 2);
		pParameters->Set_Enabled("UTM_SOUTH"    , pParameter->asInt() == 2);
	}

	if(	pParameter->Cmp_Identifier("EXTENT") )
	{
		pParameters->Set_Enabled("EXTENT_XMIN"  , pParameter->asInt() == 1);
		pParameters->Set_Enabled("EXTENT_XMAX"  , pParameter->asInt() == 1);
		pParameters->Set_Enabled("EXTENT_YMIN"  , pParameter->asInt() == 1);
		pParameters->Set_Enabled("EXTENT_YMAX"  , pParameter->asInt() == 1);
		pParameters->Set_Enabled("EXTENT_GRID"  , pParameter->asInt() == 2);
		pParameters->Set_Enabled("EXTENT_SHAPES", pParameter->asInt() == 3);
		pParameters->Set_Enabled("EXTENT_BUFFER", pParameter->asInt() >= 2);
	}

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CLandsat_Scene_Import::On_Execute(void)
{
	CSG_MetaData Metadata;

	if( !Load_Metadata(Metadata, Parameters("METAFILE")->asString()) )
	{
		Error_Fmt("%s [%s]", _TL("failed to load Landsat metadata"), Parameters("METAFILE")->asString());

		return( false );
	}

	//-----------------------------------------------------
	CSG_MetaData Info_Scene; CSG_Table Info_Bands; CSG_Strings File_Bands;

	if( !Get_Info(Metadata, File_Bands, Info_Bands, Info_Scene) )
	{
		return( false );
	}

	int Sensor = Get_Info_Sensor(Metadata);

	//-----------------------------------------------------
	if( Parameters("BAND_INFO")->asTable() )
	{
		Parameters("BAND_INFO")->asTable()->Create(Info_Bands);
	}

	Message_Add("\n___\n", false);

	for(int j=0; j<Info_Scene.Get_Children_Count(); j++)
	{
		Message_Add(Info_Scene[j].Get_Name() + ": " + Info_Scene[j].Get_Content() + "\n", false);
	}

	//-----------------------------------------------------
	CSG_String  Path = SG_File_Get_Path(Parameters("METAFILE")->asString());

	int  Calibration = Parameters("CALIBRATION")->asInt ();

	double SunHeight = -1;

	if( Info_Scene("SUN_ELEVATION") )
	{
		SunHeight = Info_Scene["SUN_ELEVATION"].Get_Content().asDouble();
	}

	//-----------------------------------------------------
	Parameters("BANDS_SPECTRAL" )->asGridList()->Del_Items();
	Parameters("BANDS_THERMAL"  )->asGridList()->Del_Items();
	Parameters("BANDS_AUXILIARY")->asGridList()->Del_Items();

	CSG_Grids *pBands = NULL;

	for(int i=0; i<File_Bands.Get_Count() && Process_Get_Okay(); i++)
	{
		if( (is_Panchromatic(Sensor, i) && Parameters("SKIP_PAN"    )->asBool())
		||  (is_Aerosol     (Sensor, i) && Parameters("SKIP_AEROSOL")->asBool())
		||  (is_Cirrus      (Sensor, i) && Parameters("SKIP_CIRRUS" )->asBool()) )
		{
			continue;
		}

		Process_Set_Text("%s: %s", _TL("loading"), File_Bands[i].c_str());

		CSG_Grid *pBand = Load_Band(SG_File_Make_Path(Path, File_Bands[i], ""));

		if( pBand )
		{
			switch( Calibration )
			{
			case  1:
				{
					Get_Radiance   (pBand, Info_Bands[i]);
				}
				break;

			case  2: if( is_Thermal(Sensor, i) )
				{
					Get_Temperature(pBand, Info_Bands[i]);
				}
				else
				{
					Get_Reflectance(pBand, Info_Bands[i], SunHeight);
				}
				break;
			}

			pBand->Get_MetaData().Add_Child(Info_Scene)->Set_Name("LANDSAT");
			pBand->Set_Description(Info_Scene.asText());

			if( is_Panchromatic(Sensor, i) )
			{
				DataObject_Add(pBand); DataObject_Set_Colors(pBand, 2, SG_COLORS_BLACK_WHITE);
			}

			if( is_Aerosol(Sensor, i) || is_Cirrus(Sensor, i) || is_Panchromatic(Sensor, i) )
			{
				Parameters("BANDS_AUXILIARY")->asGridList()->Add_Item(pBand);
			}
			else if( is_Thermal(Sensor, i) )
			{
				Parameters("BANDS_THERMAL"  )->asGridList()->Add_Item(pBand);
			}
			else if( !Parameters("MULTI2GRIDS")->asBool() ) // if( is_Spectral(Sensor, i) )
			{
				Parameters("BANDS_SPECTRAL" )->asGridList()->Add_Item(pBand);
			}
			else
			{
				if( pBands == NULL )
				{
					if( (pBands = SG_Create_Grids(pBand->Get_System(), Info_Bands)) == NULL )
					{
						Error_Set(_TL("memory allocation failed"));

						return( false );
					}

					pBands->Set_Name(SG_File_Get_Name(Parameters("METAFILE")->asString(), false));
					pBands->Get_MetaData().Add_Child(Info_Scene)->Set_Name("LANDSAT");
					pBands->Set_Description(Info_Scene.asText());
					pBands->Set_Z_Attribute (4);
					pBands->Set_Z_Name_Field(2);
				}

				pBands->Add_Grid(Info_Bands[i], pBand, true);
			}
		}
	}

	//-----------------------------------------------------
	if( pBands )
	{
		Parameters("BANDS_SPECTRAL")->asGridList()->Add_Item(pBands);
	}

	return( Parameters("BANDS_SPECTRAL")->asGridList()->Get_Grid_Count() > 0
	     || Parameters("BANDS_THERMAL" )->asGridList()->Get_Grid_Count() > 0 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CLandsat_Scene_Import::is_Spectral(int Sensor, int Band)
{
	switch( Sensor )
	{
	case SENSOR_MSS:
		return( true );

	case SENSOR_TM:
		return( Band != 5 );

	case SENSOR_ETM:
		return( Band != 5 && Band != 6 && Band != 8 );

	case SENSOR_OLI: case SENSOR_OLI_TIRS:
		return( Band != 7 && Band != 9  && Band != 10 );
	}

	return( false );
}

//---------------------------------------------------------
bool CLandsat_Scene_Import::is_Thermal(int Sensor, int Band)
{
	switch( Sensor )
	{
	case SENSOR_TM:
		return( Band == 5 );

	case SENSOR_ETM:
		return( Band == 5 || Band == 6 );

	case SENSOR_OLI_TIRS:
		return( Band == 9 || Band == 10 );
	}

	return( false );
}

//---------------------------------------------------------
bool CLandsat_Scene_Import::is_Panchromatic(int Sensor, int Band)
{
	switch( Sensor )
	{
	case SENSOR_ETM:
		return( Band == 8 );

	case SENSOR_OLI: case SENSOR_OLI_TIRS:
		return( Band == 7 );
	}

	return( false );
}

//---------------------------------------------------------
bool CLandsat_Scene_Import::is_Aerosol(int Sensor, int Band)
{
	switch( Sensor )
	{
	case SENSOR_OLI: case SENSOR_OLI_TIRS:
		return( Band == 0 );
	}

	return( false );
}

//---------------------------------------------------------
bool CLandsat_Scene_Import::is_Cirrus(int Sensor, int Band)
{
	switch( Sensor )
	{
	case SENSOR_OLI: case SENSOR_OLI_TIRS:
		return( Band == 8 );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CLandsat_Scene_Import::Load_Metadata(CSG_MetaData &Metadata, const CSG_String &File)
{
	if( SG_File_Cmp_Extension(File, "xml") )
	{
		CSG_MetaData m; if( !m.Load(File) ) { return( false ); }
		for(int i=0; i<m.Get_Children_Count(); i++)
			Metadata.Add_Children(m[i]);
		Metadata.Set_Name(m.Get_Name());
	}
	else if( SG_File_Cmp_Extension(File, "json") )
	{
		CSG_MetaData m; if( !m.Load_JSON(File) || !m(0) ) { return( false ); }
		for(int i=0; i<m[0].Get_Children_Count(); i++)
			Metadata.Add_Children(m[0][i]);
		Metadata.Set_Name(m[0].Get_Name());
	}
	else // *.met, *.txt
	{
		CSG_File Stream;

		if( !Stream.Open(File, SG_FILE_R, false) )
		{
			return( false );
		}

		CSG_String Line, Key, Value;

		if( !Stream.Read_Line(Line) || !Load_Metadata(Line, Key, Value) || Key.Cmp("GROUP") )
		{
			return( false );
		}

		Metadata.Set_Name(Value);

		while( Stream.Read_Line(Line) && Line.Cmp("END") )
		{
			if( Line.Cmp("END_GROUP") && Load_Metadata(Line, Key, Value) && Key.Cmp("GROUP") )
			{
				Metadata.Add_Child(Key, Value);
			}
		}
	}

	return( Metadata.Get_Children_Count() > 0 );
}

//---------------------------------------------------------
bool CLandsat_Scene_Import::Load_Metadata(const CSG_String &Line, CSG_String &Key, CSG_String &Value)
{
	Key = Line.BeforeFirst('='); Key.Trim(true); Key.Trim(false);

	if( !Key.is_Empty() )
	{
		Value = Line.AfterFirst('='); Value.Trim(true); Value.Trim(false);

		if( Value.Length() >= 2 && Value[0] == '\"' )
		{
			Value = Value.Mid(1, Value.Length() - 2);
		}

		return( !Value.is_Empty() );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
enum EMetadata_Version
{
	VERSION_MET	= 0,
	VERSION_MTL_1,
	VERSION_MTL_2,
	VERSION_UNKNOWN
};

//---------------------------------------------------------
int CLandsat_Scene_Import::Get_Info_Sensor(const CSG_MetaData &Metadata)
{
	if( Metadata("SENSOR_ID") )
	{
		CSG_String Sensor = Metadata["SENSOR_ID"].Get_Content();

		if( !Sensor.Cmp("MSS"     ) ) return( SENSOR_MSS      );
		if( !Sensor.Cmp("TM"      ) ) return( SENSOR_TM       );
		if( !Sensor.Cmp("ETM"     ) ) return( SENSOR_ETM      );
		if( !Sensor.Cmp("ETM+"    ) ) return( SENSOR_ETM      );
		if( !Sensor.Cmp("OLI"     ) ) return( SENSOR_OLI      );
		if( !Sensor.Cmp("OLI_TIRS") ) return( SENSOR_OLI_TIRS );
	}

	return( SENSOR_UNKNOWN );
}

//---------------------------------------------------------
int CLandsat_Scene_Import::Get_Info_Version(const CSG_MetaData &Metadata)
{
	if( !Metadata.Get_Name().Cmp("METADATA_FILE") )
	{
		return( VERSION_MET );
	}

	if( !Metadata.Get_Name().Cmp(     "L1_METADATA_FILE")
	||  !Metadata.Get_Name().Cmp("LANDSAT_METADATA_FILE") )
	{
		return(	Metadata("BAND1_FILE_NAME") != NULL	// FILE_NAME_BAND_1
			? VERSION_MTL_1 : VERSION_MTL_2
		);
	}

	return( VERSION_UNKNOWN );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
struct SBand_Keys
{
	TSG_Data_Type Type; const char *Name, *Format;
};

//---------------------------------------------------------
enum EBand_Head
{
	BAND_HEAD_ID = 0,
	BAND_HEAD_NR,
	BAND_HEAD_NAME,
	BAND_HEAD_WAVE_MIN,
	BAND_HEAD_WAVE_MID,
	BAND_HEAD_WAVE_MAX,
	BAND_HEAD_COUNT
};

//---------------------------------------------------------
struct SBand_Keys	Band_Head[BAND_HEAD_COUNT]	=
{
	{ SG_DATATYPE_Int   , "ID"             , ""                    },
	{ SG_DATATYPE_Int   , "BAND"           , ""                    },
	{ SG_DATATYPE_String, "NAME"           , ""                    },
	{ SG_DATATYPE_Double, "WAVE_MIN"       , ""                    },
	{ SG_DATATYPE_Double, "WAVE_MID"       , ""                    },
	{ SG_DATATYPE_Double, "WAVE_MAX"       , ""                    }
};

//---------------------------------------------------------
#define BAND_DATA_FILE	-1
#define BAND_DATA_COUNT	18

//---------------------------------------------------------
struct SBand_Keys	Band_Data[BAND_DATA_COUNT]	=
{
	// MET
	{ SG_DATATYPE_Double, "L_MIN"          , "LMIN_%s"             },
	{ SG_DATATYPE_Double, "L_MAX"          , "LMAX_%s"             },
	{ SG_DATATYPE_Int   , "QCAL_MIN"       , "QCALMIN_%s"          },
	{ SG_DATATYPE_Int   , "QCAL_MAX"       , "QCALMAX_%s"          },
	{ SG_DATATYPE_String, "GAIN"           , "%s_GAIN"             },	//  5
	{ SG_DATATYPE_String, "GAIN_CHANGE"    , "%s_GAIN_CHANGE"      },

	// MTL.txt
	{ SG_DATATYPE_Double, "RADIANCE_MIN"   , "RADIANCE_MINIMUM_%s" },
	{ SG_DATATYPE_Double, "RADIANCE_MAX"   , "RADIANCE_MAXIMUM_%s" },
	{ SG_DATATYPE_Int   , "QCAL_MIN"       , "QUANTIZE_CAL_MIN_%s" },
	{ SG_DATATYPE_Int   , "QCAL_MAX"       , "QUANTIZE_CAL_MAX_%s" },	// 10
	{ SG_DATATYPE_Double, "RADIANCE_ADD"   , "RADIANCE_ADD_%s"     },
	{ SG_DATATYPE_Double, "RADIANCE_MUL"   , "RADIANCE_MULT_%s"    },
	{ SG_DATATYPE_Double, "REFLECTANCE_ADD", "REFLECTANCE_ADD_%s"  },
	{ SG_DATATYPE_Double, "REFLECTANCE_MUL", "REFLECTANCE_MULT_%s" },
	{ SG_DATATYPE_Double, "THERMAL_K1"     , "K1_CONSTANT_%s"      },	// 15
	{ SG_DATATYPE_Double, "THERMAL_K2"     , "K2_CONSTANT_%s"      },
	{ SG_DATATYPE_String, "GAIN"           , "GAIN_%s"             },
	{ SG_DATATYPE_String, "GAIN_CHANGE"    , "GAIN_CHANGE_%s"      }
};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CLandsat_Scene_Import::Get_Info(const CSG_MetaData &Metadata, CSG_Strings &File_Bands, CSG_Table &Info_Bands, CSG_MetaData &Info_Scene)
{
	int Version = Get_Info_Version(Metadata);

	if( Version == VERSION_UNKNOWN )
	{
		Error_Set(_TL("unrecognized metadata version"));

		return( false );
	}

	int Sensor = Get_Info_Sensor(Metadata);

	if( Sensor == SENSOR_UNKNOWN )
	{
		Error_Set(_TL("unrecognized sensor or metadata version"));

		return( false );
	}

	//-----------------------------------------------------
	#define GET_INFO_SCENE(exit, key)	if( Metadata(key) ) { Info_Scene.Add_Child(Metadata[key]); } else if( exit ) {\
		Error_Fmt("%s: %s", _TL("missing metadata entry"), CSG_String(key).c_str()); return( false ); }

	GET_INFO_SCENE( true, "SPACECRAFT_ID"           );
	GET_INFO_SCENE( true, "SENSOR_ID"               );
	GET_INFO_SCENE(false, "WRS_PATH"                );
	GET_INFO_SCENE(false, "WRS_ROW"                 );
	GET_INFO_SCENE(false, "ACQUISITION_DATE"        );	// MET
	GET_INFO_SCENE(false, "DATE_ACQUIRED"           );	// MTL
	GET_INFO_SCENE(false, "SCENE_CENTER_TIME"       );	// MTL
	GET_INFO_SCENE(false, "SUN_AZIMUTH"             );
	GET_INFO_SCENE(false, "SUN_ELEVATION"           );
	GET_INFO_SCENE(false, "EARTH_SUN_DISTANCE"      );	// MTL
	GET_INFO_SCENE(false, "CLOUD_COVER"             );
	GET_INFO_SCENE(false, "CLOUD_COVER_LAND"        );	// MTL
	GET_INFO_SCENE(false, "IMAGE_QUALITY_OLI"       );	// MTL
	GET_INFO_SCENE(false, "IMAGE_QUALITY_TIRS"      );	// MTL
	GET_INFO_SCENE(false, "TIRS_SSM_POSITION_STATUS");	// MTL
	GET_INFO_SCENE(false, "ROLL_ANGLE"              );	// MTL

	//-----------------------------------------------------
	Info_Bands.Destroy();
	Info_Bands.Set_Name(_TL("Band Info"));

	for(int iField=0; iField<BAND_HEAD_COUNT; iField++)
	{
		Info_Bands.Add_Field(
			Band_Head[iField].Name,
			Band_Head[iField].Type
		);
	}

	bool bOkay[BAND_DATA_COUNT];

	for(int iField=0; iField<BAND_DATA_COUNT; iField++)
	{
		bOkay[iField] = false;

		Info_Bands.Add_Field(
			Band_Data[iField].Name,
			Band_Data[iField].Type
		);
	}

	//-----------------------------------------------------
	const int Sensor_nBands[SENSOR_UNKNOWN] =
	{
		4, 7, 9, 9, 11 // MSS, TM, ETM, OLI, OLI/TIRS
	};

	for(int Band=0; Band<Sensor_nBands[Sensor]; Band++)
	{
		CSG_String Value;

		if( !Get_Info_Band(Metadata, Version, Sensor, Band, BAND_DATA_FILE, Value) )
		{
			return( false );
		}

		File_Bands += Value;

		CSG_Table_Record &Info = *Info_Bands.Add_Record();

		Set_Info_Band(Sensor, Band, Info);	// set defaults

		for(int iField=0; iField<BAND_DATA_COUNT; iField++)
		{
			if( Get_Info_Band(Metadata, Version, Sensor, Band, iField, Value) )
			{
				bOkay[iField] = true;

				Info.Set_Value (BAND_HEAD_COUNT + iField, Value);
			}
			else
			{
				Info.Set_NoData(BAND_HEAD_COUNT + iField);
			}
		}
	}

	//-----------------------------------------------------
	for(int iField=BAND_DATA_COUNT-1; iField>=0; iField--)
	{
		if( !bOkay[iField] )
		{
			Info_Bands.Del_Field(BAND_HEAD_COUNT + iField);
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CLandsat_Scene_Import::Get_Info_Band(const CSG_MetaData &Metadata, int Version, int Sensor, int Band, int Key, CSG_String &Value)
{
	CSG_String Name;

	if( Sensor == SENSOR_ETM )
	{
		switch( Band )
		{
		case 0: Name = Version != VERSION_MTL_2 ? "BAND1"  : "BAND_1"       ; break;
		case 1: Name = Version != VERSION_MTL_2 ? "BAND2"  : "BAND_2"       ; break;
		case 2: Name = Version != VERSION_MTL_2 ? "BAND3"  : "BAND_3"       ; break;
		case 3: Name = Version != VERSION_MTL_2 ? "BAND4"  : "BAND_4"       ; break;
		case 4: Name = Version != VERSION_MTL_2 ? "BAND5"  : "BAND_5"       ; break;
		case 5: Name = Version != VERSION_MTL_2 ? "BAND61" : "BAND_6_VCID_1"; break;
		case 6: Name = Version != VERSION_MTL_2 ? "BAND62" : "BAND_6_VCID_2"; break;
		case 7: Name = Version != VERSION_MTL_2 ? "BAND7"  : "BAND_7"       ; break;
		case 8: Name = Version != VERSION_MTL_2 ? "BAND8"  : "BAND_8"       ; break;
		}
	}
	else if( Sensor == SENSOR_MSS && Version == VERSION_MTL_2 )
	{
		Name.Printf(Version != VERSION_MTL_2  ? "BAND%d" : "BAND_%d", 4 + Band);
	}
	else
	{
		Name.Printf(Version != VERSION_MTL_2  ? "BAND%d" : "BAND_%d", 1 + Band);
	}

	//-----------------------------------------------------
	CSG_String Tag;

	Tag.Printf(Key != BAND_DATA_FILE ? Band_Data[Key].Format : Version != VERSION_MTL_2 
		? "%s_FILE_NAME" : "FILE_NAME_%s", Name.c_str()
	);

	if( !Metadata(Tag) )
	{
		return( false );
	}

	Value = Metadata[Tag].Get_Content();

	return( true );
}

//---------------------------------------------------------
bool CLandsat_Scene_Import::Set_Info_Band(int Sensor, int Band, CSG_Table_Record &Info)
{
	#define SET_INFO_BAND(band, name, wmin, wmax) {\
		Info.Set_Value(BAND_HEAD_ID      , 1 + (int)Info.Get_Index());\
		Info.Set_Value(BAND_HEAD_NR      , band);\
		Info.Set_Value(BAND_HEAD_NAME    , name);\
		Info.Set_Value(BAND_HEAD_WAVE_MIN, wmin);\
		Info.Set_Value(BAND_HEAD_WAVE_MID, 0.5 * (wmin + wmax));\
		Info.Set_Value(BAND_HEAD_WAVE_MAX, wmax);\
		return( true );\
	}

	switch( Sensor )
	{
	case SENSOR_MSS:
		switch( Band )
		{
		case 0: SET_INFO_BAND(1, _TL("Green"), 0.50, 0.60);
		case 1: SET_INFO_BAND(2, _TL("Red"  ), 0.60, 0.70);
		case 2: SET_INFO_BAND(3, _TL("NIR 1"), 0.70, 0.80);
		case 3: SET_INFO_BAND(4, _TL("NIR 2"), 0.80, 1.10);
		}
		break;

	case SENSOR_TM:
		switch( Band )
		{
		case 0: SET_INFO_BAND(1, _TL("Blue"   ), 0.45, 0.52);
		case 1: SET_INFO_BAND(2, _TL("Green"  ), 0.52, 0.60);
		case 2: SET_INFO_BAND(3, _TL("Red"    ), 0.63, 0.69);
		case 3: SET_INFO_BAND(4, _TL("NIR"    ), 0.76, 0.90);
		case 4: SET_INFO_BAND(5, _TL("SWIR 1" ), 1.55, 1.75);
		case 5: SET_INFO_BAND(6, _TL("Thermal"), 10.4, 12.5);
		case 6: SET_INFO_BAND(7, _TL("SWIR 2" ), 2.08, 2.35);
		}
		break;

	case SENSOR_ETM:
		switch( Band )
		{
		case 0: SET_INFO_BAND(1, _TL("Blue"               ), 0.45, 0.52);
		case 1: SET_INFO_BAND(2, _TL("Green"              ), 0.53, 0.61);
		case 2: SET_INFO_BAND(3, _TL("Red"                ), 0.63, 0.69);
		case 3: SET_INFO_BAND(4, _TL("NIR"                ), 0.78, 0.90);
		case 4: SET_INFO_BAND(5, _TL("SWIR 1"             ), 1.55, 1.75);
		case 5: SET_INFO_BAND(6, _TL("Thermal (low gain)" ), 10.4, 12.5);
		case 6: SET_INFO_BAND(6, _TL("Thermal (high gain)"), 10.4, 12.5);
		case 7: SET_INFO_BAND(7, _TL("SWIR 2"             ), 2.09, 2.35);
		case 8: SET_INFO_BAND(8, _TL("Panchromatic"       ), 0.52, 0.90);
		}
		break;

	case SENSOR_OLI: case SENSOR_OLI_TIRS:
		switch( Band )
		{
		case  0: SET_INFO_BAND( 1, _TL("Coast & Aerosol"), 0.433, 0.453);
		case  1: SET_INFO_BAND( 2, _TL("Blue"           ), 0.450, 0.515);
		case  2: SET_INFO_BAND( 3, _TL("Green"          ), 0.525, 0.600);
		case  3: SET_INFO_BAND( 4, _TL("Red"            ), 0.630, 0.680);
		case  4: SET_INFO_BAND( 5, _TL("NIR"            ), 0.845, 0.885);
		case  5: SET_INFO_BAND( 6, _TL("SWIR 1"         ), 1.560, 1.660);
		case  6: SET_INFO_BAND( 7, _TL("SWIR 2"         ), 2.100, 2.300);
		case  7: SET_INFO_BAND( 8, _TL("Panchromatic"   ), 0.500, 0.680);
		case  8: SET_INFO_BAND( 9, _TL("Cirrus"         ), 1.360, 1.390);
		case  9: SET_INFO_BAND(10, _TL("Thermal 1"      ), 10.30, 11.30);
		case 10: SET_INFO_BAND(11, _TL("Thermal 2"      ), 11.50, 12.50);
		}
		break;
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Grid * CLandsat_Scene_Import::Load_Grid(const CSG_String &File)
{
	CSG_Rect Extent;

	switch( Parameters("EXTENT")->asInt() )
	{
	default: // original
		return( SG_Create_Grid(File) );

	case  1: // user defined
		Extent.Assign(
			Parameters("EXTENT_XMIN")->asDouble(),
			Parameters("EXTENT_YMIN")->asDouble(),
			Parameters("EXTENT_XMAX")->asDouble(),
			Parameters("EXTENT_YMAX")->asDouble()
		);
		break;

	case  2: // grid system
		Extent = Parameters("EXTENT_GRID"  )->asGrid_System()->Get_Extent();
		Extent.Inflate(Parameters("EXTENT_BUFFER")->asDouble(), false);
		break;

	case  3: // shapes extent
		Extent = Parameters("EXTENT_SHAPES")->asShapes     ()->Get_Extent();
		Extent.Inflate(Parameters("EXTENT_BUFFER")->asDouble(), false);
		break;
	}

	if( Parameters("PROJECTION")->asInt() == 1 ) // UTM South
	{
		Extent.Move(0, -10000000); // assume target extent is provided as UTM South coordinates
	}

	//-----------------------------------------------------
	CSG_Grid *pGrid = NULL; CSG_Tool *pTool = SG_Get_Tool_Library_Manager().Create_Tool("io_gdal", 0);	// Import Raster

	if( pTool && pTool->Set_Manager(NULL)
		&&  pTool->Set_Parameter("FILES"      , File)
		&&	pTool->Set_Parameter("EXTENT"     , 1)
		&&	pTool->Set_Parameter("EXTENT_XMIN", Extent.Get_XMin())
		&&	pTool->Set_Parameter("EXTENT_XMAX", Extent.Get_XMax())
		&&	pTool->Set_Parameter("EXTENT_YMIN", Extent.Get_YMin())
		&&	pTool->Set_Parameter("EXTENT_YMAX", Extent.Get_YMax())
		&&  pTool->Execute() )
	{
		pGrid = pTool->Get_Parameter("GRIDS")->asGridList()->Get_Grid(0);
	}

	SG_Get_Tool_Library_Manager().Delete_Tool(pTool);

	return( pGrid );
}

//---------------------------------------------------------
CSG_Grid * CLandsat_Scene_Import::Load_Band(const CSG_String &File)
{
	SG_UI_Msg_Lock(true);
	CSG_Grid *pBand = Load_Grid(File);
	SG_UI_Msg_Lock(false);

	if( !pBand )
	{
		CSG_String Message(CSG_String::Format("%s: \"%s\"", _TL("failed to load band"), File.c_str()));

		Message_Add("\n" + Message, false); SG_UI_Msg_Add_Error(Message);

		return( NULL );
	}

	pBand->Set_NoData_Value(0);	// landsat 8 pretends to use a value of 65535 (2^16 - 1)

	if( !pBand->Get_Projection().is_Okay() )
	{
		// undefined coordinate system, nothing to do be further done...
	}

	//-----------------------------------------------------
	else if( Parameters("PROJECTION")->asInt() == 1 ) // UTM South
	{
		CSG_Grid *pTmp = pBand; CSG_String Projection = pTmp->Get_Projection().Get_PROJ();

		if( Projection.Find("+proj=utm") >= 0 && Projection.Find("+zone") >= 0
		&&  (pBand = SG_Create_Grid(pTmp->Get_Type(), pTmp->Get_NX(), pTmp->Get_NY(), pTmp->Get_Cellsize(), pTmp->Get_XMin(), pTmp->Get_YMin() + 10000000)) != NULL )
		{
			CSG_String Zone = Projection.Right(Projection.Length() - Projection.Find("+zone")).AfterFirst('=');
			pBand->Get_Projection().Set_UTM_WGS84(Zone.asInt(),  true);
			pBand->Set_Name              (pTmp->Get_Name());
			pBand->Set_Description       (pTmp->Get_Description());
			pBand->Set_NoData_Value_Range(pTmp->Get_NoData_Value(), pTmp->Get_NoData_Value(true));
			pBand->Set_Scaling           (pTmp->Get_Scaling(), pTmp->Get_Offset());

			#pragma omp parallel for
			for(int y=0; y<pBand->Get_NY(); y++) for(int x=0; x<pBand->Get_NX(); x++)
			{
				pBand->Set_Value(x, y, pTmp->asDouble(x, y));
			}

			delete(pTmp);
		}
	}

	//-----------------------------------------------------
	else if( Parameters("PROJECTION")->asInt() == 2 )	// Different UTM Zone
	{
		CSG_Projection Projection = CSG_Projection::Get_UTM_WGS84(
			Parameters("UTM_ZONE" )->asInt (),
			Parameters("UTM_SOUTH")->asBool()
		);

		if( !Projection.is_Equal(pBand->Get_Projection()) )
		{
			CSG_Tool *pTool = SG_Get_Tool_Library_Manager().Create_Tool("pj_proj4", 4);	// Coordinate Transformation (Grid)

			if(	pTool )
			{
				Message_Fmt("\n%s (%s: %s >> %s)\n", _TL("re-projection to different UTM Zone"), _TL("original"), pBand->Get_Projection().Get_PROJ().c_str(), Projection.Get_PROJ().c_str());

				pTool->Set_Manager(NULL);

				if( pTool->Set_Parameter("CRS_WKT"         , Projection.Get_WKT2())
				&&  pTool->Set_Parameter("CRS_PROJ"        , Projection.Get_PROJ())
				&&  pTool->Set_Parameter("SOURCE"          , pBand)
				&&  pTool->Set_Parameter("RESAMPLING"      , Parameters("RESAMPLING"))
			//	&&  pTool->Set_Parameter("DATA_TYPE"       , 10) // "Preserve" => is already default!
				&&  pTool->Set_Parameter("TARGET_USER_SIZE", pBand->Get_Cellsize())
				&&  pTool->Execute() )
				{
					delete(pBand);

					pBand = pTool->Get_Parameters()->Get_Parameter("GRID")->asGrid();
				}

				SG_Get_Tool_Library_Manager().Delete_Tool(pTool);
			}
		}
	}

	//-----------------------------------------------------
	else if( Parameters("PROJECTION")->asInt() == 3 )	// Geographic Coordinates
	{
		CSG_Tool *pTool = SG_Get_Tool_Library_Manager().Create_Tool("pj_proj4", 4);	// Coordinate Transformation (Grid)

		if(	pTool )
		{
			Message_Fmt("\n%s (%s: %s)\n", _TL("re-projection to geographic coordinates"), _TL("original"), pBand->Get_Projection().Get_Name().c_str());

			pTool->Set_Manager(NULL);

			if( pTool->Set_Parameter("CRS_WKT"   , CSG_Projection::Get_GCS_WGS84().Get_WKT2())
			&&  pTool->Set_Parameter("CRS_PROJ"  , CSG_Projection::Get_GCS_WGS84().Get_PROJ())
			&&  pTool->Set_Parameter("SOURCE"    , pBand)
			&&  pTool->Set_Parameter("RESAMPLING", Parameters("RESAMPLING"))
		//	&&  pTool->Set_Parameter("DATA_TYPE" , 10) // "Preserve" => is already default!
			&&  pTool->Execute() )
			{
				delete(pBand);

				pBand = pTool->Get_Parameters()->Get_Parameter("GRID")->asGrid();
			}

			SG_Get_Tool_Library_Manager().Delete_Tool(pTool);
		}
	}

	//-----------------------------------------------------
	pBand->Set_File_Name("");

	return( pBand );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CLandsat_Scene_Import::Get_Float(CSG_Grid *pBand, CSG_Grid &DN)
{
	pBand->Create(DN.Get_System(), SG_DATATYPE_Float);
	pBand->Get_Projection().Create(DN.Get_Projection());
	pBand->Set_Name        (DN.Get_Name());
	pBand->Set_Description (DN.Get_Description());
	pBand->Set_NoData_Value(-1.);

	return( true );
}

//---------------------------------------------------------
bool CLandsat_Scene_Import::Get_Radiance(CSG_Grid *pBand, const CSG_Table_Record &Info_Band)
{
	//-----------------------------------------------------
	double Offset, Scale, DNmin;

	if( Info_Band.asString("RADIANCE_ADD") && Info_Band.asString("RADIANCE_MUL") )
	{
		DNmin  =  0.;
		Offset =  Info_Band.asDouble("RADIANCE_ADD");
		Scale  =  Info_Band.asDouble("RADIANCE_MUL");
	}
	else if( Info_Band.asString("L_MIN") && Info_Band.asString("L_MAX") && Info_Band.asString("QCAL_MIN") && Info_Band.asString("QCAL_MAX") )
	{
		DNmin  =  Info_Band.asDouble("QCAL_MIN");
		Offset =  Info_Band.asDouble("L_MIN");
		Scale  = (Info_Band.asDouble("L_MAX") - Offset) / (Info_Band.asDouble("QCAL_MAX") - DNmin);
	}
	else
	{
		SG_UI_Msg_Add_Error(CSG_String::Format("%s: %s", pBand->Get_Name(), _TL("failed to derive radiances")));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Grid DN(*pBand);

	if( Parameters("DATA_TYPE")->asInt() == 1 )
	{
		Get_Float(pBand, DN);
	}
	else
	{
		double MaxVal = (pBand->Get_Type() == SG_DATATYPE_Byte ? 256 : 256*256) - 1;
		pBand->Set_NoData_Value(MaxVal--);
		pBand->Set_Scaling(1000. / MaxVal, 0.);
	}

	pBand->Set_Unit("W/(m2*sr*um");

	//-----------------------------------------------------
	#pragma omp parallel for
	for(sLong i=0; i<pBand->Get_NCells(); i++)
	{
		if( DN.is_NoData(i) )
		{
			pBand->Set_NoData(i);
		}
		else
		{
			pBand->Set_Value(i, Offset + Scale * (DN.asDouble(i) - DNmin));
		}
	}

	//-----------------------------------------------------
	return( true );
}

//---------------------------------------------------------
bool CLandsat_Scene_Import::Get_Reflectance(CSG_Grid *pBand, const CSG_Table_Record &Info_Band, double SunHeight)
{
	//-----------------------------------------------------
	double Offset, Scale;

	if( Info_Band.asString("REFLECTANCE_ADD") && Info_Band.asString("REFLECTANCE_MUL") )
	{
		Offset = Info_Band.asDouble("REFLECTANCE_ADD");
		Scale  = Info_Band.asDouble("REFLECTANCE_MUL");
	}
	else
	{
		SG_UI_Msg_Add_Error(CSG_String::Format("%s: %s", pBand->Get_Name(), _TL("failed to derive reflectances")));

		return( false );
	}

	SunHeight = sin(SunHeight * M_DEG_TO_RAD);

	//-----------------------------------------------------
	CSG_Grid DN(*pBand);

	if( Parameters("DATA_TYPE")->asInt() == 1 )
	{
		Get_Float(pBand, DN);
	}
	else
	{
		double MaxVal = (pBand->Get_Type() == SG_DATATYPE_Byte ? 256 : 256*256) - 1;
		pBand->Set_NoData_Value(MaxVal--);
		pBand->Set_Scaling(1. / MaxVal, 0.);	// 0 to 1 (reflectance)
	}

	pBand->Set_Unit(_TL("Reflectance"));

	//-----------------------------------------------------
	#pragma omp parallel for
	for(sLong i=0; i<pBand->Get_NCells(); i++)
	{
		if( DN.is_NoData(i) )
		{
			pBand->Set_NoData(i);
		}
		else
		{
			double r = (Offset + Scale * DN.asDouble(i)) / SunHeight;

			pBand->Set_Value(i, r < 0. ? 0. : r > 1. ? 1. : r);
		}
	}

	//-----------------------------------------------------
	return( true );
}

//---------------------------------------------------------
bool CLandsat_Scene_Import::Get_Temperature(CSG_Grid *pBand, const CSG_Table_Record &Info_Band)
{
	if( !Info_Band.asString("RADIANCE_ADD") || !Info_Band.asString("RADIANCE_MUL") || !Info_Band.asString("THERMAL_K1") || !Info_Band.asString("THERMAL_K2") )
	{
		SG_UI_Msg_Add_Error(CSG_String::Format("%s: %s", pBand->Get_Name(), _TL("failed to derive temperatures")));

		return( false );
	}

	double Offset = Info_Band.asDouble("RADIANCE_ADD");
	double  Scale = Info_Band.asDouble("RADIANCE_MUL");

	double     k1 = Info_Band.asDouble("THERMAL_K1");
	double     k2 = Info_Band.asDouble("THERMAL_K2");

	//-----------------------------------------------------
	CSG_Grid DN(*pBand);

	int Unit = Parameters("TEMP_UNIT")->asInt();

	if( Parameters("DATA_TYPE")->asInt() == 1 )
	{
		Get_Float(pBand, DN);
	}
	else
	{
		double MaxVal = (pBand->Get_Type() == SG_DATATYPE_Byte ? 256 : 256*256) - 1;
		pBand->Set_NoData_Value(MaxVal--);
		pBand->Set_Scaling(100. / MaxVal, (Unit == 0 ? 273.15 : 0.) - 40.);	// -40�C to 60�C
	}

	pBand->Set_Unit(Unit == 0 ? "Kelvin" : "Celsius");

	//-----------------------------------------------------
	#pragma omp parallel for
	for(sLong i=0; i<pBand->Get_NCells(); i++)
	{
		if( DN.is_NoData(i) )
		{
			pBand->Set_NoData(i);
		}
		else
		{
			double r = Offset + Scale * DN.asDouble(i);

			pBand->Set_Value(i, k2 / log(1. + (k1 / r)) - (Unit == 0 ? 0. : 273.15));
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
std::vector<LUT_Keys> Generic_Bool =
{
	{ SG_COLOR_RED, 		"True"															,"",  1 },  		
};

//---------------------------------------------------------
std::vector<LUT_Keys> Generic_Confidece =
{
	{ SG_COLOR_YELLOW, 		"Low confidence"															,"",  1 },  		
	{ SG_COLOR_RED, 		"High confidence"															,"",  3 },  		
};

//---------------------------------------------------------
std::vector<LUT_Keys> Aerosol_Level =
{
	{ SG_COLOR_GREY, 		"Climatology"															,"",  0 },  		
	{ SG_COLOR_YELLOW, 		"Low"																	,"",  1 },  		
	{ SG_COLOR_YELLOW_DARK, "Medium"																,"",  2 },  		
	{ SG_COLOR_RED, 		"High"																	,"",  3 },  		
};

//---------------------------------------------------------
std::vector<LUT_Keys> TM_ETM_C2_QA_Pix_LUT =
{
	//{ SG_COLOR_BLACK, 		"Fill"																			,"",     1 },	
	//{ SG_COLOR_WHITE, 		"Clear with lows set"															,"",  5440 },  		
	{ SG_COLOR_YELLOW, 		"Dilated cloud over land"														,"",  5442 },  		
	{ SG_COLOR_BLUE, 		"Water with lows set"															,"",  5504 },  		
	{ SG_COLOR_YELLOW, 		"Dilated cloud over water"														,"",  5506 },  		
	{ SG_COLOR_YELLOW, 		"Mid conf cloud"																,"",  5696 },  	
	{ SG_COLOR_YELLOW, 		"Mid conf cloud over water"														,"",  5760 },  		
	{ SG_COLOR_YELLOW, 		"High conf Cloud"																,"",  5896 },  			
	{ SG_COLOR_GREEN, 		"High conf cloud shadow"														,"",  7440 },  		
	{ SG_COLOR_GREEN, 		"Water with cloud shadow"														,"",  7568 },  		
	{ SG_COLOR_GREEN, 		"Mid conf cloud with shadow"													,"",  7696 },  		
	{ SG_COLOR_GREEN, 		"Mid conf cloud with shadow over water"											,"",  7824 },  		
	{ SG_COLOR_GREEN, 		"High conf cloud with shadow"													,"",  7960 },  	
	{ SG_COLOR_GREEN, 		"High conf cloud with shadow over water"										,"",  8088 },  		
	{ SG_COLOR_BLUE_LIGHT, 	"High conf snow/ice"															,"", 13664 }  	
};

//---------------------------------------------------------
std::vector<LUT_Keys> OLI_TIRS_C2_QA_Pix_LUT 
{
	//{ SG_COLOR_BLACK, 			"Fill"																	, "",		   1 }, 	
	//{ SG_COLOR_BLUE_LIGHT, 	"Clear with lows set"               									    , "",      21824 },
	{ SG_COLOR_YELLOW, 		"Dilated cloud over land"               									 	, "", 21826 },
	{ SG_COLOR_BLUE, 		"Water with lows set"                   									 	, "", 21888 },
	{ SG_COLOR_YELLOW, 		"Dilated cloud over water"              									 	, "", 21890 },
	{ SG_COLOR_YELLOW, 		"Mid conf cloud"                        									 	, "", 22080 },
	{ SG_COLOR_YELLOW, 		"Mid conf cloud over water"             									 	, "", 22144 },
	{ SG_COLOR_YELLOW, 		"High conf Cloud"                       									 	, "", 22280 },
	{ SG_COLOR_GREEN, 		"High conf cloud shadow"                									 	, "", 23888 },
	{ SG_COLOR_GREEN, 		"Water with cloud shadow"               									 	, "", 23952 },
	{ SG_COLOR_GREEN, 		"Mid conf cloud with shadow"            									 	, "", 24088 },
	{ SG_COLOR_GREEN, 		"Mid conf cloud with shadow over water" 									 	, "", 24216 },
	{ SG_COLOR_GREEN, 		"High conf cloud with shadow"           									 	, "", 24344 },
	{ SG_COLOR_GREEN, 		"High conf cloud with shadow over water"									 	, "", 24472 },
	{ SG_COLOR_BLUE_LIGHT, 	"High conf snow/ice" 															, "", 30048 },
	{ SG_COLOR_YELLOW, 		"High conf Cirrus"                      									 	, "", 54596 },
	{ SG_COLOR_YELLOW, 		"Cirrus, mid cloud"                     									 	, "", 54852 },
	{ SG_COLOR_YELLOW, 		"Cirrus, high cloud"                    									 	, "", 55052 }
};


//---------------------------------------------------------
std::vector<LUT_Keys> OLI_TIRS_C2_QA_Aerosol_LUT 
{
	//{ SG_COLOR_WHITE, 		"Fill"																			, "",   1 }, 
	{ SG_COLOR_BLUE_LIGHT, 		"Valid aerosol retrieval"                                                		, "",   2 },
	{ SG_COLOR_BLUE, 			"Water"                                                                     	, "",   4 },
	{ SG_COLOR_BLUE_LIGHT, 		"Aerosol interpolated"                                                       	, "",  32 }, 
	{ SG_COLOR_BLUE_LIGHT, 		"Valid aerosol ret., low aerosol"                                            	, "",  66 }, 
	{ SG_COLOR_BLUE, 			"Water, low aerosol"                                                         	, "",  68 }, 
	{ SG_COLOR_BLUE_LIGHT, 		"Aerosol interpolated, low aerosol"                                          	, "",  96 }, 
	{ SG_COLOR_BLUE, 			"Water pixel used in interpolation, aerosol interpolated, low aerosol"       	, "", 100 },
	{ SG_COLOR_BLUE_LIGHT, 		"Valid aerosol retrieval, medium aerosol"                                    	, "", 130 },
	{ SG_COLOR_BLUE, 			"Water, medium aerosol"                                                      	, "", 132 },
	{ SG_COLOR_BLUE_LIGHT, 		"Aerosol interpolated, medium aerosol"                                       	, "", 160 },
	{ SG_COLOR_BLUE, 			"Water pixel used in interpolation, aerosol interpolated, medium aerosol"    	, "", 164 },
	{ SG_COLOR_BLUE_LIGHT, 		"High aerosol"                                                               	, "", 192 },
	{ SG_COLOR_BLUE_LIGHT, 		"Valid aerosol retrieval, high aerosol"                                      	, "", 194 },
	{ SG_COLOR_BLUE, 			"Water, high aerosol"                                                       	, "", 196 },
	{ SG_COLOR_BLUE_LIGHT, 		"Aerosol interpolated, high aerosol"                                         	, "", 224 },
	{ SG_COLOR_BLUE, 			"Water pixel used in interpolation, aerosol interpolated, high aerosol"      	, "", 228 }
};

//"Attribute Pixel Value"
//"Fill" 																							1
//"Valid Aerosol Retrieval (center pixel of 3x3 window)" 											2
//"Valid Aerosol Retrieval (center pixel of 3x3 window)" 											66
//"Valid Aerosol Retrieval (center pixel of 3x3 window)" 											130
//"Valid Aerosol Retrieval (center pixel of 3x3 window)" 											194
//"Water Pixel (or water pixel was used in the fill-the-window interpolation)" 						4
//"Water Pixel (or water pixel was used in the fill-the-window interpolation)" 						68
//"Water Pixel (or water pixel was used in the fill-the-window interpolation)" 						100
//"Water Pixel (or water pixel was used in the fill-the-window interpolation)" 						132
//"Water Pixel (or water pixel was used in the fill-the-window interpolation)" 						164
//"Water Pixel (or water pixel was used in the fill-the-window interpolation)" 						196
//"Water Pixel (or water pixel was used in the fill-the-window interpolation)" 						228
//"Non-center window pixel for which aerosol was interpolated from surrounding 3x3 center pixels" 	32
//"Non-center window pixel for which aerosol was interpolated from surrounding 3x3 center pixels" 	96
//"Non-center window pixel for which aerosol was interpolated from surrounding 3x3 center pixels" 	100
//"Non-center window pixel for which aerosol was interpolated from surrounding 3x3 center pixels" 	160
//"Non-center window pixel for which aerosol was interpolated from surrounding 3x3 center pixels" 	164
//"Non-center window pixel for which aerosol was interpolated from surrounding 3x3 center pixels" 	224
//"Non-center window pixel for which aerosol was interpolated from surrounding 3x3 center pixels" 	228
//"Low-level aerosol" 																				66
//"Low-level aerosol" 																				68
//"Low-level aerosol" 																				96
//"Low-level aerosol" 																				100
//"Medium-level aerosol" 																			130
//"Medium-level aerosol" 																			132
//"Medium-level aerosol" 																			160
//"Medium-level aerosol" 																			164
//"High-level aerosol" 																				192
//"High-level aerosol" 																				194
//"High-level aerosol" 																				196
//"High-level aerosol" 																				224
//"High-level aerosol" 																				228

//---------------------------------------------------------
std::vector<Flag_Info> MSS_C2_L1_QA_Pix =
{
	{NULL, SG_DATATYPE_Bit, 	"0", 0, 1, "Fill", 				"Fill", 			"Fill"},
	{NULL, SG_DATATYPE_Bit, 	"3", 3, 1, "Cloud", 			"Cloud", 			"Cloud"},
	{NULL, SG_DATATYPE_Byte,  "8-9", 8, 2, "Cloud_Confidence", 	"Cloud Confidence", "Cloud Confidence"},
};

//---------------------------------------------------------
std::vector<Flag_Info> MSS_C2_L1_QA_Sat =
{
	{NULL, SG_DATATYPE_Bit, "0", 0, 1, "B1_Sat", 		"Band 1", 			"Band 1 Data Saturation"},
	{NULL, SG_DATATYPE_Bit, "1", 1, 1, "B2_Sat", 		"Band 2", 			"Band 2 Data Saturation"},
	{NULL, SG_DATATYPE_Bit, "2", 2, 1, "B3_Sat", 		"Band 3", 			"Band 3 Data Saturation"},
	{NULL, SG_DATATYPE_Bit, "3", 3, 1, "B4_Sat", 		"Band 4", 			"Band 4 Data Saturation"},
	{NULL, SG_DATATYPE_Bit, "4", 4, 1, "B5_Sat", 		"Band 5", 			"Band 5 Data Saturation"},
	{NULL, SG_DATATYPE_Bit, "5", 5, 1, "B6_Sat", 		"Band 6", 			"Band 6 Data Saturation"},
	{NULL, SG_DATATYPE_Bit, "6", 6, 1, "B7_Sat", 		"Band 7", 			"Band 7 Data Saturation"},
	{NULL, SG_DATATYPE_Bit, "9", 9, 1, "Dropped", 	"Dropped Pixel", 	"Dropped Pixel"}
};

//---------------------------------------------------------
std::vector<Flag_Info> TM_ETM_C2_L1_QA_Sat =
{
	{NULL, SG_DATATYPE_Bit, "0", 0, 1, "B1_Sat", 		"Band 1", 			"Band 1 Data Saturation"},
	{NULL, SG_DATATYPE_Bit, "1", 1, 1, "B2_Sat", 		"Band 2", 			"Band 2 Data Saturation"},
	{NULL, SG_DATATYPE_Bit, "2", 2, 1, "B3_Sat", 		"Band 3", 			"Band 3 Data Saturation"},
	{NULL, SG_DATATYPE_Bit, "3", 3, 1, "B4_Sat", 		"Band 4", 			"Band 4 Data Saturation"},
	{NULL, SG_DATATYPE_Bit, "4", 4, 1, "B5_Sat", 		"Band 5", 			"Band 5 Data Saturation"},
	{NULL, SG_DATATYPE_Bit, "5", 5, 1, "B6L_Sat", 		"Band 6L", 			"Band 6L Data Saturation"},
	{NULL, SG_DATATYPE_Bit, "6", 6, 1, "B7_Sat", 		"Band 7", 			"Band 7 Data Saturation"},
	{NULL, SG_DATATYPE_Bit, "8", 8, 1, "B6H_Sat", 		"Band 6H", 			"Band 6H Data Saturation"},
	{NULL, SG_DATATYPE_Bit, "9", 9, 1, "Dropped", 	"Dropped Pixel", 	"Dropped Pixel"}
};

//---------------------------------------------------------
std::vector<Flag_Info> TM_ETM_C2_L1_QA_Pix =
{
	{NULL, SG_DATATYPE_Bit, 	"0", 	 0, 	1, "Fill", 				"Fill", 			"Fill"},
	{NULL, SG_DATATYPE_Bit, 	"1", 	 1, 	1, "Dilated_Cloud", 	"Dilated Fill", 	"Dilated Fill"},
	{NULL, SG_DATATYPE_Bit, 	"3", 	 3, 	1, "Cloud", 			"Cloud", 			"Cloud"},
	{NULL, SG_DATATYPE_Bit, 	"4", 	 4, 	1, "Cloud_Shadow", 		"Cloud Shadow", 	"Cloud Shadow"},
	{NULL, SG_DATATYPE_Bit, 	"5", 	 5, 	1, "Snow", 				"Snow", 			"Snow"},
	{NULL, SG_DATATYPE_Bit, 	"6", 	 6, 	1, "Clear", 			"Clear", 			"Clear"},
	{NULL, SG_DATATYPE_Bit, 	"7", 	 7, 	1, "Water", 			"Water", 			"Water"},
	{NULL, SG_DATATYPE_Byte, 	"8-9", 	 8, 	2, "Cloud_Confidence", 	"Cloud Confidence", "Cloud Confidence"},
	{NULL, SG_DATATYPE_Byte, 	"10-11",10, 	2, "Shadow_Confidence", "Shadow Confidence","Cloud Shadow Confidence"},
	{NULL, SG_DATATYPE_Byte, 	"12-13",12, 	2, "Snow_Confidence", 	"Snow Confidence",	"Snow/Ice Confidence"},
};

//---------------------------------------------------------
std::vector<Flag_Info> OLI_TIRS_C2_L1_QA_Sat =
{
	{NULL, SG_DATATYPE_Bit,  "0", 	0, 	1, "B1_Sat", 		"Band 1", 			"Band 1 Data Saturation"},
	{NULL, SG_DATATYPE_Bit,  "1", 	1, 	1, "B2_Sat", 		"Band 2", 			"Band 2 Data Saturation"},
	{NULL, SG_DATATYPE_Bit,  "2", 	2, 	1, "B3_Sat", 		"Band 3", 			"Band 3 Data Saturation"},
	{NULL, SG_DATATYPE_Bit,  "3", 	3, 	1, "B4_Sat", 		"Band 4", 			"Band 4 Data Saturation"},
	{NULL, SG_DATATYPE_Bit,  "4", 	4, 	1, "B5_Sat", 		"Band 5", 			"Band 5 Data Saturation"},
	{NULL, SG_DATATYPE_Bit,  "5", 	5, 	1, "B6_Sat", 		"Band 6", 			"Band 6 Data Saturation"},
	{NULL, SG_DATATYPE_Bit,  "6", 	6, 	1, "B7_Sat", 		"Band 7", 			"Band 7 Data Saturation"},
	{NULL, SG_DATATYPE_Bit,  "8", 	8, 	1, "B9_Sat", 		"Band 9", 			"Band 9 Data Saturation"},
	{NULL, SG_DATATYPE_Bit, "11", 	11,	1, "Terrain_occlusion", 	"Terrain occlusion","Terrain occlusion"}
};

//---------------------------------------------------------
std::vector<Flag_Info> OLI_TIRS_C2_L1_QA_Pix =
{
	{NULL, SG_DATATYPE_Bit, 	"0", 	 0, 	1, "Fill", 				"Fill", 			"Fill"},
	{NULL, SG_DATATYPE_Bit, 	"1", 	 1, 	1, "Dilated_Cloud", 	"Dilated Fill", 	"Dilated Fill"},
	{NULL, SG_DATATYPE_Bit, 	"2", 	 2, 	1, "Cirrus", 			"Cirrus", 			"Cirrus"},
	{NULL, SG_DATATYPE_Bit, 	"3", 	 3, 	1, "Cloud", 			"Cloud", 			"Cloud"},
	{NULL, SG_DATATYPE_Bit, 	"4", 	 4, 	1, "Cloud_Shadow", 		"Cloud Shadow", 	"Cloud Shadow"},
	{NULL, SG_DATATYPE_Bit, 	"5", 	 5, 	1, "Snow", 				"Snow", 			"Snow"},
	{NULL, SG_DATATYPE_Bit, 	"6", 	 6, 	1, "Clear", 			"Clear", 			"Clear"},
	{NULL, SG_DATATYPE_Bit, 	"7", 	 7, 	1, "Water", 			"Water", 			"Water"},
	{NULL, SG_DATATYPE_Byte, 	"8-9", 	 8, 	2, "Cloud_Confidence", 	"Cloud Confidence", "Cloud Confidence"},
	{NULL, SG_DATATYPE_Byte, 	"10-11",10, 	2, "Shadow_Confidence", "Shadow Confidence","Cloud Shadow Confidence"},
	{NULL, SG_DATATYPE_Byte, 	"12-13",12, 	2, "Snow_Confidence", 	"Snow Confidence",	"Snow/Ice Confidence"},
	{NULL, SG_DATATYPE_Byte, 	"14-15",14,		2, "Cirrus_Confidence", "Cirrus Confidence","Cirrus Confidence"}
};

//---------------------------------------------------------
std::vector<Flag_Info> OLI_TIRS_C2_L1_QA_Aerosol =
{
	{NULL, SG_DATATYPE_Bit, 	"0", 	 0, 	1, "Fill", 					"Fill", 					"Fill"},
	{NULL, SG_DATATYPE_Bit, 	"1", 	 1, 	1, "Valid_Aerosol", 		"Valid Aerosol", 			"Valid aerosol retrieval"},
	{NULL, SG_DATATYPE_Bit, 	"2", 	 2, 	1, "Water", 				"Water", 					"Water"},
	{NULL, SG_DATATYPE_Bit, 	"5", 	 5, 	1, "Interpolated_Aerosol", 	"Interpolated Aerosol", 	"Interpolated Aerosol"},
	{NULL, SG_DATATYPE_Byte, 	"6-7", 	 6, 	2, "Aerosol_Level", 		"Aerosol Level", 			"Aerosol Level"},
};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CLandsat_QA_Import::CLandsat_QA_Import(void)
{
	Set_Name		(_TL("Decode Landsat Quality Assessment Bands"));

	Set_Author		("J.Spitzmueller (c) 2024");

	Set_Version 	("0.9");

	Set_Description	(_TW(
		"This tool decodes Landsat Multispectral Scanner System (MSS), Thematic Mapper (TM), "
		"Enhanced Thematic Mapper Plus (ETM+), and Operational Land Imager/Thermal Infrared Sensor (OLI/TIRS) "
		"Quality Assessment (QA) bands. It splits these QA bands into individual bands and optionally aggregates "
		"them into a Grid Collection. It is also possible to select individual flags for output. \n\n"

		"Currently, the tool supports Pixel, Radiometric Saturation and Surface Reflectance Aerosol (only OLI/TIRS) Quality Assessment bands from Collection 2 (Level 1 and 2). "
		"It also provides value interpretation for certain sensors and QA bands, which can be optionally added to the input datasets "
		"for classified displaying in the GUI."
	));

	Add_Reference("https://www.usgs.gov/media/files/landsat-1-5-mss-collection-2-level-1-data-format-control-book",
		SG_T("Landsat 1-5 MSS Collection 2 Level 1 Data Format Control Book V3")
	);

	Add_Reference("https://www.usgs.gov/media/files/landsat-4-7-collection-2-level-2-science-product-guide",
		SG_T("Landsat 4-7 Collection 2 Level 2 Science Product Guide V4")
	);

	Add_Reference("https://www.usgs.gov/media/files/landsat-8-9-collection-2-level-2-science-product-guide",
		SG_T("Landsat 8-9 Collection 2 Level 2 Science Product Guide V6")
	);

	//-----------------------------------------------------

	Parameters.Add_Choice("",
		"SENSOR", _TL("Spacecraft (Sensor)"),
		_TL(""),
		CSG_String::Format("%s|%s|%s",
			_TL("Landsat 1-5 (MSS)"),
			_TL("Landsat 4-7 (TM & ETM+)"),
			_TL("Landsat 8-9 (OLI/TIRS)")
		), 2
	);

	//Parameters.Add_Choice("",
	//	"CL", _TL("Collection and Level"),
	//	_TL(""),
	//	CSG_String::Format("%s|%s",
	//		_TL("Collection 2 Level 1"),
	//		_TL("Collection 2 Level 2")
	//	), 0
	//);

	
	Parameters.Add_Grid("", "IN_QA_PIXEL",		_TL("Pixel QA Band"), 						_TL("\"QA_PIXEL\"-Suffix"), 	PARAMETER_INPUT_OPTIONAL, true, SG_DATATYPE_Word );
	Parameters.Add_Grid("", "IN_QA_RADSAT", 	_TL("Radiometric Saturation QA Band"), 		_TL("\"QA_RADSAT\"-Suffix"), 	PARAMETER_INPUT_OPTIONAL, true, SG_DATATYPE_Word );
	Parameters.Add_Grid("", "IN_SR_QA_AEROSOL", _TL("SR Aerosol QA Band"), 					_TL("\"SR_QA_AEROSOL\"-Suffix"),PARAMETER_INPUT_OPTIONAL, true, SG_DATATYPE_Byte );
	//
	Parameters.Add_Choices("IN_QA_PIXEL", 		"IN_QA_PIX_SELECTION", 						_TL("Flag Selection"), _TL(""), "" );
	Parameters.Add_Choices("IN_QA_RADSAT", 		"IN_QA_RADSAT_SELECTION", 					_TL("Flag Selection"), _TL(""), "" );
	Parameters.Add_Choices("IN_SR_QA_AEROSOL", 	"IN_SR_QA_AEROSOL_SELECTION", 				_TL("Flag Selection"), _TL(""), "" );
	
	Parameters.Add_Grid_List("", "OUTPUT",		_TL("Output"), 								_TL(""), 	PARAMETER_OUTPUT );

	Parameters.Add_Bool("", "SELECTION", 	_TL("Select individual Bands"), 	_TL(""), false );
	Parameters.Add_Bool("", "GRIDS", 		_TL("Output as Grid Collection"), 	_TL(""), false );
	Parameters.Add_Bool("", "SET_LUT", 		_TL("Classify Colors of Input"), 	_TL(""), true  )->do_UseInGUI();


}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CLandsat_QA_Import::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	pParameters->Set_Enabled("SET_LUT", 					(pParameters->Get_Parameter("SENSOR")->asInt() >  0 && pParameters->Get_Parameter("IN_QA_PIXEL")->asGrid())
														||	(pParameters->Get_Parameter("SENSOR")->asInt() == 2 && pParameters->Get_Parameter("IN_SR_QA_AEROSOL")->asGrid()) );
	
	pParameters->Set_Enabled("IN_SR_QA_AEROSOL", 			pParameters->Get_Parameter("SENSOR")->asInt() == 2);

	pParameters->Set_Enabled("IN_SR_QA_AEROSOL_SELECTION", 	pParameters->Get_Parameter("SELECTION")->asBool() && pParameters->Get_Parameter("SENSOR")->asInt() == 2 );
	pParameters->Set_Enabled("IN_QA_PIX_SELECTION", 		pParameters->Get_Parameter("SELECTION")->asBool() );		
	pParameters->Set_Enabled("IN_QA_RADSAT_SELECTION", 		pParameters->Get_Parameter("SELECTION")->asBool() );

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


//---------------------------------------------------------
int CLandsat_QA_Import::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->is_Input() || pParameter->Cmp_Identifier("SENSOR") )
	{
		// Use this Array to loop rather than if/else mess with repeated task
		CSG_Parameter* Input_Parameter[] = { pParameters->Get_Parameter("IN_QA_PIXEL"), pParameters->Get_Parameter("IN_QA_RADSAT"), pParameters->Get_Parameter("IN_SR_QA_AEROSOL") };
		for( auto pInput : Input_Parameter )
		{
			// Check for usage (has a grid) and ensure the first child is actually a CSG_Parameter_Choices
			CSG_Parameter_Choices *pChoices = NULL;
			if( pInput->asGrid() && pInput->Get_Child(0) && (pChoices = pInput->Get_Child(0)->asChoices()) )
			{
				pChoices->Del_Items();

				std::vector<Flag_Info> 	Flags = Get_Flags( pInput, pParameters->Get_Parameter("SENSOR")->asInt() );
				for( size_t i=0; i<Flags.size(); i++ )
				{
					Flag_Info Flag = Flags.at(i);
					// Bury the Index in the Item-Data. This will later be recovered 
					pChoices->Add_Item( Flag.Desc, CSG_String::Format("%d", i) );
				}
			}
		}
	}

	//-----------------------------------------------------
	return( CSG_Tool::On_Parameter_Changed(pParameters, pParameter) );
}


//---------------------------------------------------------
bool CLandsat_QA_Import::On_Execute(void)
{
	CSG_Grids *pBits = NULL; 
	CSG_Grids *pBytes = NULL; 

	CSG_Table Info_Bands;
	Info_Bands.Add_Field("ID", 			SG_DATATYPE_String );
	Info_Bands.Add_Field("BIT_POS", 	SG_DATATYPE_Int );
	Info_Bands.Add_Field("BIT_LEN", 	SG_DATATYPE_Int );
	Info_Bands.Add_Field("BAND", 		SG_DATATYPE_String );
	Info_Bands.Add_Field("BAND_NAME", 	SG_DATATYPE_String );
	Info_Bands.Add_Field("FLAG_DESC",  	SG_DATATYPE_String );

	Parameters("OUTPUT" )->asGridList()->Del_Items();

	// Organizing of the Inputs. The Tool offers multiple optional 
	// Parameter Input Grids which are handled differently based of the Sensor and
	// the collection and potentially level and the user selection.
	std::vector<Input> 	Inputs; 
	if( !Set_Inputs( Inputs ) )
	{
		Error_Set(_TL("No input datasets provided"));
		return false;
	}

	for( auto Input : Inputs )
	{
		for( Flag_Info &Flag : Input.Flags )
		{
			if( (Flag.pGridTarget = SG_Create_Grid(Get_System(), Flag.Type)) == NULL )
	  		{
				Error_Set(_TL("Memory allocation failed"));
				return false;
			}

			Process_Set_Text(CSG_String::Format("Decode Flag: %s", CSG_String(Flag.Band_Name).c_str() ));
			#pragma omp parallel for
			for( sLong i=0; i<Get_NCells(); i++ )
			{
				// Note: Decode_Value only supports one or two bits decoding until this point
				double Value = Decode_Value( Input.pGridInput->asShort(i), Flag.Pos, Flag.Len );
				Flag.pGridTarget->Set_Value(i, Value , false);
			}
			
			// Grid Collection
			if( Parameters("GRIDS")->asBool() )
			{
				if( Flag.Type == SG_DATATYPE_Bit )
				{
					if( pBits == NULL )
					{
						pBits = SG_Create_Grids( Get_System(), Info_Bands);
						pBits->Set_Name(Input.pGridInput->Get_Name());
					}
					pBits->Add_Grid( *Set_Grids_Attribute( Info_Bands, Flag), Flag.pGridTarget );
				}
				if( Flag.Type == SG_DATATYPE_Byte )
				{
					if( pBytes == NULL )
					{
						pBytes = SG_Create_Grids( Get_System(), Info_Bands);
						pBytes->Set_Name(Input.pGridInput->Get_Name());
					}
					pBytes->Add_Grid( *Set_Grids_Attribute( Info_Bands, Flag), Flag.pGridTarget );
				}
			}
			else
	  		{
				Flag.pGridTarget->Set_Name( CSG_String::Format("%s_%s", Input.pGridInput->Get_Name(), CSG_String(Flag.Band).c_str()) );
					
				if( has_GUI() )
				{
					// Do the Update first. This will ensure the LUT is created by the GUI. 
					SG_UI_DataObject_Add( Flag.pGridTarget, SG_UI_DATAOBJECT_UPDATE );
					Create_LUT( Flag.pGridTarget , Get_LUT( Flag ) );
				}

				Parameters("OUTPUT" )->asGridList()->Add_Item( Flag.pGridTarget );
			}
		}

		CSG_Parameter_Grid_List *pGridList = Parameters("OUTPUT" )->asGridList();
		if( pBits )
		{
			pGridList->Add_Item( pBits );
			pBits = NULL;
		}
		if( pBytes )
		{
			pGridList->Add_Item( pBytes );
			pBytes = NULL;
		}
	}
			
	//-----------------------------------------------------
	return( Parameters("OUTPUT")->asGridList()->Get_Grid_Count() > 0 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
std::vector<LUT_Keys> CLandsat_QA_Import::Get_LUT( Flag_Info Flag )
{
	if( CSG_String(Flag.Band).is_Same_As("Aerosol_Level") )
	{ 
		return Aerosol_Level;
	}

	return( Flag.Len == 1 ? Generic_Bool : Generic_Confidece );
}


//---------------------------------------------------------
std::vector<Flag_Info> CLandsat_QA_Import::Get_Flags( CSG_Parameter *pParameter, int Sensor )
{
	if( pParameter->Cmp_Identifier("IN_QA_RADSAT") && pParameter->asGrid() )
	{
		if( Sensor == 0 )
			return MSS_C2_L1_QA_Sat;
		if( Sensor == 1 )
			return TM_ETM_C2_L1_QA_Sat;
		if( Sensor == 2 )
			return OLI_TIRS_C2_L1_QA_Sat;
	}
	
	if( pParameter->Cmp_Identifier("IN_QA_PIXEL") && pParameter->asGrid() )
	{
		if( Sensor == 0 )
			return MSS_C2_L1_QA_Pix;
		if( Sensor == 1 )
			return TM_ETM_C2_L1_QA_Pix;
		if( Sensor == 2 )
			return OLI_TIRS_C2_L1_QA_Pix;
	}
	
	if( pParameter->Cmp_Identifier("IN_SR_QA_AEROSOL") && pParameter->asGrid() )
	{
		if( Sensor == 2 )
			return OLI_TIRS_C2_L1_QA_Aerosol;
	}

	//-----------------------------------------------------
	return std::vector<Flag_Info>();
}


//---------------------------------------------------------
std::vector<Flag_Info> CLandsat_QA_Import::Get_Flags_Selection( CSG_Parameter *pParameter, int Sensor )
{
	std::vector<Flag_Info> Flags = Get_Flags( pParameter, Sensor ); std::vector<Flag_Info> Selection = {};

	CSG_Parameter_Choices *pChoices = NULL;
	if( pParameter->Get_Child(0) && (pChoices = pParameter->Get_Child(0)->asChoices()) )
	{
		for( int i=0; i<pChoices->Get_Selection_Count(); i++ )
		{
			// Recover the selected indices from the Item-Data
			Selection.push_back( Flags.at(pChoices->Get_Selection_Data(i).asInt()) );
		}
	}

	//-----------------------------------------------------
	return Selection;
}


//---------------------------------------------------------
bool CLandsat_QA_Import::Set_Inputs( std::vector<Input> &Input )
{
	int Sensor 		= Parameters("SENSOR")->asInt();
	bool Selection 	= Parameters("SELECTION")->asBool();
	CSG_Parameter* Input_Parameter[] = { Parameters("IN_QA_PIXEL"), Parameters("IN_QA_RADSAT") , Parameters("IN_SR_QA_AEROSOL")};
	
	for( auto P : Input_Parameter )
	{
		CSG_Grid *pGrid = P->asGrid();
		if( pGrid )
	 	{
			Input.push_back({ pGrid, Selection ? Get_Flags_Selection(P, Sensor) : Get_Flags(P, Sensor) , false} );
		
			if( P->Cmp_Identifier("IN_QA_PIXEL") )
			{
				if( Sensor == 1 && has_GUI() && Parameters("SET_LUT")->asBool() ) 
					Create_LUT( pGrid, TM_ETM_C2_QA_Pix_LUT );
				if( Sensor == 2 && has_GUI() && Parameters("SET_LUT")->asBool() ) 
					Create_LUT( pGrid, OLI_TIRS_C2_QA_Pix_LUT );
			}
			
			if( P->Cmp_Identifier("IN_SR_QA_AEROSOL") )
			{
				if( Sensor == 2 && has_GUI() && Parameters("SET_LUT")->asBool() ) 
					Create_LUT( pGrid, OLI_TIRS_C2_QA_Aerosol_LUT );
			}
		}
	}

	//-----------------------------------------------------
	return( Input.size() > 0 );
}


//---------------------------------------------------------
CSG_Table_Record* CLandsat_QA_Import::Set_Grids_Attribute(CSG_Table &Table, Flag_Info Flag)
{
	CSG_Table_Record *pRec = Table.Add_Record();

	pRec->Set_Value(0, Flag.Band		);
	pRec->Set_Value(1, Flag.Pos			);
	pRec->Set_Value(2, Flag.Len			);
	pRec->Set_Value(3, Flag.ID			);
	pRec->Set_Value(4, Flag.Band_Name	);
	pRec->Set_Value(5, Flag.Desc		);

	//-----------------------------------------------------
	return pRec;
}


//---------------------------------------------------------
double CLandsat_QA_Import::Decode_Value( short Value, size_t Position, size_t Length )
{
	std::bitset<16> BitMask(Value);

	if( Length == 1 )
	{
		return (double) BitMask[Position];
	}
	// I was kinda lazy to implement arbitrary length since i 
	// only saw two bits used for one flag until now...
	else if( Length == 2 )
	{
		std::bitset<2> Extract(0);
		Extract[0] = BitMask[Position	 ];
		Extract[1] = BitMask[Position + 1];

		return (double) Extract.to_ulong();
	}

	//-----------------------------------------------------
	return (double) BitMask.to_ulong();
}


//---------------------------------------------------------
bool CLandsat_QA_Import::Set_LUT(CSG_Table_Record *pRec, LUT_Keys Flag)
{
	pRec->Set_Value(0, Flag.Color 		); 
	pRec->Set_Value(1, Flag.Name 		); 
	pRec->Set_Value(2, Flag.Description ); 
	pRec->Set_Value(3, Flag.Value 		);
	pRec->Set_Value(4, Flag.Value 		);

	return true;
}


//---------------------------------------------------------
bool CLandsat_QA_Import::Create_LUT(CSG_Grid *pGrid, std::vector<LUT_Keys> Keys)
{
	CSG_Parameter 	*pLUT 	= DataObject_Get_Parameter(pGrid, "LUT");

 	// Early check. If the Grid is unknown to the GUI the "LUT" Parameter is a nullptr
	// so the asTable() is unsafe
	if( pLUT == NULL ){ return false; }

	CSG_Table 		*pTable = pLUT->asTable();
	if( pTable )
	{
		pTable->Del_Records();

		for( LUT_Keys Key : Keys  )
		{
			Set_LUT( pTable->Add_Record(), Key );
		}

		//-------------------------------------------------
		return( DataObject_Set_Parameter(pGrid, pLUT) && DataObject_Set_Parameter(pGrid, "COLORS_TYPE", 1) );
	}

	//-----------------------------------------------------
	return false;
}

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////
