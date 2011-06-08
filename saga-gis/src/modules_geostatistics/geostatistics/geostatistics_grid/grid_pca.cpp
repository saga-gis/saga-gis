/**********************************************************
 * Version $Id$
 *********************************************************/

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
		"Principle Components Analysis (PCA) for grids. "
		"Implementation based on F. Murtagh's "
		"<a target=\"_blank\" href=\"http://lib.stat.cmu.edu/multi/pca.c\">code</a> "
		"as provided by the "
		"<a target=\"_blank\" href=\"http://lib.stat.cmu.edu\">StatLib</a> web site.\n"
		"\n"
		"References:\n"
		"Bahrenberg, G., Giese, E., Nipper, J. (1992): Statistische Methoden in der Geographie 2 - Multivariate Statistik. pp.198-277.\n"
	));

	//-----------------------------------------------------
	Parameters.Add_Grid_List(
		NULL	, "GRIDS"		, _TL("Grids"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid_List(
		NULL	, "PCA"			, _TL("Principle Components"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice(
		NULL	, "METHOD"		, _TL("Method"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|"),
			_TL("correlation matrix"),
			_TL("variance-covariance matrix"),
			_TL("sums-of-squares-and-cross-products matrix")
		), 1
	);

	Parameters.Add_Value(
		NULL	, "NFIRST"		, _TL("Number of Components"),
		_TL("maximum number of calculated first components; set to zero to get all"),
		PARAMETER_TYPE_Int, 3, 0, true
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
	CSG_Vector	Eigen_Values;
	CSG_Matrix	Eigen_Vectors, Matrix;

	//-----------------------------------------------------
	m_pGrids	= Parameters("GRIDS")	->asGridList();
	m_Method	= Parameters("METHOD")	->asInt();

	//-----------------------------------------------------
	if( !Get_Matrix(Matrix) )
	{
		Error_Set(_TL("matrix initialisation failed"));

		return( false );
	}

	//-----------------------------------------------------
	if( !SG_Matrix_Eigen_Reduction(Matrix, Eigen_Vectors, Eigen_Values) )
	{
		Error_Set(_TL("Eigen reduction failed"));

		return( false );
	}

	//-----------------------------------------------------
	Get_Components(Eigen_Vectors, Eigen_Values);

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline int CGrid_PCA::Get_NGrids(void)
{
	return( m_pGrids->Get_Count() );
}

//---------------------------------------------------------
inline bool CGrid_PCA::is_NoData(int iCell)
{
	for(int iFeature=0; iFeature<Get_NGrids(); iFeature++)
	{
		if( m_pGrids->asGrid(iFeature)->is_NoData(iCell) )
		{
			return( true );
		}
	}

	return( false );
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


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_PCA::Get_Matrix(CSG_Matrix &Matrix)
{
	int		j1, j2;
	long	i;

	Matrix.Create(Get_NGrids(), Get_NGrids());
	Matrix.Set_Zero();

	switch( m_Method )
	{
	//-----------------------------------------------------
	default:
	case 0:	// Correlation matrix: Center and reduce the column vectors.
		for(j1=0; j1<Get_NGrids(); j1++)
		{
			Matrix[j1][j1] = 1.0;
		}

		for(i=0; i<Get_NCells() && Set_Progress_NCells(i); i++)
		{
			if( !is_NoData(i) )
			{
				for(j1=0; j1<Get_NGrids()-1; j1++)
				{
					for(j2=j1+1; j2<Get_NGrids(); j2++)
					{
						Matrix[j1][j2]	+= Get_Value(j1, i) * Get_Value(j2, i);
					}
				}
			}
		}
		break;

	//-----------------------------------------------------
	case 1:	// Variance-covariance matrix: Center the column vectors.
	case 2:	// Sums-of-squares-and-cross-products matrix
		for(i=0; i<Get_NCells() && Set_Progress_NCells(i); i++)
		{
			if( !is_NoData(i) )
			{
				for(j1=0; j1<Get_NGrids(); j1++)
				{
					for(j2=j1; j2<Get_NGrids(); j2++)
					{
						Matrix[j1][j2]	+= Get_Value(j1, i) * Get_Value(j2, i);
					}
				}
			}
		}
		break;
	}

	//-----------------------------------------------------
	for(j1=0; j1<Get_NGrids(); j1++)
	{
		for(j2=j1; j2<Get_NGrids(); j2++)
		{
			Matrix[j2][j1] = Matrix[j1][j2];
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_PCA::Get_Components(CSG_Matrix &Eigen_Vectors, CSG_Vector &Eigen_Values)
{
	int		i, j, n;
	double	Sum, Cum;

	///////////////////////////////////////////////////////
	//-----------------------------------------------------
	for(i=0, Sum=0.0; i<Get_NGrids(); i++)
	{
		Sum	+= Eigen_Values[i];
	}

	Sum	= Sum > 0.0 ? 100.0 / Sum : 0.0;

	Message_Add(CSG_String::Format(SG_T("\n%s, %s, %s\n"), _TL("explained variance"), _TL("explained cumulative variance"), _TL("Eigenvalue")), false);

	for(j=Get_NGrids()-1, Cum=0.0; j>=0; j--)
	{
		Cum	+= Eigen_Values[j] * Sum;

		Message_Add(CSG_String::Format(SG_T("%6.2f\t%6.2f\t%18.5f\n"), Eigen_Values[j] * Sum, Cum, Eigen_Values[j]), false);
	}

	Message_Add(CSG_String::Format(SG_T("\n%s:\n"), _TL("Eigenvectors")), false);

	for(j=0; j<Get_NGrids(); j++)
	{
		for(i=0; i<Get_NGrids(); i++)
		{
			Message_Add(CSG_String::Format(SG_T("%12.4f"), Eigen_Vectors[j][Get_NGrids() - 1 - i]), false);
		}

		Message_Add(SG_T("\n"), false);
	}

	///////////////////////////////////////////////////////
	//-----------------------------------------------------
	n	= Parameters("NFIRST")->asInt();

	if( n <= 0 || n > Get_NGrids() )
	{
		n	= Get_NGrids();
	}

	//-----------------------------------------------------
	CSG_Parameter_Grid_List	*pPCA	= Parameters("PCA")->asGridList();

	pPCA->Del_Items();

	for(i=0; i<n; i++)
	{
		pPCA->Add_Item(SG_Create_Grid(*Get_System()));
		pPCA->asGrid(i)->Set_Name(CSG_String::Format(SG_T("%s %d"), _TL("Component"), i + 1));
	}

	//-----------------------------------------------------
	for(long iCell=0; iCell<Get_NCells() && Set_Progress_NCells(iCell); iCell++)
	{
		if( is_NoData(iCell) )
		{
			for(i=0; i<n; i++)
			{
				pPCA->asGrid(i)->Set_NoData(iCell);
			}
		}
		else
		{
			for(i=0, j=Get_NGrids()-1; i<n; i++, j--)
			{
				double	d	= 0.0;

				for(int k=0; k<Get_NGrids(); k++)
				{
					d	+= Get_Value(k, iCell) * Eigen_Vectors[k][j];
				}

				pPCA->asGrid(i)->Set_Value(iCell, d);
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
