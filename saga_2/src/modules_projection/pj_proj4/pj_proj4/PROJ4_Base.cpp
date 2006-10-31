
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
	Parameters.Add_Node(NULL, "SOURCE_NODE"	, _TL("Source Parameters")	, "");
	Parameters.Add_Node(NULL, "TARGET_NODE"	, _TL("Target Parameters")	, "");

	Parameters.Add_Node(NULL, "PROJECTION"	, _TL("Projection")			, "");

	//-----------------------------------------------------
	bInitialized	= false;

	Initialize();
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
bool CPROJ4_Base::Initialize(void)
{
	char				c;
	int					i;
	struct PJ_LIST		*pProjections;
	struct PJ_ELLPS		*pEllipses;
//	struct PJ_DATUMS	*pDatums;
	struct PJ_UNITS		*pUnits;
	CSG_String			sList, sName, sDesc;
	CSG_Parameter			*pNode_0, *pNode_1, *pNode_2, *pNode_3;

	//-----------------------------------------------------
	if( !bInitialized )
	{
		bInitialized	= true;

		pNode_0	= Parameters("PROJECTION");

		pNode_1	= Parameters.Add_Choice(
			pNode_0, "DIRECTION"	, _TL("Direction"),
			"",
			_TL("Geodetic to Projection|"
			"Projection to Geodetic|"), 0
		);


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

			sList	+= CSG_String::Format("%s|", sName.c_str());
			sDesc	+= CSG_String::Format("\n[%s] %s", pProjections->id, *pProjections->descr);

			Initialize_ExtraParms(pProjections, sName);
		}

		if( sList.Length() > 0 )
		{
			pNode_1	= Parameters.Add_Choice(
				pNode_0, "PROJ_TYPE"	, _TL("Projection Type"),
				sDesc,
				sList
			);
		}


		//-------------------------------------------------
		// Ellipsoid...

		pNode_1	= Parameters.Add_Choice(
			pNode_0, "ELLIPSOID"	, _TL("Ellipsoid"),
			"",_TL(
			"Predefined Standard Ellipsoids|"
			"Semimajor Axis and Semiminor Axis|"
			"Semimajor Axis and Flattening|"
			"Semimajor Axis and Reciprocal Flattening|"
			"Semimajor Axis and Eccentricity|"
			"Semimajor Axis and Eccentricity Squared|")
		);

		sList.Clear();

		for(pEllipses=pj_ellps; pEllipses->id; ++pEllipses)
		{
			sList	+= CSG_String::Format("%s (%s, %s)|", pEllipses->name, pEllipses->major, pEllipses->ell);
		}

		if( sList.Length() > 0 )
		{
			pNode_2	= Parameters.Add_Choice(
				pNode_1, "ELLPS_PREDEF"	, _TL("Predefined Standard Ellipsoids"),
				"",
				sList
			);
		}

		pNode_2	= Parameters.Add_Value(
			pNode_1, "ELLPS_A"		, _TL("Semimajor Axis (a)"),
			_TL("Semimajor axis or equatorial radius."),
			PARAMETER_TYPE_Double	, BESSEL_A
		);

		pNode_3	= Parameters.Add_Value(
			pNode_2, "ELLPS_B"		, _TL("Semiminor Axis (b)"),
			_TL("Semiminor axis or polar radius."),
			PARAMETER_TYPE_Double	, BESSEL_B
		);

		pNode_3	= Parameters.Add_Value(
			pNode_2, "ELLPS_F"		, _TL("Flattening (f)"),
			"",
			PARAMETER_TYPE_Double	, (BESSEL_A - BESSEL_B) / BESSEL_A
		);

		pNode_3	= Parameters.Add_Value(
			pNode_2, "ELLPS_RF"		, _TL("Reciprocal Flattening (rf)"),
			"",
			PARAMETER_TYPE_Double	, BESSEL_A / (BESSEL_A - BESSEL_B)
		);

		pNode_3	= Parameters.Add_Value(
			pNode_2, "ELLPS_E"		, _TL("Eccentricity (e)"),
			"",
			PARAMETER_TYPE_Double	, sqrt(BESSEL_A*BESSEL_A + BESSEL_B*BESSEL_B)
		);

		pNode_3	= Parameters.Add_Value(
			pNode_2, "ELLPS_ES"		, _TL("Squared Eccentricity (es)"),
			"",
			PARAMETER_TYPE_Double	, BESSEL_A*BESSEL_A + BESSEL_B*BESSEL_B
		);


		/*/-------------------------------------------------
		// Datums...

		sList.Clear();

		for(pDatums=pj_datums; pDatums->id; ++pDatums)
		{
			sList	+= CSG_String::Format(sName, "%s (%s) ", pDatums->defn, pDatums->ellipse_id);

			if( pDatums->comments != NULL && strlen(pDatums->comments) > 0 )
			{
				sList	+= CSG_String::Format(%s", pDatums->comments);
			}

			sList	+= '|';
		}

		if( sList.Length > 0 )
		{
			pNode_1	= Parameters.Add_Choice(
				pNode_0, "DATUM"		, "Datum",
				"",
				sList
			);
		}/**/


		//-------------------------------------------------
		// Units...

		sList.Clear();

		for(pUnits=pj_units; pUnits->id; ++pUnits)
		{
			sList	+= CSG_String::Format("%s (%s)|", pUnits->name, pUnits->to_meter);
		}

		if( sList.Length() > 0 )
		{
			pNode_1	= Parameters.Add_Choice(
				pNode_0, "UNIT"			, _TL("Unit"),
				"",
				sList, 1
			);
		}


		//-------------------------------------------------
		// More Settings...

		pNode_1	= Parameters.Add_Value(
			pNode_0, "LON_0"			, _TL("Central Meridian"),
			"",
			PARAMETER_TYPE_Double, 0.0
		);

		pNode_1	= Parameters.Add_Value(
			pNode_0, "LAT_0"			, _TL("Central Parallel"),
			"",
			PARAMETER_TYPE_Double, 0.0
		);

		pNode_1	= Parameters.Add_Value(
			pNode_0, "X_0"				, _TL("False Easting"),
			"",
			PARAMETER_TYPE_Double, 0.0
		);

		pNode_1	= Parameters.Add_Value(
			pNode_0, "Y_0"				, _TL("False Northing"),
			"",
			PARAMETER_TYPE_Double, 0.0
		);
	}

	return( bInitialized );
}

