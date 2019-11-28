
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
//               sentinel_2_scene_import.cpp             //
//                                                       //
//                 Olaf Conrad (C) 2019                  //
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
#include "sentinel_2_scene_import.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define BAND_IS_10m(b)	(b ==  2 || b ==  3 || b ==  4 || b ==  8)
#define BAND_IS_20m(b)	(b ==  5 || b ==  6 || b ==  7 || b ==  9 || b == 12 || b == 13)
#define BAND_IS_60m(b)	(b ==  1 || b == 10 || b == 11)
#define BAND_IS_TCI(b)	(b == 14)

//---------------------------------------------------------
#define FIELD_BAND	1
#define FIELD_NAME	2
#define FIELD_WAVE	3

//---------------------------------------------------------
CSG_Table CSentinel_2_Scene_Import::Get_Info_Bands(void)
{
	enum EBand_Head
	{
		BAND_HEAD_ID	= 0,
		BAND_HEAD_NR,
		BAND_HEAD_NAME,
		BAND_HEAD_WAVE_LEN,
		BAND_HEAD_COUNT
	};

	CSG_Table	Info_Bands;

	Info_Bands.Add_Field("ID"      , SG_DATATYPE_Int   );
	Info_Bands.Add_Field("BAND"    , SG_DATATYPE_String);
	Info_Bands.Add_Field("NAME"    , SG_DATATYPE_String);
	Info_Bands.Add_Field("WAVE_LEN", SG_DATATYPE_Double);

	#define ADD_INFO_BAND(band, name, wave)	{ CSG_Table_Record &Info = *Info_Bands.Add_Record();\
		Info.Set_Value(BAND_HEAD_ID      , 1 + Info.Get_Index());\
		Info.Set_Value(BAND_HEAD_NR      , band);\
		Info.Set_Value(BAND_HEAD_NAME    , CSG_String::Format("[%s] %s", SG_T(band), name));\
		Info.Set_Value(BAND_HEAD_WAVE_LEN, wave);\
	}

	ADD_INFO_BAND("01" , _TL("Aerosols"        ),  443);	//  1, 60m
	ADD_INFO_BAND("02" , _TL("Blue"            ),  490);	//  2, 10m
	ADD_INFO_BAND("03" , _TL("Green"           ),  560);	//  3, 10m
	ADD_INFO_BAND("04" , _TL("Red"             ),  665);	//  4, 10m
	ADD_INFO_BAND("05" , _TL("Red Edge"        ),  705);	//  5, 20m
	ADD_INFO_BAND("06" , _TL("Red Edge"        ),  740);	//  6, 20m
	ADD_INFO_BAND("07" , _TL("Red Edge"        ),  783);	//  7, 20m
	ADD_INFO_BAND("08" , _TL("NIR"             ),  842);	//  8, 10m
	ADD_INFO_BAND("8A" , _TL("NIR"             ),  865);	//  9, 20m
	ADD_INFO_BAND("09" , _TL("Water Vapour"    ),  945);	// 10, 20m
	ADD_INFO_BAND("10" , _TL("Cirrus"          ), 1375);	// 11, 60m
	ADD_INFO_BAND("11" , _TL("SWIR"            ), 1610);	// 12, 60m
	ADD_INFO_BAND("12" , _TL("SWIR"            ), 2190);	// 13, 20m
	ADD_INFO_BAND("TCI", _TL("True Color Image"),    0);	// 14, 10m

	return( Info_Bands );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSentinel_2_Scene_Import::CSentinel_2_Scene_Import(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Import Sentinel-2 Scene"));

	Set_Author		("O.Conrad (c) 2019");

	Set_Description	(_TW(
		"Import Sentinel-2 scenes from a folder structure as provided "
		"by the original ESA download. "
	));

	Add_Reference("https://sentinel.esa.int/web/sentinel/missions/sentinel-2",
		SG_T("Sentinel-2 at ESA's Sentinel Online")
	);

	Add_Reference("https://sentinel.esa.int/web/sentinel/document-library/content/-/article/sentinel-2-user-handbook",
		SG_T("Sentinel-2 User Handbook")
	);

	//-----------------------------------------------------
	Parameters.Add_FilePath("",
		"METAFILE"		, _TL("Metadata File"),
		_TL(""),
		CSG_String::Format("%s|MTD*.xml|%s|*.*",
			_TL("Sentinel-2 Metadata Files"),
			_TL("All Files")
		)
	);

	Parameters.Add_Grid_List("",
		"BANDS"			, _TL("Bands"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Bool("",
		"MULTI2GRIDS"	, _TL("Multispectral Bands as Grid Collection"),
		_TL(""),
		true
	);

	Parameters.Add_Bool("",
		"SKIP_TCI"		, _TL("Skip True Color Image"),
		_TL(""),
		true
	);

	Parameters.Add_Bool("",
		"SKIP_60M"		, _TL("Skip Aerosol, Vapour, Cirrus"),
		_TL(""),
		true
	);

	Parameters.Add_Choice("",
		"REFLECTANCE"	, _TL("Reflectance Values"),
		_TL(""),
		CSG_String::Format("%s|%s",
			_TL("original"),
			_TL("fraction")
		), 1
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
int CSentinel_2_Scene_Import::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
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
bool CSentinel_2_Scene_Import::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_MetaData	Info_General, Info_Granule;

	if( !Load_Metadata(Parameters("METAFILE")->asString(), Info_General, Info_Granule) )
	{
		Error_Fmt("%s [%s]", _TL("failed to load metadata"), Parameters("METAFILE")->asString());

		return( false );
	}

	CSG_String	Date	= Info_General["PRODUCT_START_TIME"].Get_Content().BeforeFirst('T');

	//-----------------------------------------------------
	CSG_String	Path	= SG_File_Get_Path(Parameters("METAFILE")->asString());

	bool	bSkipTCI	= Parameters("SKIP_TCI"   )->asBool();
	bool	bSkip60m	= Parameters("SKIP_60M"   )->asBool();
	bool	bMultiGrids	= Parameters("MULTI2GRIDS")->asBool();
	double	Scaling		= Parameters("REFLECTANCE")->asInt () == 0 ? 1. : 1. / 10000.;

	CSG_Table	Info_Bands(Get_Info_Bands());

	//-----------------------------------------------------
	Parameters("BANDS")->asGridList()->Del_Items();

	CSG_Grids	*pBands[2]; pBands[0] = pBands[1] = NULL;

	for(int i=0, Band=0; Band<Info_Bands.Get_Count() && i<Info_Granule.Get_Children_Count() && Process_Get_Okay(); i++)
	{
		if( !Info_Granule[i].Cmp_Name("IMAGE_FILE") )
		{
			continue;
		}

		Band++;

		if( (bSkipTCI && BAND_IS_TCI(Band))
		||  (bSkip60m && BAND_IS_60m(Band)) )
		{
			continue;
		}

		CSG_Grid	*pBand	= Load_Band(Path, Info_Granule[i]);

		if( !pBand )
		{
			continue;
		}

		pBand->Get_MetaData().Add_Child(Info_General)->Set_Name("SENTINEL-2");
		pBand->Set_Description(Info_General.asText());

		if( bMultiGrids && !BAND_IS_60m(Band) && !BAND_IS_TCI(Band) )
		{
			int	b	= BAND_IS_10m(Band) ? 0 : 1;

			if( pBands[b] == NULL )
			{
				if( (pBands[b] = SG_Create_Grids(pBand->Get_System(), Info_Bands)) == NULL )
				{
					Error_Set(_TL("memory allocation failed"));

					return( false );
				}

				Parameters("BANDS")->asGridList()->Add_Item(pBands[b]);
			}

			pBands[b]->Add_Grid(Info_Bands[Band - 1], pBand, true);
		}
		else
		{
			pBand->Fmt_Name("S2_%s_%s", Date.c_str(), Info_Bands[Band - 1].asString(FIELD_BAND));

			pBand->Set_Scaling(Scaling);

			Parameters("BANDS")->asGridList()->Add_Item(pBand);
		}
	}

	//-----------------------------------------------------
	for(int i=0; i<2; i++)
	{
		if( pBands[i] )
		{
			pBands[i]->Fmt_Name("S2_%s_%dm", Date.c_str(), i == 0 ? 10 : 20);
			pBands[i]->Get_MetaData().Add_Child(Info_General)->Set_Name("SENTINEL-2");
			pBands[i]->Set_Description(Info_General.asText());
			pBands[i]->Set_Z_Attribute (FIELD_WAVE);
			pBands[i]->Set_Z_Name_Field(FIELD_NAME);
			pBands[i]->Set_Scaling(Scaling);
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSentinel_2_Scene_Import::Load_Metadata(const CSG_String &File, CSG_MetaData &General, CSG_MetaData &Granule)
{
	CSG_MetaData	Metadata;

	if( !Metadata.Load(File)
	||  !Metadata("n1:General_Info")
	||  !Metadata["n1:General_Info"]("Product_Info")
	||  !Metadata["n1:General_Info"]["Product_Info"]("Product_Organisation")
	||  !Metadata["n1:General_Info"]["Product_Info"]["Product_Organisation"]("Granule_List")
	||  !Metadata["n1:General_Info"]["Product_Info"]["Product_Organisation"]["Granule_List"]("Granule") )
	{
		return( false );
	}

	Granule	= Metadata["n1:General_Info"]["Product_Info"]["Product_Organisation"]["Granule_List"]["Granule"];
	General	= Metadata["n1:General_Info"]["Product_Info"];

	General.Del_Child("Product_Organisation");

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Grid * CSentinel_2_Scene_Import::Load_Band(const CSG_String &Path, const CSG_MetaData &Granule)
{
	Process_Set_Text("%s: %s", _TL("loading"), Granule.Get_Content().AfterLast('_'));

	CSG_String	File	= Path + "/" + Granule.Get_Content() + ".jp2";

#ifdef _SAGA_MSW
	File.Replace("/", "\\");
#endif

	//-----------------------------------------------------
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
		CSG_Tool	*pTool	= SG_Get_Tool_Library_Manager().Create_Tool("pj_proj4", 4);	// Coordinate Transformation (Grid)

		if(	pTool )
		{
			Message_Fmt("\n%s (%s: %s)\n", _TL("re-projection to geographic coordinates"), _TL("original"), pBand->Get_Projection().Get_Name().c_str());

			pTool->Set_Manager(NULL);

			if( pTool->Set_Parameter("CRS_PROJ4" , SG_T("+proj=longlat +ellps=WGS84 +datum=WGS84"))
			&&  pTool->Set_Parameter("SOURCE"    , pBand)
			&&  pTool->Set_Parameter("RESAMPLING", Parameters("RESAMPLING"))
			&&  pTool->Set_Parameter("KEEP_TYPE" , true)
			&&  pTool->Execute() )
			{
				delete(pBand);

				pBand	= pTool->Get_Parameters()->Get_Parameter("GRID")->asGrid();
			}

			SG_Get_Tool_Library_Manager().Delete_Tool(pTool);
		}
	}

	//-----------------------------------------------------
	return( pBand );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
