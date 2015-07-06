/**********************************************************
 * Version $Id: OpenCVNNet.cpp 0001 2013-04-26            
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                       OpenCVNNet                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                     OpenCVNNet.cpp                    //
//                                                       //
//														 //
//                      Luca Piras                       //
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
//    e-mail:     3piras@informatik.uni-hamburg.de       //
//                                                       //
//    contact:    Luca Piras                             //
//                Rellinger Str. 39                      //
//                20257 Hamburg                          //
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
#include "opencv_nnet.h"
#include <opencv/cxcore.h>
#include <opencv/ml.h>

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
COpenCV_NNet::COpenCV_NNet(void)
{
	CSG_Parameter	*pNodeTrainData, *pNodeTrainAreas, *pNodeTopology, *pNodeActFun, *pNodeRPROP, *pNodeBPROP, *pNodeData;

	Set_Name		(_TL("Neural Networks (OpenCV)"));

	Set_Author		(SG_T("Luca Piras"));

	Set_Description	(_TW(
		"Integration of the OpenCV Neural Networks library.\n"
		"http://docs.opencv.org/modules/ml/doc/neural_networks.html"
	));

	#pragma region
	//---------------------------------------------------------
	pNodeTrainData = Parameters.Add_Table(
		NULL, "TRAIN_INPUT_TABLE", _TL("Train INPUT"),
		_TL("Input data to train the network."), PARAMETER_INPUT
	);

	Parameters.Add_Parameters(
		pNodeTrainData	, "TRAIN_FEATURES_TABLE", _TL("Select training output features"),
		_TL("Select the features that should be used as the OUTPUT features.")
	);

	Parameters.Add_Table(
		NULL, "EVAL_INPUT_TABLE", _TL("Prediction INPUT"),
		_TL("Input data for the trained network."), PARAMETER_INPUT
	);

	Parameters.Add_Table(
		NULL, "EVAL_OUTPUT_TABLE", _TL("OUTPUT"),
		_TL("Predicted responses (probabilities) for corresponding samples."), PARAMETER_OUTPUT
	);
	#pragma endregion Table INPUT and OUTPUT section

	#pragma region
	//---------------------------------------------------------
	Parameters.Add_Grid_List(NULL ,"TRAIN_INPUT_GRIDS", _TL("Train INPUT"),
		_TL("Input grids to train the network."),
		PARAMETER_INPUT
	);

	pNodeTrainAreas	= Parameters.Add_Shapes(
		NULL, "TRAIN_INPUT_AREAS" , _TL("Select training areas"),
		_TL("The areas used to train the neural network."),
		PARAMETER_INPUT, SHAPE_TYPE_Polygon
	);

	// TODO: Have to make a vector representation from the class identifier
	Parameters.Add_Table_Field(
		pNodeTrainAreas	, "TRAIN_INPUT_AREAS_CLASS_FIELD", _TL("Select class identifier"),
		_TL("The class field used to classify the shape. All classes will be vectorized.")
	);

	Parameters.Add_Grid(
		NULL, "EVAL_OUTPUT_GRID_CLASSES", _TL("OUTPUT classes"),
		_TL("These are the winner class vectors."),
		PARAMETER_OUTPUT, true, SG_DATATYPE_Short
	);

	Parameters.Add_Grid(
		NULL, "EVAL_OUTPUT_GRID_CERTAINTY", _TL("OUTPUT certainty"),
		_TL("These are the probabilities for the winner classes given by the neural network."),
		PARAMETER_OUTPUT_OPTIONAL
	);
	#pragma endregion Grid INPUT and OUTPUT section

	#pragma region
	//---------------------------------------------------------
	// Let the user select on which type of data we will operate
	Parameters.Add_Choice(
		NULL	, "DATA_TYPE" , _TL("Data type"),
		_TL("Choose the type of data used in this model."),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("Table"),	
			_TL("Grid")
		)
	);

	pNodeTopology = Parameters.Add_Node(
		NULL, "NETWORK_TOPOLOGY", _TL("Network topology"),
		_TL("In this section you can define the topology of the neural network.")
	);

	Parameters.Add_Value(
		pNodeTopology	, "NNET_LAYER"	, _TL("Number of layers"),
		_TL("You can specify the number of layers in the network (including input and output layer)."),
		PARAMETER_TYPE_Int, 3, 1, true
	);

	Parameters.Add_Value(
		pNodeTopology	, "NNET_NEURONS"	, _TL("Number of neurons"),
		_TL("You can specify the number of neurons in each layer of the network."),
		PARAMETER_TYPE_Int, 3, 1, true
	);

	Parameters.Add_Value(
		NULL	, "MAX_ITER"	, _TL("Maximum number of iterations"),
		_TL("Termination criteria of the training algorithm. You can specify the maximum number of iterations."),
		PARAMETER_TYPE_Int, 1000
	);

	Parameters.Add_Value(
		NULL	, "EPSILON"	, _TL("Error change (epsilon)"),
		_TL("Termination criteria of the training algorithm. You can specify how much the error could change between the iterations to make the algorithm continue (epsilon)."),
		PARAMETER_TYPE_Double, 0.01
	);

	pNodeActFun =  Parameters.Add_Choice(
		NULL	, "ACTIVATION_FUNCTION"		, _TL("Activation function"),
		_TL("Choose the activation function used by the neronal network."),
		CSG_String::Format(SG_T("%s|%s|%s|"),
			_TL("Identity"),	
			_TL("Sigmoid"),
			_TL("Gaussian")
		), 1
	);
 
	Parameters.Add_Value(
		pNodeActFun	, "ALPHA"	, _TL("Alpha"),
		_TL("This is the alpha parameter value for the sigmoid function."),
		PARAMETER_TYPE_Double, 1
	);

	Parameters.Add_Value(
		pNodeActFun	, "BETA"	, _TL("Beta"),
		_TL("This is the beta parameter value for the sigmoid function."),
		PARAMETER_TYPE_Double, 1
	);

	Parameters.Add_Choice(
		NULL	, "TRAINING_METHOD"		, _TL("Training method"),
		_TL("Choose the training method used for the neronal network."),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("Resilent propagation (RPROP)"),	
			_TL("Backpropagation algorithm")
		), 1
	);

	pNodeRPROP = Parameters.Add_Node(
		NULL, "RPROP_PARAMS", _TL("RPROP parameters"),
		_TL("This section contains all parameters used by the RPROP algorithm.")
	);

	Parameters.Add_Value(
		pNodeRPROP	, "RP_DW0"	, _TL("Initial update value"),
		_TL("Initial value delta_0 of update-values delta_ij."),
		PARAMETER_TYPE_Double, 0
	);

	Parameters.Add_Value(
		pNodeRPROP	, "RP_DW_PLUS"	, _TL("Increase factor"),
		_TL("Increase factor n_plus. Must be > 1."),
		PARAMETER_TYPE_Double, 1.2, 1.01, true
	);

	Parameters.Add_Value(
		pNodeRPROP	, "RP_DW_MINUS"	, _TL("Decrease factor"),
		_TL("Decrease factor n_minus. Must be < 1."),
		PARAMETER_TYPE_Double, 0.5, 0.01, true, 0.99, true
	);

	Parameters.Add_Value(
		pNodeRPROP, "RP_DW_MIN"	, _TL("Update values lower limit"),
		_TL("Corresponds to delta_min. Must be > 0"),
		PARAMETER_TYPE_Double, 0.1, 0.01, true
	);

	Parameters.Add_Value(
		pNodeRPROP	, "RP_DW_MAX"	, _TL("Update values upper limit"),
		_TL("Corresponds to delta_max. Must be > 1"),
		PARAMETER_TYPE_Double, 1.1, 1.01, true
	);

	pNodeBPROP = Parameters.Add_Node(
		NULL, "BPROP_PARAMS", _TL("Backpropagation parameters"),
		_TL("This section contains all parameters used by the backpropagation algorithm.")
	);

	Parameters.Add_Value(
		pNodeBPROP	, "BP_DW_SCALE"	, _TL("Weight gradient term"),
		_TL("Strength of the weight gradient term. The recommended value is about 0.1."),
		PARAMETER_TYPE_Double, 0.1
	);

	Parameters.Add_Value(
		pNodeBPROP	, "BP_MOMENT_SCALE"	, _TL("Moment term"),
		_TL("Strength of the momentum term (the difference between weights on the 2 previous iterations). This parameter provides some inertia to smooth the random fluctuations of the weights. It can vary from 0 (the feature is disabled) to 1 and beyond. The value 0.1 or so is good enough."),
		PARAMETER_TYPE_Double, 0.1
	);

	pNodeData = Parameters.Add_Node(
		NULL, "DATA", _TL("Data options"),
		_TL("Specifie options used for the training and test data.")
	);

	Parameters.Add_Table(
		pNodeData, "WEIGHTS", _TL("Sample weights"),
		_TL("Optional floating-point vector of weights for each sample. Some samples may be more important than others for training."), PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Table(
		pNodeData, "INDICES", _TL("Indices"),
		_TL("Optional integer vector indicating the samples (rows of inputs and outputs) that are taken into account."), PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Value(
		pNodeData	, "UPDATE_WEIGHTS"	, _TL("Update the network weights"),
		_TL(""),
		PARAMETER_TYPE_Bool, false
	);

	Parameters.Add_Value(
		pNodeData	, "NO_INPUT_SCALE"	, _TL("No INPUT vector normalization"),
		_TL("If false, the training algorithm normalizes each input feature independently, shifting its mean value to 0 and making the standard deviation equal to 1."),
		PARAMETER_TYPE_Bool, true
	);

	Parameters.Add_Value(
		pNodeData	, "NO_OUTPUT_SCALE"	, _TL("No OUTPUT vector normalization"),
		_TL("If false, the training algorithm normalizes each output feature independently, by transforming it to the certain range depending on the used activation function."),
		PARAMETER_TYPE_Bool, true
	);
	#pragma endregion NNet options
}



///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool COpenCV_NNet::On_Execute(void)
{
	//-------------------------------------------------
	bool					b_updateWeights, b_noInputScale, b_noOutputScale, b_NoData;
	int						i_matType, i_layers, i_maxIter, i_neurons, i_areasClassId, i_trainFeatTotalCount, *i_outputFeatureIdxs, i_outputFeatureCount, i_Grid, x, y, i_evalOut, i_winner;
	double					d_alpha, d_beta, d_eps;
	DATA_TYPE				e_dataType;
	TRAINING_METHOD			e_trainMet;
	ACTIVATION_FUNCTION		e_actFunc;
	CSG_Table				*t_Weights, *t_Indices, *t_TrainInput, *t_EvalInput, *t_EvalOutput;
	CSG_Parameter_Grid_List	*gl_TrainInputs;
	CSG_Grid				*g_EvalOutput, *g_EvalOutputCert;
	CSG_Shapes				*s_TrainInputAreas;
	CSG_Parameters			*p_TrainFeatures;
	TSG_Point				p;
	CvMat					*mat_Weights, *mat_Indices, **mat_data, *mat_neuralLayers, mat_layerSizesSub, *mat_EvalInput, *mat_EvalOutput;	// todo: mat_indices to respect input indices, mat_weights for initialization
	CvANN_MLP_TrainParams	tp_trainParams;
	CvANN_MLP				model;

	b_updateWeights		= Parameters("UPDATE_WEIGHTS"							)->asBool();
	b_noInputScale		= Parameters("NO_INPUT_SCALE"							)->asBool();
	b_noOutputScale		= Parameters("NO_OUTPUT_SCALE"							)->asBool();
	i_layers			= Parameters("NNET_LAYER"								)->asInt();
	i_neurons			= Parameters("NNET_NEURONS"								)->asInt();
	i_maxIter			= Parameters("MAX_ITER"									)->asInt();
	i_areasClassId		= Parameters("TRAIN_INPUT_AREAS_CLASS_FIELD"			)->asInt();
	e_dataType			= (DATA_TYPE)Parameters("DATA_TYPE"						)->asInt();
	e_trainMet			= (TRAINING_METHOD)Parameters("TRAINING_METHOD"			)->asInt();
	e_actFunc			= (ACTIVATION_FUNCTION)Parameters("ACTIVATION_FUNCTION"	)->asInt();
	d_alpha				= Parameters("ALPHA"									)->asDouble();
	d_beta				= Parameters("BETA"										)->asDouble();
	d_eps				= Parameters("EPSILON"									)->asDouble();
	t_Weights			= Parameters("WEIGHTS"									)->asTable();
	t_Indices			= Parameters("INDICES"									)->asTable();
	t_TrainInput		= Parameters("TRAIN_INPUT_TABLE"						)->asTable();
	t_EvalInput			= Parameters("EVAL_INPUT_TABLE"							)->asTable();
	t_EvalOutput		= Parameters("EVAL_OUTPUT_TABLE"						)->asTable();
	p_TrainFeatures		= Parameters("TRAIN_FEATURES_TABLE"						)->asParameters();
	gl_TrainInputs		= Parameters("TRAIN_INPUT_GRIDS"						)->asGridList();
	g_EvalOutput		= Parameters("EVAL_OUTPUT_GRID_CLASSES"					)->asGrid();
	g_EvalOutputCert	= Parameters("EVAL_OUTPUT_GRID_CERTAINTY"				)->asGrid();
	s_TrainInputAreas	= Parameters("TRAIN_INPUT_AREAS"						)->asShapes();

	// Fixed matrix type (TODO: Analyze what to do for other types of data (i.e. images))
	i_matType = CV_32FC1;

	//-------------------------------------------------
	if (e_dataType == TABLE)
	{	
		// We are working with TABLE data
		if( t_TrainInput->Get_Count() == 0 || p_TrainFeatures->Get_Count() == 0 )
		{
			Error_Set(_TL("Select an input table and at least one output feature!"));
			return( false );
		}

		// Count the total number of available features
		i_trainFeatTotalCount = t_TrainInput->Get_Field_Count();

		// Count the number of selected output features
		i_outputFeatureIdxs = (int *)SG_Calloc(i_trainFeatTotalCount, sizeof(int));
		i_outputFeatureCount = 0;
	
		for(int i=0; i<p_TrainFeatures->Get_Count(); i++)
		{
			if( p_TrainFeatures->Get_Parameter(i)->asBool() )
			{
				i_outputFeatureIdxs[i_outputFeatureCount++] = CSG_String(p_TrainFeatures->Get_Parameter(i)->Get_Identifier()).asInt();
			}
		}

		// Update the number of training features
		i_trainFeatTotalCount = i_trainFeatTotalCount-i_outputFeatureCount;

		if( i_outputFeatureCount <= 0 )
		{
			Error_Set(_TL("Select at least one output feature!"));
			return( false );
		}

		// Now convert the input and output training data into a OpenCV matrix objects
		mat_data = GetTrainAndOutputMatrix(t_TrainInput, i_matType, i_outputFeatureIdxs, i_outputFeatureCount);
	}
	else
	{
		// TODO: Add some grid validation logic
		i_trainFeatTotalCount = gl_TrainInputs->Get_Count();
		i_outputFeatureCount = s_TrainInputAreas->Get_Count();

		// Convert the data from the grid into the matrix from
		mat_data = GetTrainAndOutputMatrix(gl_TrainInputs, i_matType, s_TrainInputAreas, i_areasClassId, g_EvalOutput, g_EvalOutputCert);
	}

	//-------------------------------------------------
	// Add two additional layer to the network topology (0-th layer for input and the last as the output)
	i_layers = i_layers + 2;
	mat_neuralLayers = cvCreateMat(i_layers, 1, CV_32SC1);
	cvGetRows(mat_neuralLayers, &mat_layerSizesSub, 0, i_layers);
	
	//Setting the number of neurons on each layer
	for (int i = 0; i < i_layers; i++)
	{
		if (i == 0)
		{
			// The first layer needs the same size (number of nerons) as the number of columns in the training data
			cvSet1D(&mat_layerSizesSub, i, cvScalar(i_trainFeatTotalCount));
		}
		else if (i == i_layers-1)
		{
			// The last layer needs the same size (number of neurons) as the number of output columns
			cvSet1D(&mat_layerSizesSub, i, cvScalar(i_outputFeatureCount));
		}
		else
		{
			// On every other layer set the layer size selected by the user
			cvSet1D(&mat_layerSizesSub, i, cvScalar(i_neurons));	
		}
	}

	//-------------------------------------------------
	// Create the training params object
	tp_trainParams = CvANN_MLP_TrainParams();
	tp_trainParams.term_crit = cvTermCriteria(CV_TERMCRIT_ITER + CV_TERMCRIT_EPS, i_maxIter, d_eps);

	// Check which training method was selected and set corresponding params
	if(e_trainMet == RPROP)
	{
		// Set all RPROP specific params
		tp_trainParams.train_method = CvANN_MLP_TrainParams::RPROP;
		tp_trainParams.rp_dw0 = Parameters("RP_DW0"				)->asDouble();
		tp_trainParams.rp_dw_plus = Parameters("RP_DW_PLUS"		)->asDouble();
		tp_trainParams.rp_dw_minus = Parameters("RP_DW_MINUS"	)->asDouble();
		tp_trainParams.rp_dw_min = Parameters("RP_DW_MIN"		)->asDouble();
		tp_trainParams.rp_dw_max = Parameters("RP_DW_MAX"		)->asDouble();
	}
	else
	{
		// Set all BPROP specific params
		tp_trainParams.train_method = CvANN_MLP_TrainParams::BACKPROP;
		tp_trainParams.bp_dw_scale = Parameters("BP_DW_SCALE"			)->asDouble();
		tp_trainParams.bp_moment_scale = Parameters("BP_MOMENT_SCALE"	)->asInt();
	}
	
	//-------------------------------------------------
	// Create the model (depending on the activation function)
	if(e_actFunc == SIGMOID)
	{
		model.create(mat_neuralLayers);
	}
	else
	{
		model.create(mat_neuralLayers, CvANN_MLP::GAUSSIAN, d_alpha, d_beta);
	}

	//-------------------------------------------------
	// Now train the network

	// TODO: Integrate init weights and indicies for record selection
	// mat_Weights  = GetMatrix(t_Weights, i_matType);
	// mat_Indices = GetMatrix(t_Indices, i_matType);
	
	//model.train(mat_TrainInput, mat_TrainOutput, NULL, NULL, tp_trainParams);
	model.train(mat_data[0], mat_data[1], NULL, NULL, tp_trainParams);

	//-------------------------------------------------
	// Predict data
	if (e_dataType == TABLE)
	{
		// Get the eavaluation/test matrix from the eval table
		mat_EvalInput = GetEvalMatrix(t_EvalInput, i_matType);
	}
	else
	{
		// Train and eval data overlap in grid mode
		mat_EvalInput = GetEvalMatrix(gl_TrainInputs, i_matType);
	}

	// Prepare output matrix
	mat_EvalOutput = cvCreateMat(mat_EvalInput->rows, i_outputFeatureCount, i_matType);

	// Start prediction
	model.predict(mat_EvalInput, mat_EvalOutput);

	Message_Add(_TL("Successfully trained the network and predicted the values. Here comes the output."));
	
	//-------------------------------------------------
	// Save and print results
	if (e_dataType == TABLE)
	{
		// DEBUG -> Save results to output table and print results
		for (int i = 0; i < i_outputFeatureCount; i++)
		{
			t_EvalOutput->Add_Field(CSG_String(t_TrainInput->Get_Field_Name(i_outputFeatureIdxs[i])), SG_DATATYPE_Float);
		}
	
		for (int i = 0; i < mat_EvalOutput->rows; i++)
		{
			CSG_Table_Record* tr_record = t_EvalOutput->Add_Record();

			for (int j = 0; j < i_outputFeatureCount; j++)
			{
				float f_targetValue = mat_EvalOutput->data.fl[i*i_outputFeatureCount+j];
				tr_record->Set_Value(j, f_targetValue);
			}
		}
	}
	else
	{
		// Fill the output table output
		for (int i = 0; i < i_outputFeatureCount; i++)
		{
			// TODO: Get the class name
			t_EvalOutput->Add_Field(CSG_String::Format(SG_T("CLASS_%d"), i), SG_DATATYPE_Float);
		}
	
		for (int i = 0; i < mat_EvalOutput->rows; i++)
		{
			CSG_Table_Record* tr_record = t_EvalOutput->Add_Record();

			for (int j = 0; j < i_outputFeatureCount; j++)
			{
				float f_targetValue = mat_EvalOutput->data.fl[i*i_outputFeatureCount+j];
				tr_record->Set_Value(j, f_targetValue);
			}
		}

		i_evalOut = 0;

		// Fill the output grid
		for(y=0, p.y=Get_YMin(); y<Get_NY() && Set_Progress(y); y++, p.y+=Get_Cellsize())
		{
			for(x=0, p.x=Get_XMin(); x<Get_NX(); x++, p.x+=Get_Cellsize())
			{
				for(i_Grid=0, b_NoData=false; i_Grid<gl_TrainInputs->Get_Count() && !b_NoData; i_Grid++)
				{
					// If there is one grid that has no data in this point p, then set the no data flag
					if( gl_TrainInputs->asGrid(i_Grid)->is_NoData(x, y) )
					{
						b_NoData = true;
					}
				}

				if (!b_NoData)
				{
					// We have data in all grids, so this is a point that was predicted
					// Get the winner class for this point and set it to the output grid
					float f_targetValue = 0;

					for (int j = 0; j < i_outputFeatureCount; j++)
					{
						if (mat_EvalOutput->data.fl[i_evalOut*i_outputFeatureCount+j] > f_targetValue)
						{
							// The current value is higher than the last one, so lets memorize the current class
							f_targetValue = mat_EvalOutput->data.fl[i_evalOut*i_outputFeatureCount+j];
							i_winner = j;
						}
					}

					// Now finally set the values to the grids
					g_EvalOutput->Set_Value(x, y, i_winner);
					g_EvalOutputCert->Set_Value(x, y, f_targetValue);

					i_evalOut++;
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
int COpenCV_NNet::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("ACTIVATION_FUNCTION")) )
	{
		ACTIVATION_FUNCTION e_actFunc = (ACTIVATION_FUNCTION)pParameter->asInt();

		pParameters->Get_Parameter("ALPHA")->Set_Enabled(e_actFunc == SIGMOID);
		pParameters->Get_Parameter("BETA" )->Set_Enabled(e_actFunc == SIGMOID);
	}

	if(	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("TRAINING_METHOD")) )
	{
		TRAINING_METHOD e_trainMet = (TRAINING_METHOD)pParameter->asInt();

		pParameters->Get_Parameter("RPROP_PARAMS"	)->Set_Enabled(e_trainMet == RPROP);
		pParameters->Get_Parameter("WEIGHTS"		)->Set_Enabled(e_trainMet == RPROP);
		pParameters->Get_Parameter("BPROP_PARAMS"	)->Set_Enabled(e_trainMet == BPROP);
	}

	if(	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("DATA_TYPE")) )
	{ 
		DATA_TYPE e_dataType = (DATA_TYPE)pParameter->asInt();

		pParameters->Get_Parameter("PARAMETERS_GRID_SYSTEM"	)->Set_Enabled(e_dataType == GRID);
		pParameters->Get_Parameter("TRAIN_INPUT_AREAS"		)->Set_Enabled(e_dataType == GRID);

		pParameters->Get_Parameter("TRAIN_INPUT_TABLE"		)->Set_Enabled(e_dataType == TABLE);
		pParameters->Get_Parameter("EVAL_INPUT_TABLE"		)->Set_Enabled(e_dataType == TABLE);
		pParameters->Get_Parameter("EVAL_OUTPUT_TABLE"		)->Set_Enabled(e_dataType == TABLE);
		pParameters->Get_Parameter("TRAIN_FEATURES_TABLE"	)->Set_Enabled(e_dataType == TABLE);
		
	}

	//-----------------------------------------------------
	return( 1 );
}

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int COpenCV_NNet::On_Parameter_Changed(CSG_Parameters *p_Parameters, CSG_Parameter *p_Parameter)
{
	if(!SG_STR_CMP(p_Parameter->Get_Identifier(), SG_T("TRAIN_INPUT_TABLE")) )
    {
        CSG_Table*		t_TrainInput = p_Parameter->asTable();
		CSG_Parameters*	p_TrainFeatures = p_Parameters->Get_Parameter("TRAIN_FEATURES_TABLE")->asParameters();
		
		p_TrainFeatures->Del_Parameters();

		if (t_TrainInput && t_TrainInput->Get_Field_Count() > 0)
		{
			int i_fieldCount = t_TrainInput->Get_Field_Count();

			for (int i = 0; i < i_fieldCount; i++)
			{
				if( SG_Data_Type_is_Numeric(t_TrainInput->Get_Field_Type(i)) )
                {
					// Add a bool choice for every numeric field to mark it as an possible output feature
                    p_TrainFeatures->Add_Value(NULL, CSG_String::Format(SG_T("%d_FEATURE"), i), t_TrainInput->Get_Field_Name(i), _TL(""), PARAMETER_TYPE_Bool, false);
                }
			}
		}
    }

    //-----------------------------------------------------
    return( 1 );
}

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Vector COpenCV_NNet::GetClassVectorByName(CSG_Shapes *s_areas, const SG_Char *s_class, int i_classId)
{
	CSG_Vector	v_outVec(s_areas->Get_Count());
	v_outVec.Set_Zero();

	for (int i = 0; i < s_areas->Get_Count(); i++)
	{
		if (!SG_STR_CMP(s_class,s_areas->Get_Shape(i)->asString(i_classId)))
		{
			v_outVec[i] = 1;
		}
	}
	return v_outVec;
}

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CvMat* COpenCV_NNet::GetEvalMatrix(CSG_Table *t_Data, int i_type)
{
	double				d_value;
	CSG_Table_Record	*tr_Record;
	CvMat				*mat;

	mat = cvCreateMat(t_Data->Get_Count(), t_Data->Get_Field_Count(), i_type);

	for(int i=0; i<t_Data->Get_Count(); i++)
	{
		tr_Record = t_Data->Get_Record(i);

		for(int j=0; j<t_Data->Get_Field_Count(); j++)
		{
			d_value = tr_Record->Get_Value(j)->asDouble();
			cvSetReal2D(mat, i, j, (float)d_value);
		}
	}
	return mat;
}

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CvMat* COpenCV_NNet::GetEvalMatrix(CSG_Parameter_Grid_List *gl_grids, int type)
{
	bool				b_NoData;
	int					x,y,i_Grid;
	CSG_Table			*t_data;
	CSG_Table_Record	*tr_rec;
	TSG_Point			p;
	CvMat				*mat;

	// We will use this table as a temporary data store,
	// since we cannot dynamically resize the CvMat
	t_data = new CSG_Table();
	
	// We need a column for each grid and the output lable
	for (int i = 0; i < gl_grids->Get_Count(); i++)
	{
		t_data->Add_Field(CSG_String::Format(SG_T("GRID_%d"), i), SG_DATATYPE_Float, i); 
	}

	// Traverse all grids, every point
	for(y=0, p.y=Get_YMin(); y<Get_NY() && Set_Progress(y); y++, p.y+=Get_Cellsize())
	{
		for(x=0, p.x=Get_XMin(); x<Get_NX(); x++, p.x+=Get_Cellsize())
		{
			for(i_Grid=0, b_NoData=false; i_Grid<gl_grids->Get_Count() && !b_NoData; i_Grid++)
			{
				// If there is one grid that has no data in this point p, then set the no data flag
				if( gl_grids->asGrid(i_Grid)->is_NoData(x, y) )
				{
					b_NoData = true;
				}
			}

			if (!b_NoData)
			{
				// We have data in all grids, so lets add them to the eval data table
				tr_rec = t_data->Add_Record();
				for(i_Grid=0; i_Grid<gl_grids->Get_Count(); i_Grid++)
				{
					tr_rec->Set_Value(i_Grid, (float) gl_grids->asGrid(i_Grid)->asFloat(x, y));
				}				
			}
		}
	}

	// Now create the matrix and add all data from the table to the matrix
	mat = GetEvalMatrix(t_data, type);

	return mat;
}

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CvMat** COpenCV_NNet::GetTrainAndOutputMatrix(CSG_Table *t_trainData, int type, int *i_outputFeatureIdxs, int i_outputFeatureCount)
{
	bool				b_isOutputFeature;
	int					i_idxCorrection;
	double				d_value;
	CvMat				**mat_array;
	CvMat				*mat_trainData;
	CvMat				*mat_outputData;
	CSG_Table_Record	*tr_Record;
	
	mat_trainData = cvCreateMat(t_trainData->Get_Count(), t_trainData->Get_Field_Count()-i_outputFeatureCount, type);
	mat_outputData = cvCreateMat(t_trainData->Get_Count(), i_outputFeatureCount, type);

	for(int i=0; i<t_trainData->Get_Count(); i++)
	{
		tr_Record = t_trainData->Get_Record(i);

		i_idxCorrection = 0;
		for(int j=0; j<t_trainData->Get_Field_Count(); j++)
		{
			// Get the current value from the input table
			d_value = tr_Record->Get_Value(j)->asDouble();
			b_isOutputFeature = false;

			for (int u = 0; u < i_outputFeatureCount; u++)
			{
				// Check if j is an output feature index
				if (j==i_outputFeatureIdxs[u])
				{
					b_isOutputFeature = true;
					i_idxCorrection++;

					// See http://public.cranfield.ac.uk/c5354/teaching/ml/examples/c++/handwritten_ex/neuralnetwork.cpp
					// note that the OpenCV neural network (MLP) implementation does not
					// support categorical variables explicitly.
					// So, instead of the output class label, we will use
					// a binary vector of {0,0 ... 1,0,0} components (one element by class)
					// for training and therefore, MLP will give us a vector of "probabilities"
					// at the prediction stage - the highest probability can be accepted
					// as the "winning" class label output by the network

					// The current value is an output feature, store it in the output matrix
					cvSetReal2D(mat_outputData, i, u, (float)d_value);
				}
			}

			if(!b_isOutputFeature)
			{
				// Store the value inside the traning data matrix
				cvSetReal2D(mat_trainData, i, j-i_idxCorrection, (float)d_value);
			}
		}
	}

	mat_array = new CvMat*[2];
	mat_array[0] = mat_trainData;
	mat_array[1] = mat_outputData;
	return mat_array;
}

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CvMat** COpenCV_NNet::GetTrainAndOutputMatrix(CSG_Parameter_Grid_List *gl_grids, int type, CSG_Shapes *s_areas, int i_classId, CSG_Grid *g_evalOut, CSG_Grid *g_evalOutCert)
{
	bool				b_NoData;
	double				d_value;
	int					x, y, i_Grid, i_Shape;
	CSG_Table			*t_data;
	CSG_Table_Record	*tr_rec;
	const SG_Char		*s_class;
	CSG_Vector			v_outVec;
	CvMat				**mat_array;
	CvMat				*mat_trainData;
	CvMat				*mat_outputData;
	TSG_Point			p;

	// CGrid_Classify_Supervised was the template for this method

	g_evalOut->Set_NoData_Value(-1);
	g_evalOut->Assign(0.0);

	// We will use this table as a temporary data store,
	// since we cannot dynamically resize the CvMat
	t_data = new CSG_Table();
	
	// We need a column for each grid and the output lable
	for (int i = 0; i < gl_grids->Get_Count(); i++)
	{
		t_data->Add_Field(CSG_String::Format(SG_T("GRID_%d"), i), SG_DATATYPE_Float, i); 
	}

	t_data->Add_Field("CLASS", SG_DATATYPE_String, gl_grids->Get_Count());


	// Traverse all grids, every point
	for(y=0, p.y=Get_YMin(); y<Get_NY() && Set_Progress(y); y++, p.y+=Get_Cellsize())
	{
		for(x=0, p.x=Get_XMin(); x<Get_NX(); x++, p.x+=Get_Cellsize())
		{
			for(i_Grid=0, b_NoData=false; i_Grid<gl_grids->Get_Count() && !b_NoData; i_Grid++)
			{
				// If there is one grid that has no data in this point p, then set the no data flag
				if( gl_grids->asGrid(i_Grid)->is_NoData(x, y) )
				{
					b_NoData = true;
				}
			}

			if (b_NoData)
			{
				// Set the NoData_Value to this point in the resulting grid
				g_evalOut->Set_NoData(x, y);
				if (g_evalOutCert)
				{
					g_evalOutCert->Set_NoData(x, y);
				}
			}
			else if(s_areas)
			{				
				// We have a value in each input grid and we have training area shapes
				for(i_Shape=0; i_Shape<s_areas->Get_Count(); i_Shape++)
				{
					// TODO: Get the current shape and check if the point is in it, then store
					// its class and the corresponding value from the input grids in the training grid
					CSG_Shape_Polygon *sp_inputArea = (CSG_Shape_Polygon *)s_areas->Get_Shape(i_Shape);
						
					if (sp_inputArea->Contains(p))
					{
						tr_rec = t_data->Add_Record();
						for(i_Grid=0; i_Grid<gl_grids->Get_Count(); i_Grid++)
						{
							tr_rec->Set_Value(i_Grid, (float) gl_grids->asGrid(i_Grid)->asFloat(x, y));
						}
						tr_rec->Set_Value(gl_grids->Get_Count(), sp_inputArea->asString(i_classId));
					}
				}
			}
		}
	}

	// Now that we have got all values in t_data for all points and the classes let's create the mats
	mat_trainData = cvCreateMat(t_data->Get_Count(), gl_grids->Get_Count(), type);

	// Extract only the train data from the table
	for(int i=0; i<t_data->Get_Count(); i++)
	{
		tr_rec = t_data->Get_Record(i);

		for(int j=0; j<gl_grids->Get_Count(); j++)
		{
			d_value = tr_rec->Get_Value(j)->asDouble();
			cvSetReal2D(mat_trainData, i, j, (float)d_value);
		}
	}

	mat_outputData = cvCreateMat(t_data->Get_Count(), s_areas->Get_Count(), type);

	// Extract only the class data from the table
	for(int i=0; i<t_data->Get_Count(); i++)
	{
		tr_rec = t_data->Get_Record(i);
		s_class = tr_rec->Get_Value(gl_grids->Get_Count())->asString();

		v_outVec = GetClassVectorByName(s_areas, s_class, i_classId);

		for(int j=0; j<v_outVec.Get_N(); j++)
		{
			d_value = v_outVec(j);
			cvSetReal2D(mat_outputData, i, j, (float)d_value);
		}
	}

	mat_array = new CvMat*[2];
	mat_array[0] = mat_trainData;
	mat_array[1] = mat_outputData;
	return mat_array;
}
