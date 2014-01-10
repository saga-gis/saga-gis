/**********************************************************
 * Version $Id: classify_supervised.cpp 1921 2014-01-09 10:24:11Z oconrad $
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
//              Grid_Classify_Supervised.cpp             //
//                                                       //
//                 Copyright (C) 2005 by                 //
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
#include "classify_supervised.h"


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
CGrid_Classify_Supervised::CGrid_Classify_Supervised(void)
{
	int				i;
	CSG_String		s;
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name		(_TL("Supervised Classification"));

	Set_Author		(SG_T("O.Conrad (c) 2005"));

	Set_Description	(_TW(
		"Supervised Classification"
	));

	//-----------------------------------------------------
	Parameters.Add_Grid_List(
		NULL	, "GRIDS"			, _TL("Grids"),
		_TL(""),
		PARAMETER_INPUT
	);

	pNode	= Parameters.Add_Shapes(
		NULL	, "ROI"				, _TL("Training Areas"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Table_Field(
		pNode	, "ROI_ID"			, _TL("Class Identifier"),
		_TL("")
	);

	Parameters.Add_Table(
		NULL	, "STATS"			, _TL("Class Statistics"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table(
		NULL	, "CLASS_INFO"		, _TL("Summary"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		NULL	, "CLASSES"			, _TL("Classification"),
		_TL(""),
		PARAMETER_OUTPUT, true, SG_DATATYPE_Short
	);

	Parameters.Add_Grid(
		NULL	, "QUALITY"			, _TL("Quality"),
		_TL("Dependent on chosen method, these are distances or probabilities."),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Choice(
		NULL	, "STATS_SRC"		, _TL("Get Class Statistics from..."),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("training areas"),
			_TL("table")
		), 0
	);

	for(i=0; i<=SG_CLASSIFY_SUPERVISED_WTA; i++)
	{
		s	+= CSG_Classifier_Supervised::Get_Name_of_Method(i) + SG_T("|");
	}

	Parameters.Add_Choice(
		NULL	, "METHOD"			, _TL("Method"),
		_TL(""),
		s, SG_CLASSIFY_SUPERVISED_MinimumDistance
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
		NULL	, "THRESHOLD_PROB"	, _TL("Probability Threshold (Percent)"),
		_TL("Let pixel stay unclassified, if maximum likelihood probability is less than threshold."),
		PARAMETER_TYPE_Double, 0.0, 0.0, true, 100.0, true
	);

	Parameters.Add_Choice(
		NULL	, "RELATIVE_PROB"	, _TL("Probability Reference"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("absolute"),
			_TL("relative")
		), 1
	);

	Parameters.Add_Value(
		NULL	, "THRESHOLD_ANGLE"	, _TL("Spectral Angle Threshold (Degree)"),
		_TL("Let pixel stay unclassified, if spectral angle distance is greater than threshold."),
		PARAMETER_TYPE_Double, 0.0, 0.0, true, 90.0, true
	);

	pNode	= Parameters.Add_Node(
		NULL	, "WTA"				, _TL("Winner Takes All"),
		_TL("")
	);

	for(i=0; i<SG_CLASSIFY_SUPERVISED_WTA; i++)
	{
		Parameters.Add_Value(
			pNode, CSG_String::Format(SG_T("WTA_%d"), i), CSG_Classifier_Supervised::Get_Name_of_Method(i), _TL(""),
			PARAMETER_TYPE_Bool, false
		);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGrid_Classify_Supervised::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("STATS_SRC")) )
	{
		int	Value	= pParameter->asInt();

		pParameters->Get_Parameter("ROI"  )->Set_Enabled(Value == 0);
		pParameters->Get_Parameter("STATS")->Set_Enabled(Value == 1);
	}

	if(	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("METHOD")) )
	{
		int	Value	= pParameter->asInt();

		pParameters->Get_Parameter("THRESHOLD_DIST" )->Set_Enabled(Value == SG_CLASSIFY_SUPERVISED_MinimumDistance || Value == SG_CLASSIFY_SUPERVISED_Mahalonobis);
		pParameters->Get_Parameter("THRESHOLD_PROB" )->Set_Enabled(Value == SG_CLASSIFY_SUPERVISED_MaximumLikelihood);
		pParameters->Get_Parameter("RELATIVE_PROB"  )->Set_Enabled(Value == SG_CLASSIFY_SUPERVISED_MaximumLikelihood);
		pParameters->Get_Parameter("THRESHOLD_ANGLE")->Set_Enabled(Value == SG_CLASSIFY_SUPERVISED_SAM);
		pParameters->Get_Parameter("WTA"            )->Set_Enabled(Value == SG_CLASSIFY_SUPERVISED_WTA);
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
inline double CGrid_Classify_Supervised::Get_Value(int x, int y, int iGrid)
{
	CSG_Grid	*pGrid	= m_pGrids->asGrid(iGrid);

	return( m_bNormalise
		? (pGrid->asDouble(x, y) - pGrid->Get_ArithMean()) / pGrid->Get_StdDev()
		:  pGrid->asDouble(x, y)
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Classify_Supervised::On_Execute(void)
{
	int			Method, Class, i;
	double		Quality;

	//-------------------------------------------------
	m_pGrids		= Parameters("GRIDS"    )->asGridList();
 	m_pClasses		= Parameters("CLASSES"  )->asGrid();
	m_pQuality		= Parameters("QUALITY"  )->asGrid();
	m_bNormalise	= Parameters("NORMALISE")->asBool();
	Method			= Parameters("METHOD"   )->asInt();

	m_Classifier.Set_Distance_Threshold   (Parameters("THRESHOLD_DIST" )->asDouble());
	m_Classifier.Set_Probability_Threshold(Parameters("THRESHOLD_PROB" )->asDouble() / 100.0);
	m_Classifier.Set_Probability_Relative (Parameters("RELATIVE_PROB"  )->asBool  ());
	m_Classifier.Set_Angle_Threshold      (Parameters("THRESHOLD_ANGLE")->asDouble() * M_DEG_TO_RAD);

	for(i=0; i<SG_CLASSIFY_SUPERVISED_WTA; i++)
	{
		m_Classifier.Set_WTA(i, Parameters(CSG_String::Format(SG_T("WTA_%d"), i))->asBool());
	}

	//-----------------------------------------------------
	for(i=m_pGrids->Get_Count()-1; i>=0; i--)
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

	//-------------------------------------------------
	Process_Set_Text(_TL("initializing"));

	if( !Initialize() )
	{
		return( false );
	}

	//-------------------------------------------------
	Process_Set_Text(_TL("running classification"));

	CSG_Vector	Features(m_pGrids->Get_Count());

	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			if( !m_pClasses->is_NoData(x, y) )
			{
				for(i=0; i<m_pGrids->Get_Count(); i++)
				{
					Features[i]	= Get_Value(x, y, i);
				}

				if( m_Classifier.Get_Class(Features, Class, Quality, Method) )
				{
					m_pClasses->Set_Value(x, y, 1 + Class);
				}
				else
				{
					m_pClasses->Set_NoData(x, y);
				}

				if( m_pQuality )
				{
					m_pQuality->Set_Value(x, y, Quality);
				}
			}
		}
	}

	return( Finalize() );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Classify_Supervised::Initialize(void)
{
	bool		bNoData;
	int			x, y, iGrid, iROI_ID;
	TSG_Point	p;
	CSG_Shapes	*pROIs;

	//-----------------------------------------------------
	m_pClasses->Set_NoData_Value(-1);
	m_pClasses->Assign(0.0);

	m_Classifier.Create(m_pGrids->Get_Count());

	if( Parameters("STATS_SRC")->asInt() == 0 )
	{
		pROIs	= Parameters("ROI"   )->asShapes();
		iROI_ID	= Parameters("ROI_ID")->asInt();
	}
	else
	{
		pROIs	= NULL;

		CSG_Table	*pStats	= Parameters("STATS")->asTable();

		if( pStats->Get_Field_Count() < (1 + 2 * m_pGrids->Get_Count()) )
		{
			Error_Set(_TL("not entries in statistics table."));

			return( false );
		}

		for(int iClass=0; iClass<pStats->Get_Count(); iClass++)
		{
			CSG_Table_Record		*pRecord	= pStats->Get_Record(iClass);
			CSG_Simple_Statistics	*pClass		= m_Classifier.Get_Statistics(pRecord->asString(0));

			for(iGrid=0; iGrid<m_pGrids->Get_Count(); iGrid++)
			{
				pClass[iGrid].Create(pRecord->asDouble(1 + 2 * iGrid), pRecord->asDouble(2 + 2 * iGrid));
			}
		}
	}

	//-----------------------------------------------------
	for(y=0, p.y=Get_YMin(); y<Get_NY() && Set_Progress(y); y++, p.y+=Get_Cellsize())
	{
		for(x=0, p.x=Get_XMin(); x<Get_NX(); x++, p.x+=Get_Cellsize())
		{
			for(iGrid=0, bNoData=false; iGrid<m_pGrids->Get_Count() && !bNoData; iGrid++)
			{
				if( m_pGrids->asGrid(iGrid)->is_NoData(x, y) )
				{
					bNoData	= true;
				}
			}

			if( bNoData )
			{
				m_pClasses->Set_NoData(x, y);

				if( m_pQuality )
				{
					m_pQuality->Set_NoData(x, y);
				}
			}
			else if( pROIs )
			{
				for(int iROI=0; iROI<pROIs->Get_Count(); iROI++)
				{
					CSG_Simple_Statistics	*pClass;
					CSG_Shape_Polygon		*pROI	= (CSG_Shape_Polygon *)pROIs->Get_Shape(iROI);

					if( pROI->Contains(p) && (pClass = m_Classifier.Get_Statistics(pROI->asString(iROI_ID))) != NULL )
					{
						for(iGrid=0; iGrid<m_pGrids->Get_Count(); iGrid++)
						{
							pClass[iGrid].Add_Value(Get_Value(x, y, iGrid));
						}
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	if( m_Classifier.Get_Class_Count() <= 0 )
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
bool CGrid_Classify_Supervised::Finalize(void)
{
	int			iClass, iGrid, iOffset, nClasses;
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

	for(iGrid=0; iGrid<m_pGrids->Get_Count(); iGrid++)
	{
		pTable->Add_Field(CSG_String::Format(SG_T("B%02d_MEAN")  , iGrid + 1), SG_DATATYPE_Double);	// CLASS_ROI_M
		pTable->Add_Field(CSG_String::Format(SG_T("B%02d_STDDEV"), iGrid + 1), SG_DATATYPE_Double);	// CLASS_ROI_S
	}

	pTable->Add_Field(_TL("TOT_N"), SG_DATATYPE_Int);		// CLASS_TOT_N
	pTable->Add_Field(_TL("ROI_N"), SG_DATATYPE_Int);		// CLASS_ROI_N

	for(iClass=0; iClass<m_Classifier.Get_Class_Count(); iClass++)
	{
		CSG_Table_Record	*pRecord	= pTable->Add_Record();

		pRecord->Set_Value(CLASS_ID   , m_Classifier.Get_Class_ID(iClass).c_str());

		for(iGrid=0, iOffset=0; iGrid<m_Classifier.Get_Feature_Count(); iGrid++, iOffset+=CLASS_NPARMS)
		{
			CSG_Simple_Statistics	*pStatistics	= m_Classifier.Get_Statistics(iClass) + iGrid;

			double	m	= m_bNormalise ? m_pGrids->asGrid(iGrid)->Get_ArithMean() : 0.0;
			double	s	= m_bNormalise ? m_pGrids->asGrid(iGrid)->Get_StdDev()    : 1.0;

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

		P("COLORS_TYPE")->Set_Value(1);	// Color Classification Type: Lookup Table

		DataObject_Set_Parameters(m_pClasses, P);
	}

	//-----------------------------------------------------
	m_pClasses->Set_Name(CSG_String::Format(SG_T("%s [%s]"), _TL("Classification"), Name_Method.c_str()));

	if( m_pQuality )
	{
		m_pQuality->Set_Name(CSG_String::Format(SG_T("%s [%s]"), _TL("Classification Quality"), Name_Quality.c_str()));

		DataObject_Set_Colors(m_pQuality, 100, SG_COLORS_WHITE_GREEN);
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
