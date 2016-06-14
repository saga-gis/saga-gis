/**********************************************************
 * Version $Id: opencv_ml_normalbayes.cpp 0001 2016-05-24
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                        OpenCV                         //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//               opencv_ml_normalbayes.cpp               //
//                                                       //
//                 Copyright (C) 2016 by                 //
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
#include "opencv_ml.h"

#if CV_MAJOR_VERSION >= 3


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
COpenCV_ML_NBayes::COpenCV_ML_NBayes(void)
	: COpenCV_ML(true)
{
	Set_Name		(_TL("Normal Bayes Classification (OpenCV)"));

	Set_Author		("O.Conrad (c) 2016");

	Set_Description	(_TW(
		"Integration of the OpenCV Machine Learning library for "
		"Normal Bayes classification of gridded features.\n"
		"<a href=\"http://docs.opencv.org\">Open Source Computer Vision</a>"
	));
}

//---------------------------------------------------------
Ptr<StatModel> COpenCV_ML_NBayes::Get_Model(void)
{
	return(	NormalBayesClassifier::create() );
}

//---------------------------------------------------------
double COpenCV_ML_NBayes::Get_Probability(const Ptr<StatModel> &Model, const Mat &Sample)
{
	const Ptr<NormalBayesClassifier>	&m	= *((Ptr<NormalBayesClassifier>*)&Model);

	Mat	Output, Probabilities;

	return( m->predictProb(Sample, Output, Probabilities) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
COpenCV_ML_KNN::COpenCV_ML_KNN(void)
	: COpenCV_ML(false)
{
	Set_Name		(_TL("K-Nearest Neighbours Classification (OpenCV)"));

	Set_Author		("O.Conrad (c) 2016");

	Set_Description	(_TW(
		"Integration of the OpenCV Machine Learning library for "
		"K-Nearest Neighbours classification of gridded features.\n"
		"<a href=\"http://docs.opencv.org\">Open Source Computer Vision</a>"
	));

	//-----------------------------------------------------
	Parameters.Add_Int(
		NULL	, "NEIGHBOURS"	, _TL("Default Number of Neighbours"),
		_TL(""),
		3, 1, true
	);

	Parameters.Add_Choice(
		NULL	, "TRAINING"	, _TL("Training Method"),
		_TL(""),
		CSG_String::Format("%s|%s|",
			_TL("classification"),
			_TL("regression model")
		), 0
	);

	Parameters.Add_Choice(
		NULL	, "ALGORITHM"	, _TL("Algorithm Type"),
		_TL(""),
		CSG_String::Format("%s|%s|",
			_TL("brute force"),
			_TL("KD Tree")
		), 0
	);

	Parameters.Add_Int(
		NULL	, "EMAX"		, _TL("Parameter for KD Tree implementation"),
		_TL(""),
		1000, 1, true
	);
}

//---------------------------------------------------------
int COpenCV_ML_KNN::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	!SG_STR_CMP(pParameter->Get_Identifier(), "ALGORITHM") )
	{
		pParameters->Set_Enabled("EMAX", pParameter->asInt() == 1);
	}

	//-----------------------------------------------------
	return( COpenCV_ML::On_Parameters_Enable(pParameters, pParameter) );
}

//---------------------------------------------------------
Ptr<StatModel> COpenCV_ML_KNN::Get_Model(void)
{
	Ptr<KNearest>	Model	= KNearest::create();

	switch( Parameters("ALGORITHM")->asInt() )
	{
	default:	Model->setAlgorithmType(KNearest::BRUTE_FORCE);	break;
	case  1:	Model->setAlgorithmType(KNearest::KDTREE     );	break;
	}

	Model->setDefaultK    (Parameters("NEIGHBOURS")->asInt());
	Model->setIsClassifier(Parameters("TRAINING"  )->asInt() == 0);
//	Model->setEmax        (Parameters("EMAX"      )->asInt());

	return(	Model );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
COpenCV_ML_SVM::COpenCV_ML_SVM(void)
	: COpenCV_ML(false)
{
	Set_Name		(_TL("Support Vector Machine Classification (OpenCV)"));

	Set_Author		("O.Conrad (c) 2016");

	Set_Description	(_TW(
		"Integration of the OpenCV Machine Learning library for "
		"Support Vector Machine classification of gridded features.\n"
		"<a href=\"http://docs.opencv.org\">Open Source Computer Vision</a>"
	));

	//-----------------------------------------------------
	CSG_Parameter	*pNode;

	pNode	= Parameters.Add_Choice(
		NULL	, "SVM_TYPE"	, _TL("SVM Type"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s|%s|",
			_TL("c-support vector classification"),
			_TL("nu support vector classification"),
			_TL("distribution estimation (one class)"),
			_TL("epsilon support vector regression"),
			_TL("nu support vector regression")
		), 0
	);

	Parameters.Add_Double(pNode, "C" , _TL("C" ), _TL(""), 1.0, 0.0, true);
	Parameters.Add_Double(pNode, "NU", _TL("Nu"), _TL(""), 0.5, 0.0, true);
	Parameters.Add_Double(pNode, "P" , _TL("P" ), _TL(""), 0.5, 0.0, true);

	pNode	= Parameters.Add_Choice(
		NULL	, "KERNEL"		, _TL("Kernel Type"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s|%s|%s|",
			_TL("linear"),
			_TL("polynomial"),
			_TL("radial basis function"),
			_TL("sigmoid"),
			_TL("exponential chi2"),
			_TL("histogram intersection"),
			_TL("custom")
		), 1
	);

	Parameters.Add_Double(pNode, "COEF0" , _TL("Coefficient 0"), _TL(""), 1.0, 0.0, true);
	Parameters.Add_Double(pNode, "DEGREE", _TL("Degree"       ), _TL(""), 0.5, 0.0, true);
	Parameters.Add_Double(pNode, "GAMMA" , _TL("Gamma"        ), _TL(""), 1.0, 0.0, true);
}

//---------------------------------------------------------
int COpenCV_ML_SVM::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	!SG_STR_CMP(pParameter->Get_Identifier(), "SVM_TYPE") )
	{
		pParameters->Set_Enabled("C"     , pParameter->asInt() == 0 || pParameter->asInt() == 3 || pParameter->asInt() == 4);
		pParameters->Set_Enabled("NU"    , pParameter->asInt() == 1 || pParameter->asInt() == 2 || pParameter->asInt() == 4);
		pParameters->Set_Enabled("P"     , pParameter->asInt() == 3);
	}

	if(	!SG_STR_CMP(pParameter->Get_Identifier(), "KERNEL") )
	{
		pParameters->Set_Enabled("COEF0" , pParameter->asInt() == 1 || pParameter->asInt() == 3);
		pParameters->Set_Enabled("DEGREE", pParameter->asInt() == 1);
		pParameters->Set_Enabled("GAMMA" , pParameter->asInt() >= 1 && pParameter->asInt() <= 4);
	}

	//-----------------------------------------------------
	return( COpenCV_ML::On_Parameters_Enable(pParameters, pParameter) );
}

//---------------------------------------------------------
Ptr<StatModel> COpenCV_ML_SVM::Get_Model(void)
{
	Ptr<SVM>	Model	= SVM::create();

	switch( Parameters("SVM_TYPE")->asInt() )
	{
	default:	Model->setType(SVM::C_SVC    );	break;
	case  1:	Model->setType(SVM::NU_SVC   );	break;
	case  2:	Model->setType(SVM::ONE_CLASS);	break;
	case  3:	Model->setType(SVM::EPS_SVR  );	break;
	case  4:	Model->setType(SVM::NU_SVR   );	break;
	}

	switch( Parameters("KERNEL")->asInt() )
	{
	default:	Model->setKernel(SVM::LINEAR );	break;
	case  1:	Model->setKernel(SVM::POLY   );	break;
	case  2:	Model->setKernel(SVM::RBF    );	break;
	case  3:	Model->setKernel(SVM::SIGMOID);	break;
	case  4:	Model->setKernel(SVM::CHI2   );	break;
	case  5:	Model->setKernel(SVM::INTER  );	break;
	case  6:	Model->setKernel(SVM::CUSTOM );	break;
	}

	Model->setC     (Parameters("C"     )->asDouble());
	Model->setCoef0 (Parameters("COEF0" )->asDouble());
	Model->setDegree(Parameters("DEGREE")->asDouble());
	Model->setGamma (Parameters("GAMMA" )->asDouble());
	Model->setNu    (Parameters("NU"    )->asDouble());
	Model->setP     (Parameters("P"     )->asDouble());

	Model->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER + TermCriteria::EPS, 1000, 0.01));

	return(	Model );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
COpenCV_ML_DTrees::COpenCV_ML_DTrees(void)
	: COpenCV_ML(false)
{
	Set_Name		(_TL("Decision Tree Classification (OpenCV)"));

	Set_Author		("O.Conrad (c) 2016");

	Set_Description	(_TW(
		"Integration of the OpenCV Machine Learning library for "
		"Decision Tree classification of gridded features.\n"
		"<a href=\"http://docs.opencv.org\">Open Source Computer Vision</a>"
	));

	Parameters.Add_Int(
		NULL	, "MAX_DEPTH"		, _TL("Maximum Tree Depth"),
		_TL("The maximum possible depth of the tree. That is the training algorithms attempts to split a node while its depth is less than maxDepth. The root node has zero depth."),
		10, 1, true
	);

	Parameters.Add_Int(
		NULL	, "MIN_SAMPLES"		, _TL("Minimum Sample Count"),
		_TL("If the number of samples in a node is less than this parameter then the node will not be split."),
		2, 2, true
	);

	Parameters.Add_Int(
		NULL	, "MAX_CATEGRS"		, _TL("Maximum Categories"),
		_TL("Cluster possible values of a categorical variable into K<=maxCategories clusters to find a suboptimal split."),
		10, 1, true
	);

	//Parameters.Add_Int(
	//	NULL	, "CV_FOLDS"		, _TL("CV Folds"),
	//	_TL("If CVFolds > 1 then algorithms prunes the built decision tree using K-fold cross-validation procedure where K is equal to CVFolds. Default value is 10."),
	//	10, 0, true
	//);

	Parameters.Add_Bool(
		NULL	, "1SE_RULE"		, _TL("Use 1SE Rule"),
		_TL("If true then a pruning will be harsher. This will make a tree more compact and more resistant to the training data noise but a bit less accurate."),
		true
	);

	Parameters.Add_Bool(
		NULL	, "TRUNC_PRUNED"	, _TL("Truncate Pruned Trees"),
		_TL("If true then pruned branches are physically removed from the tree. Otherwise they are retained and it is possible to get results from the original unpruned (or pruned less aggressively) tree."),
		true
	);

	Parameters.Add_Double(
		NULL	, "REG_ACCURACY"	, _TL("Regression Accuracy"),
		_TL("Termination criteria for regression trees. If all absolute differences between an estimated value in a node and values of train samples in this node are less than this parameter then the node will not be split further."),
		 0.01, 0.0, true
	);

//	Parameters.Add_Bool(
//		NULL	, "SURROGATES"		, _TL("Use Surrogates"),
//		_TL("If true then surrogate splits will be built. These splits allow to work with missing data and compute variable importance correctly."),
//		 false
//	);
}

//---------------------------------------------------------
Ptr<DTrees> COpenCV_ML_DTrees::Get_Trees(void)
{
	return( DTrees::create() );
}

//---------------------------------------------------------
Ptr<StatModel> COpenCV_ML_DTrees::Get_Model(void)
{
	Ptr<DTrees>	Model	= Get_Trees();

	Model->setMaxDepth          (Parameters("MAX_DEPTH"   )->asInt   ());
	Model->setMinSampleCount    (Parameters("MIN_SAMPLES" )->asInt   ());
	Model->setMaxCategories     (Parameters("MAX_CATEGRS" )->asInt   ());
//	Model->setCVFolds           (Parameters("CV_FOLDS"    )->asInt   ());
	Model->setCVFolds           (0);
	Model->setUse1SERule        (Parameters("1SE_RULE"    )->asBool  ());
	Model->setTruncatePrunedTree(Parameters("TRUNC_PRUNED")->asBool  ());
	Model->setRegressionAccuracy(Parameters("REG_ACCURACY")->asDouble());
//	Model->setUseSurrogates     (Parameters("SURROGATES"  )->asBool  ());

	return(	Model );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
COpenCV_ML_Boost::COpenCV_ML_Boost(void)
{
	Set_Name		(_TL("Boosting Classification (OpenCV)"));

	Set_Author		("O.Conrad (c) 2016");

	Set_Description	(_TW(
		"Integration of the OpenCV Machine Learning library for "
		"Boosted Trees classification of gridded features.\n"
		"<a href=\"http://docs.opencv.org\">Open Source Computer Vision</a>"
	));

	Parameters.Add_Int(
		NULL	, "WEAK_COUNT"		, _TL("Weak Count"),
		_TL("The number of weak classifiers."),
		100, 0, true
	);

	Parameters.Add_Double(
		NULL	, "WGT_TRIM_RATE"	, _TL("Weight Trim Rate"),
		_TL("A threshold between 0 and 1 used to save computational time. Set this parameter to 0 to turn off this functionality."),
		0.95, 0.0, true, 1.0, true
	);

	Parameters.Add_Choice(
		NULL	, "BOOST_TYPE"	, _TL("Boost Type"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s|",
			_TL("Discrete AdaBoost"),
			_TL("Real AdaBoost"),
			_TL("LogitBoost"),
			_TL("Gentle AdaBoost")
		), 1
	);
}

//---------------------------------------------------------
Ptr<DTrees> COpenCV_ML_Boost::Get_Trees(void)
{
	Ptr<Boost>	Model	= Boost::create();

	Model->setWeakCount         (Parameters("WEAK_COUNT"   )->asInt   ());
	Model->setWeightTrimRate    (Parameters("WGT_TRIM_RATE")->asDouble());

	switch( Parameters("BOOST_TYPE")->asInt() )
	{
	default:	Model->setBoostType(Boost::DISCRETE);	break;
	case  1:	Model->setBoostType(Boost::REAL    );	break;
	case  2:	Model->setBoostType(Boost::LOGIT   );	break;
	case  3:	Model->setBoostType(Boost::GENTLE  );	break;
	}

//	Model->setPriors        (Mat());

	return(	Model );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
COpenCV_ML_RTrees::COpenCV_ML_RTrees(void)
{
	Set_Name		(_TL("Random Forest Classification (OpenCV)"));

	Set_Author		("O.Conrad (c) 2016");

	Set_Description	(_TW(
		"Integration of the OpenCV Machine Learning library for "
		"Random Forest classification of gridded features.\n"
		"<a href=\"http://docs.opencv.org\">Open Source Computer Vision</a>"
	));

	Parameters.Add_Int(
		NULL	, "ACTIVE_VARS"	, _TL("Active Variable Count"),
		_TL("The size of the randomly selected subset of features at each tree node and that are used to find the best split(s). If you set it to 0 then the size will be set to the square root of the total number of features."),
		0, 0, true
	);
}

//---------------------------------------------------------
Ptr<DTrees> COpenCV_ML_RTrees::Get_Trees(void)
{
	Ptr<RTrees>	Model	= RTrees::create();

	Model->setActiveVarCount(Parameters("ACTIVE_VARS")->asInt());

	Model->setCalculateVarImportance(false);

	Model->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER, 5, 0));

//	Model->setPriors                (Mat());

	return(	Model );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
enum E_ANN_ACTIVATION
{
	ANN_ACTIVATION_IDENTITY	= 0,
	ANN_ACTIVATION_SIGMOID,
	ANN_ACTIVATION_GAUSSIAN
};

//---------------------------------------------------------
enum E_ANN_PROPAGATION
{
	ANN_PROPAGATION_RESILIENT = 0,
	ANN_PROPAGATION_BACK
};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
COpenCV_ML_ANN::COpenCV_ML_ANN(void)
	: COpenCV_ML(false)
{
	Set_Name		(_TL("Artificial Neural Network Classification (OpenCV)"));

	Set_Author		("O.Conrad (c) 2016, L.Piras (c) 2013");

	Set_Description	(_TW(
		"Integration of the OpenCV Machine Learning library for "
		"Artificial Neural Network classification of gridded features.\n"
		"<a href=\"http://docs.opencv.org\">Open Source Computer Vision</a>"
	));

	//-----------------------------------------------------
	CSG_Parameter	*pNode;

	Parameters.Add_Int(
		NULL	, "ANN_LAYERS"		, _TL("Number of Layers"),
		_TL("You can specify the number of layers in the network (not including input and output layer)."),
		3, 1, true
	);

	Parameters.Add_Int(
		NULL	, "ANN_NEURONS"		, _TL("Number of Neurons"),
		_TL("You can specify the number of neurons in each layer of the network."),
		3, 1, true
	);

	Parameters.Add_Int(
		NULL	, "ANN_MAXITER"		, _TL("Maximum Number of Iterations"),
		_TL(""),
		300, 1, true
	);

	Parameters.Add_Double(
		NULL	, "ANN_EPSILON"		, _TL("Error Change (Epsilon)"),
		_TL("Termination criteria of the training algorithm. You can specify how much the error could change between the iterations to make the algorithm continue (epsilon)."),
		FLT_EPSILON, 0.0, true
	);

	pNode	= Parameters.Add_Choice(
		NULL	, "ANN_ACTIVATION"	, _TL("Activation Function"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|",
			_TL("Identity"),	
			_TL("Sigmoid"),
			_TL("Gaussian")
		), 1
	);
 
	Parameters.Add_Double(pNode, "ANN_ACT_ALPHA", _TL("Function's Alpha"), _TL(""), 1.0);
	Parameters.Add_Double(pNode, "ANN_ACT_BETA" , _TL("Function's Beta" ), _TL(""), 1.0);

	pNode	= Parameters.Add_Choice(
		NULL	, "ANN_PROPAGATION"		, _TL("Training Method"),
		_TL(""),
		CSG_String::Format("%s|%s|",
			_TL("resilient propagation"),
			_TL("back propagation")
		), 1
	);

	Parameters.Add_Double(pNode, "ANN_RP_DW0"     , _TL("Initial Update Value"    ), _TL(""), 0.0);							// _TL("Initial value delta_0 of update-values delta_ij.")
	Parameters.Add_Double(pNode, "ANN_RP_DW_PLUS" , _TL("Increase Factor"         ), _TL(""), 1.2, 1.01, true);				// _TL("Increase factor n_plus. Must be > 1.")
	Parameters.Add_Double(pNode, "ANN_RP_DW_MINUS", _TL("Decrease Factor"         ), _TL(""), 0.5, 0.01, true, 0.99, true);	// _TL("Decrease factor n_minus. Must be < 1.")
	Parameters.Add_Double(pNode, "ANN_RP_DW_MIN"  , _TL("Lower Value Update Limit"), _TL(""), 0.1, 0.01, true);				// _TL("Corresponds to delta_min. Must be > 0")
	Parameters.Add_Double(pNode, "ANN_RP_DW_MAX"  , _TL("Upper Value Update Limit"), _TL(""), 1.1, 1.01, true);				// _TL("Corresponds to delta_max. Must be > 1")

	Parameters.Add_Double(pNode, "ANN_BP_DW"      , _TL("Weight Gradient Term"    ), _TL(""), 0.1);							// _TL("Strength of the weight gradient term. The recommended value is about 0.1.")
	Parameters.Add_Double(pNode, "ANN_BP_MOMENT"  , _TL("Moment Term"             ), _TL(""), 0.1);							// _TL("Strength of the momentum term (the difference between weights on the 2 previous iterations). This parameter provides some inertia to smooth the random fluctuations of the weights. It can vary from 0 (the feature is disabled) to 1 and beyond. The value 0.1 or so is good enough.")
}

//---------------------------------------------------------
int COpenCV_ML_ANN::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	!SG_STR_CMP(pParameter->Get_Identifier(), "ANN_ACTIVATION") )
	{
		pParameters->Set_Enabled("ANN_ACT_ALPHA"  , pParameter->asInt() != ANN_ACTIVATION_IDENTITY);
		pParameters->Set_Enabled("ANN_ACT_BETA"   , pParameter->asInt() != ANN_ACTIVATION_IDENTITY);
	}

	if(	!SG_STR_CMP(pParameter->Get_Identifier(), "ANN_PROPAGATION") )
	{
		pParameters->Set_Enabled("ANN_RP_DW0"     , pParameter->asInt() == ANN_PROPAGATION_RESILIENT);
		pParameters->Set_Enabled("ANN_RP_DW_PLUS" , pParameter->asInt() == ANN_PROPAGATION_RESILIENT);
		pParameters->Set_Enabled("ANN_RP_DW_MINUS", pParameter->asInt() == ANN_PROPAGATION_RESILIENT);
		pParameters->Set_Enabled("ANN_RP_DW_MIN"  , pParameter->asInt() == ANN_PROPAGATION_RESILIENT);
		pParameters->Set_Enabled("ANN_RP_DW_MAX"  , pParameter->asInt() == ANN_PROPAGATION_RESILIENT);

		pParameters->Set_Enabled("ANN_BP_DW"      , pParameter->asInt() == ANN_PROPAGATION_BACK);
		pParameters->Set_Enabled("ANN_BP_MOMENT"  , pParameter->asInt() == ANN_PROPAGATION_BACK);
	}

	//-----------------------------------------------------
	return( COpenCV_ML::On_Parameters_Enable(pParameters, pParameter) );
}

//---------------------------------------------------------
Ptr<StatModel> COpenCV_ML_ANN::Get_Model(void)
{
	Ptr<ANN_MLP>	Model	= ANN_MLP::create();

	//-----------------------------------------------------
	Mat	layer_sizes(1, 2 + Parameters("ANN_LAYERS")->asInt(), CV_32SC1);

	layer_sizes.at<int>(0)	= Get_Feature_Count();	// The first layer needs the same size (number of neurons) as the number of columns in the training data

	for(int i=1; i<layer_sizes.cols-1; i++)
	{
		layer_sizes.at<int>(i)	= Parameters("ANN_NEURONS")->asInt();
	}

	layer_sizes.at<int>(layer_sizes.cols-1)	= Get_Class_Count();	// The last layer needs the same size (number of neurons) as the number of output columns

	Model->setLayerSizes(layer_sizes);

	//-----------------------------------------------------
	switch( Parameters("ANN_ACTIVATION")->asInt() )
	{
	case  0:	// Identity
		Model->setActivationFunction(ANN_MLP::IDENTITY);
		break;

	default:	// Sigmoid
		Model->setActivationFunction(ANN_MLP::SIGMOID_SYM, 
			Parameters("ANN_ACT_ALPHA")->asDouble(),
			Parameters("ANN_ACT_BETA" )->asDouble()
		);
		break;

	case  2:	// Gaussian
		Model->setActivationFunction(ANN_MLP::GAUSSIAN,
			Parameters("ANN_ACT_ALPHA")->asDouble(),
			Parameters("ANN_ACT_BETA" )->asDouble()
		);
		break;
	}

	//-----------------------------------------------------
	Model->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER + TermCriteria::EPS,
		Parameters("ANN_MAXITER")->asInt   (),
		Parameters("ANN_EPSILON")->asDouble()
	));

	//-----------------------------------------------------
	switch( Parameters("ANN_PROPAGATION")->asInt() )
	{
	case  0:	// resilient propagation
		Model->setTrainMethod(ANN_MLP::RPROP);
		Model->setRpropDW0             (Parameters("ANN_RP_DW0"     )->asDouble());
		Model->setRpropDWPlus          (Parameters("ANN_RP_DW_PLUS" )->asDouble());
		Model->setRpropDWMinus         (Parameters("ANN_RP_DW_MINUS")->asDouble());
		Model->setRpropDWMin           (Parameters("ANN_RP_DW_MIN"  )->asDouble());
		Model->setRpropDWMax           (Parameters("ANN_RP_DW_MAX"  )->asDouble());
		break;

	default:
		Model->setTrainMethod(ANN_MLP::BACKPROP);
		Model->setBackpropMomentumScale(Parameters("ANN_BP_MOMENT"  )->asDouble());
		Model->setBackpropWeightScale  (Parameters("ANN_BP_DW"      )->asDouble());
		break;
	}

	//-----------------------------------------------------
	return( Model );
}

//---------------------------------------------------------
Ptr<TrainData> COpenCV_ML_ANN::Get_Training(const CSG_Matrix &Data)
{
	Mat	Samples (Data.Get_NRows(), Data.Get_NCols() - 1 , CV_32F);
	Mat	Response(Data.Get_NRows(), Get_Class_Count()    , CV_32F);

	for(int i=0; i<Data.Get_NRows(); i++)
	{
		int	j, k	= (int)Data[i][Data.Get_NCols() - 1];

		for(j=0; j<Response.cols; j++)
		{
			Response.at<float>(i, j)	= j == k ? 1.f : 0.f;
		}

		for(j=0; j<Samples.cols; j++)
		{
			Samples.at<float>(i, j)	= (float)Data[i][j];
		}
	}

	return( TrainData::create(Samples, ROW_SAMPLE, Response) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // CV_MAJOR_VERSION >= 3


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
