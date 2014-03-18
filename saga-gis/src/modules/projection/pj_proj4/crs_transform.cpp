/**********************************************************
 * Version $Id: crs_transform.cpp 1921 2014-01-09 10:24:11Z oconrad $
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
#include "crs_transform.h"

#include <projects.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define PROJ4_FREE(p)	if( p )	{	pj_free((PJ *)p);	p	= NULL;	}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_CRSProjector::CSG_CRSProjector(void)
{
	m_bInverse	= false;

	m_pSource	= NULL;
	m_pTarget	= NULL;
	m_pGCS		= NULL;
}

//---------------------------------------------------------
CSG_CRSProjector::~CSG_CRSProjector(void)
{
	Destroy();
}

//---------------------------------------------------------
bool CSG_CRSProjector::Destroy(void)
{
	m_bInverse	= false;

	PROJ4_FREE(m_pSource);
	PROJ4_FREE(m_pTarget);
	PROJ4_FREE(m_pGCS);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String CSG_CRSProjector::Get_Version(void)
{
	return( pj_release );
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
	if( (*ppProjection = pj_init_plus(Projection.Get_Proj4())) == NULL )
	{
		SG_UI_Msg_Add_Error(CSG_String::Format(SG_T("Proj4 [%s]: %s"), _TL("initialization"), SG_STR_MBTOSG(pj_strerrno(pj_errno))));

		return( false );
	}

	//-------------------------------------------------
	if( bInverse && ((PJ *)(*ppProjection))->inv == NULL )
	{
		SG_UI_Msg_Add_Error(CSG_String::Format(SG_T("Proj4 [%s]: %s"), _TL("initialization"), _TL("inverse transformation not available")));

		return( false );
	}

	return( true );
}

//---------------------------------------------------------
bool CSG_CRSProjector::Set_Source(const CSG_Projection &Projection)
{
	SG_UI_Msg_Add_Execution(CSG_String::Format(SG_T("\n%s: %s"), _TL("source"), Projection.Get_Proj4().c_str()), false);

	return( _Set_Projection(Projection, &m_pSource,  true) && m_Source.Create(Projection) );
}

//---------------------------------------------------------
bool CSG_CRSProjector::Set_Target(const CSG_Projection &Projection)
{
	SG_UI_Msg_Add_Execution(CSG_String::Format(SG_T("\n%s: %s"), _TL("target"), Projection.Get_Proj4().c_str()), false);

	return( _Set_Projection(Projection, &m_pTarget, false) && m_Target.Create(Projection) );
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

	if( m_pTarget && ((PJ *)m_pTarget)->inv )
	{
		m_bInverse	= bOn;

		void *pTMP	= m_pSource;
		m_pSource	= m_pTarget;
		m_pTarget	= pTMP;

		return( true );
	}

	SG_UI_Msg_Add_Error(CSG_String::Format(SG_T("Proj4 [%s]: %s"), _TL("initialization"), _TL("inverse transformation not available")));

	return( false );
}

//---------------------------------------------------------
bool CSG_CRSProjector::Set_Precise_Mode(bool bOn)
{
	if( bOn )
	{
		if( m_pGCS == NULL )
		{
			return( (m_pGCS = pj_init_plus("+proj=longlat +datum=WGS84")) != NULL );
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

	if( pj_is_latlong((PJ *)m_pSource) )
	{
		x	*= DEG_TO_RAD;
		y	*= DEG_TO_RAD;
	}

	if( m_pGCS )	// precise datum conversion
	{
		if( pj_transform((PJ *)m_pSource, (PJ *)m_pGCS   , 1, 0, &x, &y, NULL) != 0
		||  pj_transform((PJ *)m_pGCS   , (PJ *)m_pTarget, 1, 0, &x, &y, NULL) != 0 )
		{
			return( false );
		}
	}
	else				// direct projection
	{
		if( pj_transform((PJ *)m_pSource, (PJ *)m_pTarget, 1, 0, &x, &y, NULL) != 0 )
		{
			return( false );
		}
	}

	if( pj_is_latlong((PJ *)m_pTarget) )
	{
		x	*= RAD_TO_DEG;
		y	*= RAD_TO_DEG;
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