//---------------------------------------------------------
#define ISID(sID)					!strcmp(pProjection->id, sID)

#define ADD_BOOLE(key, name, val)	pParms->Add_Value(NULL, key, name, "", PARAMETER_TYPE_Bool	, val);
#define ADD_INTEG(key, name, val)	pParms->Add_Value(NULL, key, name, "", PARAMETER_TYPE_Int	, val);
#define ADD_FLOAT(key, name, val)	pParms->Add_Value(NULL, key, name, "", PARAMETER_TYPE_Double, val);

//---------------------------------------------------------
bool CPROJ4_Base::Initialize_ExtraParms(struct PJ_LIST *pProjection, const char *sName)
{
	CSG_Parameters	*pParms;

	pParms	= Add_Parameters(pProjection->id, sName, *pProjection->descr);


	//-----------------------------------------------------
	// Cylindrical Projections...

	if(	ISID("cea"		)	// Equal Area Cylindrical
	||	ISID("eqc"		)	// Equidistant Cylindrical (Plate Caree) 
	||	ISID("merc"		) )	// Mercator 
	{
		ADD_FLOAT("lat_ts"	, _TL("True Scale Latitude")	, 0.0);
	}

	if(	ISID("utm"		) )	// Universal Transverse Mercator (UTM)
	{
		ADD_INTEG("zone"	, _TL("Zone")				, 32);
		ADD_BOOLE("south"	, _TL("South")				, false);
	}

	if(	ISID("omerc"	) )	// Oblique Mercator 
	{
		ADD_FLOAT("lat_1"	, _TL("Latitude 1")			, 40.0);
		ADD_FLOAT("lon_1"	, _TL("Longitude 1"	)		,-20.0);
		ADD_FLOAT("lat_2"	, _TL("Latitude 2")			, 50.0);
		ADD_FLOAT("lon_2"	, _TL("Longitude 2"	)		, 20.0);
	}

	//-----------------------------------------------------
	// Pseudocylindrical Projections...

	if(	ISID("gn_sinu"	) )	// General Sinusoidal Series
	{
		ADD_FLOAT("m"		, "m"					, 0.5);
		ADD_FLOAT("n"		, "n"					, 1.0 + M_PI_045);
	}

	if(	ISID("loxim"	) )	// Loximuthal
	{
		ADD_FLOAT("lat_1"	, _TL("Latitude 1")			, 40.0);
	}

	if(	ISID("urmfps"	) )	// Urmaev Flat-Polar Sinusoidal
	{
		ADD_FLOAT("n"		, "n"					, 1.0);
	}

	if(	ISID("urm5"		) )	// Urmaev V
	{
		ADD_FLOAT("n"		, "n"					, 1.0);
		ADD_FLOAT("q"		, "q"					, 1.0);
		ADD_FLOAT("alphi"	, "alphi"				, 45.0);
	}

	if(	ISID("wink1"	)	// Winkel I
	||	ISID("wag3"		) )	// Wagner III
	{
		ADD_FLOAT("lat_ts"	, _TL("True Scale Latitude")	, 45.0);
	}

	if(	ISID("wink2"	) )	// Winkel II
	{
		ADD_FLOAT("lat_1"	, _TL("Latitude 1")			, 40.0);
	}


	//-----------------------------------------------------
	// Conic Projections...

	if(	ISID("aea"		)	// Albers Equal Area
	||	ISID("eqdc"		)	// Equidistant Conic
	||	ISID("euler"	)	// Euler 
	||	ISID("imw_p"	)	// International Map of the World Polyconic 
	||	ISID("murd1"	)	// Murdoch I 
	||	ISID("murd2"	)	// Murdoch II 
	||	ISID("murd3"	)	// Murdoch III 
	||	ISID("pconic"	)	// Perspective Conic 
	||	ISID("tissot"	)	// Tissot 
	||	ISID("vitk1"	) )	// Vitkovsky I 
	{
		ADD_FLOAT("lat_1"	, _TL("Latitude 1")			, 33.0);
		ADD_FLOAT("lat_2"	, _TL("Latitude 2")			, 45.0);
	}

	if(	ISID("lcc"		) )	// Lambert Conformal Conic 
	{
		ADD_FLOAT("lat_1"	, _TL("Latitude 1")			, 33.0);
		ADD_FLOAT("lat_2"	, _TL("Latitude 2")			, 45.0);
	}

	if( ISID("leac"		) )	// Lambert Equal Area Conic
	{
		ADD_FLOAT("lat_1"	, _TL("Latitude 1")			, 45.0);
		ADD_BOOLE("south"	, _TL("South")				, false);
	}

	if(	ISID("rpoly"	) )	// Rectangular Polyconic
	{
		ADD_FLOAT("lat_ts"	, _TL("True Scale Latitude")	, 45.0);
	}

	if(	ISID("mpoly"	) )	// Modified Polyconic
	{
		ADD_FLOAT("lat_1"	, _TL("Latitude 1")			, 33.0);
		ADD_FLOAT("lat_2"	, _TL("Latitude 2")			, 45.0);
		ADD_BOOLE("lotsa"	, _TL("Lotsa")				, true);
	}

	if(	ISID("bonne"	) )	// Bonne
	{
		ADD_FLOAT("lat_1"	, _TL("Latitude 1")			, 45.0);
	}


	//-----------------------------------------------------
	// Azimuthal Projections...

	if(	ISID("stere"	) )	// Stereographic
	{
		ADD_FLOAT("lat_ts"	, _TL("True Scale Latitude")	, 45.0);
	}

	if(	ISID("ups"		) )	// Universal Polar Stereographic
	{
		ADD_BOOLE("south"	, _TL("South")				, true);
	}

	if(	ISID("airy"		) )	// Airy
	{
		ADD_FLOAT("lat_b"	, _TL("Latitude B")			, 45.0);
		ADD_BOOLE("no_cut"	, _TL("No Cut"	)			, true);
	}

	if(	ISID("nsper"	) )	// Near-sided perspective
	{
		ADD_FLOAT("h"		, _TL("Height of view point"), 1.0);
	}

	if(	ISID("aeqd"		) )	// Azimuthal Equidistant
	{
		ADD_BOOLE("guam"	, "guam"				, true);
	}

	if(	ISID("hammer"	) )	// Hammer & Eckert-Greifendorff
	{
		ADD_FLOAT("W"		, "W"					, 0.5);
		ADD_FLOAT("M"		, "M"					, 1.0);
	}

	if(	ISID("wintri"	) )	// Winkel Tripel 
	{
		ADD_FLOAT("lat_1"	, _TL("Latitude 1")			, 40.0);
	}


	//-----------------------------------------------------
	// Miscellaneous Projections...

	if(	ISID("ocea"		)	// Oblique Cylindrical Equal Area
	||	ISID("tpeqd"	) )	// Two Point Equidistant 
	{
		ADD_FLOAT("lat_1"	, _TL("Latitude 1")			, 40.0);
		ADD_FLOAT("lon_1"	, _TL("Longitude 1")			,-20.0);
		ADD_FLOAT("lat_2"	, _TL("Latitude 2")			, 50.0);
		ADD_FLOAT("lon_2"	, _TL("Longitude 2"	)		, 20.0);
	}

	if(	ISID("lsat"		) )	// Space oblique for LANDSAT
	{
		ADD_INTEG("lsat"	, _TL("Landsat")				, 1.0);
		ADD_INTEG("path"	, _TL("Path")				, 1.0);
	}

	if(	ISID("labrd"	) )	// Laborde
	{
		ADD_FLOAT("azi"		, _TL("Azimuth"	)			, 19.0);
		ADD_FLOAT("k_0"		, "k_0"					, 0.9995);
	}

	if(	ISID("lagrng"	) )	// Lagrange
	{
		ADD_FLOAT("lat_1"	, _TL("Latitude 1")			, 0.0);
		ADD_FLOAT("W"		, "W"					, 2.0);
	}

	if(	ISID("chamb"	) )	// Chamberlin Trimetric
	{
		ADD_FLOAT("lat_1"	, _TL("Latitude 1")			, 30.0);
		ADD_FLOAT("lon_1"	, _TL("Longitude 1"	)		,-20.0);
		ADD_FLOAT("lat_2"	, _TL("Latitude 2")			, 40.0);
		ADD_FLOAT("lon_2"	, _TL("Longitude 2")			, 00.0);
		ADD_FLOAT("lat_3"	, _TL("Latitude 3")			, 50.0);
		ADD_FLOAT("lon_3"	, _TL("Longitude 3"	)		, 20.0);
	}

	if(	ISID("oea"		) )	// Oblated Equal Area
	{
		ADD_FLOAT("m"		, "m"					, 1.0);
		ADD_FLOAT("n"		, "n"					, 1.0);
		ADD_FLOAT("theta"	, "theta"				, 45.0);
	}

	if(	ISID("tpers"	) )	// Tilted perspective
	{
		ADD_FLOAT("tilt"	, _TL("Tilt")				, 45.0);
		ADD_FLOAT("azi"		, _TL("Azimuth")				, 45.0);
		ADD_FLOAT("h"		, "h"					, 1000.0);
	}

	if(	ISID("ob_tran"	) )	// General Oblique Transformation
	{
		ADD_FLOAT("o_lat_p"	, _TL("Latitude Pole")		, 40.0);
		ADD_FLOAT("o_lon_p"	, _TL("Longitude Pole")		, 40.0);
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
	bool	bResult;

	//-----------------------------------------------------
	bResult			= false;

	m_Projection	= NULL;

	//-----------------------------------------------------
	if( bInitialized && Set_Transformation(true) )
	{
		bResult	= On_Execute_Conversion();
	}

	//-----------------------------------------------------
	if( m_Projection )
	{
		pj_free(m_Projection);
	}

	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define ADD_PARG(sKey, sVal)	s.Printf(sKey, sVal);\
								pargv			= (char **)SG_Realloc(pargv, (pargc + 1) * sizeof(char *));\
								pargv[pargc]	= (char  *)SG_Malloc((s.Length() + 1)    * sizeof(char  ));\
								memcpy(pargv[pargc], s.c_str(), s.Length() + 1);\
								pargc++;

//---------------------------------------------------------
bool CPROJ4_Base::Set_Transformation(bool bHistory)
{
	bool		bResult;
	char		**pargv;
	int			pargc, i;
	CSG_String	s;

	//-----------------------------------------------------
	bResult		= true;

	pargc		= 0;
	pargv		= NULL;

	m_Reverse	= false;
	m_Inverse	= Parameters("DIRECTION")->asInt() == 1;

	//-----------------------------------------------------
	ADD_PARG("proj=%s"	, pj_list[Parameters("PROJ_TYPE")->asInt()].id);

	//-----------------------------------------------------
	switch( Parameters("ELLIPSOID")->asInt() )
	{
	case 0:	// Predefined
		ADD_PARG("ellps=%s"	, pj_ellps[Parameters("ELLPS_PREDEF")->asInt()].id);
		break;

	case 1:	// Semiminor axis
		ADD_PARG("a=%f"	, Parameters("ELLPS_A" )->asDouble());
		ADD_PARG("b=%f"	, Parameters("ELLPS_B" )->asDouble());
		break;

	case 2:	// Flattening
		ADD_PARG("a=%f"	, Parameters("ELLPS_A" )->asDouble());
		ADD_PARG("f=%f"	, Parameters("ELLPS_F" )->asDouble());
		break;

	case 3:	// Reciprocal Flattening
		ADD_PARG("a=%f"	, Parameters("ELLPS_A" )->asDouble());
		ADD_PARG("rf=%f", Parameters("ELLPS_RF")->asDouble());
		break;

	case 4:	// Eccentricity
		ADD_PARG("a=%f"	, Parameters("ELLPS_A" )->asDouble());
		ADD_PARG("e=%f"	, Parameters("ELLPS_E" )->asDouble());
		break;

	case 5:	// Eccentricity Squared
		ADD_PARG("a=%f"	, Parameters("ELLPS_A" )->asDouble());
		ADD_PARG("es=%f", Parameters("ELLPS_ES")->asDouble());
		break;
	}

	//-----------------------------------------------------
	ADD_PARG("units=%s"	, pj_units[Parameters("UNIT")->asInt()].id);

	ADD_PARG("lon_0=%f"	, Parameters("LON_0")->asDouble());
	ADD_PARG("lat_0=%f"	, Parameters("LAT_0")->asDouble());

	ADD_PARG("x_0=%f"	, Parameters("X_0"  )->asDouble());
	ADD_PARG("y_0=%f"	, Parameters("Y_0"  )->asDouble());

	//-----------------------------------------------------
	if( !Get_ExtraParms(pargc, &pargv, pj_list[Parameters("PROJ_TYPE")->asInt()].id) )
	{
		bResult		= false;
	}
	else if( (m_Projection = pj_init(pargc, pargv)) == NULL )
	{
		Message_Add	(_TL("Projection initialization failure\ncause: "));
		Message_Add	(pj_strerrno(pj_errno));
		Error_Set	(pj_strerrno(pj_errno));

		bResult		= false;
	}
	else if( m_Inverse && !m_Projection->inv )
	{
		Message_Add	(_TL("Inverse transformation not available for selected projection type."));
		Error_Set	(_TL("Inverse transformation not available for selected projection type."));

		bResult		= false;
	}

	//-----------------------------------------------------
	for(i=0; i<pargc; i++)
	{
		SG_Free(pargv[i]);
	}

	SG_Free(pargv);

	return( bResult );
}

//---------------------------------------------------------
bool CPROJ4_Base::Get_ExtraParms(int &pargc, char ***p_pargv, char *id)
{
	char		**pargv;
	int			i;
	CSG_String	s, sFormat;
	CSG_Parameters	*pParms;
	CSG_Parameter	*pParm;

	if( (pParms = Get_Parameters(id)) != NULL && pParms->Get_Count() > 0 )
	{
		if( Dlg_Parameters(id) )
		{
			pargv	= *p_pargv;

			for(i=0; i<pParms->Get_Count(); i++)
			{
				pParm	= pParms->Get_Parameter(i);

				switch( pParm->Get_Type() )
				{
				case PARAMETER_TYPE_Bool:
					if( pParm->asBool() )
					{
						ADD_PARG("%s", pParm->Get_Identifier());
					}
					break;

				case PARAMETER_TYPE_Int:
					sFormat.Printf("%s=%%d", pParm->Get_Identifier());
					ADD_PARG(sFormat.c_str(), pParm->asInt());
					break;

				case PARAMETER_TYPE_Double:
					sFormat.Printf("%s=%%f", pParm->Get_Identifier());
					ADD_PARG(sFormat.c_str(), pParm->asDouble());
					break;
				}
			}

			*p_pargv	= pargv;
		}
		else
		{
			return( false );
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPROJ4_Base::Set_Transformation_Inverse(void)
{
	if( m_Projection && m_Projection->inv )
	{
		m_Reverse	= true;

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
	bool		bInverse;
	projUV		c;

	if( m_Projection )
	{
		bInverse	= m_Inverse ? !m_Reverse : m_Reverse;

		if( !bInverse )
		{
			c.u		= x * DEG_TO_RAD;
			c.v		= y * DEG_TO_RAD;

			c		= pj_fwd(c, m_Projection);

			if( c.u != HUGE_VAL && c.v != HUGE_VAL )
			{
				x		= c.u;
				y		= c.v;

				return( true );
			}
		}
		else
		{
			c.u		= x;
			c.v		= y;

			c		= pj_inv(c, m_Projection);

			if( c.u != HUGE_VAL && c.v != HUGE_VAL )
			{
				x		= c.u * RAD_TO_DEG;
				y		= c.v * RAD_TO_DEG;

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
