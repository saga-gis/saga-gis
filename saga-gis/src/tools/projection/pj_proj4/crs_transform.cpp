
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
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#if PROJ_VERSION_MAJOR < 6
	extern "C" {
		#include <projects.h>
	}

	#define PROJ4_FREE(p)	if( p )	{	pj_free((PJ *)p);	p	= NULL;	}

//---------------------------------------------------------
#else
	#include <proj.h>

	#if PROJ_VERSION_MINOR < 2
		#define PROJ4_FREE(p)	if( p )	{	proj_destroy((PJ *)p);	p	= NULL;	}
	#else
		#define PROJ4_FREE(p)	if( p )	{	proj_destroy((PJ *)p);	p	= NULL; proj_cleanup();	}
	#endif
#endif


///////////////////////////////////////////////////////////
//														 //
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

	#if PROJ_VERSION_MAJOR >= 6
		proj_context_destroy((PJ_CONTEXT *)m_pContext);

		#if PROJ_VERSION_MINOR >= 2
			proj_cleanup();
		#endif
	#endif
}

//---------------------------------------------------------
void CSG_CRSProjector::_On_Construction(void)
{
	m_pSource	= NULL;
	m_pTarget	= NULL;
	m_pGCS		= NULL;

	m_bInverse	= false;

	m_Copies	= NULL;
	m_nCopies	= 0;

	#if PROJ_VERSION_MAJOR < 6
		m_pContext	= NULL;
	#else
		m_pContext	= proj_context_create();
	#endif
}

//---------------------------------------------------------
bool CSG_CRSProjector::Create(const CSG_CRSProjector &Projector)
{
	Destroy();

	Set_Source(Projector.m_Source);
	Set_Target(Projector.m_Target);

	Set_Inverse(Projector.m_bInverse);

	Set_Precise_Mode(Projector.Get_Precise_Mode());

	return( true );
}

