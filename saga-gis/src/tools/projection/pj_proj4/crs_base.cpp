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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "crs_base.h"

extern "C" {
#include <projects.h>
}

//---------------------------------------------------------
#define PJ_GET_PROJS	pj_get_list_ref()
#define PJ_GET_DATUMS	pj_get_datums_ref()
#define PJ_GET_ELLPS	pj_get_ellps_ref()
#define PJ_GET_UNITS	pj_get_units_ref()


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CCRS_Base::CCRS_Base(void)
{
	m_Projection.Create(4326);	// EPSG: GCS WGS84

	//-----------------------------------------------------
	Parameters.Add_Choice("",
		"CRS_METHOD"	, _TL("Get CRS Definition from..."),
		_TL(""),
		CSG_String::Format("%s|%s|%s",
			_TL("Proj4 Parameters"),
			_TL("EPSG Code"),
			_TL("Well Known Text File")
		), 0
	)->Set_UseInGUI(false);

	//-----------------------------------------------------
	Parameters.Add_String("",
		"CRS_PROJ4"		, _TL("Proj4 Parameters"),
		_TL(""),
		m_Projection.Get_Proj4(), true
	);

	//-----------------------------------------------------
	Parameters.Add_Parameters("CRS_PROJ4",
		"CRS_DIALOG"	, _TL("User Defined"),
		_TL("")
	)->Set_UseInCMD(false);

	Set_User_Parameters(*Parameters("CRS_DIALOG")->asParameters());

	Parameters.Add_Parameters("CRS_PROJ4",
		"CRS_GRID"	, _TL("Loaded Grid")  , _TL("")
	)->asParameters()->Add_Grid("",
		"PICK"		, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL, false
	)->Set_UseInCMD(false);

	Parameters.Add_Parameters("CRS_PROJ4",
		"CRS_SHAPES", _TL("Loaded Shapes"), _TL("")
	)->asParameters()->Add_Shapes("",
		"PICK"		, _TL("Shapes"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL
	)->Set_UseInCMD(false);

	//-----------------------------------------------------
	Parameters.Add_FilePath("CRS_PROJ4",
		"CRS_FILE"		, _TL("Well Known Text File"),
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
	Parameters.Add_Int("CRS_PROJ4",
		"CRS_EPSG"		, _TL("Authority Code"),
		_TL(""),
		m_Projection.Get_EPSG()
	);

	Parameters.Add_String("CRS_EPSG",
		"CRS_EPSG_AUTH"	, _TL("Authority"),
		_TL(""),
		"EPSG"
	);

	Parameters.Add_Choice("CRS_EPSG",
		"CRS_EPSG_GEOGCS"	, _TL("Geographic Coordinate Systems"),
		_TL(""),
		SG_Get_Projections().Get_Names_List(SG_PROJ_TYPE_CS_Geographic)
	)->Set_UseInCMD(false);

	Parameters.Add_Choice("CRS_EPSG",
		"CRS_EPSG_PROJCS"	, _TL("Projected Coordinate Systems"),
		_TL(""),
		SG_Get_Projections().Get_Names_List(SG_PROJ_TYPE_CS_Projected)
	)->Set_UseInCMD(false);

	//-----------------------------------------------------
	Parameters.Add_Bool("",
		"PRECISE"		, _TL("Precise Datum Conversion"),
		_TL("avoids precision problems when source and target crs use different geodedtic datums."),
		false
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCRS_Base::On_Before_Execution(void)
{
	m_Projection.Create(Parameters("CRS_PROJ4")->asString(), SG_PROJ_FMT_Proj4);

	if( m_Projection.is_Okay() && Parameters("CRS_DIALOG") )
	{
		Set_User_Definition(*Parameters("CRS_DIALOG")->asParameters(), Parameters("CRS_PROJ4")->asString());
	}

	return( true );
}

//---------------------------------------------------------
int CCRS_Base::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	//-----------------------------------------------------
	CSG_Projection	Projection;

	//-----------------------------------------------------
	if( pParameter->Cmp_Identifier("CRS_PROJ4") )
	{
		Projection.Create(pParameter->asString(), SG_PROJ_FMT_Proj4);
	}

	//-----------------------------------------------------
	if(	pParameter->Cmp_Identifier("CRS_DIALOG") )
	{
		Projection.Create(Get_User_Definition(*pParameter->asParameters()), SG_PROJ_FMT_Proj4);
	}

	//-----------------------------------------------------
	if(	pParameter->Cmp_Identifier("CRS_FILE") )
	{
		Projection.Load(pParameter->asString());

		pParameter->Set_Value(CSG_String(""));	// clear
	}

	//-----------------------------------------------------
	if(	pParameter->Cmp_Identifier("CRS_EPSG") || pParameter->Cmp_Identifier("CRS_EPSG_AUTH") )
	{
		int			Code		= (*pParameters)("CRS_EPSG"     )->asInt   ();
		CSG_String	Authority	= (*pParameters)("CRS_EPSG_AUTH")->asString();

		if( Code >= 0 && !Authority.is_Empty() )
		{
			if( !Projection.Create(Code, Authority) )
			{
				SG_UI_Dlg_Message(_TL("Unknown Authority Code"), _TL("Warning"));
			}
		}
	}

	//-----------------------------------------------------
	if(	pParameter->Cmp_Identifier("CRS_EPSG_GEOGCS")
	||	pParameter->Cmp_Identifier("CRS_EPSG_PROJCS") )
	{
		int		EPSG;

		if( pParameter->asChoice()->Get_Data(EPSG) && Projection.Create(EPSG) )
		{
			pParameters->Set_Parameter("CRS_EPSG_AUTH", "EPSG");
		}
	}

	//-----------------------------------------------------
	if(	pParameter->Cmp_Identifier("CRS_GRID"  )
	||	pParameter->Cmp_Identifier("CRS_SHAPES") )
	{
		CSG_Data_Object	*pPick	= (*pParameter->asParameters())("PICK")->asDataObject();

		if( pPick && pPick->Get_Projection().is_Okay() )
		{
			Projection.Create(pPick->Get_Projection());
		}
	}

	//-----------------------------------------------------
	if( Projection.is_Okay() )
	{
		m_Projection	= Projection;

		pParameters->Set_Parameter("CRS_PROJ4"    , m_Projection.Get_Proj4       ());
		pParameters->Set_Parameter("CRS_EPSG"     , m_Projection.Get_Authority_ID());
		pParameters->Set_Parameter("CRS_EPSG_AUTH", m_Projection.Get_Authority   ());

		if( (*pParameters)("CRS_DIALOG") )
		{
			Set_User_Definition(*(*pParameters)("CRS_DIALOG")->asParameters(), m_Projection.Get_Proj4());
		}
	}

	//-----------------------------------------------------
	return( CSG_Tool::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
int CCRS_Base::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	pParameters->Cmp_Identifier("CRS_DIALOG") )
	{
		if(	pParameter->Cmp_Identifier("PROJ_TYPE") )
		{
			CSG_String	ID	= PJ_GET_PROJS[pParameter->asInt()].id;

			for(int i=0; i<pParameters->Get_Count(); i++)
			{
				CSG_Parameter	&p	= (*pParameters)[i];

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
			int		Value	= pParameter->asInt();

			pParameters->Set_Enabled("DATUM"      , Value == 0);
			pParameters->Set_Enabled("ELLIPSOID"  , Value == 1);
			pParameters->Set_Enabled("DATUM_SHIFT", Value == 1);
		}

		if(	pParameter->Cmp_Identifier("ELLIPSOID") )
		{
			int		Value	= pParameter->asInt();

			pParameters->Set_Enabled("ELLPS_DEF", Value == 0);
			pParameters->Set_Enabled("ELLPS_A"  , Value != 0);
			pParameters->Set_Enabled("ELLPS_B"  , Value == 1);
			pParameters->Set_Enabled("ELLPS_F"  , Value == 2);
			pParameters->Set_Enabled("ELLPS_RF" , Value == 3);
			pParameters->Set_Enabled("ELLPS_E"  , Value == 4);
			pParameters->Set_Enabled("ELLPS_ES" , Value == 5);
		}

		if(	pParameter->Cmp_Identifier("DATUM_SHIFT") )
		{
			int		Value	= pParameter->asInt();

			pParameters->Set_Enabled("DS_DX"     , Value == 1 || Value == 2);
			pParameters->Set_Enabled("DS_DY"     , Value == 1 || Value == 2);
			pParameters->Set_Enabled("DS_DZ"     , Value == 1 || Value == 2);
			pParameters->Set_Enabled("DS_RX"     , Value == 2);
			pParameters->Set_Enabled("DS_RY"     , Value == 2);
			pParameters->Set_Enabled("DS_RZ"     , Value == 2);
			pParameters->Set_Enabled("DS_SC"     , Value == 2);
			pParameters->Set_Enabled("DATUM_GRID", Value == 3);
		}
	}

	//-----------------------------------------------------
	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCRS_Base::Get_Projection(CSG_Projection &Projection)
{
	if( SG_UI_Get_Window_Main() )	// gui? projection is also updated on parameter changed event!
	{
		Projection	= m_Projection;
	}
	else switch( Parameters("CRS_METHOD")->asInt() )	// no gui? check out, how projection shall be initialized!
	{
	default:	// Proj4 Parameters
		if( !Projection.Create(Parameters("CRS_PROJ4")->asString(), SG_PROJ_FMT_Proj4) )
		{
			Error_Set(_TL("Proj4 definition string error"));
		}
		break;

	case  1:	// EPSG Code
		if( !Projection.Create(Parameters("CRS_EPSG")->asInt(), Parameters("CRS_EPSG_AUTH")->asString()) )
		{
			Error_Set(_TL("Authority code error"));
		}
		break;

	case  2:	// Well Known Text File
		if( !Projection.Load (Parameters("CRS_FILE")->asString()) )
		{
			Error_Set(_TL("Well Known Text file error"));
		}
		break;
	}

	return( Projection.is_Okay() );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define WGS84_ELLPS_A	6378137.000
#define WGS84_ELLPS_B	6356752.314

//---------------------------------------------------------
bool CCRS_Base::Set_User_Parameters(CSG_Parameters &P)
{
	CSG_String		Projections, Datums, Ellipsoids, Units, Description;

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

	Description	= _TL("Available Projections:");

	for(struct PJ_LIST *pProjection=PJ_GET_PROJS; pProjection->id; ++pProjection)
	{
		CSG_String	ID, Name, Args;

		ID		=  pProjection->id;
		Args	= *pProjection->descr;
		Name	= Args.BeforeFirst('\n');
		Args	= Args.AfterFirst ('\n').AfterFirst('\n').AfterFirst('\t');

		Projections	+=   "{" + ID + "}"  + Name + "|";
		Description	+= "\n[" + ID + "] " + Name + " (" + Args + ")";

		if( !Args.is_Empty() )
		{
			Add_User_Projection(P, ID, Args);
		}
	}

	if( Projections.is_Empty() )
	{
		return( false );
	}

	P("PROJ_TYPE")->asChoice()->Set_Items(Projections);
	P("PROJ_TYPE")->Set_Description(Description);

	// Datums ---------------------------------------------
	for(struct PJ_DATUMS *pDatum=PJ_GET_DATUMS; pDatum->id; ++pDatum)
	{
		CSG_String	id      (pDatum->id);
		CSG_String	comments(pDatum->comments);

		Datums	+= CSG_String::Format("{%s}%s|", id.c_str(), comments.Length() ? comments.c_str() : id.c_str());
	}

	// Ellipsoids -----------------------------------------
	for(struct PJ_ELLPS *pEllipse=PJ_GET_ELLPS; pEllipse->id; ++pEllipse)
	{
		Ellipsoids	+= CSG_String::Format("{%s}%s (%s, %s)|", SG_STR_MBTOSG(pEllipse->id), SG_STR_MBTOSG(pEllipse->name), SG_STR_MBTOSG(pEllipse->major), SG_STR_MBTOSG(pEllipse->ell));
	}

	// Units ----------------------------------------------
	for(struct PJ_UNITS *pUnit=PJ_GET_UNITS; pUnit->id; ++pUnit)
	{
		Units	+= CSG_String::Format("{%s}%s (%s)|", SG_STR_MBTOSG(pUnit->id), SG_STR_MBTOSG(pUnit->name), SG_STR_MBTOSG(pUnit->to_meter));
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
		CSG_String::Format("%s|%s|%s|%s",
			_TL("none"),
			_TL("3 parameters (translation only)"),
			_TL("7 parameters"),
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
		"DATUM_GRID", _TL("Datum Shift Grid File"),
		_TL(""),
		CSG_String::Format("%s|*.gsb|%s|*.*",
			_TL("NTv2 Grid Shift Binary (*.gsb)"),
			_TL("All Files")
		), NULL, false, false, false
	);


	//-----------------------------------------------------
	// General Settings...

	P.Add_Double("GENERAL", "LON_0"  , _TL("Central Meridian"), _TL(""), 0.0);
	P.Add_Double("GENERAL", "LAT_0"  , _TL("Central Parallel"), _TL(""), 0.0);
	P.Add_Double("GENERAL", "X_0"    , _TL("False Easting"   ), _TL(""), 0.0);
	P.Add_Double("GENERAL", "Y_0"    , _TL("False Northing"  ), _TL(""), 0.0);
	P.Add_Double("GENERAL", "K_0"    , _TL("Scale Factor"    ), _TL(""), 1.0, 0.0, true);
	P.Add_Choice("GENERAL", "UNIT"   , _TL("Unit"            ), _TL(""), Units, 1);
	P.Add_Bool  ("GENERAL", "NO_DEFS", _TL("Ignore Defaults" ), _TL(""), false);

	P.Add_Bool  ("GENERAL", "OVER"   , _TL("Allow longitudes outside -180 to 180 Range"), _TL(""), false);

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
	P.Add_Node("", ID, _TL("Type Specific Settings"), Args);

	//-----------------------------------------------------
	// Cylindrical Projections...

	if(	!ID.CmpNoCase("cea" )		// Equal Area Cylindrical
	||	!ID.CmpNoCase("eqc" )		// Equidistant Cylindrical (Plate Caree) 
	||	!ID.CmpNoCase("merc") )		// Mercator 
	{
		PRM_ADD_DBL("lat_ts", _TL("True Scale Latitude"), 0.0);
	}

	if(	!ID.CmpNoCase("utm" ) )	// Universal Transverse Mercator (UTM)
	{
		PRM_ADD_INT("zone" , _TL("Zone" ), 32);
		PRM_ADD_BOL("south", _TL("South"), false);
	}

	if(	!ID.CmpNoCase("omerc") )	// Oblique Mercator 
	{
		PRM_ADD_DBL("lat_1", _TL("Latitude 1" ),  40.0);
		PRM_ADD_DBL("lon_1", _TL("Longitude 1"), -20.0);
		PRM_ADD_DBL("lat_2", _TL("Latitude 2" ),  50.0);
		PRM_ADD_DBL("lon_2", _TL("Longitude 2"),  20.0);
	}

	//-----------------------------------------------------
	// Pseudocylindrical Projections...

	if(	!ID.CmpNoCase("gn_sinu") )	// General Sinusoidal Series
	{
		PRM_ADD_DBL("m", "m", 0.5           );
		PRM_ADD_DBL("n", "n", 1.0 + M_PI_045);
	}

	if(	!ID.CmpNoCase("loxim") )	// Loximuthal
	{
		PRM_ADD_DBL("lat_1", _TL("Latitude 1"), 40.0);
	}

	if(	!ID.CmpNoCase("urmfps") )	// Urmaev Flat-Polar Sinusoidal
	{
		PRM_ADD_DBL("n", "n", 1.0);
	}

	if(	!ID.CmpNoCase("urm5") )	// Urmaev V
	{
		PRM_ADD_DBL("n"    , "n"    ,  1.0);
		PRM_ADD_DBL("q"    , "q"    ,  1.0);
		PRM_ADD_DBL("alphi", "alphi", 45.0);
	}

	if(	!ID.CmpNoCase("wink1")		// Winkel I
	||	!ID.CmpNoCase("wag3" ) )	// Wagner III
	{
		PRM_ADD_DBL("lat_ts", _TL("True Scale Latitude"), 45.0);
	}

	if(	!ID.CmpNoCase("wink2") )	// Winkel II
	{
		PRM_ADD_DBL("lat_1", _TL("Latitude 1"), 40.0);
	}


	//-----------------------------------------------------
	// Conic Projections...

	if(	!ID.CmpNoCase("aea"   )	// Albers Equal Area
	||	!ID.CmpNoCase("eqdc"  )	// Equidistant Conic
	||	!ID.CmpNoCase("euler" )	// Euler 
	||	!ID.CmpNoCase("imw_p" )	// International Map of the World Polyconic 
	||	!ID.CmpNoCase("murd1" )	// Murdoch I 
	||	!ID.CmpNoCase("murd2" )	// Murdoch II 
	||	!ID.CmpNoCase("murd3" )	// Murdoch III 
	||	!ID.CmpNoCase("pconic")	// Perspective Conic 
	||	!ID.CmpNoCase("tissot")	// Tissot 
	||	!ID.CmpNoCase("vitk1" ) )	// Vitkovsky I 
	{
		PRM_ADD_DBL("lat_1", _TL("Latitude 1"), 33.0);
		PRM_ADD_DBL("lat_2", _TL("Latitude 2"), 45.0);
	}

	if(	!ID.CmpNoCase("lcc") )		// Lambert Conformal Conic 
	{
		PRM_ADD_DBL("lat_1", _TL("Latitude 1"), 33.0);
		PRM_ADD_DBL("lat_2", _TL("Latitude 2"), 45.0);
	}

	if( !ID.CmpNoCase("leac") )	// Lambert Equal Area Conic
	{
		PRM_ADD_DBL("lat_1", _TL("Latitude 1"), 45.0);
		PRM_ADD_BOL("south", _TL("South"     ), false);
	}

	if(	!ID.CmpNoCase("rpoly") )	// Rectangular Polyconic
	{
		PRM_ADD_DBL("lat_ts", _TL("True Scale Latitude"), 45.0);
	}

	if(	!ID.CmpNoCase("mpoly") )	// Modified Polyconic
	{
		PRM_ADD_DBL("lat_1", _TL("Latitude 1"), 33.0);
		PRM_ADD_DBL("lat_2", _TL("Latitude 2"), 45.0);
		PRM_ADD_BOL("lotsa", _TL("Lotsa"     ), true);
	}

	if(	!ID.CmpNoCase("bonne") )	// Bonne
	{
		PRM_ADD_DBL("lat_1", _TL("Latitude 1"), 45.0);
	}


	//-----------------------------------------------------
	// Azimuthal Projections...

	if(	!ID.CmpNoCase("stere") )	// Stereographic
	{
		PRM_ADD_DBL("lat_ts", _TL("True Scale Latitude"), 45.0);
	}

	if(	!ID.CmpNoCase("ups") )		// Universal Polar Stereographic
	{
		PRM_ADD_BOL("south", _TL("South"), true);
	}

	if(	!ID.CmpNoCase("airy") )	// Airy
	{
		PRM_ADD_DBL("lat_b" , _TL("Latitude B"), 45.0);
		PRM_ADD_BOL("no_cut", _TL("No Cut"    ), true);
	}

	if(	!ID.CmpNoCase("nsper") )	// Near-sided perspective
	{
		PRM_ADD_DBL("h", _TL("Height of view point"), 1.0);
	}

	if(	!ID.CmpNoCase("aeqd") )	// Azimuthal Equidistant
	{
		PRM_ADD_BOL("guam", _TL("Use Guam elliptical"), false);
	}

	if(	!ID.CmpNoCase("hammer") )	// Hammer & Eckert-Greifendorff
	{
		PRM_ADD_DBL("W", _TL("W"), 0.5);
		PRM_ADD_DBL("M", _TL("M"), 1.0);
	}

	if(	!ID.CmpNoCase("wintri") )	// Winkel Tripel 
	{
		PRM_ADD_DBL("lat_1", _TL("Latitude 1"), 40.0);
	}


	//-----------------------------------------------------
	// Miscellaneous Projections...

	if(	!ID.CmpNoCase("ocea" )		// Oblique Cylindrical Equal Area
	||	!ID.CmpNoCase("tpeqd") )	// Two Point Equidistant 
	{
		PRM_ADD_DBL("lat_1", _TL("Latitude 1" ),  40.0);
		PRM_ADD_DBL("lon_1", _TL("Longitude 1"), -20.0);
		PRM_ADD_DBL("lat_2", _TL("Latitude 2" ),  50.0);
		PRM_ADD_DBL("lon_2", _TL("Longitude 2"),  20.0);
	}

	if(	!ID.CmpNoCase("geos") )	// Geostationary Satellite View
	{
		PRM_ADD_DBL("h"    , _TL("Satellite Height [m]"), 35785831.0);
		PRM_ADD_CHO("sweep", _TL("Sweep Angle"         ), "x|y|");
	}

	if(	!ID.CmpNoCase("lsat") )	// Space oblique for LANDSAT
	{
		PRM_ADD_INT("lsat", _TL("Landsat Satellite (1-5)"), 5);
		PRM_ADD_INT("path", _TL("Path (1-255/233 1-3/4-5"), 195);
	}

	if(	!ID.CmpNoCase("labrd") )	// Laborde
	{
		PRM_ADD_DBL("azi", _TL("Azimuth"), 19.0);
	}

	if(	!ID.CmpNoCase("lagrng") )	// Lagrange
	{
		PRM_ADD_DBL("lat_1", _TL("Latitude 1"), 0.0);
		PRM_ADD_DBL("W"    , _TL("W"         ), 2.0);
	}

	if(	!ID.CmpNoCase("chamb") )	// Chamberlin Trimetric
	{
		PRM_ADD_DBL("lat_1", _TL("Latitude 1" ),  30.0);
		PRM_ADD_DBL("lon_1", _TL("Longitude 1"), -20.0);
		PRM_ADD_DBL("lat_2", _TL("Latitude 2" ),  40.0);
		PRM_ADD_DBL("lon_2", _TL("Longitude 2"),  00.0);
		PRM_ADD_DBL("lat_3", _TL("Latitude 3" ),  50.0);
		PRM_ADD_DBL("lon_3", _TL("Longitude 3"),  20.0);
	}

	if(	!ID.CmpNoCase("oea") )		// Oblated Equal Area
	{
		PRM_ADD_DBL("m"    , _TL("m"    ),  1.0);
		PRM_ADD_DBL("n"    , _TL("n"    ),  1.0);
		PRM_ADD_DBL("theta", _TL("theta"), 45.0);
	}

	if(	!ID.CmpNoCase("tpers") )	// Tilted perspective
	{
		PRM_ADD_DBL("tilt", _TL("Tilt"  ),   45.0);
		PRM_ADD_DBL("azi", _TL("Azimuth"),   45.0);
		PRM_ADD_DBL("h"  , _TL("h"      ), 1000.0);
	}

	if(	!ID.CmpNoCase("ob_tran") )	// General Oblique Transformation
	{
		PRM_ADD_DBL("o_lat_p", _TL("Latitude Pole" ), 40.0);
		PRM_ADD_DBL("o_lon_p", _TL("Longitude Pole"), 40.0);
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define PROJ4_ADD_BOL(key, val)	if( val ) Proj4 += CSG_String::Format("+%s ", CSG_String(key).c_str());
#define PROJ4_ADD_INT(key, val)	Proj4 += CSG_String::Format("+%s=%d ", CSG_String(key).c_str(), val);
#define PROJ4_ADD_DBL(key, val)	Proj4 += CSG_String::Format("+%s=%s ", CSG_String(key).c_str(), SG_Get_String(val, -32).c_str());
#define PROJ4_ADD_STR(key, val)	Proj4 += CSG_String::Format("+%s=%s ", CSG_String(key).c_str(), CSG_String(val).c_str());
#define PROJ4_GET_PRM(parm_id)	P(ID + parm_id)

//---------------------------------------------------------
CSG_String CCRS_Base::Get_User_Definition(CSG_Parameters &P)
{
	CSG_String	Proj4, ID = PJ_GET_PROJS[P("PROJ_TYPE")->asInt()].id;	// P("PROJ_TYPE")->asChoice()->Get_Data(ID);

	//-----------------------------------------------------
	PROJ4_ADD_STR("proj", ID);

	if( P("LON_0")->asDouble() )	PROJ4_ADD_DBL("lon_0", P("LON_0")->asDouble());
	if( P("LAT_0")->asDouble() )	PROJ4_ADD_DBL("lat_0", P("LAT_0")->asDouble());

	if( P("X_0"  )->asDouble() )	PROJ4_ADD_DBL("x_0"  , P("X_0"  )->asDouble());
	if( P("Y_0"  )->asDouble() )	PROJ4_ADD_DBL("y_0"  , P("Y_0"  )->asDouble());

	if( P("K_0")->asDouble() != 1.0 && P("K_0")->asDouble() > 0.0 )
	{
		PROJ4_ADD_DBL("k_0", P("K_0")->asDouble());
	}

	PROJ4_ADD_STR("units", PJ_GET_UNITS[P("UNIT")->asInt()].id);

	//-----------------------------------------------------
	switch( P("DATUM_DEF")->asInt() )
	{
	case 0:	// predefined datum

		PROJ4_ADD_STR("datum", PJ_GET_DATUMS[P("DATUM")->asInt()].id);

		break;

	//-----------------------------------------------------
	case 1:	// user defined datum

		switch( P("ELLIPSOID")->asInt() )
		{
		case 0:	// Predefined Ellipsoid
			PROJ4_ADD_STR("ellps", PJ_GET_ELLPS[P("ELLPS_DEF")->asInt()].id);
			break;

		case 1:	// Semiminor axis
			PROJ4_ADD_DBL("a"    , P("ELLPS_A" )->asDouble());
			PROJ4_ADD_DBL("b"    , P("ELLPS_B" )->asDouble());
			break;

		case 2:	// Flattening
			PROJ4_ADD_DBL("a"    , P("ELLPS_A" )->asDouble());
			PROJ4_ADD_DBL("f"    , P("ELLPS_F" )->asDouble());
			break;

		case 3:	// Reciprocal Flattening
			PROJ4_ADD_DBL("a"    , P("ELLPS_A" )->asDouble());
			PROJ4_ADD_DBL("rf"   , P("ELLPS_RF")->asDouble());
			break;

		case 4:	// Eccentricity
			PROJ4_ADD_DBL("a"    , P("ELLPS_A" )->asDouble());
			PROJ4_ADD_DBL("e"    , P("ELLPS_E" )->asDouble());
			break;

		case 5:	// Eccentricity Squared
			PROJ4_ADD_DBL("a"    , P("ELLPS_A" )->asDouble());
			PROJ4_ADD_DBL("es"   , P("ELLPS_ES")->asDouble());
			break;
		}

		switch( P("DATUM_SHIFT")->asInt() )
		{
		case 1:	// 3 parameters
			PROJ4_ADD_STR("towgs84", CSG_String::Format("%s,%s,%s",
				SG_Get_String(P("DS_DX")->asDouble(), -32).c_str(),
				SG_Get_String(P("DS_DY")->asDouble(), -32).c_str(),
				SG_Get_String(P("DS_DZ")->asDouble(), -32).c_str())
			);
			break;

		case 2:	// 7 parameters
			PROJ4_ADD_STR("towgs84", CSG_String::Format("%s,%s,%s,%s,%s,%s,%s",
				SG_Get_String(P("DS_DX")->asDouble(), -32).c_str(),
				SG_Get_String(P("DS_DY")->asDouble(), -32).c_str(),
				SG_Get_String(P("DS_DZ")->asDouble(), -32).c_str(),
				SG_Get_String(P("DS_RX")->asDouble(), -32).c_str(),
				SG_Get_String(P("DS_RY")->asDouble(), -32).c_str(),
				SG_Get_String(P("DS_RZ")->asDouble(), -32).c_str(),
				SG_Get_String(P("DS_SC")->asDouble(), -32).c_str())
			);
			break;

		case 3:	// datum shift grid...
			if( SG_File_Exists(P("DATUM_GRID")->asString()) )
			{
				PROJ4_ADD_STR("nadgrids", P("DATUM_GRID")->asString());
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
			CSG_Parameter	&p	= *P(ID)->Get_Child(i);

			CSG_String	key = p.Get_Identifier() + ID.Length();

			switch( p.Get_Type() )
			{
			case PARAMETER_TYPE_Choice:
			case PARAMETER_TYPE_String: PROJ4_ADD_STR(key, p.asString());	break;
			case PARAMETER_TYPE_Bool  : PROJ4_ADD_BOL(key, p.asBool  ());	break;
			case PARAMETER_TYPE_Int   : PROJ4_ADD_INT(key, p.asInt   ());	break;
			case PARAMETER_TYPE_Double: PROJ4_ADD_DBL(key, p.asDouble());	break;
			}
		}
	}

	//-----------------------------------------------------
	PROJ4_ADD_BOL("no_defs", P("NO_DEFS")->asBool());
	PROJ4_ADD_BOL("over"   , P("OVER"   )->asBool());

	return( Proj4 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define _K(key)					(CSG_String("+") + key)
#define PROJ4_HAS_KEY(key)		(Proj4.Find(_K(key)) >= 0)
#define PROJ4_HAS_VAL(key)		(Proj4.Find(_K(key)) >= 0 && !Proj4.Right(Proj4.Length() - Proj4.Find(_K(key))).BeforeFirst('=').Cmp(_K(key)))
#define PROJ4_GET_VAL(key)		(Proj4.Right(PROJ4_HAS_VAL(key) ? Proj4.Length() - Proj4.Find(_K(key)) : 0).AfterFirst('=').BeforeFirst(' '))

#define PROJ4_SET_BOL(key, id)	{             if( P(id)                                     ) P(id)->Set_Value(PROJ4_HAS_KEY(key) ? 1 : 0); }
#define PROJ4_SET_INT(key, id)	{ int    val; if( P(id) && PROJ4_GET_VAL(key).asInt   (val) ) P(id)->Set_Value(val); }
#define PROJ4_SET_FLT(key, id)	{ double val; if( P(id) && PROJ4_GET_VAL(key).asDouble(val) ) P(id)->Set_Value(val); }
#define PROJ4_SET_STR(key, id)	{             if( P(id) && PROJ4_HAS_VAL(key)               ) P(id)->Set_Value(PROJ4_GET_VAL(key)); }

#define PROJ4_SET_CHC(key, id)	if( P(id) ) { CSG_Parameter_Choice *pList = P(id)->asChoice(); CSG_String s(PROJ4_GET_VAL(key));\
		for(int i=0; i<pList->Get_Count(); i++)\
		{\
			if( !pList->Get_Item_Data(i).Cmp(s) )\
			{\
				P(id)->Set_Value(i); break;\
			}\
		}\
	}

//---------------------------------------------------------
bool CCRS_Base::Set_User_Definition(CSG_Parameters &P, const CSG_String &Proj4)
{
	P.Restore_Defaults();

	PROJ4_SET_CHC("proj" , "PROJ_TYPE");
	PROJ4_SET_CHC("datum", "DATUM"    );
	PROJ4_SET_CHC("ellps", "ELLPS_DEF");
	PROJ4_SET_CHC("units", "UNIT"     );

	PROJ4_SET_FLT("lon_0", "LON_0"    );
	PROJ4_SET_FLT("lat_0", "LAT_0"    );
	PROJ4_SET_FLT("x_0"  , "X_0"      );
	PROJ4_SET_FLT("y_0"  , "Y_0"      );
	PROJ4_SET_FLT("k_0"  , "K_0"      );

	PROJ4_SET_FLT("a"    , "ELLPS_A"  );
	PROJ4_SET_FLT("b"    , "ELLPS_B"  );
	PROJ4_SET_FLT("f"    , "ELLPS_F"  );
	PROJ4_SET_FLT("rf"   , "ELLPS_RF" );
	PROJ4_SET_FLT("e"    , "ELLPS_E"  );
	PROJ4_SET_FLT("es"   , "ELLPS_ES" );

	PROJ4_SET_BOL("no_defs", "NO_DEFS");
	PROJ4_SET_BOL("over"   , "OVER"   );

	//-----------------------------------------------------
//	switch( P("DATUM_DEF")->asInt() )
	if( PROJ4_HAS_VAL("datum") )	P("DATUM_DEF")->Set_Value(0);	//	case 0:	// predefined datum
	else							P("DATUM_DEF")->Set_Value(1);	//	case 1:	// user defined datum

//	switch( P("ELLIPSOID")->asInt() )
	if( PROJ4_HAS_VAL("ellps") )	P("ELLIPSOID")->Set_Value(0);	//	case 0:	// Predefined Ellipsoid
	if( PROJ4_HAS_VAL("b"    ) )	P("ELLIPSOID")->Set_Value(1);	//	case 1:	// Semiminor axis
	if( PROJ4_HAS_VAL("f"    ) )	P("ELLIPSOID")->Set_Value(2);	//	case 2:	// Flattening
	if( PROJ4_HAS_VAL("rf"   ) )	P("ELLIPSOID")->Set_Value(3);	//	case 3:	// Reciprocal Flattening
	if( PROJ4_HAS_VAL("e"    ) )	P("ELLIPSOID")->Set_Value(4);	//	case 4:	// Eccentricity
	if( PROJ4_HAS_VAL("es"   ) )	P("ELLIPSOID")->Set_Value(5);	//	case 5:	// Eccentricity Squared

//	switch( P("DATUM_SHIFT")->asInt() )
	CSG_String_Tokenizer	params(PROJ4_GET_VAL("towgs84"), ", ");

	if( params.Get_Tokens_Count() == 3 || params.Get_Tokens_Count() == 7 )
	{
		P("DATUM_SHIFT")->Set_Value(1);	//	case 1:	// 3 parameters

		P("DS_DX")->Set_Value(params.Get_Next_Token().asDouble());
		P("DS_DY")->Set_Value(params.Get_Next_Token().asDouble());
		P("DS_DZ")->Set_Value(params.Get_Next_Token().asDouble());
	}
	else if( params.Get_Tokens_Count() == 7 )
	{
		P("DATUM_SHIFT")->Set_Value(2);	//	case 2:	// 7 parameters

		P("DS_DX")->Set_Value(params.Get_Next_Token().asDouble());
		P("DS_DY")->Set_Value(params.Get_Next_Token().asDouble());
		P("DS_DZ")->Set_Value(params.Get_Next_Token().asDouble());
		P("DS_RX")->Set_Value(params.Get_Next_Token().asDouble());
		P("DS_RY")->Set_Value(params.Get_Next_Token().asDouble());
		P("DS_RZ")->Set_Value(params.Get_Next_Token().asDouble());
		P("DS_SC")->Set_Value(params.Get_Next_Token().asDouble());
	}
	else if( PROJ4_HAS_VAL("nadgrids") && SG_File_Exists(PROJ4_GET_VAL("nadgrids")) )
	{
		P("DATUM_SHIFT")->Set_Value(3);	//	case 3:	// datum shift grid...

		PROJ4_SET_STR("nadgrids", "DATUM_GRID");
	}
	else
	{
		P("DATUM_SHIFT")->Set_Value(0);	//	case 0:	// none
	}

	//-----------------------------------------------------
	CSG_String	ID	= PJ_GET_PROJS[P("PROJ_TYPE")->asInt()].id;

	if( P(ID) )
	{
		for(int i=0; i<P(ID)->Get_Children_Count(); i++)
		{
			CSG_Parameter	&p	= *P(ID)->Get_Child(i);

			CSG_String	key = p.Get_Identifier() + ID.Length();

			switch( p.Get_Type() )
			{
			case PARAMETER_TYPE_Choice:
			case PARAMETER_TYPE_String: p.Set_Value(PROJ4_GET_VAL(key)           );	break;
			case PARAMETER_TYPE_Bool  : p.Set_Value(PROJ4_HAS_KEY(key) ? 1 : 0   );	break;
			case PARAMETER_TYPE_Int   : p.Set_Value(PROJ4_GET_VAL(key).asInt   ());	break;
			case PARAMETER_TYPE_Double: p.Set_Value(PROJ4_GET_VAL(key).asDouble());	break;
			}
		}
	}

	On_Parameters_Enable(&P, P("PROJ_TYPE"));

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
	//-----------------------------------------------------
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
	CSG_Projection	Target;

	if( !Get_Projection(Target) )
	{
		return( false );
	}

	Message_Fmt("\n%s: %s", _TL("target"), Target.Get_Proj4().c_str());

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
	//-----------------------------------------------------
	CSG_Projection	Target;

	if( !Get_Projection(Target) || !m_Projector.Set_Target(Target) )
	{
		return( false );
	}

	Message_Fmt("\n%s: %s", _TL("target"), Target.Get_Proj4().c_str());

	//-----------------------------------------------------
	m_Projector.Set_Precise_Mode(Parameters("PRECISE") && Parameters("PRECISE")->asBool());

	bool	bResult	= On_Execute_Transformation();

	m_Projector.Destroy();

	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
