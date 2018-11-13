
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                    statistics_grid                    //
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
// Free Software Foundation, either version 2 of the     //
// License, or (at your option) any later version.       //
//                                                       //
// SAGA is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU General Public        //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU General    //
// Public License along with this program; if not, see   //
// <http://www.gnu.org/licenses/>.                       //
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

	Set_Author		("O.Conrad (c) 2010");

	Set_Description	(_TW(
		"Principle Components Analysis (PCA) for grids. "
		"PCA implementation is based on F.Murtagh's code as provided by the StatLib web site."
	));

	Add_Reference("Bahrenberg, G., Giese, E., Nipper, J.", "1992",
		"Statistische Methoden in der Geographie 2 - Multivariate Statistik", "pp.198-277."
	);

	Add_Reference("http://lib.stat.cmu.edu/multi/pca.c", SG_T("C-code by F.Murtagh"));
	Add_Reference("http://lib.stat.cmu.edu"            , SG_T("StatLib Web Site"   ));


	//-----------------------------------------------------
	Parameters.Add_Grid_List("",
		"GRIDS"		, _TL("Grids"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid_List("",
		"PCA"		, _TL("Principle Components"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Table("",
		"EIGEN"		, _TL("Eigen Vectors"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Choice("",
		"METHOD"	, _TL("Method"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|",
			_TL("correlation matrix"),
			_TL("variance-covariance matrix"),
			_TL("sums-of-squares-and-cross-products matrix")
		), 1
	);

	Parameters.Add_Int("",
		"COMPONENTS", _TL("Number of Components"),
		_TL("number of first components in the output; set to zero to get all"),
		3, 0, true
	);

	Parameters.Add_Bool("",
		"OVERWRITE"	, _TL("Overwrite Previous Results"),
		_TL(""),
		true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_PCA::On_Execute(void)
{
	CSG_Vector	Eigen_Values;
	CSG_Matrix	Eigen_Vectors, Matrix;

	//-----------------------------------------------------
	m_pGrids	= Parameters("GRIDS" )->asGridList();
	m_Method	= Parameters("METHOD")->asInt();

	m_nFeatures	= m_pGrids->Get_Grid_Count();

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
	Print_Eigen_Values (Eigen_Values );

//	Print_Eigen_Vectors(Eigen_Vectors);

	Get_Components(Eigen_Vectors);

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline bool CGrid_PCA::is_NoData(sLong iCell)
{
	for(int iFeature=0; iFeature<m_nFeatures; iFeature++)
	{
		if( m_pGrids->Get_Grid(iFeature)->is_NoData(iCell) )
		{
			return( true );
		}
	}

	return( false );
}

//---------------------------------------------------------
inline double CGrid_PCA::Get_Value(sLong iCell, int iFeature)
{
	CSG_Grid	*pGrid	= m_pGrids->Get_Grid(iFeature);

	switch( m_Method )
	{
	default:	// Correlation matrix: Center and reduce the column vectors.
		return( (pGrid->asDouble(iCell) - pGrid->Get_Mean()) / (sqrt(Get_NCells() * pGrid->Get_Variance())) );

	case  1:	// Variance-covariance matrix: Center the column vectors.
		return( (pGrid->asDouble(iCell) - pGrid->Get_Mean()) );

	case  2:	// Sums-of-squares-and-cross-products matrix
		return( (pGrid->asDouble(iCell)) );
	}
}

//---------------------------------------------------------
bool CGrid_PCA::Get_Matrix(CSG_Matrix &Matrix)
{
	if( !Matrix.Create(m_nFeatures, m_nFeatures) )
	{
		return( false );
	}

	Matrix.Set_Zero();

	int		j1, j2;
	sLong	iCell;

	switch( m_Method )
	{
	//-----------------------------------------------------
	default:	// Correlation matrix: Center and reduce the column vectors.
		for(j1=0; j1<m_nFeatures; j1++)
		{
			Matrix[j1][j1] = 1.0;
		}

		for(iCell=0; iCell<Get_NCells() && Set_Progress_NCells(iCell); iCell++)
		{
			if( !is_NoData(iCell) )
			{
				for(j1=0; j1<m_nFeatures-1; j1++)
				{
					for(j2=j1+1; j2<m_nFeatures; j2++)
					{
						Matrix[j1][j2]	+= Get_Value(iCell, j1) * Get_Value(iCell, j2);
					}
				}
			}
		}
		break;

	//-----------------------------------------------------
	case  1:	// Variance-covariance matrix: Center the column vectors.
	case  2:	// Sums-of-squares-and-cross-products matrix
		for(iCell=0; iCell<Get_NCells() && Set_Progress_NCells(iCell); iCell++)
		{
			if( !is_NoData(iCell) )
			{
				for(j1=0; j1<m_nFeatures; j1++)
				{
					for(j2=j1; j2<m_nFeatures; j2++)
					{
						Matrix[j1][j2]	+= Get_Value(iCell, j1) * Get_Value(iCell, j2);
					}
				}
			}
		}
		break;
	}

	//-----------------------------------------------------
	for(j1=0; j1<m_nFeatures; j1++)
	{
		for(j2=j1; j2<m_nFeatures; j2++)
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
void CGrid_PCA::Print_Eigen_Values(CSG_Vector &Eigen_Values)
{
	int		i;
	double	Sum, Cum;

	for(i=0, Sum=0.0, Cum=0.0; i<m_nFeatures; i++)
	{
		Sum	+= Eigen_Values[i];
	}

	Sum	= Sum > 0.0 ? 100.0 / Sum : 0.0;

	Message_Fmt("\n%s, %s, %s", _TL("explained variance"), _TL("explained cumulative variance"), _TL("Eigenvalue"));

	for(i=m_nFeatures-1; i>=0; i--)
	{
		Cum	+= Eigen_Values[i];

		Message_Fmt("\n%d.\t%.2f\t%.2f\t%f",
			m_nFeatures - i,
			Sum * Eigen_Values[i],
			Sum * Cum,
			Eigen_Values[i]
		);
	}
}

//---------------------------------------------------------
void CGrid_PCA::Print_Eigen_Vectors(CSG_Matrix &Eigen_Vectors)
{
	Message_Fmt("\n%s:", _TL("Eigenvectors"));

	for(int i=0; i<m_nFeatures; i++)
	{
		Message_Add("\n", false);

		for(int j=m_nFeatures-1; j>=0; j--)
		{
			Message_Fmt("%.4f\t", Eigen_Vectors[i][j]);
		}

		Message_Add(m_pGrids->Get_Grid(i)->Get_Name(), false);
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_PCA::Get_Components(CSG_Matrix &Eigen_Vectors)
{
	int		i;

	///////////////////////////////////////////////////////
	//-----------------------------------------------------
	CSG_Matrix	E(m_nFeatures, m_nFeatures);

	for(i=0; i<m_nFeatures; i++)
	{
		for(int j=0, k=m_nFeatures-1; j<m_nFeatures; j++, k--)
		{
			E[j][i]	= Eigen_Vectors[i][k];
		}
	}

	///////////////////////////////////////////////////////
	//-----------------------------------------------------
	CSG_Table	*pEigen	= Parameters("EIGEN")->asTable();

	if( pEigen )
	{
		pEigen->Destroy();
		pEigen->Set_Name(_TL("PCA Eigen Vectors"));

		for(i=0; i<m_nFeatures; i++)
		{
			pEigen->Add_Field(m_pGrids->Get_Grid(i)->Get_Name(), SG_DATATYPE_Double);
		}

		for(i=0; i<m_nFeatures; i++)
		{
			CSG_Table_Record	*pRecord	= pEigen->Add_Record();

			for(int j=0; j<m_nFeatures; j++)
			{
				pRecord->Set_Value(j, E[j][i]);
			}
		}
	}

	///////////////////////////////////////////////////////
	//-----------------------------------------------------
	int	nComponents	= Parameters("COMPONENTS")->asInt();

	if( nComponents <= 0 || nComponents > m_nFeatures )
	{
		nComponents	= m_nFeatures;
	}

	//-----------------------------------------------------
	CSG_Parameter_Grid_List	*pPCA	= Parameters("PCA")->asGridList();

	if( !Parameters("OVERWRITE")->asBool() )
	{
		pPCA->Del_Items();
	}

	for(i=0; i<nComponents; i++)
	{
		if( !pPCA->Get_Grid(i) )
		{
			CSG_Grid	*pGrid	= SG_Create_Grid(Get_System());

			if( !pGrid )
			{
				Error_Set(_TL("failed to allocate memory"));

				return( false );
			}

			pPCA->Add_Item(pGrid);
		}

		pPCA->Get_Grid(i)->Fmt_Name("PC%0*d", nComponents < 10 ? 1 : 2, i + 1);
	}

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for private(i)
		for(int x=0; x<Get_NX(); x++)
		{
			bool	bNoData	= false;

			CSG_Vector	X(m_nFeatures);

			for(i=0; i<m_nFeatures && !bNoData; i++)
			{
				if( m_pGrids->Get_Grid(i)->is_NoData(x, y) )
				{
					bNoData	= true;
				}
				else
				{
					X[i]	= m_pGrids->Get_Grid(i)->asDouble(x, y);
				}
			}

			if( bNoData )
			{
				for(i=0; i<nComponents; i++)
				{
					pPCA->Get_Grid(i)->Set_NoData(x, y);
				}
			}
			else
			{
				CSG_Vector	Y	= E * X;

				for(i=0; i<nComponents; i++)
				{
					pPCA->Get_Grid(i)->Set_Value(x, y, Y[i]);
				}
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
CGrid_PCA_Inverse::CGrid_PCA_Inverse(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Inverse Principle Components Rotation"));

	Set_Author		("O.Conrad (c) 2011");

	Set_Description	(_TW(
		"Inverse principle components rotation for grids."
	));

	Add_Reference("Bahrenberg, G., Giese, E., Nipper, J.", "1992",
		"Statistische Methoden in der Geographie 2 - Multivariate Statistik", "pp.198-277."
	);

	//-----------------------------------------------------
	Parameters.Add_Grid_List("",
		"PCA"		, _TL("Principle Components"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table("",
		"EIGEN"		, _TL("Eigen Vectors"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid_List("",
		"GRIDS"		, _TL("Grids"),
		_TL(""),
		PARAMETER_OUTPUT
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_PCA_Inverse::On_Execute(void)
{
	int			i, nFeatures;
	CSG_Table	*pEigen;

	//-----------------------------------------------------
	CSG_Parameter_Grid_List	*pPCA, *pGrids;

	pPCA	= Parameters("PCA"  )->asGridList();
	pGrids	= Parameters("GRIDS")->asGridList();
	pEigen	= Parameters("EIGEN")->asTable();

	//-----------------------------------------------------
	nFeatures	= pEigen->Get_Count();

	if( nFeatures != pEigen->Get_Field_Count() )
	{
		Error_Set(_TL("warning: number of Eigen vectors and components differs."));

		if( nFeatures > pEigen->Get_Field_Count() )
		{
			nFeatures	= pEigen->Get_Field_Count();
		}
	}

	if( nFeatures != pPCA->Get_Grid_Count() )
	{
		Error_Set(_TL("warning: number of component grids and components differs."));

		if( nFeatures > pPCA->Get_Grid_Count() )
		{
			nFeatures	= pPCA->Get_Grid_Count();
		}
	}

	if( nFeatures < 2 )
	{
		Error_Set(_TL("nothing to do. transformation needs at least two components."));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Matrix	E(nFeatures, nFeatures);

	for(i=0; i<nFeatures; i++)
	{
		CSG_Table_Record	*pRecord	= pEigen->Get_Record(i);

		for(int j=0; j<nFeatures; j++)
		{
			E[j][i]	= pRecord->asDouble(j);
		}
	}

	if( !E.Set_Inverse() )
	{
		Error_Set(_TL("matrix inversion failed"));

		return( false );
	}

	//-----------------------------------------------------
	pGrids->Del_Items();

	for(i=0; i<nFeatures; i++)
	{
		CSG_Grid	*pGrid	= SG_Create_Grid(Get_System());

		if( !pGrid )
		{
			Error_Set(_TL("failed to allocate memory"));

			return( false );
		}

		pGrid->Fmt_Name("%s %d", _TL("Feature"), i + 1);

		pGrids->Add_Item(pGrid);
	}

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for private(i)
		for(int x=0; x<Get_NX(); x++)
		{
			bool	bNoData	= false;

			CSG_Vector	Y(nFeatures);

			for(i=0; i<nFeatures && !bNoData; i++)
			{
				if( pPCA->Get_Grid(i)->is_NoData(x, y) )
				{
					bNoData	= true;
				}
				else
				{
					Y[i]	= pPCA->Get_Grid(i)->asDouble(x, y);
				}
			}

			if( bNoData )
			{
				for(i=0; i<nFeatures; i++)
				{
					pGrids->Get_Grid(i)->Set_NoData(x, y);
				}
			}
			else
			{
				CSG_Vector	X	= E * Y;

				for(i=0; i<nFeatures; i++)
				{
					pGrids->Get_Grid(i)->Set_Value(x, y, X[i]);
				}
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
