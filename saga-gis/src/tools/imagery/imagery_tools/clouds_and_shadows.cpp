
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

enum Sensor : int
{
	TM = 0, ETM = 1, OLI_TIRS = 2
};

enum
{
	RESULT_PCP = 0, RESULT_WATER, RESULT_SNOW, RESULT_LCP, RESULT_PCL, RESULT_WCP
};


enum 
{	
	RED = 0, GREEN, BLUE, NIR, SWIR1, SWIR2, TIR, QARAD
};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool Get_Sun_Position(CSG_Grid *pGrid, double &Azimuth, double &Height)
{
	if( pGrid )
	{
		CSG_MetaData &MetaData = pGrid->Get_Owner() ? pGrid->Get_Owner()->Get_MetaData() : pGrid->Get_MetaData();

		if( MetaData("SUN_AZIMUTH") && MetaData("SUN_HEIGHT") )
		{
			return( MetaData.Get_Content("SUN_AZIMUTH", Azimuth)
				 && MetaData.Get_Content("SUN_HEIGHT" , Height )
			);
		}

		if( MetaData("LANDSAT") )
		{
			return( MetaData["LANDSAT"].Get_Content("SUN_AZIMUTH"  , Azimuth)
				 && MetaData["LANDSAT"].Get_Content("SUN_ELEVATION", Height )
			);
		}

		if( MetaData("SENTINEL-2") && MetaData["SENTINEL-2"]("SUN_AZIMUTH") && MetaData["SENTINEL-2"]("SUN_HEIGHT") )
		{
			return( MetaData["SENTINEL-2"].Get_Content("SUN_AZIMUTH", Azimuth)
				 && MetaData["SENTINEL-2"].Get_Content("SUN_HEIGHT" , Height )
			);
		}

		if( MetaData("SENTINEL-2") && MetaData["SENTINEL-2"]("PRODUCT_START_TIME") ) // estimate from time and location
		{
			CSG_DateTime Time; CSG_Point Center(pGrid->Get_Extent().Get_Center());

			if( Time.Parse_Format(MetaData["SENTINEL-2"]["PRODUCT_START_TIME"].Get_Content(), "%Y-%m-%dT%H:%M:%S")
			&&  SG_Get_Projected(pGrid->Get_Projection(), CSG_Projection::Get_GCS_WGS84(), Center)
			&&  SG_Get_Sun_Position(Time.From_UTC().Get_JDN(), M_DEG_TO_RAD * Center.x, M_DEG_TO_RAD * Center.y, Height, Azimuth) )
			{
				Azimuth *= M_RAD_TO_DEG; Height *= M_RAD_TO_DEG;

				return( true );
			}
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
CDetect_Clouds::CDetect_Clouds(void)
{
	Set_Name		(_TL("Cloud Detection"));

	Set_Author		(SG_T("J.Spitzmüller, O.Conrad (c) 2024"));

	Set_Description	(_TW(
		"This tool implements pass one of the Function of mask (Fmask) algorithm "
		"for cloud and cloud shadow detection in Landsat imagery. Landsat Top of "
		"Atmosphere (TOA) reflectance and Brightness Temperature (BT) are used "
		"as input.\n"
		"Alternatively you can choose the scene-average automated cloud-cover assessment "
		"(ACCA) algorithm as proposed by Irish (2000) and Irish et al. (2006).\n"
		"This tool can optionally pass the cloud mask to the \"Cloud Shadow Detection\" tool as well."
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
//	Parameters.Add_Grid_System("", "BANDS_VNIR"   , _TL("Grid System"), _TL(""));
	Parameters.Add_Grid_System("", "BANDS_15M"	, _TL("15 Meter Band"), _TL(""));
	Parameters.Add_Grid_System("", "BANDS_30M"	, _TL("30 Meter Band"), _TL(""));
	Parameters.Add_Grid_System("", "BANDS_60M"	, _TL("60 Meter Band"), _TL(""));
	Parameters.Add_Grid_System("", "BANDS_120M"	, _TL("120 Meter Band"), _TL(""));
	Parameters.Add_Grid_System("", "BANDS_100M"	, _TL("100 Meter Band"), _TL(""));

	Parameters.Add_Grid("BANDS_30M", "BLUE_TM"   , _TL("Blue (Band 1)"                ), _TL(""), PARAMETER_INPUT );
	Parameters.Add_Grid("BANDS_30M", "GREEN_TM"  , _TL("Green (Band 2)"               ), _TL(""), PARAMETER_INPUT );
	Parameters.Add_Grid("BANDS_30M", "RED_TM"    , _TL("Red (Band 3)"                 ), _TL(""), PARAMETER_INPUT );
	Parameters.Add_Grid("BANDS_30M", "NIR_TM"    , _TL("Near Infrared (Band 4)"       ), _TL(""), PARAMETER_INPUT );
	Parameters.Add_Grid("BANDS_30M", "SWIR1_TM"  , _TL("Shortwave Infrared 1 (Band 5)"), _TL(""), PARAMETER_INPUT );
	Parameters.Add_Grid("BANDS_30M", "SWIR2_TM"  , _TL("Shortwave Infrared 2 (Band 7)"), _TL(""), PARAMETER_INPUT );
	Parameters.Add_Grid("BANDS_30M", "QARAD_TM"  , _TL("Radiometric Saturation QA"	  ), _TL(""), PARAMETER_INPUT );
	Parameters.Add_Grid("BANDS_60M", "THERMAL_ETM", _TL("Thermal Infrared (Band 6)"	), _TL(""), PARAMETER_INPUT	);
	Parameters.Add_Grid("BANDS_120M","THERMAL_TM", _TL("Thermal Infrared (Band 6)"	), _TL(""), PARAMETER_INPUT	);
	Parameters.Add_Grid("BANDS_15M", "PANCROMATIC_ETM", _TL("Thermal Infrared (Band 8)"	), _TL(""), PARAMETER_INPUT	);
	
	Parameters.Add_Grid("BANDS_30M", "COASTAL_OLI"   , _TL("Blue (Band 1)"                ), _TL(""), PARAMETER_INPUT );
	Parameters.Add_Grid("BANDS_30M", "BLUE_OLI"   , _TL("Blue (Band 2)"                ), _TL(""), PARAMETER_INPUT );
	Parameters.Add_Grid("BANDS_30M", "GREEN_OLI"  , _TL("Green (Band 3)"               ), _TL(""), PARAMETER_INPUT );
	Parameters.Add_Grid("BANDS_30M", "RED_OLI"    , _TL("Red (Band 4)"                 ), _TL(""), PARAMETER_INPUT );
	Parameters.Add_Grid("BANDS_30M", "NIR_OLI"    , _TL("Near Infrared (Band 5)"       ), _TL(""), PARAMETER_INPUT );
	Parameters.Add_Grid("BANDS_30M", "SWIR1_OLI"  , _TL("Shortwave Infrared 1 (Band 6)"), _TL(""), PARAMETER_INPUT );
	Parameters.Add_Grid("BANDS_30M", "SWIR2_OLI"  , _TL("Shortwave Infrared 2 (Band 7)"), _TL(""), PARAMETER_INPUT );
	Parameters.Add_Grid("BANDS_15M", "PANCROMATIC_OLI", _TL("Pancromatic (Band 8)"	), _TL(""), PARAMETER_INPUT	);
	Parameters.Add_Grid("BANDS_30M", "CIRRUS_OLI", _TL("Thermal Infrared (Band 9)"	), _TL(""), PARAMETER_INPUT	);
	Parameters.Add_Grid("BANDS_100M","THERMAL_OLI", _TL("Thermal Infrared 1 (Band 10)"	), _TL(""), PARAMETER_INPUT	);
	Parameters.Add_Grid("BANDS_100M","THERMAL_OLI", _TL("Thermal Infrared 2 (Band 11)"	), _TL(""), PARAMETER_INPUT	);



	Parameters.Add_Choice("", "THERMAL_UNIT", _TL("Unit" ), _TL(""), CSG_String::Format("%s|%s", _TL("Kelvin"), _TL("Celsius")), 0);


	//-----------------------------------------------------
	Parameters.Add_Grid("BANDS_30M",
		"CLOUDS"      , _TL("Clouds"),
		_TL(""),
		PARAMETER_OUTPUT, true, SG_DATATYPE_Char
	);


	//-----------------------------------------------------
	Parameters.Add_Choice("",
		"SENSOR", _TL("Sensor (Spacecraft)"),
		_TL(""),
		"TM (Landsat 4,5)|ETM+ (Landsat 7)|OLI/TIRS (Landsat 8)", 0 
	);


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

	//-----------------------------------------------------
	Parameters.Add_Bool("",
		"SHADOWS"    , _TL("Shadow Detection"),
		_TL("Run cloud shadow detection tool with standard settings."),
		false
	);

	Parameters.Add_Double("SHADOWS",
		"SUN_AZIMUTH" , _TL("Sun's Azimuth"),
		_TL("Direction of sun clockwise from North [degree]."),
		-180., 0., true, 360., true
	);

	Parameters.Add_Double("SHADOWS",
		"SUN_HEIGHT"  , _TL("Sun's Height"),
		_TL("Height of sun above horizon [degree]."),
		45., 0., true,  90., true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CDetect_Clouds::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->is_Input() && pParameter->asGrid() )
	{
		double Azimuth, Height;

		if( Get_Sun_Position(pParameter->asGrid(), Azimuth, Height) )
		{
			pParameters->Set_Parameter("SUN_AZIMUTH", Azimuth);
			pParameters->Set_Parameter("SUN_HEIGHT" , Height );
		}
	}

	return( CSG_Tool::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
int CDetect_Clouds::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	int Sensor = pParameters->Get_Parameter("SENSOR")->asInt();

	pParameters->Set_Enabled("BANDS_15M",		Sensor == ETM || Sensor == OLI_TIRS );
	pParameters->Set_Enabled("BANDS_30M",		Sensor == TM  || Sensor == ETM || Sensor == OLI_TIRS );
	pParameters->Set_Enabled("BANDS_60M",		Sensor == ETM );
	pParameters->Set_Enabled("BANDS_120M",		Sensor == TM);
	pParameters->Set_Enabled("BANDS_100M",		Sensor == OLI_TIRS);

	pParameters->Set_Enabled("BLUE_TM", 		Sensor == TM || Sensor == ETM ); 
	pParameters->Set_Enabled("GREEN_TM",		Sensor == TM || Sensor == ETM );
	pParameters->Set_Enabled("RED_TM" , 		Sensor == TM || Sensor == ETM );   
	pParameters->Set_Enabled("NIR_TM" , 		Sensor == TM || Sensor == ETM );   
	pParameters->Set_Enabled("SWIR1_TM" , 		Sensor == TM || Sensor == ETM ); 
	pParameters->Set_Enabled("SWIR2_TM" , 		Sensor == TM || Sensor == ETM ); 
	pParameters->Set_Enabled("THERMAL_TM", 		Sensor == TM  );
	pParameters->Set_Enabled("THERMAL_ETM", 	Sensor == ETM );
	pParameters->Set_Enabled("PANCROMATIC_ETM", Sensor == ETM );
	
	pParameters->Set_Enabled("COASTAL_OLI", 	Sensor == OLI_TIRS );
	pParameters->Set_Enabled("BLUE_OLI" , 		Sensor == OLI_TIRS );
	pParameters->Set_Enabled("GREEN_OLI", 		Sensor == OLI_TIRS );
	pParameters->Set_Enabled("RED_OLI", 		Sensor == OLI_TIRS );
	pParameters->Set_Enabled("NIR_OLI", 		Sensor == OLI_TIRS );
	pParameters->Set_Enabled("SWIR1_OLI", 		Sensor == OLI_TIRS );
	pParameters->Set_Enabled("SWIR2_OLI", 		Sensor == OLI_TIRS );
	pParameters->Set_Enabled("PANCROMATIC_OLI", Sensor == OLI_TIRS );
	pParameters->Set_Enabled("CIRRUS_OLI", 		Sensor == OLI_TIRS );
	pParameters->Set_Enabled("THERMAL_OLI", 	Sensor == OLI_TIRS );
	//pParameters->Set_Enabled("THERMAL_OLI", 	Sensor == OLI_TIRS );

	
	
	//if( pParameter->Cmp_Identifier("BAND_THERMAL") )
	//{
	//	pParameters->Set_Enabled("THERMAL_UNIT", pParameter->asGrid());
	//}

	//if( pParameter->Cmp_Identifier("ALGORITHM") )
	//{
	//	pParameters->Set_Enabled("BAND_BLUE"   , pParameter->asInt() == 0);
	//	pParameters->Set_Enabled("BAND_SWIR2"  , pParameter->asInt() == 0);
	//	pParameters->Set_Enabled("BANDS_CIRRUS", pParameter->asInt() == 0);

	//	pParameters->Set_Enabled("ACCA_B56C"   , pParameter->asInt() == 1);
	//	pParameters->Set_Enabled("ACCA_B45R"   , pParameter->asInt() == 1);
	//	pParameters->Set_Enabled("ACCA_HIST_N" , pParameter->asInt() == 1);
	//	pParameters->Set_Enabled("ACCA_CSIG"   , pParameter->asInt() == 1);
	//	pParameters->Set_Enabled("ACCA_PASS2"  , pParameter->asInt() == 1);
	//}

	//if( pParameter->Cmp_Identifier("SHADOWS") )
	//{
	//	pParameter->Set_Children_Enabled(pParameter->asBool());
	//}

	//pParameters->Set_Enabled("ACCA_SHADOW" ,
	//	(*pParameters)("ALGORITHM")->asInt () == 1
	// && (*pParameters)("SHADOWS"  )->asBool() == false
	//);

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CDetect_Clouds::On_Execute(void)
{
	/*
	m_pSystem = Parameters("BAND_BLUE")->asGrid()->Get_System();

	m_pBand[0] = Parameters("BLUE_TM"   )->asGrid();
	m_pBand[1] = Parameters("GREEN_TM"  )->asGrid();
	m_pBand[2] = Parameters("RED_TM"    )->asGrid();
	m_pBand[3] = Parameters("NIR_TM"    )->asGrid();
	m_pBand[4] = Parameters("SWIR1_TM"  )->asGrid();
	m_pBand[5] = Parameters("SWIR2_TM"  )->asGrid();
	m_pBand[6] = Parameters("THERMAL_TM")->asGrid();
	//m_pBand[7] = Parameters("CIRRUS" )->asGrid();
	
	m_pResults[RESULT_PCP] 		= SG_Create_Grid( m_pSystem, SG_DATATYPE_Bit );	//PCP
	m_pResults[RESULT_WATER] 	= SG_Create_Grid( m_pSystem, SG_DATATYPE_Bit );	//Water
	m_pResults[RESULT_SNOW] 	= SG_Create_Grid( m_pSystem, SG_DATATYPE_Bit );	//Snow 
	m_pResults[RESULT_LCP] 		= SG_Create_Grid( m_pSystem, SG_DATATYPE_Float );
	m_pResults[RESULT_WCP] 		= SG_Create_Grid( m_pSystem, SG_DATATYPE_Float );

	m_bCelsius = Parameters("THERMAL_UNIT")->asInt() == 1;

	int Algorithm = Parameters("ALGORITHM")->asInt();

	//-----------------------------------------------------
	CSG_Grid *pClouds = Parameters("CLOUDS")->asGrid();

	m_pResults[RESULT_PCL] = pClouds;

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

	
	if( Parameters("SHADOWS")->asBool() )
	{
		pClouds->Get_MetaData().Add_Child("SUN_AZIMUTH", Parameters("SUN_AZIMUTH")->asDouble());
		pClouds->Get_MetaData().Add_Child("SUN_HEIGHT" , Parameters("SUN_HEIGHT")->asDouble() );
	}
	else
	{
		for(int i=0; i<8; i++)
		{
			double Azimuth, Height;

			if( Get_Sun_Position(m_pBand[i], Azimuth, Height) )
			{
				pClouds->Get_MetaData().Add_Child("SUN_AZIMUTH", Azimuth);
				pClouds->Get_MetaData().Add_Child("SUN_HEIGHT" , Height );

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
		CDetect_CloudShadows Tool; CSG_Grid Shadows(m_pSystem, SG_DATATYPE_Char);

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
		Tool.Set_Parameter("SUN_AZIMUTH", Parameters("SUN_AZIMUTH"));
		Tool.Set_Parameter("SUN_HEIGHT" , Parameters("SUN_HEIGHT" ));

		if( (bResult = Tool.Execute()) == true )
		{
			#pragma omp parallel for
			for(sLong i=0; i<m_pSystem.Get_NCells(); i++)
			{
				if( Shadows.asInt(i) && !pClouds->asInt(i) )
				{
					pClouds->Set_Value(i, ID_SHADOW);
				}
			}
		}
	}

	//-----------------------------------------------------
	return( bResult );
	*/
	return true;
}

/*
bool CDetect_Clouds::Is_Saturated(int x, int y, int Band)
{
	size_t Band_No = 0;
	switch( Band )
	{
		default: return false;
		case GREEN: 	Band_No = 1; 	break;
		case RED: 		Band_No = 2; 	break;
	}

	unsigned short Short = m_pBand[QARAD]->asShort(x,y);
	std::bitset<16> Bits(Short);

	return (bool) Bits[Band_No];
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline bool CDetect_Clouds::Get_Brightness(int x, int y, int Band, double &Value )
{
	CSG_Point p;
	if( Band > NIR )
	{
		p = m_pSystem.Get_Grid_to_World(x, y);
	}

	switch( Band )
	{
		case RED: 	if( m_pBand[RED] ) 	{ if( m_pBand[RED]->is_NoData(  x, y) ) 	{ return( false ); } Value   = m_pBand[RED]->asDouble(  x, y); 	} else { Value = -1.; } break;
		case GREEN: if( m_pBand[GREEN]) { if( m_pBand[GREEN]->is_NoData(x, y) ) 	{ return( false ); } Value   = m_pBand[GREEN]->asDouble(x, y); 	} else { Value = -1.; } break;
		case BLUE: 	if( m_pBand[BLUE] ) { if( m_pBand[BLUE]->is_NoData( x, y) ) 	{ return( false ); } Value   = m_pBand[BLUE]->asDouble( x, y); 	} else { Value = -1.; } break;
		case NIR: 	if( m_pBand[BLUE] ) { if( m_pBand[NIR]->is_NoData(  x, y) ) 	{ return( false ); } Value   = m_pBand[NIR]->asDouble(  x, y); 	} else { Value = -1.; } break;
		case SWIR1:	if( m_pBand[SWIR1]) { if(!m_pBand[SWIR1]->Get_Value(p, Value) ) { return( false ); } 											} else { Value = -1.; } break;
		case SWIR2:	if( m_pBand[SWIR2]) { if(!m_pBand[SWIR2]->Get_Value(p, Value) ) { return( false ); } 											} else { Value = -1.; }	break;
		case TIR:	if( m_pBand[TIR] )  { if(!m_pBand[TIR]->Get_Value(  p, Value) ) { return( false ); } } else { Value = -1.; } if( m_bCelsius && m_pBand[TIR] ) { Value += 273.15; } break;
		//case CIR:	if( m_pBand[7] ) { if( !m_pBand[7]->Get_Value(p, cirr ) ) { return( false ); } } else { cirr  = -1.; }
	}
	return( true );
}

double CDetect_Clouds::Get_Brightness(int x, int y, int Band, bool &Eval )
{
	CSG_Point p;
	double Value;
	if( Band > NIR )
	{
		p = m_pSystem.Get_Grid_to_World(x, y);
	}

	switch( Band )
	{
		case RED: 	if( m_pBand[RED] ) 	{ if( m_pBand[RED]->is_NoData(  x, y) ) 	{ Eval = false; return -1.0; } return m_pBand[RED]->asDouble(  x, y); } else { Eval = false; } break;
		case GREEN: if( m_pBand[GREEN]) { if( m_pBand[GREEN]->is_NoData(x, y) ) 	{ Eval = false; return -1.0; } return m_pBand[GREEN]->asDouble(x, y); } else { Eval = false; } break;
		case BLUE: 	if( m_pBand[BLUE] )	{ if( m_pBand[BLUE]->is_NoData( x, y) ) 	{ Eval = false; return -1.0; } return m_pBand[BLUE]->asDouble( x, y); } else { Eval = false; } break;
		case NIR: 	if( m_pBand[NIR] ) 	{ if( m_pBand[NIR]->is_NoData(  x, y) ) 	{ Eval = false; return -1.0; } return m_pBand[NIR]->asDouble(  x, y); } else { Eval = false; } break;
		case SWIR1:	if( m_pBand[SWIR1] ){ if(!m_pBand[SWIR1]->Get_Value(p, Value) ) { Eval = false; return -1.0; } return Value; 						  } else { Eval = false; } break;
		case SWIR2: if( m_pBand[SWIR2] ){ if(!m_pBand[SWIR2]->Get_Value(p, Value) ) { Eval = false; return -1.0; } return Value; 						  } else { Eval = false; } break;
		case TIR: 	if( m_pBand[TIR] ) 	{ if(!m_pBand[TIR]->Get_Value(  p, Value) ) { Eval = false; return -1.0; } return Value; 						  } else { Eval = false; } break;
	}
	return -1.;
}


//---------------------------------------------------------
bool CDetect_Clouds::Set_Fmask_Pass_One_Two(void)
{
	CSG_Simple_Statistics Clear_Sky_Water = CSG_Simple_Statistics(true);
	CSG_Simple_Statistics Clear_Sky_Land = CSG_Simple_Statistics(true);

	for( int x=0; x<m_pSystem.Get_NX(); x++ )
	{
		for( int y=0; y<m_pSystem.Get_NY(); y++ )
		{
	  		bool Eval = true;
			double Red 	= Get_Brightness( x, y, RED, 	Eval ); 
			double Green= Get_Brightness( x, y, GREEN, 	Eval ); 
			double Blue = Get_Brightness( x, y, GREEN, 	Eval ); 
			double Nir 	= Get_Brightness( x, y, NIR,	Eval ); 
			double Swir1= Get_Brightness( x, y, SWIR1, 	Eval ); 
			double Swir2= Get_Brightness( x, y, SWIR2, 	Eval ); 
			double Tir 	= Get_Brightness( x, y, TIR, 	Eval ); 

			if( !Eval )
			{
				
			}

			// Eq. 1
			double NDSI = (Green - Swir1) / (Green + Swir2);
			double NDVI = (Nir - Red) / (Nir + Red);
			bool Basic_Test = Swir2 > 0.03 && Tir < 27.0 && NDSI < 0.8 && NDVI < 0.8;
			
			// Eq. 20
			if( NDVI > 0.15 && Tir < 3.8 && Nir > 0.11 && Green > 0.1 )
			{
				m_pResults[RESULT_SNOW]->Set_Value( x, y, 1.0 );
			}
			else
	  		{
				m_pResults[RESULT_SNOW]->Set_Value( x, y, 0.0 );
			}
			
			// Eq. 2
			double MV = ( Red + Green + Blue )/3.0;
	 		double Whitenes = (fabs((Blue - MV)/MV) + fabs((Green - MV)/MV) + fabs((Red - MV)/MV));
			bool Whitenes_Test = Whitenes  >= 0.7;

			// Eq. 3
			double HOT = Blue - 0.5 * Red - 0.08; 
			bool HOT_Test = HOT > 0.0;

			// Eq. 4
			bool B4_B5_Test = Nir /  Swir1 > 0.75;

			// Eq. 5
			bool Water_Test = ( NDVI < 0.01 && Nir < 0.11 ) ||( NDVI < 0.1 && Nir < 0.01 );
			if( Water_Test ) 
			{
				m_pResults[RESULT_PCP]->Set_Value(x,y, 1.0 );
			}
			else 
			{
				m_pResults[RESULT_PCP]->Set_Value(x,y, 0.0 );
			}


			// Eq. 6
			if( Basic_Test && Whitenes_Test && HOT_Test && B4_B5_Test )
			{
				m_pResults[RESULT_PCP]->Set_Value(x,y, 1.0 );
			}
			else
	  		{
				m_pResults[RESULT_PCP]->Set_Value(x,y, 0.0 );

				bool Eval;
				double Swir2= Get_Brightness( x, y, SWIR2, 	Eval ); 
				double Tir 	= Get_Brightness( x, y, TIR, 	Eval ); 

				// Eq. 7
				if( Water_Test &&  Swir2 < 0.03 )
				{	
					Clear_Sky_Water += Tir;
				}

				// Eq. 12
				if( !Water_Test )
				{
					Clear_Sky_Land += Tir;
					// Eq. 15
					// Calculating Variability_Prob in pass bc every needed value is here.
					// Store the Variability_Prob in the lCloud_Prop Grid.
					double modNDSI = Is_Saturated( x, y, GREEN) && Swir1 > Green ? 0. : NDSI;
					double modNDVI = Is_Saturated( x, y, RED) 	&& Nir 	 > Red 	 ? 0. : NDSI;
					m_pResults[RESULT_LCP]->Set_Value( x, y, 1.0 - std::max( std::abs(modNDSI), std::max( std::abs(modNDVI), Whitenes)));
				}
			}
		}
	}

	// Eq. 8
	double T_Water 	= Clear_Sky_Water.Get_Percentile(82.5);
	// Eq. 13
	double T_Low 	= Clear_Sky_Land.Get_Percentile(17.5);
	double T_High 	= Clear_Sky_Land.Get_Percentile(82.5);
	
	for( int x=0; x<m_pSystem.Get_NX(); x++ )
	{
		for( int y=0; y<m_pSystem.Get_NY(); y++ )
		{
			bool Eval;
			double Swir1= Get_Brightness( x, y, SWIR1, 	Eval ); 
			double Tir 	= Get_Brightness( x, y, TIR, 	Eval ); 

			// Eq. 9
			double wTemp_Prob = (T_Water - Tir) / 4.;

			// Eq. 10
			double Brightness_Prob = std::min( Swir1, 0.11 )/ 0.11;

			// Eq. 11	
			m_pResults[RESULT_WCP]->Set_Value( x, y, wTemp_Prob * Brightness_Prob );

			// Eq. 14
			double lTemp_Prob = ( T_High + 4.0 - Tir ) / ( T_High + 4.0 - ( T_Low - 4.0 )); 
			// The Variability_Prob is actually calculated in pass one and stored in the lCloud_Prop Grid.
			// Now recover the value and overwrite with lCloud_Prop after calculation.
			double Variability_Prob = m_pResults[RESULT_LCP]->asDouble( x, y);
			// Eq. 16
			m_pResults[RESULT_LCP]->Set_Value( x, y, lTemp_Prob * Variability_Prob );
	 	}
	}

	// Eq. 17
	double Land_threshold = m_pResults[RESULT_LCP]->Get_Percentile(82.5);

	for( int x=0; x<m_pSystem.Get_NX(); x++ )
	{
		for( int y=0; y<m_pSystem.Get_NY(); y++ )
		{
			bool Eval;
			double Tir 	= Get_Brightness( x, y, TIR, 	Eval ); 

			// Equation. 18
			if( ( m_pResults[RESULT_WATER]->asInt(x,y) == 1 && m_pResults[RESULT_PCP]->asInt(x,y) == 1 		&& m_pResults[RESULT_WCP]->asFloat(x,y) > 0.5)
			||  ( m_pResults[RESULT_WATER]->asInt(x,y) == 0 && m_pResults[RESULT_PCP]->asInt(x,y) == 1 		&& m_pResults[RESULT_LCP]->asFloat(x,y) > Land_threshold) 
			||  ( m_pResults[RESULT_WATER]->asInt(x,y) == 0 && m_pResults[RESULT_LCP]->asFloat(x,y) > 0.99)
			||	( Tir < T_Low - 35.0 ) )
	 		{
	  			m_pResults[RESULT_PCL]->Set_Value(x,y, 1.0);
			}
	 	}
	}
	

	return true;
}
*/

/*
//---------------------------------------------------------
inline int CDetect_Clouds::Get_Fmask(int x, int y)
{
	double b, g, r, nir, swir1, swir2, tir, cirr;

	if( !Get_Brightness(x, y, b, g, r, nir, swir1, swir2, tir, cirr) )
	{
		return( ID_NONE );
	}


	double ndsi = (g - swir1) / (g + swir2);
	
	double ndvi = (nir - r) / (nir + r);

	bool basic_test = swir2 > 0.03 && tir < 27.0 && ndsi < 0.8 && ndvi < 0.8;
	
	double mv = (r+g+b)/3;
	
	bool whitenes_test = (fabs((b - mv)/mv) + fabs((g - mv)/mv) + fabs((r - mv)/mv))  >= 0.7;

	double hot = b - 0.5 * r - 0.08; 

	bool hot_test = hot > 0.0;

	bool b4_b5 = nir / swir1 > 0.75;

	bool water_test = ( ndvi < 0.01 && nir < 0.11 ) ||( ndvi < 0.1 && nir < 0.01 ) 

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
	for(int y=0; y<m_pSystem.Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<m_pSystem.Get_NX(); x++)
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

*/

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
		"This tool derives cloud shadows from detected clouds based on their spectral "
		"characteristics as well as terrain, sun position and expected cloud height. "
		"The initial cloud mask can be created by the tool \"Cloud Detection\"."
	));


	Add_Reference("Irish, R.R.", "2000",
		"Landsat 7 Automatic Cloud Cover Assessment",
		"In: Shen, S.S., Descour, M.R. [Eds.]: Algorithms for Multispectral, Hyperspectral, and Ultraspectral Imagery VI. Proceedings of SPIE, 4049: 348-355.",
		SG_T("https://doi.org/10.1117/12.410358"), SG_T("doi:10.1117/12.410358")
	);

	Add_Reference("Tizado, E.J.", "2010",
		"GRASS GIS i.landsat.acca",
		"E.J. Tizado's implementation of the ACCA algorithm as proposed by Irish (2000), Irish et al. (2006).",
		SG_T("https://github.com/OSGeo/grass/tree/main/imagery/i.landsat.acca"), SG_T("Source Code") // https://grass.osgeo.org/grass72/manuals/i.landsat.acca.html
	);

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
		double Azimuth, Height;

		if( Get_Sun_Position(pParameter->asGrid(), Azimuth, Height) )
		{
			pParameters->Set_Parameter("SUN_AZIMUTH", Azimuth);
			pParameters->Set_Parameter("SUN_HEIGHT" , Height );
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

		pParameters->Set_Enabled("BAND_GREEN"      , m == 3 || m == 4);
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
				bCandidate = s.Get_Count() > 0 && s.Get_Mean() < Brightness;
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
