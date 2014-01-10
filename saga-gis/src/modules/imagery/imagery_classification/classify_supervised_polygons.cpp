/**********************************************************
 * Version $Id: classify_supervised.cpp 1308 2012-01-12 15:27:56Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
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
#include "classify_supervised_polygons.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
enum
{
	CLASS_ID	= 0,
	CLASS_ROI_M,
	CLASS_ROI_S,
	CLASS_TOT_N,
	CLASS_ROI_N
};

//---------------------------------------------------------
#define CLASS_NPARMS	(CLASS_ROI_S - CLASS_ROI_M + 1)


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPolygon_Classify_Supervised::CPolygon_Classify_Supervised(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name		(_TL("Supervised Classification for Polygons"));

	Set_Author		(SG_T("O.Conrad (c) 2012"));

	Set_Description	(_TW(
		"Supervised Classification"
	));

	//-----------------------------------------------------
	pNode	= Parameters.Add_Shapes(
		NULL	, "POLYGONS"		, _TL("Polygons"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Table_Field(
		pNode	, "CLASS_ID"		, _TL("Class Identifier"),
		_TL("")
	);

	Parameters.Add_Shapes(
		NULL	, "CLASSES"			, _TL("Classification"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Table(
		NULL	, "CLASS_INFO"		, _TL("Summary"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Parameters(
		NULL	, "FEATURES"		, _TL("Features"),
		_TL("")
	);

	Parameters.Add_Choice(
		NULL	, "METHOD"			, _TL("Method"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			CSG_Classifier_Supervised::Get_Name_of_Method(SG_CLASSIFY_SUPERVISED_MinimumDistance).c_str(),
			CSG_Classifier_Supervised::Get_Name_of_Method(SG_CLASSIFY_SUPERVISED_SAM).c_str()
		), 0
	);

	Parameters.Add_Value(
		NULL	, "NORMALISE"		, _TL("Normalise"),
		_TL(""),
		PARAMETER_TYPE_Bool, false
	);

	Parameters.Add_Value(
		NULL	, "THRESHOLD_DIST"	, _TL("Distance Threshold"),
		_TL("Let pixel stay unclassified, if minimum or mahalanobis distance is greater than threshold."),
		PARAMETER_TYPE_Double, 0.0, 0.0, true
	);

	Parameters.Add_Value(
		NULL	, "THRESHOLD_ANGLE"	, _TL("Spectral Angle Threshold (Degree)"),
		_TL("Let pixel stay unclassified, if spectral angle distance is greater than threshold."),
		PARAMETER_TYPE_Double, 0.0, 0.0, true, 90.0, true
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CPolygon_Classify_Supervised::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("POLYGONS")) )
	{
		CSG_Shapes		*pShapes	= pParameter->asShapes();
		CSG_Parameters	*pFeatures	= pParameters->Get_Parameter("FEATURES")->asParameters();

		pFeatures->Del_Parameters();

		if( pShapes && pShapes->Get_Field_Count() > 0 )
		{
			for(int i=0; i<pShapes->Get_Field_Count(); i++)
			{
				if( SG_Data_Type_is_Numeric(pShapes->Get_Field_Type(i)) )
				{
					pFeatures->Add_Value(NULL, CSG_String::Format(SG_T("%d_FEATURE"), i), pShapes->Get_Field_Name(i), _TL(""), PARAMETER_TYPE_Bool, false);
				}
			}
		}
	}

	//-----------------------------------------------------
	return( 1 );
}

//---------------------------------------------------------
int CPolygon_Classify_Supervised::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("METHOD")) )
	{
		int	Value	= pParameter->asInt();

		pParameters->Get_Parameter("THRESHOLD_DIST" )->Set_Enabled(Value == 0);
		pParameters->Get_Parameter("THRESHOLD_ANGLE")->Set_Enabled(Value == 1);
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
bool CPolygon_Classify_Supervised::On_Execute(void)
{
	bool		bNoData;
	int			Method, Class, i, iPolygon;
	double		Quality;

	//-------------------------------------------------
	m_pPolygons		= Parameters("POLYGONS" )->asShapes();
	m_Class_ID		= Parameters("CLASS_ID" )->asInt();
 	m_pClasses		= Parameters("CLASSES"  )->asShapes();
	m_bNormalise	= Parameters("NORMALISE")->asBool();
	Method			= Parameters("METHOD"   )->asInt();

	m_Classifier.Set_Distance_Threshold   (Parameters("THRESHOLD_DIST" )->asDouble());
	m_Classifier.Set_Angle_Threshold      (Parameters("THRESHOLD_ANGLE")->asDouble() * M_DEG_TO_RAD);

	switch( Method )
	{
	default:
	case  0:	Method	= SG_CLASSIFY_SUPERVISED_MinimumDistance;	break;
	case  1:	Method	= SG_CLASSIFY_SUPERVISED_SAM;				break;
	}

	//-------------------------------------------------
	CSG_Parameters	*pFeatures	= Parameters("FEATURES")->asParameters();

	m_Features	= (int *)SG_Calloc(m_pPolygons->Get_Field_Count(), sizeof(int));
	m_nFeatures	= 0;

	for(i=0; i<pFeatures->Get_Count(); i++)
	{
		if( pFeatures->Get_Parameter(i)->asBool() )
		{
			m_Features[m_nFeatures++]	= CSG_String(pFeatures->Get_Parameter(i)->Get_Identifier()).asInt();
		}
	}

	if( m_nFeatures <= 0 )
	{
		Error_Set(_TL("no features have been selected"));

		return( false );
	}

	//-------------------------------------------------
	m_pClasses->Create(SHAPE_TYPE_Polygon, CSG_String::Format(SG_T("%s [%s]"), m_pPolygons->Get_Name(), _TL("Classified")), NULL, m_pPolygons->Get_Vertex_Type());
	m_pClasses->Add_Field(_TL("CLASS_NR"), SG_DATATYPE_Int);
	m_pClasses->Add_Field(_TL("CLASS_ID"), SG_DATATYPE_String);
	m_pClasses->Add_Field(_TL("QUALITY" ), SG_DATATYPE_Double);

	CSG_Vector	Features(m_nFeatures);

	m_Classifier.Create (m_nFeatures);

	//-----------------------------------------------------
	Process_Set_Text(_TL("analyzing known classes"));

	for(iPolygon=0; iPolygon<m_pPolygons->Get_Count() && Set_Progress(iPolygon, m_pPolygons->Get_Count()); iPolygon++)
	{
		CSG_Shape	*pPolygon	= m_pPolygons->Get_Shape(iPolygon);

		if( SG_STR_LEN(pPolygon->asString(m_Class_ID)) > 0 )
		{
			for(i=0, bNoData=false; i<m_nFeatures && !bNoData; i++)
			{
				if( pPolygon->is_NoData(m_Features[i]) )
				{
					bNoData	= true;
				}
				else
				{
					Features[i]	= pPolygon->asDouble(m_Features[i]);
					
					if( m_bNormalise )
					{
						Features[i]	= (Features[i] - m_pPolygons->Get_Mean(m_Features[i])) / m_pPolygons->Get_StdDev(m_Features[i]);
					}
				}
			}

			if( !bNoData )
			{
				CSG_Simple_Statistics	*pClass	= m_Classifier.Get_Statistics(pPolygon->asString(m_Class_ID));

				for(i=0; i<m_nFeatures; i++)
				{
					pClass[i].Add_Value(Features[i]);
				}
			}
		}
	}

	if( m_Classifier.Get_Class_Count() <= 0 )
	{
		Error_Set(_TL("no training areas could be analysed."));

		return( false );
	}

	//-------------------------------------------------
	Process_Set_Text(_TL("running classification"));

	for(iPolygon=0; iPolygon<m_pPolygons->Get_Count() && Set_Progress(iPolygon, m_pPolygons->Get_Count()); iPolygon++)
	{
		CSG_Shape	*pPolygon	= m_pPolygons->Get_Shape(iPolygon);

		for(i=0, bNoData=false; i<m_nFeatures && !bNoData; i++)
		{
			if( pPolygon->is_NoData(m_Features[i]) )
			{
				bNoData	= true;
			}
			else
			{
				Features[i]	= pPolygon->asDouble(m_Features[i]);
					
				if( m_bNormalise )
				{
					Features[i]	= (Features[i] - m_pPolygons->Get_Mean(m_Features[i])) / m_pPolygons->Get_StdDev(m_Features[i]);
				}
			}
		}

		if( !bNoData )
		{
			if( m_Classifier.Get_Class(Features, Class, Quality, Method) )
			{
				CSG_Shape	*pClass	= m_pClasses->Add_Shape(pPolygon);

				pClass->Set_Value(0, 1 + Class);
				pClass->Set_Value(1, m_Classifier.Get_Class_ID(Class));
				pClass->Set_Value(2, Quality);
			}
		}
	}

	//-----------------------------------------------------
	Finalize();

	SG_Free(m_Features);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPolygon_Classify_Supervised::Finalize(void)
{
	int			iClass, iFeature, iOffset, nClasses;
	CSG_String	Name_Method, Name_Quality;
	CSG_Table	*pTable;

	//-----------------------------------------------------
	Name_Method		= CSG_Classifier_Supervised::Get_Name_of_Method (Parameters("METHOD")->asInt());
	Name_Quality	= CSG_Classifier_Supervised::Get_Name_of_Quality(Parameters("METHOD")->asInt());

	//-----------------------------------------------------
	pTable	= Parameters("CLASS_INFO")->asTable();

	pTable->Destroy();
	pTable->Set_Name(CSG_String::Format(SG_T("%s [%s]"), _TL("Class Information"), Name_Method.c_str()));

	pTable->Add_Field(_TL("ID")   , SG_DATATYPE_String);	// CLASS_ID

	for(iFeature=0; iFeature<m_Classifier.Get_Feature_Count(); iFeature++)
	{
		pTable->Add_Field(CSG_String::Format(SG_T("B%02d_MEAN")  , iFeature + 1), SG_DATATYPE_Double);	// CLASS_ROI_M
		pTable->Add_Field(CSG_String::Format(SG_T("B%02d_STDDEV"), iFeature + 1), SG_DATATYPE_Double);	// CLASS_ROI_S
	}

	pTable->Add_Field(_TL("TOT_N"), SG_DATATYPE_Int);		// CLASS_TOT_N
	pTable->Add_Field(_TL("ROI_N"), SG_DATATYPE_Int);		// CLASS_ROI_N

	for(iClass=0; iClass<m_Classifier.Get_Class_Count(); iClass++)
	{
		CSG_Table_Record	*pRecord	= pTable->Add_Record();

		pRecord->Set_Value(CLASS_ID   , m_Classifier.Get_Class_ID(iClass).c_str());

		for(iFeature=0, iOffset=0; iFeature<m_Classifier.Get_Feature_Count(); iFeature++, iOffset+=CLASS_NPARMS)
		{
			CSG_Simple_Statistics	*pStatistics	= m_Classifier.Get_Statistics(iClass) + iFeature;

			double	m	= m_bNormalise ? m_pPolygons->Get_Mean  (m_Features[iFeature]) : 0.0;
			double	s	= m_bNormalise ? m_pPolygons->Get_StdDev(m_Features[iFeature]) : 1.0;

			pRecord->Set_Value(iOffset + CLASS_ROI_M  , pStatistics->Get_Mean   () * s + m);
			pRecord->Set_Value(iOffset + CLASS_ROI_S  , pStatistics->Get_StdDev () * s);
		//	pRecord->Set_Value(iOffset + CLASS_ROI_MIN, pStatistics->Get_Minimum() * s + m);
		//	pRecord->Set_Value(iOffset + CLASS_ROI_MAX, pStatistics->Get_Maximum() * s + m);
		}

		pRecord->Set_Value(CLASS_NPARMS * m_Classifier.Get_Feature_Count() + 1, m_Classifier.Get_Statistics(iClass)->Get_Count());
		pRecord->Set_Value(CLASS_NPARMS * m_Classifier.Get_Feature_Count() + 2, m_Classifier.Get_Element_Count(iClass));
	}

	//-----------------------------------------------------
	CSG_Parameters	P;

	if( DataObject_Get_Parameters(m_pClasses, P) && P("COLORS_TYPE") && P("LUT") )
	{
		pTable	= P("LUT")->asTable();

		for(iClass=0, nClasses=0; iClass<m_Classifier.Get_Class_Count(); iClass++)
		{
			if( m_Classifier.Get_Element_Count(iClass) > 0 )
			{
				CSG_Table_Record	*pRecord	= pTable->Get_Record(nClasses++);

				if( pRecord == NULL )
				{
					pRecord	= pTable->Add_Record();
					pRecord->Set_Value(0, SG_GET_RGB(rand() * 255.0 / RAND_MAX, rand() * 255.0 / RAND_MAX, rand() * 255.0 / RAND_MAX));
				}

				pRecord->Set_Value(1, m_Classifier.Get_Class_ID(iClass).c_str());
				pRecord->Set_Value(2, m_Classifier.Get_Class_ID(iClass).c_str());
				pRecord->Set_Value(3, iClass + 1);
				pRecord->Set_Value(4, iClass + 1);
			}
		}

		while( pTable->Get_Record_Count() > m_Classifier.Get_Class_Count() )
		{
			pTable->Del_Record(pTable->Get_Record_Count() - 1);
		}

		P.Set_Parameter("COLORS_TYPE", 1);	// Color Classification Type: Lookup Table
		P.Set_Parameter("LUT_ATTRIB" , 0);	// Lookup Table Attribute

		DataObject_Set_Parameters(m_pClasses, P);
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
