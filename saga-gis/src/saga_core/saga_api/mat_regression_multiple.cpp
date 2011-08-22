/**********************************************************
 * Version $Id$
 *********************************************************/

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
#include "mat_tools.h"
#include "table.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double SG_Regression_Get_Adjusted_R2(double r2, int n, int p, TSG_Regression_Correction Correction)
{
	double	r	= 1.0 - r2;

	switch( Correction )
	{
	case REGRESSION_CORR_None: default:
		return( r2 );

	case REGRESSION_CORR_Smith:
		r2	= 1.0 - ((n      ) / (n - p      )) * r;
		break;

	case REGRESSION_CORR_Wherry_1:
		r2	= 1.0 - ((n - 1.0) / (n - p - 1.0)) * r;
		break;

	case REGRESSION_CORR_Wherry_2:
		r2	= 1.0 - ((n - 1.0) / (n - p      )) * r;
		break;

	case REGRESSION_CORR_Olkin_Pratt:
	//	r2	= 1.0 - ((n - 3.0) / (n - p - 2.0)) * (r + (2.0 / (n - p)) * r*r);
		r2	= 1.0 - ((n - 3.0) * r / (n - p - 1.0)) * (1.0 + (2.0 * r) / (n - p + 1.0));
		break;

	case REGRESSION_CORR_Pratt:
		r2	= 1.0 - ((n - 3.0) * r / (n - p - 1.0)) * (1.0 + (2.0 * r) / (n - p - 2.3));
		break;

	case REGRESSION_CORR_Claudy_3:
		r2	= 1.0 - ((n - 4.0) * r / (n - p - 1.0)) * (1.0 + (2.0 * r) / (n - p + 1.0));
		break;
	}

	return( r2 < 0.0 ? 0.0 : r2 > 1.0 ? 1.0 : r2 );
}

