
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                    imagery_tools                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 clouds_and_shadows.cpp                //
//                                                       //
//                  Copyrights (c) 2023                  //
//                  Justus Spitzmüller                   //
//                     Olaf Conrad                       //
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
#include "clouds_and_shadows.h"

//---------------------------------------------------------
#include "landsat_acca.h"

//---------------------------------------------------------
enum
{
	ID_NONE = 0, ID_CLOUD = 1, ID_CLOUD_WARM = 2, ID_SHADOW = 3
};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CDetect_Clouds::CDetect_Clouds(void)
{
	Set_Name		(_TL("Cloud Detection"));

	Set_Author		(SG_T("J.Spitzmüller, O.Conrad (c) 2023"));

	Set_Description	(_TW(
		"This tool implements pass one of the Function of mask (Fmask) algorithm "
		"for cloud and cloud shadow detection in Landsat imagery. Landsat Top of "
		"Atmosphere (TOA) reflectance and Brightness Temperature (BT) are used "
		"as input.\n"
		"Alternatively you can choose the scene-average automated cloud-cover assessment "
		"(ACCA) algorithm as proposed by Irish (2000) and Irish et al. (2006). "
	));

	Add_Reference("Zhu, Z., Woodcock, C.E.", "2012",
		"Object-based cloud and cloud shadow detection in Landsat imagery",
		"Remote Sensing of Environment 118, 83-94.",
		SG_T("https://doi.org/10.1016/j.rse.2011.10.028"), SG_T("doi:10.1016/j.rse.2011.10.028")
	);

	Add_Reference("Zhu, Z., Wang, S., Woodcock, C.E.", "2015",
		"Improvement and expansion of the Fmask algorithm: cloud, cloud shadow, and snow detection for Landsats 4-7, 8, and Sentinel 2 images",
		"Remote Sensing of Environment 159, 269-277.",
		SG_T("https://doi.org/10.1016/j.rse.2014.12.014"), SG_T("doi:10.1016/j.rse.2014.12.014")
	);

	Add_Reference("Irish, R.R.", "2000",
		"Landsat 7 Automatic Cloud Cover Assessment",
		"In: Shen, S.S., Descour, M.R. [Eds.]: Algorithms for Multispectral, Hyperspectral, and Ultraspectral Imagery VI. Proceedings of SPIE, 4049: 348-355.",
		SG_T("https://doi.org/10.1117/12.410358"), SG_T("doi:10.1117/12.410358")
	);

	Add_Reference("Irish, R.R., Barker, J.L., Goward, S.N., Arvidson, T.", "2006",
		"Characterization of the Landsat-7 ETM+ Automated Cloud-Cover Assessment (ACCA) Algorithm.",
		"Photogrammetric Engineering and Remote Sensing vol. 72(10): 1179-1188.",
		SG_T("https://doi.org/10.14358/PERS.72.10.1179"), SG_T("doi:10.14358/PERS.72.10.1179")
	);

	Add_Reference("Tizado, E.J.", "2010",
		"GRASS GIS i.landsat.acca",
		"E.J. Tizado's implementation of the ACCA algorithm as proposed by Irish (2000), Irish et al. (2006).",
		SG_T("https://github.com/OSGeo/grass/tree/main/imagery/i.landsat.acca"), SG_T("Source Code") // https://grass.osgeo.org/grass72/manuals/i.landsat.acca.html
	);

	//-----------------------------------------------------
	Parameters.Add_Grid("",
		"CLOUDS"      , _TL("Clouds"),
		_TL(""),
		PARAMETER_OUTPUT, true, SG_DATATYPE_Char
	);

//	Parameters.Add_Grid_System("", "BANDS_VNIR"   , _TL("Grid System"), _TL(""));
	Parameters.Add_Grid_System("", "BANDS_SWIR"   , _TL("Grid System"), _TL(""));
	Parameters.Add_Grid_System("", "BANDS_THERMAL", _TL("Grid System"), _TL(""));
	Parameters.Add_Grid_System("", "BANDS_CIRRUS" , _TL("Grid System"), _TL(""));

	Parameters.Add_Grid("BANDS_VNIR"   , "BAND_BLUE"   , _TL("Blue"                ), _TL(""), PARAMETER_INPUT         );
	Parameters.Add_Grid("BANDS_VNIR"   , "BAND_GREEN"  , _TL("Green"               ), _TL(""), PARAMETER_INPUT         );
	Parameters.Add_Grid("BANDS_VNIR"   , "BAND_RED"    , _TL("Red"                 ), _TL(""), PARAMETER_INPUT         );
	Parameters.Add_Grid("BANDS_VNIR"   , "BAND_NIR"    , _TL("Near Infrared"       ), _TL(""), PARAMETER_INPUT         );
	Parameters.Add_Grid("BANDS_SWIR"   , "BAND_SWIR1"  , _TL("Shortwave Infrared 1"), _TL(""), PARAMETER_INPUT         , false);
	Parameters.Add_Grid("BANDS_SWIR"   , "BAND_SWIR2"  , _TL("Shortwave Infrared 2"), _TL(""), PARAMETER_INPUT         , false);
	Parameters.Add_Grid("BANDS_THERMAL", "BAND_THERMAL", _TL("Thermal"             ), _TL(""), PARAMETER_INPUT_OPTIONAL, false);
	Parameters.Add_Grid("BANDS_CIRRUS" , "BAND_CIRRUS" , _TL("Cirrus"              ), _TL(""), PARAMETER_INPUT_OPTIONAL, false);

	Parameters.Add_Choice("BAND_THERMAL", "THERMAL_UNIT", _TL("Unit" ), _TL(""), CSG_String::Format("%s|%s", _TL("Kelvin"), _TL("Celsius")), 0);

	Parameters.Add_Bool("",
		"SHADOWS"    , _TL("Shadow Detection"),
		_TL("Run cloud shadow detection tool with standard settings."),
		false
	);

	//-----------------------------------------------------
	Parameters.Add_Choice("",
		"ALGORITHM"  , _TL("Algorithm"),
		_TL(""),
		"Fmask|ACCA", 0
	);

	Parameters.Add_Double("ALGORITHM",
		"ACCA_B56C"  , _TL("SWIR/Thermal Threshold"),
		_TL("Threshold for SWIR/Thermal Composite (step 6)."),
		225.
	);

	Parameters.Add_Double("ALGORITHM",
		"ACCA_B45R"  , _TL("Desert Detection Threshold"),
		_TL("Threshold for desert detection (step 10,  NIR/SWIR Ratio)."),
		1.
	);

	Parameters.Add_Int("ALGORITHM",
		"ACCA_HIST_N", _TL("Temperature Histogram"),
		_TL("Number of classes in the cloud temperature histogram."),
		100, 10, true
	);

	Parameters.Add_Bool("ALGORITHM",
		"ACCA_CSIG"  , _TL("Cloud Signature"),
		_TL("Always use cloud signature (step 14)."),
		true
	);

	Parameters.Add_Bool("ALGORITHM",
		"ACCA_PASS2" , _TL("Cloud Differentiation"),
		_TL("Differentiate between warm (not ambiguous) and cold clouds."),
		false
	);

	Parameters.Add_Bool("ALGORITHM",
		"ACCA_SHADOW", _TL("Shadows"),
		_TL("Include a category for cloud shadows."),
		false
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CDetect_Clouds::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	return( CSG_Tool_Grid::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
int CDetect_Clouds::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("BAND_THERMAL") )
	{
		pParameters->Set_Enabled("THERMAL_UNIT", pParameter->asGrid());
	}

	if( pParameter->Cmp_Identifier("ALGORITHM") )
	{
		pParameters->Set_Enabled("BAND_BLUE"   , pParameter->asInt() == 0);
		pParameters->Set_Enabled("BAND_SWIR2"  , pParameter->asInt() == 0);
		pParameters->Set_Enabled("BANDS_CIRRUS", pParameter->asInt() == 0);

		pParameters->Set_Enabled("ACCA_B56C"   , pParameter->asInt() == 1);
		pParameters->Set_Enabled("ACCA_B45R"   , pParameter->asInt() == 1);
		pParameters->Set_Enabled("ACCA_HIST_N" , pParameter->asInt() == 1);
		pParameters->Set_Enabled("ACCA_CSIG"   , pParameter->asInt() == 1);
		pParameters->Set_Enabled("ACCA_PASS2"  , pParameter->asInt() == 1);
	}

	pParameters->Set_Enabled("ACCA_SHADOW" ,
		(*pParameters)("ALGORITHM")->asInt () == 1
	 && (*pParameters)("SHADOWS"  )->asBool() == false
	);

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CDetect_Clouds::On_Execute(void)
{
	m_pBand[0] = Parameters("BAND_BLUE"   )->asGrid();
	m_pBand[1] = Parameters("BAND_GREEN"  )->asGrid();
	m_pBand[2] = Parameters("BAND_RED"    )->asGrid();
	m_pBand[3] = Parameters("BAND_NIR"    )->asGrid();
	m_pBand[4] = Parameters("BAND_SWIR1"  )->asGrid();
	m_pBand[5] = Parameters("BAND_SWIR2"  )->asGrid();
	m_pBand[6] = Parameters("BAND_THERMAL")->asGrid();
	m_pBand[7] = Parameters("BAND_CIRRUS" )->asGrid();

	m_bCelsius = Parameters("THERMAL_UNIT")->asInt() == 1;

	int Algorithm = Parameters("ALGORITHM")->asInt();

	//-----------------------------------------------------
	CSG_Grid *pClouds = Parameters("CLOUDS")->asGrid();

	CSG_Parameter *pLUT = DataObject_Get_Parameter(pClouds, "LUT");

	if( pLUT && pLUT->asTable() )
	{
		pLUT->asTable()->Del_Records();

		#define LUT_ADD_CLASS(id, name, color) { CSG_Table_Record &r = *pLUT->asTable()->Add_Record(); r.Set_Value(0, color); r.Set_Value(1, name); r.Set_Value(3, id); r.Set_Value(3, id); }

		switch( Algorithm )
		{
		default:
			LUT_ADD_CLASS(ID_CLOUD, _TL("Cloud"), SG_COLOR_BLUE_LIGHT);

			if( Parameters("SHADOWS")->asBool() )
			{
				LUT_ADD_CLASS(ID_SHADOW, _TL("Shadow"), SG_COLOR_RED);
			}
			break;

		case  1:
			if( Parameters("ACCA_PASS2")->asBool() )
			{
				LUT_ADD_CLASS(ID_CLOUD     , _TL("Cold Cloud"), SG_COLOR_BLUE_DARK);
				LUT_ADD_CLASS(ID_CLOUD_WARM, _TL("Warm Cloud"), SG_COLOR_BLUE_LIGHT);
			}
			else
			{
				LUT_ADD_CLASS(ID_CLOUD     , _TL("Cloud"     ), SG_COLOR_BLUE_LIGHT);
			}

			if( Parameters("ACCA_SHADOW")->asBool() || Parameters("SHADOWS")->asBool() )
			{
				LUT_ADD_CLASS(ID_SHADOW    , _TL("Shadow"    ), SG_COLOR_RED);
			}
			break;
		}

		DataObject_Set_Parameter(pClouds, pLUT);
		DataObject_Set_Parameter(pClouds, "COLORS_TYPE", 1); // Color Classification Type: Lookup Table
	}

	pClouds->Set_NoData_Value(ID_NONE);

	for(int i=0; i<8; i++)
	{
		if( m_pBand[i] )
		{
			CSG_MetaData &MD = m_pBand[i]->Get_Owner() ? m_pBand[i]->Get_Owner()->Get_MetaData() : m_pBand[i]->Get_MetaData();

			if( MD("LANDSAT") )
			{
				pClouds->Get_MetaData().Del_Child("LANDSAT");
				pClouds->Get_MetaData().Add_Child("LANDSAT")->Add_Children(MD["LANDSAT"]);
				break;
			}
		}
	}

	//-----------------------------------------------------
	bool bResult;

	switch( Algorithm )
	{
	default: bResult = Set_Fmask(pClouds); break;
	case  1: bResult = Set_ACCA (pClouds); break;
	}

	//-----------------------------------------------------
	if( bResult && Parameters("SHADOWS")->asBool() )
	{
		CDetect_CloudShadows Tool; CSG_Grid Shadows(Get_System(), SG_DATATYPE_Char);

		Tool.Set_Manager(NULL);
		Tool.Set_Parameter("CLOUDS" , pClouds);
		Tool.Set_Parameter("SHADOWS", &Shadows);
		Tool.Set_Parameter("OUTPUT"    , 1); // full shadow
		Tool.Set_Parameter("PROCESSING", 0); // all clouds at once
		Tool.Set_Parameter("CANDIDATES", 2); // average brightness threshold
		Tool.Set_Parameter("BRIGHTNESS", 0.05);
		Tool.Get_Parameter("BANDS_BRIGHTNESS")->asList()->Add_Item(m_pBand[1]);
		Tool.Get_Parameter("BANDS_BRIGHTNESS")->asList()->Add_Item(m_pBand[2]);
		Tool.Get_Parameter("BANDS_BRIGHTNESS")->asList()->Add_Item(m_pBand[3]);
		Tool.Get_Parameter("BANDS_BRIGHTNESS")->asList()->Add_Item(m_pBand[4]);
		Tool.Get_Parameter("BANDS_BRIGHTNESS")->asList()->Add_Item(m_pBand[5]);

		if( (bResult = Tool.Execute()) == true )
		{
			#pragma omp parallel for
			for(sLong i=0; i<Get_NCells(); i++)
			{
				if( Shadows.asInt(i) && !pClouds->asInt(i) )
				{
					pClouds->Set_Value(i, CACCA::IS_SHADOW);
				}
			}
		}
	}

	//-----------------------------------------------------
	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline bool CDetect_Clouds::Get_Brightness(int x, int y, double &b, double &g, double &r, double &nir, double &swir1, double &swir2, double &tir, double &cirr)
{
	if( m_pBand[0] ) { if( m_pBand[0]->is_NoData(x, y) ) { return( false ); } b   = m_pBand[0]->asDouble(x, y); } else { b   = -1.; }
	if( m_pBand[1] ) { if( m_pBand[1]->is_NoData(x, y) ) { return( false ); } g   = m_pBand[1]->asDouble(x, y); } else { g   = -1.; }
	if( m_pBand[2] ) { if( m_pBand[2]->is_NoData(x, y) ) { return( false ); } r   = m_pBand[2]->asDouble(x, y); } else { r   = -1.; }
	if( m_pBand[3] ) { if( m_pBand[3]->is_NoData(x, y) ) { return( false ); } nir = m_pBand[3]->asDouble(x, y); } else { nir = -1.; }

	CSG_Point p(Get_System().Get_Grid_to_World(x, y));

	if( m_pBand[4] ) { if( !m_pBand[4]->Get_Value(p, swir1) ) { return( false ); } } else { swir1 = -1.; }
	if( m_pBand[5] ) { if( !m_pBand[5]->Get_Value(p, swir2) ) { return( false ); } } else { swir2 = -1.; }
	if( m_pBand[6] ) { if( !m_pBand[6]->Get_Value(p, tir  ) ) { return( false ); } } else { tir   = -1.; } if( m_bCelsius && m_pBand[6] ) { tir += 273.15; }
	if( m_pBand[7] ) { if( !m_pBand[7]->Get_Value(p, cirr ) ) { return( false ); } } else { cirr  = -1.; }

	return( true );
}

//---------------------------------------------------------
inline int CDetect_Clouds::Get_Fmask(int x, int y)
{
	double b, g, r, nir, swir1, swir2, tir, cirr;

	if( !Get_Brightness(x, y, b, g, r, nir, swir1, swir2, tir, cirr) )
	{
		return( ID_NONE );
	}

	if( cirr > 0.01 ) // cirrus
	{
		return( ID_CLOUD );
	}

	if( (nir - r) / (nir + r) >= 0.8 ) // ndvi
	{
		return( ID_NONE );
	}

	if( (g - swir1) / (g + swir1) >= 0.8 ) // ndwi
	{
		return( ID_NONE );
	}

	if( swir2 <= 0.03 )
	{
		return( ID_NONE );
	}

	if( b - 0.5 * r - 0.08 <= 0. )
	{
		return( ID_NONE );
	}

	if( nir / swir1 <= 0.75 )
	{
		return( ID_NONE );
	}

	double m = (b + g + r) / 3.; // visible mean

	if( (fabs(b - m) + fabs(g - m) + fabs(r - m)) / m >= 0.7 )
	{
		return( ID_NONE );
	}

	if( tir >= 0. && tir >= (273.15 + 27) ) // thermal
	{
		return( ID_NONE );
	}

	return( ID_CLOUD );
}

//---------------------------------------------------------
bool CDetect_Clouds::Set_Fmask(CSG_Grid *pClouds)
{
	for(int y=0; y<Get_NY() && Set_Progress_Rows(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			pClouds->Set_Value(x, y, Get_Fmask(x, y));
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CDetect_Clouds::Set_ACCA(CSG_Grid *pClouds)
{
	if( !Parameters("BAND_THERMAL")->asGrid() ) // thermal ir has been defined optional for Fmask
	{
		Error_Fmt("%s\n\n%s", _TL("Please provide a thermal infrared channel!"), _TL("Temperature information is required by the ACCA algorithm."));

		return( false );
	}

	CSG_Grid *pBands[5] = {
		Parameters("BAND_GREEN"  )->asGrid(),
		Parameters("BAND_RED"    )->asGrid(),
		Parameters("BAND_NIR"    )->asGrid(),
		Parameters("BAND_SWIR1"  )->asGrid(),
		Parameters("BAND_THERMAL")->asGrid()
	};

	CACCA ACCA;

	ACCA.m_bCelsius = Parameters("THERMAL_UNIT")->asInt() == 1;

	ACCA.acca_algorithm(pClouds, pBands,
		Parameters("ACCA_PASS2" )->asBool() ? 0 : 1,
		Parameters("ACCA_SHADOW")->asBool() && !Parameters("SHADOWS")->asBool() ? 1 : 0,
		Parameters("ACCA_CSIG"  )->asBool() ? 1 : 0,
		Parameters("ACCA_HIST_N")->asInt()
	);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
enum
{
	INFO_ID = 0, INFO_CELLS, INFO_AREA, INFO_HEIGHT, INFO_DISTANCE
};

//---------------------------------------------------------
CDetect_CloudShadows::CDetect_CloudShadows(void)
{
	Set_Name		(_TL("Cloud Shadow Detection"));

	Set_Author		(SG_T("J.Spitzmüller, O.Conrad (c) 2023"));

	Set_Description	(_TW(
		""
	));

	//-----------------------------------------------------
	Parameters.Add_Grid_System("", "BANDS_SWIR"   , _TL("Grid System"), _TL(""));
	Parameters.Add_Grid_System("", "BANDS_THERMAL", _TL("Grid System"), _TL(""));

	Parameters.Add_Grid ("", "CLOUDS"       , _TL("Clouds"      ), _TL(""), PARAMETER_INPUT);
	Parameters.Add_Grid ("", "CAND_GRID_IN" , _TL("Candidates"  ), _TL(""), PARAMETER_INPUT);
	Parameters.Add_Grid ("", "DEM"          , _TL("Elevation"   ), _TL(""), PARAMETER_INPUT_OPTIONAL , true);
	Parameters.Add_Grid ("", "CLOUD_ID"     , _TL("Cloud Number"), _TL(""), PARAMETER_OUTPUT_OPTIONAL, true, SG_DATATYPE_Short);
	Parameters.Add_Grid ("", "CAND_GRID_OUT", _TL("Candidates"  ), _TL(""), PARAMETER_OUTPUT_OPTIONAL, true, SG_DATATYPE_Char );
	Parameters.Add_Grid ("", "SHADOWS"      , _TL("Shadows"     ), _TL(""), PARAMETER_OUTPUT         , true, SG_DATATYPE_Char );
	Parameters.Add_Table("", "CLOUD_INFO"   , _TL("Cloud Info"  ), _TL(""), PARAMETER_OUTPUT);

	Parameters.Add_Grid ("BANDS_VNIR"   , "BAND_GREEN"  , _TL("Green"             ), _TL(""), PARAMETER_INPUT                );
	Parameters.Add_Grid ("BANDS_VNIR"   , "BAND_RED"    , _TL("Red"               ), _TL(""), PARAMETER_INPUT                );
	Parameters.Add_Grid ("BANDS_VNIR"   , "BAND_NIR"    , _TL("Near Infrared"     ), _TL(""), PARAMETER_INPUT                );
	Parameters.Add_Grid ("BANDS_SWIR"   , "BAND_SWIR"   , _TL("Shortwave Infrared"), _TL(""), PARAMETER_INPUT         , false);
	Parameters.Add_Grid ("BANDS_THERMAL", "BAND_THERMAL", _TL("Thermal"           ), _TL(""), PARAMETER_INPUT_OPTIONAL, false);

	Parameters.Add_Grid_List("", "BANDS_BRIGHTNESS", _TL("Brightness Bands"), _TL(""), PARAMETER_INPUT, false);

	Parameters.Add_Choice("SHADOWS"     , "OUTPUT"         , _TL("Output"), _TL(""), CSG_String::Format("%s|%s|%s", _TL("visible shadow"), _TL("full shadow"), _TL("shadow and clouds")), 2);

	Parameters.Add_Double("CAND_GRID_IN", "CAND_GRID_VALUE", _TL("Value" ), _TL(""));
	Parameters.Add_Choice("BAND_THERMAL", "THERMAL_UNIT"   , _TL("Unit"  ), _TL(""), CSG_String::Format("%s|%s", _TL("Kelvin"), _TL("Celsius")), 0);

	//-----------------------------------------------------
	Parameters.Add_Choice("",
		"CANDIDATES"  , _TL("Candidates"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s|%s",
			_TL("all cells of candidates grid that are not no-data"),
			_TL("all cells of candidates grid with a specified value"),
			_TL("average brightness threshold"),
			_TL("Irish"),
			_TL("Irish modified by Tizado")
		), 0
	);

	Parameters.Add_Double("BANDS_BRIGHTNESS",
		"BRIGHTNESS"  , _TL("Brightness Threshold"),
		_TL(""),
		0.05, 0., true,  1., true
	);

	//-----------------------------------------------------
	Parameters.Add_Choice("",
		"PROCESSING"  , _TL("Processing"),
		_TL(""),
		CSG_String::Format("%s|%s",
			_TL("all clouds at once"),
			_TL("cloud by cloud")
		), 0
	);

	Parameters.Add_Node("", "SUN_POSITION", _TL("Solar Position"), _TL(""));

	Parameters.Add_Double("SUN_POSITION",
		"SUN_AZIMUTH" , _TL("Azimuth"),
		_TL("Direction of sun clockwise from North [degree]."),
		-180., 0., true, 360., true
	);

	Parameters.Add_Double("SUN_POSITION",
		"SUN_HEIGHT"  , _TL("Height"),
		_TL("Height of sun above horizon [degree]."),
		 45., 0., true,  90., true
	);

	Parameters.Add_Range("",
		"CLOUD_HEIGHT", _TL("Cloud Height"),
		_TL("The range of cloud height above ground to be tested."),
		100., 5000., 0., true, 12000., true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CDetect_CloudShadows::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->is_Input() && pParameter->asGrid() )
	{
		CSG_MetaData &MD = pParameter->asGrid()->Get_Owner()
			? pParameter->asGrid()->Get_Owner()->Get_MetaData()
			: pParameter->asGrid()             ->Get_MetaData();

		if( MD("LANDSAT") )
		{
			double Azimuth, Height;

			if( MD["LANDSAT"].Get_Content("SUN_AZIMUTH"  , Azimuth)
			&&  MD["LANDSAT"].Get_Content("SUN_ELEVATION", Height ) )
			{
				pParameters->Set_Parameter("SUN_AZIMUTH", Azimuth);
				pParameters->Set_Parameter("SUN_HEIGHT" , Height );
			}
		}
	}

	return( CSG_Tool_Grid::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
int CDetect_CloudShadows::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("CANDIDATES") )
	{
		int m = pParameter->asInt();

		pParameters->Set_Enabled("CAND_GRID_IN"    , m <= 1);
		pParameters->Set_Enabled("CAND_GRID_VALUE" , m == 1);
		pParameters->Set_Enabled("CAND_GRID_OUT"   , m >= 2);

		pParameters->Set_Enabled("BAND_GREEN"      ,           m == 4);
		pParameters->Set_Enabled("BAND_RED"        , m == 3 || m == 4);
		pParameters->Set_Enabled("BAND_NIR"        , m == 3 || m == 4);
		pParameters->Set_Enabled("BANDS_SWIR"      ,           m == 4);
		pParameters->Set_Enabled("BANDS_THERMAL"   , m == 3 || m == 4);

		pParameters->Set_Enabled("BRIGHTNESS"      , m == 2);
		pParameters->Set_Enabled("BANDS_BRIGHTNESS", m == 2);
	}

	if( pParameter->Cmp_Identifier("BAND_THERMAL") )
	{
		pParameters->Set_Enabled("THERMAL_UNIT", pParameter->asGrid());
	}

	if( pParameter->Cmp_Identifier("PROCESSING") )
	{
		pParameters->Set_Enabled("CLOUD_ID"  , pParameter->asInt() == 1); // cloud-by-cloud
	}

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CDetect_CloudShadows::On_Execute(void)
{
	CSG_Grid *pShadows = Get_Target();

	//-----------------------------------------------------
	CSG_Grid Candidates;

	if( !Get_Candidates(Candidates) )
	{
		return( false );
	}

	//-----------------------------------------------------
	m_pInfo = Parameters("CLOUD_INFO")->asTable();

	m_pInfo->Destroy();
	m_pInfo->Set_Name(_TL("Cloud Info"));
	m_pInfo->Add_Field("ID"      , SG_DATATYPE_Int   ); // INFO_ID
	m_pInfo->Add_Field("CELLS"   , SG_DATATYPE_Int   ); // INFO_CELLS
	m_pInfo->Add_Field("AREA"    , SG_DATATYPE_Double); // INFO_AREA
	m_pInfo->Add_Field("HEIGHT"  , SG_DATATYPE_Double); // INFO_HEIGHT
	m_pInfo->Add_Field("DISTANCE", SG_DATATYPE_Double); // INFO_DISTANCE

	//-----------------------------------------------------
	double zRange[2] = {
		Parameters("CLOUD_HEIGHT.MIN")->asDouble(),
		Parameters("CLOUD_HEIGHT.MAX")->asDouble()
	};

	double dx = sin(Parameters("SUN_AZIMUTH")->asDouble() * M_DEG_TO_RAD + M_PI_180);
	double dy = cos(Parameters("SUN_AZIMUTH")->asDouble() * M_DEG_TO_RAD + M_PI_180);
	double dz = tan(Parameters("SUN_HEIGHT" )->asDouble() * M_DEG_TO_RAD);

	Get_Correction(dx, dy, dz);

	CSG_Grid_Stack Cloud; CSG_Grid *pClouds = Parameters("CLOUDS")->asGrid();

	//-----------------------------------------------------
	if( Parameters("PROCESSING")->asInt() == 0 ) // all clouds at once
	{
		if( Get_Cloud(Cloud, pClouds) )
		{
			if( !Find_Shadow(pShadows, Candidates, Cloud, zRange, dx, dy, dz) )
			{
			}
		}
	}

	//-----------------------------------------------------
	else // cloud by cloud
	{
		CSG_Grid *pCloud_ID = Parameters("CLOUD_ID")->asGrid();

		if( pCloud_ID )
		{
			pCloud_ID->Assign();
			pCloud_ID->Set_NoData_Value(0);
		}

		CSG_Grid Clouds(*pClouds); // copy construct, we will remove processed clouds from input grid!

		//-------------------------------------------------
		for(int y=0; y<Get_NY() && Set_Progress_Rows(y); y++)
		{
			int yy = dy > 0. ? y : Get_NY() - 1 - y;

			for(int x=0; x<Get_NX(); x++)
			{
				int xx = dx > 0. ? x : Get_NX() - 1 - x;

				if( Get_Cloud(Cloud, &Clouds, x, y, pCloud_ID) )
				{
					SG_UI_Progress_Lock(true);

					if( !Find_Shadow(pShadows, Candidates, Cloud, zRange, dx, dy, dz) )
					{
					}

					SG_UI_Progress_Lock(false);
				}
			}
		}
	}

	//-----------------------------------------------------
	m_Correction.Destroy();

	if( Parameters("OUTPUT")->asInt() != 1 ) // full shadow has already been set
	{
		int Method = Parameters("OUTPUT")->asInt();

		#pragma omp parallel for
		for(sLong i=0; i<Get_NCells(); i++)
		{
			if( pClouds->is_NoData(i) == false )
			{
				if( Method == 0 ) // visible shadow (not covered by cloud)
				{
					pShadows->Set_NoData(i);
				}
				else
				{
					pShadows->Set_Value(i, 2);
				}
			}
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Grid * CDetect_CloudShadows::Get_Target(void)
{
	CSG_Grid *pShadows = Parameters("SHADOWS")->asGrid();

	CSG_Parameter *pLUT = DataObject_Get_Parameter(pShadows, "LUT");

	if( pLUT && pLUT->asTable() )
	{
		pLUT->asTable()->Del_Records();

		#define LUT_ADD_CLASS(id, name, color) { CSG_Table_Record &r = *pLUT->asTable()->Add_Record(); r.Set_Value(0, color); r.Set_Value(1, name); r.Set_Value(3, id); r.Set_Value(3, id); }

		LUT_ADD_CLASS(1, _TL("Shadow"), SG_COLOR_RED);
		LUT_ADD_CLASS(2, _TL("Cloud" ), SG_COLOR_BLUE_LIGHT);

		DataObject_Set_Parameter(pShadows, pLUT);
		DataObject_Set_Parameter(pShadows, "COLORS_TYPE", 1); // Color Classification Type: Lookup Table
	}

	pShadows->Set_Name(_TL("Cloud Shadows"));
	pShadows->Set_NoData_Value(0);
	pShadows->Assign(0.);

	return( pShadows );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CDetect_CloudShadows::Get_Candidates(CSG_Grid &Candidates)
{
	CSG_Grid   *pGrid = Parameters("CAND_GRID_IN"   )->asGrid();
	CSG_Grid      *pG = Parameters("BAND_GREEN"     )->asGrid();
	CSG_Grid      *pR = Parameters("BAND_RED"       )->asGrid();
	CSG_Grid    *pNIR = Parameters("BAND_NIR"       )->asGrid();
	CSG_Grid   *pSWIR = Parameters("BAND_SWIR"      )->asGrid();
	CSG_Grid     *pBT = Parameters("BAND_THERMAL"   )->asGrid();

	int        Method = Parameters("CANDIDATES"     )->asInt();
	bool      bKelvin = Parameters("THERMAL_UNIT"   )->asInt() == 0;
	double      Value = Parameters("CAND_GRID_VALUE")->asDouble();
	double Brightness = Parameters("BRIGHTNESS"     )->asDouble();

	CSG_Parameter_Grid_List *pBands = Parameters("BANDS_BRIGHTNESS")->asGridList();

	//-----------------------------------------------------
	CSG_Grid *pCandidates = Method ? Parameters("CAND_GRID_OUT")->asGrid() : NULL;

	if( pCandidates )
	{
		DataObject_Set_Parameter(pCandidates, "COLORS_TYPE" , 0); // Color Classification Type: Single Symbol
		DataObject_Set_Parameter(pCandidates, "SINGLE_COLOR", (int)SG_COLOR_RED);

		pCandidates->Set_NoData_Value(0);
		pCandidates->Set_Name(_TL("Shadow Candidates"));
	}

	//-----------------------------------------------------
	Candidates.Create(Get_System(), SG_DATATYPE_Char);
	Candidates.Set_NoData_Value(0);

	sLong nCandidates = 0;

	for(int y=0; y<Get_NY() && Set_Progress_Rows(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			CSG_Point p(Get_XMin() + x * Get_Cellsize(), Get_YMin() + y * Get_Cellsize());

			double G, R, NIR, SWIR, BT; bool bCandidate = false;

			switch( Method )
			{
			case  0:
				bCandidate = pGrid->is_NoData(x, y) == false;
				break;

			case  1:
				bCandidate = pGrid->asDouble(x, y) == Value;
				break;

			case  2: { CSG_Simple_Statistics s; for(int i=0; i<pBands->Get_Grid_Count(); i++) { double d; if( pBands->Get_Grid(i)->Get_Value(p, d) ) { s += d; } }
				bCandidate = s.Get_Count() > 0 && s.Get_Mean()  < Brightness;
				break; }

			case  3: if( pG->Get_Value(p, G) && pR->Get_Value(p, R) && pNIR->Get_Value(p, NIR) && pBT->Get_Value(p, BT) )
				bCandidate = R < 0.07 && (1. - NIR) * (bKelvin ? BT : BT + 273.15) > 240. && NIR / G > 1.;
				break;

			case  4: if( pG->Get_Value(p, G) && pR->Get_Value(p, R) && pNIR->Get_Value(p, NIR) && pSWIR->Get_Value(p, SWIR) && pBT->Get_Value(p, BT) )
				bCandidate = R < 0.07 && (1. - NIR) * (bKelvin ? BT : BT + 273.15) > 240. && NIR / G > 1. && (R - SWIR) / (R + SWIR) < 0.1;
				break;
			}

			Candidates.Set_Value(x, y, bCandidate ? 1 : 0);

			if( bCandidate )
			{
				nCandidates++;
			}

			if( pCandidates )
			{
				pCandidates->Set_Value(x, y, bCandidate ? 1 : 0);
			}
		}
	}

	return( nCandidates > 0 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double CDetect_CloudShadows::Get_Correction(double dx, double dy, double dz, CSG_Grid *pDEM, int x, int y)
{
	TSG_Grid_Resampling Resampling = GRID_RESAMPLING_BSpline; // GRID_RESAMPLING_NearestNeighbour

	CSG_Point p0(Get_System().Get_Grid_to_World(x, y)); double z;

	if( pDEM->Get_Value(p0.x, p0.y, z, Resampling) )
	{
		double Tolerance = 0.5 * dz, zRef = pDEM->Get_Mean(); // reference ground height

		if( z < (zRef - Tolerance) )
		{
			CSG_Point_3D p(p0.x, p0.y, zRef - Tolerance);

			do
			{
				p.x += dx; p.y += dy; p.z -= dz;
			}
			while( pDEM->Get_Value(p.x, p.y, z, Resampling) && z < p.z );

			return(  SG_Get_Distance(p0.x, p0.y, p.x, p.y) / Get_Cellsize() );
		}
		else if( z > (zRef + Tolerance) )
		{
			CSG_Point_3D p(p0.x, p0.y, zRef + Tolerance);

			do
			{
				p.x -= dx; p.y -= dy; p.z += dz;
			}
			while( pDEM->Get_Value(p.x, p.y, z, Resampling) && z > p.z );

			return( -SG_Get_Distance(p0.x, p0.y, p.x, p.y) / Get_Cellsize() );
		}
	}

	return( 0. );
}

//---------------------------------------------------------
bool CDetect_CloudShadows::Get_Correction(double dx, double dy, double dz)
{
	CSG_Grid *pDEM = Parameters("DEM")->asGrid();

	if( pDEM == NULL )
	{
		return( false );
	}

	m_Correction.Create(Get_System());

	dx *= Get_Cellsize();
	dy *= Get_Cellsize();
	dz *= Get_Cellsize();

	for(int y=0; y<Get_NY() && Set_Progress_Rows(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			m_Correction.Set_Value(x, y, Get_Correction(dx, dy, dz, pDEM, x, y));
		}
	}

	#ifdef _DEBUG
	m_Correction.Set_Name("Topographic Correction"); DataObject_Add(SG_Create_Grid(m_Correction));
	#endif

	return( true );
}

//---------------------------------------------------------
inline bool CDetect_CloudShadows::Get_Correction(double dx, double dy, int &x, int &y)
{
	if( m_Correction.is_Valid() && is_InGrid(x, y) )
	{
		double d = m_Correction.asDouble(x, y);

		if( d != 0. )
		{
			x = (int)(x + dx * d);
			y = (int)(y + dy * d);
		}
	}

	return( is_InGrid(x, y) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
// all clouds at once
bool CDetect_CloudShadows::Get_Cloud(CSG_Grid_Stack &Cloud, const CSG_Grid *pClouds)
{
	Cloud.Destroy();

	for(int y=0; y<Get_NY() && Set_Progress_Rows(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			if( pClouds->is_NoData(x, y) == false )
			{
				Cloud.Push(x, y);
			}
		}
	}

	CSG_Table_Record &Info = *m_pInfo->Add_Record();

	Info.Set_Value(INFO_ID   , m_pInfo->Get_Count());
	Info.Set_Value(INFO_CELLS, Cloud.Get_Size());
	Info.Set_Value(INFO_AREA , Cloud.Get_Size() *  Get_Cellarea());

	return( Cloud.Get_Size() > 0 );
}

//---------------------------------------------------------
// cloud by cloud
bool CDetect_CloudShadows::Get_Cloud(CSG_Grid_Stack &Cloud, CSG_Grid *pClouds, int x, int y, CSG_Grid *pID)
{
	if( !pClouds->is_NoData(x, y) )
	{
		CSG_Table_Record &Info = *m_pInfo->Add_Record();

		Cloud.Destroy();

		CSG_Grid_Stack Stack;

		Stack.Push(x, y); Cloud.Push(x, y); pClouds->Set_NoData(x, y); if( pID ) { pID->Set_Value(x, y, (int)m_pInfo->Get_Count()); }

		while( Stack.Get_Size() > 0 && Process_Get_Okay() )
		{
			Stack.Pop(x, y);

			for(int i=0; i<8; i+=2)
			{
				int ix = Get_xTo(i, x), iy = Get_yTo(i, y);

				if(	pClouds->is_InGrid(ix, iy) )
				{
					Stack.Push(ix, iy); Cloud.Push(ix, iy); pClouds->Set_NoData(ix, iy); if( pID ) { pID->Set_Value(ix, iy, (int)m_pInfo->Get_Count()); }
				}
			}
		}

		Info.Set_Value(INFO_ID   , m_pInfo->Get_Count());
		Info.Set_Value(INFO_CELLS, Cloud.Get_Size());
		Info.Set_Value(INFO_AREA , Cloud.Get_Size() *  Get_Cellarea());

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CDetect_CloudShadows::Find_Shadow(CSG_Grid *pShadows, CSG_Grid &Candidates, const CSG_Grid_Stack &Cloud, double zRange[2], double dx, double dy, double dz)
{
	double zMax = 0.; int nMax = 0;

	dz *= Get_Cellsize();

	//-----------------------------------------------------
	#define SHADOW_PARALLEL
	#ifdef SHADOW_PARALLEL
	int nz = (int)((zRange[1] - zRange[0]) / dz);
	#pragma omp parallel for
	for(int iz=0; iz<=nz; iz++)
	{
		double z = zRange[0] + iz * dz; if( SG_OMP_Get_Thread_Num() == 0 ) { Set_Progress(SG_OMP_Get_Max_Num_Threads() * iz, nz); }
	#else
	for(double z=zRange[0]; z<=zRange[1] && Set_Progress(z - zRange[0], zRange[1] - zRange[0]); z+=dz)
	{
	#endif
		int n = 0; double ax = dx * z / dz, ay = dy * z / dz;

		for(size_t i=0; i<Cloud.Get_Size(); i++)
		{
			int x = (int)(ax + Cloud[i].x), y = (int)(ay + Cloud[i].y);

			if( Get_Correction(dx, dy, x, y) && Candidates.is_InGrid(x, y) )
			{
				n++;
			}
		}

		if( n > nMax )
		{
			#pragma omp critical
			if( n > nMax )
			{
				nMax = n; zMax = z;
			}
		}
	}

	//-----------------------------------------------------
	if( nMax > 0 && Process_Get_Okay() )
	{
		double ax = dx * zMax / dz, ay = dy * zMax / dz;

		for(size_t i=0; i<Cloud.Get_Size(); i++)
		{
			int x = (int)(ax + Cloud[i].x), y = (int)(ay + Cloud[i].y);

			if( Get_Correction(dx, dy, x, y) )
			{
				pShadows->Set_Value(x, y, 1); Candidates.Set_Value(x, y, 0); // set shadow and remove candidate!
			}
		}

		m_pInfo->Get_Record(m_pInfo->Get_Count() - 1)->Set_Value(INFO_HEIGHT  , zMax);
		m_pInfo->Get_Record(m_pInfo->Get_Count() - 1)->Set_Value(INFO_DISTANCE, zMax * Get_Cellsize() / dz);

		return( true );
	}

	m_pInfo->Get_Record(m_pInfo->Get_Count() - 1)->Set_NoData(INFO_HEIGHT  );
	m_pInfo->Get_Record(m_pInfo->Get_Count() - 1)->Set_NoData(INFO_DISTANCE);

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
