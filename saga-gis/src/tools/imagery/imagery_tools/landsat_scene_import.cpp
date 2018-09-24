
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "landsat_scene_import.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CLandsat_Scene_Import::CLandsat_Scene_Import(void)
{
	//-----------------------------------------------------
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
		CSG_String::Format("%s|*.met;*.txt|%s (*.met)|*.met|%s (*.txt)|*.txt|%s|*.*",
			_TL("Recognized Files"),
			_TL("Landsat Metadata Files"),
			_TL("Text Files"),
			_TL("All Files")
		)
	);

	Parameters.Add_Grid_List("",
		"BANDS"			, _TL("Bands"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Table("",
		"BAND_INFO"		, _TL("Band Info"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Bool("",
		"MULTI2GRIDS"	, _TL("Multispectral Bands as Grid Collection"),
		_TL(""),
		true
	);

	Parameters.Add_Bool("",
		"SKIP_PAN"		, _TL("Skip Panchromatic Band"),
		_TL(""),
		true
	);

	Parameters.Add_Choice("",
		"CALIBRATION"	, _TL("Radiometric Calibration"),
		_TL(""),
		CSG_String::Format("%s|%s|%s",
			_TL("none"),
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
		_TL(""),
		CSG_String::Format("%s|%s|%s",
			_TL("UTM North"),
			_TL("UTM South"),
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
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CLandsat_Scene_Import::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("METAFILE") )
	{
		CSG_MetaData	Metadata;

		if( Load_Metadata(Metadata, pParameter->asString()) )
		{
			int	Sensor	= Get_Info_Sensor(Metadata);

			pParameters->Set_Enabled("SKIP_PAN", Sensor == SENSOR_ETM || Sensor == SENSOR_OLI_TIRS);
		}
		else
		{
			pParameters->Set_Enabled("SKIP_PAN", false);
		}
	}

	if( pParameter->Cmp_Identifier("CALIBRATION") )
	{
		pParameters->Set_Enabled("DATA_TYPE", pParameter->asInt() != 0);
		pParameters->Set_Enabled("TEMP_UNIT", pParameter->asInt() == 2);
	}

	if( pParameter->Cmp_Identifier("PROJECTION") )
	{
		pParameters->Set_Enabled("RESAMPLING", pParameter->asInt() == 2);
	}

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CLandsat_Scene_Import::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_MetaData	Metadata;

	if( !Load_Metadata(Metadata, Parameters("METAFILE")->asString()) )
	{
		Error_Fmt("%s [%s]", _TL("failed to load Landsat metadata"), Parameters("METAFILE")->asString());

		return( false );
	}

	//-----------------------------------------------------
	CSG_MetaData	Info_Scene;
	CSG_Table		Info_Bands;
	CSG_Strings		File_Bands;

	if( !Get_Info(Metadata, File_Bands, Info_Bands, Info_Scene) )
	{
		return( false );
	}

	int	Sensor	= Get_Info_Sensor(Metadata);

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
	CSG_String	Path	= SG_File_Get_Path(Parameters("METAFILE")->asString());

	bool	bSkipPan	= Parameters("SKIP_PAN"   )->asBool();
	bool	bMultiGrids	= Parameters("MULTI2GRIDS")->asBool();
	int		Calibration	= Parameters("CALIBRATION")->asInt ();

	double	SunHeight	= -1;

	if( Info_Scene("SUN_ELEVATION") )
	{
		SunHeight	= Info_Scene["SUN_ELEVATION"].Get_Content().asDouble();
	}

	//-----------------------------------------------------
	Parameters("BANDS")->asGridList()->Del_Items();

	CSG_Grids	*pBands	= NULL;

	for(int i=0; i<File_Bands.Get_Count() && Process_Get_Okay(); i++)
	{
		if( bSkipPan && is_Panchromatic(Sensor, i) )
		{
			continue;
		}

		Process_Set_Text(CSG_String::Format("%s: %s", _TL("loading"), File_Bands[i].c_str()));

		CSG_Grid	*pBand	= Load_Band(SG_File_Make_Path(Path, File_Bands[i], ""));

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

			if( bMultiGrids && is_Multispectral(Sensor, i) && !is_Cirrus(Sensor, i) )
			{
				if( pBands == NULL )
				{
					if( (pBands = SG_Create_Grids(pBand->Get_System(), Info_Bands)) == NULL )
					{
						Error_Set(_TL("memory allocation failed"));

						return( false );
					}

					Parameters("BANDS")->asGridList()->Add_Item(pBands);
				}

				pBands->Add_Grid(Info_Bands[i], pBand, true);
			}
			else
			{
				Parameters("BANDS")->asGridList()->Add_Item(pBand);
			}
		}
	}

	if( pBands )
	{
		pBands->Set_Name(SG_File_Get_Name(Parameters("METAFILE")->asString(), false));
		pBands->Get_MetaData().Add_Child(Info_Scene)->Set_Name("LANDSAT");
		pBands->Set_Description(Info_Scene.asText());
		pBands->Set_Z_Attribute (4);
		pBands->Set_Z_Name_Field(2);
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CLandsat_Scene_Import::is_Panchromatic(int Sensor, int Band)
{
	return( (Sensor == SENSOR_ETM && Band == 8) || (Sensor == SENSOR_OLI_TIRS && Band == 7) );
}

//---------------------------------------------------------
bool CLandsat_Scene_Import::is_Multispectral(int Sensor, int Band)
{
	switch( Sensor )
	{
	case SENSOR_MSS:
		return( true );

	case SENSOR_TM:
		return( Band != 5 );

	case SENSOR_ETM:
		return( Band != 5 && Band != 6 && Band != 8 );

	case SENSOR_OLI_TIRS:
		return( Band != 7 && Band != 9  && Band != 10 );
	}

	return( false );
}

//---------------------------------------------------------
bool CLandsat_Scene_Import::is_Cirrus(int Sensor, int Band)
{
	return( (Sensor == SENSOR_OLI_TIRS && Band == 8) );
}

//---------------------------------------------------------
bool CLandsat_Scene_Import::is_Thermal(int Sensor, int Band)
{
	return( !is_Panchromatic(Sensor, Band) && !is_Multispectral(Sensor, Band) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CLandsat_Scene_Import::Load_Metadata(CSG_MetaData &Metadata, const CSG_String &File)
{
	CSG_File	Stream;

	if( !Stream.Open(File, SG_FILE_R, false) )
	{
		return( false );
	}

	CSG_String	Line, Key, Value;

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

	return( Metadata.Get_Children_Count() > 0 );
}

//---------------------------------------------------------
bool CLandsat_Scene_Import::Load_Metadata(const CSG_String &Line, CSG_String &Key, CSG_String &Value)
{
	Key	= Line.BeforeFirst('='); Key.Trim(true); Key.Trim(false);

	if( !Key.is_Empty() )
	{
		Value	= Line.AfterFirst('='); Value.Trim(true); Value.Trim(false);

		if( Value.Length() >= 2 && Value[0] == '\"' )
		{
			Value	= Value.Mid(1, Value.Length() - 2);
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
		CSG_String	Sensor	= Metadata["SENSOR_ID"].Get_Content();

		if( !Sensor.Cmp("MSS"     ) )	return( SENSOR_MSS      );
		if( !Sensor.Cmp("TM"      ) )	return( SENSOR_TM       );
		if( !Sensor.Cmp("ETM"     ) )	return( SENSOR_ETM      );
		if( !Sensor.Cmp("ETM+"    ) )	return( SENSOR_ETM      );
		if( !Sensor.Cmp("OLI_TIRS") )	return( SENSOR_OLI_TIRS );
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

	if( !Metadata.Get_Name().Cmp("L1_METADATA_FILE") )
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
	BAND_HEAD_ID	= 0,
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
	int	Version	= Get_Info_Version(Metadata);

	if( Version == VERSION_UNKNOWN )
	{
		Error_Set(_TL("unrecognized metadata version"));

		return( false );
	}

	int	Sensor	= Get_Info_Sensor(Metadata);

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
	int	iField;

	Info_Bands.Destroy();
	Info_Bands.Set_Name(_TL("Band Info"));

	for(iField=0; iField<BAND_HEAD_COUNT; iField++)
	{
		Info_Bands.Add_Field(
			Band_Head[iField].Name,
			Band_Head[iField].Type
		);
	}

	bool	bOkay[BAND_DATA_COUNT];

	for(iField=0; iField<BAND_DATA_COUNT; iField++)
	{
		bOkay[iField]	= false;

		Info_Bands.Add_Field(
			Band_Data[iField].Name,
			Band_Data[iField].Type
		);
	}

	//-----------------------------------------------------
	const int	Sensor_nBands[SENSOR_UNKNOWN]	=
	{
		4, 7, 9, 11
	};

	for(int Band=0; Band<Sensor_nBands[Sensor]; Band++)
	{
		CSG_String	Value;

		if( !Get_Info_Band(Metadata, Version, Sensor, Band, BAND_DATA_FILE, Value) )
		{
			return( false );
		}

		File_Bands	+= Value;

		CSG_Table_Record	&Info	= *Info_Bands.Add_Record();

		Set_Info_Band(Sensor, Band, Info);	// set defaults

		for(iField=0; iField<BAND_DATA_COUNT; iField++)
		{
			if( Get_Info_Band(Metadata, Version, Sensor, Band, iField, Value) )
			{
				bOkay[iField]	= true;

				Info.Set_Value (BAND_HEAD_COUNT + iField, Value);
			}
			else
			{
				Info.Set_NoData(BAND_HEAD_COUNT + iField);
			}
		}
	}

	//-----------------------------------------------------
	for(iField=BAND_DATA_COUNT-1; iField>=0; iField--)
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
	CSG_String	Name;

	if( Sensor == SENSOR_ETM )
	{
		switch( Band )
		{
		case 0:	Name	= Version != VERSION_MTL_2  ? "BAND1"  : "BAND_1"       ; break;
		case 1:	Name	= Version != VERSION_MTL_2  ? "BAND2"  : "BAND_2"       ; break;
		case 2:	Name	= Version != VERSION_MTL_2  ? "BAND3"  : "BAND_3"       ; break;
		case 3:	Name	= Version != VERSION_MTL_2  ? "BAND4"  : "BAND_4"       ; break;
		case 4:	Name	= Version != VERSION_MTL_2  ? "BAND5"  : "BAND_5"       ; break;
		case 5:	Name	= Version != VERSION_MTL_2  ? "BAND61" : "BAND_6_VCID_1"; break;
		case 6:	Name	= Version != VERSION_MTL_2  ? "BAND62" : "BAND_6_VCID_2"; break;
		case 7:	Name	= Version != VERSION_MTL_2  ? "BAND7"  : "BAND_7"       ; break;
		case 8:	Name	= Version != VERSION_MTL_2  ? "BAND8"  : "BAND_8"       ; break;
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
	CSG_String	Tag;

	Tag.Printf(Key != BAND_DATA_FILE ? Band_Data[Key].Format : Version != VERSION_MTL_2 
		? "%s_FILE_NAME" : "FILE_NAME_%s", Name.c_str()
	);

	if( !Metadata(Tag) )
	{
		return( false );
	}

	Value	= Metadata[Tag].Get_Content();

	return( true );
}

//---------------------------------------------------------
bool CLandsat_Scene_Import::Set_Info_Band(int Sensor, int Band, CSG_Table_Record &Info)
{
	#define SET_INFO_BAND(band, name, wmin, wmax)	{\
		Info.Set_Value(BAND_HEAD_ID      , 1 + Info.Get_Index());\
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

	case SENSOR_OLI_TIRS:
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
CSG_Grid * CLandsat_Scene_Import::Load_Band(const CSG_String &File)
{
	CSG_Grid	*pBand	= SG_Create_Grid(File);

	if( !pBand )
	{
		return( NULL );
	}

	pBand->Set_NoData_Value(0);	// landsat 8 pretends to use a value of 65535 (2^16 - 1)

	if( !pBand->Get_Projection().is_Okay() )
	{
		// undefined coordinate system, nothing to do be further done...
	}

	//-----------------------------------------------------
	else if( Parameters("PROJECTION")->asInt() != 2 ) // UTM
	{
		CSG_Grid	*pTmp	= pBand;

		CSG_String	Projection	= pTmp->Get_Projection().Get_Proj4();

		if( Projection.Find("+proj=utm") >= 0
		&&  (  (Projection.Find("+south") >= 0 && Parameters("PROJECTION")->asInt() == 0)
		    || (Projection.Find("+south") <  0 && Parameters("PROJECTION")->asInt() == 1))
		&&  (pBand = SG_Create_Grid(pTmp->Get_Type(), pTmp->Get_NX(), pTmp->Get_NY(), pTmp->Get_Cellsize(),
				pTmp->Get_XMin(), pTmp->Get_YMin() + (Parameters("PROJECTION")->asInt() == 1 ? 10000000 : -10000000)
			)) != NULL )
		{
			if( Parameters("PROJECTION")->asInt() == 1 )
				Projection.Append (" +south");
			else
				Projection.Replace(" +south", "");

			pBand->Get_Projection().Create(Projection, SG_PROJ_FMT_Proj4);

			pBand->Set_Name              (pTmp->Get_Name());
			pBand->Set_Description       (pTmp->Get_Description());
			pBand->Set_NoData_Value_Range(pTmp->Get_NoData_Value(), pTmp->Get_NoData_hiValue());
			pBand->Set_Scaling           (pTmp->Get_Scaling(), pTmp->Get_Offset());

			#pragma omp parallel for
			for(int y=0; y<pBand->Get_NY(); y++)
			{
				for(int x=0; x<pBand->Get_NX(); x++)
				{
					pBand->Set_Value(x, y, pTmp->asDouble(x, y));
				}
			}

			delete(pTmp);
		}
	}

	//-----------------------------------------------------
	else if( Parameters("PROJECTION")->asInt() == 2 )	// Geographic Coordinates
	{
		CSG_Tool	*pTool	= SG_Get_Tool_Library_Manager().Get_Tool("pj_proj4", 4);	// Coordinate Transformation (Grid)

		if(	pTool )
		{
			Message_Fmt("\n%s (%s: %s)\n", _TL("re-projection to geographic coordinates"), _TL("original"), pBand->Get_Projection().Get_Name().c_str());

			pTool->Settings_Push(NULL);

			if( pTool->Set_Parameter("CRS_PROJ4" , SG_T("+proj=longlat +ellps=WGS84 +datum=WGS84"))
			&&  pTool->Set_Parameter("SOURCE"    , pBand)
			&&  pTool->Set_Parameter("RESAMPLING", Parameters("RESAMPLING"))
			&&  pTool->Set_Parameter("KEEP_TYPE" , true)
			&&  pTool->Execute() )
			{
				delete(pBand);

				pBand	= pTool->Get_Parameters()->Get_Parameter("GRID")->asGrid();
			}

			pTool->Settings_Pop();
		}
	}

	//-----------------------------------------------------
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
	pBand->Set_NoData_Value(-1.0);

	return( true );
}

//---------------------------------------------------------
bool CLandsat_Scene_Import::Get_Radiance(CSG_Grid *pBand, const CSG_Table_Record &Info_Band)
{
	//-----------------------------------------------------
	double	Offset, Scale, DNmin;

	if( Info_Band.asString("REFLECTANCE_ADD") && Info_Band.asString("REFLECTANCE_MUL") )
	{
		DNmin	=  0.0;
		Offset	=  Info_Band.asDouble("REFLECTANCE_ADD");
		Scale	=  Info_Band.asDouble("REFLECTANCE_MUL");
	}
	else if( Info_Band.asString("L_MIN") && Info_Band.asString("L_MAX") && Info_Band.asString("QCAL_MIN") && Info_Band.asString("QCAL_MAX") )
	{
		DNmin	=  Info_Band.asDouble("QCAL_MIN");
		Offset	=  Info_Band.asDouble("L_MIN");
		Scale	= (Info_Band.asDouble("L_MAX") - Offset) / (Info_Band.asDouble("QCAL_MAX") - DNmin);
	}
	else
	{
		SG_UI_Msg_Add_Error(CSG_String::Format("%s: %s", pBand->Get_Name(), _TL("failed to derive radiances")));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Grid	DN(*pBand);

	if( Parameters("DATA_TYPE")->asInt() == 1 )
	{
		Get_Float(pBand, DN);
	}
	else
	{
		double	MaxVal	= (pBand->Get_Type() == SG_DATATYPE_Byte ? 256 : 256*256) - 1;
		pBand->Set_NoData_Value(MaxVal--);
		pBand->Set_Scaling(1000.0 / MaxVal, 0.0);
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
	double	Offset, Scale;

	if( Info_Band.asString("REFLECTANCE_ADD") && Info_Band.asString("REFLECTANCE_MUL") )
	{
		Offset	= Info_Band.asDouble("REFLECTANCE_ADD");
		Scale	= Info_Band.asDouble("REFLECTANCE_MUL");
	}
	else
	{
		SG_UI_Msg_Add_Error(CSG_String::Format("%s: %s", pBand->Get_Name(), _TL("failed to derive reflectances")));

		return( false );
	}

	SunHeight	= sin(SunHeight * M_DEG_TO_RAD);

	//-----------------------------------------------------
	CSG_Grid	DN(*pBand);

	if( Parameters("DATA_TYPE")->asInt() == 1 )
	{
		Get_Float(pBand, DN);
	}
	else
	{
		double	MaxVal	= (pBand->Get_Type() == SG_DATATYPE_Byte ? 256 : 256*256) - 1;
		pBand->Set_NoData_Value(MaxVal--);
		pBand->Set_Scaling(100.0 / MaxVal, 0.0);	// 0 to 100 percent
	}

	pBand->Set_Unit("Percent");

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
			pBand->Set_Value(i, 100.0 * (Offset + Scale * DN.asDouble(i)) / SunHeight);
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

	double	Offset	= Info_Band.asDouble("RADIANCE_ADD");
	double	Scale	= Info_Band.asDouble("RADIANCE_MUL");

	double	k1		= Info_Band.asDouble("THERMAL_K1");
	double	k2		= Info_Band.asDouble("THERMAL_K2");

	//-----------------------------------------------------
	CSG_Grid	DN(*pBand);

	int	Unit	= Parameters("TEMP_UNIT")->asInt();

	if( Parameters("DATA_TYPE")->asInt() == 1 )
	{
		Get_Float(pBand, DN);
	}
	else
	{
		double	MaxVal	= (pBand->Get_Type() == SG_DATATYPE_Byte ? 256 : 256*256) - 1;
		pBand->Set_NoData_Value(MaxVal--);
		pBand->Set_Scaling(100.0 / MaxVal, (Unit == 0 ? 273.15 : 0.0) - 40.0);	// -40°C to 60°C
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
			double	r	= Offset + Scale * DN.asDouble(i);

			pBand->Set_Value(i, k2 / log(1.0 + (k1 / r)) - (Unit == 0 ? 0.0 : 273.15));
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
