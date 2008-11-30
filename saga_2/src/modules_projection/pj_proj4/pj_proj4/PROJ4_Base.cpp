
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define PRM_ADD_BOL(key, name, val)	pParms->Add_Value(NULL, key, name, _TL(""), PARAMETER_TYPE_Bool  , val);
#define PRM_ADD_INT(key, name, val)	pParms->Add_Value(NULL, key, name, _TL(""), PARAMETER_TYPE_Int   , val);
#define PRM_ADD_FLT(key, name, val)	pParms->Add_Value(NULL, key, name, _TL(""), PARAMETER_TYPE_Double, val);

//---------------------------------------------------------
#define STR_ADD_BOL(key, val)		(val ? CSG_String::Format(SG_T("+%s "), key) : SG_T(""))
#define STR_ADD_INT(key, val)		CSG_String::Format(SG_T("+%s=%d "), key, val)
#define STR_ADD_FLT(key, val)		CSG_String::Format(SG_T("+%s=%f "), key, val)
#define STR_ADD_STR(key, val)		CSG_String::Format(SG_T("+%s=%s "), key, val)

//---------------------------------------------------------
#define BESSEL_A					6377397.16
#define BESSEL_B					6356078.76


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPROJ4_Base::CPROJ4_Base(int Interface, bool bInputList)
{
	CSG_Parameter	*pNode;

	m_Interface		= Interface;
	m_bInputList	= bInputList;

	//-----------------------------------------------------
	pNode	= Parameters.Add_Node(
		NULL	, "SOURCE_NODE"	, _TL("Source Parameters"),
		_TL("")
	);

	pNode	= Parameters.Add_Node(
		NULL	, "TARGET_NODE"	, _TL("Target Parameters"),
		_TL("")
	);

	pNode	= Parameters.Add_Node(
		NULL	, "GENERAL_NODE", _TL("General Settings"),
		_TL("")
	);

	Parameters.Add_Value(
		pNode	, "INVERSE"		, _TL("Inverse"),
		_TL(""),
		PARAMETER_TYPE_Bool		, false
	);

	//-----------------------------------------------------
	switch( m_Interface )
	{
	//-----------------------------------------------------
	case PROJ4_INTERFACE_SIMPLE:	default:

		Parameters.Add_String(
			Parameters("SOURCE_NODE")	, "SOURCE_PROJ"	, _TL("Source Projection Parameters"),
			_TL(""),
			SG_T("+proj=tmerc +datum=potsdam +lon_0=9 +x_0=3500000")
		);

		Parameters.Add_String(
			Parameters("TARGET_NODE")	, "TARGET_PROJ"	, _TL("Target Projection Parameters"),
			_TL(""),
			SG_T("+proj=tmerc +datum=potsdam +lon_0=12 +x_0=4500000")
		);

		break;

	//-----------------------------------------------------
	case PROJ4_INTERFACE_DIALOG:

		pNode	= Parameters.Add_Parameters(
			Parameters("SOURCE_NODE")	, "SOURCE_PROJ"	, _TL("Source Projection Parameters"),
			_TL("")
		);

		_Init_Projection(*pNode->asParameters());

		pNode	= Parameters.Add_Parameters(
			Parameters("TARGET_NODE")	, "TARGET_PROJ"	, _TL("Target Projection Parameters"),
			_TL("")
		);

		_Init_Projection(*pNode->asParameters());

		break;
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPROJ4_Base::On_Execute(void)
{
	bool		bResult	= false;

	CSG_String	sSrc, sDst;

	//-----------------------------------------------------
	if( _Get_Projections(sSrc, sDst) )
	{
		if( (m_pPrjSrc = pj_init_plus(sSrc.b_str())) == NULL )
		{
			Error_Set(CSG_String::Format(SG_T("%s:\n%s"), _TL("projection initialization failure"), pj_strerrno(pj_errno)));
		}

		if( (m_pPrjDst = pj_init_plus(sDst.b_str())) == NULL )
		{
			Error_Set(CSG_String::Format(SG_T("%s:\n%s"), _TL("projection initialization failure"), pj_strerrno(pj_errno)));
		}

		Message_Add(CSG_String::Format(SG_T("\n%s: %s\n"), _TL("Source"), sSrc.c_str()), false);
		Message_Add(CSG_String::Format(SG_T("\n%s: %s\n"), _TL("Target"), sDst.c_str()), false);

		//-------------------------------------------------
		if(	m_pPrjSrc && m_pPrjDst )
		{
			if( Parameters("INVERSE")->asBool() )
			{
				PJ	*tmp	= m_pPrjSrc;
				m_pPrjSrc	= m_pPrjDst;
				m_pPrjDst	= tmp;
			}

			if( m_pPrjSrc->inv == NULL )
			{
				Error_Set(_TL("Inverse transformation not available for selected projection type."));
			}
			else
			{
				m_bInverse	= false;

				bResult		= On_Execute_Conversion();
			}
		}

		//-------------------------------------------------
		if( m_pPrjSrc )
		{
			pj_free(m_pPrjSrc);
		}

		if( m_pPrjDst )
		{
			pj_free(m_pPrjDst);
		}
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

	Error_Set(_TL("Inverse transformation not available for selected projection type."));

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String CPROJ4_Base::Get_Proj_Name(void)
{
	if( m_pPrjDst )
	{
		return( CSG_String(m_pPrjDst->descr).BeforeFirst('\n') );
	}

	return( _TL("") );
}

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
bool CPROJ4_Base::_Get_Projections(CSG_String &sPrjSrc, CSG_String &sPrjDst)
{
	switch( m_Interface )
	{
	//-----------------------------------------------------
	case PROJ4_INTERFACE_SIMPLE:	default:

		sPrjSrc	= Parameters("SOURCE_PROJ")->asString();
		sPrjDst	= Parameters("TARGET_PROJ")->asString();

		return( true );

	//-----------------------------------------------------
	case PROJ4_INTERFACE_DIALOG:

		return(
			_Get_Projection(sPrjSrc, *Parameters("SOURCE_PROJ")->asParameters()),
			_Get_Projection(sPrjDst, *Parameters("TARGET_PROJ")->asParameters())
		);
	}
}

//---------------------------------------------------------
bool CPROJ4_Base::_Get_Projection(CSG_String &sPrj, CSG_Parameters &P)
{
	//-----------------------------------------------------
	sPrj	.Clear();

	sPrj	+= STR_ADD_STR("proj"	, pj_list[P("PROJ_TYPE")->asInt()].id);

	sPrj	+= STR_ADD_FLT("lon_0"	, P("LON_0")->asDouble());
	sPrj	+= STR_ADD_FLT("lat_0"	, P("LAT_0")->asDouble());

	sPrj	+= STR_ADD_FLT("x_0"	, P("X_0"  )->asDouble());
	sPrj	+= STR_ADD_FLT("y_0"	, P("Y_0"  )->asDouble());

	sPrj	+= STR_ADD_STR("units"	, pj_units[P("UNIT")->asInt()].id);

	//-----------------------------------------------------
	if( P("DATUM_DEF")->asInt() == 0 )	// predefined datum
	{
		sPrj	+= STR_ADD_STR("datum"	, pj_datums[P("DATUM")->asInt()].id);
	}

	//-----------------------------------------------------
	else								// user defined datum
	{
		switch( P("ELLIPSOID")->asInt() )
		{
		case 0:	// Predefined Ellipsoid
			sPrj	+= STR_ADD_STR("ellps"	, pj_ellps[P("ELLPS_PREDEF")->asInt()].id);
			break;

		case 1:	// Semiminor axis
			sPrj	+= STR_ADD_FLT("a"		, P("ELLPS_A" )->asDouble());
			sPrj	+= STR_ADD_FLT("b"		, P("ELLPS_B" )->asDouble());
			break;

		case 2:	// Flattening
			sPrj	+= STR_ADD_FLT("a"		, P("ELLPS_A" )->asDouble());
			sPrj	+= STR_ADD_FLT("f"		, P("ELLPS_F" )->asDouble());
			break;

		case 3:	// Reciprocal Flattening
			sPrj	+= STR_ADD_FLT("a"		, P("ELLPS_A" )->asDouble());
			sPrj	+= STR_ADD_FLT("rf"		, P("ELLPS_RF")->asDouble());
			break;

		case 4:	// Eccentricity
			sPrj	+= STR_ADD_FLT("a"		, P("ELLPS_A" )->asDouble());
			sPrj	+= STR_ADD_FLT("e"		, P("ELLPS_E" )->asDouble());
			break;

		case 5:	// Eccentricity Squared
			sPrj	+= STR_ADD_FLT("a"		, P("ELLPS_A" )->asDouble());
			sPrj	+= STR_ADD_FLT("es"		, P("ELLPS_ES")->asDouble());
			break;
		}

		switch( P("DATUM_SHIFT")->asInt() )
		{
		case 1:	// 3 parameters
			sPrj	+= CSG_String::Format(SG_T("+towgs84=%f,%f,%f "),
				P("DS_DX")->asDouble(),
				P("DS_DY")->asDouble(),
				P("DS_DZ")->asDouble()
			);
			break;

		case 2:	// 7 parameters
			sPrj	+= CSG_String::Format(SG_T("+towgs84=%f,%f,%f,%f,%f,%f,%f "),
				P("DS_DX")->asDouble(),
				P("DS_DY")->asDouble(),
				P("DS_DZ")->asDouble(),
				P("DS_RX")->asDouble(),
				P("DS_RY")->asDouble(),
				P("DS_RZ")->asDouble(),
				P("DS_SC")->asDouble()
			);
			break;
		}
	}

	//-----------------------------------------------------
	CSG_Parameters	*pParms	= Get_Parameters(pj_list[P("PROJ_TYPE")->asInt()].id);

	if( pParms == NULL || pParms->Get_Count() <= 0 )
	{
		return( true );
	}

	if( Dlg_Parameters(pj_list[P("PROJ_TYPE")->asInt()].id) )
	{
		for(int i=0; i<pParms->Get_Count(); i++)
		{
			CSG_Parameter	*p	= pParms->Get_Parameter(i);

			switch( p->Get_Type() )
			{
			case PARAMETER_TYPE_Bool:	sPrj	+= STR_ADD_BOL(p->Get_Identifier(), p->asBool());	break;
			case PARAMETER_TYPE_Int:	sPrj	+= STR_ADD_INT(p->Get_Identifier(), p->asInt());	break;
			case PARAMETER_TYPE_Double:	sPrj	+= STR_ADD_FLT(p->Get_Identifier(), p->asDouble());	break;
			}
		}

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//					Initializations						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPROJ4_Base::_Init_Projection(CSG_Parameters &P)
{
	CSG_String		sList, sName, sDesc, sArgs;
	CSG_Parameter	*pNode_0, *pNode_1, *pNode_2, *pNode_3;

	pNode_0	= NULL;


	//-----------------------------------------------------
	// Projections...

	sDesc	= _TL("Available Projections:");

	sList.Clear();

	for(struct PJ_LIST *pProjection=pj_list; pProjection->id; ++pProjection)
	{
		sArgs	= *pProjection->descr;
		sName	= sArgs.BeforeFirst('\n');
		sArgs	= sArgs.AfterFirst ('\n').AfterFirst('\n').AfterFirst('\t');

		sList	+= CSG_String::Format(SG_T("%s|"), sName.c_str());
		sDesc	+= CSG_String::Format(SG_T("\n[%s] %s (%s)"), pProjection->id, sName.c_str(), sArgs.c_str());

		_Init_Projection(pProjection->id, sName, sArgs);
	}

	if( sList.Length() > 0 )
	{
		P.Add_Choice(NULL, "PROJ_TYPE", _TL("Projection Type"), sDesc, sList);
	}


	//-----------------------------------------------------
	// Datums...

	pNode_1	= P.Add_Choice(
		pNode_0, "DATUM_DEF"		, _TL("Datum Definition"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("Predefined Datum"),
			_TL("User Defined Datum")
		)
	);

	//-----------------------------------------------------
	sList.Clear();

	for(struct PJ_DATUMS *pDatum=pj_datums; pDatum->id; ++pDatum)
	{
		sList	+= CSG_String::Format(SG_T("[%s]"), SG_STR_MBTOSG(pDatum->id));

		if( pDatum->comments != NULL && *pDatum->comments != NULL )
		{
			sList	+= CSG_String::Format(SG_T(" %s"), SG_STR_MBTOSG(pDatum->comments));
		}

		sList	+= '|';
	}

	if( sList.Length() > 0 )
	{
		pNode_2	= P.Add_Choice(pNode_1, "DATUM", _TL("Predefined Datum"), _TL(""), sList);
	}


	//-----------------------------------------------------
	// User defined ellipsoid and datum shift...

	pNode_2	= P.Add_Node(
		pNode_1, "NODE_USER_DATUM"	, _TL("User Defined Datum"),
		_TL("")
	);

	//-----------------------------------------------------
	pNode_3	= P.Add_Choice(
		pNode_2, "ELLIPSOID"		, _TL("Ellipsoid Definition"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|%s|"),
			_TL("Predefined Standard Ellipsoids"),
			_TL("Semimajor Axis and Semiminor Axis"),
			_TL("Semimajor Axis and Flattening"),
			_TL("Semimajor Axis and Reciprocal Flattening"),
			_TL("Semimajor Axis and Eccentricity"),
			_TL("Semimajor Axis and Eccentricity Squared")
		)
	);

	//-----------------------------------------------------
	sList.Clear();

	for(struct PJ_ELLPS *pEllipse=pj_ellps; pEllipse->id; ++pEllipse)
	{
		sList	+= CSG_String::Format(SG_T("[%s] %s (%s, %s)|"), SG_STR_MBTOSG(pEllipse->id), SG_STR_MBTOSG(pEllipse->name), SG_STR_MBTOSG(pEllipse->major), SG_STR_MBTOSG(pEllipse->ell));
	}

	if( sList.Length() > 0 )
	{
		P.Add_Choice(pNode_3, "ELLPS_PREDEF", _TL("Predefined Standard Ellipsoids"), _TL(""), sList);
	}

	//-----------------------------------------------------
	P.Add_Value(
		pNode_3, "ELLPS_A"		, _TL("Semimajor Axis (a)"),
		_TL("Semimajor axis or equatorial radius."),
		PARAMETER_TYPE_Double	, BESSEL_A
	);

	P.Add_Value(
		pNode_3, "ELLPS_B"		, _TL("Semiminor Axis (b)"),
		_TL("Semiminor axis or polar radius."),
		PARAMETER_TYPE_Double	, BESSEL_B
	);

	P.Add_Value(
		pNode_3, "ELLPS_F"		, _TL("Flattening (f)"),
		_TL(""),
		PARAMETER_TYPE_Double	, (BESSEL_A - BESSEL_B) / BESSEL_A
	);

	P.Add_Value(
		pNode_3, "ELLPS_RF"		, _TL("Reciprocal Flattening (rf)"),
		_TL(""),
		PARAMETER_TYPE_Double	, BESSEL_A / (BESSEL_A - BESSEL_B)
	);

	P.Add_Value(
		pNode_3, "ELLPS_E"		, _TL("Eccentricity (e)"),
		_TL(""),
		PARAMETER_TYPE_Double	, sqrt(BESSEL_A*BESSEL_A + BESSEL_B*BESSEL_B)
	);

	P.Add_Value(
		pNode_3, "ELLPS_ES"		, _TL("Squared Eccentricity (es)"),
		_TL(""),
		PARAMETER_TYPE_Double	, BESSEL_A*BESSEL_A + BESSEL_B*BESSEL_B
	);


	//-----------------------------------------------------
	pNode_3	= P.Add_Choice(
		pNode_2, "DATUM_SHIFT"	, _TL("Datum Shift"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|"),
			_TL("none"),
			_TL("3 parameters (translation only)"),
			_TL("7 parameters")
		)
	);

	P.Add_Value(
		pNode_3, "DS_DX"		, _TL("Translation X"),
		_TL(""),
		PARAMETER_TYPE_Double	, 0.0
	);

	P.Add_Value(
		pNode_3, "DS_DY"		, _TL("Translation Y"),
		_TL(""),
		PARAMETER_TYPE_Double	, 0.0
	);

	P.Add_Value(
		pNode_3, "DS_DZ"		, _TL("Translation Z"),
		_TL(""),
		PARAMETER_TYPE_Double	, 0.0
	);

	P.Add_Value(
		pNode_3, "DS_RX"		, _TL("Rotation X"),
		_TL(""),
		PARAMETER_TYPE_Double	, 0.0
	);

	P.Add_Value(
		pNode_3, "DS_RY"		, _TL("Rotation Y"),
		_TL(""),
		PARAMETER_TYPE_Double	, 0.0
	);

	P.Add_Value(
		pNode_3, "DS_RZ"		, _TL("Rotation Z"),
		_TL(""),
		PARAMETER_TYPE_Double	, 0.0
	);

	P.Add_Value(
		pNode_3, "DS_SC"		, _TL("Scaling"),
		_TL(""),
		PARAMETER_TYPE_Double	, 1.0
	);


	//-----------------------------------------------------
	// General Settings...

	pNode_1	= P.Add_Node(
		pNode_0, "NODE_GENERAL"	, _TL("General Settings"),
		_TL("")
	);

	//-----------------------------------------------------
	P.Add_Value(
		pNode_1, "LON_0"		, _TL("Central Meridian"),
		_TL(""),
		PARAMETER_TYPE_Double, 0.0
	);

	P.Add_Value(
		pNode_1, "LAT_0"		, _TL("Central Parallel"),
		_TL(""),
		PARAMETER_TYPE_Double, 0.0
	);

	P.Add_Value(
		pNode_1, "X_0"			, _TL("False Easting"),
		_TL(""),
		PARAMETER_TYPE_Double, 0.0
	);

	P.Add_Value(
		pNode_1, "Y_0"			, _TL("False Northing"),
		_TL(""),
		PARAMETER_TYPE_Double, 0.0
	);


	//-----------------------------------------------------
	sList.Clear();

	for(struct PJ_UNITS *pUnit=pj_units; pUnit->id; ++pUnit)
	{
		sList	+= CSG_String::Format(SG_T("%s (%s)|"), SG_STR_MBTOSG(pUnit->name), SG_STR_MBTOSG(pUnit->to_meter));
	}

	if( sList.Length() > 0 )
	{
		P.Add_Choice(pNode_1, "UNIT", _TL("Unit"), _TL(""), sList, 1);
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
bool CPROJ4_Base::_Init_Projection(const CSG_String &sID, const CSG_String &sName, const CSG_String &sArgs)
{
	if( sArgs.Length() == 0 )
	{
		return( false );
	}

	CSG_Parameters	*pParms	= Add_Parameters(sID, sName, sArgs);

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

	if(	!sID.CmpNoCase(SG_T("lsat")) )		// Space oblique for LANDSAT
	{
		PRM_ADD_INT("lsat"		, _TL("Landsat")				, 1.0);
		PRM_ADD_INT("path"		, _TL("Path")					, 1.0);
	}

	if(	!sID.CmpNoCase(SG_T("labrd")) )		// Laborde
	{
		PRM_ADD_FLT("azi"		, _TL("Azimuth"	)				, 19.0);
		PRM_ADD_FLT("k_0"		, _TL("k_0")					, 0.9995);
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
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
