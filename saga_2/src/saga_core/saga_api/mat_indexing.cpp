
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
#define INDEX_SWAP(a,b)	{itemp=(a);(a)=(b);(b)=itemp;}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void	*MAT_Index_Values;


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int		MAT_Index_Compare_Int		(const int iElement_1, const int iElement_2)
{
	return( ((int *)MAT_Index_Values)[iElement_1] - ((int *)MAT_Index_Values)[iElement_2] );
}

//---------------------------------------------------------
int *	MAT_Create_Index(int nValues, int *Values, bool bAscending)
{
	MAT_Index_Values	= Values;

	return( MAT_Create_Index(nValues, MAT_Index_Compare_Int, bAscending) );
}

//---------------------------------------------------------
bool	MAT_Create_Index(int nValues, int *Values, bool bAscending, int *Index)
{
	MAT_Index_Values	= Values;

	return( MAT_Create_Index(nValues, MAT_Index_Compare_Int, bAscending, Index) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int		MAT_Index_Compare_Double	(const int iElement_1, const int iElement_2)
{
	double	d	= ((double *)MAT_Index_Values)[iElement_1] - ((double *)MAT_Index_Values)[iElement_2];

	return( d < 0.0 ? -1 : (d > 0.0 ? 1 : 0) );
}

//---------------------------------------------------------
int *	MAT_Create_Index(int nValues, double *Values, bool bAscending)
{
	MAT_Index_Values	= Values;

	return( MAT_Create_Index(nValues, MAT_Index_Compare_Double, bAscending) );
}

//---------------------------------------------------------
bool	MAT_Create_Index(int nValues, double *Values, bool bAscending, int *Index)
{
	MAT_Index_Values	= Values;

	return( MAT_Create_Index(nValues, MAT_Index_Compare_Double, bAscending, Index) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int *	MAT_Create_Index(int nValues, int (*Compare)(const int iElement_1, const int iElement_2), bool bAscending)
{
	int		*Index;

	if( nValues > 0 )
	{
		Index	= (int *)API_Malloc(nValues * sizeof(int));

		if( MAT_Create_Index(nValues, Compare, bAscending, Index) )
		{
			return( Index );
		}

		API_Free(Index);
	}

	return( NULL );
}

//---------------------------------------------------------
bool	MAT_Create_Index(int nValues, int (*Compare)(const int iElement_1, const int iElement_2), bool bAscending, int *Index)
{
	const int	M	= 7;

	int		indxt, itemp, *istack,
			i, j, k, a,
			l		= 0,
			ir		= nValues - 1,
			nstack	= 64,
			jstack	= 0;

	//-----------------------------------------------------
	for(j=0; j<nValues; j++)
	{
		Index[j]	= j;
	}

	istack	= (int *)API_Malloc(nstack * sizeof(int));

	//-----------------------------------------------------
	for(;;)
	{
		if( ir - l < M )
		{
			for(j=l+1; j<=ir; j++)
			{
				a		= indxt	= Index[j];

				for(i=j-1; i>=0; i--)
				{
					if( Compare(Index[i], a) <= 0 )
					{
						break;
					}

					Index[i + 1]	= Index[i];
				}

				Index[i + 1]	= indxt;
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
			INDEX_SWAP(Index[k], Index[l + 1]);

			if( Compare   (Index[l + 1], Index[ir]) > 0 )
				INDEX_SWAP(Index[l + 1], Index[ir]);

			if( Compare   (Index[l    ], Index[ir]) > 0 )
				INDEX_SWAP(Index[l    ], Index[ir]);

			if( Compare   (Index[l + 1], Index[l ]) > 0 )
				INDEX_SWAP(Index[l + 1], Index[l ]);

			i		= l + 1;
			j		= ir;
			a		= indxt	= Index[l];

			for(;;)
			{
				do	i++;	while( Compare(Index[i], a) < 0 );
				do	j--;	while( Compare(Index[j], a) > 0 );

				if( j < i )
				{
					break;
				}

				INDEX_SWAP(Index[i], Index[j]);
			}

			Index[l]	= Index[j];
			Index[j]	= indxt;
			jstack		+= 2;

			if( jstack >= nstack )
			{
				nstack	+= 64;
				istack	= (int *)API_Realloc(istack, nstack * sizeof(int));
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
		for(i=0, j=nValues-1; i<j; i++, j--)
		{
			k			= Index[i];
			Index[i]	= Index[j];
			Index[j]	= k;
		}
	}

	API_Free(istack);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
