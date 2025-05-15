
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
#include "fmask.h"
#include <vector>

//---------------------------------------------------------
enum
{
	ID_CLOUD = 1, ID_CLOUD_DILATED = 2, ID_SHADOW = 3, ID_SNOW = 4, ID_NONE = 5, ID_NODATA = 127
};


enum
{
	RESULT_PCP = 0, RESULT_WATER, RESULT_LCP, RESULT_PCL, RESULT_PCSL, RESULT_WCP, RESULT_FFN, RESULT_FFS, RESULT_SEG, RESULT_CAST
};

#undef _DEBUG

//enum 
//{	
//	RED = 0, GREEN, BLUE, NIR, SWIR1, SWIR2, TIR, QARAD
//};

static int	xnb[] = { -1,  0,  1, -1,  1, -1,  0,  1 };
static int	ynb[] = { -1, -1, -1,  0,  0,  1,  1,  1 };


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFmask::Get_Sun_Position(CSG_Grid *pGrid, double &Azimuth, double &Height)
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

//---------------------------------------------------------
bool CFmask::Get_Sensor(CSG_Grid *pGrid, Sensor &Sensor)
{
	if( pGrid )
	{
		CSG_MetaData &MetaData = pGrid->Get_Owner() ? pGrid->Get_Owner()->Get_MetaData() : pGrid->Get_MetaData();

		if( MetaData("SENSOR_ID") )
		{
			CSG_String Content = MetaData("SENSOR_ID")->Get_Content();
			if( Content.is_Same_As("OLI_TIRS") )
			{
				Sensor = OLI_TIRS;
				return( true );
			}
			if( Content.is_Same_As("ETM") )
			{
				Sensor = ETM;
				return( true );
			}
			if( Content.is_Same_As("TM") )
			{
				Sensor = TM;
				return( true );
			}
		}

		if( MetaData("SENTINEL-2") )
		{
			Sensor = MSI;
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
CFmask::CFmask(void)
{
	Set_Name		(_TL("Fmask Cloud and Cloud Shadow Detection"));

	Set_Author		(SG_T("J.Spitzmüller, O.Conrad (c) 2025"));

	Set_Description	(_TW(
		"This tool implements the Function of mask (Fmask) algorithm "
		"for cloud and cloud shadow detection in Landsat and Sentinel-2 imagery. "
		"Input is Top-of-Atmosphere (TAO) reflectance obtained from processing Level-1C (both programs)."




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

	//-----------------------------------------------------
//	Parameters.Add_Grid_System("", "BANDS_VNIR"   , _TL("Grid System"), _TL(""));
	Parameters.Add_Grid_System("", "BANDS"	, 		_TL("Bands"), 	_TL(""));
	Parameters.Add_Grid_System("", "VEGETATION"	, 	_TL("Bands"), 	_TL(""));
	Parameters.Add_Grid_System("", "ATMOSPHERE"	, 	_TL("Bands"), 	_TL(""));
	Parameters.Add_Grid_System("", "THERMAL_SYS",	_TL("Thermal"), _TL(""));

	Parameters.Add_Grid( "BANDS"		, "BLUE"   		, _TL("Blue"                				), _TL("TM & ETM: Band 1; OLI/TIRS: Band 2; MSI: Band 2")	, PARAMETER_INPUT );
	Parameters.Add_Grid( "BANDS"		, "GREEN"  		, _TL("Green"               				), _TL("TM & ETM: Band 2; OLI/TIRS: Band 3; MSI: Band 3")	, PARAMETER_INPUT );
	Parameters.Add_Grid( "BANDS"		, "RED"    		, _TL("Red"                 				), _TL("TM & ETM: Band 3; OLI/TIRS: Band 4; MSI: Band 4")	, PARAMETER_INPUT );
	Parameters.Add_Grid( "BANDS"		, "NIR"    		, _TL("Near Infrared"       				), _TL("TM & ETM: Band 4; OLI/TIRS: Band 5; MSI: Band 8")	, PARAMETER_INPUT );
	Parameters.Add_Grid( "BANDS"		, "SWIR1"  		, _TL("Shortwave Infrared 1"				), _TL("TM & ETM: Band 5; OLI/TIRS: Band 6")				, PARAMETER_INPUT );
	Parameters.Add_Grid( "BANDS"		, "SWIR2"  		, _TL("Shortwave Infrared 2"				), _TL("TM & ETM: Band 7; OLI/TIRS: Band 7")				, PARAMETER_INPUT );
	Parameters.Add_Grid( "BANDS"		, "CIRRUS"		, _TL("Cirrus"								), _TL("OLI/TIRS: Band 9; MSI: Band 10")					, PARAMETER_INPUT );
	Parameters.Add_Grid( "BANDS"		, "QARAD_G"		, _TL("Radiometric Saturation Green" 		), _TL("")													, PARAMETER_INPUT_OPTIONAL );
	Parameters.Add_Grid( "BANDS"		, "QARAD_R"		, _TL("Radiometric Saturation Red"			), _TL("")													, PARAMETER_INPUT_OPTIONAL );

	Parameters.Add_Grid( "THERMAL_SYS"	, "THERMAL"		, _TL("Thermal Infrared 1"					), _TL("TM & ETM: Band 6; OLI/TIRS: Band 10")				, PARAMETER_INPUT );

	Parameters.Add_Grid( "VEGETATION"	, "SWIR1_MSI"	, _TL("Shortwave Infrared 1"				), _TL("MSI: Band 11")										, PARAMETER_INPUT );
	Parameters.Add_Grid( "VEGETATION"	, "SWIR2_MSI"	, _TL("Shortwave Infrared 2"				), _TL("MSI: Band 12")										, PARAMETER_INPUT );
	Parameters.Add_Grid( "ATMOSPHERE"	, "CIRRUS_MSI"	, _TL("Cirrus"								), _TL("MSI: Band 10")										, PARAMETER_INPUT);
	
	//Parameters.Add_Grid("BANDS_30M", "SAA_TM"		, _TL("Sun Azimuth Angle"				), _TL(""), PARAMETER_INPUT );
	//Parameters.Add_Grid("BANDS_30M", "SZA_TM"		, _TL("Sun Zenith Angle"				), _TL(""), PARAMETER_INPUT );
	//Parameters.Add_Grid("BANDS_30M", "VAA_TM"		, _TL("Sensor Azimuth Angle"			), _TL(""), PARAMETER_INPUT );
	//Parameters.Add_Grid("BANDS_30M", "VZA_TM"		, _TL("Sensor Zenith Angle"				), _TL(""), PARAMETER_INPUT );

	
	Parameters.Add_Choice("THERMAL_SYS", "THERMAL_UNIT", _TL("Unit" ), _TL(""), CSG_String::Format("%s|%s", _TL("Kelvin"), _TL("Celsius")), 0);


	//-----------------------------------------------------
	Parameters.Add_Grid("BANDS",
		"CLOUDS"      , _TL("Clouds"),
		_TL(""),
		PARAMETER_OUTPUT, true, SG_DATATYPE_Char
	);

	#ifdef _DEBUG
	Parameters.Add_Grid("BANDS", "D_PCP", 	_TL("(Debug) Potential Cloud Pixel"), 		_TL(""), PARAMETER_OUTPUT, true, SG_DATATYPE_Bit );
	Parameters.Add_Grid("BANDS", "D_WATER", _TL("(Debug) Water"), 						_TL(""), PARAMETER_OUTPUT, true, SG_DATATYPE_Bit );
	//Parameters.Add_Grid("BANDS", "D_SNOW", 	_TL("(Debug) Snow"), 						_TL(""), PARAMETER_OUTPUT, true, SG_DATATYPE_Bit );
	Parameters.Add_Grid("BANDS", "D_LCP", 	_TL("(Debug) Land Cloud Probability"), 		_TL(""), PARAMETER_OUTPUT, true, SG_DATATYPE_Float );
	Parameters.Add_Grid("BANDS", "D_WCP", 	_TL("(Debug) Water Cloud Probability"), 	_TL(""), PARAMETER_OUTPUT, true, SG_DATATYPE_Float );
	Parameters.Add_Grid("BANDS", "D_PCSL", 	_TL("(Debug) Potential Cloud Shadow Layer"),_TL(""), PARAMETER_OUTPUT, true, SG_DATATYPE_Bit );
	Parameters.Add_Grid("BANDS", "D_SEG", 	_TL("(Debug) Segmented Clouds"), 			_TL(""), PARAMETER_OUTPUT, true, SG_DATATYPE_DWord );
	Parameters.Add_Grid("BANDS", "D_CAST", 	_TL("(Debug) 3D Shadow Casting"), 			_TL(""), PARAMETER_OUTPUT, true, SG_DATATYPE_Bit );
	#endif

	//-----------------------------------------------------
	Parameters.Add_Choice("",
		"SENSOR", _TL("Sensor (Spacecraft)"),
		_TL(""),
		"TM (Landsat 4,5)|ETM+ (Landsat 7)|OLI/TIRS (Landsat 8,9)|MSI (Sentinel-2)", 2 
	);
	
	//-----------------------------------------------------
	Parameters.Add_Choice("",
		"ALGORITHM", _TL("Algorithm"),
		_TL(""),
		"Fmask 1.6|Fmask 3.2", 1
	);

	//-----------------------------------------------------
	Parameters.Add_Double("",
		"DILATION" , _TL("Dilation Distance"),
		_TL(""),
		240., 0., true
	);

	Parameters.Add_Double("",
		"SUN_AZIMUTH" , _TL("Sun's Azimuth"),
		_TL("Direction of sun clockwise from North [degree]."),
		180., 0., true, 360., true
	);

	Parameters.Add_Double("",
		"SUN_HEIGHT"  , _TL("Sun's Height"),
		_TL("Height of sun above horizon [degree]."),
		45., 0., true,  90., true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CFmask::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->is_Input() && pParameter->asGrid() )
	{
		double Azimuth, Height;
		Sensor Sensor;

		if( Get_Sun_Position(pParameter->asGrid(), Azimuth, Height) )
		{
			pParameters->Set_Parameter("SUN_AZIMUTH", Azimuth);
			pParameters->Set_Parameter("SUN_HEIGHT" , Height );
		}
		
		if( Get_Sensor(pParameter->asGrid(), Sensor) )
		{
			pParameters->Set_Parameter("SENSOR", Sensor);
		}
	}

	if( pParameter->Cmp_Identifier("SENSOR") || pParameter->Cmp_Identifier("ALGORITHM") )
	{
		int Sensor 		= pParameters->Get_Parameter("SENSOR")->asInt();
		int Algorithm 	= pParameters->Get_Parameter("ALGORITHM")->asInt();

		if( Sensor == MSI && Algorithm == FMASK_1_6 )
		{
			pParameters->Set_Parameter("ALGORITHM", FMASK_3_2 );
		}
	}

	return( CSG_Tool::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
int CFmask::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	int Sensor 		= pParameters->Get_Parameter("SENSOR")->asInt();
	int Algorithm 	= pParameters->Get_Parameter("ALGORITHM")->asInt();
	
	pParameters->Set_Enabled("THERMAL_UNIT",	Sensor != MSI );

	pParameters->Set_Enabled("THERMAL_SYS"	, Sensor == TM  || Sensor == ETM || Sensor == OLI_TIRS );
	pParameters->Set_Enabled("VEGETATION"	, Sensor == MSI );
	pParameters->Set_Enabled("ATMOSPHERE"	, Sensor == MSI );

	pParameters->Set_Enabled("SWIR1"  		, Sensor == TM  || Sensor == ETM || Sensor == OLI_TIRS );
	pParameters->Set_Enabled("SWIR2"  		, Sensor == TM  || Sensor == ETM || Sensor == OLI_TIRS );
	pParameters->Set_Enabled("SWIR1_MSI"  	, Sensor == MSI );
	pParameters->Set_Enabled("SWIR2_MSI"  	, Sensor == MSI );
	pParameters->Set_Enabled("CIRRUS" 		, Sensor == OLI_TIRS && Algorithm == FMASK_3_2 );
	pParameters->Set_Enabled("CIRRUS_MSI" 	, Sensor == MSI );


	/*
	if( Sensor == TM || Sensor == ETM )
	{
		pParameters->Get_Parameter("BLUE"   	)->Set_Name(_TL("Blue (Band 1)"                	));
		pParameters->Get_Parameter("GREEN"  	)->Set_Name(_TL("Green (Band 2)"               	));
		pParameters->Get_Parameter("RED"    	)->Set_Name(_TL("Red (Band 3)"                 	));
		pParameters->Get_Parameter("NIR"    	)->Set_Name(_TL("Near Infrared (Band 4)"       	));
		pParameters->Get_Parameter("SWIR1"  	)->Set_Name(_TL("Shortwave Infrared 1 (Band 5)"	));
		pParameters->Get_Parameter("SWIR2"  	)->Set_Name(_TL("Shortwave Infrared 2 (Band 7)"	));
		pParameters->Get_Parameter("THERMAL"  	)->Set_Name(_TL("Thermal Infrared (Band 6)"	  	));
	}

	if( Sensor == OLI_TIRS )
	{
		pParameters->Get_Parameter("BLUE"   	)->Set_Name(_TL("Blue (Band 2)"					));
		pParameters->Get_Parameter("GREEN"  	)->Set_Name(_TL("Green (Band 3)"				));
		pParameters->Get_Parameter("RED"    	)->Set_Name(_TL("Red (Band 4)"   				));
		pParameters->Get_Parameter("NIR"    	)->Set_Name(_TL("Near Infrared (Band 5)"       	));
		pParameters->Get_Parameter("SWIR1"  	)->Set_Name(_TL("Shortwave Infrared 1 (Band 6)"	));
		pParameters->Get_Parameter("SWIR2"  	)->Set_Name(_TL("Shortwave Infrared 2 (Band 7)"	));
		pParameters->Get_Parameter("CIRRUS" 	)->Set_Name(_TL("Cirrus (Band 9)"				));
		pParameters->Get_Parameter("THERMAL"	)->Set_Name(_TL("Thermal Infrared 1 (Band 10)"	));
	}
 	
	if( Sensor == MSI )
	{
		pParameters->Get_Parameter("BLUE"   	)->Set_Name(_TL("Blue (Band 2)"                	));
		pParameters->Get_Parameter("GREEN"  	)->Set_Name(_TL("Green (Band 3)"               	));
		pParameters->Get_Parameter("RED"    	)->Set_Name(_TL("Red (Band 4)"                 	));
		pParameters->Get_Parameter("NIR"    	)->Set_Name(_TL("Near Infrared (Band 8)"       	));
		pParameters->Get_Parameter("SWIR1_MSI"  )->Set_Name(_TL("Shortwave Infrared 1 (Band 11)"));
		pParameters->Get_Parameter("SWIR2_MSI"  )->Set_Name(_TL("Shortwave Infrared 2 (Band 12)"));
		pParameters->Get_Parameter("CIRRUS_MSI" )->Set_Name(_TL("Cirrus (Band 10)"				));
	}
	*/

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

bool CFmask::Initialize(void)
{
	m_Sensor = Parameters("SENSOR")->asInt();

	m_pBand[BLUE] 	= Parameters("BLUE"   )->asGrid();
	m_pBand[GREEN] 	= Parameters("GREEN"  )->asGrid();
	m_pBand[RED] 	= Parameters("RED"    )->asGrid();
	m_pBand[NIR] 	= Parameters("NIR"    )->asGrid();


	if( m_Sensor == TM || m_Sensor == ETM || m_Sensor == OLI_TIRS )
	{
		m_pBand[SWIR1] 	= Parameters("SWIR1"  		)->asGrid();
		m_pBand[SWIR2] 	= Parameters("SWIR2"  		)->asGrid();
		m_pBand[TIR] 	= Parameters("THERMAL"		)->asGrid();
		m_pBand[QARAD_G]= Parameters("QARAD_G"		)->asGrid();
		m_pBand[QARAD_R]= Parameters("QARAD_R"		)->asGrid();
	}
	
	if( m_Sensor == OLI_TIRS )
	{
		m_pBand[CIR] 	= Parameters("CIRRUS"		)->asGrid();
	}
	
	if( m_Sensor == MSI )
	{
		m_pBand[SWIR1] 	= Parameters("SWIR1_MSI" 	)->asGrid();
		m_pBand[SWIR2] 	= Parameters("SWIR2_MSI" 	)->asGrid();
		m_pBand[CIR] 	= Parameters("CIRRUS_MSI"	)->asGrid();
		m_pBand[TIR] 	= NULL;
		m_pBand[QARAD_G]= NULL;
		m_pBand[QARAD_R]= NULL;
	}



	if( Parameters("ALGORITHM")->asInt() == 0 )
	{
		m_Algorithm = FMASK_1_6;
	}
	
	if( Parameters("ALGORITHM")->asInt() == 1 )
	{
		m_Algorithm = FMASK_3_2;
	}
	
	m_pSystem = Parameters("BANDS")->asGrid_System();
	
	m_pResults[RESULT_PCL] 		= Parameters("CLOUDS")->asGrid();
	CSG_Parameter *pLUT = DataObject_Get_Parameter(m_pResults[RESULT_PCL], "LUT");

	if( pLUT && pLUT->asTable() )
	{
		#define LUT_ADD_CLASS(id, name, color) { CSG_Table_Record &r = *pLUT->asTable()->Add_Record(); r.Set_Value(0, color); r.Set_Value(1, name); r.Set_Value(3, id); r.Set_Value(3, id); }

		LUT_ADD_CLASS( ID_CLOUD, 			"Cloud", 			SG_COLOR_YELLOW 	);
		LUT_ADD_CLASS( ID_CLOUD_DILATED, 	"Dilated Cloud", 	SG_COLOR_YELLOW 	);
		LUT_ADD_CLASS( ID_SHADOW, 			"Cloud Shadow", 	SG_COLOR_GREEN 		);
		LUT_ADD_CLASS( ID_SNOW, 			"Snow", 			SG_COLOR_BLUE_LIGHT );
		//LUT_ADD_CLASS( ID_CLOUD, "Cirrus", SG_COLOR_YELLOW )
		//LUT_ADD_CLASS( ID_WATER, "Cloud", SG_COLOR_BLUE )

		DataObject_Set_Parameter(m_pResults[RESULT_PCL], pLUT);
		DataObject_Set_Parameter(m_pResults[RESULT_PCL], "COLORS_TYPE", 1); // Color Classification Type: Lookup Table
	}

	
	#ifdef _DEBUG
	m_pResults[RESULT_PCP] 		= Parameters("D_PCP")->asGrid();
	m_pResults[RESULT_WATER] 	= Parameters("D_WATER")->asGrid();
	//m_pResults[RESULT_SNOW] 	= Parameters("D_SNOW")->asGrid();
	m_pResults[RESULT_LCP] 		= Parameters("D_LCP")->asGrid();
	m_pResults[RESULT_WCP] 		= Parameters("D_WCP")->asGrid();
	m_pResults[RESULT_PCSL] 	= Parameters("D_PCSL")->asGrid();
	m_pResults[RESULT_SEG] 		= Parameters("D_SEG")->asGrid();
	m_pResults[RESULT_CAST] 	= Parameters("D_CAST")->asGrid();
	#else	
	m_pResults[RESULT_PCP] 		= SG_Create_Grid( *m_pSystem, m_Bool_Type );	//PCP
	m_pResults[RESULT_WATER] 	= SG_Create_Grid( *m_pSystem, m_Bool_Type );	//Water
	//m_pResults[RESULT_SNOW] 	= SG_Create_Grid( *m_pSystem, SG_DATATYPE_Bit );	//Snow 
	m_pResults[RESULT_LCP] 		= SG_Create_Grid( *m_pSystem, SG_DATATYPE_Float );
	m_pResults[RESULT_WCP] 		= SG_Create_Grid( *m_pSystem, SG_DATATYPE_Float );
	m_pResults[RESULT_PCSL] 	= SG_Create_Grid( *m_pSystem, m_Bool_Type );	//Snow 
	#endif

	m_pResults[RESULT_WCP]->Assign_NoData();
	m_pResults[RESULT_LCP]->Assign_NoData();
	
	m_pResults[RESULT_PCL]->Set_NoData_Value_Range( ID_NONE, ID_NODATA );
	m_pResults[RESULT_PCL]->Assign( ID_NONE );

	m_bCelsius = Parameters("THERMAL_UNIT")->asInt() == 1;
	m_Temp_Off = m_bCelsius ? 0 : -273.15;
	
	m_Bin_Count = 2048;
	m_Temp_Min = m_pBand[TIR] ? m_pBand[TIR]->Get_Min() :    0.0;
	m_Temp_Max = m_pBand[TIR] ? m_pBand[TIR]->Get_Min() : 1000.0;

	m_Similarity_Threshold = 0.3;
	m_Decrease_Threshold 	= m_Algorithm == FMASK_1_6 ? 0.98 : 0.95;
	m_Snow_Temp_Threshold 	= m_Algorithm == FMASK_1_6 ? 3.85 : 9.85;
	
	m_dx = sin(Parameters("SUN_AZIMUTH")->asDouble() * M_DEG_TO_RAD + M_PI_180);
	m_dy = cos(Parameters("SUN_AZIMUTH")->asDouble() * M_DEG_TO_RAD + M_PI_180);
	m_dz = tan(Parameters("SUN_HEIGHT" )->asDouble() * M_DEG_TO_RAD);
	m_dz *= m_pSystem->Get_Cellsize();

	
	m_Shadow_Radius = 240.0 / m_pSystem->Get_Cellsize();
	m_Min_Cloud_Size = (int) 2700.0 / m_pSystem->Get_Cellarea();  
	
	int Dilation = Parameters("DILATION")->asDouble() / m_pSystem->Get_Cellsize();
	Create_Kernel( &m_xKernel_Dilation_Cloud, &m_yKernel_Dilation_Cloud, Dilation, false );

	int Shadow_Fill = 90 / m_pSystem->Get_Cellsize();
	Create_Kernel( &m_xKernel_Fill_Shadow, &m_yKernel_Fill_Shadow, Shadow_Fill, true );
	
	int Improve_Cloud = 30 / m_pSystem->Get_Cellsize();
	Create_Kernel( &m_xKernel_Improve_Cloud, &m_yKernel_Improve_Cloud, Improve_Cloud, true );
	m_Improve_Cloud_Count = std::ceil(m_yKernel_Improve_Cloud.Get_Size() * 0.625);

	return( true );
}

bool CFmask::Create_Kernel( CSG_Array_Int *X_Kernel, CSG_Array_Int *Y_Kernel, int Cell_Radius, bool Square )
{
	CSG_Grid_Cell_Addressor Addressor; 
	Addressor.Set_Radius( Cell_Radius, Square );
	CSG_Cloud_Stack Stack;
	for( int i=0; i<Addressor.Get_Count(); i++ )
	{
		int x = Addressor.Get_X(i), y = Addressor.Get_Y(i);
		if( !(x == 0 && y == 0) )
		{
			Stack.Push( x, y );
		}
	}

	Stack.Sort();
	
	X_Kernel->Create(Stack.Get_Size()); Y_Kernel->Create(Stack.Get_Size());
	for( int i=0; i<Stack.Get_Size(); i++ )
	{
		X_Kernel->Set(i, Stack[i].x);
		Y_Kernel->Set(i, Stack[i].y);
	}
	
	return( true );
}

//---------------------------------------------------------
bool CFmask::On_Execute(void)
{
	Initialize();

	//-----------------------------------------------------


	//-----------------------------------------------------
	double T_Water 			= 0.0;
	double T_Low 			= 0.0; 
	double T_High 			= 0.0;
	double Lower_Level_NIR	= 0.0;
	double Lower_Level_Swir = 0.0;
	
	Set_Fmask_Pass_One( T_Water, T_Low, T_High, Lower_Level_NIR, Lower_Level_Swir);
	
	
	SG_UI_Msg_Lock(true);
	//#pragma omp parallel 
	//{
	//	#pragma omp single 
	//	{
	//		#pragma omp task 
	//		{
	//			Get_Flood_Fill( Lower_Level_NIR, NIR, RESULT_FFN );
	//		}	

	//		#pragma omp task 
	//		{
	//			if( m_Algorithm == FMASK_3_2 )
	//			{
	//				Get_Flood_Fill( Lower_Level_Swir, SSWIR, RESULT_FFS );
	//			}
	//		}
	//	}
	//	
	//	#pragma omp taskwait

	//}
	Get_Flood_Fill( Lower_Level_NIR, NIR, RESULT_FFN );
   	
	if( m_Algorithm == FMASK_3_2 )
	{
   		Get_Flood_Fill( Lower_Level_Swir, SWIR1, RESULT_FFS );
   	}


	SG_UI_Msg_Lock(false);

	double Land_threshold 	= 0.0;
	double Water_threshold 	= 0.0;
	
	Set_Fmask_Pass_Two( T_Water, T_Low, T_High, Land_threshold, Water_threshold );

	Set_Cloud_Mask( T_Low, Land_threshold, Water_threshold );

	Set_Shadow_Mask( T_Low, T_High );

	m_pResults[RESULT_FFN]->Delete();
	m_pResults[RESULT_FFS]->Delete();

	Set_Final_Fmask();

	return true;
}

bool CFmask::Is_Saturated(int x, int y, SpectralBand Band)
{
	switch( Band )
	{
		case RED: 	if( m_pBand[QARAD_R] ) 	{ if( m_pBand[QARAD_R]->is_NoData(x, y)) 			{ return(false); } return m_pBand[QARAD_G]->asInt(x, y); 		} else { return( false ); } break;
		case GREEN: if( m_pBand[QARAD_G] )  { if( m_pBand[QARAD_G]->is_NoData(x, y)) 			{ return(false); } return m_pBand[QARAD_G]->asInt(x, y); 		} else { return( false ); } break;
		default: 	return( false ); break;
	}
	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double CFmask::Get_Brightness(int x, int y, int Band, bool &Eval )
{
	CSG_Point p;
	double Value;
	TSG_Grid_Resampling r = GRID_RESAMPLING_NearestNeighbour;
	if( Band >= NIR )
	{
		p = m_pSystem->Get_Grid_to_World(x, y);
	}

	switch( Band )
	{
		case RED: 	if( m_pBand[RED] ) 	{ if( m_pBand[RED]->is_NoData(  x, y)) 			{ Eval = false; return -1.0; } return m_pBand[RED]->asDouble(  x, y); 		} else { Eval = false; } break;
		case GREEN: if( m_pBand[GREEN]) { if( m_pBand[GREEN]->is_NoData(x, y)) 			{ Eval = false; return -1.0; } return m_pBand[GREEN]->asDouble(x, y); 		} else { Eval = false; } break;
		case BLUE: 	if( m_pBand[BLUE] )	{ if( m_pBand[BLUE]->is_NoData( x, y)) 			{ Eval = false; return -1.0; } return m_pBand[BLUE]->asDouble( x, y); 		} else { Eval = false; } break;
		case NIR: 	if( m_pBand[NIR] ) 	{ if(!m_pBand[NIR]->Get_Value( 	p, Value, r))	{ Eval = false; return -1.0; } return Value; 								} else { Eval = false; } break;
		case CIR: 	if( m_pBand[CIR] ) 	{ if(!m_pBand[CIR]->Get_Value( 	p, Value, r))	{ Eval = false; return -1.0; } return Value; 								} else { Eval = false; } break;
		case SWIR1: if( m_pBand[SWIR1] ){ if(!m_pBand[SWIR1]->Get_Value(p, Value, r))	{ Eval = false; return -1.0; } return Value; 								} else { Eval = false; } break;
		case SWIR2: if( m_pBand[SWIR2] ){ if(!m_pBand[SWIR2]->Get_Value(p, Value, r))	{ Eval = false; return -1.0; } return Value; 								} else { Eval = false; } break;
		case TIR: 	if( m_pBand[TIR] ) 	{ if(!m_pBand[TIR]->Get_Value( 	p, Value, r))	{ Eval = false; return -1.0; } return m_bCelsius ? Value : Value -=273.15; 	} else { Eval = false; } break;
		//case NIR: 	if( m_pBand[NIR] ) 	{ if( m_pBand[NIR]->is_NoData(  x, y)) 		{ Eval = false; return -1.0; } return m_pBand[NIR]->asDouble(  x, y); 		} else { Eval = false; } break;
		//case CIR: 	if( m_pBand[CIR] ) 	{ if( m_pBand[CIR]->is_NoData(  x, y)) 		{ Eval = false; return -1.0; } return m_pBand[CIR]->asDouble(  x, y); 		} else { Eval = false; } break;
		//case SWIR1: 	if( m_pBand[SWIR1] ){ if( m_pBand[SWIR1]->is_NoData(x, y)) 		{ Eval = false; return -1.0; } return m_pBand[SWIR1]->asDouble(x, y); 		} else { Eval = false; } break;
		//case SWIR2: 	if( m_pBand[SWIR2] ){ if( m_pBand[SWIR2]->is_NoData(x, y)) 		{ Eval = false; return -1.0; } return m_pBand[SWIR2]->asDouble(x, y); 		} else { Eval = false; } break;
	}
	Eval = false;
	return( -1. );
}


//---------------------------------------------------------
bool CFmask::Set_Fmask_Pass_One( double &T_Water, double &T_Low, double &T_High, double &Lower_Level_NIR, double &Lower_Level_Swir)
{
	/*
	CSG_Simple_Statistics Clear_Sky_Water 	  = CSG_Simple_Statistics(true);
	CSG_Simple_Statistics Clear_Sky_Land 	  = CSG_Simple_Statistics(true);
	CSG_Simple_Statistics Clear_Sky_Land_Nir  = CSG_Simple_Statistics(true);
	CSG_Simple_Statistics Clear_Sky_Land_Swir = CSG_Simple_Statistics(true);
	*/
	CSG_Histogram Clear_Sky_Land(		m_Bin_Count, m_Temp_Min + m_Temp_Off, 	m_Temp_Max + m_Temp_Off  ); 	  
	CSG_Histogram Clear_Sky_Water(		m_Bin_Count, m_Temp_Min + m_Temp_Off, 	m_Temp_Max + m_Temp_Off  ); 	  
	CSG_Histogram Clear_Sky_Land_Nir(	m_Bin_Count, m_pBand[NIR]->Get_Min(), 	m_pBand[NIR]->Get_Max()  ); 	  
	CSG_Histogram Clear_Sky_Land_Swir(	m_Bin_Count, m_pBand[SWIR1]->Get_Min(), m_pBand[SWIR1]->Get_Max()); 	  

	int Rows = 0;

	#pragma omp parallel 
	{
		CSG_Histogram Local_Clear_Sky_Land(			m_Bin_Count, m_Temp_Min + m_Temp_Off,	m_Temp_Max + m_Temp_Off  ); 	  
		CSG_Histogram Local_Clear_Sky_Water(		m_Bin_Count, m_Temp_Min + m_Temp_Off, 	m_Temp_Max + m_Temp_Off  ); 	  
		CSG_Histogram Local_Clear_Sky_Land_Nir(		m_Bin_Count, m_pBand[NIR]->Get_Min(), 	m_pBand[NIR]->Get_Max()  ); 	  
		CSG_Histogram Local_Clear_Sky_Land_Swir( 	m_Bin_Count, m_pBand[SWIR1]->Get_Min(), m_pBand[SWIR1]->Get_Max()); 	  

	 	#pragma omp for
		for( int y=0; y<m_pSystem->Get_NY(); y++ )
		{
			if( SG_OMP_Get_Thread_Num() == 0 )
			{
	  			int R;
	  			#pragma omp critical 
				{ R = Rows; }
				Set_Progress( R, m_pSystem->Get_NY() );
			}

			for( int x=0; x<m_pSystem->Get_NX(); x++ )
			{
				bool bSpectral = true, bThermal = true, bCirrus = true;
				double Red 	 = Get_Brightness( x, y, RED, 	bSpectral ); 
				double Green = Get_Brightness( x, y, GREEN, bSpectral ); 
				double Blue  = Get_Brightness( x, y, BLUE, 	bSpectral ); 
				double Nir 	 = Get_Brightness( x, y, NIR,	bSpectral ); 
				double Swir1 = Get_Brightness( x, y, SWIR1, bSpectral ); 
				double Swir2 = Get_Brightness( x, y, SWIR2, bSpectral ); 
				double Tir 	 = Get_Brightness( x, y, TIR, 	bThermal  ); 
				double Cir 	 = Get_Brightness( x, y, CIR,	bCirrus   ); 

				if( bSpectral == false || (bThermal == false && m_Sensor != MSI) || (bCirrus == false && m_Sensor >= OLI_TIRS ) )
				{
					m_pResults[RESULT_PCP]->Set_Value(	x, y, 0.0 );
					m_pResults[RESULT_WATER]->Set_Value( x, y, 0.0 );
					//m_pResults[RESULT_SNOW]->Set_Value( x, y, 0.0 );
					//m_pResults[RESULT_LCP]->Set_NoData( x, y);
					m_pResults[RESULT_WCP]->Set_NoData( x, y);
					m_pResults[RESULT_PCSL]->Set_Value( x, y, 0.0 );
					m_pResults[RESULT_PCL]->Set_Value( x, y, ID_NODATA );
				}
				else
				{
					// Eq. 1
					double NDSI = (Green - Swir1) / (Green + Swir1);
					double NDVI = (Nir - Red) / (Nir + Red);
					bool Basic_Test = Swir2 > 0.03 && (m_Sensor == MSI || Tir < 27.0) && NDSI < 0.8 && NDVI < 0.8;
					
					// Eq. 20
					if( NDSI > 0.15 && (m_Sensor == MSI || Tir < m_Snow_Temp_Threshold) && Nir > 0.11 && Green > 0.1 )
					{
						m_pResults[RESULT_PCL]->Set_Value(x,y, ID_SNOW );
				//		m_pResults[RESULT_SNOW]->Set_Value( x, y, 1.0 );
				//	}
				//	else
				//	{
				//		m_pResults[RESULT_SNOW]->Set_Value( x, y, 0.0 );
					}
					
					// Eq. 2
					double MV = ( Red + Green + Blue )/3.0;
					double Whitenes = (fabs((Blue - MV)/MV) + fabs((Green - MV)/MV) + fabs((Red - MV)/MV));
					bool Whitenes_Test = Whitenes < 0.7;

					// Eq. 3
					double HOT = Blue - 0.5 * Red - 0.08; 
					bool HOT_Test = HOT > 0.0;

					// Eq. 4
					bool B4_B5_Test = Nir /  Swir1 > 0.75;

					// Eq. 5
					bool Water_Test = ( NDVI < 0.01 && Nir < 0.11 ) || ( NDVI < 0.1 && Nir < 0.05 );
					if( Water_Test ) 
					{
						m_pResults[RESULT_WATER]->Set_Value( x, y, 1.0 );
					}
					else 
					{
						m_pResults[RESULT_WATER]->Set_Value( x, y, 0.0 );
						
						// Eq. 15
						// Calculating Variability_Prob in pass bc every needed value is here.
						// Store the Variability_Prob in the lCloud_Prop Grid.
						double modNDSI = Is_Saturated( x, y, GREEN) && Swir1 > Green ? 0. : NDSI;
						double modNDVI = Is_Saturated( x, y, RED) 	&& Nir 	 > Red 	 ? 0. : NDVI;
						double Variability_Prob = 1.0 - std::max( std::abs(modNDSI), std::max( std::abs(modNDVI), Whitenes));
						m_pResults[RESULT_LCP]->Set_Value( x, y, Variability_Prob );
					}
					
					bool Cirrus_Cloud_Test = false;
					if( m_Algorithm == FMASK_3_2 && (m_Sensor == OLI_TIRS || m_Sensor == MSI) )
					{
						Cirrus_Cloud_Test = Cir > 0.01;
					}
					
					// Eq. 6
					if( (Basic_Test && Whitenes_Test && HOT_Test && B4_B5_Test) || Cirrus_Cloud_Test )
					{
						m_pResults[RESULT_PCP]->Set_Value(x,y, 1.0 );
					}
					else
					{
						m_pResults[RESULT_PCP]->Set_Value(x,y, 0.0 );

						// Eq. 7
						if( Water_Test &&  Swir2 < 0.03 )
						{	
							Local_Clear_Sky_Water += Tir;
						}

						// Eq. 12
						if( !Water_Test )
						{
							Local_Clear_Sky_Land 		+= Tir;
							Local_Clear_Sky_Land_Nir 	+= Nir;
							Local_Clear_Sky_Land_Swir 	+= Swir1;
						}
					}
				}
			}
			#pragma omp critical 
			{ Rows++; }
		}

		#pragma omp critical 
		{
			Clear_Sky_Water.Add_Histogram(Local_Clear_Sky_Water);
			Clear_Sky_Land.Add_Histogram(Local_Clear_Sky_Land);
			Clear_Sky_Land_Nir.Add_Histogram(Local_Clear_Sky_Land_Nir);
			Clear_Sky_Land_Swir.Add_Histogram(Local_Clear_Sky_Land_Swir);
		}
	}

	// Eq. 8
	Clear_Sky_Water.Update();
	T_Water 	= Clear_Sky_Water.Get_Percentile(82.5);
	// Eq. 13
	Clear_Sky_Land.Update();
	T_Low 	= Clear_Sky_Land.Get_Percentile(17.5);
	T_High 	= Clear_Sky_Land.Get_Percentile(82.5);

	Clear_Sky_Land_Nir.Update();
	Lower_Level_NIR = Clear_Sky_Land_Nir.Get_Percentile(17.5);
	Clear_Sky_Land_Swir.Update();
	Lower_Level_Swir = Clear_Sky_Land_Swir.Get_Percentile(17.5);

	return( true );
}


bool CFmask::Set_Fmask_Pass_Two( const double T_Water, const double T_Low, const double T_High, double &Land_threshold, double &Water_threshold )
{

	// TODO This could be a Histogram but i need the range to Initialize it.
	CSG_Simple_Statistics Clear_Sky_Cloud_Prop_Land(  true ); 	  
	CSG_Simple_Statistics Clear_Sky_Cloud_Prop_Water( true ); 	  
			
	int Rows = 0;

	#pragma omp parallel
	{
		CSG_Simple_Statistics Local_Clear_Sky_Cloud_Prop_Land(  true ); 	  
		CSG_Simple_Statistics Local_Clear_Sky_Cloud_Prop_Water( true ); 	  
	
		#pragma omp for
		for( int y=0; y<m_pSystem->Get_NY(); y++ )
		{
			if( SG_OMP_Get_Thread_Num() == 0 )
			{
	  			int R;
	  			#pragma omp critical 
				{ R = Rows; }
				Set_Progress( R, m_pSystem->Get_NY() );
			}
			for( int x=0; x<m_pSystem->Get_NX(); x++ )
			{
				if( m_pResults[RESULT_PCL]->asInt(x,y) != ID_NODATA )
				{
					bool bDummy;
					double Swir1= Get_Brightness( x, y, SWIR1, bDummy ); 
					double Swir2= Get_Brightness( x, y, SWIR2, bDummy ); 
					double Tir 	= Get_Brightness( x, y, TIR,   bDummy ); 
					double Cir 	= Get_Brightness( x, y, CIR,   bDummy ); 

					double Cirrus_Prob = 0.;
					if( m_Algorithm == FMASK_3_2 && (m_Sensor == OLI_TIRS || m_Sensor == MSI) )
					{
						Cirrus_Prob = Cir / 0.04;
					}

					// Eq. 9
					double wTemp_Prob = m_Sensor == MSI ? 1.0 : (T_Water - Tir) / 4.;

					// Eq. 10
					double Brightness_Prob = std::min( Swir1, 0.11 ) / 0.11;

					// Eq. 11	
					double wCloud_Prop = (wTemp_Prob * Brightness_Prob) + Cirrus_Prob;
					

					m_pResults[RESULT_WCP]->Set_Value( x, y, wCloud_Prop );


					// Eq. 14
					double lTemp_Prob = m_Sensor == MSI ? 1.0 : ( T_High + 4.0 - Tir ) / ( T_High + 4.0 - ( T_Low - 4.0 )); 
					// The Variability_Prob is actually calculated in pass one and stored in the lCloud_Prop Grid.
					// Now recover the value and overwrite with lCloud_Prop after calculation.
					double Variability_Prob = m_pResults[RESULT_LCP]->asDouble( x, y);
					// Eq. 16
					double lCloud_Prop = (lTemp_Prob * Variability_Prob) + Cirrus_Prob;

					m_pResults[RESULT_LCP]->Set_Value( x, y, lCloud_Prop );

					if( m_pResults[RESULT_PCP]->asInt(x,y) == 0 )
					{
						Local_Clear_Sky_Cloud_Prop_Land += lCloud_Prop;
					}
						
					if( m_pResults[RESULT_WATER]->asInt(x,y) == 1 &&  Swir2 < 0.03 )
					{
						Local_Clear_Sky_Cloud_Prop_Water += wCloud_Prop;
					}
				}
			}
	  		#pragma omp critical 
			{ Rows++; }
		}
		#pragma omp critical 
		{
			Clear_Sky_Cloud_Prop_Land 	+= Local_Clear_Sky_Cloud_Prop_Land;
			Clear_Sky_Cloud_Prop_Water 	+= Local_Clear_Sky_Cloud_Prop_Water;
		}
	}

	// Eq. 17
	//Clear_Sky_Land_Cloud_Prop.Update();
	Land_threshold = Clear_Sky_Cloud_Prop_Land.Get_Percentile(82.5) + 0.2;

	// v.1.6 default Value and v. 3.2 Cloud Detection over Water value
	Water_threshold = m_Algorithm == FMASK_1_6 ? 0.5 : Clear_Sky_Cloud_Prop_Water.Get_Percentile(82.5) + 0.2;

	return( true );
}

bool CFmask::Set_Cloud_Mask( const double T_Low, const double Land_threshold, const double Water_threshold )
{
	int* x_off = m_xKernel_Improve_Cloud.Get_Array();
	int* y_off = m_yKernel_Improve_Cloud.Get_Array();
	int  s_off = m_xKernel_Improve_Cloud.Get_Size();
	
	for( int y=0; y<m_pSystem->Get_NY() && Set_Progress(y,m_pSystem->Get_NY()*2); y++ )
	{
		#pragma omp parallel for
		for( int x=0; x<m_pSystem->Get_NX(); x++ )
		{
			if( m_pResults[RESULT_PCL]->asInt(x,y) != ID_NODATA )
			{
				bool bDummy;
				double Tir  = Get_Brightness( x, y, TIR, bDummy ); 
				double Nir  = Get_Brightness( x, y, NIR, bDummy );
				double Swir1= Get_Brightness( x, y, NIR, bDummy );
			
				// Equation 19
				if( m_Algorithm == FMASK_1_6 )
				{
					m_pResults[RESULT_PCSL]->Set_Value(x,y, m_pResults[RESULT_FFN]->asFloat(x,y) - Nir > 0.02 ? 1.0 : 0.0);
				}
				else 
				{
					m_pResults[RESULT_PCSL]->Set_Value(x,y, m_pResults[RESULT_FFN]->asFloat(x,y) - Nir > 0.02 && m_pResults[RESULT_FFN]->asFloat(x,y) - Swir1 > 0.02 ? 1.0 : 0.0);
				}

				// Equation. 18
				if( ( m_pResults[RESULT_WATER]->asInt(x,y) == 1 && m_pResults[RESULT_PCP]->asInt(x,y) == 1 && m_pResults[RESULT_WCP]->asFloat(x,y) > Water_threshold)
				||  ( m_pResults[RESULT_WATER]->asInt(x,y) == 0 && m_pResults[RESULT_PCP]->asInt(x,y) == 1 && m_pResults[RESULT_LCP]->asFloat(x,y) > Land_threshold) 
				||  ( m_Algorithm == FMASK_1_6 && m_pResults[RESULT_WATER]->asInt(x,y) == 0 && m_pResults[RESULT_LCP]->asFloat(x,y) > 0.99)
				||	( m_Sensor != MSI && Tir < T_Low - 35.0 ) )
				{
					m_pResults[RESULT_PCL]->Set_Value(x,y, ID_CLOUD);
				}
			}
	 	}
	}

	for( int y=1; y<m_pSystem->Get_NY()-1 && Set_Progress(y+m_pSystem->Get_NY(),m_pSystem->Get_NY()*2); y++ )
	{
		#pragma omp parallel for
		for( int x=1; x<m_pSystem->Get_NX()-1; x++ )
		{
			int count = 0;
			for(int i=0; i<s_off; i++)
			{
				int ix = x + x_off[i];
				int iy = y + y_off[i];

				if( m_pResults[RESULT_PCL]->asInt(ix,iy) == ID_CLOUD )
	 			{
					count++;
				}
			}
			
			if( count >= m_Improve_Cloud_Count )
			{
				m_pResults[RESULT_PCL]->Set_Value(x,y, ID_CLOUD);
			}
	 	}
	}

	return( true );
}
	
bool CFmask::Set_Final_Fmask( void )
{
	int* x_off = m_xKernel_Dilation_Cloud.Get_Array();
	int* y_off = m_yKernel_Dilation_Cloud.Get_Array();
	int  s_off = m_xKernel_Dilation_Cloud.Get_Size();

	for( int y=1; y<m_pSystem->Get_NY()-1 && Set_Progress(y,m_pSystem->Get_NY()); y++ )
	{
		#pragma omp parallel for
		for( int x=1; x<m_pSystem->Get_NX()-1; x++ )
		{
			for(int i=0; i<8; i++)
			{
				int ix = x + xnb[i];
				int iy = y + ynb[i];
				
				if( m_pResults[RESULT_PCL]->asInt( x, y) != ID_CLOUD 
				&&	m_pResults[RESULT_PCL]->asInt(ix,iy) == ID_CLOUD )
	 			{
					for(int j=0; j<s_off; j++)
					{
						int jx = x + x_off[j];
						int jy = y + y_off[j];

						if( m_pSystem->is_InGrid(jx,jy)
		 				&&	m_pResults[RESULT_PCL]->asInt(jx,jy) != ID_CLOUD 
		 				&&	m_pResults[RESULT_PCL]->asInt(jx,jy) != ID_NODATA )
	 					{
	 						m_pResults[RESULT_PCL]->Set_Value(jx,jy, ID_CLOUD_DILATED);
	 					}
	 				}
					break;
				}
			}
	 	}
	}
	return( true );
}



bool CFmask::Set_Shadow_Mask( const double T_Low, const double T_High )
{

	/*
	std::vector<std::pair<int,int>> Threads( SG_OMP_Get_Max_Num_Threads() );

	int 	Pixel_Thread = m_pResults[RESULT_PCP]->Get_Data_Count() / SG_OMP_Get_Max_Num_Threads();
	int 	Thread_Num = 0;
	sLong 	Pixel_Count = 0;

	Threads[Thread_Num].first = 0;
	for( int y=0; y<m_pSystem.Get_NY(); y++ )
	{
		for( int x=0; x<m_pSystem.Get_NX(); x++ )
		{
			if( m_pResults[RESULT_PCP]->is_NoData(x,y) )
			{
				Pixel_Count++;
			}

			if( Pixel_Thread * Thread_Num > Pixel_Count )
			{
				Thread_Num++;
				Threads[Thread_Num-1].second = y;
				Threads[Thread_Num  ].first = y;
			}
	 	}
	}
	*/




	std::vector<CSG_Cloud_Stack> Leftovers;

	//int Chunk = 3473;
	//
	//Get_Segmentation( m_pResults[RESULT_PCL], &Leftovers, T_Low, T_High, 0, m_pSystem.Get_NX(), 0, Chunk );
	//
	//Get_Segmentation( m_pResults[RESULT_PCL], &Leftovers, T_Low, T_High, 0, m_pSystem.Get_NX(), Chunk, m_pSystem.Get_NY() );

	Get_Segmentation( &Leftovers, T_Low, T_High, 0, m_pSystem->Get_NX(), 0, m_pSystem->Get_NY() );
	/*
	for( int i=0; i<Leftovers.size(); i++ )
	{
		for( int j=0; j<Leftovers.size(); j++ )
		{
			if( i != j && Leftovers[i].Get_Size() && Leftovers[j].Get_Size() )
			{
				if( Leftovers[i].Overlap(Leftovers[j]) )
				{
					Leftovers[i].Merge(Leftovers[j]);
					Leftovers[j].Destroy();
				}

			}
		}
	}
	*/



	return Leftovers.size();
}

bool CFmask::Get_Flood_Fill( double Boundary, int Band_Input, int Band_Output )
{

	CSG_Tool	*pTool = SG_Get_Tool_Library_Manager().Create_Tool("ta_preprocessor", 8, false);

	if( pTool == NULL )
		return false;
	
	bool	bResult	=
		  	pTool->Set_Parameter("DEM", 	m_pBand[Band_Input])
		&&  pTool->Set_Parameter("BOUNDARY", true )
		&&  pTool->Set_Parameter("METHOD", 1 )
		&&  pTool->Set_Parameter("BOUNDARY_VALUE", Boundary )
		&&  pTool->Execute();
		
	if( bResult )
	{
		m_pResults[Band_Output] = pTool->Get_Parameter("RESULT")->asGrid();
	}

	return bResult && m_pResults[Band_Output] && SG_Get_Tool_Library_Manager().Delete_Tool(pTool);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFmask::Get_Segmentation(std::vector<CSG_Cloud_Stack> *Array, const double T_Low, const double T_High, const int xStart, const int xEnd, const int yStart, const int yEnd)
{

	#ifdef _DEBUG
	unsigned int SegmentCount = 1;
	#endif


	CSG_Grid Tick_Off( *m_pSystem, m_Bool_Type);
	Tick_Off.Set_NoData_Value(0);
	Tick_Off.Assign_NoData();

	bool Merge = false;

	CSG_Grid_Stack 			Stack;
	CSG_Histogram 			Cloud_Temps(m_Bin_Count, m_Temp_Min + m_Temp_Off, m_Temp_Max + m_Temp_Off ); 	  
	CSG_Cloud_Stack 		Cloud_Stack;
	bool 					Dummy; 
	double 					Temp_Minimum = m_Temp_Max;

	for(int y=yStart; y<yEnd; y++)
	{
		for(int x=xStart; x<xEnd; x++)
		{
			if( m_pResults[RESULT_PCL]->asInt(x,y) != ID_CLOUD )
			{
				Tick_Off.Set_Value(x, y, 1 );
			}
			else
			{

				if( Tick_Off.is_NoData(x, y) )
				{
					Tick_Off.Set_Value(x, y, 1 );
					
					Stack.Clear(); Cloud_Stack.Clear();
					Stack.Push(x, y); 
					Cloud_Stack.Push(x,y);

					Cloud_Temps.Create(m_Bin_Count, m_Temp_Min + m_Temp_Off, m_Temp_Max + m_Temp_Off ); 	  
					Temp_Minimum = m_Temp_Max;

					double Tir = Get_Brightness( x, y, TIR, Dummy );
					Cloud_Temps += Tir;
					Temp_Minimum = std::min( Tir, Temp_Minimum );

					#ifdef _DEBUG
					SegmentCount++;
					m_pResults[RESULT_SEG]->Set_Value( x, y, SegmentCount );
					#endif

					while( Stack.Get_Size() > 0 ) //&& Process_Get_Okay() )
					{
						int nx, ny;
						Stack.Pop(nx, ny);

						for(int i=0; i<8; i++)
						{
							int ix = nx + xnb[i];
							int iy = ny + ynb[i];

							if(	m_pSystem->is_InGrid(ix, iy) 	&& m_pResults[RESULT_PCL]->asInt(ix,iy) == ID_CLOUD 
							&&  Tick_Off.is_NoData(ix, iy) 		&& iy >= yStart-1 && iy <= yEnd )
							{
								if( iy == yStart-1 || iy == yEnd )
								{
									Merge = true;
								}

								double Tir = Get_Brightness( x, y, TIR, Dummy );
								Cloud_Temps += Tir;
								Temp_Minimum = std::min( Tir, Temp_Minimum );
								
								Stack.Push(ix, iy); 
								Cloud_Stack.Push(ix, iy); 
								Tick_Off.Set_Value(ix, iy, 1);
								
								#ifdef _DEBUG
								m_pResults[RESULT_SEG]->Set_Value( x, y, SegmentCount );
								#endif
							}
						}
					}

					Cloud_Temps.Update();
					CSG_Cloud_Stack Cloud_Shape;
					if( Merge )
					{
						#pragma omp critical
						{
							Array->push_back(Cloud_Stack);
							Merge = false;
						}
					}
					else
					{
						if( Cloud_Stack.Get_Size() < m_Min_Cloud_Size )
						{
							for( sLong i=0; i<Cloud_Stack.Get_Size(); i++ )
							{
								m_pResults[RESULT_PCL]->Set_Value( Cloud_Stack[i].x, Cloud_Stack[i].y, ID_NONE );
							}
						}
						else
						{
							// Eq. 22
							double R = sqrt( Cloud_Stack.Get_Size() / (2*M_PI) );
							double T_Cloud_base = Temp_Minimum;
							if( R >= m_Shadow_Radius )
							{
								T_Cloud_base = Cloud_Temps.Get_Percentile( 100. * pow((R-m_Shadow_Radius),2) / pow(R,2) ); 
							}

							double H_Cloud_base_min = 0.0;
							double H_Cloud_base_max = 0.0;
							if( m_Sensor == TM || m_Sensor == ETM || m_Sensor == OLI_TIRS )
							{
								Get_3D_Shadow( &Cloud_Stack, &Cloud_Shape, T_Cloud_base );
								// Eq. 21 
								H_Cloud_base_min = std::max( 0.2, (T_Low  - 4. - T_Cloud_base)/9.8 ) * 1000.; 
								H_Cloud_base_max = std::min( 12., (T_High + 4. - T_Cloud_base) )	 * 1000.;	
							}
							else
							{
								Cloud_Shape = Cloud_Stack;
								H_Cloud_base_min =  200;
								H_Cloud_base_max = 1200;
							}
							
							Cloud_Stack.Clear();

							Get_Shadow_Match( &Cloud_Shape, H_Cloud_base_min, H_Cloud_base_max );
						}
	 				}
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
bool CFmask::Get_3D_Shadow( CSG_Cloud_Stack *pInputStack, CSG_Cloud_Stack *pOutputStack, const double T_Cloud_base )
{
	bool bDummy;
	for( sLong i=0; i<pInputStack->Get_Size(); i++ )
	{
		int ix = (*pInputStack)[i].x;
		int iy = (*pInputStack)[i].y;	  						
		
		double Temp = Get_Brightness( ix, iy, TIR, bDummy );
		// Eq. 23 (Condensed) & 24
		double H_Top_relative = ((T_Cloud_base - std::min(Temp, T_Cloud_base)) / 6.5) * 1000;

		int cast_x = (int)( ix + (m_dx * H_Top_relative / m_dz));
		int cast_y = (int)( iy + (m_dy * H_Top_relative / m_dz));

		if( m_pSystem->is_InGrid( cast_x, cast_y ) )
		{
			pOutputStack->Push(cast_x, cast_y);
			
			#ifdef _DEBUG
			m_pResults[RESULT_CAST]->Set_Value( cast_x, cast_y, 1 );
			#endif
		}
	}

	pOutputStack->Sort();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFmask::Get_Shadow_Match( CSG_Grid_Stack *pCloud_Shape, const double H_Cloud_base_min, const double H_Cloud_base_max )
{
	int* x_off = m_xKernel_Fill_Shadow.Get_Array();
	int* y_off = m_yKernel_Fill_Shadow.Get_Array();
	int  s_off = m_xKernel_Fill_Shadow.Get_Size();
	
	int steps = (int)((H_Cloud_base_max - H_Cloud_base_min) / m_dz);

	int 	Best_Step 		= 0; 
	double 	Best_Similarity = 0.0;
	bool 	Match = false;

	for( int iz=0; iz<=steps; iz++ )
	{
		double z = H_Cloud_base_min + iz * m_dz;
		
		int n = pCloud_Shape->Get_Size();
		int count = 0;

		for( sLong i=0; i<pCloud_Shape->Get_Size(); i++ )
		{
			int cast_x = (int)( (*pCloud_Shape)[i].x + (m_dx * z / m_dz));
			int cast_y = (int)( (*pCloud_Shape)[i].y + (m_dy * z / m_dz));

			if( m_pSystem->is_InGrid( cast_x, cast_y ) )
			{
				if( m_pResults[RESULT_PCL]->asInt( cast_x, cast_y ) == ID_CLOUD )
				{
					n--; 
				}
				else if( m_pResults[RESULT_PCSL]->asInt( cast_x, cast_y ) == 1 )
				{
					count++;
				}
			}
		}

		double Similarity = (double) count / (double) n;

		if( Similarity <= Best_Similarity )
		{
			if( Match && Similarity < (Best_Similarity * m_Decrease_Threshold) )
			{
				break;
			}
		}
		else
		{
			Best_Similarity = Similarity;
			if( Similarity > m_Similarity_Threshold )
			{
				Match 		= true;
				Best_Step 	= iz;
			}
		}
	}

	if( Match )
	{

		double z = H_Cloud_base_min + Best_Step * m_dz;
		for( sLong i=0; i<pCloud_Shape->Get_Size(); i++ )
		{
			int cast_x = (int)( (*pCloud_Shape)[i].x + (m_dx * z / m_dz));
			int cast_y = (int)( (*pCloud_Shape)[i].y + (m_dy * z / m_dz));

			for( int diff=0; diff<s_off; diff++ )
			{
				int diff_x = cast_x + x_off[diff];
				int diff_y = cast_y + y_off[diff];

				if( m_pSystem->is_InGrid( diff_x, diff_y ) ) 
				{
					int Value = m_pResults[RESULT_PCL]->asInt( diff_x, diff_y);
					if( Value == ID_NONE || Value == ID_SNOW )
					{
						m_pResults[RESULT_PCL]->Set_Value( diff_x, diff_y, ID_SHADOW );
					}
				}
	 		}
		}
	}

	return( Match );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
