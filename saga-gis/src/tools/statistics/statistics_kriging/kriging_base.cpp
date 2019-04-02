/**********************************************************
 * Version $Id: kriging_base.cpp 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//            geostatistics_kriging_variogram            //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   kriging_base.cpp                    //
//                                                       //
//                 Copyright (C) 2008 by                 //
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
//                Bundesstr. 55                          //
//                20146 Hamburg                          //
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
#include "variogram_dialog.h"

#include "kriging_base.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CKriging_Base::CKriging_Base(void)
{
	//-----------------------------------------------------
	Parameters.Add_Shapes("",
		"POINTS"	, _TL("Points"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Table_Field("POINTS",
		"FIELD"		, _TL("Attribute"),
		_TL("")
	);

	//-----------------------------------------------------
	m_Grid_Target.Create(&Parameters, false, NULL, "TARGET_");

	m_Grid_Target.Add_Grid("PREDICTION", _TL("Prediction"     ), false);
	m_Grid_Target.Add_Grid("VARIANCE"  , _TL("Quality Measure"), true);

	//-----------------------------------------------------
	Parameters.Add_Double("",
		"VAR_MAXDIST"	, _TL("Maximum Distance"),
		_TL("maximum distance for variogram estimation, ignored if set to zero"),
		0.0, 0.0, true
	)->Set_UseInGUI(false);

	Parameters.Add_Int("",
		"VAR_NCLASSES"	, _TL("Lag Distance Classes"),
		_TL("initial number of lag distance classes"),
		100, 1, true
	)->Set_UseInGUI(false);

	Parameters.Add_Int("",
		"VAR_NSKIP"		, _TL("Skip"),
		_TL(""),
		1, 1, true
	)->Set_UseInGUI(false);

	Parameters.Add_String("",
		"VAR_MODEL"		, _TL("Model"),
		_TL(""),
		"a + b * x"
	)->Set_UseInGUI(false);

	//-----------------------------------------------------
	Parameters.Add_Node("",
		"NODE_KRG"	, _TL("Kriging"),
		_TL("")
	);

	Parameters.Add_Choice("NODE_KRG",
		"TQUALITY"	, _TL("Type of Quality Measure"),
		_TL(""),
		CSG_String::Format("%s|%s|",
			_TL("Standard Deviation"),
			_TL("Variance")
		), 0
	);

	Parameters.Add_Bool("NODE_KRG",
		"LOG"		, _TL("Logarithmic Transformation"),
		_TL(""),
		false
	);

	Parameters.Add_Bool("NODE_KRG",
		"BLOCK"		, _TL("Block Kriging"),
		_TL(""),
		false
	);

	Parameters.Add_Double("BLOCK",
		"DBLOCK"	, _TL("Block Size"),
		_TL("Edge length [map units]"),
		100.0, 0.0, true
	);

	//-----------------------------------------------------
	Parameters.Add_Choice("NODE_KRG",
		"CV_METHOD"		, _TL("Cross Validation"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s|",
			_TL("none"),
			_TL("leave one out"),
			_TL("2-fold"),
			_TL("k-fold")
		), 0
	);

	Parameters.Add_Table("CV_METHOD",
		"CV_SUMMARY"	, _TL("Cross Validation Summary"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Shapes("CV_METHOD",
		"CV_RESIDUALS"	, _TL("Cross Validation Residuals"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Point
	);

	Parameters.Add_Int("CV_METHOD",
		"CV_SAMPLES"	, _TL("Cross Validation Subsamples"),
		_TL("number of subsamples for k-fold cross validation"),
		10, 2, true
	);

	//-----------------------------------------------------
	m_Search.Create(&Parameters, Parameters.Add_Node("", "NODE_SEARCH", _TL("Search Options"), _TL("")), 16);

	//-----------------------------------------------------
	if( !SG_UI_Get_Window_Main() )
	{
		m_pVariogram	= NULL;
	}
	else
	{
		m_pVariogram	= new CVariogram_Dialog;
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CKriging_Base::~CKriging_Base(void)
{
	if( m_pVariogram && SG_UI_Get_Window_Main() )	// don't destroy dialog, if gui is closing (i.e. main window == NULL)
	{
		m_pVariogram->Destroy();

		delete(m_pVariogram);
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CKriging_Base::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("POINTS") )
	{
		m_Search.On_Parameter_Changed(pParameters, pParameter);

		m_Grid_Target.Set_User_Defined(pParameters, pParameter->asShapes());
	}

	m_Grid_Target.On_Parameter_Changed(pParameters, pParameter);

	return( CSG_Tool::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
int CKriging_Base::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	pParameter->Cmp_Identifier("BLOCK") )
	{
		pParameters->Set_Enabled("DBLOCK"      , pParameter->asBool());	// block size
	}

	if(	pParameter->Cmp_Identifier("CV_METHOD") )
	{
		pParameters->Set_Enabled("CV_SUMMARY"  , pParameter->asInt() != 0);	// !none
		pParameters->Set_Enabled("CV_RESIDUALS", pParameter->asInt() == 1);	// leave one out
		pParameters->Set_Enabled("CV_SAMPLES"  , pParameter->asInt() == 3);	// k-fold
	}

	m_Search     .On_Parameters_Enable(pParameters, pParameter);
	m_Grid_Target.On_Parameters_Enable(pParameters, pParameter);

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CKriging_Base::On_Execute(void)
{
	bool	bResult	= false;

	//-----------------------------------------------------
	m_Block		= Parameters("BLOCK"   )->asBool() ? Parameters("DBLOCK")->asDouble() / 2.0 : 0.0;
	m_bStdDev	= Parameters("TQUALITY")->asInt() == 0;
	m_bLog		= Parameters("LOG"     )->asBool();

	m_pPoints	= Parameters("POINTS"  )->asShapes();
	m_zField	= Parameters("FIELD"   )->asInt();

	if( m_pPoints->Get_Count() <= 1 )
	{
		SG_UI_Msg_Add(_TL("not enough points for interpolation"), true);

		return( false );
	}

	//-----------------------------------------------------
	CSG_Table	Variogram;

	if( m_pVariogram )
	{
		if( m_pVariogram->Execute(m_pPoints, m_zField, m_bLog, &Variogram, &m_Model) )
		{
			bResult	= true;
		}
	}
	else
	{
		int		nSkip		= Parameters("VAR_NSKIP"   )->asInt();
		int		nClasses	= Parameters("VAR_NCLASSES")->asInt();
		double	maxDistance	= Parameters("VAR_MAXDIST" )->asDouble();

		m_Model.Set_Formula(Parameters("VAR_MODEL")->asString());

		if( CSG_Variogram::Calculate(m_pPoints, m_zField, m_bLog, &Variogram, nClasses, maxDistance, nSkip) )
		{
			m_Model.Clr_Data();

			for(int i=0; i<Variogram.Get_Count(); i++)
			{
				CSG_Table_Record	*pRecord	= Variogram.Get_Record(i);

				m_Model.Add_Data(pRecord->asDouble(CSG_Variogram::FIELD_DISTANCE), pRecord->asDouble(CSG_Variogram::FIELD_VAR_EXP));
			}

			bResult	= m_Model.Get_Trend() || m_Model.Get_Parameter_Count() == 0;
		}
	}

	//-----------------------------------------------------
	if( bResult && (bResult = _Initialise_Grids() && On_Initialize()) )
	{
		Message_Fmt("\n%s: %s", _TL("Variogram Model"), m_Model.Get_Formula(SG_TREND_STRING_Formula_Parameters).c_str());

		for(int y=0; y<m_pGrid->Get_NY() && Set_Progress(y, m_pGrid->Get_NY()); y++)
		{
			#pragma omp parallel for
			for(int x=0; x<m_pGrid->Get_NX(); x++)
			{
				double	z, v;

				if( Get_Value(m_pGrid->Get_System().Get_Grid_to_World(x, y), z, v) )
				{
					Set_Value(x, y, z, v);
				}
				else
				{
					Set_NoData(x, y);
				}
			}
		}

		_Get_Cross_Validation();
	}

	//-----------------------------------------------------
	m_Model .Clr_Data();
	m_Search.Finalize();
	m_Data  .Clear();
	m_W     .Destroy();

	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CKriging_Base::_Initialise_Grids(void)
{
	if( (m_pGrid = m_Grid_Target.Get_Grid("PREDICTION")) != NULL )
	{
		m_pGrid->Fmt_Name("%s.%s [%s]", Parameters("POINTS")->asShapes()->Get_Name(), Parameters("FIELD")->asString(), Get_Name().c_str());

		if( (m_pVariance = m_Grid_Target.Get_Grid("VARIANCE")) != NULL )
		{
			m_pVariance->Fmt_Name("%s.%s [%s %s]", Parameters("POINTS")->asShapes()->Get_Name(), Parameters("FIELD")->asString(), Get_Name().c_str(), m_bStdDev ? _TL("Standard Deviation") : _TL("Variance"));
		}

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CKriging_Base::On_Initialize(void)
{
	//-----------------------------------------------------
	if( m_Search.Do_Use_All(true) )	// global
	{
		m_Data.Clear();

		for(int i=0; i<m_pPoints->Get_Count(); i++)
		{
			CSG_Shape	*pPoint	= m_pPoints->Get_Shape(i);

			if( !pPoint->is_NoData(m_zField) )
			{
				m_Data.Add(pPoint->Get_Point(0).x, pPoint->Get_Point(0).y, m_bLog ? log(pPoint->asDouble(m_zField)) : pPoint->asDouble(m_zField));
			}
		}

		return( Get_Weights(m_Data, m_W) );
	}

	//-----------------------------------------------------
	if( m_bLog )
	{
		CSG_Shapes	Points(SHAPE_TYPE_Point); Points.Add_Field("Z", SG_DATATYPE_Double);

		for(int i=0; i<m_pPoints->Get_Count() && Set_Progress(i, m_pPoints->Get_Count()); i++)
		{
			CSG_Shape	*pPoint	= m_pPoints->Get_Shape(i);

			if( !pPoint->is_NoData(m_zField) )
			{
				Points.Add_Shape(pPoint, SHAPE_COPY_GEOM)->Set_Value(0, log(pPoint->asDouble(m_zField)));
			}
		}

		return( m_Search.Initialize(&Points, 0) );
	}

	//-----------------------------------------------------
	return( m_Search.Initialize(m_pPoints, m_zField) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CKriging_Base::_Get_Cross_Validation(void)
{
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

			double	z, v;

			if( !pPoint->is_NoData(m_zField) && On_Initialize() && Get_Value(pPoint->Get_Point(0), z, v) )
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

					double	z, v;

					if( Get_Value(pPoint->Get_Point(0), z, v) )
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
