
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                  Geostatistics_Grid                   //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                     grid_pca.cpp                      //
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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
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
#include "grid_pca.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_PCA::CGrid_PCA(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Principle Components Analysis"));

	Set_Author		(SG_T("O.Conrad (c) 2010"));

	Set_Description	(_TW(
		""
	));

	//-----------------------------------------------------
	Parameters.Add_Grid_List(
		NULL, "GRIDS"		, _TL("Grids"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid_List(
		NULL, "PCA"			, _TL("Principle Components"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice(
		NULL, "METHOD"		, _TL("Method"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|"),
			_TL("correlation matrix"),
			_TL("variance-covariance matrix"),
			_TL("sums-of-squares-and-cross-products matrix")
		), 0
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_PCA::On_Execute(void)
{
	bool	bResult	= true;

	//-----------------------------------------------------
	m_pGrids	= Parameters("GRIDS")	->asGridList();
	m_pPCA		= Parameters("PCA")		->asGridList();
	m_Method	= Parameters("METHOD")	->asInt();

	//-----------------------------------------------------
	if( !Get_Matrix() )
	{
		Error_Set(_TL("matrix initialisation failed"));

		bResult	= false;
	}

	else if( !Get_Reduction() )
	{
		Error_Set(_TL("Eigen reduction failed"));

		bResult	= false;
	}

	//-----------------------------------------------------
	m_SymMat.Destroy();

	//-----------------------------------------------------
	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define SIGN(a, b)	((b) < 0 ? -fabs(a) : fabs(a))
#define EPSILON		0.005


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline int CGrid_PCA::Get_NGrids(void)
{
	return( m_pGrids->Get_Count() );
}

//---------------------------------------------------------
inline double CGrid_PCA::Get_Value(int iFeature, int iElement)
{
	CSG_Grid	*pGrid	= m_pGrids->asGrid(iFeature);

	switch( m_Method )
	{
	default:
	case 0:	// Correlation matrix: Center and reduce the column vectors.
		return( (pGrid->asDouble(iElement) - pGrid->Get_ArithMean()) / (sqrt(Get_NCells() * pGrid->Get_Variance())) );

	case 1:	// Variance-covariance matrix: Center the column vectors.
		return( (pGrid->asDouble(iElement) - pGrid->Get_ArithMean()) );

	case 2:	// Sums-of-squares-and-cross-products matrix
		return( (pGrid->asDouble(iElement)) );
	}
}

//---------------------------------------------------------
inline double CGrid_PCA::Get_Mean(int iFeature)
{
	return( m_pGrids->asGrid(iFeature)->Get_ArithMean() );
}

//---------------------------------------------------------
inline double CGrid_PCA::Get_StdDev(int iFeature)
{
	return( sqrt((double)Get_NCells()) * m_pGrids->asGrid(iFeature)->Get_StdDev() );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_PCA::Get_Matrix(void)
{
	int		i, j1, j2;

	m_SymMat.Create(Get_NGrids(), Get_NGrids());

	switch( m_Method )
	{
	default:
	case 0:	// Correlation matrix: Center and reduce the column vectors.
		for(j1=0; j1<Get_NGrids()-1; j1++)
		{
			m_SymMat[j1][j1] = 1.0;

			for(j2=j1+1; j2<Get_NGrids(); j2++)
			{
				m_SymMat[j1][j2] = 0.0;

				for(i=0; i<Get_NCells() && Set_Progress_NCells(i); i++)
				{
					m_SymMat[j1][j2]	+= Get_Value(j1, i) * Get_Value(j2, i);
				}

				m_SymMat[j2][j1] = m_SymMat[j1][j2];
			}
		}

		m_SymMat[Get_NGrids() - 1][Get_NGrids() - 1] = 1.0;
		break;

	case 1:	// Variance-covariance matrix: Center the column vectors.
	case 2:	// Sums-of-squares-and-cross-products matrix
		for(j1=0; j1<Get_NGrids(); j1++)
		{
			for(j2=j1; j2<Get_NGrids(); j2++)
			{
				m_SymMat[j1][j2] = 0.0;

				for(i=0; i<Get_NCells() && Set_Progress_NCells(i); i++)
				{
					m_SymMat[j1][j2]	+= Get_Value(j1, i) * Get_Value(j2, i);
				}

				m_SymMat[j2][j1] = m_SymMat[j1][j2];
			}
		}
		break;
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool	SG_Triangular_Decomposition	(CSG_Matrix &a, CSG_Vector &d, CSG_Vector &e);
bool	SG_Tridiagonal_QL			(CSG_Matrix &z, CSG_Vector &d, CSG_Vector &e);

//---------------------------------------------------------
bool CGrid_PCA::Get_Reduction(void)	// Eigen-reduction
{
	int			i, j, k;
	CSG_Vector	Eigenvalues, Intermediate;
	CSG_Matrix	SymMat(m_SymMat);

	//--------------------------------------------------------
	if( !SG_Triangular_Decomposition(m_SymMat, Eigenvalues, Intermediate) )	// Triangular decomposition
	{
		return( false );
	}

	if( !SG_Tridiagonal_QL(m_SymMat, Eigenvalues, Intermediate) )			// Reduction of sym. trid. matrix
	{
		return( false );
	}

	//--------------------------------------------------------
	if( 1 )
	{
		// Eigenvalues should be strictly positive; limited precision machine arithmetic may affect this.
		// Eigenvalues are often expressed as cumulative percentages, representing the 'percentage variance
		// explained' by the associated axis or principal component.

		Message_Add(CSG_String::Format(SG_T("\n%s\n"), _TL("Eigenvalues")), false);

		for(j=Get_NGrids()-1; j>=0; j--)
		{
			Message_Add(CSG_String::Format(SG_T("%18.5f\n"), Eigenvalues[j]), false);
		}

		Message_Add(CSG_String::Format(SG_T("\n%s:\n"), _TL("Eigenvectors")), false);

		for(j=0; j<Get_NGrids(); j++)
		{
			for(i=0; i<Get_NGrids(); i++)
			{
				Message_Add(CSG_String::Format(SG_T("%12.4f"), m_SymMat[j][Get_NGrids() - 1 - i]), false);
			}

			Message_Add(SG_T("\n"), false);
		}
	}

	//--------------------------------------------------------
	m_pPCA->Del_Items();

	for(j=0; j<3; j++)
	{
		m_pPCA->Add_Item(SG_Create_Grid(*Get_System()));
		m_pPCA->asGrid(j)->Set_Name(CSG_String::Format(SG_T("%s %d"), _TL("Component"), j + 1));
	}

	for(i=0; i<Get_NCells() && Set_Progress_NCells(i); i++)
	{
		for(j=0; j<3; j++)
		{
			double	d	= 0.0;

			for(k=0; k<Get_NGrids(); k++)
			{
				d	+= Get_Value(k, i) * m_SymMat[k][Get_NGrids() - 1 - j];
			}

			m_pPCA->asGrid(j)->Set_Value(i, d);
		}
	}

	//--------------------------------------------------------
/*	// Form projections of col.-points on first three prin. components.
	// Store in 'SymMat', overwriting what was stored in this.
	for (j = 1; j <= m; j++) {
	for (k = 1; k <= m; k++) {
	Intermediate[k] = SymMat[j][k]; }  //SymMat[j][k] will be overwritten
	for (i = 1; i <= 3; i++) {
		SymMat[j][i] = 0.0;
		for (k2 = 1; k2 <= m; k2++) {
		SymMat[j][i] += Intermediate[k2] * m_SymMat[k2][m-i+1]; }
		if (Eigenvalues[m-i+1] > 0.0005)   // Guard against zero eigenvalue
			SymMat[j][i] /= sqrt(Eigenvalues[m-i+1]);   // Rescale
		else
			SymMat[j][i] = 0.0;    // Standard kludge
	}
	}

	printf("\nProjections of column-points on first 3 prin. comps.:\n");
	for (j = 1; j <= m; j++) {
	for (k = 1; k <= 3; k++)  {
		printf("%12.4f", SymMat[j][k]);  }
		printf("\n");  }
/**/
	
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
// Householder reduction of matrix a to tridiagonal form.

bool SG_Triangular_Decomposition(CSG_Matrix &a, CSG_Vector &d, CSG_Vector &e)
{
	if( a.Get_NX() != a.Get_NY() )
	{
		return( false );
	}

	int		l, k, j, i, n;
	double	scale, hh, h, g, f;

	n	= a.Get_NX();

	d.Create(n);
	e.Create(n);

	for(i=n-1; i>=1; i--)
	{
		l	= i - 1;
		h	= scale = 0.0;

		if( l > 0 )
		{
			for(k=0; k<=l; k++)
			{
				scale	+= fabs(a[i][k]);
			}

			if( scale == 0.0 )
			{
				e[i]	= a[i][l];
			}
			else
			{
				for(k=0; k<=l; k++)
				{
					a[i][k]	/= scale;
					h		+= a[i][k] * a[i][k];
				}

				f		= a[i][l];
				g		= f > 0.0 ? -sqrt(h) : sqrt(h);
				e[i]	= scale * g;
				h		-= f * g;
				a[i][l]	= f - g;
				f		= 0.0;

				for(j=0; j<=l; j++)
				{
					a[j][i]	= a[i][j]/h;
					g		= 0.0;

					for(k=0; k<=j; k++)
					{
						g	+= a[j][k] * a[i][k];
					}

					for(k=j+1; k<=l; k++)
					{
						g	+= a[k][j] * a[i][k];
					}

					e[j]	= g / h;
					f		+= e[j] * a[i][j];
				}

				hh	= f / (h + h);

				for(j=0; j<=l; j++)
				{
					f		= a[i][j];
					e[j]	= g = e[j] - hh * f;

					for(k=0; k<=j; k++)
					{
						a[j][k]	-= (f * e[k] + g * a[i][k]);
					}
				}
			}
		}
		else
		{
			e[i]	= a[i][l];
		}

		d[i]	= h;
	}

	d[0]	= 0.0;
	e[0]	= 0.0;

	for(i=0; i<n; i++)
	{
		l	= i - 1;

		if( d[i] )
		{	
			for(j=0; j<=l; j++)
			{
				g	= 0.0;

				for(k=0; k<=l; k++)
				{
					g		+= a[i][k] * a[k][j];
				}

				for(k=0; k<=l; k++)
				{
					a[k][j]	-= g * a[k][i];
				}
			}
		}

		d[i]	= a[i][i];
		a[i][i]	= 1.0;

		for(j=0; j<=l; j++)
		{
			a[j][i]	= a[i][j] = 0.0;
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
// Tridiagonal QL algorithm -- Implicit

bool SG_Tridiagonal_QL(CSG_Matrix &z, CSG_Vector &d, CSG_Vector &e)
{
	if( z.Get_NX() != z.Get_NY() || z.Get_NX() != d.Get_N() || z.Get_NX() != e.Get_N() )
	{
		return( false );
	}

	int		m, l, iter, i, k, n;
	double	s, r, p, g, f, dd, c, b;

	n	= d.Get_N();

	for(i=1; i<n; i++)
	{
		e[i - 1]	= e[i];
	}

	e[n - 1]	= 0.0;

	for(l=0; l<n; l++)
	{
		iter	= 0;

		do
		{
			for(m=l; m<n-1; m++)
			{
				dd	= fabs(d[m]) + fabs(d[m + 1]);

				if( fabs(e[m]) + dd == dd )
				{
					break;
				}
			}

			if( m != l )
			{
				if( iter++ == 30 )
				{
					return( false );	// erhand("No convergence in TLQI.");
				}

				g	= (d[l+1] - d[l]) / (2.0 * e[l]);
				r	= sqrt((g * g) + 1.0);
				g	= d[m] - d[l] + e[l] / (g + SIGN(r, g));
				s	= c = 1.0;
				p	= 0.0;

				for(i = m-1; i >= l; i--)
				{
					f = s * e[i];
					b = c * e[i];

					if (fabs(f) >= fabs(g))
					{
						c = g / f;
						r = sqrt((c * c) + 1.0);
						e[i+1] = f * r;
						c *= (s = 1.0/r);
					}
					else
					{
						s = f / g;
						r = sqrt((s * s) + 1.0);
						e[i+1] = g * r;
						s *= (c = 1.0/r);
					}

					g		= d[i+1] - p;
					r		= (d[i] - g) * s + 2.0 * c * b;
					p		= s * r;
					d[i+1]	= g + p;
					g		= c * r - b;

					for(k=0; k<n; k++)
					{
						f			= z[k][i+1];
						z[k][i+1]	= s * z[k][i] + c * f;
						z[k][i]		= c * z[k][i] - s * f;
					}
				}

				d[l] = d[l] - p;
				e[l] = g;
				e[m] = 0.0;
			}
		}
		while( m != l );
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
