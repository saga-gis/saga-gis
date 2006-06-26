
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//           Application Programming Interface           //
//                                                       //
//                  Library: SAGA_API                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                     api_core.cpp                      //
//                                                       //
//          Copyright (C) 2005 by Olaf Conrad            //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'.                              //
//                                                       //
// This library is free software; you can redistribute   //
// it and/or modify it under the terms of the GNU Lesser //
// General Public License as published by the Free       //
// Software Foundation, version 2.1 of the License.      //
//                                                       //
// This library is distributed in the hope that it will  //
// be useful, but WITHOUT ANY WARRANTY; without even the //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU Lesser General Public //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU Lesser     //
// General Public License along with this program; if    //
// not, write to the Free Software Foundation, Inc.,     //
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
// USA.                                                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <memory.h>

#include "api_core.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CAPI_iPoints::CAPI_iPoints(void)
{
	m_nPoints	= 0;
	m_Points	= NULL;
}

//---------------------------------------------------------
CAPI_iPoints::~CAPI_iPoints(void)
{
	Clear();
}

//---------------------------------------------------------
void CAPI_iPoints::Clear(void)
{
	if( m_Points )
	{
		API_Free(m_Points);
	}

	m_nPoints	= 0;
	m_Points	= NULL;
}

//---------------------------------------------------------
bool CAPI_iPoints::Assign(const CAPI_iPoints &Points)
{
	Set_Count(Points.m_nPoints);

	if( m_nPoints > 0 )
	{
		memcpy(m_Points, Points.m_Points, m_nPoints * sizeof(TAPI_iPoint));
	}

	return( true );
}

//---------------------------------------------------------
CAPI_iPoints & CAPI_iPoints::operator  = (const CAPI_iPoints &Points)
{
	Assign(Points);

	return( *this );
}

//---------------------------------------------------------
bool CAPI_iPoints::Set_Count(int nPoints)
{
	m_nPoints	= nPoints;
	m_Points	= (TAPI_iPoint *)API_Realloc(m_Points, m_nPoints * sizeof(TAPI_iPoint));

	return( true );
}

//---------------------------------------------------------
bool CAPI_iPoints::Add(int x, int y)
{
	m_Points	= (TAPI_iPoint *)API_Realloc(m_Points, (m_nPoints + 1) * sizeof(TAPI_iPoint));
	m_Points[m_nPoints].x	= x;
	m_Points[m_nPoints].y	= y;
	m_nPoints++;

	return( true );
}

