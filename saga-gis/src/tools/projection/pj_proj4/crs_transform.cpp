
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
//                   crs_transform.cpp                   //
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
#include "crs_transform.h"


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <proj.h>

//---------------------------------------------------------
#define PROJ_FREE(p) if( p ) { proj_destroy((PJ *)p); p = NULL; }


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_CRSProjector::CSG_CRSProjector(void)
{
	_On_Construction();
}

//---------------------------------------------------------
CSG_CRSProjector::CSG_CRSProjector(const CSG_CRSProjector &Projector)
{
	_On_Construction();

	Create(Projector);
}

//---------------------------------------------------------
CSG_CRSProjector::~CSG_CRSProjector(void)
{
	Destroy();

	proj_context_destroy((PJ_CONTEXT *)m_pContext);
}

//---------------------------------------------------------
void CSG_CRSProjector::_On_Construction(void)
{
	m_pContext = proj_context_create();
}

//---------------------------------------------------------
bool CSG_CRSProjector::Create(const CSG_CRSProjector &Projector)
{
	Destroy();

	m_Source   = Projector.m_Source;
	m_Target   = Projector.m_Target;

	m_bInverse = Projector.m_bInverse;

	if( Projector.m_pSource && Projector.m_pTarget )
	{
		return( Set_Transformation() );
	}

	return( true );
}

