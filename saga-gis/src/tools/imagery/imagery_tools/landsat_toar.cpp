/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                     imagery_tools                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   landsat_toar.cpp                    //
//                                                       //
//                 Copyright (C) 2013 by                 //
//            Benjamin Bechtel & Olaf Conrad             //
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
#include "landsat_toar.h"

//---------------------------------------------------------
#include "landsat_toar_core.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
enum
{
	mss1 = 0,	// Landsat-1 MSS
	mss2,		// Landsat-2 MSS
	mss3,		// Landsat-3 MSS
	mss4,		// Landsat-4 MSS
	mss5,		// Landsat-5 MSS
	tm4,		// Landsat-4 TM
	tm5,		// Landsat-5 TM
	tm7,		// Landsat-7 ETM+
	oli8		// Landsat-8 OLI/TIRS
};

//---------------------------------------------------------
#define BAND_ID(   sensor, id)	CSG_String::Format("%s%02d", CSG_String(sensor).c_str(), id)
#define BAND_INPUT(sensor, id)	Parameters.Add_Grid(sensor, BAND_ID(sensor, id), CSG_String::Format("%s %d", _TL("Band"), id), _TL(""), PARAMETER_INPUT_OPTIONAL);


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool	Set_Date_from_String	(char *date, const CSG_String s)
{
	// yyyy-mm-dd
	// 0123456789

	if( s.Length() == 10 )
	{
		strncpy(date, s.b_str(), 11);

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool	Load_MetaFile	(const CSG_String &MetaFile, lsat_data &lsat)
{
	return( SG_File_Exists(MetaFile) && lsat_metadata(MetaFile, &lsat) );
}

//---------------------------------------------------------
int		Get_Sensor_Index	(int LSat_Number, const CSG_String &LSat_Sensor)
{
	if( !LSat_Sensor.CmpNoCase("MSS") )
	{
		switch( LSat_Number )
		{
		case 1:	return( 0 );	// Landsat-1 MSS
		case 2:	return( 1 );	// Landsat-2 MSS
		case 3:	return( 2 );	// Landsat-3 MSS
		case 4:	return( 3 );	// Landsat-4 MSS
		case 5:	return( 4 );	// Landsat-5 MSS
		}
	}
	else if( !CSG_String(LSat_Sensor).CmpNoCase("TM") )
	{
		switch( LSat_Number )
		{
		case 4:	return( 5 );	// Landsat-1 MSS
		case 5:	return( 6 );	// Landsat-2 MSS
		}
	}
	else if( !CSG_String(LSat_Sensor).Find("ETM") && LSat_Number == 7 )
	{
		return( 7 );	// Landsat-7 ETM+
	}
	else if( !CSG_String(LSat_Sensor).Find("OLI") && LSat_Number == 8 )
	{
		return( 8 );	// Landsat-8 OLI/TIRS
	}

	return( -1 );
}

//---------------------------------------------------------
CSG_String	Get_Band_Name	(int Band, int Sensor)
{
	switch( Sensor )
	{
	case mss1: case mss2: case mss3: case mss4: case mss5:
		switch( Band )
		{
		case  0: return( _TL("Green") );
		case  1: return( _TL("Red"  ) );
		case  2: return( _TL("NIR 1") );
		case  3: return( _TL("NIR 2") );
		}
		break;

	case tm4: case tm5:
		switch( Band )
		{
		case  0: return( _TL("Blue"   ) );
		case  1: return( _TL("Green"  ) );
		case  2: return( _TL("Red"    ) );
		case  3: return( _TL("NIR"    ) );
		case  4: return( _TL("SWIR 1" ) );
		case  5: return( _TL("Thermal") );
		case  6: return( _TL("SWIR 2" ) );
		}
		break;

	case tm7:
		switch( Band )
		{
		case  0: return( _TL("Blue"               ) );
		case  1: return( _TL("Green"              ) );
		case  2: return( _TL("Red"                ) );
		case  3: return( _TL("NIR"                ) );
		case  4: return( _TL("SWIR 1"             ) );
		case  5: return( _TL("Thermal (low gain)" ) );
		case  6: return( _TL("Thermal (high gain)") );
		case  7: return( _TL("SWIR 2"             ) );
		case  8: return( _TL("Panchromatic"       ) );
		}
		break;

	case oli8:
		switch( Band )
		{
		case  0: return( _TL("Coast & Aerosol") );
		case  1: return( _TL("Blue"           ) );
		case  2: return( _TL("Green"          ) );
		case  3: return( _TL("Red"            ) );
		case  4: return( _TL("NIR"            ) );
		case  5: return( _TL("SWIR 1"         ) );
		case  6: return( _TL("SWIR 2"         ) );
		case  7: return( _TL("Panchromatic"   ) );
		case  8: return( _TL("Cirrus"         ) );
		case  9: return( _TL("Thermal 1"      ) );
		case 10: return( _TL("Thermal 2"      ) );
		}
		break;
	}

	return( "" );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CLandsat_TOAR::CLandsat_TOAR(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Top of Atmosphere Reflectance"));

	Set_Author		("B.Bechtel, O.Conrad (c) 2013");

	Set_Description	(_TW(
		"Calculation of top-of-atmosphere radiance or reflectance and temperature (TOAR) for Landsat MSS/TM/ETM+. "
		"This tool incorporates E.J. Tizado's GRASS GIS implementation (i.landsat.toar)."
	));

	Add_Reference("https://grass.osgeo.org/grass72/manuals/i.landsat.toar.html",
		SG_T("GRASS i.landsat.toar")
	);

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
	Parameters.Add_Grid_System("", "MSS"  , _TL("Spectral"    ), _TL(""));
	BAND_INPUT("MSS", 1); BAND_INPUT("MSS", 2); BAND_INPUT("MSS", 3); BAND_INPUT("MSS", 4);

	Parameters.Add_Grid_System("", "TM"   , _TL("Spectral"    ), _TL(""));
	BAND_INPUT("TM" , 1); BAND_INPUT("TM" , 2); BAND_INPUT("TM" , 3); BAND_INPUT("TM" , 4); BAND_INPUT("TM" , 5); BAND_INPUT("TM" , 7);

	Parameters.Add_Grid_System("", "ETM"  , _TL("Spectral"    ), _TL(""));
	BAND_INPUT("ETM", 1); BAND_INPUT("ETM", 2); BAND_INPUT("ETM", 3); BAND_INPUT("ETM", 4); BAND_INPUT("ETM", 5); BAND_INPUT("ETM", 7);

	Parameters.Add_Grid_System("", "OLI"  , _TL("Spectral"    ), _TL(""));
	BAND_INPUT("OLI", 1); BAND_INPUT("OLI", 2); BAND_INPUT("OLI", 3); BAND_INPUT("OLI", 4); BAND_INPUT("OLI", 5); BAND_INPUT("OLI", 6); BAND_INPUT("OLI", 7); BAND_INPUT("OLI", 9);

	Parameters.Add_Grid_System("", "TM_T" , _TL("Thermal"     ), _TL(""));
	BAND_INPUT("TM_T", 6);

	Parameters.Add_Grid_System("", "ETM_T", _TL("Thermal"     ), _TL(""));
	BAND_INPUT("ETM_T", 61); BAND_INPUT("ETM_T", 62);

	Parameters.Add_Grid_System("", "TIRS" , _TL("Thermal"     ), _TL(""));
	BAND_INPUT("TIRS", 10); BAND_INPUT("TIRS", 11);

	Parameters.Add_Grid_System("", "PAN"  , _TL("Panchromatic"), _TL(""));
	BAND_INPUT("PAN", 8);

	Parameters.Add_Grid_List("", "SPECTRAL", _TL("Spectral"    ), _TL(""), PARAMETER_OUTPUT, false);
	Parameters.Add_Grid_List("", "THERMAL" , _TL("Thermal"     ), _TL(""), PARAMETER_OUTPUT, false);
	Parameters.Add_Grid_List("", "PANBAND" , _TL("Panchromatic"), _TL(""), PARAMETER_OUTPUT, false);

	//-----------------------------------------------------
	Parameters.Add_FilePath("",
		"METAFILE"	, _TL("Metadata File"),
		_TL("Name of Landsat metadata file (.met or MTL.txt)"),
		CSG_String::Format("%s|*.met;*.txt|%s|*.*",
			_TL("Metadata Files (*.met, *.txt)"),
			_TL("All Files")
		) 
	);

	//-----------------------------------------------------
	Parameters.Add_Choice("",
		"SENSOR"	, _TL("Spacecraft Sensor"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s|%s|%s|%s|%s|%s|",
			_TL("Landsat-1 MSS"),
			_TL("Landsat-2 MSS"),
			_TL("Landsat-3 MSS"),
			_TL("Landsat-4 MSS"),
			_TL("Landsat-5 MSS"),
			_TL("Landsat-4 TM"),
			_TL("Landsat-5 TM"),
			_TL("Landsat-7 ETM+"),
			_TL("Landsat-8 OLI/TIRS")
		), 7
	);

	//-----------------------------------------------------
	Parameters.Add_String("",
		"DATE_ACQU"	, _TL("Image Acquisition Date"),
		_TL("Image acquisition date (yyyy-mm-dd)"),
		"2001-01-01"
	);

	Parameters.Add_String("",
		"DATE_PROD"	, _TL("Image Creation Date"),
		_TL("Image creation date (yyyy-mm-dd)"),
		"2001-01-01"
	);

	Parameters.Add_Double("",
		"SUN_HGT"	, _TL("Suns's Height"),
		_TL("Sun's height above horizon in degree"),
		45.0, 0.0, true, 90.0, true
	);

	Parameters.Add_Bool("",
		"GRIDS_OUT"	, _TL("Spectral Output as Grid Collection"),
		_TL(""),
		true
	);

	Parameters.Add_String("GRIDS_OUT",
		"GRIDS_NAME", _TL("Name"),
		_TL(""),
		"Landsat Calibrated"
	);

	Parameters.Add_Bool("",
		"AS_RAD"	, _TL("At-Sensor Radiance"),
		_TL("Output at-sensor radiance for all bands"),
		false
	);

	//-----------------------------------------------------
	Parameters.Add_Choice("",
		"AC_METHOD"	, _TL("Atmospheric Correction"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s|%s|%s|%s|",
			_TL("uncorrected"),
			_TL("corrected"),
			_TL("dark object subtraction 1"),
			_TL("dark object subtraction 2"),
			_TL("dark object subtraction 2b"),
			_TL("dark object subtraction 3"),
			_TL("dark object subtraction 4")
		), 0
	);

	Parameters.Add_Int("AC_METHOD",
		"AC_DO_CELLS"	, _TL("Minimum Number of Dark Object Cells"),
		_TL("Minimum pixels to consider digital number as dark object"),
		1000, 0, true
	);

	Parameters.Add_Double("AC_METHOD",
		"AC_RAYLEIGH"	, _TL("Rayleigh Scattering"),
		_TL("Rayleigh atmosphere (diffuse sky irradiance)"),
		0.0, 0.0, true
	);

	Parameters.Add_Double("AC_METHOD",
		"AC_SUN_RAD"	, _TL("Solar Radiance"),
		_TL("Percent of solar radiance in path radiance"),
		1.0, 0.0, true, 100.0, true
	);

	//-----------------------------------------------------
	Parameters.Add_Node("",
		"ETM_GAIN"	, _TL("ETM+ Gain"),
		_TL("Gain (H/L) of all Landsat ETM+ bands (1-5,61,62,7,8)")
	);

	CSG_String	s;	s.Printf("%s|%s|", _TL("low"), _TL("high"));

	Parameters.Add_Choice("ETM_GAIN", "ETM_GAIN_10", _TL("Band 1" ), _TL(""), s, 1);
	Parameters.Add_Choice("ETM_GAIN", "ETM_GAIN_20", _TL("Band 2" ), _TL(""), s, 1);
	Parameters.Add_Choice("ETM_GAIN", "ETM_GAIN_30", _TL("Band 3" ), _TL(""), s, 1);
	Parameters.Add_Choice("ETM_GAIN", "ETM_GAIN_40", _TL("Band 4" ), _TL(""), s, 1);
	Parameters.Add_Choice("ETM_GAIN", "ETM_GAIN_50", _TL("Band 5" ), _TL(""), s, 1);
	Parameters.Add_Choice("ETM_GAIN", "ETM_GAIN_61", _TL("Band 61"), _TL(""), s, 0);
	Parameters.Add_Choice("ETM_GAIN", "ETM_GAIN_62", _TL("Band 62"), _TL(""), s, 1);
	Parameters.Add_Choice("ETM_GAIN", "ETM_GAIN_70", _TL("Band 7" ), _TL(""), s, 1);
	Parameters.Add_Choice("ETM_GAIN", "ETM_GAIN_80", _TL("Band 8" ), _TL(""), s, 0);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CLandsat_TOAR::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	pParameter->Cmp_Identifier("METAFILE") && *pParameter->asString() )
	{
		lsat_data	lsat;

		if( !Load_MetaFile(pParameter->asString(), lsat) )
		{
			pParameter->Set_Value((const char *)"");

			Error_Set(_TL("could not read metadata file"));
		}
		else
		{
			pParameters->Set_Parameter("SENSOR"   , Get_Sensor_Index(lsat.number, lsat.sensor));
			pParameters->Set_Parameter("DATE_ACQU", (const char *)lsat.date);
			pParameters->Set_Parameter("DATE_PROD", (const char *)lsat.creation);
			pParameters->Set_Parameter("SUN_HGT"  , lsat.sun_elev);

			On_Parameters_Enable(pParameters, (*pParameters)("SENSOR"));
		}
	}

	if(	pParameter->Cmp_Identifier("SENSOR") )
	{
		(*pParameters)("METAFILE")->Set_Value((const char *)"");
	}

	return( CSG_Tool::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
int CLandsat_TOAR::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	pParameter->Cmp_Identifier("AC_METHOD") )
	{
		pParameters->Set_Enabled("AC_DO_CELLS", pParameter->asInt() > 1);
		pParameters->Set_Enabled("AC_RAYLEIGH", pParameter->asInt() > 1);
		pParameters->Set_Enabled("AC_SUN_RAD" , pParameter->asInt() > 1);
	}

	if( pParameter->Cmp_Identifier("METAFILE")
	||  pParameter->Cmp_Identifier("SENSOR"  ) )
	{
		int	Sensor	= (*pParameters)("SENSOR")->asInt();

		pParameters->Set_Enabled("MSS"        , Sensor <= mss5);
		pParameters->Set_Enabled("TM"         , Sensor >= tm4 && Sensor <= tm5);
		pParameters->Set_Enabled("TM_T"       , Sensor >= tm4 && Sensor <= tm5);
		pParameters->Set_Enabled("ETM"        , Sensor == tm7);
		pParameters->Set_Enabled("ETM_T"      , Sensor == tm7);
		pParameters->Set_Enabled("ETM_GAIN"   , Sensor == tm7 && *(*pParameters)("METAFILE")->asString() == '\0');
		pParameters->Set_Enabled("OLI"        , Sensor == oli8);
		pParameters->Set_Enabled("TIRS"       , Sensor == oli8);
		pParameters->Set_Enabled("PAN"        , Sensor >= tm7);
	}

	if(	pParameter->Cmp_Identifier("GRIDS_OUT") )
	{
		pParameters->Set_Enabled("GRIDS_NAME" , pParameter->asBool());
	}

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Grid * CLandsat_TOAR::Get_Band_Input(int Band, int Sensor)
{
	CSG_String	id;	Band++;

	switch( Sensor )
	{
	case mss1: case mss2: case mss3: case mss4: case mss5:
		id	=             BAND_ID("MSS"  , Band);
		break;

	case tm4: case tm5:
		id	= Band == 6 ? BAND_ID("TM_T" , Band)
			:             BAND_ID("TM"   , Band);
		break;

	case tm7:
		id	= Band == 6 ? BAND_ID("ETM_T", 61  )
			: Band == 7 ? BAND_ID("ETM_T", 62  )
			: Band == 8 ? BAND_ID("ETM"  , 7   )
			: Band == 9 ? BAND_ID("PAN"  , 8   )
			:             BAND_ID("ETM"  , Band);
		break;

	case oli8:
		id	= Band ==  8 ? BAND_ID("PAN" , Band)
			: Band == 10 ? BAND_ID("TIRS", Band)
			: Band == 11 ? BAND_ID("TIRS", Band)
			:              BAND_ID("OLI" , Band);
		break;
	}

	return( Parameters(id) ? Parameters(id)->asGrid() : NULL );
}

//---------------------------------------------------------
CSG_Grid * CLandsat_TOAR::Get_Band_Output(int Band, int Sensor)
{
	CSG_Grid	*pInput	= Get_Band_Input(Band, Sensor);

	if( pInput )
	{
		CSG_Grid	*pGrid	= SG_Create_Grid(pInput, Sensor == oli8 ? SG_DATATYPE_Word : SG_DATATYPE_Byte);

		if( pGrid )
		{
			CSG_String	id; Band++;

			switch( Sensor )
			{
			case mss1: case mss2: case mss3: case mss4: case mss5:
				id	=             "SPECTRAL";
				break;

			case tm4: case tm5:
				id	= Band == 6 ? "THERMAL"
					:             "SPECTRAL";
				break;

			case tm7:
				id	= Band == 6 ? "THERMAL"
					: Band == 7 ? "THERMAL"
					: Band == 9 ? "PANBAND"
					:             "SPECTRAL";
				break;

			case oli8:
				id	= Band ==  9 ? "PANBAND"
					: Band == 10 ? "THERMAL"
					: Band == 11 ? "THERMAL"
					:              "SPECTRAL";
				break;
			}

			if( Parameters(id) )
			{
				Parameters(id)->asGridList()->Add_Item(pGrid);
			}

			return( pGrid );
		}
	}

	return( NULL );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CLandsat_TOAR::On_Execute(void)
{
	//-----------------------------------------------------
	int			Sensor;
	lsat_data	lsat;

	//-----------------------------------------------------
	if( *Parameters("METAFILE")->asString()  )
	{
		if( !Load_MetaFile(Parameters("METAFILE")->asString(), lsat) )
		{
			Error_Set(_TL("could not read metadata file"));

			return( false );
		}

		Sensor	= Get_Sensor_Index(lsat.number, lsat.sensor);
	}

	//-----------------------------------------------------
	else
	{
		memset(&lsat, 0, sizeof(lsat));

		if( !Set_Date_from_String(lsat.date    , Parameters("DATE_ACQU")->asString()) )
		{
			Error_Set(_TL("invalid acquisition date"));

			return( false );
		}

		if( !Set_Date_from_String(lsat.creation, Parameters("DATE_PROD")->asString()) )
		{
			Error_Set(_TL("invalid production date"));

			return( false );
		}

		lsat.sun_elev	= Parameters("SUN_HGT")->asDouble();

		switch( Sensor = Parameters("SENSOR")->asInt() )
		{
		case mss1:	set_MSS1(&lsat);	break;
		case mss2:	set_MSS2(&lsat);	break;
		case mss3:	set_MSS3(&lsat);	break;
		case mss4:	set_MSS4(&lsat);	break;
		case mss5:	set_MSS5(&lsat);	break;
		case tm4 :	set_TM4 (&lsat);	break;
		case tm5 :	set_TM5 (&lsat);	break;
		case oli8:	set_OLI (&lsat);	break;
		case tm7 :
			{
				char	gain[9];

				gain[0]	= Parameters("ETM_GAIN_10")->asInt() == 0 ? 'L' : 'H';
				gain[1]	= Parameters("ETM_GAIN_20")->asInt() == 0 ? 'L' : 'H';
				gain[2]	= Parameters("ETM_GAIN_30")->asInt() == 0 ? 'L' : 'H';
				gain[3]	= Parameters("ETM_GAIN_40")->asInt() == 0 ? 'L' : 'H';
				gain[4]	= Parameters("ETM_GAIN_50")->asInt() == 0 ? 'L' : 'H';
				gain[5]	= Parameters("ETM_GAIN_61")->asInt() == 0 ? 'L' : 'H';
				gain[6]	= Parameters("ETM_GAIN_62")->asInt() == 0 ? 'L' : 'H';
				gain[7]	= Parameters("ETM_GAIN_70")->asInt() == 0 ? 'L' : 'H';
				gain[8]	= Parameters("ETM_GAIN_80")->asInt() == 0 ? 'L' : 'H';

				set_ETM (&lsat, gain);	// Landsat-7 requires band gain with 9 (H/L) data
				break;
			}
		}
    }

	///////////////////////////////////////////////////////
	//-----------------------------------------------------
	// Atmospheric Correction

	int		AC_Method, dn_dark[MAX_BANDS], dn_mode[MAX_BANDS];

	switch( Parameters("AC_METHOD")->asInt() )
	{
	default:	AC_Method	= UNCORRECTED;	break;
	case  1:	AC_Method	= CORRECTED  ;	break;
	case  2:	AC_Method	= DOS1       ;	break;
	case  3:	AC_Method	= DOS2       ;	break;
	case  4:	AC_Method	= DOS2b      ;	break;
	case  5:	AC_Method	= DOS3       ;	break;
	case  6:	AC_Method	= DOS4       ;	break;
	}

	//-----------------------------------------------------
	if( AC_Method >= UNCORRECTED )
	{
		int		AC_nCells	= Parameters("AC_DO_CELLS")->asInt();
		double	AC_Rayleigh	= Parameters("AC_RAYLEIGH")->asDouble();
		double	AC_SunRad	= Parameters("AC_SUN_RAD" )->asDouble() / 100.0;

		for(int iBand=0; iBand<lsat.bands && Process_Get_Okay(); iBand++)
		{
			dn_mode[iBand] = 0;
			dn_dark[iBand] = (int)lsat.band[iBand].qcalmin;

			CSG_Grid	*pInput	= Get_Band_Input(iBand, Sensor);

			if( !pInput )
			{
				continue;
			}

			//---------------------------------------------
			if( AC_Method > DOS && !lsat.band[iBand].thermal )	// calculate dark pixel
			{
				Process_Set_Text(CSG_String::Format("%s [%d/%d]", _TL("Dark Object Subtraction"), lsat.band[iBand].number, lsat.bands));

				int				j;
				unsigned long	hist[256], h_max;

				memset(hist, 0, 256 * sizeof(unsigned long));

				//-----------------------------------------
				// histogram
				for(int y=0; y<pInput->Get_NY() && Set_Progress(y, pInput->Get_NY()); y++)
				{
					for(int x=0, q; x<pInput->Get_NX(); x++)
					{
						if( !pInput->is_NoData(x, y) && (q = pInput->asInt(x, y)) != 0 && q >= lsat.band[iBand].qcalmin && q <= 255 )
						{
							hist[q]++;
						}
					}
				}

				//-----------------------------------------
				// DN of dark object
				for(j=(int)lsat.band[iBand].qcalmin; j<256; j++)
				{
					if( hist[j] >= (unsigned int)AC_nCells )
					{
						dn_dark[iBand] = j;
			
						break;
					}
				}

				//-----------------------------------------
				// Mode of DN
				for(j=(int)lsat.band[iBand].qcalmin, h_max=0L; j<241; j++)
				{
					if( hist[j] > h_max )	// Exclude potentially saturated < 240
					{
						h_max = hist[j];
						dn_mode[iBand] = j;
					}
				}

				Message_Add(CSG_String::Format("DN = %.2d [%lu] : mode %.2d [%lu] %s",
					dn_dark[iBand], hist[dn_dark[iBand]],
					dn_mode[iBand], hist[dn_mode[iBand]],
					hist[255] > hist[dn_mode[iBand]] ? ", excluding DN > 241" : ""
				));
			}

			//---------------------------------------------
			lsat_bandctes(&lsat, iBand, AC_Method, AC_SunRad, dn_dark[iBand], AC_Rayleigh);	// calculate transformation constants
		}
	}

	///////////////////////////////////////////////////////
	//-----------------------------------------------------
	Parameters("SPECTRAL")->asGridList()->Del_Items();
	Parameters("THERMAL" )->asGridList()->Del_Items();
	Parameters("PANBAND" )->asGridList()->Del_Items();

	//-----------------------------------------------------
	bool	bRadiance	= Parameters("AS_RAD")->asBool();

	CSG_MetaData	Info;

	Info.Add_Child("Platform"              , CSG_String::Format("Landsat-%d", lsat.number));
	Info.Add_Child("Sensor"                , lsat.sensor);
	Info.Add_Child("Acquisition"           , lsat.date);
	Info.Add_Child("Production"            , lsat.creation);
	Info.Add_Child("Earth-Sun Distance"    , lsat.dist_es);
	Info.Add_Child("Solar Height"          , lsat.sun_elev);
	Info.Add_Child("Atmospheric Correction", Parameters("AC_METHOD")->asString());

	double	MaxVal	= Sensor != oli8 ? 255. : 65535.;

	//-----------------------------------------------------
	for(int iBand=0; iBand<lsat.bands && Process_Get_Okay(); iBand++)
	{
		CSG_Grid	*pInput	 = Get_Band_Input (iBand, Sensor);
		CSG_Grid	*pOutput = Get_Band_Output(iBand, Sensor);

		if( !pInput || !pOutput )
		{
			continue;
		}

		Process_Set_Text(CSG_String::Format("%s [%d/%d]", _TL("Processing"), lsat.band[iBand].number, lsat.bands));

		//-------------------------------------------------
		if( bRadiance )
		{
			pOutput->Set_Name(CSG_String::Format("%s [%s]", pInput->Get_Name(), _TL("Radiance"   )));
			pOutput->asGrid()->Set_NoData_Value(MaxVal);

			double	min	= lsat_qcal2rad(pInput->Get_Min(), &lsat.band[iBand]);
			double	max	= lsat_qcal2rad(pInput->Get_Max(), &lsat.band[iBand]);

			pOutput->asGrid()->Set_Scaling((max - min) / (MaxVal - 1.0), min);
		}
		else if( lsat.band[iBand].thermal )
		{
			pOutput->Set_Name(CSG_String::Format("%s [%s]", pInput->Get_Name(), _TL("Temperature")));
			pOutput->Set_Unit(_TL("Kelvin"));
			pOutput->asGrid()->Set_NoData_Value(MaxVal);
			pOutput->asGrid()->Set_Scaling(150.0 / (MaxVal - 1.0), 183.16);	// -90°C to 60°C
		}
		else
		{
			pOutput->Set_Name(CSG_String::Format("%s [%s]", pInput->Get_Name(), _TL("Reflectance")));
			pOutput->asGrid()->Set_NoData_Value(MaxVal);
			pOutput->asGrid()->Set_Scaling(1.0 / (MaxVal - 1.0));
		}

		//-------------------------------------------------
		for(int y=0; y<pInput->Get_NY() && Set_Progress(y, pInput->Get_NY()); y++)
		{
			#pragma omp parallel for
			for(int x=0; x<pInput->Get_NX(); x++)
			{
				double	qcal;

				if( pInput->is_NoData(x, y) || (qcal = pInput->asDouble(x, y)) == 0.0 || qcal < lsat.band[iBand].qcalmin )
				{
					pOutput->Set_NoData(x, y);
				}
				else
				{
					double	r	= lsat_qcal2rad(qcal, &lsat.band[iBand]);

					if( bRadiance )
					{
						pOutput->Set_Value(x, y, r < 0.0 ? 0.0 : r);
					}
					else if( lsat.band[iBand].thermal )
					{
						r	= lsat_rad2temp(r, &lsat.band[iBand]);

						pOutput->Set_Value(x, y, r);
					}
					else // reflectance
					{
						r	= lsat_rad2ref(r, &lsat.band[iBand]);

						pOutput->Set_Value(x, y, r < 0.0 ? 0.0 : r > 1.0 ? 1.0 : r);
					}
				}
			}
		}

		//-------------------------------------------------
		CSG_MetaData &Info_Band	= pOutput->Get_MetaData();

		Info_Band.Add_Children(Info);

		Info_Band.Add_Child("BAND"     , lsat.band[iBand].number);
		Info_Band.Add_Child("NAME"     , Get_Band_Name(iBand, Sensor));
		Info_Band.Add_Child("WAVE_MIN" , lsat.band[iBand].wavemin);
		Info_Band.Add_Child("WAVE_MID" , 0.5 * (lsat.band[iBand].wavemin + lsat.band[iBand].wavemax));
		Info_Band.Add_Child("WAVE_MAX" , lsat.band[iBand].wavemax);
		Info_Band.Add_Child("TYPE"     , bRadiance ? _TL("Radiance") : lsat.band[iBand].thermal ? _TL("Temperature") : _TL("Reflectance"));
		Info_Band.Add_Child("ATCORR"   , AC_Method > DOS && !lsat.band[iBand].thermal ? _TL("At-Surface") : _TL("At-Sensor"));
		Info_Band.Add_Child("QCalMin"  , lsat.band[iBand].qcalmin);
		Info_Band.Add_Child("QCalMax"  , lsat.band[iBand].qcalmax);
		Info_Band.Add_Child("LMin"     , lsat.band[iBand].lmin   );
		Info_Band.Add_Child("LMax"     , lsat.band[iBand].lmax   );
		Info_Band.Add_Child("Radiation", CSG_String::Format("%.5lf * DN + %.5lf", lsat.band[iBand].gain, lsat.band[iBand].bias));

		if( lsat.band[iBand].thermal )
		{
			Info_Band.Add_Child("Temperature", CSG_String::Format("%.3lf / ln[(%.3lf / %s) + 1.0]", lsat.band[iBand].K2, lsat.band[iBand].K1, _TL("Radiance")));
		}
		else
		{
			Info_Band.Add_Child("Mean_Solar_Irradiance", lsat.band[iBand].esun);	// Mean Solar Exoatmospheric Irradiance
			Info_Band.Add_Child("Reflectance"          , CSG_String::Format("%s / %.5lf", _TL("Radiance"), lsat.band[iBand].K2));

			if( AC_Method > DOS )
			{
				Info_Band.Add_Child("Minimum_of_Darkness_DN_Cells" , Parameters("AC_DO_CELLS")->asInt());
				Info_Band.Add_Child("Darkness_Digital_Number"      , dn_dark[iBand]);
				Info_Band.Add_Child("Mode_of_DN"                   , dn_mode[iBand]);
				Info_Band.Add_Child("Mode_in_Reflectance_Histogram", lsat_rad2ref(lsat_qcal2rad(dn_mode[iBand], &lsat.band[iBand]), &lsat.band[iBand]));
			}
		}

		pOutput->Set_Description(Info_Band.asText());
	}

	//-----------------------------------------------------
	if( Parameters("GRIDS_OUT")->asBool() )
	{
		CSG_Parameter_Grid_List	*pList	= Parameters("SPECTRAL")->asGridList();

		if( pList->Get_Grid_Count() > 0 )
		{
			CSG_Grids	*pGrids	= SG_Create_Grids();

			pGrids->Set_Name(Parameters("GRIDS_NAME")->asString());
			pGrids->Set_Description(Info.asText());
			pGrids->Get_MetaData().Add_Children(Info);

			pGrids->Get_Attributes_Ptr()->Destroy();
			pGrids->Add_Attribute("BAND"    , SG_DATATYPE_Int   );
			pGrids->Add_Attribute("NAME"    , SG_DATATYPE_String);
			pGrids->Add_Attribute("WAVE_MIN", SG_DATATYPE_Double);
			pGrids->Add_Attribute("WAVE_MID", SG_DATATYPE_Double);
			pGrids->Add_Attribute("WAVE_MAX", SG_DATATYPE_Double);

			for(int i=0; i<pList->Get_Grid_Count(); i++)
			{
				CSG_Grid	*pGrid	= pList->Get_Grid(i);

				pGrids->Add_Grid(i, pGrid, true);

				const CSG_MetaData	&Info_Band	= pGrid->Get_MetaData();

				pGrids->Get_Attributes(i).Set_Value("BAND"    , Info_Band.Get_Content("BAND"    ));
				pGrids->Get_Attributes(i).Set_Value("NAME"    , Info_Band.Get_Content("NAME"    ));
				pGrids->Get_Attributes(i).Set_Value("WAVE_MIN", Info_Band.Get_Content("WAVE_MIN"));
				pGrids->Get_Attributes(i).Set_Value("WAVE_MID", Info_Band.Get_Content("WAVE_MID"));
				pGrids->Get_Attributes(i).Set_Value("WAVE_MAX", Info_Band.Get_Content("WAVE_MAX"));
			}

			pGrids->Set_Z_Attribute (3);
			pGrids->Set_Z_Name_Field(1);

			pList->Del_Items();
			pList->Add_Item(pGrids);
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
