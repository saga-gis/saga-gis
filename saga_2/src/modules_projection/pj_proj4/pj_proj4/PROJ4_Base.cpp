
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
//                    PROJ4_Base.cpp                     //
//                                                       //
//                 Copyright (C) 2003 by                 //
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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
// USA.                                                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
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
#include "PROJ4_Base.h"

#include <string.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPROJ4_Base::CPROJ4_Base(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	pNode	= Parameters.Add_Node(
		NULL	, "SOURCE_NODE"	, _TL("Source Parameters"),
		_TL("")
	);

	pNode	= Parameters.Add_Parameters(
		pNode	, "SOURCE_PROJ"	, _TL("Source Projection Parameters"),
		_TL("")
	);

	_Init_Projection(*pNode->asParameters());

	//-----------------------------------------------------
	pNode	= Parameters.Add_Node(
		NULL	, "TARGET_NODE"	, _TL("Target Parameters"),
		_TL("")
	);

	pNode	= Parameters.Add_Parameters(
		pNode	, "TARGET_PROJ"	, _TL("Target Projection Parameters"),
		_TL("")
	);

	_Init_Projection(*pNode->asParameters());

	//-----------------------------------------------------
	pNode	= Parameters.Add_Node(
		NULL	, "PROJECTION"	, _TL("Projection"),
		_TL("")
	);

	Parameters.Add_Value(
		pNode	, "INVERSE"		, _TL("Inverse"),
		_TL(""),
		PARAMETER_TYPE_Bool		, false
	);
}

//---------------------------------------------------------
CPROJ4_Base::~CPROJ4_Base(void)
{}


///////////////////////////////////////////////////////////
//														 //
//					Initialization						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define BESSEL_A	6377397.16
#define BESSEL_B	6356078.76

