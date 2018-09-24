/**********************************************************
 * Version $Id: template.cpp 911 2011-11-11 11:11:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                        maxent                         //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                presence_prediction.cpp                //
//                                                       //
//                 Copyright (C) 2015 by                 //
//                     Olaf Conrad                       //
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
//    e-mail:     oconrad@saga-gis.de                    //
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
#include "presence_prediction.h"

//---------------------------------------------------------
#include "me.h"

//---------------------------------------------------------
using namespace std;


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPresence_Prediction::CPresence_Prediction(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Maximum Entropy Presence Prediction"));

	Set_Author		("O.Conrad (c) 2015");

	Set_Description	(_TW(
		""
	));

	Add_Reference("http://www.logos.t.u-tokyo.ac.jp/~tsuruoka/maxent/",
		SG_T("Yoshimasa Tsuruoka: A simple C++ library for maximum entropy classification.")
	);

	Add_Reference("http://webdocs.cs.ualberta.ca/~lindek/downloads.htm",
		SG_T("Dekang Lin: A MaxEnt Package in C++.")
	);

	//-----------------------------------------------------
	Parameters.Add_Shapes("",
		"PRESENCE"		, _TL("Presence Data"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Grid_List("",
		"FEATURES_NUM"	, _TL("Numerical Features"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Grid_List("",
		"FEATURES_CAT"	, _TL("Categorical Features"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Grid("",
		"PREDICTION"		, _TL("Presence Prediction"),
		_TL(""),
		PARAMETER_OUTPUT, true, SG_DATATYPE_Char
	);

	Parameters.Add_Grid("",
		"PROBABILITY"		, _TL("Presence Probability"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Double("",
		"BACKGROUND"		, _TL("Background Sample Density [Percent]"),
		_TL(""),
		1.0, 0.0, true, 100, true
	);

	//-----------------------------------------------------
	Parameters.Add_Choice("",
		"METHOD"			, _TL("Method"),
		_TL(""),
		CSG_String::Format("%s|%s|",
			_TL("Yoshimasa Tsuruoka"),
			_TL("Dekang Lin")
		)
	);

	//-----------------------------------------------------
	Parameters.Add_FilePath("",
		"YT_FILE_LOAD"	, _TL("Load from File..."),
		_TL(""),
		NULL, NULL, false
	);

	Parameters.Add_FilePath("",
		"YT_FILE_SAVE"	, _TL("Save to File..."),
		_TL(""),
		NULL, NULL, true
	);

	Parameters.Add_Choice("",
		"YT_REGUL"		, _TL("Regularization"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|",
			_TL("none"),
			SG_T("L1"),
			SG_T("L2")
		), 1
	);

	Parameters.Add_Double("YT_REGUL",
		"YT_REGUL_VAL"	, _TL("Regularization Factor"),
		_TL(""),
		1.0, 0.0, true
	);

	Parameters.Add_Bool("",
		"YT_NUMASREAL"	, _TL("Real-valued Numerical Features"),
		_TL(""),
		true
	);

	//-----------------------------------------------------
	Parameters.Add_Double("",
		"DL_ALPHA"		, _TL("Alpha"),
		_TL(""),
		0.1
	);

	Parameters.Add_Double("",
		"DL_THRESHOLD"	, _TL("Threshold"),
		_TL(""),
		0.0, 0.0, true
	);

	Parameters.Add_Int("",
		"DL_ITERATIONS"	, _TL("Maximum Iterations"),
		_TL(""),
		100, 1, true
	);

	//-----------------------------------------------------
	Parameters.Add_Int("",
		"NUM_CLASSES"		, _TL("Number of Numeric Value Classes"),
		_TL(""),
		32, 1, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CPresence_Prediction::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	bool	bFile	= SG_File_Exists(pParameters->Get_Parameter("YT_FILE_LOAD")->asString());
	int		Method	= pParameters->Get_Parameter("METHOD")->asInt();

	pParameters->Set_Enabled("NUM_CLASSES"  , Method == 1 || !pParameters->Get_Parameter("YT_NUMASREAL")->asBool());

	pParameters->Set_Enabled("TRAINING"     , Method == 1 || !bFile);

	pParameters->Set_Enabled("YT_REGUL"     , Method == 0 && !bFile);
	pParameters->Set_Enabled("YT_REGUL_VAL" , Method == 0 && !bFile);
	pParameters->Set_Enabled("YT_FILE_SAVE" , Method == 0 && !bFile);
	pParameters->Set_Enabled("YT_FILE_LOAD" , Method == 0);
	pParameters->Set_Enabled("YT_NUMASREAL" , Method == 0);

	pParameters->Set_Enabled("DL_ALPHA"     , Method == 1);
	pParameters->Set_Enabled("DL_THRESHOLD" , Method == 1);
	pParameters->Set_Enabled("DL_ITERATIONS", Method == 1);

	return( 1 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPresence_Prediction::On_Execute(void)
{
	//-----------------------------------------------------
	EventSet	DL_Events ;	m_DL_Events  = &DL_Events ;
	GISTrainer	DL_Trainer;	m_DL_Trainer = &DL_Trainer;
	MaxEntModel	DL_Model  ;	m_DL_Model   = &DL_Model  ;

	m_YT_Model.clear();

	//-----------------------------------------------------
	CSG_Grid	*pPrediction	= Parameters("PREDICTION" )->asGrid();
	CSG_Grid	*pProbability	= Parameters("PROBABILITY")->asGrid();

	if( !pPrediction ->Get_Range() ) DataObject_Set_Colors(pPrediction , 11, SG_COLORS_YELLOW_GREEN);
	if( !pProbability->Get_Range() ) DataObject_Set_Colors(pProbability, 11, SG_COLORS_YELLOW_GREEN);

	m_Method		= Parameters("METHOD"      )->asInt ();
	m_nNumClasses	= Parameters("NUM_CLASSES" )->asInt ();
	m_bYT_Weights	= Parameters("YT_NUMASREAL")->asBool();

	//-----------------------------------------------------
	CSG_Array	Features;

	if( !Get_Features(Features) )
	{
		Error_Set(_TL("invalid features"));

		return( false );
	}

	//-----------------------------------------------------
	if( m_Method == 0 && SG_File_Exists(Parameters("YT_FILE_LOAD")->asString()) )
	{
		if( !Get_File(Parameters("YT_FILE_LOAD")->asString()) )
		{
			return( false );
		}
	}
	else if( !Get_Training() )
	{
		return( false );
	}

	//-----------------------------------------------------
	Process_Set_Text(_TL("prediction"));

	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			int			i;
			CSG_Strings	Values;

			for(i=0; i<m_nFeatures; i++)
			{
				if( !m_Features[i].pGrid->is_NoData(x, y) )
				{
					Values.Add(Get_Feature(x, y, i));
				}
				else
				{
					break;
				}
			}

			if( Values.Get_Count() != m_nFeatures )
			{
				pPrediction ->Set_NoData(x, y);
				pProbability->Set_NoData(x, y);
			}
			else switch( m_Method )
			{
			//---------------------------------------------
			default:	// Kyoshida
				{
					ME_Sample	Sample;

					for(i=0; i<m_nFeatures; i++)
					{
						if( m_bYT_Weights && m_Features[i].bNumeric )
						{
							Sample.add_feature(m_Features[i].Name, m_Features[i].pGrid->asDouble(x, y));
						}
						else
						{
							Sample.add_feature(Values[i].b_str());
						}
					}

					vector<double> Probs	= m_YT_Model.classify(Sample);

					pPrediction ->Set_Value(x, y, m_YT_Model.get_class_id(Sample.label) == 0 ? 1 : 0);
					pProbability->Set_Value(x, y, Probs[0]);
				}
				break;

			//---------------------------------------------
			case  1:	// Dekang Lin
				{
					MaxEntEvent Event;	Event.count(1);

					for(i=0; i<m_nFeatures; i++)
					{
						Event.push_back(m_DL_Trainer->getId(Values[i].b_str()));
					}

					vector<double> Probs;

					pPrediction ->Set_Value(x, y, m_DL_Model->getProbs(Event, Probs) == 0 ? 1 : 0);
					pProbability->Set_Value(x, y, Probs[0]);
				}
				break;
			}
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String CPresence_Prediction::Get_Feature(int x, int y, int i)
{
	CSG_Grid	*pFeature	= m_Features[i].pGrid;

	if( m_nNumClasses > 1 && m_Features[i].bNumeric )
	{
		return( CSG_String::Format("%d", (int)(m_nNumClasses * (pFeature->asDouble(x, y) - pFeature->Get_Min()) / pFeature->Get_Range())) );
	}

	return( SG_Get_String(pFeature->asDouble(x, y), -2) );
}

//---------------------------------------------------------
bool CPresence_Prediction::Get_Features(CSG_Array &Features)
{
	CSG_Parameter_Grid_List	*pNum	= Parameters("FEATURES_NUM")->asGridList();
	CSG_Parameter_Grid_List	*pCat	= Parameters("FEATURES_CAT")->asGridList();

	m_Features	= (TFeature *)Features.Create(sizeof(TFeature), m_nFeatures = pNum->Get_Grid_Count() + pCat->Get_Grid_Count());

	for(int i=0; i<m_nFeatures; i++)
	{
		if( i < pNum->Get_Grid_Count() )
		{
			m_Features[i].bNumeric	= true;
			m_Features[i].pGrid		= pNum->Get_Grid(i);
		}
		else
		{
			m_Features[i].bNumeric	= false;
			m_Features[i].pGrid		= pCat->Get_Grid(i - pNum->Get_Grid_Count());
		}

		CSG_String	Name(m_Features[i].pGrid->Get_Name());
		strncpy(m_Features[i].Name, Name.b_str(), 255);	m_Features[i].Name[255]	= '\0';
	}

	return( m_nFeatures > 0 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPresence_Prediction::Get_Training(void)
{
	//-----------------------------------------------------
	Process_Set_Text(_TL("collecting presence data"));

	if( m_Method == 1 )	// Dekang Lin
	{
		m_DL_Trainer->addClass("0");
		m_DL_Trainer->addClass("1");
	}

	CSG_Shapes	*pPresence	= Parameters("PRESENCE")->asShapes();

	if( pPresence->Get_Count() < 1 )
	{
		Error_Set(_TL("invalid training data"));

		return( false );
	}

	for(int iPoint=0; iPoint<pPresence->Get_Count() && Set_Progress(iPoint, pPresence->Get_Count()); iPoint++)
	{
		TSG_Point	p	= pPresence->Get_Shape(iPoint)->Get_Point(0);

		int	x	= Get_System()->Get_xWorld_to_Grid(p.x);
		int	y	= Get_System()->Get_yWorld_to_Grid(p.y);

		Get_Training(x, y, "0");
	}

	//-----------------------------------------------------
	Process_Set_Text(_TL("collecting background data"));

	double	Background	= Parameters("BACKGROUND")->asDouble() / 100.0;

	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			if( CSG_Random::Get_Uniform() <= Background )
			{
				Get_Training(x, y, "1");
			}
		}
	}

	//-----------------------------------------------------
	Process_Set_Text(_TL("training"));

	switch( m_Method )
	{
	//-----------------------------------------------------
	default:	// Kyoshida
		switch( Parameters("YT_REGUL")->asInt() )
		{
		default:
			m_YT_Model.use_l1_regularizer(0.0);
			m_YT_Model.use_l2_regularizer(0.0);
			break;

		case  1:
			m_YT_Model.use_l1_regularizer(Parameters("YT_REGUL_VAL")->asDouble());
			m_YT_Model.use_l2_regularizer(0.0);
			break;

		case  2:
			m_YT_Model.use_l1_regularizer(0.0);
			m_YT_Model.use_l2_regularizer(Parameters("YT_REGUL_VAL")->asDouble());
			break;
		}

	//	m_YT_Model.use_SGD(Parameters("SGD_ITER")->asInt());

		m_YT_Model.train();

		{
		/*	list< pair< pair<string, string>, double > > Features;

			m_YT_Model.get_features(Features);

			for(list< pair< pair<string, string>, double> >::const_iterator i=Features.begin(); i!=Features.end(); i++)
			{
				Message_Fmt("\n%10.3f  %-10s %s", i->second, i->first.first.c_str(), i->first.second.c_str());
			}/**/

			CSG_String	File(Parameters("YT_FILE_SAVE")->asString());

			if( !File.is_Empty() )
			{
				m_YT_Model.save_to_file(File.b_str());
			}
		}
		break;

	//-----------------------------------------------------
	case  1:	// Dekang Lin
		m_DL_Trainer->printDetails(true);	// show the parameters during training

		m_DL_Trainer->Set_Alpha     (Parameters("DL_ALPHA"     )->asDouble());
		m_DL_Trainer->Set_Threshold (Parameters("DL_THRESHOLD" )->asDouble());
		m_DL_Trainer->Set_Iterations(Parameters("DL_ITERATIONS")->asInt());

		m_DL_Model->classes(m_DL_Trainer->classes().size());

		m_DL_Trainer->train(*m_DL_Model, *m_DL_Events);	// train the model

		break;
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPresence_Prediction::Get_Training(int x, int y, const char *ID)
{
	int		i;

	//-----------------------------------------------------
	CSG_Strings	Values;

	for(i=0; i<m_nFeatures; i++)
	{
		if( !m_Features[i].pGrid->is_InGrid(x, y) )
		{
			return( false );
		}

		Values.Add(Get_Feature(x, y, i));
	}

	//-----------------------------------------------------
	switch( m_Method )
	{
	//-----------------------------------------------------
	default:	// Kyoshida
		{
			ME_Sample	Sample(ID);

			for(i=0; i<m_nFeatures; i++)
			{
				if( m_bYT_Weights && m_Features[i].bNumeric )
				{
					Sample.add_feature(m_Features[i].Name, m_Features[i].pGrid->asDouble(x, y));
				}
				else
				{
					Sample.add_feature(Values[i].b_str());
				}
			}

			m_YT_Model.add_training_sample(Sample);
		}
		break;

	//-----------------------------------------------------
	case  1:	// Dekang Lin
		{
			MaxEntEvent	*pEvent	= new MaxEntEvent;

			pEvent->count(1);
			pEvent->classId(m_DL_Trainer->getClassId(ID));

			for(i=0; i<m_nFeatures; i++)
			{
				pEvent->push_back(m_DL_Trainer->getId(Values[i].b_str()));
			}

			m_DL_Events->push_back(pEvent);
		}
		break;
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPresence_Prediction::Get_File(const CSG_String &File)
{
	//-----------------------------------------------------
	if( !m_YT_Model.load_from_file(File.b_str()) )
	{
		Error_Set(_TL("could not load model from file"));

		return( false );
	}

	//-----------------------------------------------------
/*	list< pair< pair<string, string>, double > > Features;

	m_YT_Model.get_features(Features);

	if( (int)Features.size() != m_nFeatures )
	{
		Error_Set(_TL("number of model features do not fit input features list"));

		return( false );
	}/**/

	//-----------------------------------------------------
	if( m_YT_Model.num_classes() <= 1 )
	{
		Error_Set(_TL("less than two classes in model"));

		return( false );
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
