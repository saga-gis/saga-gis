
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
//                   mat_indexing.cpp                    //
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
#include "mat_tools.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define SG_INDEX_COMPARE_INT		0
#define SG_INDEX_COMPARE_DOUBLE		1
#define SG_INDEX_COMPARE_FUNCTION	2


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Index::CSG_Index(void)
{
	_On_Construction();
}

//---------------------------------------------------------
CSG_Index::CSG_Index(int nValues, int *Values, bool bAscending)
{
	_On_Construction();

	Create(nValues, Values, bAscending);
}

bool CSG_Index::Create(int nValues, int *Values, bool bAscending)
{
	m_iCompare	= SG_INDEX_COMPARE_INT;
	m_Values	= Values;

	if( _Set_Array(nValues) && _Set_Index(bAscending) )
	{
		return( true );
	}

	Destroy();

	return( false );
}

//---------------------------------------------------------
CSG_Index::CSG_Index(int nValues, double *Values, bool bAscending)
{
	_On_Construction();

	Create(nValues, Values, bAscending);
}

bool CSG_Index::Create(int nValues, double *Values, bool bAscending)
{
	m_iCompare	= SG_INDEX_COMPARE_DOUBLE;
	m_Values	= Values;

	if( _Set_Array(nValues) && _Set_Index(bAscending) )
	{
		return( true );
	}

	Destroy();

	return( false );
}

//---------------------------------------------------------
CSG_Index::CSG_Index(int nValues, TSG_PFNC_Compare fCompare, bool bAscending)
{
	_On_Construction();

	Create(nValues, fCompare, bAscending);
}

bool CSG_Index::Create(int nValues, TSG_PFNC_Compare fCompare, bool bAscending)
{
	m_iCompare	= SG_INDEX_COMPARE_FUNCTION;
	m_fCompare	= fCompare;

	if( _Set_Array(nValues) && _Set_Index(bAscending) )
	{
		return( true );
	}

	Destroy();

	return( false );
}

//---------------------------------------------------------
CSG_Index::~CSG_Index(void)
{
	Destroy();
}

bool CSG_Index::Destroy(void)
{
	if( m_Values )
	{
		SG_Free(m_Values);
	}

	_On_Construction();

	return( true );
}

//---------------------------------------------------------
void CSG_Index::_On_Construction(void)
{
	m_nValues	= 0;
	m_Values	= NULL;
}

//---------------------------------------------------------
bool CSG_Index::_Set_Array(int nValues)
{
	if( nValues > 0 )
	{
		if( nValues != m_nValues )
		{
			m_nValues	= nValues;
			m_Values	= SG_Realloc(m_Values, m_nValues * sizeof(int));
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
inline int CSG_Index::_Compare(const int iElement_1, const int iElement_2)
{
	double	d;

	switch( m_iCompare )
	{
	case SG_INDEX_COMPARE_INT:
		return(  ((int *)m_Values)[iElement_1] - ((int    *)m_Values)[iElement_2] );

	case SG_INDEX_COMPARE_DOUBLE:
		d	= ((double *)m_Values)[iElement_1] - ((double *)m_Values)[iElement_2];

		return( d < 0.0 ? -1 : (d > 0.0 ? 1 : 0) );

	case SG_INDEX_COMPARE_FUNCTION:
		return( m_fCompare(iElement_1, iElement_2) );
	}

	return( 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define SG_INDEX_SWAP(a, b)	{itemp=(a);(a)=(b);(b)=itemp;}

//---------------------------------------------------------
bool CSG_Index::_Set_Index(bool bAscending)
{
	const int	M	= 7;

	int		indxt, itemp, *istack,
			i, j, k, a,
			l		= 0,
			ir		= m_nValues - 1,
			nstack	= 64,
			jstack	= 0;

	//-----------------------------------------------------
	for(j=0; j<m_nValues; j++)
	{
		m_Index[j]	= j;
	}

	istack	= (int *)SG_Malloc(nstack * sizeof(int));

	//-----------------------------------------------------
	for(;;)
	{
		if( ir - l < M )
		{
			for(j=l+1; j<=ir; j++)
			{
				a		= indxt	= m_Index[j];

				for(i=j-1; i>=0; i--)
				{
					if( _Compare(m_Index[i], a) <= 0 )
					{
						break;
					}

					m_Index[i + 1]	= m_Index[i];
				}

				m_Index[i + 1]	= indxt;
			}

			if( jstack == 0 )
			{
				break;
			}

			ir		= istack[jstack--];
			l		= istack[jstack--];
		}
		else
		{
			k		= (l + ir) >> 1;
			SG_INDEX_SWAP(m_Index[k], m_Index[l + 1]);

			if( _Compare     (m_Index[l + 1], m_Index[ir]) > 0 )
				SG_INDEX_SWAP(m_Index[l + 1], m_Index[ir]);

			if( _Compare     (m_Index[l    ], m_Index[ir]) > 0 )
				SG_INDEX_SWAP(m_Index[l    ], m_Index[ir]);

			if( _Compare     (m_Index[l + 1], m_Index[l ]) > 0 )
				SG_INDEX_SWAP(m_Index[l + 1], m_Index[l ]);

			i		= l + 1;
			j		= ir;
			a		= indxt	= m_Index[l];

			for(;;)
			{
				do	i++;	while( _Compare(m_Index[i], a) < 0 );
				do	j--;	while( _Compare(m_Index[j], a) > 0 );

				if( j < i )
				{
					break;
				}

				SG_INDEX_SWAP(m_Index[i], m_Index[j]);
			}

			m_Index[l]	= m_Index[j];
			m_Index[j]	= indxt;
			jstack		+= 2;

			if( jstack >= nstack )
			{
				nstack	+= 64;
				istack	= (int *)SG_Realloc(istack, nstack * sizeof(int));
			}

			if( ir - i + 1 >= j - l )
			{
				istack[jstack]		= ir;
				istack[jstack - 1]	= i;
				ir					= j - 1;
			}
			else
			{
				istack[jstack]		= j - 1;
				istack[jstack - 1]	= l;
				l					= i;
			}
		}
	}

	//-----------------------------------------------------
	if( !bAscending )
	{
		for(i=0, j=m_nValues-1; i<j; i++, j--)
		{
			k			= m_Index[i];
			m_Index[i]	= m_Index[j];
			m_Index[j]	= k;
		}
	}

	SG_Free(istack);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