//---------------------------------------------------------
bool CPROJ4_Base::_Init_Projection(CSG_Parameters &P)
{
	//-----------------------------------------------------
	char				c;
	int					i;
	struct PJ_LIST		*pProjections;
	struct PJ_ELLPS		*pEllipses;
	struct PJ_DATUMS	*pDatums;
	struct PJ_UNITS		*pUnits;
	CSG_String			sList, sName, sDesc;
	CSG_Parameter		*pNode_0, *pNode_1, *pNode_2, *pNode_3;

	//-----------------------------------------------------
	pNode_0	= NULL;

	//-------------------------------------------------
	// Projections List...

	sDesc	= _TL("Available Projections:");

	for(pProjections=pj_list; pProjections->id; ++pProjections)
	{
		sName.Clear();

		for(i=0; i<256; i++)
		{
			c	= (*pProjections->descr)[i];

			if( c != '\0' && c != '\n' )
			{
				sName	+= c;
			}
			else
			{
				break;
			}
		}

		sList	+= CSG_String::Format(SG_T("%s|"), sName.c_str());
		sDesc	+= CSG_String::Format(SG_T("\n[%s] %s"), pProjections->id, *pProjections->descr);

		_Init_Projection(pProjections->id, *pProjections->descr, sName);
	}

	if( sList.Length() > 0 )
	{
		pNode_1	= P.Add_Choice(
			pNode_0, "PROJ_TYPE"	, _TL("Projection Type"),
			sDesc,
			sList
		);
	}


	//-------------------------------------------------
	// Ellipsoid...

	pNode_1	= P.Add_Choice(
		pNode_0, "ELLIPSOID"	, _TL("Ellipsoid"),
		_TL(""),

		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|%s|%s|"),
			_TL("Predefined Standard Ellipsoids"),
			_TL("Semimajor Axis and Semiminor Axis"),
			_TL("Semimajor Axis and Flattening"),
			_TL("Semimajor Axis and Reciprocal Flattening"),
			_TL("Semimajor Axis and Eccentricity"),
			_TL("Semimajor Axis and Eccentricity Squared"),
			_TL("Predefined Datum")
		)
	);

	sList.Clear();

	for(pEllipses=pj_ellps; pEllipses->id; ++pEllipses)
	{
		sList	+= CSG_String::Format(SG_T("%s (%s, %s)|"), pEllipses->name, pEllipses->major, pEllipses->ell);
	}

	if( sList.Length() > 0 )
	{
		pNode_2	= P.Add_Choice(
			pNode_1, "ELLPS_PREDEF"	, _TL("Predefined Standard Ellipsoids"),
			_TL(""),
			sList
		);
	}

	pNode_2	= P.Add_Value(
		pNode_1, "ELLPS_A"		, _TL("Semimajor Axis (a)"),
		_TL("Semimajor axis or equatorial radius."),
		PARAMETER_TYPE_Double	, BESSEL_A
	);

	pNode_3	= P.Add_Value(
		pNode_2, "ELLPS_B"		, _TL("Semiminor Axis (b)"),
		_TL("Semiminor axis or polar radius."),
		PARAMETER_TYPE_Double	, BESSEL_B
	);

	pNode_3	= P.Add_Value(
		pNode_2, "ELLPS_F"		, _TL("Flattening (f)"),
		_TL(""),
		PARAMETER_TYPE_Double	, (BESSEL_A - BESSEL_B) / BESSEL_A
	);

	pNode_3	= P.Add_Value(
		pNode_2, "ELLPS_RF"		, _TL("Reciprocal Flattening (rf)"),
		_TL(""),
		PARAMETER_TYPE_Double	, BESSEL_A / (BESSEL_A - BESSEL_B)
	);

	pNode_3	= P.Add_Value(
		pNode_2, "ELLPS_E"		, _TL("Eccentricity (e)"),
		_TL(""),
		PARAMETER_TYPE_Double	, sqrt(BESSEL_A*BESSEL_A + BESSEL_B*BESSEL_B)
	);

	pNode_3	= P.Add_Value(
		pNode_2, "ELLPS_ES"		, _TL("Squared Eccentricity (es)"),
		_TL(""),
		PARAMETER_TYPE_Double	, BESSEL_A*BESSEL_A + BESSEL_B*BESSEL_B
	);


	//-------------------------------------------------
	// Datums...

	sList.Clear();

	for(pDatums=pj_datums; pDatums->id; ++pDatums)
	{
		sList	+= CSG_String::Format(SG_T("%s [%s] "), pDatums->id, pDatums->ellipse_id);

		if( pDatums->comments != NULL && *pDatums->comments != NULL )
		{
			sList	+= CSG_String::Format(SG_T("%s"), pDatums->comments);
		}

		sList	+= '|';
	}

	if( sList.Length() > 0 )
	{
		pNode_2	= P.Add_Choice(
			pNode_1, "DATUM"		, _TL("Datum"),
			_TL(""),
			sList
		);
	}/**/


	//-------------------------------------------------
	// Units...

	sList.Clear();

	for(pUnits=pj_units; pUnits->id; ++pUnits)
	{
		sList	+= CSG_String::Format(SG_T("%s (%s)|"), pUnits->name, pUnits->to_meter);
	}

	if( sList.Length() > 0 )
	{
		pNode_1	= P.Add_Choice(
			pNode_0, "UNIT"			, _TL("Unit"),
			_TL(""),
			sList, 1
		);
	}


	//-------------------------------------------------
	// Standard Settings...

	pNode_1	= P.Add_Value(
		pNode_0, "LON_0"			, _TL("Central Meridian"),
		_TL(""),
		PARAMETER_TYPE_Double, 0.0
	);

	pNode_1	= P.Add_Value(
		pNode_0, "LAT_0"			, _TL("Central Parallel"),
		_TL(""),
		PARAMETER_TYPE_Double, 0.0
	);

	pNode_1	= P.Add_Value(
		pNode_0, "X_0"				, _TL("False Easting"),
		_TL(""),
		PARAMETER_TYPE_Double, 0.0
	);

	pNode_1	= P.Add_Value(
		pNode_0, "Y_0"				, _TL("False Northing"),
		_TL(""),
		PARAMETER_TYPE_Double, 0.0
	);

	return( true );
}

//---------------------------------------------------------
#define ADD_1_BOL(key, name, val)	pParms->Add_Value(NULL, key, name, _TL(""), PARAMETER_TYPE_Bool  , val);
#define ADD_1_INT(key, name, val)	pParms->Add_Value(NULL, key, name, _TL(""), PARAMETER_TYPE_Int   , val);
#define ADD_1_FLT(key, name, val)	pParms->Add_Value(NULL, key, name, _TL(""), PARAMETER_TYPE_Double, val);