//---------------------------------------------------------
bool CSG_CRSProjector::Destroy(void)
{
	PROJ_FREE(m_pSource); PROJ_FREE(m_pTarget);

	m_bInverse = false;

	Set_Copies();

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_CRSProjector::Set_Copies(int nCopies)
{
	if( m_Copies )
	{
		delete[](m_Copies);

		m_Copies = NULL; m_nCopies = 0;
	}

	if( nCopies > 1 )
	{
		m_Copies = new CSG_CRSProjector[m_nCopies = nCopies - 1];

		for(int i=0; i<m_nCopies; i++)
		{
			m_Copies[i].Create(*this);
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String CSG_CRSProjector::Get_Version(void)
{
	return( CSG_String::Format("%d.%d.%d", PROJ_VERSION_MAJOR, PROJ_VERSION_MINOR, PROJ_VERSION_PATCH) );
}

//---------------------------------------------------------
CSG_String CSG_CRSProjector::Get_Description(void)
{
	CSG_String s;

	s += _TL("Projection routines make use of the PROJ generic coordinate transformation software.");
	s += "\n";
	s += _TL("PROJ Version is ") + Get_Version();
	s += "\n";
	s += "<a target=\"_blank\" href=\"https://proj.org\">PROJ Homepage</a>";

	return( s );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String CSG_CRSProjector::Convert_CRS_To_PROJ(const CSG_String &Definition)
{
	return( Convert_CRS_Format(Definition, TCRS_Format::PROJ) );
}

CSG_String CSG_CRSProjector::Convert_CRS_To_WKT1(const CSG_String &Definition, bool bMultiLine)
{
	return( Convert_CRS_Format(Definition, TCRS_Format::WKT1, bMultiLine) );
}

CSG_String CSG_CRSProjector::Convert_CRS_To_WKT2(const CSG_String &Definition, bool bMultiLine, bool bSimplified)
{
	return( Convert_CRS_Format(Definition, TCRS_Format::WKT2, bMultiLine, bSimplified) );
}

CSG_String CSG_CRSProjector::Convert_CRS_To_JSON(const CSG_String &Definition, bool bMultiLine)
{
	return( Convert_CRS_Format(Definition, TCRS_Format::JSON, bMultiLine) );
}

CSG_String CSG_CRSProjector::Convert_CRS_To_ESRI(const CSG_String &Definition)
{
	return( Convert_CRS_Format(Definition, TCRS_Format::ESRI) );
}

//---------------------------------------------------------
CSG_String CSG_CRSProjector::Convert_CRS_Format(const CSG_String &Definition, TCRS_Format Format, bool bMultiLine, bool bSimplified)
{
	if( Definition.is_Empty() )
	{
		return( "" );
	}

	//-----------------------------------------------------
	if( Definition.Find("+proj") >= 0 && Definition.Find("+type=crs") < 0 )
	{
		return( Convert_CRS_Format(Definition + " +type=crs", Format, bMultiLine, bSimplified) );
	}

	//-----------------------------------------------------
	CSG_Projection Projection; // check preference list first!

	if( SG_Get_Projections().Get_Preference(Projection, Definition) )
	{
		if( Format == TCRS_Format::WKT2 && !bMultiLine ) { return( Projection.Get_WKT2() ); }
		if( Format == TCRS_Format::PROJ                ) { return( Projection.Get_PROJ() ); }

		return( Convert_CRS_Format(Projection.Get_WKT2(), Format, bMultiLine, bSimplified) );
	}

	//-----------------------------------------------------
	CSG_String CRS; PJ *pProjection = proj_create(0, Definition);

	if( pProjection )
	{
		const char *s = NULL, *options[] = { bMultiLine ? "MULTILINE=YES" : "MULTILINE=NO", NULL };

		switch( Format )
		{
		case TCRS_Format::PROJ   : s = proj_as_proj_string(0, pProjection, PJ_PROJ_STRING_TYPE::PJ_PROJ_5, 0); break;
		case TCRS_Format::JSON   : s = proj_as_projjson   (0, pProjection, options); break;
		case TCRS_Format::ESRI   : s = proj_as_wkt        (0, pProjection, PJ_WKT_TYPE::PJ_WKT1_ESRI, options); break;
		case TCRS_Format::WKT1   : s = proj_as_wkt        (0, pProjection, PJ_WKT_TYPE::PJ_WKT1_GDAL, options); break;
		case TCRS_Format::WKT2   :
		case TCRS_Format::WKT2015: s = proj_as_wkt        (0, pProjection, bSimplified ? PJ_WKT_TYPE::PJ_WKT2_2015_SIMPLIFIED : PJ_WKT_TYPE::PJ_WKT2_2015, options); break;
		case TCRS_Format::WKT2018: s = proj_as_wkt        (0, pProjection, bSimplified ? PJ_WKT_TYPE::PJ_WKT2_2018_SIMPLIFIED : PJ_WKT_TYPE::PJ_WKT2_2018, options); break;
		case TCRS_Format::WKT2019: s = proj_as_wkt        (0, pProjection, bSimplified ? PJ_WKT_TYPE::PJ_WKT2_2019_SIMPLIFIED : PJ_WKT_TYPE::PJ_WKT2_2019, options); break;
		}

		if( s && *s )
		{
			CRS = CSG_String::from_UTF8(s); if( CRS.is_Empty() ) { CRS = s; } CRS.Replace("\"unknown\"", "\"<custom>\"");
		}

		proj_destroy(pProjection);
	}

	return( CRS );
}

//---------------------------------------------------------
bool CSG_CRSProjector::Convert_CRS_Format(const CSG_String &Definition, CSG_String *PROJ, CSG_String *WKT1, CSG_String *WKT2, CSG_String *JSON, CSG_String *ESRI, bool bMultiLine, bool bSimplified)
{
	if( Definition.is_Empty() || (!PROJ && !WKT1 && !WKT2 && !JSON && !ESRI) )
	{
		return( false );
	}

	//-----------------------------------------------------
	if( Definition.Find("+proj") >= 0 && Definition.Find("+type=crs") < 0 )
	{
		return( Convert_CRS_Format(Definition + " +type=crs", PROJ, WKT1, WKT2, JSON, ESRI, bMultiLine, bSimplified) );
	}

	//-----------------------------------------------------
	CSG_Projection Projection; // check preference list first!

	if( SG_Get_Projections().Get_Preference(Projection, Definition) )
	{
		if( WKT2 ) { *WKT2 = Projection.Get_WKT2(); }
		if( PROJ ) { *PROJ = Projection.Get_PROJ(); }

		Convert_CRS_Format(Projection.Get_WKT2(), NULL, WKT1, NULL, JSON, ESRI, bMultiLine, bSimplified);

		return( true );
	}

	//-----------------------------------------------------
	PJ *pProjection = proj_create(0, Definition);

	if( pProjection )
	{
		const char *options[] = { bMultiLine ? "MULTILINE=YES" : "MULTILINE=NO", NULL };

		#define GET_FORMAT(crs, def) if( crs ) { const char *s = def; if( s && *s ) { *crs = CSG_String::from_UTF8(s); if( crs->is_Empty() ) { *crs = s; } crs->Replace("\"unknown\"", "\"<custom>\""); } }

		GET_FORMAT(PROJ, proj_as_proj_string(0, pProjection, PJ_PROJ_STRING_TYPE::PJ_PROJ_5, 0));
		GET_FORMAT(WKT1, proj_as_wkt        (0, pProjection, PJ_WKT_TYPE::PJ_WKT1_GDAL, options));
		GET_FORMAT(WKT2, proj_as_wkt        (0, pProjection, bSimplified ? PJ_WKT_TYPE::PJ_WKT2_2015_SIMPLIFIED : PJ_WKT_TYPE::PJ_WKT2_2015, options));
	//	GET_FORMAT(WKT2, proj_as_wkt        (0, pProjection, bSimplified ? PJ_WKT_TYPE::PJ_WKT2_2018_SIMPLIFIED : PJ_WKT_TYPE::PJ_WKT2_2018, options));
	//	GET_FORMAT(WKT2, proj_as_wkt        (0, pProjection, bSimplified ? PJ_WKT_TYPE::PJ_WKT2_2019_SIMPLIFIED : PJ_WKT_TYPE::PJ_WKT2_2019, options));
		GET_FORMAT(JSON, proj_as_projjson   (0, pProjection, options));
		GET_FORMAT(ESRI, proj_as_wkt        (0, pProjection, PJ_WKT_TYPE::PJ_WKT1_ESRI, options));

		proj_destroy(pProjection);

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_CRSProjector::Set_Source(const CSG_Projection &Projection, bool bSetTransformation)
{
	return( Projection.is_Okay() && m_Source.Create(Projection) && (!bSetTransformation || Set_Transformation()) );
}

//---------------------------------------------------------
bool CSG_CRSProjector::Set_Target(const CSG_Projection &Projection, bool bSetTransformation)
{
	return( Projection.is_Okay() && m_Target.Create(Projection) && (!bSetTransformation || Set_Transformation()) );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_CRSProjector::_Set_Projection(const CSG_Projection &Projection, void **ppProjection) const
{
	PROJ_FREE(*ppProjection);

	if( !Projection.is_Okay() )
	{
		SG_UI_Msg_Add_Error(CSG_String::Format("PROJ [%s]: %s", _TL("initialization"), _TL("undefined coordinate reference system")));

		return( false );
	}

	CSG_String PROJ(Projection.Get_PROJ()); PROJ.Replace("+type=crs", "");

	*ppProjection = proj_create((PJ_CONTEXT *)m_pContext, PROJ);

	if( proj_errno((PJ *)(*ppProjection)) )
	{
		CSG_String Error(proj_errno_string(proj_errno((PJ *)(*ppProjection))));

		proj_errno_reset((PJ *)(*ppProjection));

		SG_UI_Msg_Add_Error(CSG_String::Format("PROJ [%s]: %s", _TL("initialization"), Error.c_str()));

		PROJ_FREE(*ppProjection);

		return( false );
	}

	return( true );
}

//---------------------------------------------------------
bool CSG_CRSProjector::Set_Transformation(void)
{
	PROJ_FREE(m_pSource); PROJ_FREE(m_pTarget);

	return( _Set_Projection(m_Source, &m_pSource) && _Set_Projection(m_Target, &m_pTarget) );
}

//---------------------------------------------------------
bool CSG_CRSProjector::Set_Transformation(const CSG_Projection &Source, const CSG_Projection &Target)
{
	return( Set_Source(Source) && Set_Target(Target) && Set_Transformation() );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_CRSProjector::Set_Forward(bool bOn)
{
	return( Set_Inverse(!bOn) );
}

//---------------------------------------------------------
bool CSG_CRSProjector::Set_Inverse(bool bOn)
{
	if( m_bInverse != bOn )
	{
		m_bInverse = bOn;

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_CRSProjector::Has_Inverse(void) const
{
	return( m_pTarget && proj_pj_info((PJ *)(m_pTarget)).has_inverse );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_CRSProjector::Get_Projection(double &x, double &y)	const
{
	if( !m_pSource || !m_pTarget ) { return( false ); }

	PJ *pSource = (PJ *)(m_bInverse ? m_pTarget : m_pSource);
	PJ *pTarget = (PJ *)(m_bInverse ? m_pSource : m_pTarget);

	if( proj_angular_input(pSource, PJ_INV) )
	{
		x *= M_DEG_TO_RAD; y *= M_DEG_TO_RAD;
	}

	PJ_COORD c = proj_coord(x, y, 0., 0.);

	c = proj_trans(pSource, PJ_INV, c); if( proj_errno(pSource) ) { proj_errno_reset(pSource); return( false ); }
	c = proj_trans(pTarget, PJ_FWD, c); if( proj_errno(pTarget) ) { proj_errno_reset(pTarget); return( false ); }

	x = c.v[0]; y = c.v[1];

	if( proj_angular_output(pTarget, PJ_FWD) )
	{
		x *= M_RAD_TO_DEG; y *= M_RAD_TO_DEG;
	}

	return( true );
}

//---------------------------------------------------------
bool CSG_CRSProjector::Get_Projection(double &x, double &y, double &z)	const
{
	if( !m_pSource || !m_pTarget ) { return( false ); }

	PJ *pSource = (PJ *)(m_bInverse ? m_pTarget : m_pSource);
	PJ *pTarget = (PJ *)(m_bInverse ? m_pSource : m_pTarget);

	if( proj_angular_input(pSource, PJ_INV) )
	{
		x *= M_DEG_TO_RAD; y *= M_DEG_TO_RAD;
	}

	PJ_COORD c = proj_coord(x, y, z, 0.);
	
	c = proj_trans(pSource, PJ_INV, c); if( proj_errno(pSource) ) { proj_errno_reset(pSource); return( false ); }
	c = proj_trans(pTarget, PJ_FWD, c); if( proj_errno(pTarget) ) { proj_errno_reset(pTarget); return( false ); }

	x = c.v[0]; y = c.v[1]; z = c.v[2];

	if( proj_angular_output(pTarget, PJ_FWD) )
	{
		x *= M_RAD_TO_DEG; y *= M_RAD_TO_DEG;
	}

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_CRSProjector::Get_Projection(TSG_Point &Point) const { return( Get_Projection(Point.x, Point.y) ); }
bool CSG_CRSProjector::Get_Projection(CSG_Point &Point) const
{
	CSG_Point p(Point);

	if( Get_Projection(p.x, p.y) )
	{
		Point = p;

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_CRSProjector::Get_Projection(TSG_Point_3D &Point) const { return( Get_Projection(Point.x, Point.y, Point.z) ); }
bool CSG_CRSProjector::Get_Projection(CSG_Point_3D &Point) const
{
	CSG_Point_3D p(Point);

	if( Get_Projection(p.x, p.y, p.z) )
	{
		Point = p;

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//                                                       //
//                   CRS Operation                       //
//                                                       //
///////////////////////////////////////////////////////////

////---------------------------------------------------------
//bool CSG_CRSProjector::_Set_Transformation(void *pSource, void *pTarget, void **ppTransformation) const
//{
//	PROJ_FREE(*ppTransformation);
//
//	*ppTransformation = proj_create_crs_to_crs_from_pj((PJ_CONTEXT *)m_pContext, (PJ *)pSource, (PJ *)pTarget, NULL, NULL);
//
//	if( proj_errno((PJ *)(*ppTransformation)) )
//	{
//		CSG_String Error(proj_errno_string(proj_errno((PJ *)(*ppTransformation))));
//
//		proj_errno_reset((PJ *)(*ppTransformation));
//
//		SG_UI_Msg_Add_Error(CSG_String::Format("PROJ [%s]: %s", _TL("initialization"), Error.c_str()));
//
//		PROJ_FREE(*ppTransformation);
//
//		return( false );
//	}
//
//	return( true );
//}
//
////---------------------------------------------------------
//bool CSG_CRSProjector::_Set_Transformation(const CSG_Projection &Source, const CSG_Projection &Target, void **ppTransformation) const
//{
//	PROJ_FREE(*ppTransformation);
//
//	*ppTransformation = proj_create_crs_to_crs((PJ_CONTEXT *)m_pContext, Source.Get_PROJ(), Target.Get_PROJ(), NULL);
//
//	if( proj_errno((PJ *)(*ppTransformation)) )
//	{
//		CSG_String Error(proj_errno_string(proj_errno((PJ *)(*ppTransformation))));
//
//		proj_errno_reset((PJ *)(*ppTransformation));
//
//		SG_UI_Msg_Add_Error(CSG_String::Format("PROJ [%s]: %s", _TL("initialization"), Error.c_str()));
//
//		PROJ_FREE(*ppTransformation);
//
//		return( false );
//	}
//
//	return( true );
//}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
