/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                     Grid_Gridding                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   Interpolation.cpp                   //
//                                                       //
//                 Copyright (C) 2003 by                 //
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
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
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
#include "Interpolation.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CInterpolation::CInterpolation(bool bCrossValidation, bool bMultiThreading)
{
	m_bMultiThreading	= bMultiThreading;

	//-----------------------------------------------------
	Parameters.Add_Shapes(NULL,
		"POINTS"	, _TL("Points"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Table_Field(Parameters("POINTS"),
		"FIELD"		, _TL("Attribute"),
		_TL("")
	);

	//-----------------------------------------------------
	if( bCrossValidation )
	{
		Parameters.Add_Choice(NULL,
			"CV_METHOD"		, _TL("Cross Validation"),
			_TL(""),
			CSG_String::Format("%s|%s|%s|%s|",
				_TL("none"),
				_TL("leave one out"),
				_TL("2-fold"),
				_TL("k-fold")
			), 0
		);

		Parameters.Add_Table(Parameters("CV_METHOD"),
			"CV_SUMMARY"	, _TL("Cross Validation Summary"),
			_TL(""),
			PARAMETER_OUTPUT_OPTIONAL
		);

		Parameters.Add_Shapes(Parameters("CV_METHOD"),
			"CV_RESIDUALS"	, _TL("Cross Validation Residuals"),
			_TL(""),
			PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Point
		);

		Parameters.Add_Int(Parameters("CV_METHOD"),
			"CV_SAMPLES"	, _TL("Cross Validation Subsamples"),
			_TL("number of subsamples for k-fold cross validation"),
			10, 2, true
		);
	}

	//-----------------------------------------------------
	m_Grid_Target.Create(&Parameters, true, NULL, "TARGET_");
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CInterpolation::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), "POINTS") )
	{
		m_Grid_Target.Set_User_Defined(pParameters, pParameter->asShapes());
	}

	m_Grid_Target.On_Parameter_Changed(pParameters, pParameter);

	return( CSG_Tool::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
int CInterpolation::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	!SG_STR_CMP(pParameter->Get_Identifier(), "CV_METHOD") )
	{
		pParameters->Set_Enabled("CV_SUMMARY"  , pParameter->asInt() != 0);	// !none
		pParameters->Set_Enabled("CV_RESIDUALS", pParameter->asInt() == 1);	// leave one out
		pParameters->Set_Enabled("CV_SAMPLES"  , pParameter->asInt() == 3);	// k-fold
	}

	m_Grid_Target.On_Parameters_Enable(pParameters, pParameter);

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CInterpolation::On_Execute(void)
{
	//-----------------------------------------------------
	m_pPoints	= Parameters("POINTS")->asShapes();
	m_zField	= Parameters("FIELD" )->asInt   ();

	if( m_pPoints->Get_Count() < 3 )
	{
		Error_Set(_TL("not enough points"));

		return( false );
	}

	//-----------------------------------------------------
	m_pGrid		= m_Grid_Target.Get_Grid();

	if( m_pGrid == NULL )
	{
		return( false );
	}

	m_pGrid->Set_Name(CSG_String::Format("%s [%s]", Parameters("FIELD")->asString(), Get_Name().c_str()));

	//-----------------------------------------------------
	if( !Interpolate() )
	{
		return( false );
	}

	_Get_Cross_Validation();

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline void CInterpolation::_Interpolate(int x, int y)
{
	double	z;

	if( Get_Value(m_pGrid->Get_System().Get_Grid_to_World(x, y), z) )
	{
		m_pGrid->Set_Value(x, y, z);
	}
	else
	{
		m_pGrid->Set_NoData(x, y);
	}
}

//---------------------------------------------------------
bool CInterpolation::Interpolate(void)
{
	if( !On_Initialize() )
	{
		return( false );
	}

	//-----------------------------------------------------
	for(int y=0; y<m_pGrid->Get_NY() && Set_Progress(y, m_pGrid->Get_NY()); y++)
	{
		if( m_bMultiThreading )
		{
			#pragma omp parallel for
			for(int x=0; x<m_pGrid->Get_NX(); x++)
			{
				_Interpolate(x, y);
			}
		}
		else
		{
			for(int x=0; x<m_pGrid->Get_NX(); x++)
			{
				_Interpolate(x, y);
			}
		}
	}

	//-----------------------------------------------------
	On_Finalize();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CInterpolation::_Get_Cross_Validation(void)
{
	if( !Parameters("CV_METHOD") )
	{
		return( true );
	}

	//-----------------------------------------------------
	int nSubSets;

	switch( Parameters("CV_METHOD")->asInt() )
	{
	default:	// none
		return( true );

	case  1:	// leave one out (LOOVC)
		nSubSets = 1;
		break;

	case  2:	// 2-fold
		nSubSets = 2;
		break;

	case  3:	// k-fold
		nSubSets = Parameters("CV_SAMPLES")->asInt();
		break;
	}

	//-----------------------------------------------------
	Process_Set_Text(_TL("Cross Validation"));

	CSG_Simple_Statistics	SFull, SR, SE;

	CSG_Shapes	*pFull	= m_pPoints;

	int		i, nSamples	= 0;

	for(i=0; i<pFull->Get_Count(); i++)
	{
		CSG_Shape	*pPoint	= pFull->Get_Shape(i);

		if( !pPoint->is_NoData(m_zField) )
		{
			SFull	+= pPoint->asDouble(m_zField);
		}
	}

	//-----------------------------------------------------
	// leave-one-out cross validation (LOOCV)

	if( nSubSets <= 1 || nSubSets > pFull->Get_Count() / 2 )
	{
		CSG_Shapes	Sample(*pFull), *pResiduals;	m_pPoints	= &Sample;

		if( (pResiduals = Parameters("CV_RESIDUALS")->asShapes()) != NULL )
		{
			pResiduals->Create(SHAPE_TYPE_Point, CSG_String::Format("%s [%s, %s]", m_pPoints->Get_Name(), Get_Name().c_str(), _TL("Residuals")));
			pResiduals->Add_Field(pFull->Get_Field_Name(m_zField), SG_DATATYPE_Double);
			pResiduals->Add_Field("PREDICTED", SG_DATATYPE_Double);
			pResiduals->Add_Field("RESIDUALS" , SG_DATATYPE_Double);
		}

		for(i=pFull->Get_Count()-1; i>=0 && Set_Progress(pFull->Get_Count()-1-i, pFull->Get_Count()); i--)
		{
			CSG_Shape	*pPoint	= pFull->Get_Shape(i);

			Sample.Del_Shape(i);

			double	z;

			if( !pPoint->is_NoData(m_zField) && On_Initialize() && Get_Value(pPoint->Get_Point(0), z) )
			{
				nSamples++;

				SE	+= SG_Get_Square(z - pPoint->asDouble(m_zField));
				SR	+= SG_Get_Square(z - (SFull.Get_Sum() - pPoint->asDouble(m_zField)) / Sample.Get_Count());

				if( pResiduals )
				{
					CSG_Shape	*pResidual	= pResiduals->Add_Shape();

					pResidual->Add_Point(pPoint->Get_Point(0));
					pResidual->Set_Value(0, pPoint->asDouble(m_zField));
					pResidual->Set_Value(1, z);
					pResidual->Set_Value(2, pPoint->asDouble(m_zField) - z);
				}
			}

			Sample.Add_Shape(pPoint);
		}
	}

	//-----------------------------------------------------
	// k-fold cross validation

	else
	{
		CSG_Array_Int	SubSet(pFull->Get_Count());

		for(i=0; i<pFull->Get_Count(); i++)
		{
			SubSet[i]	= i % nSubSets;
		}

		//-------------------------------------------------
		for(int iSubSet=0; iSubSet<nSubSets && Process_Get_Okay(); iSubSet++)
		{
			CSG_Simple_Statistics	iSFull;

			CSG_Shapes	Sample[2];

			Sample[0].Create(SHAPE_TYPE_Point, SG_T(""), pFull); m_pPoints = &Sample[0];
			Sample[1].Create(SHAPE_TYPE_Point, SG_T(""), pFull);

			for(i=0; i<pFull->Get_Count(); i++)
			{
				CSG_Shape	*pPoint	= pFull->Get_Shape(i);

				if( !pPoint->is_NoData(m_zField) )
				{
					if( SubSet[i] == iSubSet )
					{
						Sample[1].Add_Shape(pPoint);
					}
					else
					{
						Sample[0].Add_Shape(pPoint);

						iSFull	+= pPoint->asDouble(m_zField);
					}
				}
			}

			//---------------------------------------------
			if( On_Initialize() )
			{
				nSamples++;

				for(i=0; i<Sample[1].Get_Count(); i++)
				{
					CSG_Shape	*pPoint	= Sample[1].Get_Shape(i);

					double	z;

					if( Get_Value(pPoint->Get_Point(0), z) )
					{
						SE	+= SG_Get_Square(z - pPoint->asDouble(m_zField));
						SR	+= SG_Get_Square(z - iSFull.Get_Mean());
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	if( nSamples == 0 )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_Table	*pSummary	= Parameters("CV_SUMMARY")->asTable();

	if( pSummary )
	{
		pSummary->Destroy();
		pSummary->Set_Name(_TL("Cross Validation"));

		pSummary->Add_Field(_TL("Parameter"), SG_DATATYPE_String);
		pSummary->Add_Field(_TL("Value"    ), SG_DATATYPE_Double);

		#define CV_ADD_SUMMARY(name, value)	{ CSG_Table_Record *pR = pSummary->Add_Record();\
			pR->Set_Value(0, name);\
			pR->Set_Value(1, value);\
		}

		CV_ADD_SUMMARY("SAMPLES", nSamples);
		CV_ADD_SUMMARY("MSE"    ,      SE.Get_Mean());
		CV_ADD_SUMMARY("RMSE"   , sqrt(SE.Get_Mean()));
		CV_ADD_SUMMARY("NRMSE"  , sqrt(SE.Get_Mean()) / SFull.Get_Range() * 100.0);
		CV_ADD_SUMMARY("R2"     , SR.Get_Sum() / (SR.Get_Sum() + SE.Get_Sum()) * 100.0);
	}

	//-----------------------------------------------------
	CSG_String	Message;

	Message	+= CSG_String::Format("\n%s:\n"      , _TL("Cross Validation"));
	Message	+= CSG_String::Format("\t%s:\t%s\n"  , _TL("Method" ), Parameters("CV_METHOD")->asString());
	Message	+= CSG_String::Format("\t%s:\t%d\n"  , _TL("Samples"), nSamples);
	Message	+= CSG_String::Format("\t%s:\t%f\n"  , _TL("MSE"    ),      SE.Get_Mean());
	Message	+= CSG_String::Format("\t%s:\t%f\n"  , _TL("RMSE"   ), sqrt(SE.Get_Mean()));
	Message	+= CSG_String::Format("\t%s:\t%.2f\n", _TL("NRMSE"  ), sqrt(SE.Get_Mean()) / SFull.Get_Range() * 100.0);
	Message	+= CSG_String::Format("\t%s:\t%.2f\n", _TL("R2"     ), SR.Get_Sum() / (SR.Get_Sum() + SE.Get_Sum()) * 100.0);

	Message_Add(Message, false);

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
