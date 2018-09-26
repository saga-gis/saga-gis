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
//                imagery_classification                 //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//              classify_supervised_polygons.cpp         //
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
#include "classify_supervised_polygons.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPolygon_Classify_Supervised::CPolygon_Classify_Supervised(bool bShapes)
{
	m_bShapes	= bShapes;

	//-----------------------------------------------------
	int				i;
	CSG_String		s;
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name		(m_bShapes
		? _TL("Supervised Classification for Shapes")
		: _TL("Supervised Classification for Tables")
	);

	Set_Author		("O.Conrad (c) 2012");

	Set_Description	(_TW(
		"Supervised classification for attribute data. To train the classifier "
		"choose an attribute that provides class identifiers for those records, "
		"for which the target class is known, and no data for all other records."
	));

	//-----------------------------------------------------
	if( m_bShapes )
	{
		pNode	= Parameters.Add_Shapes(
			NULL	, "SHAPES"		, _TL("Shapes"),
			_TL(""),
			PARAMETER_INPUT
		);

		Parameters.Add_Shapes(
			NULL	, "CLASSES"		, _TL("Classification"),
			_TL(""),
			PARAMETER_OUTPUT
		);
	}
	else
	{
		pNode	= Parameters.Add_Table(
			NULL	, "TABLE"		, _TL("Table"),
			_TL(""),
			PARAMETER_INPUT
		);

		Parameters.Add_Table(
			NULL	, "CLASSES"		, _TL("Classification"),
			_TL(""),
			PARAMETER_OUTPUT
		);
	}

	Parameters.Add_Table_Fields(
		pNode	, "FEATURES"		, _TL("Features"),
		_TL("")
	);

	Parameters.Add_Value(
		pNode	, "NORMALISE"		, _TL("Normalise"),
		_TL(""),
		PARAMETER_TYPE_Bool, false
	);

	Parameters.Add_Table_Field(
		pNode	, "TRAINING"		, _TL("Training Classes"),
		_TL(""),
		true
	);

	Parameters.Add_FilePath(
		pNode	, "FILE_LOAD"		, _TL("Load Statistics from File..."),
		_TL(""),
		NULL, NULL, false
	);

	Parameters.Add_FilePath(
		NULL	, "FILE_SAVE"		, _TL("Save Statistics to File..."),
		_TL(""),
		NULL, NULL, true
	);

	//-----------------------------------------------------
	for(i=0; i<=SG_CLASSIFY_SUPERVISED_WTA; i++)
	{
		s	+= CSG_Classifier_Supervised::Get_Name_of_Method(i) + "|";
	}

	Parameters.Add_Choice(
		NULL	, "METHOD"			, _TL("Method"),
		_TL(""),
		s, SG_CLASSIFY_SUPERVISED_MinimumDistance
	);

	Parameters.Add_Value(
		NULL	, "THRESHOLD_DIST"	, _TL("Distance Threshold"),
		_TL("Let pixel stay unclassified, if minimum euclidian or mahalanobis distance is greater than threshold."),
		PARAMETER_TYPE_Double, 0.0, 0.0, true
	);

	Parameters.Add_Value(
		NULL	, "THRESHOLD_ANGLE"	, _TL("Spectral Angle Threshold (Degree)"),
		_TL("Let pixel stay unclassified, if spectral angle distance is greater than threshold."),
		PARAMETER_TYPE_Double, 0.0, 0.0, true, 90.0, true
	);

	Parameters.Add_Value(
		NULL	, "THRESHOLD_PROB"	, _TL("Probability Threshold"),
		_TL("Let pixel stay unclassified, if maximum likelihood probability value is less than threshold."),
		PARAMETER_TYPE_Double, 0.0, 0.0, true, 100.0, true
	);

	Parameters.Add_Choice(
		NULL	, "RELATIVE_PROB"	, _TL("Probability Reference"),
		_TL(""),
		CSG_String::Format("%s|%s|",
			_TL("absolute"),
			_TL("relative")
		), 1
	);

	pNode	= Parameters.Add_Node(
		NULL	, "WTA"				, _TL("Winner Takes All"),
		_TL("")
	);

	for(i=0; i<SG_CLASSIFY_SUPERVISED_WTA; i++)
	{
		Parameters.Add_Value(
			pNode, CSG_String::Format("WTA_%d", i), CSG_Classifier_Supervised::Get_Name_of_Method(i), _TL(""),
			PARAMETER_TYPE_Bool, false
		);
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CPolygon_Classify_Supervised::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	pParameter->Cmp_Identifier("TRAINING") )
	{
		pParameters->Set_Enabled("FILE_LOAD", pParameter->asInt() <  0);
		pParameters->Set_Enabled("FILE_SAVE", pParameter->asInt() >= 0);
	}

	if(	pParameter->Cmp_Identifier("METHOD") )
	{
		pParameters->Set_Enabled("THRESHOLD_DIST" , pParameter->asInt() == SG_CLASSIFY_SUPERVISED_MinimumDistance
			||                                      pParameter->asInt() == SG_CLASSIFY_SUPERVISED_Mahalonobis      );
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
bool CPolygon_Classify_Supervised::On_Execute(void)
{
	//-----------------------------------------------------
	if( !Get_Features() )
	{
		Error_Set(_TL("invalid features"));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Classifier_Supervised	Classifier;

	if( !Set_Classifier(Classifier) )
	{
		return( false );
	}

	//-------------------------------------------------
	CSG_Table	*pClasses	= Parameters("CLASSES")->asTable();

	pClasses->Destroy();

	if( m_bShapes )
	{
		CSG_Shapes	*pShapes	= (CSG_Shapes *)m_pTable;

		((CSG_Shapes *)pClasses)->Create(pShapes->Get_Type(), NULL, NULL, pShapes->Get_Vertex_Type());
	}

	pClasses->Add_Field(_TL("CLASS_NR"), SG_DATATYPE_Int);
	pClasses->Add_Field(_TL("CLASS_ID"), SG_DATATYPE_String);
	pClasses->Add_Field(_TL("QUALITY" ), SG_DATATYPE_Double);

	//-------------------------------------------------
	Process_Set_Text(_TL("prediction"));

	int	Method	= Parameters("METHOD")->asInt();

	for(int iRecord=0; iRecord<m_pTable->Get_Count() && Set_Progress(iRecord, m_pTable->Get_Count()); iRecord++)
	{
		int			Class;
		double		Quality;
		CSG_Vector	Features(m_nFeatures);

		if( Get_Features(iRecord, Features) && Classifier.Get_Class(Features, Class, Quality, Method) )
		{
			CSG_Table_Record	*pClass	= pClasses->Add_Record();

			pClass->Set_Value(0, 1 + Class);
			pClass->Set_Value(1, Classifier.Get_Class_ID(Class));
			pClass->Set_Value(2, Quality);

			if( m_bShapes )
			{
				((CSG_Shape	*)pClass)->Assign((CSG_Shape *)m_pTable->Get_Record(iRecord), false);
			}
		}
	}

	//-----------------------------------------------------
	return( Set_Classification(Classifier) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPolygon_Classify_Supervised::Get_Features(void)
{
	m_pTable		= Parameters(m_bShapes ? "SHAPES" : "TABLE")->asTable();

	m_Features		= (int *)Parameters("FEATURES" )->asPointer();
	m_nFeatures		=        Parameters("FEATURES" )->asInt    ();
	m_bNormalise	=        Parameters("NORMALISE")->asBool   ();

	return( m_Features && m_nFeatures > 0 );
}

//---------------------------------------------------------
bool CPolygon_Classify_Supervised::Get_Features(int iRecord, CSG_Vector &Features)
{
	CSG_Table_Record	*pRecord	= m_pTable->Get_Record(iRecord);

	if( !pRecord )
	{
		return( false );
	}

	for(int i=0; i<m_nFeatures; i++)
	{
		if( pRecord->is_NoData(m_Features[i]) )
		{
			return( false );
		}

		Features[i]	= pRecord->asDouble(m_Features[i]);

		if( m_bNormalise && m_pTable->Get_StdDev(m_Features[i]) > 0.0 )
		{
			Features[i]	= (Features[i] - m_pTable->Get_Mean(m_Features[i])) / m_pTable->Get_StdDev(m_Features[i]);
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPolygon_Classify_Supervised::Set_Classifier(CSG_Classifier_Supervised &Classifier)
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
	if( Parameters("TRAINING")->asInt() >= 0 )	// training areas
	{
		if( !Set_Classifier(Classifier, Parameters("TRAINING")->asInt()) )
		{
			Error_Set(_TL("could not initialize classifier from training areas"));

			return( false );
		}
	}
	else	// from file
	{
		if( !Classifier.Load(Parameters("FILE_LOAD")->asString()) )
		{
			Error_Set(_TL("could not initialize classifier from file"));

			return( false );
		}
	}

	//-----------------------------------------------------
	Message_Add(Classifier.Print(), false);

	return( true );
}

//---------------------------------------------------------
bool CPolygon_Classify_Supervised::Set_Classifier(CSG_Classifier_Supervised &Classifier, int Training)
{
	//-----------------------------------------------------
	Process_Set_Text(_TL("training"));

	for(int iRecord=0; iRecord<m_pTable->Get_Count() && Set_Progress(iRecord, m_pTable->Get_Count()); iRecord++)
	{
		CSG_Table_Record	*pRecord	= m_pTable->Get_Record(iRecord);

		if( SG_STR_LEN(pRecord->asString(Training)) > 0 )
		{
			CSG_Vector	Features(m_nFeatures);

			if( Get_Features(iRecord, Features) )
			{
				Classifier.Train_Add_Sample(pRecord->asString(Training), Features);
			}
		}
	}

	//-----------------------------------------------------
	if( Classifier.Train(true) )
	{
		Classifier.Save(Parameters("FILE_SAVE")->asString());

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPolygon_Classify_Supervised::Set_Classification(CSG_Classifier_Supervised &Classifier)
{
	//-----------------------------------------------------
	CSG_Table	*pClasses	= Parameters("CLASSES")->asTable();

	CSG_Parameter	*pLUT	= DataObject_Get_Parameter(pClasses, "LUT");

	if( pLUT && pLUT->asTable() )
	{
		for(int iClass=0; iClass<Classifier.Get_Class_Count(); iClass++)
		{
			CSG_Table_Record	*pClass	= pLUT->asTable()->Get_Record(iClass);

			if( !pClass )
			{
				(pClass	= pLUT->asTable()->Add_Record())->Set_Value(0, SG_Color_Get_Random());
			}

			pClass->Set_Value(1, Classifier.Get_Class_ID(iClass).c_str());
			pClass->Set_Value(2, "");
			pClass->Set_Value(3, iClass + 1);
			pClass->Set_Value(4, iClass + 1);
		}

		pLUT->asTable()->Set_Record_Count(Classifier.Get_Class_Count());

		DataObject_Set_Parameter(pClasses, pLUT);
		DataObject_Set_Parameter(pClasses, "COLORS_TYPE", 1);	// Color Classification Type: Lookup Table
		DataObject_Set_Parameter(pClasses, "LUT_ATTRIB" , 0);	// Lookup Table Attribute
	}

	pClasses->Set_Name("%s [%s]", m_pTable->Get_Name(), CSG_Classifier_Supervised::Get_Name_of_Method(Parameters("METHOD")->asInt()).c_str());

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
