/**********************************************************
 * Version $Id: svm_grids.cpp 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                         svm                           //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    svm_grids.cpp                      //
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
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
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
#include "svm_grids.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void	SVM_Printf(const char *s)
{
	SG_UI_Msg_Add_Execution(s, false);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSVM_Grids::CSVM_Grids(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("SVM Classification"));

	Set_Author		(SG_T("O.Conrad (c) 2012"));

	Set_Description	(_TW(
		"Support Vector Machine (SVM) based classification for grids.\n"
		"Reference:\n"
		"Chang, C.-C. / Lin, C.-J. (2011): A library for support vector machines. "
		"ACM Transactions on Intelligent Systems and Technology, vol.2/3, p.1-27. "
		"<a target=\"_blank\" href=\"http://www.csie.ntu.edu.tw/~cjlin/libsvm\">LIBSVM Homepage</a>.\n"
	));

	//-----------------------------------------------------
	Parameters.Add_Grid_List(
		NULL	, "GRIDS"		, _TL("Grids"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "CLASSES"		, _TL("Classification"),
		_TL(""),
		PARAMETER_OUTPUT, true, SG_DATATYPE_Short
	);

	Parameters.Add_Choice(
		NULL	, "SCALING"		, _TL("Scaling"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|"),
			_TL("none"),
			_TL("normalize (0-1)"),
			_TL("standardize")
		), 2
	);

	Parameters.Add_Value(
		NULL	, "MESSAGE"		, _TL("Verbose Messages"),
		_TL(""),
		PARAMETER_TYPE_Bool, false
	);

	Parameters.Add_Choice(
		NULL	, "MODEL_SRC"	, _TL("Model Source"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("create from training areas"),
			_TL("restore from file")
		), 0
	);

	//-----------------------------------------------------
	Parameters.Add_FilePath(
		NULL	, "MODEL_LOAD"	, _TL("Restore Model from File"),
		_TL(""),
		NULL, NULL, false
	);

	//-----------------------------------------------------
	CSG_Parameter	*pNode	= Parameters.Add_Node(
		NULL	, "MODEL_TRAIN"	, _TL("Training Options"),
		_TL("")
	);

	CSG_Parameter	*pShape	= Parameters.Add_Shapes(
		pNode	, "ROI"			, _TL("Training Areas"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Table_Field(
		pShape	, "ROI_ID"		, _TL("Class Identifier"),
		_TL("")
	);

	Parameters.Add_FilePath(
		pNode	, "MODEL_SAVE"	, _TL("Store Model to File"),
		_TL(""),
		NULL, NULL, true
	);

	Parameters.Add_Choice(
		pNode	, "SVM_TYPE"	, _TL("SVM Type"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|"),
			_TL("C-SVC"),
			_TL("nu-SVC"),
			_TL("one-class SVM"),
			_TL("epsilon-SVR"),
			_TL("nu-SVR")
		), 0
	);

	Parameters.Add_Choice(
		pNode	, "KERNEL_TYPE"	, _TL("Kernel Type"),
		_TL("linear: u'*v\npolynomial: (gamma*u'*v + coef0)^degree\nradial basis function: exp(-gamma*|u-v|^2)\nsigmoid: tanh(gamma*u'*v + coef0)"),
		CSG_String::Format(SG_T("%s|%s|%s|%s|"),
			_TL("linear"),
			_TL("polynomial"),
			_TL("radial basis function"),
			_TL("sigmoid")
		), 2
	);

	Parameters.Add_Value(
		pNode	, "DEGREE"		, _TL("Degree"),
		_TL("degree in kernel function"),
		PARAMETER_TYPE_Int, 3
	);

	Parameters.Add_Value(
		pNode	, "GAMMA"		, _TL("Gamma"),
		_TL("gamma in kernel function"),
		PARAMETER_TYPE_Double	, 0
	);

	Parameters.Add_Value(
		pNode	, "COEF0"		, _TL("coef0"),
		_TL("coef0 in kernel function"),
		PARAMETER_TYPE_Double	, 0
	);

	Parameters.Add_Value(
		pNode	, "COST"		, _TL("C"),
		_TL("parameter C (cost) of C-SVC, epsilon-SVR, and nu-SVR"),
		PARAMETER_TYPE_Double	, 1
	);

	Parameters.Add_Value(
		pNode	, "NU"			, _TL("nu-SVR"),
		_TL("parameter nu of nu-SVC, one-class SVM, and nu-SVR"),
		PARAMETER_TYPE_Double	, 0.5
		);

	Parameters.Add_Value(
		pNode	, "EPS_SVR"		, _TL("SVR Epsilon"),
		_TL("epsilon in loss function of epsilon-SVR"),
		PARAMETER_TYPE_Double	, 0.1
	);

	Parameters.Add_Value(
		pNode	, "CACHE_SIZE"	, _TL("Cache Size"),
		_TL("cache memory size in MB"),
		PARAMETER_TYPE_Double	, 100
	);

	Parameters.Add_Value(
		pNode	, "EPS"			, _TL("Epsilon"),
		_TL("tolerance of termination criterion"),
		PARAMETER_TYPE_Double	, 1e-3
	);

	Parameters.Add_Value(
		pNode	, "SHRINKING"	, _TL("Shrinking"),
		_TL("whether to use the shrinking heuristics"),
		PARAMETER_TYPE_Bool		, false
	);

	Parameters.Add_Value(
		pNode	, "PROBABILITY"	, _TL("Probability Estimates"),
		_TL("whether to train a SVC or SVR model for probability estimates"),
		PARAMETER_TYPE_Bool		, false
	);

//	Parameters.Add_Value(pNode	, "nr_weight"		, _TL("Type"), _TL(""), PARAMETER_TYPE_Int	, 0);
//	Parameters.Add_Value(pNode	, "weight_label"	, _TL("Type"), _TL(""), PARAMETER_TYPE_Int	, NULL);
//	Parameters.Add_Value(pNode	, "weight"			, _TL("Type"), _TL(""), PARAMETER_TYPE_Int	, NULL);

	Parameters.Add_Value(
		pNode	, "CROSSVAL"	, _TL("Cross Validation"),
		_TL("n-fold cross validation: n must > 1"),
		PARAMETER_TYPE_Int		, 1, 1, true
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CSVM_Grids::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("MODEL_SRC")) )
	{
		pParameters->Get_Parameter("MODEL_TRAIN")->Set_Enabled(pParameter->asInt() == 0);
		pParameters->Get_Parameter("MODEL_LOAD" )->Set_Enabled(pParameter->asInt() == 1);
	}

	return( 1 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline double CSVM_Grids::Get_Value(int x, int y, int iGrid)
{
	CSG_Grid	*pGrid	= m_pGrids->asGrid(iGrid);

	switch( m_Scaling )
	{
	default:	return( (pGrid->asDouble(x, y)) );
	case  1:	return( (pGrid->asDouble(x, y) - pGrid->Get_ZMin()) / pGrid->Get_ZRange() );
	case  2:	return( (pGrid->asDouble(x, y) - pGrid->Get_ArithMean()) / pGrid->Get_StdDev() );
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSVM_Grids::On_Execute(void)
{
	//-----------------------------------------------------
	m_pModel	= NULL;

	m_pClasses	= Parameters("CLASSES")->asGrid();
	m_pClasses	->Set_NoData_Value(-1);
	m_pClasses	->Assign(0.0);

	m_Scaling	= Parameters("SCALING")->asInt();

	m_pGrids	= Parameters("GRIDS"  )->asGridList();

	for(int i=m_pGrids->Get_Count()-1; i>=0; i--)
	{
		if( m_pGrids->asGrid(i)->Get_ZRange() <= 0.0 )
		{
			Message_Add(CSG_String::Format(SG_T("%s: %s"), _TL("grid has been dropped"), m_pGrids->asGrid(i)->Get_Name()));

			m_pGrids->Del_Item(i);
		}
	}

	if( m_pGrids->Get_Count() <= 0 )
	{
		Error_Set(_TL("no valid grid in list."));

		return( false );
	}

	//-----------------------------------------------------
	svm_set_print_string_function(Parameters("MESSAGE")->asBool() ? SVM_Printf : NULL);

	//-------------------------------------------------
	m_Problem.y	= NULL;
	m_Problem.x	= NULL;
	m_Nodes		= NULL;

	switch( Parameters("MODEL_SRC")->asInt() )
	{
	case 0:	// create from training areas
		if( !Training() )
		{
			return( false );
		}
		break;

	case 1:	// restore from file
		if( !Load() )
		{
			return( false );
		}
		break;
	}

	//-----------------------------------------------------
	Predict();

	svm_free_and_destroy_model(&m_pModel);

	SG_FREE_SAFE(m_Problem.y);
	SG_FREE_SAFE(m_Problem.x);
	SG_FREE_SAFE(m_Nodes);

	Finalize();

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSVM_Grids::Predict(void)
{
	Process_Set_Text(_TL("prediction"));

	struct svm_node	*Features	= (struct svm_node *)SG_Malloc((m_pGrids->Get_Count() + 1) * sizeof(struct svm_node));

	Features[m_pGrids->Get_Count()].index	= -1;

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			if( !m_pClasses->is_NoData(x, y) )
			{
				for(int iGrid=0; iGrid<m_pGrids->Get_Count(); iGrid++)
				{
					Features[iGrid].index	= iGrid + 1;
					Features[iGrid].value	= Get_Value(x, y, iGrid);
				}

				m_pClasses->Set_Value(x, y, svm_predict(m_pModel, Features));
			}
		}
	}

	//-----------------------------------------------------
	SG_Free(Features);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSVM_Grids::Load(void)
{
	Process_Set_Text(_TL("restore model from file"));

	CSG_String	File	= Parameters("MODEL_LOAD")->asString();

	if( (m_pModel = svm_load_model(File)) == NULL )
	{
		Error_Set(CSG_String::Format(SG_T("%s [%s]"), _TL("could not open model file"), File.c_str()));

		return( false );
	}

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			bool	bNoData	= false;

			for(int iGrid=0; iGrid<m_pGrids->Get_Count() && !bNoData; iGrid++)
			{
				if( m_pGrids->asGrid(iGrid)->is_NoData(x, y) )
				{
					bNoData	= true;

					m_pClasses->Set_NoData(x, y);
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
bool CSVM_Grids::Training(void)
{
	CSG_Table				Elements;

	struct svm_parameter	param;

	//-----------------------------------------------------
	Process_Set_Text(_TL("create model from training areas"));

	if( !Training_Get_Elements(Elements) )
	{
		return( false );
	}

	if( !Training_Get_Parameters(param) )
	{
		return( false );
	}

	//-----------------------------------------------------
	m_Problem.l	= Elements.Get_Count();
	m_Problem.y	= (double           *)SG_Malloc(m_Problem.l * sizeof(double));
	m_Problem.x	= (struct svm_node **)SG_Malloc(m_Problem.l * sizeof(struct svm_node *));
	m_Nodes		= (struct svm_node  *)SG_Malloc(m_Problem.l * sizeof(struct svm_node  ) * (1 + m_pGrids->Get_Count()));

	//-----------------------------------------------------
	CSG_String	ID_ROI;

	m_Classes.Destroy();
	m_Classes.Add_Field(SG_T("NAME"), SG_DATATYPE_String);

	Elements.Set_Index(0, TABLE_INDEX_Ascending);

	for(int i=0, j=0, ID_Class=0; i<Elements.Get_Count(); i++, j++)
	{
		CSG_Table_Record	*pElement	= Elements.Get_Record_byIndex(i);

		if( ID_ROI.Cmp(pElement->asString(0)) )
		{
			ID_ROI		= pElement->asString(0);
			ID_Class	++;

			m_Classes.Add_Record()->Set_Value(0, pElement->asString(0));
		}

		m_Problem.x[i]	= &m_Nodes[j];
		m_Problem.y[i]	= ID_Class;

		for(int iGrid=0; iGrid<m_pGrids->Get_Count(); iGrid++, j++)
		{
			m_Nodes[j].index	= 1 + iGrid;
			m_Nodes[j].value	= pElement->asDouble(1 + iGrid);
		}

		m_Nodes[j].index	= -1;
	}

	/*/-----------------------------------------------------
	if( param.kernel_type == PRECOMPUTED )
	{
		for(i=0; i<m_Problem.l; i++)
		{
			if( m_Problem.x[i][0].index != 0 )
			{
				Error_Set("Wrong input format: first column must be 0:sample_serial_number");

				return( false );
			}

			if ((int)m_Problem.x[i][0].value <= 0 || (int)m_Problem.x[i][0].value > max_index)
			{
				Error_Set("Wrong input format: sample_serial_number out of range");

				return( false );
			}
		}
	}/**/

	//-----------------------------------------------------
	const char *error_msg	= svm_check_parameter(&m_Problem, &param);

	if( error_msg )
	{
		Error_Set(_TL("training failed"));
		Error_Set(error_msg);
	}
	else if( (m_pModel = svm_train(&m_Problem, &param)) != NULL )
	{
		//-------------------------------------------------
		CSG_String	File	= Parameters("MODEL_SAVE")->asString();

		if( File.Length() > 0 && svm_save_model(File, m_pModel) )
		{
			Error_Set(CSG_String::Format(SG_T("%s [%s]"), _TL("could not save model to file"), File.c_str()));
		}

		//-------------------------------------------------
		if( Parameters("CROSSVAL")->asInt() > 1 )
		{
			CSG_String	Summary;

			double *target = (double *)SG_Malloc(m_Problem.l * sizeof(double));

			svm_cross_validation(&m_Problem, &param, Parameters("CROSSVAL")->asInt(), target);

			if( param.svm_type == EPSILON_SVR || param.svm_type == NU_SVR )
			{
				double	total_error = 0.0, sum_v = 0.0, sum_y = 0.0, sum_vv = 0.0, sum_yy = 0.0, sum_vy = 0.0;

				for(int i=0; i<m_Problem.l; i++)
				{
					double	y	= m_Problem.y[i];
					double	v	= target[i];

					total_error	+= (v - y) * (v - y);
					sum_v		+= v;
					sum_y		+= y;
					sum_vv		+= v * v;
					sum_yy		+= y * y;
					sum_vy		+= v * y;
				}

				Summary	 = CSG_String::Format(SG_T("%s\n\t%s: %g\n"),
					_TL("Cross Validation"),
					_TL("Mean Squared Error"),
					total_error / m_Problem.l
				);

				Summary	+= CSG_String::Format(SG_T("\t%s: %g\n"),
					_TL("Squared Correlation Coefficient"),
					((m_Problem.l * sum_vy - sum_v * sum_y) * (m_Problem.l * sum_vy - sum_v * sum_y))
				/	((m_Problem.l * sum_vv - sum_v * sum_v) * (m_Problem.l * sum_yy - sum_y * sum_y))
				);
			}
			else
			{
				int	total_correct	= 0;

				for(int i=0; i<m_Problem.l; i++)
				{
					if( target[i] == m_Problem.y[i] )
					{
						++total_correct;
					}
				}

				Summary	 = CSG_String::Format(SG_T("%s\n\t%s: %g%%\n"),
					_TL("Cross Validation"),
					_TL("Accuracy"),
					100.0 * total_correct / m_Problem.l
				);
			}

			free(target);
		}
	}

	//-----------------------------------------------------
	svm_destroy_param(&param);

	return( m_pModel != NULL );
}

