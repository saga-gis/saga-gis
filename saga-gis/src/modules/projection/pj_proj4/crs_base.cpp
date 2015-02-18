/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
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
// Free Software Foundation; version 2 of the License.   //
//                                                       //
// SAGA is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU General Public        //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU General    //
// Public License along with this program; if not,       //
// write to the Free Software Foundation, Inc.,          //
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
// USA.                                                  //
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

#include <projects.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CCRS_Base::CCRS_Base(void)
{
	m_Projection.Create(4326);

	CSG_Parameter	*pNode_0, *pNode_1;

	//-----------------------------------------------------
	if( SG_UI_Get_Window_Main() == NULL )
	{
		Parameters.Add_Choice(
			NULL	, "CRS_METHOD"		, _TL("Get CRS Definition from..."),
			_TL(""),
			CSG_String::Format(SG_T("%s|%s|%s|"),
				_TL("Proj4 Parameters"),
				_TL("EPSG Code"),
				_TL("Well Known Text File")
			), 0
		);
	}

	//-----------------------------------------------------
	pNode_0	= Parameters.Add_String(
		NULL	, "CRS_PROJ4"		, _TL("Proj4 Parameters"),
		_TL(""),
		m_Projection.Get_Proj4(), true
	);

	if( SG_UI_Get_Window_Main() )
	{
		Parameters.Add_Parameters(
			pNode_0	, "CRS_DIALOG"		, _TL("User Defined"),
			_TL("")
		);

		Set_User_Parameters(Parameters("CRS_DIALOG")->asParameters());
	}

	//-----------------------------------------------------
	if( SG_UI_Get_Window_Main() )
	{
		pNode_1	= Parameters.Add_Parameters(pNode_0, "CRS_GRID"  , _TL("Loaded Grid")  , _TL(""));

		pNode_1->asParameters()->Add_Grid(
			NULL	, "PICK"	, _TL("Grid"),
			_TL(""),
			PARAMETER_INPUT_OPTIONAL, false
		);

		pNode_1	= Parameters.Add_Parameters(pNode_0, "CRS_SHAPES", _TL("Loaded Shapes"), _TL(""));

		pNode_1->asParameters()->Add_Shapes(
			NULL	, "PICK"	, _TL("Shapes"),
			_TL(""),
			PARAMETER_INPUT_OPTIONAL
		);
	}

	//-----------------------------------------------------
	Parameters.Add_FilePath(
		pNode_0	, "CRS_FILE"		, _TL("Well Known Text File"),
		_TL(""),
		CSG_String::Format(
			SG_T("%s|*.prj;*.wkt;*.txt|%s|*.prj|%s|*.wkt|%s|*.txt|%s|*.*"),
			_TL("All Recognized Files"),
			_TL("ESRI WKT Files (*.prj)"),
			_TL("WKT Files (*.wkt)"),
			_TL("Text Files (*.txt)"),
			_TL("All Files")
		)
	);

	//-----------------------------------------------------
	pNode_1	= Parameters.Add_Value(
		pNode_0	, "CRS_EPSG"	, _TL("EPSG Code"),
		_TL(""),
		PARAMETER_TYPE_Int, m_Projection.Get_EPSG(), -1, true, 99999, true
	);

	if( SG_UI_Get_Window_Main() )
	{
		Parameters.Add_Choice(
			pNode_1	, "CRS_EPSG_GEOGCS"	, _TL("Geographic Coordinate Systems"),
			_TL(""),
			SG_Get_Projections().Get_Names_List(SG_PROJ_TYPE_CS_Geographic)
		);

		Parameters.Add_Choice(
			pNode_1	, "CRS_EPSG_PROJCS"	, _TL("Projected Coordinate Systems"),
			_TL(""),
			SG_Get_Projections().Get_Names_List(SG_PROJ_TYPE_CS_Projected)
		);
	}

	//-----------------------------------------------------
	Parameters.Add_Value(
		NULL	, "PRECISE"		, _TL("Precise Datum Conversion"),
		_TL("avoids precision problems when source and target crs use different geodedtic datums."),
		PARAMETER_TYPE_Bool, false
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
	if(	!SG_STR_CMP(pParameters->Get_Identifier(), "CRS_DIALOG") )
	{
		if(	!SG_STR_CMP(pParameter->Get_Identifier(), "PROJ_TYPE") )
		{
			pParameters->Get_Parameter("OPTIONS")->asParameters()->Assign(Get_Parameters(SG_STR_MBTOSG(pj_list[pParameter->asInt()].id)));
		}

		return( 1 );
	}

	//-----------------------------------------------------
	CSG_Projection	Projection;

	//-----------------------------------------------------
	if( pParameters->Get_Parameter("CRS_PROJ4") )
	{
		Projection.Create(pParameter->asString(), SG_PROJ_FMT_Proj4);
	}

	//-----------------------------------------------------
	if(	!SG_STR_CMP(pParameter->Get_Identifier(), "CRS_DIALOG") )
	{
		Projection.Create(Get_User_Definition(*pParameter->asParameters()), SG_PROJ_FMT_Proj4);
	}

	//-----------------------------------------------------
	if(	!SG_STR_CMP(pParameter->Get_Identifier(), "CRS_FILE") )
	{
		Projection.Load(pParameter->asString());

		pParameter->Set_Value(CSG_String(""));	// clear
	}

	//-----------------------------------------------------
	if(	!SG_STR_CMP(pParameter->Get_Identifier(), "CRS_EPSG") )
	{
		if (!Projection.Create(pParameter->asInt()))
		{
			SG_UI_Dlg_Message(_TL("Unkown EPSG Code"), _TL("WARNING"));
		}
	}

	//-----------------------------------------------------
	if(	!SG_STR_CMP(pParameter->Get_Identifier(), "CRS_EPSG_GEOGCS")
	||	!SG_STR_CMP(pParameter->Get_Identifier(), "CRS_EPSG_PROJCS") )
	{
		int		EPSG;

		if( pParameter->asChoice()->Get_Data(EPSG) )
		{
			Projection.Create(EPSG);
		}
	}

	//-----------------------------------------------------
	if(	!SG_STR_CMP(pParameter->Get_Identifier(), "CRS_GRID")
	||	!SG_STR_CMP(pParameter->Get_Identifier(), "CRS_SHAPES") )
	{
		CSG_Data_Object	*pPick	= pParameter->asParameters()->Get_Parameter("PICK")->asDataObject();

		if( pPick && pPick->Get_Projection().is_Okay() )
		{
			Projection.Create(pPick->Get_Projection());
		}
	}

	//-----------------------------------------------------
	if( Projection.is_Okay() )
	{
		m_Projection	= Projection;

		pParameters->Get_Parameter("CRS_PROJ4")->Set_Value(m_Projection.Get_Proj4());
		pParameters->Get_Parameter("CRS_EPSG" )->Set_Value(m_Projection.Get_EPSG ());

		if( pParameters->Get_Parameter("CRS_DIALOG") )
		{
			Set_User_Definition(*pParameters->Get_Parameter("CRS_DIALOG")->asParameters(), m_Projection.Get_Proj4());
		}
	}

	//-----------------------------------------------------
	return( 1 );
}

//---------------------------------------------------------
int CCRS_Base::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	!SG_STR_CMP(pParameters->Get_Identifier(), "CRS_DIALOG") )
	{
		if(	!SG_STR_CMP(pParameter->Get_Identifier(), "PROJ_TYPE") )
		{
		//	pParameters->Get_Parameter("OPTIONS")->asParameters()->Assign(Get_Parameters(SG_STR_MBTOSG(pj_list[pParameter->asInt()].id)));

			pParameters->Get_Parameter("OPTIONS")->Set_Enabled(pParameters->Get_Parameter("OPTIONS")->asParameters()->Get_Count() > 0);
		}

		if(	!SG_STR_CMP(pParameter->Get_Identifier(), "DATUM_DEF") )
		{
			int		Value	= pParameter->asInt();

			pParameters->Get_Parameter("DATUM"    )->Set_Enabled(Value == 0);
			pParameters->Get_Parameter("ELLIPSOID")->Set_Enabled(Value == 1);
		}

		if(	!SG_STR_CMP(pParameter->Get_Identifier(), "ELLIPSOID") )
		{
			int		Value	= pParameter->asInt();

			pParameters->Get_Parameter("ELLPS_DEF")->Set_Enabled(Value == 0);
			pParameters->Get_Parameter("ELLPS_A"  )->Set_Enabled(Value != 0);
			pParameters->Get_Parameter("ELLPS_B"  )->Set_Enabled(Value == 1);
			pParameters->Get_Parameter("ELLPS_F"  )->Set_Enabled(Value == 2);
			pParameters->Get_Parameter("ELLPS_RF" )->Set_Enabled(Value == 3);
			pParameters->Get_Parameter("ELLPS_E"  )->Set_Enabled(Value == 4);
			pParameters->Get_Parameter("ELLPS_ES" )->Set_Enabled(Value == 5);
		}

		if(	!SG_STR_CMP(pParameter->Get_Identifier(), "DATUM_SHIFT") )
		{
			int		Value	= pParameter->asInt();

			pParameters->Get_Parameter("DS_DX"     )->Set_Enabled(Value == 1 || Value == 2);
			pParameters->Get_Parameter("DS_DY"     )->Set_Enabled(Value == 1 || Value == 2);
			pParameters->Get_Parameter("DS_DZ"     )->Set_Enabled(Value == 1 || Value == 2);
			pParameters->Get_Parameter("DS_RX"     )->Set_Enabled(Value == 2);
			pParameters->Get_Parameter("DS_RY"     )->Set_Enabled(Value == 2);
			pParameters->Get_Parameter("DS_RZ"     )->Set_Enabled(Value == 2);
			pParameters->Get_Parameter("DS_SC"     )->Set_Enabled(Value == 2);
			pParameters->Get_Parameter("DATUM_GRID")->Set_Enabled(Value == 3);
		}
	}

	//-----------------------------------------------------
	return( 1 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCRS_Base::Get_Projection(CSG_Projection &Projection)
{
	if( !Parameters("CRS_METHOD") )
	{
		Projection	= m_Projection;
	}
	else switch( Parameters("CRS_METHOD")->asInt() )
	{
	default:	// Proj4 Parameters
		if( !Projection.Create(Parameters("CRS_PROJ4")->asString(), SG_PROJ_FMT_Proj4) )
		{
			Error_Set(_TL("Proj4 defintion string error"));
		}
		break;

	case  1:	// EPSG Code
		if( !Projection.Create(Parameters("CRS_EPSG" )->asInt()) )
		{
			Error_Set(_TL("EPSG code error"));
		}
		break;

	case  2:	// Well Known Text File
		if( !Projection.Load (Parameters("CRS_FILE"  )->asString()) )
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
bool CCRS_Base::Set_User_Parameters(CSG_Parameters *pParameters)
{
	CSG_Parameter	*pNode_0 = NULL, *pNode_1, *pNode_2;
	CSG_String		sProjections, sName, sDescription, sArguments, sDatums, sEllipsoids, sUnits;

	///////////////////////////////////////////////////////

	// Projection -----------------------------------------
	sDescription	= _TL("Available Projections:");

	for(struct PJ_LIST *pProjection=pj_list; pProjection->id; ++pProjection)
	{
		sArguments		= *pProjection->descr;
		sName			= sArguments.BeforeFirst('\n');
		sArguments		= sArguments.AfterFirst ('\n').AfterFirst('\n').AfterFirst('\t');

		sProjections	+= CSG_String::Format(SG_T("{%s}%s|")       , SG_STR_MBTOSG(pProjection->id), sName.c_str());
		sDescription	+= CSG_String::Format(SG_T("\n[%s] %s (%s)"), SG_STR_MBTOSG(pProjection->id), sName.c_str(), sArguments.c_str());

		Add_User_Projection(pProjection->id, sName, sArguments);
	}

	// Datums ---------------------------------------------
	for(struct PJ_DATUMS *pDatum=pj_datums; pDatum->id; ++pDatum)
	{
		CSG_String	id      (pDatum->id);
		CSG_String	comments(pDatum->comments);

		sDatums	+= CSG_String::Format(SG_T("{%s}%s|"), id.c_str(), comments.Length() ? comments.c_str() : id.c_str());
	}

	// Ellipsoids -----------------------------------------
	for(struct PJ_ELLPS *pEllipse=pj_ellps; pEllipse->id; ++pEllipse)
	{
		sEllipsoids	+= CSG_String::Format(SG_T("{%s}%s (%s, %s)|"), SG_STR_MBTOSG(pEllipse->id), SG_STR_MBTOSG(pEllipse->name), SG_STR_MBTOSG(pEllipse->major), SG_STR_MBTOSG(pEllipse->ell));
	}

	// Units ----------------------------------------------
	for(struct PJ_UNITS *pUnit=pj_units; pUnit->id; ++pUnit)
	{
		sUnits	+= CSG_String::Format(SG_T("{%s}%s (%s)|"), SG_STR_MBTOSG(pUnit->id), SG_STR_MBTOSG(pUnit->name), SG_STR_MBTOSG(pUnit->to_meter));
	}


	///////////////////////////////////////////////////////

	//-----------------------------------------------------
	// Projection...

	if( sProjections.Length() == 0 )
	{
		return( false );
	}

	pParameters->Add_Choice	(pNode_0, "PROJ_TYPE"	, _TL("Projection Type")			, sDescription, sProjections);


	//-----------------------------------------------------
	// Datum...

	pNode_1	= pParameters->Add_Choice(
		pNode_0, "DATUM_DEF"		, _TL("Datum Definition"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("Predefined Datum"),
			_TL("User Defined Datum")
		)
	);


	//-----------------------------------------------------
	// Predefined Datum...

	pParameters->Add_Choice	(pNode_1, "DATUM"		, _TL("Predefined Datum")			, _TL(""), sDatums);


	//-----------------------------------------------------
	// Ellipsoid...

	pNode_2	= pParameters->Add_Choice(
		pNode_1, "ELLIPSOID"		, _TL("Ellipsoid Definition"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|%s|"),
			_TL("Predefined Ellipsoids"),
			_TL("Semimajor Axis and Semiminor Axis"),
			_TL("Semimajor Axis and Flattening"),
			_TL("Semimajor Axis and Reciprocal Flattening"),
			_TL("Semimajor Axis and Eccentricity"),
			_TL("Semimajor Axis and Eccentricity Squared")
		)
	);

	pParameters->Add_Choice	(pNode_2, "ELLPS_DEF"	, _TL("Predefined Ellipsoids")		, _TL(""), sEllipsoids);

	pParameters->Add_Value	(pNode_2, "ELLPS_A"		, _TL("Semimajor Axis (a)")			, _TL(""), PARAMETER_TYPE_Double, WGS84_ELLPS_A);
	pParameters->Add_Value	(pNode_2, "ELLPS_B"		, _TL("Semiminor Axis (b)")			, _TL(""), PARAMETER_TYPE_Double, WGS84_ELLPS_B);
	pParameters->Add_Value	(pNode_2, "ELLPS_F"		, _TL("Flattening (f)")				, _TL(""), PARAMETER_TYPE_Double, (WGS84_ELLPS_A - WGS84_ELLPS_B) / WGS84_ELLPS_A);
	pParameters->Add_Value	(pNode_2, "ELLPS_RF"	, _TL("Reciprocal Flattening (rf)")	, _TL(""), PARAMETER_TYPE_Double, WGS84_ELLPS_A / (WGS84_ELLPS_A - WGS84_ELLPS_B));
	pParameters->Add_Value	(pNode_2, "ELLPS_E"		, _TL("Eccentricity (e)")			, _TL(""), PARAMETER_TYPE_Double, sqrt(WGS84_ELLPS_A*WGS84_ELLPS_A + WGS84_ELLPS_B*WGS84_ELLPS_B));
	pParameters->Add_Value	(pNode_2, "ELLPS_ES"	, _TL("Squared Eccentricity (es)")	, _TL(""), PARAMETER_TYPE_Double, WGS84_ELLPS_A*WGS84_ELLPS_A + WGS84_ELLPS_B*WGS84_ELLPS_B);


	//-----------------------------------------------------
	// Datum Shift...

	pNode_2	= pParameters->Add_Choice(
		pNode_1, "DATUM_SHIFT"	, _TL("Datum Shift"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|"),
			_TL("none"),
			_TL("3 parameters (translation only)"),
			_TL("7 parameters"),
			_TL("Datum Shift Grid")
		)
	);

	pParameters->Add_Value	(pNode_2, "DS_DX"		, _TL("Translation X")				, _TL(""), PARAMETER_TYPE_Double, 0.0);
	pParameters->Add_Value	(pNode_2, "DS_DY"		, _TL("Translation Y")				, _TL(""), PARAMETER_TYPE_Double, 0.0);
	pParameters->Add_Value	(pNode_2, "DS_DZ"		, _TL("Translation Z")				, _TL(""), PARAMETER_TYPE_Double, 0.0);
	pParameters->Add_Value	(pNode_2, "DS_RX"		, _TL("Rotation X")					, _TL(""), PARAMETER_TYPE_Double, 0.0);
	pParameters->Add_Value	(pNode_2, "DS_RY"		, _TL("Rotation Y")					, _TL(""), PARAMETER_TYPE_Double, 0.0);
	pParameters->Add_Value	(pNode_2, "DS_RZ"		, _TL("Rotation Z")					, _TL(""), PARAMETER_TYPE_Double, 0.0);
	pParameters->Add_Value	(pNode_2, "DS_SC"		, _TL("Scaling")					, _TL(""), PARAMETER_TYPE_Double, 0.0);

	pParameters->Add_FilePath(
		pNode_2, "DATUM_GRID"	, _TL("Datum Shift Grid File"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s"),
			_TL("NTv2 Grid Shift Binary (*.gsb)")	, SG_T("*.gsb"),
			_TL("All Files")						, SG_T("*.*")
		), NULL, false, false, false
	);


	//-----------------------------------------------------
	// General Settings...

	pNode_1	= pParameters->Add_Node(pNode_0, "NODE_GENERAL"	, _TL("General Settings"), _TL(""));

	pParameters->Add_Value	(pNode_1, "LON_0"		, _TL("Central Meridian")		, _TL(""), PARAMETER_TYPE_Double, 0.0);
	pParameters->Add_Value	(pNode_1, "LAT_0"		, _TL("Central Parallel")		, _TL(""), PARAMETER_TYPE_Double, 0.0);
	pParameters->Add_Value	(pNode_1, "X_0"			, _TL("False Easting")			, _TL(""), PARAMETER_TYPE_Double, 0.0);
	pParameters->Add_Value	(pNode_1, "Y_0"			, _TL("False Northing")			, _TL(""), PARAMETER_TYPE_Double, 0.0);
	pParameters->Add_Value	(pNode_1, "K_0"			, _TL("Scale Factor")			, _TL(""), PARAMETER_TYPE_Double, 1.0, 0.0, true);

	pParameters->Add_Choice	(pNode_1, "UNIT"		, _TL("Unit")					, _TL(""), sUnits, 1);

	pParameters->Add_Value	(pNode_1, "NO_DEFS"		, _TL("Ignore Defaults")		, _TL(""), PARAMETER_TYPE_Bool, false);

	pParameters->Add_Value	(pNode_1, "OVER"		, _TL("Allow longitudes outside -180 to 180 Range")	, _TL(""), PARAMETER_TYPE_Bool, false);

	//-----------------------------------------------------
	pParameters->Add_Parameters(
		pNode_0, "OPTIONS"	, _TL("Projection Settings"),
		_TL("")
	);

	pParameters->Get_Parameter("OPTIONS")->asParameters()->Assign(Get_Parameters(SG_STR_MBTOSG(pj_list[0].id)));

	//-----------------------------------------------------
	return( true );
}

//---------------------------------------------------------
#define PRM_ADD_BOL(key, name, val)	pParms->Add_Value (NULL, key, name, _TL(""), PARAMETER_TYPE_Bool  , val);
#define PRM_ADD_INT(key, name, val)	pParms->Add_Value (NULL, key, name, _TL(""), PARAMETER_TYPE_Int   , val);
#define PRM_ADD_FLT(key, name, val)	pParms->Add_Value (NULL, key, name, _TL(""), PARAMETER_TYPE_Double, val);
#define PRM_ADD_STR(key, name, val)	pParms->Add_String(NULL, key, name, _TL(""), val);
#define PRM_ADD_CHO(key, name, val)	pParms->Add_Choice(NULL, key, name, _TL(""), val);

//---------------------------------------------------------
bool CCRS_Base::Add_User_Projection(const CSG_String &sID, const CSG_String &sName, const CSG_String &sArgs)
{
	if( sArgs.Length() == 0 )
	{
		return( false );
	}

	CSG_Parameters	*pParms	= Add_Parameters(sID, sName, sArgs);

//	pParms->Add_Info_String(NULL, "DESC", _TL("Description"), _TL(""), sArgs.c_str(), true);

	//-----------------------------------------------------
	// Cylindrical Projections...

	if(	!sID.CmpNoCase(SG_T("cea"))			// Equal Area Cylindrical
	||	!sID.CmpNoCase(SG_T("eqc"))			// Equidistant Cylindrical (Plate Caree) 
	||	!sID.CmpNoCase(SG_T("merc")) )		// Mercator 
	{
		PRM_ADD_FLT("lat_ts"	, _TL("True Scale Latitude")	, 0.0);
	}

	if(	!sID.CmpNoCase(SG_T("utm")) )		// Universal Transverse Mercator (UTM)
	{
		PRM_ADD_INT("zone"		, _TL("Zone")					, 32);
		PRM_ADD_BOL("south"		, _TL("South")					, false);
	}

	if(	!sID.CmpNoCase(SG_T("omerc")) )		// Oblique Mercator 
	{
		PRM_ADD_FLT("lat_1"		, _TL("Latitude 1")				, 40.0);
		PRM_ADD_FLT("lon_1"		, _TL("Longitude 1"	)			,-20.0);
		PRM_ADD_FLT("lat_2"		, _TL("Latitude 2")				, 50.0);
		PRM_ADD_FLT("lon_2"		, _TL("Longitude 2"	)			, 20.0);
	}

	//-----------------------------------------------------
	// Pseudocylindrical Projections...

	if(	!sID.CmpNoCase(SG_T("gn_sinu")) )	// General Sinusoidal Series
	{
		PRM_ADD_FLT("m"			, SG_T("m")						, 0.5);
		PRM_ADD_FLT("n"			, SG_T("n")						, 1.0 + M_PI_045);
	}

	if(	!sID.CmpNoCase(SG_T("loxim")) )		// Loximuthal
	{
		PRM_ADD_FLT("lat_1"		, _TL("Latitude 1")				, 40.0);
	}

	if(	!sID.CmpNoCase(SG_T("urmfps")) )	// Urmaev Flat-Polar Sinusoidal
	{
		PRM_ADD_FLT("n"			, SG_T("n")						, 1.0);
	}

	if(	!sID.CmpNoCase(SG_T("urm5")) )		// Urmaev V
	{
		PRM_ADD_FLT("n"			, SG_T("n")						, 1.0);
		PRM_ADD_FLT("q"			, SG_T("q")						, 1.0);
		PRM_ADD_FLT("alphi"		, SG_T("alphi")					, 45.0);
	}

	if(	!sID.CmpNoCase(SG_T("wink1"))		// Winkel I
	||	!sID.CmpNoCase(SG_T("wag3")) )		// Wagner III
	{
		PRM_ADD_FLT("lat_ts"	, _TL("True Scale Latitude")	, 45.0);
	}

	if(	!sID.CmpNoCase(SG_T("wink2")) )		// Winkel II
	{
		PRM_ADD_FLT("lat_1"		, _TL("Latitude 1")				, 40.0);
	}


	//-----------------------------------------------------
	// Conic Projections...

	if(	!sID.CmpNoCase(SG_T("aea"))			// Albers Equal Area
	||	!sID.CmpNoCase(SG_T("eqdc"))		// Equidistant Conic
	||	!sID.CmpNoCase(SG_T("euler"))		// Euler 
	||	!sID.CmpNoCase(SG_T("imw_p"))		// International Map of the World Polyconic 
	||	!sID.CmpNoCase(SG_T("murd1"))		// Murdoch I 
	||	!sID.CmpNoCase(SG_T("murd2"))		// Murdoch II 
	||	!sID.CmpNoCase(SG_T("murd3"))		// Murdoch III 
	||	!sID.CmpNoCase(SG_T("pconic"))		// Perspective Conic 
	||	!sID.CmpNoCase(SG_T("tissot"))		// Tissot 
	||	!sID.CmpNoCase(SG_T("vitk1")) )		// Vitkovsky I 
	{
		PRM_ADD_FLT("lat_1"		, _TL("Latitude 1")				, 33.0);
		PRM_ADD_FLT("lat_2"		, _TL("Latitude 2")				, 45.0);
	}

	if(	!sID.CmpNoCase(SG_T("lcc")) )		// Lambert Conformal Conic 
	{
		PRM_ADD_FLT("lat_1"		, _TL("Latitude 1")				, 33.0);
		PRM_ADD_FLT("lat_2"		, _TL("Latitude 2")				, 45.0);
	}

	if( !sID.CmpNoCase(SG_T("leac")) )		// Lambert Equal Area Conic
	{
		PRM_ADD_FLT("lat_1"		, _TL("Latitude 1")				, 45.0);
		PRM_ADD_BOL("south"		, _TL("South")					, false);
	}

	if(	!sID.CmpNoCase(SG_T("rpoly")) )		// Rectangular Polyconic
	{
		PRM_ADD_FLT("lat_ts"	, _TL("True Scale Latitude")	, 45.0);
	}

	if(	!sID.CmpNoCase(SG_T("mpoly")) )		// Modified Polyconic
	{
		PRM_ADD_FLT("lat_1"		, _TL("Latitude 1")				, 33.0);
		PRM_ADD_FLT("lat_2"		, _TL("Latitude 2")				, 45.0);
		PRM_ADD_BOL("lotsa"		, _TL("Lotsa")					, true);
	}

	if(	!sID.CmpNoCase(SG_T("bonne")) )		// Bonne
	{
		PRM_ADD_FLT("lat_1"		, _TL("Latitude 1")				, 45.0);
	}


	//-----------------------------------------------------
	// Azimuthal Projections...

	if(	!sID.CmpNoCase(SG_T("stere")) )		// Stereographic
	{
		PRM_ADD_FLT("lat_ts"	, _TL("True Scale Latitude")	, 45.0);
	}

	if(	!sID.CmpNoCase(SG_T("ups")) )		// Universal Polar Stereographic
	{
		PRM_ADD_BOL("south"		, _TL("South")					, true);
	}

	if(	!sID.CmpNoCase(SG_T("airy")) )		// Airy
	{
		PRM_ADD_FLT("lat_b"		, _TL("Latitude B")				, 45.0);
		PRM_ADD_BOL("no_cut"	, _TL("No Cut")					, true);
	}

	if(	!sID.CmpNoCase(SG_T("nsper")) )		// Near-sided perspective
	{
		PRM_ADD_FLT("h"			, _TL("Height of view point")	, 1.0);
	}

	if(	!sID.CmpNoCase(SG_T("aeqd")) )		// Azimuthal Equidistant
	{
		PRM_ADD_BOL("guam"		, _TL("guam")					, true);
	}

	if(	!sID.CmpNoCase(SG_T("hammer")) )	// Hammer & Eckert-Greifendorff
	{
		PRM_ADD_FLT("W"			, _TL("W")						, 0.5);
		PRM_ADD_FLT("M"			, _TL("M")						, 1.0);
	}

	if(	!sID.CmpNoCase(SG_T("wintri")) )	// Winkel Tripel 
	{
		PRM_ADD_FLT("lat_1"		, _TL("Latitude 1")				, 40.0);
	}


	//-----------------------------------------------------
	// Miscellaneous Projections...

	if(	!sID.CmpNoCase(SG_T("ocea"))		// Oblique Cylindrical Equal Area
	||	!sID.CmpNoCase(SG_T("tpeqd")) )		// Two Point Equidistant 
	{
		PRM_ADD_FLT("lat_1"		, _TL("Latitude 1")				, 40.0);
		PRM_ADD_FLT("lon_1"		, _TL("Longitude 1")			,-20.0);
		PRM_ADD_FLT("lat_2"		, _TL("Latitude 2")				, 50.0);
		PRM_ADD_FLT("lon_2"		, _TL("Longitude 2"	)			, 20.0);
	}

	if(	!sID.CmpNoCase(SG_T("geos")) )		// Geostationary Satellite View
	{
		PRM_ADD_FLT("h"			, _TL("Satellite Height [m]")	, 35785831.0);
		PRM_ADD_CHO("sweep"		, _TL("Sweep Angle")			, "x|y|");
	}

	if(	!sID.CmpNoCase(SG_T("lsat")) )		// Space oblique for LANDSAT
	{
		PRM_ADD_INT("lsat"		, _TL("Landsat Satellite (1-5)"), 1.0);
		PRM_ADD_INT("path"		, _TL("Path (1-255/233 1-3/4-5"), 1.0);
	}

	if(	!sID.CmpNoCase(SG_T("labrd")) )		// Laborde
	{
		PRM_ADD_FLT("azi"		, _TL("Azimuth"	)				, 19.0);
	}

	if(	!sID.CmpNoCase(SG_T("lagrng")) )	// Lagrange
	{
		PRM_ADD_FLT("lat_1"		, _TL("Latitude 1")				, 0.0);
		PRM_ADD_FLT("W"			, _TL("W")						, 2.0);
	}

	if(	!sID.CmpNoCase(SG_T("chamb")) )		// Chamberlin Trimetric
	{
		PRM_ADD_FLT("lat_1"		, _TL("Latitude 1")				, 30.0);
		PRM_ADD_FLT("lon_1"		, _TL("Longitude 1"	)			,-20.0);
		PRM_ADD_FLT("lat_2"		, _TL("Latitude 2")				, 40.0);
		PRM_ADD_FLT("lon_2"		, _TL("Longitude 2")			, 00.0);
		PRM_ADD_FLT("lat_3"		, _TL("Latitude 3")				, 50.0);
		PRM_ADD_FLT("lon_3"		, _TL("Longitude 3"	)			, 20.0);
	}

	if(	!sID.CmpNoCase(SG_T("oea")) )		// Oblated Equal Area
	{
		PRM_ADD_FLT("m"			, _TL("m")						, 1.0);
		PRM_ADD_FLT("n"			, _TL("n")						, 1.0);
		PRM_ADD_FLT("theta"		, _TL("theta")					, 45.0);
	}

	if(	!sID.CmpNoCase(SG_T("tpers")) )		// Tilted perspective
	{
		PRM_ADD_FLT("tilt"		, _TL("Tilt")					, 45.0);
		PRM_ADD_FLT("azi"		, _TL("Azimuth")				, 45.0);
		PRM_ADD_FLT("h"			, _TL("h")						, 1000.0);
	}

	if(	!sID.CmpNoCase(SG_T("ob_tran")) )	// General Oblique Transformation
	{
		PRM_ADD_FLT("o_lat_p"	, _TL("Latitude Pole")			, 40.0);
		PRM_ADD_FLT("o_lon_p"	, _TL("Longitude Pole")			, 40.0);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define PROJ4_ADD_BOL(key, val)		if( val ) Proj4 += CSG_String::Format(SG_T("+%s "), CSG_String(key).c_str());
#define PROJ4_ADD_INT(key, val)		Proj4 += CSG_String::Format(SG_T("+%s=%d "), CSG_String(key).c_str(), val);
#define PROJ4_ADD_FLT(key, val)		Proj4 += CSG_String::Format(SG_T("+%s=%s "), CSG_String(key).c_str(), SG_Get_String(val, -32).c_str());
#define PROJ4_ADD_STR(key, val)		Proj4 += CSG_String::Format(SG_T("+%s=%s "), CSG_String(key).c_str(), CSG_String(val).c_str());

//---------------------------------------------------------
CSG_String CCRS_Base::Get_User_Definition(CSG_Parameters &P)
{
	CSG_String	Proj4;

	//-----------------------------------------------------
	PROJ4_ADD_STR("proj"	, pj_list[P("PROJ_TYPE")->asInt()].id);

	if( P("LON_0")->asDouble() )	PROJ4_ADD_FLT("lon_0", P("LON_0")->asDouble());
	if( P("LAT_0")->asDouble() )	PROJ4_ADD_FLT("lat_0", P("LAT_0")->asDouble());

	if( P("X_0"  )->asDouble() )	PROJ4_ADD_FLT("x_0"  , P("X_0"  )->asDouble());
	if( P("Y_0"  )->asDouble() )	PROJ4_ADD_FLT("y_0"  , P("Y_0"  )->asDouble());

	if( P("K_0")->asDouble() != 1.0 && P("K_0")->asDouble() > 0.0 )
	{
		PROJ4_ADD_FLT("k_0", P("K_0")->asDouble());
	}

	PROJ4_ADD_STR("units", pj_units[P("UNIT")->asInt()].id);

	//-----------------------------------------------------
	switch( P("DATUM_DEF")->asInt() )
	{
	case 0:	// predefined datum

		PROJ4_ADD_STR("datum", pj_datums[P("DATUM")->asInt()].id);

		break;

	//-----------------------------------------------------
	case 1:	// user defined datum

		switch( P("ELLIPSOID")->asInt() )
		{
		case 0:	// Predefined Ellipsoid
			PROJ4_ADD_STR("ellps", pj_ellps[P("ELLPS_DEF")->asInt()].id);
			break;

		case 1:	// Semiminor axis
			PROJ4_ADD_FLT("a"    , P("ELLPS_A" )->asDouble());
			PROJ4_ADD_FLT("b"    , P("ELLPS_B" )->asDouble());
			break;

		case 2:	// Flattening
			PROJ4_ADD_FLT("a"    , P("ELLPS_A" )->asDouble());
			PROJ4_ADD_FLT("f"    , P("ELLPS_F" )->asDouble());
			break;

		case 3:	// Reciprocal Flattening
			PROJ4_ADD_FLT("a"    , P("ELLPS_A" )->asDouble());
			PROJ4_ADD_FLT("rf"   , P("ELLPS_RF")->asDouble());
			break;

		case 4:	// Eccentricity
			PROJ4_ADD_FLT("a"    , P("ELLPS_A" )->asDouble());
			PROJ4_ADD_FLT("e"    , P("ELLPS_E" )->asDouble());
			break;

		case 5:	// Eccentricity Squared
			PROJ4_ADD_FLT("a"    , P("ELLPS_A" )->asDouble());
			PROJ4_ADD_FLT("es"   , P("ELLPS_ES")->asDouble());
			break;
		}

		switch( P("DATUM_SHIFT")->asInt() )
		{
		case 1:	// 3 parameters
			PROJ4_ADD_STR("towgs84", CSG_String::Format(SG_T("%s,%s,%s"),
				SG_Get_String(P("DS_DX")->asDouble(), -32).c_str(),
				SG_Get_String(P("DS_DY")->asDouble(), -32).c_str(),
				SG_Get_String(P("DS_DZ")->asDouble(), -32).c_str())
			);
			break;

		case 2:	// 7 parameters
			PROJ4_ADD_STR("towgs84", CSG_String::Format(SG_T("%s,%s,%s,%s,%s,%s,%s"),
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
	for(int i=0; i<P("OPTIONS")->asParameters()->Get_Count(); i++)
	{
		CSG_Parameter	*p	= P("OPTIONS")->asParameters()->Get_Parameter(i);

		switch( p->Get_Type() )
		{
		case PARAMETER_TYPE_Choice:
		case PARAMETER_TYPE_String: PROJ4_ADD_STR(p->Get_Identifier(), p->asString());	break;
		case PARAMETER_TYPE_Bool:   PROJ4_ADD_BOL(p->Get_Identifier(), p->asBool  ());	break;
		case PARAMETER_TYPE_Int:    PROJ4_ADD_INT(p->Get_Identifier(), p->asInt   ());	break;
		case PARAMETER_TYPE_Double: PROJ4_ADD_FLT(p->Get_Identifier(), p->asDouble());	break;
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
	P("OPTIONS")->asParameters()->Assign(Get_Parameters(SG_STR_MBTOSG(pj_list[P("PROJ_TYPE")->asInt()].id)));

	for(int i=0; i<P("OPTIONS")->asParameters()->Get_Count(); i++)
	{
		CSG_Parameter	*p	= P("OPTIONS")->asParameters()->Get_Parameter(i);

		switch( p->Get_Type() )
		{
		case PARAMETER_TYPE_Choice:
		case PARAMETER_TYPE_String: p->Set_Value(PROJ4_GET_VAL(p->Get_Identifier())           );	break;
		case PARAMETER_TYPE_Bool:   p->Set_Value(PROJ4_HAS_KEY(p->Get_Identifier()) ? 1 : 0   );	break;
		case PARAMETER_TYPE_Int:    p->Set_Value(PROJ4_GET_VAL(p->Get_Identifier()).asInt   ());	break;
		case PARAMETER_TYPE_Double: p->Set_Value(PROJ4_GET_VAL(p->Get_Identifier()).asDouble());	break;
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
CCRS_Picker::CCRS_Picker(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Coordinate Reference System Picker"));

	Set_Author		(SG_T("O.Conrad (c) 2014"));

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

	Message_Add(CSG_String::Format(SG_T("\n%s: %s"), _TL("target"), Target.Get_Proj4().c_str()), false);

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

	Message_Add(CSG_String::Format(SG_T("\n%s: %s"), _TL("target"), Target.Get_Proj4().c_str()), false);

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