//---------------------------------------------------------
bool CPROJ4_Base::_Init_Projection(const CSG_String &sID, const CSG_String &sDesc, const CSG_String &sName)
{
	CSG_Parameters	*pParms;

	pParms	= Add_Parameters(sID, sName, sDesc);


	//-----------------------------------------------------
	// Cylindrical Projections...

	if(	!sID.CmpNoCase(SG_T("cea"))			// Equal Area Cylindrical
	||	!sID.CmpNoCase(SG_T("eqc"))			// Equidistant Cylindrical (Plate Caree) 
	||	!sID.CmpNoCase(SG_T("merc")) )		// Mercator 
	{
		ADD_1_FLT(SG_T("lat_ts")	, _TL("True Scale Latitude")	, 0.0);
	}

	if(	!sID.CmpNoCase(SG_T("utm")) )		// Universal Transverse Mercator (UTM)
	{
		ADD_1_INT(SG_T("zone")	, _TL("Zone")				, 32);
		ADD_1_BOL(SG_T("south")	, _TL("South")				, false);
	}

	if(	!sID.CmpNoCase(SG_T("omerc")) )		// Oblique Mercator 
	{
		ADD_1_FLT(SG_T("lat_1")	, _TL("Latitude 1")			, 40.0);
		ADD_1_FLT(SG_T("lon_1")	, _TL("Longitude 1"	)		,-20.0);
		ADD_1_FLT(SG_T("lat_2")	, _TL("Latitude 2")			, 50.0);
		ADD_1_FLT(SG_T("lon_2")	, _TL("Longitude 2"	)		, 20.0);
	}

	//-----------------------------------------------------
	// Pseudocylindrical Projections...

	if(	!sID.CmpNoCase(SG_T("gn_sinu")) )	// General Sinusoidal Series
	{
		ADD_1_FLT(SG_T("m")		, SG_T("m")					, 0.5);
		ADD_1_FLT(SG_T("n")		, SG_T("n")					, 1.0 + M_PI_045);
	}

	if(	!sID.CmpNoCase(SG_T("loxim")) )		// Loximuthal
	{
		ADD_1_FLT(SG_T("lat_1")	, _TL("Latitude 1")			, 40.0);
	}

	if(	!sID.CmpNoCase(SG_T("urmfps")) )	// Urmaev Flat-Polar Sinusoidal
	{
		ADD_1_FLT(SG_T("n")		, SG_T("n")					, 1.0);
	}

	if(	!sID.CmpNoCase(SG_T("urm5")) )		// Urmaev V
	{
		ADD_1_FLT(SG_T("n")		, SG_T("n")					, 1.0);
		ADD_1_FLT(SG_T("q")		, SG_T("q")					, 1.0);
		ADD_1_FLT(SG_T("alphi")	, SG_T("alphi")				, 45.0);
	}

	if(	!sID.CmpNoCase(SG_T("wink1"))		// Winkel I
	||	!sID.CmpNoCase(SG_T("wag3")) )		// Wagner III
	{
		ADD_1_FLT(SG_T("lat_ts"), _TL("True Scale Latitude"), 45.0);
	}

	if(	!sID.CmpNoCase(SG_T("wink2")) )		// Winkel II
	{
		ADD_1_FLT(SG_T("lat_1")	, _TL("Latitude 1")			, 40.0);
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
		ADD_1_FLT(SG_T("lat_1")	, _TL("Latitude 1")			, 33.0);
		ADD_1_FLT(SG_T("lat_2")	, _TL("Latitude 2")			, 45.0);
	}

	if(	!sID.CmpNoCase(SG_T("lcc")) )		// Lambert Conformal Conic 
	{
		ADD_1_FLT(SG_T("lat_1")	, _TL("Latitude 1")			, 33.0);
		ADD_1_FLT(SG_T("lat_2")	, _TL("Latitude 2")			, 45.0);
	}

	if( !sID.CmpNoCase(SG_T("leac")) )		// Lambert Equal Area Conic
	{
		ADD_1_FLT(SG_T("lat_1")	, _TL("Latitude 1")			, 45.0);
		ADD_1_BOL(SG_T("south")	, _TL("South")				, false);
	}

	if(	!sID.CmpNoCase(SG_T("rpoly")) )		// Rectangular Polyconic
	{
		ADD_1_FLT(SG_T("lat_ts"), _TL("True Scale Latitude"), 45.0);
	}

	if(	!sID.CmpNoCase(SG_T("mpoly")) )		// Modified Polyconic
	{
		ADD_1_FLT(SG_T("lat_1")	, _TL("Latitude 1")			, 33.0);
		ADD_1_FLT(SG_T("lat_2")	, _TL("Latitude 2")			, 45.0);
		ADD_1_BOL(SG_T("lotsa")	, _TL("Lotsa")				, true);
	}

	if(	!sID.CmpNoCase(SG_T("bonne")) )		// Bonne
	{
		ADD_1_FLT(SG_T("lat_1")	, _TL("Latitude 1")			, 45.0);
	}


	//-----------------------------------------------------
	// Azimuthal Projections...

	if(	!sID.CmpNoCase(SG_T("stere")) )		// Stereographic
	{
		ADD_1_FLT(SG_T("lat_ts"), _TL("True Scale Latitude"), 45.0);
	}

	if(	!sID.CmpNoCase(SG_T("ups")) )		// Universal Polar Stereographic
	{
		ADD_1_BOL(SG_T("south")	, _TL("South")				, true);
	}

	if(	!sID.CmpNoCase(SG_T("airy")) )		// Airy
	{
		ADD_1_FLT(SG_T("lat_b")	, _TL("Latitude B")			, 45.0);
		ADD_1_BOL(SG_T("no_cut"), _TL("No Cut"	)			, true);
	}

	if(	!sID.CmpNoCase(SG_T("nsper")) )		// Near-sided perspective
	{
		ADD_1_FLT(SG_T("h")		, _TL("Height of view point"), 1.0);
	}

	if(	!sID.CmpNoCase(SG_T("aeqd")) )		// Azimuthal Equidistant
	{
		ADD_1_BOL(SG_T("guam")	, _TL("guam")				, true);
	}

	if(	!sID.CmpNoCase(SG_T("hammer")) )	// Hammer & Eckert-Greifendorff
	{
		ADD_1_FLT(SG_T("W")		, _TL("W")					, 0.5);
		ADD_1_FLT(SG_T("M")		, _TL("M")					, 1.0);
	}

	if(	!sID.CmpNoCase(SG_T("wintri")) )	// Winkel Tripel 
	{
		ADD_1_FLT(SG_T("lat_1")	, _TL("Latitude 1")			, 40.0);
	}


	//-----------------------------------------------------
	// Miscellaneous Projections...

	if(	!sID.CmpNoCase(SG_T("ocea"))		// Oblique Cylindrical Equal Area
	||	!sID.CmpNoCase(SG_T("tpeqd")) )		// Two Point Equidistant 
	{
		ADD_1_FLT(SG_T("lat_1")	, _TL("Latitude 1")			, 40.0);
		ADD_1_FLT(SG_T("lon_1")	, _TL("Longitude 1")		,-20.0);
		ADD_1_FLT(SG_T("lat_2")	, _TL("Latitude 2")			, 50.0);
		ADD_1_FLT(SG_T("lon_2")	, _TL("Longitude 2"	)		, 20.0);
	}

	if(	!sID.CmpNoCase(SG_T("lsat")) )		// Space oblique for LANDSAT
	{
		ADD_1_INT(SG_T("lsat")	, _TL("Landsat")			, 1.0);
		ADD_1_INT(SG_T("path")	, _TL("Path")				, 1.0);
	}

	if(	!sID.CmpNoCase(SG_T("labrd")) )		// Laborde
	{
		ADD_1_FLT(SG_T("azi")		, _TL("Azimuth"	)		, 19.0);
		ADD_1_FLT(SG_T("k_0")		, _TL("k_0")			, 0.9995);
	}

	if(	!sID.CmpNoCase(SG_T("lagrng")) )	// Lagrange
	{
		ADD_1_FLT(SG_T("lat_1")	, _TL("Latitude 1")			, 0.0);
		ADD_1_FLT(SG_T("W")		, _TL("W")					, 2.0);
	}

	if(	!sID.CmpNoCase(SG_T("chamb")) )		// Chamberlin Trimetric
	{
		ADD_1_FLT(SG_T("lat_1")	, _TL("Latitude 1")			, 30.0);
		ADD_1_FLT(SG_T("lon_1")	, _TL("Longitude 1"	)		,-20.0);
		ADD_1_FLT(SG_T("lat_2")	, _TL("Latitude 2")			, 40.0);
		ADD_1_FLT(SG_T("lon_2")	, _TL("Longitude 2")		, 00.0);
		ADD_1_FLT(SG_T("lat_3")	, _TL("Latitude 3")			, 50.0);
		ADD_1_FLT(SG_T("lon_3")	, _TL("Longitude 3"	)		, 20.0);
	}

	if(	!sID.CmpNoCase(SG_T("oea")) )		// Oblated Equal Area
	{
		ADD_1_FLT(SG_T("m")		, _TL("m")					, 1.0);
		ADD_1_FLT(SG_T("n")		, _TL("n")					, 1.0);
		ADD_1_FLT(SG_T("theta")	, _TL("theta")				, 45.0);
	}

	if(	!sID.CmpNoCase(SG_T("tpers")) )		// Tilted perspective
	{
		ADD_1_FLT(SG_T("tilt")	, _TL("Tilt")				, 45.0);
		ADD_1_FLT(SG_T("azi")	, _TL("Azimuth")			, 45.0);
		ADD_1_FLT(SG_T("h")		, _TL("h")					, 1000.0);
	}

	if(	!sID.CmpNoCase(SG_T("ob_tran")) )	// General Oblique Transformation
	{
		ADD_1_FLT(SG_T("o_lat_p")	, _TL("Latitude Pole")	, 40.0);
		ADD_1_FLT(SG_T("o_lon_p")	, _TL("Longitude Pole")	, 40.0);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPROJ4_Base::On_Execute(void)
{
	bool	bResult	= false;

	//-----------------------------------------------------
	m_pPrjSrc	= NULL;
	m_pPrjDst	= NULL;

	//-----------------------------------------------------
	if(	_Get_Projection(&m_pPrjSrc, *Parameters("SOURCE_PROJ")->asParameters())
	&&	_Get_Projection(&m_pPrjDst, *Parameters("TARGET_PROJ")->asParameters()) )
	{
		m_bInverse	= false;

		if( Parameters("INVERSE")->asBool() )
		{
			PJ	*tmp	= m_pPrjSrc;
			m_pPrjSrc	= m_pPrjDst;
			m_pPrjDst	= tmp;
		}

		//-------------------------------------------------
		if( m_pPrjSrc->inv == NULL )
		{
			Message_Add	(_TL("Inverse transformation not available for selected projection type."));
			Error_Set	(_TL("Inverse transformation not available for selected projection type."));

			return( false );
		}

		//-------------------------------------------------
		bResult		= On_Execute_Conversion();
	}

	//-----------------------------------------------------
	if( m_pPrjSrc )
	{
		pj_free(m_pPrjSrc);
	}

	if( m_pPrjDst )
	{
		pj_free(m_pPrjDst);
	}

	//-----------------------------------------------------
	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define ADD_2_BOL(key, val)	(val ? CSG_String::Format(SG_T("+%s "), key) : SG_T(""))
#define ADD_2_STR(key, val)	CSG_String::Format(SG_T("+%s=%s "), key, val)
#define ADD_2_INT(key, val)	CSG_String::Format(SG_T("+%s=%d "), key, val)
#define ADD_2_FLT(key, val)	CSG_String::Format(SG_T("+%s=%f "), key, val)

//---------------------------------------------------------
bool CPROJ4_Base::_Get_Projection(PJ **ppPrj, CSG_Parameters &P)
{
	CSG_String	sPrj;

	//-----------------------------------------------------
	sPrj	+= ADD_2_STR("proj"		, pj_list[P("PROJ_TYPE")->asInt()].id);

	//-----------------------------------------------------
	switch( P("ELLIPSOID")->asInt() )
	{
	case 0:	// Predefined
		sPrj	+= ADD_2_STR("ellps", pj_ellps[P("ELLPS_PREDEF")->asInt()].id);
		break;

	case 1:	// Semiminor axis
		sPrj	+= ADD_2_FLT("a"	, P("ELLPS_A" )->asDouble());
		sPrj	+= ADD_2_FLT("b"	, P("ELLPS_B" )->asDouble());
		break;

	case 2:	// Flattening
		sPrj	+= ADD_2_FLT("a"	, P("ELLPS_A" )->asDouble());
		sPrj	+= ADD_2_FLT("f"	, P("ELLPS_F" )->asDouble());
		break;

	case 3:	// Reciprocal Flattening
		sPrj	+= ADD_2_FLT("a"	, P("ELLPS_A" )->asDouble());
		sPrj	+= ADD_2_FLT("rf"	, P("ELLPS_RF")->asDouble());
		break;

	case 4:	// Eccentricity
		sPrj	+= ADD_2_FLT("a"	, P("ELLPS_A" )->asDouble());
		sPrj	+= ADD_2_FLT("e"	, P("ELLPS_E" )->asDouble());
		break;

	case 5:	// Eccentricity Squared
		sPrj	+= ADD_2_FLT("a"	, P("ELLPS_A" )->asDouble());
		sPrj	+= ADD_2_FLT("es"	, P("ELLPS_ES")->asDouble());
		break;
	}

	//-----------------------------------------------------
	sPrj	+= ADD_2_STR("datum"	, pj_datums[P("DATUM")->asInt()].id);

	//-----------------------------------------------------
	sPrj	+= ADD_2_STR("units"	, pj_units[P("UNIT")->asInt()].id);

	sPrj	+= ADD_2_FLT("lon_0"	, P("LON_0")->asDouble());
	sPrj	+= ADD_2_FLT("lat_0"	, P("LAT_0")->asDouble());

	sPrj	+= ADD_2_FLT("x_0"		, P("X_0"  )->asDouble());
	sPrj	+= ADD_2_FLT("y_0"		, P("Y_0"  )->asDouble());

	//-----------------------------------------------------
	if( !_Get_Projection(sPrj, pj_list[P("PROJ_TYPE")->asInt()].id) )
	{
		return( false );
	}

	//-----------------------------------------------------
	if( ((*ppPrj) = pj_init_plus(sPrj.b_str())) == NULL )
	{
		Message_Add	(_TL("Projection initialization failure\ncause: "));
		Message_Add	(CSG_String(pj_strerrno(pj_errno)));
		Error_Set	(CSG_String(pj_strerrno(pj_errno)));

		return( false );
	}

	//-----------------------------------------------------
	return( true );
}

//---------------------------------------------------------
bool CPROJ4_Base::_Get_Projection(CSG_String &sPrj, const CSG_String &sPrjID)
{
	CSG_Parameters	*pParms	= Get_Parameters(sPrjID);

	if( pParms == NULL || pParms->Get_Count() <= 0 )
	{
		return( true );
	}

	if( Dlg_Parameters(sPrjID) )
	{
		for(int i=0; i<pParms->Get_Count(); i++)
		{
			CSG_Parameter	*p	= pParms->Get_Parameter(i);

			switch( p->Get_Type() )
			{
			case PARAMETER_TYPE_Bool:	sPrj	+= ADD_2_BOL(p->Get_Identifier(), p->asBool());	break;
			case PARAMETER_TYPE_Int:	sPrj	+= ADD_2_INT(p->Get_Identifier(), p->asInt());	break;
			case PARAMETER_TYPE_Double:	sPrj	+= ADD_2_FLT(p->Get_Identifier(), p->asDouble());	break;
			}
		}

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
bool CPROJ4_Base::Set_Inverse(bool bOn)
{
	if( m_bInverse == bOn )
	{
		return( true );
	}

	if( m_pPrjDst && m_pPrjDst->inv )
	{
		m_bInverse	= bOn;

		PJ	*tmp	= m_pPrjSrc;
		m_pPrjSrc	= m_pPrjDst;
		m_pPrjDst	= tmp;

		return( true );
	}

	Message_Add	(_TL("Inverse transformation not available for selected projection type."));
	Error_Set	(_TL("Inverse transformation not available for selected projection type."));

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPROJ4_Base::Get_Converted(TSG_Point &Point)
{
	return( Get_Converted(Point.x, Point.y) );
}

bool CPROJ4_Base::Get_Converted(double &x, double &y)
{
	if( m_pPrjSrc && m_pPrjDst )
	{
		double	z	= 0.0;

		if( pj_is_latlong(m_pPrjSrc) )
		{
			x	*= DEG_TO_RAD;
			y	*= DEG_TO_RAD;
		}

		if( pj_transform(m_pPrjSrc, m_pPrjDst, 1, 0, &x, &y, &z) == 0 )
		{
			if( pj_is_latlong(m_pPrjDst) )
			{
				x	*= RAD_TO_DEG;
				y	*= RAD_TO_DEG;
			}

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