//---------------------------------------------------------
bool CSVM_Grids::Training_Get_Parameters(struct svm_parameter &param)
{
	param.svm_type		= Parameters("SVM_TYPE"   )->asInt   ();
	param.kernel_type	= Parameters("KERNEL_TYPE")->asInt   ();
	param.degree		= Parameters("DEGREE"     )->asInt   ();
	param.gamma			= Parameters("GAMMA"      )->asDouble();
	param.coef0			= Parameters("COEF0"      )->asDouble();
	param.nu			= Parameters("NU"         )->asDouble();
	param.cache_size	= Parameters("CACHE_SIZE" )->asDouble();
	param.C				= Parameters("COST"       )->asDouble();
	param.eps			= Parameters("EPS"        )->asDouble();
	param.p				= Parameters("EPS_SVR"    )->asDouble();
	param.shrinking		= Parameters("SHRINKING"  )->asBool  () ? 1 : 0;
	param.probability	= Parameters("PROBABILITY")->asBool  () ? 1 : 0;

	param.nr_weight		= 0;
	param.weight_label	= NULL;
	param.weight		= NULL;

	if( param.gamma == 0 && m_pGrids->Get_Count() > 0 )
	{
		param.gamma	= 1.0 / m_pGrids->Get_Count();
	}

	return( true );
}

