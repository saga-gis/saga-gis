
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
//              mat_regression_multiple.cpp              //
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
#include "table.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
enum
{
	MRFIELD_NR	= 0,
	MRFIELD_NAME,
	MRFIELD_RCOEFF,
	MRFIELD_DCOEFF,
	MRFIELD_ORDER
};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CMAT_Regression_Multiple::CMAT_Regression_Multiple(void)
{
	m_pResult	= new CTable;

	m_pResult->Add_Field("Field"				, TABLE_FIELDTYPE_Int);
	m_pResult->Add_Field("Variable"				, TABLE_FIELDTYPE_String);
	m_pResult->Add_Field("Regression Coeff."	, TABLE_FIELDTYPE_Double);
	m_pResult->Add_Field("Determination Coeff."	, TABLE_FIELDTYPE_Double);
	m_pResult->Add_Field("Order"				, TABLE_FIELDTYPE_Int);
}

//---------------------------------------------------------
CMAT_Regression_Multiple::~CMAT_Regression_Multiple(void)
{
	delete(m_pResult);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CMAT_Regression_Multiple::Destroy(void)
{
	m_pResult->Del_Records();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CMAT_Regression_Multiple::Calculate(CTable *pValues)
{
	int				i, nVariables, nValues;
	CTable_Record	*pRecord;

	//-----------------------------------------------------
	Destroy();

	if(	(nVariables = pValues->Get_Field_Count() - 1) > 0
	&&	(nValues    = pValues->Get_Record_Count()) > nVariables )
	{
		for(i=0; i<=nVariables; i++)
		{
			pRecord	= m_pResult->Add_Record();
			pRecord->Set_Value(MRFIELD_NR	, i);
			pRecord->Set_Value(MRFIELD_NAME	, pValues->Get_Field_Name(i));
		}

		//-------------------------------------------------
		_Get_Regression(pValues);

		_Get_Correlation(pValues);

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
bool CMAT_Regression_Multiple::_Get_Regression(CTable *pValues)
{
	int		i, j, k, nVariables, nValues;
	double	sum, *B, **P, **X, *Y;

	if(	(nVariables = pValues->Get_Field_Count() - 1) > 0
	&&	(nValues    = pValues->Get_Record_Count()) > nVariables )
	{
		B		= (double  *)API_Malloc( (nVariables + 1) * sizeof(double));
		P		= (double **)MATRIX_Alloc(nVariables + 1, nVariables + 1, sizeof(double));

		Y		= (double  *)API_Malloc(nValues * sizeof(double));
		X		= (double **)MATRIX_Alloc(nVariables + 1, nValues, sizeof(double));

		//-------------------------------------------------
		for(k=0; k<nValues; k++)
		{
			Y[k]	= pValues->Get_Record(k)->asDouble(0);
			X[0][k] = 1.0;
		}

		for(i=1; i<=nVariables; i++)
		{
			for(k=0; k<nValues; k++)
			{
				X[i][k] = pValues->Get_Record(k)->asDouble(i);
			}
		}

		//-------------------------------------------------
		for(i=0; i<=nVariables; i++)
		{
			for(k=0, sum=0.0; k<nValues; k++)
			{
				sum		+= X[i][k] * Y[k];
			}

			B[i]	= sum;

			for(j=0; j<=nVariables; j++) 
			{ 
				for(k=0, sum=0.0; k<nValues; k++)
				{
					sum		+= X[i][k] * X[j][k];
				}

				P[i][j]	= sum;
			}
		}

		MATRIX_Invert(nVariables + 1, P);

		//-------------------------------------------------
		for(i=0; i<=nVariables; i++)
		{
			for(j=0, sum=0.0; j<=nVariables; j++)
			{
				sum		+= P[i][j] * B[j];
			}

			m_pResult->Get_Record(i)->Set_Value(MRFIELD_RCOEFF, sum);
		}

		//-------------------------------------------------
		API_Free(B);
		MATRIX_Free(P);
		MATRIX_Free(X);

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
bool CMAT_Regression_Multiple::_Get_Correlation(CTable *pValues)
{
	int		i, j, nVariables, nValues;
	double	**Values, r2, r2_sum;

	if(	(nVariables = pValues->Get_Field_Count() - 1) > 0
	&&	(nValues    = pValues->Get_Record_Count()) > nVariables )
	{
		Values	= (double **)MATRIX_Alloc(nVariables + 1, nValues, sizeof(double));

		for(i=0; i<=nVariables; i++)
		{
			for(j=0; j<nValues; j++)
			{
				Values[i][j]	= pValues->Get_Record(j)->asDouble(i);
			}
		}

		//-------------------------------------------------
		m_pResult->Get_Record(0)->Set_Value(MRFIELD_ORDER	, -1);
		m_pResult->Get_Record(0)->Set_Value(MRFIELD_DCOEFF	, -1);

		for(i=0, r2_sum=0.0; i<nVariables; i++)
		{
			_Get_Correlation(nValues, nVariables, Values + 1, Values[0], j, r2);

			r2_sum	+= (1.0 - r2_sum) * r2;

			m_pResult->Get_Record(j+1)->Set_Value(MRFIELD_ORDER , i);
			m_pResult->Get_Record(j+1)->Set_Value(MRFIELD_DCOEFF, r2_sum);
		}

		//-------------------------------------------------
		API_Free(Values[0]);
		API_Free(Values);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CMAT_Regression_Multiple::_Get_Correlation(int nValues, int nVariables, double **X, double *Y, int &iMax, double &rMax)
{
	int				i, n;
	double			*XMax;
	CMAT_Regression	r;

	//-----------------------------------------------------
	for(i=0, n=0, iMax=-1, rMax=0.0; i<nVariables; i++)
	{
		if( X[i] && r.Calculate(nValues, X[i], Y) )
		{
			n++;

			if( iMax < 0 || rMax < r.Get_R2() )
			{
				iMax	= i;
				rMax	= r.Get_R2();
			}
		}
	}

	//-----------------------------------------------------
	if( n > 1 )
	{
		XMax	= X[iMax];
		X[iMax]	= NULL;

		for(i=0; i<nVariables; i++)
		{
			if( X[i] )
			{
				_Eliminate(nValues, XMax, X[i]);
			}
		}

		_Eliminate(nValues, XMax, Y);
	}

	return( iMax >= 0 );
}

//---------------------------------------------------------
bool CMAT_Regression_Multiple::_Eliminate(int nValues, double *X, double *Y)
{
	CMAT_Regression	r;

	if( r.Calculate(nValues, X, Y) )
	{
		for(int i=0; i<nValues; i++)
		{
			Y[i]	-= r.Get_Constant() + r.Get_Coefficient() * X[i];
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
int CMAT_Regression_Multiple::Get_Ordered(int iOrder)
{
	for(int i=0; i<m_pResult->Get_Record_Count(); i++)
	{
		if( iOrder == m_pResult->Get_Record(i)->asInt(MRFIELD_ORDER) )
		{
			return( i - 1 );
		}
	}

	return( -1 );
}

//---------------------------------------------------------
int CMAT_Regression_Multiple::Get_Order(int iVariable)
{
	if( ++iVariable > 0 && iVariable < m_pResult->Get_Record_Count() )
	{
		return( m_pResult->Get_Record(iVariable)->asInt(MRFIELD_ORDER) );
	}

	return( -1 );
}

//---------------------------------------------------------
double CMAT_Regression_Multiple::Get_R2(int iVariable)
{
	if( ++iVariable > 0 && iVariable < m_pResult->Get_Record_Count() )
	{
		return( m_pResult->Get_Record(iVariable)->asDouble(MRFIELD_DCOEFF) );
	}

	return( 0.0 );
}

//---------------------------------------------------------
double CMAT_Regression_Multiple::Get_R2_Change(int iVariable)
{
	int		iOrder	= Get_Order(iVariable);

	if( iOrder > 0 )
	{
		return( Get_R2(iVariable) - Get_R2(Get_Ordered(iOrder - 1)) );
	}

	if( iOrder == 0 )
	{
		return( Get_R2(iVariable) );
	}

	return( 0.0 );
}

//---------------------------------------------------------
double CMAT_Regression_Multiple::Get_RConst(void)
{
	if( m_pResult->Get_Record_Count() > 1 )
	{
		return( m_pResult->Get_Record(0)->asDouble(MRFIELD_RCOEFF) );
	}

	return( 0.0 );
}

//---------------------------------------------------------
double CMAT_Regression_Multiple::Get_RCoeff(int iVariable)
{
	if( ++iVariable > 0 && iVariable < m_pResult->Get_Record_Count() )
	{
		return( m_pResult->Get_Record(iVariable)->asDouble(MRFIELD_RCOEFF) );
	}

	return( 0.0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