//---------------------------------------------------------
double SG_Regression_Get_Significance(double R2, int nSamples, int nPredictors, TSG_Regression_Correction Correction)
{
	double	F	= (R2 / nPredictors) / ((1.0 - R2) / (nSamples - nPredictors - 1));

	return( CSG_Test_Distribution::Get_F_Tail(F, nPredictors, nSamples - nPredictors - 1, TESTDIST_TYPE_Left) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
enum ESG_Regression_Fields
{
	MRFIELD_NR	= 0,
	MRFIELD_NAME,
	MRFIELD_RCOEFF,
	MRFIELD_DCOEFF,
	MRFIELD_ORDER,
	MRFIELD_SIGNIF
};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Regression_Multiple::CSG_Regression_Multiple(void)
{
	m_pResult	= new CSG_Table;

	m_pResult->Add_Field("Field"				, SG_DATATYPE_Int);
	m_pResult->Add_Field("Variable"				, SG_DATATYPE_String);
	m_pResult->Add_Field("Regression Coeff."	, SG_DATATYPE_Double);
	m_pResult->Add_Field("Determination Coeff."	, SG_DATATYPE_Double);
	m_pResult->Add_Field("Order"				, SG_DATATYPE_Int);
}

//---------------------------------------------------------
CSG_Regression_Multiple::~CSG_Regression_Multiple(void)
{
	delete(m_pResult);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_Regression_Multiple::Destroy(void)
{
	m_pResult->Del_Records();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Regression_Multiple::Calculate(const CSG_Table &Values)
{
	Destroy();

	int		nVariables,	nValues;

	if(	(nVariables = Values.Get_Field_Count() - 1) > 0
	&&	(nValues    = Values.Get_Record_Count()) > nVariables )
	{
		for(int i=0; i<=nVariables; i++)
		{
			CSG_Table_Record	*pRecord	= m_pResult->Add_Record();

			pRecord->Set_Value(MRFIELD_NR	, i);
			pRecord->Set_Value(MRFIELD_NAME	, Values.Get_Field_Name(i));
		}

		_Get_Regression (Values);
		_Get_Correlation(Values);

		m_pResult->Set_Index(MRFIELD_ORDER, TABLE_INDEX_Ascending);

		m_nSamples	= nValues;

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
bool CSG_Regression_Multiple::_Get_Regression(const CSG_Table &Values)
{
	int			i, j, k, nVariables, nValues;
	double		sum;
	CSG_Vector	B, Y;
	CSG_Matrix	P, X;

	if(	(nVariables = Values.Get_Field_Count() - 1) > 0
	&&	(nValues    = Values.Get_Record_Count()) > nVariables )
	{
		B.Create(nVariables + 1);
		P.Create(nVariables + 1, nVariables + 1);

		Y.Create(nValues);
		X.Create(nValues, nVariables + 1);

		//-------------------------------------------------
		for(k=0; k<nValues; k++)
		{
			Y[k]	= Values[k][0];
			X[0][k] = 1.0;
		}

		for(i=1; i<=nVariables; i++)
		{
			for(k=0; k<nValues; k++)
			{
				X[i][k] = Values[k][i];
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

		P.Set_Inverse();

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
bool CSG_Regression_Multiple::_Get_Correlation(const class CSG_Table &Values)
{
	int			i, j, nVariables, nValues;
	double		r2, r2_sum;
	CSG_Matrix	z;

	//-----------------------------------------------------
	if(	(nVariables = Values.Get_Field_Count() - 1) > 0
	&&	(nValues    = Values.Get_Record_Count()) > nVariables )
	{
		z.Create(nValues, nVariables + 1);

		for(i=0; i<=nVariables; i++)
		{
			for(j=0; j<nValues; j++)
			{
				z[i][j]	= Values[j][i];
			}
		}

		//-------------------------------------------------
		m_pResult->Get_Record(0)->Set_Value(MRFIELD_ORDER	, -1);
		m_pResult->Get_Record(0)->Set_Value(MRFIELD_DCOEFF	, -1);

		for(i=0, r2_sum=0.0; i<nVariables; i++)
		{
			_Get_Correlation(nValues, nVariables, z.Get_Data() + 1, z[0], j, r2);

			r2_sum	+= (1.0 - r2_sum) * r2;

			m_pResult->Get_Record(j+1)->Set_Value(MRFIELD_ORDER , i);
			m_pResult->Get_Record(j+1)->Set_Value(MRFIELD_DCOEFF, r2_sum);
		}

		//-------------------------------------------------
		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Regression_Multiple::_Get_Correlation(int nValues, int nVariables, double **X, double *Y, int &iMax, double &rMax)
{
	int				i, n;
	double			*XMax;
	CSG_Regression	r;

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
bool CSG_Regression_Multiple::_Eliminate(int nValues, double *X, double *Y)
{
	CSG_Regression	r;

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
int CSG_Regression_Multiple::Get_Count(void) const
{
	return( m_pResult->Get_Count() - 1 );
}

//---------------------------------------------------------
int CSG_Regression_Multiple::Get_Index(int iVariable) const
{
	if( iVariable >= 0 && iVariable < Get_Count() )
	{
		return( m_pResult->Get_Record_byIndex(1 + iVariable)->asInt(MRFIELD_NR) );
	}

	return( -1 );
}

//---------------------------------------------------------
int CSG_Regression_Multiple::Get_Order(int iVariable) const
{
	if( iVariable >= 0 && iVariable < Get_Count() )
	{
		return( m_pResult->Get_Record(1 + iVariable)->asInt(MRFIELD_ORDER) );
	}

	return( -1 );
}

//---------------------------------------------------------
const SG_Char * CSG_Regression_Multiple::Get_Name(int iVariable, bool bOrdered) const
{
	if( iVariable >= 0 && iVariable < Get_Count() )
	{
		iVariable	= bOrdered ? Get_Index(iVariable) : iVariable + 1;

		return( m_pResult->Get_Record(iVariable)->asString(MRFIELD_NAME) );
	}

	return( SG_T("") );
}

//---------------------------------------------------------
double CSG_Regression_Multiple::Get_RConst(void) const
{
	if( Get_Count() > 0 )
	{
		return( m_pResult->Get_Record(0)->asDouble(MRFIELD_RCOEFF) );
	}

	return( -1.0 );
}

//---------------------------------------------------------
double CSG_Regression_Multiple::Get_RCoeff(int iVariable, bool bOrdered) const
{
	if( iVariable >= 0 && iVariable < Get_Count() )
	{
		iVariable	= bOrdered ? Get_Index(iVariable) : iVariable + 1;

		return( m_pResult->Get_Record(iVariable)->asDouble(MRFIELD_RCOEFF) );
	}

	return( -1.0 );
}

//---------------------------------------------------------
double CSG_Regression_Multiple::Get_R2(int iVariable, bool bOrdered, TSG_Regression_Correction Correction) const
{
	if( iVariable >= 0 && iVariable < Get_Count() )
	{
		int	iIndex	= bOrdered ? Get_Index(iVariable) : iVariable + 1;

		return( SG_Regression_Get_Adjusted_R2(
			m_pResult->Get_Record(iIndex)->asDouble(MRFIELD_DCOEFF),
			m_nSamples,		
			Get_Order(iIndex - 1) + 1,
			Correction
		));
	}

	return( -1.0 );
}

//---------------------------------------------------------
double CSG_Regression_Multiple::Get_R2_Partial(int iVariable, bool bOrdered, TSG_Regression_Correction Correction) const
{
	if( iVariable >= 0 && iVariable < Get_Count() )
	{
		if( !bOrdered )
		{
			iVariable	= Get_Order(iVariable);
		}

		double	r2	= Get_R2(iVariable, true, REGRESSION_CORR_None);

		if( iVariable > 0 )
		{
			r2	-= Get_R2(iVariable - 1, true, REGRESSION_CORR_None);
		}

		return( SG_Regression_Get_Adjusted_R2(r2, m_nSamples, iVariable + 1, Correction) );
	}

	return( -1.0 );
}

//---------------------------------------------------------
double CSG_Regression_Multiple::Get_Signif(int iVariable, bool bOrdered, TSG_Regression_Correction Correction) const
{
	if( iVariable >= 0 && iVariable < Get_Count() )
	{
		if( !bOrdered )
		{
			iVariable	= Get_Order(iVariable);
		}

		return( SG_Regression_Get_Significance(Get_R2(iVariable, true, Correction), m_nSamples, 1 + iVariable) );
	}

	return( -1.0 );
}

//---------------------------------------------------------
double CSG_Regression_Multiple::Get_Signif_Partial(int iVariable, bool bOrdered, TSG_Regression_Correction Correction) const
{
	if( iVariable >= 0 && iVariable < Get_Count() )
	{
		if( !bOrdered )
		{
			iVariable	= Get_Order(iVariable);
		}

		return( SG_Regression_Get_Significance(Get_R2_Partial(iVariable, true, Correction), m_nSamples, 1 + iVariable) );
	}

	return( -1.0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
