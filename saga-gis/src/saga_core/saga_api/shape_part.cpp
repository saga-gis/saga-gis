
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
//                    shape_part.cpp                     //
//                                                       //
//          Copyright (C) 2008 by Olaf Conrad            //
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

#include "shapes.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Shape_Part::CSG_Shape_Part(CSG_Shape_Points *pOwner)
{
	m_pOwner	= pOwner;

	m_Points	= NULL;
	m_nPoints	= 0;
	m_nBuffer	= 0;

	m_bUpdate	= true;
}

//---------------------------------------------------------
CSG_Shape_Part::~CSG_Shape_Part(void)
{
	Destroy();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define GET_GROW_SIZE(n)	(n < 128 ? 1 : (n < 2048 ? 32 : 256))

//---------------------------------------------------------
bool CSG_Shape_Part::_Alloc_Memory(int nPoints)
{
	if( m_nPoints != nPoints )
	{
		int		nGrow	= GET_GROW_SIZE(nPoints),
				nBuffer = (nPoints / nGrow) * nGrow;

		while( nBuffer < nPoints )
		{
			nBuffer	+= nGrow;
		}

		if( m_nBuffer != nBuffer )
		{
			m_nBuffer	= nBuffer;

			TSG_Point	*Points	= (TSG_Point *)SG_Realloc(m_Points, m_nBuffer * sizeof(TSG_Point));

			if( Points == NULL )
			{
				return( false );
			}

			m_Points	= Points;
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
bool CSG_Shape_Part::Destroy(void)
{
	if( m_Points != NULL )
	{
		SG_Free(m_Points);
	}

	m_Points	= NULL;
	m_nPoints	= 0;
	m_nBuffer	= 0;

	m_bUpdate	= true;

	_Invalidate();

	return( true );
}

//---------------------------------------------------------
bool CSG_Shape_Part::Assign(CSG_Shape_Part *pPart)
{
	if( _Alloc_Memory(pPart->Get_Count()) )
	{
		memcpy(m_Points, pPart->m_Points, pPart->m_nPoints * sizeof(TSG_Point));
 
		m_Extent	= pPart->m_Extent;
		m_bUpdate	= pPart->m_bUpdate;

		if( m_pOwner )
		{
			m_pOwner->_Invalidate();
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
int CSG_Shape_Part::Add_Point(double x, double y)
{
	return( Ins_Point(x, y, m_nPoints) );
}

//---------------------------------------------------------
int CSG_Shape_Part::Ins_Point(double x, double y, int iPoint)
{
	if( iPoint >= 0 && iPoint <= m_nPoints && _Alloc_Memory(m_nPoints + 1) )
	{
		for(int i=m_nPoints; i>iPoint; i--)
		{
			m_Points[i]	= m_Points[i - 1];
		}

		m_nPoints++;

		m_Points[iPoint].x	= x;
		m_Points[iPoint].y	= y;

		_Invalidate();

		return( m_nPoints );
	}

	return( 0 );
}

//---------------------------------------------------------
int CSG_Shape_Part::Set_Point(double x, double y, int iPoint)
{
	if( iPoint >= 0 && iPoint < m_nPoints )
	{
		m_Points[iPoint].x	= x;
		m_Points[iPoint].y	= y;

		_Invalidate();

		return( 1 );
	}

	return( 0 );
}

//---------------------------------------------------------
int CSG_Shape_Part::Del_Point(int del_Point)
{
	if( del_Point >= 0 && del_Point < m_nPoints )
	{
		m_nPoints--;

		for(int iPoint=del_Point; iPoint<m_nPoints; iPoint++)
		{
			m_Points[iPoint]	= m_Points[iPoint + 1];
		}

		_Alloc_Memory(m_nPoints);

		_Invalidate();

		return( 1 );
	}

	return( 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline void CSG_Shape_Part::_Invalidate(void)
{
	m_bUpdate	= true;

	if( m_pOwner )
	{
		m_pOwner->_Invalidate();
	}
}

//---------------------------------------------------------
void CSG_Shape_Part::_Update_Extent(void)
{
	if( m_bUpdate )
	{
		if( m_nPoints > 0 )
		{
			int			i;
			TSG_Point	*p	= m_Points;
			TSG_Rect	*r	= &m_Extent.m_rect;

			r->xMin	= r->xMax	= p->x;
			r->yMin	= r->yMax	= p->y;

			for(i=1, p++; i<m_nPoints; i++, p++)
			{
				if( r->xMin > p->x )
				{
					r->xMin	= p->x;
				}
				else if( r->xMax < p->x )
				{
					r->xMax	= p->x;
				}

				if( r->yMin > p->y )
				{
					r->yMin	= p->y;
				}
				else if( r->yMax < p->y )
				{
					r->yMax	= p->y;
				}
			}
		}

		m_bUpdate	= false;
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Shape_Part_Z::CSG_Shape_Part_Z(CSG_Shape_Points *pOwner)
	: CSG_Shape_Part(pOwner)
{
	m_Z	= NULL;
}

//---------------------------------------------------------
CSG_Shape_Part_Z::~CSG_Shape_Part_Z(void)
{
	Destroy();
}

//---------------------------------------------------------
bool CSG_Shape_Part_Z::Destroy(void)
{
	if( m_Z != NULL )
	{
		SG_Free(m_Z);
	}

	m_Z	= NULL;

	return( CSG_Shape_Part::Destroy() );
}

//---------------------------------------------------------
bool CSG_Shape_Part_Z::_Alloc_Memory(int nPoints)
{
	if( CSG_Shape_Part::_Alloc_Memory(nPoints) )
	{
		double	*Z	= (double *)SG_Realloc(m_Z, m_nBuffer * sizeof(double));

		if( Z )
		{
			m_Z	= Z;

			return( true );
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Shape_Part_Z::Assign(CSG_Shape_Part *pPart)
{
	if( CSG_Shape_Part::Assign(pPart) )
	{
		if( ((CSG_Shapes *)pPart->Get_Owner()->Get_Table())->Get_Vertex_Type() == SG_VERTEX_TYPE_XYZ
		||	((CSG_Shapes *)pPart->Get_Owner()->Get_Table())->Get_Vertex_Type() == SG_VERTEX_TYPE_XYZM )
		{
			memcpy(m_Z, ((CSG_Shape_Part_Z *)pPart)->m_Z, pPart->Get_Count() * sizeof(double));
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
void CSG_Shape_Part_Z::_Update_Extent(void)
{
	if( m_bUpdate )
	{
		if( m_nPoints > 0 )
		{
			m_ZMin	= m_ZMax	= m_Z[0];

			for(int i=1; i<m_nPoints; i++)
			{
				if( m_ZMin > m_Z[i] )
				{
					m_ZMin	= m_Z[i];
				}
				else if( m_ZMax < m_Z[i] )
				{
					m_ZMax	= m_Z[i];
				}
			}
		}

		CSG_Shape_Part::_Update_Extent();
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Shape_Part_ZM::CSG_Shape_Part_ZM(CSG_Shape_Points *pOwner)
	: CSG_Shape_Part_Z(pOwner)
{
	m_M	= NULL;
}

//---------------------------------------------------------
CSG_Shape_Part_ZM::~CSG_Shape_Part_ZM(void)
{
	Destroy();
}

//---------------------------------------------------------
bool CSG_Shape_Part_ZM::Destroy(void)
{
	if( m_M != NULL )
	{
		SG_Free(m_M);
	}

	m_M	= NULL;

	return( CSG_Shape_Part_Z::Destroy() );
}

//---------------------------------------------------------
bool CSG_Shape_Part_ZM::_Alloc_Memory(int nPoints)
{
	if( CSG_Shape_Part_Z::_Alloc_Memory(nPoints) )
	{
		double	*M	= (double *)SG_Realloc(m_M, m_nBuffer * sizeof(double));

		if( M )
		{
			m_M	= M;

			return( true );
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Shape_Part_ZM::Assign(CSG_Shape_Part *pPart)
{
	if( CSG_Shape_Part_Z::Assign(pPart) )
	{
		if( ((CSG_Shapes *)pPart->Get_Owner()->Get_Table())->Get_Vertex_Type() == SG_VERTEX_TYPE_XYZM )
		{
			memcpy(m_M, ((CSG_Shape_Part_ZM *)pPart)->m_M, pPart->Get_Count() * sizeof(double));
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
void CSG_Shape_Part_ZM::_Update_Extent(void)
{
	if( m_bUpdate )
	{
		if( m_nPoints > 0 )
		{
			m_ZMin	= m_ZMax	= m_Z[0];

			for(int i=1; i<m_nPoints; i++)
			{
				if( m_ZMin > m_Z[i] )
				{
					m_ZMin	= m_Z[i];
				}
				else if( m_ZMax < m_Z[i] )
				{
					m_ZMax	= m_Z[i];
				}
			}
		}

		CSG_Shape_Part_Z::_Update_Extent();
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
