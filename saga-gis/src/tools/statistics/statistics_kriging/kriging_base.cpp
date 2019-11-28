
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                  statistics_kriging                   //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  kriging_base.cpp                     //
//                                                       //
//                 Olaf Conrad (C) 2008                  //
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
		"POINTS"		, _TL("Points"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Table_Field("POINTS",
		"FIELD"			, _TL("Attribute"),
		_TL("")
	);

	//-----------------------------------------------------
	m_Grid_Target.Create(&Parameters, false, "", "TARGET_");

	m_Grid_Target.Add_Grid("PREDICTION", _TL("Prediction"      ), false);
	m_Grid_Target.Add_Grid("VARIANCE"  , _TL("Prediction Error"),  true);

	Parameters.Add_Choice("VARIANCE",
		"TQUALITY"		, _TL("Error Measure"),
		_TL(""),
		CSG_String::Format("%s|%s",
			_TL("Standard Deviation"),
			_TL("Variance")
		), 1
	);

	//-----------------------------------------------------
	Parameters.Add_Double("",
		"VAR_MAXDIST"	, _TL("Maximum Distance"),
		_TL("maximum distance for variogram estimation, ignored if set to zero"),
		0., 0., true
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
		"NODE_KRG"		, _TL("Kriging"),
		_TL("")
	);

	Parameters.Add_Bool("NODE_KRG",
		"LOG"			, _TL("Logarithmic Transformation"),
		_TL(""),
		false
	);

	Parameters.Add_Bool("NODE_KRG",
		"BLOCK"			, _TL("Block Kriging"),
		_TL(""),
		false
	);

	Parameters.Add_Double("BLOCK",
		"DBLOCK"		, _TL("Block Size"),
		_TL("Edge length [map units]"),
		100., 0., true
	);

	//-----------------------------------------------------
	Parameters.Add_Choice("NODE_KRG",
		"CV_METHOD"		, _TL("Cross Validation"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s",
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
	m_Search_Options.Create(&Parameters, "NODE_SEARCH", 16);

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
		m_Search_Options.On_Parameter_Changed(pParameters, pParameter);

		m_Grid_Target.Set_User_Defined(pParameters, pParameter->asShapes());
	}

	m_Grid_Target.On_Parameter_Changed(pParameters, pParameter);

	return( CSG_Tool::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
int CKriging_Base::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	pParameter->Cmp_Identifier("VARIANCE") )
	{
		pParameters->Set_Enabled("TQUALITY"    , pParameter->asPointer() != NULL);
	}

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

	m_Search_Options.On_Parameters_Enable(pParameters, pParameter);
	m_Grid_Target   .On_Parameters_Enable(pParameters, pParameter);

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CKriging_Base::On_Execute(void)
{
	CSG_Shapes	*pPoints	= Parameters("POINTS")->asShapes();

	if( pPoints->Get_Count() < 2 )
	{
		Error_Set(_TL("not enough points for interpolation"));

		return( false );
	}

	//-----------------------------------------------------
	int	Field	= Parameters("FIELD")->asInt();

	m_Block		= Parameters("BLOCK")->asBool() ? Parameters("DBLOCK")->asDouble() / 2. : 0.;

	bool	bLog	= Parameters("LOG")->asBool();

	//-----------------------------------------------------
	bool	bResult	= Init_Points(pPoints, Field, bLog);	

	//-----------------------------------------------------
	if( bResult )
	{
		CSG_Table	Variogram;

		if( m_pVariogram )
		{
			bResult	= m_pVariogram->Execute   (m_Points, &Variogram, &m_Model);
		}
		else
		{
			bResult	= CSG_Variogram::Calculate(m_Points, &Variogram,
				Parameters("VAR_NCLASSES")->asInt   (),
				Parameters("VAR_MAXDIST" )->asDouble(),
				Parameters("VAR_NSKIP"   )->asInt   ()
			);

			if( bResult )
			{
				m_Model.Clr_Data();

				for(int i=0; i<Variogram.Get_Count(); i++)
				{
					CSG_Table_Record	*pRecord	= Variogram.Get_Record(i);

					m_Model.Add_Data(pRecord->asDouble(CSG_Variogram::FIELD_DISTANCE), pRecord->asDouble(CSG_Variogram::FIELD_VAR_EXP));
				}

				bResult	= m_Model.Set_Formula(Parameters("VAR_MODEL")->asString())
					&& (m_Model.Get_Trend() || m_Model.Get_Parameter_Count() == 0);
			}
		}
	}

	//-----------------------------------------------------
	if( bResult )
	{
		bResult	= _Init_Grids() && _Init_Search(true);
	}

	//-----------------------------------------------------
	if( bResult )
	{
		bool	bStdDev	= Parameters("TQUALITY")->asInt() == 0;

		Message_Fmt("\n%s: %s", _TL("Variogram Model"), m_Model.Get_Formula(SG_TREND_STRING_Formula_Parameters).c_str());

		for(int y=0; y<m_pValue->Get_NY() && Set_Progress(y, m_pValue->Get_NY()); y++)
		{
			double	py = m_pValue->Get_YMin() + y * m_pValue->Get_Cellsize();

			#ifndef _DEBUG
			#pragma omp parallel for
			#endif // !_DEBUG
			for(int x=0; x<m_pValue->Get_NX(); x++)
			{
				double	v, e, px = m_pValue->Get_XMin() + x * m_pValue->Get_Cellsize();

				if( Get_Value(px, py, v, e) )
				{
					if( bLog )
					{
						v	= exp(v) - 1. + pPoints->Get_Minimum(Field);
					}

					if( bStdDev )
					{
						e	= sqrt(e);
					}

					Set_Value(x, y, v, e);
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
	m_Model.Clr_Data();

	m_Search.Destroy();
	m_W     .Destroy();
	m_Points.Destroy();

	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CKriging_Base::_Init_Grids(void)
{
	if( (m_pValue = m_Grid_Target.Get_Grid("PREDICTION")) != NULL )
	{
		m_pValue->Fmt_Name("%s.%s [%s]", Parameters("POINTS")->asShapes()->Get_Name(), Parameters("FIELD")->asString(), Get_Name().c_str());

		if( (m_pError = m_Grid_Target.Get_Grid("VARIANCE")) != NULL )
		{
			m_pError->Fmt_Name("%s.%s [%s %s]", Parameters("POINTS")->asShapes()->Get_Name(), Parameters("FIELD")->asString(), Get_Name().c_str(), _TL("Error"));
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CKriging_Base::Init_Points(CSG_Shapes *pPoints, int Field, bool bLog)
{
	m_Points.Create(3, pPoints->Get_Count());

	int	n	= 0;

	for(int i=0; i<pPoints->Get_Count(); i++)
	{
		CSG_Shape	*pPoint	= pPoints->Get_Shape(i);

		if( !pPoint->is_NoData(Field) )
		{
			m_Points[n][0]	= pPoint->Get_Point(0).x;
			m_Points[n][1]	= pPoint->Get_Point(0).y;
			m_Points[n][2]	= bLog ? log(1. + pPoint->asDouble(Field) - pPoints->Get_Minimum(Field))
							: pPoint->asDouble(Field);

			n++;
		}
	}

	if( n < 2 )
	{
		return( false );
	}

	m_Points.Set_Rows(n);	// resize if there are no-data values

	return( true );
}

//---------------------------------------------------------
bool CKriging_Base::_Init_Search(bool bUpdate)
{
	if( m_Search_Options.Do_Use_All(bUpdate) )	// global
	{
		return( Get_Weights(m_Points, m_W) );
	}

	return( m_Search.Create(m_Points) );	// local
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CKriging_Base::Get_Points(double x, double y, CSG_Matrix &Points)
{
	if( m_Search.is_Okay() )
	{
		CSG_Array_Int	Index;	CSG_Vector	Distance;

		m_Search.Get_Nearest_Points(x, y, m_Search_Options.Get_Max_Points(), m_Search_Options.Get_Radius(), Index, Distance);
		
		if( Index.Get_Size() >= m_Search_Options.Get_Min_Points() && Points.Create(3, (int)Index.Get_Size()) )
		{
			for(size_t i=0; i<Index.Get_Size(); i++)
			{
				Points.Set_Row(i, m_Points[Index[i]]);
			}

			return( true );
		}
	}

	return( false );
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

	int		i, nSamples	= 0;

	for(i=0; i<m_Points.Get_NRows(); i++)
	{
		SFull	+= m_Points[i][2];
	}

	//-----------------------------------------------------
	// leave-one-out cross validation (LOOCV)

	if( nSubSets < 2 || nSubSets > m_Points.Get_NRows() / 2. )
	{
		CSG_Shapes	*pResiduals	= Parameters("CV_RESIDUALS")->asShapes();

		if( pResiduals )
		{
			pResiduals->Create(SHAPE_TYPE_Point);
			pResiduals->Fmt_Name("%s.%s [%s, CV %s]", Parameters("POINTS")->asShapes()->Get_Name(), Parameters("FIELD")->asString(), Get_Name().c_str(), _TL("Residuals"));
			pResiduals->Add_Field("ORIGINALS", SG_DATATYPE_Double);
			pResiduals->Add_Field("PREDICTED", SG_DATATYPE_Double);
			pResiduals->Add_Field("RESIDUALS", SG_DATATYPE_Double);
		}

		for(i=m_Points.Get_NRows()-1; i>=0; i--)
		{
			CSG_Vector	Point	= m_Points.Get_Row(i);

			m_Points.Del_Row(i);

			double	v, e;

			if( _Init_Search() && Get_Value(Point, v, e) )
			{
				nSamples++;

				SE	+= SG_Get_Square(v - Point[2]);
				SR	+= SG_Get_Square(v - (SFull.Get_Sum() - Point[2]) / (1. + m_Points.Get_NRows()));

				if( pResiduals )
				{
					CSG_Shape	*pResidual	= pResiduals->Add_Shape();

					pResidual->Set_Point(   Point[0], Point[1], 0);
					pResidual->Set_Value(0, Point[2]    );
					pResidual->Set_Value(1,            v);
					pResidual->Set_Value(2, Point[2] - v);
				}
			}

			m_Points.Add_Row(Point);
		}
	}

	//-----------------------------------------------------
	// k-fold cross validation

	else
	{
		CSG_Matrix	Points_All(m_Points);

		CSG_Array_Int	SubSet(Points_All.Get_NRows());

		for(i=0; i<Points_All.Get_NRows(); i++)
		{
			SubSet[i]	= i % nSubSets;
		}

		//-------------------------------------------------
		for(int iSubSet=0; iSubSet<nSubSets && Process_Get_Okay(); iSubSet++)
		{
			CSG_Simple_Statistics	iSFull;

			CSG_Matrix	Points; m_Points.Destroy();

			for(i=0; i<Points_All.Get_NRows(); i++)
			{
				CSG_Vector	Point	= Points_All.Get_Row(i);

				if( SubSet[i] == iSubSet )
				{
					Points  .Add_Row(Point);
				}
				else
				{
					m_Points.Add_Row(Point);

					iSFull	+= Point[2];
				}
			}

			//---------------------------------------------
			if( _Init_Search() )
			{
				nSamples++;

				for(i=0; i<Points.Get_NRows(); i++)
				{
					double	v, e, *Point = Points[i];

					if( Get_Value(Point, v, e) )
					{
						SE	+= SG_Get_Square(v - Point[2]);
						SR	+= SG_Get_Square(v - iSFull.Get_Mean());
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
	if( Parameters("CV_SUMMARY")->asTable() )
	{
		CSG_Table	&Summary	= *Parameters("CV_SUMMARY")->asTable();

		Summary.Destroy();
		Summary.Set_Name(_TL("Cross Validation"));

		Summary.Add_Field(_TL("Parameter"), SG_DATATYPE_String);
		Summary.Add_Field(_TL("Value"    ), SG_DATATYPE_Double);

		#define CV_ADD_SUMMARY(name, value)	{ CSG_Table_Record *pR = Summary.Add_Record();\
			pR->Set_Value(0, name);\
			pR->Set_Value(1, value);\
		}

		CV_ADD_SUMMARY("SAMPLES", nSamples);
		CV_ADD_SUMMARY("MSE"    ,      SE.Get_Mean());
		CV_ADD_SUMMARY("RMSE"   , sqrt(SE.Get_Mean()));
		CV_ADD_SUMMARY("NRMSE"  , sqrt(SE.Get_Mean()) / SFull.Get_Range() * 100.);
		CV_ADD_SUMMARY("R2"     , SR.Get_Sum() / (SR.Get_Sum() + SE.Get_Sum()) * 100.);
	}

	//-----------------------------------------------------
	Message_Fmt("\n%s:\n"      , _TL("Cross Validation"));
	Message_Fmt("\t%s:\t%s\n"  , _TL("Method" ), Parameters("CV_METHOD")->asString());
	Message_Fmt("\t%s:\t%d\n"  , _TL("Samples"), nSamples);
	Message_Fmt("\t%s:\t%f\n"  , _TL("MSE"    ),      SE.Get_Mean());
	Message_Fmt("\t%s:\t%f\n"  , _TL("RMSE"   ), sqrt(SE.Get_Mean()));
	Message_Fmt("\t%s:\t%.2f\n", _TL("NRMSE"  ), sqrt(SE.Get_Mean()) / SFull.Get_Range() * 100.);
	Message_Fmt("\t%s:\t%.2f\n", _TL("R2"     ), SR.Get_Sum() / (SR.Get_Sum() + SE.Get_Sum()) * 100.);

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