//---------------------------------------------------------
bool CSG_CRSProjector::Destroy(void)
{
	m_bInverse	= false;

	PROJ4_FREE(m_pSource);
	PROJ4_FREE(m_pTarget);
	PROJ4_FREE(m_pGCS   );

	Set_Copies();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_CRSProjector::Set_Copies(int nCopies)
{
	if( m_Copies )
	{
		delete[](m_Copies);

		m_Copies	= NULL;
		m_nCopies	= 0;
	}

	if( nCopies > 1 )
	{
		m_Copies	= new CSG_CRSProjector[m_nCopies = nCopies - 1];

		for(int i=0; i<m_nCopies; i++)
		{
			m_Copies[i].Create(*this);
		}
	}

	return( true );
}

//---------------------------------------------------------
CSG_CRSProjector & CSG_CRSProjector::operator [] (int iCopy)
{
	if( iCopy > 0 && iCopy <= m_nCopies )
	{
		return( m_Copies[iCopy - 1] );
	}

	return( *this );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String CSG_CRSProjector::Get_Version(void)
{
	#if PROJ_VERSION_MAJOR < 6
		return( pj_release );
	#else
		return( CSG_String::Format("%d.%d.%d", PROJ_VERSION_MAJOR, PROJ_VERSION_MINOR, PROJ_VERSION_PATCH) );
	#endif
}

//---------------------------------------------------------
CSG_String CSG_CRSProjector::Get_Description(void)
{
	CSG_String	s;

	s	+= _TL("Projection routines make use of the Proj.4 Cartographic Projections library.");
	s	+= "\n";
	s	+= _TW("Proj.4 was originally developed by Gerald Evenden and later continued by the "
		       "United States Department of the Interior, Geological Survey (USGS).");
	s	+= "\n";
	s	+= _TL("Proj.4 Version is ") + Get_Version();
	s	+= "\n";
	s	+= "<a target=\"_blank\" href=\"http://trac.osgeo.org/proj/\">Proj.4 Homepage</a>";

	return( s );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_CRSProjector::_Set_Projection(const CSG_Projection &Projection, void **ppProjection, bool bInverse)
{
	PROJ4_FREE(*ppProjection);

	//-------------------------------------------------
	#if PROJ_VERSION_MAJOR < 6
	if( (*ppProjection = pj_init_plus(Projection.Get_Proj4())) == NULL )
	{
		CSG_String	Error(pj_strerrno(pj_errno));
	#else
	if( (*ppProjection = proj_create((PJ_CONTEXT *)m_pContext, Projection.Get_Proj4())) == NULL )
	{
		CSG_String	Error(proj_errno_string(proj_errno((PJ *)(*ppProjection))));
	#endif

		SG_UI_Msg_Add_Error(CSG_String::Format("Proj4 [%s]: %s", _TL("initialization"), Error.c_str()));

		return( false );
	}

	//-------------------------------------------------
	#if PROJ_VERSION_MAJOR < 6
	if( bInverse && ((PJ *)(*ppProjection))->inv == NULL )
	#else
	if( bInverse && !proj_pj_info((PJ *)(*ppProjection)).has_inverse )
	#endif
	{
		SG_UI_Msg_Add_Error(CSG_String::Format("Proj4 [%s]: %s", _TL("initialization"), _TL("inverse transformation not available")));

		return( false );
	}

	return( true );
}

//---------------------------------------------------------
bool CSG_CRSProjector::Set_Source(const CSG_Projection &Projection)
{
	return( Projection.is_Okay() && _Set_Projection(Projection, &m_pSource,  true) && m_Source.Create(Projection) );
}

//---------------------------------------------------------
bool CSG_CRSProjector::Set_Target(const CSG_Projection &Projection)
{
	return( Projection.is_Okay() && _Set_Projection(Projection, &m_pTarget, false) && m_Target.Create(Projection) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_CRSProjector::Set_Inverse(bool bOn)
{
	if( m_bInverse == bOn )
	{
		return( true );
	}

	#if PROJ_VERSION_MAJOR < 6
	if( m_pTarget && ((PJ *)m_pTarget)->inv )
	#else
	if( m_pTarget && proj_pj_info((PJ *)m_pTarget).has_inverse )
	#endif
	{
		m_bInverse	= bOn;

		void *pTMP	= m_pSource;
		m_pSource	= m_pTarget;
		m_pTarget	= pTMP;

		return( true );
	}

	SG_UI_Msg_Add_Error(CSG_String::Format("Proj4 [%s]: %s", _TL("initialization"), _TL("inverse transformation not available")));

	return( false );
}

//---------------------------------------------------------
bool CSG_CRSProjector::Set_Precise_Mode(bool bOn)
{
	if( bOn )
	{
		if( m_pGCS == NULL )
		{
			#if PROJ_VERSION_MAJOR < 6
			return( (m_pGCS = pj_init_plus("+proj=longlat +datum=WGS84")) != NULL );
			#else
			return( (m_pGCS = proj_create((PJ_CONTEXT *)m_pContext, "+proj=longlat +datum=WGS84")) != NULL );
			#endif
		}
	}
	else
	{
		PROJ4_FREE(m_pGCS);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_CRSProjector::Get_Projection(double &x, double &y)	const
{
	if( !m_pSource || !m_pTarget )
	{
		return( false );
	}

	#if PROJ_VERSION_MAJOR < 6
	if( pj_is_latlong((PJ *)m_pSource) )
	#else
	if( proj_angular_output((PJ *)m_pSource, PJ_FWD) )
	#endif
	{
		x	*= M_DEG_TO_RAD;
		y	*= M_DEG_TO_RAD;
	}

	#if PROJ_VERSION_MAJOR < 6
	if( m_pGCS )	// precise datum conversion
	{
		if( pj_transform((PJ *)m_pSource, (PJ *)m_pGCS   , 1, 0, &x, &y, NULL) != 0
		||  pj_transform((PJ *)m_pGCS   , (PJ *)m_pTarget, 1, 0, &x, &y, NULL) != 0 )
		{
			return( false );
		}
	}
	else			// direct projection
	{
		if( pj_transform((PJ *)m_pSource, (PJ *)m_pTarget, 1, 0, &x, &y, NULL) != 0 )
		{
			return( false );
		}
	}
	#else
	PJ_COORD	c	= proj_coord(x, y, 0, 0);
	
	c	= proj_trans((PJ *)m_pSource, PJ_INV, c); if( proj_errno((PJ *)m_pSource) ) { proj_errno_reset((PJ *)m_pSource); return( false ); }
	c	= proj_trans((PJ *)m_pTarget, PJ_FWD, c); if( proj_errno((PJ *)m_pTarget) ) { proj_errno_reset((PJ *)m_pTarget); return( false ); }

	x	= c.v[0];
	y	= c.v[1];
	#endif

	#if PROJ_VERSION_MAJOR < 6
	if( pj_is_latlong((PJ *)m_pTarget) )
	#else
	if( proj_angular_output((PJ *)m_pTarget, PJ_FWD) )
	#endif
	{
		x	*= M_RAD_TO_DEG;
		y	*= M_RAD_TO_DEG;
	}

	return( true );
}

//---------------------------------------------------------
bool CSG_CRSProjector::Get_Projection(TSG_Point &Point)	const
{
	return( Get_Projection(Point.x, Point.y) );
}

//---------------------------------------------------------
bool CSG_CRSProjector::Get_Projection(CSG_Point &Point)	const
{
	double	x	= Point.Get_X();
	double	y	= Point.Get_Y();

	if( Get_Projection(x, y) )
	{
		Point.Assign(x, y);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_CRSProjector::Get_Projection(double &x, double &y, double &z)	const
{
	if( !m_pSource || !m_pTarget )
	{
		return( false );
	}

	#if PROJ_VERSION_MAJOR < 6
	if( pj_is_latlong((PJ *)m_pSource) )
	#else
	if( proj_angular_output((PJ *)m_pSource, PJ_FWD) )
	#endif
	{
		x	*= M_DEG_TO_RAD;
		y	*= M_DEG_TO_RAD;
	}

	#if PROJ_VERSION_MAJOR < 6
	if( m_pGCS )	// precise datum conversion
	{
		if( pj_transform((PJ *)m_pSource, (PJ *)m_pGCS   , 1, 0, &x, &y, &z) != 0
		||  pj_transform((PJ *)m_pGCS   , (PJ *)m_pTarget, 1, 0, &x, &y, &z) != 0 )
		{
			return( false );
		}
	}
	else			// direct projection
	{
		if( pj_transform((PJ *)m_pSource, (PJ *)m_pTarget, 1, 0, &x, &y, &z) != 0 )
		{
			return( false );
		}
	}
	#else
	PJ_COORD	c	= proj_coord(x, y, z, 0);
	
	c	= proj_trans((PJ *)m_pSource, PJ_INV, c); if( proj_errno((PJ *)m_pSource) ) { proj_errno_reset((PJ *)m_pSource); return( false ); }
	c	= proj_trans((PJ *)m_pTarget, PJ_FWD, c); if( proj_errno((PJ *)m_pTarget) ) { proj_errno_reset((PJ *)m_pTarget); return( false ); }

	x	= c.v[0];
	y	= c.v[1];
	z	= c.v[2];
	#endif

	#if PROJ_VERSION_MAJOR < 6
	if( pj_is_latlong((PJ *)m_pTarget) )
	#else
	if( proj_angular_output((PJ *)m_pTarget, PJ_FWD) )
	#endif
	{
		x	*= M_RAD_TO_DEG;
		y	*= M_RAD_TO_DEG;
	}

	return( true );
}

//---------------------------------------------------------
bool CSG_CRSProjector::Get_Projection(TSG_Point_Z &Point)	const
{
	return( Get_Projection(Point.x, Point.y, Point.z) );
}

//---------------------------------------------------------
bool CSG_CRSProjector::Get_Projection(CSG_Point_Z &Point)	const
{
	double	x	= Point.Get_X();
	double	y	= Point.Get_Y();
	double	z	= Point.Get_Z();

	if( Get_Projection(x, y, z) )
	{
		Point.Assign(x, y, z);

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
