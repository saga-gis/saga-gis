
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
#include "table_regression_multiple.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CTable_Regression_Multiple_Base::Initialise(void)
{
	Set_Author		("O.Conrad (c) 2012");

	Set_Description	(_TW(
		"Multiple linear regression analysis using ordinary least squares."
	));

	//-----------------------------------------------------
	Parameters.Add_Table_Field("TABLE",
		"DEPENDENT"		, _TL("Dependent Variable"),
		_TL("")
	);

	Parameters.Add_Parameters("TABLE",
		"PREDICTORS"	, _TL("Predictors"),
		_TL("")
	);

	Parameters.Add_Table("",
		"INFO_COEFF"	, _TL("Details: Coefficients"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Table("",
		"INFO_MODEL"	, _TL("Details: Model"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Table("",
		"INFO_STEPS"	, _TL("Details: Steps"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Choice("",
		"METHOD"		, _TL("Method"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s",
			_TL("include all"),
			_TL("forward"),
			_TL("backward"),
			_TL("stepwise")
		), 3
	);

	Parameters.Add_Double("",
		"P_VALUE"		, _TL("Significance Level"),
		_TL("Significance level (aka p-value) as threshold for automated predictor selection, given as percentage"),
		5., 0., true, 100., true
	);

	Parameters.Add_Choice("",
		"CROSSVAL"		, _TL("Cross Validation"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s",
			_TL("none"),
			_TL("leave one out"),
			_TL("2-fold"),
			_TL("k-fold")
		), 0
	);

	Parameters.Add_Int("",
		"CROSSVAL_K"	, _TL("Cross Validation Subsamples"),
		_TL("number of subsamples for k-fold cross validation"),
		10, 2, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CTable_Regression_Multiple_Base::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("TABLE") )
	{
		CSG_Table      *pTable      = pParameter->asTable();
		CSG_Parameters *pPredictors = (*pParameters)("PREDICTORS")->asParameters();

		pPredictors->Del_Parameters();

		if( pTable )
		{
			for(int i=0; i<pTable->Get_Field_Count(); i++)
			{
				if( SG_Data_Type_is_Numeric(pTable->Get_Field_Type(i)) )
				{
					pPredictors->Add_Bool("", SG_Get_String(i), pTable->Get_Field_Name(i), _TL(""), false);
				}
			}
		}
	}

	return( CSG_Tool::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
int CTable_Regression_Multiple_Base::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	pParameter->Cmp_Identifier("METHOD") )
	{
		pParameters->Set_Enabled("P_VALUE", pParameter->asInt() > 0);
	}

	if(	pParameter->Cmp_Identifier("CROSSVAL") )
	{
		pParameters->Set_Enabled("CROSSVAL_K", pParameter->asInt() == 3);	// k-fold
	}

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTable_Regression_Multiple_Base::On_Execute(void)
{
	CSG_Parameters	*pPredictors	= Parameters("PREDICTORS")->asParameters();

	if( pPredictors->Get_Count() < 1 )
	{
		Message_Add(_TL("no predictors available"));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Table	*pTable	= Parameters("TABLE")->asTable();

	int  Dependent	= Parameters("DEPENDENT")->asInt();
	int *Predictors	= new int[pPredictors->Get_Count()], nPredictors = 0;

	CSG_Strings	Names; Names += pTable->Get_Field_Name(Dependent);

	for(int iPredictor=0; iPredictor<pPredictors->Get_Count(); iPredictor++)	// Independent Variables
	{
		if( pPredictors->Get_Parameter(iPredictor)->asBool() )
		{
			Names	+= pTable->Get_Field_Name(Predictors[nPredictors++]	=
				CSG_String(pPredictors->Get_Parameter(iPredictor)->Get_Identifier()).asInt()
			);
		}
	}

	if( nPredictors < 1 )
	{
		Message_Add(_TL("no predictors in selection"));

		delete[](Predictors);

		return( false );
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

		pResults->Fmt_Name("%s.%s [%s]", pTable->Get_Name(), pTable->Get_Field_Name(Dependent), _TL("OLS"));

		pTable	= pResults;
	}

	//-----------------------------------------------------
	CSG_Matrix	Samples;	CSG_Vector	Sample(1 + nPredictors);

	for(int iSample=0; iSample<pTable->Get_Count(); iSample++)
	{
		CSG_Table_Record	*pRecord	= pTable->Get_Record(iSample);

		if( !pRecord->is_NoData(Dependent) )
		{
			Sample[0]	= pRecord->asDouble(Dependent);

			bool	bOkay	= true;

			for(int iPredictor=0; bOkay && iPredictor<nPredictors; iPredictor++)
			{
				if( (bOkay = !pRecord->is_NoData(Predictors[iPredictor])) == true )
				{
					Sample[1 + iPredictor]	= pRecord->asDouble(Predictors[iPredictor]);
				}
			}

			if( bOkay )
			{
				Samples.Add_Row(Sample);
			}
		}
	}

	if( Samples.Get_NRows() < 1 )
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
	default: if( !Regression.Get_Model         (Samples      , &Names) ) return( false ); break;
	case  1: if( !Regression.Get_Model_Forward (Samples, P   , &Names) ) return( false ); break;
	case  2: if( !Regression.Get_Model_Backward(Samples,    P, &Names) ) return( false ); break;
	case  3: if( !Regression.Get_Model_Stepwise(Samples, P, P, &Names) ) return( false ); break;
	}

	Message_Add(Regression.Get_Info(), false);

	//-----------------------------------------------------
	int	CrossVal;

	switch( Parameters("CROSSVAL")->asInt() )
	{
	default: CrossVal = 0                                ; break;	// none
	case  1: CrossVal = 1                                ; break;	// leave one out (LOOVC)
	case  2: CrossVal = 2                                ; break;	// 2-fold
	case  3: CrossVal = Parameters("CROSSVAL_K")->asInt(); break;	// k-fold
	}

	if( CrossVal > 0 && Regression.Get_CrossValidation(CrossVal) )
	{
		Message_Fmt("\n%s:", _TL("Cross Validation"));
		Message_Fmt("\n\t%s:\t%s"  , _TL("Type"   ), Parameters("CROSSVAL")->asString());
		Message_Fmt("\n\t%s:\t%d"  , _TL("Samples"), Regression.Get_CV_nSamples()      );
		Message_Fmt("\n\t%s:\t%f"  , _TL("RMSE"   ), Regression.Get_CV_RMSE()          );
		Message_Fmt("\n\t%s:\t%.2f", _TL("NRMSE"  ), Regression.Get_CV_NRMSE()   * 100.);
		Message_Fmt("\n\t%s:\t%.2f", _TL("R2"     ), Regression.Get_CV_R2()      * 100.);
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
	int	Offset	= pTable->Get_Field_Count();

	pTable->Add_Field(_TL("Estimated"), SG_DATATYPE_Double);
	pTable->Add_Field(_TL("Residual" ), SG_DATATYPE_Double);
	pTable->Add_Field(_TL("StdResid" ), SG_DATATYPE_Double);

	CSG_Simple_Statistics	Residuals;

	for(int iSample=0; iSample<pTable->Get_Count(); iSample++)
	{
		CSG_Table_Record	*pRecord	= pTable->Get_Record(iSample);

		double	z	= Regression.Get_RConst();

		bool	bOkay	= !pRecord->is_NoData(Dependent);

		for(int iPredictor=0; bOkay && iPredictor<Regression.Get_nPredictors(); iPredictor++)
		{
			if( (bOkay = !pRecord->is_NoData(Predictors[iPredictor])) == true )
			{
				z	+= Regression.Get_RCoeff(Regression.Get_Predictor(iPredictor)) * pRecord->asDouble(Predictors[iPredictor]);
			}
		}

		if( bOkay )
		{
			pRecord->Set_Value(Offset + 0, z);
			pRecord->Set_Value(Offset + 1, pRecord->asDouble(Dependent) - z);

			Residuals	+= pRecord->asDouble(Dependent) - z;
		}
		else
		{
			pRecord->Set_NoData(Offset + 0);
			pRecord->Set_NoData(Offset + 1);
		}
	}

	//-----------------------------------------------------
	for(int iSample=0; iSample<pTable->Get_Count(); iSample++)	// standardized residuals...
	{
		CSG_Table_Record	*pRecord	= pTable->Get_Record(iSample);

		if( !pRecord->is_NoData(Offset + 1) && Residuals.Get_StdDev() > 0. )
		{
			pRecord->Set_Value(Offset + 2, pRecord->asDouble(Offset + 1) / Residuals.Get_StdDev());
		}
		else
		{
			pRecord->Set_NoData(Offset + 2);
		}
	}

	//-----------------------------------------------------
	delete[](Predictors);

	if( pTable != Parameters("RESULTS")->asTable() )
	{
		DataObject_Update(pTable);
	}

	Set_Classification(pTable->asShapes(), Offset + 2);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTable_Regression_Multiple_Base::Set_Classification(CSG_Shapes *pShapes, int Field)
{
	CSG_Parameter	*pLUT	= DataObject_Get_Parameter(pShapes, "LUT");

	if( pLUT && pLUT->asTable() )
	{
		pLUT->asTable()->Del_Records();

		#define ADD_CLASS(r, g, b, min, max, name) { CSG_Table_Record *pClass = pLUT->asTable()->Add_Record();\
			pClass->Set_Value(0, SG_GET_RGB(r, g, b));\
			pClass->Set_Value(1, name);\
			pClass->Set_Value(2, "");\
			pClass->Set_Value(3, min);\
			pClass->Set_Value(4, max);\
		}

		ADD_CLASS( 69, 117, 181, -1e6, -2.5,      "< -2.5 Std.Dev");
		ADD_CLASS(132, 158, 168, -2.5, -1.5, "-2.5 - -1.5 Std.Dev");
		ADD_CLASS(192, 204, 190, -1.5, -0.5, "-1.5 - -0.5 Std.Dev");
		ADD_CLASS(255, 255, 191, -0.5,  0.5,  "-0.5 - 0.5 Std.Dev");
		ADD_CLASS(250, 185, 132,  0.5,  1.5,   "0.5 - 1.5 Std.Dev");
		ADD_CLASS(237, 117,  81,  1.5,  2.5,   "1.5 - 2.5 Std.Dev");
		ADD_CLASS(214,  47,  39,  2.5,  1e6,       "> 2.5 Std.Dev");

		DataObject_Set_Parameter(pShapes, pLUT);
		DataObject_Set_Parameter(pShapes, "COLORS_TYPE",     1);	// Color Classification Type: Lookup Table
		DataObject_Set_Parameter(pShapes, "LUT_ATTRIB" , Field);	// Lookup Table Attribute   : StdResid
	}

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
	Set_Name(CSG_String::Format("%s (%s)", _TL("Multiple Linear Regression Analysis"), _TL("Table")));

	Parameters.Add_Table("", "TABLE"  , _TL("Table"  ), _TL(""), PARAMETER_INPUT          );
	Parameters.Add_Table("", "RESULTS", _TL("Results"), _TL(""), PARAMETER_OUTPUT_OPTIONAL);

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
	Set_Name(CSG_String::Format("%s (%s)", _TL("Multiple Linear Regression Analysis"), _TL("Shapes")));

	Parameters.Add_Shapes("", "TABLE"  , _TL("Shapes" ), _TL(""), PARAMETER_INPUT          );
	Parameters.Add_Shapes("", "RESULTS", _TL("Results"), _TL(""), PARAMETER_OUTPUT_OPTIONAL);

	Initialise();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
