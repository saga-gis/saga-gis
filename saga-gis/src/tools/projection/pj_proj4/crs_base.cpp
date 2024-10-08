
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                   Projection_Proj4                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                     crs_base.cpp                      //
//                                                       //
//                 Copyright (C) 2010 by                 //
//                      Olaf Conrad                      //
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
#include "crs_base.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <proj.h>

//---------------------------------------------------------
#define PJ_GET_PROJS proj_list_operations()
#define PJ_GET_ELLPS proj_list_ellps()
#define PJ_GET_UNITS proj_list_units()

#define TPJ_PROJS const PJ_OPERATIONS
#define TPJ_ELLPS const PJ_ELLPS
#define TPJ_UNITS const PJ_UNITS

//---------------------------------------------------------
struct PJ_DATUMS
{
	const char *id, *comments;
};

#define TPJ_DATUMS struct PJ_DATUMS

struct PJ_DATUMS PJ_GET_DATUMS[] =
{
	{ "WGS84"        , "WGS84"                                },
	{ "GGRS87"       , "Greek Geodetic Reference System 1987" },
	{ "NAD83"        , "North American Datum 1983"            },
	{ "NAD27"        , "North American Datum 1927"            },
	{ "potsdam"      , "Potsdam Rauenberg 1950 DHDN"          },
	{ "carthage"     , "Carthage 1934 Tunisia"                },
	{ "hermannskogel", "Hermannskogel"                        },
	{ "ire65"        , "Ireland 1965"                         },
	{ "nzgd49"       , "New Zealand Geodetic Datum 1949"      },
	{ "OSGB36"       , "Airy 1830"                            },
	{ NULL           , NULL                                   }
};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CCRS_Base::CCRS_Base(void)
{
	CSG_Projection Projection; Projection.Set_GCS_WGS84();

	Parameters.Add_String("",
		"CRS_STRING"     , _TL("Definition String"),
		_TL("Supported formats comprise PROJ and WKT strings, object codes (e.g. \"EPSG:4326\")."),
		Projection.Get_PROJ()
	);

	//-----------------------------------------------------
	if( has_GUI() )
	{
		Parameters.Add_Choice("CRS_STRING",
			"CRS_DISPLAY", _TL("Display Definition as..."),
			_TL(""),
			CSG_String::Format("%s|%s",
				_TL("PROJ"),
				_TL("WKT")
			), 0
		)->Set_UseInCMD(false);

		Parameters.Add_Int("CRS_STRING",
			"CRS_CODE"       , _TL("Authority Code"),
			_TL(""),
			Projection.Get_Code()
		)->Set_UseInCMD(false);

		Parameters.Add_String("CRS_CODE",
			"CRS_AUTHORITY"  , _TL("Authority"),
			_TL(""),
			Projection.Get_Authority()
		)->Set_UseInCMD(false);
	}

	//-----------------------------------------------------
	Parameters.Add_FilePath("CRS_STRING",
		"CRS_FILE"       , _TL("Well Known Text File"),
		_TL(""),
		CSG_String::Format("%s|*.prj;*.wkt;*.txt|%s (*.prj)|*.prj|%s (*.wkt)|*.wkt|%s (*.txt)|*.txt|%s|*.*",
			_TL("All Recognized Files"),
			_TL("ESRI WKT Files"),
			_TL("WKT Files"),
			_TL("Text Files"),
			_TL("All Files")
		)
	);

	//-----------------------------------------------------
	if( has_GUI() )
	{
		Parameters.Add_Choice("CRS_CODE",
			"CRS_GEOGCS" , _TL("Geographic Coordinate Systems"),
			_TL("Select definition from a list of predefined geographic coordinate systems."),
			SG_Get_Projections().Get_Names_List(ESG_CRS_Type::Geographic)
		)->Set_UseInCMD(false);

		Parameters.Add_Choice("CRS_CODE",
			"CRS_PROJCS" , _TL("Projected Coordinate Systems"),
			_TL("Select definition from a list of predefined projected coordinate systems."),
			SG_Get_Projections().Get_Names_List(ESG_CRS_Type::Projection)
		)->Set_UseInCMD(false);

		//-------------------------------------------------
		Parameters.Add_Parameters("CRS_STRING",
			"CRS_PICKER" , _TL("Pick from Data Set"),
			_TL("Copy coordinate system definition from a loaded data set.")
		)->Set_UseInCMD(false);

		Parameters("CRS_PICKER")->asParameters()->Add_Grid("",
			"CRS_GRID"   , _TL("Grid"),
			_TL(""),
			PARAMETER_INPUT_OPTIONAL, false
		)->Set_UseInCMD(false);

		Parameters("CRS_PICKER")->asParameters()->Add_Shapes("",
			"CRS_SHAPES" , _TL("Shapes"),
			_TL(""),
			PARAMETER_INPUT_OPTIONAL
		)->Set_UseInCMD(false);

		//-------------------------------------------------
		Parameters.Add_Parameters("CRS_STRING",
			"CRS_DIALOG" , _TL("Customize"),
			_TL("Interactively customize your coordinate system definition.")
		)->Set_UseInCMD(false);

		Set_User_Parameters(*Parameters("CRS_DIALOG")->asParameters());
		Set_User_Definition(*Parameters("CRS_DIALOG")->asParameters(), Projection.Get_PROJ());
	}

	//-----------------------------------------------------
	Parameters.Add_Info_String("", "CRS_WKT" , _TL("WKT" ), _TL(""), Projection.Get_WKT2())->Set_Enabled(false); // for requesting projection in a generic/safe way
	Parameters.Add_Info_String("", "CRS_PROJ", _TL("PROJ"), _TL(""), Projection.Get_PROJ())->Set_Enabled(false); // for requesting projection in a generic/safe way

	Parameters("CRS_WKT")->Set_UseInCMD(false); Parameters("CRS_PROJ")->Set_UseInCMD(false);
	Parameters("CRS_WKT")->Set_UseInGUI(false); Parameters("CRS_PROJ")->Set_UseInGUI(false);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCRS_Base::On_Before_Execution(void)
{
	if( Parameters("CRS_DISPLAY") )
	{
		CSG_Projection Projection(Parameters["CRS_WKT"].asString(), Parameters["CRS_PROJ"].asString()); // fast construction!

		bool bCallback = Parameters.Set_Callback(false);

		switch( Parameters["CRS_DISPLAY"].asInt() )
		{
		default: Parameters["CRS_STRING"].Set_Value(Projection.Get_PROJ()); break;
		case  1: Parameters["CRS_STRING"].Set_Value(Projection.Get_WKT2()); break;
		}

		Parameters["CRS_CODE"     ].Set_Value(Projection.Get_Code     ());
		Parameters["CRS_AUTHORITY"].Set_Value(Projection.Get_Authority());

		Parameters.Set_Callback(bCallback);
	}

	return( true );
}

//---------------------------------------------------------
int CCRS_Base::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	Parameter_Changed(pParameters, pParameter);

	return( CSG_Tool::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
bool CCRS_Base::Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	CSG_Projection Projection;

	//-----------------------------------------------------
	if( pParameter->Cmp_Identifier("CRS_STRING") )
	{
		CSG_String Definition(pParameter->asString()); Definition.Trim(); Definition.Replace("\\n", "");

		if( Definition.Find("[ERROR]") == 0 )
		{
			Definition.Replace("[ERROR]", ""); Definition.Trim();
		}

		if( Projection.Create(Definition) == false )
		{
			Definition.Prepend("[ERROR] "); pParameters->Set_Parameter("CRS_STRING", Definition);
		}
	}

	//-----------------------------------------------------
	else if( pParameter->Cmp_Identifier("CRS_DISPLAY") )
	{
		Projection.Create((*pParameters)("CRS_WKT")->asString(), (*pParameters)("CRS_PROJ")->asString());
	}

	//-----------------------------------------------------
	else if( pParameter->Cmp_Identifier("CRS_FILE") )
	{
		Projection.Load(pParameter->asString());

		pParameter->Set_Value(""); // clear
	}

	//-----------------------------------------------------
	else if( pParameter->Cmp_Identifier("CRS_CODE") || pParameter->Cmp_Identifier("CRS_AUTHORITY") )
	{
		Projection.Create((*pParameters)("CRS_CODE")->asInt(), (*pParameters)("CRS_AUTHORITY")->asString());
	}

	//-----------------------------------------------------
	else if( pParameter->Cmp_Identifier("CRS_GEOGCS") || pParameter->Cmp_Identifier("CRS_PROJCS") )
	{
		CSG_String Authority_Code;

		if( pParameter->asChoice()->Get_Data(Authority_Code) )
		{
			Projection.Create(Authority_Code);
		}
	}

	//-----------------------------------------------------
	else if( pParameter->Cmp_Identifier("CRS_DIALOG") )
	{
		Projection.Create(Get_User_Definition(*pParameter->asParameters()));
	}

	//-----------------------------------------------------
	else if( pParameter->Cmp_Identifier("CRS_GRID"  ) ) { (*pParameters)("CRS_SHAPES")              ->Set_Value(DATAOBJECT_NOTSET); return( false ); }
	else if( pParameter->Cmp_Identifier("CRS_SHAPES") ) { (*pParameters)("CRS_GRID"  )->Get_Parent()->Set_Value(DATAOBJECT_NOTSET); return( false ); }
	else if( pParameter->Cmp_Identifier("CRS_PICKER") )
	{
		CSG_Data_Object *pPick;

		if( (pPick = (*pParameter->asParameters())("CRS_GRID"  )->asDataObject()) && pPick->Get_Projection().is_Okay() )
		{
			Projection.Create(pPick->Get_Projection());
		}

		(*pParameter->asParameters())("CRS_GRID"  )->Get_Parent()
			->Set_Value(DATAOBJECT_NOTSET);

		if( (pPick = (*pParameter->asParameters())("CRS_SHAPES")->asDataObject()) && pPick->Get_Projection().is_Okay() )
		{
			Projection.Create(pPick->Get_Projection());
		}

		(*pParameter->asParameters())("CRS_SHAPES")
			->Set_Value(DATAOBJECT_NOTSET);
	}

	//-----------------------------------------------------
	else // CRS definition has not been changed, just return...
	{
		return( true );
	}

	//-----------------------------------------------------
	pParameters->Set_Parameter("CRS_PROJ", Projection.Get_PROJ());

	if( !pParameter->Cmp_Identifier("CRS_WKT"   ) ) { pParameters->Set_Parameter("CRS_WKT", Projection.Get_WKT()); }

	if(	!pParameter->Cmp_Identifier("CRS_GEOGCS") ) { pParameters->Set_Parameter("CRS_GEOGCS", 0); }
	if(	!pParameter->Cmp_Identifier("CRS_PROJCS") ) { pParameters->Set_Parameter("CRS_PROJCS", 0); }

	if( Projection.is_Okay() )
	{
		pParameters->Set_Parameter("CRS_CODE"     , Projection.Get_Code     ());
		pParameters->Set_Parameter("CRS_AUTHORITY", Projection.Get_Authority());

		if( (*pParameters)("CRS_DIALOG") )
		{
			Set_User_Definition(*(*pParameters)("CRS_DIALOG")->asParameters(), Projection.Get_PROJ());
		}

		if( (*pParameters)("CRS_DISPLAY") )
		{
			switch( (*pParameters)("CRS_DISPLAY")->asInt() )
			{
			default: pParameters->Set_Parameter("CRS_STRING", Projection.Get_PROJ()); break;
			case  1: pParameters->Set_Parameter("CRS_STRING", Projection.Get_WKT2()); break;
			}
		}
	}
	else
	{
		if( !pParameter->Cmp_Identifier("CRS_CODE") && !pParameter->Cmp_Identifier("CRS_AUTHORITY") )
		{
			pParameters->Set_Parameter("CRS_CODE"     , Projection.Get_Code     ());
			pParameters->Set_Parameter("CRS_AUTHORITY", Projection.Get_Authority());
		}

		if( !pParameter->Cmp_Identifier("CRS_STRING") )
		{
			pParameters->Set_Parameter("CRS_STRING", "[ERROR]");
		}
	}

	return( Projection.is_Okay() );
}

//---------------------------------------------------------
int CCRS_Base::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	Parameters_Enable(pParameters, pParameter);

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}

