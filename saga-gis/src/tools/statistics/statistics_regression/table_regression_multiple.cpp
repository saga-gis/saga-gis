/**********************************************************
 * Version $Id: table_regression_multiple.cpp 1098 2011-06-16 16:06:32Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                 statistics_regression                 //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//             table_regression_multiple.cpp             //
//                                                       //
//                 Copyright (C) 2012 by                 //
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
#include "table_regression_multiple.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CTable_Regression_Multiple_Base::Initialise(void)
{
	//-----------------------------------------------------
	Set_Author		(SG_T("O.Conrad (c) 2012"));

	Set_Description	(_TW(
		"Multiple linear regression analysis using ordinary least squares."
	));

	//-----------------------------------------------------
	CSG_Parameter	*pNode	= Parameters("TABLE");

	Parameters.Add_Table_Field(
		pNode	, "DEPENDENT"	, _TL("Dependent Variable"),
		_TL("")
	);

	Parameters.Add_Parameters(
		pNode	, "PREDICTORS"	, _TL("Predictors"),
		_TL("")
	);

	Parameters.Add_Table(
		NULL	, "INFO_COEFF"	, _TL("Details: Coefficients"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Table(
		NULL	, "INFO_MODEL"	, _TL("Details: Model"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Table(
		NULL	, "INFO_STEPS"	, _TL("Details: Steps"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Choice(
		NULL	,"METHOD"		, _TL("Method"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|"),
			_TL("include all"),
			_TL("forward"),
			_TL("backward"),
			_TL("stepwise")
		), 3
	);

	Parameters.Add_Value(
		NULL	, "P_VALUE"		, _TL("Significance Level"),
		_TL("Significance level (aka p-value) as threshold for automated predictor selection, given as percentage"),
		PARAMETER_TYPE_Double, 5.0, 0.0, true, 100.0, true
	);

	Parameters.Add_Choice(
		NULL	,"CROSSVAL"		, _TL("Cross Validation"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|"),
			_TL("none"),
			_TL("leave one out"),
			_TL("2-fold"),
			_TL("k-fold")
		), 0
	);

	Parameters.Add_Value(
		NULL	, "CROSSVAL_K"	, _TL("Cross Validation Subsamples"),
		_TL("number of subsamples for k-fold cross validation"),
		PARAMETER_TYPE_Int, 10, 2, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CTable_Regression_Multiple_Base::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier(SG_T("TABLE")) )
	{
		CSG_Table		*pTable			= pParameter->asTable();
		CSG_Parameters	*pPredictors	= pParameters->Get_Parameter("PREDICTORS")->asParameters();

		pPredictors->Del_Parameters();

		if( pTable )
		{
			for(int i=0; i<pTable->Get_Field_Count(); i++)
			{
				if( SG_Data_Type_is_Numeric(pTable->Get_Field_Type(i)) )
				{
					pPredictors->Add_Value(NULL, SG_Get_String(i), pTable->Get_Field_Name(i), _TL(""), PARAMETER_TYPE_Bool, false);
				}
			}
		}
	}

	return( true );
}

//---------------------------------------------------------
int CTable_Regression_Multiple_Base::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	pParameter->Cmp_Identifier(SG_T("METHOD")) )
	{
		pParameters->Set_Enabled("P_VALUE", pParameter->asInt() > 0);
	}

	if(	pParameter->Cmp_Identifier(SG_T("CROSSVAL")) )
	{
		pParameters->Get_Parameter("CROSSVAL_K")->Set_Enabled(pParameter->asInt() == 3);	// k-fold
	}

	return( 0 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTable_Regression_Multiple_Base::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Parameters	*pPredictors	= Parameters("PREDICTORS")->asParameters();

	if( pPredictors->Get_Count() <= 0 )
	{
		Message_Add(_TL("no predictors available"));

		return( false );
	}

	//-----------------------------------------------------
	int			Dependent, iPredictor, nPredictors, *Predictors, iSample;
	CSG_Strings	Names;
	CSG_Table	*pTable;

	Dependent	= Parameters("DEPENDENT")->asInt();
	Predictors	= new int[pPredictors->Get_Count()];

	pTable		= Parameters("TABLE")->asTable();

	Names	+= pTable->Get_Field_Name(Dependent);

	for(iPredictor=0, nPredictors=0; iPredictor<pPredictors->Get_Count(); iPredictor++)	// Independent Variables
	{
		if( pPredictors->Get_Parameter(iPredictor)->asBool() )
		{
			Names	+= pTable->Get_Field_Name(
				Predictors[nPredictors++]	= CSG_String(pPredictors->Get_Parameter(iPredictor)->Get_Identifier()).asInt()
			);
		}
	}

	if( nPredictors <= 0 )
	{
		Message_Add(_TL("no predictors in selection"));

		delete[](Predictors);

		return( false );
	}

	//-----------------------------------------------------
	CSG_Vector	Sample(1 + nPredictors);
	CSG_Matrix	Samples;

	for(iSample=0; iSample<pTable->Get_Count(); iSample++)
	{
		CSG_Table_Record	*pRecord	= pTable->Get_Record(iSample);

		if( !pRecord->is_NoData(Dependent) )
		{
			bool	bOkay	= true;

			Sample[0]	= pRecord->asDouble(Dependent);

			for(iPredictor=0; iPredictor<nPredictors && bOkay; iPredictor++)
			{
				if( !pRecord->is_NoData(Predictors[iPredictor]) )
				{
					Sample[1 + iPredictor]	= pRecord->asDouble(Predictors[iPredictor]);
				}
				else
				{
					bOkay	= false;
				}
			}

			if( bOkay )
			{
				Samples.Add_Row(Sample);
			}
		}
	}

	if( Samples.Get_NRows() <= 0 )
	{
		Message_Add(_TL("no valid samples in data set"));

		delete[](Predictors);

		return( false );
	}

	//-----------------------------------------------------
	CSG_Regression_Multiple	Regression;

	double	P	= Parameters("P_VALUE")->asDouble();

	switch( Parameters("METHOD")->asInt() )
	{
	default:
	case 0:	if( !Regression.Get_Model         (Samples      , &Names) )	return( false );	break;
	case 1:	if( !Regression.Get_Model_Forward (Samples, P   , &Names) )	return( false );	break;
	case 2:	if( !Regression.Get_Model_Backward(Samples,    P, &Names) )	return( false );	break;
	case 3:	if( !Regression.Get_Model_Stepwise(Samples, P, P, &Names) )	return( false );	break;
	}

	Message_Add(Regression.Get_Info(), false);

	//-----------------------------------------------------
	int	CrossVal;

	switch( Parameters("CROSSVAL")->asInt() )
	{
	default:	CrossVal	= 0;									break;	// none
	case 1:		CrossVal	= 1;									break;	// leave one out (LOOVC)
	case 2:		CrossVal	= 2;									break;	// 2-fold
	case 3:		CrossVal	= Parameters("CROSSVAL_K")->asInt();	break;	// k-fold
	}

	if( CrossVal > 0 && Regression.Get_CrossValidation(CrossVal) )
	{
		Message_Fmt("\n%s:", _TL("Cross Validation"));
		Message_Fmt("\n\t%s:\t%s"  , _TL("Type"   ), Parameters("CROSSVAL")->asString());
		Message_Fmt("\n\t%s:\t%d"  , _TL("Samples"), Regression.Get_CV_nSamples()      );
		Message_Fmt("\n\t%s:\t%f"  , _TL("RMSE"   ), Regression.Get_CV_RMSE()          );
		Message_Fmt("\n\t%s:\t%.2f", _TL("NRMSE"  ), Regression.Get_CV_NRMSE()  * 100.0);
		Message_Fmt("\n\t%s:\t%.2f", _TL("R2"     ), Regression.Get_CV_R2()     * 100.0);
	}

	//-----------------------------------------------------
	if( Parameters("INFO_COEFF")->asTable() )
	{
		Parameters("INFO_COEFF")->asTable()->Assign(Regression.Get_Info_Regression());
		Parameters("INFO_COEFF")->asTable()->Set_Name(_TL("MLRA Coefficients"));
	}

	if( Parameters("INFO_MODEL")->asTable() )
	{
		Parameters("INFO_MODEL")->asTable()->Assign(Regression.Get_Info_Model());
		Parameters("INFO_MODEL")->asTable()->Set_Name(_TL("MLRA Model"));
	}

	if( Parameters("INFO_STEPS")->asTable() )
	{
		Parameters("INFO_STEPS")->asTable()->Assign(Regression.Get_Info_Steps());
		Parameters("INFO_STEPS")->asTable()->Set_Name(_TL("MLRA Steps"));
	}

	//-----------------------------------------------------
	CSG_Table	*pResults	= Parameters("RESULTS")->asTable();

	if( pResults != NULL && pResults != pTable )
	{
		if( pResults->Get_ObjectType() == SG_DATAOBJECT_TYPE_Shapes
		&&	pTable  ->Get_ObjectType() == SG_DATAOBJECT_TYPE_Shapes )
		{
			((CSG_Shapes *)pResults)->Create(*((CSG_Shapes *)pTable));
		}
		else
		{
			pResults->Create(*pTable);
		}

		pResults->Fmt_Name("%s [%s]", pTable->Get_Name(), _TL("Regression"));

		pTable	= pResults;
	}

	//-----------------------------------------------------
	int	Offset	= pTable->Get_Field_Count();

	pTable->Add_Field(_TL("PREDICTED"), SG_DATATYPE_Double);
	pTable->Add_Field(_TL("RESIDUAL" ), SG_DATATYPE_Double);

	for(iSample=0; iSample<pTable->Get_Count(); iSample++)
	{
		CSG_Table_Record	*pRecord	= pTable->Get_Record(iSample);

		bool	bOkay	= false;
		double	z		= Regression.Get_RConst();

		if( !pRecord->is_NoData(Dependent) )
		{
			for(iPredictor=0, bOkay=true; iPredictor<Regression.Get_nPredictors() && bOkay; iPredictor++)
			{
				if( !pRecord->is_NoData(Predictors[iPredictor]) )
				{
					z	+= Regression.Get_RCoeff(Regression.Get_Predictor(iPredictor)) * pRecord->asDouble(Predictors[iPredictor]);
				}
				else
				{
					bOkay	= false;
				}
			}
		}

		if( bOkay )
		{
			pRecord->Set_Value(Offset + 0, z);
			pRecord->Set_Value(Offset + 1, z - pRecord->asDouble(Dependent));
		}
		else
		{
			pRecord->Set_NoData(Offset + 0);
			pRecord->Set_NoData(Offset + 1);
		}
	}

	if( pTable != Parameters("RESULTS")->asTable() )
	{
		DataObject_Update(pTable);
	}

	//-----------------------------------------------------
	delete[](Predictors);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTable_Regression_Multiple::CTable_Regression_Multiple(void)
	: CTable_Regression_Multiple_Base()
{
	Set_Name		(_TL("Multiple Linear Regression Analysis"));

	Parameters.Add_Table(
		NULL	, "TABLE"	, _TL("Table"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table(
		NULL	, "RESULTS"	, _TL("Results"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Initialise();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTable_Regression_Multiple_Shapes::CTable_Regression_Multiple_Shapes(void)
	: CTable_Regression_Multiple_Base()
{
	Set_Name		(_TL("Multiple Linear Regression Analysis (Shapes)"));

	Parameters.Add_Shapes(
		NULL	, "TABLE"	, _TL("Shapes"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Shapes(
		NULL	, "RESULTS"	, _TL("Results"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Initialise();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