//---------------------------------------------------------
bool CSVM_Grids::Training_Get_Elements(CSG_Table &Elements)
{
	int			iROI_ID, iGrid;
	CSG_Shapes	*pROIs;

	//-----------------------------------------------------
	pROIs	= Parameters("ROI"   )->asShapes();
	iROI_ID	= Parameters("ROI_ID")->asInt();

	Elements.Destroy();
	Elements.Add_Field(SG_T("ID"), SG_DATATYPE_String);

	for(iGrid=0; iGrid<m_pGrids->Get_Count(); iGrid++)
	{
		Elements.Add_Field(SG_Get_String(iGrid, 0), SG_DATATYPE_Double);
	}

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		double	p_y	= Get_YMin() + y * Get_Cellsize();

	//	#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			bool	bNoData	= false;

			for(iGrid=0; iGrid<m_pGrids->Get_Count() && !bNoData; iGrid++)
			{
				if( m_pGrids->asGrid(iGrid)->is_NoData(x, y) )
				{
					bNoData	= true;
				}
			}

			if( bNoData )
			{
				m_pClasses->Set_NoData(x, y);
			}
			else
			{
				double	p_x		= Get_XMin() + x * Get_Cellsize();

				for(int iROI=0; iROI<pROIs->Get_Count(); iROI++)
				{
					CSG_Shape_Polygon	*pROI	= (CSG_Shape_Polygon *)pROIs->Get_Shape(iROI);

					if( pROI->Contains(p_x, p_y) )
					{
						CSG_Table_Record	*pElement	= Elements.Add_Record();

						pElement->Set_Value(0, pROI->asString(iROI_ID));

						for(iGrid=0; iGrid<m_pGrids->Get_Count(); iGrid++)
						{
							pElement->Set_Value(1 + iGrid, Get_Value(x, y, iGrid));
						}
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	if( Elements.Get_Count() <= 0 )
	{
		Error_Set(_TL("no training areas could be analysed."));

		return( false );
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSVM_Grids::Finalize(void)
{
	//-----------------------------------------------------
	CSG_Parameters	P;

	if( DataObject_Get_Parameters(m_pClasses, P) && P("COLORS_TYPE") && P("LUT") )
	{
		CSG_Table	*pTable	= P("LUT")->asTable();

		for(int iClass=0; iClass<m_Classes.Get_Count(); iClass++)
		{
			CSG_Table_Record	*pRecord	= pTable->Get_Record(iClass);

			if( pRecord == NULL )
			{
				pRecord	= pTable->Add_Record();
				pRecord->Set_Value(0, SG_GET_RGB(rand() * 255.0 / RAND_MAX, rand() * 255.0 / RAND_MAX, rand() * 255.0 / RAND_MAX));
			}

			pRecord->Set_Value(1, m_Classes[iClass].asString(0));
			pRecord->Set_Value(2, m_Classes[iClass].asString(0));
			pRecord->Set_Value(3, iClass + 1);
			pRecord->Set_Value(4, iClass + 1);
		}

		while( pTable->Get_Record_Count() > m_Classes.Get_Count() )
		{
			pTable->Del_Record(pTable->Get_Record_Count() - 1);
		}

		P("COLORS_TYPE")->Set_Value(1);	// Color Classification Type: Lookup Table

		DataObject_Set_Parameters(m_pClasses, P);
	}

	//-----------------------------------------------------
	m_pClasses->Set_Name(CSG_String::Format(SG_T("%s [%s]"), _TL("Classification"), _TL("SVM")));

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
