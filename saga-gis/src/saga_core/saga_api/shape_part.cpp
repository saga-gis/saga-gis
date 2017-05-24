/**********************************************************
 * Version $Id$
 *********************************************************/

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
// Software Foundation, either version 2.1 of the        //
// License, or (at your option) any later version.       //
//                                                       //
// This library is distributed in the hope that it will  //
// be useful, but WITHOUT ANY WARRANTY; without even the //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU Lesser General Public //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU Lesser     //
// General Public License along with this program; if    //
// not, see <http://www.gnu.org/licenses/>.              //
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
	m_Z			= NULL;
	m_M			= NULL;
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
	if( m_nPoints == nPoints )
	{
		return( true );
	}

	int	nGrow	= GET_GROW_SIZE(nPoints);
	int	nBuffer = (nPoints / nGrow) * nGrow;

	while( nBuffer < nPoints )
	{
		nBuffer	+= nGrow;
	}

	if( m_nBuffer == nBuffer )
	{
		return( true );
	}

	m_nBuffer	= nBuffer;

	//-----------------------------------------------------
	TSG_Point	*Points	= (TSG_Point *)SG_Realloc(m_Points, m_nBuffer * sizeof(TSG_Point));

	if( Points == NULL )
	{
		return( false );
	}

	m_Points	= Points;

	//-----------------------------------------------------
	if( m_Z || ((CSG_Shapes *)m_pOwner->Get_Table())->Get_Vertex_Type() != SG_VERTEX_TYPE_XY )
	{
		double	*Z	= (double *)SG_Realloc(m_Z, m_nBuffer * sizeof(double));

		if( !Z )
		{
			return( false );
		}

		m_Z	= Z;
	}

	//-----------------------------------------------------
	if( m_M || ((CSG_Shapes *)m_pOwner->Get_Table())->Get_Vertex_Type() == SG_VERTEX_TYPE_XYZM )
	{
		double	*M	= (double *)SG_Realloc(m_M, m_nBuffer * sizeof(double));

		if( !M )
		{
			return( false );
		}

		m_M	= M;
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

	if( m_Z != NULL )
	{
		SG_Free(m_Z);
	}

	if( m_M != NULL )
	{
		SG_Free(m_M);
	}

	m_Points	= NULL;
	m_Z			= NULL;
	m_M			= NULL;
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
		memcpy(m_Points, pPart->m_Points, m_nPoints * sizeof(TSG_Point));
 
		if( m_Z && pPart->m_Z )
		{
			memcpy(m_Z, pPart->m_Z, m_nPoints * sizeof(double));
		}

		if( m_M && pPart->m_M )
		{
			memcpy(m_M, pPart->m_M, m_nPoints * sizeof(double));
		}

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

			if( m_Z )
			{
				m_Z[i]	= m_Z[i - 1];

				if( m_M )
				{
					m_M[i]	= m_M[i - 1];
				}
			}
		}

		m_nPoints++;

		m_Points[iPoint].x	= x;
		m_Points[iPoint].y	= y;

		if( m_Z )
		{
			m_Z[iPoint]	= 0.0;

			if( m_M )
			{
				m_M[iPoint]	= 0.0;
			}
		}

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
int CSG_Shape_Part::Del_Point(int iPoint)
{
	if( iPoint >= 0 && iPoint < m_nPoints )
	{
		m_nPoints--;

		for(int i=iPoint; i<m_nPoints; i++)
		{
			m_Points[i]	= m_Points[i + 1];

			if( m_Z )
			{
				m_Z[i]	= m_Z[i + 1];

				if( m_M )
				{
					m_M[i]	= m_M[i + 1];
				}
			}
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
bool CSG_Shape_Part::Revert_Points(void)
{
	for(int i=0, j=m_nPoints-1; i<j; i++, j--)
	{
		TSG_Point	p	= m_Points[i];
		m_Points[i]		= m_Points[j];
		m_Points[j]		= p;

		if( m_Z )
		{
			double	d;
					
			d		= m_Z[i];
			m_Z[i]	= m_Z[j];
			m_Z[j]	= d;

			if( m_M )
			{
				d		= m_M[i];
				m_M[i]	= m_M[j];
				m_M[j]	= d;
			}
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
	if( !m_bUpdate )
	{
		return;
	}

	//-----------------------------------------------------
	int						i;
	TSG_Point				*p;
	CSG_Simple_Statistics	x, y, z, m;

	for(i=0, p=m_Points; i<m_nPoints; i++, p++)
	{
		x.Add_Value(p->x);
		y.Add_Value(p->y);

		if( m_Z )
		{
			z.Add_Value(m_Z[i]);

			if( m_M )
			{
				m.Add_Value(m_M[i]);
			}
		}
	}

	m_Extent.Assign(x.Get_Minimum(), y.Get_Minimum(), x.Get_Maximum(), y.Get_Maximum());

	m_ZMin		= z.Get_Minimum();
	m_ZMax		= z.Get_Maximum();

	m_MMin		= m.Get_Minimum();
	m_MMax		= m.Get_Maximum();

	m_bUpdate	= false;
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
