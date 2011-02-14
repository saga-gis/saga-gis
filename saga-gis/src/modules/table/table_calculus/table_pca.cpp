
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
//                     table_pca.cpp                     //
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
#include "table_pca.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTable_PCA::CTable_PCA(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name		(_TL("Principle Components Analysis"));

	Set_Author		(SG_T("O.Conrad (c) 2010"));

	Set_Description	(_TW(
		"Principle Components Analysis (PCA) for tables. "
		"Implementation based on F. Murtagh's "
		"<a target=\"_blank\" href=\"http://lib.stat.cmu.edu/multi/pca.c\">code</a> "
		"as provided by the "
		"<a target=\"_blank\" href=\"http://lib.stat.cmu.edu\">StatLib</a> web site.\n"
		"\n"
		"References:\n"
		"Bahrenberg, G., Giese, E., Nipper, J. (1992): Statistische Methoden in der Geographie 2 - Multivariate Statistik. pp.198-277.\n"
	));

	//-----------------------------------------------------
	pNode	= Parameters.Add_Table(
		NULL	, "TABLE"		, _TL("Table"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Parameters(
		pNode	, "FIELDS"		, _TL("Attributes"),
		_TL("")
	);

	Parameters.Add_Table(
		NULL	, "PCA"			, _TL("Principle Components"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
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
int CTable_PCA::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("TABLE")) )
	{
		CSG_Table		*pTable		= pParameter->asTable();
		CSG_Parameters	*pFields	= pParameters->Get_Parameter("FIELDS")->asParameters();

		pFields->Del_Parameters();

		for(int i=0; i<pTable->Get_Field_Count(); i++)
		{
			if( SG_Data_Type_is_Numeric(pTable->Get_Field_Type(i)) )
			{
				pFields->Add_Value(NULL, CSG_String::Format(SG_T("%d"), i), pTable->Get_Field_Name(i), _TL(""), PARAMETER_TYPE_Bool, false);
			}
		}
	}

	return( 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTable_PCA::On_Execute(void)
{
	CSG_Vector	Eigen_Values;
	CSG_Matrix	Eigen_Vectors, Matrix;

	//-----------------------------------------------------
	m_pTable	= Parameters("TABLE")	->asTable();
	m_Method	= Parameters("METHOD")	->asInt();

	//-----------------------------------------------------
	if( !Get_Fields() )
	{
		Error_Set(_TL("invalid field selection"));

		SG_FREE_SAFE(m_Features);

		return( false );
	}

	if( !Get_Matrix(Matrix) )
	{
		Error_Set(_TL("matrix initialisation failed"));

		SG_FREE_SAFE(m_Features);

		return( false );
	}

	if( !SG_Matrix_Eigen_Reduction(Matrix, Eigen_Vectors, Eigen_Values) )
	{
		Error_Set(_TL("Eigen reduction failed"));

		SG_FREE_SAFE(m_Features);

		return( false );
	}

	//-----------------------------------------------------
	Get_Components(Eigen_Vectors, Eigen_Values);

	//-----------------------------------------------------
	SG_FREE_SAFE(m_Features);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTable_PCA::Get_Fields(void)
{
	CSG_Parameters	*pFields	= Parameters("FIELDS")->asParameters();

	m_Features	= (int *)SG_Calloc(pFields->Get_Count(), sizeof(int));
	m_nFeatures	= 0;

	for(int iFeature=0; iFeature<pFields->Get_Count(); iFeature++)
	{
		if( pFields->Get_Parameter(iFeature)->asBool() )
		{
			CSG_String	s(pFields->Get_Parameter(iFeature)->Get_Identifier());

			m_Features[m_nFeatures++]	= s.asInt();
		}
	}

	return( m_nFeatures > 1 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline bool CTable_PCA::is_NoData(int iElement)
{
	CSG_Table_Record	*pElement	= m_pTable->Get_Record(iElement);

	for(int iFeature=0; iFeature<m_nFeatures; iFeature++)
	{
		if( pElement->is_NoData(m_Features[iFeature]) )
		{
			return( true );
		}
	}

	return( false );
}

//---------------------------------------------------------
inline double CTable_PCA::Get_Value(int iFeature, int iElement)
{
	CSG_Table_Record	*pElement	= m_pTable->Get_Record(iElement);

	switch( m_Method )
	{
	default:
	case 0:	// Correlation matrix: Center and reduce the column vectors.
		return( (pElement->asDouble(iFeature) - m_pTable->Get_Mean(iFeature)) / (sqrt(m_pTable->Get_Count() * m_pTable->Get_Variance(iFeature))) );

	case 1:	// Variance-covariance matrix: Center the column vectors.
		return( (pElement->asDouble(iFeature) - m_pTable->Get_Mean(iFeature)) );

	case 2:	// Sums-of-squares-and-cross-products matrix
		return( (pElement->asDouble(iFeature)) );
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTable_PCA::Get_Matrix(CSG_Matrix &Matrix)
{
	int		i, j1, j2;

	Matrix.Create(m_nFeatures, m_nFeatures);
	Matrix.Set_Zero();

	switch( m_Method )
	{
	//-----------------------------------------------------
	default:
	case 0:	// Correlation matrix: Center and reduce the column vectors.
		for(j1=0; j1<m_nFeatures; j1++)
		{
			Matrix[j1][j1] = 1.0;
		}

		for(i=0; i<m_pTable->Get_Count() && Set_Progress(i, m_pTable->Get_Count()); i++)
		{
			if( !is_NoData(i) )
			{
				for(j1=0; j1<m_nFeatures-1; j1++)
				{
					for(j2=j1+1; j2<m_nFeatures; j2++)
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
		for(i=0; i<m_pTable->Get_Count() && Set_Progress(i, m_pTable->Get_Count()); i++)
		{
			if( !is_NoData(i) )
			{
				for(j1=0; j1<m_nFeatures; j1++)
				{
					for(j2=j1; j2<m_nFeatures; j2++)
					{
						Matrix[j1][j2]	+= Get_Value(j1, i) * Get_Value(j2, i);
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
bool CTable_PCA::Get_Components(CSG_Matrix &Eigen_Vectors, CSG_Vector &Eigen_Values)
{
	int		i, j, n, field0;
	double	Sum, Cum;

	///////////////////////////////////////////////////////
	//-----------------------------------------------------
	for(i=0, Sum=0.0; i<m_nFeatures; i++)
	{
		Sum	+= Eigen_Values[i];
	}

	Sum	= Sum > 0.0 ? 100.0 / Sum : 0.0;

	Message_Add(CSG_String::Format(SG_T("\n%s, %s, %s\n"), _TL("explained variance"), _TL("explained cumulative variance"), _TL("Eigenvalue")), false);

	for(j=m_nFeatures-1, Cum=0.0; j>=0; j--)
	{
		Cum	+= Eigen_Values[j] * Sum;

		Message_Add(CSG_String::Format(SG_T("%6.2f\t%6.2f\t%18.5f\n"), Eigen_Values[j] * Sum, Cum, Eigen_Values[j]), false);
	}

	Message_Add(CSG_String::Format(SG_T("\n%s:\n"), _TL("Eigenvectors")), false);

	for(j=0; j<m_nFeatures; j++)
	{
		for(i=0; i<m_nFeatures; i++)
		{
			Message_Add(CSG_String::Format(SG_T("%12.4f"), Eigen_Vectors[j][m_nFeatures - 1 - i]), false);
		}

		Message_Add(SG_T("\n"), false);
	}

	///////////////////////////////////////////////////////
	//-----------------------------------------------------
	n	= Parameters("NFIRST")->asInt();

	if( n <= 0 || n > m_nFeatures )
	{
		n	= m_nFeatures;
	}

	//-----------------------------------------------------
	CSG_Table	*pPCA	= Parameters("PCA")->asTable();

	if( pPCA == NULL )
	{
		pPCA	= m_pTable;
	}

	if( pPCA != m_pTable )
	{
		pPCA->Destroy();
		pPCA->Set_Name(CSG_String::Format(SG_T("%s [%s]"), m_pTable->Get_Name(), _TL("Principal Components")));
	}

	//-----------------------------------------------------
	field0	= pPCA->Get_Field_Count();

	for(i=0; i<n; i++)
	{
		pPCA->Add_Field(CSG_String::Format(SG_T("%s %d"), _TL("Component"), i + 1), SG_DATATYPE_Double);
	}

	//-----------------------------------------------------
	for(int iElement=0; iElement<m_pTable->Get_Count() && Set_Progress(iElement, m_pTable->Get_Count()); iElement++)
	{
		if( !is_NoData(iElement) )
		{
			CSG_Table_Record	*pElement	= pPCA == m_pTable ? pPCA->Get_Record(iElement) : pPCA->Add_Record();

			for(i=0, j=m_nFeatures-1; i<n; i++, j--)
			{
				double	d	= 0.0;

				for(int k=0; k<m_nFeatures; k++)
				{
					d	+= Get_Value(k, iElement) * Eigen_Vectors[k][j];
				}

				pElement->Set_Value(field0 + i, d);
			}
		}
	}

	//-----------------------------------------------------
	if( pPCA == m_pTable )
	{
		DataObject_Update(m_pTable);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
