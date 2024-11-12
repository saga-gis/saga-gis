
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                     table_tools                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//              table_classify_supervised.cpp            //
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
#include "table_classify_supervised.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTable_Classify_Supervised::CTable_Classify_Supervised(void)
{
	Set_Name		(CSG_String::Format("%s (%s)", _TL("Supervised Classification"), _TL("Table Fields")));

	Set_Author		("O.Conrad (c) 2012");

	Set_Description	(_TW(
		"Standard classifiers for supervised classification based on attributes.\n"
		"\n"
		"Classifiers can be trained in three different ways:<ul>"
		"<li>Known classes field: choose an attribute field that provides class "
		"identifiers for those records, for which the target class is known, and "
		"no-data (or empty string) for all other records.</li>"
		"<li>Training samples: a table with sample records providing the class "
		"identifier in the first field followed by sample data corresponding to "
		"the selected feature attributes.</li>"
		"<li>Load statistics from file: loads feature statistics from a file "
		"that has been previously stored after training with one of the other "
		"two options.</li></ul>"
	));

	//-----------------------------------------------------
	Parameters.Add_Table("",
		"TABLE"			, _TL("Table"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Fields("TABLE",
		"FEATURES"		, _TL("Features"),
		_TL("")
	);

	Parameters.Add_Bool("TABLE",
		"NORMALISE"		, _TL("Normalise"),
		_TL(""),
		false
	);

	Parameters.Add_Table ("", "RESULT_TABLE" , _TL("Classification"), _TL(""), PARAMETER_OUTPUT_OPTIONAL);
	Parameters.Add_Shapes("", "RESULT_SHAPES", _TL("Classification"), _TL(""), PARAMETER_OUTPUT_OPTIONAL);

	//-----------------------------------------------------
	Parameters.Add_Choice("",
		"TRAIN_WITH"	, _TL("Training"),
		_TL(""),
		CSG_String::Format("%s|%s|%s",
			_TL("known classes field"),
			_TL("training samples"),
			_TL("load statistics from file")
		)
	);

	Parameters.Add_Table_Field("TABLE",
		"TRAIN_FIELD"	, _TL("Known Classes Field"),
		_TL(""),
		false
	);

	Parameters.Add_Table("TRAIN_WITH",
		"TRAIN_SAMPLES"	, _TL("Training Samples"),
		_TL("Provide a class identifier in the first field followed by sample data corresponding to the selected feature attributes."),
		PARAMETER_INPUT
	);

	Parameters.Add_FilePath("TRAIN_WITH",
		"FILE_LOAD"		, _TL("Load Statistics from File..."),
		_TL(""),
		NULL, NULL, false
	);

	Parameters.Add_FilePath("TRAIN_WITH",
		"FILE_SAVE"		, _TL("Save Statistics to File..."),
		_TL(""),
		NULL, NULL, true
	);

	//-----------------------------------------------------
	CSG_String Methods;

	for(int i=0; i<=SG_CLASSIFY_SUPERVISED_WTA; i++)
	{
		Methods += CSG_Classifier_Supervised::Get_Name_of_Method(i) + "|";
	}

	Parameters.Add_Choice("",
		"METHOD"		, _TL("Method"),
		_TL(""),
		Methods, SG_CLASSIFY_SUPERVISED_MinimumDistance
	);

	Parameters.Add_Double("METHOD",
		"THRESHOLD_DIST", _TL("Distance Threshold"),
		_TL("Let pixel stay unclassified, if minimum euclidean or mahalanobis distance is greater than threshold."),
		0., 0., true
	);

	Parameters.Add_Double("METHOD",
		"THRESHOLD_ANGLE", _TL("Spectral Angle Threshold (Degree)"),
		_TL("Let pixel stay unclassified, if spectral angle distance is greater than threshold."),
		0., 0., true, 90., true
	);

	Parameters.Add_Double("METHOD",
		"THRESHOLD_PROB", _TL("Probability Threshold"),
		_TL("Let pixel stay unclassified, if maximum likelihood probability value is less than threshold."),
		0., 0., true, 100., true
	);

	Parameters.Add_Choice("METHOD",
		"RELATIVE_PROB"	, _TL("Probability Reference"),
		_TL(""),
		CSG_String::Format("%s|%s",
			_TL("absolute"),
			_TL("relative")
		), 1
	);

	Parameters.Add_Node("METHOD",
		"WTA"			, _TL("Winner Takes All"),
		_TL("")
	);

	for(int i=0; i<SG_CLASSIFY_SUPERVISED_WTA; i++)
	{
		Parameters.Add_Bool("WTA", CSG_String::Format("WTA_%d", i), CSG_Classifier_Supervised::Get_Name_of_Method(i), _TL(""), false);
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CTable_Classify_Supervised::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	pParameter->Cmp_Identifier("TABLE") )
	{
		if( pParameter->asDataObject() )
		{
			pParameters->Set_Enabled("RESULT_TABLE" , pParameter->asDataObject()->asShapes() == NULL);
			pParameters->Set_Enabled("RESULT_SHAPES", pParameter->asDataObject()->asShapes() != NULL);
		}
		else
		{
			pParameters->Set_Enabled("RESULT_TABLE" , false);
			pParameters->Set_Enabled("RESULT_SHAPES", false);
		}
	}

	if(	pParameter->Cmp_Identifier("TRAIN_WITH") )
	{
		pParameters->Set_Enabled("TRAIN_FIELD"    , pParameter->asInt() == 0);
		pParameters->Set_Enabled("TRAIN_SAMPLES"  , pParameter->asInt() == 1);
		pParameters->Set_Enabled("FILE_LOAD"      , pParameter->asInt() == 2);
		pParameters->Set_Enabled("FILE_SAVE"      , pParameter->asInt() != 2);
	}

	if(	pParameter->Cmp_Identifier("METHOD") )
	{
		pParameters->Set_Enabled("THRESHOLD_DIST" , pParameter->asInt() == SG_CLASSIFY_SUPERVISED_MinimumDistance
		                                         || pParameter->asInt() == SG_CLASSIFY_SUPERVISED_Mahalonobis      );
		pParameters->Set_Enabled("THRESHOLD_PROB" , pParameter->asInt() == SG_CLASSIFY_SUPERVISED_MaximumLikelihood);
		pParameters->Set_Enabled("RELATIVE_PROB"  , pParameter->asInt() == SG_CLASSIFY_SUPERVISED_MaximumLikelihood);
		pParameters->Set_Enabled("THRESHOLD_ANGLE", pParameter->asInt() == SG_CLASSIFY_SUPERVISED_SAM              );
		pParameters->Set_Enabled("WTA"            , pParameter->asInt() == SG_CLASSIFY_SUPERVISED_WTA              );
	}

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTable_Classify_Supervised::On_Execute(void)
{
	if( !Get_Features() )
	{
		Error_Set(_TL("invalid features"));

		return( false );
	}

	//-----------------------------------------------------
	Process_Set_Text(_TL("training"));

	CSG_Classifier_Supervised Classifier;

	if( !Set_Classifier(Classifier) )
	{
		return( false );
	}

	//-------------------------------------------------
	CSG_Table *pClasses = Parameters(m_pTable->asShapes() ? "RESULT_SHAPES" : "RESULT_TABLE")->asTable(); if( !pClasses ) { pClasses = m_pTable; }

	if( pClasses != m_pTable )
	{
		if( pClasses->asShapes() )
		{
			pClasses->asShapes()->Create(m_pTable->asShapes()->Get_Type(), NULL, NULL, m_pTable->asShapes()->Get_Vertex_Type());
		}
		else
		{
			pClasses->Destroy();
		}

		pClasses->Fmt_Name("%s [%s]", m_pTable->Get_Name(), _TL("Classification"));
	}

	int Offset = pClasses->Get_Field_Count();

	pClasses->Add_Field("CLASS_NUM"    , SG_DATATYPE_Int   );
	pClasses->Add_Field("CLASS_ID"     , SG_DATATYPE_String);
	pClasses->Add_Field("CLASS_QUALITY", SG_DATATYPE_Double);

	//-------------------------------------------------
	Process_Set_Text(_TL("prediction"));

	int Method = Parameters("METHOD")->asInt();

	for(sLong i=0; i<m_pTable->Get_Count() && Set_Progress(i, m_pTable->Get_Count()); i++)
	{
		int Class; double Quality; CSG_Vector Features(m_nFeatures);

		if( Get_Features(i, Features) && Classifier.Get_Class(Features, Class, Quality, Method) )
		{
			CSG_Table_Record *pClass = pClasses != m_pTable ? pClasses->Add_Record() : pClasses->Get_Record(i);

			if( pClasses != m_pTable && m_pTable->asShapes() )
			{
				((CSG_Shape	*)pClass)->Assign((CSG_Shape *)m_pTable->Get_Record(i), false);
			}

			pClass->Set_Value(Offset + 0, Class);
			pClass->Set_Value(Offset + 1, Classifier.Get_Class_ID(Class));
			pClass->Set_Value(Offset + 2, Quality);
		}
	}

	//-----------------------------------------------------
	return( Set_Classification(Classifier, Offset) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTable_Classify_Supervised::Get_Features(void)
{
	m_pTable     = Parameters("TABLE")->asTable();

	m_Features   = (int *)Parameters("FEATURES" )->asPointer();
	m_nFeatures  =        Parameters("FEATURES" )->asInt    ();
	m_bNormalize =        Parameters("NORMALISE")->asBool   ();

	return( m_Features && m_nFeatures > 0 );
}

//---------------------------------------------------------
bool CTable_Classify_Supervised::Get_Features(sLong iRecord, CSG_Vector &Features)
{
	CSG_Table_Record *pRecord = m_pTable->Get_Record(iRecord);

	if( pRecord )
	{
		for(int i=0; i<m_nFeatures; i++)
		{
			if( pRecord->is_NoData(m_Features[i]) )
			{
				return( false );
			}

			Features[i]	= pRecord->asDouble(m_Features[i]);

			if( m_bNormalize && m_pTable->Get_StdDev(m_Features[i]) > 0. )
			{
				Features[i]	= (Features[i] - m_pTable->Get_Mean(m_Features[i])) / m_pTable->Get_StdDev(m_Features[i]);
			}
		}

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTable_Classify_Supervised::Set_Classifier(CSG_Classifier_Supervised &Classifier)
{
	Classifier.Create(m_nFeatures);

	Classifier.Set_Threshold_Distance   (Parameters("THRESHOLD_DIST" )->asDouble());
	Classifier.Set_Threshold_Angle      (Parameters("THRESHOLD_ANGLE")->asDouble() * M_DEG_TO_RAD);
	Classifier.Set_Threshold_Probability(Parameters("THRESHOLD_PROB" )->asDouble());
	Classifier.Set_Probability_Relative (Parameters("RELATIVE_PROB"  )->asBool  ());

	for(int i=0; i<SG_CLASSIFY_SUPERVISED_WTA; i++)
	{
		Classifier.Set_WTA(i, Parameters(CSG_String::Format("WTA_%d", i))->asBool());
	}

	//-----------------------------------------------------
	switch( Parameters("TRAIN_WITH")->asInt() )
	{
	case  0: // classes field
		if( !Set_Classifier(Classifier, Parameters("TRAIN_FIELD")->asInt()) )
		{
			Error_Set(_TL("could not initialize classifier from classes field"));

			return( false );
		}
		break;

	case  1: // training samples
		if( !Set_Classifier(Classifier, *Parameters("TRAIN_SAMPLES")->asTable()) )
		{
			Error_Set(_TL("could not initialize classifier from training samples"));

			return( false );
		}
		break;

	case  2: // from file
		if( !Classifier.Load(Parameters("FILE_LOAD")->asString()) )
		{
			Error_Set(_TL("could not initialize classifier from file"));

			return( false );
		}
		break;
	}

	//-----------------------------------------------------
	if( Parameters("TRAIN_WITH")->asInt() != 2 )
	{
		Classifier.Save(Parameters("FILE_SAVE")->asString());
	}

	Message_Add(Classifier.Print(), false);

	return( true );
}

//---------------------------------------------------------
bool CTable_Classify_Supervised::Set_Classifier(CSG_Classifier_Supervised &Classifier, int Field)
{
	CSG_Index Index; m_pTable->Set_Index(Index, Field); CSG_Vector Features(m_nFeatures);

	for(sLong i=0; i<m_pTable->Get_Count() && Set_Progress(i, m_pTable->Get_Count()); i++)
	{
		CSG_Table_Record &Sample = *m_pTable->Get_Record(Index[i]);

		if( *Sample.asString(Field) && !Sample.is_NoData(Field) && Get_Features(Index[i], Features) )
		{
			Classifier.Train_Add_Sample(Sample.asString(Field), Features);
		}
	}

	return( Classifier.Train(true) );
}

//---------------------------------------------------------
bool CTable_Classify_Supervised::Set_Classifier(CSG_Classifier_Supervised &Classifier, const CSG_Table &Samples)
{
	if( Samples.Get_Field_Count() < m_nFeatures + 1 )
	{
		Error_Set(_TL("Training samples have to provide a class identifier in the first field followed by a value for each feature."));

		return( false );
	}

	int Field = 0; CSG_Index Index; Samples.Set_Index(Index, Field); CSG_Vector Features(m_nFeatures);

	for(sLong iSample=0; iSample<Samples.Get_Count() && Set_Progress(iSample, Samples.Get_Count()); iSample++)
	{
		CSG_Table_Record &Sample = *Samples.Get_Record(Index[iSample]);

		for(int i=0; i<m_nFeatures; i++)
		{
			Features[i] = Sample.asDouble(i + 1);

			if( m_bNormalize && m_pTable->Get_StdDev(m_Features[i]) > 0. )
			{
				Features[i]	= (Features[i] - m_pTable->Get_Mean(m_Features[i])) / m_pTable->Get_StdDev(m_Features[i]);
			}
		}

		Classifier.Train_Add_Sample(Sample.asString(Field), Features);
	}

	return( Classifier.Train(true) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTable_Classify_Supervised::Set_Classification(CSG_Classifier_Supervised &Classifier, int Offset)
{
	CSG_Table *pClasses = Parameters(m_pTable->asShapes() ? "RESULT_SHAPES" : "RESULT_TABLE")->asTable(); if( !pClasses ) { pClasses = m_pTable; }

	if( pClasses == m_pTable )
	{
		DataObject_Update(m_pTable);
	}

	CSG_Parameter *pLUT = DataObject_Get_Parameter(pClasses, "LUT");

	if( pLUT && pLUT->asTable() )
	{
		for(int Class=0; Class<Classifier.Get_Class_Count(); Class++)
		{
			CSG_Table_Record *pClass = pLUT->asTable()->Get_Record(Class);

			if( !pClass )
			{
				(pClass	= pLUT->asTable()->Add_Record())->Set_Value(0, SG_Color_Get_Random());
			}

			pClass->Set_Value(1, Classifier.Get_Class_ID(Class).c_str());
			pClass->Set_Value(2, "");
			pClass->Set_Value(3, Class);
			pClass->Set_Value(4, Class);
		}

		pLUT->asTable()->Set_Count(Classifier.Get_Class_Count());

		DataObject_Set_Parameter(pClasses, pLUT                 ); // Lookup Table
		DataObject_Set_Parameter(pClasses, "LUT_ATTRIB" , Offset); // Lookup Table Attribute
		DataObject_Set_Parameter(pClasses, "COLORS_TYPE", 1     ); // Color Classification Type: Lookup Table
	}

	if( pClasses != m_pTable )
	{
		pClasses->Fmt_Name("%s [%s]", m_pTable->Get_Name(), CSG_Classifier_Supervised::Get_Name_of_Method(Parameters("METHOD")->asInt()).c_str());
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