//---------------------------------------------------------
bool CAPI_iPoints::Add(const TAPI_iPoint &Point)
{
	return( Add(Point.x, Point.y) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CAPI_dPoints::CAPI_dPoints(void)
{
	m_nPoints	= 0;
	m_Points	= NULL;
}

//---------------------------------------------------------
CAPI_dPoints::~CAPI_dPoints(void)
{
	Clear();
}

//---------------------------------------------------------
void CAPI_dPoints::Clear(void)
{
	if( m_Points )
	{
		API_Free(m_Points);
	}

	m_nPoints	= 0;
	m_Points	= NULL;
}

//---------------------------------------------------------
bool CAPI_dPoints::Assign(const CAPI_dPoints &Points)
{
	Set_Count(Points.m_nPoints);

	if( m_nPoints > 0 )
	{
		memcpy(m_Points, Points.m_Points, m_nPoints * sizeof(TAPI_dPoint));
	}

	return( true );
}

//---------------------------------------------------------
CAPI_dPoints & CAPI_dPoints::operator  = (const CAPI_dPoints &Points)
{
	Assign(Points);

	return( *this );
}

//---------------------------------------------------------
bool CAPI_dPoints::Set_Count(int nPoints)
{
	m_nPoints	= nPoints;
	m_Points	= (TAPI_dPoint *)API_Realloc(m_Points, m_nPoints * sizeof(TAPI_dPoint));

	return( true );
}

//---------------------------------------------------------
bool CAPI_dPoints::Add(double x, double y)
{
	m_Points	= (TAPI_dPoint *)API_Realloc(m_Points, (m_nPoints + 1) * sizeof(TAPI_dPoint));
	m_Points[m_nPoints].x	= x;
	m_Points[m_nPoints].y	= y;
	m_nPoints++;

	return( true );
}

//---------------------------------------------------------
bool CAPI_dPoints::Add(const TAPI_dPoint &Point)
{
	return( Add(Point.x, Point.y) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CAPI_3D_iPoints::CAPI_3D_iPoints(void)
{
	m_nPoints	= 0;
	m_Points	= NULL;
}

//---------------------------------------------------------
CAPI_3D_iPoints::~CAPI_3D_iPoints(void)
{
	Clear();
}

//---------------------------------------------------------
void CAPI_3D_iPoints::Clear(void)
{
	if( m_Points )
	{
		API_Free(m_Points);
	}

	m_nPoints	= 0;
	m_Points	= NULL;
}

//---------------------------------------------------------
bool CAPI_3D_iPoints::Assign(const CAPI_3D_iPoints &Points)
{
	Set_Count(Points.m_nPoints);

	if( m_nPoints > 0 )
	{
		memcpy(m_Points, Points.m_Points, m_nPoints * sizeof(TAPI_3D_iPoint));
	}

	return( true );
}

//---------------------------------------------------------
CAPI_3D_iPoints & CAPI_3D_iPoints::operator  = (const CAPI_3D_iPoints &Points)
{
	Assign(Points);

	return( *this );
}

//---------------------------------------------------------
bool CAPI_3D_iPoints::Set_Count(int nPoints)
{
	m_nPoints	= nPoints;
	m_Points	= (TAPI_3D_iPoint *)API_Realloc(m_Points, m_nPoints * sizeof(TAPI_3D_iPoint));

	return( true );
}

//---------------------------------------------------------
bool CAPI_3D_iPoints::Add(int x, int y, int z)
{
	m_Points	= (TAPI_3D_iPoint *)API_Realloc(m_Points, (m_nPoints + 1) * sizeof(TAPI_3D_iPoint));
	m_Points[m_nPoints].x	= x;
	m_Points[m_nPoints].y	= y;
	m_Points[m_nPoints].z	= z;
	m_nPoints++;

	return( true );
}

//---------------------------------------------------------
bool CAPI_3D_iPoints::Add(const TAPI_3D_iPoint &Point)
{
	return( Add(Point.x, Point.y, Point.z) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CAPI_3D_dPoints::CAPI_3D_dPoints(void)
{
	m_nPoints	= 0;
	m_Points	= NULL;
}

//---------------------------------------------------------
CAPI_3D_dPoints::~CAPI_3D_dPoints(void)
{
	Clear();
}

//---------------------------------------------------------
void CAPI_3D_dPoints::Clear(void)
{
	if( m_Points )
	{
		API_Free(m_Points);
	}

	m_nPoints	= 0;
	m_Points	= NULL;
}

//---------------------------------------------------------
bool CAPI_3D_dPoints::Assign(const CAPI_3D_dPoints &Points)
{
	Set_Count(Points.m_nPoints);

	if( m_nPoints > 0 )
	{
		memcpy(m_Points, Points.m_Points, m_nPoints * sizeof(TAPI_3D_dPoint));
	}

	return( true );
}

//---------------------------------------------------------
CAPI_3D_dPoints & CAPI_3D_dPoints::operator  = (const CAPI_3D_dPoints &Points)
{
	Assign(Points);

	return( *this );
}

//---------------------------------------------------------
bool CAPI_3D_dPoints::Set_Count(int nPoints)
{
	m_nPoints	= nPoints;
	m_Points	= (TAPI_3D_dPoint *)API_Realloc(m_Points, m_nPoints * sizeof(TAPI_3D_dPoint));

	return( true );
}

//---------------------------------------------------------
bool CAPI_3D_dPoints::Add(double x, double y, double z)
{
	m_Points	= (TAPI_3D_dPoint *)API_Realloc(m_Points, (m_nPoints + 1) * sizeof(TAPI_3D_dPoint));
	m_Points[m_nPoints].x	= x;
	m_Points[m_nPoints].y	= y;
	m_Points[m_nPoints].z	= z;
	m_nPoints++;

	return( true );
}

//---------------------------------------------------------
bool CAPI_3D_dPoints::Add(const TAPI_3D_dPoint &Point)
{
	return( Add(Point.x, Point.y, Point.z) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