bool CCRS_Base::Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	pParameters->Cmp_Identifier("CRS_DIALOG") )
	{
		if(	pParameter->Cmp_Identifier("PROJ_TYPE") )
		{
			CSG_String ID; pParameter->asChoice()->Get_Data(ID);

			bool bGCS = !ID.Cmp("longlat") || !ID.Cmp("geogoffset");
			bool bUTM = !ID.Cmp("utm");

			pParameters->Set_Enabled("OVER" ,  bGCS);
			pParameters->Set_Enabled("UNIT" , !bGCS);

			pParameters->Set_Enabled("LON_0", !bGCS && !bUTM);
			pParameters->Set_Enabled("LAT_0", !bGCS && !bUTM);
			pParameters->Set_Enabled("X_0"  , !bGCS && !bUTM);
			pParameters->Set_Enabled("Y_0"  , !bGCS && !bUTM);
			pParameters->Set_Enabled("K_0"  , !bGCS && !bUTM);

			for(int i=0; i<pParameters->Get_Count(); i++)
			{
				CSG_Parameter &p = (*pParameters)[i];

				if( !p.Get_Parent()
				&&  !p.Cmp_Identifier("PROJ_TYPE")
				&&  !p.Cmp_Identifier("DATUM_DEF")
				&&  !p.Cmp_Identifier("GENERAL"  ) )
				{
					p.Set_Enabled(p.Cmp_Identifier(ID));
				}
			}
		}

		if(	pParameter->Cmp_Identifier("DATUM_DEF") )
		{
			int Value = pParameter->asInt();

			pParameters->Set_Enabled("DATUM"      , Value == 0);
			pParameters->Set_Enabled("ELLIPSOID"  , Value == 1);
			pParameters->Set_Enabled("DATUM_SHIFT", Value == 1);
		}

		if(	pParameter->Cmp_Identifier("ELLIPSOID") )
		{
			int Value = pParameter->asInt();

			pParameters->Set_Enabled("ELLPS_DEF"  , Value == 0);
			pParameters->Set_Enabled("ELLPS_A"    , Value != 0);
			pParameters->Set_Enabled("ELLPS_B"    , Value == 1);
			pParameters->Set_Enabled("ELLPS_F"    , Value == 2);
			pParameters->Set_Enabled("ELLPS_RF"   , Value == 3);
			pParameters->Set_Enabled("ELLPS_E"    , Value == 4);
			pParameters->Set_Enabled("ELLPS_ES"   , Value == 5);
		}

		if(	pParameter->Cmp_Identifier("DATUM_SHIFT") )
		{
			int Value = pParameter->asInt();

			pParameters->Set_Enabled("DS_DX"      , Value == 1);
			pParameters->Set_Enabled("DS_DY"      , Value == 1);
			pParameters->Set_Enabled("DS_DZ"      , Value == 1);
			pParameters->Set_Enabled("DS_RX"      , Value == 1);
			pParameters->Set_Enabled("DS_RY"      , Value == 1);
			pParameters->Set_Enabled("DS_RZ"      , Value == 1);
			pParameters->Set_Enabled("DS_SC"      , Value == 1);
			pParameters->Set_Enabled("DS_GRID"    , Value == 2);
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCRS_Base::Get_Projection(CSG_Projection &Projection)
{
	return( Projection.Create(Parameters["CRS_WKT"].asString(), Parameters["CRS_PROJ"].asString()) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define WGS84_ELLPS_A 6378137.000
#define WGS84_ELLPS_B 6356752.314

//---------------------------------------------------------
bool CCRS_Base::Set_User_Parameters(CSG_Parameters &P)
{
	CSG_String Projections, Datums, Ellipsoids, Units, Description;

	P.Add_Choice("", "PROJ_TYPE", _TL("Projection Type"), _TL(""), "");

	P.Add_Choice("",
		"DATUM_DEF", _TL("Datum Definition"),
		_TL(""),
		CSG_String::Format("%s|%s",
			_TL("Predefined Datum"),
			_TL("User Defined Datum")
		)
	);

	P.Add_Node("", "GENERAL", _TL("General Settings"), _TL(""));


	///////////////////////////////////////////////////////

	// Projection -----------------------------------------

	const char *Blacklist[] = { "noop",
		"lonlat", "latlong", "latlon", // geodetic alias
		"affine", "helmert",
		"hgridshift", "vgridshift", "xyzgridshift",
		NULL
	};

	CSG_Table List;
	List.Add_Field("id"  , SG_DATATYPE_String);
	List.Add_Field("name", SG_DATATYPE_String);
	List.Add_Field("desc", SG_DATATYPE_String);

	for(TPJ_PROJS *pProjection=PJ_GET_PROJS; pProjection->id; ++pProjection)
	{
		CSG_String ID(pProjection->id);

		for(int i=0; Blacklist[i]; i++)
		{
			if( !ID.Cmp(Blacklist[i]) )
			{
				ID.Clear(); break;
			}
		}

		if( !ID.is_Empty() )
		{
			CSG_String Args(*pProjection->descr);
			CSG_Table_Record &Entry = *List.Add_Record();
			Entry.Set_Value(0, ID);
			Entry.Set_Value(1, ID.Cmp("longlat") ? Args.BeforeFirst('\n') : CSG_String("Geographic Coordinates (Longitudes/Latitudes)"));
			Entry.Set_Value(2, Args.AfterFirst ('\n').AfterFirst('\n').AfterFirst('\t'));
		}
	}

	if( List.Get_Count() < 1 )
	{
		return( false );
	}

	List.Set_Index(1);

	Description	= _TL("Available Projections:");

	for(sLong i=0; i<List.Get_Count(); i++)
	{
		CSG_String ID(List[i].asString(0)), Name(List[i].asString(1)), Args(List[i].asString(2));

		Projections +=   "{" + ID + "}"  + Name + "|";
		Description += "\n[" + ID + "] " + Name;

		if( !Args.is_Empty() )
		{
			Description += " (" + Args + ")";

			Add_User_Projection(P, ID, Args);
		}
	}

	P("PROJ_TYPE")->asChoice()->Set_Items(Projections);
	P("PROJ_TYPE")->Set_Description(Description);

	// Datums ---------------------------------------------
	for(TPJ_DATUMS *pDatum=PJ_GET_DATUMS; pDatum->id; ++pDatum)
	{
		CSG_String id(pDatum->id), comments(pDatum->comments);

		Datums     += CSG_String::Format("{%s}%s|", id.c_str(), comments.Length() ? comments.c_str() : id.c_str());
	}

	// Ellipsoids -----------------------------------------
	for(TPJ_ELLPS *pEllipse=PJ_GET_ELLPS; pEllipse->id; ++pEllipse)
	{
		CSG_String id(pEllipse->id), name(pEllipse->name), _major(pEllipse->major), ell(pEllipse->ell);

		Ellipsoids += CSG_String::Format("{%s}%s (%s, %s)|", id.c_str(), name.c_str(), _major.c_str(), ell.c_str());
	}

	// Units ----------------------------------------------
	for(TPJ_UNITS *pUnit=PJ_GET_UNITS; pUnit->id; ++pUnit)
	{
		CSG_String id(pUnit->id), name(pUnit->name), to_meter(pUnit->to_meter);

		Units      += CSG_String::Format("{%s}%s (%s)|", id.c_str(), name.c_str(), to_meter.c_str());
	}


	///////////////////////////////////////////////////////

	//-----------------------------------------------------
	// Datum...

	P.Add_Choice("DATUM_DEF", "DATUM", _TL("Predefined Datum"), _TL(""), Datums);


	//-----------------------------------------------------
	// Datum Ellipsoid...

	P.Add_Choice("DATUM_DEF",
		"ELLIPSOID", _TL("Ellipsoid Definition"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s|%s|%s",
			_TL("Predefined Ellipsoids"),
			_TL("Semimajor Axis and Semiminor Axis"),
			_TL("Semimajor Axis and Flattening"),
			_TL("Semimajor Axis and Reciprocal Flattening"),
			_TL("Semimajor Axis and Eccentricity"),
			_TL("Semimajor Axis and Eccentricity Squared")
		)
	);

	P.Add_Choice("ELLIPSOID", "ELLPS_DEF", _TL("Predefined Ellipsoids"), _TL(""), Ellipsoids);

	P.Add_Double("ELLIPSOID", "ELLPS_A" , _TL("Semimajor Axis (a)"        ), _TL(""), WGS84_ELLPS_A);
	P.Add_Double("ELLIPSOID", "ELLPS_B" , _TL("Semiminor Axis (b)"        ), _TL(""), WGS84_ELLPS_B);
	P.Add_Double("ELLIPSOID", "ELLPS_F" , _TL("Flattening (f)"            ), _TL(""), (WGS84_ELLPS_A - WGS84_ELLPS_B) / WGS84_ELLPS_A);
	P.Add_Double("ELLIPSOID", "ELLPS_RF", _TL("Reciprocal Flattening (rf)"), _TL(""), WGS84_ELLPS_A / (WGS84_ELLPS_A - WGS84_ELLPS_B));
	P.Add_Double("ELLIPSOID", "ELLPS_E" , _TL("Eccentricity (e)"          ), _TL(""), sqrt(WGS84_ELLPS_A*WGS84_ELLPS_A + WGS84_ELLPS_B*WGS84_ELLPS_B));
	P.Add_Double("ELLIPSOID", "ELLPS_ES", _TL("Squared Eccentricity (es)" ), _TL(""), WGS84_ELLPS_A*WGS84_ELLPS_A + WGS84_ELLPS_B*WGS84_ELLPS_B);


	//-----------------------------------------------------
	// Datum Shift...

	P.Add_Choice("DATUM_DEF",
		"DATUM_SHIFT", _TL("Datum Shift"),
		_TL(""),
		CSG_String::Format("%s|%s|%s",
			_TL("none"),
			_TL("Helmert Transform"),
			_TL("Datum Shift Grid")
		)
	);

	P.Add_Double("DATUM_SHIFT", "DS_DX", _TL("Translation X"), _TL(""));
	P.Add_Double("DATUM_SHIFT", "DS_DY", _TL("Translation Y"), _TL(""));
	P.Add_Double("DATUM_SHIFT", "DS_DZ", _TL("Translation Z"), _TL(""));
	P.Add_Double("DATUM_SHIFT", "DS_RX", _TL("Rotation X"   ), _TL(""));
	P.Add_Double("DATUM_SHIFT", "DS_RY", _TL("Rotation Y"   ), _TL(""));
	P.Add_Double("DATUM_SHIFT", "DS_RZ", _TL("Rotation Z"   ), _TL(""));
	P.Add_Double("DATUM_SHIFT", "DS_SC", _TL("Scaling"      ), _TL(""));

	P.Add_FilePath("DATUM_SHIFT",
		"DS_GRID"    , _TL("Datum Shift Grid File"),
		_TL(""),
		CSG_String::Format("%s|*.gsb;*.gsa;*.dat|%s|*.*",
			_TL("NTv2 Grid Shift"),
			_TL("All Files")
		), NULL, false, false, false
	);


	//-----------------------------------------------------
	// General Settings...

	P.Add_Double("GENERAL", "LON_0"  , _TL("Central Meridian"), _TL(""), 0.);
	P.Add_Double("GENERAL", "LAT_0"  , _TL("Central Parallel"), _TL(""), 0.);
	P.Add_Double("GENERAL", "X_0"    , _TL("False Easting"   ), _TL(""), 0.);
	P.Add_Double("GENERAL", "Y_0"    , _TL("False Northing"  ), _TL(""), 0.);
	P.Add_Double("GENERAL", "K_0"    , _TL("Scale Factor"    ), _TL(""), 1., 0., true);
	P.Add_Choice("GENERAL", "UNIT"   , _TL("Unit"            ), _TL(""), Units, 1);

	P.Add_Bool  ("GENERAL", "OVER"   , _TL("Allow longitudes outside -180 to 180 Range"), _TL(""), false);

	P.Add_Bool  ("GENERAL", "NO_DEFS", _TL("Ignore Defaults" ), _TL(""), false);

	//-----------------------------------------------------
	On_Parameters_Enable(&P, P("PROJ_TYPE"));

	//-----------------------------------------------------
	return( true );
}

//---------------------------------------------------------
#define PRM_ADD_BOL(key, name, val)	P.Add_Bool  (ID, ID + key, name, _TL(""), val);
#define PRM_ADD_INT(key, name, val)	P.Add_Int   (ID, ID + key, name, _TL(""), val);
#define PRM_ADD_DBL(key, name, val)	P.Add_Double(ID, ID + key, name, _TL(""), val);
#define PRM_ADD_STR(key, name, val)	P.Add_String(ID, ID + key, name, _TL(""), val);
#define PRM_ADD_CHO(key, name, val)	P.Add_Choice(ID, ID + key, name, _TL(""), val);

//---------------------------------------------------------
bool CCRS_Base::Add_User_Projection(CSG_Parameters &P, const CSG_String &ID, const CSG_String &Args)
{
	P.Add_Node("", ID, _TL("Projection Settings"), Args);

	//-----------------------------------------------------
	// Cylindrical Projections...

	if(	!ID.CmpNoCase("cea" )		// Equal Area Cylindrical
	||	!ID.CmpNoCase("eqc" )		// Equidistant Cylindrical (Plate Caree) 
	||	!ID.CmpNoCase("merc") )		// Mercator 
	{
		PRM_ADD_DBL("lat_ts", _TL("True Scale Latitude"), 0.);
	}

	if(	!ID.CmpNoCase("utm" ) )		// Universal Transverse Mercator (UTM)
	{
		PRM_ADD_INT("zone" , _TL("Zone" ), 32);
		PRM_ADD_BOL("south", _TL("South"), false);
	}

	if(	!ID.CmpNoCase("omerc") )	// Oblique Mercator 
	{
		PRM_ADD_DBL("lat_1", _TL("Latitude 1" ),  40.);
		PRM_ADD_DBL("lon_1", _TL("Longitude 1"), -20.);
		PRM_ADD_DBL("lat_2", _TL("Latitude 2" ),  50.);
		PRM_ADD_DBL("lon_2", _TL("Longitude 2"),  20.);
	}

	//-----------------------------------------------------
	// Pseudocylindrical Projections...

	if(	!ID.CmpNoCase("gn_sinu") )	// General Sinusoidal Series
	{
		PRM_ADD_DBL("m", "m", 0.5          );
		PRM_ADD_DBL("n", "n", 1. + M_PI_045);
	}

	if(	!ID.CmpNoCase("loxim") )	// Loximuthal
	{
		PRM_ADD_DBL("lat_1", _TL("Latitude 1"), 40.);
	}

	if(	!ID.CmpNoCase("urmfps") )	// Urmaev Flat-Polar Sinusoidal
	{
		PRM_ADD_DBL("n", "n", 1.);
	}

	if(	!ID.CmpNoCase("urm5") )		// Urmaev V
	{
		PRM_ADD_DBL("n"    , "n"    ,  1.);
		PRM_ADD_DBL("q"    , "q"    ,  1.);
		PRM_ADD_DBL("alphi", "alphi", 45.);
	}

	if(	!ID.CmpNoCase("wink1")		// Winkel I
	||	!ID.CmpNoCase("wag3" ) )	// Wagner III
	{
		PRM_ADD_DBL("lat_ts", _TL("True Scale Latitude"), 45.);
	}

	if(	!ID.CmpNoCase("wink2") )	// Winkel II
	{
		PRM_ADD_DBL("lat_1", _TL("Latitude 1"), 40.);
	}


	//-----------------------------------------------------
	// Conic Projections...

	if(	!ID.CmpNoCase("aea"   ) 	// Albers Equal Area
	||	!ID.CmpNoCase("eqdc"  ) 	// Equidistant Conic
	||	!ID.CmpNoCase("euler" ) 	// Euler 
	||	!ID.CmpNoCase("imw_p" ) 	// International Map of the World Polyconic 
	||	!ID.CmpNoCase("murd1" ) 	// Murdoch I 
	||	!ID.CmpNoCase("murd2" ) 	// Murdoch II 
	||	!ID.CmpNoCase("murd3" ) 	// Murdoch III 
	||	!ID.CmpNoCase("pconic") 	// Perspective Conic 
	||	!ID.CmpNoCase("tissot") 	// Tissot 
	||	!ID.CmpNoCase("vitk1" ) )	// Vitkovsky I 
	{
		PRM_ADD_DBL("lat_1", _TL("Latitude 1"), 33.);
		PRM_ADD_DBL("lat_2", _TL("Latitude 2"), 45.);
	}

	if(	!ID.CmpNoCase("lcc") )		// Lambert Conformal Conic 
	{
		PRM_ADD_DBL("lat_1", _TL("Latitude 1"), 33.);
		PRM_ADD_DBL("lat_2", _TL("Latitude 2"), 45.);
	}

	if( !ID.CmpNoCase("leac") )		// Lambert Equal Area Conic
	{
		PRM_ADD_DBL("lat_1", _TL("Latitude 1"), 45.);
		PRM_ADD_BOL("south", _TL("South"     ), false);
	}

	if(	!ID.CmpNoCase("rpoly") )	// Rectangular Polyconic
	{
		PRM_ADD_DBL("lat_ts", _TL("True Scale Latitude"), 45.);
	}

	if(	!ID.CmpNoCase("mpoly") )	// Modified Polyconic
	{
		PRM_ADD_DBL("lat_1", _TL("Latitude 1"), 33.);
		PRM_ADD_DBL("lat_2", _TL("Latitude 2"), 45.);
		PRM_ADD_BOL("lotsa", _TL("Lotsa"     ), true);
	}

	if(	!ID.CmpNoCase("bonne") )	// Bonne
	{
		PRM_ADD_DBL("lat_1", _TL("Latitude 1"), 45.);
	}


	//-----------------------------------------------------
	// Azimuthal Projections...

	if(	!ID.CmpNoCase("stere") )	// Stereographic
	{
		PRM_ADD_DBL("lat_ts", _TL("True Scale Latitude"), 45.);
	}

	if(	!ID.CmpNoCase("ups") )		// Universal Polar Stereographic
	{
		PRM_ADD_BOL("south", _TL("South"), true);
	}

	if(	!ID.CmpNoCase("airy") )	// Airy
	{
		PRM_ADD_DBL("lat_b" , _TL("Latitude B"), 45.);
		PRM_ADD_BOL("no_cut", _TL("No Cut"    ), true);
	}

	if(	!ID.CmpNoCase("nsper") )	// Near-sided perspective
	{
		PRM_ADD_DBL("h", _TL("Height of view point"), 1.);
	}

	if(	!ID.CmpNoCase("aeqd") )	// Azimuthal Equidistant
	{
		PRM_ADD_BOL("guam", _TL("Use Guam elliptical"), false);
	}

	if(	!ID.CmpNoCase("hammer") )	// Hammer & Eckert-Greifendorff
	{
		PRM_ADD_DBL("W", _TL("W"), 0.5);
		PRM_ADD_DBL("M", _TL("M"), 1. );
	}

	if(	!ID.CmpNoCase("wintri") )	// Winkel Tripel 
	{
		PRM_ADD_DBL("lat_1", _TL("Latitude 1"), 40.);
	}


	//-----------------------------------------------------
	// Miscellaneous Projections...

	if(	!ID.CmpNoCase("ocea" )		// Oblique Cylindrical Equal Area
	||	!ID.CmpNoCase("tpeqd") )	// Two Point Equidistant 
	{
		PRM_ADD_DBL("lat_1", _TL("Latitude 1" ),  40.);
		PRM_ADD_DBL("lon_1", _TL("Longitude 1"), -20.);
		PRM_ADD_DBL("lat_2", _TL("Latitude 2" ),  50.);
		PRM_ADD_DBL("lon_2", _TL("Longitude 2"),  20.);
	}

	if(	!ID.CmpNoCase("geos") )	// Geostationary Satellite View
	{
		PRM_ADD_DBL("h"    , _TL("Satellite Height [m]"), 35785831.);
		PRM_ADD_CHO("sweep", _TL("Sweep Angle"         ), "x|y|");
	}

	if(	!ID.CmpNoCase("lsat") )	// Space oblique for LANDSAT
	{
		PRM_ADD_INT("lsat", _TL("Landsat Satellite (1-5)"), 5);
		PRM_ADD_INT("path", _TL("Path (1-255/233 1-3/4-5"), 195);
	}

	if(	!ID.CmpNoCase("labrd") )	// Laborde
	{
		PRM_ADD_DBL("azi", _TL("Azimuth"), 19.);
	}

	if(	!ID.CmpNoCase("lagrng") )	// Lagrange
	{
		PRM_ADD_DBL("lat_1", _TL("Latitude 1"), 0.);
		PRM_ADD_DBL("W"    , _TL("W"         ), 2.);
	}

	if(	!ID.CmpNoCase("chamb") )	// Chamberlin Trimetric
	{
		PRM_ADD_DBL("lat_1", _TL("Latitude 1" ),  30.);
		PRM_ADD_DBL("lon_1", _TL("Longitude 1"), -20.);
		PRM_ADD_DBL("lat_2", _TL("Latitude 2" ),  40.);
		PRM_ADD_DBL("lon_2", _TL("Longitude 2"),  00.);
		PRM_ADD_DBL("lat_3", _TL("Latitude 3" ),  50.);
		PRM_ADD_DBL("lon_3", _TL("Longitude 3"),  20.);
	}

	if(	!ID.CmpNoCase("oea") )		// Oblated Equal Area
	{
		PRM_ADD_DBL("m"    , _TL("m"    ),  1.);
		PRM_ADD_DBL("n"    , _TL("n"    ),  1.);
		PRM_ADD_DBL("theta", _TL("theta"), 45.);
	}

	if(	!ID.CmpNoCase("tpers") )	// Tilted perspective
	{
		PRM_ADD_DBL("tilt", _TL("Tilt"   ),   45.);
		PRM_ADD_DBL("azi" , _TL("Azimuth"),   45.);
		PRM_ADD_DBL("h"   , _TL("h"      ), 1000.);
	}

	if(	!ID.CmpNoCase("ob_tran") )	// General Oblique Transformation
	{
		PRM_ADD_DBL("o_lat_p", _TL("Latitude Pole" ), 40.);
		PRM_ADD_DBL("o_lon_p", _TL("Longitude Pole"), 40.);
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define PROJ_ADD_BOL(key, val)	if( val ) Proj += CSG_String::Format("+%s ", CSG_String(key).c_str());
#define PROJ_ADD_INT(key, val)	Proj += CSG_String::Format("+%s=%d ", CSG_String(key).c_str(), val);
#define PROJ_ADD_DBL(key, val)	Proj += CSG_String::Format("+%s=%s ", CSG_String(key).c_str(), SG_Get_String(val, -32).c_str());
#define PROJ_ADD_STR(key, val)	Proj += CSG_String::Format("+%s=%s ", CSG_String(key).c_str(), CSG_String(val).c_str());
#define PROJ_GET_PRM(parm_id )	P(ID + parm_id)

//---------------------------------------------------------
CSG_String CCRS_Base::Get_User_Definition(CSG_Parameters &P)
{
	CSG_String Proj, ID = P("PROJ_TYPE")->asChoice()->Get_Data();

	//-----------------------------------------------------
	PROJ_ADD_STR("proj", ID);

	if( P("LON_0")->is_Enabled() && P("LON_0")->asDouble() ) PROJ_ADD_DBL("lon_0", P("LON_0")->asDouble());
	if( P("LAT_0")->is_Enabled() && P("LAT_0")->asDouble() ) PROJ_ADD_DBL("lat_0", P("LAT_0")->asDouble());

	if( P("X_0"  )->is_Enabled() && P("X_0"  )->asDouble() ) PROJ_ADD_DBL("x_0"  , P("X_0"  )->asDouble());
	if( P("Y_0"  )->is_Enabled() && P("Y_0"  )->asDouble() ) PROJ_ADD_DBL("y_0"  , P("Y_0"  )->asDouble());

	if( P("K_0"  )->is_Enabled() && P("K_0"  )->asDouble() != 1. && P("K_0")->asDouble() > 0. )
	{
		PROJ_ADD_DBL("k_0", P("K_0")->asDouble());
	}

	if( P("UNIT")->is_Enabled() )
	{
		PROJ_ADD_STR("units", P("UNIT")->asChoice()->Get_Data());
	}

	//-----------------------------------------------------
	switch( P("DATUM_DEF")->asInt() )
	{
	case 0:	// predefined datum

		PROJ_ADD_STR("datum", P("DATUM")->asChoice()->Get_Data());

		break;

	//-----------------------------------------------------
	case 1:	// user defined datum

		switch( P("ELLIPSOID")->asInt() )
		{
		case 0:	// Predefined Ellipsoid
			PROJ_ADD_STR("ellps", P("ELLPS_DEF")->asChoice()->Get_Data());
			break;

		case 1:	// Semiminor axis
			PROJ_ADD_DBL("a"    , P("ELLPS_A" )->asDouble());
			PROJ_ADD_DBL("b"    , P("ELLPS_B" )->asDouble());
			break;

		case 2:	// Flattening
			PROJ_ADD_DBL("a"    , P("ELLPS_A" )->asDouble());
			PROJ_ADD_DBL("f"    , P("ELLPS_F" )->asDouble());
			break;

		case 3:	// Reciprocal Flattening
			PROJ_ADD_DBL("a"    , P("ELLPS_A" )->asDouble());
			PROJ_ADD_DBL("rf"   , P("ELLPS_RF")->asDouble());
			break;

		case 4:	// Eccentricity
			PROJ_ADD_DBL("a"    , P("ELLPS_A" )->asDouble());
			PROJ_ADD_DBL("e"    , P("ELLPS_E" )->asDouble());
			break;

		case 5:	// Eccentricity Squared
			PROJ_ADD_DBL("a"    , P("ELLPS_A" )->asDouble());
			PROJ_ADD_DBL("es"   , P("ELLPS_ES")->asDouble());
			break;
		}

		switch( P("DATUM_SHIFT")->asInt() )
		{
		case 1:	// Helmert Transformation (7 parameters)
			PROJ_ADD_STR("towgs84", CSG_String::Format("%s,%s,%s,%s,%s,%s,%s",
				SG_Get_String(P("DS_DX")->asDouble(), -32).c_str(),
				SG_Get_String(P("DS_DY")->asDouble(), -32).c_str(),
				SG_Get_String(P("DS_DZ")->asDouble(), -32).c_str(),
				SG_Get_String(P("DS_RX")->asDouble(), -32).c_str(),
				SG_Get_String(P("DS_RY")->asDouble(), -32).c_str(),
				SG_Get_String(P("DS_RZ")->asDouble(), -32).c_str(),
				SG_Get_String(P("DS_SC")->asDouble(), -32).c_str())
			);
			break;

		case 2:	// datum shift grid...
			if( SG_File_Exists(P("DS_GRID")->asString()) )
			{
				PROJ_ADD_STR("nadgrids", P("DS_GRID")->asString());
			}
			break;
		}

		break;
	}

	//-----------------------------------------------------
	if( P(ID) )
	{
		for(int i=0; i<P(ID)->Get_Children_Count(); i++)
		{
			CSG_Parameter &p = *P(ID)->Get_Child(i);

			CSG_String key = p.Get_Identifier() + ID.Length();

			switch( p.Get_Type() )
			{
			case PARAMETER_TYPE_Choice:
			case PARAMETER_TYPE_String: PROJ_ADD_STR(key, p.asString()); break;
			case PARAMETER_TYPE_Bool  : PROJ_ADD_BOL(key, p.asBool  ()); break;
			case PARAMETER_TYPE_Int   : PROJ_ADD_INT(key, p.asInt   ()); break;
			case PARAMETER_TYPE_Double: PROJ_ADD_DBL(key, p.asDouble()); break;
			default                   :                                   break;
			}
		}
	}

	//-----------------------------------------------------
	PROJ_ADD_BOL("no_defs", P("NO_DEFS")->asBool());
	PROJ_ADD_BOL("over"   , P("OVER"   )->asBool());

	return( Proj );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define _K(key)					(CSG_String("+") + key)
#define PROJ_HAS_KEY(key)		(Proj.Find(_K(key)) >= 0)
#define PROJ_HAS_VAL(key)		(Proj.Find(_K(key)) >= 0 && !Proj.Right(Proj.Length() - Proj.Find(_K(key))).BeforeFirst('=').Cmp(_K(key)))
#define PROJ_GET_VAL(key)		(Proj.Right(PROJ_HAS_VAL(key) ? Proj.Length() - Proj.Find(_K(key)) : 0).AfterFirst('=').BeforeFirst(' '))

#define PROJ_SET_BOL(key, id)	{             if( P(id)                                     ) P(id)->Set_Value(PROJ_HAS_KEY(key) ? 1 : 0); }
#define PROJ_SET_INT(key, id)	{ int    val; if( P(id) && PROJ_GET_VAL(key).asInt   (val) ) P(id)->Set_Value(val); }
#define PROJ_SET_FLT(key, id)	{ double val; if( P(id) && PROJ_GET_VAL(key).asDouble(val) ) P(id)->Set_Value(val); }
#define PROJ_SET_STR(key, id)	{             if( P(id) && PROJ_HAS_VAL(key)               ) P(id)->Set_Value(PROJ_GET_VAL(key)); }

#define PROJ_SET_CHC(key, id)	if( P(id) ) { CSG_Parameter_Choice *pList = P(id)->asChoice(); CSG_String s(PROJ_GET_VAL(key));\
		for(int i=0; i<pList->Get_Count(); i++)\
		{\
			if( !pList->Get_Item_Data(i).Cmp(s) )\
			{\
				P(id)->Set_Value(i); break;\
			}\
		}\
	}

//---------------------------------------------------------
bool CCRS_Base::Set_User_Definition(CSG_Parameters &P, const CSG_String &Proj)
{
	P.Restore_Defaults();

	PROJ_SET_CHC("proj"   , "PROJ_TYPE");
	PROJ_SET_CHC("datum"  , "DATUM"    );
	PROJ_SET_CHC("ellps"  , "ELLPS_DEF");
	PROJ_SET_CHC("units"  , "UNIT"     );

	PROJ_SET_FLT("lon_0"  , "LON_0"    );
	PROJ_SET_FLT("lat_0"  , "LAT_0"    );
	PROJ_SET_FLT("x_0"    , "X_0"      );
	PROJ_SET_FLT("y_0"    , "Y_0"      );
	PROJ_SET_FLT("k_0"    , "K_0"      );

	PROJ_SET_FLT("a"      , "ELLPS_A"  );
	PROJ_SET_FLT("b"      , "ELLPS_B"  );
	PROJ_SET_FLT("f"      , "ELLPS_F"  );
	PROJ_SET_FLT("rf"     , "ELLPS_RF" );
	PROJ_SET_FLT("e"      , "ELLPS_E"  );
	PROJ_SET_FLT("es"     , "ELLPS_ES" );

	PROJ_SET_BOL("no_defs", "NO_DEFS"  );
	PROJ_SET_BOL("over"   , "OVER"     );

	//-----------------------------------------------------
//	switch( P("DATUM_DEF")->asInt() )
	if( PROJ_HAS_VAL("datum") ) P("DATUM_DEF")->Set_Value(0);	//	case 0:	// predefined datum
	else                        P("DATUM_DEF")->Set_Value(1);	//	case 1:	// user defined datum

//	switch( P("ELLIPSOID")->asInt() )
	if( PROJ_HAS_VAL("ellps") )	P("ELLIPSOID")->Set_Value(0);	//	case 0:	// Predefined Ellipsoid
	if( PROJ_HAS_VAL("b"    ) )	P("ELLIPSOID")->Set_Value(1);	//	case 1:	// Semiminor axis
	if( PROJ_HAS_VAL("f"    ) )	P("ELLIPSOID")->Set_Value(2);	//	case 2:	// Flattening
	if( PROJ_HAS_VAL("rf"   ) )	P("ELLIPSOID")->Set_Value(3);	//	case 3:	// Reciprocal Flattening
	if( PROJ_HAS_VAL("e"    ) )	P("ELLIPSOID")->Set_Value(4);	//	case 4:	// Eccentricity
	if( PROJ_HAS_VAL("es"   ) )	P("ELLIPSOID")->Set_Value(5);	//	case 5:	// Eccentricity Squared

//	switch( P("DATUM_SHIFT")->asInt() )
	CSG_Strings params = SG_String_Tokenize(PROJ_GET_VAL("towgs84"), ",");

	if( params.Get_Count() > 0 )
	{
		P("DATUM_SHIFT")->Set_Value(1); // Helmert Transformation

		P("DS_DX")->Set_Value(params.Get_Count() > 0 ? params[0].asDouble() : 0.);
		P("DS_DY")->Set_Value(params.Get_Count() > 1 ? params[1].asDouble() : 0.);
		P("DS_DZ")->Set_Value(params.Get_Count() > 2 ? params[2].asDouble() : 0.);
		P("DS_RX")->Set_Value(params.Get_Count() > 3 ? params[3].asDouble() : 0.);
		P("DS_RY")->Set_Value(params.Get_Count() > 4 ? params[4].asDouble() : 0.);
		P("DS_RZ")->Set_Value(params.Get_Count() > 5 ? params[5].asDouble() : 0.);
		P("DS_SC")->Set_Value(params.Get_Count() > 6 ? params[6].asDouble() : 0.);
	}
	else if( PROJ_HAS_VAL("nadgrids") )
	{
		P("DATUM_SHIFT")->Set_Value(2); // Shift Grid

		PROJ_SET_STR("nadgrids", "DS_GRID");
	}
	else
	{
		P("DATUM_SHIFT")->Set_Value(0); // none
	}

	//-----------------------------------------------------
	CSG_String ID; P("PROJ_TYPE")->asChoice()->Get_Data(ID);

	if( P(ID) )
	{
		for(int i=0; i<P(ID)->Get_Children_Count(); i++)
		{
			CSG_Parameter &p = *P(ID)->Get_Child(i);

			CSG_String key = p.Get_Identifier() + ID.Length();

			switch( p.Get_Type() )
			{
			case PARAMETER_TYPE_Choice:
			case PARAMETER_TYPE_String: p.Set_Value(PROJ_GET_VAL(key)           ); break;
			case PARAMETER_TYPE_Bool  : p.Set_Value(PROJ_HAS_KEY(key) ? 1 : 0   ); break;
			case PARAMETER_TYPE_Int   : p.Set_Value(PROJ_GET_VAL(key).asInt   ()); break;
			case PARAMETER_TYPE_Double: p.Set_Value(PROJ_GET_VAL(key).asDouble()); break;
			default: break;
			}
		}
	}

	Parameters_Enable(&P, P("PROJ_TYPE"));

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CCRS_Picker::CCRS_Picker(void)
{
	Set_Name		(_TL("Coordinate Reference System Picker"));

	Set_Author		("O.Conrad (c) 2014");

	Set_Description	(_TW(
		"Define or pick a Coordinate Reference System (CRS). "
		"It is intended to call this tool only from other tools."
	));
}

//---------------------------------------------------------
bool CCRS_Picker::On_Execute(void)
{
	CSG_Projection Target;

	if( !Get_Projection(Target) )
	{
		return( false );
	}

	Message_Fmt("\n%s: %s", _TL("target"), Target.Get_PROJ().c_str());

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCRS_Transform::On_Execute(void)
{
	CSG_Projection Target;

	if( !Get_Projection(Target) || !m_Projector.Set_Target(Target) )
	{
		return( false );
	}

	//-----------------------------------------------------
	bool bResult = On_Execute_Transformation();

	Message_Fmt("\n\n%s: %s", _TL("source"), m_Projector.Get_Source().Get_PROJ().c_str());
	Message_Fmt(  "\n%s: %s", _TL("target"), m_Projector.Get_Target().Get_PROJ().c_str());

	m_Projector.Destroy();

	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
