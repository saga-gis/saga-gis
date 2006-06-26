
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
//                    mat_matrix.cpp                     //
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

#include "mat_tools.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void **		MATRIX_Alloc(int ny, int nx, int Value_Size)
{
	char	**Matrix;

	Matrix		= (char **)API_Malloc(ny * sizeof(char *));
	Matrix[0]	= (char  *)API_Malloc(ny * nx * Value_Size);

	nx	*= Value_Size * sizeof(char);

	for(int i=1; i<ny; i++)
	{
		Matrix[i]	= Matrix[0] + i * nx;
	}

	return( (void **)Matrix );
}

//---------------------------------------------------------
void		MATRIX_Free(void **Matrix)
{
	API_Free(Matrix[0]);
	API_Free(Matrix);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool		MATRIX_Invert(int nSize, double **Matrix)
{
	bool	bResult;
	int		i, j, *Index;
	double	*Vector, **Matrix_Tmp;

	bResult	= false;

	if( nSize > 1 )
	{
		Index	= (int *)API_Malloc(nSize * sizeof(int));

		if( MATRIX_LU_Decomposition(nSize, Matrix, Index) )
		{
			Vector			= (double  *)API_Malloc(nSize * sizeof(double));

			Matrix_Tmp		= (double **)API_Malloc(nSize * sizeof(double *));
			Matrix_Tmp[0]	= (double  *)API_Malloc(nSize * nSize * sizeof(double));

			for(j=0; j<nSize; j++)
			{
				Matrix_Tmp[j]	= Matrix_Tmp[0] + j * nSize;
				memcpy(Matrix_Tmp[j], Matrix[j], nSize * sizeof(double));
			}

			//---------------------------------------------
			for(j=0; j<nSize; j++)
			{
				memset(Vector, 0, nSize * sizeof(double));
				Vector[j]	= 1.0;

				MATRIX_Solve(nSize, Matrix_Tmp, Index, Vector);

				for(i=0; i<nSize; i++)
				{
					Matrix[i][j]	= Vector[i];
				}
			}

			//---------------------------------------------
			API_Free(Vector);

			API_Free(Matrix_Tmp[0]);
			API_Free(Matrix_Tmp);

			bResult	= true;
		}

		API_Free(Index);
	}

	return( bResult );
}

//---------------------------------------------------------
bool		MATRIX_LU_Decomposition(int nSize, double **Matrix, int *Index)
{
	int		i, j, k, iMax	= 0;
	double	dMax, d, Sum, *Vector;

	Vector	= (double *)API_Malloc(nSize * sizeof(double));

	for(i=0; i<nSize; i++)
	{
		dMax	= 0.0;

		for(j=0; j<nSize; j++)
		{
			if( (d = fabs(Matrix[i][j])) > dMax )
			{
				dMax	= d;
			}
		}

		if( dMax <= 0.0 )	// singular matrix !!!...
		{
			API_Free(Vector);

			return( false );
		}

		Vector[i]	= 1.0 / dMax;
	}

	for(j=0; j<nSize; j++)
	{
		for(i=0; i<j; i++)
		{
			Sum		= Matrix[i][j];

			for(k=0; k<i; k++)
			{
				Sum		-= Matrix[i][k] * Matrix[k][j];
			}

			Matrix[i][j]	= Sum;
		}

		dMax		= 0.0;

		for(i=j; i<nSize; i++)
		{
			Sum		= Matrix[i][j];

			for(k=0; k<j; k++)
			{
				Sum		-= Matrix[i][k] * Matrix[k][j];
			}

			Matrix[i][j]	= Sum;

			if( (d = Vector[i] * fabs(Sum)) >= dMax )
			{
				dMax	= d;
				iMax	= i;
			}
		}

		if( j != iMax )
		{
			for(k=0; k<nSize; k++)
			{
				d				= Matrix[iMax][k];
				Matrix[iMax][k]	= Matrix[j   ][k];
				Matrix[j   ][k]	= d;
			}

			Vector[iMax]	= Vector[j];
		}

		Index[j]	= iMax;

		if( Matrix[j][j] == 0.0 )
		{
			Matrix[j][j]	= M_TINY;
		}

		if( j != nSize )
		{
			d	= 1.0 / (Matrix[j][j]);

			for(i=j+1; i<nSize; i++)
			{
				Matrix[i][j]	*= d;
			}
		}
	}

	API_Free(Vector);

	return( true );
}

//---------------------------------------------------------
void		MATRIX_Solve(int nSize, double **Matrix, int *Index, double *Vector)
{
	int		i, j, k;
	double	Sum;

	for(i=0, k=-1; i<nSize; i++)
	{
		Sum					= Vector[Index[i]];
		Vector[Index[i]]	= Vector[i];

		if( k >= 0 )
		{
			for(j=k; j<=i-1; j++)
			{
				Sum	-= Matrix[i][j] * Vector[j];
			}
		}
		else if( Sum )
		{
			k	= i;
		}

		Vector[i]	= Sum;
	}

	for(i=nSize-1; i>=0; i--)
	{
		Sum			= Vector[i];

		for(j=i+1; j<nSize; j++)
		{
			Sum			-= Matrix[i][j] * Vector[j];
		}

		Vector[i]	= Sum / Matrix[i][i];
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
