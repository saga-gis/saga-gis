
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
void **		MATRIX_Alloc(int nRows, int nCols, int nValueBytes)
{
	void	**Matrix;

	Matrix		= (void **)API_Malloc(nRows * sizeof(void *));
	Matrix[0]	= (void  *)API_Malloc(nRows * nCols * nValueBytes);

	nCols	*= nValueBytes * sizeof(char);

	for(int iRow=1; iRow<nRows; iRow++)
	{
		Matrix[iRow]	= (char *)Matrix[0] + iRow * nCols;
	}

	return( Matrix );
}

//---------------------------------------------------------
void **		MATRIX_Get_Copy(int nRows, int nCols, int nValueBytes, void **Matrix)
{
	void	**Copy	= MATRIX_Alloc(nRows, nCols, nValueBytes);

	for(int iRow=0; iRow<nRows; iRow++)
	{
		memcpy(Copy[iRow], Matrix[iRow], nCols * nValueBytes);
	}

	return( Copy );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool		MATRIX_Set_Identitiy(int nSize, double **Matrix)
{
	if( nSize > 0 )
	{
		for(int i=0; i<nSize; i++)
		{
			memset(Matrix[i], 0, nSize * sizeof(double));

			Matrix[i][i]	= 1.0;
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool		MATRIX_Add(int nSize, double **A, double **B)
{
	if( nSize > 0 )
	{
		for(int i=0; i<nSize; i++)
		{
			for(int j=0; j<nSize; j++)
			{
				A[i][j]	+= B[j][i];
			}
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool		MATRIX_Multiply(int nSize, double **Matrix, double Scalar)
{
	if( nSize > 0 )
	{
		for(int i=0; i<nSize; i++)
		{
			for(int j=0; j<nSize; j++)
			{
				Matrix[i][j]	*= Scalar;
			}
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool		MATRIX_Multiply(int nSize, double **Matrix, double *Vector)
{
	if( nSize > 0 )
	{
		double	*X	= (double *)API_Calloc(nSize, sizeof(double));

		for(int i=0; i<nSize; i++)
		{
			for(int j=0; j<nSize; j++)
			{
				X[i]	+= Matrix[i][j] * Vector[j];
			}
		}

		memcpy(Vector, X, nSize * sizeof(double));

		API_Free(X);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool		MATRIX_Multiply(int nSize, double **A, double **B)
{
	if( nSize > 0 )
	{
		double	**C	= (double **)MATRIX_Get_Copy(nSize, nSize, sizeof(double), (void **)A);

		for(int i=0; i<nSize; i++)
		{
			memset(A[i], 0, nSize * sizeof(double));

			for(int j=0; j<nSize; j++)
			{
				A[i][j]	+= C[i][j] * B[j][i];
			}
		}

		MATRIX_Free(C);

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
bool		MATRIX_Solve(int nSize, double **Matrix, double *Vector, bool bSilent)
{
	bool	bResult	= false;
	int		*Permutation;

	if( nSize > 1 )
	{
		Permutation	= (int *)API_Malloc(nSize * sizeof(int));

		if( MATRIX_LU_Decomposition(nSize, Matrix, Permutation, bSilent) )
		{
			MATRIX_LU_Solve(nSize, Matrix, Permutation, Vector, bSilent);

			bResult	= true;
		}

		API_Free(Permutation);
	}

	return( bResult );
}

//---------------------------------------------------------
bool		MATRIX_Invert(int nSize, double **Matrix, bool bSilent)
{
	bool	bResult	= false;;
	int		i, j, *Permutation;
	double	*Vector, **Matrix_Tmp;

	if( nSize > 1 )
	{
		Permutation	= (int *)API_Malloc(nSize * sizeof(int));

		if( MATRIX_LU_Decomposition(nSize, Matrix, Permutation, bSilent) )
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
			for(j=0; j<nSize && (bSilent || API_Callback_Process_Set_Progress(i, nSize)); j++)
			{
				memset(Vector, 0, nSize * sizeof(double));
				Vector[j]	= 1.0;

				MATRIX_LU_Solve(nSize, Matrix_Tmp, Permutation, Vector, false);

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

		API_Free(Permutation);
	}

	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool		MATRIX_LU_Decomposition(int nSize, double **Matrix, int *Permutation, bool bSilent)
{
	int		i, j, k, iMax	= 0;
	double	dMax, d, Sum, *Vector;

	Vector	= (double *)API_Malloc(nSize * sizeof(double));

	for(i=0; i<nSize && (bSilent || API_Callback_Process_Set_Progress(i, nSize)); i++)
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

	for(j=0; j<nSize && (bSilent || API_Callback_Process_Set_Progress(j, nSize)); j++)
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

		Permutation[j]	= iMax;

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

	return( bSilent || API_Callback_Process_Get_Okay(false) );
}

//---------------------------------------------------------
void		MATRIX_LU_Solve(int nSize, double **Matrix, int *Permutation, double *Vector, bool bSilent)
{
	int		i, j, k;
	double	Sum;

	for(i=0, k=-1; i<nSize && (bSilent || API_Callback_Process_Set_Progress(i, nSize)); i++)
	{
		Sum						= Vector[Permutation[i]];
		Vector[Permutation[i]]	= Vector[i];

		if( k >= 0 )
		{
			for(j=k; j<=i-1; j++)
			{
				Sum	-= Matrix[i][j] * Vector[j];
			}
		}
		else if( Sum )
		{
			k		= i;
		}

		Vector[i]	= Sum;
	}

	for(i=nSize-1; i>=0 && (bSilent || API_Callback_Process_Set_Progress(nSize-i, nSize)); i--)
	{
		Sum			= Vector[i];

		for(j=i+1; j<nSize; j++)
		{
			Sum		-= Matrix[i][j] * Vector[j];
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
