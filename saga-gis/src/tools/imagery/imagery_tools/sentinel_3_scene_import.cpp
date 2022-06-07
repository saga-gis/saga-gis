
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
//               sentinel_3_scene_import.cpp             //
//                                                       //
//                 Olaf Conrad (C) 2022                  //
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
#include "sentinel_3_scene_import.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
enum EBand_Head
{
	INFO_FIELD_ID = 0,
	INFO_FIELD_BAND,
	INFO_FIELD_CENTRE,
	INFO_FIELD_WIDTH,
	INFO_FIELD_LMIN,
	INFO_FIELD_LREF,
	INFO_FIELD_LSAT,
	INFO_FIELD_SNR,
	INFO_FIELD_FUNCTION,
	INFO_FIELD_COUNT
};

//---------------------------------------------------------
CSG_Table CSentinel_3_Scene_Import::Get_Info_Bands(void)
{
	CSG_Table Info_Bands;

	Info_Bands.Add_Field("ID"      , SG_DATATYPE_Int   );
	Info_Bands.Add_Field("BAND"    , SG_DATATYPE_String);
	Info_Bands.Add_Field("CENTRE"  , SG_DATATYPE_Double);
	Info_Bands.Add_Field("WIDTH"   , SG_DATATYPE_Double);
	Info_Bands.Add_Field("LMIN"    , SG_DATATYPE_Double);
	Info_Bands.Add_Field("LREF"    , SG_DATATYPE_Double);
	Info_Bands.Add_Field("LSAT"    , SG_DATATYPE_Double);
	Info_Bands.Add_Field("SNR"     , SG_DATATYPE_Double);
	Info_Bands.Add_Field("FUNCTION", SG_DATATYPE_String);

	#define ADD_INFO_BAND(Centre, Width, Lmin, Lref, Lsat, SNR, Function) { CSG_Table_Record &Info = *Info_Bands.Add_Record();\
		Info.Set_Value(INFO_FIELD_ID      ,                              1 + Info.Get_Index());\
		Info.Set_Value(INFO_FIELD_BAND    , CSG_String::Format("Oa%02d", 1 + Info.Get_Index()));\
		Info.Set_Value(INFO_FIELD_CENTRE  , Centre  );\
		Info.Set_Value(INFO_FIELD_WIDTH   , Width   );\
		Info.Set_Value(INFO_FIELD_LMIN    , Lmin    );\
		Info.Set_Value(INFO_FIELD_LREF    , Lref    );\
		Info.Set_Value(INFO_FIELD_LSAT    , Lsat    );\
		Info.Set_Value(INFO_FIELD_SNR     , SNR     );\
		Info.Set_Value(INFO_FIELD_FUNCTION, Function);\
	}

	ADD_INFO_BAND( 400    , 15   , 21.6 , 62.95 , 413.5, 2188, "Aerosol correction, improved water constituent retrieval."                                             );
	ADD_INFO_BAND( 412.5  , 10   , 25.93, 74.14 , 501.3, 2061, "Yellow substance and detrital pigments (Turbidity)."                                                   );
	ADD_INFO_BAND( 442.5  , 10   , 23.96, 65.61 , 466.1, 1811, "Chl absorption max., Biogeochemistry, vegetation."                                                     );
	ADD_INFO_BAND( 442    , 10   , 19.78, 51.21 , 483.3, 1541, "High Chl, other pigments."                                                                             );
	ADD_INFO_BAND( 510    , 10   , 17.45, 44.39 , 449.6, 1488, "Chl, sediment, turbidity, red tide."                                                                   );
	ADD_INFO_BAND( 560    , 10   , 12.73, 31.49 , 524.5, 1280, "Chlorophyll reference (Chl minimum)"                                                                   );
	ADD_INFO_BAND( 620    , 10   ,  8.86, 21.14 , 397.9,  997, "Sediment loading"                                                                                      );
	ADD_INFO_BAND( 665    , 10   ,  7.12, 16.38 , 364.9,  883, "Chl (2nd Chl abs. max.), sediment, yellow	substance/vegetation"                                      );
	ADD_INFO_BAND( 673.75 ,  7.5 ,  6.87, 15.7  , 443.1,  707, "For improved fluorescence retrieval"                                                                   );
	ADD_INFO_BAND( 681.25 ,  7.5 ,  6.65, 15.11 , 350.3,  745, "Chl fluorescence peak, red edge."                                                                      );
	ADD_INFO_BAND( 708.75 , 10   ,  5.66, 12.73 , 332.4,  785, "Chl fluorescence baseline, red edge transition."                                                       );
	ADD_INFO_BAND( 753.75 ,  7.5 ,  4.7 , 10.33 , 377.7,  605, "O2 absorption reference, clouds, vegetation."                                                          );
	ADD_INFO_BAND( 761.25 ,  2.5 ,  2.53,  6.09 , 369.5,  232, "O2 absorption band, fluorescence over land."                                                           );
	ADD_INFO_BAND( 764.375,  3.75,  3   ,  7.13 , 373.4,  305, "O2 absorption band, fluorescence over land."                                                           );
	ADD_INFO_BAND( 767.5  ,  2.5 ,  3.27,  7.58 , 250  ,  330, "O2 absorption band, fluorescence over land."                                                           );
	ADD_INFO_BAND( 778.75 , 15   ,  4.22,  9.18 , 277.5,  812, "Atmos. Corr./aerosol corr."                                                                            );
	ADD_INFO_BAND( 865    , 20   ,  2.88,  6.17 , 229.5,  666, "Atmos. Corr./aerosol corr., clouds, pixel coregistration. Common reference band with SLSTR instrument.");
	ADD_INFO_BAND( 885    , 10   ,  2.8 ,  6    , 281  ,  395, "Water vapour absorption reference band."                                                               );
	ADD_INFO_BAND( 900    , 10   ,  2.05,  4.73 , 237.6,  308, "Water vapour absorption/vegetation monitoring (max. reflectance)."                                     );
	ADD_INFO_BAND( 940    , 20   ,  0.94,  2.39 , 171.7,  203, "Water vapour absorption, atmos./aerosol corr."                                                         );
	ADD_INFO_BAND(1020    , 40   ,  1.81,  3.86 , 163.7,  152, "Atmos./aerosol corr."                                                                                  );

	return( Info_Bands );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSentinel_3_Scene_Import::CSentinel_3_Scene_Import(void)
{
	Set_Name		(_TL("Import Sentinel-3 OLCI Scene"));

	Set_Author		("O.Conrad (c) 2022");

	Set_Description	(_TW(
		"Import Sentinel-3 OLCI (Ocean and Land Colour Instrument) scenes "
		"from a folder structure as provided by the original ESA download. "
	));

	Add_Reference("https://sentinel.esa.int/web/sentinel/missions/sentinel-3",
		SG_T("Sentinel-3 at ESA's Sentinel Online")
	);

	Add_Reference("https://sentinel.esa.int/documents/247904/4598066/Sentinel-3-OLCI-Land-Handbook.pdf",
		SG_T("Sentinel-3 User Handbook")
	);

	//-----------------------------------------------------
	Parameters.Add_FilePath("",
		"DIRECTORY"		, _TL("Directory"),
		_TL(""), NULL, NULL, false, true
	);

	Parameters.Add_Grid_List("",
		"BANDS"			, _TL("Bands"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Bool("",
		"COLLECTION"	, _TL("Bands as Grid Collection"),
		_TL(""),
		true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CSentinel_3_Scene_Import::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSentinel_3_Scene_Import::On_Execute(void)
{
	CSG_String Directory = Parameters("DIRECTORY")->asString();

	if( !SG_Dir_Exists(Directory) )
	{
		Error_Fmt("%s [%s]", _TL("directory does not exist"), Directory.c_str());

		return( false );
	}

	//-----------------------------------------------------
	CSG_Grid *pLon = Load_Band(Directory, "geo_coordinates.nc", "longitude");
	CSG_Grid *pLat = Load_Band(Directory, "geo_coordinates.nc",  "latitude");

	if( !pLon || !pLat )
	{
		m_Data.Delete_All();

		return( false );
	}

	pLon->Set_Scaling(0.000001);
	pLat->Set_Scaling(0.000001);

	//-----------------------------------------------------
	CSG_Parameters P; CSG_Parameter_Grid_List *pBands = P.Add_Grid_List("", "BANDS", "", "", PARAMETER_OUTPUT, false)->asGridList();

	for(int i=0; i<21; i++)
	{
		pBands->Add_Item(Load_Band(Directory, CSG_String::Format("Oa%02d_radiance.nc", i + 1)));
	}

	//-----------------------------------------------------
	if( !Georeference(pLon, pLat, pBands) )
	{
		return( false );
	}

	//-----------------------------------------------------
	if( Parameters("COLLECTION")->asBool() )
	{
		CSG_Table Info_Bands(Get_Info_Bands());

		CSG_Grids *pCollection = SG_Create_Grids(pBands->Get_Grid(0)->Get_System(), Info_Bands);

		pBands = Parameters("BANDS")->asGridList();

		for(int i=0; i<pBands->Get_Grid_Count(); i++)
		{
			pCollection->Add_Grid(Info_Bands[i], pBands->Get_Grid(i), true);
		}

		pBands->Del_Items();

		pCollection->Set_Z_Attribute (INFO_FIELD_CENTRE);
		pCollection->Set_Z_Name_Field(INFO_FIELD_BAND  );

		pBands->Add_Item(pCollection);

		#if defined(_SAGA_MSW)
		Directory = Directory.AfterLast('\\');
		#else
		Directory = Directory.AfterLast('/' );
		#endif

	//	0         1         2         3         4         5         6         7         8         9         
	//	0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789
	//	S3A_OL_1_EFR____20180505T101722_20180505T102022_20180506T150404_0179_031_008_1980_LN1_O_NT_002.SEN3

		CSG_String Name = Directory.Left(11) + "_" + Directory.Mid(64, 8) + "_"
			+ Directory.Mid(16, 4) + "-" + Directory.Mid(20, 2) + "-" + Directory.Mid(22, 2);

		pCollection->Set_Name(Name);
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Grid * CSentinel_3_Scene_Import::Load_Band(const CSG_String &Directory, const CSG_String &Name, const CSG_String &Band)
{
	CSG_String File = SG_File_Make_Path(Directory, Name);

	if( !SG_File_Exists(File) )
	{
		Error_Fmt("%s [%s]", _TL("file does not exist"), Directory.c_str());

		return( NULL );
	}

	if( !Band.is_Empty() )
	{
		File = "HDF5:\"" + File + "\"://" + Band;

		Process_Set_Text("%s: %s.%s", _TL("loading"), Name.c_str(), Band.c_str());
	}
	else
	{
		Process_Set_Text("%s: %s"   , _TL("loading"), Name.c_str());
	}

	//-----------------------------------------------------
	SG_UI_Msg_Lock(true);

	CSG_Tool *pTool = SG_Get_Tool_Library_Manager().Create_Tool("io_gdal", 0); // Import Raster

	if( !pTool || !pTool->Set_Manager(&m_Data) || !pTool->On_Before_Execution()
	||  !pTool->Set_Parameter("FILES"     , File )
	||  !pTool->Set_Parameter("MULTIPLE"  , 0    ) // single grids
	||  !pTool->Set_Parameter("TRANSFORM" , true )
	||  !pTool->Set_Parameter("RESAMPLING", 0    ) // Nearest Neighbour
	||  !pTool->Set_Parameter("EXTENT"    , 0    ) // original
	||  !pTool->Execute() )
	{
		if( !Band.is_Empty() )
		{
			Error_Fmt("%s [%s].%s]", _TL("failed to import band"), Name.c_str(), Band.c_str());
		}
		else
		{
			Error_Fmt("%s [%s]"    , _TL("failed to import band"), Name.c_str());
		}

		SG_Get_Tool_Library_Manager().Delete_Tool(pTool);

		SG_UI_Msg_Lock(false);

		return( NULL );
	}

	//-----------------------------------------------------
	CSG_Parameter_Grid_List *pGrids = pTool->Get_Parameter("GRIDS")->asGridList();

	CSG_Grid *pBand = pGrids->Get_Grid(0);

	SG_Get_Tool_Library_Manager().Delete_Tool(pTool);

	SG_UI_Msg_Lock(false);

	return( pBand );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSentinel_3_Scene_Import::Georeference(CSG_Grid *pLon, CSG_Grid *pLat, CSG_Parameter_Grid_List *pBands)
{
	Process_Set_Text("%s", _TL("georeferencing"));

	SG_UI_Msg_Lock(true);

	CSG_Tool *pTool = SG_Get_Tool_Library_Manager().Create_Tool("pj_georeference", 7);

	if( !pTool || !pTool->Set_Manager(NULL) || !pTool->On_Before_Execution()
	||  !pTool->Set_Parameter("GRID_X"           , pLon  )
	||  !pTool->Set_Parameter("GRID_Y"           , pLat  )
	||  !pTool->Set_Parameter("GRIDS"            , pBands)
	||  !pTool->Set_Parameter("BYTEWISE"         , false )
	||  !pTool->Set_Parameter("KEEP_TYPE"        , true  )
	||  !pTool->Set_Parameter("RESAMPLING"       , 3     ) // B-Spline Interpolation
	||  !pTool->Set_Parameter("TARGET_DEFINITION", 0     ) // user defined
	||  !pTool->Set_Parameter("TARGET_USER_XMIN" , pLon->Get_Min())
	||  !pTool->Set_Parameter("TARGET_USER_XMAX" , pLon->Get_Max())
	||  !pTool->Set_Parameter("TARGET_USER_YMIN" , pLat->Get_Min())
	||  !pTool->Set_Parameter("TARGET_USER_YMAX" , pLat->Get_Max())
	||  !pTool->Set_Parameter("TARGET_USER_SIZE" , 0.0032)
	||  !pTool->Execute() )
	{
		Error_Fmt("%s", _TL("failed to apply georeferencing"));

		SG_Get_Tool_Library_Manager().Delete_Tool(pTool);

		m_Data.Delete_All();

		SG_UI_Msg_Lock(false);

		return( false );
	}

	//-----------------------------------------------------
	CSG_Parameter_Grid_List *pOutput = pTool->Get_Parameter("OUTPUT")->asGridList();

	pBands = Parameters("BANDS")->asGridList();

	for(int i=0; i<pOutput->Get_Grid_Count(); i++)
	{
		CSG_Grid *pBand = pOutput->Get_Grid(i);

		pBand->Get_Projection().Set_GCS_WGS84();

		pBands->Add_Item(pBand);
	}

	//-----------------------------------------------------
	SG_Get_Tool_Library_Manager().Delete_Tool(pTool);

	m_Data.Delete_All();

	SG_UI_Msg_Lock(false);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSentinel_3_Scene_Import::Load_Classification(CSG_Grid *pGrid, const CSG_String &File)
{
	CSG_MetaData	Metadata;

	if( !Metadata.Load(File)
	||  !Metadata("n1:General_Info")
	||  !Metadata["n1:General_Info"]("Product_Image_Characteristics")
	||  !Metadata["n1:General_Info"]["Product_Image_Characteristics"]("Scene_Classification_List") )
	{
		return( false );
	}

	CSG_MetaData ClassList = Metadata["n1:General_Info"]["Product_Image_Characteristics"]["Scene_Classification_List"];

	//-----------------------------------------------------
	CSG_Table LUT;

	LUT.Add_Field("Color"      , SG_DATATYPE_Color );
	LUT.Add_Field("Name"       , SG_DATATYPE_String);
	LUT.Add_Field("Description", SG_DATATYPE_String);
	LUT.Add_Field("Minimum"    , SG_DATATYPE_Double);
	LUT.Add_Field("Maximum"    , SG_DATATYPE_Double);

	for(int i=0, Index; i<ClassList.Get_Children_Count(); i++)
	{
		if( ClassList[i].Cmp_Name("Scene_Classification_ID")
		&&  ClassList[i]("SCENE_CLASSIFICATION_TEXT" )
		&&  ClassList[i].Get_Content("SCENE_CLASSIFICATION_INDEX", Index) )
		{
			CSG_String Name(ClassList[i]["SCENE_CLASSIFICATION_TEXT"].Get_Content().AfterFirst('_')); Name.Replace("_", " ");

			CSG_Table_Record &Class = *LUT.Add_Record();

			Class.Set_Value(0, SG_Color_Get_Random());
			Class.Set_Value(1, Name);
			Class.Set_Value(3, Index);
			Class.Set_Value(4, Index);
		}
	}

	if( LUT.Get_Count() > 0 )
	{
		DataObject_Add(pGrid);

		CSG_Parameter *pLUT = DataObject_Get_Parameter(pGrid, "LUT");

		if( pLUT && pLUT->asTable() && pLUT->asTable()->Assign_Values(&LUT) )
		{
			DataObject_Set_Parameter(pGrid, pLUT);
			DataObject_Set_Parameter(pGrid, "COLORS_TYPE", 1);	// Color Classification Type: Lookup Table

			return( true );
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
