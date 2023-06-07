
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
		_TL("Differentiate between warm (not ambiguous) and cold clouds.."),
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
		pParameters->Set_Enabled("ACCA_SHADOW" , pParameter->asInt() == 1);
	}

	if( pParameter->Cmp_Identifier("BAND_THERMAL") )
	{
		pParameters->Set_Enabled("THERMAL_UNIT", pParameter->asGrid());
	}

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
			LUT_ADD_CLASS(1, _TL("Cloud"), SG_COLOR_BLUE_LIGHT);
			break;

		case  1:
			if( Parameters("ACCA_PASS2")->asBool() )
			{
				LUT_ADD_CLASS(CACCA::IS_COLD_CLOUD, _TL("Cold Cloud"), SG_COLOR_BLUE_DARK);
				LUT_ADD_CLASS(CACCA::IS_WARM_CLOUD, _TL("Warm Cloud"), SG_COLOR_BLUE_LIGHT);
			}
			else
			{
				LUT_ADD_CLASS(CACCA::IS_COLD_CLOUD, _TL("Cloud"     ), SG_COLOR_BLUE_LIGHT);
			}

			if( Parameters("ACCA_SHADOW")->asBool() )
			{
				LUT_ADD_CLASS(CACCA::IS_SHADOW    , _TL("Shadow"    ), SG_COLOR_RED);
			}
			break;
		}

		DataObject_Set_Parameter(pClouds, pLUT);
		DataObject_Set_Parameter(pClouds, "COLORS_TYPE", 1); // Color Classification Type: Lookup Table
	}

	pClouds->Set_NoData_Value(0.);

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
	switch( Algorithm )
	{
	default: return( Set_Fmask(pClouds) );
	case  1: return( Set_ACCA (pClouds) );
	}
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
		return( 0 );
	}

	if( cirr >= 0. && cirr > 0.01 ) // cirrus
	{
		return( 1 );
	}

	if( (nir - r) / (nir + r) >= 0.8 ) // ndvi
	{
		return( 0 );
	}

	if( (g - swir1) / (g + swir1) >= 0.8 ) // ndwi
	{
		return( 0 );
	}

	if( swir2 <= 0.03 )
	{
		return( 0 );
	}

	if( b - 0.5 * r - 0.08 <= 0. )
	{
		return( 0 );
	}

	if( nir / swir1 <= 0.75 )
	{
		return( 0 );
	}

	double m = (b + g + r) / 3.; // visible mean

	if( (fabs(b - m) + fabs(g - m) + fabs(r - m)) / m >= 0.7 )
	{
		return( 0 );
	}

	if( tir >= 0. && tir >= (273.15 + 27) ) // thermal
	{
		return( 0 );
	}

	return( 1 );
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
		Parameters("ACCA_SHADOW")->asBool() ? 1 : 0,
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

	Parameters.Add_Grid("", "CLOUDS"       , _TL("Clouds"    ), _TL(""), PARAMETER_INPUT);
	Parameters.Add_Grid("", "CAND_GRID_IN" , _TL("Candidates"), _TL(""), PARAMETER_INPUT);
	Parameters.Add_Grid("", "DEM"          , _TL("Elevation" ), _TL(""), PARAMETER_INPUT_OPTIONAL , true);
	Parameters.Add_Grid("", "SHADOWS"      , _TL("Shadows"   ), _TL(""), PARAMETER_OUTPUT         , true, SG_DATATYPE_Char);
	Parameters.Add_Grid("", "CAND_GRID_OUT", _TL("Candidates"), _TL(""), PARAMETER_OUTPUT_OPTIONAL, true, SG_DATATYPE_Char);

	Parameters.Add_Grid("BANDS_VNIR"   , "BAND_GREEN"  , _TL("Green"             ), _TL(""), PARAMETER_INPUT                );
	Parameters.Add_Grid("BANDS_VNIR"   , "BAND_RED"    , _TL("Red"               ), _TL(""), PARAMETER_INPUT                );
	Parameters.Add_Grid("BANDS_VNIR"   , "BAND_NIR"    , _TL("Near Infrared"     ), _TL(""), PARAMETER_INPUT                );
	Parameters.Add_Grid("BANDS_SWIR"   , "BAND_SWIR"   , _TL("Shortwave Infrared"), _TL(""), PARAMETER_INPUT         , false);
	Parameters.Add_Grid("BANDS_THERMAL", "BAND_THERMAL", _TL("Thermal"           ), _TL(""), PARAMETER_INPUT_OPTIONAL, false);

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
	double zRange[2] = {
		Parameters("CLOUD_HEIGHT.MIN")->asDouble(),
		Parameters("CLOUD_HEIGHT.MAX")->asDouble()
	};

	double dx = sin(Parameters("SUN_AZIMUTH")->asDouble() * M_DEG_TO_RAD + M_PI_180);
	double dy = cos(Parameters("SUN_AZIMUTH")->asDouble() * M_DEG_TO_RAD + M_PI_180);
	double dz = tan(Parameters("SUN_HEIGHT" )->asDouble() * M_DEG_TO_RAD);

	//-----------------------------------------------------
	CSG_Grid_Stack Cloud; CSG_Grid *pClouds = Parameters("CLOUDS")->asGrid();

	if( Parameters("PROCESSING")->asInt() == 0 ) // all clouds at once
	{
		if( !Get_Cloud(Cloud, pClouds) || !Find_Shadow(pShadows, Candidates, Cloud, zRange, dx, dy, dz) )
		{
		}
	}
	else // cloud by cloud
	{
		CSG_Grid Clouds(*pClouds); // copy construct

		for(int y=0; y<Get_NY() && Set_Progress_Rows(y); y++)
		{
			int yy = dy > 0. ? y : Get_NY() - 1 - y;

			for(int x=0; x<Get_NX(); x++)
			{
				int xx = dx > 0. ? x : Get_NX() - 1 - x;

				if( Get_Cloud(Cloud, &Clouds, x, y) )
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

	return( Cloud.Get_Size() > 0 );
}

//---------------------------------------------------------
// cloud by cloud
bool CDetect_CloudShadows::Get_Cloud(CSG_Grid_Stack &Cloud, CSG_Grid *pClouds, int x, int y)
{
	if( !pClouds->is_NoData(x, y) )
	{
		Cloud.Destroy();

		CSG_Grid_Stack Stack;

		Stack.Push(x, y); Cloud.Push(x, y); pClouds->Set_NoData(x, y);

		while( Stack.Get_Size() > 0 && Process_Get_Okay() )
		{
			Stack.Pop(x, y);

			for(int i=0; i<8; i+=2)
			{
				int ix = Get_xTo(i, x), iy = Get_yTo(i, y);

				if(	pClouds->is_InGrid(ix, iy) )
				{
					Stack.Push(ix, iy); Cloud.Push(ix, iy); pClouds->Set_NoData(ix, iy);
				}
			}
		}

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CDetect_CloudShadows::Get_GroundCell(CSG_Grid *pDEM, int &x, int &y, double z, double dx, double dy, double dz)
{
	CSG_Point p(Get_System().Get_Grid_to_World(x, y)); double pz;

	while( pDEM->Get_Value(p, pz, GRID_RESAMPLING_NearestNeighbour) && pz < z )
	{
		p.x += dx; p.y += dy; z -= dz;
	}

	if( pz >= z )
	{
		x = Get_System().Get_xWorld_to_Grid(p.x);
		y = Get_System().Get_yWorld_to_Grid(p.y);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CDetect_CloudShadows::Find_Shadow(CSG_Grid *pShadows, CSG_Grid &Candidates, const CSG_Grid_Stack &Cloud, double zRange[2], double dx, double dy, double dz)
{
	CSG_Grid *pDEM = Parameters("DEM")->asGrid();

	double zMax = 0.; int nMax = 0;

	dz *= Get_Cellsize();

	//-----------------------------------------------------
	if( pDEM )
	{
		dx *= Get_Cellsize();
		dy *= Get_Cellsize();

		for(double z=zRange[0]; z<=zRange[1] && Set_Progress(z - zRange[0], zRange[1] - zRange[0]); z+=dz)
		{
			int n = 0;

			for(size_t i=0; i<Cloud.Get_Size(); i++)
			{
				int x = Cloud[i].x, y = Cloud[i].y;

				if( Get_GroundCell(pDEM, x, y, z, dx, dy, dz) && Candidates.is_InGrid(x, y) )
				{
					n++;
				}
			}

			if( n > nMax )
			{
				nMax = n; zMax = z;
			}
		}

		//-------------------------------------------------
		if( nMax > 0 && Process_Get_Okay() )
		{
			for(size_t i=0; i<Cloud.Get_Size(); i++)
			{
				int x = Cloud[i].x, y = Cloud[i].y;

				if( Get_GroundCell(pDEM, x, y, zMax, dx, dy, dz) && is_InGrid(x, y) )
				{
					pShadows->Set_Value(x, y, 1); Candidates.Set_Value(x, y, 0); // set shadow and remove candidate!
				}
			}
		}
	}

	//-----------------------------------------------------
	else
	{
		for(double z=zRange[0]; z<=zRange[1] && Set_Progress(z - zRange[0], zRange[1] - zRange[0]); z+=dz)
		{
			int n = 0; double ax = dx * z / dz, ay = dy * z / dz;

			for(size_t i=0; i<Cloud.Get_Size(); i++)
			{
				int x = (int)(ax + Cloud[i].x), y = (int)(ay + Cloud[i].y);

				if( Candidates.is_InGrid(x, y) )
				{
					n++;
				}
			}

			if( n > nMax )
			{
				nMax = n; zMax = z;
			}
		}

		//-------------------------------------------------
		if( nMax > 0 && Process_Get_Okay() )
		{
			double ax = dx * zMax / dz, ay = dy * zMax / dz;

			for(size_t i=0; i<Cloud.Get_Size(); i++)
			{
				int x = (int)(ax + Cloud[i].x), y = (int)(ay + Cloud[i].y);

				if( is_InGrid(x, y) )
				{
					pShadows->Set_Value(x, y, 1); Candidates.Set_Value(x, y, 0); // set shadow and remove candidate!
				}
			}
		}
	}

	//-----------------------------------------------------
	if( nMax > 0 )
	{
		Message_Fmt("\n%s: %.2f", _TL("Cloud height"), zMax);
		Message_Fmt(" (%s: %.2f => %0.f %s)", _TL("Distance"), Get_Cellsize() * zMax / dz, zMax / dz, _TL("cells"));

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
