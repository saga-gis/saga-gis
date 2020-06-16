
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
#include "mat_tools.h"


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
void CSG_Index::_On_Construction(void)
{
	m_nValues	= 0;
	m_Index		= NULL;
	m_bProgress	= false;
}

//---------------------------------------------------------
CSG_Index::~CSG_Index(void)
{
	Destroy();
}

//---------------------------------------------------------
bool CSG_Index::Destroy(void)
{
	if( m_Index )
	{
		SG_Free(m_Index);
	}

	m_nValues	= 0;
	m_Index		= NULL;

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Index::CSG_Index(int nValues, CSG_Index_Compare &Compare)
{
	_On_Construction();

	Create(nValues, Compare);
}

//---------------------------------------------------------
bool CSG_Index::Create(int nValues, CSG_Index_Compare &Compare)
{
	if( _Set_Array(nValues) && _Set_Index(&Compare) )
	{
		return( true );
	}

	Destroy();

	return( false );
}

//---------------------------------------------------------
CSG_Index::CSG_Index(int nValues, CSG_Index_Compare *pCompare)
{
	_On_Construction();

	Create(nValues, pCompare);
}

//---------------------------------------------------------
bool CSG_Index::Create(int nValues, CSG_Index_Compare *pCompare)
{
	if( pCompare && _Set_Array(nValues) && _Set_Index(pCompare) )
	{
		return( true );
	}

	Destroy();

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CSG_Index_Compare_Int : public CSG_Index::CSG_Index_Compare
{
public:
	int	*m_Values;	bool	m_Ascending;

	CSG_Index_Compare_Int(int *Values, bool Ascending) : m_Values(Values), m_Ascending(Ascending) {}

	virtual int			Compare		(const int _a, const int _b)
	{
		int	a	= m_Ascending ? _a : _b;
		int	b	= m_Ascending ? _b : _a;

		return( m_Values[a] - m_Values[b] );
	}
};

//---------------------------------------------------------
CSG_Index::CSG_Index(int nValues, int *Values, bool bAscending)
{
	_On_Construction();

	Create(nValues, Values, bAscending);
}

//---------------------------------------------------------
bool CSG_Index::Create(int nValues, int *Values, bool bAscending)
{
	CSG_Index_Compare_Int	Compare(Values, bAscending);

	return( Create(nValues, &Compare) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CSG_Index_Compare_Double : public CSG_Index::CSG_Index_Compare
{
public:
	double	*m_Values;	bool	m_Ascending;

	CSG_Index_Compare_Double(double *Values, bool Ascending) : m_Values(Values), m_Ascending(Ascending) {}

	virtual int			Compare		(const int _a, const int _b)
	{
		int	a	= m_Ascending ? _a : _b;
		int	b	= m_Ascending ? _b : _a;

		double	d	= m_Values[a] - m_Values[b];

		return( d < 0. ? -1 : d > 0. ? 1 : 0 );
	}
};

//---------------------------------------------------------
CSG_Index::CSG_Index(int nValues, double *Values, bool bAscending)
{
	_On_Construction();

	Create(nValues, Values, bAscending);
}

//---------------------------------------------------------
bool CSG_Index::Create(int nValues, double *Values, bool bAscending)
{
	CSG_Index_Compare_Double	Compare(Values, bAscending);

	return( Create(nValues, &Compare) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CSG_Index_Compare_Function : public CSG_Index::CSG_Index_Compare
{
public:
	TSG_PFNC_Compare	m_Function;

	CSG_Index_Compare_Function(TSG_PFNC_Compare Function) : m_Function(Function) {}

	virtual int			Compare		(const int _a, const int _b)
	{
		return( m_Function(_a, _b) );
	}
};

//---------------------------------------------------------
CSG_Index::CSG_Index(int nValues, TSG_PFNC_Compare fCompare)
{
	_On_Construction();

	Create(nValues, fCompare);
}

//---------------------------------------------------------
bool CSG_Index::Create(int nValues, TSG_PFNC_Compare fCompare)
{
	CSG_Index_Compare_Function	Compare(fCompare);

	return( Create(nValues, &Compare) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_Index::Show_Progress(bool bProgress)
{
	m_bProgress	= bProgress;
}

//---------------------------------------------------------
bool CSG_Index::Add_Entry(int Position)
{
	if( Position < 0 || Position >= m_nValues - 1 )
	{
		return( _Set_Array(m_nValues + 1) );
	}

	if( _Set_Array(m_nValues + 1) )
	{
		for(int i=Position, Value=m_nValues-1; i<m_nValues; i++)
		{
			int	v = m_Index[i]; m_Index[i] = Value; Value = v;
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Index::Del_Entry(int Position)
{
	if( Position < 0 || Position >= m_nValues - 1 )
	{
		return( _Set_Array(m_nValues - 1) );
	}

	int	Value	= m_Index[Position];

	for(int i=Position; i<m_nValues-1; i++)
	{
		m_Index[i]	= m_Index[i + 1];
	}

	m_Index[m_nValues - 1]	= Value;

	return( _Set_Array(m_nValues - 1) );
}

//---------------------------------------------------------
bool CSG_Index::_Set_Array(int nValues)
{
	if( nValues < 1 )
	{
		return( Destroy() );
	}

	if( nValues == m_nValues )
	{
		return( true );
	}

	if( m_nValues > nValues )	// keep current sorting as far as possible...
	{
		for(int i=0, j=nValues; i<nValues && j<m_nValues; i++)
		{
			if( m_Index[i] >= nValues )
			{
				while( m_Index[j] >= nValues )
				{
					j++;

					if( j >= m_nValues )
					{
						return( false ); // this should never happen!
					}
				}

				int	c = m_Index[i]; m_Index[i] = m_Index[j]; m_Index[j] = c;
			}
		}
	}

	int	*Index	= (int *)SG_Realloc(m_Index, nValues * sizeof(int));

	if( !Index )
	{
		return( false );
	}

	m_Index	= Index;

	if( m_nValues < nValues )	// keep current sorting as far as possible...
	{
		for(int i=m_nValues; i<nValues; i++)
		{
			m_Index[i]	= i;
		}
	}

	m_nValues	= nValues;

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define SG_INDEX_SWAP(a, b)	{itemp=(a);(a)=(b);(b)=itemp;}

//---------------------------------------------------------
bool CSG_Index::_Set_Index(CSG_Index_Compare *pCompare)
{
	const int	M	= 7;

	int		indxt, itemp,
			i, j, k, a,
			l		= 0,
			ir		= m_nValues - 1,
			nstack	= 64,
			jstack	= 0;

	//-----------------------------------------------------
	CSG_Array_Int	istack(nstack);

	int	nProcessed	= 0;

	//-----------------------------------------------------
	for(;;)
	{
		if( ir - l < M )
		{
			if( m_bProgress && !SG_UI_Process_Set_Progress((double)(nProcessed += M - 1), (double)m_nValues) )
			{
				SG_UI_Msg_Add_Error(_TL("index creation stopped by user"));

				SG_UI_Process_Set_Ready();

				return( false );
			}

			for(j=l+1; j<=ir; j++)
			{
				a		= indxt	= m_Index[j];

				for(i=j-1; i>=0; i--)
				{
					if( pCompare->Compare(m_Index[i], a) <= 0 )
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

			if( pCompare->Compare(m_Index[l + 1], m_Index[ir]) > 0 )
				SG_INDEX_SWAP    (m_Index[l + 1], m_Index[ir]);

			if( pCompare->Compare(m_Index[l    ], m_Index[ir]) > 0 )
				SG_INDEX_SWAP    (m_Index[l    ], m_Index[ir]);

			if( pCompare->Compare(m_Index[l + 1], m_Index[l ]) > 0 )
				SG_INDEX_SWAP    (m_Index[l + 1], m_Index[l ]);

			i		= l + 1;
			j		= ir;
			a		= indxt	= m_Index[l];

			for(;;)
			{
				do	i++;	while( pCompare->Compare(m_Index[i], a) < 0 );
				do	j--;	while( pCompare->Compare(m_Index[j], a) > 0 );

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
				istack.Set_Array(nstack += 64);
			}

			if( ir - i + 1 >= j - l )
			{
				istack[jstack    ]	= ir;
				istack[jstack - 1]	= i;
				ir					= j - 1;
			}
			else
			{
				istack[jstack    ]	= j - 1;
				istack[jstack - 1]	= l;
				l					= i;
			}
		}
	}

	//-----------------------------------------------------
	if( m_bProgress )
	{
		SG_UI_Process_Set_Ready();
	}

	return( true );
}

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_PriorityQueue::CSG_PriorityQueue(size_t maxSize) : m_Items(NULL), m_nItems(0), m_maxSize(0)
{
	m_pLeaf[0] = m_pLeaf[1] = NULL;

	Create(maxSize);
}

//---------------------------------------------------------
CSG_PriorityQueue::~CSG_PriorityQueue(void)
{
	Destroy();
}

//---------------------------------------------------------
void CSG_PriorityQueue::Create(size_t maxSize)
{
	Destroy();

	if( maxSize > 1 )
	{
		m_maxSize	= maxSize;

		m_Items	= (CSG_PriorityQueueItem **)SG_Malloc(m_maxSize * sizeof(CSG_PriorityQueueItem *));
	}
}

//---------------------------------------------------------
void CSG_PriorityQueue::Destroy(void)
{
	if( m_Items )
	{
		SG_Free(m_Items);

		m_Items	= NULL;
	}

	if( m_pLeaf[0] )
	{
		delete(m_pLeaf[0]);

		m_pLeaf[0]	= NULL;
	}

	if( m_pLeaf[1] )
	{
		delete(m_pLeaf[1]);

		m_pLeaf[1]	= NULL;
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
size_t CSG_PriorityQueue::_Insert_Position(CSG_PriorityQueueItem *pItem)
{
	if( m_nItems == 0 )
	{
		return( 0 );
	}

	size_t	a	= 0;
	size_t	b	= m_nItems - 1;

	if( pItem->Compare(m_Items[a]) < 0 )
	{
		return( a );
	}

	if( pItem->Compare(m_Items[b]) > 0 )
	{
		return( b + 1 );
	}

	for(size_t d=(b-a)/2 ; d>0; d/=2)
	{
		size_t	i	= a + d;

		if( pItem->Compare(m_Items[i]) > 0 )
		{
			a	= a < i ? i : a + 1;
		}
		else
		{
			b	= b > i ? i : b - 1;
		}
	}

	for(size_t i=a; i<=b; i++)
	{
		if( pItem->Compare(m_Items[i]) < 0 )
		{
			return( i );
		}
	}

	return( b );
}

//---------------------------------------------------------
void CSG_PriorityQueue::Add(CSG_PriorityQueueItem *pItem)
{
	if( m_Items && m_nItems < m_maxSize )
	{
		size_t	Position	= _Insert_Position(pItem);

		memmove(m_Items + Position + 1, m_Items + Position, sizeof(CSG_PriorityQueueItem *) * (m_nItems - Position));

		m_Items[Position]	= pItem;
	}
	else
	{
		if( !m_pLeaf[0] )
		{
			size_t	Divide	= m_maxSize / 2;

			m_pLeaf[0]	= new CSG_PriorityQueue(m_maxSize);
			m_pLeaf[1]	= new CSG_PriorityQueue(m_maxSize);

			m_pLeaf[0]->m_nItems	= Divide;
			m_pLeaf[1]->m_nItems	= m_maxSize - Divide;

			memcpy(m_pLeaf[0]->m_Items, m_Items                       , m_pLeaf[0]->m_nItems * sizeof(CSG_PriorityQueueItem *));
			memcpy(m_pLeaf[1]->m_Items, m_Items + m_pLeaf[0]->m_nItems, m_pLeaf[1]->m_nItems * sizeof(CSG_PriorityQueueItem *));

			SG_Free(m_Items);
			m_Items	= NULL;
		}

		if( pItem->Compare(m_pLeaf[1]->Minimum()) > 0 )
		{
			m_pLeaf[1]->Add(pItem);
		}
		else
		{
			m_pLeaf[0]->Add(pItem);
		}
	}

	m_nItems++;
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_PriorityQueue::CSG_PriorityQueueItem * CSG_PriorityQueue::Poll(void)
{
	if( m_nItems > 0 )
	{
		m_nItems--;

		if( m_Items )
		{
			return( m_Items[m_nItems] );
		} // else if( m_pLeaf[0] )

		CSG_PriorityQueueItem	*pItem	= m_pLeaf[1]->Poll();

		if( m_pLeaf[1]->m_nItems == 0 )
		{
			delete(m_pLeaf[1]);

			CSG_PriorityQueue	*pLeaf	= m_pLeaf[0];

		//	m_nItems   = pLeaf->m_nItems;
			m_Items    = pLeaf->m_Items;
			m_pLeaf[0] = pLeaf->m_pLeaf[0];
			m_pLeaf[1] = pLeaf->m_pLeaf[1];

			pLeaf->m_Items    = NULL;
			pLeaf->m_pLeaf[0] = NULL;
			pLeaf->m_pLeaf[1] = NULL;
			delete(pLeaf);
		}

		return( pItem );
	}

	return( NULL );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
